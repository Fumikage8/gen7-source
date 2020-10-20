// ============================================================================
/*
 * @file JoinFestaPlayerListLowerViewBaseListener.h
 * @brief JFPL_MIN_LOW_000.flyt を扱う基底クラスです
 * @date 2015.10.05
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTLOWERVIEWBASELISTENER_H_INCLUDE )
#define JOINFESTAPLAYERLISTLOWERVIEWBASELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListLowerViewBaseListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaPlayerListLowerViewBaseListener()
  {
  }
  ~JoinFestaPlayerListLowerViewBaseListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_UPDATE_BUTTON,
    INPUT_EVENT_ID_TOUCH_DECIDE_BUTTON,
    INPUT_EVENT_ID_PUSH_L_BUTTON,
    INPUT_EVENT_ID_PUSH_R_BUTTON,
    INPUT_EVENT_ID_TOUCH_INFO_BUTTON,
    /*  photo */
    INPUT_EVENT_ID_TOUCH_PHOTOVIEW_BUTTON,
    INPUT_EVENT_ID_TOUCH_SLIDESHOW_BUTTON,
    INPUT_EVENT_ID_START                /**<  派生先EventID開始位置  */
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void TouchUpdateButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_UPDATE_BUTTON ); }
  void TouchDecideButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_DECIDE_BUTTON ); }
  void PushLButton(){ SetInputEventID( INPUT_EVENT_ID_PUSH_L_BUTTON ); }
  void PushRButton(){ SetInputEventID( INPUT_EVENT_ID_PUSH_R_BUTTON ); }
  void TouchInfoButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_INFO_BUTTON ); }
  /*  photo  */
  void TouchPhotoViewButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_PHOTOVIEW_BUTTON ); }
  void TouchSlideShowButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_SLIDESHOW_BUTTON ); }
};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTLOWERVIEWBASELISTENER_H_INCLUDE
