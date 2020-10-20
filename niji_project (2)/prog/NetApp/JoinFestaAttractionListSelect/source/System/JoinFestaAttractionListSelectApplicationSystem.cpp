// ============================================================================
/*
 * @file JoinFestaAttractionListSelectApplicationSystem.cpp
 * @brief アトラクション開催参加アプリのアプリケーションシステムです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectApplicationSystem.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectViewList.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectViewObjects.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectViewSystemAccessor.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectBuilder.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionListSelectApplicationSystem::JoinFestaAttractionListSelectApplicationSystem() :
  m_pJoinFestaAttractionListSelectWork( NULL ),
  m_pJoinFestaAttractionListSelectViewList( NULL ),
  m_pJoinFestaAttractionListSelectViewObjects( NULL ),
  m_pJoinFestaAttractionListSelectViewSystemAccessor( NULL )
{
}


JoinFestaAttractionListSelectApplicationSystem::~JoinFestaAttractionListSelectApplicationSystem()
{
  if( m_pJoinFestaAttractionListSelectViewList )
  {
    m_pJoinFestaAttractionListSelectViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaAttractionListSelectViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionListSelectViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionListSelectViewList );
}


void JoinFestaAttractionListSelectApplicationSystem::Build( NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork, NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaAttractionListSelectWork->GetAppHeap();

  m_pJoinFestaAttractionListSelectWork               = pJoinFestaAttractionListSelectWork;
  m_pJoinFestaAttractionListSelectViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewList( pAppHeap );
  m_pJoinFestaAttractionListSelectViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewObjects();
  m_pJoinFestaAttractionListSelectViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor( m_pJoinFestaAttractionListSelectViewList, m_pJoinFestaAttractionListSelectViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaAttractionListSelectViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaAttractionListSelectApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST, print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION, print::GetMessageArcId(), GARC_message_jf_attraction_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaAttractionListSelectApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_playerlist_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_attraction_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION ) );
  m_pJoinFestaAttractionListSelectWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaAttractionListSelectApplicationSystem::CreateView()
{
  m_pJoinFestaAttractionListSelectViewObjects->CreateView( m_pJoinFestaAttractionListSelectWork, *m_pNetAppMessageMenu );
}


void JoinFestaAttractionListSelectApplicationSystem::UpdateView()
{
  m_pJoinFestaAttractionListSelectViewList->Update();

  m_pJoinFestaAttractionListSelectWork->GetJoinFestaAttractionListManager().UpdateEntryTimeLimit();
}


void JoinFestaAttractionListSelectApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaAttractionListSelectViewList->Draw( displayNo );
}


bool JoinFestaAttractionListSelectApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaAttractionListSelectViewObjects )
  {
    if( m_pJoinFestaAttractionListSelectViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
