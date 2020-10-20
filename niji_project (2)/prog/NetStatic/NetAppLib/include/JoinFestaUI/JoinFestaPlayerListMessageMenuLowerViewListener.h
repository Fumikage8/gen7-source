// ============================================================================
/*
 * @file JoinFestaPlayerListMessageMenuLowerViewListener.h
 * @brief メッセージメニュー表示
 * @date 2015.10.14
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTAPLAYERLISTMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListMessageMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaPlayerListMessageMenuLowerViewListener() :
    m_MessageID( 0xffffffff ),
    m_SelectMenuID( 0xffffffff )
  {
  }
  ~JoinFestaPlayerListMessageMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_SCREEN,
    INPUT_EVENT_ID_TOUCH_YES_BUTTON,
    INPUT_EVENT_ID_TOUCH_NO_BUTTON,
    INPUT_EVENT_ID_SELECT_MENU
  };

  void TouchScreen( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SCREEN ); m_MessageID = messageID; }
  void TouchYesButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_YES_BUTTON ); m_MessageID = messageID; }
  void TouchNoButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_NO_BUTTON ); m_MessageID = messageID; }
  void SelectMenu( u32 messageID, u32 menuID ){ SetInputEventID( INPUT_EVENT_ID_SELECT_MENU ); m_MessageID = messageID; m_SelectMenuID = menuID; }

  u32 GetMessageID() const { return m_MessageID; }
  u32 GetSelectMenuID() const { return m_SelectMenuID; }

private:

  u32           m_MessageID;
  u32           m_SelectMenuID;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
