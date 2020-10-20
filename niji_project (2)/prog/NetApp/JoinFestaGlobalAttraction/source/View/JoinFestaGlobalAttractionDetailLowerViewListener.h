// ============================================================================
/*
 * @file JoinFestaGlobalAttractionDetailLowerViewListener.h
 * @brief グローバルアトラクションアプリの下画面の入力イベントを受け取るリスナーです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONDETAILLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONDETAILLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionDetailLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaGlobalAttractionDetailLowerViewListener()
  {
  }
  ~JoinFestaGlobalAttractionDetailLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONDETAILLOWERVIEWLISTENER_H_INCLUDE
