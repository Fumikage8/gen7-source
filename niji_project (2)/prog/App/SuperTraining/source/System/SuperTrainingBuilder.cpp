// ============================================================================
/*
 * @file SuperTrainingBuilder.cpp
 * @brief すごい特訓アプリを構築するクラスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/source/System/SuperTrainingBuilder.h"
#include "App/SuperTraining/source/System/SuperTrainingFrameSwitcher.h"
#include "App/SuperTraining/source/System/SuperTrainingFrameFactory.h"
#include "App/SuperTraining/source/SuperTrainingResourceID.h"
#include "App/SuperTraining/source/SuperTrainingFrameID.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  SuperTrainingBuilder::SuperTrainingBuilder( App::SuperTraining::System::SuperTrainingWork* pSuperTrainingWork ) :
    m_pSuperTrainingWork( pSuperTrainingWork )
  {
  }


  SuperTrainingBuilder::~SuperTrainingBuilder()
  {
  }


  NetAppLib::System::IApplicationFrameSwitcher* SuperTrainingBuilder::CreateApplicationFrameSwitcher()
  {
    app::util::Heap* pAppHeap = m_pSuperTrainingWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::SuperTraining::System::SuperTrainingFrameSwitcher();
  }


  NetAppLib::System::IApplicationFrameFactory* SuperTrainingBuilder::CreateApplicationFrameFactory()
  {
    app::util::Heap* pAppHeap = m_pSuperTrainingWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::SuperTraining::System::SuperTrainingFrameFactory( m_pSuperTrainingWork );
  }


  NetAppLib::System::ApplicationWorkBase* SuperTrainingBuilder::GetApplicationWork()
  {
    return m_pSuperTrainingWork;
  }


  NetAppLib::System::FrameID SuperTrainingBuilder::GetApplicationStartFrameID()
  {
    return App::SuperTraining::SUPER_TRAINIG_FRAME_ID_MAIN;
  }


  u32 SuperTrainingBuilder::GetApplicationResourceCount()
  {
    return App::SuperTraining::SUPER_TRAINIMG_RESOURCE_MAX;
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  app::util::AppRenderingManager::Description*  SuperTrainingBuilder::CreateAppRenderingManagerDescription()
  {
    app::util::Heap* pAppHeap = m_pSuperTrainingWork->GetAppHeap();

    app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description();
    //  レンダー環境を設定
    //  下レイアウトしか使わない
    //  上はレイアウトとレーダチャートを使う
    pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART;
    pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder = app::util::AppRenderingManager::RenderOrderType::NONE;
    pDescription->isUpperDisplayStereo                                                                     = false;
    return pDescription;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)
