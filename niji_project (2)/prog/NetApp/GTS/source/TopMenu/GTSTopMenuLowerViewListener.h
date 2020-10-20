// ============================================================================
/*
 * @file GTSTopMenuLowerViewListener.h
 * @brief GTSのトップメニューの入力イベントを受け取る為のリスナーです。
 * @date 2015.03.30
 */
// ============================================================================
#if !defined( GTSTOPMENULOWERVIEWLISTENER_H_INCLUDE )
#define GTSTOPMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TopMenu)


class GTSTopMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSTopMenuLowerViewListener()
  {
  }
  ~GTSTopMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_UPLOAD_POKEMON_BUTTON,
    INPUT_EVENT_ID_TOUCH_SEARCH_POKEMON_BUTTON,
    INPUT_EVENT_ID_TOUCH_DOWNLOAD_MY_POKEMON_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void TouchUploadPokemonButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_UPLOAD_POKEMON_BUTTON ); }
  void TouchSearchPokemonButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SEARCH_POKEMON_BUTTON ); }
  void TouchDownloadMyPokemonButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_DOWNLOAD_MY_POKEMON_BUTTON ); }
};


GFL_NAMESPACE_END(TopMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTOPMENULOWERVIEWLISTENER_H_INCLUDE
