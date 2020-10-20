// ============================================================================
/*
 * @file GameSyncApplicationSystem.cpp
 * @brief ゲームシンクアプリのアプリケーションシステムです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/System/GameSyncApplicationSystem.h"
#include "NetApp/GameSync/source/System/GameSyncViewList.h"
#include "NetApp/GameSync/source/System/GameSyncViewObjects.h"
#include "NetApp/GameSync/source/System/GameSyncViewSystemAccessor.h"
#include "NetApp/GameSync/source/System/GameSyncBuilder.h"
#include "NetApp/GameSync/source/GameSyncResourceID.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/gamesync.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


GameSyncApplicationSystem::GameSyncApplicationSystem() :
  m_pGameSyncWork( NULL ),
  m_pGameSyncViewList( NULL ),
  m_pGameSyncViewObjects( NULL ),
  m_pGameSyncViewSystemAccessor( NULL )
{
}


GameSyncApplicationSystem::~GameSyncApplicationSystem()
{
  if( m_pGameSyncViewList )
  {
    m_pGameSyncViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pGameSyncViewSystemAccessor );
  GFL_SAFE_DELETE( m_pGameSyncViewObjects );
  GFL_SAFE_DELETE( m_pGameSyncViewList );
}


void GameSyncApplicationSystem::Build( NetApp::GameSync::System::GameSyncWork* pGameSyncWork, NetApp::GameSync::System::GameSyncBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pGameSyncWork->GetAppHeap();

  m_pGameSyncWork               = pGameSyncWork;
  m_pGameSyncViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::System::GameSyncViewList( pAppHeap );
  m_pGameSyncViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::System::GameSyncViewObjects();
  m_pGameSyncViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::System::GameSyncViewSystemAccessor( m_pGameSyncViewList, m_pGameSyncViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pGameSyncViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void GameSyncApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::GameSync::GAMESYNC_RESOURCE_ID_LAYOUT, ARCID_GAMESYNC, GARC_gamesync_gamesync_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::GameSync::GAMESYNC_RESOURCE_ID_MESSAGE_GAMESYNC, print::GetMessageArcId(), GARC_message_gamesync_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::GameSync::GAMESYNC_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void GameSyncApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_gamesync_DAT, m_pResourceManager->GetResourceBuffer( NetApp::GameSync::GAMESYNC_RESOURCE_ID_MESSAGE_GAMESYNC ) );
  m_pGameSyncWork->SetMessageUtility( m_pMessageUtility );
}


void GameSyncApplicationSystem::CreateView()
{
  m_pGameSyncWork->SetTrainerIconUtility( m_pNetAppTrainerIconUtility );

  m_pGameSyncViewObjects->CreateView( m_pGameSyncWork, *m_pNetAppMessageMenu );
}


void GameSyncApplicationSystem::UpdateView()
{
  m_pGameSyncViewList->Update();
}


void GameSyncApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pGameSyncViewList->Draw( displayNo );
}


bool GameSyncApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pGameSyncViewObjects )
  {
    if( m_pGameSyncViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
