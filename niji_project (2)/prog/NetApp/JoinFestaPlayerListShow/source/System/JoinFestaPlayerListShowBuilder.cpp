// ============================================================================
/*
 * @file JoinFestaPlayerListShowBuilder.cpp
 * @brief 閲覧アプリを構築するクラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowBuilder.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowFrameFactory.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowResourceID.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowFrameID.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// Save
#include "Savedata/include/JoinFestaDataSave.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListShowBuilder::JoinFestaPlayerListShowBuilder( NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork ) :
  m_pJoinFestaPlayerListShowWork( pJoinFestaPlayerListShowWork ),
  m_pJoinFestaPlayerListShowViewSystemAccessor( NULL )
{
}


JoinFestaPlayerListShowBuilder::~JoinFestaPlayerListShowBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaPlayerListShowBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListShowWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaPlayerListShowBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListShowWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowFrameFactory( m_pJoinFestaPlayerListShowWork, m_pJoinFestaPlayerListShowViewSystemAccessor );
}


app::util::AppRenderingManager::Description* JoinFestaPlayerListShowBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListShowWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


NetAppLib::System::ApplicationWorkBase* JoinFestaPlayerListShowBuilder::GetApplicationWork()
{
  return m_pJoinFestaPlayerListShowWork;
}


NetAppLib::System::FrameID JoinFestaPlayerListShowBuilder::GetApplicationStartFrameID()
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetAppLib::System::FrameID eStartFrameID = NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_SELECT;

  bool bIsFirstBootFlag = !pSave->GetPlayerListShowAppTutorialFlag(); // 初回起動フラグをセーブデータから参照する

  if( bIsFirstBootFlag )
  {
    pSave->SetPlayerListShowAppTutorialFlag();
    eStartFrameID = NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_DESCRIPTION;
  }

  return eStartFrameID;
}


u32 JoinFestaPlayerListShowBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_MAX;
}


bool JoinFestaPlayerListShowBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaPlayerListShowBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaPlayerListShowBuilder::IsCreateAppCountryRegionNameUtility()
{
  return true;
}


bool JoinFestaPlayerListShowBuilder::IsCreateAppCommonGrpIconData()
{
  return true;
}


bool JoinFestaPlayerListShowBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  JoinFestaPlayerListShowBuilder::GetTrainerIconNum()
{
  return 1;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE JoinFestaPlayerListShowBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_UPPER;
}


bool JoinFestaPlayerListShowBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
