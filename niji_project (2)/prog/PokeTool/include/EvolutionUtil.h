//======================================================================
/**
 * @file EvolutionUtil.h
 * @date 2015/09/10 17:30:31
 * @author saita_kazuki
 * @brief ポケモン進化用補助ツール
 * @note sangoから移殖
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __POKE_TOOL_EVOLUTION_UTIL_H_INCLUDED__
#define __POKE_TOOL_EVOLUTION_UTIL_H_INCLUDED__
#pragma once

#include <pml/include/pmlib.h>

namespace GameSys {
  class GameManager;
}

GFL_NAMESPACE_BEGIN( PokeTool )

//======================================================================
//
//    進化条件パラメータ生成
//
//======================================================================
//------------------------------------------------------------------------
/**
 * @brief 進化条件パラメータのセットアップ処理
 * @param[out]  situation   セットアップ対象へのポインタ
 * @param[in]   p_gman      ゲームマネジャーへのポインタ
 * @param[in]   is_device_turned_over 逆さ判定の結果
 * @param[in]   weather 天候種類
 * @note  呼び出し元でDeviceTurnCheckerを使って引数を入れてください。
 */
//------------------------------------------------------------------------
extern void SetupEvolveSituation(
  pml::pokepara::EvolveSituation * situation,
  GameSys::GameManager * p_gman,
  bool is_device_turned_over,
  Field::weather::WeatherKind weather
  );

GFL_NAMESPACE_END( PokeTool )

#endif // __POKE_TOOL_EVOLUTION_UTIL_H_INCLUDED__