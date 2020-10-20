// ============================================================================
/*
 * @file MiracleTradeBuilder.cpp
 * @brief ミラクル交換アプリを構築するクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeBuilder.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeFrameSwitcher.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeFrameFactory.h"
#include "NetApp/MiracleTrade/source/MiracleTradeResourceID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


MiracleTradeBuilder::MiracleTradeBuilder(
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork,
  NetAppLib::System::FrameID startFrameID ) :
  m_pMiracleTradeWork( pMiracleTradeWork ),
  m_StartFrameID( startFrameID ),
  m_pMiracleTradeViewObjects( NULL )
{
}


MiracleTradeBuilder::~MiracleTradeBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* MiracleTradeBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::ApplicationSystem::MiracleTradeFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* MiracleTradeBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::ApplicationSystem::MiracleTradeFrameFactory( m_pMiracleTradeWork, m_pMiracleTradeViewObjects );
}


app::util::AppRenderingManager::Description* MiracleTradeBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].lytWkMax    = 32;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_EFFECT_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].lytWkMax    = 32;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].lytWkMax       = 32;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


NetAppLib::System::ApplicationWorkBase* MiracleTradeBuilder::GetApplicationWork()
{
  return m_pMiracleTradeWork;
}


NetAppLib::System::FrameID MiracleTradeBuilder::GetApplicationStartFrameID()
{
  return m_StartFrameID;
}


u32 MiracleTradeBuilder::GetApplicationResourceCount()
{
  return NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_MAX;
}


bool MiracleTradeBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  MiracleTradeBuilder::GetTrainerIconNum()
{
  return NetApp::MiracleTrade::MIRACLETRADE_TRAINER_ICON_NUM;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE MiracleTradeBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_LOWER;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
