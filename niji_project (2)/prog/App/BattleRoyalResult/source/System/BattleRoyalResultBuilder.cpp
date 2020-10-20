// ============================================================================
/*
 * @file BattleRoyalResultBuilder.cpp
 * @brief バトルロイヤルの結果アプリを構築するクラスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/System/BattleRoyalResultBuilder.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultFrameSwitcher.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultFrameFactory.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultResourceID.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultFrameID.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)


  BattleRoyalResultBuilder::BattleRoyalResultBuilder( App::BattleRoyalResult::System::BattleRoyalResultWork* pBattleRoyalResultWork ) :
    m_pWork( pBattleRoyalResultWork )
  {
  }


  BattleRoyalResultBuilder::~BattleRoyalResultBuilder()
  {
  }


  NetAppLib::System::IApplicationFrameSwitcher* BattleRoyalResultBuilder::CreateApplicationFrameSwitcher()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::System::BattleRoyalResultFrameSwitcher();
  }


  NetAppLib::System::IApplicationFrameFactory* BattleRoyalResultBuilder::CreateApplicationFrameFactory()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::System::BattleRoyalResultFrameFactory( m_pWork );
  }


  NetAppLib::System::ApplicationWorkBase* BattleRoyalResultBuilder::GetApplicationWork()
  {
    return m_pWork;
  }


  app::util::AppRenderingManager::Description* BattleRoyalResultBuilder::CreateAppRenderingManagerDescriptions()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

    app::util::AppRenderingManager::Description* pDescriptions = GFL_NEW_ARRAY( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description[ RENDER_DESCRIPTION_MAX ];

    app::util::AppRenderingManager::Description*  pActiveDescription  = NULL;
    {
      pActiveDescription  = &pDescriptions[ RENDER_DESCRIPTION_NORMAL ];
      pActiveDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      pActiveDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      pActiveDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
    }
    
    return pDescriptions;
  }

  NetAppLib::System::FrameID BattleRoyalResultBuilder::GetApplicationStartFrameID()
  {
    return App::BattleRoyalResult::BATTLE_ROYAL_RESULT_FRAME_ID_EVENT;
  }


  u32 BattleRoyalResultBuilder::GetApplicationResourceCount()
  {
    return App::BattleRoyalResult::BATTLE_ROYAL_RESULT_RESOURCE_MAX;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
