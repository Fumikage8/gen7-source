//=============================================================================================
/**
 * @file  btl_server.cpp
 * @brief ポケモンXY バトルシステム サーバモジュール
 * @author  taya
 *
 * @date  2010.12.17  作成
 */
//=============================================================================================

//#include <str/include/gfl2_TagProc.h>
#include <util/include/gfl2_std_string.h>

#include "./btl_common.h"
#include "./btl_ui.h"
#include "./btl_mainmodule.h"
#include "./btl_calc.h"
#include "./btl_server_cmd.h"
#include "./btl_SendData.h"
#include "./btl_SendDataContainer.h"
#include "./btl_SendData_ActionSelect.h"
#include "./btl_SendData_ClientLimitTime.h"
#include "./btl_ServerCommandCalculator.h"
#include "./btl_AdapterCommandData.h"
#include "./btl_AdapterCommandGenerator.h"

#include "./btl_server.h"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  SERVER_STRBUF_SIZE = 32,
};

enum {
  QUITSTEP_NONE = 0,    ///
  QUITSTEP_REQ,
  QUITSTEP_CORE,
};




//--------------------------------------------------------------
/**
 *  クライアントアクション格納構造体
 */
//--------------------------------------------------------------
/*
struct _BTL_SVCL_ACTION {
  BTL_ACTION_PARAM     param[ BTL_CLIENT_MAX ][ BTL_POSIDX_MAX ];
  u8                   count[ BTL_CLIENT_MAX ];
};
*/

GFL_NAMESPACE_BEGIN(btl)

BTL_SERVER::BTL_SERVER(
    MainModule* mainModule,
    const u32 randSeed,
    POKECON* pokeCon,
    SendDataContainer* sendDataContainer,
    HeapHandle pHeap,
    u32 clientLimitTime
 ):
    m_currentSequence( SERVER_SEQ_END ),
    m_seqStep( 0 ),
    m_mainModule( mainModule ),
    m_pokeCon( pokeCon ),
    m_flowWork( NULL ),
    m_sendDataContainer( sendDataContainer ),
    m_adapterCommandGenerator( NULL ),
    m_adapterCommandSerialNumber( SEND_DATA_SERIAL_NUMBER_MIN ),
    m_changePokeCnt( 0 )
{

  m_strbuf = GFL_NEW(pHeap) STRBUF( SERVER_STRBUF_SIZE, pHeap );

  calc::ResetSys( randSeed );

  for(u32 i=0; i<BTL_CLIENT_MAX; ++i){
    Svcl_Clear( &m_client[i] );
    m_timeLimit.clientLimitTime[i] = clientLimitTime;
  }

  m_flowWork = GFL_NEW(pHeap) ServerFlow( this, m_mainModule, m_pokeCon, &m_que, pHeap );
  m_escapeInfo = m_flowWork->GetEscapeInfoPointer();
  m_serverCommandCalculator =  GFL_NEW( pHeap ) ServerCommandCalculator();
  m_adapterCommandGenerator = createAdapterCommandGenerator( pHeap );

  changeSequence( SERVER_SEQ_BATTLE_START_SETUP );
}

/**
 * @brief アダプターコマンド生成オブジェクトを作成する
 * @param heap  インスタンスの生成に使用するヒープ
 */
AdapterCommandGenerator* BTL_SERVER::createAdapterCommandGenerator( HeapHandle heap )
{
  AdapterCommandGenerator::SetupParam setupParam;
  setupParam.sendDataContainer      = m_sendDataContainer;
  setupParam.serverFlow             = m_flowWork;
  setupParam.serverCommandQueue     = &m_que;
  setupParam.serverCommandCalclator = m_serverCommandCalculator;
  setupParam.recTool                = &m_recTool;
  setupParam.clientWorkArray        = m_client;
  setupParam.clientAction           = &m_clientAction;
  setupParam.changePokePosArray     = m_changePokePos;
  setupParam.changePokePosNum       = &m_changePokeCnt;
  setupParam.enemyPutPokeId         = &m_enemyPutPokeID;
  setupParam.battleResultContext    = &m_btlResultContext;
  setupParam.escapeInfo             = m_escapeInfo;
  setupParam.timeLimit              = &m_timeLimit;
  return GFL_NEW( heap ) AdapterCommandGenerator( setupParam );
}

BTL_SERVER::~BTL_SERVER()
{
  GFL_SAFE_DELETE( m_adapterCommandGenerator );
  GFL_SAFE_DELETE( m_serverCommandCalculator );
  GFL_DELETE  m_flowWork;
  GFL_DELETE  m_strbuf;
}

//--------------------------------------------------------------------------------------
/**
 * @brief クライアントとのアダプタを接続する
 *
 * @param clientID  接続するクライアントのID
 * @param adapter   接続するアダプタ（既にクライアントに関連付けられている）
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER::AttachClient( u8 clientID, Adapter* adapter )
{
  GFL_ASSERT(clientID < BTL_CLIENT_MAX);
  GFL_ASSERT(m_client[clientID].adapter==NULL);

  {
    SVCL_WORK* client;
    BTL_PARTY* party;

    party = m_pokeCon->GetPartyData( clientID );
    client = &m_client[ clientID ];

    Svcl_Setup( client, clientID, adapter );
    Svcl_SetParty( client, party );
  }
}

//=============================================================================================
/**
 * コマンド整合性チェックモードでクライアントをセットアップ
 *
 * @param   server
 * @param   clientID
 */
//=============================================================================================
void BTL_SERVER::CmdCheckMode( u8 clientID )
{
  GFL_ASSERT(clientID < BTL_CLIENT_MAX);
  GFL_ASSERT(m_client[clientID].adapter==NULL);

  {
    SVCL_WORK* client;
    BTL_PARTY* party;

    party = m_pokeCon->GetPartyData( clientID );
    client = &m_client[ clientID ];

    Svcl_Setup( client, clientID, NULL );
    Svcl_SetParty( client, party );
  }
}

//----------------------------------------------------------------------------------
/**
 * 終了時、逃げたクライアントの情報を取得
 */
//----------------------------------------------------------------------------------
void BTL_SERVER::GetEscapeInfo( EscapeInfo* dst )
{
  m_escapeInfo->Copy( dst );
}

//--------------------------------------------------------------------------------------
/**
 * 全クライアント生成・接続完了後のスタートアップ処理
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER::Startup( void )
{
  changeSequence( SERVER_SEQ_BATTLE_START_SETUP );
  initAllAdapter();
  m_flowWork->ResetSystem();
}

//--------------------------------------------------------------------------------------
/**
 * 戦闘の途中でサーバーがいなくなった後、新サーバーとしてのスタートアップ処理
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER::StartupAsNewServer( void )
{
  changeSequence( SERVER_SEQ_NEW_STARTUP_SEND_LATEST_REPLY );
  resetAdapterCmd();
}

//--------------------------------------------------------------------------------------
/**
 * @brief クライアントからの返信待ち状態か？
 * @retval true   返信待ち状態である
 * @retval false  返信待ち状態でない
 */
//--------------------------------------------------------------------------------------
bool BTL_SERVER::IsWaitingClientReply( void ) const
{
  for( u32 clientId=0; clientId<BTL_CLIENT_MAX; clientId++ )
  {
    const SVCL_WORK* clientWork    = &m_client[ clientId ];
    const Adapter*   clientAdapter = clientWork->adapter;
    if( ( Svcl_IsEnable( clientWork ) &&
        ( clientAdapter->IsWaitingClientReply() ) ) )
    {
      return true;
    }
  }

  return false;
}

//--------------------------------------------------------------------------------------
/**
 * サーバメインループ
 *
 * @param   sv      サーバハンドラ
 *
 * @retval  bool    終了時 true を返す
 */
//--------------------------------------------------------------------------------------
bool BTL_SERVER::MainLoop( void )
{
  return callMainProc();
}

/**
 * @brief メインループ呼び出し
 *
 * @retval true   バトルが終了した場合
 * @retval false  バトルが継続する場合
 */
