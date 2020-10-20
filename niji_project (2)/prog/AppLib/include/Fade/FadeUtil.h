//======================================================================
/**
 * @file FadeUtil.h
 * @date 2015/07/23 11:12:38
 * @author miyachi_soichi
 * @brief フェード補助関数群
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FADE_UTIL_H_INCLUDED__
#define __FADE_UTIL_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Fade/include/gfl2_FadeTypes.h>

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)
GFL_NAMESPACE_BEGIN(Util)

/**
 *  @brief  フェードアウトに必要なパラメータを取得する
 *  @param  in_eType    ロードするフェードの種類
 *  @param  out_Anim    アニメーション番号
 *  @param  out_Pane    ペイン
 *  @retval true        アニメーション
 *  @retval false       カラー
 */
u32 GetFadeParamForFadeOut( gfl2::Fade::FADE_TYPE in_eType, s32 &out_Anim, s32 &out_Pane );

/**
 *  @brief  フェードインに必要なパラメータを取得する
 *  @param  in_eType    ロードするフェードの種類
 *  @param  out_Anim    アニメーション番号
 *  @param  out_Pane    ペイン
 *  @retval true        アニメーション
 *  @retval false       カラー
 */
u32 GetFadeParamForFadeIn( gfl2::Fade::FADE_TYPE in_eType, s32 &out_Anim, s32 &out_Pane );

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)

#endif // __FADE_UTIL_H_INCLUDED__