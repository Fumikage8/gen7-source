// ============================================================================
/*
 * @file QuickMatchBuilder.cpp
 * @brief クイックマッチアプリを構築するクラスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/System/QuickMatchBuilder.h"
#include "NetApp/QuickMatch/source/System/QuickMatchFrameSwitcher.h"
#include "NetApp/QuickMatch/source/System/QuickMatchFrameFactory.h"
#include "NetApp/QuickMatch/source/QuickMatchResourceID.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)


QuickMatchBuilder::QuickMatchBuilder( NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork ) :
  m_pQuickMatchWork( pQuickMatchWork )
{
}


QuickMatchBuilder::~QuickMatchBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* QuickMatchBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pQuickMatchWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::QuickMatch::System::QuickMatchFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* QuickMatchBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pQuickMatchWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::QuickMatch::System::QuickMatchFrameFactory( m_pQuickMatchWork );
}


NetAppLib::System::ApplicationWorkBase* QuickMatchBuilder::GetApplicationWork()
{
  return m_pQuickMatchWork;
}


NetAppLib::System::FrameID QuickMatchBuilder::GetApplicationStartFrameID()
{
  if( m_pQuickMatchWork->m_pAppParam->m_isBattleTreeMode == false )
  {
    return NetApp::QuickMatch::QUICKMATCH_FRAME_ID_INIT;
  }
  else
  {
    return NetApp::QuickMatch::QUICKMATCH_FRAME_ID_BATTLE_TREE;
  }
}


u32 QuickMatchBuilder::GetApplicationResourceCount()
{
  return NetApp::QuickMatch::QUICKMATCH_RESOURCE_MAX;
}


app::util::AppRenderingManager::Description* QuickMatchBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pQuickMatchWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
