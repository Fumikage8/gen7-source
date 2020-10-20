//======================================================================
/**
 * @file CharaViewerListenerPage1.cpp
 * @date 2015/07/24 18:53:21
 * @author araki_syo
 * @brief CharaViewer1ページ目のリスナー
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
  * @brief 1ページ目がタッチされたときのコールバック
  */
app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_Page1( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CHARASELECTOR))
  {
    return OnTouchEvent_CharaSelector(pTouchPanel, isTouch);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_ORIGINLOCATOR))
  {
    return OnTouchEvent_OriginLocator(pTouchPanel, isTouch);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SCALESELECTOR))
  {
    return OnTouchEvent_ScaleSelector(pTouchPanel, isTouch);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_DEFAULT_BTN))
  {
    m_view->ResetAllParts();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SAVE_BTN))
  {
    m_view->SaveCurrentPartsSetting();
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

/**
  * @brief キャラセレクターがタッチされたときのコールバック
  */
app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_CharaSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_L))
  {
    m_view->SlideCharaSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_R))
  {
    m_view->SlideCharaSelector(true);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_LOAD))
  {
    m_view->LoadCharactor();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_UNLOA))
  {
    m_view->UnloadCharactor();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_BT))
  {
    // 選択中のモデルにBT/FIモデルが存在するかどうかを取得したい
    m_view->SetModelType(CharaViewerView::BATTLE_MODEL);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_FI))
  {
    // 選択中のモデルにBT/FIモデルが存在するかどうかを取得したい
    m_view->SetModelType(CharaViewerView::FIELD_MODEL);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

/**
  * @brief 原点リセットがタッチされたときのコールバック
  */
app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_OriginLocator( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_ORIGIN_LOCATOR_BTN))
  {
    m_view->LocateOrigin();
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

/**
  * @brief モデルスケールのセレクターがタッチされたときのコールバック
  */
app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_ScaleSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SCALE_SELECTOR_BTN_L))
  {
    m_view->SlideScaleSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SCALE_SELECTOR_BTN_R))
  {
    m_view->SlideScaleSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

GFL_NAMESPACE_END(chara_viewer)
  GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
