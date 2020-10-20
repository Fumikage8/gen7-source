// ============================================================================
/*
 * @file NBREvent.cpp
 * @brief 通信対戦受付アプリ起動イベント
 * @date 2015.06.23
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetStatic/NetEvent/include/NBREvent.h"
#include "NetApp/NBR/include/NBRProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

// NBRNet
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(NBR)


//--------------------------------------------------------------
/**
 * @brief  通信対戦受付アプリ呼び出し
 * 
 * @param   pGameManager  ゲームマネージャ
 * @param   pEventParam   イベント呼び出しパラメータ構造体
 */
//--------------------------------------------------------------
void NBREvent::StartEvent( GameSys::GameManager* pGameManager, EVENT_PARAM* pEventParam )
{
  ICHI_PRINT("NBREvent::StartEvent\n");
  NBREvent* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NBREvent>( pGameManager->GetGameEventManager() );

  // 本イベントの呼び出しパラメータのポインタセット
  p_event->SetEventParam( pEventParam );
  ICHI_PRINT("hostNetId:%d battleRule=%d \n",pEventParam->hostNetID, pEventParam->battleRule );
}


// コンストラクタ
NBREvent::NBREvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_pEventParam(NULL),
 m_sequence(),
 m_callAppType(),
 m_nbrAppParam(),
 m_regulationSelectResult(),
 m_teamSelectEventParam(),
 m_networkBattleParam(),
 m_regulationDrawInfo( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) )
{
  ICHI_PRINT("NBREvent::NBREvent\n");
  // 対戦に必要な個人データ作成
  for( int i=0; i<BTL_CLIENT_NUM; i++ ){
    m_personalData[i].pPokeParty = GFL_NEW( pHeap ) pml::PokeParty( pHeap );
    m_personalData[i].pMyStatus = GFL_NEW( pHeap ) Savedata::MyStatus();
  }
  gfl2::std::MemClear( &m_nbrAppParam, sizeof(NetApp::NBR::APP_PARAM) );

  // NBR通信クラス
  {
    gfl2::heap::HeapBase* pNetHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    m_pNBRNet = GFL_NEW( pNetHeap ) NetApp::NBR::NBRNet( pNetHeap );
  }
}

// デストラクタ
NBREvent::~NBREvent()
{
  ICHI_PRINT("NBREvent::~NBREvent\n");
  // NBR通信クラス破棄
  GFL_DELETE m_pNBRNet;
  // 対戦に必要な個人データ破棄
  for( int i=0; i<BTL_CLIENT_NUM; i++ ){
    GFL_DELETE m_personalData[i].pMyStatus;
    GFL_DELETE m_personalData[i].pPokeParty;
  }
}


//--------------------------------------------------------------------------
bool NBREvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//--------------------------------------------------------------------------
void NBREvent::InitFunc( GameSys::GameManager* pGameManager )
{
  // NBRアプリ呼び出しパラメータの初期化
  InitializeNbrAppParam();
  // レギュレーション選択アプリ呼び出しパラメータの初期化
  InitializeRegulationSelectAppParam();
  // バトルチーム選択アプリ呼び出しパラメータの初期化
  InitializeTeamSelectAppParam();

  // 初期呼び出しアプリはNBR
  SetCallApp( CALL_NBR );
}


//--------------------------------------------------------------------------
void NBREvent::EndFunc( GameSys::GameManager* pGameManager )
{
  m_pNBRNet->Terminate(); // NBR通信クラス終了
}


