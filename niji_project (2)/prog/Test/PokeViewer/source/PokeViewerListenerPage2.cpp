//======================================================================
/**
 * @file PokeViewerListenerPage2.cpp
 * @date 2015/12/03 12:02:29
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

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent_Page2(gfl2::ui::TouchPanel* pTouchPanel, bool isTouch)
{
  app::util::G2DUtil* pG2DUtil = m_pView->GetG2DUtil();

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