bool BTL_SERVER::callMainProc( void )
{
  if( m_currentSequence == SERVER_SEQ_END )
  {
    BTL_PRINT( "[BTL_SERVER] バトルが終了しました\n" );
    return true;
  }

  static const SequenceFunc SEQUENCE_TABLE[ SERVER_SEQ_NUM ] = 
  {
    /* SERVER_SEQ_BATTLE_START_SETUP                    */ &BTL_SERVER::serverMain_BATTLE_START_SETUP,
    /* SERVER_SEQ_BATTLE_START_SWITCH                   */ &BTL_SERVER::serverMain_BATTLE_START_SWITCH,
    /* SERVER_SEQ_BATTLE_START_TIMING                   */ &BTL_SERVER::serverMain_BATTLE_START_TIMING,
    /* SERVER_SEQ_BATTLE_START_COMMAND                  */ &BTL_SERVER::serverMain_BATTLE_START_COMMAND,
    /* SERVER_SEQ_BATTLE_START_RECORD                   */ &BTL_SERVER::serverMain_BATTLE_START_RECORD,
    /* SERVER_SEQ_ACTION_SELECT_START                   */ &BTL_SERVER::serverMain_ACTION_SELECT_START,
    /* SERVER_SEQ_ACTION_SELECT_SWITCH                  */ &BTL_SERVER::serverMain_ACTION_SELECT_SWITCH,
    /* SERVER_SEQ_ACTION_SELECT_RECORD                  */ &BTL_SERVER::serverMain_ACTION_SELECT_RECORD,
    /* SERVER_SEQ_ACTION_SELECT_COMMAND                 */ &BTL_SERVER::serverMain_ACTION_SELECT_COMMAND,
    /* SERVER_SEQ_ACTION_SELECT_COMMAND_SWITCH          */ &BTL_SERVER::serverMain_ACTION_SELECT_COMMAND_SWITCH,
    /* SERVER_SEQ_CHANGE_OR_ESCAPE_START                */ &BTL_SERVER::serverMain_CHANGE_OR_ESCAPE_START,
    /* SERVER_SEQ_CHANGE_OR_ESCAPE_SWITCH               */ &BTL_SERVER::serverMain_CHANGE_OR_ESCAPE_SWITCH,
    /* SERVER_SEQ_CHANGE_OR_ESCAPE_ESCAPE_SELECTED      */ &BTL_SERVER::serverMain_CHANGE_OR_ESCAPE_ESCAPE_SELECTED,
    /* SERVER_SEQ_COVER_SELECT_START                    */ &BTL_SERVER::serverMain_COVER_SELECT_START,
    /* SERVER_SEQ_COVER_COMFIRM_PLAYER_POKECHANGE       */ &BTL_SERVER::serverMain_COVER_COMFIRM_PLAYER_POKECHANGE,
    /* SERVER_SEQ_COVER_SELECT_SWITCH                   */ &BTL_SERVER::serverMain_COVER_SELECT_SWITCH,
    /* SERVER_SEQ_COVER_SELECT_RECORD                   */ &BTL_SERVER::serverMain_COVER_SELECT_RECORD,
    /* SERVER_SEQ_COVER_COMMAND                         */ &BTL_SERVER::serverMain_COVER_COMMAND,
    /* SERVER_SEQ_COVER_COMMAND_SWITCH                  */ &BTL_SERVER::serverMain_COVER_COMMAND_SWITCH,
    /* SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT        */ &BTL_SERVER::serverMain_INTERVAL_POKECHANGE_POKESELECT,
    /* SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT_SWITCH */ &BTL_SERVER::serverMain_INTERVAL_POKECHANGE_POKESELECT_SWITCH,
    /* SERVER_SEQ_INTERVAL_POKECHANGE_RECORD            */ &BTL_SERVER::serverMain_INTERVAL_POKECHANGE_RECORD,
    /* SERVER_SEQ_INTERVAL_POKECHANGE_COMMAND           */ &BTL_SERVER::serverMain_INTERVAL_POKECHANGE_COMMAND,
    /* SERVER_SEQ_INTERVAL_POKECHANGE_SWITCH            */ &BTL_SERVER::serverMain_INTERVAL_POKECHANGE_SWITCH,
    /* SERVER_SEQ_INTRUDE_PREPARE                       */ &BTL_SERVER::serverMain_INTRUDE_PREPARE,
    /* SERVER_SEQ_INTRUDE_COMMAND                       */ &BTL_SERVER::serverMain_INTRUDE_COMMAND,
    /* SERVER_SEQ_TIMEUP_RECORD                         */ &BTL_SERVER::serverMain_TIMEUP_RECORD,
    /* SERVER_SEQ_TIMEUP_COMMAND                        */ &BTL_SERVER::serverMain_TIMEUP_COMMAND,
    /* SERVER_SEQ_EXIT_COMMON                           */ &BTL_SERVER::serverMain_EXIT_COMMON,
    /* SERVER_SEQ_EXIT_SWITCH                           */ &BTL_SERVER::serverMain_EXIT_SWITCH,
    /* SERVER_SEQ_EXIT_NPC                              */ &BTL_SERVER::serverMain_EXIT_NPC,
    /* SERVER_SEQ_EXIT_BATTLE_SPOT_TRAINER              */ &BTL_SERVER::serverMain_EXIT_BATTLE_SPOT_TRAINER,
    /* SERVER_SEQ_EXIT_COMM                             */ &BTL_SERVER::serverMain_EXIT_COMM,
    /* SERVER_SEQ_EXIT_WILD_WIN                         */ &BTL_SERVER::serverMain_EXIT_WILD_WIN,
    /* SERVER_SEQ_EXIT_WILD_LOSE                        */ &BTL_SERVER::serverMain_EXIT_WILD_LOSE,
    /* SERVER_SEQ_EXIT_WILD_CAPTURE                     */ &BTL_SERVER::serverMain_EXIT_WILD_CAPTURE,
    /* SERVER_SEQ_EXIT_KEY_WAIT                         */ &BTL_SERVER::serverMain_EXIT_KEY_WAIT,
    /* SERVER_SEQ_EXIT_FADEOUT                          */ &BTL_SERVER::serverMain_EXIT_FADEOUT,
    /* SERVER_SEQ_EXIT_QUIT                             */ &BTL_SERVER::serverMain_EXIT_QUIT,
    /* SERVER_SEQ_NEW_STARTUP_SEND_LATEST_REPLY         */ &BTL_SERVER::serverMain_NEW_STARTUP_SEND_LATEST_REPLY,
    /* SERVER_SEQ_NEW_STARTUP_SWITCH                    */ &BTL_SERVER::serverMain_NEW_SEND_SWITCH,
    /* SERVER_SEQ_END                                   */ NULL,
  };

  const SequenceFunc seqFunc = SEQUENCE_TABLE[ m_currentSequence ];
  if( seqFunc == NULL )
  {
    GFL_ASSERT(0);
    return true;
  }

  const ServerSequence nextSeq = ( this->*seqFunc )( &m_seqStep );
  if( nextSeq != m_currentSequence )
  {
    changeSequence( nextSeq );
  }

  return false;
}

/**
 * @brief シーケンスを変更する
 * @param nextSeq  次のシーケンス
 */
void BTL_SERVER::changeSequence( ServerSequence nextSeq )
{
  BTL_PRINT( "[BTL_SERVER] シーケンス遷移( %d -> %d )\n", m_currentSequence, nextSeq );
  m_currentSequence = nextSeq;
  m_seqStep = 0;
}



//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘開始直後のセットアップ
 *
 * @note ACMD_WAIT_SETUP  送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_BATTLE_START_SETUP( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_START_SETUP );
    (*seqStep)++;
    /* fallthru */

  case 1:
    if( waitAllAdapterReply() )
    {
      BTL_PRINT( "全クライアントからリプライあり %d\n", __LINE__ );
      resetAdapterCmd();
      return SERVER_SEQ_BATTLE_START_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_BATTLE_START_SETUP;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：セットアップ後の分岐
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_BATTLE_START_SWITCH( int* seqStep )
{
  // 入場演出処理後、捕獲デモ以外の処理
  if( m_mainModule->GetCompetitor() != BTL_COMPETITOR_DEMO_CAPTURE )
  {
    return SERVER_SEQ_BATTLE_START_TIMING;
  }
  // 捕獲デモなら
  else
  {
    return SERVER_SEQ_EXIT_COMMON;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘開始時のタイミングコード送信
 *
 * @note ACMD_RECORD_DATA 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_BATTLE_START_TIMING( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_RECORD_BATTLE_START_TIMING );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      BTL_PRINT( "全クライアントからリプライあり %d\n", __LINE__ );
      resetAdapterCmd();
      return SERVER_SEQ_BATTLE_START_COMMAND;
    }
    break;
  }

  return SERVER_SEQ_BATTLE_START_TIMING;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：初期ポケモン投入コマンド
 *
 * @note ACMD_SERVER_CMD  送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_BATTLE_START_COMMAND( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_SERVER_COMMAND_FIRST_BATTLE_IN );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      BTL_PRINT( "全クライアントからリプライあり %d\n", __LINE__ );
      resetAdapterCmd();
      return SERVER_SEQ_BATTLE_START_RECORD;
    }
    break;
  }

  return SERVER_SEQ_BATTLE_START_COMMAND;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：バトル開始の録画チャプター記録
 *
 * @note ACMD_RECORD_DATA 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_BATTLE_START_RECORD( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_RECORD_BATTLE_START_CHAPTER );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      BTL_PRINT( "全クライアントからリプライあり %d\n", __LINE__ );
      resetAdapterCmd();
      return SERVER_SEQ_ACTION_SELECT_START;
    }
    break;
  }

  return SERVER_SEQ_BATTLE_START_RECORD;
}




