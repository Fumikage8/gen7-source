// ============================================================================
/*
 * @file GameSyncTwoSelectMenuLowerViewListener.h
 * @brief ゲームシンクアプリの下画面の入力イベントを受け取るリスナーです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCTWOSELECTMENULOWERVIEWLISTENER_H_INCLUDE )
#define GAMESYNCTWOSELECTMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)


class GameSyncTwoSelectMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GameSyncTwoSelectMenuLowerViewListener() :
    m_MessageID( 0xffffffff ),
    m_SelectMenuID( 0xffffffff )
  {
  }
  ~GameSyncTwoSelectMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_SELECT_MENU
  };

  void TouchBackButton( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); m_MessageID = messageID; }
  void TouchSelectMenu( u32 messageID, u32 selectMenuID ){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SELECT_MENU ); m_MessageID = messageID; m_SelectMenuID = selectMenuID; }

  u32 GetMessageID() const { return m_MessageID; }
  u32 GetSelectMenuID() const { return m_SelectMenuID; }

private:

  u32       m_MessageID;
  u32       m_SelectMenuID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCTWOSELECTMENULOWERVIEWLISTENER_H_INCLUDE
