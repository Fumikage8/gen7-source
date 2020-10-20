// ============================================================================
/*
 * @file StaffrollBuilder.cpp
 * @brief スタッフロールアプリを構築するクラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/System/StaffrollBuilder.h"
#include "App/Staffroll/source/System/StaffrollFrameSwitcher.h"
#include "App/Staffroll/source/System/StaffrollFrameFactory.h"
#include "App/Staffroll/source/StaffrollResourceID.h"
#include "App/Staffroll/source/StaffrollFrameID.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)


  StaffrollBuilder::StaffrollBuilder( App::Staffroll::System::StaffrollWork* pStaffrollWork ) :
    m_pWork( pStaffrollWork )
  {
  }


  StaffrollBuilder::~StaffrollBuilder()
  {
  }


  NetAppLib::System::IApplicationFrameSwitcher* StaffrollBuilder::CreateApplicationFrameSwitcher()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::System::StaffrollFrameSwitcher();
  }


  NetAppLib::System::IApplicationFrameFactory* StaffrollBuilder::CreateApplicationFrameFactory()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::System::StaffrollFrameFactory( m_pWork );
  }


  NetAppLib::System::ApplicationWorkBase* StaffrollBuilder::GetApplicationWork()
  {
    return m_pWork;
  }


  app::util::AppRenderingManager::Description* StaffrollBuilder::CreateAppRenderingManagerDescriptions()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

    app::util::AppRenderingManager::Description* pDescriptions = GFL_NEW_ARRAY( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description[ RENDER_DESCRIPTION_MAX ];

    app::util::AppRenderingManager::Description*  pActiveDescription  = NULL;
    {
      pActiveDescription  = &pDescriptions[ RENDER_DESCRIPTION_NORMAL ];
      pActiveDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      pActiveDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    }
    
    return pDescriptions;
  }

  NetAppLib::System::FrameID StaffrollBuilder::GetApplicationStartFrameID()
  {
    return App::Staffroll::STAFFROLL_FRAME_ID_EVENT;
  }


  u32 StaffrollBuilder::GetApplicationResourceCount()
  {
    return App::Staffroll::STAFFROLL_RESOURCE_MAX;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
