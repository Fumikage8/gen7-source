#if !defined( __BTL_POSEFF_H__ )
#define __BTL_POSEFF_H__
//=============================================================================================
/**
 * @file    btl_PosEff.h
 * @brief   ポケモンXY バトル -システム - 位置エフェクト種類等 -
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================
#pragma once

enum {
//  BTL_POSEFF_PARAM_MAX = 4,
  POSEFF_PARAM_MAX = 4,
};

/**
 *  位置エフェクト種類
 */
typedef enum {

  BTL_POSEFF_NEGAIGOTO,
  BTL_POSEFF_MIKADUKINOMAI,
  BTL_POSEFF_IYASINONEGAI,
  BTL_POSEFF_DELAY_ATTACK,
  BTL_POSEFF_BATONTOUCH,
  BTL_POSEFF_ZENRYOKU_EFFECT_REPAIR_HP, // 全力技効果「次に場に出たポケモンのHPを回復」

  BTL_POSEFF_MAX,
  BTL_POSEFF_NULL = BTL_POSEFF_MAX,

}BtlPosEffect;

#endif // __BTL_POSEFF_H__