//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：行動選択開始
 *
 * @note ACMD_SELECT_ACTION 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_ACTION_SELECT_START( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    // 持ち時間が無くなったクライアントが存在する場合
    if( this->isClientLimitTimeOver() )
    {
      return SERVER_SEQ_TIMEUP_RECORD;
    }
    (*seqStep)++;
    // fall through

  case 1:
    sendRequest( SERVER_REQ_START_ACTION_SELECT );
    (*seqStep)++;
    break;

  case 2:
    if( waitAllAdapterReply() )
    {
      BTL_N_Printf( DBGSTR_SERVER_ActionSelectDoneAll );
      storeClientAction();
      resetAdapterCmd();
      return SERVER_SEQ_ACTION_SELECT_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_ACTION_SELECT_START;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：行動選択開始後の分岐
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_ACTION_SELECT_SWITCH( int* seqStep )
{
  GFL_UNUSED( seqStep );

  // 試合制限時間切れのチェック
  if( m_mainModule->CheckGameLimitTimeOver() )
  {
    return SERVER_SEQ_TIMEUP_RECORD;
  }

  // 録画再生時のエラーチェック
  if( m_mainModule->CheckRecPlayError() )
  {
    return SERVER_SEQ_EXIT_COMMON;
  }

  return SERVER_SEQ_ACTION_SELECT_RECORD;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：行動選択の結果を記録
 *
 * @note ACMD_RECORD_DATA   送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_ACTION_SELECT_RECORD( int* seqStep )
{
  enum
  {
    SEQ_Record,                    // 行動選択を共有
    SEQ_Record_Wait,               // 行動選択を共有( 返信待ち )
    SEQ_ClientLimitTimeReq,        // 手持ち時間送信リクエスト
    SEQ_ClientLimitTimeReq_Wait,   // 手持ち時間送信リクエスト( 返信待ち )
    SEQ_ClientLimitTimeSync,       // 手持ち時間を同期
    SEQ_ClientLimitTimeSync_Wait,  // 手持ち時間を同期( 返信待ち )
    SEQ_End,
  };

  switch( *seqStep )
  {
  case SEQ_Record:
    sendRequest( SERVER_REQ_RECORD_SELECTED_ACTION );
    (*seqStep)++;
    break;

  case SEQ_Record_Wait:
    if( waitAllAdapterReply() )
    {
       resetAdapterCmd();
       *seqStep = ( m_mainModule->IsClientLimitTimeExist() ) ? ( SEQ_ClientLimitTimeReq ) : ( SEQ_End );
    }
    break;

  case SEQ_ClientLimitTimeReq:
    sendRequest( SERVER_REQ_SEND_CLIENT_LIMIT_TIME );
    (*seqStep)++;
    break;

  case SEQ_ClientLimitTimeReq_Wait:
    if( waitAllAdapterReply() )
    {
      storeClientLimitTime();
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case SEQ_ClientLimitTimeSync:
    sendRequest( SERVER_REQ_SYNC_CLIENT_LIMIT_TIME );
    (*seqStep)++;
    break;

  case SEQ_ClientLimitTimeSync_Wait:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case SEQ_End:
    return SERVER_SEQ_ACTION_SELECT_COMMAND;
  }

  return SERVER_SEQ_ACTION_SELECT_RECORD;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：行動選択後のターン再生コマンド
 *
 * @note ACMD_SERVER_CMD    送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_ACTION_SELECT_COMMAND( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    resetAdapterCmd();
    sendRequest( SERVER_REQ_SERVER_COMMAND_AFTER_ACTION_SELECT );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      return SERVER_SEQ_ACTION_SELECT_COMMAND_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_ACTION_SELECT_COMMAND;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：ターン再生後の分岐
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_ACTION_SELECT_COMMAND_SWITCH( int* seqStep )
{
  GFL_UNUSED( seqStep );

  switch( m_adapterCommandGenerator->GetFlowResult() )
  {
  case ServerFlow::RESULT_DEFAULT:
    return SERVER_SEQ_ACTION_SELECT_START;

  case ServerFlow::RESULT_LEVELUP:
    return SERVER_SEQ_ACTION_SELECT_COMMAND;

  case ServerFlow::RESULT_INTRUDE:
    return SERVER_SEQ_INTRUDE_PREPARE;

  case ServerFlow::RESULT_POKE_COVER:
    BTL_N_Printf( DBGSTR_SV_PokeInReqForEmptyPos );
    if( isPokeCoverEscapeMode() )
    {
      return SERVER_SEQ_CHANGE_OR_ESCAPE_START; // 野生シングルor群れで、自分側が倒されていた場合は「次を出す／逃げる」を選択するモードへ
    }
    else
    {
      return SERVER_SEQ_COVER_SELECT_START;
    }

  case ServerFlow::RESULT_POKE_CHANGE:
    BTL_N_Printf( DBGSTR_SV_ChangePokeOnTheTurn );
    GFL_ASSERT( m_changePokeCnt );
    return SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT;

  case ServerFlow::RESULT_BTL_QUIT_CAPTURE:
    {
      BtlPokePos pos = m_flowWork->GetCapturedPokePos();
      m_mainModule->NotifyCapturedPokePos( pos );
      return SERVER_SEQ_EXIT_COMMON;
    }

  case ServerFlow::RESULT_BTL_QUIT_SHOWDOWN:
    {
      ResultCause result_cause;
      BtlResult result = m_flowWork->ChecBattleResult( &result_cause );
      m_mainModule->NotifyBattleResult( result, result_cause );
      return SERVER_SEQ_EXIT_COMMON;
    }

  default:
    GFL_ASSERT(0);
    /* fallthru */

  case ServerFlow::RESULT_BTL_QUIT_ESCAPE:
    BTL_N_Printf( DBGSTR_SVFL_GotoQuit );
    return SERVER_SEQ_EXIT_FADEOUT;
  }
}

/**
 * serverFlowからポケモン入れ替えリクエストが帰ってきた場合のモード判定
 * （野生シングルor乱入で、自分側が倒されていた場合は「次を出す／逃げる」を選択するモードになる）
 *
 * @retval  bool    「次出す／逃げる」選択モードならtrue / 通常の空き埋めモードなら false
 */
bool BTL_SERVER::isPokeCoverEscapeMode( void )
{
  if( m_mainModule->GetCompetitor() == BTL_COMPETITOR_WILD )
  {
    // 試練中は逃げられない
    if( m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_SIREN_BATTLE ) ||
        m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) ||
        // @fix GFMMCat[209] VSヒカリドラゴン戦で、にげることができる
        // @fix GFMMCat[547] 合体ネクロズマ戦にて逃げられる
        m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_NO_CAPTURE ) )
    {
      return false;
    }
    BtlRule rule = m_mainModule->GetRule();
    if( ( rule == BTL_RULE_SINGLE ) ||
        ( rule == BTL_RULE_INTRUDE ) )
    {
      return checkPokeChangeReqClient( BTL_CLIENTID_SA_PLAYER );
    }
  }

  return false;
}





//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：逃げる、次を出す　選択開始
 *
 * @note シングル・野生戦のみ
 * @note ACMD_SELECT_CHANGE_OR_ESCAPE 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_CHANGE_OR_ESCAPE_START( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_START_CHANGE_OR_ESCAPE );
    ++(*seqStep);
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      return SERVER_SEQ_CHANGE_OR_ESCAPE_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_CHANGE_OR_ESCAPE_START;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：逃げる、次を出す　選択後の分岐
 *
 * @note シングル・野生戦のみ
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_CHANGE_OR_ESCAPE_SWITCH( int* seqStep )
{
  GFL_UNUSED( seqStep );

  u8 clientID = m_mainModule->GetPlayerClientID();
  const SendData& returnData = m_client[clientID].adapter->GetReturnData();
  const void* data = returnData.GetData();
  const u8* result = reinterpret_cast<const u8*>( data );
  if( (*result) == BTL_CLIENTASK_CHANGE )
  {
    return SERVER_SEQ_COVER_SELECT_START;
  }
  else
  {
    return SERVER_SEQ_CHANGE_OR_ESCAPE_ESCAPE_SELECTED;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：逃げる、次を出す　「逃げる」を選択した場合
 *
 * @note シングル・野生戦のみ
 * @note ACMD_SERVER_CMD 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_CHANGE_OR_ESCAPE_ESCAPE_SELECTED( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_SERVER_COMMAND_ESCAPE_BY_CHANGE_OR_ESCAPE );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();

      if( m_adapterCommandGenerator->IsEscapeSucceededOnChangeOrEscape() )
      {
        return SERVER_SEQ_EXIT_FADEOUT;
      }
      else
      {
        return SERVER_SEQ_COVER_SELECT_START;
      }
    }
    break;
  }

  return SERVER_SEQ_CHANGE_OR_ESCAPE_ESCAPE_SELECTED;
}




//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：カバーポケモン投入　投入ポケモン選択開始
 *
 * @note ACMD_SELECT_POKEMON_FOR_COVER 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_COVER_SELECT_START( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_START_COVER_POKE_SELECT );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      return SERVER_SEQ_COVER_COMFIRM_PLAYER_POKECHANGE;
    }
    break;
  }

  return SERVER_SEQ_COVER_SELECT_START;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：カバーポケモン投入　プレイヤへのポケモン入れ替え確認
 *
 * @note ACMD_CONFIRM_IREKAE 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_COVER_COMFIRM_PLAYER_POKECHANGE( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    if( irekae_IsNeedConfirm() )
    {
      m_enemyPutPokeID = irekae_GetEnemyPutPokeID();
      sendRequestAlone( SERVER_REQ_START_CONFIRM_COUNTER_POKECHANGE, BTL_CLIENTID_SA_PLAYER );
    }
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      storeClientAction();
      resetAdapterCmd();
      return SERVER_SEQ_COVER_SELECT_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_COVER_COMFIRM_PLAYER_POKECHANGE;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：カバーポケモン投入　投入ポケモン選択後の分岐
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_COVER_SELECT_SWITCH( int* seqStep )
{
  // 録画再生時のエラーチェック
  if( m_mainModule->CheckRecPlayError() )
  {
    return SERVER_SEQ_EXIT_COMMON;
  }
  return SERVER_SEQ_COVER_SELECT_RECORD;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：カバーポケモン投入　投入ポケモン選択結果を記録
 *
 * @note ACMD_RECORD_DATA 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_COVER_SELECT_RECORD( int* seqStep )
{
  enum
  {
    SEQ_Record,                    // 行動選択を共有
    SEQ_Record_Wait,               // 行動選択を共有( 返信待ち )
    SEQ_ClientLimitTimeReq,        // 手持ち時間送信リクエスト
    SEQ_ClientLimitTimeReq_Wait,   // 手持ち時間送信リクエスト( 返信待ち )
    SEQ_ClientLimitTimeSync,       // 手持ち時間を同期
    SEQ_ClientLimitTimeSync_Wait,  // 手持ち時間を同期( 返信待ち )
    SEQ_End,
  };

  switch( *seqStep )
  {
  case SEQ_Record:
    sendRequest( SERVER_REQ_RECORD_COVER_POKE_SELECT );
    (*seqStep)++;
    break;

  case SEQ_Record_Wait:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      *seqStep = ( m_mainModule->IsClientLimitTimeExist() ) ? ( SEQ_ClientLimitTimeReq ) : ( SEQ_End );
    }
    break;

  case SEQ_ClientLimitTimeReq:
    sendRequest( SERVER_REQ_SEND_CLIENT_LIMIT_TIME );
    (*seqStep)++;
    break;

  case SEQ_ClientLimitTimeReq_Wait:
    if( waitAllAdapterReply() )
    {
      storeClientLimitTime();
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case SEQ_ClientLimitTimeSync:
    sendRequest( SERVER_REQ_SYNC_CLIENT_LIMIT_TIME );
    (*seqStep)++;
    break;

  case SEQ_ClientLimitTimeSync_Wait:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case SEQ_End:
    return SERVER_SEQ_COVER_COMMAND;
  }

  return SERVER_SEQ_COVER_SELECT_RECORD;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：カバーポケモン投入　ポケモン投入後のコマンド
 *
 * @note ACMD_SERVER_CMD 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_COVER_COMMAND( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    resetAdapterCmd();
    sendRequest( SERVER_REQ_SERVER_COMMAND_AFTER_COVER_POKE_SELECT );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      return SERVER_SEQ_COVER_COMMAND_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_COVER_COMMAND;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：カバーポケモン投入　ポケモン投入後のコマンド再生後の分岐
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_COVER_COMMAND_SWITCH( int* seqStep )
{
  GFL_UNUSED( seqStep );

  switch( m_adapterCommandGenerator->GetFlowResult() )
  {
  case ServerFlow::RESULT_POKE_COVER:
    return SERVER_SEQ_COVER_SELECT_START;

  // @fix NMCat[429] 瀕死後の入れ替え処理で、「ききかいひ」「にげごし」の効果が発動するとはまってしまう
  case ServerFlow::RESULT_POKE_CHANGE:
    return SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT;

  case ServerFlow::RESULT_LEVELUP:
    return SERVER_SEQ_COVER_COMMAND;

  case ServerFlow::RESULT_INTRUDE:
    return SERVER_SEQ_INTRUDE_PREPARE;

  case ServerFlow::RESULT_BTL_QUIT_SHOWDOWN:
    {
      ResultCause result_cause;
      BtlResult result = m_flowWork->ChecBattleResult( &result_cause );
      m_mainModule->NotifyBattleResult( result, result_cause );
      return SERVER_SEQ_EXIT_COMMON;
    }

  case ServerFlow::RESULT_BTL_QUIT_ESCAPE:
    BTL_N_Printf( DBGSTR_SVFL_GotoQuit );
    return SERVER_SEQ_EXIT_FADEOUT;

  default:
    return SERVER_SEQ_ACTION_SELECT_START;
  }
}


/**
 *  スタンドアロン入れ替え戦にて、プレイヤー側に入れ替えを確認する必要があるか判定
 */
bool BTL_SERVER::irekae_IsNeedConfirm( void )
{
  if( m_mainModule->IsIrekaeMode() )
  {
    if( m_changePokeCnt )
    {
      // プレイヤー側も瀕死リクエストがある場合、入れ替え確認の必要なし
      if( checkPokeChangeReqClient(BTL_CLIENTID_SA_PLAYER) ){
        return false;
      }

      // 交替できるポケモンがいない場合は確認の必要なし
      {
        BTL_PARTY* party = m_pokeCon->GetPartyData( m_mainModule->GetPlayerClientID() );
        if( party->GetAliveMemberCount() < 2 ){
          return false;
        }
      }

      return true;
    }
  }
  return false;
}
/**
 *  スタンドアロン入れ替え戦にて、敵側が次にくり出すポケモンIDを取得
 */
u8 BTL_SERVER::irekae_GetEnemyPutPokeID( void )
{
  SVCL_WORK*  svcl = &m_client[ BTL_CLIENTID_SA_ENEMY1 ];

  if( Svcl_IsEnable(svcl) )
  {
    const SendData& returnData = svcl->adapter->GetReturnData();
    const void* data = returnData.GetData();
    const SEND_DATA_ACTION_SELECT* actionSelectData = reinterpret_cast<const SEND_DATA_ACTION_SELECT*>( data );
    const BTL_ACTION_PARAM* actParam = &( actionSelectData->actionParam[0] );

    if( BTL_ACTION_GetAction(actParam) == BTL_ACTION_CHANGE )
    {
      const BTL_POKEPARAM* bpp = m_pokeCon->GetClientPokeDataConst(
                  BTL_CLIENTID_SA_ENEMY1, actParam->change.memberIdx );
      return bpp->GetID();
    }
  }
  return BTL_POKEID_NULL;
}





//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：ターン途中のポケモン入れ替え　投入ポケモンの選択
 *
 * @note ACMD_SELECT_POKEMON_FOR_CHANGE 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_INTERVAL_POKECHANGE_POKESELECT( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_START_SELECT_POKE_BY_INTERVAL_POKECHANGE );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      storeClientAction();
      resetAdapterCmd();
      return SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：ターン途中のポケモン入れ替え　投入ポケモン選択後の分岐
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_INTERVAL_POKECHANGE_POKESELECT_SWITCH( int* seqStep )
{
  GFL_UNUSED( seqStep );

  // 録画再生時のエラーチェック
  if( m_mainModule->CheckRecPlayError() )
  {
    return SERVER_SEQ_EXIT_COMMON;
  }

  return SERVER_SEQ_INTERVAL_POKECHANGE_RECORD;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：ターン途中のポケモン入れ替え　投入ポケモンの選択結果を記録
 *
 * @note ACMD_RECORD_DATA 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_INTERVAL_POKECHANGE_RECORD( int* seqStep )
{
  enum
  {
    SEQ_Record,                    // 行動選択を共有
    SEQ_Record_Wait,               // 行動選択を共有( 返信待ち )
    SEQ_ClientLimitTimeReq,        // 手持ち時間送信リクエスト
    SEQ_ClientLimitTimeReq_Wait,   // 手持ち時間送信リクエスト( 返信待ち )
    SEQ_ClientLimitTimeSync,       // 手持ち時間を同期
    SEQ_ClientLimitTimeSync_Wait,  // 手持ち時間を同期( 返信待ち )
    SEQ_End,
  };

  switch( *seqStep )
  {
  case SEQ_Record:
    sendRequest( SERVER_REQ_RECORD_INTERVAL_POKECHANGE );
    (*seqStep)++;
    break;

  case SEQ_Record_Wait:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      *seqStep = ( m_mainModule->IsClientLimitTimeExist() ) ? ( SEQ_ClientLimitTimeReq ) : ( SEQ_End );
    }
    break;

  case SEQ_ClientLimitTimeReq:
    sendRequest( SERVER_REQ_SEND_CLIENT_LIMIT_TIME );
    (*seqStep)++;
    break;

  case SEQ_ClientLimitTimeReq_Wait:
    if( waitAllAdapterReply() )
    {
      storeClientLimitTime();
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case SEQ_ClientLimitTimeSync:
    sendRequest( SERVER_REQ_SYNC_CLIENT_LIMIT_TIME );
    (*seqStep)++;
    break;

  case SEQ_ClientLimitTimeSync_Wait:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case SEQ_End:
    return SERVER_SEQ_INTERVAL_POKECHANGE_COMMAND;
  }

  return SERVER_SEQ_INTERVAL_POKECHANGE_RECORD;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：ターン途中のポケモン入れ替え　ターン途中のポケモン入れ替え後のターン再生コマンド
 *
 * @note ACMD_SERVER_CMD  送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_INTERVAL_POKECHANGE_COMMAND( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    SCQUE_Init( &m_que );
    resetAdapterCmd();
    sendRequest( SERVER_REQ_SERVER_COMMAND_AFTER_INTERVAL_POKECHANGE );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      return SERVER_SEQ_INTERVAL_POKECHANGE_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_INTERVAL_POKECHANGE_COMMAND;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：ターン途中のポケモン入れ替え　ターン途中のポケモン入れ替え後のターン再生コマンド再生後の分岐
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_INTERVAL_POKECHANGE_SWITCH( int* seqStep )
{
  GFL_UNUSED( seqStep );

  switch( m_adapterCommandGenerator->GetFlowResult() )
  {
  case ServerFlow::RESULT_POKE_CHANGE:
    return SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT;

  case ServerFlow::RESULT_LEVELUP:
    return SERVER_SEQ_INTERVAL_POKECHANGE_COMMAND;

  case ServerFlow::RESULT_INTRUDE:
    return SERVER_SEQ_INTRUDE_PREPARE;

  case ServerFlow::RESULT_POKE_COVER:
    return SERVER_SEQ_COVER_SELECT_START;

  case ServerFlow::RESULT_BTL_QUIT_SHOWDOWN:
    {
      ResultCause result_cause;
      BtlResult result = m_flowWork->ChecBattleResult( &result_cause );
      m_mainModule->NotifyBattleResult( result, result_cause );
      return SERVER_SEQ_EXIT_COMMON;
    }

  case ServerFlow::RESULT_BTL_QUIT_ESCAPE:
    BTL_N_Printf( DBGSTR_SVFL_GotoQuit );
    return SERVER_SEQ_EXIT_FADEOUT;

  default:
    return SERVER_SEQ_ACTION_SELECT_START;
  }
}





//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：乱入　準備
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_INTRUDE_PREPARE( int* seqStep )
{
  const BtlIntrudeType               intrudeType      = m_flowWork->GetIntrudeType();
  const BtlPokePos                   intrudePokePos   = m_flowWork->GetIntrudePokePos();
  const pml::pokepara::PokemonParam& intrudePokeParam = m_flowWork->GetIntrudePokeParam();
  m_mainModule->StartIntrudeBattle( intrudePokePos, intrudePokeParam, intrudeType );

  return SERVER_SEQ_INTRUDE_COMMAND;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：乱入　入場コマンド
 *
 * @note ACMD_SERVER_CMD  送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_INTRUDE_COMMAND( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    SCQUE_Init( &m_que );
    resetAdapterCmd();
    sendRequest( SERVER_REQ_SERVER_COMMAND_INTRUDE );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      return SERVER_SEQ_ACTION_SELECT_START;
    }
    break;
  }

  return SERVER_SEQ_INTRUDE_COMMAND;
}




//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：制限時間終了　録画チャプター記録
 *
 * @note ACMD_RECORD_DATA 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_TIMEUP_RECORD( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_RECORD_TIMEUP_CHAPTER );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      return SERVER_SEQ_TIMEUP_COMMAND;
    }
    break;
  }

  return SERVER_SEQ_TIMEUP_RECORD;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：制限時間終了　終了コマンド
 *
 * @note ACMD_NOTIFY_TIMEUP 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_TIMEUP_COMMAND( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_NOTIFY_TIMEUP );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();

      {
        ResultCause result_cause;
        BtlResult result = m_flowWork->ChecBattleResult( &result_cause );
        m_mainModule->NotifyBattleResult( result, result_cause );
        return SERVER_SEQ_EXIT_COMMON;
      }
    }
    break;
  }

  return SERVER_SEQ_TIMEUP_COMMAND;
}



//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　共通処理
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_COMMON( int* seqStep )
{
  GFL_UNUSED( seqStep );

  m_btlResultContext.resultCode  = m_mainModule->GetBattleResult();
  m_btlResultContext.resultCause = m_mainModule->GetBattleResultCause();
  m_btlResultContext.clientID    = m_mainModule->GetPlayerClientID();
  return SERVER_SEQ_EXIT_SWITCH;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　対戦相手ごとの分岐
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_SWITCH( int* seqStep )
{
  GFL_UNUSED( seqStep );

  BtlResult result = m_mainModule->GetBattleResult();
  BtlCompetitor  competitor = this->checkCompetitorExistType();
  BTL_PRINT("バトル終了ルート COMPETITOR=%d\n", competitor);

  switch( competitor )
  {
  case BTL_COMPETITOR_TRAINER:
    return SERVER_SEQ_EXIT_NPC;

  case BTL_COMPETITOR_INST:
    BTL_PRINT(" -> inst\n");
    return SERVER_SEQ_EXIT_BATTLE_SPOT_TRAINER;

  case BTL_COMPETITOR_COMM:
    return SERVER_SEQ_EXIT_COMM;

  case BTL_COMPETITOR_WILD:
    if( result == BTL_RESULT_WIN )
    {
      return SERVER_SEQ_EXIT_WILD_WIN;
    }
    else if( (result == BTL_RESULT_LOSE) || (result == BTL_RESULT_DRAW) )
    {
      return SERVER_SEQ_EXIT_WILD_LOSE;
    }
    else if( result == BTL_RESULT_CAPTURE )
    {
      return SERVER_SEQ_EXIT_WILD_CAPTURE;
    }
    else
    {
      return SERVER_SEQ_EXIT_KEY_WAIT;
    }

  default:
    if( result == BTL_RESULT_WIN )
    {
      m_mainModule->StartWinBGM();
    }
    return SERVER_SEQ_EXIT_KEY_WAIT;
  }
}

/**
 * バトル終了演出のタイプを取得（Competitorごとに別ける）
 * @return
 */
BtlCompetitor BTL_SERVER::checkCompetitorExistType( void )
{
  // 模擬戦の場合は相手がNPC由来かヒューマンプレイヤー由来かで別ける
  if( m_mainModule->IsRecordFightMode() )
  {
    if( m_mainModule->IsRecordFightWithNPCData() )
    {
      return BTL_COMPETITOR_INST;
    }
    else
    {
      return BTL_COMPETITOR_COMM;
    }
  }

  // それ以外はフツウに
  return m_mainModule->GetCompetitor();
}




//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　野生戦勝ち
 *
 * @note ACMD_EXIT_WIN_WILD 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_WILD_WIN( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_EXIT_WILD_WIN );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case 2:
    return SERVER_SEQ_EXIT_KEY_WAIT;
  }

  return SERVER_SEQ_EXIT_WILD_WIN;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　野生戦負け
 *
 * @note ACMD_EXIT_LOSE_WILD 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_WILD_LOSE( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_EXIT_WILD_LOSE );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case 2:
    return SERVER_SEQ_EXIT_KEY_WAIT;
  }

  return SERVER_SEQ_EXIT_WILD_LOSE;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　野生戦捕獲
 *
 * @note ACMD_EXIT_CAPTURE 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_WILD_CAPTURE( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_EXIT_WILD_CAPTURE );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case 2:
    return SERVER_SEQ_EXIT_KEY_WAIT;
  }

  return SERVER_SEQ_EXIT_WILD_CAPTURE;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　通信対戦
 *
 * @note ACMD_EXIT_COMM 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_COMM( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_EXIT_COMM );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case 2:
    return SERVER_SEQ_EXIT_FADEOUT;
  }

  return SERVER_SEQ_EXIT_COMM;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　NPC
 *
 * @note ACMD_EXIT_NPC 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_NPC( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_EXIT_NPC );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case 2:
    return SERVER_SEQ_EXIT_KEY_WAIT;
  }

  return SERVER_SEQ_EXIT_NPC;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　バトル施設トレーナー
 *
 * @note ACMD_EXIT_SUBWAY_TRAINER 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_BATTLE_SPOT_TRAINER( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_EXIT_BATTLE_SPOT_TRAINER );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      BTL_PRINT("サブウェイトレーナー終了コマンドがおわり\n");
      resetAdapterCmd();
      (*seqStep)++;
    }
    break;

  case 2:
    if( m_mainModule->GetCommMode() == BTL_COMM_NONE )
    {
      return SERVER_SEQ_EXIT_KEY_WAIT;
    }
    else
    {
      return SERVER_SEQ_EXIT_FADEOUT;
    }
  }

  return SERVER_SEQ_EXIT_BATTLE_SPOT_TRAINER;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　キー入力待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_KEY_WAIT( int*  )
{
  return SERVER_SEQ_EXIT_FADEOUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　フェードアウト
 *
 * @note ACMD_FADEOUT 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_FADEOUT( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_EXIT_FADEOUT );
    ++(*seqStep);
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      resetAdapterCmd();
      return SERVER_SEQ_EXIT_QUIT;
    }
    break;
  }

  return SERVER_SEQ_EXIT_FADEOUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：戦闘終了　終了コマンド発行
 *
 * @note ACMD_QUIT_BTL 送信　→　クライアントからの返信待ち
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_EXIT_QUIT( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    sendRequest( SERVER_REQ_QUIT_BATTLE );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      BTL_N_Printf( DBGSTR_SV_ReplyQuitACmad );
      return SERVER_SEQ_END;
    }
    break;
  }

  return SERVER_SEQ_EXIT_QUIT;
}


