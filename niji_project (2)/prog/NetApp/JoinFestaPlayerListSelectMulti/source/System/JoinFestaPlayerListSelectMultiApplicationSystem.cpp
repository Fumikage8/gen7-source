// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiApplicationSystem.cpp
 * @brief プレイヤー複数選択アプリのアプリケーションシステムです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiApplicationSystem.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiViewList.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiViewObjects.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiViewSystemAccessor.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiBuilder.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectMultiApplicationSystem::JoinFestaPlayerListSelectMultiApplicationSystem() :
  m_pJoinFestaPlayerListSelectMultiWork( NULL ),
  m_pJoinFestaPlayerListSelectMultiViewList( NULL ),
  m_pJoinFestaPlayerListSelectMultiViewObjects( NULL ),
  m_pJoinFestaPlayerListSelectMultiViewSystemAccessor( NULL )
{
}


JoinFestaPlayerListSelectMultiApplicationSystem::~JoinFestaPlayerListSelectMultiApplicationSystem()
{
  if( m_pJoinFestaPlayerListSelectMultiViewObjects )
  {
    m_pJoinFestaPlayerListSelectMultiViewObjects->RemoveFromSuperViewAll();
  }

  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectMultiViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectMultiViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectMultiViewList );
}


void JoinFestaPlayerListSelectMultiApplicationSystem::Build( NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork, NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerListSelectMultiWork->GetAppHeap();

  m_pJoinFestaPlayerListSelectMultiWork               = pJoinFestaPlayerListSelectMultiWork;
  m_pJoinFestaPlayerListSelectMultiViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewList( pAppHeap );
  m_pJoinFestaPlayerListSelectMultiViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewObjects();
  m_pJoinFestaPlayerListSelectMultiViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor( m_pJoinFestaPlayerListSelectMultiViewList, m_pJoinFestaPlayerListSelectMultiViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaPlayerListSelectMultiViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaPlayerListSelectMultiApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_MESSAGE_JOINFESTAPLAYERLISTSELECTMULTI, print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaPlayerListSelectMultiApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_playerlist_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_MESSAGE_JOINFESTAPLAYERLISTSELECTMULTI ) );
  m_pJoinFestaPlayerListSelectMultiWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaPlayerListSelectMultiApplicationSystem::CreateView()
{
  m_pJoinFestaPlayerListSelectMultiViewObjects->CreateView( m_pJoinFestaPlayerListSelectMultiWork, *m_pNetAppMessageMenu, *m_pNetAppTrainerIconUtility );
}


void JoinFestaPlayerListSelectMultiApplicationSystem::UpdateView()
{
  m_pJoinFestaPlayerListSelectMultiViewList->Update();
}


void JoinFestaPlayerListSelectMultiApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaPlayerListSelectMultiViewList->Draw( displayNo );
}


bool JoinFestaPlayerListSelectMultiApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaPlayerListSelectMultiViewObjects )
  {
    if( m_pJoinFestaPlayerListSelectMultiViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)
