// ============================================================================
/*
 * @file GameSyncMessageMenuLowerViewListener.h
 * @brief ゲームシンクアプリの下画面の入力イベントを受け取るリスナーです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE )
#define GAMESYNCMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)


class GameSyncMessageMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GameSyncMessageMenuLowerViewListener() :
    m_MessageID( 0xffffffff )
  {
  }
  ~GameSyncMessageMenuLowerViewListener()
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
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
