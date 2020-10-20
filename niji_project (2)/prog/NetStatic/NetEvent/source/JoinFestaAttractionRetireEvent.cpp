// ============================================================================
/*
 * @file JoinFestaAttractionRetireEvent.cpp
 * @brief アトラクションリタイアのイベント
 * @date 2015.12.02
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaAttractionRetireEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

// JoinFesta
#include "FieldScript/include/EventScriptCall.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include <niji_conv_header/field/script/inc/join_festa_attraction.inc>

GFL_NAMESPACE_BEGIN(NetEvent)


JoinFestaAttractionRetireEvent::JoinFestaAttractionRetireEvent( gfl2::heap::HeapBase* pHeap ) :
  GameSys::GameEvent( pHeap )
{
}


JoinFestaAttractionRetireEvent::~JoinFestaAttractionRetireEvent()
{
}


bool JoinFestaAttractionRetireEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaAttractionRetireEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  // アトラクションリタイアリクエストをONにする
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->SetRetireRequest();
}


void JoinFestaAttractionRetireEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaAttractionRetireEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  switch( GetSeqNo() ){
  case 0:
    // アトラクション終了待ち
    if( !GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
    {
      AddSeqNo();
    }
    break;
  case 1:
    // ローカルアトラクションリタイアスクリプト起動
    Field::EventScriptCall::CallScript( pGameManager, SCRID_JOIN_FESTA_ATTRACTION_RETIRE, NULL );
    AddSeqNo();
    break;
  case 2:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}


GFL_NAMESPACE_END(NetEvent)
