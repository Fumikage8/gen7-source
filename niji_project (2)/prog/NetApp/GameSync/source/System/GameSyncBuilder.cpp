// ============================================================================
/*
 * @file GameSyncBuilder.cpp
 * @brief ゲームシンクアプリを構築するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/System/GameSyncBuilder.h"
#include "NetApp/GameSync/source/System/GameSyncFrameSwitcher.h"
#include "NetApp/GameSync/source/System/GameSyncFrameFactory.h"
#include "NetApp/GameSync/source/GameSyncResourceID.h"
#include "NetApp/GameSync/source/GameSyncFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


GameSyncBuilder::GameSyncBuilder( NetApp::GameSync::System::GameSyncWork* pGameSyncWork ) :
  m_pGameSyncWork( pGameSyncWork ),
  m_pGameSyncViewSystemAccessor( NULL )
{
}


GameSyncBuilder::~GameSyncBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* GameSyncBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pGameSyncWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::System::GameSyncFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* GameSyncBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pGameSyncWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::System::GameSyncFrameFactory( m_pGameSyncWork, m_pGameSyncViewSystemAccessor );
}


NetAppLib::System::ApplicationWorkBase* GameSyncBuilder::GetApplicationWork()
{
  return m_pGameSyncWork;
}


NetAppLib::System::FrameID GameSyncBuilder::GetApplicationStartFrameID()
{
  return NetApp::GameSync::GAMESYNC_FRAME_ID_MAIN;
}


u32 GameSyncBuilder::GetApplicationResourceCount()
{
  return NetApp::GameSync::GAMESYNC_RESOURCE_MAX;
}


bool GameSyncBuilder::IsCreateMessageUtility()
{
  return true;
}


bool GameSyncBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool GameSyncBuilder::IsCreateAppCommonGrpIconData()
{
  return true;
}

bool GameSyncBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}

app::util::AppRenderingManager::Description* GameSyncBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pGameSyncWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


u32  GameSyncBuilder::GetTrainerIconNum()
{ 
  return 1; 
}

app::util::G2DUtil::SETUP_DISPLAY_TYPE GameSyncBuilder::GetTrainerIconDrawDisplayType()
{ 
  return app::util::G2DUtil::SETUP_UPPER; 
}

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
