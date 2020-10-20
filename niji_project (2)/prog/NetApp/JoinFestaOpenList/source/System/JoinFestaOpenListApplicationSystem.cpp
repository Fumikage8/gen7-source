// ============================================================================
/*
 * @file JoinFestaOpenListApplicationSystem.cpp
 * @brief 開催選択アプリのアプリケーションシステムです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListApplicationSystem.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewList.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewObjects.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewSystemAccessor.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListBuilder.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


JoinFestaOpenListApplicationSystem::JoinFestaOpenListApplicationSystem() :
  m_pJoinFestaOpenListWork( NULL ),
  m_pJoinFestaOpenListViewList( NULL ),
  m_pJoinFestaOpenListViewObjects( NULL ),
  m_pJoinFestaOpenListViewSystemAccessor( NULL )
{
}


JoinFestaOpenListApplicationSystem::~JoinFestaOpenListApplicationSystem()
{
  if( m_pJoinFestaOpenListViewList )
  {
    m_pJoinFestaOpenListViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaOpenListViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaOpenListViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaOpenListViewList );
}


void JoinFestaOpenListApplicationSystem::Build( NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::System::JoinFestaOpenListBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaOpenListWork->GetAppHeap();

  m_pJoinFestaOpenListWork               = pJoinFestaOpenListWork;
  m_pJoinFestaOpenListViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewList( pAppHeap );
  m_pJoinFestaOpenListViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewObjects();
  m_pJoinFestaOpenListViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor( m_pJoinFestaOpenListViewList, m_pJoinFestaOpenListViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaOpenListViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaOpenListApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST, print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION, print::GetMessageArcId(), GARC_message_jf_attraction_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaOpenListApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_playerlist_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_attraction_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION ) );
  m_pJoinFestaOpenListWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaOpenListApplicationSystem::CreateView()
{
  m_pJoinFestaOpenListViewObjects->CreateView( m_pJoinFestaOpenListWork, *m_pNetAppMessageMenu, *m_pNetAppTrainerIconUtility );
}


void JoinFestaOpenListApplicationSystem::UpdateView()
{
  m_pJoinFestaOpenListViewList->Update();

  m_pJoinFestaOpenListWork->GetJoinFestaAttractionListManager().UpdateEntryTimeLimit();
}


void JoinFestaOpenListApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaOpenListViewList->Draw( displayNo );
}


bool JoinFestaOpenListApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaOpenListViewObjects )
  {
    if( m_pJoinFestaOpenListViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
