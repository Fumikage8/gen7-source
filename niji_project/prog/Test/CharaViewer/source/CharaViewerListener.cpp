//======================================================================
/**
* @file CharaViewerListener.cpp
* @date 2015/07/23 18:52:46
* @author araki_syo
* @brief CharaViewerのリスナー
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include "../include/CharaViewerView.h"
#include "../include/CharaViewerListener.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(chara_viewer)


CharaViewerListener::CharaViewerListener(CharaViewerView* view)
  : app::ui::UIInputListener(),
  m_view(view)
{}

CharaViewerListener::~CharaViewerListener()
{}


app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();


  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_LOCKPANE))
  {
    return OnTouchEvent_LockPane( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE1))
  {
    return OnTouchEvent_Page1( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE2))
  {
    return OnTouchEvent_Page2( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE3))
  {
    return OnTouchEvent_Page3( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE4))
  {
    return OnTouchEvent_Page4( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE5))
  {
    return OnTouchEvent_Page5( pTouchPanel, isTouch );
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( pButton->IsHold(gfl2::ui::BUTTON_START) )
  {
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_LockPane( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  //タブ移動(矢印)
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE_U))
  {
    m_view->UpdatePage(true);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE_D))
  {
    m_view->UpdatePage(false);
  }

  // タブ移動(数字)
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE_BUT_1))
  {
    m_view->SetPage(0);
    m_view->UpdatePageDisp();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE_BUT_2))
  {
    m_view->SetPage(1);
    m_view->UpdatePageDisp();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE_BUT_3))
  {
    m_view->SetPage(2);
    m_view->UpdatePageDisp();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE_BUT_4))
  {
    m_view->SetPage(3);
    m_view->UpdatePageDisp();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE_BUT_5))
  {
    m_view->SetPage(4);
    m_view->UpdatePageDisp();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_POKE_D))
  {
    m_view->SlideModelSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_POKE_U))
  {
    m_view->SlideModelSelector(true);
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

GFL_NAMESPACE_END(chara_viewer)
  GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
