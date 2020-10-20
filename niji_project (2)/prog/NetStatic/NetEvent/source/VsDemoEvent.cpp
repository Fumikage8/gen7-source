//=================================================================================================
/**
 * @file    VsDemoEvent.cpp
 * @brief VSデモイベント
 * @date 2016/01/21
 * @author ichiraku_katsuhiko
 */
//=================================================================================================
#include "NetStatic/NetEvent/include/VsDemoEvent.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "GameSys/include/DllProc.h"

#include "GameSys/include/GameEventManager.h" // GAMESYSTEM_EVENT_CALL

GFL_NAMESPACE_BEGIN( NetEvent )

//--------------------------------------------------------------
/**
 * @brief  VSデモ呼び出し
 * 
 * @param   pGameManager  ゲームマネージャ
 * @param   pEventParam   イベント呼び出しパラメータ構造体
 */
//--------------------------------------------------------------
void VsDemoEvent::StartEvent( GameSys::GameManager* pGameManager, App::AppDemo::AppDemoProcParam_VS procParam )
{
  ICHI_PRINT("VsDemoEvent::StartEvent\n");
  VsDemoEvent* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, VsDemoEvent>( pGameManager->GetGameEventManager() );

  // 本イベントの呼び出しパラメータのポインタセット
  p_event->SetAppDemoParamVs( procParam );
}

// コンストラクタ
VsDemoEvent::VsDemoEvent( gfl2::heap::HeapBase* pHeap ) :
GameSys::GameEvent( pHeap ),
 m_procParam(Regulation::BATTLE_RULE_SINGLE),
 m_pProc(NULL)
{
  GFL_PRINT("VsDemoEvent 開始\n");
}

// デストラクタ
VsDemoEvent::~VsDemoEvent()
{
  GFL_PRINT("VsDemoEvent 終了\n");
}

//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool VsDemoEvent::BootChk( GameSys::GameManager* pGameManager )
{
  GFL_UNUSED( pGameManager );
  return true;
}

//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void VsDemoEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  GFL_UNUSED( pGameManager );
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT VsDemoEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  enum{
    SEQ_PROC_CALL,        //!< プロセスコール
    SEQ_PROC_END_WAIT,    //!< プロセス終了待ち
  };

  switch( GetSeqNo() ){
  case SEQ_PROC_CALL:        //!< プロセスコール
    {
      m_pProc = GameSys::CallAppDemoProc_VS( &m_procParam );
      SetSeqNo( SEQ_PROC_END_WAIT );
    }
    break;
  case SEQ_PROC_END_WAIT:    //!< プロセス終了待ち
    {
      if( pGameManager->IsProcessExists() == false )
      {
        return GameSys::GMEVENT_RES_FINISH;
      }
    }
    break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void VsDemoEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_UNUSED( pGameManager );

}

GFL_NAMESPACE_END( NetEvent )
