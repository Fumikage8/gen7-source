// ============================================================================
/*
 * @file GTSSearchOptionLowerViewListener.h
 * @brief 検索オプション画面の入力イベントを受け取る為のリスナーです。
 * @date 2015.05.11
 */
// ============================================================================
#if !defined( GTSSEARCHOPTIONLOWERVIEWLISTENER_H_INCLUDE )
#define GTSSEARCHOPTIONLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SearchPokemon)


class GTSSearchOptionLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSSearchOptionLowerViewListener()
  {
  }
  ~GTSSearchOptionLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_SEARCH_BUTTON,
    INPUT_EVENT_ID_TOUCH_NOT_SEARCH_BUTTON
  };

  void TouchSearchButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SEARCH_BUTTON ); }
  void TouchNotSearchButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_NOT_SEARCH_BUTTON ); }

};


GFL_NAMESPACE_END(SearchPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSEARCHOPTIONLOWERVIEWLISTENER_H_INCLUDE
