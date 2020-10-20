// ============================================================================
/*
 * @file JoinFestaBeaconApplicationSystem.cpp
 * @brief ビーコンアプリのアプリケーションシステムです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconApplicationSystem.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconBuilder.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconResourceID.h"

#include <arc_def_index/arc_def.h>

#include <arc_index/join_festa_beacon.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include "arc_index/message.gaix"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


JoinFestaBeaconApplicationSystem::JoinFestaBeaconApplicationSystem()
: m_pJoinFestaBeaconWork( NULL )
, m_pJoinFestaBeaconUpperView( NULL )
, m_pJoinFestaBeaconLowerView( NULL )
, m_pJoinFestaPlayerListBGLowerView( NULL )
, m_pJoinFestaPlayerListBGUpperView( NULL )
{
}


JoinFestaBeaconApplicationSystem::~JoinFestaBeaconApplicationSystem()
{
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaBeaconUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaBeaconLowerView );
}


void JoinFestaBeaconApplicationSystem::Build( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork, NetApp::JoinFestaBeacon::System::JoinFestaBeaconBuilder* pApplicationSystemBuilder )
{
  m_pJoinFestaBeaconWork = pJoinFestaBeaconWork;

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaBeaconApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_BEACON, GARC_join_festa_beacon_join_festa_beacon_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_LAYOUT_BG, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_MSG, print::GetMessageArcId(), GARC_message_jf_menu_DAT, false, true );
}


void JoinFestaBeaconApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_menu_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_MSG ) );
  m_pJoinFestaBeaconWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaBeaconApplicationSystem::CreateView()
{
  app::util::Heap* pAppHeap = m_pJoinFestaBeaconWork->GetAppHeap();

  m_pJoinFestaBeaconWork->SetTrainerIconUtility( m_pNetAppTrainerIconUtility );

  m_pJoinFestaBeaconUpperView = GFL_NEW( pAppHeap->GetDeviceHeap() ) View::JoinFestaBeaconUpperView( m_pJoinFestaBeaconWork , pAppHeap );
  m_pJoinFestaBeaconLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) View::JoinFestaBeaconLowerView( m_pJoinFestaBeaconWork , *m_pNetAppMessageMenu , pAppHeap , m_pJoinFestaBeaconUpperView );

  m_pJoinFestaBeaconWork->SetView( m_pJoinFestaBeaconUpperView , m_pJoinFestaBeaconLowerView );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( m_pJoinFestaBeaconWork, NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_LAYOUT_BG );
  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( m_pJoinFestaBeaconWork, NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_LAYOUT_BG );
}


void JoinFestaBeaconApplicationSystem::UpdateView()
{
  m_pJoinFestaPlayerListBGLowerView->UpdateTree();
  m_pJoinFestaPlayerListBGUpperView->UpdateTree();
  m_pJoinFestaBeaconUpperView->UpdateTree();
  m_pJoinFestaBeaconLowerView->UpdateTree();
}


void JoinFestaBeaconApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaPlayerListBGLowerView->DrawTree( displayNo );
  m_pJoinFestaPlayerListBGUpperView->DrawTree( displayNo );
  m_pJoinFestaBeaconUpperView->DrawTree( displayNo );
  m_pJoinFestaBeaconLowerView->DrawTree( displayNo );
}


bool JoinFestaBeaconApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaBeaconUpperView )
  {
    if( m_pJoinFestaBeaconUpperView->IsDrawing() )
    {
      bExit = false;
    }
  }

  if( m_pJoinFestaBeaconLowerView )
  {
    if( m_pJoinFestaBeaconLowerView->IsDrawing() )
    {
      bExit = false;
    }
  }

  if( m_pJoinFestaPlayerListBGLowerView )
  {
    if( m_pJoinFestaPlayerListBGLowerView->IsDrawing() )
    {
      bExit = false;
    }
  }

  if( m_pJoinFestaPlayerListBGUpperView )
  {
    if( m_pJoinFestaPlayerListBGUpperView->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)
