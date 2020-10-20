// ============================================================================
/*
 * @file JoinFestaPlayerListSelectBuilder.cpp
 * @brief プレイヤー単体選択アプリを構築するクラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectBuilder.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectFrameFactory.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectResourceID.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectBuilder::JoinFestaPlayerListSelectBuilder( NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork ) :
  m_pJoinFestaPlayerListSelectWork( pJoinFestaPlayerListSelectWork ),
  m_pJoinFestaPlayerListSelectViewSystemAccessor( NULL )
{
}


JoinFestaPlayerListSelectBuilder::~JoinFestaPlayerListSelectBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaPlayerListSelectBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaPlayerListSelectBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectFrameFactory( m_pJoinFestaPlayerListSelectWork, m_pJoinFestaPlayerListSelectViewSystemAccessor );
}


app::util::AppRenderingManager::Description* JoinFestaPlayerListSelectBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


NetAppLib::System::ApplicationWorkBase* JoinFestaPlayerListSelectBuilder::GetApplicationWork()
{
  return m_pJoinFestaPlayerListSelectWork;
}


NetAppLib::System::FrameID JoinFestaPlayerListSelectBuilder::GetApplicationStartFrameID()
{
  return NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_FRAME_ID_SELECT;
}


u32 JoinFestaPlayerListSelectBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_MAX;
}


bool JoinFestaPlayerListSelectBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaPlayerListSelectBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaPlayerListSelectBuilder::IsCreateAppCountryRegionNameUtility()
{
  return true;
}


bool JoinFestaPlayerListSelectBuilder::IsCreateAppCommonGrpIconData()
{
  return true;
}


bool JoinFestaPlayerListSelectBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  JoinFestaPlayerListSelectBuilder::GetTrainerIconNum()
{
  return 1;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE JoinFestaPlayerListSelectBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_UPPER;
}


bool JoinFestaPlayerListSelectBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