//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：新サーバーとしてのスタートアップ　最後に返信したデータを再送信してもらう
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_NEW_STARTUP_SEND_LATEST_REPLY( int* seqStep )
{
  switch( *seqStep )
  {
  case 0:
    BTL_PRINT( "[BTL_SERVER] 全クライアントに、再送信リクエストを送信します\n" );
    m_adapterCommandSerialNumber = SEND_DATA_SERIAL_NUMBER_NULL;
    sendRequest( SERVER_REQ_SEND_LATEST_REPLY );
    (*seqStep)++;
    break;

  case 1:
    if( waitAllAdapterReply() )
    {
      BTL_PRINT( "[BTL_SERVER] 全クライアントからの再送信を受信しました\n" );
      resetAdapterCmd();
      return SERVER_SEQ_NEW_STARTUP_SWITCH;
    }
    break;
  }

  return SERVER_SEQ_NEW_STARTUP_SEND_LATEST_REPLY;
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーシーケンス：新サーバーとしてのスタートアップ　再送信してもらったデータを元にシーケンス遷移する
 *
 * @param seqStep  シーケンス内のステップカウンタ
 *
 * @return 次のシーケンス
 */
//-----------------------------------------------------------------------------
ServerSequence BTL_SERVER::serverMain_NEW_SEND_SWITCH( int* seqStep )
{
  SendDataSerialNumber minSerialNo = SEND_DATA_SERIAL_NUMBER_NULL;
  ServerSequence nextSeq = SERVER_SEQ_NEW_STARTUP_SWITCH;

  for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
  {
    SVCL_WORK* clwk = &m_client[i];
    if( Svcl_IsEnable(clwk) )
    {
      const SendData& returnData = clwk->adapter->GetReturnData();
      const SendDataSerialNumber serialNo = returnData.GetSerialNumber();

      if( serialNo == SEND_DATA_SERIAL_NUMBER_NULL )
      {
        continue;
      }

      if( ( minSerialNo == SEND_DATA_SERIAL_NUMBER_NULL ) ||
          ( serialNo < minSerialNo ) )
      {
        minSerialNo = serialNo;
        nextSeq = returnData.GetServerSequence();
      }
    }
  }

  // @fix NMCat[4192] バトルロイヤルで「Host Migration Enable」が表示される直前に無線OFFを行うとアサートが表示されることがある
  // 再送信してもらったデータが全て無効値だったら、バトル開始処理からスタートする
  if( minSerialNo == SEND_DATA_SERIAL_NUMBER_NULL )
  {
    minSerialNo = SEND_DATA_SERIAL_NUMBER_MIN;
    nextSeq = SERVER_SEQ_BATTLE_START_SETUP;
  }

  m_adapterCommandSerialNumber = minSerialNo;

  BTL_PRINT( "[BTL_SERVER] 新サーバーとして、シーケンス %d から処理を開始します\n", nextSeq );
  BTL_PRINT( "[BTL_SERVER] 送信コマンドの通し番号は %d から開始します\n", m_adapterCommandSerialNumber );
  return nextSeq;
}



//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void BTL_SERVER::CMDCHECK_RestoreActionData(  const void* recData, u32 recDataSize )
{
  BTL_ACTION_PARAM action[ BTL_POSIDX_MAX ];
  u32 rp = 0;
  u8  clientID, numAction;

  for(u32 i=0; i<BTL_CLIENT_MAX; ++i){
    m_clientAction.count[ i ] = 0;
  }

  m_recTool.RestoreStart( recData, recDataSize );
  while( m_recTool.RestoreFwd( &rp, &clientID, &numAction, action) )
  {
    for(u32 i=0; i<numAction; ++i){
      m_clientAction.param[ clientID ][ i ] = action[ i ];
    }
    m_clientAction.count[ clientID ] = numAction;
  }
}

void BTL_SERVER::CMDCHECK_RestoreClientLimitTime( const SEND_DATA_CLIENT_LIMIT_TIME_SYNC& syncData )
{
  for( u32 clientID=0; clientID<BTL_CLIENT_NUM; ++clientID )
  {
    m_timeLimit.clientLimitTime[ clientID ] = syncData.limitTime[ clientID ];
    BTL_PRINT( "[BTL_SERVER] CMDCHECK_RestoreClientLimitTime syncData.limitTime[ %d ] = %d\n", clientID, syncData.limitTime[ clientID ] );
  }

  m_flowWork->SetTimeLimit( m_timeLimit );
}

/**
 * @brief コマンド整合性チェック
 *
 * @param timingCode  コマンド整合性チェックのタイミングコード
 * @param recvedCmd   受信したコマンドのバッファ
 * @param cmdSize     受信したコマンドのサイズ
 *
 * @retval true   整合性チェックNG
 * @retval false  整合性チェックOK
 */
bool BTL_SERVER::CMDCHECK_Make( rec::Timing timingCode, const void* recvedCmd, u32 cmdSize )
{
  SCQUE_Init( &m_que );

  AdapterCommand command;
  const void* pCalcCommand = NULL;
  u32 calcCommandSize = 0;

  GFL_PRINT( "[BTL_SERVER] ▼コマンド整合性チェック開始( timmingCode=%d )\n", timingCode );

  switch( timingCode )
  {
  case rec::RECTIMING_BtlIn:
    m_adapterCommandGenerator->GenerateCommandData( &command, &pCalcCommand, &calcCommandSize, SERVER_REQ_SERVER_COMMAND_FIRST_BATTLE_IN, m_adapterCommandSerialNumber );
    break;

  case rec::RECTIMING_StartTurn:
    m_adapterCommandGenerator->GenerateCommandData( &command, &pCalcCommand, &calcCommandSize, SERVER_REQ_SERVER_COMMAND_AFTER_ACTION_SELECT, m_adapterCommandSerialNumber );
    break;

  case rec::RECTIMING_PokeInCover:
    m_adapterCommandGenerator->GenerateCommandData( &command, &pCalcCommand, &calcCommandSize, SERVER_REQ_SERVER_COMMAND_AFTER_COVER_POKE_SELECT, m_adapterCommandSerialNumber );
    break;

  case rec::RECTIMING_PokeInChange:
    m_adapterCommandGenerator->GenerateCommandData( &command, &pCalcCommand, &calcCommandSize, SERVER_REQ_SERVER_COMMAND_AFTER_INTERVAL_POKECHANGE, m_adapterCommandSerialNumber );
    break;

  default:
    break;
  }

  GFL_PRINT( "[BTL_SERVER] ▲コマンド整合性チェック終了\n" );

  if( calcCommandSize == cmdSize )
  {
    if( gfl2::std::MemComp( pCalcCommand, recvedCmd, cmdSize ) == 0 )
    {
      BTL_N_Printf( DBGSTR_SV_CmdCheckOK );
      return false;
    }
  }

  // 以下、コマンド内容が完全一致しなかった場合のデバッグ出力
#if PM_DEBUG
  {
    BTL_PRINT( "[BTL_SERVER] コマンド整合性 NG timingCode=%d\n", timingCode );

    print_client_action( &m_clientAction );
    print_que_info( &m_que, "cmd by check_server" );

    SCQUE_Setup( &m_que, recvedCmd, cmdSize );
    BTL_PRINT( "  True Server Cmd = %d bytes, adrs=%p, %02x, %02x, %02x, ... %02x, %02x, %02x\n", cmdSize, recvedCmd,
        ((const u8*)recvedCmd)[0], ((const u8*)recvedCmd)[1], ((const u8*)recvedCmd)[2],
        m_que.buffer[0], m_que.buffer[1], m_que.buffer[2] );
    print_que_info( &m_que, "cmd by true_server" );
  }
#endif // PM_DEBUG

  return true;
}

#if PM_DEBUG

void BTL_SERVER::print_client_action( const SVCL_ACTION* clientAction ) const
{
  u32 i;
  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( clientAction->count[i] )
    {
      u32 j;
      BTL_PRINT("  Client(%d) action cnt=%d\n", i, clientAction->count[ i ] );
      for(j=0; j<clientAction->count[i]; ++j)
      {
        const BTL_ACTION_PARAM* action = &(clientAction->param[i][j]);

        BTL_PRINT("   Action(%d) = %d:", j, action->gen.cmd );
        if( action->gen.cmd == BTL_ACTION_FIGHT )
        {
          BTL_PRINT(" waza=%d, targetPos=%d", action->fight.waza, action->fight.targetPos );
        }
        BTL_PRINT("\n");
      }
    }
  }
}

