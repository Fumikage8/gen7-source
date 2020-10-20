// ============================================================================
/*
 * @file RegulationSelectMessageMenuLowerViewListener.h
 * @brief メッセージウィンドウと選択メニューの入力イベントを受け取るリスナーです。
 * @date 2015.06.18
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECTMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE )
#define REGULATIONSELECTMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)


class RegulationSelectMessageMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  RegulationSelectMessageMenuLowerViewListener() :
    m_MessageID( 0xffffffff ),
    m_SelectMenuID( 0xffffffff )
  {
  }
  ~RegulationSelectMessageMenuLowerViewListener()
  {
  }


  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_SCREEN,
    INPUT_EVENT_ID_TOUCH_YES_BUTTON,
    INPUT_EVENT_ID_TOUCH_NO_BUTTON,
    INPUT_EVENT_ID_SELECT_MENU
  };

  void TouchBackButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); m_MessageID = messageID; }
  void TouchScreen( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SCREEN ); m_MessageID = messageID; }
  void TouchYesButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_YES_BUTTON ); m_MessageID = messageID; }
  void TouchNoButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_NO_BUTTON ); m_MessageID = messageID; }
  void SelectMenu( u32 messageID, u32 menuID ){ SetInputEventID( INPUT_EVENT_ID_SELECT_MENU ); m_MessageID = messageID; m_SelectMenuID = menuID; }

  u32 GetMessageID() const { return m_MessageID; }
  u32 GetSelectMenuID() const { return m_SelectMenuID; }

private:

  u32             m_MessageID;
  u32             m_SelectMenuID;


};


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTMESSAGEMENULOWERVIEWLISTENER_H_INCLUDE
