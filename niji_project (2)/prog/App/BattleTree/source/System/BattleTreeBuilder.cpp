// ============================================================================
/*
 * @file BattleTreeBuilder.cpp
 * @brief バトルツリーアプリを構築するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/System/BattleTreeBuilder.h"
#include "App/BattleTree/source/System/BattleTreeFrameSwitcher.h"
#include "App/BattleTree/source/System/BattleTreeFrameFactory.h"
#include "App/BattleTree/source/BattleTreeResourceID.h"
#include "App/BattleTree/source/BattleTreeFrameID.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  BattleTreeBuilder::BattleTreeBuilder( App::BattleTree::System::BattleTreeWork* pBattleTreeWork ) :
    m_pBattleTreeWork( pBattleTreeWork )
  {
  }


  BattleTreeBuilder::~BattleTreeBuilder()
  {
  }


  NetAppLib::System::IApplicationFrameSwitcher* BattleTreeBuilder::CreateApplicationFrameSwitcher()
  {
    app::util::Heap* pAppHeap = m_pBattleTreeWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::System::BattleTreeFrameSwitcher();
  }


  NetAppLib::System::IApplicationFrameFactory* BattleTreeBuilder::CreateApplicationFrameFactory()
  {
    app::util::Heap* pAppHeap = m_pBattleTreeWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::System::BattleTreeFrameFactory( m_pBattleTreeWork );
  }


  NetAppLib::System::ApplicationWorkBase* BattleTreeBuilder::GetApplicationWork()
  {
    return m_pBattleTreeWork;
  }


  NetAppLib::System::FrameID BattleTreeBuilder::GetApplicationStartFrameID()
  {
    return App::BattleTree::BATTLE_TREE_FRAME_ID;
  }


  u32 BattleTreeBuilder::GetApplicationResourceCount()
  {
    return App::BattleTree::BATTLE_TREE_RESOURCE_MAX;
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  app::util::AppRenderingManager::Description*  BattleTreeBuilder::CreateAppRenderingManagerDescription()
  {
    app::util::Heap* pAppHeap = m_pBattleTreeWork->GetAppHeap();

    app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description();
    //  レンダー環境を設定
    //  上下ともにレイアウトをしか使わない
    pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder = app::util::AppRenderingManager::RenderOrderType::NONE;
    pDescription->isUpperDisplayStereo                                                                     = false;
    return pDescription;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