void BTL_SERVER::print_que_info( SCQUE* que, const char* caption )
{
  GFL_UNUSED( que );
  GFL_UNUSED( caption );

  // TWL-DEBUGGER 上でないと文字列モジュールが正しく動かないので…
  //if( OS_GetConsoleType() & OS_CONSOLE_TWLDEBUGGER )
  {
    BTL_PRINT("  * %s : %d bytes\n", caption, que->writePtr );
    {
      static int args[ BTL_SERVERCMD_ARG_MAX ];
      ServerCmd  cmd;
      int i;

      for(i=0; i<(int)(que->writePtr); ++i)
      {
        BTL_PRINT("%02x,", que->buffer[i] );
        if( (i+1)%16==0 ){ BTL_PRINT("\n"); }
      }
      BTL_PRINT("\n");

      que->readPtr = 0;

      do {
        const char* scname;
        for(i=0; i<GFL_NELEMS(args); ++i){
          args[ i ] = -1;
        }

        cmd = scmd::SCQUE_Read( que, args, GFL_NELEMS(args) );
        scname = scmd::SCMD_GetLabelString(cmd);
        if( scname == NULL ){
          BTL_PRINT(" UnKnown Command [%d]  \n", cmd );
          return;
        }
        BTL_PRINT(" cmd=%04x ", cmd);
        BTL_PRINT( scname );
        BTL_PRINT(" args=" );
        for(i=0; i<GFL_NELEMS(args); ++i){
          BTL_PRINT(" %d,", args[i]);
        }
        // 追加引数のあるコマンド処理
        {
          u32 j, exArgCnt = 0;
          switch( cmd ){
          case SC_ACT_WAZA_DMG_PLURAL:  exArgCnt = args[0]; break;
          }
          if( exArgCnt )
          {
            BTL_PRINT("\n   ExArg=");
            for(j=0; j<exArgCnt; ++j){
              BTL_PRINT("%d, ", scmd::SCQUE_READ_ArgOnly(que) );
            }
            BTL_PRINT("\n");
          }
        }
        BTL_PRINT( "\n" );

      } while( !SCQUE_IsFinishRead(que) );

    }
  }
}

