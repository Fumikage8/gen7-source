// ============================================================================
/*
 * @file JoinFestaGlobalAttractionMessageMenuLowerViewListener.h
 * @brief メッセージメニュー表示
 * @date 2015.10.14
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTASELECTWORDMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionMessageMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaGlobalAttractionMessageMenuLowerViewListener() :
    m_MessageID( 0xffffffff )
  {
  }
  ~JoinFestaGlobalAttractionMessageMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_SCREEN,
    INPUT_EVENT_ID_TOUCH_YES_BUTTON,
    INPUT_EVENT_ID_TOUCH_NO_BUTTON
  };

  void TouchScreen( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SCREEN ); m_MessageID = messageID; }
  void TouchYesButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_YES_BUTTON ); m_MessageID = messageID; }
  void TouchNoButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_NO_BUTTON ); m_MessageID = messageID; }

  u32 GetMessageID() const { return m_MessageID; }

private:

  u32           m_MessageID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
