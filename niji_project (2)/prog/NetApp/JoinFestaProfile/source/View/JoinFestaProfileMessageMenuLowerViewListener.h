// ============================================================================
/*
 * @file JoinFestaProfileMessageMenuLowerViewListener.h
 * @brief メッセージメニュー表示
 * @date 2015.11.04
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILEMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTAPROFILEMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaProfileMessageMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaProfileMessageMenuLowerViewListener() :
    m_MessageID( 0xffffffff ),
    m_SelectMenuID( 0xffffffff )
  {
  }
  ~JoinFestaProfileMessageMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_SCREEN,
    INPUT_EVENT_ID_SELECT_MENU
  };

  void TouchScreen( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SCREEN ); m_MessageID = messageID; }
  void SelectMenu( u32 messageID, u32 menuID ){ SetInputEventID( INPUT_EVENT_ID_SELECT_MENU ); m_MessageID = messageID; m_SelectMenuID = menuID; }

  u32 GetMessageID() const { return m_MessageID; }
  u32 GetSelectMenuID() const { return m_SelectMenuID; }

private:

  u32           m_MessageID;
  u32           m_SelectMenuID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILEMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
