// ============================================================================
/*
 * @file JoinFestaSelectWordMessageMenuLowerViewListener.h
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
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSelectWordMessageMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaSelectWordMessageMenuLowerViewListener() :
    m_MessageID( 0xffffffff )
  {
  }
  ~JoinFestaSelectWordMessageMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_SCREEN
  };

  void TouchScreen( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SCREEN ); m_MessageID = messageID; }

  u32 GetMessageID() const { return m_MessageID; }

private:

  u32           m_MessageID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