#endif // PM_DEBUG


/**
 *  各クライアントから返信のあったアクション内容を構造体に格納
 */
void BTL_SERVER::storeClientAction( void )
{
  SVCL_WORK* clwk;

  for( u32 clientID=0; clientID<BTL_CLIENT_MAX; ++clientID )
  {
    clwk = &m_client[ clientID ];
    m_clientAction.count[ clientID ] = 0;

    if( Svcl_IsEnable(clwk) )
    {
      const SendData&                returnData       = clwk->adapter->GetReturnData();
      const void*                    data             = returnData.GetData();
      const u32                      dataSize         = returnData.GetDataSize();
      const SEND_DATA_ACTION_SELECT* actionSelectData = reinterpret_cast<const SEND_DATA_ACTION_SELECT*>( data );
      const BTL_ACTION_PARAM*        action           = actionSelectData->actionParam;
      const u32                      numAction        = actionSelectData->actionNum;

      BTL_PRINT("[BTL_SERVER] Client(%d) の 送信データサイズ=%d, numAction=%d\n", clientID, dataSize, numAction);
      BTL_ACTION_Print( action, numAction );

      for(u32 actionIndex=0; actionIndex<numAction; ++actionIndex )
      {
        m_clientAction.param[ clientID ][ actionIndex ] = *action;
        m_clientAction.count[ clientID ]++;
        ++action;
      }
    }
  }
}

