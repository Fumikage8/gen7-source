// ============================================================================
/*
 * @file GTSSearchPokemonLowerViewListener.h
 * @brief 探したいポケモンの選択を行うメニューの入力イベントを受け取る為のリスナーです。
 * @date 2015.04.02
 */
// ============================================================================
#if !defined( GTSSEARCHPOKEMONLOWERVIEWLISTENER_H_INCLUDE )
#define GTSSEARCHPOKEMONLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SearchPokemon)


class GTSSearchPokemonLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSSearchPokemonLowerViewListener()
  {
  }
  ~GTSSearchPokemonLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_POKEMON_BUTTON,
    INPUT_EVENT_ID_TOUCH_GENDER_BUTTON,
    INPUT_EVENT_ID_TOUCH_LEVEL_BUTTON,
    INPUT_EVENT_ID_TOUCH_SEARCH_BUTTON,
    INPUT_EVENT_ID_TOUCH_OPTION_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void TouchPokemonButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_POKEMON_BUTTON ); }
  void TouchGenderButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_GENDER_BUTTON ); }
  void TouchLevelButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_LEVEL_BUTTON ); }
  void TouchSearchButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SEARCH_BUTTON ); }
  void TouchOptionButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_OPTION_BUTTON ); }
};


GFL_NAMESPACE_END(SearchPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSEARCHPOKEMONLOWERVIEWLISTENER_H_INCLUDE
