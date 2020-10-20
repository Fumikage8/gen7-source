//============================================================================================
/**
 * @file    app_util_Common.h
 * @brief   アプリ関連共通定義
 * @author  Hiroyuki Nakamura
 * @date    12.05.08
 */
//============================================================================================
#if !defined( __APP_UTIL_COMMON_H__ )
#define __APP_UTIL_COMMON_H__

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>
#include <Layout/include/gfl2_GraphicsType.h>
// pmlib
#include <pml/include/pml_PokePara.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

extern const u32 DEFAULT_FADE_SYNC;   //!< 共通フェード速度

extern const u32 DEFAULT_GET_COUNT_60F;   //!< 項目取得カウント(60frm用)
extern const u32 DEFAULT_GET_COUNT_30F;   //!< 項目取得カウント(30frm用)

extern const gfl2::lyt::ColorU8 SEIKAKU_POWER_UP;   //!< 性格による能力補正での、「こうげき」等見出しの色付け、上昇版
extern const gfl2::lyt::ColorU8 SEIKAKU_POWER_DOWN; //!< 性格による能力補正での、「こうげき」等見出しの色付け、下降版

//  レーダーチャートの基本値の色(フィールド、boxのステータスでレーダーチャートを表示しているので共通値に)
extern const gfl2::math::Vector4 g_raderChartPokeParamBaseColor;

//-----------------------------------------------------------------------------------------
/**
  * @brief  性格から得意・不得意のパラメータ項目を出力
  * @param  seiakku:        ポケモンの性格
  * @param  pOutGoodPoint:  得意なパラメータを出力
  * @param  pOutWeekPoint:  不得意なパラメータを出力
  */
//-----------------------------------------------------------------------------------------
extern void OutputSpecialPointPokemSeikaku( const pml::pokepara::Seikaku seikaku, pml::pokepara::PowerID* pOutGoodPoint, pml::pokepara::PowerID* pOutWeakPoint );

//-----------------------------------------------------------------------------------------
/**
  * @brief  引数指定したポケモンパラメータから現在のEXP値を取得
  * @param  pPokeParam      : 経験値取得対象ポケモンパラメータ
  * @param  nowLvNum        : 現在のポケモンのLV値
  * @return 指定したnowLvNumからレベルアップに必要な経験値
  */
//-----------------------------------------------------------------------------------------
extern u32 GetPokeNowLvExpNum( const pml::pokepara::PokemonParam* pPokeParam );

//-----------------------------------------------------------------------------------------
/**
  * @brief  引数指定したポケモンパラメータからレベルアップに必要なExp値を取得
  * @param  pPokeParam      : 経験値取得対象ポケモンパラメータ
  * @param  nowLvNum        : 現在のポケモンのLV値
  * @return 指定したnowLvNumからレベルアップに必要な経験値
  */
//-----------------------------------------------------------------------------------------
extern u32 GetPokeNextLvExpNum( const pml::pokepara::PokemonParam* pPokeParam );

//-----------------------------------------------------------------------------------------
/**
  * @brief  わざのPP値と最大PP値からのわざ名のテキスト色
  * @param  nowPPNum      : 現在のPP値
  * @param  maxPPNum      : わざのPP最大値
  * @param  defaultColor  : 比率が100の時の色(引数指定がないと白をデフォルト色とする)
  * @return PP値に従ったテキスト色
  */
//-----------------------------------------------------------------------------------------
extern const gfl2::lyt::ColorU8 GetWazaTextColor( const u32 nowPPNum, const u32 maxPPNum, const gfl2::lyt::ColorU8 defaultColor = gfl2::lyt::ColorU8( 255, 255, 255, 255 ) );

//-----------------------------------------------------------------------------------------
/**
  * @brief  ポケモンパラメータからポケモンのそれぞれのタイプを出力
  * @param[out]           : １つ目のタイプの出力
  * @param[out]           : ２つ目のタイプの出力
  * @param[in]            : ポケモンパラメータ
  */
//-----------------------------------------------------------------------------------------
extern void OutputPokemonType( pml::PokeType* pOutputType1, pml::PokeType* pOutputType2, const pml::pokepara::PokemonParam* pPokeParam );

//-----------------------------------------------------------------------------------------
/**
  * @brief  各タイプ設定からそれぞれのタイプを出力
  * @param[out]           : １つ目のタイプの出力
  * @param[out]           : ２つ目のタイプの出力
  * @param[in]            : １つ目のタイプの設定
  * @param[in]            : ２つ目のタイプの設定
  */
//-----------------------------------------------------------------------------------------
extern void OutputPokemonTypeManual( pml::PokeType* pOutputType1, pml::PokeType* pOutputType2, const pml::PokeType& rInType1, const pml::PokeType& rInType2 );

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

#endif // __APP_UTIL_COMMON_H__
