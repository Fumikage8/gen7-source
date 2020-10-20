// ============================================================================
/*
 * @file JoinFestaAlertLowerViewListener.h
 * @brief アラート下画面のリスナー
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAALERTLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTAALERTLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaAlertLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaAlertLowerViewListener() :
    m_MessageID( 0xffffffff )
  {
  }
  ~JoinFestaAlertLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_PUSH_L_BUTTON,
    INPUT_EVENT_ID_PUSH_R_BUTTON
  };

  void TouchBackButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); m_MessageID = messageID; }
  void PushLButton(){ SetInputEventID( INPUT_EVENT_ID_PUSH_L_BUTTON ); }
  void PushRButton(){ SetInputEventID( INPUT_EVENT_ID_PUSH_R_BUTTON ); }

  u32 GetMessageID(){ return m_MessageID; }

private:
  
  u32     m_MessageID;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAALERTLOWERVIEWLISTENER_H_INCLUDE
