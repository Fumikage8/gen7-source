//=============================================================================================
/**
 * @file    btl_mainmodule.cpp
 * @brief   XY バトルシステムメイン
 * @author  taya
 *
 * @date  2010.12.16  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugLogWindow.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/MyStatus.h>
#include <Savedata/include/MyItemSave.h>
#include <Savedata/include/ZukanSave.h>
#include <Sound/include/Sound.h>
#include <Battle/include/battle_SetupParam.h>
#include <System/include/GflUse.h>
#include <System/include/PmlUse.h>
#include <System/source/pokelib/NijiEvolveManager.h>

#include "./btl_common.h"
#include "./btl_ui.h"
#include "./btl_calc.h"
#include "./btl_net.h"
#include "./btl_pokecon.h"
#include "./btl_adapter.h"
#include "./btl_server.h"
#include "./btl_sound.h"
#include "./btl_ServerFlow.h"
#include "./btl_sound.h"
#include "./btl_PokeID.h"
#include "./btl_BattleRule.h"
#include "./btl_BattleResult.h"
#include "./btl_PokemonPosition.h"
#include "./btl_ServerClientFactory.h"
#include "./btl_SendDataContainer.h"
#include "./btl_SendDataContainerFactory.h"
#include "./btl_ClientPublicInformation.h"
#include "./btl_ClientCommunicationObserver.h"
#include "./btl_ZenryokuWazaStatus.h"
#include "./btl_UltraBeast.h"
#include "./btl_FormChange.h"
#include "./btl_Oteire.h"
#include "./btl_RecordSave.h"
#include "./tr_ai/btl_BattleAiSystem.h"
#include "./tr_ai/btl_AiScript.h"
#include "./btl_mainmodule.h"
// ロトムパワー
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"



#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)



/**
 * @brief コンストラクタ
 * @param heapSys    バトルシステムで使用するヒープ
 * @param heapView   ビューシステムで使用するヒープ
 * @param heapNet    ネットシステムで使用するヒープ
 */
MainModule::MainModule(
  BATTLE_SETUP_PARAM* setupParam,
  HeapHandle heapSys,
  HeapHandle heapView,
  HeapHandle heapNet,
  GameSys::GameManager* gameMan,
  gfl2::ui::DeviceManager *uiMan,
  GameSys::GameProc *battleProc
)
: m_heapSys( heapSys ),
  m_heapView( heapView ),
  m_heapNet( heapNet ),
  m_setupParam( setupParam ),
  m_gameMan( gameMan ),
  m_uiMan( uiMan ),
  m_battleProc(battleProc)
{
  m_pokeconForClient = GFL_NEW( heapSys ) POKECON( heapSys );
  m_pokeconForServer = GFL_NEW( heapSys ) POKECON( heapSys );
  m_clientCommunicationObserver = GFL_NEW( heapSys ) ClientCommunicationObserver();

  for( u32 i=0; i<BTL_CLIENT_NUM; ++i ) {
    m_clientPublicInformation[i] = GFL_NEW( heapSys ) ClientPublicInformation( m_pokeconForClient );
  }

#if defined( DEBUG_ONLY_FOR_taya )
  test_pokeIDRec();
#endif
}

MainModule::~MainModule()
{
  m_soundSys.ForceFlatBgmLayer();

  for( u32 i=0; i<BTL_CLIENT_NUM; ++i ) {
    GFL_SAFE_DELETE( m_clientPublicInformation[i] );
  }

  GFL_SAFE_DELETE( m_clientCommunicationObserver );
  GFL_SAFE_DELETE( m_pokeconForServer );
  GFL_SAFE_DELETE( m_pokeconForClient );
}


//=============================================================================================
/**
 * 初期化処理ループ
 *
 * @retval  bool    初期化処理が完了したらtrueを返す
 */
//=============================================================================================
bool MainModule::Initialize( int *seq )
{
  switch( *seq ){
  case 0:
    {
      BattleAiSystem::InitSystem( m_heapSys );

      m_setupParam->capturedPokeIdx = BTL_PARTY_MEMBER_MAX;
      m_rule = m_setupParam->rule;
      m_playerStatus = m_setupParam->playerStatus[ BTL_CLIENT_PLAYER ];
      m_LimitTimeGame = m_setupParam->LimitTimeGame;
      m_LimitTimeClient = m_setupParam->LimitTimeClient;
      m_LimitTimeCommand = m_setupParam->LimitTimeCommand;
      m_fGetMoneyFixed = false;
      m_fLoseMoneyFixed = false;
      m_fCommError = false;
      m_fCommErrorMainQuit = false;
      m_fBGMFadeOutDisable = false;
      m_fSoundPushed       = false;
      m_isOteireButtonPressed = false;
      m_isOteireButtonDisable = false;
      m_oteireTargetPokeID = 0;
      m_MultiAIClientNum = 0;
      m_viewCore = NULL;
      m_serverResult = BTL_RESULT_MAX;  // 無効コードとして
      m_serverResultCause = RESULT_CAUSE_OTHER;
      m_zukanDataOnBattleStart = createZukanDataClone( m_heapSys, m_setupParam->zukanData );

      if(this->GetCommMode() == BTL_COMM_NONE ){
        m_msgSpeed = m_setupParam->configData->GetTalkingSpeed();
      }else{
        m_msgSpeed = Savedata::ConfigSave::MSG_FAST;
      }
      m_fWazaEffectEnable = (m_setupParam->configData->GetBattleAnimation() == Savedata::ConfigSave::BATTLE_ANIMATION_VISIBLE);
      m_subProc = NULL;

      m_soundSys.Initialize();

      m_pokeconForClient->Init( this, false );
      m_pokeconForServer->Init( this, true );

      for(u32 i=0; i<GFL_NELEMS(m_moneyDblUpCause); ++i){
        m_moneyDblUpCause[i] = 0;
      }

      Kentei_ClearField( m_setupParam );

      m_ppIllusionZoroArc = NULL;
      m_AIDataContainer = NULL;

      if( !IsRecordPlayMode() ){
        m_randomSeed = System::GflUse::GetPublicRand();
        m_randSys.Initialize(m_randomSeed);
        m_setupParam->recRandSeed = m_randomSeed;
      }else{
        m_randomSeed = m_setupParam->recRandSeed;
        m_randSys.Initialize( m_randomSeed );
      }

      calc::InitSys( &m_randSys, m_setupParam->bSakasaBattle, m_heapSys );
      ui::InitSystem( m_heapSys );

      #if PM_DEBUG
      BTL_CLIENT::DebugAICtrl_Init();
      #endif

      m_regularMoney =  calc::CalcWinMoney( m_setupParam );
      m_bonusMoney = 0;
      m_loseMoney = 0;

      AdapterFactory::InitSystem( m_setupParam->commMode, m_heapSys );
      SendDataContainerFactory::CreateContainer( m_heapSys );

      m_escapeInfo.Clear();

      m_changeMode = m_setupParam->configData->GetBattleRule();

      net::InitSystem( m_setupParam->commNetIDBit, m_heapNet , m_setupParam );

      POKECON::InitSystem( m_heapSys );

      for(u32 i=0; i<GFL_NELEMS(m_fMegaEvolved); ++i){
        m_fMegaEvolved[i] = false;
      }

      for (u32 i = 0; i < GFL_NELEMS(m_fUltraBursted); ++i){
        m_fUltraBursted[i] = false;
      }
      
      if( IsRecordPlayMode() ){
        SetupBattleSE();
      }
      
      if( m_setupParam->commMode == BTL_COMM_NONE )
      {
        //@バトル変更箇所
        //図鑑であったフラグの制御
        for( int i=0;i<BTL_CLIENT_MAX;i++ )
        {
          for( int j=0;j<BTL_PARTY_MEMBER_MAX;j++ )
          {
            if( i == BTL_CLIENTID_SA_PLAYER )
            {
              m_checkSeeAmount[i][j] = true;
            }
            else
            {
              m_checkSeeAmount[i][j] = false;
            }
          }
        }
      }

      (*seq)++;
    }
    break;

  case 1:
    net::Initialize();
    (*seq)++;
    break;

  case 2:
    if( m_setupParam->commMode != BTL_COMM_NONE )
    {//通信だったらエラーチェック
      if( net::CheckError() )
      {
        m_fCommError = true;
        m_fCommErrorMainQuit = true;
        return true;
      }
    }

    if( net::IsInitialized() ||
        m_setupParam->commMode == BTL_COMM_NONE )
    {
  //    net::InitializeListener();  //NMCat4132

      setSubProcForSetup( m_setupParam );
      m_subSeq = 0;

      trainerParam_Init();
      srcParty_Create();
      (*seq)++;
    }
    else{
      BTL_N_Printf( DBGSTR_line, __LINE__ );
    }
    break;

  case 3:
    if( callSubProc() )
    {
      BTL_N_Printf( DBGSTR_SETUP_DONE );

      if( m_fCommError )
      {
        m_fCommErrorMainQuit = true;
        return true;
      }

      net::InitializeListener();  //NMCat4132
      

      this->RECORDDATA_Inc( Savedata::Record::RECID_BATTLE_COUNT );
      return true;
    }
    break;
  }

  return false;
}

/**
 * @brief 図鑑データのクローンを生成する
 * @param heap      クローンの生成に使用するヒープ
 * @param original  クローンの元になる図鑑データ
 * @return  生成したクローン
 */
Savedata::ZukanData* MainModule::createZukanDataClone( HeapHandle heap, const Savedata::ZukanData* original ) const
{
  void* originalData = const_cast<Savedata::ZukanData*>( original )->GetData();
  Savedata::ZukanData* clone = GFL_NEW( heap ) Savedata::ZukanData();
  clone->SetData( originalData ); // メモリコピーされる
  return clone;
}

/**
 * バトル用SEを使う準備（時間がかかるため、暗転中を避けて任意タイミングで行う）
 */
