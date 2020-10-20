//======================================================================
/**
* @file PokeViewerListener.h
* @date 2015/12/02 16:12:45
* @author araki_syo
* @brief ポケビューアのUIリスナー
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#if !defined POKEVIEWER_LISTENER_H_INCLUDED
#define POKEVIEWER_LISTENER_H_INCLUDED
#pragma once

#include <AppLib/include/Ui/UIInputListener.h>

#include "niji_conv_header/app/pokeviewer/res2d/down1.h"
#include "niji_conv_header/app/pokeviewer/res2d/down1_pane.h"
#include "niji_conv_header/app/pokeviewer/res2d/down1_anim_list.h"

#include "PokeViewerView.h"
#include "PokeViewerPokeModelManager.h"


GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

class PokeViewerView;

/**
 * @class PokeViewerListener
 * @brief PokeViewerのUIリスナー
 */
class PokeViewerListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeViewerListener);

public:
  /**
   * @fn PokeViewerListener
   * @brief ctor
   */
  PokeViewerListener(PokeViewerView* pView, PokeViewerPokeModelManager* pModelManager);

  /**
   * @fn ~PokeViewerListener
   * @brief dtor
   */
  ~PokeViewerListener();

public:

  /**
   * @fn OnTouchEvent
   * @brief 画面がタッチされたときに呼び出されるコールバック
   * @param[in] pTouchPanel
   * @param[in] isTouch
   */
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page1( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page2( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page3( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page4( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page5( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page6( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

  /**
   * @fn OnKeyAction
   * @brief ボタンが押された時に呼び出されるコールバック
   * @param[in] pButton
   * @param[in] pKey
   * @param[in] pStick
   */
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  /**
   * @fn OnTouchEvent_LockPane
   * @brief ロックペインがタッチされたときに呼び出されるコールバック
   * @param[in] pTouchPanel
   * @param[in] isTouch
   */
  app::ui::UIInputListener::ListenerResult OnTouchEvent_LockPane( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_LockPane2( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );


public:
  PokeViewerView* m_pView;
  PokeViewerPokeModelManager* m_pModelManager;
};

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // POKEVIEWER_LISTENER_H_INCLUDED

#endif // PM_DEBUG
