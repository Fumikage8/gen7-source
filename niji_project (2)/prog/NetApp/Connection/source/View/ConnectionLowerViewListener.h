// ============================================================================
/*
 * @file ConnectionLowerViewListener.h
 * @brief インターネット接続アプリの下画面の入力イベントを受け取るリスナーです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTIONLOWERVIEWLISTENER_H_INCLUDE )
#define CONNECTIONLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(View)


class ConnectionLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  ConnectionLowerViewListener() :
    m_MessageID( 0xffffffff )
  {
  }
  ~ConnectionLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_END_STREAM_MESSAGE,
    INPUT_EVENT_ID_TOUCH_YES_BUTTON,
    INPUT_EVENT_ID_TOUCH_NO_BUTTON
  };

  void TouchYesButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_YES_BUTTON ); m_MessageID = messageID; }
  void TouchNoButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_NO_BUTTON ); m_MessageID = messageID; }
  void EndStreamMessage( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_END_STREAM_MESSAGE ); m_MessageID = messageID; }

  u32 GetMessageID() const { return m_MessageID; }

private:

  u32     m_MessageID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTIONLOWERVIEWLISTENER_H_INCLUDE
