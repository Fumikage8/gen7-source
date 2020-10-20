// ============================================================================
/*
 * @file GTSUploadPokemonLowerViewListener.h
 * @brief ポケモンを預ける画面の入力イベントを受け取る為のリスナーです。
 * @date 2015.04.08
 */
// ============================================================================
#if !defined( GTSUPLOADPOKEMONLOWERVIEWLISTENER_H_INCLUDE )
#define GTSUPLOADPOKEMONLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(UploadPokemon)


class GTSUploadPokemonLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSUploadPokemonLowerViewListener()
  {
  }
  ~GTSUploadPokemonLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_POKEMON_BUTTON,
    INPUT_EVENT_ID_TOUCH_GENDER_BUTTON,
    INPUT_EVENT_ID_TOUCH_LEVEL_BUTTON,
    INPUT_EVENT_ID_TOUCH_MESSAGE_BUTTON,
    INPUT_EVENT_ID_TOUCH_UPLOAD_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void TouchPokemonButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_POKEMON_BUTTON ); }
  void TouchGenderButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_GENDER_BUTTON ); }
  void TouchLevelButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_LEVEL_BUTTON ); }
  void TouchMessageButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_MESSAGE_BUTTON ); }
  void TouchUploadButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_UPLOAD_BUTTON ); }
};


GFL_NAMESPACE_END(UploadPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADPOKEMONLOWERVIEWLISTENER_H_INCLUDE
