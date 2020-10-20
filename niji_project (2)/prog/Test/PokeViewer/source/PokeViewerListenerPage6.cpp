//======================================================================
/**
 * @file PokeViewerListenerPage6.cpp
 * @date 2015/12/03 12:08:01
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

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent_Page6(gfl2::ui::TouchPanel* pTouchPanel, bool isTouch)
{
  gfl2::lyt::LytPaneIndex ret = m_pView->GetG2DUtil()->GetTouchTriggerPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP);
 
  switch (ret)
  {
    // キャプチャモード切り替え
  case PANENAME_DOWN1_PANE_SS_START_BTN:
    if(!m_pModelManager->IsCaptureMode())
    {
      m_pModelManager->SetCaptureMode(PokeViewerPokeModelManager::UPDATE_TYPE_CAPTURE_ALL);
    }
    break;

  // 負荷計測モード切り替え
  case PANENAME_DOWN1_PANE_SS_START_MEASURE_BTN:
    if(!m_pModelManager->IsCaptureMode())
    {
      m_pModelManager->SetCaptureMode(PokeViewerPokeModelManager::UPDATE_TYPE_LOAD_MEASUREMENT);
    }
    break;

  case PANENAME_DOWN1_PANE_SS_DIR_BTN_FRONT:
    m_pModelManager->ToggleCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_FRONT);
    break;

  case PANENAME_DOWN1_PANE_SS_DIR_BTN_BACK:
    m_pModelManager->ToggleCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_BACK);
    break;

  case PANENAME_DOWN1_PANE_SS_DIR_BTN_RIGHT:
    m_pModelManager->ToggleCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_RIGHT);
    break;

  case PANENAME_DOWN1_PANE_SS_DIR_BTN_LEFT:
    m_pModelManager->ToggleCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_LEFT);
    break;

  case PANENAME_DOWN1_PANE_SS_DIR_BTN_TOP:
    m_pModelManager->ToggleCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_TOP);
    break;

  case PANENAME_DOWN1_PANE_SS_DIR_BTN_BOTTOM:
    m_pModelManager->ToggleCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_BOTTOM);
    break;

    // キャプチャ開始位置
  case PANENAME_DOWN1_PANE_SS_START_U_00:
    m_pModelManager->SetStartMonsNo(std::min((s32)m_pModelManager->GetStartMonsNo() + 100, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_SS_START_U_01:
    m_pModelManager->SetStartMonsNo(std::min((s32)m_pModelManager->GetStartMonsNo() + 10, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_SS_START_U_02:
    m_pModelManager->SetStartMonsNo(std::min((s32)m_pModelManager->GetStartMonsNo() + 1, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_SS_START_D_00:
    m_pModelManager->SetStartMonsNo(std::max((s32)m_pModelManager->GetStartMonsNo() - 100, (s32)MONSNO_HUSIGIDANE));
    break;

  case PANENAME_DOWN1_PANE_SS_START_D_01:
    m_pModelManager->SetStartMonsNo(std::max((s32)m_pModelManager->GetStartMonsNo() - 10, (s32)MONSNO_HUSIGIDANE));
    break;

  case PANENAME_DOWN1_PANE_SS_START_D_02:
    m_pModelManager->SetStartMonsNo(std::max((s32)m_pModelManager->GetStartMonsNo() - 1, (s32)MONSNO_HUSIGIDANE));
    break;

    // キャプチャ終了位置
  case PANENAME_DOWN1_PANE_SS_END_U_00:
    m_pModelManager->SetEndMonsNo(std::min((s32)m_pModelManager->GetEndMonsNo() + 100, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_SS_END_U_01:
    m_pModelManager->SetEndMonsNo(std::min((s32)m_pModelManager->GetEndMonsNo() + 10, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_SS_END_U_02:
    m_pModelManager->SetEndMonsNo(std::min((s32)m_pModelManager->GetEndMonsNo() + 1, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_SS_END_D_00:
    m_pModelManager->SetEndMonsNo(std::max((s32)m_pModelManager->GetEndMonsNo() - 100, (s32)MONSNO_HUSIGIDANE));
    break;

  case PANENAME_DOWN1_PANE_SS_END_D_01:
    m_pModelManager->SetEndMonsNo(std::max((s32)m_pModelManager->GetEndMonsNo() - 10, (s32)MONSNO_HUSIGIDANE));
    break;

  case PANENAME_DOWN1_PANE_SS_END_D_02:
    m_pModelManager->SetEndMonsNo(std::max((s32)m_pModelManager->GetEndMonsNo() - 1, (s32)MONSNO_HUSIGIDANE));
    break;

  case PANENAME_DOWN1_PANE_SS_LEN_D_BTN:
    m_pModelManager->SetCameraDistance(std::max(m_pModelManager->GetCameraDistance() - 0.1f, 0.0f));
    break;

  case PANENAME_DOWN1_PANE_SS_LEN_U_BTN:
    m_pModelManager->SetCameraDistance(std::min(m_pModelManager->GetCameraDistance() + 0.1f, 10.0f));
    break;

  case PANENAME_DOWN1_PANE_SS_LEN_SET_S:
    m_pModelManager->SetCameraDistance(0.8f);
    break;

  case PANENAME_DOWN1_PANE_SS_LEN_SET_M:
    m_pModelManager->SetCameraDistance(1.0f);
    break;

  case PANENAME_DOWN1_PANE_SS_LEN_SET_L:
    m_pModelManager->SetCameraDistance(1.2f);
    break;

  default:
    break;
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
