//=============================================================================
/**
 *
 *	@file		gfl2_FadeTypes.h
 *	@brief  フェードに必要なデフォルト、共通定義
 *	@author k.ohno
 *	@data		2015.04.06
 *
 */
//=============================================================================
#ifndef __GFL2_FADE_TYPES_H__
#define __GFL2_FADE_TYPES_H__
#pragma once

#include "macro/include/gfl2_Macros.h"
#include <types/include/gfl2_Typedef.h>


#define OPEN_FADE_MACRO  (1)


GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )

const int FADE_DEFAULT_SYNC = 2;

/**
 * @brief 画面指定
 */
enum DISP
{
  DISP_UPPER = 0,         ///< 上画面
  DISP_LOWER,             ///< 下画面
  DISP_CUSTOM_UPPER,
  DISP_CUSTOM_LOWER,
  DISP_DOUBLE,            ///< 両画面

  DISP_BUFF_MAX = 4,
};

/**
 * @brief   演出の種類
 */
enum FADE_TYPE
{
  FADE_TYPE_NONE = 0,     //!< 未設定
  FADE_TYPE_ALPHA,        //!< アルファ
  FADE_TYPE_BALL,         //!< ボール
  FADE_TYPE_MASK,         //!< マスク
  FADE_TYPE_DIAMOND,      //!< ダイア
  FADE_TYPE_CIRCLE,       //!< サークル
  FADE_TYPE_CROSS,        //!< クロスフェード
  FADE_TYPE_BALL_TILT,    //!< ボール(傾き)

  FADE_TYPE_MAX,
};

/**
 *  @brief  挙動の種類
 */
enum FADE_MODE
{
  MODE_ALPHA = 0,   //!< アルファ
  MODE_ANIME,       //!< アニメーション
  MODE_CROSS,       //!< クロスフェード
};

static const int SCREAN_WIDTH = 512;
static const int SCREAN_HEIGHT = 256;
static const int BLOCK_WIDTH_SIZE = 8;
static const int BLOCK_HEIGHT_SIZE = 8;

/**
 *  @brief  遷移後の結果
 */
enum FADE_RESULT
{
  FADE_RESULT_IN = 0,
  FADE_RESULT_OUT,
};


GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_END( gfl2 )



#endif // __GFL2_FADE_TYPES_H__
