#ifndef __APP_FRAME_LISTENER_H__
#define __APP_FRAME_LISTENER_H__
//=============================================================================
/**
 * @file    AppFrameListener.h
 * @brief   メニューフレームの状態通知クラス
 * @author  k.ohno
 * @date    2015.3.9
 */
//=============================================================================
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include "gfx/include/gfl2_types.h"


//===========================================================================================

GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame ) 

class CellSuper;

class AppFrameListener {
//  GFL_FORBID_COPY_AND_ASSIGN(AppFrameListener); //コピーコンストラクタ＋代入禁止


public:

  //------------------------------------------------------------------
  /**
   * @brief	  EndFuncが完了した直後に呼び出される関数
   */
  //------------------------------------------------------------------
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell ) = 0;

};


GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )


#endif// __APP_FRAME_LISTENER_H__
