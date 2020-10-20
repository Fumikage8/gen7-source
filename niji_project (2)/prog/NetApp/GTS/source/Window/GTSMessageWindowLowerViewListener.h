// ============================================================================
/*
 * @file GTSMessageWindowLowerViewListener.h
 * @brief メッセージウィンドウの入力イベントを受け取る為のリスナーです。
 * @date 2015.04.07
 */
// ============================================================================
#if !defined( GTSMESSAGEWINDOWLOWERVIEWLISTENER_H_INCLUDE )
#define GTSMESSAGEWINDOWLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(Window)


class GTSMessageWindowLowerViewListener: public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSMessageWindowLowerViewListener() :
    m_MessageID( 0xffffffff )
  {
  }
  ~GTSMessageWindowLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_SCREEN,
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON
  };

  void TouchScreen( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SCREEN ); m_MessageID = messageID; }
  void TouchBackButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); m_MessageID = messageID; }

  u32 GetMessageID() const { return m_MessageID; }

private:

  u32     m_MessageID;

};


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSMESSAGEWINDOWLOWERVIEWLISTENER_H_INCLUDE
