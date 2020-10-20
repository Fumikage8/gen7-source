// ============================================================================
/*
 * @file JoinFestaPlayerDetailBuilder.cpp
 * @brief 詳細アプリを構築するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailBuilder.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailFrameFactory.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailResourceID.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerDetailBuilder::JoinFestaPlayerDetailBuilder( NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork ) :
  m_pJoinFestaPlayerDetailWork( pJoinFestaPlayerDetailWork ),
  m_pJoinFestaPlayerDetailViewSystemAccessor( NULL )
{
}


JoinFestaPlayerDetailBuilder::~JoinFestaPlayerDetailBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaPlayerDetailBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerDetailWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaPlayerDetailBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerDetailWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailFrameFactory( m_pJoinFestaPlayerDetailWork, m_pJoinFestaPlayerDetailViewSystemAccessor );
}


app::util::AppRenderingManager::Description* JoinFestaPlayerDetailBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerDetailWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


NetAppLib::System::ApplicationWorkBase* JoinFestaPlayerDetailBuilder::GetApplicationWork()
{
  return m_pJoinFestaPlayerDetailWork;
}


NetAppLib::System::FrameID JoinFestaPlayerDetailBuilder::GetApplicationStartFrameID()
{
  return NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_FRAME_ID_DETAIL;
}


u32 JoinFestaPlayerDetailBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_MAX;
}


bool JoinFestaPlayerDetailBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaPlayerDetailBuilder::IsCreateAppCountryRegionNameUtility()
{
  return true;
}


bool JoinFestaPlayerDetailBuilder::IsCreateAppCommonGrpIconData()
{
  return true;
}


bool JoinFestaPlayerDetailBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  JoinFestaPlayerDetailBuilder::GetTrainerIconNum()
{
  return 1;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE JoinFestaPlayerDetailBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_UPPER;
}


bool JoinFestaPlayerDetailBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
