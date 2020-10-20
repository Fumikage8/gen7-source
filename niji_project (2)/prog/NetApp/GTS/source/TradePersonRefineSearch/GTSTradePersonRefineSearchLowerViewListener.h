// ============================================================================
/*
 * @file GTSTradePersonRefineSearchLowerViewListener.h
 * @brief 交換相手を絞り込む画面の入力イベントを受け取る為のリスナーです。
 * @date 2015.04.22
 */
// ============================================================================
#if !defined( GTSTRADEPERSONREFINESEARCHLOWERVIEWLISTENER_H_INCLUDE )
#define GTSTRADEPERSONREFINESEARCHLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TradePersonRefineSearch)

class GTSTradePersonRefineSearchLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSTradePersonRefineSearchLowerViewListener()
  {
  }
  ~GTSTradePersonRefineSearchLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_REQUEST_POKEMON_BUTTON,
    INPUT_EVENT_ID_TOUCH_AREA_BUTTON,
    INPUT_EVENT_ID_TOUCH_REFINE_SEARCH_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void TouchRequestPokemonButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_REQUEST_POKEMON_BUTTON ); }
  void TouchAreaButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_AREA_BUTTON ); }
  void TouchRefineSearchButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_REFINE_SEARCH_BUTTON ); }
};


GFL_NAMESPACE_END(TradePersonRefineSearch)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPERSONREFINESEARCHLOWERVIEWLISTENER_H_INCLUDE
