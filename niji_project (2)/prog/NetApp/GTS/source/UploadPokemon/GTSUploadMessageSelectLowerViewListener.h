// ============================================================================
/*
 * @file GTSUploadMessageSelectLowerViewListener.h
 * @brief ポケモンを預ける際のメッセージを選択する画面の入力イベントを受け取る為のリスナーです。
 * @date 2015.05.11
 */
// ============================================================================
#if !defined( GTSUPLOADMESSAGESELECTLOWERVIEWLISTENER_H_INCLUDE )
#define GTSUPLOADMESSAGESELECTLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(UploadPokemon)


class GTSUploadMessageSelectLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSUploadMessageSelectLowerViewListener() :
    m_SelectMessageID( 0xffffffff )
  {
  }
  ~GTSUploadMessageSelectLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_SELECT_MESSAGE
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void SelectMessage( u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_SELECT_MESSAGE ); m_SelectMessageID = messageID; }

  u32 GetSelectMessageID() const { return m_SelectMessageID; }

private:

  u32     m_SelectMessageID;

};


GFL_NAMESPACE_END(UploadPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADMESSAGESELECTLOWERVIEWLISTENER_H_INCLUDE