//--------------------------------------------------------------------------
GameSys::GMEVENT_RESULT NBREvent::MainFunc( GameSys::GameManager* pGameManager )
{
  // シーケンス制御
  switch( m_sequence ){
  case SEQ_CALL_APP:   //! アプリ呼び出し
    CallApp( pGameManager );
    break;
  case SEQ_APP_END_WAIT://! アプリ終了待ち
    {
      // プロセス終了待ち
      if( pGameManager->IsProcessExists() == false )
      {
        m_sequence = SEQ_RETURN_APP; 
      }
    }
    break;
  case SEQ_RETURN_APP: //! アプリ戻り
    ReturnApp();
    break;
  case SEQ_END:        //! 終了
    return GameSys::GMEVENT_RES_FINISH;
  default:
    GFL_ASSERT(0);
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * @brief  呼び出すアプリ設定
 * @param  callAppType アプリ種類
 */
//--------------------------------------------------------------------------
void NBREvent::SetCallApp( CallAppType callAppType )
{
  m_callAppType = callAppType;
  ICHI_PRINT("呼び出しアプリ %d \n", callAppType);

  m_sequence = SEQ_CALL_APP;
}

//--------------------------------------------------------------------------
/**
 * @brief  アプリ呼び出し

 */
//--------------------------------------------------------------------------
void NBREvent::CallApp( GameSys::GameManager* pGameManager )
{
  GFL_PRINT("NBREvent::CallApp %d \n", m_callAppType);
  switch( m_callAppType ){
  case CALL_NBR:               //! NBRアプリ呼び出し
    {
      // @note : 最初にdllを読み込まないとdll内のクラスをnew出来ない。要注意
      GameSys::CallNBRProc( &m_nbrAppParam );
      // 終了待ち
      m_sequence = SEQ_APP_END_WAIT;
    }
    break;
  case CALL_REGULATION_SELECT: //! レギュレーション選択アプリ呼び出し
    {
      NetApp::RegulationSelect::APP_PARAM appParam;
      appParam.bootMode = NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P;
      // バトル形式
      appParam.battleRule = m_nbrAppParam.battleRule;
      NetEvent::RegulationSelect::RegulationSelectEvent::StartEvent( pGameManager, appParam, &m_regulationSelectResult );
      // 終了待ち
      m_sequence = SEQ_RETURN_APP;
    }
    break;
  case CALL_BATTLE_TEAM_SELECT://! バトルチーム選択アプリ呼び出し
    {
      m_teamSelectEventParam.in.bAddQRPokeParty = m_regulationDrawInfo.GetRegulation().GetBoolParam( Regulation::QR_TEAM );
      m_teamSelectEventParam.in.bOpenRegulationButton = true;//レギュレーション詳細画面に遷移できるか
      NetEvent::TeamSelect::TeamSelectEvent::StartEvent( pGameManager, &m_teamSelectEventParam );
      // 終了待ち
      m_sequence = SEQ_RETURN_APP;
    }
    break;
  case CALL_BATTLE:            //! バトル呼び出し
    {
      // 呼び出しパラメータ設定
      SetupNetworkBattleParam();
      NetEvent::NetworkBattle::NetworkBattleEvent::StartEvent( pGameManager, &m_networkBattleParam );
      // 終了待ち
      m_sequence = SEQ_RETURN_APP;
    }
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief  アプリ戻り
 */
//--------------------------------------------------------------------------
void NBREvent::ReturnApp()
{
  GFL_PRINT("NBREvent::ReturnApp %d \n", m_callAppType);

  // エラー時はイベント終了
  if( IsError() )
  {
    m_sequence = SEQ_END;
    return;
  }

  switch( m_callAppType ){
  case CALL_NBR:               //! NBRアプリ呼び出し
    {
      GFL_PRINT("m_nbrAppParam.returnMode = %d\n", m_nbrAppParam.returnMode);
      // アプリ戻り値
      switch( m_nbrAppParam.returnMode ){
      case NetApp::NBR::RET_END:          m_sequence = SEQ_END; break;
      case NetApp::NBR::RET_RULE_SELECT:  SetCallApp( CALL_REGULATION_SELECT ); break;
      case NetApp::NBR::RET_TEAM_SELECT:  SetCallApp( CALL_BATTLE_TEAM_SELECT ); break;
      case NetApp::NBR::RET_BATTLE:       SetCallApp( CALL_BATTLE ); break;
      default:
        GFL_ASSERT(0);
        m_sequence = SEQ_END;
        break;
      }
    }
    break;
  case CALL_REGULATION_SELECT: //! レギュレーション選択アプリ呼び出し
    {
      // レギュレーション選択したかで起動モード分ける
      m_nbrAppParam.startupMode = ( m_regulationSelectResult.bIsSelectedRegulation == true ) ? NetApp::NBR::STARTUP_RULE_SELECT_DECIDE : NetApp::NBR::STARTUP_RULE_SELECT_CANCEL; 

      // NBR起動
      SetCallApp( CALL_NBR );
    }
    break;
  case CALL_BATTLE_TEAM_SELECT://! バトルチーム選択アプリ呼び出し
    {
      GFL_PRINT("選択したチーム番号 %d \n", m_teamSelectEventParam.out.teamIdx );
      // アプリ結果
      m_nbrAppParam.teamSelectReturnMode = ( m_teamSelectEventParam.out.result == NetApp::TeamSelect::EVENT_APP_RESULT_TEAM_SELECT ) ? NetApp::NBR::TEAM_SELECT_RETURN_DECIDE : NetApp::NBR::TEAM_SELECT_RETURN_CANCEL;
      // 選択したチームインデックス
      m_nbrAppParam.teamSelectIndex = m_teamSelectEventParam.out.teamIdx;

#ifdef GF_PLATFORM_WIN32
      // Win版は強制で選んだことにする
      m_nbrAppParam.teamSelectReturnMode = NetApp::NBR::TEAM_SELECT_RETURN_DECIDE;
#endif

      // NBRに戻る
      m_nbrAppParam.startupMode = NetApp::NBR::STARTUP_TEAM_SELECT_AGAIN;
      SetCallApp( CALL_NBR );
    }
    break;
  case CALL_BATTLE:            //! バトル呼び出し
    {
      // @fix NMCat[2698]：ホストマイグレーションだと通信エラーではないのでこのフラグで見る必要がある。
      if( m_networkBattleParam.isDisconnectOccur == true )
      {
        // 切断だけしにいく
        m_nbrAppParam.startupMode = NetApp::NBR::STARTUP_DISCONNECT_OCCUR;
      }
      else
      {
        // 正常に終了したらNBRに戻る
        m_nbrAppParam.startupMode = NetApp::NBR::STARTUP_BATTLE_AGAIN;
      }

      // 対戦毎にリセットしたいパラメータ
      m_nbrAppParam.bgm = 0;
      m_nbrAppParam.selectBgmIndex = 0;

      SetCallApp( CALL_NBR );
    }
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief  NBRアプリ呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void NBREvent::InitializeNbrAppParam()
{
  m_nbrAppParam.startupMode = NetApp::NBR::STARTUP_FIRST; // 初期起動モード
  m_nbrAppParam.hostNetID = m_pEventParam->hostNetID; // 主催者のNetId
  m_nbrAppParam.battleRule = m_pEventParam->battleRule; // バトルルール
  ICHI_PRINT(" nbr param4 %x \n", m_pEventParam);
  GFL_PRINT("hostNetId:%d battleRule=%d \n",m_pEventParam->hostNetID, m_pEventParam->battleRule );
  GFL_PRINT("hostNetId:%d battleRule=%d \n",m_nbrAppParam.hostNetID, m_nbrAppParam.battleRule );
  m_nbrAppParam.teamSelectReturnMode = NetApp::NBR::TEAM_SELECT_RETURN_CANCEL;
  m_nbrAppParam.pNet = m_pNBRNet;

  // 接続相手の対戦データ受け取り用
  for( int i=0; i<BTL_CLIENT_NUM; i++ ){
    m_nbrAppParam.pPersonalData[i] = &m_personalData[i];
  }
  // レギュレーション受け取り用
  m_nbrAppParam.pSelectRegulationDrawInfo = &m_regulationDrawInfo;
}

//--------------------------------------------------------------------------
/**
 * @brief  レギュレーション選択アプリ呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void NBREvent::InitializeRegulationSelectAppParam()
{
  // パラメータで実体が必要な物
  m_regulationSelectResult.pSelectRegulationDrawInfo = &m_regulationDrawInfo;
}

//--------------------------------------------------------------------------
/**
 * @brief  バトルチーム選択アプリ呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void NBREvent::InitializeTeamSelectAppParam()
{
  // レギュレーションポインタ渡しておく
  m_teamSelectEventParam.in.pRegulationDrawInfo = &m_regulationDrawInfo;
}

//--------------------------------------------------------------------------
/**
 * @brief  通信対戦イベント呼び出しパラメータの設定
 */
//--------------------------------------------------------------------------
void NBREvent::SetupNetworkBattleParam()
{
  // 個々人のデータ
  for( int i=0; i<BTL_CLIENT_NUM; i++ ) {
    m_networkBattleParam.pPersonalData[i] = &m_personalData[i];
  }
  // レギュレーション
  m_networkBattleParam.pRegulation = &m_regulationDrawInfo.GetRegulation();
  // 立ち位置
  m_networkBattleParam.standingPos = m_nbrAppParam.standingPos;
  // BGM
  m_networkBattleParam.bgm = m_nbrAppParam.bgm;
}


GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetEvent)
