// ============================================================================
/*
 * @file JoinFestaOpenListBuilder.cpp
 * @brief 開催選択アプリを構築するクラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListBuilder.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListFrameSwitcher.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListFrameFactory.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListResourceID.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


JoinFestaOpenListBuilder::JoinFestaOpenListBuilder( NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork ) :
  m_pJoinFestaOpenListWork( pJoinFestaOpenListWork ),
  m_pJoinFestaOpenListViewSystemAccessor( NULL )
{
}


JoinFestaOpenListBuilder::~JoinFestaOpenListBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaOpenListBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaOpenListWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaOpenList::System::JoinFestaOpenListFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaOpenListBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaOpenListWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaOpenList::System::JoinFestaOpenListFrameFactory( m_pJoinFestaOpenListWork, m_pJoinFestaOpenListViewSystemAccessor );
}


app::util::AppRenderingManager::Description* JoinFestaOpenListBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pJoinFestaOpenListWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


NetAppLib::System::ApplicationWorkBase* JoinFestaOpenListBuilder::GetApplicationWork()
{
  return m_pJoinFestaOpenListWork;
}


NetAppLib::System::FrameID JoinFestaOpenListBuilder::GetApplicationStartFrameID()
{
  return NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_FRAME_ID_SELECT;
}


u32 JoinFestaOpenListBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_MAX;
}


bool JoinFestaOpenListBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaOpenListBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaOpenListBuilder::IsCreateAppCountryRegionNameUtility()
{
  return true;
}


bool JoinFestaOpenListBuilder::IsCreateAppCommonGrpIconData()
{
  return true;
}


bool JoinFestaOpenListBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  JoinFestaOpenListBuilder::GetTrainerIconNum()
{
  return 1;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE JoinFestaOpenListBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_UPPER;
}


bool JoinFestaOpenListBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
