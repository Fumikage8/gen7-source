// ============================================================================
/*
 * @file JoinFestaProfileApplicationSystem.cpp
 * @brief プロフィールアプリのアプリケーションシステムです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileApplicationSystem.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewList.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewObjects.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewSystemAccessor.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileBuilder.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_profile.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


JoinFestaProfileApplicationSystem::JoinFestaProfileApplicationSystem() :
  m_pJoinFestaProfileWork( NULL ),
  m_pJoinFestaProfileViewList( NULL ),
  m_pJoinFestaProfileViewObjects( NULL ),
  m_pJoinFestaProfileViewSystemAccessor( NULL )
{
}


JoinFestaProfileApplicationSystem::~JoinFestaProfileApplicationSystem()
{
  if( m_pJoinFestaProfileViewList )
  {
    m_pJoinFestaProfileViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaProfileViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaProfileViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaProfileViewList );
}


void JoinFestaProfileApplicationSystem::Build( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork, NetApp::JoinFestaProfile::System::JoinFestaProfileBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaProfileWork->GetAppHeap();

  m_pJoinFestaProfileWork               = pJoinFestaProfileWork;
  m_pJoinFestaProfileViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::System::JoinFestaProfileViewList( pAppHeap );
  m_pJoinFestaProfileViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::System::JoinFestaProfileViewObjects();
  m_pJoinFestaProfileViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor( m_pJoinFestaProfileViewList, m_pJoinFestaProfileViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaProfileViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaProfileApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PROFILE, GARC_join_festa_profile_join_festa_profile_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_LAYOUT_PLAYER_LIST, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_MESSAGE_JOINFESTAPROFILE, print::GetMessageArcId(), GARC_message_jf_myprofile_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_MESSAGE_JF_PHRASE, print::GetMessageArcId(), GARC_message_jf_phrase_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaProfileApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_myprofile_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_MESSAGE_JOINFESTAPROFILE ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_phrase_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_MESSAGE_JF_PHRASE ) );
  m_pJoinFestaProfileWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaProfileApplicationSystem::CreateView()
{
  m_pJoinFestaProfileViewObjects->CreateView( m_pJoinFestaProfileWork, *m_pNetAppMessageMenu );
}


void JoinFestaProfileApplicationSystem::UpdateView()
{
  m_pJoinFestaProfileViewList->Update();
}


void JoinFestaProfileApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaProfileViewList->Draw( displayNo );
}


bool JoinFestaProfileApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaProfileViewObjects )
  {
    if( m_pJoinFestaProfileViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
