// ============================================================================
/*
 * @file GTSTwoSelectMenuLowerViewListener.h
 * @brief 二つの選択ボタンと戻るボタンを表示するクラスのリスナーです。
 * @date 2015.05.12
 */
// ============================================================================
#if !defined( GTSTWOSELECTMENULOWERVIEWLISTENER_H_INCLUDE )
#define GTSTWOSELECTMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SelectMenu)


class GTSTwoSelectMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSTwoSelectMenuLowerViewListener()
  {
  }
  ~GTSTwoSelectMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_1,
    INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_2
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void TouchSelectButton1(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_1 ); }
  void TouchSelectButton2(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_2 ); }
};


GFL_NAMESPACE_END(SelectMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTWOSELECTMENULOWERVIEWLISTENER_H_INCLUDE