/**
 *  各クライアントから返信された残り手持ち時間を格納する
 */
void BTL_SERVER::storeClientLimitTime( void )
{
  for( u32 clientID=0; clientID<BTL_CLIENT_MAX; ++clientID )
  {
    SVCL_WORK* clwk = &m_client[ clientID ];

    if( Svcl_IsEnable(clwk) )
    {
      const SendData&                    returnData      = clwk->adapter->GetReturnData();
      const void*                        data            = returnData.GetData();
      const u32                          dataSize        = returnData.GetDataSize();
      const SEND_DATA_CLIENT_LIMIT_TIME* clientLimitTime = reinterpret_cast<const SEND_DATA_CLIENT_LIMIT_TIME*>( data );

      m_timeLimit.clientLimitTime[ clientID ] = clientLimitTime->limitTime;
      BTL_PRINT("[BTL_SERVER] Client(%d) の 残り手持ち時間=%d[s]\n", clientID, m_timeLimit.clientLimitTime[ clientID ] );
    }
  }

#if PM_DEBUG
  // 不正書き換え：相手の残り時間を0にする
  if( ui::CheckKeyCont( DIR_UP ) && ui::CheckKeyCont( BUTTON_X ) )
  {
    const u16 playerClientID = m_mainModule->GetPlayerClientID();
    for( u32 clientID=0; clientID<BTL_CLIENT_MAX; ++clientID )
    {
      if( clientID != playerClientID )
      {
        m_timeLimit.clientLimitTime[ clientID ] = 0;
        BTL_PRINT( "[BTL_SERVER] Client(%d) の 残り手持ち時間を不正に0に書き換えました\n" );
      }
    }
  }

  // 不正書き換え：自分の残り時間を99分にする
  if( ui::CheckKeyCont( DIR_DOWN ) && ui::CheckKeyCont( BUTTON_X ) )
  {
    const u16 playerClientID = m_mainModule->GetPlayerClientID();
    m_timeLimit.clientLimitTime[ playerClientID ] = (99*60);
    BTL_PRINT( "[BTL_SERVER] Client(%d) の 残り手持ち時間を不正に99分に書き換えました\n", playerClientID );
  }
#endif

  m_flowWork->SetTimeLimit( m_timeLimit );
}

/**
 * @brief 持ち時間が無くなったクライアントが存在するか？
 * @retval true   持ち時間が無くなったクライアントが存在する
 * @retval false  持ち時間が無くなったクライアントが存在しない
 */
bool BTL_SERVER::isClientLimitTimeOver( void ) const
{
  if( !m_mainModule->IsClientLimitTimeExist() )
  {
    return false;
  }

  for( u32 clientID=0; clientID<BTL_CLIENT_MAX; ++clientID )
  {
    const SVCL_WORK* clwk = &m_client[ clientID ];

    if( ( Svcl_IsEnable(clwk) ) &&
        ( m_timeLimit.clientLimitTime[ clientID ] == 0 ) )
    {
      return true;
    }
  }

  return false;
}


//=================================================================================================
//=================================================================================================


//----------------------------------------------------------------------------------
/**
 * @brief 全クライアントへリクエストを送信する
 * @param request  送信するリクエスト
 */
//----------------------------------------------------------------------------------
void BTL_SERVER::sendRequest( ServerRequest request )
{
  this->sendRequestCore( request, BTL_CLIENT_MAX );
}

//----------------------------------------------------------------------------------
/**
 * @brief 指定クライアントへのみ、リクエストを送信する
 * @param request   送信するリクエスト
 * @param clientID  送信対象クライアントのID
 */
//----------------------------------------------------------------------------------
void BTL_SERVER::sendRequestAlone( ServerRequest request, u8 clientID )
{
  this->sendRequestCore( request, clientID );
}

/**
 * @brief クライアントへのリクエストを送信する
 * @param request   送信するリクエスト
 * @param clientID  送信対象クライアントのID( BTL_CLIENT_MAX なら、全員に送信する )
 */
void BTL_SERVER::sendRequestCore( ServerRequest request, u8 clientID )
{
  AdapterCommand command;
  const void* data = NULL;
  u32 dataSize = 0;

  // コマンドを生成
  m_adapterCommandGenerator->GenerateCommandData( &command, &data, &dataSize, request, m_adapterCommandSerialNumber );

  // コマンドを送信対象クライアントのアダプタにセット
  if( clientID == BTL_CLIENT_MAX )
  {
    storeSendDataInAllAdapter( m_adapterCommandSerialNumber, m_currentSequence, command, data, dataSize );
  }
  else
  {
    storeSendDataInSingleAdapter( m_adapterCommandSerialNumber, m_currentSequence, command, data, dataSize, clientID );
  }

  // 送信したコマンドを記憶
  registerSendData( m_adapterCommandSerialNumber, m_currentSequence, command, data, dataSize );

  ++m_adapterCommandSerialNumber;
}

/**
 * @brief 全クライアントへの送信データをアダプタのバッファにセットする
 *
 * @param   serialNumber  送信データの通し番号
 * @param   serverSeq     送信時のサーバーシーケンス
 * @param   cmd           送信するコマンド
 * @param   sendData      送信データ本体
 * @param   dataSize      送信データ本体のサイズ
 */
void BTL_SERVER::storeSendDataInAllAdapter( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand cmd, const void* sendData, u32 dataSize )
{
  Adapter::BeginSetCmd();

  for(u32 i=0; i<BTL_CLIENT_MAX; i++)
  {
    SVCL_WORK* svcl = &m_client[ i ];

    if( Svcl_IsEnable(svcl) ){
      svcl->adapter->SetCmd( serialNumber, serverSeq, cmd, sendData, dataSize );
    }
  }

  Adapter::EndSetCmd();
}

