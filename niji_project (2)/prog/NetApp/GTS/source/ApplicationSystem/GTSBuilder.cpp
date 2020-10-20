// ============================================================================
/*
 * @file GTSBuilder.cpp
 * @brief GTSアプリを構築するクラスです。
 * @date 2015.03.26
 */
// ============================================================================
#include "NetApp/GTS/source/ApplicationSystem/GTSBuilder.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSFrameSwitcher.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSFrameFactory.h"
#include "NetApp/GTS/source/GTSResourceID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


GTSBuilder::GTSBuilder( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork, NetAppLib::System::FrameID startFrameID ) :
  m_pGTSWork( pGTSWork ),
  m_StartFrameID( startFrameID ),
  m_pGTSViewObjects( NULL )
{
}


GTSBuilder::~GTSBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* GTSBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::ApplicationSystem::GTSFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* GTSBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::ApplicationSystem::GTSFrameFactory( m_pGTSWork, m_pGTSViewObjects );
}


u32 GTSBuilder::CreateAppRenderingManagerCount()
{
  return 2;
}


app::util::AppRenderingManager::Description* GTSBuilder::CreateAppRenderingManagerDescriptions()
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::util::AppRenderingManager::Description* pDescriptions = GFL_NEW_ARRAY( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description[ 2 ]; 

  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL0_LAYOUT_POKEMODEL1;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART_POKEMODEL;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::NONE;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder = app::util::AppRenderingManager::RenderOrderType::NONE;
  
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].lytWkMax            = 8;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].pokeModelNumMax     = 1; // 上画面にポケモンモデル１体
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].modelNumMax         = 1; // 上画面に背景モデル１個
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].charaModelNumMax    = 0;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].dressUpModelNumMax  = 0;

  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].lytWkMax            = 8;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].pokeModelNumMax     = 0;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].modelNumMax         = 1; // 下画面に背景モデル１個
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].charaModelNumMax    = 0;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].dressUpModelNumMax  = 0;

  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].lytWkMax            = 0;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].pokeModelNumMax     = 0;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].modelNumMax         = 0;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].charaModelNumMax    = 0;
  pDescriptions[0].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].dressUpModelNumMax  = 5; // トレーナーアイコン５個

  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].lytWkMax            = 8;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].pokeModelNumMax     = 1; // Box画面の上画面のポケモンモデル１体
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].modelNumMax         = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].charaModelNumMax    = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].dressUpModelNumMax  = 0;

  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].lytWkMax            = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].pokeModelNumMax     = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].modelNumMax         = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].charaModelNumMax    = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].dressUpModelNumMax  = 0;

  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].lytWkMax            = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].pokeModelNumMax     = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].modelNumMax         = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].charaModelNumMax    = 0;
  pDescriptions[1].renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].dressUpModelNumMax  = 0;



  return pDescriptions;
}


NetAppLib::System::ApplicationWorkBase* GTSBuilder::GetApplicationWork()
{
  return m_pGTSWork;
}


NetAppLib::System::FrameID GTSBuilder::GetApplicationStartFrameID()
{
  return m_StartFrameID;
}


u32 GTSBuilder::GetApplicationResourceCount()
{
  return NetApp::GTS::GTS_RESOURCE_MAX;
}


bool GTSBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  GTSBuilder::GetTrainerIconNum()
{
  return NetApp::GTS::GTS_TRAINER_ICON_NUM;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE GTSBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_LOWER;
}


bool GTSBuilder::IsCreateAppCountryRegionNameUtility()
{
  return true;
}


bool GTSBuilder::IsCreateAppCommonGrpIconData()
{
  return true;
}


bool GTSBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
