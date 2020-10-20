// ============================================================================
/*
 * @file JoinFestaSelectWordLowerViewListener.h
 * @brief 挨拶選択アプリの下画面の入力イベントを受け取るリスナーです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTASELECTWORDLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSelectWordLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaSelectWordLowerViewListener() :
    m_ListIndex( 0 ),
    m_MessageID( 0xffffffff )
  {
  }
  ~JoinFestaSelectWordLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_SELECT_MESSAGE,
    INPUT_EVENT_ID_TOUCH_INFO_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void SelectMessage( u32 listIndex, u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_SELECT_MESSAGE ); m_ListIndex = listIndex; m_MessageID = messageID; }
  void TouchInfoButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_INFO_BUTTON ); }

  u32 GetSelectMessageID() const { return m_MessageID; }
  u32 GetSelectListIndex() const { return m_ListIndex; }

private:

  u32     m_ListIndex;
  u32     m_MessageID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDLOWERVIEWLISTENER_H_INCLUDE
