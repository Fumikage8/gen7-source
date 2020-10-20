#ifndef NIJI_PROJECT_APP_HERO_SELECT_DRAW_LISTENER_H_INCLUDED
#define NIJI_PROJECT_APP_HERO_SELECT_DRAW_LISTENER_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file HeroSelectDrawListener.h
 * @date 2015/08/26 11:55:02
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   DrawListenerクラス
//------------------------------------------------------------------------------
class HeroSelectDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   画面のオープン
   */
  //--------------------------------------------------------------------------
  virtual void OpenHeroSelect( void ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   画面のクローズ
   */
  //--------------------------------------------------------------------------
  virtual void CloseHeroSelect( void ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   画面の終了通知
   */
  //--------------------------------------------------------------------------
  virtual void EndHeroSelect( void ) = 0;
};

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )


#endif // NIJI_PROJECT_APP_HERO_SELECT_DRAW_LISTENER_H_INCLUDED