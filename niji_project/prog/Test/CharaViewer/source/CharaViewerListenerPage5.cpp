//======================================================================
/**
 * @file CharaViewerListenerPage5.cpp
 * @date 2015/07/24 19:02:24
 * @author araki_syo
 * @brief CharaViewer5ページ目のリスナー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include "../include/CharaViewerListener.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(chara_viewer)


  /**
   * @fn
   * ここに関数の説明を書く
   * @brief 要約説明
   * @param[in] var1 var1の説明
   * @param[out] var2 var2の説明
   * @param[in,out] var3 var3の説明
   * @return 戻り値の説明
   * @sa 参照すべき関数を書けばリンクが貼れる
   * @detail 詳細な説明
   */
app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_Page5( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_LIGHTCONTROLLERBASE))
  {
    return OnTouchEvent_LightController( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAMERACONTROLLERBASE))
  {
    return OnTouchEvent_CameraController( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CLEARCOLORBASE))
  {
    return OnTouchEvent_ClearColor( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_GENERALCONFIGBASE))
  {
    return OnTouchEvent_GeneralConfig( pTouchPanel, isTouch );
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_LightController( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_LIGHT_CTRL_RESET))
  {
    m_view->ResetLight();
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_CameraController( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAMERA_CTRL_RESET))
  {
    m_view->ResetCamera();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAMERA_FIELD_CHECKBOX))
  {
    m_view->SetCameraAngle(CharaViewerView::FIELD);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAMERA_BATTLE_CHECKBOX))
  {
    m_view->SetCameraAngle(CharaViewerView::BATTLE);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAMERA_NEAR_CHECKBOX))
  {
    m_view->SetCameraMode(CharaViewerView::NF_FIELD_NEAR);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAMERA_MID_CHECKBOX))
  {
    m_view->SetCameraMode(CharaViewerView::NF_FIELD_MID);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAMERA_FAR_CHECKBOX))
  {
    m_view->SetCameraMode(CharaViewerView::NF_FIELD_FAR);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAMERA_BATTLE_CHECKBO_00))
  {
    m_view->SetCameraMode(CharaViewerView::NF_BATTLE);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_ANGLE_SELECTOR_BTN_L))
  {
    m_view->SlideCameraAngleSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_ANGLE_SELECTOR_BTN_R))
  {
    m_view->SlideCameraAngleSelector(true);
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_ClearColor( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CLEAR_COLOR_0_CHECK))
  {
    m_view->SetClearColorType(0);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CLEAR_COLOR_1_CHECK))
  {
    m_view->SetClearColorType(1);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CLEAR_COLOR_2_CHECK))
  {
    m_view->SetClearColorType(2);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CLEAR_COLOR_3_CHECK))
  {
    m_view->SetClearColorType(3);
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_GeneralConfig( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CTRLMODE_BTN_MDL))
  {
    m_view->ToggleCameraControllMode();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_GRID_BTN_OFF))
  {
    m_view->ToggleGrid();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAPTURE))
  {
    m_view->Capture();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CAPTURE_ALL))
  {
    m_view->CaptureAll();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_DS_RAND))
  {
    m_view->DressUpRand();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_DS_DUMP))
  {
    m_view->DressUpDump();
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

void CharaViewerListener::ResetCameraAngleCheckBox()
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_FIELD_CHECKBOX, true);
  pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_BATTLE_CHECKBOX, true);
}

void CharaViewerListener::ResetCameraNearFarCheckBox()
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();
  pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_NEAR_CHECKBOX, true);
  pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_MID_CHECKBOX, true);
  pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_FAR_CHECKBOX, true);
}


GFL_NAMESPACE_END(chara_viewer)
  GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