/**
 * @brief 指定クライアントのみ、送信データをアダプタのバッファにセットする
 *
 * @param   serialNumber  送信データの通し番号
 * @param   serverSeq     送信時のサーバーシーケンス
 * @param   cmd           送信するコマンド
 * @param   sendData      送信データ本体
 * @param   dataSize      送信データ本体のサイズ
 * @param   clientId      送信対象クライアントのID
 */
void BTL_SERVER::storeSendDataInSingleAdapter( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand cmd, const void* sendData, u32 dataSize, u8 clientId )
{
  SVCL_WORK* svcl = &m_client[ clientId ];

  if( Svcl_IsEnable(svcl) ){
    svcl->adapter->ResetCmd();
    svcl->adapter->SetCmd( serialNumber, serverSeq, cmd, sendData, dataSize );
  }
  else{
    GFL_ASSERT(0);
  }
}
/**
 * @brief 送信済みデータを記録する
 * @param   serialNumber  送信データの通し番号
 * @param   serverSeq     送信時のサーバーシーケンス
 * @param   cmd           送信するコマンド
 * @param   sendData      送信データ本体
 * @param   dataSize      送信データ本体のサイズ
 */
void BTL_SERVER::registerSendData( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand cmd, const void* data, u32 dataSize )
{
  // シリアルNo.が無効値なら、登録しない
  if( serialNumber == SEND_DATA_SERIAL_NUMBER_NULL ) {
    return;
  }
  // 既に登録されているなら、登録しない
  if( m_sendDataContainer->IsDataExist( serialNumber ) ) {
    return;
  }

  SendData sendData;
  sendData.Store( serialNumber, serverSeq, cmd, data, dataSize );
  m_sendDataContainer->AddData( sendData );
  BTL_PRINT( "[BTL_SERVER] 送信データを記録しました( serialNumber=%d, serverSeq=%d, cmd=%d )\n", sendData.GetSerialNumber(), sendData.GetServerSequence(), sendData.GetAdapterCommand() );
}


//----------------------------------------------------------------------------------
/**
 * 接続している全クライアントからの返信を待つ
 *
 * @retval  bool    全クライアントからの返信が揃ったらtrue
 */
//----------------------------------------------------------------------------------
bool BTL_SERVER::waitAllAdapterReply( void )
{
  bool ret = true;

  for(u32 i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( Svcl_IsEnable(&m_client[i]) )
    {
      if( !( m_client[i].adapter->WaitCmd() ) ){
//        BTL_PRINT("waitReply ..  Client[%d]がまだ返ってこない\n", i );
        ret = false;
      }
    }
  }

  if( ret ){
//    BTL_PRINT("全クライアントから返信あり！\n");
  }

  return ret;
}
//----------------------------------------------------------------------------------
/**
 * コマンドリクエスト処理リセット
 */
//----------------------------------------------------------------------------------
void BTL_SERVER::resetAdapterCmd( void )
{
  for(u32 i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( Svcl_IsEnable(&m_client[i]) ){
      m_client[i].adapter->ResetCmd();
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * 全アダプタの初期化処理
 */
//----------------------------------------------------------------------------------
void BTL_SERVER::initAllAdapter( void )
{
  for(u32 i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( Svcl_IsEnable(&m_client[i]))
    {
      m_client[i].adapter->Init();
    }
  }
}


//=================================================================================================
//=================================================================================================

//----------------------------------------------------------------------------------
/**
 * サーバ保持用クライアントワーク：初期化
 *
 * @param   clwk
 */
//----------------------------------------------------------------------------------
void BTL_SERVER::Svcl_Clear( SVCL_WORK* clwk )
{
  clwk->myID = CLIENT_DISABLE_ID;
}
//----------------------------------------------------------------------------------
/**
 * サーバ保持用クライアントワーク：有効判定
 *
 * @param   clwk
 */
//----------------------------------------------------------------------------------
bool BTL_SERVER::Svcl_IsEnable( const SVCL_WORK* clwk ) const
{
  return clwk->myID != CLIENT_DISABLE_ID;
}
//----------------------------------------------------------------------------------
/**
 * サーバ保持用クライアントワーク：基本設定
 *
 * @param   clwk
 * @param   adapter
 */
//----------------------------------------------------------------------------------
void BTL_SERVER::Svcl_Setup( SVCL_WORK* clwk, u8 clientID, Adapter* adapter )
{
  clwk->myID = clientID;
  clwk->adapter = adapter;
}
//--------------------------------------------------------------------------
/**
 * サーバ保持用クライアントワーク：パーティ設定
 *
 * @param   client  クライアントワーク
 * @param   party   パーティデータ
 */
//--------------------------------------------------------------------------
void BTL_SERVER::Svcl_SetParty( SVCL_WORK* client, BTL_PARTY* party )
{
  client->party = party;
}
//--------------------------------------------------------------------------------------
/**
 * サーバ保持用クライアントワークへのポインタを返す（存在しないとASSERT失敗）
 *
 * @param   server
 * @param   clientID
 *
 * @retval  BTL_SERVER_CLWORK*
 */
//--------------------------------------------------------------------------------------
SVCL_WORK*  BTL_SERVER::GetClientWork( u8 clientID )
{
  if( (clientID < GFL_NELEMS(m_client))
  &&  (m_client[clientID].myID != CLIENT_DISABLE_ID)
  ){
    return &m_client[ clientID ];
  }
  return NULL;
}
//--------------------------------------------------------------------------------------
/**
 * 指定IDのクライアントが存在するか判定
 *
 * @param   clientID
 *
 * @retval  bool    存在すればtrue
 */
//--------------------------------------------------------------------------------------
bool BTL_SERVER::IsClientEnable( u8 clientID ) const
{
  if( clientID < BTL_CLIENT_MAX )
  {
    const SVCL_WORK* svcl = &(m_client[clientID]);
    return Svcl_IsEnable( svcl );
  }
  return false;
}


//----------------------------------------------
void BTL_SERVER::NotifyPokemonLevelUp( const BTL_POKEPARAM* bpp )
{
  #if BTL_OLD_IGNORE
  const pml::pokepara::PokemonParam* pp = bpp->GetSrcData();
  pp->GetNickName( m_strbuf );
  GAMEBEACON_Set_PokemonLevelUp( m_strbuf );
  #else
  GFL_UNUSED( bpp );
  #endif
}

/**
 * @brief ポケモンを捕まえたことを通知する
 * @param bpp  捕まえたポケモン
 */
void BTL_SERVER::NotifyPokemonCapture( const BTL_POKEPARAM* bpp )
{
  #if BTL_OLD_IGNORE
  const pml::pokepara::PokemonParam* pp = bpp->GetSrcData();
  GAMEBEACON_Set_PokemonGet( pp->GetMonsNo() );
  #endif

  m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_CAPTURE_POKE );
  m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_DAYCNT_CAPTURE );

/**
  2017/04/21 Fri. Yu Muto
  RECID_GLOBAL_ATTRACTION_YASHIGANI
    -> RECID_GLOBAL_ATTRACTION_BATTLEFES
  となったため削除
*/
/*
  if( bpp->GetMonsNo() == MONSNO_YASIGANI1 )
  {
    m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_GLOBAL_ATTRACTION_YASHIGANI );
  }  
*/
}

void BTL_SERVER::AddBonusMoney( u32 volume )
{
  m_mainModule->AddBonusMoney( volume );
}
void BTL_SERVER::SetMoneyDblUp( MoneyDblUpCause cause )
{
  m_mainModule->SetMoneyDblUp( cause );
}

void BTL_SERVER::InitChangePokemonReq( void )
{
  m_changePokeCnt = 0;
}
void BTL_SERVER::RequestChangePokemon( BtlPokePos pos )
{
  GFL_ASSERT(m_changePokeCnt < BTL_EXIST_POS_MAX);
  {
    for(u32 i=0; i<m_changePokeCnt; ++i){
      if( m_changePokePos[i] == pos ){
        return;
      }
    }
    m_changePokePos[ m_changePokeCnt++ ] = pos;
  }
}
bool BTL_SERVER::IsPokemonChangeRequestExist( void ) const
{
  return ( m_changePokeCnt != 0 );
}

/**
 * @brief 指定クライアントについて、入れ替えリクエスト中のポケモン数を取得する
 * @param clientID  クライアントID
 */
u8 BTL_SERVER::GetChangeRequestPokeCount( BTL_CLIENT_ID clientID ) const
{
  u8 count = 0;

  for( u32 i=0; i<m_changePokeCnt; ++i )
  {
    if( m_mainModule->BtlPosToClientID(m_changePokePos[i]) == clientID )
    {
      count++;
    }
  }

  return count;
}

bool BTL_SERVER::checkPokeChangeReqClient( u8 clientID )
{
  for( u32 i=0; i<m_changePokeCnt; ++i )
  {
    if( m_mainModule->BtlPosToClientID(m_changePokePos[i]) == clientID ){
      return true;
    }
  }
  return false;
}

ServerFlow* BTL_SERVER::GetFlowWork( void )
{
  return m_flowWork;
}

const ServerFlow* BTL_SERVER::GetFlowWorkConst( void ) const
{
  return m_flowWork;
}

GFL_NAMESPACE_END(btl)



