//======================================================================
/**
 * @file PokeViewerListenerPage1.cpp
 * @date 2015/12/04 17:32:22
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "../include/PokeViewerListener.h"

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent_Page1(gfl2::ui::TouchPanel* pTouchPanel, bool isTouch)
{
  gfl2::lyt::LytPaneIndex ret = m_pView->GetG2DUtil()->GetTouchTriggerPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP);

  switch (ret)
  {
    // モーション選択
  case PANENAME_DOWN1_PANE_MOTION_RIGHT:
    m_pModelManager->SetMotionIndex(m_pModelManager->GetMotionIndex() + 1);
    break;

  case PANENAME_DOWN1_PANE_MOTION_LEFT:
    m_pModelManager->SetMotionIndex(m_pModelManager->GetMotionIndex() - 1);
    break;

    // モーションのループ設定切り替え
  case PANENAME_DOWN1_PANE_LOOP_00:
    m_pModelManager->SetMotionLoop(!m_pModelManager->GetMotionLoop());
    break;

    // モーションの再生/停止
  case PANENAME_DOWN1_PANE_MOTION_PLAY:
    m_pModelManager->PlayMotion(!m_pModelManager->IsPlaying());
    break;

    // モーションのステップ再生
  case PANENAME_DOWN1_PANE_MOTION_STEP:
    m_pModelManager->PlaySingleFrame(true);
    break;

  case PANENAME_DOWN1_PANE_MOTION_STEP2:
    m_pModelManager->PlaySingleFrame(false);
    break;

    // モーションの再生速度設定
  case PANENAME_DOWN1_PANE_MOTION_SPEED_U:
    m_pModelManager->SetMotionSpeed(gfl2::math::clamp(m_pModelManager->GetMotionSpeed() + 0.1f, 0.1f, 2.0f));
    break;

  case PANENAME_DOWN1_PANE_MOTION_SPEED_D:
    m_pModelManager->SetMotionSpeed(gfl2::math::clamp(m_pModelManager->GetMotionSpeed() - 0.1f, 0.1f, 2.0f));
    break;

    // 光源位置設定
  case PANENAME_DOWN1_PANE_LIGHT_DIR_LU:
    m_pModelManager->SetLightRotation(70.0f, 45.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_DIR_U:
    m_pModelManager->SetLightRotation(70.0f, 0.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_DIR_RU:
    m_pModelManager->SetLightRotation(70.0f, -45.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_DIR_L:
    m_pModelManager->SetLightRotation(50.0f, 45.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_DIR_C:
    m_pModelManager->SetLightRotation(50.0f, 0.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_DIR_R:
    m_pModelManager->SetLightRotation(50.0f, -45.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_DIR_LD:
    m_pModelManager->SetLightRotation(30.0f, 45.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_DIR_D:
    m_pModelManager->SetLightRotation(30.0f, 0.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_DIR_RD:
    m_pModelManager->SetLightRotation(30.0f, -45.0f);
    break;

  default:
    break;
  }

  gfl2::lyt::LytPaneIndex HoldPane = m_pView->GetG2DUtil()->GetTouchHoldPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP);

  // 光源位置設定
  switch (HoldPane)
  {
  case PANENAME_DOWN1_PANE_LIGHT_U:
    m_pModelManager->SetLightRotationV(gfl2::math::clamp(m_pModelManager->GetLightRotationV() - 1.0f, -90.f, 90.0f));
    break;

  case PANENAME_DOWN1_PANE_LIGHT_D:
    m_pModelManager->SetLightRotationV(gfl2::math::clamp(m_pModelManager->GetLightRotationV() + 1.0f, -90.f, 90.0f));
    break;

  case PANENAME_DOWN1_PANE_LIGHT_L:
    m_pModelManager->SetLightRotationH(m_pModelManager->GetLightRotationH() - 1.0f);
    break;

  case PANENAME_DOWN1_PANE_LIGHT_R:
    m_pModelManager->SetLightRotationH(m_pModelManager->GetLightRotationH() + 1.0f);
    break;

  default:
    break;
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
