//=============================================================================================
/**
 * @file  btl_client.cpp
 * @brief ポケモンXY バトルシステム クライアントモジュール
 * @author  taya
 *
 * @date  2010.12.28  作成
 */
//=============================================================================================
#include <debug/include/gfl2_DebugLogWindow.h>

#include <niji_conv_header/sound/SoundMiddleID.h>
#include <Savedata/include/MyStatus.h>
#include <PokeTool/include/KawaigariParamCareCoreManager.h>

#include "./btl_common.h"
#include "./btl_mainmodule.h"
#include "./btl_calc.h"
#include "./btl_tables.h"
#include "./btl_ui.h"
#include "./btl_DebugPrint.h"
#include "./btl_Kawaigari.h"
#include "./btl_Oteire.h"
#include "./btl_BattleResultJudge.h"

#include "./btl_client.h"
#include "./btl_server.h"
#include "./btl_server_cmd.h"
#include "./btl_ClientPublicInformation.h"
#include "./btl_SendData.h"
#include "./btl_SendDataContainer.h"

#include "./btl_TrainerMessage.h"
#include "./btl_TrainerMessageManager.h"
#include "./btl_ClientSeq_TrainerMessage.h"
#include "./btl_ClientSeq_WinWild.h"
#include "./btl_ClientSeq_Capture.h"
#include "./btl_ClientSeq_BattleRoyalShowdown.h"

#include "./btl_ZenryokuWazaStatus.h"
#include "./btl_SideEffectStatus.h"
#include "./btl_PosEffectStatus.h"

#include "./tr_ai/btl_AiScript.h"
#include "./tr_ai/btl_BattleAi.h"
#include "./tr_ai/btl_BattleAiSystem.h"


#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //'const int' : ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif


GFL_NAMESPACE_BEGIN(btl)


BTL_CLIENT::BTL_CLIENT(
      MainModule*         mainModule,
      ServerFlow*         serverFlow,
      BtlSound*           soundSys,
      POKECON*            pokeCon,
      Adapter*            adapter,
      SendDataContainer*  sendDataContainer,
      SendDataContainer*  receiveDataContainer,
      ZenryokuWazaStatus* zenryokuWazaStatus,
      BtlCommMode         commMode,
      u16                 clientID,
      BtlClientType       clientType,
      BtlBagMode          bagMode,
      bool                fRecPlayMode,
      const u32           randSeed,
      HeapHandle          heapHandle
)
{
  m_myID = static_cast<u8>(clientID);

  s_ClearMegaEvoFlag( m_myID );
  s_ClearUltraBurstFlag( m_myID );

  m_heapHandle = heapHandle;
  m_heapForAiScriptExec = NULL;
  m_myType = clientType;
  m_mainModule = mainModule;
  m_pokeCon = pokeCon;
  m_procPokeIdx = 0;
  m_viewCore = NULL;
  m_soundSys = soundSys;
  m_EnemyPokeHPBase = 0;
  m_b1stReadyMsgDisped = false;
  m_tmpVariableArgs = NULL;

  m_adapter = adapter;
  m_sendDataContainer = sendDataContainer;
  m_receiveDataContainer = receiveDataContainer;
  m_zenryokuWazaStatus = zenryokuWazaStatus;
  m_myParty = m_pokeCon->GetPartyDataConst( clientID );
  m_cmdQue = reinterpret_cast<BTL_SERVER_CMD_QUE*>( GflHeapAllocMemory( m_heapHandle, sizeof(BTL_SERVER_CMD_QUE) ) );

  m_mainProc = &BTL_CLIENT::main_Normal;
  m_myState = 0;

  m_cmdCheckServer = NULL;
  m_fCmdCheckEnable = false;
  m_cmdCheckTimingCode = rec::RECTIMING_None;

  m_commWaitInfoOn = false;
  m_cmdLimitTime = 0;
  m_gameLimitTime = 0;
  m_clientLimitTime = 0;
  m_fForceQuitSelAct = false;
  m_fAITrainerBGMChanged = false;
  m_fCommError = false;
  m_fRecPlayEndTimeOver = false;
  m_fRecPlayEndBufOver = false;
  m_bRecPlayFadeStarted = false;
  m_isWaitingAdapterCommand = false;
  m_turnCount = 0;

  m_random.Initialize();
  m_AIRand.Initialize( randSeed );

  m_fldSim = pokeCon->GetFieldStatus();

  m_bagMode = bagMode;
//  BTL_ESCAPEINFO_Clear( &m_escapeInfo );

  RecPlayer_Init( &m_recPlayer );

  AIItem_Setup();

  if( (m_myType == BTL_CLIENT_TYPE_AI) && (!fRecPlayMode) )
  {
    BattleAi::SetupParam setupParam;
    setupParam.mainModule = m_mainModule;
    setupParam.serverFlow = serverFlow;
    setupParam.pokecon = m_pokeCon;
    setupParam.scriptNoBit = m_mainModule->GetClientAIBit( m_myID );
    setupParam.randSeed = randSeed;
    m_ai = GFL_NEW( m_heapHandle ) BattleAi( m_heapHandle, setupParam );
  }
  else
  {
    m_ai = NULL;
  }

  for(u32 i=0; i<GFL_NELEMS(m_AITrainerMsgCheckedFlag); ++i){
    m_AITrainerMsgCheckedFlag[ i ] = false;
  }

  if( (m_myType == BTL_CLIENT_TYPE_UI)
  &&  (m_mainModule->IsRecordEnable())
  ){
    m_recData = GFL_NEW(m_heapHandle) rec::Data();
  }else{
    m_recData = NULL;
  }

  //@バトル変更箇所
  //テレビナビ対応 先頭ポケモンの記録
  if( m_mainModule->IsPlayerSide(m_mainModule->GetClientSide(m_myID) ) )
  {
    m_mainModule->SetTvNaviData_FrontPoke(m_myParty->GetMemberDataConst(0),m_myParty->GetMemberDataConst(1));
  }

  m_trainerMessageManager   = GFL_NEW( m_heapHandle ) TrainerMessageManager( m_mainModule );
  m_seq_TrainerMessage      = GFL_NEW( m_heapHandle ) ClientSeq_TrainerMessage();
  m_seq_WinWild             = GFL_NEW( m_heapHandle ) ClientSeq_WinWild();
  m_seq_Capture             = GFL_NEW( m_heapHandle ) ClientSeq_Capture();
  m_seq_BattleRoyalShowdown = GFL_NEW( m_heapHandle ) ClientSeq_BattleRoyalShowdown();
  

  #if PM_DEBUG
  m_viewOldClient = NULL;
  m_isLiveRecSeedSetup = false;
  //@fix [fix]GFNMCat[5735]:[#5735 ライブ大会：観戦機能で全員のHPゲージと残りポケモン数が最初しか表示されない]
  m_liveRecWaitCameraSeq = 0;
  m_liveRecSizeSave = 0;
  #endif
}


BTL_CLIENT::~BTL_CLIENT()
{
  GFL_SAFE_DELETE( m_seq_BattleRoyalShowdown );
  GFL_SAFE_DELETE( m_seq_Capture );
  GFL_SAFE_DELETE( m_seq_WinWild );
  GFL_SAFE_DELETE( m_seq_TrainerMessage );
  GFL_SAFE_DELETE( m_trainerMessageManager );

  if( m_recData ){
    GFL_DELETE  m_recData;
    m_recData = NULL;
  }

  GFL_SAFE_DELETE( m_ai );

  if( m_cmdQue ){
    GflHeapFreeMemory( m_cmdQue );
  }
}


/**
 *  メインループ関数を差し替える
 */
void BTL_CLIENT::changeMainProc( ClientMainProc proc )
{
  m_mainProc = proc;
  m_myState = 0;
}

/**
 * @brief 自身が担当する立ち位置の数を取得する
 */
u8 BTL_CLIENT::getMyCoverPosNum( void ) const
{
  return m_mainModule->GetClientCoverPosNum( m_myID );
}



//---------------------------------------------------------------------------
/**
 * @brief ＡＩを、乱入ＡＩに変更する
 */
//---------------------------------------------------------------------------
void BTL_CLIENT::SetIntrudeAI( void )
{
  if( m_ai == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  m_ai->ChangeScript( BTL_AISCRIPT_BIT_WAZA_INTRUDE );
  BTL_PRINT( "[BTL_CLIENT_%d] 乱入ＡＩに変更しました\n", m_myID );
}

//---------------------------------------------------------------------------
/**
 * @brief ＡＩを、援軍ＡＩに変更する
 */
//---------------------------------------------------------------------------
void BTL_CLIENT::SetReinforceAI( void )
{
  if( m_ai == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  u32 ai_bit = BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_STRONG | BTL_AISCRIPT_BIT_WAZA_EXPERT | BTL_AISCRIPT_BIT_WAZA_DOUBLE;
  m_ai->ChangeScript( ai_bit );
  BTL_PRINT( "[BTL_CLIENT_%d] 援軍ＡＩに変更しました\n", m_myID );
}

//-----------------------------------------------------------------------------
/**
 * @brief オーラ技の使用状況を取得する
 */
//-----------------------------------------------------------------------------
const ZenryokuWazaStatus* BTL_CLIENT::GetZenryokuWazaStatus( void ) const
{
  return m_zenryokuWazaStatus;
}

//-----------------------------------------------------------------------------
/**
 * @brief バトルロイヤルの現在順位を取得する
 */
//-----------------------------------------------------------------------------
const RoyalRankingContainer& BTL_CLIENT::GetRoyalRankingContainer( void ) const
{
  return m_royalRankingContainer;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンドの受信を待っている状態か？
 * @retval true   アダプターコマンドの受信待ち
 * @retval false  何らかの処理を行っている
 */
//-----------------------------------------------------------------------------
bool BTL_CLIENT::IsWaitingAdapterCommand( void ) const
{
  return m_isWaitingAdapterCommand;
}




/**
 * @brief メインループ：通常時
 * @retval true   メインループが終了した場合
 * @retval false  メインループが終了していない場合
 */
bool BTL_CLIENT::main_Normal( void )
{
  enum {
    SEQ_READ_ACMD = 0,
    SEQ_EXEC_CMD,
    SEQ_RETURN_TO_SV,
    SEQ_RETURN_TO_SV_QUIT,
    SEQ_RECPLAY_CTRL,
    SEQ_RECPLAY_STOP,
    SEQ_BGM_FADEOUT,
    SEQ_WAIT_RECPLAY_FADEOUT,
    SEQ_COMM_ERROR,
    SEQ_WAIT_CLEANUP_FOR_COMM_ERROR,
    SEQ_QUIT,
  };

  RecPlayerCtrl_Main( &m_recPlayer );


  switch( m_myState ){
  case SEQ_READ_ACMD:
    {
      AdapterCommand       receivedCommandId    = ACMD_NONE;
      SendDataSerialNumber receivedSerialNumber = SEND_DATA_SERIAL_NUMBER_NULL;
      ServerSequence       receivedServerSeq    = SERVER_SEQ_NULL;
      m_adapter->RecvCmd( &receivedCommandId, &receivedSerialNumber, &receivedServerSeq );

      m_isWaitingAdapterCommand = ( receivedCommandId == ACMD_NONE );

      // 受信したコマンドを保存
      if( receivedCommandId != ACMD_NONE ) 
      {
        const void* data;
        const u32 dataSize = m_adapter->GetRecvData( (const void**)&data );
        registerReceivedData( receivedSerialNumber, receivedServerSeq, receivedCommandId, data, dataSize );
      }

      // 録画再生時のタイムオーバー検出
      if( ( m_myType == BTL_CLIENT_TYPE_RECPLAY ) && 
          ( m_fRecPlayEndTimeOver || m_fRecPlayEndBufOver ) && 
          ( receivedCommandId == ACMD_NONE ) )
      {
        return false;
      }

      // 受信したコマンドに対して既に返信済みなら、過去に返信したデータを、そのまま返信する
      if( receivedCommandId != ACMD_NONE )
      {
        const SendData* sendData = m_sendDataContainer->GetData( receivedSerialNumber );
        if( sendData != NULL )
        {
          m_returnDataCommandID    = sendData->GetAdapterCommand();
          m_returnDataSerialNumber = sendData->GetSerialNumber();
          m_returnDataServerSeq    = sendData->GetServerSequence();
          m_returnDataPtr          = sendData->GetData();
          m_returnDataSize         = sendData->GetDataSize();
          BTL_PRINT( "[BTL_CLIENT_%d] 既に返信済みです。過去に送信したデータを再送信します( serialNumber=%d, serverSeq=%d, cmd=%d )\n", m_myID, m_returnDataSerialNumber, m_returnDataServerSeq, m_returnDataCommandID );

          m_myState = SEQ_RETURN_TO_SV;
          return false;
        }
      }

      // 返信データを初期化
      setDummyReturnData();
      m_returnDataCommandID    = receivedCommandId;
      m_returnDataSerialNumber = receivedSerialNumber;
      m_returnDataServerSeq    = receivedServerSeq;

      // バトル終了コマンドを処理
      if( m_returnDataCommandID == ACMD_QUIT_BTL )
      {
        const void* p;
        u32 dataSize = m_adapter->GetRecvData( (const void**)&p );
        m_escapeInfo.Deserialize( p, dataSize );
        BTL_N_Printf( DBGSTR_CLIENT_RecvedQuitCmd, m_myID );
        m_subSeq = 0;
        m_myState = SEQ_RETURN_TO_SV_QUIT;
        break;
      }

      // それ以外のコマンドを処理
      if( m_returnDataCommandID != ACMD_NONE )
      {
        BTL_PRINT("[BTL_CLIENT_%d]▼サーバ命令 %d を処理します( serialNo=%d )\n", m_myID, m_returnDataCommandID, m_returnDataSerialNumber );

        bool fRecCtrlLock;
        bool fSubProcDecide = setSubProc( m_returnDataCommandID, &fRecCtrlLock );

        if( fRecCtrlLock )
        {
          if( !RecPlayerCtrl_Lock(&m_recPlayer) ){  // 既にフェード中でロックできない->フェード待ちへ
            m_myState = SEQ_WAIT_RECPLAY_FADEOUT;
            break;
          }
        }
        else
        {
          RecPlayerCtrl_Unlock( &m_recPlayer );
        }

        if( fSubProcDecide )
        {
          BTL_PRINT( "[BTL_CLIENT_%d]サーバーコマンドの処理を開始します\n", m_myID );
          m_myState = SEQ_EXEC_CMD;
        }
        else
        {
          BTL_PRINT( "[BTL_CLIENT_%d]サーバーコマンドに対する処理が見当たりませんでした\n", m_myID );
          m_myState = SEQ_RETURN_TO_SV;
        }
      }
      else
      {
        if( m_fCommError ){
          m_myState = SEQ_COMM_ERROR;
          break;
        }
      }
    }
    break;


  case SEQ_EXEC_CMD:
    if( callSubProc() )
    {
      if( m_fCommError ){
        m_myState = SEQ_COMM_ERROR;
        break;
      }

      // １コマンド終了毎に、録画プレイヤーがブラックアウトさせているかチェック
      if( RecPlayer_CheckBlackOut(&m_recPlayer) )
      {
        m_myState = SEQ_RECPLAY_CTRL;
      }
      else
      {
        BTL_N_Printf( DBGSTR_CLIENT_RETURN_CMD_START, m_myID );
        m_myState = SEQ_RETURN_TO_SV;
      }
    }
    break;

  case SEQ_RETURN_TO_SV:
    if( returnToServer() ){
      m_myState = SEQ_READ_ACMD;
      BTL_PRINT("▲Client_%d  サーバへ返信しました( cmd=%d, serialNo=%d, datSize=%d )\n", m_myID, m_returnDataCommandID, m_returnDataSerialNumber, m_returnDataSize);
    }
    if( m_fCommError ){
      m_myState = SEQ_COMM_ERROR;
      break;
    }
    break;

  case SEQ_RETURN_TO_SV_QUIT:
    if( returnToServer() ){
      m_myState = SEQ_QUIT;
      BTL_N_Printf( DBGSTR_CLIENT_ReplyToQuitCmd, m_myID );
    }
    if( m_fCommError ){
      m_myState = SEQ_COMM_ERROR;
      break;
    }
    break;

  // 録画再生コントロール：ブラックアウト待ち
  case SEQ_WAIT_RECPLAY_FADEOUT:
    if( RecPlayer_CheckBlackOut(&m_recPlayer) ){
      m_myState = SEQ_RECPLAY_CTRL;
    }
    break;

  // 録画再生コントロール：ブラックアウト後
  case SEQ_RECPLAY_CTRL:
    if( RecPlayer_GetCtrlCode(&m_recPlayer) == RECCTRL_QUIT )
    {
      m_myState = SEQ_RECPLAY_STOP;
    }
    else
    {
      m_myState = SEQ_BGM_FADEOUT;
    }
    break;

  case SEQ_BGM_FADEOUT:
    if( !(m_soundSys->CheckFadeOnBGM()) )
    {
      // nextTurn を引数にして全クライアントの BTL_CLIENT_SetChapterSkip がコールバックされる
      BTL_PRINT("[CLI_%d] チャプタスキップリクエスト通知\n", m_myID);
      u32 nextTurn = RecPlayer_GetNextTurn( &m_recPlayer );
      m_mainModule->ResetForRecPlay( nextTurn );
    }
    break;

  case SEQ_COMM_ERROR:
  case SEQ_RECPLAY_STOP:
    if( m_viewCore ){
      m_viewCore->CMD_StartCleanUp();
      m_myState = SEQ_WAIT_CLEANUP_FOR_COMM_ERROR;
    }else{
      m_myState = SEQ_QUIT;
    }
    break;

  case SEQ_WAIT_CLEANUP_FOR_COMM_ERROR:
    if( m_viewCore )
    {
      if( m_viewCore->CMD_WaitCleanUp() ){
        m_myState = SEQ_QUIT;
      }
    }
    else{
      m_myState = SEQ_QUIT;
    }
    break;


  case SEQ_QUIT:
    return true;
  }

  return false;
}
/**
 *  メインループ：チャプタスキップ時
 */
bool BTL_CLIENT::main_ChapterSkip( void )
{
  enum {
    SEQ_RECPLAY_START = 0,
    SEQ_RECPLAY_READ_ACMD,
    SEQ_RECPLAY_EXEC_CMD,
    SEQ_RECPLAY_RETURN_TO_SV,
    SEQ_RECPLAY_FADEIN,

    SEQ_RECPLAY_QUIT,
  };

  RecPlayerCtrl_Main( &m_recPlayer );

  switch( m_myState ){
  case SEQ_RECPLAY_START:
    m_myState = SEQ_RECPLAY_READ_ACMD;
    BTL_PRINT("[CLI_%d] ***** Start Chapter Skip Mode *****\n", m_myID);

    /* fallthru */

  case SEQ_RECPLAY_READ_ACMD:
    // 時間制限オーバーなどで稼働を停止していたら何もしない
    if( !RecPlayer_IsActive(&m_recPlayer) )
    {
      BTL_PRINT("[CLI_%d] RecPlayer not running\n", m_myID);
      setDummyReturnData();
      m_myState = SEQ_RECPLAY_RETURN_TO_SV;
      break;
    }
    else
    {
      bool fSkipEnd = RecPlayer_CheckChapterSkipEnd( &m_recPlayer );
      if( !fSkipEnd )
      {
        m_adapter->RecvCmd( &m_returnDataCommandID, &m_returnDataSerialNumber, &m_returnDataServerSeq );
        BTL_PRINT("[CLI_%d] ChapterSkipMode: m_returnDataCommandID=%d\n", m_myID, m_returnDataCommandID);
        if( m_returnDataCommandID != ACMD_NONE )
        {
          bool fSubProcDecide = setSubProc( m_returnDataCommandID, NULL );
          if( fSubProcDecide ){
            m_myState = SEQ_RECPLAY_EXEC_CMD;
          }
          else
          {
            setDummyReturnData();
            m_myState = SEQ_RECPLAY_RETURN_TO_SV;
          }
        }
      }
      else
      {
        if( m_viewCore )
        {
          m_viewCore->CMD_RecPlayFadeIn_Start();
          m_soundSys->FadeInBGM( 30 );
//          PMV_ResetMasterVolume();//PokeVoice再生再開
        }
        m_myState = SEQ_RECPLAY_FADEIN;
      }
    }
    break;

  case SEQ_RECPLAY_EXEC_CMD:
    if( callSubProc() )
    {
      m_myState = SEQ_RECPLAY_RETURN_TO_SV;
      m_subSeq = 0;
    }
    break;

  case SEQ_RECPLAY_RETURN_TO_SV:
    if( returnToServer() ){
      m_myState = SEQ_RECPLAY_READ_ACMD;
      BTL_N_PrintfEx( true, DBGSTR_CLIENT_RETURN_CMD_DONE, m_myID );
    }
    break;

  case SEQ_RECPLAY_FADEIN:
    if( m_viewCore != NULL )
    {
      if( m_viewCore->CMD_RecPlayFadeIn_Wait()
      &&  !(m_soundSys->CheckFadeOnBGM())
      ){
        u16 currentChapter = RecPlayer_GetNextTurn( &m_recPlayer );
        m_viewCore->CMD_UpdateRecPlayerInput( currentChapter, currentChapter );
        m_mainModule->NotifyChapterSkipEnd();
      }
    }
    break;

  case SEQ_RECPLAY_QUIT:
    return true;

  }

  return false;
}

 /**
  * @brief サーバーから受信したデータを記憶する
  * @param serialNumber 受信データの通し番号
  * @param serverSeq    受信データのサーバーシーケンス番号
  * @param commandId    受信したコマンド
  * @param data         受信データ本体
  * @param dataSize     受信データ本体のサイズ
  */
 void BTL_CLIENT::registerReceivedData( 
  SendDataSerialNumber serialNumber, 
  ServerSequence       serverSeq,
  AdapterCommand       commandId, 
  const void*          data, 
  u32                  dataSize )
{
  // シリアルNo.が無効値なら、記録しない
  if( serialNumber == SEND_DATA_SERIAL_NUMBER_NULL ) {
    return;
  }
  // 既に登録済みなら、記録しない
  if( m_receiveDataContainer->IsDataExist( serialNumber ) ) {
    return;
  }

  SendData receivedData;
  receivedData.Store( serialNumber, serverSeq, commandId, data, dataSize );
  m_receiveDataContainer->AddData( receivedData );

  BTL_PRINT( "[BTL_CLIENT_%d] アダプターコマンドを受信しました( serialNumber=%d, serverSeq=%d, cmd=%d )\n", m_myID, serialNumber, serverSeq, commandId );
}

/**
 * @brief サーバーにデータを返信する
 * @retval true   返信に成功した
 * @retval false  返信に失敗した
 */
bool BTL_CLIENT::returnToServer( void ) const
{
  // 返信データをアダプタにセット
  SendData returnData;
  returnData.Store( m_returnDataSerialNumber, m_returnDataServerSeq, m_returnDataCommandID, m_returnDataPtr, m_returnDataSize );
  bool succeeded = m_adapter->ReturnCmd( returnData );

  // 返信したデータを記憶
  if( ( succeeded ) &&
      ( m_returnDataSerialNumber != SEND_DATA_SERIAL_NUMBER_NULL ) &&
     !( m_sendDataContainer->IsDataExist( m_returnDataSerialNumber ) ) ) 
  {
    m_sendDataContainer->AddData( returnData );
    BTL_PRINT( "[BTL_CLIENT_%d] サーバーに返信したデータを記憶しました( serialNumber=%d, cmd=%d )\n", m_myID, returnData.GetSerialNumber(), returnData.GetAdapterCommand() );
  }

  return succeeded;
}











//------------------------------------------------------------------------------------------------------
// 録画データプレイヤー
//------------------------------------------------------------------------------------------------------

/**
 *  領域初期化（再生時でなくても、これを呼び出して初期化する必要がある）
 */
void BTL_CLIENT::RecPlayer_Init( RECPLAYER_CONTROL* ctrl )
{
  ctrl->ctrlCode = static_cast<u8>( RECCTRL_NONE );
  ctrl->seq = 0;
  ctrl->fTurnIncrement = false;
  ctrl->fFadeOutStart = false;
  ctrl->fFadeOutDone = false;
  ctrl->fChapterSkip = false;
  ctrl->fLock = false;
  ctrl->fQuit = false;
  ctrl->turnCount = 0;
  ctrl->nextTurnCount = 0;
  ctrl->maxTurnCount = 0;
  ctrl->skipTurnCount = 0;
  ctrl->handlingTimer = 0;
}
/**
 *  再生準備処理（再生時にのみ呼び出す）
 */
void BTL_CLIENT::RecPlayer_Setup( RECPLAYER_CONTROL* ctrl, u32 turnCnt )
{
  ctrl->maxTurnCount = turnCnt;
}
/**
 *  停止またはチャプター送り前のブラックアウトが発生したか判定
 */
bool BTL_CLIENT::RecPlayer_CheckBlackOut( const RECPLAYER_CONTROL* ctrl )
{
  return ctrl->fFadeOutDone;
}
/**
 *  ターンカウンタインクリメントリクエスト
 */
void BTL_CLIENT::RecPlayer_TurnIncReq( RECPLAYER_CONTROL* ctrl )
{
  ctrl->fTurnIncrement = true;
}
/**
 *  状態取得
 */
BTL_CLIENT::RecCtrlCode BTL_CLIENT::RecPlayer_GetCtrlCode( const RECPLAYER_CONTROL* ctrl )
{
  return static_cast<RecCtrlCode>( ctrl->ctrlCode );
}
/**
 *  チャプタースキップモードへ切り替え
 */
void BTL_CLIENT::RecPlayer_ChapterSkipOn( RECPLAYER_CONTROL* ctrl, u32 nextTurnNum )
{
  ctrl->fChapterSkip  = true;
  ctrl->skipTurnCount = 0;
  ctrl->nextTurnCount = nextTurnNum;
  ctrl->fFadeOutStart = false;
  ctrl->fFadeOutDone  = false;
}
/**
 *
 */
void BTL_CLIENT::RecPlayer_ChapterSkipOff( RECPLAYER_CONTROL* ctrl )
{
  ctrl->seq = 0;
  ctrl->fChapterSkip = false;
}
/**
 *  スキップ処理が終了したか判定
 */
bool BTL_CLIENT::RecPlayer_CheckChapterSkipEnd( const RECPLAYER_CONTROL* ctrl )
{
  return ( ctrl->skipTurnCount == ctrl->nextTurnCount );
}
/**
 *
 */
u32 BTL_CLIENT::RecPlayer_GetNextTurn( const RECPLAYER_CONTROL* ctrl )
{
  return ctrl->nextTurnCount;
}
/**
 *  キー・タッチパネル操作ロック
 *
 *  @retval bool  既にフェード中でロック不可の場合、falseを返す
 */
bool BTL_CLIENT::RecPlayerCtrl_Lock( RECPLAYER_CONTROL* ctrl )
{
  if( ctrl->fFadeOutStart ){
    return false;
  }
  ctrl->fLock = true;
  return true;
}
/**
 *  キー・タッチパネル操作アンロック
 */
void BTL_CLIENT::RecPlayerCtrl_Unlock( RECPLAYER_CONTROL* ctrl )
{
  ctrl->fLock = false;
}
/**
 *  キー・タッチパネル操作終了（以降、復帰できない）
 */
void BTL_CLIENT::RecPlayer_Quit( RECPLAYER_CONTROL* ctrl )
{
  ctrl->fQuit = true;
}
/**
 * キー・タッチ派寝す操作  稼働中か判定
 */
bool  BTL_CLIENT::RecPlayer_IsActive( const RECPLAYER_CONTROL* ctrl ) const
{
  return !(ctrl->fQuit);
}

/**
 *  メインコントロール（キー・タッチパネルに反応、状態遷移する）
 */
void BTL_CLIENT::RecPlayerCtrl_Main( RECPLAYER_CONTROL* ctrl )
{
  GFL_UNUSED( ctrl );

  enum {
    CHAPTER_CTRL_FRAMES = 45,
  };

  if( (m_myType == BTL_CLIENT_TYPE_RECPLAY)
  &&  (ctrl->fLock == false)
  &&  (ctrl->fQuit == false)
  &&  (ctrl->maxTurnCount)
  ){
    enum {
      SEQ_DEFAULT = 0,
      SEQ_FADEOUT,
      SEQ_STAY,
    };

    bool fTurnUpdate = false;

    if( ctrl->fTurnIncrement )
    {
      ctrl->fTurnIncrement = false;

      if( ctrl->fChapterSkip == false )
      {
        if( (ctrl->turnCount < ctrl->maxTurnCount)
        &&  (ctrl->seq == SEQ_DEFAULT)
        ){
          ++(ctrl->turnCount);
          if(ctrl->handlingTimer == 0)
          {
            ctrl->nextTurnCount = ctrl->turnCount;
          }
          fTurnUpdate = true;
        }
      }
      else
      {
        if( ctrl->skipTurnCount < ctrl->nextTurnCount )
        {
          ctrl->skipTurnCount++;
          if( ctrl->skipTurnCount == ctrl->nextTurnCount )
          {
            ctrl->turnCount = ctrl->nextTurnCount;
            if( m_viewCore ){
              m_viewCore->CMD_UpdateRecPlayerInput( ctrl->turnCount, ctrl->nextTurnCount );
            }
          }
        }
        return;
      }
    }

    // 最初のチャプタまで何もしない（入場エフェクト待ち）
    if( ctrl->turnCount == 0 ){
      return;
    }

    // ここから先、描画クライアント以外は何もしない
    if( m_viewCore == NULL) {
      return;
    }

    switch( ctrl->seq ){
    case SEQ_DEFAULT:
      {
        int result = m_viewCore->CMD_CheckRecPlayerInput( );
        bool fCtrlUpdate = false;

        switch( result ){
        case  BTLV_INPUT_BR_SEL_STOP:     //停止
          ctrl->ctrlCode = RECCTRL_QUIT;
          m_mainModule->StartFadeoutForRecPlay();
          m_viewCore->CMD_RecPlayFadeOut_Start();
          ctrl->fFadeOutStart = true;
          ctrl->seq = SEQ_FADEOUT;
          ctrl->handlingTimer = 0;
          m_viewCore->CMD_RecPlayer_StartQuit( ctrl->turnCount, BTLV_INPUT_BR_STOP_KEY );

          //BGM、SE停止
          //BTS:2360
          //BTS:6700
          //このあとは、バトルをぬけるだけなのでBGMフェードアウト、SE停止。
          //SEは、btl_main QUITの流れで元に戻す。
          {
            m_soundSys->FadeOutBGM( BTL_BGM_FADEOUT_FRAMES );
            m_soundSys->StopAllSE();
//            PMV_SetMasterVolume( 0 );//この後再生されるポケモンVOICEをならさない
            m_mainModule->BGMFadeOutDisable( );
          }
          break;

        case BTLV_INPUT_BR_SEL_REW:
          if( ctrl->nextTurnCount > 1 )
          {
            ctrl->nextTurnCount--;
            fCtrlUpdate = true;
          }
          ctrl->handlingTimer = CHAPTER_CTRL_FRAMES;
          break;

        case BTLV_INPUT_BR_SEL_FF:
          if( ctrl->nextTurnCount < ctrl->maxTurnCount ){
            ctrl->nextTurnCount++;
            fCtrlUpdate = true;
          }
          ctrl->handlingTimer = CHAPTER_CTRL_FRAMES;
          break;
        }

        if( fCtrlUpdate || fTurnUpdate ){
          m_viewCore->CMD_UpdateRecPlayerInput( ctrl->turnCount, ctrl->nextTurnCount );
        }

        if( ctrl->handlingTimer )
        {
          if( --(ctrl->handlingTimer) == 0 )
          {
            ctrl->ctrlCode = RECCTRL_CHAPTER;
            m_mainModule->StartFadeoutForRecPlay();
            m_viewCore->CMD_RecPlayFadeOut_Start();
            m_viewCore->CMD_RecPlayer_StartSkipDisplay( ctrl->nextTurnCount ); //「スキップしています」と表示
            ctrl->fFadeOutStart = true;
            ctrl->seq = SEQ_FADEOUT;

            //BGM、SE停止
            //BTS:2360
            //BTS:6700
            //チャンプタースキップ時のBGMFadeOut　SEStop
            //SEは、ClientMain_Normal  SEQ_RECPLAY_CTRL の流れで元に戻す。
            {
              m_soundSys->FadeOutBGM( 30 );
              m_soundSys->StopAllSE();
//              PMV_SetMasterVolume( 0 );//この後再生されるポケモンVOICEをならさない
              //BTLV_EFFECT_StopAllPMVoice();//今再生中のポケモンVOICEを停止
            }
          }
          break;
        }
      }
      break;

    case SEQ_FADEOUT:
      if( m_viewCore->CMD_RecPlayFadeOut_Wait() ){
        ctrl->fFadeOutDone = true;
        ctrl->seq = SEQ_STAY;
      }
      break;

    case SEQ_STAY:
      break;
    }
  }
}


//------------------------------------------------------------------------------------------------------
// AIアイテム
//------------------------------------------------------------------------------------------------------

/**
 *  ワーク初期化
 */
void BTL_CLIENT::AIItem_Setup( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_AIItem); ++i)
  {
    m_AIItem[i] = m_mainModule->GetClientUseItem( m_myID, i );
  }
}

/**
 *  使用アイテムチェック
 */
u16 BTL_CLIENT::AIItem_CheckUse( const BTL_POKEPARAM* bpp, const BTL_PARTY* party )
{
  static const u8 borderMembers[] = {
    BTL_PARTY_MEMBER_MAX, 4, 2, 1,
  };
  u32 aliveMembers = party->GetAliveMemberCount();

  if( bpp->CheckSick(pml::wazadata::WAZASICK_SASIOSAE) ){
    return ITEM_DUMMY_DATA;
  }

  // アイテム使用Priorityを記録しておく配列に
  s8  priority[ GFL_NELEMS(m_AIItem) ];
  // -1 = 不使用をセットしておく
  for(u32 j=0; j<GFL_NELEMS(priority); ++j){
    priority[j] = -1;
  }
  s8 priorityMax = -1;

  for(u32 i=0; i<GFL_NELEMS(m_AIItem); ++i)
  {
    if( borderMembers[i] < aliveMembers ){
      break;
    }
    if( m_AIItem[i] != ITEM_DUMMY_DATA )
    {
      // HP回復系
      if( btl::calc::ITEM_GetParam(m_AIItem[i], item::ITEM_DATA::PRM_ID_HP_RCV) )
      {
        u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
        if( hp <= btl::calc::QuotMaxHP_Zero(bpp, 4) ){
          priority[i] = tables::GetAIItemPriority( m_AIItem[i] );
        }
      }
      // 能力ランクアップ系
      else if( check_status_up_item(m_AIItem[i], bpp) )
      {
        priority[i] = tables::GetAIItemPriority( m_AIItem[i] );
      }
      // 状態異常回復系
      else if( check_cure_sick_item(m_AIItem[i], bpp) )
      {
        priority[i] = tables::GetAIItemPriority( m_AIItem[i] );
      }

      if( priority[i] > priorityMax ){
        priorityMax = priority[i];
      }
    }
  }

  if( priorityMax >= 0 )
  {
    for(u32 i=0; i<GFL_NELEMS(priority); ++i)
    {
      if( priorityMax == priority[i] )
      {
        u16 useItem = m_AIItem[i];
        m_AIItem[i] = ITEM_DUMMY_DATA;
        return useItem;
      }
    }
  }

  return ITEM_DUMMY_DATA;
}
/**
 *  能力ランクアップ系アイテムである＆使って効果がある判定
 */
bool BTL_CLIENT::check_status_up_item( u16 itemID, const BTL_POKEPARAM* bpp )
{
  static const struct {
    u8                      itemParamID;
    BTL_POKEPARAM::ValueID  rankID;
  }check_tbl[] = {
    { item::ITEM_DATA::PRM_ID_ATTACK_UP,     BTL_POKEPARAM::BPP_ATTACK_RANK     },   // 攻撃力アップ
    { item::ITEM_DATA::PRM_ID_DEFENCE_UP,    BTL_POKEPARAM::BPP_DEFENCE_RANK    },   // 防御力アップ
    { item::ITEM_DATA::PRM_ID_SP_ATTACK_UP,  BTL_POKEPARAM::BPP_SP_ATTACK_RANK  },   // 特攻アップ
    { item::ITEM_DATA::PRM_ID_SP_DEFENCE_UP, BTL_POKEPARAM::BPP_SP_DEFENCE_RANK },   // 特防アップ
    { item::ITEM_DATA::PRM_ID_AGILITY_UP,    BTL_POKEPARAM::BPP_AGILITY_RANK    },   // 素早さアップ
    { item::ITEM_DATA::PRM_ID_HIT_UP,        BTL_POKEPARAM::BPP_HIT_RATIO       },   // 命中率アップ
  };

  for(u32 i=0; i<GFL_NELEMS(check_tbl); ++i)
  {
    if( btl::calc::ITEM_GetParam(itemID, check_tbl[i].itemParamID) )
    {
      if( bpp->IsRankEffectValid(check_tbl[i].rankID, 1) ){
        return true;
      }
    }
  }

 // クリティカルだけ別判定
  if( btl::calc::ITEM_GetParam(itemID, item::ITEM_DATA::PRM_ID_CRITICAL_UP) )
  {
    if( bpp->GetCriticalRank() == 0 ){
      return true;
    }
  }
  
  //@バトル変更箇所
  //SBTS[1250]
  //エフェクトガードが効いていない。
  //ただ、現状効いているのかがハンドラの存在のみでチェックしずらいので、持っていたら強制true
  //2個持っている場合はないという事なので持っていればtrueにしてしまう。 西野さん確認済み
  if( btl::calc::ITEM_GetParam(itemID, item::ITEM_DATA::PRM_ID_ABILITY_GUARD) )
  {
    return true;
  }
  
  return false;
}

/**
 *  状態異常回復系アイテムである＆使って効果がある判定
 */
bool BTL_CLIENT::check_cure_sick_item( u16 itemID, const BTL_POKEPARAM* bpp )
{
  static const struct {
    u8 itemParamID;
    u8 sickID;
  }check_tbl[] = {
    { item::ITEM_DATA::PRM_ID_SLEEP_RCV,     pml::wazadata::WAZASICK_NEMURI      },   // 眠り回復
    { item::ITEM_DATA::PRM_ID_POISON_RCV,    pml::wazadata::WAZASICK_DOKU        },   // 毒回復
    { item::ITEM_DATA::PRM_ID_BURN_RCV,      pml::wazadata::WAZASICK_YAKEDO      },   // 火傷回復
    { item::ITEM_DATA::PRM_ID_ICE_RCV,       pml::wazadata::WAZASICK_KOORI       },   // 氷回復
    { item::ITEM_DATA::PRM_ID_PARALYZE_RCV,  pml::wazadata::WAZASICK_MAHI        },   // 麻痺回復
    { item::ITEM_DATA::PRM_ID_PANIC_RCV,     pml::wazadata::WAZASICK_KONRAN      },   // 混乱回復
    { item::ITEM_DATA::PRM_ID_MEROMERO_RCV,  pml::wazadata::WAZASICK_MEROMERO    },   // メロメロ回復
  };

  for(u32 i=0; i<GFL_NELEMS(check_tbl); ++i)
  {
    if( btl::calc::ITEM_GetParam(itemID, check_tbl[i].itemParamID) )
    {
      if( bpp->CheckSick(static_cast<WazaSick>(check_tbl[i].sickID)) ){
        return true;
      }
    }
  }

  return false;
}

/**
 *  録画再生モードに切り替え
 */
void BTL_CLIENT::SetRecordPlayerMode( rec::Reader* recReader )
{
  m_myType = BTL_CLIENT_TYPE_RECPLAY;
  m_btlRecReader = recReader;

  {
    u32 turnCnt = m_btlRecReader->GetTurnCount();
    RecPlayer_Setup( &m_recPlayer, turnCnt );
  }
}

void BTL_CLIENT::NotifyCommError( void )
{
  m_fCommError = true;
}

/**
 *  録画記録データを取得
 */
const void* BTL_CLIENT::GetRecordData( u32* size ) const
{
  if( m_recData ){
    return m_recData->GetDataPtr( size );
  }
  return NULL;
}



//=============================================================================================
/**
 * 描画処理モジュールをアタッチ（UIクライアントのみ）
 *
 * @param   viewCore
 *
 */
//=============================================================================================
void BTL_CLIENT::AttachViewCore( BattleViewSystem* viewCore )
{
  m_viewCore = viewCore;
}

//=============================================================================================
/**
 * @brief AIを実行するために使用するヒープをアタッチする
 * @param heap  AIを実行するために使用するヒープ
 */
//=============================================================================================
void BTL_CLIENT::AttachAiExecHeap( HeapHandle heap )
{
  m_heapForAiScriptExec = heap;
}

//=============================================================================================
/**
 * コマンド整合性チェク用のサーバモジュールをアタッチ
 *
 * @param   server
 */
//=============================================================================================
void BTL_CLIENT::AttachCmdCheckServer( BTL_SERVER* server )
{
  m_cmdCheckServer = server;
  m_fCmdCheckEnable = true;
}

//=============================================================================================
/**
 * コマンド整合性チェク用のサーバモジュールをデタッチ
 */
//=============================================================================================
void BTL_CLIENT::DetachCmdCheckServer( void )
{
  m_cmdCheckServer = NULL;
  m_fCmdCheckEnable = false;
}

//=============================================================================================
/**
 * 生成したアダプタを返す
 *
 * @param   wk
 *
 * @retval  Adapter*
 */
//=============================================================================================
Adapter* BTL_CLIENT::GetAdapter( void )
{
  return m_adapter;
}


//=============================================================================================
/**
 * クライアントメインループ
 *
 * @param   wk
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_CLIENT::Main( void )
{
  return (this->*m_mainProc)();
}

//=============================================================================================
/**
 * チャプタスキップ／停止のためのフェードアウト開始通知を描画クライアントからMainModule経由で受ける
 * （全クライアント対象）
 *
 */
//=============================================================================================
void BTL_CLIENT::NotifyFadeoutStartForRecPlay( void )
{
  m_bRecPlayFadeStarted = true;
}

//=============================================================================================
/**
 * チャプタスキップの開始通知を描画クライアントからMainModule経由で受ける（全クライアント）
 *
 * @param   fChapterSkipMode
 */
//=============================================================================================
void BTL_CLIENT::SetChapterSkip( u32 nextTurnNum )
{
  m_zenryokuWazaStatus->Clear();

  m_isWaitingAdapterCommand = false;
  m_bRecPlayFadeStarted     = false;

  m_btlRecReader->Reset();

  BTL_PRINT("[CLI] === SetChapterSkip / reset adapter buffer ===\n");

  m_adapter->ResetRecvBuffer();
  RecPlayer_ChapterSkipOn( &m_recPlayer, nextTurnNum );
  changeMainProc( &BTL_CLIENT::main_ChapterSkip );
}

void BTL_CLIENT::StopChapterSkip( void )
{
  RecPlayer_ChapterSkipOff( &m_recPlayer );
  changeMainProc( &BTL_CLIENT::main_Normal );
}


//=============================================================================================
/**
 * クライアントが録画再生モードで動いているか判定
 *
 * @param   wk
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_CLIENT::IsRecPlayerMode( void ) const
{
  return m_myType == BTL_CLIENT_TYPE_RECPLAY;
}
//=============================================================================================
/**
 *
 *
 * @param   wk
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CLIENT::GetRecPlayerMaxChapter( void ) const
{
  return m_recPlayer.maxTurnCount;
}

//=============================================================================================
/**
 * チャプタースキップモードで動作しているか判定
 *
 * @param   wk
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_CLIENT::IsChapterSkipMode( void ) const
{
  return (m_mainProc == &BTL_CLIENT::main_ChapterSkip);
}

/**
 *  ダミー返信データ（サーバに返信する必要があるが内容は問われないケース）を作成
 */
void BTL_CLIENT::setDummyReturnData( void )
{
  m_dummyReturnData = 0;
  m_returnDataPtr = &(m_dummyReturnData);
  m_returnDataSize = sizeof(m_dummyReturnData);
}
/**
 * @brief サブプロセス関数設定
 * @param      cmd           アダプターコマンド
 * @param[out] fRecCtrlLock  録画再生コントロールをロックする必要があるか？
 * @retval true   指定したアダプターコマンドに対応するサブプロセスが存在する場合
 * @retval false  指定したアダプターコマンドに対応するサブプロセスが存在しない場合( 想定外のアサート )
 */
bool  BTL_CLIENT::setSubProc( AdapterCommand cmd, bool* fRecCtrlLock )
{
  static const struct {
    AdapterCommand  cmd;                            ///< コマンド
    u8              fRecCtrlLock;                   ///< 録画再生コントロールをロックする（コマンド処理中）
    ClientSubProc   proc[ BTL_CLIENT_TYPE_MAX ];    ///< コマンド処理関数ポインタテーブル
  }procTbl[] = {

    { ACMD_WAIT_SETUP,        true,
      { &BTL_CLIENT::SubProc_UI_Setup,         &BTL_CLIENT::SubProc_AI_Setup,            &BTL_CLIENT::SubProc_REC_Setup          } },

    #if 1
    { ACMD_SELECT_ACTION,     false,
     { &BTL_CLIENT::SubProc_UI_SelectAction,   &BTL_CLIENT::SubProc_AI_SelectAction,   &BTL_CLIENT::SubProc_REC_SelectAction   } },
    #else
    // AIにテスト駆動させる
    { ACMD_SELECT_ACTION,   false,
       { &BTL_CLIENT::SubProc_AI_SelectAction,  &BTL_CLIENT::SubProc_AI_SelectAction,  &BTL_CLIENT::SubProc_REC_SelectAction   } },
    #endif

    { ACMD_SELECT_CHANGE_OR_ESCAPE,   false,
       { &BTL_CLIENT::SubProc_UI_SelectChangeOrEscape, NULL,  NULL  }
    },
    { ACMD_SELECT_POKEMON_FOR_COVER,    false,
       { &BTL_CLIENT::SubProc_UI_SelectPokemonForCover,  &BTL_CLIENT::SubProc_AI_SelectPokemon,   &BTL_CLIENT::SubProc_REC_SelectPokemon  }
    },
    { ACMD_SELECT_POKEMON_FOR_CHANGE,   false,
       { &BTL_CLIENT::SubProc_UI_SelectPokemonForChange, &BTL_CLIENT::SubProc_AI_SelectPokemon,  &BTL_CLIENT::SubProc_REC_SelectPokemon  } },

    { ACMD_CONFIRM_IREKAE,          false,
       { &BTL_CLIENT::SubProc_UI_ConfirmIrekae,      NULL,  &BTL_CLIENT::SubProc_REC_SelectPokemon  } },

    { ACMD_SERVER_CMD,        false,
       { &BTL_CLIENT::SubProc_UI_ServerCmd,    NULL,   &BTL_CLIENT::SubProc_REC_ServerCmd      } },

    { ACMD_RECORD_DATA,       false,
       { &BTL_CLIENT::SubProc_UI_RecordData,   NULL,                      NULL                       } },

    { ACMD_EXIT_NPC,          true,
      { &BTL_CLIENT::SubProc_UI_ExitForNPC,    NULL,     &BTL_CLIENT::SubProc_REC_ExitForNPC }, },

    { ACMD_EXIT_SUBWAY_TRAINER,         true,
        { &BTL_CLIENT::SubProc_UI_ExitForSubwayTrainer,  NULL,   &BTL_CLIENT::SubProc_REC_ExitForSubwayTrainer }, },

    { ACMD_EXIT_WIN_WILD,     true,
      { &BTL_CLIENT::SubProc_UI_WinWild,   NULL,  NULL }, },

    { ACMD_EXIT_LOSE_WILD,    true,
      { &BTL_CLIENT::SubProc_UI_LoseWild,   NULL,  NULL }, },

    { ACMD_EXIT_CAPTURE,    true,
      { &BTL_CLIENT::SubProc_UI_CaptureWild,   NULL,  NULL }, },

    { ACMD_EXIT_COMM,         true,
      { &BTL_CLIENT::SubProc_UI_ExitCommTrainer,   NULL,  &BTL_CLIENT::SubProc_REC_ExitCommTrainer }, },

    { ACMD_NOTIFY_TIMEUP,     true,
      { &BTL_CLIENT::SubProc_UI_NotifyTimeUp,  NULL,  NULL }, },

    { ACMD_FADEOUT,           true,
      { &BTL_CLIENT::SubProc_UI_FadeOut,  NULL,  &BTL_CLIENT::SubProc_REC_FadeOut }, },

    { ACMD_SEND_LATEST_REPLY, false,
      { &BTL_CLIENT::SubProc_UI_SendLastDataAgain, &BTL_CLIENT::SubProc_UI_SendLastDataAgain, &BTL_CLIENT::SubProc_UI_SendLastDataAgain }, },

    { ACMD_CLIENT_LIMIT_TIME_SEND, false,
      { &BTL_CLIENT::SubProc_UI_SendClientLimitTime, &BTL_CLIENT::SubProc_UI_SendClientLimitTime, NULL }, },

    { ACMD_CLIENT_LIMIT_TIME_SYNC, false,
      { &BTL_CLIENT::SubProc_UI_SyncClientLimitTime, NULL, NULL }, },

  };

  for(u32 i=0; i<GFL_NELEMS(procTbl); i++)
  {
    if( procTbl[i].cmd == cmd )
    {
      if( fRecCtrlLock != NULL ){
        *fRecCtrlLock = procTbl[i].fRecCtrlLock;
      }
      m_subProc = procTbl[i].proc[ m_myType ];
      m_subSeq  = 0;
      return true;
    }
  }

  GFL_ASSERT_MSG(0, "cmd=%d", cmd);
  return false;
}
/**
 * @brief サブプロセスを実行する
 * @retval true   サブプロセスが終了した
 * @retval false  サブプロセスが終了していない
 */
bool BTL_CLIENT::callSubProc( void )
{
  if( m_subProc )
  {
    return (this->*m_subProc)( &m_subSeq );
  }
  return true;
}


//=============================================================================================
/**
 * 逃げたクライアントのIDを取得（サーバコマンドを介して送られてくる）
 *
 */
//=============================================================================================
void BTL_CLIENT::GetEscapeInfo( EscapeInfo* dst ) const
{
  m_escapeInfo.Copy( dst );
}
//=============================================================================================
/**
 * ゲーム制限時間の終了チェック
 *
 * @param   wk
 *
 * @retval  bool    ゲームに制限時間が設定されており、かつそれが終わっている場合にtrue
 */
//=============================================================================================
bool BTL_CLIENT::IsGameTimeOver( void ) const
{
  if( m_myType != BTL_CLIENT_TYPE_RECPLAY )
  {
    if( m_gameLimitTime )
    {
      return m_viewCore->CMD_EFFECT_IsZero( BTLV_TIMER_TYPE_GAME_TIME );
    }
    return false;
  }
  else
  {
    return m_fRecPlayEndTimeOver;
  }
}
//=============================================================================================
/**
 * 録画バッファ読み取りエラーチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_CLIENT::IsRecPlayBufOver( void ) const
{
  return m_fRecPlayEndBufOver;
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------



bool BTL_CLIENT::SubProc_UI_Setup( int* seq )
{
  switch( *seq ){
  case 0:
    //@fix [#5742 ライブ大会：観戦機能にて観戦者と対戦者で乱数に差異が生じます]
    net::ReloadRecSender( this );

    BTL_PRINT("Client_%d, myType=%d, ViewCore=%d\n", m_myID, m_myType, m_viewCore );

    if( m_mainModule->GetCompetitor() != BTL_COMPETITOR_DEMO_CAPTURE ){
      m_viewCore->CMD_StartSetup();
    }else{
      GFL_ASSERT_MSG(0,"未対応\n");
    }
    (*seq)++;
    break;

  case 1:
    if( m_viewCore->CMD_WaitSetup() )
    {
      u8 myCoverPosNum = this->getMyCoverPosNum();
      this->m_frEffectProc.Start( m_mainModule, m_pokeCon, m_viewCore, m_myID, myCoverPosNum );
      (*seq)++;
    }
    break;

  case 2:
    if( this->m_frEffectProc.Wait() ){
      (*seq)++;
    }
    break;

  case 3:
    {
      m_cmdLimitTime  = m_mainModule->GetCommandLimitTime();
      m_gameLimitTime = m_mainModule->GetGameLimitTime();
      m_clientLimitTime = m_mainModule->GetClientLimitTime();
      if( m_cmdLimitTime || m_gameLimitTime || m_clientLimitTime )
      {
        m_viewCore->CMD_EFFECT_CreateTimer( m_gameLimitTime, m_clientLimitTime, m_cmdLimitTime );
        if( m_gameLimitTime ){
          m_viewCore->CMD_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_GAME_TIME, true, true, false );
        }
        if( m_clientLimitTime ) {
          m_viewCore->CMD_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_CLIENT_TIME, true, true, false );
        }
      }
    }

    enemyPokeHPBase_Update();

    // 録画スキップ状態の場合、スキップ中フラグを設定
    if( IsChapterSkipMode() )
    {
      m_viewCore->CMD_RecPlayer_StartSkip();
    }

#if PM_DEBUG
#if defined( GF_PLATFORM_CTR )
    if( m_mainModule->CheckImServerMachine() && ( m_mainModule->GetCommMode() != BTL_COMM_NONE ) )
    {
      gfl2::debug::LogWindow_AddMsg( gfl2::debug::LOGWIN_ID_NEX, L"I'm battle server" );
      GFL_RELEASE_PRINT("I'm battle server\n");
    }
#endif
#endif
    return true;

  }
  return false;
}

bool BTL_CLIENT::SubProc_AI_Setup( int* seq )
{
  m_cmdLimitTime  = m_mainModule->GetCommandLimitTime();
  m_gameLimitTime = m_mainModule->GetGameLimitTime();
  m_clientLimitTime = m_mainModule->GetClientLimitTime();
  return true;
}

bool BTL_CLIENT::SubProc_REC_Setup( int* seq )
{
#if PM_DEBUG
  //@fix [#5742 ライブ大会：観戦機能にて観戦者と対戦者で乱数に差異が生じます]
  if( m_mainModule->GetBattleSetupParam()->isLiveRecPlayMode == true )
  {
    if( m_mainModule->GetBattleSetupParam()->recRandSeed == 0 )
    {
      return false;
    }

    if( m_isLiveRecSeedSetup == false )
    {
      m_isLiveRecSeedSetup = true;
      GFL_PRINT("LiveRec Rand Setup\n");

      calc::ResetSys( m_mainModule->GetBattleSetupParam()->recRandSeed );
    }
  }
#endif

  if( m_viewCore )
  {
    bool fDone = SubProc_UI_Setup( seq );
    if( fDone )
    {
      if( m_btlRecReader->CheckBtlInCmd(m_myID) )
      {
        RecPlayer_TurnIncReq( &m_recPlayer );
      }
    }

    return fDone;
  }

  m_btlRecReader->CheckBtlInCmd( m_myID );
  return true;
}
/**
 * セットアップ直後、ふりむきなどの「かわいがり」関連演出を逐次処理開始
 */
void  BTL_CLIENT::FriendshipEffectProc::Start(
  const MainModule* mainModule,  const POKECON* pokeCon,  BattleViewSystem* viewCore,
  u8 myID, u8 numCoverPos )
{
  m_mainModule  = mainModule;
  m_pokeCon     = pokeCon;
  m_viewCore    = viewCore;
  m_numCoverPos = numCoverPos;
  m_myID        = myID;

  m_step = 0;
  m_searchIdx = 0;
}
/**
 * セットアップ直後、ふりむきなどの「かわいがり」関連演出を逐次処理実行
 * @return 処理終了したらtrue
 */
bool BTL_CLIENT::FriendshipEffectProc::Wait( void )
{
  switch( m_step ){
    case 0:
      while( m_searchIdx < m_numCoverPos ){
        const BTL_POKEPARAM* bpp = m_pokeCon->GetClientPokeDataConst( m_myID, m_searchIdx++ );
        if( !S_IsEnable(bpp) ){
          continue;
        }
        int effNo = this->checkEffectNo( bpp );
        if( effNo >= 0 ){
          BtlvPos vpos =  m_mainModule->PokeIDtoViewPos( m_pokeCon, bpp->GetID() );
          m_viewCore->CMD_AddEffectByPos( vpos, effNo );
          ++m_step;
          break;
        }
      }
      if( m_step == 0 ){
        return true;
      }
      break;

    case 1:
      if( m_viewCore->CMD_WaitEffectByPos() ){
        m_step = 0;
      }
      break;

    default:
      return true;
  }
  return false;
}
/**
 * セットアップ直後、ふりむきなどの「かわいがり」演出を行うかどうか「なかよし度」を見て判定
 */
int BTL_CLIENT::FriendshipEffectProc::checkEffectNo( const BTL_POKEPARAM* bpp ) const
{
  u8  friendship = m_mainModule->GetPokeFriendship( bpp );
  if( friendship >= 150 ){
    return BTLEFF_KAWAIGARI_TURN;
  }
  if( friendship >= 100 ){
    return BTLEFF_KAWAIGARI_JUMP;
  }
  if( friendship >= 50 ){
    return BTLEFF_KAWAIGARI_SHAKE;
  }
  return -1;
}
/** エフェクト動作させて良い状態のポケモンデータかどうか判定 */
bool BTL_CLIENT::FriendshipEffectProc::S_IsEnable( const BTL_POKEPARAM* bpp )
{
  if( !bpp || bpp->IsDead()
  || bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_ONLY_SICK )
  || bpp->CheckSick(pml::wazadata::WAZASICK_KOORI)
  ){
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------------------------------
// 敵先頭ポケモンの基準HP値を記録（ポケモン引っ込めメッセージ用：非通信、シングル戦のみ）
//------------------------------------------------------------------------------------------------------
void BTL_CLIENT::enemyPokeHPBase_Update( void )
{
  const BTL_POKEPARAM* bpp = enemyPokeHPBase_GetTargetPoke();
  if( bpp ){
    m_EnemyPokeHPBase = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
  }
}
u32 BTL_CLIENT::enemyPokeHPBase_CheckRatio( void )
{
  const BTL_POKEPARAM* bpp = enemyPokeHPBase_GetTargetPoke();
  if( bpp ){
    u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
    if( ( hp <= 0 ) ||
        ( hp >= m_EnemyPokeHPBase ) ) {
      return 0;
    }
    return ((m_EnemyPokeHPBase - hp) * 100) / hp;
  }
  return 0;
}
const BTL_POKEPARAM* BTL_CLIENT::enemyPokeHPBase_GetTargetPoke( void )
{
  const BtlCommMode commMode = m_mainModule->GetCommMode();
  if( commMode != BTL_COMM_NONE )
  {
    return NULL;
  }

  const BtlRule rule = m_mainModule->GetRule();
  if( ( rule != BTL_RULE_SINGLE ) &&
      ( rule != BTL_RULE_INTRUDE ) )
  {
    return NULL;
  }

  u8 clientID = m_mainModule->GetOpponentClientID( m_myID, 0 );
  return m_pokeCon->GetClientPokeDataConst( clientID, 0 );
}

//----------------------------------------------------------------------------------
/**
 * 制限時間 表示＆カウント開始
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
void BTL_CLIENT::cmdLimit_Start( void )
{
  if( m_cmdLimitTime )
  {
    m_viewCore->CMD_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_COMMAND_TIME, true, true, true );
    m_fForceQuitSelAct = false;
  }

  if( m_clientLimitTime )
  {
    m_viewCore->CMD_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_CLIENT_TIME, true, true, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * 制限時間 タイムオーバーチェック
 *
 * @param   wk
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::cmdLimit_CheckOver( void )
{
  if( m_cmdLimitTime )
  {
    if( m_fForceQuitSelAct == false )
    {
      if( m_viewCore->CMD_EFFECT_IsZero( BTLV_TIMER_TYPE_COMMAND_TIME ) ){
        BTL_N_Printf( DBGSTR_CLIENT_CmdLimitTimeOver );
        m_fForceQuitSelAct = true;
      }
    }
    return m_fForceQuitSelAct;
  }
  return false;
}
/**
 *  コマンド時間制限による強制終了があれば、メインプロセスを切り替えてtrueを返す
 */
bool BTL_CLIENT::checkSelactForceQuit( ClientSubProc nextProc )
{
  if( cmdLimit_CheckOver() )
  {
    if( nextProc != NULL ){
      selActSubProc_Set( nextProc );
    }
    return true;
  }
  return false;
}
/**
 *  コマンド時間制限表示を終了
 */
void BTL_CLIENT::cmdLimit_End( void )
{
  if( m_cmdLimitTime )
  {
    m_viewCore->CMD_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_COMMAND_TIME, false, false, false );
  }

  if( m_clientLimitTime )
  {
    m_viewCore->CMD_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_CLIENT_TIME, false, false, false );
  }
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 *  ○○はどうする？　文字列ID取得
 */
void BTL_CLIENT::setupSelectStartStr( const BTL_POKEPARAM* procPoke, BTLV_STRPARAM* strParam )
{
  int spStrID = -1;

  if( !m_b1stReadyMsgDisped )
  {
    spStrID = Kawaigari::Check1stReadyMessage( m_mainModule, procPoke, &m_random );
  }

  if( spStrID < 0 )
  {
    spStrID = Kawaigari::CheckCommonReadyMessage( m_mainModule, procPoke, &m_random );
  }

  // かわいがり特殊メッセージなし
  if( spStrID < 0 )
  {
    // なかよし度100未満は通常どおり（[ポケモン]はどうする？）
    if( m_mainModule->GetPokeFriendship(procPoke) < 100 )
    {
      BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_SelectAction );
      BTLV_STRPARAM_AddArg( strParam, procPoke->GetID() );
      BTLV_STRPARAM_SetWait( strParam, BTLV_MSGWAIT_AUTO_HIDE );
    }
    // なかよし度100以上は特殊（[ポケモン]は[トレーナー]の指示をまっている…）
    else
    {
      BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_SelectFriendship );
      BTLV_STRPARAM_AddArg( strParam, procPoke->GetID() );
      BTLV_STRPARAM_AddArg( strParam, m_myID );
      BTLV_STRPARAM_SetWait( strParam, BTLV_MSGWAIT_AUTO_HIDE );
    }
  }
  // かわいがり特殊メッセージあり
  else
  {
    BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, spStrID );
    BTLV_STRPARAM_AddArg( strParam, procPoke->GetID() );
    BTLV_STRPARAM_AddArg( strParam, m_myID );
    BTLV_STRPARAM_SetWait( strParam, BTLV_MSGWAIT_AUTO_HIDE );
  }
}
//--------------------------------------------------------------------------------------------
/**
 * なかよし度効果による特殊メッセージを出力する必要があるか判定＆文字列パラメータ生成
 *
 * @param[in]    procPoke   繰り出すポケモン
 * @param[out]   strParam   特殊メッセージ文字列パラメータ（出力する必要がある場合のみ）
 *
 * @retval  bool    なかよし度特殊メッセージあり = true
 */
//--------------------------------------------------------------------------------------------
bool BTL_CLIENT::checkFriendshipSpecialMessage( const BTL_POKEPARAM* procPoke, BTLV_STRPARAM* strParam )
{
  return false;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 *  サブプロセス設定
 */
void BTL_CLIENT::selActSubProc_Set( ClientSubProc proc )
{
  m_selActProc = proc;
  m_selActSeq = 0;
}
/**
 *  サブプロセスコール
 */
bool BTL_CLIENT::selActSubProc_Call( void )
{
  return (this->*m_selActProc)( &m_selActSeq );
}


bool BTL_CLIENT::SubProc_UI_SelectAction( int* seq )
{
  enum {
    SEQ_CAMERA_INIT,
    SEQ_CAMERA_WAIT,
    SEQ_TRAINER_MESSAGE_SWITCH,
    SEQ_TRAINER_MESSAGE_START,
    SEQ_TRAINER_MESSAGE_WAIT,
    SEQ_SELACT_START,
    SEQ_SELACT_WAIT,
  };

  static s8 trainerMessage_clientID;
  static TrainerMessage::MessageID trainerMessage_messageID;

  switch( *seq )
  {
  case SEQ_CAMERA_INIT:
    if(m_viewCore->CMD_InitStartWaitCameraEffect()){
      ++(*seq);
    }
    break;

  case SEQ_CAMERA_WAIT:
    if( m_viewCore->CMD_WaitStartWaitCameraEffect() ) {
      ++(*seq);
    }
    break;

  case SEQ_TRAINER_MESSAGE_SWITCH:
    (*seq ) = 
      ( this->DecideTrainerMessage_OnSelectAction( &trainerMessage_clientID, &trainerMessage_messageID ) ) ? 
      ( SEQ_TRAINER_MESSAGE_START ) : 
      ( SEQ_SELACT_START );
    break;

  case SEQ_TRAINER_MESSAGE_START:
    this->StartTrainerMessage( trainerMessage_clientID, trainerMessage_messageID );
    ++(*seq);
    break;

  case SEQ_TRAINER_MESSAGE_WAIT:
    if( this->UpdateTrainerMessage() ) {
      (*seq) = SEQ_TRAINER_MESSAGE_SWITCH;
    }
    break;

  case SEQ_SELACT_START:
    selActSubProc_Set( &BTL_CLIENT::selact_Start );
    cmdLimit_Start();
    (*seq)++;
    break;

  case SEQ_SELACT_WAIT:
    cmdLimit_CheckOver();
    if( selActSubProc_Call() )
    {
      #if PM_DEBUG
      if( m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_AI_CTRL) ){
        debugAICtrl_Delegate();
      }
      #endif
      return true;
    }
    break;
  }

  return false;
}

/**
 * @brief コマンド入力前において、表示すべきトレーナーメッセージを決定する
 * @param[out] clientID   メッセージを表示するクライアントのID
 * @param[out] messageID  表示するメッセージ
 * @retval true   表示すべきトレーナーメッセージが存在する
 * @retval false  表示すべきトレーナーメッセージが存在しない
 */
bool BTL_CLIENT::DecideTrainerMessage_OnSelectAction( s8* clientID, TrainerMessage::MessageID* messageID )
{
  // シナリオでないなら表示しない
  if( !( m_mainModule->IsCompetitorScenarioMode() ) ) {
    return false;
  }

  // 最初の攻撃直後のメッセージ
  *clientID = this->DecideTrainerMessage_OnSelectAction_FirstAttackDone();
  if( 0 <= *clientID ) {
    *messageID = TrainerMessage::MESSAGE_FIRST_ATTACK_DONE;
    return true;
  }

  // 最後のポケモンのHPが半分以下になった際のメッセージ
  *clientID = this->DecideTrainerMessage_OnSelectAction_LastPokeHpHalf();
  if( 0 <= *clientID ) {
    *messageID = TrainerMessage::MESSAGE_LAST_POKE_HP_HALF;
    return true;
  }

  return false;
}

/**
 * @brief コマンド入力前において、
 *       「最初のわざを出した後」のトレーナーメッセージを表示するクライアントを決定する
 * @return トレーナーメッセージ表示対象のクライアントID
 * @retval -1  どのトレーナーもメッセージを表示しない場合
 */
s8 BTL_CLIENT::DecideTrainerMessage_OnSelectAction_FirstAttackDone( void ) const
{
  for( u8 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    if( clientId == BTL_CLIENTID_SA_PLAYER ) {
      continue;
    }

    if( this->IsTrainerMessageEnable_OnSelectAction_FirstAttackDone( clientId ) ) {
      return clientId;
    }
  }
  return -1;
}

/**
 * @brief コマンド入力前において、
 *       「最初のわざを出した後」のトレーナーメッセージを表示するか？
 * @param clientID  チェック対象クライアント
 * @retval true   トレーナーメッセージを表示する
 * @retval false  トレーナーメッセージを表示しない
 */
bool BTL_CLIENT::IsTrainerMessageEnable_OnSelectAction_FirstAttackDone( u8 clientID ) const
{
 // トレーナーがいない
 if( !( m_mainModule->IsClientTrainerExist( clientID ) ) ) {
   return false;
 }

 // 技を出していない
 const BTL_PARTY* party = m_pokeCon->GetPartyDataConst( clientID );
 const BTL_POKEPARAM* pokeParam = party->GetMemberDataConst( 0 );
 if( pokeParam->WAZA_GetUsedCount() <= 0 ) {
   return false;
 }

 return m_trainerMessageManager->IsMessageExist( m_heapHandle->GetLowerHandle(), clientID, TrainerMessage::MESSAGE_FIRST_ATTACK_DONE );
}

/**
 * @brief コマンド入力前において、
 *       「最後のポケモンのHPが半分以下になった場合」のトレーナーメッセージを表示するクライアントを決定する
 * @return トレーナーメッセージ表示対象のクライアントID
 * @retval -1  どのトレーナーもメッセージを表示しない場合
 */
s8 BTL_CLIENT::DecideTrainerMessage_OnSelectAction_LastPokeHpHalf( void ) const
{
  for( u8 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    if( clientId == BTL_CLIENTID_SA_PLAYER ) {
      continue;
    }

    if( this->IsTrainerMessageEnable_OnSelectAction_LastPokeHpHalf( clientId ) ) {
      return clientId;
    }
  }
  return -1;
}

 /**
  * @brief コマンド入力前において、
  *       「最後のポケモンのHPが半分以下になった場合」のトレーナーメッセージを表示するか？
  * @param clientID  チェック対象クライアント
  * @retval true   トレーナーメッセージを表示する
  * @retval false  トレーナーメッセージを表示しない
  */
bool BTL_CLIENT::IsTrainerMessageEnable_OnSelectAction_LastPokeHpHalf( u8 clientID ) const
{
  // トレーナーがいない
  if( !( m_mainModule->IsClientTrainerExist( clientID ) ) ) {
    return false;
  }

  // 最後のポケモンでない
  const BTL_PARTY* party = m_pokeCon->GetPartyDataConst( clientID );
  if( party->GetAliveMemberCount() != 1 ) {
    return false;
  }
  
  // HPが半分以下でない
  const BTL_POKEPARAM* pokeParam = party->GetMemberDataConst( 0 );
  if( ( pokeParam->IsDead() ) || 
      ( FX32_CONST(50.0f) < pokeParam->GetHPRatio() ) ) {
    return false;
  }
  
  return m_trainerMessageManager->IsMessageExist( m_heapHandle->GetLowerHandle(), clientID, TrainerMessage::MESSAGE_LAST_POKE_HP_HALF );
}


bool BTL_CLIENT::SubProc_REC_SelectAction( int* seq )
{
#if PM_DEBUG
  enum
  {
    SEQ_NONE = 0,
    SEQ_START_WAIT,
    SEQ_CAMERA_RUNNING,
    SEQ_END_WAIT,
  };
  //@fix [fix]GFNMCat[5735]:[#5735 ライブ大会：観戦機能で全員のHPゲージと残りポケモン数が最初しか表示されない]
  if( m_mainModule->GetBattleSetupParam()->isLiveRecPlayMode && m_viewCore )
  {
    switch( m_liveRecWaitCameraSeq )
    {
    case SEQ_NONE:
    break;
    case SEQ_START_WAIT:
      if( m_viewCore->CMD_WaitStartWaitCameraEffect() == true )
      {
        m_liveRecWaitCameraSeq = SEQ_CAMERA_RUNNING;
      }
    break;
    case SEQ_CAMERA_RUNNING:
      if( m_liveRecSizeSave != m_mainModule->GetBattleSetupParam()->recDataSize )
      {//新しいデータが到着している
        m_viewCore->CMD_InitFinishWaitCameraEffect();
        m_liveRecWaitCameraSeq = SEQ_END_WAIT;
      }
    break;
    case SEQ_END_WAIT:
      if( m_viewCore->CMD_WaitFinishWaitCameraEffect() == true )
      {
        m_liveRecWaitCameraSeq = SEQ_NONE;
      }
    break;
    }

    if( m_liveRecWaitCameraSeq != SEQ_NONE )
    {//カメラ処理中
      return false;
    }
  }
#endif

  switch( *seq ){
  case 0:
    // チャプタスキップ等のためフェードが開始されていたら何もしない
    if( m_fRecPlayEndBufOver || m_bRecPlayFadeStarted )
    {
      setNullActionRecplay();
      return true;
    }
    else
    {
      u8 numAction, fChapter;
#if PM_DEBUG
      if( m_mainModule->GetBattleSetupParam()->isLiveRecPlayMode )
      {//ライブ大会観戦モード
        m_btlRecReader->ResetError();
        m_btlRecReader->SetDataSize( m_mainModule->GetBattleSetupParam()->recDataSize );
        
        //@fix [fix]GFNMCat[5735]:[#5735 ライブ大会：観戦機能で全員のHPゲージと残りポケモン数が最初しか表示されない]
        if( m_btlRecReader->IsReadComplete( m_myID ) == true )
        {//一旦すべて再生しきった
          m_liveRecWaitCameraSeq = SEQ_START_WAIT;
          m_liveRecSizeSave = m_mainModule->GetBattleSetupParam()->recDataSize;
          if( m_viewCore )
          {
            m_viewCore->CMD_InitStartWaitCameraEffect();
          }
          return false;
        }
      }
#endif
      const BTL_ACTION_PARAM* act = m_btlRecReader->ReadAction( m_myID, &numAction, &fChapter );
      if( fChapter ){
        RecPlayer_TurnIncReq( &m_recPlayer );
      }

      storeActionSelectSendData( act, numAction );

      // 再生時タイムオーバー検出処理
      if( act->gen.cmd == BTL_ACTION_RECPLAY_TIMEOVER )
      {
        RecPlayer_Quit( &m_recPlayer );

        // 描画クライアントなのでメッセージ表示へ
        if( m_viewCore )
        {
          BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_BattleTimeOver );
          m_viewCore->CMD_StartMsg( &m_strParam );
          (*seq)++;
        // 描画クライアントではないので終了
        }else{
          setNullActionRecplay();
          m_fRecPlayEndTimeOver = true;
          return true;
        }
      }
      // 読み込みエラー
      else if( act->gen.cmd == BTL_ACTION_RECPLAY_ERROR )
      {

#if PM_DEBUG
        if( m_mainModule->GetBattleSetupParam()->isLiveRecPlayMode )
        {//ライブ大会観戦モード
          return false;
        }
#endif

        BTL_N_Printf( DBGSTR_CLIENT_ReadRecError, m_myID );
        m_fRecPlayEndBufOver = true;
        RecPlayer_Quit( &m_recPlayer );

        // 描画クライアントなのでメッセージ表示へ
        if( m_viewCore )
        {
          BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_BatteRecBufferOver );
          m_viewCore->CMD_StartMsg( &m_strParam );
          (*seq) += 2;
        }
        else
        {
          setNullActionRecplay();
          return true;
        }
      }
      else
      {
        BTL_N_Printf( DBGSTR_CLIENT_ReadRecAct, m_myID, numAction);
        if( act->gen.cmd  == BTL_ACTION_FIGHT ){
          BTL_N_Printf( DBGSTR_CLIENT_ReadRecAct_Fight, act->fight.waza);
        }
        if( act->gen.cmd  == BTL_ACTION_CHANGE ){
          BTL_N_Printf( DBGSTR_CLIENT_ReadRecAct_Change, act->change.memberIdx);
        }
        return true;
      }
    }
    break;

  // タイムオーバー検出後、描画クライアントのみここに来る
  case 1:
    if( m_viewCore->CMD_WaitMsg() ){
      setNullActionRecplay();
      m_fRecPlayEndTimeOver = true;
      return true;
    }
    break;

  // 録画データバッファ読み取りオーバー検出後、描画クライアントのみここに来る
  case 2:
    if( m_viewCore->CMD_WaitMsg() ){
      setNullActionRecplay();
      return true;
    }
    break;

  }
  return false;
}
/**
 *  タイムオーバー検出時などの無効アクションパラメータ返信設定
 */
void BTL_CLIENT::setNullActionRecplay( void )
{
  m_procAction = &m_actionParam[ 0 ];
  BTL_ACTION_SetNULL( m_procAction );

  storeActionSelectSendData( m_actionParam, 1 );
}


//----------------------------------------------------------------------
/**
 *  アクション選択ワーク初期化
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_Start( int*  seq )
{
  m_procPokeIdx = 0;
  m_prevPokeIdx = -1;
  m_firstPokeIdx = 0;
  m_wazaInfoPokeIdx = 0;
  m_fSelActForceFinish = false;

  const u8 myCoverPosNum = this->getMyCoverPosNum();

  // ダブル以上の時、「既に選ばれているポケモン」を記録するために初期化をここで行う
  setupPokeSelParam( myCoverPosNum, &m_pokeSelParam, &m_pokeSelResult );

  // アイテム選択ワーク初期化
  selItemWork_Init();

  // 選択行動管理ワーク初期化（AIと共通処理）
  selact_ClearWorks();


  for(u32 i=0; i<myCoverPosNum; ++i)
  {
    const BTL_POKEPARAM* bpp = m_pokeCon->GetClientPokeData( m_myID, i );
    BTL_PRINT("SelAct (myClientID=%d, MyPoke(%d) = %d\n", m_myID, i, bpp->GetID());

    if( !checkActionForceSet(bpp,  NULL) ){
      m_firstPokeIdx = i;
      break;
    }
  }

  // バトル開始直後の暗転中に行うと間が長く感じるためここでＳＥロードする（何回読んでも１度しか行われないようになってます）
  //着地音が鳴らなくなったため、エンカウントエフェクト中に移動しました Ariizumi 130307
  //m_mainModule->SetupBattleSE();

  selActSubProc_Set( &BTL_CLIENT::selact_Root );

  return false;
}

/**
 *  アクション選択途中の標準メッセージウィンドウ文字出力
 */
void BTL_CLIENT::selact_startMsg( const BTLV_STRPARAM* strParam )
{
  m_viewCore->CMD_StartMsg( strParam );
  m_fStdMsgChanged = true; // 「○○はどうする？」メッセージを書き換えたフラグON
}

//------------------------------------------------------------
/**
 * 選択行動管理ワーク初期化（AIと共通処理）
 */
//------------------------------------------------------------
void BTL_CLIENT::selact_ClearWorks( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_actionParam); ++i){
    BTL_ACTION_SetNULL( &m_actionParam[i] );
  }
  for(u32 i=0; i < GFL_NELEMS(m_actionCountWork); ++i){
    m_actionCountWork[i] = 0;
  }

  //@バトル変更箇所 BTS7052 の移植忘れ
  m_bSetMegaEvoThisTurn = false;
  m_bSetUltraBurstThisTurn = false;
}

//----------------------------------------------------------------------
/**
 *  アクション選択の強制終了
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_ForceQuit( int* seq )
{
  switch( *seq ){
  case 0:
    m_viewCore->CMD_ForceQuitInput_Notify();
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_ForceQuitInput_Wait() )
    {
      u8 myCoverPosNum = this->getMyCoverPosNum();
      while( m_procPokeIdx < myCoverPosNum )
      {
        setupCurrentPokeActionPtr();
        BTL_ACTION_SetNULL( m_procAction );
        if( !checkActionForceSet(m_procPoke, m_procAction) )
        {
          u8 usableWazaFlag[ PTL_WAZA_MAX ];
          u8 wazaIdx = storeSelectableWazaFlag( m_procPoke, usableWazaFlag );
          if( wazaIdx < PTL_WAZA_MAX )
          {
            WazaID waza = m_procPoke->WAZA_GetID( wazaIdx );
            BtlPokePos  targetPos = calc::DecideWazaTargetAutoForClient( m_mainModule, m_pokeCon, m_procPoke, waza, &m_randContext );
            BTL_ACTION_SetFightParam( m_procAction, waza, targetPos );
          }else{
            setWaruagakiAction( m_procAction, m_procPoke );
          }
        }
        m_procPokeIdx++;
      }
      (*seq)++;
    }
    break;
  case 2:
    setupSelActReturnData();
    selActSubProc_Set( &BTL_CLIENT::selact_Finish );
    break;
  }
  return false;
}
//----------------------------------------------------------------------
/**
 *  ゆびをふるデバッグ
 */
//----------------------------------------------------------------------
#if PM_DEBUG
bool BTL_CLIENT::selact_YubiFuruDebug( int* seq )
{
  switch( *seq ){
  case 0:
    s_YubiCtrlPos = 0;
    m_fStdMsgChanged = false;
    (*seq)++;
    /* fallthru */
  case 1:
    BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_YubiFuruDebug );
    BTLV_STRPARAM_AddArg( &m_strParam, s_YubiCtrlPos );
    BTLV_STRPARAM_AddArg( &m_strParam, tables::g_YubiFuruDebugNumber[s_YubiCtrlPos] );
    BTLV_STRPARAM_AddArg( &m_strParam, tables::g_YubiFuruDebugNumber[s_YubiCtrlPos] );
    m_viewCore->CMD_PrintMsgAtOnce( &m_strParam );
    (*seq)++;
    break;
  case 2:
    if( !m_viewCore->CMD_WaitMsg() ){
      (*seq)++;
    }
    break;
  case 3:
    {
      u8 fUpdate = false;

      if( ui::CheckKeyRepeat(DIR_LEFT) )
      {
        s_YubiCtrlPos = (s_YubiCtrlPos)? (s_YubiCtrlPos-1) : (BTL_POS_NUM-1);
        fUpdate = true;
      }
      else if( ui::CheckKeyRepeat(DIR_RIGHT) )
      {
        if( ++s_YubiCtrlPos >= (BTL_POS_NUM) ){ s_YubiCtrlPos = 0; }
        fUpdate = true;
      }
      else if( ui::CheckKeyRepeat(DIR_UP) )
      {
        tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ]++;
        fUpdate = true;
      }
      else if( ui::CheckKeyRepeat(DIR_DOWN) )
      {
        tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ]--;
        fUpdate = true;
      }
      else if( ui::CheckKeyRepeat(BUTTON_X) )
      {
        tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ] += 10;
        fUpdate = true;
      }
      else if( ui::CheckKeyRepeat(BUTTON_Y) )
      {
        tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ] -= 10;
        fUpdate = true;
      }
      else if( ui::CheckKeyRepeat(BUTTON_R) )
      {
        tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ] += 100;
        fUpdate = true;
      }
      else if( ui::CheckKeyRepeat(BUTTON_L) )
      {
        tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ] -= 100;
        fUpdate = true;
      }

      if( fUpdate )
      {
        if( tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ] >= WAZANO_MAX ){
          tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ] = 0;
        }
        else if( tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ] < 0 ){
          tables::g_YubiFuruDebugNumber[ s_YubiCtrlPos ] = WAZANO_MAX - 1;
        }
        (*seq) = 1;
        break;
      }
      else
      {
        if( ui::CheckKeyRepeat(BUTTON_A) )
        {
          tables::YubifuruDebugSetEnd();
          setupSelectStartStr( m_procPoke, &m_strParam );
          m_viewCore->CMD_PrintMsgAtOnce( &m_strParam );
          selActSubProc_Set( &BTL_CLIENT::selact_Root );
          (*seq) = m_yubifuruDebugRetSeq;

          BTL_PRINT("[Client] SystemheapState / Total:0x%08x, Free:0x%08x, Allocatable:0x%08x\n",
                    m_heapHandle->GetTotalSize(), m_heapHandle->GetTotalFreeSize(), m_heapHandle->GetTotalAllocatableSize() );

          BattleAiSystem::DumpHeap();

        }
      }
    }
  }
  return false;
}
#endif  // #if PM_DEBUG

//----------------------------------------------------------------------------------------------
/**
 *  アクション選択ルート
 */
//----------------------------------------------------------------------------------------------
bool BTL_CLIENT::selact_Root( int* seq )
{
  switch( *seq ){
  case SELACT_ROOTSEQ_START:
    return selact_root_start( seq );

  case SELACT_ROOTSEQ_FRIENDSHIP_MSG_WAIT:
    return selact_root_friendship_msg_wait( seq );

  case SELACT_ROOTSEQ_WAIT_MSG_CHECK:
    return selact_root_wait_msg_check( seq );

  case SELACT_ROOTSEQ_WAIT_MSG_WAIT:
    //if( !m_viewCore->CMD_WaitMsg() ){
      //return false;
    //}
    (*seq) = SELACT_ROOTSEQ_SEL_START;

    /* fallthru */

  // アクション選択開始
  case SELACT_ROOTSEQ_SEL_START:
    m_viewCore->CMD_UI_SelectAction_Start( m_procPoke, m_procPokeIdx, (m_procPokeIdx>m_firstPokeIdx), m_procAction );
    (*seq) = SELACT_ROOTSEQ_SEL_MAIN;
    break;

  case SELACT_ROOTSEQ_SEL_MAIN:
    return selact_root_sel_main( seq );

  case SELACT_ROOTSEQ_WAIT_UI_RESTART:
    if( m_viewCore->CMD_UI_WaitRestart() ){
      (*seq) = SELACT_ROOTSEQ_SEL_START;
    }
    break;

  // フリーフォール中に「たたかう」以外のコマンドを選んだ時にここ
  case SELACT_ROOTSEQ_FREEFALL_WARN:
    m_viewCore->CMD_UI_RestartIfNotStandBy( );
    (*seq) = SELACT_ROOTSEQ_FREEFALL_WARN_MSG;
    break;
  case SELACT_ROOTSEQ_FREEFALL_WARN_MSG:
    if( m_viewCore->CMD_UI_WaitRestart() )
    {
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_FreeFallBind );
      BTLV_STRPARAM_AddArg( &m_strParam, m_procPoke->GetID() );
      selact_startMsg( &m_strParam );
      (*seq) = SELACT_ROOTSEQ_FREEFALL_WARN_WAIT;
    }
    break;
  case SELACT_ROOTSEQ_FREEFALL_WARN_WAIT:
    if( m_viewCore->CMD_WaitMsg() )
    {
      (*seq) = SELACT_ROOTSEQ_WAIT_MSG_CHECK;
    }
    break;

  }
  return false;
}
/**
 *  指定順のポケモンまでに追加されたアクション数を返す
 */
u8 BTL_CLIENT::calcAddActionCountSum( u8 pokeIdx )
{
  u8 sum = 0;

  for(u32 i=0; i<pokeIdx; ++i)
  {
    sum += m_actionCountWork[ i ];
  }

  return sum;
}
/**
 * m_procPokeIdx に指定された順番のポケモン行動決定用、各種メンバをセットアップ
 */
void BTL_CLIENT::setupCurrentPokeActionPtr( void )
{
  u8  addActionCount = calcAddActionCountSum( m_procPokeIdx );

  m_procPoke   = m_pokeCon->GetClientPokeData( m_myID, m_procPokeIdx );
  m_procAction = &m_actionParam[ m_procPokeIdx+addActionCount ];
  m_currentActionCount = &m_actionCountWork[ m_procPokeIdx ];
  (*m_currentActionCount) = 0;

  BTL_ACTION_SetNULL( m_procAction );
}

//------------------------------------------------------------
/**
 *  アクション選択ルート（サブプロセス ）  初回
 */
//------------------------------------------------------------
bool BTL_CLIENT::selact_root_start( int* seq )
{
  setupCurrentPokeActionPtr();

  if( checkActionForceSet(m_procPoke, m_procAction) ){
    selActSubProc_Set( &BTL_CLIENT::selact_CheckFinish );
  }
  else
  {
    (*seq) = SELACT_ROOTSEQ_WAIT_MSG_CHECK;
  }
  return false;
}
/** 追加アクション数をインクリメントする */
void BTL_CLIENT::incrementAddActionCount( void )
{
  ++(*m_currentActionCount);
}
/** 追加アクション数をデクリメントする */
void BTL_CLIENT::decrementAddActionCount( void )
{
  if( *m_currentActionCount )
  {
    --(*m_currentActionCount);
  }
}

//------------------------------------------------------------
/**
 *  アクション選択ルート：サブプロセス    なつき度による特殊メッセージの終了待ち
 */
//------------------------------------------------------------
bool BTL_CLIENT::selact_root_friendship_msg_wait( int* seq )
{
  if( m_viewCore->CMD_WaitMsg() )
  {
    (*seq) = SELACT_ROOTSEQ_WAIT_MSG_CHECK;
  }
  return false;
}
//------------------------------------------------------------
/**
 *  アクション選択ルート：サブプロセス    行動選択待ちメッセージのチェック
 */
//------------------------------------------------------------
bool BTL_CLIENT::selact_root_wait_msg_check( int* seq )
{
  // 「○○はどうする？」表示
  if( (m_prevPokeIdx != m_procPokeIdx)
  ||  (m_fStdMsgChanged)
  ){
    setupSelectStartStr( m_procPoke, &m_strParam );
    m_viewCore->CMD_StartMsg( &m_strParam );
    m_fStdMsgChanged = false;
    m_b1stReadyMsgDisped = true;
    m_prevPokeIdx = m_procPokeIdx;
    (*seq) = SELACT_ROOTSEQ_WAIT_MSG_WAIT;
  }
  else{
    (*seq) = SELACT_ROOTSEQ_SEL_START;
  }
  return false;
}
//------------------------------------------------------------
/**
 *  アクション選択ルート：サブプロセス    行動選択メイン処理
 */
//------------------------------------------------------------
bool BTL_CLIENT::selact_root_sel_main( int* seq )
{
  // デバッグ用Print制御
  #if PM_DEBUG
  BTL_DEBUGPRINT_Ctrl();
  #endif

  // デバッグ用ゆびをふる制御
  #if PM_DEBUG
  if( ui::CheckKeyCont(BUTTON_R) && ui::CheckKeyCont(BUTTON_Y) )
  {
    if( m_mainModule->CheckImServerMachine() ){
      m_yubifuruDebugRetSeq = (*seq);
      selActSubProc_Set( &BTL_CLIENT::selact_YubiFuruDebug );
      return false;
    }
  }
  #endif

  if( checkSelactForceQuit(&BTL_CLIENT::selact_ForceQuit) )
  {
    BTL_N_Printf( DBGSTR_CLIENT_ForceQuitByTimeLimit, m_myID );
    m_viewCore->CMD_UI_SelectAction_ForceQuit();
    return false;
  }

  switch( m_viewCore->CMD_UI_SelectAction_Wait() ){

  // 「ポケモン」を選んだ->交替対象の選択へ
  case BTL_ACTION_CHANGE:
    // フリーフォール捕まれチェック
    if( m_procPoke->CheckSick(pml::wazadata::WAZASICK_FREEFALL) )
    {
      m_viewCore->CMD_UI_Restart();
      (*seq) = SELACT_ROOTSEQ_FREEFALL_WARN;
      break;
    }

    selActSubProc_Set( &BTL_CLIENT::selact_SelectChangePokemon );
    break;

  // 「たたかう」を選んだ
  case BTL_ACTION_FIGHT:
    BTL_N_Printf( DBGSTR_CLIENT_SelectAction_Fight );
    selActSubProc_Set( &BTL_CLIENT::selact_Fight );
    break;

  // 「どうぐ」を選んだ
  case BTL_ACTION_ITEM:
    // フリーフォール捕まれチェック
    if( m_procPoke->CheckSick(pml::wazadata::WAZASICK_FREEFALL) ){
      m_viewCore->CMD_UI_Restart();
      (*seq) = SELACT_ROOTSEQ_FREEFALL_WARN;
      break;
    }
    selActSubProc_Set( &BTL_CLIENT::selact_Item );
    break;

  // 「にげる」or「もどる」
  case BTL_ACTION_ESCAPE:
    // 先頭のポケなら「にげる」として処理
    if( m_procPokeIdx == m_firstPokeIdx )
    {
      // フリーフォール捕まれチェック
      if( m_procPoke->CheckSick(pml::wazadata::WAZASICK_FREEFALL) )
      {
        // 「降参しますか？」モードでなければ実行不可
        if( m_mainModule->GetEscapeMode() != BTL_ESCAPE_MODE_CONFIRM ){
          m_viewCore->CMD_UI_Restart();
          (*seq) = SELACT_ROOTSEQ_FREEFALL_WARN;
          break;
        }
      }
      selActSubProc_Set( &BTL_CLIENT::selact_Escape );
    }
    // ２体目以降は「もどる」として処理
    else
    {
      BTL_POKEPARAM* bpp;
      while( m_procPokeIdx )
      {
        m_procPokeIdx--;
        bpp = m_pokeCon->GetClientPokeData( m_myID, m_procPokeIdx );
        if( !checkActionForceSet(bpp, NULL) )
        {
          //@バトル変更箇所 田谷さん確認済み
          //BTS97の対応。メガが積まれているとm_actionParamがずれる。
          u8  addActionCount = calcAddActionCountSum( m_procPokeIdx );
          BTL_ACTION_PARAM* prevPokeAction = &m_actionParam[ m_procPokeIdx + addActionCount];

          // 「もどる」先のポケモンが、既に「ポケモン」で交換対象を選んでいた場合はその情報をPopする
          if( BTL_ACTION_GetAction(prevPokeAction) == BTL_ACTION_CHANGE ){
            m_pokeSelResult.Pop();
          }
          // 「もどる」先のポケモンが、既に「どうぐ」で道具を選んでいた場合はアイテム選択ワークを１つ戻す
          if( BTL_ACTION_GetAction(prevPokeAction) == BTL_ACTION_ITEM )
          {
            // 「ポケモン」->「かいふくする」で道具を使っている場合は選択情報も Pop する
            if( selItemWork_IsFromPokeSelect(m_procPokeIdx) ){
              m_pokeSelResult.Pop();
            }
            selItemWork_Restore( m_procPokeIdx );
          }
          selActSubProc_Set( &BTL_CLIENT::selact_Root );
          return false;
        }
      }
      GFL_ASSERT(0);
    }
    break;
  }
  return false;
}

/**
 * 「たたかう」を選んだアクションパラメータの前に、メガ進化を差し込む
 */
BTL_ACTION_PARAM* BTL_CLIENT::insertActMegaEvoParam( BTL_ACTION_PARAM* ary, u8 pokeIdx )
{
  BTL_ACTION_PARAM tmp = *ary;

  BTL_ACTION_OffMegaEvo( &tmp );
  BTL_ACTION_OffUltraBurst( &tmp );
  BTL_ACTION_SetMegaEvoParam( ary++, pokeIdx );
  this->incrementAddActionCount();
  *ary = tmp;

//  BTL_PRINT("メガ進化コマンドをセットしました ActionPtr=%p, pokeIdx=%d, actCount=%d, ptr=%p\n", ary, pokeIdx, *(m_currentActionCount), m_currentActionCount );

  return ary;
}
/**
 * insertActMegaEvoParam でメガ進化コマンドが差し込まれていた場合、それを元に戻す
 */
BTL_ACTION_PARAM* BTL_CLIENT::deleteActMegaEvoParam( BTL_ACTION_PARAM* ary )
{
  if( *m_currentActionCount )
  {
    BTL_ACTION_PARAM  tmp = *ary;

    --ary;
    if( BTL_ACTION_IsMegaEvoParam(ary) )
    {
      *ary = tmp;
      this->decrementAddActionCount();
    }
    else
    {
      ++ary;
    }
  }

  return ary;
}



//----------------------------------------------------------------------
/**
 *  「たたかう」選択後の分岐
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_Fight( int* seq )
{
  enum {
    SEQ_START = 0,
    SEQ_SELECT_WAZA_START,
    SEQ_SELECT_WAZA_WAIT,
    SEQ_SELECT_WAZA_END,
    SEQ_CHECK_WAZA_TARGET,
    SEQ_SELECT_TARGET_START,
    SEQ_SELECT_TARGET_WAIT,
    SEQ_WAIT_UNSEL_WAZA_MSG,
    SEQ_WAIT_UNSEL_TARGET_MSG,
  };

  static ClientSubProc nextProcAfterWazaSelectEnd = NULL;

  switch( *seq ){
  case SEQ_START:
    if( checkWazaForceSet( m_procPoke, m_procAction ) ){
      selActSubProc_Set( &BTL_CLIENT::selact_CheckFinish );
    }else{
      (*seq) = SEQ_SELECT_WAZA_START;
    }
    break;

  case SEQ_SELECT_WAZA_START:
    m_viewCore->CMD_UI_SelectWaza_Start( m_procPoke, m_procPokeIdx, m_procAction );
    (*seq) = SEQ_SELECT_WAZA_WAIT;
    break;

  case SEQ_SELECT_WAZA_WAIT:
    if( checkSelactForceQuit(&BTL_CLIENT::selact_ForceQuit) )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForceQuitByTimeLimit, m_myID );
      m_viewCore->CMD_UI_SelectWaza_ForceQuit();
      return false;
    }

    if( m_viewCore->CMD_UI_SelectWaza_Wait() )
    {
      nextProcAfterWazaSelectEnd = NULL;

      BtlAction action = BTL_ACTION_GetAction( m_procAction );

      if( action == BTL_ACTION_NULL )
      {
        nextProcAfterWazaSelectEnd = &BTL_CLIENT::selact_Root;
        (*seq) = SEQ_SELECT_WAZA_END;
        return false;
      }
      else
      {
        const BTL_POKEPARAM* procPoke = m_procPoke;
        BTL_ACTION_PARAM*    procAction = m_procAction;
        WazaNo  checkWaza  = BTL_ACTION_GetWazaID( procAction, procPoke );
        bool    isZenryoku = BTL_ACTION_IsFightWithZenryokuWaza( procAction );
        u8 pokeIdx = m_procPokeIdx;

        // ワザ説明ショートカットへ
        if( BTL_ACTION_IsWazaInfoMode(procAction) )
        {
          m_wazaInfoPokeIdx = pokeIdx;
          m_wazaInfoWazaIdx = procPoke->WAZA_SearchIdx( BTL_ACTION_GetOriginalWazaID( procAction ) );
          if( m_wazaInfoWazaIdx != PTL_WAZA_MAX ) 
          {
            nextProcAfterWazaSelectEnd = &BTL_CLIENT::selact_WazaInfoView;
            (*seq) = SEQ_SELECT_WAZA_END;
            return false;
          }
        }

        // 選択できない技を選んでいたら、メッセージを表示して、再度技選択へ
        if( is_unselectable_waza( procPoke, checkWaza, isZenryoku, &m_strParam ) )
        {
          BTLV_STRPARAM_SetWait( &m_strParam, BTLV_MSGWAIT_NONE );
          selact_startMsg( &m_strParam );
          (*seq) = SEQ_WAIT_UNSEL_WAZA_MSG;
          return false;
        }

        // メガ進化をセットする
        if (BTL_ACTION_IsFightWithMegaEvo(m_procAction) || BTL_ACTION_IsFightWithUltraBurst(m_procAction)) {
          m_procAction = insertActMegaEvoParam( m_procAction, m_procPokeIdx );
        }
        (*seq) = SEQ_SELECT_WAZA_END;
        return false;
      }
    }
    break;


  case SEQ_SELECT_WAZA_END:
    if( m_viewCore->CMD_UI_SelectWaza_End() )
    {
      if( nextProcAfterWazaSelectEnd != NULL )  {
        selActSubProc_Set( nextProcAfterWazaSelectEnd );
        break;
      }

      (*seq) = SEQ_CHECK_WAZA_TARGET;
    }
    break;


  case SEQ_CHECK_WAZA_TARGET:
    // シングルなら対象選択なし
    {
      BTL_PRINT("[UIclient %d 番目] 選択したワザ:%d\n", m_procPokeIdx, m_procAction->fight.waza );
      BtlRule rule = m_mainModule->GetRule();
      if( calc::RULE_IsNeedSelectTarget(rule) )
      {
        (*seq) = SEQ_SELECT_TARGET_START;
      }
      else
      {
        BtlPokePos procPokePos = m_mainModule->PokeIDtoPokePos( m_pokeCon, m_procPoke->GetID() );
        m_procAction->fight.targetPos = m_mainModule->GetOpponentPokePos( procPokePos, 0 );

        selActSubProc_Set( &BTL_CLIENT::selact_CheckFinish );
      }
    }
    break;

  case SEQ_SELECT_TARGET_START:
    BTL_N_Printf( DBGSTR_CLIENT_StartWazaTargetSel );
    m_viewCore->CMD_UI_SelectTarget_Start( m_procPokeIdx, m_procPoke, m_procAction );
    (*seq) = SEQ_SELECT_TARGET_WAIT;
    break;

  case SEQ_SELECT_TARGET_WAIT:
    if( checkSelactForceQuit(&BTL_CLIENT::selact_ForceQuit) ){
      m_viewCore->CMD_UI_SelectTarget_ForceQuit();
      return false;
    }

    {
      BtlvResult result = m_viewCore->CMD_UI_SelectTarget_Wait();
      if( result == BTLV_RESULT_DONE )
      {
        WazaNo  checkWaza     = BTL_ACTION_GetWazaID( m_procAction, m_procPoke );
        bool    isZenryoku    = BTL_ACTION_IsFightWithZenryokuWaza( m_procAction );
        BtlPokePos  targetPos = BTL_ACTION_GetWazaTargetPos( m_procAction );

        // 選択できない対象を選んでいたら、メッセージを表示して、再度技選択へ
        if( is_unselectable_target( m_procPoke, checkWaza, targetPos, isZenryoku, &m_strParam ) )
        {
          BTLV_STRPARAM_SetWait( &m_strParam, BTLV_MSGWAIT_NONE );
          selact_startMsg( &m_strParam );
          (*seq) = SEQ_WAIT_UNSEL_TARGET_MSG;
          return false;
        }

        selActSubProc_Set( &BTL_CLIENT::selact_CheckFinish );
      }
      else if( result == BTLV_RESULT_CANCEL )
      {
        m_procAction = deleteActMegaEvoParam( m_procAction );
        (*seq) = SEQ_SELECT_WAZA_START;
      }
    }
    break;

  case SEQ_WAIT_UNSEL_WAZA_MSG:
    if( m_viewCore->CMD_WaitMsg() )
    {
      m_viewCore->CMD_UI_SelectWaza_Restart( m_procPokeIdx );
      (*seq) = SEQ_SELECT_WAZA_WAIT;
    }
    break;

  case SEQ_WAIT_UNSEL_TARGET_MSG:
    if( m_viewCore->CMD_WaitMsg() )
    {
      m_viewCore->CMD_UI_SelectTarget_Start( m_procPokeIdx, m_procPoke, m_procAction );
      (*seq) = SEQ_SELECT_TARGET_WAIT;
    }
    break;
  }

  return false;
}
//----------------------------------------------------------------------
/**
 *  ワザ説明画面（ショートカットモード）
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_WazaInfoView( int* seq )
{
  switch( *seq ){
  case 0:
    m_viewCore->CMD_StartWazaInfoView( m_wazaInfoPokeIdx, m_wazaInfoWazaIdx );
    (*seq)++;
    break;
  case 1:
    // 制限時間による強制終了（ポケモン選択画面の終了通知）
    if( checkSelactForceQuit(NULL) ){
      m_viewCore->CMD_ForceQuitPokeSelect();
      (*seq) = 2;
      return false;
    }

    if( m_viewCore->CMD_WaitPokeSelect() )
    {
      selActSubProc_Set( &BTL_CLIENT::selact_Fight );
    }
    break;

  case 2:
    if( m_viewCore->CMD_WaitForceQuitPokeSelect() ){
      selActSubProc_Set( &BTL_CLIENT::selact_ForceQuit );
    }
    break;
  }
  return false;
}

//----------------------------------------------------------------------
/**
 *  「ポケモン」選択後のいれかえポケモン選択
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_SelectChangePokemon( int* seq )
{
  switch( *seq ){
  case 0:
    {
      u16 tokuseiID;
      u8  pokeID   = BTL_POKEID_NULL;
      u8  fCantEsc = false;
      u8  code = isForbidPokeChange( m_procPoke, &pokeID, &tokuseiID );
      if( (code != CANTESC_NULL)
      ||  (m_procPoke->GetID() == pokeID)
      ){
        fCantEsc = true;
      }
      BTL_PRINT("逃げ禁止コード:%d, pokeID=%d, tokusei=%d, ...res=%d -> %d\n", code, pokeID, tokuseiID, (code != CANTESC_NULL), fCantEsc);
      m_viewCore->CMD_StartPokeList( &m_pokeSelParam, m_procPoke, m_procPokeIdx, fCantEsc, &m_pokeSelResult );
      (*seq)++;
    }
    break;

  case 1:
    // 制限時間による強制終了（ポケモン選択画面の終了通知）
    if( checkSelactForceQuit(NULL) ){
      m_viewCore->CMD_ForceQuitPokeList();
      (*seq) = 2;
      return false;
    }

    if( m_viewCore->CMD_WaitPokeList() )
    {
      if( !m_pokeSelResult.IsCancel() )
      {
        u16 itemID;
        u8  targetIdx;
        u8  wazaIdx;
        // 道具を使う指定があった場合
        if( m_pokeSelResult.IsItemUse(&targetIdx, &itemID, &wazaIdx) )
        {
          u8 shortPokeID = this->calcShortPokeID( targetIdx );
          BTL_PRINT("道具を使う ITEM=%d, Idx=%d, shortID=%d, wazaIdx=%d\n", itemID, targetIdx, shortPokeID, wazaIdx );
          BTL_ACTION_SetItemParam( m_procAction, itemID, shortPokeID, wazaIdx );
          m_viewCore->CMD_ITEMSELECT_ReflectUsedItem();
          selItemWork_Reserve( m_procPokeIdx, itemID, true );
          selActSubProc_Set( &BTL_CLIENT::selact_CheckFinish );
          break;
        }
        // それ以外はポケモン入れ替え
        else{
          u8 idx = m_pokeSelResult.GetLast();
          BTL_PRINT( "通常入れ替えIDX=%d\n", idx );
          if( idx < BTL_PARTY_MEMBER_MAX )
          {
            BTL_N_Printf( DBGSTR_CLIENT_SelectChangePoke, idx);
            BTL_ACTION_SetChangeParam( m_procAction, m_procPokeIdx, idx );
            selActSubProc_Set( &BTL_CLIENT::selact_CheckFinish );
            break;
          }
        }
      }

      BTL_N_Printf( DBGSTR_CLIENT_SelectChangePokeCancel );
      selActSubProc_Set( &BTL_CLIENT::selact_Root );
    }
    break;

  // 制限時間による強制終了（ポケモン選択画面の終了待ち後）
  case 2:
    if( m_viewCore->CMD_WaitForceQuitPokeList() ){
      selActSubProc_Set( &BTL_CLIENT::selact_ForceQuit );
    }
    break;
  }
  return false;
}
//----------------------------------------------------------------------
/**
 *  「どうぐ」選択
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_Item( int* seq )
{
  switch( *seq ){
  case 0:
    {
      u8 fFirstPokemon = (m_procPokeIdx == m_firstPokeIdx);
      u8 fBallTargetHide = checkBallTargetHide();
      m_viewCore->CMD_ITEMSELECT_Start( m_bagMode, 0, 0, fFirstPokemon, fBallTargetHide );
      (*seq)++;
    }
    break;

  case 1:
    // 制限時間による強制終了通知
    if( checkSelactForceQuit(NULL) ){
      m_viewCore->CMD_ITEMSELECT_ForceQuit();
      (*seq) = 2;
      return false;
    }

    if( m_viewCore->CMD_ITEMSELECT_Wait() )
    {
      u16 itemID, targetIdx;
      itemID = m_viewCore->CMD_ITEMSELECT_GetItemID();
      targetIdx = m_viewCore->CMD_ITEMSELECT_GetTargetIdx();
      if( (itemID != ITEM_DUMMY_DATA) && (targetIdx != BTL_PARTY_MEMBER_MAX) )
      {
        u8  wazaIdx = m_viewCore->CMD_ITEMSELECT_GetWazaIdx();
        u8  shortPokeID = this->calcShortPokeID( static_cast<u8>(targetIdx) );
        BTL_ACTION_SetItemParam( m_procAction, itemID, shortPokeID, wazaIdx );
        m_viewCore->CMD_ITEMSELECT_ReflectUsedItem( );

        // ボールを選択した場合、残りポケモンのアクション選択は全てスキップさせる
        if( btl::calc::ITEM_GetParam(itemID, item::ITEM_DATA::PRM_ID_ITEM_TYPE) == ITEMTYPE_BALL )
        {
          m_fSelActForceFinish = true;
        }
        selItemWork_Reserve( m_procPokeIdx, itemID, false );
        selActSubProc_Set( &BTL_CLIENT::selact_CheckFinish );
      }else{
//      (*seq)=SEQ_SELECT_ACTION;
        selActSubProc_Set( &BTL_CLIENT::selact_Root );
      }
    }
    break;

  // 制限時間による強制終了待ち後
  case 2:
    if( m_viewCore->CMD_ITEMSELECT_Wait() ){
      selActSubProc_Set( &BTL_CLIENT::selact_ForceQuit );
    }
    break;

  }
  return false;
}
//------------------------------------------------------
/**
 * ボールを投げる対象が「そらをとぶ」など場から消えている判定
 */
//------------------------------------------------------
bool BTL_CLIENT::checkBallTargetHide( void )
{
  if( m_mainModule->GetCompetitor() == BTL_COMPETITOR_WILD )
  {
    const BTL_PARTY* party = m_pokeCon->GetPartyDataConst( BTL_CLIENT_ENEMY1 );
    u32 partyMemberCnt = party->GetMemberCount();
    u32 frontPosNum = m_mainModule->GetFrontPosNum( BTL_CLIENT_ENEMY1 );
    const BTL_POKEPARAM* bpp;
    u32 i, aliveCnt, fHide;

    if( frontPosNum > partyMemberCnt ){
      frontPosNum = partyMemberCnt;
    }

    fHide = false;
    aliveCnt = 0;
    for(i=0; i<frontPosNum; ++i)
    {
      bpp = party->GetMemberDataConst( i );
      if( bpp && !bpp->IsDead() )
      {
        ++aliveCnt;
        if( bpp->IsWazaHide() ){
          fHide = true;
        }
      }
    }

    if( (aliveCnt == 1) && (fHide) ){
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------
/**
 *  「にげる」選択
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_Escape( int* seq )
{
  enum {
    SEQ_INIT = 0,
    SEQ_WAIT_MSG_RETURN,
    SEQ_WAIT_MSG_RETURN_END,

    SEQ_WAIT_MSG_CONFIRM,
    SEQ_WAIT_CONFIRM_YESNO,

    SEQ_RETURN_ESCAPE,
  };

  switch( *seq ){
  case SEQ_INIT:
  {
    // 戦闘モード等による禁止チェック
    BtlEscapeMode esc = m_mainModule->GetEscapeMode();
    switch( esc ){
    case BTL_ESCAPE_MODE_WILD:  // 素早さなど条件クリアすれば逃げられる（野生）
    default:

#if PM_DEBUG
      if( ui::CheckKeyCont( BUTTON_L | BUTTON_R ) )
      {
        (*seq) = SEQ_RETURN_ESCAPE;
        break;
      }
#endif

      // 試練中は逃げられない
      if( m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_SIREN_BATTLE ) ||
          m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) )
      {
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_SirenEscape );
        selact_startMsg( &m_strParam );
        (*seq) = SEQ_WAIT_MSG_RETURN;
        break;
      }

      // @fix GFMMCat[209] VSヒカリドラゴン戦で、にげることができる
      // @fix GFMMCat[547] 合体ネクロズマ戦にて逃げられる
      if (m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_NO_CAPTURE))
      {
        BTLV_STRPARAM_Setup(&m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_EscapeProhibit);
        selact_startMsg(&m_strParam);
        (*seq) = SEQ_WAIT_MSG_RETURN;
        break;
      }

      {
        CantEscapeCode  code;
        u16 tokuseiID = TOKUSEI_NULL;
        u8  pokeID = BTL_POKEID_NULL;
        code = isForbidEscape( &pokeID, &tokuseiID );

        // とくせい、ワザ効果等による禁止チェック -> 何もない
        if( code == CANTESC_NULL )
        {
          (*seq) = SEQ_RETURN_ESCAPE;
        }
        // とくせい、ワザ効果等による禁止チェック -> 引っかかって逃げられない
        else
        {
          if( tokuseiID != TOKUSEI_NULL ){
            BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_CantEscTok );
            BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
            BTLV_STRPARAM_AddArg( &m_strParam, tokuseiID );
          }else{
            BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_EscapeProhibit );
          }
          selact_startMsg( &m_strParam );
          (*seq) = SEQ_WAIT_MSG_RETURN;
        }
      }
      break;

    case BTL_ESCAPE_MODE_NG:  // 逃げること自体が禁止されている
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_EscapeCant );
      selact_startMsg( &m_strParam );
      (*seq) = SEQ_WAIT_MSG_RETURN;
      break;

    case BTL_ESCAPE_MODE_CONFIRM: // 逃げると負けになるのを確認させ、確実に逃げられる（通信対戦＆バトル施設）
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_EscapeCheck );
      BTLV_STRPARAM_SetWait( &m_strParam, BTLV_MSGWAIT_NONE );
      selact_startMsg( &m_strParam );
      (*seq) = SEQ_WAIT_MSG_CONFIRM;
    }
  }
  break;

  case SEQ_WAIT_MSG_RETURN:
    if( m_viewCore->CMD_WaitMsg() )
    {
      selActSubProc_Set( &BTL_CLIENT::selact_Root );
    }
    break;

  case SEQ_WAIT_MSG_RETURN_END:
    if( m_viewCore->CMD_WaitMsg() ){
      selActSubProc_Set( &BTL_CLIENT::selact_Root );
    }
    break;

  // 逃げると負けるけどいいですか？
  case SEQ_WAIT_MSG_CONFIRM:
    if( m_viewCore->CMD_WaitMsg_WithoutHide() )
    {
      m_viewCore->CMD_YESNO_Start( true, YESNO_MODE_YESNO );
      (*seq) = SEQ_WAIT_CONFIRM_YESNO;
    }
    break;

  case SEQ_WAIT_CONFIRM_YESNO:
    {
      BtlYesNo result;
      if( m_viewCore->CMD_YESNO_Wait(&result) )
      {
        m_viewCore->CMD_HideMsg();
        if( result == BTL_YESNO_YES )
        {
          (*seq) = SEQ_RETURN_ESCAPE;
        }
        else
        {
          selActSubProc_Set( &BTL_CLIENT::selact_Root );
        }
      }
      // コマンド選択時間切れ対処
      else if( cmdLimit_CheckOver() )
      {
        m_viewCore->CMD_HideMsg();
        m_viewCore->CMD_YESNO_ForceQuit();
        m_viewCore->CMD_YESNO_Delete();
        selActSubProc_Set( &BTL_CLIENT::selact_ForceQuit );
      }
    }
    break;

  case SEQ_RETURN_ESCAPE:
    BTL_ACTION_SetEscapeParam( m_procAction );
    m_fSelActForceFinish = true;
    selActSubProc_Set( &BTL_CLIENT::selact_CheckFinish );
    break;

  }
  return false;
}
//----------------------------------------------------------------------
/**
 *  全アクション選択終了チェック
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_CheckFinish( int* seq )
{
  switch( *seq ){
  case 0:
    m_viewCore->CMD_UI_Restart();
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_UI_WaitRestart() )
    {
      m_procPokeIdx++;

      const u8 myCoverPosNum = this->getMyCoverPosNum();
      if( m_procPokeIdx < myCoverPosNum )
      {
        // まだ担当ポケ数分の選択が済んでいない＆強制終了状態でなければ、再びルートへ
        if( m_fSelActForceFinish == false ){
          BTL_N_Printf( DBGSTR_CLIENT_SelectActionBacktoRoot, m_procPokeIdx );
          selActSubProc_Set( &BTL_CLIENT::selact_Root );
          break;
        }
        // 強制終了状態（「にげる」や「バッグ->ボール」の場合）なら、残りはNULLアクションで埋める
        else
        {
          BTL_POKEPARAM* bpp;
          BTL_ACTION_PARAM* action;

          while( m_procPokeIdx < myCoverPosNum )
          {
            bpp = m_pokeCon->GetClientPokeData( m_myID, m_procPokeIdx );
            action = &(m_actionParam[m_procPokeIdx]);
            if( !checkActionForceSet(bpp, action) ){
              BTL_ACTION_SetNULL( &(m_actionParam[m_procPokeIdx]) );
            }
            m_procPokeIdx++;
          }
        }
      }

      // 担当ポケ数分のアクションがそろった -> 終了へ
      if( m_procPokeIdx >= myCoverPosNum )
      {
        setupSelActReturnData();
        selActSubProc_Set( &BTL_CLIENT::selact_Finish );
      }
    }
    break;
  }

  return false;
}
/**
 * アクション選択終了時、サーバに返信するデータアドレスとサイズを用意
 */
void BTL_CLIENT::setupSelActReturnData( void )
{
  u8 myCoverPosNum = this->getMyCoverPosNum();
  u8 addActionCount = calcAddActionCountSum( myCoverPosNum );
  u8 actionCnt = myCoverPosNum + addActionCount;
  BTL_PRINT("[BTL_CLI]総アクション数 ... %d + %d = %d\n", myCoverPosNum, addActionCount, actionCnt);
  BTL_ACTION_Print( m_actionParam, actionCnt );

  storeActionSelectSendData( m_actionParam, actionCnt );
}

/**
 * @brief アクション配列を、行動選択の返信データに格納する
 * @param actionParamArray  アクション配列
 * @apram actionNum         アクションの数
 */
void BTL_CLIENT::storeActionSelectSendData( const BTL_ACTION_PARAM* actionParamArray, u32 actionNum )
{
  m_returnData_ActionSelect.actionNum = actionNum;

  for( u32 i=0; i<actionNum; ++i )
  {
    m_returnData_ActionSelect.actionParam[i] = actionParamArray[i];
  }

  m_returnDataPtr = &m_returnData_ActionSelect;
  m_returnDataSize = sizeof( SEND_DATA_ACTION_SELECT );
}

//----------------------------------------------------------------------
/**
 *  全アクション選択終了->サーバ返信へ
 */
//----------------------------------------------------------------------
bool BTL_CLIENT::selact_Finish( int* seq )
{
  switch( *seq ){
  case 0:
    selItemWork_Quit();

    if( m_mainModule->GetCommMode() != BTL_COMM_NONE )
    {
      m_commWaitInfoOn = true;
      m_viewCore->CMD_StartCommWait();
      (*seq)++;
    }
    else
    {
      (*seq)+=2;
    }
    break;

  case 1:
    if( m_viewCore->CMD_WaitCommWait() ){
      (*seq)++;
    }
    break;

  case 2:
    cmdLimit_End();
    (*seq)++;
    return true;
    
  default:
    return true;
  }
  return false;
}
/*============================================================================================*/
/* 選択済みアイテム保持用ワーク                                                               */
/*============================================================================================*/
// 初期化
void BTL_CLIENT::selItemWork_Init( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_selItemWork); ++i){
    m_selItemWork[ i ].itemNo = ITEM_DUMMY_DATA;
    m_selItemWork[ i ].bFromPokeSelect = false;
  }
}
// アイテム１つ使う予約
void BTL_CLIENT::selItemWork_Reserve( u8 pokeIdx, u16 itemID, bool bFromPokeSelect )
{
  if( m_bagMode == BBAG_MODE_NORMAL )
  {
    if( m_myID == m_mainModule->GetPlayerClientID() )
    {
      //@バトル変更箇所
      //ビードロは使っても減らないのに登録されていた。
      //そのため使うと1個増えてしまう症状が出た
      if( m_mainModule->DecrementPlayerItem( m_myID, itemID ) )
      {
        m_selItemWork[ pokeIdx ].itemNo = itemID;
        m_selItemWork[ pokeIdx ].bFromPokeSelect = bFromPokeSelect;
      }
    }
  }
}
// ポケモン選択から「かいふくする」を経由してアイテムを使う予定だったか判定
bool BTL_CLIENT::selItemWork_IsFromPokeSelect( u8 pokeIdx ) const
{
  if( m_bagMode == BBAG_MODE_NORMAL )
  {
    if( m_myID == m_mainModule->GetPlayerClientID() )
    {
      if( pokeIdx < GFL_NELEMS(m_selItemWork) )
      {
        return m_selItemWork[ pokeIdx ].bFromPokeSelect;
      }
    }
  }
  return false;
}
// 使う予約していたアイテム情報をキャンセル
void BTL_CLIENT::selItemWork_Restore( u8 pokeIdx )
{
  if( m_bagMode == BBAG_MODE_NORMAL )
  {
    if( m_myID == m_mainModule->GetPlayerClientID() )
    {
      u16 itemID = m_selItemWork[ pokeIdx ].itemNo;
      if( itemID != ITEM_DUMMY_DATA )
      {
        m_selItemWork[ pokeIdx ].itemNo = ITEM_DUMMY_DATA;
        m_selItemWork[ pokeIdx ].bFromPokeSelect = false;
        m_mainModule->AddItem( m_myID, itemID );
      }
    }
  }
}
// 予約情報を全クリア
void BTL_CLIENT::selItemWork_Quit( void )
{
  if( m_bagMode == BBAG_MODE_NORMAL )
  {
    for(u32 i=0; i<GFL_NELEMS(m_selItemWork); ++i)
    {
      selItemWork_Restore( i );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * アクション選択不可状態のポケモンかどうか判定
 * 不可状態であれば、アクション内容を自動生成する
 *
 * @param   wk
 * @param   bpp       判定対象ポケモンパラメータ
 * @param   action    [out] アクション内容生成先
 *
 * @retval  bool      不可状態ならtrue
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::checkActionForceSet( const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action )
{
  // 死んでたらNULLデータを返す
  if( bpp->IsDead() )
  {
    if( action ){
      // 死んでる状態のアクション内容セット
      BTL_ACTION_SetNULL( action );
    }
    return true;
  }
  // アクション選択できない（攻撃の反動など）場合はスキップ
  if( bpp->CONTFLAG_Get(BTL_POKEPARAM::CONTFLG_CANT_ACTION) )
  {
    if( action ){
      BTL_ACTION_SetSkip( action );
    }
    return true;
  }
  // ワザロック状態（記録ワザをそのまま使う）
  if( bpp->CheckSick(pml::wazadata::WAZASICK_WAZALOCK) )
  {
    const BTL_SICKCONT& cont = bpp->GetSickCont( pml::wazadata::WAZASICK_WAZALOCK );
    WazaNo waza = (WazaNo)(SICKCONT_GetParam( cont ));
    BtlPokePos pos = bpp->GetPrevTargetPos();

    GFL_ASSERT(waza!=WAZANO_NULL);  // @check
    GFL_ASSERT(waza<WAZANO_MAX);    // @check

    if( action ){
      BTL_ACTION_SetFightParam( action, waza, pos );
    }
    return true;
  }

  // 溜めロック状態（記録ワザをそのまま使う）
  if( bpp->CheckSick(pml::wazadata::WAZASICK_TAMELOCK) )
  {
    WazaID waza = bpp->GetPrevWazaID( );
    BtlPokePos pos = bpp->GetPrevTargetPos( );

    // PP ゼロでも実行
    if( action ){
      BTL_ACTION_SetFightParam( action, waza, pos );
    }
    return true;
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * ワザ選択不可状態のポケモンかどうか判定
 * ワザ選択不可状態であれば、アクション内容を自動生成する
 *
 * @param   wk
 * @param   bpp         判定対象ポケモンパラメータ
 * @param   fUIMode     UI処理モード（出せるワザが１つでもあればアクション内容生成しない）
 * @param   action      [out] アクション内容生成先
 *
 * @retval  bool      ワザ選択不可状態ならtrue
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::checkWazaForceSet( const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action )
{
  u32 wazaCount = bpp->WAZA_GetCount();
  u32 selectableCount, i;

  // 使えるワザのPPが全て0なら「わるあがき」
  selectableCount = 0;
  for(i=0; i<wazaCount; ++i)
  {
    if( is_unselectable_waza(bpp, bpp->WAZA_GetID(i), false, NULL) ){
      continue;
    }
    if( bpp->WAZA_GetPP(i) == 0 ){
      continue;
    }
    ++selectableCount;
  }

  if( selectableCount == 0 )
  {
    setWaruagakiAction( action, bpp );
    return true;
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * 「わるあがき」用アクションパラメータセット
 *
 * @param   dst
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void BTL_CLIENT::setWaruagakiAction( BTL_ACTION_PARAM* dst, const BTL_POKEPARAM* bpp )
{
  RandContext  rc;

  BtlPokePos targetPos = calc::DecideWazaTargetAutoForClient( m_mainModule, m_pokeCon, bpp, WAZANO_WARUAGAKI, &rc );
  BTL_ACTION_SetFightParam( dst, WAZANO_WARUAGAKI, targetPos );
}
//----------------------------------------------------------------------------------
/**
 * 使用できないワザが選ばれたかどうか判定
 *
 * @param   wk
 * @param   bpp             対象ポケモン
 * @param   waza            ワザ
 * @param   isZenryokuWaza  全力技として使用する場合 true
 * @param   strParam        [out]警告メッセージパラメータ出力先
 *
 * @retval  bool    使用できないワザの場合true
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::is_unselectable_waza( const BTL_POKEPARAM* bpp, WazaID waza, bool isZenryokuWaza, BTLV_STRPARAM* strParam ) const
{
  //わるあがきが選ばれているなら無条件で繰り出す
  if( waza == WAZANO_WARUAGAKI )
  {
    return false;
  }
  // こだわりアイテム効果（最初に使ったワザしか選べない／ただしマジックルーム非発動時のみ）
  u16 effectiveItem = bpp->GetItemEffective( *m_fldSim );
  if( effectiveItem != ITEM_DUMMY_DATA )
  {
    if( bpp->CheckSick(pml::wazadata::WAZASICK_KODAWARI) )
    {
      const BTL_SICKCONT& cont = bpp->GetSickCont( pml::wazadata::WAZASICK_KODAWARI );
      WazaNo  kodawariWaza = (WazaNo)(SICKCONT_GetParam( cont ));

      // こだわり対象のワザを覚えていて、それ以外のワザを使おうとしたらダメ
      if( (bpp->WAZA_IsUsable(kodawariWaza))
      &&  (kodawariWaza != waza )
      ){
        GFL_ASSERT(kodawariWaza != WAZANO_NULL); // @check
        GFL_ASSERT(kodawariWaza < WAZANO_MAX);   // @check
        if( strParam != NULL )
        {
          BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_KodawariLock );
          BTLV_STRPARAM_AddArg( strParam, effectiveItem );
          BTLV_STRPARAM_AddArg( strParam, kodawariWaza );
        }
        return true;
      }
    }

    // とつげきチョッキを装備していて、ダメージワザ以外を使ったらダメ
    if( (effectiveItem == ITEM_TOTUGEKITYOKKI)
    &&  (!WAZADATA_IsDamage(waza))
    ){
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_TotsugekiLock );
        BTLV_STRPARAM_AddArg( strParam, effectiveItem );
      }
      return true;
    }
  }

  // アンコール効果（前回と同じワザしか出せない）
  if( !isZenryokuWaza &&
       bpp->CheckSick(pml::wazadata::WAZASICK_ENCORE) )
  {
    WazaNo prevWaza = (WazaNo)(SICKCONT_GetParam( bpp->GetSickCont(pml::wazadata::WAZASICK_ENCORE) ));
    if( waza != prevWaza ){
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WazaLock );
        BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
        BTLV_STRPARAM_AddArg( strParam, prevWaza );
      }
      return true;
    }
  }

  // ちょうはつ状態（ダメージワザしか選べない）
  if( !isZenryokuWaza &&
       bpp->CheckSick(pml::wazadata::WAZASICK_TYOUHATSU) )
  {
    if( !WAZADATA_IsDamage(waza) )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_ChouhatuWarn );
        BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return true;
    }
  }

  // いちゃもん状態（２ターン続けて同じワザを選べない）
  if( !isZenryokuWaza &&
       bpp->CheckSick(pml::wazadata::WAZASICK_ICHAMON) )
  {
    if( !( bpp->IsPrevWazaZenryoku() ) &&          // @fix GFNMcat[29]Ｚ技を使用した次のターン、元になった攻撃技が「いちゃもん」で出せない
         ( bpp->GetPrevOrgWazaID() == waza ) )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_IchamonWarn );
        BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return true;
    }
  }

  // かなしばり状態（かなしばり直前に出していたワザを選べない）
  if( !isZenryokuWaza &&
       bpp->CheckSick(pml::wazadata::WAZASICK_KANASIBARI) )
  {
    u16 kanasibariWaza = bpp->GetSickParam( pml::wazadata::WAZASICK_KANASIBARI );
    BTL_Printf("かなしばりチェック waza=%d\n", kanasibariWaza);
    if( (waza == kanasibariWaza) && (waza != WAZANO_WARUAGAKI) )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_KanasibariWarn );
        BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return true;
    }
  }

  // かいふくふうじ状態（回復ワザを選べない）
  if( !isZenryokuWaza &&
       bpp->CheckSick(pml::wazadata::WAZASICK_KAIHUKUHUUJI) )
  {
    if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_KaifukuHuuji) )
    {
      if( strParam )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_KaifukuFujiWarn );
        BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return true;
    }
  }

  // ふういんチェック（ふういんをかけたポケが持ってるワザを出せない）
  if( !isZenryokuWaza &&
       m_fldSim->CheckEffect(FieldStatus::EFF_FUIN) )
  {
    if( m_fldSim->CheckFuin(*m_mainModule, m_pokeCon, bpp, waza) )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_FuuinWarn );
        BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return true;
    }
  }

  // じゅうりょくチェック
  if( !isZenryokuWaza &&
       m_fldSim->CheckEffect(FieldStatus::EFF_JURYOKU) )
  {
    if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Flying) )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_JyuryokuWazaFail );
        BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return true;
    }
  }

  // ワザ「ゲップ」は、きのみを食べた後でないと出せない
  if( (waza == WAZANO_GEPPU)
  &&  (!(bpp->PERMFLAG_Get(BTL_POKEPARAM::PERMFLAG_ATE_KINOMI)))
  ){
    if( strParam != NULL )
    {
      BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_GeppuFail );
      BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
    }
    return true;
  }

  // スカイバトルで出せない
  if( m_mainModule->IsSkyBattle() )
  {
    if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_SkyBattleFail) )
    {
      if( strParam ){
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_SkyBattleFail );
      }
      return true;
    }
  }

  // じごくづき( 音技を出せない )
  if( !isZenryokuWaza &&
       bpp->CheckSick( pml::wazadata::WAZASICK_ZIGOKUDUKI ) &&
       WAZADATA_GetFlag( waza, pml::wazadata::WAZAFLAG_Sound ) )
  {
    if( strParam != NULL )
    {
      BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_Zigokuduki_Warn );
      BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
    }
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------
/**
 * 使用できないワザ対象が選ばれたかどうか判定
 *
 * @param   wk
 * @param   bpp             ワザを使おうとしているポケモン
 * @param   waza            ワザ
 * @param   targetPos       ワザの対象ポケモン位置
 * @param   isZenryokuWaza  全力技として使用する場合 true
 * @param   strParam        [out]警告メッセージパラメータ出力先
 *
 * @retval  bool    使用できないワザの場合true
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::is_unselectable_target( const BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos, bool isZenryokuWaza, BTLV_STRPARAM* strParam ) const
{
  //わるあがきが選ばれているなら無条件で繰り出す
  if( waza == WAZANO_WARUAGAKI ){
    return false;
  }

  // かいふくふうじ状態で「かふんだんご」を味方に使うのはNG
  if( !isZenryokuWaza 
  &&  (waza == WAZANO_KAHUNDANGO)
  &&  bpp->CheckSick(pml::wazadata::WAZASICK_KAIHUKUHUUJI)
  ){
    BtlPokePos  procPokePos = m_mainModule->PokeIDtoPokePos( m_pokeCon, bpp->GetID() );
    if( m_mainModule->IsFriendPokePos(procPokePos, targetPos) )
    {
      if( strParam )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_KaifukuFujiWarn );
        BTLV_STRPARAM_AddArg( strParam, bpp->GetID() );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return true;
    }
  }


  return false;
}


//----------------------------------------------------------------------------------
/**
 * 選択できるワザのIndexを全て列挙
 *
 * @param   wk
 * @param   bpp
 * @param   dst   [out] u8 x 4 の配列。選択できるワザなら対応Indexが1, それ以外は0になる
 *
 * @retval  u8    選択できる最初のワザIndex。選択できるワザがない場合、PTL_WAZA_MAX
 */
//----------------------------------------------------------------------------------
u8 BTL_CLIENT::storeSelectableWazaFlag( const BTL_POKEPARAM* bpp, u8* dst ) const
{
  u8 wazaCount = bpp->WAZA_GetCount( );
  u8 firstIdx = PTL_WAZA_MAX;
  u8 i;

  for(i=0; i<wazaCount; ++i )
  {
    if( bpp->WAZA_GetPP(i) )
    {
      WazaID  waza = bpp->WAZA_GetID( i );
      dst[i] = !is_unselectable_waza( bpp, waza, false, NULL );
      if( (firstIdx==PTL_WAZA_MAX) && (dst[i]) ){
        firstIdx = i;
      }
    }
    else{
      dst[i] = 0;
    }
  }
  for( ; i<PTL_WAZA_MAX; ++i ){
    dst[i] = 0;
  }
  return firstIdx;
}


//---------------------------------------------------------------------------
/**
 * @brief 交換禁止チェック
 *
 * @param      procPoke   逃げ・交換が可能かどうかのチェック対象ポケモン
 * @param[out] pokeID     交換できない原因となっているポケモンのID
 * @param[out] tokuseiID  交換できない原因が相手とくせいの場合、そのとくせいID（それ以外TOKUSEI_NULL）
 *
 * @return 逃げ・交換が出来ない原因
 * @retval CANTESC_NULL  逃げ・交換が可能な場合
 */
//---------------------------------------------------------------------------
BTL_CLIENT::CantEscapeCode BTL_CLIENT::isForbidPokeChange( const BTL_POKEPARAM* procPoke, u8* pokeID, u16* tokuseiID ) const
{
  // 「きれいなぬけがら」を持っていたら確実にOK
  if( procPoke->GetItemEffective(*m_fldSim) == ITEM_KIREINANUKEGARA ){
    return CANTESC_NULL;
  }

  // 逃げ・交換禁止チェック共通部分
  {
    CantEscapeCode code = checkForbidChangeEscapeCommon( procPoke, pokeID, tokuseiID );
    if( code != CANTESC_NULL ){
      return code;
    }
  }

  *pokeID = BTL_POKEID_NULL;
  *tokuseiID = TOKUSEI_NULL;
  return CANTESC_NULL;
}
//----------------------------------------------------------------------------------
/**
 * 逃げ禁止チェック
 *
 * @param   wk
 * @param   procPoke
 * @param   pokeID      [out] 逃げ・交換できない原因となるポケID（敵・味方ともあり得る）
 * @param   tokuseiID   [out] 逃げ・交換できない原因がとくせいの場合、そのとくせいID（それ以外TOKUSEI_NULL）
 *
 * @retval  CantEscapeCode
 */
//----------------------------------------------------------------------------------
BTL_CLIENT::CantEscapeCode BTL_CLIENT::isForbidEscape( u8* pokeID, u16* tokuseiID )
{
  const BTL_POKEPARAM* procPoke;
  u32 i;

  #if  PM_DEBUG
  if( ui::CheckKeyCont(BUTTON_L|BUTTON_R) ){
    return CANTESC_NULL;
  }
  #endif


  u8 myCoverPosNum = this->getMyCoverPosNum();

  for(i=0; i<myCoverPosNum; ++i)
  {
    procPoke = m_myParty->GetMemberDataConst( i );
    if( procPoke->IsDead() ){
      continue;
    }

    // 「けむりだま」を持っていたら確実にOK
    if( procPoke->GetItemEffective(*m_fldSim) == ITEM_KEMURIDAMA ){
      return CANTESC_NULL;
    }
    // とくせい「にげあし」で確実にOK
    if( procPoke->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_NIGEASI ){
      return CANTESC_NULL;
    }
  }
  for(i=0; i<myCoverPosNum; ++i)
  {
    procPoke = m_myParty->GetMemberDataConst( i );

    // 逃げ・交換禁止チェック共通部分
    {
      CantEscapeCode code = checkForbidChangeEscapeCommon( procPoke, pokeID, tokuseiID );
      if( code != CANTESC_NULL ){
        return code;
      }
    }
  }

  return CANTESC_NULL;
}

/**
 * @brief 逃げ・交換禁止チェック共通部分
 * @param      procPoke   逃げ・交換が可能かどうかのチェック対象ポケモン
 * @param[out] pokeID     交換できない原因となっているポケモンのID
 * @param[out] tokuseiID  交換できない原因が相手とくせいの場合、そのとくせいID（それ以外TOKUSEI_NULL）
 * @return 逃げ・交換が出来ない原因
 * @retval CANTESC_NULL  逃げ・交換が可能な場合
 */
BTL_CLIENT::CantEscapeCode  BTL_CLIENT::checkForbidChangeEscapeCommon( const BTL_POKEPARAM* procPoke, u8* pokeID, u16* tokuseiID ) const
{
  BtlExPos    exPos;
  u16         checkTokusei;
  BtlPokePos  myPos;
  u8          procPokeID;
  u8          checkPokeCnt;
  u32         i;
  const BTL_POKEPARAM* bpp;
  u8 pokeIDAry[ BTL_POSIDX_MAX ];



  // XYより：ゴーストタイプは必ず逃げられる
  if( procPoke->IsMatchType(POKETYPE_GHOST) ){
    return CANTESC_NULL;
  }

  // XYより：フェアリーロックがかかっていると無条件に逃げられない
  if( m_fldSim->CheckRemainingTurn(FieldStatus::EFF_FAIRY_LOCK) == 1){
    return CANTESC_FAIRY_LOCK;
  }

  procPokeID = procPoke->GetID();
  myPos = m_mainModule->PokeIDtoPokePos( m_pokeCon, procPokeID );
  exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );


  checkPokeCnt = m_mainModule->ExpandExistPokeID( m_pokeCon, exPos, pokeIDAry );
  for(i=0; i<checkPokeCnt; ++i)
  {
    // 相手側とくせい「かげふみ」チェック
    bpp = m_pokeCon->GetPokeParamConst( pokeIDAry[i] );
    checkTokusei = bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE );

    if( checkTokusei == TOKUSEI_KAGEHUMI )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Kagefumi_Chk, bpp->GetID() );
      if( checkForbitEscapeEffective_Kagefumi(procPoke) ){
        BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Kagefumi_Enable );
        *pokeID = bpp->GetID();
        *tokuseiID = checkTokusei;
        return CANTESC_KAGEFUMI;
      }
    }
    // 相手側とくせい「ありじごく」チェック
    if( checkTokusei == TOKUSEI_ARIZIGOKU )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Arijigoku_Chk, bpp->GetID() );
      if( checkForbitEscapeEffective_Arijigoku(procPoke) ){
        BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Arijigoku_Enable );
        *pokeID = bpp->GetID();
        *tokuseiID = checkTokusei;
        return CANTESC_KAGEFUMI;
      }
    }
    // 相手側とくせい「じりょく」チェック
    if( checkTokusei == TOKUSEI_ZIRYOKU )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Jiryoku_Chk, bpp->GetID() );
      if( checkForbitEscapeEffective_Jiryoku(procPoke) ){
        BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Jiryoku_Enable );
        *pokeID = bpp->GetID( );
        *tokuseiID = checkTokusei;
        return CANTESC_KAGEFUMI;
      }
    }
  }

  // こちらのポケモン状態異常による逃げ・交換禁止チェック
  if( procPoke->CheckSick( pml::wazadata::WAZASICK_TOOSENBOU )
  ||  procPoke->CheckSick( pml::wazadata::WAZASICK_BIND )
  ||  procPoke->CheckSick( pml::wazadata::WAZASICK_NEWOHARU )
  ){
     *pokeID = procPoke->GetID( );
     *tokuseiID = TOKUSEI_NULL;
     return CANTESC_TOOSENBOU;
  }

  return CANTESC_NULL;
}
/**
 *  逃げ・交換禁止チェック：自分に「かげふみ」が効くか
 */
bool BTL_CLIENT::checkForbitEscapeEffective_Kagefumi( const BTL_POKEPARAM* procPoke ) const
{
  if( procPoke->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_KAGEHUMI ){
    return false;
  }
  return true;
}
/**
 *  逃げ・交換禁止チェック：自分に「ありじごく」が効くか
 */
bool BTL_CLIENT::checkForbitEscapeEffective_Arijigoku(  const BTL_POKEPARAM* procPoke ) const
{
  u16 effectiveItem = procPoke->GetItemEffective( *m_fldSim );

  if( m_fldSim->CheckEffect(FieldStatus::EFF_JURYOKU) ){
    return true;
  }
  if( procPoke->CheckSick(pml::wazadata::WAZASICK_FLYING_CANCEL) ){
    return true;
  }
  if( procPoke->CheckSick(pml::wazadata::WAZASICK_NEWOHARU) ){
    return true;
  }
  if( effectiveItem == ITEM_KUROITEKKYUU ){
    return true;
  }

  if( procPoke->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_HUYUU ){
    return false;
  }
  if( procPoke->IsMatchType(POKETYPE_HIKOU) ){
    return false;
  }
  if( procPoke->CheckSick(pml::wazadata::WAZASICK_FLYING) ){
    return false;
  }
  if( procPoke->CheckSick(pml::wazadata::WAZASICK_TELEKINESIS) ){
    return false;
  }
  if( effectiveItem == ITEM_HUUSEN ){
    return false;
  }

  return true;
}
/**
 *  逃げ・交換禁止チェック：自分に「じりょく」が効くか
 */
bool BTL_CLIENT::checkForbitEscapeEffective_Jiryoku( const BTL_POKEPARAM* procPoke ) const
{
  if( procPoke->IsMatchType(POKETYPE_HAGANE) ){
    return true;
  }
  return false;
}

/**
 * AI 入れ替え判定用ワーク初期化
 *
 * @param   wk
 *
 * @retval  bool
 */
void BTL_CLIENT::changeAI_InitWork( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_AIChangeIndex); ++i){
    m_AIChangeIndex[ i ] = -1;
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief AI 入れ替え判定用ワーク：すでに入れ替えることが決定しているメンバーかどうか判定
 * @param pokeIndexWithinParty  チェック対象ポケモンのパーティ内インデックス
 * @return  すでに入れ替え決定しているなら true
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::changeAI_CheckReserve( u8 pokeIndexWithinParty ) const
{
  for(u32 i=0; i<GFL_NELEMS(m_AIChangeIndex); ++i)
  {
    if( m_AIChangeIndex[i] == pokeIndexWithinParty ){
      return true;
    }
  }

  return false;
}
/**
 * AI 入れ替え判定用ワーク：入れ替えることが決定したメンバーを記憶する
 * @param outPokeIdx
 * @param inPokeIdx
 */
void BTL_CLIENT::changeAI_SetReserve( u8 outPokeIdx, u8 inPokeIdx )
{
  if( outPokeIdx < GFL_NELEMS(m_AIChangeIndex) )
  {
    m_AIChangeIndex[ outPokeIdx ] = inPokeIdx;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 入れ替え可能な控えポケモンが存在するか？
 * @retval true   存在する
 * @retval false  存在しない
 */
//-----------------------------------------------------------------------------
bool BTL_CLIENT::IsPuttablePokemonExist( void ) const
{
  u8 myCoverPosNum = this->getMyCoverPosNum();
  return ( myCoverPosNum < m_myParty->GetAliveMemberCount() );
}

/**
 *  交換チェック時に考慮する「敵」ポケモンの決定
 */
BTL_POKEPARAM* BTL_CLIENT::changeAI_SupposeEnemy( BtlPokePos basePos )
{
  BtlExPos    exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, basePos );
  u8 aryPokeID[ BTL_POSIDX_MAX ];
  u8 cnt;

  cnt = m_mainModule->ExpandExistPokeID( m_pokeCon, exPos, aryPokeID );
  if( cnt )
  {
    u8 idx = m_AIRand.Next( cnt );
    return m_pokeCon->GetPokeParam( aryPokeID[idx] );
  }

  return NULL;
}

/**
 * メガ進化可能なため入れ替え・交換対象から除外するかどうか判定
 * @return 除外する場合は true
 */
bool BTL_CLIENT::changeAI_IsOmitMegaEvolvable( const BTL_POKEPARAM*  bpp )
{
  // シナリオトレーナー戦以外はチェックしない
  if( m_mainModule->GetCompetitor() != BTL_COMPETITOR_TRAINER ){
    return false;
  }

  return bpp->IsAbleToMegaEvo();
}

/**
* momiji限定対応？
* 特定伝説ポケモンのため入れ替え・交換対象から除外するかどうか判定
* @return 除外する場合は true
*/
bool BTL_CLIENT::changeAI_IsSpecialMons(const BTL_POKEPARAM*  bpp)
{
  // シナリオトレーナー戦以外はチェックしない
  if (m_mainModule->GetCompetitor() != BTL_COMPETITOR_TRAINER){
    return false;
  }

  static const u16 SPECIAL_MONS_LIST[] = {
    MONSNO_MYUUTUU,
    MONSNO_KAIOOGA,
    MONSNO_GURAADON,
    MONSNO_DHIARUGA,
    MONSNO_PARUKIA,
    MONSNO_ZEKUROMU,
    MONSNO_RESIRAMU,
    MONSNO_ZERUNEASU,
    MONSNO_IBERUTARU,
  };

  for (u32 i = 0; i < GFL_NELEMS(SPECIAL_MONS_LIST); ++i)
  {
    if (SPECIAL_MONS_LIST[i] == bpp->GetMonsNo())
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief サブプロセス( アクション選択：AI )
 * @retval true   サブプロセスが終了した
 * @retval false  サブプロセスが継続中
 */
bool BTL_CLIENT::SubProc_AI_SelectAction( int* seq )
{
  enum {
    SEQ_INIT,      // 初期化
    SEQ_AI_START,  // AI開始
    SEQ_AI_WAIT,   // AI終了待ち
    SEQ_NEXT_POKE, // 次のポケモンへ
  };

  GFL_ASSERT_STOP( m_ai );

  #if PM_DEBUG
  if( m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_AI_CTRL) )
  {
    if( debugAICtrl_IsMyFase() ){
      return SubProc_UI_SelectAction( seq );
    }
    return false;
  }
  #endif

  switch( *seq )
  {
  case SEQ_INIT:
    changeAI_InitWork();
    BattleAiSystem::SetCommonRand( m_AIRand.Next( BattleAiSystem::BASIC_RAND_RANGE ) );
    m_procPokeIdx = 0;
    m_aiZenryokuSelected = false;
    selact_ClearWorks();
    (*seq) = SEQ_AI_START;
    /* fallthru */


  case SEQ_AI_START:
    if( m_procPokeIdx < this->getMyCoverPosNum() )
    {
      setupCurrentPokeActionPtr();

      if( m_procPoke->IsDead() )
      {
        BTL_ACTION_SetNULL( m_procAction );
        (*seq) = SEQ_NEXT_POKE;
        break;
      }
      else
      {
        // 行動選択できないチェック
        if( checkActionForceSet(m_procPoke, m_procAction) ){
          (*seq) = SEQ_NEXT_POKE;
          break;
        }

        BattleAi::StartParam aiStartParam;
        aiStartParam.pokeId = m_procPoke->GetID();
        aiStartParam.itemId = this->AIItem_CheckUse( m_procPoke, m_myParty );
        aiStartParam.canUseZenryokuWaza = this->AI_CheckZWazaEnable();
        storeSelectableWazaFlag( m_procPoke, aiStartParam.selectableWazaFlags );
        m_ai->StartActionSelect( m_heapForAiScriptExec, aiStartParam );

        (*seq) = SEQ_AI_WAIT;
      }
    }
    break;


  case SEQ_AI_WAIT:
    m_ai->UpdateActionSelect();
    if( m_ai->IsActionSelectFinished() ) 
    {
      const BattleAi::Result& aiResult = m_ai->GetResult();
      m_procAction = this->buildActionParamFromAiResult( m_procAction, &m_bSetMegaEvoThisTurn, m_procPokeIdx, m_procPoke, m_myParty, aiResult );

      if( aiResult.action == BattleAi::AIACT_POKECHANGE ) {
        this->changeAI_SetReserve( m_procPokeIdx, aiResult.changePokeIndex );
      }

      if( ( aiResult.action == BattleAi::AIACT_WAZA ) &&
          ( aiResult.wazaZenryokuFlag ) ) {
        m_aiZenryokuSelected = true;
      }

      (*seq) = SEQ_NEXT_POKE;
    }
    break;


  case SEQ_NEXT_POKE:
    m_procPokeIdx++;
    {
      const u8 myCoverPosNum = this->getMyCoverPosNum();
      if( m_procPokeIdx >= myCoverPosNum )
      {
        u32 addActionCount = calcAddActionCountSum( myCoverPosNum );
        u32 actionCount = myCoverPosNum + addActionCount;
        /*
        u32 dataSize = (actionCount * sizeof(m_actionParam[0]));
        BTL_PRINT("[Client] AI Client(%d) のアクション数 = %d + %d = %d\n", m_myID, myCoverPosNum, addActionCount, actionCount );
        */

        storeActionSelectSendData( m_actionParam, actionCount );
        return true;
      }
      else
      {
        (*seq) = SEQ_AI_START;
      }
    }
    break;

  }
  return false;
}

/**
 * @brief AIがZ技を使用可能か？
 * @retval true   使用可能
 * @retval false  使用できない
 */
bool BTL_CLIENT::AI_CheckZWazaEnable( void ) const
{
  // 現バトルにおいて、すでに使用済み
  if( m_zenryokuWazaStatus->IsZenryokuWazaUsed() )
  {
    return false;
  }

  // 現ターンにおいて、すでに選択済み
  if( m_aiZenryokuSelected )
  {
    return false;
  }
  
  // 模擬戦なら、ゼンリョクリングを持っていたなら、使用可能
  if(  m_mainModule->IsRecordFightMode() &&
      !m_mainModule->IsRecordFightWithNPCData() )
  {
    const Savedata::MyStatus* myStatus = m_mainModule->GetClientPlayerData( m_myID );
    return myStatus->HasZenryokuRing();
  }

  // ゲーム内トレーナー or バトル施設トレーナーなら、使用可能
  BtlCompetitor competitor = m_mainModule->GetCompetitor();
  return ( ( competitor == BTL_COMPETITOR_TRAINER ) ||
           ( competitor == BTL_COMPETITOR_INST ) );
}


/**
 * @brief AIの結果から、アクションパラメータを構築する
 * @param[out]    actionParam        構築するアクションパラメータ
 * @param[in,out] isMegaEvoThisTurn  現在のターンでメガ進化したか？
 * @param         actPokeIndex       行動するポケモンのインデックス
 * @param         actPoke            行動するポケモン
 * @param         actPokeParty       行動するポケモンが所属するパーティ
 * @param         aiResult           AIの結果
 *
 * @return 構築後のアクションパラメータ
 */
BTL_ACTION_PARAM* BTL_CLIENT::buildActionParamFromAiResult( 
  BTL_ACTION_PARAM*       actionParam, 
  bool*                   isMegaEvoThisTurn,
  u8                      actPokeIndex,
  const BTL_POKEPARAM*    actPoke,
  const BTL_PARTY*        actPokeParty,
  const BattleAi::Result& aiResult )
{
  switch( aiResult.action )
  {
  case BattleAi::AIACT_WAZA:
    {
      // ワザを選択した場合、メガ進化できるなら必ずさせる
      if( checkAiMegaEvoEnable( *actPoke, *isMegaEvoThisTurn ) )
      {
        int idx = actPokeParty->FindMember( actPoke );  // ローテ後だと procPoke , procPokeIdx は必ずしも一致しないのでパーティデータから位置検索
        if( idx >= 0 ) {
          actionParam = this->insertActMegaEvoParam( actionParam, idx );
          *isMegaEvoThisTurn = true;
        }
      }
      // ワザを選択した場合、ウルトラバーストできるなら必ずさせる
      else if (checkAiUltraBurstEnable(*actPoke))
      {
        int idx = actPokeParty->FindMember(actPoke);  // ローテ後だと procPoke , procPokeIdx は必ずしも一致しないのでパーティデータから位置検索
        if (idx >= 0) {
          actionParam = this->insertActMegaEvoParam(actionParam, idx);
          m_bSetUltraBurstThisTurn = true;
        }
      }
      WazaID  waza = actPoke->WAZA_GetID( aiResult.wazaIndex );
      BTL_ACTION_SetFightParam( actionParam, waza, aiResult.wazaTargetPos, aiResult.wazaZenryokuFlag );
    }
    break;

  case BattleAi::AIACT_ITEM:
    {
      u8 shortPokeID = this->calcShortPokeID( actPokeIndex );
      BTL_ACTION_SetItemParam( actionParam, aiResult.itemId, shortPokeID, 0 );
    }
    break;

  case BattleAi::AIACT_POKECHANGE:
    BTL_ACTION_SetChangeParam( actionParam, actPokeIndex, aiResult.changePokeIndex );
    break;

  case BattleAi::AIACT_ESCAPE:
    BTL_ACTION_SetEscapeParam( actionParam );
    break;

  case BattleAi::AIACT_WARUAGAKI:
    this->setWaruagakiAction( actionParam, actPoke );
    break;

  default:
    GFL_ASSERT(0);
    BTL_ACTION_SetSkip( actionParam );
    break;
  }

  return actionParam;
}

/**
 * @brief AIがメガシンカ可能かどうかチェックする
 * @param poke               チェック対象ポケモン
 * @param isMegaEvoThisTurn  現在のターンでメガ進化したか？
 * @retval true   メガシンカ出来る
 * @retval false  メガシンカ出来ない
 */
bool BTL_CLIENT::checkAiMegaEvoEnable( const BTL_POKEPARAM& poke, bool isMegaEvoThisTurn ) const
{
  // 既にメガシンカしているなら、メガシンカできない
  if( s_IsMegaEvolved( m_myID ) || isMegaEvoThisTurn )
  {
    return false;
  }

  // メガシンカ出来るポケモンじゃない
  if( !poke.IsAbleToMegaEvo() )
  {
    return false;
  }

  // 模擬戦では、メガリングを所持しているなら、メガシンカできる
  if( m_mainModule->IsRecordFightMode() &&
     !m_mainModule->IsRecordFightWithNPCData() )
  {
    const Savedata::MyStatus* myStatus = m_mainModule->GetClientPlayerData( m_myID );
    return myStatus->HasMegaRing();
  }

  // 野生戦以外なら、メガシンカできる
  if( m_mainModule->GetCompetitor() != BTL_COMPETITOR_WILD )
  {
    return true;
  }
          
  return false;
}

/**
* @brief AIがウルトラバースト可能かどうかチェックする
* @param poke               チェック対象ポケモン
* @param isMegaEvoThisTurn  現在のターンでウルトラバーストしたか？
* @retval true   ウルトラバースト出来る
* @retval false  ウルトラバースト出来ない
*/
bool BTL_CLIENT::checkAiUltraBurstEnable(const BTL_POKEPARAM& poke) const
{
  // 既にウルトラバーストしているならできない
  if (s_IsUltraBursted(m_myID) || m_bSetUltraBurstThisTurn)
  {
    return false;
  }

  // ウルトラバースト出来るポケモンじゃない
  if (!poke.IsAbleToUltraBurst())
  {
    return false;
  }

  // 野生戦以外なら、ウルトラバーストできる
  if (m_mainModule->GetCompetitor() != BTL_COMPETITOR_WILD)
  {
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief 死亡時の選択可能ポケモン( 場に出ているポケモン以外で、生きているポケモン )をリストアップする
 *
 * @param[out] list 選択可能ポケモンのパーティ内インデックスを格納する配列( メガシンカ可能なポケモンは末尾になる )
 *
 * @return 選択可能ポケモンの数
 */
//--------------------------------------------------------------------------
u8 BTL_CLIENT::countPuttablePokemons( u8* list ) const
{
  u8 myCoverPosNum = this->getMyCoverPosNum();
  u8 numFrontPos = calc::RULE_HandPokeIndex( m_mainModule->GetRule(), myCoverPosNum );
  u8 cnt;

  cnt = countPuttablePokemons_sub( list,   0, numFrontPos, true );
  cnt = countPuttablePokemons_sub( list, cnt, numFrontPos, false );

  return cnt;
}
/**
 * 控えメンバーから選択可能ポケモンのパーティ内Indexをリストアップする
 *
 * @param[out]  list        選択可能ポケモンのパーティ内インデックスを格納する配列
 * @param       startIdx     list の格納Index開始値
 * @param       numFrontPos 自分が担当する「場」の数
 * @param       bSkipMegaEvolbable  true:メガシンカ可能なポケモンをスキップ / false:メガシンカ可能なポケモンのみ対象とする
 *
 * @return      u8    list に格納されている有効なIndexの数
 */
u8 BTL_CLIENT::countPuttablePokemons_sub( u8* list, u8 startIndex, u8 numFrontPos, bool bSkipMegaEvolvable ) const
{
  u8 idx = startIndex;
  u8 numMembers = m_myParty->GetMemberCount();

  for(u32 i=numFrontPos; i<numMembers; ++i)
  {
    const BTL_POKEPARAM* bpp = m_myParty->GetMemberDataConst( i );
    if( !bpp->IsFightEnable() ){
      continue;
    }

    bool bAbleToMegaEvo = bpp->IsAbleToMegaEvo();
    if( bSkipMegaEvolvable && bAbleToMegaEvo ){
      continue;
    }
    if( !bSkipMegaEvolvable && !bAbleToMegaEvo ){
      continue;
    }

    if( list ){
      list[idx] = i;
    }
    ++idx;
  }

  return idx;
}

//----------------------------------------------------------------------------------
/**
 * countPuttablePokemonsで生成した list を出すべき順に並べ替え（相手との相性による）
 * ※シナリオトレーナー戦の時、メガシンカ可能なポケモンは末尾になる
 * ※シナリオトレーナー戦の時、特定の伝説ポケモンは末尾になる
 *
 * @param[io]   list                 場に出すことが可能なポケモンiD配列
 * @param        numPoke
 * @param        target               場に出た時に対峙することになる想定ポケモンパラメータ（相性計算に使う：不要ならNULL）
 * @param        posForCheckEffect    位置エフェクト効果を調べる場合は位置ID指定（BTL_POS_NULL指定で調べない）
 *
 */
//----------------------------------------------------------------------------------
void BTL_CLIENT::sortPuttablePokemonList( u8* list, u8 numPoke, const BTL_POKEPARAM* target, BtlPokePos posForCheckEffect )
{
  int affVal[ BTL_PARTY_MEMBER_MAX ];
  const BTL_POKEPARAM* bpp;

  BTL_PRINT("[cli] %d 件の配列を相性順に並べ替えます\n", numPoke);


  for(u32 i=0; i<numPoke; ++i)
  {
    affVal[i] = 0;
    bpp = m_myParty->GetMemberDataConst( list[i] );

    // この時点で死んでることは無いはずだが一応、末尾に送る
    if( bpp->IsDead() ){
      affVal[i] = -1;
      continue;
    }

    // メガシンカ可能なら末尾になるように負の値を入れる
    if( this->changeAI_IsOmitMegaEvolvable(bpp) ){
      affVal[i] = -1;
      continue;
    }

    // 特定の伝説ポケモンなら末尾になるように負の値を入れる
    if( this->changeAI_IsSpecialMons(bpp) ){
      affVal[i] = -1;
      continue;
    }

    // 対峙する相手が想定されている場合、ワザダメージ計算して最も得点の高いものを記録
    if( target )
    {
      u8 wazaCnt = bpp->WAZA_GetCount();
      for(u32 j=0; j<wazaCnt; ++j)
      {
        if( bpp->WAZA_GetPP(j) )
        {
          WazaID waza = bpp->WAZA_GetID( j );
          if( WAZADATA_IsDamage(waza) )
          {
            pml::PokeType wazaType = WAZADATA_GetType( waza );
            u32 pow = WAZADATA_GetPower( waza );
            if( pow < 10 ){ // 10未満は特殊設定として大体60くらいに…
              pow = 60;
            }
            // ダメージ計算と同じ方法で倍率も計算する（フライングプレスは「かくとう」のみになってしまうが）
            BtlTypeAff  aff = calc::TypeAffPair( wazaType, target->GetPokeType() );
            pow = calc::AffDamage( pow, aff );
            if( affVal[i] < static_cast<int>(pow) ){
              affVal[ i ] = pow;
              BTL_PRINT("%d:ポケ(ID:%d)の相性値を %d に設定\n", i, bpp->GetID(), affVal[i]);
            }
          }
        }
      }
    } // if( target )

    // 場に出て有利な効果が効いている場合HPの低いポケモンを優先
    if( posForCheckEffect != BTL_POS_NULL && isExistHPRecoverEffect(posForCheckEffect) )
    {
      fx32 hp_ratio = bpp->GetHPRatio();
      BTL_PRINT("  ratio:%08x, check:%08x\n", hp_ratio, FX32_CONST(66.67f));
      if( hp_ratio <= FX32_CONST(66.67f) ){
        BTL_PRINT("  ポケ(ID:%d) の体力は2/3以下である\n", bpp->GetID());
        fx32    diff  = FX32_CONST(100) - hp_ratio;
        double  value = FXtoFloat( diff ) + 2000; // ダメージ計算よりは大幅に大きくなる値
        BTL_PRINT("    ->相性値として %d を加算\n", (int)value);
        affVal[i] += (int)( value );
      }
    }
  }

  // 得点順ソート
  for(u32 i=0; i<numPoke; ++i)
  {
    for(u32 j=i+1; j<numPoke; ++j)
    {
      if( affVal[i] < affVal[j] )
      {
        int tmp_i = affVal[i];
        affVal[i] = affVal[j];
        affVal[j] = tmp_i;

        u8 tmp_u8 = list[i];
        list[i] = list[j];
        list[j] = tmp_u8;
      }
    }
  }

  BTL_PRINT("[BTL_CLIENT]入れ替え対象ソート結果　%d 体\n  ", numPoke);
  for (u32 i=0; i<numPoke; ++i) {
    BTL_PRINT("%d,", list[i]);
  }
  BTL_PRINT("\n");
}
/**
 * HP回復効果のある位置エフェクトが効いているかどうか判定
 * @param pos
 * @return  効いていたら true
 */
bool BTL_CLIENT::isExistHPRecoverEffect( BtlPokePos pos ) const
{
  if( pos < BTL_POS_NUM )
  {
    if( m_pokeCon->GetPosEffectStatusConst( pos, BTL_POSEFF_IYASINONEGAI )->IsEffective() ) {
      return true;
    }
    if( m_pokeCon->GetPosEffectStatusConst( pos, BTL_POSEFF_MIKADUKINOMAI )->IsEffective() ) {
      return true;
    }
  }
  return false;
}


//--------------------------------------------------------------------------
/**
 * ポケモン選択画面用パラメータセット
 *
 * @param   numSelect
 * @param   param       [out] 選択画面用パラメータを格納する
 * @param   result      [out] 選択画面結果データ（初期化される）
 *
 */
//--------------------------------------------------------------------------
void BTL_CLIENT::setupPokeSelParam( u8 numSelect, PokeSelParam* param, PokeSelResult* result )
{
  u8 myCoverPosNum = this->getMyCoverPosNum();
  param->Init( m_myParty, numSelect );
  param->SetProhibitFighting( myCoverPosNum );
  result->Init( param );
}
//-----------------------------------------------------------------------------
/**
 * ポケモン選択画面結果 -> 決定アクションパラメータに変換
 */
//-----------------------------------------------------------------------------
void BTL_CLIENT::storePokeSelResult( const PokeSelResult* res )
{
  // res->cnt=選択されたポケモン数,  m_myChangePokeCnt=自分が担当する入れ替えリクエストポケモン数
  GFL_ASSERT_MSG(res->m_cnt <= m_myChangePokeCnt, "selCnt=%d, changePokeCnt=%d\n", res->m_cnt, m_myChangePokeCnt); // @check

  {
    u8 outIdx,selIdx;

    for(u32 i=0; i<res->m_cnt; i++)
    {
      selIdx = res->m_selIdx[i];
      outIdx = res->m_outPokeIdx[i];

//      BTL_PRINT("  -> 退場ポケモンIdx:%d, 入場ポケモンIdx:%d\n", outIdx, selIdx);
      BTL_ACTION_SetChangeParam( &m_actionParam[i], outIdx, selIdx );
    }
  }

  storeActionSelectSendData( m_actionParam, res->m_cnt );
}
//-----------------------------------------------------------------------------
/**
 *  強制終了時のポケモン選択自動決定処理
 */
//-----------------------------------------------------------------------------
void BTL_CLIENT::storePokeSelResult_ForceQuit( void )
{
  const BTL_POKEPARAM* bpp;
  u8 clientID, posIdx, memberMax, prevMemberIdx, selectCnt=0;
  u32 i, j;

  memberMax = m_myParty->GetMemberCount();
  prevMemberIdx = m_mainModule->GetClientBenchPosIndex( m_myID );

  for(i=0; i<m_myChangePokeCnt; ++i)
  {
    m_mainModule->BtlPosToClientID_and_PosIdx( m_myChangePokePos[i], &clientID, &posIdx );

    for(j=prevMemberIdx; j<memberMax; ++j)
    {
      bpp = m_myParty->GetMemberDataConst( j );
      if( !bpp->IsDead() )
      {
        BTL_N_Printf( DBGSTR_CLIENT_ForcePokeChange, selectCnt+1, m_myChangePokeCnt, posIdx, j );
        BTL_ACTION_SetChangeParam( &m_actionParam[selectCnt++], posIdx, j );
        prevMemberIdx = j+1;
        break;
      }
      else{
//        TAYA_Printf("たたかえない\n");
      }
    }
  }

  GFL_ASSERT(selectCnt);  // @check

  storeActionSelectSendData( m_actionParam, selectCnt );
}

//----------------------------------------------------------------------------------
/**
 * サーバから送られてきた交替リクエストポケモン位置から、自分の担当する位置のみを抜き出してバッファに格納
 *
 * @param   wk
 * @param   myCoverPos      [out] changePokePos中、自分が担当する位置を格納するバッファ
 *
 * @retval  u8    myCoverPos配列に格納した、自分が担当する位置の数.
 */
//----------------------------------------------------------------------------------
u8 BTL_CLIENT::storeMyChangePokePos( BtlPokePos* myCoverPos )
{
  u8 numChangePoke;
  const u8 *changePokePos;
  u32 i, cnt;

  numChangePoke = m_adapter->GetRecvData( reinterpret_cast<const void**>(&changePokePos) );
  for(i=0, cnt=0; i<numChangePoke; ++i)
  {
    if( m_mainModule->BtlPosToClientID(changePokePos[i]) == m_myID ){
      myCoverPos[ cnt++ ] = changePokePos[ i ];
    }
  }

  return cnt;
}

//----------------------------------------------------------------------------------
/**
 * 野生・シングル戦で自分のポケモンが倒れた時の「逃げる」「入れ替え」選択処理
 *
 * @param   wk
 * @param   seq
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::SubProc_UI_SelectChangeOrEscape( int* seq )
{
  switch( *seq ){
  case 0:
    m_viewCore->CMD_YESNO_Start( false, YESNO_MODE_NEXT_POKE );
    (*seq)++;
    break;

  case 1:
    {
      BtlYesNo result;
      if( m_viewCore->CMD_YESNO_Wait(&result) )
      {
        m_change_escape_code = (result == BTL_YESNO_YES)? BTL_CLIENTASK_CHANGE : BTL_CLIENTASK_ESCAPE;
        m_returnDataPtr = &(m_change_escape_code);
        m_returnDataSize = sizeof(m_change_escape_code);
        return true;
      }
    }
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * 【ポケモン選択】ひん死・生き返りで次に出すポケモンを選択
 *
 * @param   wk
 * @param   seq
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::SubProc_UI_SelectPokemonForCover( int* seq )
{
  return selectPokemonUI_Core( seq, false );
}
//----------------------------------------------------------------------------------
/**
 * 【ポケモン選択】バトンタッチなど生きているポケモンをターン途中に入れ替える
 *
 * @param   wk
 * @param   seq
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::SubProc_UI_SelectPokemonForChange( int* seq )
{
  return selectPokemonUI_Core( seq, true );
}
//----------------------------------------------------------------------------------
/**
 * 【ポケモン選択】AI：自分の「場」に空きが出来た時にくりだすポケモンを決定する
 *
 * @param   seq
 *
 * @retval  bool  終了時 true
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::SubProc_AI_SelectPokemon( int*  )
{
  m_myChangePokeCnt = storeMyChangePokePos( m_myChangePokePos );

  // 自分の「場」には空きが無い場合、NULLコマンドを返す
  if( m_myChangePokeCnt == 0 )
  {
     BTL_ACTION_SetNULL( &m_actionParam[0] );
     storeActionSelectSendData( m_actionParam, 1 );
     return true;
  }


  u8 puttableList[ BTL_PARTY_MEMBER_MAX ];
  u8 numPuttable;

  numPuttable = countPuttablePokemons( puttableList );
  if( numPuttable )
  {
    u8 numSelect = m_myChangePokeCnt;
    u8 posIdx, clientID, i;

    // 生きてるポケが出さなければいけない数に足りない場合、そこを諦める
    if( numSelect > numPuttable )
    {
      numSelect = numPuttable;
    }

    // みかづきのまい等の効果がある位置を優先するため繰り出す位置候補を並べ直し
    sortChangePos( m_myChangePokePos, m_myChangePokeCnt );

    for(i=0; i<numSelect; i++)
    {
      m_mainModule->BtlPosToClientID_and_PosIdx( m_myChangePokePos[i], &clientID, &posIdx );

      const BTL_POKEPARAM* target = changeAI_SupposeEnemy( m_myChangePokePos[i] );
      sortPuttablePokemonList( &puttableList[i], numPuttable-i, target, m_myChangePokePos[i] );
      BTL_ACTION_SetChangeParam( &m_actionParam[i], posIdx, puttableList[i] );
    }
    storeActionSelectSendData( m_actionParam, numSelect );
  }
  else
  {
    // そうでなければカバーを諦める
    BTL_ACTION_SetChangeDepleteParam( &m_actionParam[0] );
    storeActionSelectSendData( m_actionParam, 1 );
  }
  return true;
}

/**
 * 自分が担当する空き位置を、優先して埋めるべき順に並べ替え
 * （みかづきのまい効果など考慮）
 * @param[io] posAry  空き位置IDの格納された配列
 * @param posCnt      posAryに格納されている空き位置数
 */
void BTL_CLIENT::sortChangePos( BtlPokePos* posAry, u32 posCnt )
{
  if( posCnt > 1 )
  {
    u8   advantage[ BTL_POS_NUM ];

    posCnt = gfl2::math::Min( posCnt, static_cast<u32>(GFL_NELEMS(advantage)) );
    for(u32 i=0; i<posCnt; ++i)
    {
      if( isExistHPRecoverEffect(posAry[i]) ) {
        advantage[ i ] = 1;
      }else{
        advantage[ i ] = 0;
      }
    }

    u8 max;
    u8 max_idx;
    for(u32 i=0; i<posCnt; ++i)
    {
      max = advantage[ i ];
      max_idx = i;

      for(u32 j=i+1; j<posCnt; ++j)
      {
        if( advantage[j] > max ){
          max = advantage[j];
          max_idx = j;
        }
      }

      if( max_idx != i )
      {
        u8 tmp = advantage[i];
        advantage[i] = advantage[max_idx];
        advantage[max_idx] = tmp;

        BtlPokePos tmpPos = posAry[i];
        posAry[i] = posAry[max_idx];
        posAry[max_idx] = tmpPos;
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * 【ポケモン選択】録画再生
 *
 * @param   wk
 * @param   seq
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::SubProc_REC_SelectPokemon( int* seq )
{
  return SubProc_REC_SelectAction( seq );
}
/**
 *  UIポケモン入れ替えコア処理
 * @param    seq                シーケンス遷移用変数アドレス
 * @param    bForceChangeMode   「とんぼがえり」などの強制入れ替えモードの時、true
 */
bool BTL_CLIENT::selectPokemonUI_Core( int* seq, bool bForceChangeMode )
{
  enum {
    SEQ_INIT = 0,
    SEQ_SELECT_ROOT,
    SEQ_TIMELIMIT_OVER,
    SEQ_SELECT_END,

    SEQ_PROC_QUIT_ROOT,
    SEQ_COMM_WAIT,
    SEQ_PROC_QUIT_END,
  };

  switch( *seq ){
  case SEQ_INIT:
  {
    m_myChangePokeCnt = storeMyChangePokePos( m_myChangePokePos );

    if( m_myChangePokeCnt )
    {
      u8 puttableList[ BTL_PARTY_MEMBER_MAX ];
      u8 numPuttable = countPuttablePokemons( puttableList );

      // 控えに出せるポケモンが居る
      if( numPuttable )
      {
        u8 numSelect = m_myChangePokeCnt;
        u8 clientID, outPokeIdx;

        // 生きてるポケが出さなければいけない数に足りない場合、そこを諦める
        if( numSelect > numPuttable )
        {
          numSelect = numPuttable;
        }

        m_mainModule->BtlPosToClientID_and_PosIdx( m_myChangePokePos[0], &clientID, &outPokeIdx );

        setupPokeSelParam( numSelect, &m_pokeSelParam, &m_pokeSelResult );
        if( bForceChangeMode ){
          m_pokeSelParam.DisablePutPosSequence();
        }
        BTL_PRINT("[CLI] 退出メンバーIndex:%d\n", outPokeIdx );
        m_viewCore->CMD_StartPokeSelect( &m_pokeSelParam, outPokeIdx, false, &m_pokeSelResult );

        cmdLimit_Start();
        (*seq) = SEQ_SELECT_ROOT;
      }
      // 控えに出せるポケモンがもう居ない
      else
      {
        BTL_N_Printf( DBGSTR_CLIENT_NoMorePuttablePoke, m_myID);
        BTL_ACTION_SetChangeDepleteParam( &m_actionParam[0] );
        storeActionSelectSendData( m_actionParam, 1 );
        (*seq) = SEQ_PROC_QUIT_ROOT;
      }
    }
    // 自分は空きが出来ていないので何も選ぶ必要がない
    else
    {
      BTL_N_Printf( DBGSTR_CLIENT_NotDeadMember, m_myID );
      BTL_ACTION_SetNULL( &m_actionParam[0] );
      storeActionSelectSendData( m_actionParam, 1 );
      (*seq) = SEQ_PROC_QUIT_ROOT;
    }
  }
  break;

  case SEQ_SELECT_ROOT:
    if( cmdLimit_CheckOver() ){
      m_viewCore->CMD_ForceQuitPokeSelect();
      (*seq) = SEQ_TIMELIMIT_OVER;
      break;
    }
    if( m_viewCore->CMD_WaitPokeSelect() )
    {
      storePokeSelResult( &m_pokeSelResult );
      (*seq) = SEQ_SELECT_END;
    }
    break;

  // コマンド制限時間による強制終了処理
  case SEQ_TIMELIMIT_OVER:
    if( m_viewCore->CMD_WaitForceQuitPokeSelect() )
    {
      storePokeSelResult_ForceQuit();
      (*seq) = SEQ_SELECT_END;
    }
    break;

  case SEQ_SELECT_END:
    cmdLimit_End();
    (*seq) = SEQ_PROC_QUIT_ROOT;
    break;

  // 自分は選ぶ必要が無い場合は直接ココ
  case SEQ_PROC_QUIT_ROOT:
    if( m_mainModule->GetCommMode() != BTL_COMM_NONE )
    {
      m_commWaitInfoOn = true;
      m_viewCore->CMD_StartCommWait();
      (*seq) = SEQ_COMM_WAIT;
    }
    else{
      (*seq) = SEQ_PROC_QUIT_END;
    }
    break;

  case SEQ_COMM_WAIT:
    if( m_viewCore->CMD_WaitCommWait() )
    {
      (*seq) = SEQ_PROC_QUIT_END;
    }
    break;

  case SEQ_PROC_QUIT_END:
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------
/**
 * スタンドアロン入れ替え戦の入れ替え確認処理
 *
 * @param   wk
 * @param   seq
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::SubProc_UI_ConfirmIrekae( int* seq )
{
  enum {
    SEQ_INIT = 0,
    SEQ_START_CONFIRM,
    SEQ_WAIT_CONFIRM,
    SEQ_WAIT_POKESELECT,
    SEQ_DONT_CHANGE,
    SEQ_RETURN,
  };

  switch( *seq ){
  case 0:
    {
      const u8* enemyPutPokeID;
      m_adapter->GetRecvData( (const void**)(&enemyPutPokeID) );
      if( (*enemyPutPokeID) != BTL_POKEID_NULL )
      {
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_IrekaeConfirm );
        BTLV_STRPARAM_AddArg( &m_strParam, BTL_CLIENTID_SA_ENEMY1 );
        BTLV_STRPARAM_AddArg( &m_strParam, *enemyPutPokeID );
        BTLV_STRPARAM_SetWait( &m_strParam, BTLV_MSGWAIT_NONE );
        m_viewCore->CMD_StartMsg( &m_strParam );
        (*seq) = SEQ_START_CONFIRM;
      }
      else
      {
        (*seq) = SEQ_DONT_CHANGE;
      }
    }
    break;

  case SEQ_START_CONFIRM:
    if( m_viewCore->CMD_WaitMsg_WithoutHide() )
    {
      m_viewCore->CMD_YESNO_Start( true, YESNO_MODE_POKE_CHANGE );
      (*seq) = SEQ_WAIT_CONFIRM;
    }
    break;

  case SEQ_WAIT_CONFIRM:
    {
      BtlYesNo result;      if( m_viewCore->CMD_YESNO_Wait(&result) )
      {
        m_viewCore->CMD_HideMsg();
        if( result == BTL_YESNO_YES )
        {
          setupPokeSelParam( 1, &m_pokeSelParam, &m_pokeSelResult );
          m_viewCore->CMD_StartPokeSelect( &m_pokeSelParam, 0, true, &m_pokeSelResult );
          (*seq) = SEQ_WAIT_POKESELECT;
        }
        else
        {
          (*seq) = SEQ_DONT_CHANGE;
        }
      }
    }
    break;

  case SEQ_WAIT_POKESELECT:
    if( m_viewCore->CMD_WaitPokeSelect() )
    {
      if( !m_pokeSelResult.IsCancel() )
      {
        u8 idx = m_pokeSelResult.GetLast();
        if( (idx!=0) && (idx < BTL_PARTY_MEMBER_MAX) )
        {
          BTL_ACTION_SetChangeParam( &m_actionParam[0], 0, idx );
          (*seq) = SEQ_RETURN;
          break;
        }
      }
      (*seq) = SEQ_DONT_CHANGE;
    }
    break;

  case SEQ_DONT_CHANGE:
    BTL_ACTION_SetNULL( &m_actionParam[0] );
    (*seq) = SEQ_RETURN;
    /* fallthru */
  case SEQ_RETURN:
    storeActionSelectSendData( m_actionParam, 1 );
    return true;

  }
  return false;
}

//---------------------------------------------------
// 操作記録データ受信->保存
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_RecordData( int* )
{
  u32 dataSize;
  const void* dataBuf;

  dataSize = m_adapter->GetRecvData( &dataBuf );

  if( dataBuf == NULL )
  {//まだ届いてない
    return false;
  }

  BTL_PRINT( "[BTL_CLIENT_%d] 操作記録データを受信しました\n", m_myID );

  if( m_recData )
  {
    m_recData->Write( dataBuf, dataSize );
    BTL_PRINT( "[BTL_CLIENT_%d] 操作記録データを保存しました\n", m_myID );
  }

  if( m_cmdCheckServer )
  {
    m_cmdCheckServer->CMDCHECK_RestoreActionData( dataBuf, dataSize );
    m_cmdCheckTimingCode = m_recData->GetTimingCode( dataBuf );
    BTL_PRINT( "[BTL_CLIENT_%d] コマンド整合性チェックのためのユーザー入力をセットしました( m_cmdCheckTimingCode=%d )\n", m_myID, m_cmdCheckTimingCode );
  }

  net::ReloadRecSender( this );

  return true;
}
//---------------------------------------------------
// 通信対戦終了
//---------------------------------------------------
bool BTL_CLIENT::SubProc_REC_ExitCommTrainer( int* seq )
{
  if( m_viewCore )
  {
    bool fDone = SubProc_ExitCommTrainer( seq, true );
    return fDone;
  }
  return true;
}
bool BTL_CLIENT::SubProc_UI_ExitCommTrainer( int* seq )
{
  return SubProc_ExitCommTrainer( seq, false );
}
bool BTL_CLIENT::SubProc_ExitCommTrainer( int* seq, bool isRecPlayMode )
{
  enum
  {
    SEQ_START,
    SEQ_ROYAL_SHOWDOWN_START,        // バトルロイヤル決着演出( 開始 )
    SEQ_ROYAL_SHOWDOWN_WAIT,         // バトルロイヤル決着演出( 終了待ち )
    SEQ_SHOWDOWN_MESSAGE_START,
    SEQ_SHOWDOWN_MESSAGE_WAIT_WIN,
    SEQ_SHOWDOWN_MESSAGE_WAIT_LOSE,
    SEQ_LOSE_BGM_FADEOUT_WAIT,
  };

  switch( *seq )
  {
  case SEQ_START:
    {
      // 自分が非サーバである場合のため、Mainにサーバ計算した勝敗結果を渡しておく
      ResultCause result_cause;
      BtlResult result = expandServerResult( &result_cause );
      m_mainModule->NotifyBattleResult( result, result_cause );
    }
    *seq = ( m_mainModule->GetRule() == BTL_RULE_ROYAL ) ? ( SEQ_ROYAL_SHOWDOWN_START ) : ( SEQ_SHOWDOWN_MESSAGE_START );
    break;

  case SEQ_ROYAL_SHOWDOWN_START:
    {
      ClientSeq_BattleRoyalShowdown::SetupParam setupParam;
      setupParam.viewSystem = m_viewCore;
      m_seq_BattleRoyalShowdown->Setup( setupParam );
      m_seq_BattleRoyalShowdown->Start();
    }
    *seq = SEQ_ROYAL_SHOWDOWN_WAIT;
    break;

  case SEQ_ROYAL_SHOWDOWN_WAIT:
    m_seq_BattleRoyalShowdown->Update();
    if( m_seq_BattleRoyalShowdown->IsFinished() )
    {
      return true;
    }
    break;


  case SEQ_SHOWDOWN_MESSAGE_START:
    // @fix GFNMCat[5584]「持ち時間制」の対戦で時間切れになった場合、バトルビデオで再生すると結果が相違する場合がある
    if( isRecPlayMode && m_fRecPlayEndTimeOver )
    {
      return true;
    }
    // 録画データ長すぎで終わった場合は何も表示しない
    if( m_fRecPlayEndBufOver ){
      return true;
    }
    else
    {
      ResultCause result_cause;
      BtlResult result = expandServerResult( &result_cause );
      u8 fMulti = m_mainModule->IsMultiMode();
      if( !getVsCommTrainerExitMessage( &m_strParam, result, fMulti ) ) 
      {
        return true;
      }
      m_viewCore->CMD_StartMsg( &m_strParam );

      if( result == BTL_RESULT_WIN )
      {
        m_mainModule->StartWinBGM();
        *seq = SEQ_SHOWDOWN_MESSAGE_WAIT_WIN;
      }
      else
      {
        m_soundSys->FadeOutBGM( BTL_BGM_FADEOUT_FRAMES );
        *seq = SEQ_SHOWDOWN_MESSAGE_WAIT_LOSE;
      }
    }
    break;

  case SEQ_SHOWDOWN_MESSAGE_WAIT_WIN:
    if( m_viewCore->CMD_WaitMsg() )
    {
      return true;
    }
    break;

  case SEQ_SHOWDOWN_MESSAGE_WAIT_LOSE:
    if( m_viewCore->CMD_WaitMsg() )
    {
      *seq = SEQ_LOSE_BGM_FADEOUT_WAIT;
    }
    break;

  case SEQ_LOSE_BGM_FADEOUT_WAIT:
    if( !m_soundSys->CheckFadeOnBGM() )
    {
      m_soundSys->StopBGM();
      return true;
    }
  }

  return false;
}
/**
 *  サーバが送ってきた勝敗パラメータを解釈->自分が勝ったか負けたか判定
 */
BtlResult BTL_CLIENT::expandServerResult( ResultCause* resultCause ) const
{
  const BTL_RESULT_CONTEXT* resultContext;

  m_adapter->GetRecvData( (const void**)(&resultContext) );
  *resultCause = resultContext->resultCause;

  // @fix GFNMCat[1587] 「バトルビデオ選択画面」で「バトルロイヤル」の対戦結果を見ると4位のみ「LOSE」で他は「WIN」と表示される
  // バトルロイヤルは、親の勝敗だけでは、自身の勝敗が分からないため、自身で管理している現在順位を参照する。
  if( m_mainModule->GetRule() == BTL_RULE_ROYAL )
  {
    return BattleResultJudge::CheckBattleResult_Royal( *m_mainModule, m_royalRankingContainer, resultCause );
  }

  switch( resultContext->resultCode ){
  case BTL_RESULT_LOSE:
    if( !m_mainModule->IsFriendClientID(static_cast<u8>(resultContext->clientID), m_myID) ){
      return BTL_RESULT_WIN;
    }
    break;

  case BTL_RESULT_WIN:
    if( !m_mainModule->IsFriendClientID(static_cast<u8>(resultContext->clientID), m_myID) ){
      return BTL_RESULT_LOSE;
    }
    break;

  }
  return static_cast<BtlResult>(resultContext->resultCode);
}

/**
 * @brief 通信対戦終了時のメッセージを取得する
 * @param[out] strParam      取得したメッセージパラメータの格納先
 * @param      battleResult  戦闘結果
 * @param      isMultiMode   マルチモードか？
 * @retval true   メッセージパラメータを取得できた
 * @retval false  メッセージパラメータを取得できなかった
 */
bool BTL_CLIENT::getVsCommTrainerExitMessage( BTLV_STRPARAM* strParam, BtlResult battleResult, bool isMultiMode ) const
{
  u16 strID;
  u8 clientID = m_mainModule->GetEnemyClientID( 0 );

  switch( battleResult )
  {
  case BTL_RESULT_WIN:
    strID = ( isMultiMode ) ? BTL_STRID_STD_WinCommMulti : BTL_STRID_STD_WinComm;
    break;

  case BTL_RESULT_LOSE:
    strID = ( isMultiMode ) ? BTL_STRID_STD_LoseCommMulti : BTL_STRID_STD_LoseComm;
    break;

  case BTL_RESULT_DRAW:
    strID = ( isMultiMode ) ? BTL_STRID_STD_DrawCommMulti : BTL_STRID_STD_DrawComm;
    break;

  default:
    return false;
  }

  BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, strID );
  BTLV_STRPARAM_AddArg( strParam, clientID );

  if( isMultiMode )
  {
    u8 clientID_2 = m_mainModule->GetEnemyClientID( 1 );
    BTLV_STRPARAM_AddArg( strParam, clientID_2 );
  }

  return true;
}

/**
 *  相手クライアントが２人かどうか判定
 */
bool BTL_CLIENT::isEnemyClientDouble( void )
{
  u8 clientID_1 = m_mainModule->GetEnemyClientID( 0 );
  u8 clientID_2 = m_mainModule->GetEnemyClientID( 1 );

  return (clientID_1 != clientID_2);
}
/**
 *  トレーナー（ゲーム内・サブウェイ共通）グラフィックスライドイン
 */
void BTL_CLIENT::trainerGraphicIn( int client_idx )
{
  //u8 clientID = m_mainModule->GetEnemyClientID( client_idx );
  //int trtype = m_mainModule->GetClientTrainerType( clientID );

  //BTLV_EFFECT_SetTrainer( trtype, BTL_VPOS_TR_FAR_1, 0, 0, 0 );
  //BTLV_EFFECT_Add( BTLEFF_TRAINER_IN );
}
/**
 *  トレーナー（ゲーム内・サブウェイ共通）に勝った時の勝利メッセージプリント開始
 */
void BTL_CLIENT::msgWinningTrainerStart( void )
{
  u8 clientID_1 = m_mainModule->GetEnemyClientID( 0 );
  u8 clientID_2 = m_mainModule->GetEnemyClientID( 1 );

  // 相手クライアントが１人
  if( clientID_1 == clientID_2 )
  {
    BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WinTrainer );
    BTLV_STRPARAM_AddArg( &m_strParam, clientID_1 );
  }
  // 相手クライアントが２人
  else
  {
    u16 trtype1 = m_mainModule->GetClientTrainerType(clientID_1);
    u16 trtype2 = m_mainModule->GetClientTrainerType(clientID_2);
    int tagType = trainer::GetTagTrainerType(trtype1,trtype2);
    if( tagType != -1 )
    {
      //タッグトレーナー(ふたごちゃんとか)の分岐
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WinTrainer_Tag );
      BTLV_STRPARAM_AddArg( &m_strParam, tagType );
      BTLV_STRPARAM_AddArg( &m_strParam, clientID_1 );
      BTLV_STRPARAM_AddArg( &m_strParam, clientID_2 );
    }
    else
    {
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WinTrainer2 );
      BTLV_STRPARAM_AddArg( &m_strParam, clientID_1 );
      BTLV_STRPARAM_AddArg( &m_strParam, clientID_2 );
    }
  }
}

//---------------------------------------------------
// ゲーム内トレーナー戦を終了
//---------------------------------------------------
bool BTL_CLIENT::SubProc_REC_ExitForNPC( int* seq )
{
  if( m_viewCore )
  {
    bool result = SubProc_UI_ExitForNPC( seq );

    return result;
  }
  return true;
}
bool BTL_CLIENT::SubProc_UI_ExitForNPC( int* seq )
{
  enum {
    SEQ_ROYAL_SHOWDOWN_START,
    SEQ_ROYAL_SHOWDOWN_WAIT,

    SEQ_INIT,
    SEQ_WIN_START,
    SEQ_WIN_WAIT_TR1_IN,
    SEQ_WIN_WAIT_TR1_MSG,
    SEQ_WIN_WAIT_TR1_OUT,
    SEQ_WIN_WAIT_TR2_IN,
    SEQ_WIN_WAIT_TR2_MSG,

    SEQ_WIN_GET_MONEY_MSG,
    SEQ_WIN_GET_MONEY,
    SEQ_WIN_BONUS_MONEY,

    SEQ_LOSE_START,
    SEQ_LOSE_WAIT_MSG1,

    SEQ_END,
  };


  switch( *seq )
  {
  case SEQ_ROYAL_SHOWDOWN_START:
    if( m_mainModule->GetRule() == BTL_RULE_ROYAL )
    {
      ClientSeq_BattleRoyalShowdown::SetupParam setupParam;
      setupParam.viewSystem = m_viewCore;
      m_seq_BattleRoyalShowdown->Setup( setupParam );
      m_seq_BattleRoyalShowdown->Start();
      *seq = SEQ_ROYAL_SHOWDOWN_WAIT;
    }
    else
    {
      *seq = SEQ_INIT;
    }
    break;

  case SEQ_ROYAL_SHOWDOWN_WAIT:
    m_seq_BattleRoyalShowdown->Update();
    if( m_seq_BattleRoyalShowdown->IsFinished() )
    {
      return true;
    }
    break;

  case SEQ_INIT:
    {
      ResultCause resultCause;
      BtlResult resultCode = expandServerResult( &resultCause );
      if( resultCode == BTL_RESULT_WIN )
      {
        m_mainModule->StartWinBGM();
        msgWinningTrainerStart();
        m_viewCore->CMD_StartMsg( &m_strParam );
        (*seq) = SEQ_WIN_START;
      }
      else if( m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_NO_LOSE) == false )
      {
        u8 clientID = m_mainModule->GetPlayerClientID();

        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseStart );
        BTLV_STRPARAM_AddArg( &m_strParam, clientID );
        BTLV_STRPARAM_AddArg( &m_strParam, clientID );
        m_viewCore->CMD_StartMsg( &m_strParam );
        (*seq) = SEQ_LOSE_START;
      }
      else{
        return true;
      }
    }
    break;
  case SEQ_WIN_START:
    if( m_viewCore->CMD_WaitMsg() )
    {
      m_viewCore->CMD_TrainerIn_Lose( BTL_VPOS_TR_FAR_1 );
      (*seq) = SEQ_WIN_WAIT_TR1_IN;
    }
    break;
  case SEQ_WIN_WAIT_TR1_IN:
    if( m_viewCore->CMD_WaitTrainerIn() )
    {
      u8 clientID = m_mainModule->GetEnemyClientID( 0 );
      u32 trainerID = m_mainModule->GetClientTrainerID( clientID );
      // キー待ちを処理で行う
      m_viewCore->CMD_StartMsgTrainer( trainerID, trainer::TRMSG_FIGHT_LOSE, true );
      (*seq) = SEQ_WIN_WAIT_TR1_MSG;
    }
    break;
  case SEQ_WIN_WAIT_TR1_MSG:
    if( m_viewCore->CMD_WaitMsg() )
    {
      if( isEnemyClientDouble() )
      {
        (*seq) = SEQ_WIN_WAIT_TR1_OUT;
      }
      else
      {
        //@バトル変更箇所 勝利演出はない
        (*seq) = SEQ_WIN_GET_MONEY_MSG;
      }
    }
    break;
  case SEQ_WIN_WAIT_TR1_OUT:
    {
      //nijiではトレーナーを2人並べて表示できないので、別々に入ってくる
      m_viewCore->CMD_TrainerIn_Lose( BTL_VPOS_TR_FAR_2 );
      (*seq) = SEQ_WIN_WAIT_TR2_IN;
    }
    break;
  case SEQ_WIN_WAIT_TR2_IN:
    if( m_viewCore->CMD_WaitTrainerIn() )
    {
      u8 clientID = m_mainModule->GetEnemyClientID( 1 );
      u32 trainerID = m_mainModule->GetClientTrainerID( clientID );
      // キー待ちを処理で行う
      m_viewCore->CMD_StartMsgTrainer( trainerID, trainer::TRMSG_FIGHT_LOSE, true );
      (*seq) = SEQ_WIN_WAIT_TR2_MSG;
    }
    break;
  case SEQ_WIN_WAIT_TR2_MSG:
    if( m_viewCore->CMD_WaitMsg() )
    {
      //@バトル変更箇所 勝利演出はない
      (*seq) = SEQ_WIN_GET_MONEY_MSG;
    }
    break;



  case SEQ_WIN_GET_MONEY_MSG:
    if( m_mainModule->IsMoneySeqEnable() )
    {
      (*seq) = SEQ_WIN_GET_MONEY;
    }else{
      return true;
    }
    break;
  case SEQ_WIN_GET_MONEY:
    {
      u32 getMoney = m_mainModule->FixRegularMoney( );
      if( getMoney )
      {
        u8 clientID = m_mainModule->GetPlayerClientID( );

        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_GetMoney );
        BTLV_STRPARAM_AddArg( &m_strParam, clientID );
        BTLV_STRPARAM_AddArg( &m_strParam, getMoney );
        m_viewCore->CMD_StartMsg( &m_strParam );
      }

      if( m_mainModule->GetBonusMoney() == 0 ){
        (*seq) = SEQ_END;
      }else{
        (*seq) = SEQ_WIN_BONUS_MONEY;
      }
    }
    break;
  case SEQ_WIN_BONUS_MONEY:
    if( m_viewCore->CMD_WaitMsg() )
    {
      u32 bonus = m_mainModule->GetBonusMoney();
      if( bonus )
      {
        setBonusMoneyGettingStr( &m_strParam, bonus );
        m_viewCore->CMD_StartMsg( &m_strParam );
      }
      (*seq) = SEQ_END;
    }
    break;

  case SEQ_LOSE_START:
    if( m_viewCore->CMD_WaitMsg() )
    {
      u32 loseMoney = m_mainModule->FixLoseMoney();
      if( loseMoney )
      {
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseMoneyTrainer );
        BTLV_STRPARAM_AddArg( &m_strParam, m_myID );
        BTLV_STRPARAM_AddArg( &m_strParam, loseMoney );
        m_viewCore->CMD_StartMsg( &m_strParam );
      }
      (*seq) = SEQ_LOSE_WAIT_MSG1;
    }
    break;
  case SEQ_LOSE_WAIT_MSG1:
    if( m_viewCore->CMD_WaitMsg() )
    {
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseEnd );
      BTLV_STRPARAM_AddArg( &m_strParam, m_myID );
      m_viewCore->CMD_StartMsg( &m_strParam );
      (*seq) = SEQ_END;
    }
    break;

  case SEQ_END:
    if( m_viewCore->CMD_WaitMsg() ){
      return true;
    }
    break;

  default:
    return true;
  }
  return false;
}
//---------------------------------------------------
// サブウェイトレーナー戦を終了
//---------------------------------------------------
bool BTL_CLIENT::SubProc_REC_ExitForSubwayTrainer( int* seq )
{
  if( m_viewCore )
  {
    bool result = SubProc_ExitForSubwayTrainer( seq, true );

    return result;
  }
  return true;
}
bool BTL_CLIENT::SubProc_UI_ExitForSubwayTrainer( int* seq )
{
  return SubProc_ExitForSubwayTrainer( seq, false );
}
bool BTL_CLIENT::SubProc_ExitForSubwayTrainer( int* seq, bool isRecPlayMode )
{
  enum {
    SEQ_START,
    SEQ_ROYAL_SHOWDOWN_START,  // バトルロイヤル決着演出( 開始 )
    SEQ_ROYAL_SHOWDOWN_WAIT,   // バトルロイヤル決着演出( 終了待ち )
    SEQ_INIT,
    SEQ_WAIT_TRAINER_IN,
    SEQ_WAIT_MSG,
    SEQ_WAIT_TRAINER_OUT,
    SEQ_WAIT_TRAINER2_IN,
    SEQ_WAIT_MSG2,

    SEQ_WAIT_TRAINER_IN_FES,  //フェス用追加
    SEQ_WAIT_TRAINER_IN_WAIT_FES,
    SEQ_WAIT_MSG_FES,
  };

  switch( *seq )
  {
  case SEQ_START:
    {
      // サブウェイはAIマルチなどもあり得るので
      // 自分が非サーバである場合のため、Mainにサーバ計算した勝敗結果を渡しておく
      ResultCause resultCause;
      BtlResult result = expandServerResult( &resultCause );
      m_mainModule->NotifyBattleResult( result, resultCause );
    }
    *seq = ( m_mainModule->GetRule() == BTL_RULE_ROYAL ) ? ( SEQ_ROYAL_SHOWDOWN_START ) : ( SEQ_INIT );
    break;

  case SEQ_ROYAL_SHOWDOWN_START:
    {
      ClientSeq_BattleRoyalShowdown::SetupParam setupParam;
      setupParam.viewSystem = m_viewCore;
      m_seq_BattleRoyalShowdown->Setup( setupParam );
      m_seq_BattleRoyalShowdown->Start();
    }
    *seq = SEQ_ROYAL_SHOWDOWN_WAIT;
    break;

  case SEQ_ROYAL_SHOWDOWN_WAIT:
    m_seq_BattleRoyalShowdown->Update();
    if( m_seq_BattleRoyalShowdown->IsFinished() )
    {
      return true;
    }
    break;

  case SEQ_INIT:
    // @fix GFNMCat[5584]「持ち時間制」の対戦で時間切れになった場合、バトルビデオで再生すると結果が相違する場合がある
    if( isRecPlayMode && m_fRecPlayEndTimeOver )
    {
      return true;
    }
    // 録画データ長すぎで終わった場合は何も表示しない
    if( m_fRecPlayEndBufOver ){
      BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
      return true;
    }
    else if (m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_BATTLE_FES) )
    {
      //フェス専用分岐(プレイヤーが勝った時だけ演出がある
      ResultCause resultCause;
      BtlResult result = expandServerResult(&resultCause);
      if (result == BTL_RESULT_WIN)
      {
        //流れが分からなくなるので「○○の○○にかった」を出す
        m_mainModule->StartWinBGM();
        msgWinningTrainerStart();
        m_viewCore->CMD_StartMsg(&m_strParam);
        BTL_PRINT("[BTL_CLI] ExitForBattleFes  %d\n", __LINE__);
        (*seq) = SEQ_WAIT_TRAINER_IN_FES;
      }
      else
      {
        return true;
      }
    }
    else
    {
      ResultCause resultCause;
      BtlResult result = expandServerResult( &resultCause );
      BTL_PRINT("[BTL_CLI] ExitForSubwayTr  RESULT=%d,  \n", result );

      //トレーナー呼び出し周り修正。勝ち負け分岐を一応追加 ariizumi 121030

      if( result == BTL_RESULT_WIN )
      {
        m_mainModule->StartWinBGM();
        //コマンド置き換え ariizumi
        m_viewCore->CMD_TrainerIn_Lose( BTL_VPOS_TR_FAR_1 );
        BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
      }
      else
      if( result == BTL_RESULT_LOSE )
      {
        m_viewCore->CMD_TrainerIn_Win( BTL_VPOS_TR_FAR_1 );
        BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
      }

      if( (result == BTL_RESULT_WIN) || (result == BTL_RESULT_LOSE) ){
        setupSubwayTrainerMsg( result, 0 );
        (*seq) = SEQ_WAIT_TRAINER_IN;
        BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
      }
      else{
        BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
        return true;
      }
    }
    break;

  case SEQ_WAIT_TRAINER_IN:
    if( m_viewCore->CMD_WaitTrainerIn() )
    {
      BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
      // キー待ちを処理で行う
      m_viewCore->CMD_StartMsgInBuffer( true );

      if( m_mainModule->GetRule() == BTL_RULE_ROYAL )
      {
        getVsCommTrainerExitMessage( &m_strParam, BTL_RESULT_WIN, false );
        m_viewCore->CMD_StartMsg( &m_strParam );
      }

      (*seq) = SEQ_WAIT_MSG;
    }
    break;

  case SEQ_WAIT_MSG:
    if( m_viewCore->CMD_WaitMsg() )
    {
      if( !isEnemyClientDouble() ){
        BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
        return true;
      }else{
      BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
        (*seq) = SEQ_WAIT_TRAINER_OUT;
      }
    }
    break;

  case SEQ_WAIT_TRAINER_OUT:
    {
      ResultCause result_cause;
      BtlResult result = expandServerResult( &result_cause );

      BTL_PRINT("[BTL_CLI] ExitForSubwayTr  result=%d\n", result );
      //nijiではトレーナーを2人並べて表示できないので、別々に入ってくる

      if( result == BTL_RESULT_WIN )
      {
        m_viewCore->CMD_TrainerIn_Lose( BTL_VPOS_TR_FAR_2 );
        BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
      }
      else
      if( result == BTL_RESULT_LOSE )
      {
        m_viewCore->CMD_TrainerIn_Win( BTL_VPOS_TR_FAR_2 );
        BTL_PRINT("[BTL_CLI] ExitForSubwayTr  %d\n", __LINE__ );
      }
      setupSubwayTrainerMsg( result, 1 );
      (*seq) = SEQ_WAIT_TRAINER2_IN;
    }
    break;

  case SEQ_WAIT_TRAINER2_IN:
    if (m_viewCore->CMD_WaitTrainerIn())
    {
      // キー待ちを処理で行う
      m_viewCore->CMD_StartMsgInBuffer( true );
      (*seq) = SEQ_WAIT_MSG2;
    }
    break;

  case SEQ_WAIT_MSG2:
    if( m_viewCore->CMD_WaitMsg() )
    {
      return true;
    }
    break;

  case SEQ_WAIT_TRAINER_IN_FES:  //フェス用追加
    if (m_viewCore->CMD_WaitMsg())
    {
      m_viewCore->CMD_TrainerIn_WinFes(BTL_VPOS_TR_FAR_1);
      (*seq) = SEQ_WAIT_TRAINER_IN_WAIT_FES;
    }
    break;
  case SEQ_WAIT_TRAINER_IN_WAIT_FES:
    if (m_viewCore->CMD_WaitTrainerIn())
    {
      //フェス用メッセージ(先頭のキャラのフェスの選択メッセージを流す
      u8 pokeId = m_pokeCon->GetFrontPokeData(BTL_POS_1ST_0)->GetID();
      u16 msgId = m_mainModule->GetBattleFesPlayerWinMsgId(pokeId);
      m_viewCore->CMD_StartMsgFesTrainer(msgId);
      (*seq) = SEQ_WAIT_MSG_FES;
    }
    break;
  case SEQ_WAIT_MSG_FES:
    if (m_viewCore->CMD_WaitMsg())
    {
      return true;
    }
    break;
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * サブウェイトレーナーの勝敗メッセージをBTLV内部に生成しておく
 *
 * @param   wk
 * @param   result
 */
//----------------------------------------------------------------------------------
void BTL_CLIENT::setupSubwayTrainerMsg( BtlResult result, u8 client_idx )
{
  u8 clientID = m_mainModule->GetEnemyClientID( client_idx );

  const STRBUF* msg = m_mainModule->GetClientTrainerMsg( clientID, result );


  if( msg )
  {
    BTL_PRINT("トレーナーメッセージあり\n");
    gfl2::str::PrintCode( msg->GetPtr() );
    // EFIGS文法タグの展開のためExpand
    m_viewCore->CMD_CopyMsgToBufferExpanded( msg );
  }
  else{
    BTL_PRINT("トレーナーメッセージなし\n");
  }
}
//----------------------------------------------------------------------------------
/**
 *  ねこに小判などの効果で得たボーナスを拾ったメッセージを生成
 */
//----------------------------------------------------------------------------------
void BTL_CLIENT::GetBonusMoneyGettingStr( BTLV_STRPARAM* strParam, u32 bonus ) const
{
  setBonusMoneyGettingStr( strParam, bonus );
}
/**
 *  ねこに小判などの効果で得たボーナスを拾ったメッセージを生成
 */
void BTL_CLIENT::setBonusMoneyGettingStr( BTLV_STRPARAM* strParam, u32 bonus ) const
{
  u8 clientID = m_mainModule->GetPlayerClientID();

  BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_PickMoney );
  BTLV_STRPARAM_AddArg( strParam, clientID );
  BTLV_STRPARAM_AddArg( strParam, bonus );
}

//---------------------------------------------------
// 野生戦で勝った
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_WinWild( int* seq )
{
  switch( *seq )
  {
  case 0:
    {
      ClientSeq_WinWild::SetupParam setupParam;
      setupParam.client     = this;
      setupParam.pokecon    = m_pokeCon;
      setupParam.mainModule = m_mainModule;
      setupParam.viewSystem = m_viewCore;
      m_seq_WinWild->Setup( setupParam );
      m_seq_WinWild->Start();
    }
    ++( *seq );
    return false;

  case 1:
    m_seq_WinWild->Update();
    if( m_seq_WinWild->IsFinished() )
    {
      return true;
    }
    return false;

  default:
    GFL_ASSERT(0);
    return true;
  }
}
//---------------------------------------------------
// 野生戦で負けた
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_LoseWild( int* seq )
{
  if( m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_NO_LOSE) )
  {
    return true;
  }
  
  switch( *seq ){
  case 0:
    BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseStart );
    BTLV_STRPARAM_AddArg( &m_strParam, m_myID );
    m_viewCore->CMD_StartMsg( &m_strParam );
    (*seq)++;
    break;

  case 1:
    if( m_viewCore->CMD_WaitMsg() )
    {
      u32 loseMoney = m_mainModule->FixLoseMoney( );
      if( loseMoney )
      {
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseMoneyWild );
        BTLV_STRPARAM_AddArg( &m_strParam, m_myID );
        BTLV_STRPARAM_AddArg( &m_strParam, loseMoney );
        m_viewCore->CMD_StartMsg( &m_strParam );
      }
      (*seq)++;
    }
    break;

  case 2:
    if( m_viewCore->CMD_WaitMsg() )
    {
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseEnd );
      BTLV_STRPARAM_AddArg( &m_strParam, m_myID );
      m_viewCore->CMD_StartMsg( &m_strParam );
      (*seq)++;
    }
    break;

  case 3:
    if( m_viewCore->CMD_WaitMsg() ){
      (*seq)++;
    }
    break;

  default:
    return true;
  }

  return false;
}

//---------------------------------------------------
// 野生戦で捕獲した
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_CaptureWild( int* seq )
{
  
  switch( *seq )
  {
  case 0:
    {
      ClientSeq_Capture::SetupParam setupParam;
      setupParam.client     = this;
      setupParam.pokecon    = m_pokeCon;
      setupParam.mainModule = m_mainModule;
      setupParam.viewSystem = m_viewCore;
      m_seq_Capture->Setup( setupParam );
      m_seq_Capture->Start();
    }
    ++( *seq );
    return false;

  case 1:
    m_seq_Capture->Update();
    if( m_seq_Capture->IsFinished() )
    {
      return true;
    }
    return false;
  }

  return false;
}


//---------------------------------------------------
// バトル制限時間切れによる終了
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_NotifyTimeUp( int* seq )
{
  switch( *seq ){
  case 0:
    m_viewCore->CMD_InitFinishWaitCameraEffect();
    ++(*seq);
    break;
  case 1:
    if( m_viewCore->CMD_WaitFinishWaitCameraEffect() ){
      ++(*seq);
    }
    break;
  case 2:
    {
      u32 strID;
      BtlStrType strType;
      this->getTimeUpMessage( &strID, &strType );
      BTLV_STRPARAM_Setup( &m_strParam, strType, strID );
      m_viewCore->CMD_StartMsg( &m_strParam );
    }
    (*seq)++;
    break;
  case 3:
    if( m_viewCore->CMD_WaitMsg() ){
      (*seq)++;
    }
    break;
  default:
    return true;
  }
  return false;
}

/**
 * @brief 制限時間切れによって終了した際に表示するメッセージを取得する
 * @param[out] strID    表示するメッセージのID
 * @param[out] strType  表示するメッセージのタイプ
 */
void BTL_CLIENT::getTimeUpMessage( u32* strID, BtlStrType* strType ) const
{
  // 持ち時間が無くなった場合
  if( m_mainModule->IsClientLimitTimeExist() )
  {
    getTimeUpMessage_ClientLimitTime( strID, strType );
    return;
  }

  *strID = BTL_STRID_STD_BattleTimeOver;  //「せいげんじかんが　なくなりました！」
  *strType = BTL_STRTYPE_STD;
}

/**
 * @brief 持ち時間がなくなった場合に表示するメッセージを取得する
 * @param[out] strID    表示するメッセージのID
 * @param[out] strType  表示するメッセージのタイプ
 */
void BTL_CLIENT::getTimeUpMessage_ClientLimitTime( u32* strID, BtlStrType* strType ) const
{
  if( !m_mainModule->IsClientLimitTimeExist() )
  {
    *strID = BTL_STRID_STD_BattleTimeOver;  //「せいげんじかんが　なくなりました！」
    *strType = BTL_STRTYPE_STD;
    GFL_ASSERT(0);
    return;
  }

  bool isMyTimeOver = false;
  bool isEnemyTimeOver = false;
  for( u32 clientID=0; clientID<BTL_CLIENT_NUM; ++clientID )
  {
    if( !m_mainModule->IsExistClient( clientID ) )
    {
      continue;
    }

    u32 limitTime = m_viewCore->CMD_EFFECT_GetClientTime( clientID );
    if( limitTime == 0 )
    {
      if( m_mainModule->IsFriendClientID( m_myID, clientID ) )
      {
        isMyTimeOver = true;
      }
      else
      {
        isEnemyTimeOver = true;
      }
    }
  }

  *strID = ( isMyTimeOver && isEnemyTimeOver ) ? ( BTL_STRID_STD_BattleTimeOver ) : 
           ( isMyTimeOver ) ?                    ( BTL_STRID_STD_BattleTimeOver_My ) : ( BTL_STRID_STD_BattleTimeOver_Your );
  *strType = BTL_STRTYPE_STD;
}

//---------------------------------------------------
// バトル終了時フェードアウト
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_FadeOut( int* seq )
{
  enum
  {
    SEQ_START,
    SEQ_OTEIRE_START,   // お手入れ移行演出( 開始 )
    SEQ_OTEIRE_WAIT,    // お手入れ移行演出( 終了待ち )
    SEQ_FADEOUT_START,  // フェードアウト( 開始 )
    SEQ_FAIDEOUT_WAIT,  // フェードアウト( 終了待ち )
    SEQ_END,
  };

  switch( *seq )
  {
  case SEQ_START:
    *seq = ( isOteireExecuteEffectEnable() ) ? ( SEQ_OTEIRE_START ) : ( SEQ_FADEOUT_START );
    break;

  case SEQ_OTEIRE_START:
    {
      u8 pokeID = m_mainModule->GetOteireTargetPokeID();
      BtlvPos vpos = m_mainModule->PokeIDtoViewPos( m_pokeCon, pokeID );
      m_viewCore->CMD_OteireExecuteEffect_Start( vpos );
    }
    ++(*seq);
    break;

  case SEQ_OTEIRE_WAIT:
    if( m_viewCore->CMD_OteireExecuteEffect_Wait() )
    {
      ++(*seq);
    }
    break;

  case SEQ_FADEOUT_START:
    m_viewCore->CMD_StartCleanUp();
    ++(*seq);
    break;

  case SEQ_FAIDEOUT_WAIT:
    if( m_viewCore->CMD_WaitCleanUp() )
    {
      ++(*seq);
    }
    break;

  case SEQ_END:
    m_mainModule->SetOteireButtonDisable();

    // @fix NMCat[11] 戦闘終了後に複数処理を重ねるとBGMがなくなる
    // お手入れボタンが押されていて、進化するポケモンが存在しないなら、
    // バトルの後、勝利BGMを継続したままリフレ画面に遷移させるために、戦闘終了時に BGM をフェードアウトさせないようにする
    if( m_mainModule->IsOteireButtonPressed() && !isEvolveEnablePokeExsist() )
    {
      m_mainModule->BGMFadeOutDisable();
    }
    return true;
  }
  return false;
}
bool BTL_CLIENT::SubProc_REC_FadeOut( int* seq )
{
  if( m_viewCore )
  {
    bool fDone = SubProc_UI_FadeOut( seq );
    return fDone;
  }
  return true;
}

/**
 * @brief お手入れ移行演出を実行するか？
 * @retval true   実行する
 * @retval false  実行しない
 */
bool BTL_CLIENT::isOteireExecuteEffectEnable( void ) const
{
  // トレーナー戦は演出を実行しない
  if( m_mainModule->GetCompetitor() == BTL_COMPETITOR_TRAINER )
  {
    return false;
  }

  // お手入れボタンが押されていない
  if( !m_mainModule->IsOteireButtonPressed() )
  {
    return false;
  }

  // お手入れ対象が決まらない
  const u8 oteireTargetID = m_mainModule->GetOteireTargetPokeID();
  if( oteireTargetID == BTL_POKEID_NULL )
  {
    return false;
  }

  // お手入れ対象が場にいない
  {
    BtlvPos vpos = m_mainModule->PokeIDtoViewPos( m_pokeCon, oteireTargetID );
    if( vpos == BTL_VPOS_ERROR )
    {
      return false;
    }
  }

  // お手入れ対象が「そらをとぶ」等で隠れている
  {
    const BTL_POKEPARAM* poke = m_pokeCon->GetPokeParamConst( oteireTargetID );
    if( poke->IsWazaHide() )
    {
      return false;
    }
  }

  // お手入れ対象が状態異常
  {
    const BTL_POKEPARAM* poke = m_pokeCon->GetPokeParamConst( oteireTargetID );
    if( poke->GetPokeSick() != pml::pokepara::SICK_NULL )
    {
      return false;
    }
  }

  // 進化するポケモンが存在する
  if( isEvolveEnablePokeExsist() )
  {
    return false;
  }

  // 図鑑登録するポケモンが存在する
  if( m_mainModule->IsCapturedPokeZukanNewRegistered() )
  {
    return false;
  }

  return true;
}

/**
 * @brief 進化可能なポケモンが存在するかどうかをチェックする
 * @retval true   存在する
 * @retval false  存在しない
 */
bool BTL_CLIENT::isEvolveEnablePokeExsist( void ) const
{
  const BTL_PARTY* party = this->GetParty();
  const u32 memberNum = party->GetMemberCount();
  for( u32 i=0; i<memberNum; ++i )
  {
    const BTL_POKEPARAM* poke = party->GetMemberDataConst( i );
    if( m_mainModule->CanEvolveAfterBattle( *poke ) )
    {
      return true;
    }
  }
  return false;
}


//---------------------------------------------------
// 最後に返信したデータを再送信
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_SendLastDataAgain( int* seq )
{
  const SendData* latestSendData = m_sendDataContainer->GetLatestData();

  if( latestSendData == NULL )
  {
    m_returnDataSerialNumber = SEND_DATA_SERIAL_NUMBER_NULL;
    m_returnDataServerSeq    = SERVER_SEQ_END;
    m_returnDataCommandID    = ACMD_NONE;
    m_returnDataPtr          = NULL;
    m_returnDataSize         = 0;
  }
  else
  {
    m_returnDataSerialNumber = latestSendData->GetSerialNumber();
    m_returnDataServerSeq    = latestSendData->GetServerSequence();
    m_returnDataCommandID    = latestSendData->GetAdapterCommand();
    m_returnDataPtr          = latestSendData->GetData();
    m_returnDataSize         = latestSendData->GetDataSize();
  }

  return true;
}

//---------------------------------------------------
// 残り手持ち時間を送信
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_SendClientLimitTime( int* seq )
{
  if( m_viewCore )
  {
    m_returnData_ClientLimitTime.limitTime = m_viewCore->CMD_EFFECT_GetClientTime( m_myID );
  }
  else
  {
    m_returnData_ClientLimitTime.limitTime = m_mainModule->GetClientLimitTime();
  }

  m_returnDataSerialNumber = SEND_DATA_SERIAL_NUMBER_NULL;
  m_returnDataServerSeq    = SERVER_SEQ_END;
  m_returnDataCommandID    = ACMD_NONE;
  m_returnDataPtr          = &m_returnData_ClientLimitTime;
  m_returnDataSize         = sizeof( SEND_DATA_CLIENT_LIMIT_TIME );

  BTL_PRINT( "[BTL_CLIENT_%d] 残り手持ち時間を送信しました。limitTime=%d\n", m_myID, m_returnData_ClientLimitTime.limitTime );

  return true;
}

//---------------------------------------------------
// 残り手持ち時間を同期
//---------------------------------------------------
bool BTL_CLIENT::SubProc_UI_SyncClientLimitTime( int* seq )
{
  bool isCmdCheckError = false;
  u16 prevTime[ BTL_CLIENT_NUM ];  // 同期する前の残り時間
  u16 afterTime[ BTL_CLIENT_NUM ]; // 同期した後の残り時間

  // 同期する前の残り時間を取得
  for( u32 clientID=0; clientID<BTL_CLIENT_NUM; ++clientID )
  {
    if( m_mainModule->IsExistClient( clientID ) )
    {
      prevTime[ clientID ] = m_viewCore->CMD_EFFECT_GetClientTime( clientID );
    }
  }

  // 残り時間を同期
  {
    const void* data;
    const SEND_DATA_CLIENT_LIMIT_TIME_SYNC* syncData;

    m_adapter->GetRecvData( &data );
    syncData = reinterpret_cast<const SEND_DATA_CLIENT_LIMIT_TIME_SYNC*>( data );

    for( u32 clientID=0; clientID<BTL_CLIENT_NUM; ++clientID )
    {
      if( m_mainModule->IsExistClient( clientID ) )
      {
        m_viewCore->CMD_EFFECT_SetClientTime( clientID, syncData->limitTime[ clientID ] );
        afterTime[ clientID ] = syncData->limitTime[ clientID ];
        BTL_PRINT( "[BTL_CLIENT_%d] 残り手持ち時間を同期( clientID=%d, limitTime[s]=%d )\n", m_myID, clientID, syncData->limitTime[ clientID ] );
      }
    }

    // @fix NMCat[2759] バトルロイヤルで子機が切断するとゲームが進行しなくなる
    // 残り時間を同期した後で自分が新しいサーバーになることが有り得るので、コマンド整合性チェックサーバーも同期する
    if( m_cmdCheckServer )
    {
      m_cmdCheckServer->CMDCHECK_RestoreClientLimitTime( *syncData );
    }
  }

  // 自分の残り時間が不正に書き換えられていた場合、不整合フラグを立てる
  if( prevTime[ m_myID ] != afterTime[ m_myID ] )
  {
    isCmdCheckError = true;
    BTL_PRINT( "[BTL_CLIENT_%d] 自分の持ち時間が不正に書き換えられました。不整合フラグを立てます。prevTime=%d, afterTime=%d", m_myID, prevTime[ m_myID ], afterTime[ m_myID ] );
  }

  // 自分以外の残り時間が不正に書き換えられ、増えていた場合、不整合フラグを立てる
  // 
  // バトルロイヤルでは、通信が切断した子をAIに変更してバトルが継続しますが、AIが送信する自分の残り時間は、常に持ち時間の初期値となります。
  // そのため、AIになったクライアントの残り時間は必ず増加します。
  // 子は、自分以外のクライアントがAIなのか、UIなのかの判別が出来ないので、バトルロイヤルでは、このチェック自体を無効にします。
  {
    const BtlRule rule = m_mainModule->GetRule();
    if( rule != BTL_RULE_ROYAL )
    {
      for( u32 clientID=0; clientID<BTL_CLIENT_NUM; ++clientID )
      {
        if( ( m_mainModule->IsExistClient( clientID ) ) && 
            ( clientID != m_myID ) &&
            ( prevTime[ clientID ] < afterTime[ clientID ] ) )
        {
          isCmdCheckError = true;
          BTL_PRINT( "[BTL_CLIENT_%d] 自分以外の持ち時間が不正に書き換えられました。不整合フラグを立てます。prevTime=%d, afterTime=%d", m_myID, prevTime[ clientID ], afterTime[ clientID ] );
        }
      }
    }
  }

  if( isCmdCheckError )
  {
    m_mainModule->NotifyCmdCheckError();
    m_fCmdCheckEnable = false;
  }

  return true;
}


//--------------------------------------------------------------------------------
// サーバコマンド処理
//--------------------------------------------------------------------------------
bool BTL_CLIENT::SubProc_REC_ServerCmd( int* seq )
{
  if( m_viewCore )
  {
    return SubProc_UI_ServerCmd( seq );
  }
  return true;
}

//--------------------------------------------------------------------------------
/**
 * サーバコマンド -> 実行関数ポインタ取得
 *
 * @param   cmd   サーバコマンドID
 *
 * @retval  ServerCmdProc   実行関数ポインタ（実行関数の存在しない特殊コマンドの場合NULL）
 */
//--------------------------------------------------------------------------------
BTL_CLIENT::ServerCmdProc BTL_CLIENT::dispatchServerCmdProc( ServerCmd cmd )
{
  static const struct {
    ServerCmd     cmd;
    ServerCmdProc proc;
  }scprocTbl[] = {
    { SC_MSG_STD,                                  &BTL_CLIENT::scProc_MSG_Std                    },
    { SC_MSG_STD_SE,                               &BTL_CLIENT::scProc_MSG_StdSE                  },
    { SC_MSG_SET,                                  &BTL_CLIENT::scProc_MSG_Set                    },
    { SC_MSG_SET_SE,                               &BTL_CLIENT::scProc_MSG_SetSE                  },
    { SC_MSG_WAZA,                                 &BTL_CLIENT::scProc_MSG_Waza                   },
    { SC_ACT_WAZA_EFFECT,                          &BTL_CLIENT::scProc_ACT_WazaEffect             },
    { SC_ACT_TAMEWAZA_HIDE,                        &BTL_CLIENT::scProc_ACT_TameWazaHide           },
    { SC_ACT_WAZA_DMG,                             &BTL_CLIENT::scProc_ACT_WazaDmg                },
    { SC_ACT_WAZA_DMG_PLURAL,                      &BTL_CLIENT::scProc_ACT_WazaDmg_Plural         },
    { SC_ACT_WAZA_ICHIGEKI,                        &BTL_CLIENT::scProc_ACT_WazaIchigeki           },
    { SC_ACT_SICK_ICON,                            &BTL_CLIENT::scProc_ACT_SickIcon               },
    { SC_ACT_CONF_DMG,                             &BTL_CLIENT::scProc_ACT_ConfDamage             },
    { SC_ACT_DEAD,                                 &BTL_CLIENT::scProc_ACT_Dead                   },
    { SC_ACT_RELIVE,                               &BTL_CLIENT::scProc_ACT_Relive                 },
    { SC_ACT_MEMBER_OUT_MSG,                       &BTL_CLIENT::scProc_ACT_MemberOutMsg           },
    { SC_ACT_MEMBER_OUT,                           &BTL_CLIENT::scProc_ACT_MemberOut              },
    { SC_ACT_MEMBER_IN,                            &BTL_CLIENT::scProc_ACT_MemberIn               },
    { SC_ACT_RANKUP,                               &BTL_CLIENT::scProc_ACT_RankUp                 },
    { SC_ACT_RANKDOWN,                             &BTL_CLIENT::scProc_ACT_RankDown               },
    { SC_ACTOP_WEATHER_START,                      &BTL_CLIENT::scProc_ACTOP_WeatherStart         },
    { SC_ACTOP_WEATHER_END,                        &BTL_CLIENT::scProc_ACTOP_WeatherEnd           },
    { SC_ACTOP_MEGAEVO,                            &BTL_CLIENT::scProc_ACTOP_MegaEvo              },
    { SC_ACT_SIMPLE_HP,                            &BTL_CLIENT::scProc_ACT_SimpleHP               },
    { SC_ACT_USE_ITEM,                             &BTL_CLIENT::scProc_ACT_UseItem                },
    { SC_ACT_KINOMI_PRE_WAZA,                      &BTL_CLIENT::scProc_ACT_KinomiPrevWaza         },
    { SC_TOKWIN_IN,                                &BTL_CLIENT::scProc_TOKWIN_In                  },
    { SC_TOKWIN_OUT,                               &BTL_CLIENT::scProc_TOKWIN_Out                 },
    { SC_OP_HP_MINUS,                              &BTL_CLIENT::scProc_OP_HpMinus                 },
    { SC_OP_HP_MINUS_SYNC_WAZAEFF,                 &BTL_CLIENT::scProc_OP_HpMinus                 },   // ワザエフェクトと同期させるためにコマンドIDを別けているだけなので、実行内容は全く同じで良いハズ…
    { SC_OP_HP_PLUS,                               &BTL_CLIENT::scProc_OP_HpPlus                  },
    { SC_OP_HP_ZERO,                               &BTL_CLIENT::scProc_OP_HpZero                  },
    { SC_OP_PP_MINUS,                              &BTL_CLIENT::scProc_OP_PPMinus                 },
    { SC_OP_PP_MINUS_ORG,                          &BTL_CLIENT::scProc_OP_PPMinus_Org             },
    { SC_OP_WAZA_USED,                             &BTL_CLIENT::scProc_OP_WazaUsed                },
    { SC_OP_PP_PLUS,                               &BTL_CLIENT::scProc_OP_PPPlus                  },
    { SC_OP_PP_PLUS_ORG,                           &BTL_CLIENT::scProc_OP_PPPlus_Org              },
    { SC_OP_RANK_UP,                               &BTL_CLIENT::scProc_OP_RankUp                  },
    { SC_OP_RANK_DOWN,                             &BTL_CLIENT::scProc_OP_RankDown                },
    { SC_OP_RANK_SET8,                             &BTL_CLIENT::scProc_OP_RankSet8                },
    { SC_OP_RANK_RECOVER,                          &BTL_CLIENT::scProc_OP_RankRecover             },
    { SC_OP_RANK_UP_RESET,                         &BTL_CLIENT::scProc_OP_RankUpReset             },
    { SC_OP_RANK_RESET,                            &BTL_CLIENT::scProc_OP_RankReset               },
    { SC_OP_ADD_CRITICAL,                          &BTL_CLIENT::scProc_OP_AddCritical             },
    { SC_OP_SICK_SET,                              &BTL_CLIENT::scProc_OP_SickSet                 },
    { SC_OP_CURE_POKESICK,                         &BTL_CLIENT::scProc_OP_CurePokeSick            },
    { SC_OP_CURE_WAZASICK,                         &BTL_CLIENT::scProc_OP_CureWazaSick            },
    { SC_OP_MEMBER_IN,                             &BTL_CLIENT::scProc_OP_MemberIn                },
    { SC_OP_SET_STATUS,                            &BTL_CLIENT::scProc_OP_SetStatus               },
    { SC_OP_SET_WEIGHT,                            &BTL_CLIENT::scProc_OP_SetWeight               },
    { SC_OP_CHANGE_POKETYPE,                       &BTL_CLIENT::scProc_OP_ChangePokeType          },
    { SC_OP_EX_POKETYPE,                           &BTL_CLIENT::scProc_OP_ExPokeType              },
    { SC_OP_WAZASICK_TURNCHECK,                    &BTL_CLIENT::scProc_OP_WSTurnCheck             },
    { SC_OP_CONSUME_ITEM,                          &BTL_CLIENT::scProc_OP_ConsumeItem             },
    { SC_OP_UPDATE_USE_WAZA,                       &BTL_CLIENT::scProc_OP_UpdateUseWaza           },
    { SC_OP_SET_CONTFLAG,                          &BTL_CLIENT::scProc_OP_SetContFlag             },
    { SC_OP_RESET_CONTFLAG,                        &BTL_CLIENT::scProc_OP_ResetContFlag           },
    { SC_OP_SET_TURNFLAG,                          &BTL_CLIENT::scProc_OP_SetTurnFlag             },
    { SC_OP_RESET_TURNFLAG,                        &BTL_CLIENT::scProc_OP_ResetTurnFlag           },
    { SC_OP_SET_PERMFLAG,                          &BTL_CLIENT::scProc_OP_SetPermFlag             },
    { SC_OP_INC_BATTLE_TURN_COUNT,                 &BTL_CLIENT::scProc_OP_IncBattleTurnCount      },
    { SC_OP_INC_POKE_TURN_COUNT,                   &BTL_CLIENT::scProc_OP_IncPokeTurnCount        },
    { SC_OP_CHANGE_TOKUSEI,                        &BTL_CLIENT::scProc_OP_ChangeTokusei           },
    { SC_OP_SET_ITEM,                              &BTL_CLIENT::scProc_OP_SetItem                 },
    { SC_OP_UPDATE_WAZANUMBER,                     &BTL_CLIENT::scProc_OP_UpdateWazaNumber        },
    { SC_OP_OUTCLEAR,                              &BTL_CLIENT::scProc_OP_OutClear                },
    { SC_OP_ADD_FLDEFF,                            &BTL_CLIENT::scProc_OP_AddFldEff               },
    { SC_OP_ADD_FLDEFF_DEPEND,                     &BTL_CLIENT::scProc_OP_AddFldEffDepend         },
    { SC_OP_CHANGE_GROUND,                         &BTL_CLIENT::scProc_OP_ChangeGround            },
    { SC_OP_DEL_FLDEFF_DEPEND,                     &BTL_CLIENT::scProc_OP_DelFldEffDepend         },
    { SC_OP_REMOVE_FLDEFF,                         &BTL_CLIENT::scProc_OP_RemoveFldEff            },
    { SC_OP_SET_POKE_COUNTER,                      &BTL_CLIENT::scProc_OP_SetPokeCounter          },
    { SC_OP_INC_KILLCOUNT,                         &BTL_CLIENT::scProc_OP_IncKillCount            },
    { SC_OP_BATONTOUCH,                            &BTL_CLIENT::scProc_OP_BatonTouch              },
    { SC_OP_MIGAWARI_CREATE,                       &BTL_CLIENT::scProc_OP_MigawariCreate          },
    { SC_OP_MIGAWARI_DELETE,                       &BTL_CLIENT::scProc_OP_MigawariDelete          },
    { SC_OP_SET_ILLUSION,                          &BTL_CLIENT::scProc_OP_SetFakeSrc              },
    { SC_OP_CLEAR_CONSUMED_ITEM,                   &BTL_CLIENT::scProc_OP_ClearConsumedItem       },
    { SC_OP_CURESICK_DEPEND_POKE,                  &BTL_CLIENT::scProc_OP_CureSickDependPoke      },
    { SC_OP_WAZADMG_REC,                           &BTL_CLIENT::scProc_OP_AddWazaDamage           },
    { SC_OP_TURN_CHECK,                            &BTL_CLIENT::scProc_OP_TurnCheck               },
    { SC_OP_TURN_CHECK_FIELD,                      &BTL_CLIENT::scProc_OP_TurnCheckField          },
    { SC_OP_SET_DORYOKU,                           &BTL_CLIENT::scProc_OP_SetDoryoku              },
    { SC_OP_START_POSEFF,                          &BTL_CLIENT::scProc_OP_StartPosEff             },
    { SC_OP_REMOVE_POSEFF,                         &BTL_CLIENT::scProc_OP_RemovePosEff            },
    { SC_OP_POSEFFECT_UPDATE_PARAM,                &BTL_CLIENT::scProc_OP_UpdatePosEffectParam    },
    { SC_OP_PGL_RECORD,                            &BTL_CLIENT::scProc_OP_PGLRecord               },
    { SC_OP_SIDEEFFECT_ADD,                        &BTL_CLIENT::scProc_OP_SideEffect_Add          },
    { SC_OP_SIDEEFFECT_REMOVE,                     &BTL_CLIENT::scProc_OP_SideEffect_Remove       },
    { SC_OP_SIDEEFFECT_INC_TURNCOUNT,              &BTL_CLIENT::scProc_OP_SideEffect_IncTurnCount },
    { SC_OP_PUBLISH_CLIENT_INFO_APPEARED_POKEMON,  &BTL_CLIENT::scProc_OP_PublishClientInformation_AppaearPokemon },
    { SC_OP_PUBLISH_CLIENT_INFO_HAVE_POKEMON_ITEM, &BTL_CLIENT::scProc_OP_PublishClientInformation_HavePokemonItem },
    { SC_OP_SET_CURRENT_ROYAL_RANKING,             &BTL_CLIENT::scProc_OP_SetCurrentRoyalRanking  },
    { SC_OP_SET_ZENRYOKU_WAZA_USED,                &BTL_CLIENT::scProc_OP_SetZenryokuWazaUsed     },
    { SC_OP_SET_DIRT,                              &BTL_CLIENT::scProc_OP_SetDirt                 },
    { SC_OP_INC_WEATHER_PASSED_TURN,               &BTL_CLIENT::scProc_OP_IncWeatherPassedTurn    },
    { SC_OP_SET_SPACT_PRIORITY,                    &BTL_CLIENT::scProc_OP_SetSpActPriority        },
    { SC_ACT_KILL,                                 &BTL_CLIENT::scProc_ACT_Kill                   },
    { SC_ACTOP_MOVE,                               &BTL_CLIENT::scProc_ACTOP_Move                 },
    { SC_ACT_EXP,                                  &BTL_CLIENT::scProc_ACT_Exp                    },
    { SC_ACT_NADERU,                               &BTL_CLIENT::scProc_ACT_Naderu                 },
    { SC_ACT_BALL_THROW,                           &BTL_CLIENT::scProc_ACT_BallThrow              },
    { SC_ACT_BALL_THROW_CAPTURED,                  &BTL_CLIENT::scProc_ACT_BallThrowCaptured      },
    { SC_ACT_BALL_THROW_FORBIDDEN,                 &BTL_CLIENT::scProc_ACT_BallThrowForbidden     },
    { SC_ACTOP_CHANGE_TOKUSEI,                     &BTL_CLIENT::scProc_ACTOP_ChangeTokusei        },
    { SC_ACTOP_SWAP_TOKUSEI,                       &BTL_CLIENT::scProc_ACTOP_SwapTokusei          },
    { SC_ACT_FAKE_DISABLE,                         &BTL_CLIENT::scProc_ACT_FakeDisable            },
    { SC_ACT_EFFECT_SIMPLE,                        &BTL_CLIENT::scProc_ACT_EffectSimple           },
    { SC_ACT_EFFECT_BYPOS,                         &BTL_CLIENT::scProc_ACT_EffectByPos            },
    { SC_ACT_PLURALHIT_EX_2ND,                     &BTL_CLIENT::scProc_ACT_PluralEx2ndHit         },
    { SC_ACT_EFFECT_BYVECTOR,                      &BTL_CLIENT::scProc_ACT_EffectByVector         },
    { SC_ACT_EFFECT_FIELD,                         &BTL_CLIENT::scProc_ACT_EffectField            },
    { SC_ACT_CHANGE_FORM,                          &BTL_CLIENT::scProc_ACT_ChangeForm             },
    { SC_ACT_MIGAWARI_CREATE,                      &BTL_CLIENT::scProc_ACT_MigawariCreate         },
    { SC_ACT_MIGAWARI_DELETE,                      &BTL_CLIENT::scProc_ACT_MigawariDelete         },
    { SC_ACT_HENSIN,                               &BTL_CLIENT::scProc_ACT_Hensin                 },
    { SC_ACT_MIGAWARI_DAMAGE,                      &BTL_CLIENT::scProc_ACT_MigawariDamage         },
    { SC_ACT_WIN_BGM,                              &BTL_CLIENT::scProc_ACT_PlayWinBGM             },
    { SC_ACT_MSGWIN_HIDE,                          &BTL_CLIENT::scProc_ACT_MsgWinHide             },
    { SC_ACT_FRIENDSHIP_EFFECT,                    &BTL_CLIENT::scProc_ACT_FriendshipEffect       },
    { SC_ACT_FRIENDSHIP_EFFECT_MSG,                &BTL_CLIENT::scProc_ACT_FriendshipEffectMsg    },
    { SC_ACT_DEAD_PLURAL,                          &BTL_CLIENT::scProc_ACT_DeadPlural             },
    { SC_ACT_ZENRYOKUWAZA_BEGIN,                   &BTL_CLIENT::scProc_ACT_ZenryokuWazaBegin      },
    { SC_ACT_ZENRYOKUWAZA_END,                     &BTL_CLIENT::scProc_ACT_ZenryokuWazaEnd        },
    { SC_ACT_OTEIRE_BUTTON_DISPLAY,                &BTL_CLIENT::scProc_ACT_OteireButtonDisplay    },
    { SC_ACT_INTRUDE_CALL,                         &BTL_CLIENT::scProc_ACT_IntrudeCall            },
    { SC_ACT_INTRUDE_APPEAR,                       &BTL_CLIENT::scProc_ACT_IntrudeAppear          },
    { SC_ACT_INTRUDE_NOT_APPEARD,                  &BTL_CLIENT::scProc_ACT_IntrudeNotAppeared     },
    { SC_ACT_TURN_START,                           &BTL_CLIENT::scProc_ACT_TurnStart              },
    { SCEX_VALIABLE_ARGS,                          &BTL_CLIENT::scProc_ExArg                      },
  };


  for(uint32_t i=0; i<GFL_NELEMS(scprocTbl); ++i)
  {
    if( scprocTbl[i].cmd == cmd )
    {
      return scprocTbl[i].proc;
    }
  }
  return NULL;
}


bool BTL_CLIENT::SubProc_UI_ServerCmd( int* seq )
{
restart:

  switch( *seq ){
  case 0:
    {
      u32 cmdSize;
      const void* cmdBuf;

      cmdSize = m_adapter->GetRecvData( &cmdBuf );

      if( (m_cmdCheckServer != NULL)
      &&  (m_cmdCheckTimingCode != rec::RECTIMING_None)
      &&  (m_fCmdCheckEnable)
      ){
        if( m_cmdCheckServer->CMDCHECK_Make( (rec::Timing)(m_cmdCheckTimingCode), cmdBuf, cmdSize) )
        {
          m_mainModule->NotifyCmdCheckError( );
          m_fCmdCheckEnable = false;
        }
        m_cmdCheckTimingCode = rec::RECTIMING_None;
      }

      SCQUE_Setup( m_cmdQue, cmdBuf, cmdSize );

      if( m_commWaitInfoOn )
      {
        m_commWaitInfoOn = false;
        m_viewCore->CMD_ResetCommWaitInfo();
      }
      m_viewCore->CMD_InitFinishWaitCameraEffect();
      (*seq)++;
    }
    /* fallthru */
  case 1:
    if( m_viewCore->CMD_WaitFinishWaitCameraEffect() ){
      (*seq)++;
    }
    break;

  case 2:
    if( SCQUE_IsFinishRead(m_cmdQue) )
    {
      (*seq) = 5;
      return true;
    }
    // 録画再生処理チェック
    else if( RecPlayer_CheckBlackOut(&m_recPlayer) )
    {
      return true;
    }
    (*seq)++;
    /* fallthru */
  case 3:
    {
      m_serverCmd = scmd::SCQUE_Read( m_cmdQue, m_cmdArgs, GFL_NELEMS(m_cmdArgs) );
      if( m_serverCmd == SC_MAX ){
        return true;
      }

      m_seq_WinWild->UpdateOteireWaitSequenceExist( m_serverCmd );
      m_seq_Capture->UpdateOteireWaitSequenceExist( m_serverCmd );

      m_scProc = dispatchServerCmdProc( m_serverCmd );
      m_scSeq = 0;

      if( m_scProc == NULL ){
        (*seq) = 2;
        goto restart;
      }

      TAYA_PRINT( "=================== Next serverCmd=%d =========>\n", m_serverCmd );
      (*seq)++;
    }
    /* fallthru */
  case 4:
    if( (this->*m_scProc)(&m_scSeq, m_cmdArgs) )
    {
      TAYA_PRINT("  ServerCmd:%d done\n", m_serverCmd);

      (*seq) = 2;
      goto restart;
    }
    break;

  default:
    return true;
  }

  return false;
}


//=============================================================================================================
/**
 * SC_ACT_WAZA_EFFECT の実行時、同時に再生すべきコマンド＆引数群など、必要な情報をワークメンバに保存する
 */
//=============================================================================================================
void BTL_CLIENT::storeWazaEffectSyncDamageParams( WAZAEFF_SYNCDAMAGE_CMD_WORK* work )
{
#define DEBUG_PRINT_ENABLE  (0)

  /**
   *  同時に再生すべきコマンド群
   */
  static const ServerCmd TargetCmdList[] = {
    SC_OP_HP_MINUS_SYNC_WAZAEFF,
    SC_ACT_WAZA_DMG,
    SC_ACT_WAZA_DMG_PLURAL,
    SC_ACT_MIGAWARI_DAMAGE,
  };

  scmd::QUE_READING_STATE  initState = scmd::SCQUE_ReadStatePush( m_cmdQue );

  #if DEBUG_PRINT_ENABLE
    TAYA_PRINT("[CLIENT] 先読み開始 ------\n");
  #endif

  {
    uint32_t  count = 0;
    scmd::QUE_READING_STATE  readVarArgsState = static_cast<scmd::QUE_READING_STATE>( scmd::QUE_READING_STATE_NULL );

    for ever
    {
      if( SCQUE_IsFinishRead(m_cmdQue) ){
        #if DEBUG_PRINT_ENABLE
          TAYA_PRINT(" [CLIENT] もう読めないのでぬける\n");
        #endif
        break;
      }
      scmd::QUE_READING_STATE  curState = scmd::SCQUE_ReadStatePush( m_cmdQue );

      ServerCmd  cmd = scmd::SCQUE_Read( m_cmdQue, work->store[count].cmdArgs, GFL_NELEMS(work->store[count].cmdArgs) );
      if( readVarArgsState == scmd::QUE_READING_STATE_NULL ){
        work->store[ count ].variableArgs.Clear();
      }

      if( cmd == SCEX_WAZAEFF_SYNC_CHAPTER )
      {
        #if DEBUG_PRINT_ENABLE
          TAYA_PRINT(" [CLIENT] コマンドチャプタ(=%d)により抜ける, state(rp)=%d\n", cmd, curState );
        #endif
        scmd::SCQUE_ReplaceSkipCmd( m_cmdQue, curState );
        break;
      }
      if( cmd == SC_ACT_WAZA_EFFECT )
      {
        #if DEBUG_PRINT_ENABLE
          TAYA_PRINT(" [CLIENT] 別の技エフェクトコマンド(=%d)により抜ける, state(rp)=%d\n", cmd, curState );
        #endif
        break;
      }
      if( cmd == SC_MAX )
      {
        #if DEBUG_PRINT_ENABLE
          TAYA_PRINT(" [CLIENT] コマンドＭＡＸ(=%d)により抜ける\n", cmd);
        #endif
        break;
      }
      if( cmd == SCEX_VALIABLE_ARGS )
      {
        #if DEBUG_PRINT_ENABLE
          BTL_PRINT(" [CLIENT] 可変個引数命令を一時的に格納\n");
        #endif
        readVarArgsState = curState;
        work->store[ count ].variableArgs.Setup( work->store[count].cmdArgs );
        continue;
      }

      if( tables::IsInclude(cmd, TargetCmdList, GFL_NELEMS(TargetCmdList)) )
      {
        if( readVarArgsState != scmd::QUE_READING_STATE_NULL )
        {
          #if DEBUG_PRINT_ENABLE
            BTL_PRINT(" [CLIENT] Cmd[%d] の保存前に対象の可変個引数をスキップ対象としておく\n", cmd );
          #endif
          scmd::SCQUE_ReplaceSkipCmd( m_cmdQue, readVarArgsState );
          readVarArgsState = static_cast<scmd::QUE_READING_STATE>( scmd::QUE_READING_STATE_NULL );
        }
        scmd::SCQUE_ReplaceSkipCmd( m_cmdQue, curState );
        work->store[ count ].cmdProc = dispatchServerCmdProc( cmd );
        work->store[ count ].cmd = cmd;
        work->store[ count ].bDone = false;
        work->store[ count ].seq = 0;
        ++count;
        #if DEBUG_PRINT_ENABLE
          TAYA_PRINT(" [CLIENT] コマンド( %d )が並列処理対象として保存された  myID=%d / %d 件目\n", cmd, m_myID, count );
        #endif
      }

      readVarArgsState = static_cast<scmd::QUE_READING_STATE>( scmd::QUE_READING_STATE_NULL );

      if( count >= GFL_NELEMS(work->store) ){
        GFL_ASSERT(0);
        break;
      }
    }

    work->cmdCount = count;
  }

  #if DEBUG_PRINT_ENABLE
    TAYA_PRINT("[CLIENT] 先読み終了 ------\n");
  #endif

  scmd::SCQUE_ReadStatePop( m_cmdQue, initState );

#ifdef DEBUG_PRINT_ENABLE
 #undef DEBUG_PRINT_ENABLE
#endif

}
//=============================================================================================================
/**
 * storeWazaEffectSyncDamageParams で保存したワークメンバの内容を並列実行する
 */
//=============================================================================================================
bool BTL_CLIENT::putWazaEffSyncDamage( WAZAEFF_SYNCDAMAGE_CMD_WORK* work )
{
  bool bAllDone = true;

  for(uint32_t i=0; i<work->cmdCount; ++i)
  {
    if( !(work->store[i].bDone) )
    {
      const VariableArgs* oldVa = NULL;
      if( work->store[i].variableArgs.GetCount() ){
        oldVa = this->PushTmpVariableArgsWork( &work->store[i].variableArgs );
      }

      work->store[i].bDone = (this->*(work->store[i].cmdProc))(&(work->store[i].seq), work->store[i].cmdArgs);

      if( work->store[i].variableArgs.GetCount() ){
        this->PopTmpVariableArgsWork( oldVa );
      }

      if( !(work->store[i].bDone) ){
        bAllDone = false;
      }
    }
  }

  return bAllDone;
}

//----------------------------------------------------------------------------------
/**
 * メンバー引き上げメッセージ表示
 *
 * @param   wk
 * @param   seq
 * @param   args    [0]:ClientID, [1]:pokeID
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_MemberOutMsg( int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      bool fClientArg;
      u16  strID;
      strID = checkMemberOutStrID( args[0], args[1], &fClientArg );

      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, strID );
      if( fClientArg ){
        BTLV_STRPARAM_AddArg( &m_strParam, args[0] );
      }
      BTLV_STRPARAM_AddArg( &m_strParam, args[1] );

      m_viewCore->CMD_StartMsg( &m_strParam );
      ++(*seq);
    }
    break;

  case 1:
    if( m_viewCore->CMD_WaitMsg() ){
      return true;
    }
    break;
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * メンバー引き上げメッセージ（●●　もどれ！など）のIDを取得
 *
 * @param   clientID
 * @param   pokeID
 * @param   fClientArg    [out] 文字列の引数としてクライアントIDを使うケースでtrueが入る
 *
 * @retval  u16   文字列ID
 */
//----------------------------------------------------------------------------------
u16 BTL_CLIENT::checkMemberOutStrID( u8 clientID, u8 pokeID, bool* fClientArg )
{
  // 自分が対象
  if( clientID == m_myID )
  {
    *fClientArg = false;

    // なかよし度が有効なら特殊メッセージ
    {
      const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
      if( m_mainModule->GetPokeFriendship(bpp) >= 100 )
      {
        return BTL_STRID_STD_FR_ChangeBack;
      }
    }

    // シングル＆非通信なら相手に与えたダメージ量に応じてメッセージを変える
    if( ( ( m_mainModule->GetRule() == BTL_RULE_SINGLE  ) || 
          ( m_mainModule->GetRule() == BTL_RULE_INTRUDE ) ) &&
        ( m_mainModule->GetCommMode() == BTL_COMM_NONE    ) )
    {
      u32 dmg_per = enemyPokeHPBase_CheckRatio();
      if( dmg_per >= 75 ){
        return BTL_STRID_STD_BackChange5;
      }
      if( dmg_per > 50 ){
        return BTL_STRID_STD_BackChange4;
      }
      if( dmg_per > 25 ){
        return BTL_STRID_STD_BackChange3;
      }
      if( dmg_per > 0 ){
        return BTL_STRID_STD_BackChange2;
      }
      return BTL_STRID_STD_BackChange1;
    }
    else
    {
      return BTL_STRID_STD_BackChange2;
    }
  }
  // 自分以外が対象
  else
  {
    *fClientArg = true;
    if( m_mainModule->IsClientNPC(clientID) ){
      return BTL_STRID_STD_BackChange_NPC;
    }else{
      return BTL_STRID_STD_BackChange_Player;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * メンバー退場アクション
 *
 * args[0]: pos  args[1]: effectNo
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_MemberOut(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlvPos  vpos = m_mainModule->BtlPosToViewPos( args[0] );

      m_viewCore->CMD_ACT_MemberOut_Start(  vpos, args[1] );
      (*seq)++;
    }
    break;

  case 1:
    if( m_viewCore->CMD_ACT_MemberOut_Wait() )
    {
      return true;
    }
    break;
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * メンバー入場アクション
 *
 * @param   wk
 * @param   seq
 * @param   args
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_MemberIn(  int* seq, const int* args )
{
  u8 clientID  = args[0];
  u8 posIdx    = args[1];
  //u8 memberIdx = args[2];
  u8 fPutMsg   = args[3];

  enum {
    SEQ_TRAINER_MSG_SWITCH,
    SEQ_TRAINER_MSG_START,
    SEQ_TRAINER_MSG_WAIT,
    SEQ_MEMBERIN_MSG_START,
    SEQ_MEMBERIN_MSG_WAIT,
    SEQ_MEMBERIN_ACT,
  };

  switch( *seq )
  {
  case SEQ_TRAINER_MSG_SWITCH:
    (*seq) = ( this->IsTrainerMessageEnable_OnLastPokeIn( clientID ) ) ? ( SEQ_TRAINER_MSG_START ) : ( SEQ_MEMBERIN_MSG_START );
    break;

  case SEQ_TRAINER_MSG_START:
    this->StartTrainerMessage( clientID, TrainerMessage::MESSAGE_LAST_POKE_IN );
    (*seq)++;
    break;

  case SEQ_TRAINER_MSG_WAIT:
    if( this->UpdateTrainerMessage() ) {
      (*seq)++;
    }
    break;


  case SEQ_MEMBERIN_MSG_START:
    if( fPutMsg )
    {
      const BTL_POKEPARAM* bpp = m_pokeCon->GetClientPokeDataConst( clientID, posIdx );
      u8 pokeID = bpp->GetID();

      if( !m_mainModule->IsOpponentClientID(m_myID, clientID) )
      {
        if( m_myID == clientID )
        {
          u16 strID = checkMemberPutStrID( bpp );;
          BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, strID );
          BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        }
        else
        {
          BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_PutSingle_Player );
          BTLV_STRPARAM_AddArg( &m_strParam, clientID );
          BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        }
      }
      else
      {
        // 相手が入れ替え
//      if( BTL_MAIN_GetCompetitor(m_mainModule) == BTL_COMPETITOR_TRAINER )
        if( m_mainModule->IsClientNPC(clientID) )
        {
          BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_PutSingle_NPC );
          BTLV_STRPARAM_AddArg( &m_strParam, clientID );
          BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        }
        else
        {
          BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_PutSingle_Player );
          BTLV_STRPARAM_AddArg( &m_strParam, clientID );
          BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        }
      }
      m_viewCore->CMD_StartMsg( &m_strParam );
    }
    (*seq)++;
    break;

  case SEQ_MEMBERIN_MSG_WAIT:
    if( m_viewCore->CMD_WaitMsg() )
    {
      BtlPokePos pokePos = m_mainModule->GetClientPokePos( clientID, posIdx );
      m_viewCore->CMD_StartMemberChangeAct( pokePos, clientID, posIdx );
      (*seq)++;
    }
    break;

  case SEQ_MEMBERIN_ACT:
    if( m_viewCore->CMD_WaitMemberChangeAct() )
    {
      enemyPokeHPBase_Update();
      return true;
    }
    break;
  }
  return false;
}

 /**
  * @brief 「最後のポケモンを繰り出す直前」において、トレーナーメッセージを表示するか？
  * @param clientID  チェック対象クライアント
  * @retval true   トレーナーメッセージを表示する
  * @retval false  トレーナーメッセージを表示しない
  */
 bool BTL_CLIENT::IsTrainerMessageEnable_OnLastPokeIn( u8 clientID ) const
 {
   // トレーナーがいない
   if( !( m_mainModule->IsClientTrainerExist( clientID ) ) ) {
     return false;
   }

   // 最後のポケモンでない
   const BTL_PARTY* party = m_pokeCon->GetPartyDataConst( clientID );
   if( party->GetAliveMemberCount() != 1 ) {
     return false;
   }

   return m_trainerMessageManager->IsMessageExist( m_heapHandle->GetLowerHandle(), clientID, TrainerMessage::MESSAGE_LAST_POKE_IN );
 }

 /**
  * @brief トレーナーメッセージの表示を開始する
  * @param clientID   メッセージを表示するクライアント
  * @param messageID  表示するメッセージのID
  */
 void BTL_CLIENT::StartTrainerMessage( u8 clientID, TrainerMessage::MessageID messageID )
 {
   ClientSeq_TrainerMessage::SetupParam setupParam;
   setupParam.pMainModule = m_mainModule;
   setupParam.pViewSystem = m_viewCore;
   setupParam.pMessageManager = m_trainerMessageManager;
   m_seq_TrainerMessage->Setup( setupParam );
   m_seq_TrainerMessage->Start( clientID, messageID );
 }

 /**
  * @brief トレーナーメッセージの表示を更新する
  * @retval true   表示が終了した
  * @retval false  表示が継続中
  */
 bool BTL_CLIENT::UpdateTrainerMessage( void )
 {
   m_seq_TrainerMessage->Update();
   return m_seq_TrainerMessage->IsFinished();
 }



//----------------------------------------------------------------------------------
/**
 * メンバー繰り出しメッセージ（ゆけっ！●●など）のIDを取得
 *
 * @param   putPoke   繰り出すポケモン
 *
 * @retval  u16
 */
//----------------------------------------------------------------------------------
u16 BTL_CLIENT::checkMemberPutStrID( const BTL_POKEPARAM* putPoke )
{
  u8                   opponentClientID = m_mainModule->GetOpponentClientID( m_myID, 0 );
  const BTL_POKEPARAM* opponentPoke     = m_pokeCon->GetClientPokeDataConst( opponentClientID, 0 );
  u16                  strID            = this->getMemberPutStrID( putPoke, opponentPoke );
  return strID;
}

/**
 * @brief メンバー繰り出しメッセージ( ゆけ！○○　など )のＩＤを取得する
 * @param putPoke       繰り出すポケモン
 * @param opponentPoke  相手ポケモン
 */
u16 BTL_CLIENT::getMemberPutStrID( const BTL_POKEPARAM* putPoke, const BTL_POKEPARAM* opponentPoke )
{
  u16 strId = BTL_STRID_STD_PutSingle;

  if( m_mainModule->GetRule() != BTL_RULE_SINGLE )
  {
    strId = BTL_STRID_STD_PutSingle;  // シングル戦以外はデフォルト
  }
  else if( opponentPoke->IsDead() )
  {
    strId = BTL_STRID_STD_PutSingle;
  }
  else
  {
    fx32 hp_ratio = opponentPoke->GetHPRatio();

    if( hp_ratio >= FX32_CONST(77.5) ){
      strId = BTL_STRID_STD_PutSingle;
    }
    else if( hp_ratio >= FX32_CONST(55) ){
      strId = BTL_STRID_STD_PutChange1;
    }
    else if( hp_ratio >= FX32_CONST(32.5) ){
      strId = BTL_STRID_STD_PutChange2;
    }
    else if( hp_ratio >= FX32_CONST(10) ){
      strId = BTL_STRID_STD_PutChange3;
    }
    else {
      strId = BTL_STRID_STD_PutChange4;
    }
  }

  // デフォルトのままだったらなかよし度チェック
  if( strId == BTL_STRID_STD_PutSingle )
  {
    if( m_mainModule->GetPokeFriendship(putPoke) >= 100 ) {
      strId = BTL_STRID_STD_FR_PutPoke;
    }
  }

  return strId;
}

//----------------------------------------------------------------------------------
/**
 * 標準メッセージ出力
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_MSG_Std( int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  return scproc_msgStdCore( seq, args[0], &args[1] );
}
//----------------------------------------------------------------------------------
/**
 * 標準メッセージ出力 + SE再生
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_MSG_StdSE( int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  static int subSeq = 0;

  switch( *seq ){
  case 0:
    subSeq = 0;
    ++(*seq);
    /* fallthru */
  case 1:
    if( scproc_msgStdCore(&subSeq, args[0], &args[2]) ){
      m_soundSys->PlaySE( args[1] );
      ++(*seq);
    }
    break;
  case 2:
    if( m_soundSys->IsSEFinished(args[1]) ){
      return true;
    }
  }
  return false;
}
//------------------------------------------
// 標準メッセージ出力共通処理
//------------------------------------------
bool BTL_CLIENT::scproc_msgStdCore( int* seq, u16 strID, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  switch( *seq ){
  case 0:
    BTL_PRINT("[CLIENT] PutStdMsg strID=%d, args0=%d\n", strID, args[0]);
    m_viewCore->CMD_StartMsgStd( strID, args );
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_WaitMsg() ){
      return true;
    }
    break;
  }
  return false;
}


//----------------------------------------------------------------------------------
/**
 * セットメッセージ出力
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_MSG_Set(  int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }
  return scproc_msgSetCore( seq, args[0], &args[1] );
}
//----------------------------------------------------------------------------------
/**
 * セットメッセージ出力 + SE再生
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_MSG_SetSE(  int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  static int subSeq = 0;

  switch( *seq ){
  case 0:
    subSeq = 0;
    ++(*seq);
    /* fallthru */
  case 1:
    if( scproc_msgSetCore(&subSeq, args[0], &args[2]) ){
      m_soundSys->PlaySE( args[1] );
      ++(*seq);
    }
    break;
  case 2:
    if( m_soundSys->IsSEFinished(args[1]) ){
      return true;
    }
  }
  return false;
}
 //------------------------------------------
// セットメッセージ出力共通処理
//------------------------------------------
bool BTL_CLIENT::scproc_msgSetCore( int* seq, u16 strID, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  switch( *seq ){
  case 0:
    m_viewCore->CMD_StartMsgSet( strID, args );
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_WaitMsg() ){
      return true;
    }
    break;
  }
  return false;
}


//----------------------------------------------------------------------------------
/**
 * ワザメッセージ出力
 *
 * args[0] 技を使用するポケモンのID
 * args[1] 使用する技が全力技か？
 * args[2] 使用する技の技No.
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_MSG_Waza(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      return true;
    }

    m_viewCore->CMD_StartMsgWaza( args[0], args[2], args[1] );
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_WaitMsg() )
    {
      return true;
    }
  }
  return false;
}
/**
 *  ワザエフェクト
 *  args  [0]:攻撃ポケ位置  [1]:防御ポケ位置  [2]:ワザID  [3]:ワザサブID
 * 　　　 [4]:関連コマンドとの同期フラグ
 *        [5]:全力技の種類( ZenryokuWazaKind )
 */
bool BTL_CLIENT::scProc_ACT_WazaEffect(  int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  switch( *seq ) {
  case 0:
    {
      WazaEffectCmdProcResult result = scproc_wazaEffProc_Start( args );

      switch( result ){
        case WAZAEFF_CMD_RESULT_PROC_NO_SYNC:
          (*seq) = 1;
          break;

        case WAZAEFF_CMD_RESULT_PROC_SYNC:
          m_bWazaEffectDone = false;
          m_bSyncEffectDone = false;
          this->storeWazaEffectSyncDamageParams( &m_wazaEffDmgSyncWork );
          (*seq) = 2;
          break;

        case WAZAEFF_CMD_RESULT_NO_PROC:
        default:
          return true;
      }
    }
    break;

  case 1:
    if( m_viewCore->CMD_ACT_WazaEffect_Wait() ){
      return true;
    }
    break;

  case 2:
    if( !m_bWazaEffectDone )
    {
      m_bWazaEffectDone = m_viewCore->CMD_ACT_WazaEffect_Wait();
      if(m_bWazaEffectDone){
        BTL_PRINT("ワザエフェクトの方は終わった\n");
      }
    }
    if( !m_bSyncEffectDone )
    {
      m_bSyncEffectDone = this->putWazaEffSyncDamage( &m_wazaEffDmgSyncWork );
      if( m_bSyncEffectDone ){
        BTL_PRINT("同時処理エフェクトの方は終わった\n");
      }
    }
    if( m_bWazaEffectDone && m_bSyncEffectDone ){
      return true;
    }
    break;
  }

  return false;

  //#undef WAZAEFF_SYNC_DAMAGE_EFF
}

/**
 * [scProc_ACT_WazaEffect の下請け] 引数解釈し、ワザエフェクト開始コマンドを実行する
 * @param args　scProc_ACT_WazaEffect引数
 * @return ワザエフェクト開始コマンド処理結果
 */
BTL_CLIENT::WazaEffectCmdProcResult
  BTL_CLIENT::scproc_wazaEffProc_Start( const int* args )
{
  WazaNo    waza             = (WazaNo)args[2];
  u8        atPokePos        = args[0];
  u8        defPokePos       = args[1];
  ZenryokuWazaKind zWazaKind = (ZenryokuWazaKind)args[5];

  //@バトル変更箇所
  //テレビナビ用データを保存する(最後に使用した技)
  BtlRule rule = m_mainModule->GetRule();
  if( m_mainModule->IsPlayerSide( MainModule::PosToSide( rule, atPokePos ) ) )
  {
    const BTL_POKEPARAM *bpp = m_pokeCon->GetFrontPokeDataConst( atPokePos );
    m_mainModule->SetTvNaviData_UseWaza(bpp,waza);
  }
  
  // エフェクトみない設定なら何もしない
  if( ( !m_mainModule->IsWazaEffectEnable() ) )
  {
    return WAZAEFF_CMD_RESULT_NO_PROC;
  }

  BtlvWazaEffect_TurnType  turnType          = (BtlvWazaEffect_TurnType)args[3];
  bool                     bSyncDamageEffect = args[4];
  WazaTarget               wazaRange         = checkWazaRange( waza, atPokePos );

  // 味方攻撃エフェクトを出さない設定の時、何もしない
  if( wazaEff_IsOmitFriendAttackEffect(waza, wazaRange, atPokePos, defPokePos)){
    return WAZAEFF_CMD_RESULT_NO_PROC;
  }

  #if WAZAEFF_SYNC_DAMAGE_EFF
    m_viewCore->CMD_ACT_WazaEffect_Start( atPokePos, defPokePos, waza, wazaRange, turnType, 0 , bSyncDamageEffect );
    BTL_PRINT("[BTL_CLIENT] WazaEffect  wazaNo:%d,  atkPos(%d), defPos(%d), sync(%d)\n", waza, atPokePos, defPokePos, bSyncDamageEffect);

    return (bSyncDamageEffect)?
              WAZAEFF_CMD_RESULT_PROC_SYNC : WAZAEFF_CMD_RESULT_PROC_NO_SYNC;
  #else
    BTL_PRINT("[CLIENT]従来のワザエフェクト処理\n");
    m_viewCore->CMD_ACT_WazaEffect_Start( atPokePos, defPokePos, waza, wazaRange, turnType, 0 , bSyncDamageEffect );
    (*seq)++;
  #endif
}
/**
 * 味方対象攻撃の際、エフェクトを出さない設定かどうかを判定する
 * @param waza
 * @param wazaRange
 * @param atkPokePos
 * @param defPokePos
 * @return  エフェクト出さない設定なら true 
 */
bool
BTL_CLIENT::wazaEff_IsOmitFriendAttackEffect( WazaNo waza, WazaTarget wazaRange, BtlPokePos atkPokePos, BtlPokePos defPokePos )
{
  // 防御位置指定がされていない場合、通常処理
  if( defPokePos == BTL_POS_NULL ){
    return false;
  }

  // 味方が対象ではない場合、通常処理
  BtlRule rule = m_mainModule->GetRule();
  if( !MainModule::IsFriendPokePos( rule, atkPokePos, defPokePos ) ){
    return false;
  }

  // 効果範囲「通常ポケ」以外は、通常処理
  if( wazaRange != pml::wazadata::TARGET_OTHER_SELECT ){
    return false;
  }

  // 味方エフェクト有効フラグが設定されているワザは、通常処理
  if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_EffectToFriend) ){
    return false;
  }


  // ここまで来たら、エフェクトを出さない特殊処理
  return true;
}

/**
 *  ワザ効果範囲取得（のろい用）
 */
WazaTarget BTL_CLIENT::checkWazaRange( WazaID waza, BtlPokePos atPokePos )
{
  const BTL_POKEPARAM* poke = m_pokeCon->GetFrontPokeDataConst( atPokePos );
  return calc::GetWazaTarget( waza, poke );
}

/**
 *  溜めワザ（そらをとぶ等）による、表示オン／オフ
 *  args  [0]:対象ポケID  [1]:true=非表示, false=表示
 */
bool BTL_CLIENT::scProc_ACT_TameWazaHide(  int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
    {
      u8 pokeID = args[0];
      BtlvPos vpos = m_mainModule->PokeIDtoViewPos( m_pokeCon, pokeID );

      BTLV_VANISH_FLAG  flag = (args[1])? BTLV_VANISH_ON : BTLV_VANISH_OFF;
      BTL_PRINT("[tamehide] vpos=%d, flag=%d\n", vpos, flag);
      m_viewCore->CMD_ACT_TameWazaHide( vpos, flag );
      (*seq)++;
    }
    break;

  case 1:
    //sangoでは未使用
    //return ( !BTLV_EFFECT_CheckExecute() );
    return true;
  }
  return false;
}
/**
 * 【アクション】単体ダメージ演出  [0]:pokeID, [1]:affAbout [2]:wazaID
 */
bool BTL_CLIENT::scProc_ACT_WazaDmg(  int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    WazaNo waza;
    u8 defPokePos;
    BtlTypeAffAbout  affAbout;

    defPokePos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
    affAbout    = (BtlTypeAffAbout)args[1];
    waza         = (WazaNo)args[2];

    m_viewCore->CMD_ACT_DamageEffectSingle_Start( waza, defPokePos, affAbout );
    (*seq)++;
  }
  break;

  case 1:
    if( m_viewCore->CMD_ACT_DamageEffectSingle_Wait() )
    {
      return true;
    }
    break;
  }
  return false;
}
/**
 * @brief 【アクション】複数体一斉ダメージ処理
 *
 * @param args[0] 技を受けるポケモンの数
 * @param args[1] 簡易効果相性
 * @param args[2] 技No.
 *
 * @note 拡張引数
 *       ( 技を受けるポケモンのID, 個別の効果相性 ) のセットが、技を受けるポケモンの数だけ繰り返し存在
 */
bool BTL_CLIENT::scProc_ACT_WazaDmg_Plural(  int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    u16               poke_cnt  = args[0];
    BtlTypeAffAbout   aff_about = (BtlTypeAffAbout)args[1];
    WazaNo            waza      = (WazaNo)args[2];

    u8 pokeID[ BTL_EXIST_POS_MAX ];
    BtlTypeAff pokeAffinity[ BTL_EXIST_POS_MAX ];
    for(int i=0; i<poke_cnt; ++i)
    {
      pokeID[i] = this->GetVariableArgs( i*2 );
      pokeAffinity[i] = static_cast<BtlTypeAff>( this->GetVariableArgs( i*2+1 ) );
    }
    m_viewCore->CMD_ACT_DamageEffectPlural_Start( poke_cnt, aff_about, pokeID, pokeAffinity, waza );
    (*seq)++;
  }
  break;

  case 1:
    if( m_viewCore->CMD_ACT_DamageEffectPlural_Wait() )
    {
      return true;
    }
    break;
  }
  return false;
}
/**
 * 【アクション】一撃必殺ワザ処理
 */
bool BTL_CLIENT::scProc_ACT_WazaIchigeki(  int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );

    m_viewCore->CMD_ACT_SimpleHPEffect_Start( pos );
    (*seq)++;
  }
  break;

  case 1:
    if( m_viewCore->CMD_ACT_SimpleHPEffect_Wait() )
    {
      m_viewCore->CMD_StartMsgStd( BTL_STRID_STD_Ichigeki, args );
      (*seq)++;
    }
    break;

  case 2:
    if( m_viewCore->CMD_WaitMsg() )
    {
      return true;
    }
    break;
  }
  return false;
}
/**
 * 【アクション】状態異常アイコン  args[0]:pokeID  args[1]:状態異常コード
 */
bool BTL_CLIENT::scProc_ACT_SickIcon(  int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
    BtlvPos vpos = m_mainModule->BtlPosToViewPos( pos );
    m_viewCore->CMD_EFFECT_SetGaugeStatus( static_cast<PokeSick>(args[1]), vpos );
    (*seq)++;
  }
  break;

  case 1:
    return true;
  }

  return false;
}
/**
 * 【アクション】こんらん自爆ダメージ
 */
bool BTL_CLIENT::scProc_ACT_ConfDamage(  int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  switch( *seq ) {
  case 0:
    {
      BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
      m_viewCore->CMD_ACT_DamageEffectSingle_Start( WAZANO_HATAKU, pos, pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_ACT_DamageEffectSingle_Wait() )
    {
      return true;
    }
    break;
  }
  return false;
}
/**
 * 【アクション】ポケモンひんし
 */
bool BTL_CLIENT::scProc_ACT_Dead( int* seq, const int* args )
{
  const u8   pokeID            = static_cast<u8>( args[0] );
  const bool isKillCountEffect = static_cast<bool>( args[1] );
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );
      m_viewCore->CMD_StartDeadAct( pos, isKillCountEffect );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_WaitDeadAct() )
    {
      BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
      bpp->Clear_ForDead();
      updateClientPublicInformation_FormNo( *bpp );  // 死亡によりフォルムが元に戻ることがあるので、他クライアントへの公開情報を更新する
      return true;
    }
    break;
  }
  return false;
}
/**
 * 【アクション】ポケモン生き返り
 */
bool BTL_CLIENT::scProc_ACT_Relive(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
      m_viewCore->CMD_StartReliveAct( pos );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_WaitReliveAct() )
    {
      return true;
    }
    break;
  }
  return false;
}
/**
 * 【アクション】能力ランクダウン
 *
 * args[0] 対象ポケモンのID
 * args[1] 追加効果の種類( WazaRankEffect )
 * args[2] 能力ランクが何段階ダウンするか( 正数 )
 */
bool BTL_CLIENT::scProc_ACT_RankDown(  int* seq, const int* args )
{
  BtlPokePos pos           = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
  u8        vpos           = m_mainModule->BtlPosToViewPos(  pos );
  u8        rankDownVolume = args[2];

  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      return true;
    }
    if( !m_mainModule->IsWazaEffectEnable() ){
      return true;
    }

    m_viewCore->CMD_StartRankDownEffect( vpos, rankDownVolume );
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_WaitRankEffect(vpos) ){
      return true;
    }
  }
  return false;
}
/**
 * 【アクション】能力ランクアップ
 *
 * args[0] 対象ポケモンのID
 * args[1] 追加効果の種類( WazaRankEffect )
 * args[2] 能力ランクが何段階アップするか
 */
bool BTL_CLIENT::scProc_ACT_RankUp(  int* seq, const int* args )
{
  BtlPokePos pos          = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
  u8         vpos         = m_mainModule->BtlPosToViewPos( pos );
  u8         rankUpVolume = args[2];

  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      return true;
    }
    if( !m_mainModule->IsWazaEffectEnable() ){
      return true;
    }

    m_viewCore->CMD_StartRankUpEffect( vpos, rankUpVolume );
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_WaitRankEffect(vpos) ){
      return true;
    }
  }
  return false;
}


/**
 * @brief ポケモンに汚れをセットする
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理継続中
 *
 * args[0] 汚れをセットするポケモンのID
 * args[1] セットする汚れの種類( DirtType )
 */
bool BTL_CLIENT::scProc_OP_SetDirt( int* seq, const int* args )
{
  const u8       pokeID   = args[0];
  const DirtType dirtType = static_cast<DirtType>( args[1] );

  BTL_POKEPARAM* poke = m_pokeCon->GetPokeParam( pokeID );
  poke->SetDirtType( dirtType );
  return true;
}

/**
 * @brief 天候の経過ターン数をインクリメントする
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理継続中
 */
bool BTL_CLIENT::scProc_OP_IncWeatherPassedTurn( int* seq, const int* args )
{
  FieldStatus* field = m_pokeCon->GetFieldStatus();
  field->IncWeatherPassedTurn();
  return true;
}

/**
 * @brief ポケモンの行動の特殊優先度を設定する
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理継続中
 *
 * args[0]  設定対象ポケモンのID
 * args[1]  設定する特殊優先度
 */
bool BTL_CLIENT::scProc_OP_SetSpActPriority( int* seq, const int* args )
{
  const u8 pokeID   = args[0];
  const u8 priority = args[1];

  BTL_POKEPARAM* poke = m_pokeCon->GetPokeParam( pokeID );
  poke->SetSpActPriority( priority );
  return true;
}

/**
 * @brief 全力技を使用したことを記録する
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理継続中
 *
 * args[0] 全力技を使用したポケモンのID
 */
bool BTL_CLIENT::scProc_OP_SetZenryokuWazaUsed( int* , const int* args )
{
  u16                 pokeId             = args[0];
  u8                  clientId           = MainModule::PokeIDtoClientID( pokeId );
  ZenryokuWazaStatus* zenryokuWazaStatus = m_mainModule->GetZenryokuWazaStatus( clientId );

  if( zenryokuWazaStatus == NULL )
  {
    GFL_ASSERT(0);
    return true;
  }

  zenryokuWazaStatus->SetZenryokuWazaUsed();
  return true;
}

/**
 * @brief オーラ技開始演出
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   演出終了
 * @retval false  演出中
 *
 * args[0]  演出対象ポケモンのID
 * args[1]  ターゲットの立ち位置
 * args[2]  全力効果発動時に表示するメッセージの引数1
 * args[3]  全力効果発動時に表示するメッセージの引数2
 * args[4]  全力効果発動時に表示するメッセージの引数3
 * args[5]  全力効果発動時に表示するメッセージの文字列ID( BTL_STRID_SET_xxxx )
 * args[6]  実行する全力技の技No.( ただし、実行する技が「ゆびをふる」等の派生技の場合は、派生技の技No. )
 */
bool BTL_CLIENT::scProc_ACT_ZenryokuWazaBegin( int* seq, const int* args )
{
  BtlPokePos pokePos       = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
  BtlPokePos targetPos     = static_cast<BtlPokePos>( args[1] );
  u8         messageParam1 = args[2];
  u8         messageParam2 = args[3];
  u8         messageParam3 = args[4];
  u16        messageStrId  = args[5];
  WazaNo     wazaNo        = static_cast<WazaNo>( args[6] );

  switch( *seq )
  {
  case 0:
    if( IsChapterSkipMode() ) {
      return true;
    }
    m_viewCore->CMD_ACT_ZenryokuWazaBegin_Start( pokePos, targetPos , wazaNo, messageStrId, messageParam1, messageParam2, messageParam3 );
    ( *seq )++;
    break;

  case 1:
    if( m_viewCore->CMD_ACT_ZenryokuWazaBegin_Wait() ) {
      return true;
    }
  }
  return false;
}

/**
 * @brief オーラ技終了演出
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   演出終了
 * @retval false  演出中
 *
 * args[0]  演出対象ポケモンのID
 */
bool BTL_CLIENT::scProc_ACT_ZenryokuWazaEnd( int* seq, const int* args )
{
  BtlPokePos pokePos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );

  switch( *seq )
  {
  case 0:
    if( IsChapterSkipMode() ) {
      return true;
    }
    m_viewCore->CMD_ACT_ZenryokuWazaEnd_Start( pokePos );
    ( *seq )++;
    break;

  case 1:
    if( m_viewCore->CMD_ACT_ZenryokuWazaEnd_Wait() ) 
    {
      return true;
    }
  }
  return false;
}

/**
 * @brief 「お手入れボタン」の表示を開始する
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理中
 *
 * @fix NMCat[1778]【ポケリフレ】発生しないと設定されている症状が発生する
 *
 * ●経緯
 * ・バトル側では、技データに設定されている汚れをセットし、
 *   バトル呼び出し元で、付着してはいけない汚れだったら、汚れを取り除いた上で、リフレに遷移するはずだった。
 * ・バトル呼び出し元で、付着してはいけない汚れを取り除く処理が機能していなかった。
 * ・バトル呼び出し元で、付着してはいけない汚れを取り除く処理が機能を実装した場合、
 *   お手入れボタンを押したのに、汚れが無い状態でリフレ画面に遷移することになり、その現象が不具合であると判断された。
 *
 * ●対処
 * ・本来なら、汚れをセットする時点( 技を撃ったり受けたりした時点 )で判定すべきだが、そうするには、付着可能かどうかのデータをあらかじめ読み込んでおく必要がある。
 * ・データを読み込むなら、戦闘開始時に行う必要がある。
 * ・戦闘開始までの速度に影響を出したくない。
 * 以上の理由から、
 * 技を撃ったり受けたりするたびに、付着可能な汚れかどうかをチェックするのではなく、
 * お手入れボタンを表示するタイミングで、ポケモンにセットされている汚れが付着可能な汚れかどうかをチェックし、付着してはいけない汚れだったら汚れを取り除く
 * という対処を行う。
 */
bool BTL_CLIENT::scProc_ACT_OteireButtonDisplay( int* seq, const int* args )
{
  static PokeTool::KawaigariParamCareCoreManager* kawaigariData = NULL;

  switch( *seq )
  {
  case 0:

    // 念のため
    if( kawaigariData != NULL )
    {
      GFL_ASSERT(0);
      GFL_SAFE_DELETE( kawaigariData );
    }

    // お手入れデータの読み込み開始
    {
      gfl2::heap::HeapBase* tempHeap = m_viewCore->GetTempHeap()->GetLowerHandle();
      gfl2::fs::AsyncFileManager* fileManager = m_viewCore->GetAsyncFileManager();
      kawaigariData = GFL_NEW( tempHeap ) PokeTool::KawaigariParamCareCoreManager( tempHeap, tempHeap, fileManager );
    }

    (*seq)++;
    return false;

  case 1:

    // 不正な汚れを取り除く
    if( kawaigariData->IsLoaded() )
    {
      m_mainModule->RemoveIllegalDirt( kawaigariData, m_myID );
      GFL_SAFE_DELETE( kawaigariData );
      (*seq)++;
    }
    return false;

  case 2:
    {
      u8 oteireTargetPokeID = BTL_POKEID_NULL;
      BTL_POKEPARAM* oteireTarget = NULL;

      // お手入れ対象が存在しない
      {
        bool isFound = false;
        Oteire::GetOteireTarget( &isFound, &oteireTargetPokeID, m_mainModule, m_pokeCon, m_myID );
        if( !isFound )
        {
          return true;
        }
      }

      // お手入れ対象が取得できない
      oteireTarget = m_pokeCon->GetPokeParam( oteireTargetPokeID );
      if( oteireTarget == NULL )
      {
        return true;
      }

      if( Oteire::IsOteireTarget( *oteireTarget ) )
      {
        m_seq_WinWild->SetOteireButtonDisplayed();
        m_seq_Capture->SetOteireButtonDisplayed();
        m_viewCore->CMD_OteireButton_StartDisp();
      }
    }
    return true;

  default:
    GFL_ASSERT(0);
    return true;
  }
}

/**
 * @brief 乱入( 助けを呼ぶ演出 )
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理中
 *
 * args[0]  助けを呼ぶポケモンの立ち位置
 */
bool BTL_CLIENT::scProc_ACT_IntrudeCall( int* seq, const int* args )
{
  switch( *seq )
  {
  case 0:
    {
      BtlPokePos callPokePos = static_cast<BtlPokePos>( args[0] );
      m_viewCore->CMD_Intrude_Call_Start( callPokePos );
    }
    ++( *seq );
    return false;


  case 1:
    if( m_viewCore->CMD_Intrude_Call_Wait() )
    {
      return true;
    }
    return false;


  default:
    GFL_ASSERT(0);
    return true;
  }
}

/**
 * @brief 乱入( 乱入ポケモンの登場演出 )
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理中
 *
 * args[0]  助けを呼んだポケモンの立ち位置
 * args[1]  乱入ポケモンの立ち位置
 */
bool BTL_CLIENT::scProc_ACT_IntrudeAppear( int* seq, const int* args )
{
  switch( *seq )
  {
  case 0:
    {
      BtlPokePos callPokePos    = static_cast<BtlPokePos>( args[0] );
      BtlPokePos intrudePokePos = static_cast<BtlPokePos>( args[1] );
      m_viewCore->CMD_Intrude_Appear_Start( callPokePos, intrudePokePos );
    }
    ++( *seq );
    return false;


  case 1:
    if( m_viewCore->CMD_Intrude_Appear_Wait() )
    {
      return true;
    }
    return false;


  default:
    GFL_ASSERT(0);
    return true;
  }
}

/**
 * @brief 乱入( 助けが来なかった場合の演出 )
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理中
 *
 * args[0]  助けを呼んだポケモンの立ち位置
 */
bool BTL_CLIENT::scProc_ACT_IntrudeNotAppeared( int* seq, const int* args )
{
  switch( *seq )
  {
  case 0:
    {
      BtlPokePos callPokePos = static_cast<BtlPokePos>( args[0] );
      m_viewCore->CMD_Intrude_NotAppeared_Start( callPokePos );
    }
    ++( *seq );
    return false;


  case 1:
    if( m_viewCore->CMD_Intrude_NotAppeared_Wait() )
    {
      return true;
    }
    return false;


  default:
    GFL_ASSERT(0);
    return true;
  }
}

/**
 * @brief ターン開始時の演出
 * @param seq  現在のシーケンス
 * @param args サーバーコマンド引数
 * @retval true   コマンド処理終了
 * @retval false  コマンド処理中
 */
bool BTL_CLIENT::scProc_ACT_TurnStart( int* seq, const int* args )
{
  switch( *seq )
  {
  case 0:
    // 録画再生時のみ有効
    if( !this->IsRecPlayerMode() )
    {
      return true;
    }
    m_viewCore->CMD_RecDispTurnGauge_Start();
    ++( *seq );
    return false;


  case 1:
    if( m_viewCore->CMD_RecDispTurnGauge_Wait() )
    {
      return true;
    }
    return false;


  default:
    GFL_ASSERT(0);
    return true;
  }
}


//---------------------------------------------------------------------------------------
/**
 *  天候変化開始
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACTOP_WeatherStart(  int* seq, const int* args )
{
  static const struct {
    u16 strID;
    u16 effectID;
  }ParamTbl[] = {
    { BTL_STRID_STD_ShineStart,   BTLEFF_WEATHER_HARE       },
    { BTL_STRID_STD_ShineStart,   BTLEFF_WEATHER_HARE       },
    { BTL_STRID_STD_RainStart,    BTLEFF_WEATHER_AME        },
    { BTL_STRID_STD_SnowStart,    BTLEFF_WEATHER_ARARE      },
    { BTL_STRID_STD_StormStart,   BTLEFF_WEATHER_SUNAARASHI },
    //@バトル変更箇所
    { BTL_STRID_STD_RainStormStart, BTLEFF_WEATHER_STORM },
    { BTL_STRID_STD_DayStart,       BTLEFF_WEATHER_DAY },
    { BTL_STRID_STD_TurbulenceStart,BTLEFF_WEATHER_TURBULENCE },
  };

  BtlWeather weather = args[0];

  switch( *seq ){
  case 0:
    m_fldSim->SetWeather( args[0], args[1], args[2], args[3] );

    if( weather < GFL_NELEMS(ParamTbl) )
    {
      m_viewCore->CMD_ChangeWheather( weather );
    }

    (*seq)++;
    break;

  case 1:
    if( m_viewCore->CMD_ChangeWheather_Wait() )
    {
      // チャプタスキップ時には演出だけ呼ぶ
      if( IsChapterSkipMode() ){
        return true;
      }
      m_viewCore->CMD_StartMsgStd( ParamTbl[weather].strID, NULL );
      (*seq)++;
    }
    break;

  case 2:
    if( m_viewCore->CMD_WaitMsg() ){
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  ターンチェックによる天候の終了処理
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACTOP_WeatherEnd(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlWeather weather = static_cast<BtlWeather>(args[0]);
      u16  msgID;
      switch( weather ){
      case BTL_WEATHER_SHINE: msgID = BTL_STRID_STD_ShineEnd; break;
      case BTL_WEATHER_RAIN:  msgID = BTL_STRID_STD_RainEnd; break;
      case BTL_WEATHER_SAND:  msgID = BTL_STRID_STD_StormEnd; break;
      case BTL_WEATHER_SNOW:  msgID = BTL_STRID_STD_SnowEnd; break;
      //@バトル変更箇所
      case BTL_WEATHER_STORM:       msgID = BTL_STRID_STD_RainStormEnd; break;
      case BTL_WEATHER_DAY:         msgID = BTL_STRID_STD_DayEnd; break;
      case BTL_WEATHER_TURBULENCE:  msgID = BTL_STRID_STD_TurbulenceEnd; break;
//      case BTL_WEATHER_MIST:  msgID = BTL_STRID_STD_MistEnd; break;
      default:
        GFL_ASSERT(0); // おかしな天候ID
        return true;
      }
      m_viewCore->CMD_StartMsgStd( msgID, NULL );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_WaitMsg() )
    {
      //変更 Ariizumi 120827
      m_viewCore->CMD_ChangeWheather( BTL_WEATHER_NONE );
      //m_viewCore->CMD_AddEffect( BTLEFF_RESET_WEATHER );
      (*seq)++;
    }
    break;
  case 2:
    //変更 Ariizumi 120827
    if( m_viewCore->CMD_ChangeWheather_Wait() )
    //if( m_viewCore->CMD_WaitEffect() )
    {
      m_fldSim->EndWeather( );
      return true;
    }

    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  メガ進化
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACTOP_MegaEvo(  int* seq, const int* args )
{
  u8 pokeID = args[0];
  BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );
  BtlvPos vpos = m_mainModule->BtlPosToViewPos( pos );

  switch( *seq ){
  case 0:
    {
      BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );

      //@バトル変更箇所
      //テレビナビ用対応
      BtlRule rule = m_mainModule->GetRule();
      if( m_mainModule->IsPlayerSide(MainModule::PosToSide(rule,pos)) )
      {
        m_mainModule->SetTvNaviData_MegaEvo(bpp);
      }

      if( m_mainModule->IsLongVersionMegaEvolve(bpp) )
      {
        //メガ進化ロングバージョン再生用手続き
        BTL_PRINT("メガ進化ロングバージョン!!\n");
      }

      u8 clientID = MainModule::PokeIDtoClientID(pokeID);
      // @momijiバトル変更箇所
      if (bpp->IsAbleToUltraBurst())
      {
        s_SetUltraBursted(clientID);
      }
      else
      {
        s_SetMegaEvolved(clientID);
      }
      
      //@バトル変更箇所
      //レックウザ専用メッセージ
      if( bpp->GetMonsNo() == MONSNO_REKKUUZA )
      {
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_MegaRayquazaEvoStart );
        BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        BTLV_STRPARAM_AddArg( &m_strParam, clientID );
      }
      //ヒカリドラゴン専用メッセージ
      else if (bpp->IsAbleToUltraBurst())
      {
        BTLV_STRPARAM_Setup(&m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_UltraBurstStart);
        BTLV_STRPARAM_AddArg(&m_strParam, pokeID);
      }
      else
      {
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_MegaEvoStart );
        BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        BTLV_STRPARAM_AddArg( &m_strParam, clientID );
        BTLV_STRPARAM_AddArg( &m_strParam, bpp->GetItem() );
      }
      BTLV_STRPARAM_AddArg( &m_strParam, m_mainModule->GetClientTrainerMegaItemId(clientID) );

      m_viewCore->CMD_StartMsg( &m_strParam );
      (*seq)++;
    }
    break;

  case 1:
    if( m_viewCore->CMD_WaitMsg() )
    {
      BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
      bool bLongVer = false;

      // @momijiバトル変更箇所
      // ヒカリドラゴン進化
      if (bpp->IsAbleToUltraBurst())
      {
        bpp->UltraBurst();
        m_mainModule->RecUltraBursted(MainModule::PokeIDtoClientID(pokeID));
        // メガシンカしたことにしない
      }
      else
      {
        bpp->MegaEvo();
        m_mainModule->RecMegaEvolved(MainModule::PokeIDtoClientID(pokeID));
      }
      
      if( m_mainModule->IsLongVersionMegaEvolve(bpp) )
      {
        m_mainModule->RecLongVersionMegaEvolved(bpp);
        bLongVer = true;
      }

      // 他クライアントへの公開情報を更新する
      updateClientPublicInformation_FormNo( *bpp );

      m_viewCore->CMD_StartMegaEvo( vpos, bLongVer );
      (*seq)++;
    }
    break;

  case 2:
    if( m_viewCore->CMD_WaitMegaEvo(vpos) )
    {
      BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
      //@バトル変更箇所
      //レックウザ専用メッセージ
      if( bpp->GetMonsNo() == MONSNO_REKKUUZA )
      {
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_MegaRayquazaEvo );
        BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
      }
      //ヒカリドラゴン専用メッセージ
      else if (bpp->IsUltraBurst())
      {
        BTLV_STRPARAM_Setup(&m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_UltraBurst);
        BTLV_STRPARAM_AddArg(&m_strParam, pokeID);
      }
      else
      {
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_MegaEvo );
        BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
      }
      m_viewCore->CMD_StartMsg( &m_strParam );
      (*seq)++;
    }
    break;

  case 3:
    if( m_viewCore->CMD_WaitMsg() ){
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  シンプルなHPゲージ増減処理
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_SimpleHP(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
      m_viewCore->CMD_ACT_SimpleHPEffect_Start( pos );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_ACT_SimpleHPEffect_Wait() )
    {
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  装備アイテム使用エフェクト
 *  args .. [0]:対象ポケID  [1]:きのみフラグ
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_UseItem(  int* seq, const int* args )
{
  u8 pokeID = args[0];
  BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );

  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      return true;
    }
    if( !m_mainModule->IsWazaEffectEnable() ){
      return true;
    }

    if( args[1] )
    {
      m_viewCore->CMD_KinomiAct_Start( pos );
      (*seq)++;
    }else
    {
      m_viewCore->CMD_ItemAct_Start( pos );
      (*seq) += 2;
    }
    break;

  case 1:
    if( m_viewCore->CMD_KinomiAct_Wait( pos ) )
    {
      return true;
    }
    break;

  case 2:
    if( m_viewCore->CMD_ItemAct_Wait( pos ) )
    {
      return true;
    }
    break;

  }

  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  ワザ演出前のきのみ食べ演出
 *  args .. [0]:対象ポケ数  [1]?[5]:対象ポケID
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_KinomiPrevWaza(  int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }
  if( !m_mainModule->IsWazaEffectEnable() ){
    return true;
  }

  static u32 procIdx = 0;

  u32 poke_cnt = args[0];
  BtlPokePos  pos = BTL_POS_NULL;
  if( *seq ){
    pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[1 + procIdx] );
  }

  switch( *seq ){
  case 0:
    procIdx = 0;
    pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[1] );
    ++(*seq);
    /* fallthru */
  case 1:
    if( procIdx >= poke_cnt ){
      return true;
    }
    else{
      m_viewCore->CMD_KinomiAct_Start( pos );
      ++(*seq);
    }
    break;
    
  case 2:
    if( m_viewCore->CMD_KinomiAct_Wait( pos ) ){
      ++procIdx;
      (*seq) = 1;
    }
    break;
  }
  return false;
}

//---------------------------------------------------------------------------------------
/**
 *  強制ひんし演出
 *  args .. [0]:対象ポケID  [1]:エフェクトタイプ
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_Kill(  int* seq, const int* args )
{
  u8 pokeID = args[0];
  //u8 effType = args[1];

  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );
      m_viewCore->CMD_ACT_SimpleHPEffect_Start( pos );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_ACT_SimpleHPEffect_Wait() )
    {
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  ムーブ処理
 *  args .. [0]:対象クライアントID  [1]:対象ポケモン位置1  [2]:対象ポケモン位置2
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACTOP_Move(  int* seq, const int* args )
{

  switch( *seq ){
  case 0:
    {
      u8 clientID = args[0];
      u8 pos1 = args[1];
      u8 pos2 = args[2];
      u8 posIdx1 = m_mainModule->BtlPosToPosIdx( pos1 );
      u8 posIdx2 = m_mainModule->BtlPosToPosIdx( pos2 );
      u8 vpos1 = m_mainModule->BtlPosToViewPos( pos1 );
      u8 vpos2 = m_mainModule->BtlPosToViewPos( pos2 );

      BTL_PARTY* party = m_pokeCon->GetPartyData( clientID );
      party->SwapMembers( posIdx1, posIdx2 );

      BTL_N_Printf( DBGSTR_CLIENT_MoveAction, pos1, pos2, vpos1, vpos2 );

      m_viewCore->CMD_ACT_MoveMember_Start( clientID, vpos1, vpos2, posIdx1, posIdx2 );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_ACT_MoveMember_Wait() )
    {
      return true;
    }
    break;
  }
  return false;
}

//---------------------------------------------------------------------------------------
/**
 *  みがわり作成
 *  args .. [0]:pos
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_MigawariCreate(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlvPos  vpos = m_mainModule->BtlPosToViewPos( args[0] );

      //BTLV_EFFECT_CreateMigawari( vpos );
      m_viewCore->CMD_AddEffectByPos(vpos , BTLEFF_MIGAWARI_FALL);
      (*seq)++;
    }
    break;
  case 1:
    //if( !BTLV_EFFECT_CheckExecute() )
    if( m_viewCore->CMD_WaitEffectByPos() )
    {
      return true;
    }
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  みがわり削除
 *  args .. [0]:pos
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_MigawariDelete(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlvPos  vpos = m_mainModule->BtlPosToViewPos( args[0] );

      if( IsChapterSkipMode() ){
        //sangoでは未使用
        //BTLV_EFFECT_SetSEMode( BTLV_EFFECT_SE_MODE_MUTE );
      }

      //BTLV_EFFECT_DeleteMigawari( vpos );
      m_viewCore->CMD_AddEffectByPos(vpos , BTLEFF_MIGAWARI_DEAD);
      (*seq)++;
    }
    break;
  case 1:
    //if( !BTLV_EFFECT_CheckExecute() )
    if( m_viewCore->CMD_WaitEffectByPos() )
    {
      if( IsChapterSkipMode() ){
        //sangoでは未使用
        //BTLV_EFFECT_SetSEMode( BTLV_EFFECT_SE_MODE_PLAY );
      }
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * へんしん
 *
 * args ..[0]: へんしんするポケID   [1]: へんしん対象のポケID
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_Hensin(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BTL_POKEPARAM* atkPoke = m_pokeCon->GetPokeParam( args[0] );
      BTL_POKEPARAM* tgtPoke = m_pokeCon->GetPokeParam( args[1] );
      BtlvPos  atkVpos = m_mainModule->PokeIDtoViewPos( m_pokeCon, args[0] );
      BtlvPos  tgtVpos = m_mainModule->PokeIDtoViewPos( m_pokeCon, args[1] );

//      TAYA_Printf("PokeSick=%d, line=%d\n", BPP_GetPokeSick(atkPoke), __LINE__);
      atkPoke->HENSIN_Set( tgtPoke );
//      TAYA_Printf("PokeSick=%d, line=%d\n", BPP_GetPokeSick(atkPoke), __LINE__);


      m_viewCore->CMD_Hensin_Start( atkVpos, tgtVpos );
      (*seq)++;
    }
    break;

  case 1:
    if( m_viewCore->CMD_Hensin_Wait() ){
      return true;
    }
    break;
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * みがわりダメージ
 *
 * args ..[0]: みがわりを出しているポケID   [1]: ダメージ相性  [2]:ワザID
 */
//----------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_MigawariDamage(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( !IsChapterSkipMode() )
    {
      BtlPokePos       tgtPos   = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
      BtlTypeAffAbout  affAbout = calc::TypeAffAbout( (BtlTypeAff)(args[1]) );

      m_viewCore->CMD_ACT_MigawariDamageEffect_Start( (WazaNo)(args[2]), tgtPos, affAbout );
      (*seq)++;
    }
    else{
      return true;
    }
    break;

  case 1:
    if( m_viewCore->CMD_ACT_MigawariDamageEffect_Wait() ){
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  PlayWinBGM
 *  args .. [0]:BGMナンバー
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_PlayWinBGM(  int* , const int* args )
{
  m_mainModule->StartWinBGM();
  return true;
}
//---------------------------------------------------------------------------------------
/**
 *  MsgWindow フェードアウト
 *  args .. [0]:dummy
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_MsgWinHide(  int* seq, const int*  )
{
  switch( *seq ){
  case 0:
    m_viewCore->CMD_MsgWinHide_Start();
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_MsgWinHide_Wait() ){
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------------------
/**
 *  なかよし度発揮アクション実行
 *  args .. [0]:pokeID  [1]: enum FriendshipEffect (@ btl_common.h)
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_FriendshipEffect( int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );

  switch( *seq ){
  case 0:
    {
      if( pos == BTL_POS_NULL ){
        return true;
      }

      const BTL_POKEPARAM* bpp = m_pokeCon->GetFrontPokeDataConst( pos );
      if( !FriendshipEffectProc::S_IsEnable(bpp) ){
        return true;
      }

      BtlvPos vpos = m_mainModule->BtlPosToViewPos( pos );
      if( (args[1] == FREFF_FURIMUKI) || (args[1] == FREFF_BOTH) ){
        m_viewCore->CMD_AddEffectByPos( vpos, BTLEFF_KAWAIGARI_TURN );
      }
      if( (args[1] == FREFF_HEART) || (args[1] == FREFF_BOTH) ){
        m_viewCore->CMD_AddEffectByPos( vpos, BTLEFF_KAWAIGARI_EFFECT );
      }
      (*seq)++;
    }
    break;

  case 1:
    if( m_viewCore->CMD_WaitEffectByPos() ){
      (*seq)++;
    }
    break;

  default:
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  なかよし度発揮アクション実行
 *  args .. [0]:pokeID  [1]: enum FriendshipEffect (@ btl_common.h)
 *          [2]:msgType [3]: strID  [4]:arg1 [5]:arg2
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_FriendshipEffectMsg( int* seq, const int* args )
{
  if( IsChapterSkipMode() ){
    return true;
  }

  static int effSeq = 0;
  static int msgSeq = 0;

  if( *seq == 0 ){
    effSeq = 0;
    msgSeq = 0;
    (*seq) = 1;
  }

  bool bEffectDone = scProc_ACT_FriendshipEffect( &effSeq, args );

  bool bMsgDone = true;
  switch( args[2] ){
    case BTL_STRTYPE_STD:  bMsgDone = scproc_msgStdCore( &msgSeq, args[3], &args[4] ); break;
    case BTL_STRTYPE_SET:  bMsgDone = scproc_msgSetCore( &msgSeq, args[3], &args[4] ); break;
    default:
      break;
  }

  if( bEffectDone && bMsgDone ){
    return true;
  }
  return false;


}

//---------------------------------------------------------------------------------------
/**
 *  ひんしアクションの複数体同時処理（群れバトル時）
 *  args .. [0]:同時にひんしになったポケモン数
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_DeadPlural( int* seq, const int* args )
{
  u8 deadPokeCount = this->GetVariableArgsCount();

  switch( *seq ){
  case 0:
    for(u32 i=0; i<deadPokeCount; ++i)
    {
      u8 pokeID = (u8)(this->GetVariableArgs(i));
      m_deadPokePos[i] = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );
    }
    m_viewCore->CMD_StartDeadActPlural( deadPokeCount, m_deadPokePos );
    ++(*seq);
    break;

  case 1:
    if( m_viewCore->CMD_WaitDeadActPlural() )
    {
      for(u32 i=0; i<deadPokeCount; ++i)
      {
        BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( this->GetVariableArgs(i) );
        if( bpp ){
          bpp->Clear_ForDead();
          updateClientPublicInformation_FormNo( *bpp );  // 死亡によりフォルムが元に戻ることがあるので、他クライアントへの公開情報を更新する
        }
      }
      ++(*seq);
    }
    break;

  case 2:
    m_strArgs[0] = deadPokeCount;
    m_viewCore->CMD_StartMsgStd( BTL_STRID_STD_DeadPlural, m_strArgs );
    ++(*seq);
    break;

  case 3:
    if( m_viewCore->CMD_WaitMsg() ){
      ++(*seq);
    }
    break;

  default:
    return true;

  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  なでる（かわいがり）処理
 *  args .. [0]:対象ポケモンID
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_Naderu(  int* seq, const int* args )
{
  u8 pokeID = args[0];
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
  BtlvPos vpos = m_mainModule->PokeIDtoViewPos( m_pokeCon, pokeID );
  bool fGaugeExist = ((vpos != BTL_VPOS_ERROR) && m_viewCore->CMD_EFFECT_CheckExistGauge(vpos) );

  switch( *seq ){
    case 0:
      if( fGaugeExist
      &&  (m_mainModule->GetPokeFriendship(bpp) >= 150)
      &&  (FriendshipEffectProc::S_IsEnable(bpp))
      ){
        m_viewCore->CMD_Naderu_Start( vpos );
        ++(*seq);
      }
      else
      {
        return true;
      }
      break;

    case 1:
      if( m_viewCore->CMD_Naderu_Wait(vpos) )
      {
        ++(*seq);
      }
      break;

    default:
      return true;
  }
  return false;

}
//---------------------------------------------------------------------------------------
/**
 *  経験値加算処理
 *  args .. [0]:対象ポケモンID  [1]:経験値加算分
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_Exp(  int* seq, const int* args )
{
  enum {
    SEQ_INIT = 0,
    SEQ_ADD_ROOT,
    SEQ_GAUGE_NORMAL_WAIT,
    SEQ_LVUP_ROOT,
    SEQ_LVUP_GAUGE_WAIT,
    SEQ_LVUP_EFFECT_WAIT,
    SEQ_LVUP_INFO_START,
    SEQ_LVUP_INFO_MSG_WAIT,
    SEQ_LVUP_INFO_MSG_WAIT_FADE,
    SEQ_LVUP_INFO_MSG_WAIT2,
    SEQ_LVUP_INFO_PARAM_START,
    SEQ_LVUP_INFO_PARAM_SEQ1,
    SEQ_LVUP_INFO_PARAM_SEQ2,
    SEQ_LVUP_INFO_PARAM_END,
    SEQ_LVUP_WAZAOBOE_START,
    SEQ_LVUP_WAZAOBOE_WAIT,
    SEQ_END_EXP,
    SEQ_END_ALL,
  };

  static BTL_LEVELUP_INFO  lvupInfo;
  static u32  addExp;
  static int  subSeq;

  u8 pokeID = args[0];
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
  BtlvPos vpos = m_mainModule->PokeIDtoViewPos( m_pokeCon, pokeID );
  bool fGaugeExist = ((vpos != BTL_VPOS_ERROR) && m_viewCore->CMD_EFFECT_CheckExistGauge(vpos) );

  switch( *seq ){
  case SEQ_INIT:
    addExp = args[1];
    subSeq = 0;
    (*seq) = SEQ_ADD_ROOT;
    /* fallthru */
  case SEQ_ADD_ROOT:
    if( addExp )
    {
      u32 expRest = addExp;

      if( bpp->AddExp(&expRest, &lvupInfo) )
      {
        (*seq) = SEQ_LVUP_ROOT;
      }
      else
      {
        if( fGaugeExist ){
          m_viewCore->CMD_EFFECT_CalcGaugeEXP( vpos, addExp );
          (*seq) = SEQ_GAUGE_NORMAL_WAIT;
        }
        else {
          (*seq) = SEQ_END_EXP;
        }
        expRest = 0;
      }
      addExp = expRest;
    }
    else
    {
      (*seq) = SEQ_END_EXP;
    }
    break;

  // 通常の（レベルアップしない）ゲージ演出の終了待ち
  case SEQ_GAUGE_NORMAL_WAIT:
    if( !m_viewCore->CMD_EFFECT_CheckExecuteGauge() ){
      (*seq) = SEQ_END_EXP;
    }
    break;

  // レベルアップ処理ルート
  case SEQ_LVUP_ROOT:
    m_viewCore->CMD_Start3DSReverseWatcher( fGaugeExist == false ); //3DSさかさま監視：起動
    bpp->ReflectByPP();
    {
       // 場に出ているならゲージ演出
       if( fGaugeExist )
       {
         m_viewCore->CMD_EFFECT_CalcGaugeEXPLevelUp( vpos, bpp );
         (*seq) = SEQ_LVUP_GAUGE_WAIT;
       }
       // 出ていないなら能力上昇表示までジャンプ
       else{
         (*seq) = SEQ_LVUP_INFO_START;
       }
    }
    break;

  case SEQ_LVUP_GAUGE_WAIT:
    if( (!m_viewCore->CMD_EFFECT_CheckExecuteGauge())
//    &&  (!PMSND_CheckPlaySE())  // これはBTLV_EFFECTで待ち処理を入れるべき
    ){
      m_viewCore->CMD_AddEffectByPos( vpos, BTLEFF_LVUP );
      (*seq) = SEQ_LVUP_EFFECT_WAIT;
    }
    break;
  case SEQ_LVUP_EFFECT_WAIT:
    if( m_viewCore->CMD_WaitEffectByPos() )
    {
      (*seq) = SEQ_LVUP_INFO_START;
    }
    break;

  case SEQ_LVUP_INFO_START:
    BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LevelUp );
    BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
    BTLV_STRPARAM_AddArg( &m_strParam, lvupInfo.level );
    m_viewCore->CMD_StartMsg( &m_strParam );
    (*seq) = SEQ_LVUP_INFO_MSG_WAIT;
    break;

  case SEQ_LVUP_INFO_MSG_WAIT:
    if( m_viewCore->CMD_WaitMsg_WithoutHide() )
    {
      (*seq) = SEQ_LVUP_INFO_MSG_WAIT_FADE;
    }
    break;

  // レスポンスは落ちるが、Fade中にStartMEをリクエストすると失敗するのでこのようにする taya
  case SEQ_LVUP_INFO_MSG_WAIT_FADE:
    if( m_soundSys->CheckFadeOnBGM() == false )
    {
      m_soundSys->StartME( STRM_ME_LVUP );
      (*seq) = SEQ_LVUP_INFO_MSG_WAIT2;
    }
    break;

  case SEQ_LVUP_INFO_MSG_WAIT2:
    {
      bool bDone = true;
      if( !(m_soundSys->WaitMEFinish(STRM_ME_LVUP)) ){ bDone = false; }

      if( bDone )
      {
        m_mainModule->NotifyPokemonLevelup( bpp );
        m_soundSys->RestartBaseBGM();
        subSeq = 0;
        if( fGaugeExist ){
          (*seq) = SEQ_LVUP_INFO_PARAM_START;
        }else{
          BTL_PRINT("[BTLCLIENT] this=%p, viewCore=%p\n", this, m_viewCore );
          (*seq) = SEQ_LVUP_WAZAOBOE_START;
        }
      }
    }
    break;

  case SEQ_LVUP_INFO_PARAM_START:
    ++subSeq;
    if( (ui::CheckNextTrg())
    ||  (subSeq > 80)
    ){
      subSeq = 0;
      m_viewCore->CMD_LvupWin_StartDisp( bpp, &lvupInfo );
      (*seq) = SEQ_LVUP_INFO_PARAM_SEQ1;
    }
    break;

  case SEQ_LVUP_INFO_PARAM_SEQ1:
    if( m_viewCore->CMD_LvupWin_WaitDisp() )
    {
      if( ui::CheckNextTrg() )
      {
        m_viewCore->CMD_LvupWin_StepFwd( );
        (*seq) = SEQ_LVUP_INFO_PARAM_SEQ2;
      }
    }
    break;

  case SEQ_LVUP_INFO_PARAM_SEQ2:
    if( m_viewCore->CMD_LvupWin_WaitFwd() )
    {
      if( ui::CheckNextTrg() )
      {
        m_viewCore->CMD_LvupWin_StartHide( );
        (*seq) = SEQ_LVUP_INFO_PARAM_END;
      }
    }
    break;

  case SEQ_LVUP_INFO_PARAM_END:
    if( m_viewCore->CMD_LvupWin_WaitHide() )
    {
      (*seq) = SEQ_LVUP_WAZAOBOE_START;
    }
    break;

  case SEQ_LVUP_WAZAOBOE_START:
    if(  m_viewCore->CMD_WaitMsg() )
    {
      if( m_viewCore->CMD_Get3DSReverseWatcherResult() )
      { //3DSさかさま監視：検知したので通知しておく
        m_mainModule->NotifyTurnedLevelUpPokePos( pokeID );
      }
      m_viewCore->CMD_Stop3DSReverseWatcher(); //3DSさかさま監視：終了
      subSeq = 0;
      (*seq) = SEQ_LVUP_WAZAOBOE_WAIT;
    }
    break;

  case SEQ_LVUP_WAZAOBOE_WAIT:
    if( wazaOboeSeq(&subSeq, bpp) ){
      (*seq) = SEQ_ADD_ROOT;
    }
    break;

  case SEQ_END_EXP:
    m_mainModule->ReflectPokeWazaOboe( pokeID );
    (*seq) = SEQ_END_ALL;
    break;

  case SEQ_END_ALL:
    return true;

  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  可変個引数をワークメンバに保存
 *  args .. [0]:引数の個数  [1〜] 引数群
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ExArg(  int* seq, const int* args )
{
  m_stdVariableArgs.Setup( args );
  return true;
}


/**
 *  １、２のポカンSE管理コールバック
 */
bool BTL_CLIENT::PrintCallback( PrintCallbackArg arg )
{
  /*
  enum {
//    SE_LABEL = SEQ_SE_BT_KOUKA_H,
    SE_LABEL = SEQ_SE_BT_WAZAWASURE,
  };

  switch( arg ){
  case PRINTCB_JUST_DONE:
    m_soundSys->PlaySE( SE_LABEL );
    break;

  case PRINTCB_AFTER_DONE:
    if( m_soundSys->IsSEFinished(SE_LABEL) ){
      return true;
    }
    break;
  }
  return false;
  */

  //Btlv側でタグのコールバックを使って実装。
  //一応動作はします。 Ariizumi121028
  return true;
}

/**
 * ワザ覚え処理
 *  （経験値加算処理 scProc_ACT_Exp のサブシーケンス）
 */
bool BTL_CLIENT::wazaOboeSeq( int* seq, BTL_POKEPARAM* bpp )
{
  enum {
    SEQ_INIT = 0,
    SEQ_CHECK_ROOT,
    SEQ_OBOETA_MSG_START,
    SEQ_OBOETA_MSG_WAIT,
    SEQ_OBOETA_ME_WAIT,

    SEQ_WASURE_ROOT,
    SEQ_WASURE_WAIT_MSG_1ST,
    SEQ_WASURE_YESNO_WAIT,
    SEQ_WASURE_CANCEL_MSG_START,
    SEQ_WASURE_CANCEL_MSG_WAIT,

    SEQ_WASURE_SELECT,
    SEQ_WASURE_DECIDE,
  };

  static u32 wazaoboe_index;
  static WazaNo wazaoboe_no;

  u8 pokeID = bpp->GetID();

  switch( *seq ){
  case SEQ_INIT:
    wazaoboe_index = 0;
    wazaoboe_no = WAZANO_NULL;
    (*seq) = SEQ_CHECK_ROOT;
    /* fallthru */
  case SEQ_CHECK_ROOT:
    //技覚えチェック
    {
      pml::pokepara::PokemonParam* pp = const_cast<pml::pokepara::PokemonParam*>(bpp->GetSrcData());
      pml::pokepara::WazaLearningResult result = pp->LearnNewWazaOnCurrentLevel( &wazaoboe_index, &wazaoboe_no );
      if( result == pml::pokepara::WAZA_LEARNING_FAILED_NOT_EXIST )
      {
        m_viewCore->CMD_HideMsg();
        return true;
      }
      else if( result == pml::pokepara::WAZA_LEARNING_FAILED_SAME )
      {
        break;
      }
      else if( result == pml::pokepara::WAZA_LEARNING_FAILED_FULL )
      {
        (*seq) = SEQ_WASURE_ROOT;
      }
      else
      {
        (*seq) = SEQ_OBOETA_MSG_START;
      }
    }
    break;

  case SEQ_OBOETA_MSG_START:
    {
      bpp->ReflectByPP();
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WazaOboe_03 ); //「○○は　あたらしく　××を　おぼえた！」
      BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
      BTLV_STRPARAM_AddArg( &m_strParam, wazaoboe_no );
      m_viewCore->CMD_StartMsg( &m_strParam );
      (*seq) = SEQ_OBOETA_MSG_WAIT;
    }
    /* fallthru */
  case SEQ_OBOETA_MSG_WAIT:
    //技覚え処理
    if( m_viewCore->CMD_WaitMsg_WithoutHide() ){
      m_soundSys->StartME( STRM_ME_LVUP );
      (*seq) = SEQ_OBOETA_ME_WAIT;
    }
    break;
  case SEQ_OBOETA_ME_WAIT:
    if( m_soundSys->WaitMEFinish( STRM_ME_LVUP ) )
    {
      BTL_PRINT("技覚えmeの終了確認しました\n");
      m_soundSys->RestartBaseBGM();
      (*seq) = SEQ_CHECK_ROOT;
    }
    break;

  case SEQ_WASURE_ROOT:
    BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WazaOboe_01 ); //「○○覚えるために他のワザを忘れさせますか？」
    BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
    BTLV_STRPARAM_AddArg( &m_strParam, wazaoboe_no );
    BTLV_STRPARAM_SetWait( &m_strParam, BTLV_MSGWAIT_NONE );
    m_viewCore->CMD_StartMsg( &m_strParam );
    (*seq) = SEQ_WASURE_WAIT_MSG_1ST;
    /* fallthru */
  case SEQ_WASURE_WAIT_MSG_1ST:
    if( m_viewCore->CMD_WaitMsg_WithoutHide() )
    {
      m_viewCore->CMD_YESNO_Start( true, YESNO_MODE_FORGET );
      (*seq) = SEQ_WASURE_YESNO_WAIT;
    }
    break;

  case SEQ_WASURE_YESNO_WAIT:
    {
      BtlYesNo result;

      if( m_viewCore->CMD_YESNO_Wait( &result ) )
      {
        if( result == BTL_YESNO_YES )
        {
          const BTL_PARTY* party = m_pokeCon->GetPartyDataConst( m_myID );
          int index = party->FindMemberByPokeID( pokeID );
          m_viewCore->CMD_WAZAWASURE_Start( index, wazaoboe_no );
          (*seq) = SEQ_WASURE_SELECT;
        }
        else
        {
          *seq = SEQ_WASURE_CANCEL_MSG_START;
        }
      }
    }
    break;

  case SEQ_WASURE_CANCEL_MSG_START:
    BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WazaOboe_04 ); //「○○は　××を　おぼえずに　おわった！」
    BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
    BTLV_STRPARAM_AddArg( &m_strParam, wazaoboe_no );
    m_viewCore->CMD_StartMsg( &m_strParam );
    *seq = SEQ_WASURE_CANCEL_MSG_WAIT;
    break;

  case SEQ_WASURE_CANCEL_MSG_WAIT:
    if( m_viewCore->CMD_WaitMsg() ) {
      (*seq) = SEQ_CHECK_ROOT;
    }
    break;

  case SEQ_WASURE_SELECT:  //技忘れ選択処理
    {
      u8 selectedWazaIndex;
      BattleViewSystem::WazaWasureResult result = m_viewCore->CMD_WAZAWASURE_Wait( &selectedWazaIndex );

      if( result == BattleViewSystem::WAZAWASURE_RESULT_CANCEL )
      {
        (*seq) = SEQ_WASURE_ROOT;
      }
      else if( result == BattleViewSystem::WAZAWASURE_RESULT_SELECT_NEW )
      {
        (*seq) = SEQ_WASURE_CANCEL_MSG_START;
      }
      else if( result == BattleViewSystem::WAZAWASURE_RESULT_SELECT_OLD )
      {
        pml::pokepara::PokemonParam* pp = const_cast<pml::pokepara::PokemonParam*>(bpp->GetSrcData());
        WazaID forget_wazano = pp->GetWazaNo( selectedWazaIndex );

        pp->SetWaza( selectedWazaIndex, wazaoboe_no );
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WazaOboe_02 ); //「１　２の　……　ポカン！」
        BTLV_STRPARAM_AddArg( &m_strParam, pokeID );
        BTLV_STRPARAM_AddArg( &m_strParam, forget_wazano );
        m_viewCore->CMD_StartMsgCallback( &m_strParam, this );
        (*seq) = SEQ_WASURE_DECIDE;
      }
    }
    break;

  case SEQ_WASURE_DECIDE:
    //技忘れ処理
    if( m_viewCore->CMD_WaitMsg_WithoutHide() ){
      (*seq) = SEQ_OBOETA_MSG_START;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  モンスターボール投げつけ（野生）
 *  args .. [0]:対象ポケ位置  [1]:ゆれ回数  [2]:捕獲成功フラグ [3]: 図鑑登録フラグ [4]:クリティカルフラグ
 *           [5]:ボールのアイテムナンバー
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_BallThrow( int* seq, const int* args )
{
  enum {
    SEQ_START_BALL_THROW = 0,
    SEQ_WAIT_BALL_THROW,
    SEQ_WAIT_SUCCESS_ME_PLAYING,
    SEQ_WAIT_SUCCESS_MESSAGE,
    SEQ_WAIT_FAIL_MESSAGE,
    SEQ_WAIT_JINGLE,
    SEQ_START_RECOVER_CAMERA,
    SEQ_WAIT_RECOVER_CAMERA,
    SEQ_DONE,
  };

  BtlPokePos  targetPos         = args[0];
  int         yureCount         = args[1];
  bool        bCaptureSuccess   = args[2];
  //bool        bRegisterZukan    = args[3];
  bool        bCritical         = args[4];
  u32         ballItemID        = args[5];

  switch( *seq ){
  case SEQ_START_BALL_THROW:
    {
      u8 vpos = m_mainModule->BtlPosToViewPos( targetPos );
      m_viewCore->CMD_EFFECT_BallThrow( vpos, ballItemID, yureCount, bCaptureSuccess, bCritical );
      (*seq) = SEQ_WAIT_BALL_THROW;
    }
    break;
  case SEQ_WAIT_BALL_THROW:
    if( m_viewCore->CMD_EFFECT_WaitBallThrow() )
    {
      // 捕獲成功時はまずMEの再生を開始
      if( bCaptureSuccess )
      {
        m_soundSys->ForceFlatBgmLayer();
        m_soundSys->StartME( STRM_ME_POKEGET );
        (*seq) = SEQ_WAIT_SUCCESS_ME_PLAYING;
      }
      // 捕獲失敗の時はメッセージ開始
      else
      {
        static const u16 strTbl[] = {
          BTL_STRID_STD_BallThrow0, BTL_STRID_STD_BallThrow1, BTL_STRID_STD_BallThrow2, BTL_STRID_STD_BallThrow3,
        };
        u16 strID = ( targetPos < GFL_NELEMS(strTbl) )?  strID = strTbl[yureCount] : strTbl[0];
        BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, strID );
        m_viewCore->CMD_StartMsg( &m_strParam );
        (*seq) = SEQ_WAIT_FAIL_MESSAGE;
      }
    }
    break;

  case SEQ_WAIT_FAIL_MESSAGE:
    if( m_viewCore->CMD_WaitMsg() ){
      (*seq) = SEQ_START_RECOVER_CAMERA;
    }
    break;

  case  SEQ_WAIT_SUCCESS_ME_PLAYING:
    if( m_soundSys->IsMEPlaying(STRM_ME_POKEGET) )
    {
      const BTL_POKEPARAM* bpp = m_pokeCon->GetFrontPokeDataConst( targetPos );
      GFL_ASSERT_STOP(bpp);
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_BallThrowS );
      BTLV_STRPARAM_AddArg( &m_strParam, bpp->GetID() );
      m_viewCore->CMD_StartMsg( &m_strParam );
      (*seq) = SEQ_WAIT_SUCCESS_MESSAGE;
    }
    break;

  case SEQ_WAIT_SUCCESS_MESSAGE:
    if( m_viewCore->CMD_WaitMsg_WithoutHide() )
    {
      const BTL_POKEPARAM* bpp = m_pokeCon->GetFrontPokeDataConst( targetPos );
      m_mainModule->NotifyPokemonGetToGameSystem( bpp );
      (*seq) = SEQ_WAIT_JINGLE;
    }
    break;


  case SEQ_WAIT_JINGLE:
    if( m_soundSys->WaitMEFinish(STRM_ME_POKEGET) )
    {
      if( m_viewCore->CMD_WaitMsg() ){
        //戦闘エフェクト一時停止を解除
        //sango未使用
        //BTLV_EFFECT_Restart();
        m_soundSys->StartNewBGMFromME( STRM_BGM_WIN1 );
        m_mainModule->BGMFadeOutDisable();
        (*seq) = SEQ_START_RECOVER_CAMERA;
      }
    }
    break;

  case SEQ_START_RECOVER_CAMERA:
    m_viewCore->CMD_ResetCamera();
    (*seq) = SEQ_WAIT_RECOVER_CAMERA;
    break;
  case SEQ_WAIT_RECOVER_CAMERA:
    if( m_viewCore->CMD_ResetCamera_Wait() ){
      (*seq) = SEQ_DONE;
    }
    break;
  case SEQ_DONE:
  default:
      return true;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  ポケモン捕獲後の追加コマンド
 *  args .. [0]:対象ポケ位置  [1]:図鑑登録フラグ
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_BallThrowCaptured(  int* seq, const int* args )
{
  //BtlPokePos  targetPos         = args[0];
  //bool        bRegisterZukan    = args[1];

  switch( *seq ){
  case 0:
#if 0
    // 図鑑登録メッセージ
    if( bRegisterZukan )
    {
      const BTL_POKEPARAM* bpp = m_pokeCon->GetFrontPokeDataConst( targetPos );
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_ZukanRegister );
      BTLV_STRPARAM_AddArg( &m_strParam, bpp->GetID() );
      m_viewCore->CMD_StartMsg( &m_strParam );
      (*seq) = 1;
    }
    else{
      (*seq) = 2;
    }
#endif
    (*seq) = 2;
    break;

  case 1:
    if( m_viewCore->CMD_WaitMsg() ){
      (*seq) = 2;
    }
    break;

  case 2:
  default:
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------------
/**
 *  モンスターボール投げつけ（トレーナー戦なので怒られる）
 *  args .. [0]:対象ポケ位置  [1]:ボールのアイテムナンバー
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_BallThrowForbidden(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    // スカイバトルで無ければボール投げ->失敗演出
    if( !m_mainModule->IsSkyBattle() )
    {
      u8 vpos = m_mainModule->BtlPosToViewPos( args[0] );
      m_viewCore->CMD_EFFECT_BallThrowTrainer( vpos, args[1] );
      ++(*seq);
    }
    else{
      (*seq) += 2;
    }
    break;
  case 1:
    if( m_viewCore->CMD_EFFECT_WaitBallThrowTrainer() )
    {
      ++(*seq);
    }
    break;
    // 人のものをとったらどろぼう
  case 2:
    BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_BallThrowTrainer );
    m_viewCore->CMD_StartMsg( &m_strParam );
    ++(*seq);
    break;
  case 3:
    if( m_viewCore->CMD_WaitMsg() ){
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  とくせい書き換えアクション
 *  args .. [0]:対象のポケモンID [1]:書き換え後のとくせい
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACTOP_ChangeTokusei(  int* seq, const int* args )
{
  u8 pokeID = args[0];
  BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );

  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
      bpp->ChangeTokusei( (TokuseiNo)(args[1]) );
      return true;
    }
    else
    {
      m_viewCore->CMD_TokWin_DispStart( pos, false );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_TokWin_DispWait( pos ) )
    {
      BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
      bpp->ChangeTokusei( (TokuseiNo)(args[1]) );
      m_viewCore->CMD_TokWin_Renew_Start( pos );
      (*seq)++;
    }
    break;
  case 2:
    if( m_viewCore->CMD_TokWin_Renew_Wait( pos ) )
    {
      notifyTokuseiToAI( pokeID );
      (*seq)++;
    }
    break;
  default:
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  とくせい入れ替え演出  args [0]:pokeID_1, [1]:pokeID_2 [2]:tokID_1, [3]:tokID_2
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACTOP_SwapTokusei(  int* seq, const int* args )
{
  static u32 timer = 0;

  u8 poke1_ID = args[0];
  u8 poke2_ID = args[1];
  TokuseiNo poke1_nextTokID = (TokuseiNo)args[2];
  TokuseiNo poke2_nextTokID = (TokuseiNo)args[3];

  BtlPokePos pos1 = m_mainModule->PokeIDtoPokePos( m_pokeCon, poke1_ID );
  BtlPokePos pos2 = m_mainModule->PokeIDtoPokePos( m_pokeCon, poke2_ID );

  BTL_POKEPARAM* bpp1 = m_pokeCon->GetPokeParam( poke1_ID );
  BTL_POKEPARAM* bpp2 = m_pokeCon->GetPokeParam( poke2_ID );


  switch( (*seq) ){
  case 0:
    // チャプタスキップ時はデータ書き換えのみ
    if( IsChapterSkipMode() )
    {
      bpp1->ChangeTokusei( poke1_nextTokID );
      bpp2->ChangeTokusei( poke2_nextTokID );
      return true;
    }

    // 味方同士なら演出なし
    if( m_mainModule->IsFriendPokeID(poke1_ID, poke2_ID) )
    {
      bpp1->ChangeTokusei( poke1_nextTokID );
      bpp2->ChangeTokusei( poke2_nextTokID );
      return true;
    }
    else
    {
      m_viewCore->CMD_TokWin_DispStart( pos1, false );
      timer = 8;
      (*seq)++;
    }
    break;

  case 1:
    m_viewCore->CMD_TokWin_DispWait( pos1 );
    if( timer ){
      --timer;
    }else{
      m_viewCore->CMD_TokWin_DispStart( pos2, false );
      (*seq)++;
    }
    break;

  case 2:
    {
      u8 wait1 = m_viewCore->CMD_TokWin_DispWait( pos1 ) ;
      u8 wait2 = m_viewCore->CMD_TokWin_DispWait( pos2 ) ;
      if( wait1 && wait2 ){
        (*seq)++;
      }
    }
    break;

  case 3:
    {
      bpp1->ChangeTokusei( poke1_nextTokID );
      bpp2->ChangeTokusei( poke2_nextTokID );
      timer = 0;
      (*seq)++;
    }
    break;

  case 4:
    if( ++timer > 8 ){
      m_viewCore->CMD_TokWin_Renew_Start( pos1 );
      (*seq)++;
    }
    break;

  case 5:
    if( m_viewCore->CMD_TokWin_Renew_Wait( pos1 ) ){
      m_viewCore->CMD_TokWin_Renew_Start( pos2 );
      (*seq)++;
    }
    break;

  case 6:
    if( m_viewCore->CMD_TokWin_Renew_Wait( pos2 ) )
    {
      notifyTokuseiToAI( poke1_ID );
      notifyTokuseiToAI( poke2_ID );

      timer = 0;
      (*seq)++;
    }
    break;

  case 7:
    if( ++timer > 8 ){
      m_viewCore->CMD_QuitTokWin( pos1 );
      m_viewCore->CMD_QuitTokWin( pos2 );
      (*seq)++;
    }
    break;

  case 8:
    {
      bool bDone1 = m_viewCore->CMD_QuitTokWinWait( pos1 );
      bool bDone2 = m_viewCore->CMD_QuitTokWinWait( pos2 );
      if( bDone1 && bDone2 ){
        ++(*seq);
      }
    }
    break;

  default:
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  イリュージョン状態を解除
 *  args .. [0]:対象のポケモンID
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_FakeDisable(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      u8 pokeID = args[0];
      BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );
      BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
      bool fSkipMode = IsChapterSkipMode();

      bpp->FakeDisable();
      m_viewCore->CMD_FakeDisable_Start( pos, fSkipMode );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_FakeDisable_Wait() )
    {
      (*seq)++;
    }
    break;
  default:
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  位置指定なしのエフェクト発動
 *  args .. [0]:位置指定  [1]:エフェクトナンバー指定
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_EffectSimple(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      return true;
    }
    m_viewCore->CMD_AddEffect( args[0] );
    (*seq)++;
    break;

  default:
    if( m_viewCore->CMD_WaitEffect()  ){
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  指定位置にエフェクト発動
 *  args .. [0]:位置指定  [1]:エフェクト指定
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_EffectByPos(  int* seq, const int* args )
{
  BtlvPos vpos = m_mainModule->BtlPosToViewPos( args[0] );

  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      return true;
    }
    if( !m_mainModule->IsWazaEffectEnable() )
    {
      if( tables::IsEffectDisableType(args[1]) ){
        return true;
      }
    }

    m_viewCore->CMD_AddEffectByPos( vpos, args[1] );
    (*seq)++;
    break;

  default:
    if( m_viewCore->CMD_WaitEffectByPos( ) ){
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  特殊条件による２回攻撃の２回目ワザエフェクトを発動
 *  args .. [0]:位置指定  [1]:エフェクト指定
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_PluralEx2ndHit(  int* seq, const int* args )
{
  // 処理内容は EffectByPos と全く同じだが
  // エフェクト＆ダメージを同時に処理する仕様の都合上、
  // 同時処理の対象となる特殊なコマンドとそうでないコマンドを区別する必要があるため
  // こうしています（コマンドナンバーで区別している）  taya
  //  BTL_PRINT("エフェクト=%d, 場所=%d, おやこあい? ->%d\n", args[1],args[0],(args[1]==BTLEFF_SIMPLE_HIT));
  
  
  // エフェクトみない設定なら何もしない
  #if 0
  if( !m_mainModule->IsWazaEffectEnable() ){
    return true;
  }
  #endif

  return scProc_ACT_EffectByPos( seq, args );
}


//---------------------------------------------------------------------------------------
/**
 *  指定位置 -> 指定位置 にエフェクト発動
 *  args .. [0]:開始位置指定  [1]:終端位置指定  [2]:エフェクト指定
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_EffectByVector(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      return true;
    }
    if( !m_mainModule->IsWazaEffectEnable() )
    {
      if( tables::IsEffectDisableType(args[2]) ){
        return true;
      }
    }

    {
      BtlvPos vpos_from = m_mainModule->BtlPosToViewPos( args[0] );
      BtlvPos vpos_to   = m_mainModule->BtlPosToViewPos( args[1] );

      m_viewCore->CMD_AddEffectByDir( vpos_from, vpos_to, args[2] );
      (*seq)++;
    }
    break;

  default:
    if( m_viewCore->CMD_WaitEffectByDir() ){
      return true;
    }
    break;
  }
  return false;
}
//---------------------------------------------------------------------------------------
/**
 *  フィールド状態書き換え用のエフェクト発動
 *  args .. [0]:エフェクト指定
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_EffectField(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    m_viewCore->CMD_ChangeGround( (BtlGround)(args[0]) );
    (*seq)++;
    break;

  default:
    if( m_viewCore->CMD_ChangeGround_Wait()  ){
      return true;
    }
    break;
  }
  return false;

}

//---------------------------------------------------------------------------------------
/**
 *  指定ポケモンのフォルムナンバーチェンジ
 *  args .. [0]:pokeID  [1]:formNo  [2]:dontResetFormByOut
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_ACT_ChangeForm(  int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      const u8 pokeID = args[0];
      const FormNo formno = args[1];
      BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );
      BtlvPos vpos   = m_mainModule->BtlPosToViewPos( pos );
      BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
      bool dontResetFormByOut = static_cast<bool>( args[2] );

      //@バトル変更箇所
      //ゲンシカイキはメガシンカに近いので、処理を完全に分ける！
      //scproc_MegaEvoのコピペなので注意
      if( bpp->GetMonsNo() == MONSNO_KAIOOGA ||
          bpp->GetMonsNo() == MONSNO_GURAADON )
      {
        bpp->GensiKaiki( formno );
      }
      else
      {
        bpp->ChangeForm( formno, dontResetFormByOut );
      }

      // 他クライアントへの公開情報を更新する
      updateClientPublicInformation_FormNo( *bpp );

      m_viewCore->CMD_ChangeForm_Start( vpos );
      (*seq)++;
    }
    break;
  case 1:
    if( m_viewCore->CMD_ChangeForm_Wait() )
    {
      (*seq)++;
    }
    break;
  default:
    return true;

  }
  return false;
}

/**
 * @brief 他クライアントへの公開情報を更新する
 * @param poke  更新対象のポケモン
 */
void BTL_CLIENT::updateClientPublicInformation_FormNo( const BTL_POKEPARAM& poke )
{
  const u8     pokeID   = poke.GetID();
  const FormNo formno   = poke.GetFormNo();
  const u8     clientID = m_mainModule->PokeIDtoClientID( pokeID );
  ClientPublicInformation* publicInfo = m_mainModule->GetClientPublicInformation( clientID );
  publicInfo->SetPokemonFormNo( pokeID, formno );
}

//---------------------------------------------------------------------------------------
/**
 *  とくせいウィンドウ表示オン
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_TOKWIN_In(  int* seq, const int* args )
{
  BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
  switch( *seq ){
  case 0:
    if( IsChapterSkipMode() ){
      return true;
    }

    m_viewCore->CMD_TokWin_DispStart( pos, true );
    (*seq)++;
    break;

  case 1:
    if( m_viewCore->CMD_TokWin_DispWait(pos) ){
      notifyTokuseiToAI( args[0] );
      (*seq)++;
    }
    break;

  default:
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------------
/**
 *  とくせいウィンドウ表示オフ
 */
//---------------------------------------------------------------------------------------
bool BTL_CLIENT::scProc_TOKWIN_Out(  int* seq, const int* args )
{
  BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, args[0] );
  switch( *seq ){
  case 0:
    m_viewCore->CMD_QuitTokWin( pos );
    (*seq)++;
    break;
  case 1:
    if( m_viewCore->CMD_QuitTokWinWait(pos) ){
      return true;
    }
    break;
  }
  return false;
}
//------------------------------------------------------------
/**
 *  AI 側に、とくせいが明かになったポケモンを通知する
 */
//------------------------------------------------------------
void BTL_CLIENT::notifyTokuseiToAI( u8 pokeID )
{
  BattleAiSystem::NotifyTokuseiOpen( pokeID );
}


bool BTL_CLIENT::scProc_OP_HpMinus( int*  , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->HpMinus( args[1] );
  return true;
}


bool BTL_CLIENT::scProc_OP_HpPlus( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->HpPlus( args[1] );
  BTL_N_Printf( DBGSTR_CLIENT_OP_HPPlus, bpp, args[0], args[1], bpp->GetValue(BTL_POKEPARAM::BPP_HP) );
  return true;
}
bool BTL_CLIENT::scProc_OP_PPMinus( int* , const int* args )
{
  u8 pokeID = args[0];
  u8 wazaIdx = args[1];
  u8 value = args[2];

  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( pokeID );
  pp->WAZA_DecrementPP( wazaIdx, value );

  return true;
}

bool BTL_CLIENT::scProc_OP_PPMinus_Org( int* , const int* args )
{
  u8 pokeID = args[0];
  u8 wazaIdx = args[1];
  u8 value = args[2];

  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( pokeID );
  pp->WAZA_DecrementPP_Org( wazaIdx, value );

  return true;
}

bool BTL_CLIENT::scProc_OP_WazaUsed( int* , const int* args )
{
  u8 pokeID = args[0];
  u8 wazaIdx = args[1];

  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
  bpp->WAZA_SetUsedFlag( wazaIdx );

  return true;
}


bool BTL_CLIENT::scProc_OP_HpZero( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->HpZero();
  return true;
}

/**
 *  PP回復  args[0]:pokeID  args[1]:WazaIdx  args[2]:回復PP値
 */
bool BTL_CLIENT::scProc_OP_PPPlus( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->WAZA_IncrementPP( args[1], args[2] );
  return true;
}
bool BTL_CLIENT::scProc_OP_PPPlus_Org( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->WAZA_IncrementPP_Org( args[1], args[2] );
  return true;
}


bool BTL_CLIENT::scProc_OP_RankUp( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->RankUp( (BTL_POKEPARAM::ValueID)(args[1]), args[2] );
  return true;
}
bool BTL_CLIENT::scProc_OP_RankDown( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->RankDown( (BTL_POKEPARAM::ValueID)(args[1]), args[2] );
  return true;
}
bool BTL_CLIENT::scProc_OP_RankSet8( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->RankSet( BTL_POKEPARAM::BPP_ATTACK_RANK,      args[1] );
  bpp->RankSet( BTL_POKEPARAM::BPP_DEFENCE_RANK,     args[2] );
  bpp->RankSet( BTL_POKEPARAM::BPP_SP_ATTACK_RANK,   args[3] );
  bpp->RankSet( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK,  args[4] );
  bpp->RankSet( BTL_POKEPARAM::BPP_AGILITY_RANK,     args[5] );
  bpp->RankSet( BTL_POKEPARAM::BPP_HIT_RATIO,        args[6] );
  bpp->RankSet( BTL_POKEPARAM::BPP_AVOID_RATIO,      args[7] );
  bpp->SetCriticalRank( args[8] );
  return true;
}
bool BTL_CLIENT::scProc_OP_RankRecover( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->RankRecover();
  return true;
}
bool BTL_CLIENT::scProc_OP_RankReset( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->RankReset( );
  return true;
}
bool BTL_CLIENT::scProc_OP_RankUpReset( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->RankUpReset( );
  return true;
}
bool BTL_CLIENT::scProc_OP_AddCritical( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->AddCriticalRank( args[1] );
  return true;
}
bool BTL_CLIENT::scProc_OP_SickSet( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );

  BTL_SICKCONT cont;
  u32 cont_high = static_cast<u32>( args[2] );
  u32 cont_low  = static_cast<u32>( args[3] );
  SICKCONT_Unite32bit( &cont, cont_high, cont_low );

  pp->SetWazaSick( (WazaSick)(args[1]), cont );
  return true;
}

bool BTL_CLIENT::scProc_OP_CurePokeSick( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->CurePokeSick();
  return true;
}
bool BTL_CLIENT::scProc_OP_CureWazaSick( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->CureWazaSick( (WazaSick)(args[1]) );
  return true;
}
bool BTL_CLIENT::scProc_OP_MemberIn( int* , const int* args )
{
  u8 clientID  = m_cmdArgs[0];
  u8 posIdx    = m_cmdArgs[1];
  u8 memberIdx = m_cmdArgs[2];

  {
    BTL_PARTY* party = m_pokeCon->GetPartyData( clientID );

    BTL_POKEPARAM* bpp;
    if( posIdx != memberIdx ){
      party->SwapMembers( posIdx, memberIdx );
    }
    bpp = party->GetMemberData( posIdx );
    bpp->SetAppearTurn( args[3] );
    bpp->Clear_ForIn();

    // 他クライアントへの公開情報を更新する
      updateClientPublicInformation_FormNo( *bpp );
  }
  return true;
}

/**
 * @brief ポケモンのタイプを変更する
 * @param args[0] 対象ポケモンのID
 * @param args[1] 新しいタイプが拡張タイプを含むなら、タイプ拡張の原因( BTL_POKEPARAM::ExTypeCause )
 * @param args[2] 新しいタイプ( PokeTypePair )
 */
bool BTL_CLIENT::scProc_OP_ChangePokeType( int* , const int* args )
{
  BTL_POKEPARAM*             pp          = m_pokeCon->GetPokeParam( args[0] );
  BTL_POKEPARAM::ExTypeCause exTypeCause = static_cast<BTL_POKEPARAM::ExTypeCause>( args[1] );
  PokeTypePair               newType     = static_cast<PokeTypePair>( args[2] );
  pp->ChangePokeType( newType, exTypeCause );
  return true;
}

/**
 * @brief ポケモンのタイプを拡張する
 * @param args[0] 対象ポケモンのID
 * @param args[1] 新しいタイプ( pml::PokeType )
 * @param args[2] タイプ拡張の原因( BTL_POKEPARAM::ExTypeCause )
 */
bool BTL_CLIENT::scProc_OP_ExPokeType( int* , const int* args )
{
  BTL_POKEPARAM*             bpp         = m_pokeCon->GetPokeParam( args[0] );
  pml::PokeType              exType      = static_cast<pml::PokeType>( args[1] );
  BTL_POKEPARAM::ExTypeCause exTypeCause = static_cast<BTL_POKEPARAM::ExTypeCause>( args[2] );
  bpp->ExPokeType( exType, exTypeCause );
  return true;
}


/**
 *  状態異常ターンチェック処理
 *  args [0]:numPokemons, [1]:maxSickIndex, [2]:pokeIDPack;
 */
bool BTL_CLIENT::scProc_OP_WSTurnCheck( int* , const int* args )
{
  u8 pokeIDList[ BTL_EXIST_POS_MAX ];
  u32 pokeCnt = args[0];
  WazaSick sick;
  u32 i, maxSickIdx;
  BTL_POKEPARAM* bpp;

  maxSickIdx = args[1];
  calc::PokeIDx6_Unpack32bit( (u32)(args[2]), pokeIDList );

  for(i=0; i<maxSickIdx; ++i)
  {
    sick = tables::GetTurnCheckWazaSickByOrder( i );
    if( sick != pml::wazadata::WAZASICK_NULL )
    {
      for(u32 p=0; p<pokeCnt; ++p)
      {
        bpp = m_pokeCon->GetPokeParam( pokeIDList[ p ] );
        if( !bpp->IsDead() )
        {
          bpp->WazaSick_TurnCheck( sick, NULL, NULL );
        }
      }
    }
    else
    {
      break;
    }
  }

  return true;
}
bool BTL_CLIENT::scProc_OP_ConsumeItem( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->ConsumeItem( args[1] );
  return true;
}
bool BTL_CLIENT::scProc_OP_UpdateUseWaza( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );

  {
    BTL_N_Printf( DBGSTR_CLIENT_UpdateWazaProcResult, args[0], args[1], args[2], args[3], args[4], args[5], args[6] );
  }

  bpp->UpdateWazaProcResult( args[1], args[2], args[3], args[4], (WazaNo)(args[5]), (WazaNo)(args[6]) );
  return true;
}
/**
 *  継続フラグセット  args[0]=pokeID, args[1]=flagID
 */
bool BTL_CLIENT::scProc_OP_SetContFlag(  int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->CONTFLAG_Set( (BTL_POKEPARAM::ContFlag)(args[1]) );
  return true;
}
bool BTL_CLIENT::scProc_OP_ResetContFlag(  int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->CONTFLAG_Clear( (BTL_POKEPARAM::ContFlag)(args[1]) );
  return true;
}
bool BTL_CLIENT::scProc_OP_SetTurnFlag(  int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->TURNFLAG_Set( (BTL_POKEPARAM::TurnFlag)(args[1]) );
  return true;
}
bool BTL_CLIENT::scProc_OP_ResetTurnFlag(  int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->TURNFLAG_ForceOff( (BTL_POKEPARAM::TurnFlag)(args[1]) );
  return true;
}
bool BTL_CLIENT::scProc_OP_SetPermFlag( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->PERMFLAG_Set( (BTL_POKEPARAM::PermFlag)(args[1]) );
  return true;
}

/**
 * @brief サーバーコマンド SC_OP_INC_BATTLE_TURN_COUNT
 *        経過ターンの総数をインクリメントする
 */
bool BTL_CLIENT::scProc_OP_IncBattleTurnCount( int*, const int* args )
{
  m_turnCount++;
  return true;
}

/**
 * @brief サーバーコマンド SC_OP_INC_POKE_TURN_COUNT
 *        場に出たターンの総数をインクリメントする
 *
 * @param args[0] 操作対象ポケモンのID
 */
bool BTL_CLIENT::scProc_OP_IncPokeTurnCount( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->IncTotalTurnCount();
  return true;
}

bool BTL_CLIENT::scProc_OP_ChangeTokusei(  int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->ChangeTokusei( (TokuseiNo)(args[1]) );
  return true;
}
bool BTL_CLIENT::scProc_OP_SetItem(  int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->SetItem( args[1] );
  return true;
}
bool BTL_CLIENT::scProc_OP_UpdateWazaNumber( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  // サーバコマンド送信時の都合で引数がヘンな並びになってる
  pp->WAZA_UpdateID( args[1], (WazaNo)(args[4]), args[2], args[3] );
  return true;
}
bool BTL_CLIENT::scProc_OP_OutClear( int* , const int* args )
{
  BTL_POKEPARAM* pp = m_pokeCon->GetPokeParam( args[0] );
  pp->Clear_ForOut( );
  updateClientPublicInformation_FormNo( *pp );  // 退場によりフォルムが元に戻ることがあるので、他クライアントへの公開情報を更新する
  return true;
}
/**
 *  フィールドエフェクト追加   args[0]:FieldEffect, args[1]:BTL_SICKCONT上位32bit, args[2]:BTL_SICKCONT下位32bit
 */
bool BTL_CLIENT::scProc_OP_AddFldEff( int* , const int* args )
{
  u32 cont_high = static_cast<u32>( args[1] );
  u32 cont_low  = static_cast<u32>( args[2] );

  BTL_SICKCONT cont;
  SICKCONT_Unite32bit( &cont, cont_high, cont_low );

  m_fldSim->AddEffect( (FieldStatus::EffectType)(args[0]), cont );
  return true;
}
/**
 *  フィールドエフェクト追加   args[0]:FieldEffect,  args[1]:依存ポケID
 */
bool BTL_CLIENT::scProc_OP_AddFldEffDepend( int* , const int* args )
{
  m_fldSim->AddDependPoke( (FieldStatus::EffectType)(args[0]), args[1] );
  return true;
}
/**
 *  フィールドグラウンド効果変更   args[0]:FieldEffect,  args[1]:BTL_SICKCONT上位32bit, args[2]:BTL_SICKCONT下位32bit
 */
bool BTL_CLIENT::scProc_OP_ChangeGround( int* seq, const int* args )
{
  u32 cont_high = static_cast<u32>( args[1] );
  u32 cont_low  = static_cast<u32>( args[2] );

  BTL_SICKCONT cont;
  SICKCONT_Unite32bit( &cont, cont_high, cont_low );

  m_fldSim->ChangeGround( args[0], cont );
  return true;
}
/**
 *  特定ポケモン依存のフィールドエフェクトを除去  args[0]:pokeID
 */
bool BTL_CLIENT::scProc_OP_DelFldEffDepend( int* , const int* args )
{
  m_fldSim->RemoveDependPokeEffect( args[0] );
  return true;
}
/**
 *  フィールドエフェクト除去   args[0]:FieldEffect
 */
bool BTL_CLIENT::scProc_OP_RemoveFldEff( int*, const int* args )
{
  BTL_PRINT("[client]  op remove fieldEff args=%d\n", args[0] );
  m_fldSim->RemoveEffect( (FieldStatus::EffectType)(args[0]) );
  return true;
}

bool BTL_CLIENT::scProc_OP_SetPokeCounter( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->COUNTER_Set( (BTL_POKEPARAM::Counter)(args[1]), args[2] );
  return true;
}
bool BTL_CLIENT::scProc_OP_IncKillCount( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->IncKillCount();
  return true;
}
bool BTL_CLIENT::scProc_OP_BatonTouch( int*, const int* args )
{
  BTL_POKEPARAM* user = m_pokeCon->GetPokeParam( args[0] );
  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( args[1] );
  target->CopyBatonTouchParams( user );
  return true;
}
bool BTL_CLIENT::scProc_OP_MigawariCreate( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->MIGAWARI_Create( args[1] );
  return true;
}
bool BTL_CLIENT::scProc_OP_MigawariDelete( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->MIGAWARI_Delete( );
  return true;
}
bool BTL_CLIENT::scProc_OP_SetFakeSrc( int* , const int* args )
{
  u8 clientID = args[0];
  //u8 memberIdx = args[1];

  BTL_PARTY* party = m_pokeCon->GetPartyData( clientID );
  m_mainModule->SetIllusionForParty(  party, clientID );
  return true;
}
bool BTL_CLIENT::scProc_OP_ClearConsumedItem( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->ClearConsumedItem( );
  return true;
}
/**
 *  特定ポケ依存の状態異常を回復  args[0]:回復するポケID, [1]:依存対象ポケID
 */
bool BTL_CLIENT::scProc_OP_CureSickDependPoke( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->CureWazaSickDependPoke( args[1] );
  return true;
}

/**
 *  ワザダメージ記録追加
 *  args [0]:defPokeID [1]:atkPokeID [2]:pokePos [3]:wazaType [4]:WazaDamageType [5]:wazaID [6]:damage
 */
bool BTL_CLIENT::scProc_OP_AddWazaDamage( int*, const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  WazaDamageType damageType = static_cast<WazaDamageType>( args[4] );

  BTL_POKEPARAM::WAZADMG_REC rec;
  BTL_POKEPARAM::WAZADMG_REC_Setup( &rec, args[1], args[2], args[5], args[3], args[6], damageType );
  bpp->WAZADMGREC_Add( &rec );

  return true;
}
/**
 *  ポケモン１体ターンチェック処理
 *  args [0]:pokeID
 */
bool BTL_CLIENT::scProc_OP_TurnCheck( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );

  bpp->TurnCheck( );
  bpp->CombiWaza_ClearParam( );

  return true;
}
/**
 *  フィールドエフェクトターンチェック処理
 */
bool BTL_CLIENT::scProc_OP_TurnCheckField( int* , const int*  )
{
  m_fldSim->TurnCheck( NULL );
  return true;
}

/**
 *  努力値加算処理
 *  args [0]:pokeID [1]:hp [2]:atk [3]:def [4]:agi [5]:sp_atk [6]:sp_def
 */
bool BTL_CLIENT::scProc_OP_SetDoryoku( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );

  for(u32 i=0; i<GFL_NELEMS(calc::PokePowerIDTable); ++i)
  {
    if( args[i+1] )
    {
      bpp->AddEffortPower( calc::PokePowerIDTable[i], args[i+1] );
    }
  }

  return true;
}

/**
 * @brief 位置エフェクトの開始通知
 * @param args[0] 位置エフェクトID( BtlPosEffect )
 * @param args[1] 位置ID( BtlPokePos )
 * @param args[2] 位置エフェクトパラメータ( PosEffect::EffectParam )
 */
bool BTL_CLIENT::scProc_OP_StartPosEff( int* , const int* args )
{
  BtlPosEffect posEffect = static_cast<BtlPosEffect>( args[0] );
  BtlPokePos   pos       = static_cast<BtlPokePos>( args[1] );

  PosEffect::EffectParam effectParam;
  effectParam.Raw.param1 = static_cast<u32>( args[2] );

  PosEffectStatus* posEffectStatus = m_pokeCon->GetPosEffectStatus( pos, posEffect );
  posEffectStatus->SetEffect( effectParam );
  return true;
}

/**
 * 位置エフェクトの終了通知
 * @param args[0] 位置エフェクトID( BtlPosEffect )
 * @param args[1] 位置ID( BtlPokePos )
 */
bool BTL_CLIENT::scProc_OP_RemovePosEff( int* , const int* args )
{
  BtlPosEffect     posEffect       = static_cast<BtlPosEffect>( args[0] );
  BtlPokePos       pos             = static_cast<BtlPokePos>( args[1] );
  PosEffectStatus* posEffectStatus = m_pokeCon->GetPosEffectStatus( pos, posEffect );
  posEffectStatus->RemoveEffect();
  return true;
}

/**
 * @brief 位置エフェクトのパラメータ更新
 * @param args[0] 位置エフェクトID( BtlPosEffect )
 * @param args[1] 位置ID( BtlPokePos )
 * @param args[2] 位置エフェクトパラメータ( PosEffect::EffectParam )
 */
bool BTL_CLIENT::scProc_OP_UpdatePosEffectParam( int* , const int* args )
{
  BtlPosEffect posEffect = static_cast<BtlPosEffect>( args[0] );
  BtlPokePos   pos       = static_cast<BtlPokePos>( args[1] );

  PosEffect::EffectParam effectParam;
  effectParam.Raw.param1 = static_cast<u32>( args[2] );

  PosEffectStatus* posEffectStatus = m_pokeCon->GetPosEffectStatus( pos, posEffect );
  posEffectStatus->SetEffectParam( effectParam );
  return true;
}


/**
 *  PGL Record 記録
 *  args [0]:defPokeID [1]:atkPokeID [2]:atkWaza
 */
bool BTL_CLIENT::scProc_OP_PGLRecord( int* , const int* args )
{
  BTL_POKEPARAM* defPoke = m_pokeCon->GetPokeParam( args[0] );
  
  if( (args[1] != BTL_POKEID_NULL)
  &&  (args[2] != WAZANO_NULL)
  ){
    BTL_POKEPARAM* atkPoke = m_pokeCon->GetPokeParam( args[1] );
    MainModule::PGLRecParam  recParam( atkPoke, (WazaID)args[2] );
    m_mainModule->NotifyPGLRecord( defPoke, &recParam );
  }
  else{
    m_mainModule->NotifyPGLRecord( defPoke, NULL );
  }
  
  return true;
}



/**
 * @brief サーバーコマンド SC_OP_SIDEEFFECT_ADD
 *        サイドエフェクトを追加する
 *
 * @param args[0]  操作対象の陣営( BtlSide )
 * @param args[1]  操作対象のサイドエフェクト( BtlSideEffect )
 * @param args[2]  追加するサイドエフェクトの継続パラメータ( BTL_SICKCONT上位32bit )
 * @param args[3]  追加するサイドエフェクトの継続パラメータ( BTL_SICKCONT下位32bit )
 */
bool BTL_CLIENT::scProc_OP_SideEffect_Add( int* , const int* args )
{
  BtlSide       side       = static_cast<BtlSide>( args[0] );
  BtlSideEffect sideEffect = static_cast<BtlSideEffect>( args[1] );
  u32           cont_high  = static_cast<u32>( args[2] );
  u32           cont_low   = static_cast<u32>( args[3] );

  BTL_SICKCONT contParam;
  SICKCONT_Unite32bit( &contParam, cont_high, cont_low );

  SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatus( side, sideEffect );
  sideEffectStatus->AddEffect( contParam );

  return true;
}

/**
 * @brief サーバーコマンド SC_OP_SIDEEFFECT_REMOVE
 *        サイドエフェクトを解除する
 *
 * @param args[0]  操作対象の陣営( BtlSide )
 * @param args[1]  操作対象のサイドエフェクト( BtlSideEffect )
 */
bool BTL_CLIENT::scProc_OP_SideEffect_Remove( int* , const int* args )
{
  BtlSide       side       = static_cast<BtlSide>( args[0] );
  BtlSideEffect sideEffect = static_cast<BtlSideEffect>( args[1] );

  SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatus( side, sideEffect );
  sideEffectStatus->RemoveEffect();
  return true;
}

/**
 * @brief サーバーコマンド SC_OP_SIDEEFFECT_INC_TURNCOUNT
 *        サイドエフェクトの経過ターン数をインクリメントする
 *
 * @param args[0]  操作対象の陣営( BtlSide )
 * @param args[1]  操作対象のサイドエフェクト( BtlSideEffect )
 */
bool BTL_CLIENT::scProc_OP_SideEffect_IncTurnCount( int* , const int* args )
{
  BtlSide       side       = static_cast<BtlSide>( args[0] );
  BtlSideEffect sideEffect = static_cast<BtlSideEffect>( args[1] );

  SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatus( side, sideEffect );
  sideEffectStatus->IncTurnCount();
  return true;
}

/**
 * @brief サーバーコマンド SC_OP_PUBLISH_CLIENT_INFO_APPEARED_POKEMON
 *        クライアント公開情報を更新する( ポケモンが場に出たことを記録 )
 *
 * @param args[0]  場に出たポケモンのID
 */
bool BTL_CLIENT::scProc_OP_PublishClientInformation_AppaearPokemon( int* , const int* args )
{
  const u8                 pokeId     = args[0];
  const u8                 clientId   = m_mainModule->PokeIDtoClientID( pokeId );
  ClientPublicInformation* clientInfo = m_mainModule->GetClientPublicInformation( clientId );

  if( clientInfo ) {
    clientInfo->SetPokemonAppearedOnBattleField( pokeId );
  }
  return true;
}

/**
 * @brief サーバーコマンド SC_OP_PUBLISH_CLIENT_INFO_HAVE_POKEMON_ITEM
 *        クライアント公開情報を更新する( ポケモンが道具を持っているかどうか )
 *
 * @param args[0]  更新対象ポケモンのID
 * @param args[1]  更新対象ポケモンが道具を持っているかどうか
 */
bool BTL_CLIENT::scProc_OP_PublishClientInformation_HavePokemonItem( int* , const int* args )
{
  const u8                 pokeId     = args[0];
  const bool               haveItem   = ( args[1] != 0 );
  const u8                 clientId   = m_mainModule->PokeIDtoClientID( pokeId );
  ClientPublicInformation* clientInfo = m_mainModule->GetClientPublicInformation( clientId );


  if( clientInfo ) {
    clientInfo->SetPokemonHaveItem( pokeId, haveItem );
  }
  return true;
}

/**
 * @brief サーバーコマンド SC_OP_SET_CURRENT_ROYAL_RANKING
 *        バトルロイヤルの現在順位を更新する
 *
 * @param args[0]  クライアント0の現在順位
 * @param args[1]  クライアント1の現在順位
 * @param args[2]  クライアント2の現在順位
 * @param args[3]  クライアント3の現在順位
 */
bool BTL_CLIENT::scProc_OP_SetCurrentRoyalRanking( int* , const int* args )
{
  m_royalRankingContainer.SetClientRanking( 0, args[0] );
  m_royalRankingContainer.SetClientRanking( 1, args[1] );
  m_royalRankingContainer.SetClientRanking( 2, args[2] );
  m_royalRankingContainer.SetClientRanking( 3, args[3] );
  return true;
}


/*
 *  能力基本値書き換え  [0]:pokeID, [1]:statusID [2]:value
 */
bool BTL_CLIENT::scProc_OP_SetStatus( int* , const int* args )
{
  BTL_POKEPARAM::ValueID statusID = (BTL_POKEPARAM::ValueID)(args[1]);

  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->SetBaseStatus( statusID, args[2] );
  return true;
}
/*
 *  体重値書き換え  [0]:pokeID, [2]:weight
 */
bool BTL_CLIENT::scProc_OP_SetWeight( int* , const int* args )
{
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( args[0] );
  bpp->SetWeight( args[1] );
  return true;
}
/*============================================================================================*/
/* 可変個の引数＆ワザエフェクトとの同時処理をうまくさばくための仕組み                         */
/*============================================================================================*/

/**
 * システムの可変個引数ワークを一時的に別のものと差し替え
 * @param  newArgs    差し替え後の新しい可変個引数ワーク
 * @return const VariableArgs*   差し替え前の古い可変個引数ワーク
 */
const BTL_CLIENT::VariableArgs*
  BTL_CLIENT::PushTmpVariableArgsWork( const VariableArgs* newArgs )
{
  const VariableArgs* oldArgs = m_tmpVariableArgs;
  m_tmpVariableArgs = newArgs;
  return oldArgs;
}
/**
 * PushTmpVariableArgsWork で一時的に差し替えたシステムの可変個引数ワークを元に戻す
 * @param oldArgs PushTmpVariableArgsWorkの戻り値で帰ってきた、古いワーク
 */
void  BTL_CLIENT::PopTmpVariableArgsWork( const VariableArgs* oldArgs )
{
  m_tmpVariableArgs = oldArgs;
}
/**
 * システムの可変個引数ワークから、引数の数を取得する
 * @return
 */
u8 BTL_CLIENT::GetVariableArgsCount( void ) const
{
  if( m_tmpVariableArgs ){
    return m_tmpVariableArgs->GetCount();
  }
  return m_stdVariableArgs.GetCount();
}
/**
 * システムの可変個引数ワークから、引数を取得する
 * @param idx 引数Index
 * @return 引数
 */
int  BTL_CLIENT::GetVariableArgs( u8 idx ) const
{
  if( m_tmpVariableArgs ){
    return m_tmpVariableArgs->GetArg( idx );
  }
  return m_stdVariableArgs.GetArg( idx );
}

/**
 * ctor : 可変個引数の格納クラス
 */
BTL_CLIENT::VariableArgs::VariableArgs()
{
  this->Clear();
}
/**
 * 可変個引数の格納ワークを初期化
 */
void BTL_CLIENT::VariableArgs::Clear( void )
{
  m_cnt = 0;
  for(u32 i=0; i<GFL_NELEMS(m_args); ++i) {
    m_args[i] = 0;
  }
}
/**
 * 可変個引数の格納コマンドパラメータを解釈して引数を格納する
 * @param cmd_args  コマンドパラメータ
 */
void BTL_CLIENT::VariableArgs::Setup( const int* cmd_args )
{
  m_cnt = cmd_args[0];

  if( m_cnt < GFL_NELEMS(m_args) )
  {
    for(u32 i=0; i<m_cnt; ++i){
      m_args[i] = cmd_args[i+1];
    }
  }
  else{
    GFL_ASSERT_MSG(0, "ExArgCnt=%d", m_cnt);
  }
}
/**
 * 格納した引数の個数を返す
 * @return 引数の個数
 */
u8 BTL_CLIENT::VariableArgs::GetCount( void ) const
{
  return m_cnt;
}
/**
 * 格納した引数を取得
 * @param idx 引数Index
 * @return  引数
 */
int BTL_CLIENT::VariableArgs::GetArg( u8 idx ) const
{
  if( idx < GFL_NELEMS(m_args) ){
    return m_args[idx];
  }
  GFL_ASSERT(0);
  return 0;
}


//------------------------------------------------------------------------------------------------------
// 外部モジュールからの情報取得処理
//------------------------------------------------------------------------------------------------------
u8 BTL_CLIENT::GetClientID( void ) const
{
  return m_myID;
}

const BTL_PARTY* BTL_CLIENT::GetParty( void ) const
{
  return m_myParty;
}

BtlWeather BTL_CLIENT::GetWeather( void ) const
{
  return m_fldSim->GetWeather();
}

/**
 * @brief 経過ターン数を取得する
 */
u32 BTL_CLIENT::GetTurnCount( void ) const
{
  return m_turnCount;
}

//=============================================================================================
/**
 * 現在、行動選択処理中のポケモン位置IDを取得
 *
 * @param   client
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BTL_CLIENT::GetProcPokePos( void ) const
{
  return m_mainModule->GetClientPokePos( m_myID, m_procPokeIdx );
}

/**
 * 道具やワザの効果により、選択できないワザになっているか判定
 * @param bpp
 * @param waza
 * @return 選択できないワザであれば true
 */
bool  BTL_CLIENT::IsUnselectableWaza( const BTL_POKEPARAM* bpp, WazaNo  waza ) const
{
  return is_unselectable_waza( bpp, waza, false, NULL );
}
/**
 * 自分のパーティ内Indexから、ポケモンIDの短bit版（自分に割り振られる先頭IDからの差分）を取得
 */
u8  BTL_CLIENT::calcShortPokeID( u8 partyIdx ) const
{
  const BTL_POKEPARAM* bpp = m_myParty->GetMemberDataConst( partyIdx );
  if( bpp )
  {
    return MainModule::PokeIDtoShortID( bpp->GetID() );
  }
  else{
    GFL_ASSERT(0);
    return 0;
  }
}

/*============================================================================================*/
/* クライアントごとのメガシンカ実行フラグ管理                                                 */
/*============================================================================================*/

u8 BTL_CLIENT::s_bMegaEvolved[ BTL_CLIENT_MAX ];

void BTL_CLIENT::s_ClearMegaEvoFlag( u8 clientID )
{
  if( clientID < GFL_NELEMS(s_bMegaEvolved) ){
    s_bMegaEvolved[ clientID ] = false;
  }else{
    GFL_ASSERT(0);
  }
}
bool BTL_CLIENT::s_IsMegaEvolved( u8 clientID )
{
  if( clientID < GFL_NELEMS(s_bMegaEvolved) ){
    return s_bMegaEvolved[ clientID ];
  }
  GFL_ASSERT(0);
  return false;
}
void BTL_CLIENT::s_SetMegaEvolved( u8 clientID )
{
  if( clientID < GFL_NELEMS(s_bMegaEvolved) ){
    s_bMegaEvolved[ clientID ] = true;
  }
  else{
    GFL_ASSERT(0);
  }

}

/*============================================================================================*/
/* クライアントごとのウルトラバースト実行フラグ管理                                                 */
/*============================================================================================*/

u8 BTL_CLIENT::s_bUltraBursted[BTL_CLIENT_MAX];

void BTL_CLIENT::s_ClearUltraBurstFlag(const u8 clientID)
{
  if (clientID < GFL_NELEMS(s_bUltraBursted)){
    s_bUltraBursted[clientID] = false;
  }
  else{
    GFL_ASSERT(0);
  }
}
bool BTL_CLIENT::s_IsUltraBursted(const u8 clientID)
{
  if (clientID < GFL_NELEMS(s_bUltraBursted)){
    return s_bUltraBursted[clientID];
  }
  GFL_ASSERT(0);
  return false;
}
void BTL_CLIENT::s_SetUltraBursted(const u8 clientID)
{
  if (clientID < GFL_NELEMS(s_bUltraBursted)){
    s_bUltraBursted[clientID] = true;
  }
  else{
    GFL_ASSERT(0);
  }
}



/*============================================================================================*/
/* デバッグ用AIクライアントを操作するためのシステム                                           */
/*============================================================================================*/
#if PM_DEBUG
BattleViewSystem*   BTL_CLIENT::s_ViewCore;
int                 BTL_CLIENT::s_ControlableAIClientID;
int                 BTL_CLIENT::s_YubiCtrlPos;

/**
 *  AI操作システム：現在、ＡＩクライアントとしてＵＩを呼び出しているかどうか判定
 */
bool BTL_CLIENT::DebugAICtrl_IsUsing( void )
{
  return ( s_ControlableAIClientID != BTL_CLIENTID_NULL );
}

/**
 *  AI操作システム：初期化
 */
void BTL_CLIENT::DebugAICtrl_Init( void )
{
  s_ViewCore = NULL;
  s_ControlableAIClientID = BTL_CLIENTID_NULL;
}

/**
 *  AI操作システム：１クライアント操作終了->次のクライアントへ権限委譲
 */
bool BTL_CLIENT::debugAICtrl_Delegate( void )
{

  u8 clientID = m_myID + 1;
  while( clientID < BTL_CLIENT_MAX )
  {
    if( m_mainModule->IsExistClient(clientID) )
    {
      s_ControlableAIClientID = clientID;
      s_ViewCore = m_viewCore;
      debugAICtrl_RestoreViewClient();
      return true;
    }
    ++clientID;
  }

  DebugAICtrl_Init();
  debugAICtrl_RestoreViewClient();
  return false;
}
/**
 *  AI操作システム：描画モジュールの管理クライアントを自分から元に戻す
 */
void BTL_CLIENT::debugAICtrl_RestoreViewClient( void )
{
  if( m_viewOldClient != NULL )
  {
    m_viewCore->SetTmpClient( m_viewOldClient );
    m_viewCore = NULL;
    m_viewOldClient = NULL;
  }
}
/**
 *  AI操作システム：操作権限が自分になっているか判定
 */
bool BTL_CLIENT::debugAICtrl_IsMyFase( void )
{
  if( s_ControlableAIClientID == m_myID )
  {
    if( m_viewCore == NULL )
    {
      m_viewCore = s_ViewCore;
      m_viewOldClient = m_viewCore->SetTmpClient( this );
    }
    return true;
  }
  return false;
}
#endif  // #if PM_DEBUG


/*============================================================================================*/
/* CmdQue の詳細を出力する                                                                    */
/*============================================================================================*/

#if PM_DEBUG
/**
 *  デバッグ用コマンドQue出力
 */
void BTL_CLIENT::printQue( const char* caption, SCQUE* que )
{
#if USE_BTLV_PRINT
  u32 cmdNameMaxLen = scmd::SCMD_GetMaxLabelLength();

  BTL_PRINT("==== cmd que at %s ====\n", caption );
  BTL_PRINT(" rp:%d /  wp:%d\n", que->readPtr, que->writePtr );

  u32 rp_save = que->readPtr;
  int  cmdArgs[ BTL_SERVERCMD_ARG_MAX ];
  int  cmdCount = 0;

  que->readPtr = 0;

  while( !SCQUE_IsFinishRead(que) )
  {
    gfl2::std::MemClear( cmdArgs, sizeof(cmdArgs) );

    u32  prev_rp = que->readPtr;

    ServerCmd  cmd = scmd::SCQUE_Read( que, cmdArgs, GFL_NELEMS(cmdArgs) );
    const char* cmd_name = scmd::SCMD_GetLabelString( cmd );

    BTL_PRINT(" %03d::%03d ", cmdCount++, prev_rp);

    if( cmd_name )
    {
      BTL_PRINT( "[%s] ", cmd_name );
      u32 len;
      for(len=0; cmd_name[len] != '\0'; ++len);
      while(len++ < cmdNameMaxLen){
        BTL_PRINT(" ");
      }
    }
    else
    {
      BTL_PRINT( "[????????] " );
    }

    u32 i=0;
    for(i=0; i<GFL_NELEMS(cmdArgs); ++i){
      BTL_PRINT("%08x, ", cmdArgs[i] );
    }
    BTL_PRINT("\n");
  }
  que->readPtr = rp_save;
#endif
}
#endif
GFL_NAMESPACE_END(btl)


#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif
