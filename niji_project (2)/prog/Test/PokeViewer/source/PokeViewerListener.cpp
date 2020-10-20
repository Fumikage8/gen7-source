//======================================================================
/**
 * @file PokeViewerListener.cpp
 * @date 2015/12/02 16:23:03
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

/**
 * @fn PokeViewerListener
 * @brief ctor
 */
PokeViewerListener::PokeViewerListener(PokeViewerView* pView, PokeViewerPokeModelManager* pModelManager)
: app::ui::UIInputListener(),
  m_pView(pView),
  m_pModelManager(pModelManager)
{
}

/**
 * @fn ~PokeViewerListener
 * @brief dtor
 */
PokeViewerListener::~PokeViewerListener()
{
}

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_pView->GetG2DUtil();

  GFL_PRINT("Hit[%d]\n", pG2DUtil->GetTouchTriggerPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP));

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_LOCKPANE))
  {
    return OnTouchEvent_LockPane( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_LOCKPANE2))
  {
    return OnTouchEvent_LockPane2( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE1) || pG2DUtil->IsPaneTouchHold(0, PANENAME_DOWN1_PANE_PAGE1))
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

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PAGE6))
  {
    return OnTouchEvent_Page6( pTouchPanel, isTouch );
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( pButton->IsHold(gfl2::ui::BUTTON_START) )
  {
    m_pView->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent_LockPane( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  gfl2::lyt::LytPaneIndex ret = m_pView->GetG2DUtil()->GetTouchTriggerPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP);

  switch(ret)
  {
    // タブ移動（矢印）
  case PANENAME_DOWN1_PANE_PAGE_U:
    m_pView->UpdatePage(true);
    break;

  case PANENAME_DOWN1_PANE_PAGE_D:
    m_pView->UpdatePage(false);
    break;

    // タブ移動（数字）
  case PANENAME_DOWN1_PANE_PAGE_BUT_1:
    m_pView->SetPage(0);
    m_pView->UpdatePageDisp();
    break;

  case PANENAME_DOWN1_PANE_PAGE_BUT_2:
    m_pView->SetPage(1);
    m_pView->UpdatePageDisp();
    break;

  case PANENAME_DOWN1_PANE_PAGE_BUT_3:
    m_pView->SetPage(2);
    m_pView->UpdatePageDisp();
    break;

  case PANENAME_DOWN1_PANE_PAGE_BUT_4:
    m_pView->SetPage(3);
    m_pView->UpdatePageDisp();
    break;
      
  case PANENAME_DOWN1_PANE_PAGE_BUT_5:
    m_pView->SetPage(4);
    m_pView->UpdatePageDisp();
    break;

  case PANENAME_DOWN1_PANE_PAGE_BUT_6:
    m_pView->SetPage(5);
    m_pView->UpdatePageDisp();
    break;
  }

  // モデル切り替えは実装しない

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent_LockPane2( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  gfl2::lyt::LytPaneIndex ret = m_pView->GetG2DUtil()->GetTouchTriggerPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP);

  switch(ret)
  {
    // プリセットカメラ
  case PANENAME_DOWN1_PANE_CAMERA_SET_0:
    m_pModelManager->SetPresetCamera(PokeViewerPokeModelManager::CAMERA_TYPE_DEFAULT);
    break;

  case PANENAME_DOWN1_PANE_CAMERA_SET_1:
    m_pModelManager->SetPresetCamera(PokeViewerPokeModelManager::CAMERA_TYPE_STATUS);
    break;

  case PANENAME_DOWN1_PANE_CAMERA_SET_4:
    m_pModelManager->SetPresetCamera(PokeViewerPokeModelManager::CAMERA_TYPE_KAWAIGARI);
    break;

  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
