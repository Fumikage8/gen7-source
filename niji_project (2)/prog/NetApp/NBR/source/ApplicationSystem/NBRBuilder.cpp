// ============================================================================
/*
 * @file NBRBuilder.cpp
 * @brief 通信対戦受付アプリを構築するクラスです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/ApplicationSystem/NBRBuilder.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRFrameSwitcher.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRFrameFactory.h"
#include "NetApp/NBR/source/NBRResourceID.h"
#include "NetApp/NBR/source/NBRFrameID.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


NBRBuilder::NBRBuilder( APP_PARAM* pAppParam, NBRWork* pNBRWork ):
  m_pAppParam( pAppParam ),
  m_pNBRWork( pNBRWork ),
  m_pParentView( NULL )
{
}


NBRBuilder::~NBRBuilder()
{
}

NetAppLib::System::IApplicationFrameSwitcher* NBRBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pNBRWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::NBR::ApplicationSystem::NBRFrameSwitcher( m_pAppParam, m_pNBRWork );
}


NetAppLib::System::IApplicationFrameFactory* NBRBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pNBRWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::NBR::ApplicationSystem::NBRFrameFactory( m_pParentView, m_pNBRWork, m_pAppParam->pNet );
}

app::util::AppRenderingManager::Description* NBRBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pNBRWork->GetAppHeap();
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
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder         = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].lytWkMax            = 8;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].pokeModelNumMax     = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].modelNumMax         = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].charaModelNumMax    = 0;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].dressUpModelNumMax  = 4;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}

NetAppLib::System::ApplicationWorkBase* NBRBuilder::GetApplicationWork()
{
  return m_pNBRWork;
}

NetAppLib::System::FrameID NBRBuilder::GetApplicationStartFrameID()
{
  NetAppLib::System::FrameID startFrameId = NBR_FRAME_ID_RULE_SELECTOR_PRIORITY;

  // 起動モード
  switch( m_pAppParam->startupMode ){
  case STARTUP_FIRST:             startFrameId = NBR_FRAME_ID_RULE_SELECTOR_PRIORITY; break;
  case STARTUP_RULE_SELECT_DECIDE:startFrameId = NBR_FRAME_ID_RULE_SELECT_APP_END; break;
  case STARTUP_RULE_SELECT_CANCEL:startFrameId = NBR_FRAME_ID_RULE_SELECT_APP_END_CANCEL; break;
  case STARTUP_TEAM_SELECT_AGAIN: startFrameId = NBR_FRAME_ID_TEAM_SELECT; break;
  case STARTUP_BATTLE_AGAIN:      startFrameId = NBR_FRAME_ID_CONTINUE; break;
  case STARTUP_DISCONNECT_OCCUR:
    //! バトル後の起動（ロイヤルで切断発生していた場合）
    {
      m_pNBRWork->m_appEndFrameEndType = NBRWork::END_TYPE_OPPONENT;
      startFrameId = NBR_FRAME_ID_APP_END;
    }
    break;
  }

  return startFrameId;
}


u32 NBRBuilder::GetApplicationResourceCount()
{
  return NetApp::NBR::NBR_RESOURCE_MAX;
}

bool NBRBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  NBRBuilder::GetTrainerIconNum()
{
  return 4;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE NBRBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_UPPER;
}

NBRWork* NBRBuilder::GetWork()
{
  return m_pNBRWork;
}

void NBRBuilder::SetParentView( View::NBRParentView* pParentView )
{
  m_pParentView = pParentView;
}

GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
