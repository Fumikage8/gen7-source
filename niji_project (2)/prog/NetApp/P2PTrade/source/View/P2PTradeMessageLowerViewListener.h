// ============================================================================
/*
 * @file P2PTradeMessageLowerViewListener.h
 * @brief 
 * @date 2016.02.18
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADEMESSAGELOWERVIEWLISTENER_H_INCLUDE )
#define P2PTRADEMESSAGELOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(View)


class P2PTradeMessageLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  P2PTradeMessageLowerViewListener() :
    m_MessageID( 0xffffffff )
  {
  }
  ~P2PTradeMessageLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_SCREEN,
    INPUT_EVENT_ID_TOUCH_YES_BUTTON,
    INPUT_EVENT_ID_TOUCH_NO_BUTTON,
  };

  void TouchBackButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); m_MessageID = messageID; }
  void TouchScreen( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SCREEN ); m_MessageID = messageID; }
  void TouchYesButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_YES_BUTTON ); m_MessageID = messageID; }
  void TouchNoButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_NO_BUTTON ); m_MessageID = messageID; }

  u32 GetMessageID() const { return m_MessageID; }

private:

  u32           m_MessageID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADEMESSAGELOWERVIEWLISTENER_H_INCLUDE
