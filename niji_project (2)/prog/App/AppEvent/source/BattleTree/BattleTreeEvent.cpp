// ============================================================================
/*
 * @file BattleTreeEvent.cpp
 * @brief バトルツリーアプリのイベント
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/AppEvent/include/BattleTree/BattleTreeEvent.h"
#include "App/BattleTree/include/BattleTreeProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include "NetStatic/NetEvent/include/BattleVideoRecordingEvent.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


void BattleTreeEvent::StartEvent( GameSys::GameManager* pGameManager, App::BattleTree::APP_PARAM* pAppParam )
{
  GFL_ASSERT( pAppParam );

  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  BattleTreeEvent*  pEvnet  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, BattleTreeEvent>( pGameEventManager );
  pEvnet->m_pAppParam = pAppParam;
}


BattleTreeEvent::BattleTreeEvent( gfl2::heap::HeapBase* pHeap )
: GameSys::GameEvent( pHeap )
, m_pAppParam( NULL )
, m_pProc( NULL )
{
}


BattleTreeEvent::~BattleTreeEvent()
{
}


bool BattleTreeEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void BattleTreeEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  //  proc生成
  m_pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::BattleTree::BattleTreeProc, App::BattleTree::APP_PARAM>( 
            pGameManager->GetProcManager(), m_pAppParam, pGameManager->GetAsyncFileManager() , "BattleTree.cro" );
}


void BattleTreeEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT BattleTreeEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  //  procが死んでいるかチェック
  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
  {
    UCHIDA_PRINT( "%s: m_pAppParam->out.npcSelectIdx(%d)\n", __FUNCTION__, m_pAppParam->out.npcSelectIdx );

    //  正常終了
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}


/**
* @class EventBattleInstBattleVideoRecord
* @brief バトルビデオ呼び出し
*/
void EventBattleInstBattleVideoRecord::StartEvent( GameSys::GameManager* gmgr, int battleCount )
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  EventBattleInstBattleVideoRecord* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleInstBattleVideoRecord>( pGameEventManager );
  pEvent->m_BattleCount = battleCount;
}

EventBattleInstBattleVideoRecord::EventBattleInstBattleVideoRecord( gfl2::heap::HeapBase* pHeap )
: GameSys::GameEvent( pHeap ),
  mpInst(NULL),
  seq(CALL_EVENT),
  m_battleVideoRecordingParam(),
  m_battleRecorderSaveData( pHeap, GFL_SINGLETON_INSTANCE( GameSys::GameManager ) ),
  m_BattleCount(0)
{
}

//デストラクタ
EventBattleInstBattleVideoRecord::~EventBattleInstBattleVideoRecord()
{
}

// 起動チェック
bool EventBattleInstBattleVideoRecord::BootChk( GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventBattleInstBattleVideoRecord::InitFunc( GameSys::GameManager* gmgr)
{
  mpInst = gmgr->GetBattleInstManager()->GetBattleInst();

  if ( mpInst->GetIsComm() )
  {
    m_battleVideoRecordingParam.in.appInParam.saveDataServerVersion = mpInst->GetBattleSetupParam()->commServerVer;
  }

  // バトルで使用したバトルサーバーバージョンを渡す
  // 切断が発生していたら録画できなくする
  m_battleVideoRecordingParam.in.appInParam.bRecNG = mpInst->GetBattleSetupParam()->isDisconnectOccur;

  // BattleSetupParamとレギュレーションからバトルビデオセーブデータ作成
  m_battleRecorderSaveData.CreateSaveData( mpInst->GetBattleSetupParam(), &mpInst->GetRegulation() );
  m_battleRecorderSaveData.SetBattleCounter(m_BattleCount);
  m_battleVideoRecordingParam.in.appInParam.pRecorderSaveData = &m_battleRecorderSaveData;

}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBattleInstBattleVideoRecord::MainFunc( GameSys::GameManager* gmgr)
{
  switch (seq)
  {
  case CALL_EVENT:
    NetEvent::BattleVideoRecording::Event::StartEvent( gmgr, &m_battleVideoRecordingParam );
    seq = END;
    break;
  case END:
    return GameSys::GMEVENT_RES_FINISH;
  default:
    break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventBattleInstBattleVideoRecord::EndFunc( GameSys::GameManager* gmgr)
{
}


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
