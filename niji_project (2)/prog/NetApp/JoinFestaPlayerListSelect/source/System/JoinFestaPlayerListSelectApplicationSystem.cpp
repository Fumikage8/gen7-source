// ============================================================================
/*
 * @file JoinFestaPlayerListSelectApplicationSystem.cpp
 * @brief プレイヤー単体選択アプリのアプリケーションシステムです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectApplicationSystem.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectViewList.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectViewObjects.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectViewSystemAccessor.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectBuilder.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectApplicationSystem::JoinFestaPlayerListSelectApplicationSystem() :
  m_pJoinFestaPlayerListSelectWork( NULL ),
  m_pJoinFestaPlayerListSelectViewList( NULL ),
  m_pJoinFestaPlayerListSelectViewObjects( NULL ),
  m_pJoinFestaPlayerListSelectViewSystemAccessor( NULL )
{
}


JoinFestaPlayerListSelectApplicationSystem::~JoinFestaPlayerListSelectApplicationSystem()
{
  if( m_pJoinFestaPlayerListSelectViewObjects )
  {
    m_pJoinFestaPlayerListSelectViewObjects->RemoveFromSuperViewAll();
  }

  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectViewList );
}


void JoinFestaPlayerListSelectApplicationSystem::Build( NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork, NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerListSelectWork->GetAppHeap();

  m_pJoinFestaPlayerListSelectWork               = pJoinFestaPlayerListSelectWork;
  m_pJoinFestaPlayerListSelectViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewList( pAppHeap );
  m_pJoinFestaPlayerListSelectViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewObjects();
  m_pJoinFestaPlayerListSelectViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor( m_pJoinFestaPlayerListSelectViewList, m_pJoinFestaPlayerListSelectViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaPlayerListSelectViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaPlayerListSelectApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTAPLAYERLISTSELECT, print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaPlayerListSelectApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_playerlist_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTAPLAYERLISTSELECT ) );
  m_pJoinFestaPlayerListSelectWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaPlayerListSelectApplicationSystem::CreateView()
{
  m_pJoinFestaPlayerListSelectViewObjects->CreateView( m_pJoinFestaPlayerListSelectWork, *m_pNetAppMessageMenu, *m_pNetAppTrainerIconUtility );
}


void JoinFestaPlayerListSelectApplicationSystem::UpdateView()
{
  m_pJoinFestaPlayerListSelectViewList->Update();
}


void JoinFestaPlayerListSelectApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaPlayerListSelectViewList->Draw( displayNo );
}


bool JoinFestaPlayerListSelectApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaPlayerListSelectViewObjects )
  {
    if( m_pJoinFestaPlayerListSelectViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
