// ============================================================================
/*
 * @file Builder.cpp
 * @brief きせかえアプリで使うビルダークラス
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/System/KisekaeBuilder.h"
#include "App/Kisekae/source/System/KisekaeFrameSwitcher.h"
#include "App/Kisekae/source/System/KisekaeFrameFactory.h"
#include "App/Kisekae/source/KisekaeResourceID.h"
#include "App/Kisekae/source/KisekaeFrameID.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

  Builder::Builder( App::Kisekae::System::Work* pWork )
  {
    GFL_ASSERT( pWork );
    m_pWork = pWork;
  }

  Builder::~Builder()
  {
  }

  NetAppLib::System::IApplicationFrameSwitcher* Builder::CreateApplicationFrameSwitcher()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::System::FrameSwitcher();
  }

  NetAppLib::System::IApplicationFrameFactory* Builder::CreateApplicationFrameFactory()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::System::FrameFactory( m_pWork );
  }

  NetAppLib::System::ApplicationWorkBase* Builder::GetApplicationWork()
  {
    return m_pWork;
  }

  NetAppLib::System::FrameID Builder::GetApplicationStartFrameID()
  {
    APP_PARAM*  pAppParam = m_pWork->GetAppParam();
    GFL_ASSERT( pAppParam );

    if( pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_SHOP )
    {
      return App::Kisekae::KISEKAE_FRAME_ID_SHOP;
    }
    else
    if( pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_DRESS_UP )
    {
      return App::Kisekae::KISEKAE_FRAME_ID_DRESS_UP;
    }
    else
    if( pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_COLOR_SHOP )
    {
      return  App::Kisekae::KISEKAE_FRAME_ID_COLOR_DRESS_UP;
    }
    else
    if( pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_FINDER_STUDIO )
    {
      return  App::Kisekae::KISEKAE_FRAME_ID_DRESS_UP;
    }
    else
    {
      //  ここにきたらアウト！
      GFL_ASSERT( 0 );
    }

    return App::Kisekae::KISEKAE_FRAME_ID_SHOP;
  }

  u32 Builder::GetApplicationResourceCount()
  {
    return App::Kisekae::RESOURCE_MAX;
  }

  u32 Builder::CreateAppRenderingManagerCount()
  {
    return RENDER_DESCRIPTION_MAX;
  }

  app::util::AppRenderingManager::Description* Builder::CreateAppRenderingManagerDescriptions()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

    app::util::AppRenderingManager::Description* pDescriptions = GFL_NEW_ARRAY( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description[ RENDER_DESCRIPTION_MAX ];

    app::util::AppRenderingManager::Description*  pActiveDescription  = NULL;
    {
      pActiveDescription  = &pDescriptions[ RENDER_DESCRIPTION_NORMAL ];
      pActiveDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
      pActiveDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      pActiveDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::NONE;
    }
    
    return pDescriptions;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
