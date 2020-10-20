// ============================================================================
/*
 * @file GTSOneSelectMenuLowerViewListener.h
 * @brief 一つの選択ボタンと戻るボタンを表示するクラスのリスナーです。
 * @date 2015.05.12
 */
// ============================================================================
#if !defined( GTSONESELECTMENULOWERVIEWLISTENER_H_INCLUDE )
#define GTSONESELECTMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SelectMenu)


class GTSOneSelectMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSOneSelectMenuLowerViewListener()
  {
  }
  ~GTSOneSelectMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_SELECT_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void TouchSelectButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SELECT_BUTTON ); }
};


GFL_NAMESPACE_END(SelectMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSONESELECTMENULOWERVIEWLISTENER_H_INCLUDE
