//=============================================================================
/**
 * @file    ShopAppParam.h
 * @brief   ショップ購入画面外部設定パラメータ
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================
#if !defined( SHOP_APP_PARAM_H_INCLUDED )
#define SHOP_APP_PARAM_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// module
#include "ShopDef.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Shop)

//! 外部設定パラメータ
struct APP_PARAM
{
  // [in]
  ShopType type;        //!< ショップタイプ
  u32 id;               //!< ショップID

  u32 howmany_msg;      //!< メッセージ：いくつ買う？
  u32 really_msg;       //!< メッセージ：xxコでxx円です
  u32 really_msg2;      //!< メッセージ：xxコでxx円です（複数形）（ローカライズ対応）
  u32 hereitis_msg;     //!< メッセージ：まいど
  u32 youcanttake_msg;  //!< メッセージ：持ちきれない
  u32 nomoney_msg;      //!< メッセージ：お金が足りない
  u32 selected_msg;     //!< メッセージ：今日は終了しました（フェスショップ用）

  IShopFeatureDelegate*  pShopFeatureDelegate;  //!< Shop機能の委譲（FesCircle用）

  // [out]
  s16 select_action;    //!< フェスコン（行動系）/ BP技覚え用：選択した行動/技番号
  s16 select_cost;      //!< フェスコン（行動系）/ BP技覚え用：選択した行動で消費するコイン/BP
  u32 select_pos;       //!< フェスコン（行動系）/ BP技覚え用：選択したリスト位置　※キャンセル時は不定値
};

// キャンセル定義
enum
{
  // フェス用
  FC_SELECT_ACTION_CANCEL = -1, //!< 選択した行動
  FC_SELECT_COST_CANCEL   = 0,  //!< 選択した行動で消費するコイン

  // BP技覚え用
  BP_WAZA_SELECT_ACTION_CANCEL = FC_SELECT_ACTION_CANCEL,   //!< 選択した技番号
  BP_WAZA_SELECT_COST_CANCEL = FC_SELECT_COST_CANCEL,       //!< 消費するBP
};


GFL_NAMESPACE_END(Shop)
GFL_NAMESPACE_END(App)

#endif  // SHOP_APP_PARAM_H_INCLUDED
