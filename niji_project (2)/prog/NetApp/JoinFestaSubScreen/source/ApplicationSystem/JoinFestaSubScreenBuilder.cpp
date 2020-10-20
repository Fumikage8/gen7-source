// ============================================================================
/*
 * @file JoinFestaSubScreenBuilder.cpp
 * @brief ジョインフェスタ下画面を構築するクラスです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenBuilder.h"
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenFrameSwitcher.h"
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenFrameFactory.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"

// アトラクションマネージャ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


JoinFestaSubScreenBuilder::JoinFestaSubScreenBuilder( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork ) :
  m_pJoinFestaSubScreenWork( pJoinFestaSubScreenWork ),
  m_pJoinFestaSubScreenParentView( NULL )
{
}


JoinFestaSubScreenBuilder::~JoinFestaSubScreenBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaSubScreenBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaSubScreenWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaSubScreenBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaSubScreenWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenFrameFactory( m_pJoinFestaSubScreenParentView, m_pJoinFestaSubScreenWork );
}

app::util::AppRenderingManager::Description* JoinFestaSubScreenBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pJoinFestaSubScreenWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder         = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].lytWkMax            = 8;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].pokeModelNumMax     = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].modelNumMax         = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].charaModelNumMax    = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].dressUpModelNumMax  = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder         = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].lytWkMax            = 8;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].pokeModelNumMax     = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].modelNumMax         = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].charaModelNumMax    = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].dressUpModelNumMax  = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder         = app::util::AppRenderingManager::RenderOrderType::NONE;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].lytWkMax            = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].pokeModelNumMax     = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].modelNumMax         = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].charaModelNumMax    = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].dressUpModelNumMax  = 0;
  pDescription->isUpperDisplayStereo                                                          = false;
  return pDescription;
}

NetAppLib::System::ApplicationWorkBase* JoinFestaSubScreenBuilder::GetApplicationWork()
{
  return m_pJoinFestaSubScreenWork;
}

NetAppLib::System::FrameID JoinFestaSubScreenBuilder::GetApplicationStartFrameID()
{
  // BG表示のみ画面か（現在はRR団イベント中のみ）
  if( m_pJoinFestaSubScreenWork->IsBgOnly() )
  {
    return NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_BG_ONLY;
  }

  // チュートリアル中は専用画面
  if( m_pJoinFestaSubScreenWork->IsTutorial() )
  {
    return NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_TUTORIAL;
  }
  // アトラクション中は専用画面
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
  {
    return NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION;
  }
  return NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_MAIN;
}


u32 JoinFestaSubScreenBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_RESOURCE_MAX;
}

// MessageUtilytyを使用する
bool JoinFestaSubScreenBuilder::IsCreateMessageUtility()
{
  return true;
}

NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* JoinFestaSubScreenBuilder::GetWork()
{
  return m_pJoinFestaSubScreenWork;
}

GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
