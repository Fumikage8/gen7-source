//=================================================================================================
/**
 * @file     EventXMenuListener.h
 * @brief    メニューリスナー
 * @author   k.ohno
 * @date     2015.8.18
 */
//=================================================================================================
#pragma once
#ifndef __EVENT_XMENU_LISTENER_H__
#define __EVENT_XMENU_LISTENER_H__

#include <macro/include/gfl2_Macros.h>
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"




GFL_NAMESPACE_BEGIN(Field)


class EventXMenuListener
{
public:
  //  XMENU終了を伝えるListener
  virtual void SetXMenuEnd(void)=0;

  //  次の挙動を伝達する
  virtual void SetNextFunction(Field::SubScreen::FieldMenuOutRequestID)=0;


};




GFL_NAMESPACE_END(Field)


#endif // __EVENT_XMENU_LISTENER_H__