void MainModule::SetupBattleSE( void )
{
  if( !m_fSoundPushed )
  {
    m_fSoundPushed = true;
    //ここでは行わずBattleViewSystemで一括して行います。
    //xy_snd::LoadAndPushBattleGlobalSE();
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief 技のダメージ相性を計算する
 *      ( 攻撃側からは知ることが出来ない防御側ポケモンの特性、道具等を考慮しない )
 *
 * @param   atkPokeID     攻撃側ポケモンのID
 * @param   defPokeID     防御側ポケモンのID
 * @param   waza          使用する技
 *
 * @return  技の相性
 */
//-----------------------------------------------------------------------------
BtlTypeAff MainModule::SimulationWazaAffinityOnlyAttacker( u8 atkPokeID, u8 defPokeID, WazaID waza )
{
  BTL_SERVER* server = ( m_server ) ? ( m_server ) : ( m_cmdCheckServer );

  if( server == NULL )
  {
    GFL_ASSERT(0);
    return pml::battle::TypeAffinity::TYPEAFF_1;
  }

  ServerFlow* serverFlow = server->GetFlowWork();
  return serverFlow->Hnd_SimulationAffinityOnlyAttacker( atkPokeID, defPokeID, waza );
}


//----------------------------------------------------------------------------------------------
/**
 *  Proc Main
 */
//----------------------------------------------------------------------------------------------
bool MainModule::MainLoop( int* seq )
{
  GFL_UNUSED( seq );

  if( m_mainLoop == NULL ){
    return true;
  }

  net::Update();

  m_soundSys.Main();
  m_clientCommunicationObserver->ObserveClinetCommunication();

  if( callMainLoop() )
  {
    storeBattleResult();

    //MCat調整要望リスト410/411対応
    if( this->isBgmFadeOutDisable()){
      this->BGMFadeOutDisable();
    }

    return true;
  }

  net::Update();

  return false;
}

/**
 * @brief 戦闘結果を BATTLE_SETUP_PARAM に格納する
 */
void MainModule::storeBattleResult( void )
{
  if( !IsRecordPlayMode() )
  {
    BtlResult result = checkWinner();

    Bspstore_RecordData();
    Bspstore_KenteiData();
    BattleResult::ApplyBattlePartyData( m_setupParam, m_pokeconForServer, m_pokeconForClient, this, m_myClientID );
    BattleResult::ApplyZenryokuPowerResult( m_setupParam, *m_clientZenryokuWazaStatus[ m_myClientID ] );
    BattleResult::ApplyOteireTarget( m_setupParam, this, m_pokeconForServer, m_myClientID );

    {
      const BTL_SERVER* server = ( m_server == NULL ) ? ( m_cmdCheckServer ) : ( m_server );
      if( server ) 
      {
        BattleResult::ApplyRecordHeader( m_setupParam, this, m_myClientID, result );
      }
    }

    if( this->GetRule() == BTL_RULE_ROYAL )
    {
      BattleResult::ApplyBattleRoyalResult( m_setupParam, this, m_pokeconForClient, m_myClientID );
    }

    const u32 bonusMoney = this->GetBonusMoney();

    switch( result ){
    case BTL_RESULT_WIN:
      m_setupParam->getMoney = (m_regularMoney + bonusMoney);
      break;
    case BTL_RESULT_LOSE:
    case BTL_RESULT_DRAW:
      m_setupParam->getMoney = (int)(m_loseMoney) * -1;
      break;
    }

    RecordSave::AddNekonikobanTotal( *this, result, bonusMoney );

  }
  // 録画再生時なら最後までデータを読み込んだか判定
  else
  {
    m_setupParam->recPlayCompleteFlag = checkRecReadComplete();
  }

  BTL_PRINT( "[MainModule] m_setupParam->cmdIllegalFlag = %d\n", m_setupParam->cmdIllegalFlag );
}


/*
 *  @brief バトル終了時のBGMフェードアウトを無効化するか？
 *  @retval true   無効化する
 *  @retval false  無効化しない
 */
bool MainModule::isBgmFadeOutDisable( void ) const
{
  // @fix NMCat[11] 戦闘終了後に複数処理を重ねるとBGMがなくなる
  // お手入れボタンが押されていて、進化するポケモンが存在しないなら、
  // バトルの後、勝利BGMを継続したままリフレ画面に遷移させるために、戦闘終了時に BGM をフェードアウトさせないようにする
  // この条件のフォローはbtl_client.cppで行っている
  // まとめたほうがキレイな気もするが、差分を少なくするためにそのままにする
  // if( m_mainModule->IsOteireButtonPressed() && !isEvolveEnablePokeExsist() )

  BtlResult result = m_setupParam->result;  //格納された結果で動く

  //フラグがたっているときだけ
  if( !this->GetSetupStatusFlag( BTL_STATUS_FLAG_WIN_BGM_CONTINUE ) ){
    return false;
  }
  //録画モード以外
  if( IsRecordPlayMode() ){
    return false;
  }

  //勝った/逃げた/逃げられたときだけ(捕獲/引き分け/負けた、は今までと同じ挙動でいいので変えない)
  //捕獲はそもそも継続フラグがすでに捕獲成功シーケンスで立っている
  if( result != BTL_RESULT_WIN &&
      result != BTL_RESULT_RUN &&
      result != BTL_RESULT_RUN_ENEMY )
  {
    return false;
  }
  //トレーナー戦か野生戦だけ
  if( this->GetCompetitor() != BTL_COMPETITOR_TRAINER &&
      this->GetCompetitor() != BTL_COMPETITOR_WILD )
  {
    return false;
  }

  //進化がないときだけ
  if( this->GetClient(BTL_CLIENT_PLAYER)->isEvolveEnablePokeExsist() ){
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------
/**
 *  Proc Main - Draw
 */
//----------------------------------------------------------------------------------------------
void MainModule::PreDrawLoop( int* seq )
{
  GFL_UNUSED(seq);
  //UpdateとDrawがついになって呼ばれているかはBtlvCoreが見ています。
  if( m_viewCore )
  {
    m_viewCore->PreDraw();
  }
}

void MainModule::DrawLoop( int* seq , gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_UNUSED(seq);
  //UpdateとDrawがついになって呼ばれているかはBtlvCoreが見ています。
  if( m_viewCore )
  {
    m_viewCore->Draw(displayNo);
  }
}

//----------------------------------------------------------------------------------
/**
 * 録画再生データを全クライアントが最後まで読んだか判定
 */
//----------------------------------------------------------------------------------
bool MainModule::checkRecReadComplete( void )
{
  for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( IsExistClient(i) )
    {
      if( m_recReader.IsReadComplete(i) == false ){
        return false;
      }
    }
  }
  return true;
}
//----------------------------------------------------------------------------------------------
/**
 *  Proc Quit
 */
//----------------------------------------------------------------------------------------------
bool MainModule::Finalize( int* seq )
{
  switch( *seq ){
  case 0:
    if( m_fBGMFadeOutDisable == false ){
      m_soundSys.FadeOutBGM( Sound::BGM_FADE_FAST );
    }
    
    m_subSeq = 0;
    (*seq)++;

  case 1:
    if( m_fBGMFadeOutDisable == false )
    {
#if 0
      if( !m_soundSys.CheckFadeOnBGM() )
      {
        m_soundSys.StopBGM();
        (*seq)++;
      }
#else
      (*seq)++;
#endif
    }
    else{
      (*seq)++;
    }
    break;
  case 2:
    BTL_Printf("クリーンアッププロセス１\n");
    if( m_ppIllusionZoroArc ){
      GFL_DELETE m_ppIllusionZoroArc;
      m_ppIllusionZoroArc = NULL;
    }
    if( m_AIDataContainer ){
      net::AIDC_Delete( m_AIDataContainer );
      m_AIDataContainer = NULL;
    }

    POKECON::QuitSystem();
    ui::QuitSystem();
    calc::QuitSys();
    srcParty_Delete();
    trainerParam_Clear();
    SendDataContainerFactory::DeleteContainer();
    AdapterFactory::QuitSystem();
    setSubProcForCleanUp( m_setupParam );
    (*seq)++;
    break;

  case 3:
    if( callSubProc() )
    {
      (*seq)++;
    }
    break;

  case 4:
    if( m_fSoundPushed ){
      //ここでは行わずBattleViewSystemで一括して行います。
      //xy_snd::PopAndUnloadBattleGlobalSE();
      m_fSoundPushed = false;
    }
    net::QuitSystem();
    BattleAiSystem::QuitSystem();
    return true;
  }

  return false;
}


//======================================================================================================
// 通信エラー時の強制終了処理
//======================================================================================================
void MainModule::StartForceRemoveView( void )
{
}
bool MainModule::WaitForceRemoveView( void )
{
  if( m_viewCore )
  {
    return m_viewCore->FinalizeForce();
  }

  return true;
}

//======================================================================================================
//======================================================================================================

/**
 *  メインループ関数設定
 */
void MainModule::setMainLoop( pMainProc proc )
{
  m_mainLoop    = proc;
}
/**
 *  メインループ関数呼び出し
 *
 * @retval  bool      終了していたらtrue
 */
bool MainModule::callMainLoop( void )
{
  if( m_mainLoop != NULL )
  {
    return (this->*m_mainLoop)();
  }
  return true;
}
/**
 *  サブプロセス関数設定
 */
void MainModule::setSubProc( pSubProc subProc )
{
  m_subProc = subProc;
  m_subSeq = 0;
}
/**
 *  サブプロセス関数呼び出し
 *
 * @retval  bool      終了していたらtrue
 */
bool MainModule::callSubProc( void )
{
  if( m_subProc != NULL )
  {
    return (this->*m_subProc)( &m_subSeq );
  }
  return true;
}


//======================================================================================================
//======================================================================================================


// 各種モジュール生成＆関連付けルーチンを設定
void MainModule::setSubProcForSetup( const BATTLE_SETUP_PARAM* setup_param )
{
  if( setup_param->commMode == BTL_COMM_NONE )
  {
    BTL_UTIL_SetPrintType( BTL_PRINTTYPE_STANDALONE );

    switch( setup_param->rule ){
    case BTL_RULE_SINGLE:
      setSubProc( &MainModule::setup_alone_single );
      break;
    case BTL_RULE_DOUBLE:
      if( setup_param->multiMode == BTL_MULTIMODE_NONE ){
        setSubProc( &MainModule::setup_alone_double );
      }else{
        setSubProc( &MainModule::setup_alone_double_multi );
      }
      break;
    case BTL_RULE_ROYAL:
      setSubProc( &MainModule::setup_alone_royal );
      break;
    default:
      GFL_ASSERT(0);
      setSubProc( &MainModule::setup_alone_single );
      break;
    }
  }
  else
  {
    switch( setup_param->rule ){
    case BTL_RULE_SINGLE:
      setSubProc( &MainModule::setup_comm_single  );
      break;
    case BTL_RULE_DOUBLE:
      setSubProc( &MainModule::setup_comm_double );
      break;
    case BTL_RULE_ROYAL:
      setSubProc( &MainModule::setup_comm_royal  );
      break;
    default:
      GFL_ASSERT(0);
      setSubProc( &MainModule::setup_alone_single );
      break;
    }
  }
}
// 各種モジュール解放＆削除処理ルーチンを設定
void MainModule::setSubProcForCleanUp( const BATTLE_SETUP_PARAM* setup_param )
{
  GFL_UNUSED( setup_param );
  setSubProc( &MainModule::cleanup_common );
}

//----------------------------------------------------------------------------------
/**
 * スタンドアローンセットアップ関数共通（マルチ以外）：トレーナーデータ設定
 */
//----------------------------------------------------------------------------------
void MainModule::setupCommon_TrainerParam( const BATTLE_SETUP_PARAM* sp )
{
  if( !IsRecordPlayMode() )
  {
    trainerParam_StorePlayer( &m_trainerParam[0], m_heapSys, m_playerStatus );
    if( IsRecordFightMode() )
    {
      BTL_PRINT("模擬戦です\n");
      if( sp->playerStatus[BTL_CLIENT_ENEMY1] ){
        BTL_PRINT(" Cliend_%d をプレイヤーパラメータで構築する\n", BTL_CLIENT_ENEMY1);
        trainerParam_StorePlayer( &m_trainerParam[BTL_CLIENT_ENEMY1], m_heapSys, sp->playerStatus[BTL_CLIENT_ENEMY1]  );
        return;
      }
      BTL_PRINT(" Cliend_%d はプレイヤーではないので、トレーナーデータで構築されるはず\n", BTL_CLIENT_ENEMY1);
    }

    if( sp->tr_data[BTL_CLIENT_ENEMY1] ){
      BTL_PRINT(" Cliend_%d を トレーナーデータで構築する\n", BTL_CLIENT_ENEMY1);
      trainerParam_StoreNPCTrainer( &m_trainerParam[BTL_CLIENT_ENEMY1], sp->tr_data[BTL_CLIENT_ENEMY1] );
    }

    //バトルフェスでWinMsgのために使っているので、通らないようにしておく
    if (GetSetupStatusFlag(BTL_STATUS_FLAG_BATTLE_FES) == false)
    {
      if( sp->tr_data[BTL_CLIENT_ENEMY2] ){
        BTL_PRINT(" Cliend_%d を トレーナーデータで構築する\n", BTL_CLIENT_ENEMY2);
        trainerParam_StoreNPCTrainer( &m_trainerParam[BTL_CLIENT_ENEMY2], sp->tr_data[BTL_CLIENT_ENEMY2] );
      }

      if( sp->tr_data[BTL_CLIENT_PARTNER] ){
        BTL_PRINT(" Cliend_%d を トレーナーデータで構築する\n", BTL_CLIENT_PARTNER);
        trainerParam_StoreNPCTrainer( &m_trainerParam[BTL_CLIENT_PARTNER], sp->tr_data[BTL_CLIENT_PARTNER] );
      }
    }
  }
  else
  {
    for( u32 clientID=0; clientID<BTL_CLIENT_NUM; ++clientID )
    {
      if( this->IsExistClient( clientID ) )
      {
        trainerParam_SetupForRecPlay( clientID );
      }
    }
  }

  //バトルフェス用のTRAINER_DATAを初期化
  if ( GetSetupStatusFlag(BTL_STATUS_FLAG_BATTLE_FES))
  {
    trainerParam_StorePlayer(&m_fesTrainerParam[0], m_heapSys, m_setupParam->playerStatus[0]);
    trainerParam_StorePlayer(&m_fesTrainerParam[1], m_heapSys, m_setupParam->playerStatus[2]);  //012ではなく023でOK
    trainerParam_StorePlayer(&m_fesTrainerParam[2], m_heapSys, m_setupParam->playerStatus[3]);
  }
}

//----------------------------------------------------------------------------------
/**
 * スタンドアローンセットアップ関数共通（マルチ以外）：クライアント録画再生モード設定
 */
//----------------------------------------------------------------------------------
void MainModule::setupCommon_SetRecplayerClientMode( const BATTLE_SETUP_PARAM* sp )
{
  if( IsRecordPlayMode() )
  {
    m_recReader.Init( sp->recBuffer, sp->recDataSize );
    m_client[0]->SetRecordPlayerMode( &m_recReader );
    m_client[1]->SetRecordPlayerMode( &m_recReader );

    if( m_client[2] )
    {
      m_client[2]->SetRecordPlayerMode( &m_recReader );
    }

    if( m_client[3] )
    {
      m_client[3]->SetRecordPlayerMode( &m_recReader );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * スタンドアローンセットアップ関数共通（マルチ含む）
 *  描画エンジン生成＆UIクライアントに関連付ける
 */
//----------------------------------------------------------------------------------
void MainModule::setupCommon_CreateViewModule( const BATTLE_SETUP_PARAM* sp, BtlBagMode bagMode )
{
  GFL_UNUSED(bagMode);

  // 描画エンジン生成、プレイヤークライアントに関連付ける
  BTL_CLIENT* uiClient = (IsRecordPlayMode())? m_client[sp->commPos] : m_client[m_myClientID];

  BTLV_INIT_PARAM initParam;
  initParam.mMainModule = this;
  initParam.mClient = uiClient;
  initParam.mPokeCon = m_pokeconForClient;
  initParam.mBagMode = bagMode;
  initParam.mMsgSpeed = m_msgSpeed;
#if PM_DEBUG
  initParam.mDebugParam = const_cast<BTLV_DEBUG_PARAM*>(&sp->btlvDebugParam);
#endif
  m_viewCore = GFL_NEW(m_heapView) BattleViewSystem(&initParam);

  uiClient->AttachViewCore( m_viewCore );
}

//----------------------------------------------------------------------------------
/**
 * @brief スタンドアローンセットアップ関数共通（マルチ含む）
 *        描画エンジン初期化
 * @retval true   初期化完了
 * @retval false  初期化中
 */
//----------------------------------------------------------------------------------
bool MainModule::setupCommon_InitViewModule( void )
{
  bool finished = m_viewCore->Initialize( m_heapView );

  if( !finished )
  {
    return false;
  }

  // 録画モードの時、最大ターン数を通知しておく
  if( IsRecordPlayMode() )
  {
    m_viewCore->CMD_RecPlayer_Init( m_recReader.GetTurnCount() );
  }

  // AIの実行に必要なヒープが、バトルシステムのヒープから確保できないため、
  // スクリプトの実行には「ビューシステムの一時ヒープ」を使用します。
  // このヒープは、行動選択終了後、ビューシステムが使用するため、
  // AIは、実行が終了した時点で確保したヒープを解放する必要があります。
  {
    HeapHandle heapForAiExec = m_viewCore->GetTempHeap();

    for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
    {
      if( m_client[i] )
      {
        m_client[i]->AttachAiExecHeap( heapForAiExec );
      }

      if( m_dummyClient[i] )
      {
        m_dummyClient[i]->AttachAiExecHeap( heapForAiExec );
      }
    }
  }

  return true;
}

/**
 * @brief クライアント毎の公開情報をセットアップする
 * @param setupParam  セットアップパラメータ
 */
void MainModule::setupCommon_ClientPublicInformation( const BATTLE_SETUP_PARAM& setupParam )
{
  for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    const MiseaiData* miseaiData = setupParam.miseaiData[ clientId ];
    if( miseaiData == NULL ) {
      continue;
    }

    this->setupCommon_ClientPublicInformation( m_clientPublicInformation[ clientId ], clientId, *miseaiData );
  }
}

/**
 * @brief クライアント毎の公開情報をセットアップする
 * @param[out] publicInfo  セットアップ対象の公開情報
 * @param      clientId    セットアップ対象のクライアントID
 * @param      miseaiData  相手に見せたデータ
 */
void MainModule::setupCommon_ClientPublicInformation( ClientPublicInformation* publicInfo, u8 clientId, const MiseaiData& miseaiData )
{
  for( u32 memberIndex=0; memberIndex<pml::PokeParty::MAX_MEMBERS; ++memberIndex )
  {
    u32 battlePokeId = BTL_POKEID_NULL;
    if( 0 <= miseaiData.memberSelectedIndex[ memberIndex ] ) {
      battlePokeId = this->GetClientBasePokeID( clientId ) + miseaiData.memberSelectedIndex[ memberIndex ]; // 選出されたメンバーは、選出順にPokeIDが設定されている
    }

    const pml::pokepara::PokemonParam* pokeParam = miseaiData.partyFullMember->GetMemberPointerConst( memberIndex );
    const MonsNo monsno = pokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ? ( MONSNO_TAMAGO ) : ( pokeParam->GetMonsNo() );
    const FormNo formno = pokeParam->GetFormNo();
    const Sex sex = pokeParam->GetSex();
    const u8 level = pokeParam->GetLevel();
    const bool haveItem = ( pokeParam->GetItem() != ITEM_DUMMY_DATA );
    publicInfo->PublishPokemon( memberIndex, battlePokeId, monsno, formno, sex, level, haveItem );
  }
}

//--------------------------------------------------------------------------
/**
 * スタンドアローン／シングルバトル
 */
//--------------------------------------------------------------------------
bool MainModule::setup_alone_single( int* seq )
{
  // server*1, client*2
  const BATTLE_SETUP_PARAM* sp = m_setupParam;
  BtlBagMode bagMode = checkBagMode( sp );

  switch( *seq ){
    case 0:
      setup_alone_common_ClientID_and_srcParty( sp );
      setupUnknownUBNickName( m_srcParty[ BTL_CLIENT_ENEMY1 ] );
      setupUnknownUBNickName( m_srcPartyForServer[ BTL_CLIENT_ENEMY1 ] );
      break;
    // トレーナーデータ設定
    case 1:
      setupCommon_TrainerParam( sp );
      break;
    case 2:
      m_pokeconForClient->Init( this, false );
      this->setSrcPartyToPokecon( m_pokeconForClient, 0, false );
      break;
    case 3:
      this->setSrcPartyToPokecon( m_pokeconForClient, 1, false );
      break;
    case 4:
      m_pokeconForServer->Init( this, true );
      this->setSrcPartyToPokecon( m_pokeconForServer, 0, true );
      break;
    case 5:
      this->setSrcPartyToPokecon( m_pokeconForServer, 1, true );
      break;
    case 6:
      setupCommon_CreateServerClient( m_heapSys, *m_setupParam );
      setupCommon_ClientPublicInformation( *m_setupParam );
      break;
    case 7:
      // 録画再生モード設定
      setupCommon_SetRecplayerClientMode( sp );
      break;
    case 8:
      // 描画エンジン生成＆UIクライアントに関連付ける
      setupCommon_CreateViewModule( sp, bagMode );
      ++(*seq);
      // fall throuh
    case 9:
      if( setupCommon_InitViewModule() )
      {
        break;
      }
      return false;
    case 10:
      // Server 始動
      m_server->Startup();
      setMainLoop( &MainModule::MainLoop_StandAlone );
      break;
    default:
      return true;
  }

  ++(*seq);

  return false;
}
//--------------------------------------------------------------------------
/**
 * スタンドアローン／ダブルバトル（通常）：セットアップ
 */
//--------------------------------------------------------------------------
bool MainModule::setup_alone_double( int* seq )
{
  // server*1, client*2 を作成
  const BATTLE_SETUP_PARAM* sp = m_setupParam;
  BtlBagMode bagMode = checkBagMode( sp );

  switch( *seq ){
    case 0:
      m_myClientID = 0;

      setup_alone_common_ClientID_and_srcParty( sp );
      break;
    case 1:
      // トレーナーデータ生成
      setupCommon_TrainerParam( sp );
      break;
    case 2:
      // パーティデータ生成
      m_pokeconForClient->Init( this, false );
      this->setSrcPartyToPokecon( m_pokeconForClient, 0, false );
      break;
    case 3:
      this->setSrcPartyToPokecon( m_pokeconForClient, 1, false );
      break;
    case 4:
      m_pokeconForServer->Init( this, true );
      this->setSrcPartyToPokecon( m_pokeconForServer, 0, true );
      break;
    case 5:
      this->setSrcPartyToPokecon( m_pokeconForServer, 1, true );
      break;
    case 6:
      setupCommon_CreateServerClient( m_heapSys, *m_setupParam );
      setupCommon_ClientPublicInformation( *m_setupParam );
      break;
    case 7:
      // 録画再生モード設定
      setupCommon_SetRecplayerClientMode( sp );
      break;
    case 8:
      // 描画エンジン生成＆UIクライアントに関連付ける
      setupCommon_CreateViewModule( sp, bagMode );
      ++(*seq);
      // fall throuh
    case 9:
      if( setupCommon_InitViewModule() )
      {
        break;
      }
      return false;
    case 10:
      // Server 始動
      m_server->Startup();
      setMainLoop( &MainModule::MainLoop_StandAlone );
      break;
    default:
      return true;
  }

  ++(*seq);
  return false;
}

//--------------------------------------------------------------------------
/**
 * スタンドアローン／ダブルバトル（マルチ含む）：セットアップ
 */
//--------------------------------------------------------------------------
bool MainModule::setup_alone_double_multi( int* seq )
{
  // server*1, client*2
  
  static const pSubProc funcs[] = {
    NULL,
    &MainModule::setup_alone_double_multi_00,
    &MainModule::setup_alone_double_multi_01,
    &MainModule::setup_alone_double_multi_02,
    &MainModule::setup_alone_double_multi_03,
    &MainModule::setup_alone_double_multi_04,
    &MainModule::setup_alone_double_multi_05,
    &MainModule::setup_alone_double_multi_06,
    &MainModule::setup_alone_double_multi_07,
    &MainModule::setup_alone_double_multi_08,
    &MainModule::setup_alone_double_multi_09,
  };

  if( *seq == 0 ){
    m_setupStep = 0;
    ++(*seq);
    return false;
  }
  else if( (*seq) < GFL_NELEMS(funcs) )
  {
    if( (this->*funcs[ *seq ])( &m_setupStep ) )
    {
      m_setupStep = 0;
      ++(*seq);
    }
    return false;
  }

  return true;
}
bool MainModule::setup_alone_double_multi_00( int* seq )
{
  const BATTLE_SETUP_PARAM* sp = m_setupParam;

  // 基本設定
  m_myClientID = 0;

  switch( sp->multiMode ){
  case BTL_MULTIMODE_NONE:  // 通常のダブルバトル = 基本設定のままでOK
    break;

  case BTL_MULTIMODE_PP_PP: // 通信マルチ
  case BTL_MULTIMODE_PP_AA: // 通信タッグマルチ
  default:
    break;

  case BTL_MULTIMODE_PA_AA: // AIマルチ
    break;

  case BTL_MULTIMODE_P_AA:  // AIタッグマルチ
    break;
  }
  return true;
}
bool MainModule::setup_alone_double_multi_01( int* seq )
{
  const BATTLE_SETUP_PARAM* sp = m_setupParam;
  setup_alone_common_ClientID_and_srcParty( sp );
  return true;
}
bool MainModule::setup_alone_double_multi_02( int* seq )
{
  const BATTLE_SETUP_PARAM* sp = m_setupParam;
  // トレーナーパラメータ設定（通常プレイ用）
  if( !IsRecordPlayMode() )
  {
    switch( *seq ){
      case 0:
        trainerParam_StorePlayer( &m_trainerParam[0], m_heapSys, m_playerStatus );
        break;
      case 1:
        trainerParam_StoreNPCTrainer( &m_trainerParam[1], sp->tr_data[BTL_CLIENT_ENEMY1] );
        break;
      case 2:
        if( IsExistClient(BTL_CLIENT_PARTNER) ){
          trainerParam_StoreNPCTrainer( &m_trainerParam[BTL_CLIENT_PARTNER], sp->tr_data[BTL_CLIENT_PARTNER] );
        }
        break;
      case 3:
        if( IsExistClient(BTL_CLIENT_ENEMY2) ){
          trainerParam_StoreNPCTrainer( &m_trainerParam[BTL_CLIENT_ENEMY2], sp->tr_data[BTL_CLIENT_ENEMY2] );
        }
        break;
      default:
        return true;
    }
    ++(*seq);
    return false;
  }
  // 録画再生用
  else
  {
    if( (*seq) < BTL_CLIENT_MAX )
    {
      if( IsExistClient(*seq) ){
        trainerParam_SetupForRecPlay( *seq );
      }
      ++(*seq);
      return false;
    }
    else{
      return true;
    }
  }
}
bool MainModule::setup_alone_double_multi_03( int* seq )
{
  // ポケモンパーティデータ生成
  m_pokeconForClient->Init( this, false );
  m_pokeconForServer->Init( this, true );
  return true;
}
bool MainModule::setup_alone_double_multi_04( int* seq )
{
  int  partyMax = BTL_CLIENT_MAX * 2; // server, client で２倍

  while( (*seq) < partyMax )
  {
    u8   clientID   = (*seq) / 2;
    bool bForServer = (*seq) & 1;
    bool bAddParty = false;

    if( IsExistClient(clientID) )
    {
      if( bForServer ){
        this->setSrcPartyToPokecon( m_pokeconForServer, clientID, true );
      }else{
        this->setSrcPartyToPokecon( m_pokeconForClient, clientID, false );
      }
      bAddParty = true;
    }
    ++(*seq);
    if( bAddParty ){
      break;
    }
  }

  return ( (*seq) >= partyMax );
}
bool MainModule::setup_alone_double_multi_05( int* seq )
{
  return true;
}
bool MainModule::setup_alone_double_multi_06( int* seq )
{
  setupCommon_CreateServerClient( m_heapSys, *m_setupParam );
  setupCommon_ClientPublicInformation( *m_setupParam );
  return true;
}
bool MainModule::setup_alone_double_multi_07( int* seq )
{
  const BATTLE_SETUP_PARAM* sp = m_setupParam;
  BtlBagMode  bagMode  = checkBagMode( sp );

  // 録画再生モード設定
  if( IsRecordPlayMode() )
  {
    m_recReader.Init( sp->recBuffer, sp->recDataSize );
    for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
    {
      if( IsExistClient(i) ){
        m_client[i]->SetRecordPlayerMode( &m_recReader );
      }
    }
  }

  // 描画エンジン生成＆UIクライアントに関連付ける
  setupCommon_CreateViewModule( sp, bagMode );
  setupCommon_InitViewModule();  // ビュワー生成後に1回Initializeを走らせないと危ない
  return true;
}

bool MainModule::setup_alone_double_multi_08( int* seq )
{
  return setupCommon_InitViewModule();
}

bool MainModule::setup_alone_double_multi_09( int* seq )
{
  // Server 始動
  m_server->Startup();
  setMainLoop( &MainModule::MainLoop_StandAlone );

  return true;
}

//--------------------------------------------------------------------------
/**
 * スタンドアローン／バトルロイヤル
 */
//--------------------------------------------------------------------------
bool MainModule::setup_alone_royal( int* seq )
{
  const BATTLE_SETUP_PARAM* sp = m_setupParam;
  BtlBagMode bagMode = checkBagMode( sp );

  switch( *seq ){
    case 0:
      setup_alone_common_ClientID_and_srcParty( sp );
      break;
    // トレーナーデータ設定
    case 1:
      setupCommon_TrainerParam( sp );
      break;
    case 2:
      m_pokeconForClient->Init( this, false );
      this->setSrcPartyToPokecon( m_pokeconForClient, 0, false );
      break;
    case 3:
      this->setSrcPartyToPokecon( m_pokeconForClient, 1, false );
      this->setSrcPartyToPokecon( m_pokeconForClient, 2, false );
      this->setSrcPartyToPokecon( m_pokeconForClient, 3, false );
      break;
    case 4:
      m_pokeconForServer->Init( this, true );
      this->setSrcPartyToPokecon( m_pokeconForServer, 0, true );
      break;
    case 5:
      this->setSrcPartyToPokecon( m_pokeconForServer, 1, true );
      this->setSrcPartyToPokecon( m_pokeconForServer, 2, true );
      this->setSrcPartyToPokecon( m_pokeconForServer, 3, true );
      break;
    case 6:
      setupCommon_CreateServerClient( m_heapSys, *m_setupParam );
      setupCommon_ClientPublicInformation( *m_setupParam );
      break;
    case 7:
      // 録画再生モード設定
      setupCommon_SetRecplayerClientMode( sp );
      break;
    case 8:
      // 描画エンジン生成＆UIクライアントに関連付ける
      setupCommon_CreateViewModule( sp, bagMode );
      ++(*seq);
      // fall throuh
    case 9:
      if( setupCommon_InitViewModule() )
      {
        break;
      }
      return false;
    case 10:
      // Server 始動
      m_server->Startup();
      setMainLoop( &MainModule::MainLoop_StandAlone );
      break;
    default:
      return true;
  }

  ++(*seq);

  return false;
}

/**
 * @brief サーバー・クライアントを生成する
 * @param heap           使用するヒープ
 * @param setupParam     セットアップパラメータ
 */
void MainModule::setupCommon_CreateServerClient( HeapHandle heap, const BATTLE_SETUP_PARAM& setupParam )
{
  ServerClientFactory::Input input;
  ServerClientFactory::Output output;
   
  input.heap                 = heap;
  input.rule                 = setupParam.rule;
  input.multiMode            = setupParam.multiMode;
  input.commMode             = setupParam.commMode;
  input.bagMode              = this->checkBagMode( &setupParam );
  input.isRecordPlayMode     = this->IsRecordPlayMode();
  input.myClientId           = static_cast<BTL_CLIENT_ID>( m_myClientID );
  input.amIServer            = this->CheckImServerMachine();
  input.haveZenryokuRing     = this->GetClientPlayerData( m_myClientID )->HasZenryokuRing();
  input.mainModule           = this;
  input.randomSeed           = m_randomSeed;
  input.soundSystem          = &m_soundSys;
  input.pokeconForClient     = m_pokeconForClient;
  input.pokeconForServer     = m_pokeconForServer;
  input.sendDataContainerForServerSend = SendDataContainerFactory::GetServerSendContainer();
  input.clientLimitTime      = this->GetClientLimitTime();
  for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
  {
    BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );
    input.sendDataContainerForClientSend[ clientId ]    = SendDataContainerFactory::GetClientSendContainer( clientId );
    input.sendDataContainerForClientReceive[ clientId ] = SendDataContainerFactory::GetClientReceiveContainer( clientId );
  }

  ServerClientFactory::CreateServerClient( input, &output );

  for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId ) 
  {
    m_client[ clientId ]                   = output.localClients[ clientId ];
    m_dummyClient[ clientId ]              = output.dummyClients[ clientId ];
    m_clientZenryokuWazaStatus[ clientId ] = output.clientZenryokuWazaStatus[ clientId ];
  }

  if( CheckImServerMachine() ) 
  {
    m_server = output.server;
  }
  else 
  {
    m_cmdCheckServer = output.server;
  }
}


/**
 *  セットアップパラメータからバッグモードを決定
 */
BtlBagMode MainModule::checkBagMode( const BATTLE_SETUP_PARAM* sp ) const
{
  switch( sp->competitor ){
  case BTL_COMPETITOR_WILD:
    return BBAG_MODE_NORMAL;

  case BTL_COMPETITOR_TRAINER:
    return BBAG_MODE_NORMAL;

  case BTL_COMPETITOR_INST:
    return BBAG_MODE_NORMAL;

  case BTL_COMPETITOR_COMM:
//    return BBAG_MODE_SHOOTER; // xy ではシューター無くなる 2011.09.28
    return BBAG_MODE_NORMAL;
  }

  return BBAG_MODE_NORMAL;
}

/**
 *  スタンドアロンモードでプレイヤーのクライアントID、基本立ち位置
 *  各クライアントのパーティデータを生成（録画再生に対応）
 */
void MainModule::setup_alone_common_ClientID_and_srcParty( const BATTLE_SETUP_PARAM* sp )
{
  m_myClientID = sp->commPos;;
  m_myOrgPos = GetClientPokePos( m_myClientID, 0 );

  setupCommon_srcParty( sp );

  BTL_N_Printf( DBGSTR_DEBUGFLAG_BIT, sp->DebugFlagBit );
}



/**
 *  スタンドアロンモードでパーティデータ生成（録画再生対応）
 */
void MainModule::setupCommon_srcParty( const BATTLE_SETUP_PARAM* sp )
{
  u8 relation_0 = ClientIDtoRelation( m_myClientID, 0 );
  u8 relation_1 = ClientIDtoRelation( m_myClientID, 1 );

  // バトル用ポケモンパラメータ＆パーティデータを生成
  srcParty_Set( 0, sp->party[ relation_0 ] );
  srcParty_Set( 1, sp->party[ relation_1 ] );

  {
    u8 relation_2 = ClientIDtoRelation( m_myClientID, 2 );
    u8 relation_3 = ClientIDtoRelation( m_myClientID, 3 );
    if( sp->party[ relation_2 ] != NULL ){
      srcParty_Set( 2, sp->party[relation_2] );
    }
    if( sp->party[ relation_3 ] != NULL ){
      srcParty_Set( 3, sp->party[relation_3] );
    }
  }
}


/**
 * @brief POKECON にパーティデータを追加する
 * @param[out] pokecon    追加先の POKECON
 * @param      clientID   追加パーティのクライアントID
 * @param      forServer  追加先がサーバー用POKECONか？
 */
void MainModule::setSrcPartyToPokecon( POKECON* pokecon, u8 clientID, bool forServer )
{
  PokeParty*                srcParty   = this->GetSrcParty( clientID, forServer );
  const PartyParam*         partyParam = this->GetPartySetupParam( clientID );
  const Savedata::MyStatus* playerData = NULL;

  if( this->IsClientNPC( clientID ) == false )
  {
    playerData = this->GetClientPlayerData( clientID );
  }

  pokecon->SetParty( clientID, srcParty, *partyParam, playerData );
}


//----------------------------------------------------------------------------------
/**
 * 通信相手のクライアントIDと自分のクライアントIDを比較し、関係性を返す
 *
 * @param   myClientID
 * @param   targetClientID
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
u8  MainModule::ClientIDtoRelation( u8 myClientID, u8 targetClientID ) const
{
  if( myClientID == targetClientID )
  {
    return BTL_CLIENT_PLAYER;
  }

  const BtlRule rule = this->GetRule();
  if( rule == BTL_RULE_ROYAL )
  {
    return ( targetClientID + BTL_CLIENT_ROYAL_NUM - myClientID ) % BTL_CLIENT_ROYAL_NUM;
  }

  BtlSide mySide     = PokemonPosition::GetClientSide( rule, static_cast<BTL_CLIENT_ID>( myClientID ) );
  BtlSide targetSide = PokemonPosition::GetClientSide( rule, static_cast<BTL_CLIENT_ID>( targetClientID ) );

  if( mySide == targetSide )
  {
    return BTL_CLIENT_PARTNER;
  }
  else
  {
    return (targetClientID <= 1)? BTL_CLIENT_ENEMY1 : BTL_CLIENT_ENEMY2;
  }
}


/**
 *  クライアントが管理する位置数を取得 
 */
u8 MainModule::checkNumFrontPos( u8 clientID ) const
{
  switch( this->GetRule() )
  {
  case BTL_RULE_SINGLE:
    return 1;

  case BTL_RULE_DOUBLE:
    if( m_setupParam->multiMode == false ){
      return 2;
    }else{
      if( m_setupParam->competitor == BTL_COMPETITOR_COMM ){
        return 1;
      }
      else
      {
        // マルチで非通信時、相方のパーティデータがあるなら自分の管理領域は１
        u8 friendClientID = getFriendCrientID( clientID );
        if(  (m_setupParam->party[friendClientID] != NULL)
        &&   (m_setupParam->party[friendClientID]->GetMemberCount() > 0)
        ){
          return 1;
        }
        return 2;
      }
    }

  case BTL_RULE_ROYAL:
    return 1;

  case BTL_RULE_INTRUDE:
    return ( clientID == BTL_CLIENT_PLAYER ) ? ( 1 ) : ( 2 );
  }
  
  GFL_ASSERT(0);
  return 1;
}


//----------------------------------------------------------------------------------
/**
 * 上記セットアップ処理で生成したモジュールを全て解放する共通クリーンアップ処理
 *
 * @param   seq
 * @param   work
 *
 * @retval  bool    trueで終了
 */
//----------------------------------------------------------------------------------
bool MainModule::cleanup_common( int* seq )
{
  GFL_UNUSED( seq );

  BTL_Printf("クリーンアップ 1-1\n");

  if( m_viewCore ){
    const bool ret = m_viewCore->Finalize();
    if( ret == false )
    {
      return false;
    }
    GFL_DELETE( m_viewCore );
    m_viewCore = NULL;
  }

  BTL_Printf("クリーンアップ 1-2\n");

  for(u32 i=0; i<GFL_NELEMS(m_client); ++i)
  {
    GFL_SAFE_DELETE( m_client[i] );
  }
  for(u32 i=0; i<GFL_NELEMS(m_dummyClient); ++i)
  {
    GFL_SAFE_DELETE( m_dummyClient[i] );
  }
  for(u32 i=0; i<GFL_NELEMS(m_clientZenryokuWazaStatus); ++i)
  {
    GFL_SAFE_DELETE( m_clientZenryokuWazaStatus[i] );
  }

  BTL_Printf("クリーンアップ 1-3\n");

  if( m_server ){
    GFL_DELETE  m_server;
    m_server = NULL;
  }
  if( m_cmdCheckServer ){
    GFL_DELETE  m_cmdCheckServer;
    m_cmdCheckServer = NULL;
  }

  GFL_SAFE_DELETE( m_zukanDataOnBattleStart );


  BTL_Printf("クリーンアップ 1-4\n");

  return true;
}

//--------------------------------------------------------------------------
/**
 * 通信／シングルバトル
 */
//--------------------------------------------------------------------------
bool MainModule::setup_comm_single( int* seq )
{
  static const pSubProc funcs[] = {
    NULL,
    &MainModule::setupseq_comm_determine_server,
    &MainModule::setupseq_comm_notify_party_data,
    &MainModule::setupseq_comm_notify_player_data,
    &MainModule::setupseq_comm_create_server_client,
    &MainModule::setupseq_comm_start_server,
  };

  if( (*seq) == 0 )
  {
    m_myClientID = m_setupParam->commPos;
    m_myOrgPos = GetClientPokePos( m_myClientID, 0 );
    m_setupStep = 0;
    (*seq)++;
    return false;
  }
  else if( (*seq) < GFL_NELEMS(funcs) )
  {
    int prevStep = m_setupStep;
    if( (this->*funcs[ *seq ])( &m_setupStep ) )
    {
      m_setupStep = 0;
      ++(*seq);
      //BTL_PRINT("SetupCommSingle シーケンス[%d]へ\n", *seq);
    }
    else if( prevStep != m_setupStep ){
      //BTL_PRINT("　　SetupCommSingle Seq=%d, step = %d\n", (*seq), m_setupStep );
    }
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * 通信／ダブルバトル：セットアップ
 */
//--------------------------------------------------------------------------
bool MainModule::setup_comm_double( int* seq )
{
  static const pSubProc funcs[] = {
    NULL,
    &MainModule::setupseq_comm_determine_server,
    &MainModule::setupseq_comm_notify_party_data,
    &MainModule::setupseq_comm_notify_player_data,
    &MainModule::setupseq_comm_create_server_client,
    &MainModule::setupseq_comm_start_server,
  };

  if( (*seq) == 0 )
  {
    const BATTLE_SETUP_PARAM* sp = m_setupParam;

    switch( sp->multiMode ){
    case BTL_MULTIMODE_NONE:
    default:
      break;
    case BTL_MULTIMODE_PP_AA:
      m_MultiAIClientNum = 2;
      /* fallthru */
    case BTL_MULTIMODE_PP_PP:
      break;
    }
    m_myClientID = m_setupParam->commPos;
    BTL_PRINT("[btl_main] 自分のClientID=%d\n", m_myClientID);
    m_myOrgPos = GetClientPokePos( m_myClientID, 0 );
    m_setupStep = 0;
    (*seq)++;
    return false;
  }
  else if( (*seq) < GFL_NELEMS(funcs) ){
    if( (this->*funcs[ *seq ])( &m_setupStep ) ){
      m_setupStep = 0;
      ++(*seq);
    }
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * 通信／バトルロイヤル
 */
//--------------------------------------------------------------------------
bool MainModule::setup_comm_royal( int* seq )
{
  static const pSubProc funcs[] = {
    NULL,
    &MainModule::setupseq_comm_determine_server,
    &MainModule::setupseq_comm_notify_party_data,
    &MainModule::setupseq_comm_notify_player_data,
    &MainModule::setupseq_comm_create_server_client,
    &MainModule::setupseq_comm_start_server,
  };

  if( (*seq) == 0 )
  {
    m_myClientID = m_setupParam->commPos;
    m_myOrgPos = GetClientPokePos( m_myClientID, 0 );
    m_setupStep = 0;
    (*seq)++;
    return false;
  }
  else if( (*seq) < GFL_NELEMS(funcs) )
  {
    if( (this->*funcs[ *seq ])( &m_setupStep ) )
    {
      m_setupStep = 0;
      ++(*seq);
    }
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------
/**
 * 通信対戦セットアップシーケンス：サーバ＆クライアントIDの確定
 *
 * @param   seq
 *
 * @retval  bool    終了時にtrueを返す
 */
//----------------------------------------------------------------------------------
bool MainModule::setupseq_comm_determine_server( int* seq )
{
  GFL_UNUSED( seq );

  if( net::CheckError() ){
    m_fCommError = true;
    return true;
  }

  switch( *seq ){
  case 0:
    if( net::DetermineServer(m_myClientID) ){
      (*seq)++;
    }
    break;
  case 1:
    if( net::IsServerDetermained() )
    {
      m_setupParam->commServerVer = net::GetServerVersion();

      if( net::CheckImServer() )
      {
        BTL_UTIL_SetPrintType( BTL_PRINTTYPE_SERVER );
        (*seq) = 2;
      }
      else
      {
        BTL_UTIL_SetPrintType( BTL_PRINTTYPE_CLIENT );
        (*seq) = 3;
      }
    }
    break;

  case 2:
    // サーバマシンは各クライアントにサーバパラメータを通知する
    m_serverNotifyParam.randomSeed       = m_randomSeed;
    m_serverNotifyParam.debugFlagBit     = m_setupParam->DebugFlagBit;
    m_serverNotifyParam.LimitTimeGame    = m_setupParam->LimitTimeGame;
    m_serverNotifyParam.LimitTimeClient  = m_setupParam->LimitTimeClient;
    m_serverNotifyParam.LimitTimeCommand = m_setupParam->LimitTimeCommand;

    // Config Share モードならサーバマシンの設定を引き継ぎ
    if( GetSetupStatusFlag(BTL_STATUS_FLAG_CONFIG_SHARE) )
    {
      m_serverNotifyParam.msgSpeed = m_msgSpeed;
      m_serverNotifyParam.fWazaEffectEnable = m_fWazaEffectEnable;
    }
    else{
      m_serverNotifyParam.msgSpeed = MSGSPEED_FAST;
      m_serverNotifyParam.fWazaEffectEnable = true;
    }

    if( net::NotifyServerParam(&m_serverNotifyParam) ){
      ++(*seq);
    }
    break;

  case 3:
    // デバッグパラメータ受信完了
    if( net::IsServerParamReceived(&m_serverNotifyParam) )
    {
      BATTLE_SETUP_PARAM* sp = m_setupParam;

      sp->DebugFlagBit    = m_serverNotifyParam.debugFlagBit;
      sp->recRandSeed     = m_serverNotifyParam.randomSeed;

      m_randomSeed        = m_serverNotifyParam.randomSeed;
      m_fWazaEffectEnable = m_serverNotifyParam.fWazaEffectEnable;
      m_msgSpeed          = (Savedata::ConfigSave::MSG_SPEED)(m_serverNotifyParam.msgSpeed);
      m_LimitTimeGame     = m_serverNotifyParam.LimitTimeGame;
      m_LimitTimeClient   = m_serverNotifyParam.LimitTimeClient;
      m_LimitTimeCommand  = m_serverNotifyParam.LimitTimeCommand;

      return true;
    }
    break;
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * 通信対戦セットアップシーケンス：各クライアントのパーティデータを相互に送受信
 *
 * @param   seq
 *
 * @retval  bool    終了時にtrueを返す
 */
//----------------------------------------------------------------------------------
bool MainModule::setupseq_comm_notify_party_data( int* seq )
{
  const BATTLE_SETUP_PARAM* sp = m_setupParam;

  if( net::CheckError() ){
    m_fCommError = true;
    return true;
  }

  switch( *seq ){
  case 0:
    BTL_N_Printf( DBGSTR_line, __LINE__ );
    net::TimingSyncStart( net::TIMING_NOTIFY_PARTY_DATA );
    (*seq)++;
    break;
  case 1:
    if( net::IsTimingSync(net::TIMING_NOTIFY_PARTY_DATA) ){
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      (*seq)++;
    }
    break;
  case 2:
    {
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      u32 partyIndex = sp->commPos;
#if PM_DEBUG
      if( sp->party[ partyIndex ]->GetMemberCount() == 0 )
      {//デバッグ通信バトルの場合はこっちになる
        partyIndex = BTL_CLIENT_PLAYER;
      }
#endif
      if( net::StartNotifyPartyData(sp->party[ partyIndex ]) ){
        GFL_PRINT("sp->party[%d]monsno[%d]\n",partyIndex,sp->party[ partyIndex ]->GetMemberPointer(0)->GetMonsNo());
        BTL_N_Printf( DBGSTR_line, __LINE__ );
        (*seq)++;
      }
    }
    break;
  case 3:
    // パーティデータ相互受信を完了
    if( net::IsCompleteNotifyPartyData() )
    {
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      BTL_N_Printf( DBGSTR_MAIN_PartyDataNotifyComplete );

      if( m_MultiAIClientNum == 0 ){
        (*seq) = 10;
      }
      else
      {
        BTL_N_Printf ( DBGSTR_MAIN_SendAIPartyStart, m_MultiAIClientNum );

        if( CheckImServerMachine() )
        {
          m_AIDataContainer = net::AIDC_Create( PokeParty::GetSerializeShortStructSize(), m_heapSys );
        }
        m_MultiAIDataSeq = 0;
        (*seq)++;
      }
    }
    break;
  case 4:
    m_MultiAIClientID = (m_MultiAIDataSeq == 0) ? BTL_CLIENT_ENEMY1 : BTL_CLIENT_ENEMY2;
    if( CheckImServerMachine() )
    {
      net::AIDC_SetClientID( m_AIDataContainer, m_MultiAIClientID );
      sp->party[m_MultiAIClientID]->SerializeToShortStruct( net::AIDC_GetBuffer(m_AIDataContainer) );
      {
        sp->party[m_MultiAIClientID]->DeserializeFromShortStruct( net::AIDC_GetBuffer(m_AIDataContainer) );

        #if PM_DEBUG
        {
          pml::pokepara::PokemonParam* pp = sp->party[m_MultiAIClientID]->GetMemberPointer( 0 );
          pp  = sp->party[m_MultiAIClientID]->GetMemberPointer( 0 );
          BTL_PRINT("  Deserialize .. 1stMonsNo=%d\n", pp->GetMonsNo());
        }
        #endif

//        net::DebugPartySerialDump( "Send Serial", (u8*)(net::AIDC_GetBuffer(m_AIDataContainer)) );
      }
    }
    BTL_N_Printf( DBGSTR_line, __LINE__ );
    net::TimingSyncStart( net::TimingID_Add(net::TIMING_NOTIFY_AI_PARTY_1, m_MultiAIDataSeq) );
    (*seq)++;
    break;
  case 5:
    if( net::IsTimingSync( net::TimingID_Add(net::TIMING_NOTIFY_AI_PARTY_1, m_MultiAIDataSeq)) )
    {
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      (*seq)++;
    }
    break;
  case 6:
    if( CheckImServerMachine() )
    {
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      if( !net::StartNotify_AI_PartyData(m_AIDataContainer) ){
        break;
      }
    }
    (*seq)++;
    break;

  case 7:
    if( net::IsRecved_AI_PartyData(m_MultiAIClientID) )
    {
      BTL_PRINT("AIパーティ(clientID=%d)を受け取りました\n", m_MultiAIClientID);
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      m_MultiAIDataSeq++;
      if( m_MultiAIDataSeq >= m_MultiAIClientNum )
      {
        BTL_PRINT("AIパーティ,もう全部受け取った\n");
        BTL_N_Printf( DBGSTR_MAIN_AIPartyDataSendComplete );
        if( CheckImServerMachine() ){
          net::AIDC_Delete( m_AIDataContainer );
          m_AIDataContainer = NULL;
        }
        (*seq) = 8;
      }
      else{
        (*seq) = 4;
      }
    }
    break;

  case 8:
    net::TimingSyncStart( net::TimingID_Add(net::TIMING_NOTIFY_AI_PARTY_3, m_MultiAIDataSeq) );
    (*seq)++;
    break;
  case 9:
    if( net::IsTimingSync( net::TimingID_Add(net::TIMING_NOTIFY_AI_PARTY_3, m_MultiAIDataSeq)) )
    {
      (*seq)++;
    }
    break;


  case 10:
    BTL_N_Printf( DBGSTR_line, __LINE__ );
    m_pokeconForClient->Init( this, false );
    for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
    {
      if( IsExistClient(i) )
      {
        GFL_PRINT(" Client_%d が存在しているためその分のパーティデータを受け取ります\n", i );
        if( i==1 || i==3 ){
          net::DebugPartySerialDump( "RecvedSerial", (u8*)(net::GetPartySerializedData(i,sp)) );
        }
        srcParty_SetSerial( i, net::GetPartySerializedData(i,sp) );
        Bspstore_Party( i, net::GetPartySerializedData(i,sp) );
        this->setSrcPartyToPokecon( m_pokeconForClient, i, false );

        
      }
    }
    (*seq)++;
    break;
  case 11:
    m_pokeconForServer->Init( this, true );
    for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
    {
      if( IsExistClient(i) ){
        this->setSrcPartyToPokecon( m_pokeconForServer, i, true );
      }
    }
    (*seq)++;
    break;
  case 12:
    net::EndNotifyPartyData();
    (*seq)++;
    break;
  default:
    return true;
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * 通信対戦セットアップシーケンス：各クライアントのプレイヤーデータを相互に送受信
 *
 * @param   wk
 * @param   seq
 *
 * @retval  bool    終了時にtrueを返す
 */
//----------------------------------------------------------------------------------
bool MainModule::setupseq_comm_notify_player_data( int* seq )
{
  const BATTLE_SETUP_PARAM* sp = m_setupParam;

  if( net::CheckError() ){
    m_fCommError = true;
    return true;
  }

  switch( *seq ){
  case 0:
    net::TimingSyncStart( net::TIMING_NOTIFY_PLAYER_DATA );
    BTL_N_Printf( DBGSTR_line, __LINE__ );
    (*seq)++;
    break;
  case 1:
    if( net::IsTimingSync(net::TIMING_NOTIFY_PLAYER_DATA) ){
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      (*seq)++;
    }
    break;
  case 2:
    if( net::StartNotifyPlayerData(m_playerStatus) ){
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      (*seq)++;
    }
    break;
  case 3:
    if( net::IsCompleteNotifyPlayerData() )
    {
      BTL_N_Printf( DBGSTR_line, __LINE__ );
      const BtlRule rule = this->GetRule();
      const u32 clientNum = BattleRule::GetClientNum( rule, m_setupParam->multiMode );
      for(u32 i=0; i<clientNum; ++i)
      {
        if( BattleRule::IsClientAi( rule, m_setupParam->commMode, m_setupParam->multiMode, static_cast<BTL_CLIENT_ID>(i) ) ) {
//          BTL_PRINT("Client_%d は AI だから飛ばす..\n");
          continue;
        }
        const void* playerSerialData = net::GetPlayerSerialData( i );
        if( playerSerialData )
        {
          trainerParam_StoreSelialData( &m_trainerParam[i], m_heapSys, playerSerialData );

          #if BTL_OLD_IGNORE
          //Wifiの対戦相手が不正名だったら名前を置き換え
          //BTS:5210対処　tomoya
          if( ( i != m_myClientID ) && ( sp->WifiBadNameFlag ) )
          {
            STRCODE name[ BUFLEN_PERSON_NAME ];
            DWC_TOOL_SetBadNickName( name, BUFLEN_PERSON_NAME, m_heapID );
            MyStatus_SetMyName( m_trainerParam[ i ].playerStatus, name );
            GFL_STR_DeleteBuffer( m_trainerParam[ i ].name );
            m_trainerParam[ i ].name = DWC_TOOL_CreateBadNickName( m_heapID );
          }
          #endif

          Bspstore_PlayerStatus( i, m_trainerParam[ i ].playerStatus ); ///<BadName書き換え後のMyStatusを渡すように変更 tomoya
        }
      }
//      BTL_PRINT("プレイヤーデータ相互受信できました。\n");
      (*seq)++;
    }
    break;
  case 4:
    if( m_MultiAIClientNum == 0 ){
      (*seq) = 10;
    }
    else{
      m_MultiAIDataSeq = 0;
      (*seq)++;
    }
    break;
  // マルチ対戦でAIトレーナーデータの送信（Server->Client）が必要
  case 5:
    net::TimingSyncStart( net::TimingID_Add(net::TIMING_NOTIFY_AI_TRAINER_1, m_MultiAIDataSeq) );
    (*seq)++;
    break;
  case 6:
    if( net::IsTimingSync(net::TimingID_Add(net::TIMING_NOTIFY_AI_TRAINER_1, m_MultiAIDataSeq)) ){
      (*seq)++;
    }
    break;
  case 7:
    m_MultiAIClientID = (m_MultiAIDataSeq == 0) ? BTL_CLIENT_ENEMY1 : BTL_CLIENT_ENEMY2;
    if( CheckImServerMachine() )
    {
      if( !net::StartNotify_AI_TrainerData(sp->tr_data[m_MultiAIClientID], m_MultiAIClientID) ){
        break;
      }
      BTL_N_Printf( DBGSTR_MAIN_MultiAITrainer_SendDone, m_MultiAIClientID );
    }
    (*seq)++;
    /* fallthru */
  case 8:
    if( net::IsRecved_AI_TrainerData(m_MultiAIClientID) )
    {
      const net::TRAINER_SEND_DATA*  trSendData = net::Get_AI_TrainerData( m_MultiAIClientID );
      trainerParam_StoreCommNPCTrainer( &m_trainerParam[m_MultiAIClientID], trSendData );

      m_MultiAIDataSeq++;
      if( m_MultiAIDataSeq >= m_MultiAIClientNum ){
        (*seq) = 10;
      }else{
        (*seq) = 5;
      }
    }
    break;

  case 10:
    net::EndNotifyPlayerData();
    (*seq)++;
    break;
  default:
    return true;
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * 通信対戦セットアップシーケンス：サーバ・クライアントモジュール構築
 *
 * @param   wk
 * @param   seq
 *
 * @retval  bool    終了時にtrueを返す
 */
//----------------------------------------------------------------------------------
bool MainModule::setupseq_comm_create_server_client( int* seq )
{
  GFL_UNUSED( seq );

  // 通信エラーが起きていたら終了
  if( net::CheckError() ){
    m_fCommError = true;
    return true;
  }

  setupCommon_CreateServerClient( m_heapSys, *m_setupParam );
  setupCommon_ClientPublicInformation( *m_setupParam );
  return true;
}

//----------------------------------------------------------------------------------
/**
 * 通信対戦セットアップシーケンス：描画エンジン生成、メインループ始動
 *
 * @param   wk
 * @param   seq
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool MainModule::setupseq_comm_start_server( int* seq )
{
  GFL_UNUSED( seq );

  // 通信エラーが起きていたら終了
  if( net::CheckError() ){
    //0の場合でエラーが起きている場合はすでにエラーになっているので即抜け。
    //1の場合のみViewSystem生成中なので待つ。
    if( *seq != 1 )
    {
      m_fCommError = true;
      return true;
    }
  }

  switch( *seq ){
  case 0:
    {
      const BATTLE_SETUP_PARAM* sp = m_setupParam;
      BtlBagMode bagMode = checkBagMode( sp );

      // 描画エンジン生成
      setupCommon_CreateViewModule( sp, bagMode );
    }
    (*seq)++;
    // fall throuh

  case 1:
    if( setupCommon_InitViewModule() )
    {
      (*seq)++;
    }
    break;

  case 2:
    {
      if( CheckImServerMachine() )
      {
        // Server 始動
        setMainLoop( &MainModule::MainLoop_Comm_Server );
        m_server->Startup();
      }
      else
      {
        setMainLoop( &MainModule::MainLoop_Comm_NotServer );
      }
      (*seq)++;
    }
    break;
  default:
    return true;
  }
  return false;
}

//======================================================================================================
//======================================================================================================

void MainModule::viewUpdate( void )
{
  m_viewCore->Update();
}

bool MainModule::MainLoop_StandAlone( void )
{
  bool quitFlag = false;
  m_server->MainLoop();

  for(u32 i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( m_client[i] != NULL )
    {
      if( m_client[i]->Main() )
      {
        m_client[ i ]->GetEscapeInfo( &m_escapeInfo );
        quitFlag = true;
      }
    }
  }

  viewUpdate();

  return quitFlag;
}


/**
 * @brief 自身がサーバーである場合のメインループ
 * @retval true   バトルが終了
 * @retval false  バトルが継続
 */
bool MainModule::MainLoop_Comm_Server( void )
{
  if( m_fCommError == false )
  {
    if( net::CheckError() )
    {
      m_fCommError = true;
      NotifyCommErrorToLocalClient();
    }
  }
  else if( m_fCommErrorMainQuit )
  {
    return true;
  }

  // リモートクライアントとの通信接続を監視
  watchRemoteClientCommunication();

  {
    bool quitFlag = false;
    int i;

    quitFlag = m_server->MainLoop();
    if( quitFlag ){
      m_server->GetEscapeInfo( &m_escapeInfo );
    }

    for(i=0; i<BTL_CLIENT_MAX; i++)
    {
      if( m_client[i] && (m_fClientQuit[i] == false) )
      {
        m_fClientQuit[i] = m_client[i]->Main();
      }
    }

    if( CheckAllClientQuit() ){
      m_client[m_myClientID]->GetEscapeInfo( &m_escapeInfo );
      quitFlag = true;
    }

    viewUpdate();

    return quitFlag;
  }
}

/**
 * @brief リモートクライアントとの通信接続を監視し、必要ならダミークライアントを起動する
 */
void MainModule::watchRemoteClientCommunication( void )
{
  // 自身がサーバーでないなら不正呼び出し
  if( m_server == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  if( !canLaunchDammyClient() )
  {
    return;
  }

  const BtlRule rule = this->GetRule();

  for( u32 i=0; i<BTL_CLIENT_MAX; ++i )
  {
    const BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );

    if( ( m_client[ clientId ] == NULL ) &&
        ( BattleRule::IsClientExist( rule, m_setupParam->multiMode, clientId ) ) &&
        !( m_clientCommunicationObserver->IsClientCommunicationExist( clientId ) ) )
    {
      launchDammyClient( clientId );
    }
  }
}

/**
 * @brief ダミークライアントを起動することが出来るか？
 * @retval true   ダミークライアントを起動できる
 * @retval false  ダミークライアントを起動できない
 */
bool MainModule::canLaunchDammyClient( void ) const
{
  // 自身がサーバーでないなら不正呼び出し
  if( m_server == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }

  // ルール上、禁止されている
  BtlRule rule = this->GetRule();
  if( !BattleRule::IsDummyClientSwitchEnable( rule ) )
  {
    return false;
  }

  // サーバーが、クライアントからの返信待ちでないなら、禁止する
  // 
  // ※サーバー、クライアント間の通信処理は、
  // 　サーバーがクライアントにコマンドを送信し、クライアントがサーバーに返信する、という、常に一方通行のやり取りで行われています。
  // 　ダミークライアントが起動すると、ダミークライアントは、サーバーが最後に送信したコマンドに対する返信を行います。
  // 　このとき、サーバー、クライアントの両方が送信を行わないようにするため、
  // 　サーバーがクライアントからの返信待ち状態でない間は、ダミークライアントの起動を禁止します。
  if( !m_server->IsWaitingClientReply() )
  {
    return false;
  }

  return true;
}

/**
 * @brief 指定したクライアントをダミークライアントに変更する
 * @param clientId  変更対象クライアントのID
 */
void MainModule::launchDammyClient( BTL_CLIENT_ID clientId )
{
  // 既にローカルクライアントが存在する
  if( m_client[ clientId ] != NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  m_client[ clientId ] = m_dummyClient[ clientId ];
  m_client[ clientId ]->GetAdapter()->ChangeCommType( false );
  m_dummyClient[ clientId ] = NULL;
  BTL_PRINT( "[MainModule] クライアント %d との通信接続が途切れました。ダミークライアントに差し替えます\n", clientId );

  // @fix[2759] バトルロイヤルで子機が切断するとゲームが進行しなくなる
  if( m_fCommError )
  {
    m_client[ clientId ]->NotifyCommError();
  }
}




/**
 * @brief 自身がサーバーでない場合のメインループ
 * @retval true   バトルが終了
 * @retval false  バトルが継続
 */
bool MainModule::MainLoop_Comm_NotServer( void )
{
  if( m_fCommError == false )
  {
    if( net::CheckError() )
    {
      BTL_N_Printf( DBGSTR_MAIN_CommError, __LINE__ );
      m_fCommError = true;
      NotifyCommErrorToLocalClient();
    }
  }
  else if( m_fCommErrorMainQuit )
  {
    return true;
  }

  {
    bool quitFlag = false;
    int i;

    for(i=0; i<BTL_CLIENT_MAX; i++)
    {
      if( m_client[i] )
      {
        if( m_client[i]->Main() )
        {
          m_client[i]->GetEscapeInfo( &m_escapeInfo );
          quitFlag = true;
        }
      }
    }

    // リモートサーバーとの通信接続を監視
    watchRemoteServerLoss();

    viewUpdate();

    return quitFlag;
  }
}

/**
 * @brief リモートサーバーとの通信接続を監視し、
 *        リモートサーバーがいなくなった際に、必要なら自身をサーバーに変更する
 */
void MainModule::watchRemoteServerLoss( void )
{
  // 自身がサーバーなら、不正呼び出し
  if( m_server != NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  // リモートサーバーが存在する or 自身が新サーバーとして選出されていないなら、何もしない
  if( !( this->CheckImServerMachine() ) )
  {
    return;
  }

  // 全てのローカルクライアントがサーバーからの受信待ち状態になるまで待つ
  for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    if( ( m_client[ clientId ] != NULL ) &&
       !( m_client[ clientId ]->IsWaitingAdapterCommand() ) ) {
       return;
    }
  }

  changeServerSelf();
}

/**
 * @brief 自身をサーバーに変更する
 */
void MainModule::changeServerSelf( void )
{
  // 以降、コマンド整合性チェックサーバーのインスタンスを、正式なサーバーとして使用する
  m_server         = m_cmdCheckServer;
  m_cmdCheckServer = NULL;

  // サーバーの送信履歴を、自身が受信したデータから復元
  {
    const SendDataContainer* myReceivedContainer = SendDataContainerFactory::GetClientReceiveContainer( static_cast<BTL_CLIENT_ID>( m_myClientID ) );
    SendDataContainer* serverSendContainer = SendDataContainerFactory::GetServerSendContainer();
    serverSendContainer->CopyFrom( *myReceivedContainer ); 
  }

  // サーバーを、新サーバーとしてセットアップ
  m_server->StartupAsNewServer();

  // 自身のクライアントから、コマンド整合性チェックサーバーをデタッチ
  m_client[ m_myClientID ]->DetachCmdCheckServer();

  // サーバー用のメインループに切り替える
  setMainLoop( &MainModule::MainLoop_Comm_Server );

  BTL_PRINT( "[MainModule] 自身をサーバーに変更しました\n" );

#if PM_DEBUG
#if defined( GF_PLATFORM_CTR )
  if( this->CheckImServerMachine() && ( this->GetCommMode() != BTL_COMM_NONE ) )
  {
    gfl2::debug::LogWindow_AddMsg( gfl2::debug::LOGWIN_ID_NEX, L"I'm battle server" );
    GFL_RELEASE_PRINT("I'm battle server\n");
  }
#endif
#endif
}





bool MainModule::MainLoop_Comm_Error( void )
{
  return true;
}


/**
 *  同一マシンに存在する全クライアントに通信エラー通知
 */
void MainModule::NotifyCommErrorToLocalClient( void )
{
  for(u32 i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( m_client[i] )
    {
      m_client[i]->NotifyCommError();
    }
  }
}
/**
 *  同一マシンに存在する全クライアントの終了フラグチェック
 */
bool  MainModule::CheckAllClientQuit( void )
{
  for(u32 i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( m_client[i] )
    {
      if( m_fClientQuit[i] == false ){
        return false;
      }
    }
  }
  return true;
}

//======================================================================================================
// Sound
//======================================================================================================
void MainModule::StartBGM( u32 bgmNo )
{
  m_soundSys.StartBGM( bgmNo );
}
void MainModule::StartWinBGM( void )
{
  u32 bgmNo = this->GetWinBGMNo();
  m_soundSys.SwitchBGM( bgmNo, Sound::BGM_FADE_VERY_FAST );
}

//======================================================================================================
// クライアント等からの問い合わせ対応
//======================================================================================================

//=============================================================================================
/**
 * 戦闘のルール指定子を返す
 *
 * @param   wk
 *
 * @retval  BtlRule
 */
//=============================================================================================
BtlRule MainModule::GetRule( void ) const
{
  return m_rule;
}
//=============================================================================================
/**
 * ワザエフェクト表示が有効かチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsWazaEffectEnable( void ) const
{
  return m_fWazaEffectEnable;
}
//=============================================================================================
/**
 * @brief 命令無視しないで言うことを聞くポケモンの最大レベルを取得する
 *
 * @retval [0, 100]
 */
//=============================================================================================
u8 MainModule::GetMaxFollowPokeLevel( void ) const
{
  return m_setupParam->maxFollowPokeLevel;
}

//=============================================================================================
/**
 * 戦闘ルール「いれかえ」を採用するか
 *
 * @param   wk
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsIrekaeMode( void ) const
{
  if( (GetCompetitor() == BTL_COMPETITOR_TRAINER)
  &&  (GetRule() == BTL_RULE_SINGLE)
  &&  (GetCommMode() == BTL_COMM_NONE)
  &&  (m_changeMode == Savedata::ConfigSave::BATTLE_RULE_REPLACEMENT)
  ){
    return true;
  }
  return false;
}
#if 0
//=============================================================================================
/**
 * 文字出力のwait値を返す
 *
 * @param   wk
 *
 * @retval  int
 */
//=============================================================================================
int BTL_MAIN_GetPrintWait( const BTL_MAIN_MODULE* wk )
{
  return MSGSPEED_GetWaitByConfigParam( wk->msgSpeed );
}



#endif

//=============================================================================================
/**
 * 対戦相手タイプがシナリオバトル用のものかどうか判定
 * @return シナリオバトルなら true
 */
//=============================================================================================
bool MainModule::IsCompetitorScenarioMode( void ) const
{
  if( (m_setupParam->competitor == BTL_COMPETITOR_WILD)
  ||  (m_setupParam->competitor == BTL_COMPETITOR_TRAINER)
  ){
    return true;
  }
  return false;
}
//=============================================================================================
/**
 * 経験値取得シーケンスが有効な対戦かどうか判定
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsExpSeqEnable( void ) const
{
  // @fix GFNMCat[1839] バトルロイヤルで経験値が入る
  // バトルロイヤルでは経験値が入らないように修正
  if( this->GetRule() == BTL_RULE_ROYAL )
  {
    return false;
  }

  if( this->IsCompetitorScenarioMode() )
  {
    BTL_PRINT("[btl_main] competitor = %d, exp seq enable\n", m_setupParam->competitor );

#ifdef AMPROTECT_FUNC
      //プロテクトを入れたい場合はこのあたりに
#endif

    // 経験値・お金なしバトルが指定されている場合
    if ( m_setupParam->bNoGainBattle ) {
      return false;
    }

    //特殊ポケモン戦は経験値なし
    if( GetSetupStatusFlag(BTL_STATUS_FLAG_LEGEND_EX) ){
      return false;
    }

    //AZ戦（殿堂入り初回直後）は経験値なし
    if( GetSetupStatusFlag(BTL_STATUS_FLAG_ENDING_BATTLE) ){
      return false;
    }

    return true;
  }
    BTL_PRINT("[btl_main] competitor = %d, exp seq disable\n", m_setupParam->competitor );

  return false;
}

//=============================================================================================
/**
 * お金・アイテム取得シーケンスが有効な対戦かどうか判定
 */
//=============================================================================================
bool MainModule::IsMoneySeqEnable( void ) const
{
  if( m_setupParam->bNoGainBattle ) return false;
  return true;
}

//=============================================================================================
/**
 * 現在のルールに応じて、有効になる位置指定子の最大値を返す
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos MainModule::GetValidPosMax( void ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetValidPosMax( rule );
}
//=============================================================================================
/**
 * 現在のルールに応じて、戦闘の前衛（ワザがあたる場所）の数を返す
 *
 * @retval  u32
 */
//=============================================================================================
u32 MainModule::GetFrontPosNum( u8 clientID ) const
{
  BtlSide side = this->GetClientSide( clientID );
  return this->GetSidePosNum( side );
}
//=============================================================================================
/**
 * @brief 現在のルールに応じて、戦闘の前衛（ワザがあたる場所）の数を返す
 * @param side  チェック対象の陣営
 */
//=============================================================================================
u32 MainModule::GetSidePosNum( BtlSide side ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetFrontPosNum( rule, side );
}
//=============================================================================================
/**
 * 現在のルールに応じて、相手陣営の、前衛（ワザがあたる場所）の数を返す
 *
 * @param   clientID
 *
 * @retval  u32
 */
//=============================================================================================
u32 MainModule::GetOpponentFrontPosNum( u8 clientID ) const
{
  u8 opponentClientID = this->GetOpponentClientID( clientID, 0 );
  return GetFrontPosNum( opponentClientID );
}

//=============================================================================================
/**
 * @brief 立ち位置が存在するか？
 * @param pos     立ち位置
 * @retval true   指定した立ち位置が存在する
 * @retval false  指定した立ち位置が存在しない
 */
//=============================================================================================
bool MainModule::IsPokePosExist( BtlPokePos pos ) const
{
  BtlRule rule = this->GetRule();
  BtlMultiMode multiMode = m_setupParam->multiMode;
  return PokemonPosition::IsPokePosExist( rule, multiMode, pos );
}

//=============================================================================================
/**
 * 指定位置が前衛（ワザがあたる場所）かどうか判定
 *
 * @param   pos
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsFrontPos( BtlPokePos pos ) const
{
  if( pos < BTL_POS_NUM )
  {
    u8 clientID, posIdx;

    this->BtlPosToClientID_and_PosIdx( pos, &clientID, &posIdx );
    if( clientID != BTL_CLIENTID_NULL )
    {
      u32 frontNum = this->GetFrontPosNum( clientID );
      return (posIdx < frontNum);
    }
  }
  return false;
}
//=============================================================================================
/**
 * 対戦相手タイプを返す
 *
 * @retval  BtlCompetitor
 */
//=============================================================================================
BtlCompetitor  MainModule::GetCompetitor( void ) const
{
  return m_setupParam->competitor;
}

//=============================================================================================
/**
 * 通信タイプを返す
 *
 * @retval  BtlCommMode
 */
//=============================================================================================
BtlCommMode MainModule::GetCommMode( void ) const
{
  return m_setupParam->commMode;
}

//=============================================================================================
/**
 * 勝利時BGMナンバーを返す
 *
 * @retval  u32
 */
//=============================================================================================
u32 MainModule::GetWinBGMNo( void ) const
{
  return m_setupParam->btlEffData.bgm_win;
}

//=============================================================================================
/**
 * 「にげる」を選択した時の反応タイプ
 *
 * @param   wk
 *
 * @retval  BtlEscapeMode   反応タイプ
 */
//=============================================================================================
BtlEscapeMode MainModule::GetEscapeMode( void ) const
{
  // @fix GFNMCat[2130]「バトルロイヤル」で誰かが降参したとき、降参した人が最下位となりません
  // バトルロイヤルでは逃げることが出来ないようにする
  if( this->GetRule() == BTL_RULE_ROYAL )
  {
    #if PM_DEBUG
    if( m_uiMan->GetButton(0)->IsHold(gfl2::ui::BUTTON_L|gfl2::ui::BUTTON_R) ){
      return BTL_ESCAPE_MODE_WILD;
    }
    #endif
    return BTL_ESCAPE_MODE_NG;
  }

  switch( m_setupParam->competitor ){
  case BTL_COMPETITOR_WILD:
    return BTL_ESCAPE_MODE_WILD;

  case BTL_COMPETITOR_TRAINER:
    #if PM_DEBUG
    if( m_uiMan->GetButton(0)->IsHold(gfl2::ui::BUTTON_L|gfl2::ui::BUTTON_R) ){
      return BTL_ESCAPE_MODE_WILD;
    }
    #endif
    return BTL_ESCAPE_MODE_NG;

  case BTL_COMPETITOR_INST:
    #if PM_DEBUG
    if( m_uiMan->GetButton(0)->IsHold(gfl2::ui::BUTTON_L|gfl2::ui::BUTTON_R) ){
      return BTL_ESCAPE_MODE_WILD;
    }
    #endif
    return BTL_ESCAPE_MODE_CONFIRM;

  case BTL_COMPETITOR_COMM:
    return BTL_ESCAPE_MODE_CONFIRM;


  default:
    return BTL_ESCAPE_MODE_NG;
  }
}
//=============================================================================================
/**
 * フィールドシチュエーションデータポインタを返す
 *
 * @param   wk
 *
 * @retval  const BTL_FIELD_SITUATION*
 */
//=============================================================================================
const BTL_FIELD_SITUATION* MainModule::GetFieldSituation( void ) const
{
  return &m_setupParam->fieldSituation;
}
//=============================================================================================
/**
 * バトル開始時の天候を返す
 *
 * @retval  BtlWeather    バトル開始時の天候
 */
//=============================================================================================
BtlWeather MainModule::GetDefaultWeather( void ) const
{
  const BTL_FIELD_SITUATION* sit = this->GetFieldSituation();
  return sit->weather;
}
//=============================================================================================
/**
 * エンカウントデータデータポインタを返す
 *
 * @param   wk
 *
 * @retval  const BSP_ENCOUNT_EFFECT*
 */
//=============================================================================================
const BSP_ENCOUNT_EFFECT* MainModule::GetEncountData( void ) const
{
  return &m_setupParam->btlEffData.enceff;
}
const BSP_BATTLE_EFFECT* MainModule::GetBattleEncountData( void ) const
{
  return &m_setupParam->btlEffData;
}

//=============================================================================================
/**
 * プレイヤー情報（セーブデータ）ハンドラを返す
 *
 * @param   wk
 *
 * @retval  const Savedata::MyStatus*
 */
//=============================================================================================
const Savedata::MyStatus* MainModule::GetPlayerStatus( void ) const
{
  return m_playerStatus;
}
//=============================================================================================
/**
 * 図鑑に登録されているポケモンかどうか判定
 *
 * @param monsno  チェック対象モンスター
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsZukanRegistered( MonsNo monsno ) const
{
  if( m_setupParam->zukanData )
  {
    return m_setupParam->zukanData->GetPokeGetFlag( monsno );
  }
  return false;
}
//=============================================================================================
/**
 * 図鑑に登録されているポケモンかどうか判定
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsZukanRegistered( const BTL_POKEPARAM* bpp ) const
{
  MonsNo monsno = (MonsNo)(bpp->GetMonsNo());
  return this->IsZukanRegistered( monsno );
}
//=============================================================================================
/**
 * 図鑑登録（捕獲）されているポケモン数を返す
 *
 * @param   wk
 *
 * @retval  u32
 */
//=============================================================================================
u32 MainModule::GetZukanCapturedCount( void ) const
{
  if( m_setupParam->zukanData ){
    return m_setupParam->zukanData->GetPokeGetCount();
  }

  return 0;
}
//=============================================================================================
/**
 * 図鑑セーブデータを返す
 *
 * @param   wk
 *
 * @retval  ZUKAN_SAVEDATA*
 */
//=============================================================================================
const Savedata::ZukanData* MainModule::GetZukanSaveData( void ) const
{
  return m_setupParam->zukanData;
}

//=============================================================================================
/**
 * @brief バトル開始時点での、図鑑セーブデータを返す
 */
//=============================================================================================
const Savedata::ZukanData* MainModule::GetZukanSaveDataOnBattleStart( void ) const
{
  return m_zukanDataOnBattleStart;
}

//=============================================================================================
/**
 * アイテムセーブデータを返す
 *
 * @param   wk
 *
 * @retval  Savedata::MyItem*
 */
//=============================================================================================
Savedata::MyItem* MainModule::GetItemSaveData( void ) const
{
  return m_setupParam->itemData;
}

Savedata::ConfigSave* MainModule::GetConfigSaveData( void ) const
{
  return m_setupParam->configData;
}

//=============================================================================================
/**
 *  図鑑「見た」フラグセット
 *
 * @param   clientID
 * @param   bpp
 */
//=============================================================================================
void MainModule::RegisterZukanSeeFlag( const BTL_POKEPARAM* bpp ) const
{
  // GFNMCat[880] ロイヤルドームで対戦したポケモンに「見たフラグ」が付く
  // バトルロイヤルでは、見たフラグを立てないようにする
  if( this->GetRule() == BTL_RULE_ROYAL )
  {
    return;
  }

  // 名前を「？？？？？」と表示するポケモンは、見たフラグをセットしない( 捕まえた時点で、見たフラグもセットされる )
  const MonsNo monsno = static_cast<MonsNo>( bpp->GetMonsNo() );
  if( UltraBeast::IsUnknownNamePokemon( *this, monsno ) )
  {
    return;
  }

  if( this->IsCompetitorScenarioMode() )
  {
    const pml::pokepara::PokemonParam* pp = bpp->GetSrcData();

    Savedata::ZukanData*  zukanData = m_setupParam->zukanData;
    if( zukanData )
    {
      //@バトル変更箇所
      //ここでは出会った回数を追加しない。
      //140109 Ariizumi
      zukanData->SetPokeSee( *pp );

      //@バトル変更箇所
      //見たフラグをカウントする
      u8 clientID   = PokeIDtoClientID( bpp->GetID() );
      u8 shortID = PokeIDtoShortID( bpp->GetID() );
      ARIIZUMI_PRINT("見たフラグチェック! PokeId[%d]Client[%d]baseID[%d]\n",bpp->GetID(),clientID,shortID);
      if( m_checkSeeAmount[clientID][shortID] == false )
      {
        ARIIZUMI_PRINT("フラグ追加！\n");
        //この関数のconstを外すと修正個所が大きくなってしまうので保留・・・
        bool *tempFlg = const_cast<bool*>(m_checkSeeAmount[clientID]);
        tempFlg[shortID] = true;
        zukanData->IncrementPokeSeeAmount( pp->GetMonsNo() );
      }
    }
  }
}

//=============================================================================================
/**
 * メガ進化ロングバージョンを再生するかどうかの判定
 *
 * @param   bpp
 * @retval  bool  trueのとき、ロングバージョンを再生する
 */
//=============================================================================================
bool MainModule::IsLongVersionMegaEvolve( const BTL_POKEPARAM* bpp ) const
{
  // @attention sangoでは使っていたがnijiでは無い
  return false;
}

//=============================================================================================
/**
 * メガ進化ロングバージョンを再生した!フラグのセット
 *
 * @param   bpp
 * @retval  bool  trueのとき、ロングバージョンを再生する
 */
//=============================================================================================
void MainModule::RecLongVersionMegaEvolved( const BTL_POKEPARAM* bpp )
{
  // レックウザのために関数変更
  //PokeTool::Mega::SetSeeFlagItem( m_setupParam->megaFlags, static_cast<MonsNo>(bpp->GetMonsNo()) ,bpp->GetItem() );
}

//=============================================================================================
/**
 * 特殊ポケ位置指定子を、実際のポケ位置に変換
 *
 * @param   wk            メインモジュールハンドラ
 * @param   exPos         特殊ポケモン位置指定子
 * @param   dst           [out] 対象ポケ位置を格納するバッファ
 *
 * @retval  u8    対象ポケ位置数
 */
//=============================================================================================
u8 MainModule::ExpandBtlPos( BtlExPos exPos, u8* dst ) const
{
  u8 exType = EXPOS_GET_TYPE( exPos );
  u8 basePos = EXPOS_GET_BASEPOS( exPos );

  if( exType == BTL_EXPOS_DEFAULT )
  {
    dst[0] = basePos;
    return (dst[0] != BTL_POS_NULL)? 1 : 0;
  }

  if( basePos != BTL_POS_NULL )
  {
    switch( this->GetRule() ){
    case BTL_RULE_SINGLE:
    default:
      return expandPokePos_single( exType, basePos, dst );
    case BTL_RULE_DOUBLE:
      return expandPokePos_double( exType, basePos, dst );
    case BTL_RULE_ROYAL:
      return expandPokePos_royal( exType, basePos, dst );
    case BTL_RULE_INTRUDE:
      return expandPokePos_intrude( exType, basePos, dst );
    }
  }

  dst[0] = BTL_POS_NULL;
  return 0;
}
// シングル用
u8 MainModule::expandPokePos_single( BtlExPos exType, u8 basePos, u8* dst ) const
{
  switch( exType ){
  default:
    return 0;
    /* fallthru */
  case BTL_EXPOS_AREA_ENEMY:
  case BTL_EXPOS_AREA_OTHERS:
  case BTL_EXPOS_FULL_ENEMY:
    dst[0] = GetOpponentPokePosByRule( BTL_RULE_SINGLE, basePos, 0 );
    break;
  case BTL_EXPOS_AREA_MYTEAM:
    dst[0] = basePos;
    break;
  case BTL_EXPOS_AREA_ALL:
  case BTL_EXPOS_FULL_ALL:
    dst[0] = BTL_POS_1ST_0;
    dst[1] = BTL_POS_2ND_0;
    return 2;
  }
  return 1;
}
// ダブル用
u8 MainModule::expandPokePos_double( BtlExPos exType, u8 basePos, u8* dst ) const
{
  switch( exType ){
  default:
    GFL_ASSERT(0);
    return 0;
    /* fallth ru */
  case BTL_EXPOS_AREA_ENEMY:
  case BTL_EXPOS_FULL_ENEMY:
    dst[0] = GetOpponentPokePosByRule( BTL_RULE_DOUBLE, basePos, 0 );
    dst[1] = GetOpponentPokePosByRule( BTL_RULE_DOUBLE, basePos, 1 );
    return 2;
  case BTL_EXPOS_AREA_OTHERS:
    dst[0] = GetOpponentPokePosByRule( BTL_RULE_DOUBLE, basePos, 0 );
    dst[1] = GetOpponentPokePosByRule( BTL_RULE_DOUBLE, basePos, 1 );
    dst[2] = GetFriendPokePos( basePos, 1 );
    return 3;
  case BTL_EXPOS_AREA_MYTEAM:
    dst[0] = basePos;
    dst[1] = GetFriendPokePos( basePos, 1 );
    if( dst[0] > dst[1] )
    {
      u8 tmp = dst[0];
      dst[0] = dst[1];
      dst[1] = tmp;
    }
    return 2;
  case BTL_EXPOS_AREA_FRIENDS:
  case BTL_EXPOS_NEXT_FRIENDS:
    dst[0] = GetFriendPokePos( basePos, 1 );
    return 1;

  case BTL_EXPOS_FULL_FRIENDS:
    dst[0] = basePos;
    dst[1] = GetFriendPokePos( basePos, 1 );
    return 2;

  case BTL_EXPOS_AREA_ALL:
  case BTL_EXPOS_FULL_ALL:
    dst[0] = BTL_POS_1ST_0;
    dst[1] = BTL_POS_2ND_0;
    dst[2] = BTL_POS_1ST_1;
    dst[3] = BTL_POS_2ND_1;
    return 4;
  }
}

// バトルロイヤル用
u8 MainModule::expandPokePos_royal( BtlExPos exType, u8 basePos, u8* dst ) const
{
  switch( exType )
  {
  default:
    GFL_ASSERT(0);
    return 0;

  case BTL_EXPOS_AREA_ENEMY:
  case BTL_EXPOS_FULL_ENEMY:
  case BTL_EXPOS_AREA_OTHERS:
    dst[0] = GetOpponentPokePosByRule( BTL_RULE_ROYAL, basePos, 0 );
    dst[1] = GetOpponentPokePosByRule( BTL_RULE_ROYAL, basePos, 1 );
    dst[2] = GetOpponentPokePosByRule( BTL_RULE_ROYAL, basePos, 2 );
    return 3;

  case BTL_EXPOS_AREA_MYTEAM:
    dst[0] = basePos;
    return 1;

  case BTL_EXPOS_AREA_FRIENDS:
  case BTL_EXPOS_FULL_FRIENDS:
  case BTL_EXPOS_NEXT_FRIENDS:
    return 0;

  case BTL_EXPOS_AREA_ALL:
  case BTL_EXPOS_FULL_ALL:
    dst[0] = BTL_POS_1ST_0;
    dst[1] = BTL_POS_2ND_0;
    dst[2] = BTL_POS_1ST_1;
    dst[3] = BTL_POS_2ND_1;
    return 4;
  }
}

// 乱入用
u8 MainModule::expandPokePos_intrude( BtlExPos exType, u8 basePos, u8* dst ) const
{
  switch( exType )
  {
  default:
    GFL_ASSERT(0);
    return 0;
    /* fallth ru */

  case BTL_EXPOS_AREA_ENEMY:
  case BTL_EXPOS_FULL_ENEMY:
    {
      u32 num = 0;
      for( u32 pos=0; pos<BTL_POS_NUM; ++pos )
      {
        if(  ( this->IsPokePosExist( pos ) ) &&
            !( this->IsFriendPokePos( basePos, pos ) ) )
        {
          dst[ num ] = pos;
          num = num + 1;
        }
      }
      return num;
    }

  case BTL_EXPOS_AREA_OTHERS:
    {
      u32 num = 0;
      for( u32 pos=0; pos<BTL_POS_NUM; ++pos )
      {
        if( ( this->IsPokePosExist( pos ) ) &&
            ( pos != basePos ) )
        {
          dst[ num ] = pos;
          num = num + 1;
        }
      }
      return num;
    }

  case BTL_EXPOS_AREA_MYTEAM:
  case BTL_EXPOS_FULL_FRIENDS:
    {
      u32 num = 0;
      for( u32 pos=0; pos<BTL_POS_NUM; ++pos )
      {
        if( ( this->IsPokePosExist( pos ) ) &&
            ( this->IsFriendPokePos( basePos, pos ) ) )
        {
          dst[ num ] = pos;
          num = num + 1;
        }
      }
      return num;
    }

  case BTL_EXPOS_AREA_FRIENDS:
  case BTL_EXPOS_NEXT_FRIENDS:
    {
      u32 num = 0;
      for( u32 pos=0; pos<BTL_POS_NUM; ++pos )
      {
        if( ( this->IsPokePosExist( pos ) ) &&
            ( pos != basePos ) &&
            ( this->IsFriendPokePos( basePos, pos ) ) )
        {
          dst[ num ] = pos;
          num = num + 1;
        }
      }
      return num;
    }

  case BTL_EXPOS_AREA_ALL:
  case BTL_EXPOS_FULL_ALL:
    {
      u32 num = 0;
      for( u32 pos=0; pos<BTL_POS_NUM; ++pos )
      {
        if( this->IsPokePosExist( pos ) )
        {
          dst[ num ] = pos;
          num = num + 1;
        }
      }
      return num;
    }
  }
}

/**
 *  指定クライアントの担当する位置数を返す
 */
u32 MainModule::GetClientCoverPosNum( u8 clientID ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetClientCoverPosNum( rule, m_setupParam->multiMode, static_cast<BTL_CLIENT_ID>( clientID ) );
}

/**
 * @brief 立ち位置の担当クライアントを取得する
 * @param pos  立ち位置
 */
BTL_CLIENT_ID MainModule::GetPosCoverClientId( BtlPokePos pos ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetPosCoverClientId( rule, m_setupParam->multiMode, pos );
}

//=============================================================================================
/**
 * 有効なクライアントかどうか判定
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsExistClient( u8 clientID ) const
{
  BtlRule rule = this->GetRule();
  return BattleRule::IsClientExist( rule, m_setupParam->multiMode, static_cast<BTL_CLIENT_ID>( clientID ) );
}

//=============================================================================================
/**
 * マルチモードかどうかを返す
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsMultiMode( void ) const
{
  return m_setupParam->multiMode != BTL_MULTIMODE_NONE;
}

//=============================================================================================
/**
 * @brief プレイヤのレーティングを表示するか？
 * @retval true   表示する
 * @retval false  表示しない
 */
//=============================================================================================
bool MainModule::IsPlayerRatingDisplay( void ) const
{
  return m_setupParam->isPlayerRatingDisplay;
}

//=============================================================================================
/**
 * クライアントIDからサイドIDを返す
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  BtlSide
 */
//=============================================================================================
BtlSide MainModule::GetClientSide( u8 clientID ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetClientSide( rule, static_cast<BTL_CLIENT_ID>( clientID ) );
}
//=============================================================================================
/**
 * プレイヤーサイドかどうか判定
 *
 * @param   wk
 * @param   side
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsPlayerSide( BtlSide side ) const
{
  BtlSide playerSide = GetClientSide( m_myClientID );
  return PokemonPosition::IsPlayerSide( playerSide, side );
}
//=============================================================================================
/**
 * @brief 陣営が存在するか？
 * @param side  チェック対象の陣営
 * @retval true   陣営が存在する
 * @retval false  陣営が存在しない
 */
//=============================================================================================
bool MainModule::IsSideExist( BtlSide side ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::IsSideExist( rule, side );
}

//=============================================================================================
/**
 * @brief 複数の陣営を表す BtlSide( BTL_MULTI_SIDE_xxxx ) を、単一の陣営を表す BtlSide( BTL_SIDE_xxxx )に展開する
 * @param[out] expandSide     展開後の陣営の格納先配列( 要素数 BTL_SIDE_NUM 必要 )
 * @param[out] expandSideNum  展開後の陣営の数
 * @param      targetSide     展開する陣営
 */
//=============================================================================================
void MainModule::ExpandSide( BtlSide* expandSide, u8* expandSideNum, BtlSide side ) const
{
  BtlRule rule = this->GetRule();
  PokemonPosition::ExpandSide( expandSide, expandSideNum, rule, side );
}

//=============================================================================================
/**
 * @brief 陣営から立ち位置を取得する
 * @param side      陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
 * @param posIndex  陣営内の位置インデックス
 */
//=============================================================================================
BtlPokePos MainModule::GetSidePos( BtlSide side, u8 index ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetSidePos( rule, side, index );
}

//=============================================================================================
/**
 * @brief 存在する陣営の数を取得する
 */
//=============================================================================================
u8 MainModule::GetSideNum( void ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetSideNum( rule );
}
//=============================================================================================
/**
 * @brief 敵クライアントの数を取得する
 * @param clientID  自分のクライアントID
 */
//=============================================================================================
u8 MainModule::GetEnemyClientNum( u8 clientID ) const
{
  BtlRule rule = this->GetRule();
  return BattleRule::GetEnemyClientNum( rule, m_setupParam->multiMode, static_cast<BTL_CLIENT_ID>( clientID ) );
}
//=============================================================================================
/**
 * @brief 相手側のポケモン位置ID取得
 * @param basePos   自分側の位置
 * @param idx       相手側の何番目の位置を取得するか？
 */
//=============================================================================================
BtlPokePos MainModule::GetOpponentPokePos( BtlPokePos basePos, u8 idx ) const
{
  BtlRule rule = this->GetRule();
  return GetOpponentPokePosByRule( rule, basePos, idx );
}

//=============================================================================================
/**
 * @brief 指定位置の味方の戦闘位置を返す
 *
 * @param   basePos
 * @param   idx  (==0だと自分の位置）
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos MainModule::GetFriendPokePos( BtlPokePos basePos, u8 idx ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetFriendPokePos( rule, basePos, idx );
}

//=============================================================================================
/**
 * @brief 味方同士の位置かどうかを判定する
 *
 * @param   pos1  判定対象の位置1
 * @param   pos2  判定対象の位置2
 *
 * @retval  true   味方同士の場合
 * @retval  false  味方同士でない場合
 */
//=============================================================================================
bool MainModule::IsFriendPokePos( BtlPokePos pos1, BtlPokePos pos2 ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::IsFriendPokePos( rule, pos1, pos2 );
}

//=============================================================================================
/**
 * ２つのクライアントIDが対戦相手同士のものかどうかを判別
 *
 * @param   clientID1
 * @param   clientID2
 *
 * @retval  bool    対戦相手同士ならtrue
 */
//=============================================================================================
bool MainModule::IsOpponentClientID( u8 clientID1, u8 clientID2 ) const
{
  BtlRule rule = this->GetRule();
  return BattleRule::IsOpponentClient( rule, static_cast<BTL_CLIENT_ID>( clientID1 ), static_cast<BTL_CLIENT_ID>( clientID2 ) );
}

//--------------------------------------------------------------------------
/**
 * 戦闘位置->クライアントIDに 変換
 *
 * @param   btlPos
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------
u8 MainModule::btlPos_to_clientID( BtlPokePos btlPos ) const
{
  if( BTL_POS_NUM <= btlPos )
  {
    GFL_PRINT( "pos=%d\n", btlPos );
    GFL_ASSERT(0);
  }
  return this->GetPosCoverClientId( btlPos );
}
//--------------------------------------------------------------------------
/**
 * 戦闘位置->クライアントID，位置インデクス（クライアントごと0オリジン，連番）に変換
 *
 * @param   btlPos    [in] 戦闘位置
 * @param   clientID  [out]
 * @param   posIdx    [out]
 *
 */
//--------------------------------------------------------------------------
void MainModule::btlPos_to_cliendID_and_posIdx( BtlPokePos btlPos, u8* clientID, u8* posIdx ) const
{
  *clientID = btlPos_to_clientID( btlPos );

  if( *clientID != BTL_CLIENTID_NULL )
  {
    u8 idx = 0;
    while( btlPos-- )
    {
      BTL_CLIENT_ID posCoverClientId = this->GetPosCoverClientId( btlPos );
      if( posCoverClientId == *clientID  )
      {
        ++idx;
      }
    }

    *posIdx = idx;
  }
  else
  {
    *posIdx = 0;
  }
}

//=============================================================================================
/**
 * 特殊ポケ位置指定子を、実際のポケ位置に展開後、
 * 実際にそこに存在しているポケモンパラメータをポインタ配列に格納
 *
 * @param   pokeCon
 * @param   exPos
 * @param   bppAry
 *
 * @retval  u8        展開されたポケモン数
 */
//=============================================================================================
u8 MainModule::ExpandExistPokeID( const POKECON* pokeCon, BtlExPos exPos, u8* pokeIDAry ) const
{
  const BTL_POKEPARAM* bpp;
  u8 pokePos[ BTL_EXIST_POS_MAX ];
  u8 posCnt, existCnt, i;

  posCnt = ExpandBtlPos( exPos, pokePos );
  for(i=0, existCnt=0; i<posCnt; ++i)
  {
    bpp = pokeCon->GetFrontPokeDataConst( pokePos[i] );
    if( (bpp != NULL) && !(bpp->IsDead()) )
    {
      pokeIDAry[ existCnt++ ] = bpp->GetID();
    }
  }
  return existCnt;
}

//=============================================================================================
/**
 * バトルポケモンIDをポケモン戦闘位置に変換
 *
 * @param   pokeID
 *
 * @retval  BtlPokePos    ポケモン戦闘位置（戦闘に出ていなければBTL_POS_NULL）
 */
//=============================================================================================
BtlPokePos  MainModule::PokeIDtoPokePos( const POKECON* pokeCon, u8 pokeID ) const
{
  u8 clientID = PokeIDtoClientID( pokeID );

  int idx = pokeCon->FindPokemon( clientID, pokeID );
  if( idx >= 0 )
  {
    BtlPokePos pos = GetClientPokePos( clientID, idx );
    if( pos != BTL_POS_NUM )
    {
      return pos;
    }
  }
  return BTL_POS_NULL;
}
//=============================================================================================
/**
 * バトルポケモンIDをポケモン描画位置に変換
 *
 * @param   pokeCon
 * @param   pokeID
 *
 * @retval  u8   （戦闘に出ていない場合 BTL_VPOS_ERROR )
 */
//=============================================================================================
BtlvPos  MainModule::PokeIDtoViewPos( const POKECON* pokeCon, u8 pokeID ) const
{
  BtlPokePos pos = PokeIDtoPokePos( pokeCon, pokeID );
  if( pos != BTL_POS_NULL )
  {
    return BtlPosToViewPos( pos );
  }
  else
  {
    return BTL_VPOS_ERROR;
  }
}
//=============================================================================================
/**
 * 戦闘位置->クライアントID変換
 *
 * @param   wk
 * @param   pos
 *
 */
//=============================================================================================
u8 MainModule::BtlPosToClientID( BtlPokePos pos ) const
{
  return btlPos_to_clientID( pos );
}
//=============================================================================================
/**
 * 戦闘位置->位置インデックス変換
 *
 * @param   pos
 *
 * @retval  u8
 */
//=============================================================================================
u8 MainModule::BtlPosToPosIdx( BtlPokePos pos ) const
{
  u8 clientID, posIdx;
  btlPos_to_cliendID_and_posIdx( pos, &clientID, &posIdx );
  return posIdx;
}
//=============================================================================================
/**
 * 戦闘位置 -> クライアントID＆位置インデックス変換
 *
 * @param   wk
 * @param   pos
 * @param   clientID
 * @param   posIdx
 *
 */
//=============================================================================================
void MainModule::BtlPosToClientID_and_PosIdx( BtlPokePos pos, u8* clientID, u8* posIdx ) const
{
  btlPos_to_cliendID_and_posIdx( pos, clientID, posIdx );
}

/**
 * @brief クライアントID -> トレーナー表示位置
 * @retval BTL_VPOS_TR_ERROR  指定したクライアントIDが不正な場合
 */
BtlvTrainerPos MainModule::ClientIDtoTrainerViewPos( u8 clientID ) const
{
  u8 clientRelation = this->ClientIDtoRelation( m_myClientID, clientID );
  switch( clientRelation ) {
  case BTL_CLIENT_PLAYER:  return BTL_VPOS_TR_NEAR_1;
  case BTL_CLIENT_PARTNER:  return BTL_VPOS_TR_NEAR_2;
  case BTL_CLIENT_ENEMY1:  return BTL_VPOS_TR_FAR_1;
  case BTL_CLIENT_ENEMY2:  return BTL_VPOS_TR_FAR_2;
  }
  GFL_ASSERT(0);
  return BTL_VPOS_TR_ERROR;
}


//=============================================================================================
/**
 * 戦闘位置 -> 描画位置変換
 *
 * @param   wk
 * @param   pos
 *
 * @retval  u8
 */
//=============================================================================================
BtlvPos MainModule::BtlPosToViewPos( BtlPokePos pos ) const
{
  u8 isPlayerSide = ( (m_myOrgPos&1) == (pos&1) );

  switch( this->GetRule() ){
  default:
    GFL_ASSERT(0);
    /* fallthru */
  case BTL_RULE_SINGLE:
    return (isPlayerSide)? BTL_VPOS_NEAR_1 : BTL_VPOS_FAR_1;

  case BTL_RULE_DOUBLE:
  case BTL_RULE_INTRUDE:
    {
      static const u8 vpos[2][BTL_POSIDX_MAX] = {
        { BTL_VPOS_FAR_1,  BTL_VPOS_FAR_2   },
        { BTL_VPOS_NEAR_1, BTL_VPOS_NEAR_2  },
      };
      u8 posIdx = PokemonPosition::PosToSidePosIndex( BTL_RULE_DOUBLE, pos );
      u8 result = vpos[ isPlayerSide ][ posIdx ];

      return (BtlvPos)result;
    }

  case BTL_RULE_ROYAL:
    {
      static const BtlvPos VPOS_LIST[] = 
      {
        BTL_VPOS_ROYAL_NEAR, 
        BTL_VPOS_ROYAL_RIGHT, 
        BTL_VPOS_ROYAL_FAR,
        BTL_VPOS_ROYAL_LEFT,
      };
      for( u32 i=0; i<GFL_NELEMS(VPOS_LIST); ++i )
      {
        const BtlvPos vpos = VPOS_LIST[i];
        if( vpos == BTL_VPOS_ERROR )
        {
          continue;
        }

        const BtlPokePos calc_pos = this->ViewPosToBtlPos( vpos );
        if( calc_pos == pos )
        {
          return vpos;
        }
      }
      //@attention 本来であればBTL_VPOS_ERRORを返すべきだが、下記理由によりNEARを返す
      //ROYALで初めてERRORが返るようになったため、ERRORをケアしているコードが少ない
      //ROYAL以外ではERRORではなく存在する範囲内の値(恐らくNEAR)が返っていた。
      GFL_ASSERT_MSG(0,"BtlPokePos is NULL!![%d]\n",pos);
      return BTL_VPOS_ROYAL_NEAR;
      //return BTL_VPOS_ERROR;
    }
  }
}
//=============================================================================================
/**
 * 描画位置 -> 戦闘位置変換
 *
 * @param   wk
 * @param   vpos
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos MainModule::ViewPosToBtlPos( u8 vpos ) const
{  
  const BtlRule rule = this->GetRule();

  if( rule == BTL_RULE_SINGLE )
  {
    bool isFarSide = ( vpos & 1 );
    if( isFarSide )
    {
      return !(m_myOrgPos);
    }
    else
    {
      return m_myOrgPos;
    }
  }

  if( rule == BTL_RULE_ROYAL )
  {
    BtlPokePos pos = ( m_myOrgPos + vpos ) % BTL_POS_ROYAL_NUM;
    return pos;
  }

  {
    bool isFarSide = ( vpos & 1 );
    u8 org_pos = ( m_myOrgPos & 1 );
    u8 result;
  
    if( isFarSide )
    {
      result =  (org_pos ^ 1) + ((vpos - BTL_VPOS_NEAR_1 ) - 1);
    }
    else
    {
      result =  org_pos + (vpos - BTL_VPOS_NEAR_1);
    }
  
    BTL_Printf("myOrgPos=%d, vpos=%d, resultPos=%d\n", m_myOrgPos, vpos, result);
  
    return result;
  }
}

u8 MainModule::GetPlayerClientID( void ) const
{
  return m_myClientID;
}
/**
 *  存在するならプレイヤーの味方クライアントIDを返す（存在しない場合 BTL_CLIENTID_NULL ）
 */
u8 MainModule::GetPlayerFriendCleintID( void ) const
{
  return GetFriendCleintID( m_myClientID );
}
/**
 *  存在するなら指定クライアントの味方クライアントIDを返す（存在しない場合 BTL_CLIENTID_NULL ）
 */
u8 MainModule::GetFriendCleintID( u8 clientID ) const
{
  // ロイヤルはフレンドはいない
  if( GetRule() == BTL_RULE_ROYAL )
  {
    return BTL_CLIENTID_NULL;
  }
  u8 friendClientID = getFriendCrientID( clientID );
  if( IsExistClient(friendClientID) ){
    return friendClientID;
  }
  return BTL_CLIENTID_NULL;
}
u8 MainModule::GetEnemyClientID( u8 idx ) const
{
  return GetOpponentClientID( m_myClientID, idx );
}
u32 MainModule::GetOpponentClientID( u8 clientID, u8 idx ) const
{
  BtlRule rule = this->GetRule();
  return BattleRule::GetOpponentClientId( rule, m_setupParam->multiMode, static_cast<BTL_CLIENT_ID>( clientID ), idx );
}



//=============================================================================================
/**
 * プレイヤー使用アイテムをバッグから減らす
 *
 * @param   wk
 * @param   clientID
 * @param   itemID
 *
 * @return  減らしたら true （消費しないアイテムの場合 false）
 */
//=============================================================================================
//@バトル変更箇所
//戻り値に減らせたかのチェックを追加(ビードロ使用時に、ビードロが増えていった。
//ダブルバトルとかで2体目の行動時にアイテムの使用分を減らす処理で、
//ビードロは減らないけど数は復帰させていたから。btl_client selItemWork_ReserveとselItemWork_Restore)
bool  MainModule::DecrementPlayerItem( u8 clientID, u16 itemID ) const
{
  if( !IsRecordPlayMode() )
  {
    if( clientID == m_myClientID )
    {
      //@バトル変更箇所 田谷さん確認済み
      //ビードロは使ってもなくならない。アイテムデータに非消費フラグを設定して、フックする。
      if( calc::ITEM_GetParam(itemID, item::ITEM_DATA::PRM_ID_USE_SPEND) == 1 )
      {
        return false;
      }
      
      m_setupParam->itemData->Sub( itemID, 1 );
      return true;
    }
  }
  return false;
}
//=============================================================================================
/**
 * プレイヤー使用アイテムを追加（書き戻し）
 *
 * @param   wk
 * @param   clientID
 * @param   itemID
 *
 * @return  追加できたら true （アイテム満タンなどで追加できなければ false）
 */
//=============================================================================================
bool MainModule::AddItem( u8 clientID, u16 itemID )
{
  if( !IsRecordPlayMode() )
  {
    if( clientID == m_myClientID )
    {
      return m_setupParam->itemData->Add( itemID, 1 );
    }
  }
  return false;
}

#if 0
//=============================================================================================
/**
 * アイテムデータポインタ取得
 *
 * @param   wk
 *
 * @retval  MYITEM_PTR
 */
//=============================================================================================
MYITEM_PTR BTL_MAIN_GetItemDataPtr( BTL_MAIN_MODULE* wk )
{
  return wk->setupParam->itemData;
}
//=============================================================================================
/**
 * バッグカーソルデータ取得
 *
 * @param   wk
 *
 * @retval  BAG_CURSOR*
 */
//=============================================================================================
BAG_CURSOR* BTL_MAIN_GetBagCursorData( BTL_MAIN_MODULE* wk )
{
  return wk->setupParam->bagCursor;
}
#endif
//=============================================================================================
/**
 * 録画モード有効かチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsRecordEnable( void ) const
{
  if( (m_setupParam->recBuffer != NULL)
  &&  (!IsRecordPlayMode())
  ){
    return true;
  }
  return false;
}
//=============================================================================================
/**
 * デバッグフラグ取得
 *
 * @param   wk
 * @param   flag
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::GetDebugFlag( BtlDebugFlag flag ) const
{
  bool result = BTL_SETUP_GetDebugFlag( m_setupParam, flag );
//  BTL_PRINT("[MAIN_MOD] DebugFlag(%d) = %d\n", flag, result );
  return result;
}

//=============================================================================================
/**
 * サーバから捕獲したポケモン位置の通知を受け付け
 *
 * @param   wk
 * @param   pos
 */
//=============================================================================================
void MainModule::NotifyCapturedPokePos( BtlPokePos pos )
{
  if( m_setupParam->capturedPokeIdx == BTL_PARTY_MEMBER_MAX )
  {
    u8 clientID, posIdx;
    btlPos_to_cliendID_and_posIdx( pos, &clientID, &posIdx );
    m_setupParam->capturedPokeIdx = posIdx;
  }
}

//=============================================================================================
/**
 * @brief 捕獲したポケモンが、図鑑に新規登録されるか？
 * @retval true   図鑑に新規登録される
 * @retval false  図鑑に新規登録されない( 捕獲していない or 既に登録済みの場合 )
 */
//=============================================================================================
bool MainModule::IsCapturedPokeZukanNewRegistered( void ) const
{
  const u8 capturedPokeIndex = m_setupParam->capturedPokeIdx;
  if( capturedPokeIndex == BTL_PARTY_MEMBER_MAX )
  {
    return false;
  }

  const pml::pokepara::PokemonParam* capturedPokeParam = m_srcParty[ BTL_CLIENT_ENEMY1 ]->GetMemberPointerConst( capturedPokeIndex );
  if( capturedPokeParam == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }

  const MonsNo capturedMonsNo = capturedPokeParam->GetMonsNo();
  const Savedata::ZukanData* zukan = this->GetZukanSaveDataOnBattleStart();
  return !( zukan->GetPokeGetFlag( capturedMonsNo ) );
}

//=============================================================================================
/**
 * 3DSを逆さにしてレベルアップしたポケモン位置の通知を受付
 * @param pos
 */
//=============================================================================================
void MainModule::NotifyTurnedLevelUpPokePos( u8 pokeID )
{
  u8 clientID = PokeIDtoClientID( pokeID );
  if( clientID == m_myClientID )
  {
    u8 posIdx = PokeIDtoShortID(pokeID);
    m_setupParam->turnedLvUpPokeIndex[ posIdx ] = true;
  }
}

//=============================================================================================
/**
 * @brief 3DSを逆さにしてレベルアップしたか？
 * @param pokeID  チェック対象ポケモンのID
 */
//=============================================================================================
bool MainModule::CheckTurnedLevelUp( u8 pokeID ) const
{
  u8 clientID = PokeIDtoClientID( pokeID );
  if( clientID == m_myClientID )
  {
    u8 posIdx = PokeIDtoShortID(pokeID);
    return m_setupParam->turnedLvUpPokeIndex[ posIdx ];
  }
  return false;
}

//=============================================================================================
/**
 * サーバから勝敗判定結果を受け付け
 *
 * @param result      勝敗判定結果
 * @param resultCause 勝敗判定結果の判定理由
 */
//=============================================================================================
void MainModule::NotifyBattleResult( BtlResult result, ResultCause resultCause )
{
  if( m_serverResult == BTL_RESULT_MAX )
  {
    m_serverResult = result;
    m_serverResultCause = resultCause;
  }
}
//=============================================================================================
/**
 *
 *
 * @param   wk
 *
 * @retval  BtlResult
 */
//=============================================================================================
BtlResult MainModule::GetBattleResult( void )
{
  return checkWinner();
}

ResultCause MainModule::GetBattleResultCause( void ) const
{
  return m_serverResultCause;
}

//=============================================================================================
/**
 * 通信対戦時コマンド整合性チェックエラー発生通知を受け付け
 *
 * @param   wk
 */
//=============================================================================================
void MainModule::NotifyCmdCheckError( void )
{
  m_setupParam->cmdIllegalFlag = true;
  net::SetNetworkError();//切断フローに流す
}

//=============================================================================================
/**
 * おこずかい額を現在の値で決定する
 *
 * @param   wk
 */
//=============================================================================================
u32 MainModule::FixRegularMoney( void )
{
  if( m_fGetMoneyFixed == false )
  {
    BTL_PRINT("デフォでもらえるお金   = %d\n", m_regularMoney );
    BTL_PRINT("ボーナスでもらえるお金 = %d\n", m_bonusMoney );

    u32 dblupRatio = this->calcMoneyDblUpRatio();
    if( dblupRatio > 1 )
    {
      m_regularMoney *= dblupRatio;
      m_bonusMoney   *= dblupRatio;
      if( m_bonusMoney > BTL_BONUS_MONEY_MAX ){
        m_bonusMoney = BTL_BONUS_MONEY_MAX;
      }

    }

    // ロトムパワー補正
    m_regularMoney = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->CalcMoney(m_regularMoney);
    
    //汎用お小遣いレート反映(再戦施設など)
    m_regularMoney = static_cast<u32>(static_cast<f32>(m_regularMoney)*m_setupParam->moneyRate);
    BTL_PRINT("  汎用レート補正[x%4.1f]で %d\n", m_setupParam->moneyRate, m_regularMoney );

    if( m_regularMoney > BTL_REGULAR_MONEY_MAX ){
      m_regularMoney = BTL_REGULAR_MONEY_MAX;
    }

    m_fGetMoneyFixed = true;
  }

  return m_regularMoney;
}

//=============================================================================================
/**
 * ボーナス額の増量受け付け
 *
 * @param   wk
 * @param   volume
 */
//=============================================================================================
void MainModule::AddBonusMoney( u32 volume )
{
  if( m_fGetMoneyFixed == false )
  {
    m_bonusMoney += volume;
    if( m_bonusMoney > BTL_BONUS_MONEY_MAX ){
      m_bonusMoney = BTL_BONUS_MONEY_MAX;
    }
  }
}
//=============================================================================================
/**
 * ボーナス額を取得
 *
 * @param   wk
 * @param   volume
 */
//=============================================================================================
u32 MainModule::GetBonusMoney( void ) const
{
  // 経験値・お金なしバトルが指定されている場合
  if ( !this->IsMoneySeqEnable() ) return 0;

  u32 bonus = m_bonusMoney;
  if( !m_fGetMoneyFixed )
  {
    u32 dblup_ratio = this->calcMoneyDblUpRatio();
    if( dblup_ratio > 1 )
    {
      bonus *= dblup_ratio;
      if( bonus > BTL_BONUS_MONEY_MAX ){
        bonus = BTL_BONUS_MONEY_MAX;
      }
    }
  }
  return bonus;
}

//=============================================================================================
/**
 * おこづかい・ボーナスのダブルアップ設定
 *
 * @param   wk
 */
//=============================================================================================
void MainModule::SetMoneyDblUp( MoneyDblUpCause cause )
{
  if( cause < GFL_NELEMS(m_moneyDblUpCause) )
  {
    m_moneyDblUpCause[ cause ] = true;
  }
  else{
    GFL_ASSERT(0);
  }
}
/**
 *  おこづかい・ボーナスのダブルアップ倍率を計算
 */
u32 MainModule::calcMoneyDblUpRatio( void ) const
{
  uint32_t ratio = 1;

  for(u32 i=0; i<GFL_NELEMS(m_moneyDblUpCause); ++i)
  {
    if( m_moneyDblUpCause[i] ){
      ratio *= 2;
    }
  }
  return ratio;
}

//=============================================================================================
/**
 * 負けた時の支払い金額を確定させる
 *
 * @param   wk
 *
 * @retval  u32
 */
//=============================================================================================
u32 MainModule::FixLoseMoney( void )
{
  // 経験値・お金なしバトルが指定されている場合
  if ( !this->IsMoneySeqEnable() ) return 0;

  u32 loseMoney = 0;

  if( m_fLoseMoneyFixed == false )
  {
    if( this->IsCompetitorScenarioMode() )
    {
      loseMoney = btl::calc::CalcLoseMoney( m_setupParam, m_pokeconForServer );
      m_loseMoney = loseMoney;
    }
    m_fLoseMoneyFixed = true;
  }
  return loseMoney;
}
//=============================================================================================
/**
 * 戦闘終了時、BGMをフェードアウトさせない（捕獲時など）
 *
 * @param   wk
 */
//=============================================================================================
void MainModule::BGMFadeOutDisable( void )
{
  m_fBGMFadeOutDisable = true;
}
//=============================================================================================
/**
 * プレイヤーポケモンが死んだ時のなつき度反映
 *
 * @param   wk
 * @param   bpp
 * @param   fLargeDiffLevel
 */
//=============================================================================================
void MainModule::ReflectNatsukiDead( BTL_POKEPARAM* bpp, bool fLargeDiffLevel ) const
{
  if( this->IsCompetitorScenarioMode() )
  {
    if( !IsRecordPlayMode() )
    {
      PokeTool::NatsukiType calcID = (fLargeDiffLevel)? PokeTool::NATSUKI_TYPE_LEVEL30_HINSHI : PokeTool::NATSUKI_TYPE_HINSHI;
      natsukiPut( bpp, calcID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * なつき度計算、サーバ・クライアント双方のsrcPPに反映させる
 *
 * @param   wk
 * @param   bpp
 * @param   calcID
 */
//----------------------------------------------------------------------------------
void MainModule::natsukiPut( BTL_POKEPARAM* bpp, PokeTool::NatsukiType calcID ) const
{
  u8 pokeID = bpp->GetID();

  const BTL_POKEPARAM* bppServer = m_pokeconForServer->GetPokeParamConst( pokeID );
  const BTL_POKEPARAM* bppClient = m_pokeconForClient->GetPokeParamConst( pokeID );

  pml::pokepara::PokemonParam* ppServer = const_cast<pml::pokepara::PokemonParam*>(bppServer->GetSrcData());
  pml::pokepara::PokemonParam* ppClient = const_cast<pml::pokepara::PokemonParam*>(bppClient->GetSrcData());

  const BTL_FIELD_SITUATION* sit = GetFieldSituation();

  PokeTool::NatsukiManager::Calc(ppServer, calcID, sit->place_id );
  PokeTool::NatsukiManager::Calc(ppClient, calcID, sit->place_id );
}


//=============================================================================================
/**
 * クライアント側で処理したレベルアップ・ワザおぼえの結果をサーバ用パラメータに反映
 *
 * @param   wk
 * @param   pokeID
 */
//=============================================================================================
void MainModule::ReflectPokeWazaOboe( u8 pokeID )
{
  BTL_POKEPARAM  *bppSv, *bppCl;

  bppSv = m_pokeconForServer->GetPokeParam( pokeID );
  bppCl = m_pokeconForClient->GetPokeParam( pokeID );

  bppCl->WAZA_Copy( bppSv );
}

/**
 *  イリュージョン中ポケモンのイリュージョン対象データを取得（AI用）
 */
const BTL_POKEPARAM* MainModule::GetFakeTargetPokeParam( POKECON* pokeCon, const BTL_POKEPARAM* bpp ) const
{
  if( bpp->IsFakeEnable() )
  {
    u8 clientID = PokeIDtoClientID( bpp->GetID() );
    const pml::pokepara::PokemonParam* ppFake = bpp->GetViewSrcData();
    const BTL_PARTY* party = pokeCon->GetPartyDataConst( clientID );
    u32 memberCount = party->GetMemberCount();
    const BTL_POKEPARAM* member;
    u32 i;
    for(i=0; i<memberCount; ++i)
    {
      member = party->GetMemberDataConst( i );
      if( (uptr)(member->GetSrcData()) == (uptr)(ppFake) ){
        return member;
      }
    }
  }
  return bpp;
}
//=============================================================================================
/**
 * クライアントの管理位置数を返す
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  u8
 */
//=============================================================================================
u8 MainModule::GetClientFrontPosCount( u8 clientID ) const
{
  return checkNumFrontPos( clientID );
}
//=============================================================================================
/**
 * 指定クライアントの控えポケモンIndex開始値を返す
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  u8
 */
//=============================================================================================
u8 MainModule::GetClientBenchPosIndex( u8 clientID ) const
{
  u8 index = checkNumFrontPos( clientID );
  return index;
}

//-------------------------------------------------------------------
/**
 * 味方同士のポケモンIDかどうかを判定する
 *
 * @param   pokeID1
 * @param   pokeID2
 *
 * @retval  bool    味方同士ならtrue
 */
//-------------------------------------------------------------------
bool MainModule::IsFriendPokeID( u8 pokeID1, u8 pokeID2 ) const
{
  if( (pokeID1 == BTL_POKEID_NULL)
  ||  (pokeID2 == BTL_POKEID_NULL)
  ){
    return false;
  }
  BtlSide side1 = PokeIDtoSide( pokeID1 );
  BtlSide side2 = PokeIDtoSide( pokeID2 );
  return side1 == side2;
}
//-------------------------------------------------------------------
/**
 * 味方同士の位置かどうかを判定する
 *
 * @param   pos1
 * @param   pos2
 *
 * @retval  bool    味方同士ならtrue
 */
//-------------------------------------------------------------------
bool MainModule::IsFriendPokePos( BtlRule rule, BtlPokePos pos1, BtlPokePos pos2 )
{
  return PokemonPosition::IsFriendPokePos( rule, pos1, pos2 );
}

//-------------------------------------------------------------------
/**
 *ポケモンID -> サイドID変換
 *
 * @param   pokeID
 *
 * @retval  BtlSide
 */
//-------------------------------------------------------------------
BtlSide MainModule::PokeIDtoSide( u8 pokeID ) const
{
  BtlRule rule     = this->GetRule();
  u8      clientId = PokeIDtoClientID( pokeID );
  return PokemonPosition::GetClientSide( rule, static_cast<BTL_CLIENT_ID>( clientId ) );
}
//-------------------------------------------------------------------
/**
 * ポケモンID -> 敵方サイドID変換
 *
 * @param   pokeID
 *
 * @retval  BtlSide
 */
//-------------------------------------------------------------------
BtlSide MainModule::PokeIDtoOpponentSide( u8 pokeID ) const
{
  BtlSide side = PokeIDtoSide( pokeID );
  return this->GetOpponentSide( side );
}
//-------------------------------------------------------------------
/**
 *  敵型サイドID変換
 */
//-------------------------------------------------------------------
BtlSide MainModule::GetOpponentSide( BtlSide side ) const
{
  BtlRule rule = this->GetRule();
  return PokemonPosition::GetOpponentSide( rule, side );
}

//----------------------------------------------------------------------
/**
 * とくせい「イリュージョン」を持つメンバーの参照ポケデータ更新
 *
 * @param   party
 */
//----------------------------------------------------------------------
void MainModule::SetIllusionForParty( BTL_PARTY* party, u8 clientID ) const
{
  int lastPokeIdx   = party->GetIllusionTargetMemberIndex();
  int visiblePosCnt = GetClientCoverPosNum(clientID);

  TAYA_Printf("見えている開始位置(%d)まではイリュージョンにしません\n", visiblePosCnt);

  for(int i=visiblePosCnt; i<party->GetMemberCount(); ++i)
  {
    BTL_POKEPARAM* bpp = party->GetMemberData( i );
    // イリュージョン持ちなら対象を更新
    if( bpp->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_IRYUUZYON )
    {
      if( i < lastPokeIdx )
      {
        const BTL_POKEPARAM*               illusionTargetPoke     = party->GetMemberDataConst( lastPokeIdx );
        const pml::pokepara::PokemonParam* illusionTaragetPokeSrc = illusionTargetPoke->GetSrcData();
        const u8                           illusionTargetPokeId   = illusionTargetPoke->GetID();
        bpp->SetViewSrcData( illusionTargetPokeId, illusionTaragetPokeSrc );
        BTL_N_Printf( DBGSTR_MAIN_Illusion1st, i, bpp->GetID() );
        BTL_N_PrintfSimple( DBGSTR_MAIN_Illusion2nd, lastPokeIdx, illusionTargetPokeId );
      }
      // 自身が最後尾にいる場合、イリュージョンは無効（ただし野生ゾロアークの特殊処理は除外）
      else if( bpp->GetViewSrcData() != m_ppIllusionZoroArc )
      {
         bpp->FakeDisable();
      }
    }
  }
}


//=============================================================================================
/**
 * セットアップパラメータのステータスフラグを取得
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::GetSetupStatusFlag( BTL_STATUS_FLAG flag ) const
{
  return BATTLE_PARAM_CheckBtlStatusFlag( m_setupParam, flag );
}


//=============================================================================================
/**
 * 記録データインクリメント
 *
 * @param   wk
 * @param   recID
 */
//=============================================================================================
void MainModule::RECORDDATA_Inc( Savedata::Record::RECORD_ID recID ) const
{
  if( !IsRecordPlayMode() )
  {
    Savedata::IncRecord( recID );
  }
}
//=============================================================================================
/**
 * 記録データ加算
 *
 * @param   wk
 * @param   recID
 * @param   value
 */
//=============================================================================================
void MainModule::RECORDDATA_Add( Savedata::Record::RECORD_ID recID, u32 value ) const
{
   if( !IsRecordPlayMode() )
  {
    Savedata::AddRecord( recID , value );
  }
}

#if 0
//=============================================================================================
/**
 *
 *
 * @param   wk
 *
 * @retval  const SHOOTER_ITEM_BIT_WORK*
 */
//=============================================================================================
const SHOOTER_ITEM_BIT_WORK* BTL_MAIN_GetSetupShooterBit( const BTL_MAIN_MODULE* wk )
{
  return &(wk->setupParam->shooterBitWork);
}
#endif
//=============================================================================================
/**
 *
 *
 * @param   wk
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsShooterEnable( void ) const
{
  bool ret = false;
  if( checkBagMode(m_setupParam) == BBAG_MODE_SHOOTER )
  {
    const SHOOTER_ITEM_BIT_WORK* shooterBitWork = &(m_setupParam->shooterBitWork);
    ret = (shooterBitWork->shooter_use != 0);
  }
  return ret;
}

//=============================================================================================
/**
 * アイテム使用できるかどうか判定（使用できないモードならUI表示が暗くなり反応しない）
 *
 * @param   wk
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsItemEnable( void ) const
{
  if( checkBagMode(m_setupParam) == BBAG_MODE_SHOOTER )
  {
    return IsShooterEnable();
  }
  else
  {
    return this->IsCompetitorScenarioMode();
  }
}
//=============================================================================================
/**
 * なかよし度関連の処理を有効にするかどうか判定（ルールなどによる）
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsFriendshipActive( void ) const
{
  if( this->GetRule() ==  BTL_RULE_ROYAL )
  {
    return false;
  }

  if( this->IsCompetitorScenarioMode() )
  {
    return true;
  }
  return false;

}
//=============================================================================================
/**
 * @brief ポケモンのなかよし度を取得
 *        なかよし度が有効に働くルールでない場合、0を返す
 *        ポケモンがメガシンカしている場合、0を返す
 *        ポケモンがゲンシカイキフォルムの場合、0を返す
 *        ポケモンがウルトラバーストフォルムの場合、0を返す
 *
 * @param  bpp  なかよし度を取得したいポケモン
 */
//=============================================================================================
u8  MainModule::GetPokeFriendship( const BTL_POKEPARAM* bpp ) const
{
  if( this->IsFriendshipActive() )
  {
    if( !(bpp->IsMegaEvolved())
    &&  !(bpp->IsGensiKaiki())
    &&  !(bpp->IsUltraBurst())
    &&  (PokeIDtoClientID(bpp->GetID()) == BTL_CLIENT_PLAYER)
    ){
      return bpp->GetFriendship();
    }
  }
  return 0;
}

//=============================================================================================
/**
 * @brief 戦闘後に進化可能かどうかチェックする
 * @param poke  チェック対象ポケモン
 * @retval true  進化可能
 * @retval false 進化できない
 */
//=============================================================================================
bool MainModule::CanEvolveAfterBattle( const BTL_POKEPARAM& poke ) const
{
  // レベルアップしていない
  if( !poke.PERMFLAG_Get( BTL_POKEPARAM::PERMFLAG_LEVELUP ) )
  {
    return false;
  }

  const u8 pokeId   = poke.GetID();
  const u8 clientId = PokeIDtoClientID( pokeId );

  // 進化状況を取得
  pml::pokepara::EvolveSituation situation;
  this->GetEvolveSituation( &situation, pokeId );

  // 進化後のモンスターNo.を取得
  const System::PmlUse::NijiEvolveManager* evolveManager = System::PmlUse::GetEvolveManager();
  const pml::pokepara::PokemonParam*       pokeParam     = poke.GetSrcData();
  const PokeParty*                         party         = this->GetSrcParty( clientId, false );
  const MonsNo                             evolvedMonsno = evolveManager->GetEvolvedMonsNo_byLevelUp( pokeParam, party, &situation, NULL );

  const MonsNo beforeMonsno = static_cast<MonsNo>( poke.GetMonsNo() );
  return ( evolvedMonsno != beforeMonsno );
}

/**
 * @brief 進化状況を取得する
 * @param[out] dest    進化状況の格納先
 * @param      pokeId  進化対象ポケモンのID
 */
void MainModule::GetEvolveSituation( pml::pokepara::EvolveSituation* dest, u8 pokeId ) const
{
  // 基本は戦闘開始時に指定された状況
  *dest = m_setupParam->evolveSituation;

  // 逆さ進化対応
  dest->is_device_turned_over = this->CheckTurnedLevelUp( pokeId );
}

//=============================================================================================
/**
 * メガ進化したことを記録しておく
 *
 * @param   clientID
 */
//=============================================================================================
void  MainModule::RecMegaEvolved( u8 clientID )
{
  if( clientID < BTL_CLIENT_MAX )
  {
    m_fMegaEvolved[ clientID ] = true;
  }
}
//=============================================================================================
/**
 * 既にメガ進化しているかどうか判定
 *
 * @param   clientID
 *
 * @retval  bool
 */
//=============================================================================================
bool  MainModule::IsMegaEvolved(u8 clientID) const
{
  if( clientID < BTL_CLIENT_MAX )
  {
    return m_fMegaEvolved[ clientID ];
  }
  GFL_ASSERT(0);
  return true;
}

//=============================================================================================
/**
* ウルトラバーストしたことを記録しておく
*
* @param   clientID
*/
//=============================================================================================
void  MainModule::RecUltraBursted(const u8 clientID)
{
  if (clientID < BTL_CLIENT_MAX)
  {
    m_fUltraBursted[clientID] = true;
  }
}
//=============================================================================================
/**
* 既にウルトラバーストしているかどうか判定
*
* @param   clientID
*
* @retval  bool
*/
//=============================================================================================
bool  MainModule::IsUltraBursted(const u8 clientID) const
{
  if (clientID < BTL_CLIENT_MAX)
  {
    return m_fUltraBursted[clientID];
  }
  GFL_ASSERT(0);
  return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------

//=============================================================================================
/**
 * ポケモンレベルアップ通知をクライアントから受け取り、必要な処理を行う
 *
 * ->ゲームシステム（ビーコン）に通知
 * ->なつき度計算
 *
 * @param   wk
 */
//=============================================================================================
void MainModule::NotifyPokemonLevelup( BTL_POKEPARAM* bpp )
{
  if ( m_setupParam->competitor != BTL_COMPETITOR_COMM
    && m_setupParam->competitor != BTL_COMPETITOR_INST )
  {
    // レベルアップでなつき度上昇
    natsukiPut( bpp, PokeTool::NATSUKI_TYPE_LEVELUP );

    // ビーコン送信
    m_server->NotifyPokemonLevelUp( bpp );
  }
  else
  { //通信対戦・バトルサブウェイでレベルアップはありえない！
    GFL_ASSERT( 0 );
  }
}
//=============================================================================================
/**
 * アイテム使用によるなつき度上昇処理
 *
 * @param   wk
 * @param   bpp
 * @param   itemNo
 */
//=============================================================================================
void MainModule::CalcNatsukiItemUse( BTL_POKEPARAM* bpp, u16 itemNo ) const
{
  u8 pokeID = bpp->GetID();
  const BTL_POKEPARAM* bppServer = m_pokeconForServer->GetPokeParamConst( pokeID );
  const BTL_POKEPARAM* bppClient = m_pokeconForClient->GetPokeParamConst( pokeID );


  pml::pokepara::PokemonParam* ppServer = const_cast<pml::pokepara::PokemonParam*>(bppServer->GetSrcData());
  pml::pokepara::PokemonParam* ppClient = const_cast<pml::pokepara::PokemonParam*>(bppClient->GetSrcData());

  const BTL_FIELD_SITUATION* sit = GetFieldSituation();
  
  PokeTool::NatsukiManager::CalcWhenUseItem( ppServer, sit->place_id, itemNo, m_heapSys );
  PokeTool::NatsukiManager::CalcWhenUseItem( ppClient, sit->place_id, itemNo, m_heapSys );
}

//=============================================================================================
/**
 * ポケモン捕獲通知をクライアントから受け取り、ゲームシステム（レコード、ビーコン）に通知
 *
 * @param   wk
 * @param   bpp
 */
//=============================================================================================
void MainModule::NotifyPokemonGetToGameSystem( const BTL_POKEPARAM* bpp )
{
  if( m_server )
  {
    m_server->NotifyPokemonCapture( bpp );
  }
}

//----------------------------------------------------------------------------------------------
// 勝敗判定
//----------------------------------------------------------------------------------------------
bool MainModule::IsResultStrictMode( void ) const
{
  BtlRule rule = this->GetRule();
  BtlCompetitor competitor = this->GetCompetitor();
  return BattleRule::IsResultStrictJudge( rule, competitor );
}

BtlResult   MainModule::checkWinner( void )
{
  BtlResult result;

  BTL_N_Printf( DBGSTR_MAIN_CheckResultStart );

  // 捕獲デモ
  if( GetCompetitor() == BTL_COMPETITOR_DEMO_CAPTURE )
  {
    result = BTL_RESULT_WIN;
  }
  // 通信エラー
  else if( m_fCommError )
  {
    BTL_N_Printf( DBGSTR_MAIN_Result_CommError );
    result = BTL_RESULT_COMM_ERROR;
  }
  // 野生ポケモン捕獲
  else if( m_setupParam->capturedPokeIdx != BTL_PARTY_MEMBER_MAX )
  {
    BTL_N_Printf( DBGSTR_MAIN_Result_Capture );
    result = BTL_RESULT_CAPTURE;
  }
  // 逃げて終了
  else if( m_escapeInfo.GetCount() )
  {
    result = m_escapeInfo.CheckWinner( *this, m_myClientID, GetCompetitor() );

    // デバッグ時、Ｒボタン押しっぱなしなら逃げても勝ったことにします
    #if PM_DEBUG
    if( ui::CheckKeyCont(BUTTON_R) ){
      result = BTL_RESULT_WIN;
    }
    #endif

    // デバッグ時、Ｌボタン押しっぱなしなら逃げても負けたことにします
    #if PM_DEBUG
    if( ui::CheckKeyCont(BUTTON_L) ){
      result = BTL_RESULT_LOSE;
    }
    #endif
  }
  // 通常決着
  else
  {
    if( m_serverResult == BTL_RESULT_MAX )
    {
      // サーバ計算無効のままここに来ることは本来は有り得ない
      GFL_ASSERT(0);
      m_serverResult = BTL_RESULT_WIN;
    }
    result = m_serverResult;
    BTL_N_Printf( DBGSTR_MAIN_Result_ServerCalc, result );
  }

  m_setupParam->result = result;
  BTL_PRINT("setup params result=%d\n",m_setupParam->result);
  return result;
}


//----------------------------------------------------------------------------------------------
// 制限時間関連
//----------------------------------------------------------------------------------------------

/**
 *  コマンド選択の制限時間取得（秒）
 */
u32 MainModule::GetCommandLimitTime( void ) const
{
  return m_LimitTimeCommand;
}
/**
 * @brief クライアント毎の持ち時間[秒]を取得する
 * @note 試合開始時の持ち時間を返します。この値は変化しません。
 */
u32 MainModule::GetClientLimitTime( void ) const
{
  return m_LimitTimeClient;
}
/**
 * @brief クライアント毎の持ち時間が存在するバトルか？
 * @retval true   持ち時間が存在する
 * @retval false  持ち時間が存在しない
 */
bool MainModule::IsClientLimitTimeExist( void ) const
{
  u32 limitTime = this->GetClientLimitTime();
  return ( limitTime != 0 );
}
/**
 *  試合全体の制限時間取得（秒）
 */
u32 MainModule::GetGameLimitTime( void ) const
{
  return m_LimitTimeGame;
}
/**
 * @brief 試合全体の制限時間が存在するバトルか？
 * @retval true   制限時間が存在する
 * @retval false  制限時間が存在しない
 */
bool MainModule::IsGameLimitTimeExist( void ) const
{
  u32 limitTime = this->GetGameLimitTime();
  return ( limitTime != 0 );
}

/**
 *  試合の制限時間が終了したか判定
 */
bool MainModule::CheckGameLimitTimeOver( void ) const
{
  if( !IsRecordPlayMode() )
  {
    if( m_LimitTimeGame ){
      return m_client[m_myClientID]->IsGameTimeOver();
    }
    return false;
  }
  else
  {
    return m_client[m_myClientID]->IsGameTimeOver();
  }
}

/**
 * @brief 持ち時間オーバーで戦闘が終了したか判定
 * @retval true   持ち時間オーバーで戦闘が終了した
 * @retval false  持ち時間オーバー以外の理由で戦闘が終了した
 */
bool MainModule::CheckClientLimitTimeOver( void ) const
{
  return ( ( 0 < m_LimitTimeClient ) &&
           ( m_serverResultCause == RESULT_CAUSE_TIMEOVER ) );
}

/**
 *  @brief  ロングバージョン演出かチェック
 */
bool MainModule::IsLongEncount( void )
{
  // @attention sangoでは使っていたがnijiでは無い
  return false;
}

/**
 *  録画再生時、読み取りエラー発生チェック
 */
bool MainModule::CheckRecPlayError( void )
{
  if( IsRecordPlayMode() )
  {
    bool fError = m_client[m_myClientID]->IsRecPlayBufOver();
    if( fError ){
      m_serverResult = BTL_RESULT_DRAW;
    }
    return fError;
  }
  return false;
}

//----------------------------------------------------------------------------------------------
// バトルパラメータへ書き戻し
//----------------------------------------------------------------------------------------------

/**
 *  通信対戦相手のパーティデータを書き戻し
 */
void MainModule::Bspstore_Party( u8 clientID, const void* serialData )
{
  u8 relation = ClientIDtoRelation( m_myClientID, clientID );
  PokeParty* dstParty = m_setupParam->party[ relation ];
  GFL_PRINT("[MainModule] bspstore..clientID:%d ->relationID:%d..\n", clientID, relation);
  if( dstParty != NULL )
  {
    dstParty->DeserializeFromShortStruct( serialData );
    {
      pml::pokepara::PokemonParam* pp = dstParty->GetMemberPointer( 0 );
      GFL_PRINT("   1st pokeNumber=%d\n", pp->GetMonsNo());
    }
  }
}
/**
 *  通信対戦相手のプレイヤーデータを書き戻し
 */
void MainModule::Bspstore_PlayerStatus( u8 clientID, const Savedata::MyStatus* playerStatus )
{
  if( m_myClientID == clientID )
    return;//自分自身のセーブデータは書き換えない

  u8 relation = ClientIDtoRelation( m_myClientID, clientID );
  Savedata::MyStatus* dst = reinterpret_cast<Savedata::MyStatus*>(m_setupParam->playerStatus[ relation ]);
  if( dst != NULL){
    dst->CopyFrom( *playerStatus );
  }
}
/**
 *  AIトレーナーデータを書き戻し
 */

/**
 *  録画用操作データがあれば格納
 */
void MainModule::Bspstore_RecordData( void )
{
  if( IsRecordEnable() )
  {
    BTL_CLIENT* myClient = m_client[ m_myClientID ];
    const void* recData;
    u32 dataSize;

    recData = myClient->GetRecordData( &dataSize );
    if( recData )
    {
      BTL_Printf("録画データ %d bytes 書き戻し\n", dataSize );
      gfl2::std::MemCopy( recData, m_setupParam->recBuffer, dataSize );
      m_setupParam->recDataSize = dataSize;
      m_setupParam->recRandSeed = m_randomSeed;
    }
  }
}

//=============================================================================================
/**
 * 録画再生時のプレイヤー操作によりフェードアウト（中断orスキップのため）が始まったことを
 * 全クライアントに通知する
 */
//=============================================================================================
void MainModule::StartFadeoutForRecPlay( void )
{
  for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( IsExistClient(i) )
    {
      m_client[i]->NotifyFadeoutStartForRecPlay();
    }
  }

}


//=============================================================================================
/**
 * 録画再生リセット（パーティデータ、サーバ初期化など）
 *
 * @param   wk
 */
//=============================================================================================
void MainModule::ResetForRecPlay( u32 nextTurnNum )
{
  calc::ResetSys( m_randomSeed );
  setupCommon_srcParty( m_setupParam );

  m_pokeconForClient->Clear();
  m_pokeconForServer->Clear();

  for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( IsExistClient(i) )
    {
      this->setSrcPartyToPokecon( m_pokeconForServer, i, true );
      this->setSrcPartyToPokecon( m_pokeconForClient, i, false );

      m_client[i]->SetChapterSkip( nextTurnNum );
    }
  }
/*
  GFL_DELETE  (m_viewCore);

  #if BTL_OLD_IGNORE
  BTLV_ResetSystem( HEAPID_BTL_VIEW );
  #endif

//構築した描画システムに情報を引き渡す処理が必要
  BTLV_INIT_PARAM initParam;
  initParam.mMainModule = this;
  initParam.mClient = m_client[m_setupParam->commPos];
  initParam.mPokeCon = m_pokeconForClient;
  initParam.mBagMode = checkBagMode( m_setupParam );
  m_viewCore = GFL_NEW(m_heapView) BattleViewSystem(&initParam);
  m_client[m_setupParam->commPos]->AttachViewCore( m_viewCore );
*/
  // Server 始動
  m_server->Startup();

}
void MainModule::NotifyChapterSkipEnd( void )
{
  for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( IsExistClient(i) )
    {
      m_client[i]->StopChapterSkip();
    }
  }
}

ServerFlow*  MainModule::GetSVFWorkForAI( void )
{
  GFL_ASSERT(m_server);
  return m_server->GetFlowWork();
}


//-----------------------------------------------------------------------------
/**
 * @brief クライアントを取得する
 * @param clientID  取得するクライアントのID
 * @retval NULL  指定したクライアントが存在しない場合
 */
//-----------------------------------------------------------------------------
const BTL_CLIENT* MainModule::GetClient( u8 clientID ) const
{
  if( !this->IsExistClient( clientID ) ) {
    return NULL;
  }
  return m_client[ clientID ];
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定したポケモンの所属するクライアントを取得する
 * @param pokeID  ポケモンID
 * @retval NULL  指定したクライアントが存在しない場合
 */
//-----------------------------------------------------------------------------
const BTL_CLIENT* MainModule::GetClientByPokeID( u8 pokeID ) const
{
  u8 clientId = this->PokeIDtoClientID( pokeID );
  return this->GetClient( clientId );
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定したクライアントの全力技使用状況を取得する
 * @param clientID  取得対象クライアントのID
 * @retval NULL  指定したクライアントが存在しない場合
 */
//-----------------------------------------------------------------------------
ZenryokuWazaStatus* MainModule::GetZenryokuWazaStatus( u8 clientID ) const
{
  if( !this->IsExistClient( clientID ) )
  {
    return NULL;
  }
  return m_clientZenryokuWazaStatus[ clientID ];
}

//----------------------------------------------------------------------------------------------
// バトル検定データ処理
//----------------------------------------------------------------------------------------------
void MainModule::Kentei_ClearField( BATTLE_SETUP_PARAM* sp )
{
  BATTLE_KENTEI_RESULT* result = &(m_setupParam->kenteiResult);

  result->TurnNum = 0;       //かかったターン数
  result->PokeChgNum = 0;    //交代回数

  result->VoidAtcNum = 0;    //効果がない技を出した回数
  result->WeakAtcNum = 0;    //ばつぐんの技を出した回数
  result->ResistAtcNum = 0;  //いまひとつの技を出した回数
  result->VoidNum = 0;       //効果がない技を受けた回数
  result->ResistNum = 0;     //いまひとつの技を受けた回数

  result->WinTrainerNum = 0; //倒したトレーナー数
  result->WinPokeNum = 0;    //倒したポケモン数
  result->LosePokeNum = 0;   //倒されたポケモン数
  result->UseWazaNum = 0;    //使用した技の数
  result->HPSum = 0;
}
// レストランデータ格納
void MainModule::Bspstore_KenteiData( void )
{
  if( this->GetCommMode() != BTL_COMM_NONE ){
    return;
  }

  if( !m_server ){
    return;
  }

  ServerFlow* sv_flow = m_server->GetFlowWork();
  if( !sv_flow ){
    return;
  }

  BATTLE_KENTEI_RESULT* result = &(m_setupParam->kenteiResult);

  result->TurnNum    = sv_flow->Hnd_GetTurnCount();
  result->PokeChgNum = sv_flow->Hnd_GetPokeChangeCount( BTL_CLIENT_PLAYER );

  {
    const AffCounter& afcnt = sv_flow->GetAffCounter();
    result->VoidAtcNum   = afcnt.GetVoid();
    result->WeakAtcNum   = afcnt.GetAdvantage();
    result->ResistAtcNum = afcnt.GetDisadvantage();
    result->VoidNum      = afcnt.GetRecvVoid();
    result->ResistNum    = afcnt.GetRecvDisadvantage();
  }

  if( m_setupParam->result == BTL_RESULT_WIN ){
    result->WinTrainerNum = 1;
  }

  {
    BTL_PARTY* party = m_pokeconForServer->GetPartyData( BTL_CLIENT_PLAYER );
    u32 memberCnt = party->GetMemberCount();
    for(u32 i=0; i<memberCnt; ++i)
    {
      BTL_POKEPARAM* bpp = party->GetMemberData( i );
      result->UseWazaNum += bpp->WAZA_GetUsedCount();
      result->HPSum      += bpp->GetValue( BTL_POKEPARAM::BPP_HP );
    }
    result->LosePokeNum = sv_flow->GetDeadPokeCountByClientID( BTL_CLIENT_PLAYER );

    party = m_pokeconForServer->GetPartyData( BTL_CLIENT_ENEMY1 );
    result->WinPokeNum = party->GetDeadMemberCount();

    if( IsExistClient(BTL_CLIENT_ENEMY2) )
    {
      party = m_pokeconForServer->GetPartyData( BTL_CLIENT_ENEMY2 );
      if( party )
      {
        result->WinPokeNum += party->GetDeadMemberCount();
      }
    }
  }
  //検定データの「効果がバツグンを当てた回数」をレコードに反映する
  RECORDDATA_Add( Savedata::Record::RECID_WEAK_ATTACK, result->WeakAtcNum );

  BTL_PRINT("**** Kentei Result ****\n");
  BTL_PRINT("TurnNum       = %d\n", result->TurnNum );       // かかったターン数
  BTL_PRINT("HPSum         = %d\n", result->HPSum );         // 生き残りポケモンのHP総和
  BTL_PRINT("PokeChgNum    = %d\n", result->PokeChgNum );    // 交代回数
  BTL_PRINT("VoidAtcNum    = %d\n", result->VoidAtcNum );    // 効果がない技を出した回数
  BTL_PRINT("WeakAtcNum    = %d\n", result->WeakAtcNum );    // ばつぐんの技を出した回数
  BTL_PRINT("ResistAtcNum  = %d\n", result->ResistAtcNum );  // いまひとつの技を出した回数
  BTL_PRINT("VoidNum       = %d\n", result->VoidNum );       // 効果がない技を受けた回数
  BTL_PRINT("ResistNum     = %d\n", result->ResistNum );     // いまひとつの技を受けた回数
  BTL_PRINT("WinTrainerNum = %d\n", result->WinTrainerNum ); // 倒したトレーナー数
  BTL_PRINT("WinPokeNum    = %d\n", result->WinPokeNum );    // 倒したポケモン数
  BTL_PRINT("LosePokeNum   = %d\n", result->LosePokeNum );   // 倒されたポケモン数
  BTL_PRINT("UseWazaNum    = %d\n", result->UseWazaNum );    // 使用した技の数

}



//----------------------------------------------------------------------------------------------
// トレーナーパラメータ関連
//----------------------------------------------------------------------------------------------

void MainModule::trainerParam_Init( void )
{
  u32 i;

  static const TRAINER_DATA initData =
  {
    NULL,                     //Savedata::MyStatus*   playerStatus;

    NULL,                     //STRBUF*     name;
    NULL,                     //STRBUF*     trtype_name;
    NULL,                     //STRBUF*     win_msg;
    NULL,                     //STRBUF*     lose_msg;

    TRID_NULL,                //u16         trainerID;
    0,                        //u16         trainerType;
    trainer::TRTYPE_GRP_NORA, //u8          trainerGroup;
    PM_MALE,                  //u8          trainerSex;
    0,                        //u8          trainerGold;
    0,                        //u8          pad1;
    0,                        //u32         ai_bit;
    { ITEM_DUMMY_DATA, ITEM_DUMMY_DATA, ITEM_DUMMY_DATA, ITEM_DUMMY_DATA }
                              //u16         useItem[ BSP_TRAINER_DATA::USE_ITEM_MAX ];
  };

  for(i=0; i<GFL_NELEMS(m_trainerParam); ++i)
  {
    m_trainerParam[i] = initData;
  }
  for (i = 0; i < GFL_NELEMS(m_fesTrainerParam); ++i)
  {
    m_fesTrainerParam[i] = initData;
  }
}


void MainModule::trainerParam_Clear( void )
{
  for (u32 i = 0; i < GFL_NELEMS(m_trainerParam); ++i)
  {
    trainerParam_ClearCore(&m_trainerParam[i]);
  }
  for (u32 i = 0; i < GFL_NELEMS(m_fesTrainerParam); ++i)
  {
    trainerParam_ClearCore(&m_fesTrainerParam[i]);
  }
}

void MainModule::trainerParam_ClearCore(TRAINER_DATA *data)
{
  GFL_SAFE_DELETE(data->playerStatus);
  GFL_SAFE_DELETE(data->win_msg);
  GFL_SAFE_DELETE(data->lose_msg);
  GFL_SAFE_DELETE(data->name);
  GFL_SAFE_DELETE(data->trtype_name);
}


/** プレイヤーデータからセットアップ */
void MainModule::trainerParam_StorePlayer( TRAINER_DATA* dst, HeapHandle heapHandle, const Savedata::MyStatus* playerData )
{
  dst->playerStatus = GFL_NEW(heapHandle) Savedata::MyStatus();
  dst->playerStatus->CopyFrom( *playerData );

  trainerParam_StoreCore( dst, heapHandle );
}
void MainModule::trainerParam_StoreSelialData( TRAINER_DATA* dst, HeapHandle heapHandle, const void* serialData )
{
  dst->playerStatus = GFL_NEW(heapHandle) Savedata::MyStatus();
  dst->playerStatus->Deserialize( serialData );

  // @fix MMCat[337]：相手がnijiの場合フラグをたてる。
  if( dst->playerStatus->GetRomCode() == VERSION_SUN ||
      dst->playerStatus->GetRomCode() == VERSION_MOON )
  {
    dst->playerStatus->SetDressUpParamRomVersionNiji();
  }

  trainerParam_StoreCore( dst, heapHandle );
}
void MainModule::trainerParam_StoreCore( TRAINER_DATA* dst, HeapHandle heapHandle )
{
  dst->trainerID    = TRID_NULL;
  dst->trainerSex   = dst->playerStatus->GetSex();

  dst->trainerType  = (dst->trainerSex == pml::SEX_MALE)? trainer::TRTYPE_HERO : trainer::TRTYPE_HEROINE;
  dst->name         = GFL_NEW(m_heapSys) gfl2::str::StrBuf( System::STRLEN_PERSON_NAME + System::EOM_LEN, m_heapSys );
  dst->playerStatus->GetNameString( dst->name );

  dst->win_msg      = NULL;
  dst->lose_msg     = NULL;
  dst->ai_bit = 0;

  dst->grammer_attr = 0;
  dst->grammer_isPlural = false;
}
/** NPC プレイヤーからセットアップ */
void MainModule::trainerParam_StoreNPCTrainer( TRAINER_DATA* dst, const BSP_TRAINER_DATA* trData )
{
  dst->playerStatus = NULL;

  if( trData )
  {
    dst->trainerID    = trData->GetTrainerID();
    dst->trainerType  = trData->GetTrainerType();
    dst->trainerSex   = trData->GetTrainerSex();
    dst->trainerGroup = trData->GetTrainerGroup();
    dst->trainerGold  = trData->GetGoldParam();
    dst->ai_bit       = trData->GetAIBit();
    dst->grammer_attr = trData->GetTrTypeGrammarAttr();
    dst->grammer_isPlural = trData->GetTrTypePlural();
    dst->name         = GFL_NEW(m_heapSys) gfl2::str::StrBuf( *(trData->GetName()), m_heapSys );
    dst->trtype_name  = GFL_NEW(m_heapSys) gfl2::str::StrBuf( *(trData->GetTrTypeName()), m_heapSys );
    dst->win_msg      = GFL_NEW(m_heapSys) gfl2::str::StrBuf( BTL_STRBUF_STD_LENGTH, m_heapSys );
    dst->lose_msg     = GFL_NEW(m_heapSys) gfl2::str::StrBuf( BTL_STRBUF_STD_LENGTH, m_heapSys );

    if( trData->HasMsgArcId() ){
      gfl2::str::MsgData msg_data( print::GetMessageArcId(), trData->GetMsgArcId(), m_heapSys->GetLowerHandle() );
      msg_data.GetString( trData->GetMsgWinId(), *(dst->win_msg) );
      msg_data.GetString( trData->GetMsgLoseId(), *(dst->lose_msg) );
    }

    for (int i=0; i < BSP_TRAINER_DATA::USE_ITEM_MAX; ++ i ){
      dst->useItem[i] = trData->GetUseItem( i );
    }
  }
  else {
    GFL_ASSERT( 0 );  //データが来なかった場合、Assert。
  }
}
/**
 *  録画再生時用のトレーナーパラメータセット
 */
void MainModule::trainerParam_SetupForRecPlay( u8 clientID )
{
  const BATTLE_SETUP_PARAM* sp = m_setupParam;
  TRAINER_DATA* trParam;

  u8 relation_clientID = ClientIDtoRelation( m_myClientID, clientID );

  trParam = &(m_trainerParam[clientID]);

  if( sp->playerStatus[relation_clientID] != NULL )
  {
    trainerParam_StorePlayer( trParam, m_heapSys, sp->playerStatus[relation_clientID] );
  }
  else if( sp->tr_data[relation_clientID] != NULL )
  {
    trainerParam_StoreNPCTrainer( trParam, sp->tr_data[relation_clientID] );
  }
}

void MainModule::trainerParam_StoreCommNPCTrainer( TRAINER_DATA* dst, const net::TRAINER_SEND_DATA* trSendData )
{
  dst->playerStatus = NULL;

  if( trSendData )
  {
    BSP_TRAINER_DATA * trData = GFL_NEW_LOW(m_heapSys) BSP_TRAINER_DATA( m_heapSys );
    if( trData ){
      trData->Deserialize( &trSendData->base_data );
      trainerParam_StoreNPCTrainer( dst, trData );
      GFL_DELETE trData;
    }
  }
  else{
    GFL_ASSERT( 0 );  //データが来なかった場合、Assert。
  }
}
/**
 * 指定トレーナーIDに有効なデータが設定されているか
 * @param trData
 * @return 有効なデータ（プレイヤーorNPC）が設定されていたら true
 */
bool MainModule::trainerParam_IsExist( const TRAINER_DATA* trData ) const
{
  if( trData->name == NULL )
  {
    return false;
  }
  if( trData->name->GetLength() <= 0 )
  {
    return false;
  }
  return true;
}

/**
 * 指定クライアントIDにトレーナーが居るかどうか判定（野生などでfalseが返る）
 * @param clientID  クライアントID
 * @return  clientID にトレーナー（プレイヤーorNPC）がいれば true / 野生などで false
 */
bool MainModule::IsClientTrainerExist( u8 clientID ) const
{
  if( clientID < GFL_NELEMS(m_trainerParam) ){
    return trainerParam_IsExist( &(m_trainerParam[clientID]) );
  }
  GFL_ASSERT(0);
  return false;
}

bool MainModule::IsClientNPC( u8 clientID ) const
{
  bool isNpc = ( m_trainerParam[clientID].playerStatus == NULL );
  return isNpc;
}

u16 MainModule::GetClientUseItem( u8 clientID, u8 itemIdx ) const
{
  if( IsClientNPC(clientID) )
  {
    const TRAINER_DATA* trData = &m_trainerParam[clientID];
    if( itemIdx < GFL_NELEMS(trData->useItem) ){
      return trData->useItem[ itemIdx ];
    }
  }
  return ITEM_DUMMY_DATA;
}
u32 MainModule::GetClientAIBit( u8 clientID ) const
{
  if( GetRule() == BTL_RULE_ROYAL )
  {
    return BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_EXPERT | BTL_AISCRIPT_BIT_WAZA_ROYAL | BTL_AISCRIPT_BIT_POKECHANGE_BASIC;
  }

  // ウルトラビースト戦
  if( GetSetupStatusFlag( BTL_STATUS_FLAG_VS_ULTRA_BEAST ) )
  {
    return BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_STRONG | BTL_AISCRIPT_BIT_WAZA_EXPERT;
  }

  if( GetCompetitor() == BTL_COMPETITOR_WILD )
  {
    if( GetSetupStatusFlag(BTL_STATUS_FLAG_VS_NUSI) || GetSetupStatusFlag(BTL_STATUS_FLAG_VS_NEKUROZUMA) )
    {
      return BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_STRONG | BTL_AISCRIPT_BIT_WAZA_EXPERT;
    }
    if( GetRule() == BTL_RULE_DOUBLE )
    {
      return BTL_AISCRIPT_BIT_WAZA_DOUBLE;
    }
  }
  else
  {
    if( !IsRecordFightMode() )
    {
      u16 ai_bit = m_trainerParam[ clientID ].ai_bit;

      // @fix GFNMCat[5647] シングルトレーナーにダブルＡＩが設定されている
      // シングルなのにダブルAIが設定されていたら、ダブルAIを解除する
      if( ( GetRule() == BTL_RULE_SINGLE ) && ( ( ai_bit & BTL_AISCRIPT_BIT_WAZA_DOUBLE ) != 0 ) )
      {
        GFL_ASSERT(0);
        ai_bit = ( ai_bit ) & ( ~BTL_AISCRIPT_BIT_WAZA_DOUBLE );
      }

      return ai_bit;
    }
    else
    {
      u32 ai_bit = BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_STRONG | BTL_AISCRIPT_BIT_WAZA_EXPERT | BTL_AISCRIPT_BIT_POKECHANGE_BASIC;
      BtlRule rule = this->GetRule();
      if( rule == BTL_RULE_DOUBLE )
      {
        ai_bit |= BTL_AISCRIPT_BIT_WAZA_DOUBLE;
      }
      return ai_bit;
    }
  }
  return 0;
}

const STRBUF* MainModule::GetClientTrainerName( u8 clientID,print::GrammaticalAttribute * grammer ) const
{
  grammer->Raw = 0;
  //最初の音について： トレーナー名に冠詞がつく状況はない。フランス語のdeは変化するがバトルでは使っていない。
  grammer->InitialSound   = 0;
  //文法性はトレーナータイプのものを反映すれば正しい
  grammer->Grammer        = m_trainerParam[clientID].trainerSex;
  //常に加算
  grammer->IsUncountable  = 0;
  grammer->IsAlwaysPlural = m_trainerParam[clientID].grammer_isPlural;

  return m_trainerParam[clientID].name;
}

const STRBUF* MainModule::GetClientTrTypeName( u8 clientID, print::GrammaticalAttribute * grammer ) const
{
  grammer->Raw = m_trainerParam[clientID].grammer_attr;
  return m_trainerParam[clientID].trtype_name;
}

// string x 2

u32 MainModule::GetClientTrainerID( u8 clientID ) const
{
  if( IsClientNPC(clientID) ){
    return m_trainerParam[clientID].trainerID;
  }else{
    return TRID_NULL;
  }
}
// scu x 2
u16 MainModule::GetClientTrainerType( u8 clientID ) const
{
  const TRAINER_DATA* trData = &m_trainerParam[ clientID ];
  return trData->trainerType;
}
trainer::TrTypeGroup MainModule::GetClientTrainerGroup( u8 clientID ) const
{
  const TRAINER_DATA* trData = &m_trainerParam[ clientID ];
  return (trainer::TrTypeGroup)(trData->trainerGroup);
}
u8 MainModule::GetClientTrainerGold( u8 clientID ) const
{
  const TRAINER_DATA* trData = &m_trainerParam[ clientID ];
  return trData->trainerGold;
}
u16 MainModule::GetClientTrainerMegaItemId( u8 clientID ) const
{
  const TRAINER_DATA* trData = &m_trainerParam[ clientID ];
  return trainer::TrainerTypeData::GetMegaItemId( static_cast<trainer::TrType>(trData->trainerType) );
}

const gfl2::str::StrBuf* MainModule::GetClientTrainerMsg( u8 clientID, BtlResult playerResult ) const
{
  if( (GetCompetitor() == BTL_COMPETITOR_INST)
  &&  IsClientNPC(clientID)
  ){
    if( playerResult == BTL_RESULT_WIN ){
      return m_trainerParam[ clientID ].lose_msg;;
    }
    else{
      return m_trainerParam[ clientID ].win_msg;
    }
  }
  return NULL;

}
const Savedata::MyStatus* MainModule::GetClientPlayerData( u8 clientID ) const
{
  if( !IsClientNPC(clientID) )
  {
    return m_trainerParam[ clientID ].playerStatus;
  }
  GFL_ASSERT_MSG(0, "This Client(%d) is not Human Player\n", clientID);
  return NULL;
}

//バトルフェス用の取得
const Savedata::MyStatus* MainModule::GetBattleFesPlayerData(u8 idx) const
{
  return m_fesTrainerParam[idx].playerStatus;
}

u16 MainModule::GetBattleFesPlayerWinMsgId(u8 idx) const
{
  u8 tempIdx = idx;
  if (idx != 0)
  {
    tempIdx -= 1;
  }
  return m_setupParam->tr_data[tempIdx]->GetMsgWinId();
}

/**
 * @brief 指定したクライアントのレーティングを取得する
 * @param clientID  クライアントID
 *
 * @attention 通信対戦でのみ有効です
 */
u16 MainModule::GetClientRating( u8 clientID ) const
{
  if( ( m_setupParam->commMode == BTL_COMM_NONE ) ||
      ( GFL_NELEMS( m_setupParam->playerRating ) <= clientID ) )
  {
#if PM_DEBUG
    if( m_setupParam->isLiveRecPlayMode == false )
#endif
    {
      GFL_ASSERT(0);
    }
    
    return 0;
  }
  return m_setupParam->playerRating[ clientID ];
}

/**
 * @brief 指定したクライアントが、他クライアントに対して公開済みの情報を取得する
 * @param clientID  クライアントID
 * @retval NULL 指定したクライアントが存在しない場合
 */
ClientPublicInformation* MainModule::GetClientPublicInformation( u8 clientID )
{
  if( !( this->IsExistClient( clientID ) ) )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_clientPublicInformation[ clientID ];
}

/**
 * @brief 指定したクライアントが、他クライアントに対して公開済みの情報を取得する
 * @param clientID  クライアントID
 * @retval NULL 指定したクライアントが存在しない場合
 */
const ClientPublicInformation* MainModule::GetClientPublicInformation( u8 clientID ) const
{
  if( !( this->IsExistClient( clientID ) ) )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_clientPublicInformation[ clientID ];
}

//=============================================================================================
/**
 * 録画データ再生モードかチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsRecordPlayMode( void ) const
{
  return m_setupParam->recordDataMode == BTL_RECORDDATA_PLAY;
}
//=============================================================================================
/**
 * 録画対戦（模擬戦）モードかチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsRecordFightMode( void ) const
{
  return m_setupParam->recordDataMode == BTL_RECORDDATA_FIGHT;
}
//=============================================================================================
/**
 * 録画対戦（模擬戦）モード時、相手がNPC（施設トレーナー）データか否かを判定
 * @retval  bool
 */
//=============================================================================================
bool MainModule::IsRecordFightWithNPCData( void ) const
{
  if( this->IsRecordFightMode() )
  {
    if( m_setupParam->playerStatus[ BTL_CLIENT_ENEMY1 ] == NULL ){
      BTL_PRINT("[BtlMain] クライアント１番が NULL なので NPC 対戦です\n");
      return true;
    }
    BTL_PRINT("[BtlMain] クライアント１番が NULL ではないので 対人対戦です\n");
    return false;
  }
  return false;
}

//=============================================================================================
/**
 * 自分がサーバマシンか判定
 *
 * @retval  bool
 */
//=============================================================================================
bool MainModule::CheckImServerMachine( void ) const
{
  return net::CheckImServer();
}
//=============================================================================================
/**
 *  プレイヤーが「がくしゅうそうち」を有効にしているかどうか判定
 */
//=============================================================================================
bool MainModule::HasPlayerGakusyuSouti( void ) const
{
  return m_setupParam->bGakusyuSouti;
}

//----------------------------------------------------------------------------------------------
// 位置ID、サイドIDなど
//----------------------------------------------------------------------------------------------
//=============================================================================================
/**
 * クライアントIDからポケモン戦闘位置を返す
 *
 * @param   wk
 * @param   basePos
 * @param   idx
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos MainModule::GetClientPokePos( u8 clientID, u8 posIdx ) const
{
  u8 i;

  for(i=0; i<BTL_POS_NUM; ++i)
  {
    BTL_CLIENT_ID posCoverClientId = this->GetPosCoverClientId( static_cast<BtlPokePos>(i) );
    if( posCoverClientId == clientID )
    {
      if( posIdx == 0 )
      {
        return i;
      }
      --posIdx;
    }
  }
//  GFL_ASSERT(0);
  return BTL_POS_NUM;
}





//----------------------------------------------------------------------------------------------
// オリジナルパーティデータ管理
//----------------------------------------------------------------------------------------------
// 管理ワーク初期化
void MainModule::srcParty_Create( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_srcParty); ++i)
  {
    m_srcParty[i] = GFL_NEW(m_heapSys) PokeParty( m_heapSys );
    m_srcPartyForServer[i] = GFL_NEW(m_heapSys) PokeParty( m_heapSys );
  }
  m_tmpParty = GFL_NEW(m_heapSys) PokeParty( m_heapSys );

}

// 管理ワーククリア
void MainModule::srcParty_Delete( void )
{
  if( m_tmpParty )
  {
    GFL_DELETE  m_tmpParty;
    m_tmpParty = NULL;
  }
  for(u32 i=0; i<GFL_NELEMS(m_srcParty); ++i)
  {
    if( m_srcParty[i] ){
      GFL_DELETE m_srcParty[i];
      m_srcParty[i] = NULL;
    }
    if( m_srcPartyForServer[i] ){
      GFL_DELETE m_srcPartyForServer[i];
      m_srcPartyForServer[i] = NULL;
    }
  }
}
//------------------------------------------------
// パーティデータ領域確保
//------------------------------------------------
// PokeParty 直接
void MainModule::srcParty_Set( u8 clientID, const PokeParty* party )
{
  if( m_srcParty[clientID] != NULL )
  {
    m_srcParty[clientID]->CopyFrom( *party );
    m_srcPartyForServer[clientID]->CopyFrom( *party );

    srcParty_FormChangeForX( m_srcParty[clientID] );
    srcParty_FormChangeForX( m_srcPartyForServer[clientID] );
  }
}
// シリアライズデータ
void MainModule::srcParty_SetSerial( u8 clientID, const void* data )
{
  if( data )
  {
    if( m_srcParty[clientID] != NULL )
    {
      m_srcParty[clientID]->DeserializeFromShortStruct( data );
      m_srcPartyForServer[clientID]->DeserializeFromShortStruct( data );

      GFL_PRINT("m_srcParty[%d]monsno[%d]\n",clientID,m_srcParty[clientID]->GetMemberPointer(0)->GetMonsNo());

      srcParty_FormChangeForX( m_srcParty[clientID] );
      srcParty_FormChangeForX( m_srcPartyForServer[clientID] );
    }
  }
}

/**
 * @brief 戦闘開始時のフォルムチェンジ
 * @param party  操作対象のパーティ
 */
void MainModule::srcParty_FormChangeForX( PokeParty* party )
{
  u32 memberCount = party->GetMemberCount();
  for( u32 i=0; i<memberCount; ++i )
  {
    pml::pokepara::PokemonParam* pp = party->GetMemberPointer( i );
    srcParty_FromChange_OnBattleStart( pp );
  }
}

/**
 * @brief 戦闘開始時に、条件を満たしているならフォルムチェンジさせる
 * @param pokeParam  フォルムチェンジさせるポケモン
 */
void MainModule::srcParty_FromChange_OnBattleStart( pml::pokepara::PokemonParam* pokeParam )
{
  const MonsNo monsno = pokeParam->GetMonsNo();

  // ゼルネアス
  if( monsno == MONSNO_ZERUNEASU )
  {
    pokeParam->ChangeFormNo( FORMNO_ZERUNEASU_RADIATION );
  }
}

/**
 * @brief 未発見ウルトラビーストのニックネームをセットアップする
 */
void MainModule::setupUnknownUBNickName( PokeParty* party ) const
{
  u32 memberCount = party->GetMemberCount();
  for( u32 i=0; i<memberCount; ++i )
  {
    pml::pokepara::PokemonParam* pp = party->GetMemberPointer( i );
    const MonsNo monsno = pp->GetMonsNo();

    // 必要なら、ニックネームを未発見の状態にする
    if( UltraBeast::IsUnknownNamePokemon( *this, monsno ) )
    {
      str::SetUnknownUBNickName( m_heapSys, pp );
    }
  }
}

// パーティデータ取得
PokeParty* MainModule::GetSrcParty( u8 clientID, bool fForServer ) const
{
  GFL_ASSERT( m_srcParty[clientID] );

  if( fForServer )
  {
    return m_srcPartyForServer[ clientID ];

  }else{
    return m_srcParty[ clientID ];
  }
}

/**
 * @brief 指定パーティのセットアップパラメータを取得する
 * @param 対象パーティのクライアントID
 */
const PartyParam* MainModule::GetPartySetupParam( u8 clientID ) const
{
  GFL_ASSERT_STOP( clientID < BTL_CLIENT_NUM );
  return &( m_setupParam->partyParam[ clientID ] );
}


/*--------------------------------------------------------------------------*/
/* static functions                                                         */
/*--------------------------------------------------------------------------*/

/**
 *  ClientID -> BasePokeID
 */
u8 MainModule::GetClientBasePokeID( u8 clientID )
{
  return PokeID::GetClientBasePokeId( static_cast<BTL_CLIENT_ID>( clientID ) );
}

/**
 *  PokeID -> ClientID
 */
u8 MainModule::PokeIDtoClientID( u8 pokeID )
{
  return PokeID::PokeIdToClientId( pokeID );
}

/**
 * PokeID -> ShortPokeID(3bit)
 */
u8  MainModule::PokeIDtoShortID( u8 pokeID )
{
  return PokeID::PokeIdToShortId( pokeID );
}
/**
 * ClientID & ShortPokeID -> PokeID
 */
u8  MainModule::ShortIDtoPokeID( u8 clientID, u8 shortID )
{
  return PokeID::ShortIdToPokeId( static_cast<BTL_CLIENT_ID>( clientID ), shortID );
}


/**
 *  位置ID->真正面の相手にあたる位置ID
 */
BtlPokePos  MainModule::GetFacedPokePos( BtlRule rule, BtlPokePos pos )
{
  return PokemonPosition::GetFacedPokePos( rule, pos );
}

/**
 *  自分または味方（マルチ時）同士のクライアントIDか判定
 */
bool  MainModule::IsFriendClientID( u8 clientID_1, u8 clientID_2 ) const
{
  if( clientID_1 == clientID_2 )
  {
    return true;
  }

  const BtlRule rule = this->GetRule();
  if( rule == BTL_RULE_ROYAL )
  {
    return false;
  }

  if( clientID_1 == (getFriendCrientID(clientID_2)) )
  {
    return true;
  }

  return false;
}
/**
 *  基準位置ID、Idx から味方位置ID を取得
 */
BtlPokePos MainModule::GetFriendPokePos( BtlRule rule, BtlPokePos basePos, u8 idx )
{
  return PokemonPosition::GetFriendPokePos( rule, basePos, idx );
}

/**
 * @brief 相手側のポケモン位置ID取得
 * @param rule      バトルルール
 * @param basePos   自分側の位置
 * @param idx       相手側の何番目の位置を取得するか？
 */
BtlPokePos  MainModule::GetOpponentPokePosByRule( BtlRule rule, BtlPokePos basePos, u8 idx )
{
  return PokemonPosition::GetOpponentPokePos( rule, basePos, idx );
}
BtlPokePos  MainModule::GetSidePos( BtlRule rule, BtlSide side, u8 idx )
{
  return PokemonPosition::GetSidePos( rule, side, idx );
}
BtlSide  MainModule::PosToSide( BtlRule rule, BtlPokePos pos )
{
  return PokemonPosition::PosToSide( rule, pos );
}


/**
 * @breif 大会用演出データ取得
 * @param id  clientId
 * @return  大会用演出データ    ないときNULL
 */
BATTLE_CONVENTION_INFO* MainModule::GetBattleConventionInfo( u8 id ) const
{
  if( id < 2){
    return &m_setupParam->conventionInfo[ id ];
  }
  GFL_ASSERT_MSG(0,"%d", id );
  return NULL;
}

//----------------------------------------------------------------------------------------------
// PGLデータ集計関連
//----------------------------------------------------------------------------------------------
/**
 * バトル結果用にPGLデータを格納
 * @param bpp         倒されたポケモンパラメータ（必須）
 * @param recParam    倒したポケモンとそのワザパラメータ（取得しきれないケースではNULL）
 */
void MainModule::NotifyPGLRecord( const BTL_POKEPARAM* bpp, const PGLRecParam* recParam ) const
{
  if( GetCompetitor() == BTL_COMPETITOR_COMM )
  {
    u8 pokeID   = bpp->GetID();
    u8 pokeIdx = 0;

    bool  bPlayersSide = pokeID_to_PGL_RecPos( pokeID, &pokeIdx );


    BATTLE_PGL_RECORD*  rec = (bPlayersSide)?
            const_cast<BATTLE_PGL_RECORD*>(m_setupParam->PGL_Record.myParty)
          : const_cast<BATTLE_PGL_RECORD*>(m_setupParam->PGL_Record.enemysParty);

    rec[pokeIdx].deadPokeNumber = bpp->GetSrcData()->GetMonsNo();
    rec[pokeIdx].deadPokeForm   = bpp->GetSrcData()->GetFormNo();

    BTL_PRINT("[PGL] recPokeIdx:%d, isDeadPoke players'side?:%d, deadMonsNo=%d\n",
            pokeIdx, bPlayersSide, rec[pokeIdx].deadPokeNumber  );

    if( (recParam != NULL)
    &&  (!IsFriendPokeID(pokeID, recParam->attacker->GetID()))
    ){
      rec[pokeIdx].attackPokeNumber = recParam->attacker->GetSrcData()->GetMonsNo();
      rec[pokeIdx].attackPokeForm   = recParam->attacker->GetSrcData()->GetFormNo();
      rec[pokeIdx].finishWazaNumber = recParam->wazaID;
      BTL_PRINT("[PGL] normal record atkMonsNo=%d, wazaNo=%d\n",
              rec[pokeIdx].attackPokeNumber, rec[pokeIdx].finishWazaNumber);
    }
    else
    {
     rec[pokeIdx].attackPokeNumber = MONSNO_NULL;
     rec[pokeIdx].attackPokeForm   = 0;
     rec[pokeIdx].finishWazaNumber = WAZANO_NULL;
     BTL_PRINT("[PGL] uncomplete record\n");
    }
  }
}
/**
 * PGLデータの記録位置を計算
 *
 * @param       pokeID    対象ポケモンID
 * @param[out]  pokeIdx   記録位置インデックス（0〜5（BTL_PARTY_MEMBER_MAX-1)）
 *
 * @retval   bool  味方側パーティであれば true
 */
bool MainModule::pokeID_to_PGL_RecPos( u8 pokeID, u8* pokeIdx ) const
{
  u8 clientID = PokeIDtoClientID( pokeID );
  *pokeIdx = 0;


  int  diffIdx = (int)pokeID - (int)(GetClientBasePokeID(clientID));

  BTL_PRINT("[pgl] diedPokeID:%d -> clientID:%d, idx:%d\n", pokeID, clientID, diffIdx);

  if( diffIdx < 0 ){
    GFL_ASSERT(0);
    diffIdx = 0;
  }

  bool bPlayersSide = IsFriendClientID( clientID, GetPlayerClientID() );
  if( bPlayersSide )
  {
    BTL_PRINT("  this pokemon is player's side ");
    if( clientID != GetPlayerClientID() ){
      BTL_PRINT("  but not player's pokemon");
      *pokeIdx = 3;
    }
    (*pokeIdx) += diffIdx;
  }
  else
  {
    BTL_PRINT("  this pokemon is enemy's side ");
    if( clientID >= 2 ){
      *pokeIdx = 3;
    }
    (*pokeIdx) += diffIdx;
  }
  BTL_PRINT(" last idx=%d\n", (*pokeIdx));

  if( *pokeIdx >= BTL_PARTY_MEMBER_MAX )
  {
    // ここに来るのはおかしい。マルチで４体以上のポケモンを持ってきているなど、
    // １チームに６体以上のポケモンが存在している可能性がある
    GFL_ASSERT_MSG(0, "clientID=%d, pokeID=%d, pokeIdx=%d", clientID, pokeID, *pokeIdx);
    *pokeIdx = (BTL_PARTY_MEMBER_MAX - 1);
  }

  return bPlayersSide;
}

//@バトル変更箇所
//----------------------------------------------------------------------------------------------
// sango用テレビナビデータ関連
//----------------------------------------------------------------------------------------------
//先頭ポケモンの記録
void MainModule::SetTvNaviData_FrontPoke( const BTL_POKEPARAM* bpp1, const BTL_POKEPARAM* bpp2 )
{
  m_setupParam->tvNaviData.frontPoke[0] = bpp1->GetMonsNo();
  if( bpp2 )
  {
    m_setupParam->tvNaviData.frontPoke[1] = bpp2->GetMonsNo();
  }
  ARIIZUMI_PRINT("TvNavi Set MonsNo [%d][%d]\n",m_setupParam->tvNaviData.frontPoke[0],m_setupParam->tvNaviData.frontPoke[1]);
}

//使用技の記録(毎回セットするので、結果的に最後に使った技
void MainModule::SetTvNaviData_UseWaza( const BTL_POKEPARAM *bpp , u16 wazaNo )
{
  //派生技をブロックするため、覚えている技かチェック
  //SBTS[1395]でも,わるあがきは持っているわけがないので例外チェック
  if( bpp->WAZA_IsUsable(static_cast<WazaID>(wazaNo)) || 
      wazaNo == WAZANO_WARUAGAKI )
  {
    m_setupParam->tvNaviData.lastWaza = wazaNo;
    ARIIZUMI_PRINT("TvNavi Set Waza [%d]\n",wazaNo);
  }
}

//メガシンカポケモンのセット
void MainModule::SetTvNaviData_MegaEvo( const BTL_POKEPARAM* bpp )
{
  m_setupParam->tvNaviData.megaPoke = bpp->GetMonsNo();
  ARIIZUMI_PRINT("TvNavi Set Mega [%d]\n",bpp->GetMonsNo());
}



//-------------------------------------------------------------------------
/**
 * @brief 戦闘終了後の「お手入れ」が可能か？
 * @retval true   戦闘後、「お手入れ( ポケパルレ )」への遷移が可能
 * @retval false  戦闘後、「お手入れ( ポケパルレ )」へ遷移してはいけない
 */
//-------------------------------------------------------------------------
bool MainModule::IsOteireEnable( void ) const
{
  if( GetSetupStatusFlag( BTL_STATUS_FLAG_OTEIRE_DISABLE ) )
  {
    return false;
  }

  return BattleRule::IsOteireEnable( m_setupParam->commMode, m_setupParam->competitor );
}

//-------------------------------------------------------------------------
/**
 * @brief 「お手入れ」ボタンを無効化する
 *
 * @note この関数を呼ぶと、以降、下画面でお手入れボタンをタッチしても
 *       お手入れボタンを押したことになりません。
 */
//-------------------------------------------------------------------------
void MainModule::SetOteireButtonDisable( void )
{
  m_isOteireButtonDisable = true;
}

//-------------------------------------------------------------------------
/**
 * @brief 「お手入れ」ボタンが押されたか？
 * @retval true   押された
 * @retval false  押されていない
 */
//-------------------------------------------------------------------------
bool MainModule::IsOteireButtonPressed( void ) const
{
  return m_isOteireButtonPressed;
}

//-----------------------------------------------------------------------------
/**
 * @brief 「お手入れ」対象ポケモンのIDを取得する
 * @retval BTL_POKEID_NULL  お手入れ対象ポケモンが存在しない場合
 *
 * @attention お手入れ対象ポケモンは、お手入れボタンが押された時点で確定します。
 */
//-----------------------------------------------------------------------------
u8 MainModule::GetOteireTargetPokeID( void ) const
{
  return m_oteireTargetPokeID;
}

//-----------------------------------------------------------------------------
/**
 * @brief 「お手入れ」ボタンが押されたことを通知する
 */
//-----------------------------------------------------------------------------
void MainModule::NotifyOteireButtonPressed( void )
{
  if( m_isOteireButtonDisable )
  {
    return;
  }

  m_isOteireButtonPressed = true;

  // お手入れ対象ポケモンを決定
  bool isFound = false;
  Oteire::GetOteireTarget( &isFound, &m_oteireTargetPokeID, this, m_pokeconForServer, m_myClientID );
}

//-----------------------------------------------------------------------------
/**
 * @brief 設定上、付いてはいけない汚れが付いていたら、汚れを取り除く
 * @param kawaigariData  汚れが付いて良いかのデータ
 * @param clientID       操作対象クライアントのID
 *
 * @attention この関数は、サーバ・クライアント両方の POKECON を変更します。
 * サーバー側の POKECON はサーバが、
 * クライアント側の POKECON はクライアントが変更するのが正しい設計です。
 * 通信対戦では使用しないでください。
 * 通信対戦で使用した場合、その後のバトルが正常に進行しない可能性があります。
 */
//-----------------------------------------------------------------------------
void MainModule::RemoveIllegalDirt( PokeTool::KawaigariParamCareCoreManager* kawaigariData, u8 clientID )
{
  if( ( kawaigariData == NULL ) ||
      ( clientID >= BTL_CLIENT_NUM ) )
  {
    GFL_ASSERT(0);
    return;
  }

  removeIllegalDirt( kawaigariData, clientID, m_pokeconForServer );
  removeIllegalDirt( kawaigariData, clientID, m_pokeconForClient );
}

/**
 * @brief 設定上、付いてはいけない汚れが付いていたら、汚れを取り除く
 * @param kawaigariData  汚れが付いて良いかのデータ
 * @param clientID       操作対象クライアントのID
 * @param pokecon        操作対象のPOKECON
 */
void MainModule::removeIllegalDirt( PokeTool::KawaigariParamCareCoreManager* kawaigariData, u8 clientID, POKECON* pokecon )
{
  if( ( kawaigariData == NULL ) ||
      ( clientID >= BTL_CLIENT_NUM ) ||
      ( pokecon == NULL ) )
  {
    GFL_ASSERT(0);
    return;
  }

  BTL_PARTY* myParty = pokecon->GetPartyData( clientID );
  Oteire::RemoveIllegalDirt( myParty, kawaigariData );
}







//-----------------------------------------------------------------------------
/**
 * @brief 乱入ポケモンのパラメータ抽選オブジェクトを取得する
 */
//-----------------------------------------------------------------------------
Field::Encount::PokeSet* MainModule::GetIntrudePokeParamGenerator( void ) const
{
  return m_setupParam->intrudeParam.intrudePokeParamGenerator;
}

//-----------------------------------------------------------------------------
/**
 * @brief 乱入バトルが可能か？
 * @retval true    乱入できる
 * @retval false   乱入できない
 */
//-----------------------------------------------------------------------------
bool MainModule::CanIntrudeBattle( void ) const
{
  const BtlRule       rule       = this->GetRule();
  const BtlCompetitor competitor = this->GetCompetitor();
  if( !BattleRule::CanIntrudeBattle( rule, competitor ) )
  {
    return false;
  }

  return INTRUDE_BATTLE_PARAM_IsEnable( m_setupParam->intrudeParam );
}

//-----------------------------------------------------------------------------
/**
 * @brief 乱入ポケモンをバトルに追加する
 * @param intrudePokePos    乱入ポケモンの立ち位置
 * @param intrudePokeParam  乱入ポケモンのパラメータ
 * @param intrudeType       乱入の種類( 乱入 or 援軍 )
 */
//-----------------------------------------------------------------------------
void MainModule::StartIntrudeBattle( 
  BtlPokePos                         intrudePokePos, 
  const pml::pokepara::PokemonParam& intrudePokeParam,
  BtlIntrudeType                     intrudeType )
{
  // 乱入ポケモンを登録
  if( !( this->registerIntrudePokemon( m_pokeconForClient, intrudePokePos, intrudePokeParam, false ) ) ||
      !( this->registerIntrudePokemon( m_pokeconForServer, intrudePokePos, intrudePokeParam, true ) ) )
  {
    GFL_ASSERT(0);
    return;
  }

  // AIを変更
  this->changeIntrudeClientAi( intrudePokePos, intrudeType );

  // ルールを変更
  m_rule = BTL_RULE_INTRUDE;
}

/**
 * @brief 乱入ポケモンを担当するクライアントＡＩを、乱入ＡＩに変更する
 * @param intrudePokePos    乱入ポケモンの立ち位置
 * @param intrudeType       乱入の種類( 乱入 or 援軍 )
 */
void MainModule::changeIntrudeClientAi( BtlPokePos intrudePokePos, BtlIntrudeType intrudeType )
{
  const BTL_CLIENT_ID clientId = PokemonPosition::GetPosCoverClientId( BTL_RULE_INTRUDE, BTL_MULTIMODE_NONE, intrudePokePos );
  if( ( clientId == BTL_CLIENT_NULL ) ||
      ( m_client[ clientId ] == NULL ) )
  {
    GFL_ASSERT(0);
    return;
  }

  switch( intrudeType )
  {
  case BTL_INTRUDE_TYPE_INTRUDE:
    m_client[ clientId ]->SetIntrudeAI();
    break;

  case BTL_INTRUDE_TYPE_REINFORCE:
    m_client[ clientId ]->SetReinforceAI();
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
}


/**
 * @brief 乱入ポケモンを POKECON に登録する
 * @param[out] pokecon           追加先のPOKECON
 * @param      intrudePokePos    乱入ポケモンの立ち位置
 * @param      intrudePokeParam  乱入ポケモンのパラメータ
 * @param      fMemoryPokeID     乱入ポケモンIDをpokeconに記録するかどうか
 * @retval true   登録できた
 * @retval false  登録できなかった
 */
bool MainModule::registerIntrudePokemon( 
  POKECON* pokecon, 
  BtlPokePos intrudePokePos, 
  const pml::pokepara::PokemonParam& intrudePokeParam, 
  bool fMemoryPokeID )
{
  const u8                  clientId   = PokemonPosition::GetPosCoverClientId( BTL_RULE_INTRUDE, BTL_MULTIMODE_NONE, intrudePokePos );
  const u8                  posIndex   = PokemonPosition::PosToSidePosIndex( BTL_RULE_INTRUDE, intrudePokePos );
  const u8                  pokeId     = decideIntrudePokemonID( pokecon, clientId, posIndex );
  const Savedata::MyStatus* playerData = this->IsClientNPC( clientId ) ? ( NULL ) : ( this->GetPlayerStatus() );

  DefaultPowerUpDesc defaultPowerUpDesc;
  DEFAULT_POWERUP_DESC_Clear( &defaultPowerUpDesc );

  bool succeeded = false;

  if( pokecon->IsExistPokemon( pokeId ) )
  {
    succeeded = pokecon->ReplacePokemon( clientId, pokeId, intrudePokeParam, defaultPowerUpDesc, playerData );
  }
  else
  {
    succeeded = pokecon->AddPokemon( clientId, pokeId, intrudePokeParam, defaultPowerUpDesc, playerData );
  }

  // 乱入によって現れたポケモンであることを設定する
  if( succeeded )
  {
    BTL_POKEPARAM* intrudePoke = pokecon->GetPokeParam( pokeId );
    intrudePoke->PERMFLAG_Set( BTL_POKEPARAM::PERMFLAG_INTRUDE_IN );
    if( fMemoryPokeID ){
      pokecon->IntrudePokeID_Set( pokeId );
    }
  }

  return succeeded;
}
/**
 * @brief   乱入ポケモンのIDを決定する
 * @param[in]  pokeCon  追加先のPokeCon
 * @param[in]  clientID 追加ポケモンが属するクライアントID
 * @param[in]  posIndex 追加位置のIndex
 * @return  乱入させるポケモンのID
 */
u8 MainModule::decideIntrudePokemonID( POKECON* pokeCon, u8 clientID, u8 posIndex ) const
{
  u8 defaultID = PokeID::GetClientPokeId( static_cast<BTL_CLIENT_ID>( clientID ), posIndex );
  u8 nextID = defaultID;

  for(u32 i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
  {
    u8 index = posIndex + i;
    if( index >= BTL_PARTY_MEMBER_MAX ){
      index %= BTL_PARTY_MEMBER_MAX;
    }
    nextID = PokeID::GetClientPokeId( static_cast<BTL_CLIENT_ID>( clientID ), index );

    if( pokeCon->IsExistPokemon(nextID) )
    {
      // すでに存在して生きているなら使えない
      const BTL_POKEPARAM* bpp = pokeCon->GetPokeParam( nextID );
      if( !bpp->IsDead()  ){
        continue;
      }

      // 直近にひん死になったポケモンIDは使わない
      // （フリーフォールなどターンをまたぐ攻撃で別ポケモンを同一ポケモンと勘違いする可能性があるので）
      if( m_deadPokeIDRec.IsRegisteredRecent(nextID, 2) ){
        continue;
      }
    }

    return nextID;
  }

  GFL_ASSERT(0);
  return defaultID;
}

#if PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief [デバッグ機能] 乱入ポケモンの固定パラメータを取得する
 * @retval NULL  固定パラメータが存在しない場合
 */
//-----------------------------------------------------------------------------
const pml::pokepara::PokemonParam* MainModule::DEBUG_GetIntrudePokeParam( void ) const
{
  return m_setupParam->intrudeParam.intrudePokeParam;
}

#endif




//-----------------------------------------------------------------------------
/**
 * @brief 援軍の乱入が可能か？
 * @retval true   援軍が可能
 * @retval false  援軍が可能でない
 */
//-----------------------------------------------------------------------------
bool MainModule::IsReinforceEnable( void ) const
{
  const BtlRule       rule       = this->GetRule();
  const BtlCompetitor competitor = this->GetCompetitor();
  if( !BattleRule::CanIntrudeBattle( rule, competitor ) )
  {
    return false;
  }

  return REINFORCE_BATTLE_PARAM_IsEnable( m_setupParam->reinforceParam );
}

//-----------------------------------------------------------------------------
/**
 * @brief 援軍のパラメータを取得する
 */
//-----------------------------------------------------------------------------
const ReinforceBattleParam& MainModule::GetReinforceBattleParam( void ) const
{
  return m_setupParam->reinforceParam;
}

/**
 * @brief ポケモンがひん死になった通知を受け取る（乱入ポケモンIDの生成に利用している）
 * @param[in]  ひん死になったポケモンID
 */
void MainModule::NotifyPokemonDead( u8 pokeID ) const
{
  PokeIDRec* rec = const_cast<PokeIDRec*>( &m_deadPokeIDRec );
  rec->Register( pokeID );
}

//Rotom関連
bool MainModule::IsEnableRotom(void)const  ///<ロトムが有効か？
{
  return m_setupParam->bEnableRotom;
}
Savedata::FieldMenu::RotomRank MainModule::GetRotomRank(void)const///<ロトムの仲良し度
{
  return m_setupParam->rotomRank;
}

//バトルフェスフィールド効果関連
BtlFesField MainModule::GetBtlFesField(void)const///< バトルフェスフィールド効果
{
  return m_setupParam->btlFesFieldType;
}


#if defined( DEBUG_ONLY_FOR_taya )
void  MainModule::test_pokeIDRec( void )
{
  m_deadPokeIDRec.Register( 12 );
  m_deadPokeIDRec.Register( 13 );
  if( !m_deadPokeIDRec.IsRegistered(12) ){
    GFL_ASSERT(0);
  }
  if( !m_deadPokeIDRec.IsRegistered(13) ){
    GFL_ASSERT(0);
  }
  if( m_deadPokeIDRec.IsRegistered(11) ){
    GFL_ASSERT(0);
  }
  if( m_deadPokeIDRec.IsRegistered(0) ){
    GFL_ASSERT(0);
  }
  if( m_deadPokeIDRec.IsRegistered(5) ){
    GFL_ASSERT(0);
  }
  m_deadPokeIDRec.Register( 0 );
  if( !m_deadPokeIDRec.IsRegistered(0) ){
    GFL_ASSERT(0);
  }
  if( !m_deadPokeIDRec.IsRegisteredRecent(0,1) ){
    GFL_ASSERT(0);
  }
  if( !m_deadPokeIDRec.IsRegisteredRecent(13,1) ){
    GFL_ASSERT(0);
  }
  if( !m_deadPokeIDRec.IsRegisteredRecent(13,3) ){
    GFL_ASSERT(0);
  }
  if( m_deadPokeIDRec.IsRegisteredRecent(12,1) ){
    GFL_ASSERT(0);
  }
  if( !m_deadPokeIDRec.IsRegisteredRecent(12,2) ){
    GFL_ASSERT(0);
  }
  m_deadPokeIDRec.Register( 14 );
  m_deadPokeIDRec.Register( 15 );
  m_deadPokeIDRec.Register( 16 );
  if( m_deadPokeIDRec.IsRegisteredRecent(12,2) ){
    GFL_ASSERT(0);
  }
  if( !m_deadPokeIDRec.IsRegisteredRecent(15,2) ){
    GFL_ASSERT(0);
  }
  if( m_deadPokeIDRec.IsRegisteredRecent(14,2) ){
    GFL_ASSERT(0);
  }
  if( !m_deadPokeIDRec.IsRegisteredRecent(0,1) ){
    GFL_ASSERT(0);
  }
  m_deadPokeIDRec.Register( 12 );
  if( !m_deadPokeIDRec.IsRegisteredRecent(12,1) ){
    GFL_ASSERT(0);
  }

  m_deadPokeIDRec.Clear();
}
#endif



GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif
