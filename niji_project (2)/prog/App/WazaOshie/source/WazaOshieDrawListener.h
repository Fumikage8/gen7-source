#ifndef NIJI_PROJECT_APP_WAZAOSHIE_DRAW_LISTENER_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_DRAW_LISTENER_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieDrawListener.h
 * @date 2015/07/01 14:53:47
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   技教え・技忘れ画面用のリスナー
//------------------------------------------------------------------------------
class WazaOshieDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   技教え・技忘れ画面の終了通知
   */
  //--------------------------------------------------------------------------
  virtual void EndWazaOshie( void ) = 0;
};
  
GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_DRAW_LISTENER_H_INCLUDED