#if !defined( __RESORT_UPPER_BG_DRAW_H__ )
#define __RESORT_UPPER_BG_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortUpperBGDraw.h
 * @date    2015/09/08 18:32:17
 * @author  fukushima_yuya
 * @brief   ポケリゾート：BG画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortUpperBaseDraw.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   上画面BGの描画クラス
//------------------------------------------------------------------------------
class UpperBGDraw
  : public UpperBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(UpperBGDraw);

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  UpperBGDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~UpperBGDraw( void ) { ; }


private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void Initialize2D( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   初期アニメ
   */
  //------------------------------------------------------------------
  virtual void InitializeAnim( void );
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_UPPER_BG_DRAW_H__