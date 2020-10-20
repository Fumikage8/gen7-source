//======================================================================
/**
 * @file CharaViewerListenerPage4.cpp
 * @date 2015/07/24 19:01:34
 * @author araki_syo
 * @brief CharaViewer4ページ目のリスナー
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
app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_Page4( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CONTROLLERBASE))
  {
    return OnTouchEvent_Controller( pTouchPanel, isTouch);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_BASE_00))
  {
    return OnTouchEvent_PlayList( pTouchPanel, isTouch );
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_Controller( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_MOTION_LOOP_00))
  {
    m_view->ToggleLoopFlag();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_MOTION_STEP2_00))
  {
    m_view->PlaySingleFrame(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_MOTION_STEP_00))
  {
    m_view->PlaySingleFrame(true);
  }

  
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_MOTION_PLAY_00))
  {
    m_view->PlayMotion();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_ALL_PLAY_TOGGLE))
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_ALL_PLAY_TOGGLE, !pG2DUtil->GetPaneVisible(0, PANENAME_DOWN1_PANE_ALL_PLAY_TOGGLE));
    m_view->ToggleAllPlayFlag();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_TRANSLATE_TOGGLE))
  {
    m_view->ToggleTranslateFlag();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_FPS_SELECTOR_BTN_L))
  {
    m_view->SlideFpsSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_FPS_SELECTOR_BTN_R))
  {
    m_view->SlideFpsSelector(true);
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_PlayList( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_L_07))
  {
    m_view->SlideMotionSelector(0, false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_L_08))
  {
    m_view->SlideMotionSelector(1, false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_L_09))
  {
    m_view->SlideMotionSelector(2, false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_L_10))
  {
    m_view->SlideMotionSelector(3, false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_L_11))
  {
    m_view->SlideMotionSelector(4, false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_R_07))
  {
    m_view->SlideMotionSelector(0, true);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_R_08))
  {
    m_view->SlideMotionSelector(1, true);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_R_09))
  {
    m_view->SlideMotionSelector(2, true);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_R_10))
  {
    m_view->SlideMotionSelector(3, true);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_R_11))
  {
    m_view->SlideMotionSelector(4, true);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_07))
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_07, !pG2DUtil->GetPaneVisible(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_07));
    m_view->ToggleMotion(0);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_08))
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_08, !pG2DUtil->GetPaneVisible(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_08));
    m_view->ToggleMotion(1);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_09))
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_09, !pG2DUtil->GetPaneVisible(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_09));
    m_view->ToggleMotion(2);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_10))
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_10, !pG2DUtil->GetPaneVisible(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_10));
    m_view->ToggleMotion(3);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_11))
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_11, !pG2DUtil->GetPaneVisible(0, PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_11));
    m_view->ToggleMotion(4);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_T_POSE_PLAY_BTN))
  {
    m_view->PlayTPose();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CHECK_MOTION_PLAY_BTN))
  {
    m_view->PlayStackCheckMotion();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_EYESELECTORBASE))
  {
    return OnTouchEvent_EyeSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_MOUTHSELECTORBASE))
  {
    return OnTouchEvent_MouthSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_AUTO_BLINK_DOT))
  {
    m_view->ToggleAutoBlink();
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_EyeSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_EYE_BTN_L))
  {
    m_view->SlideEyeSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_EYE_BTN_R))
  {
    m_view->SlideEyeSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_MouthSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_MOUTH_BTN_L))
  {
    m_view->SlideMouthSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_MOUTH_BTN_R))
  {
    m_view->SlideMouthSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

GFL_NAMESPACE_END(chara_viewer)
  GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
