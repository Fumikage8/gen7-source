// ============================================================================
/*
 * @file JoinFestaRecordDetailLowerViewListener.h
 * @brief レコード詳細画面
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTARECORDDETAILLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTARECORDDETAILLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaRecordDetailLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaRecordDetailLowerViewListener()
  {
  }
  ~JoinFestaRecordDetailLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTARECORDDETAILLOWERVIEWLISTENER_H_INCLUDE
