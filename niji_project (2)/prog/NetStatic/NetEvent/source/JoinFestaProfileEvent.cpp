// ============================================================================
/*
 * @file JoinFestaProfileEvent.cpp
 * @brief プロフィールアプリのイベント
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaProfileEvent.h"
#include "NetApp/JoinFestaProfile/include/JoinFestaProfileProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "NetStatic/NetEvent/include/JoinFestaSelectWordEvent.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)

enum Sequence
{
  SEQUENCE_PROFILE,
  SEQUENCE_SELECT_WORD
};

void JoinFestaProfileEvent::StartEvent( GameSys::GameManager* pGameManager )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaProfileEvent>( pGameEventManager );
}


JoinFestaProfileEvent::JoinFestaProfileEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap ),
 m_AppParam(),
 m_pJoinFestaProfileProc( NULL ),
 m_pJoinFestaProfileWork( NULL )
{
}


JoinFestaProfileEvent::~JoinFestaProfileEvent()
{
}


bool JoinFestaProfileEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaProfileEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  bootProfileProc();
}


void JoinFestaProfileEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_SAFE_DELETE( m_pJoinFestaProfileWork );
}


void JoinFestaProfileEvent::bootProfileProc()
{
  m_pJoinFestaProfileProc = GameSys::CallJoinFestaProfileProc( &m_AppParam );

  if( m_pJoinFestaProfileWork == NULL )
  {
    gfl2::heap::HeapBase* pDevHeap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  
    m_pJoinFestaProfileWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaProfile::System::JoinFestaProfileWork();
  }

  m_pJoinFestaProfileProc->SetProfileWork( m_pJoinFestaProfileWork );

  SetSeqNo( SEQUENCE_PROFILE );
}


void JoinFestaProfileEvent::bootSelectWordEvent( GameSys::GameManager* pGameManager )
{
  NetEvent::JoinFestaSelectWord::EVENT_PARAM eventParam;

  NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenuInfo = m_pJoinFestaProfileWork->GetSelectEditMenu();

  if( pSelectEditMenuInfo )
  {
    eventParam.editMenuID = pSelectEditMenuInfo->editMenuID;
  }
  else
  {
    eventParam.editMenuID = NetApp::JoinFestaProfile::EDIT_MENU_ID_MEETING_MESSAGE;
  }

  NetEvent::JoinFestaSelectWord::JoinFestaSelectWordEvent::StartEvent( pGameManager, eventParam );

  SetSeqNo( SEQUENCE_SELECT_WORD );
}


GameSys::GMEVENT_RESULT JoinFestaProfileEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  switch( GetSeqNo() )
  {
    // --------------------------------------------------------------------------------------------
    case SEQUENCE_PROFILE:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaProfileProc ) == false )
      {
        switch( m_pJoinFestaProfileWork->GetBootProcID() )
        {
          case NetApp::JoinFestaProfile::BOOT_PROC_ID_NONE:
          {
            result = GameSys::GMEVENT_RES_FINISH;
          }
          break;

          case NetApp::JoinFestaProfile::BOOT_PROC_ID_SELECT_WORD:
          {
            bootSelectWordEvent( pGameManager );
          }
          break;
        }
        m_pJoinFestaProfileWork->SetBootProcID( NetApp::JoinFestaProfile::BOOT_PROC_ID_NONE );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case SEQUENCE_SELECT_WORD:
    {
      bootProfileProc();
    }
    break;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetEvent)
