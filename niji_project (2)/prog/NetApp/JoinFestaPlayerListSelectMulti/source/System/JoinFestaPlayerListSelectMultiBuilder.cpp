// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiBuilder.cpp
 * @brief プレイヤー複数選択アプリを構築するクラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiBuilder.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiFrameFactory.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiResourceID.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectMultiBuilder::JoinFestaPlayerListSelectMultiBuilder( NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork ) :
  m_pJoinFestaPlayerListSelectMultiWork( pJoinFestaPlayerListSelectMultiWork ),
  m_pJoinFestaPlayerListSelectMultiViewSystemAccessor( NULL )
{
}


JoinFestaPlayerListSelectMultiBuilder::~JoinFestaPlayerListSelectMultiBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaPlayerListSelectMultiBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectMultiWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaPlayerListSelectMultiBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectMultiWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiFrameFactory( m_pJoinFestaPlayerListSelectMultiWork, m_pJoinFestaPlayerListSelectMultiViewSystemAccessor );
}


app::util::AppRenderingManager::Description* JoinFestaPlayerListSelectMultiBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectMultiWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


NetAppLib::System::ApplicationWorkBase* JoinFestaPlayerListSelectMultiBuilder::GetApplicationWork()
{
  return m_pJoinFestaPlayerListSelectMultiWork;
}


NetAppLib::System::FrameID JoinFestaPlayerListSelectMultiBuilder::GetApplicationStartFrameID()
{
  return NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_FRAME_ID_SELECT;
}


u32 JoinFestaPlayerListSelectMultiBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_MAX;
}


bool JoinFestaPlayerListSelectMultiBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaPlayerListSelectMultiBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaPlayerListSelectMultiBuilder::IsCreateAppCountryRegionNameUtility()
{
  return true;
}


bool JoinFestaPlayerListSelectMultiBuilder::IsCreateAppCommonGrpIconData()
{
  return true;
}


bool JoinFestaPlayerListSelectMultiBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  JoinFestaPlayerListSelectMultiBuilder::GetTrainerIconNum()
{
  return 1;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE JoinFestaPlayerListSelectMultiBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_UPPER;
}


bool JoinFestaPlayerListSelectMultiBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)
