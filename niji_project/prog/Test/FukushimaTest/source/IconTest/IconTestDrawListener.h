#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_ICON_TEST_DRAW_LISTENER_H_INCLUDED__
#define __TEST_FUKUSHIMA_ICON_TEST_DRAW_LISTENER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file IconTestDrawListener.h
 * @date 2015/07/17 16:47:55
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   リスナー
//------------------------------------------------------------------------------
class IconTestDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   終了通知
   */
  //--------------------------------------------------------------------------
  virtual void OnEnd( void ) = 0;
};

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // __TEST_FUKUSHIMA_ICON_TEST_DRAW_LISTENER_H_INCLUDED__

#endif // PM_DEBUG