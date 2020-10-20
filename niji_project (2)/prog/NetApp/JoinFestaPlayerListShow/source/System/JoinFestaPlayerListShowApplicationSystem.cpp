// ============================================================================
/*
 * @file JoinFestaPlayerListShowApplicationSystem.cpp
 * @brief 閲覧アプリのアプリケーションシステムです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowApplicationSystem.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowViewList.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowViewObjects.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowViewSystemAccessor.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowBuilder.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListShowApplicationSystem::JoinFestaPlayerListShowApplicationSystem() :
  m_pJoinFestaPlayerListShowWork( NULL ),
  m_pJoinFestaPlayerListShowViewList( NULL ),
  m_pJoinFestaPlayerListShowViewObjects( NULL ),
  m_pJoinFestaPlayerListShowViewSystemAccessor( NULL )
{
}


JoinFestaPlayerListShowApplicationSystem::~JoinFestaPlayerListShowApplicationSystem()
{
  if( m_pJoinFestaPlayerListShowViewList )
  {
    m_pJoinFestaPlayerListShowViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaPlayerListShowViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListShowViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListShowViewList );
}


void JoinFestaPlayerListShowApplicationSystem::Build( NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerListShowWork->GetAppHeap();

  m_pJoinFestaPlayerListShowWork               = pJoinFestaPlayerListShowWork;
  m_pJoinFestaPlayerListShowViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewList( pAppHeap );
  m_pJoinFestaPlayerListShowViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewObjects();
  m_pJoinFestaPlayerListShowViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor( m_pJoinFestaPlayerListShowViewList, m_pJoinFestaPlayerListShowViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaPlayerListShowViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaPlayerListShowApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST, print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA, print::GetMessageArcId(), GARC_message_jf_questiondata_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaPlayerListShowApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_playerlist_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_questiondata_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA ) );
  m_pJoinFestaPlayerListShowWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaPlayerListShowApplicationSystem::CreateView()
{
  m_pJoinFestaPlayerListShowViewObjects->CreateView( m_pJoinFestaPlayerListShowWork, *m_pNetAppMessageMenu, *m_pNetAppTrainerIconUtility );
}


void JoinFestaPlayerListShowApplicationSystem::UpdateView()
{
  m_pJoinFestaPlayerListShowViewList->Update();
}


void JoinFestaPlayerListShowApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaPlayerListShowViewList->Draw( displayNo );
}


bool JoinFestaPlayerListShowApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaPlayerListShowViewObjects )
  {
    if( m_pJoinFestaPlayerListShowViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
