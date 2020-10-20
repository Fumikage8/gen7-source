//======================================================================
/**
 * @file PokeModelPokeFinderMotion.h
 * @date 2015/10/28 15:56:39
 * @author ariizumi_nobuhiko
 * @brief ポケモンモデルのポケファインダー用のモーション定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __POKE_MODEL_POKE_FINFER_MOTION_H__
#define __POKE_MODEL_POKE_FINFER_MOTION_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(PokeTool)


////このモーションはモーションの種類をポケファインダーにした時専用の定義です。
////バトル、可愛がり、フィールドの場合はPokeModel.hのMODEL_ANIMEを使って下さい。
/// ポケファインダー専用アニメーションタイプ定義
typedef enum
{
  ///バトル用モーション
  MODEL_ANIME_PF_BT_WAIT_A = 0,   ///待機A
  MODEL_ANIME_PF_BT_WAIT_B,       ///待機B
  MODEL_ANIME_PF_BT_WAIT_C,       ///待機C
  MODEL_ANIME_PF_LAND_A,          ///登場1(飛び出し)
  MODEL_ANIME_PF_LAND_B,          ///登場1(落下ループ)
  MODEL_ANIME_PF_LAND_C,          ///登場1(着地)
  MODEL_ANIME_PF_LAND,            ///登場2
  MODEL_ANIME_PF_APPEAL,          ///メガシンカアピール
  MODEL_ANIME_PF_BUTURI1,         ///物理攻撃１
  MODEL_ANIME_PF_BUTURI2,         ///物理攻撃２
  MODEL_ANIME_PF_BUTURI3,         ///物理攻撃３
  MODEL_ANIME_PF_BUTURI4,         ///物理攻撃４
  MODEL_ANIME_PF_TOKUSHU1,        ///特殊攻撃１
  MODEL_ANIME_PF_TOKUSHU2,        ///特殊攻撃２
  MODEL_ANIME_PF_TOKUSHU3,        ///特殊攻撃３
  MODEL_ANIME_PF_TOKUSHU4,        ///特殊攻撃４
  MODEL_ANIME_PF_DAMAGE_S,        ///ダメージ
  MODEL_ANIME_PF_DOWN,            ///ダウン
  ///可愛がり用モーション        
  MODEL_ANIME_PF_KW_WAIT,         ///待機
  MODEL_ANIME_PF_RESPOND,         ///反応する
  MODEL_ANIME_PF_TURN_A,          ///振り向き１
  MODEL_ANIME_PF_TURN_B,          ///振り向き２
  MODEL_ANIME_PF_DROWSE_A,        ///うたたね(開始)
  MODEL_ANIME_PF_DROWSE_B,        ///うたたね(ループ)
  MODEL_ANIME_PF_DROWSE_C,        ///うたたね(起きる)
  MODEL_ANIME_PF_SLEEP_A,         ///ねむる(ループ)
  MODEL_ANIME_PF_SLEEP_B,         ///ねむる(目を覚ます)
  MODEL_ANIME_PF_HATE,            ///いやがる
  MODEL_ANIME_PF_QUESTION,        ///わからない
  MODEL_ANIME_PF_HAPPY_A,         ///よろこぶ１
  MODEL_ANIME_PF_HAPPY_B,         ///よろこぶ２
  MODEL_ANIME_PF_HAPPY_C,         ///よろこぶ３
  MODEL_ANIME_PF_MOVE_A,          ///くせ１
  MODEL_ANIME_PF_MOVE_B,          ///くせ２
  MODEL_ANIME_PF_MOVE_C,          ///くせ３
  MODEL_ANIME_PF_MOVE_D,          ///くせ４
  MODEL_ANIME_PF_LONELY,          ///さみしがる
  MODEL_ANIME_PF_PLAY_A,          ///かまってほしい１
  MODEL_ANIME_PF_PLAY_B,          ///かまってほしい２
  MODEL_ANIME_PF_MAD,             ///怒る
  MODEL_ANIME_PF_EAT_A,           ///かぶりつく(開始)
  MODEL_ANIME_PF_EAT_B,           ///かぶりつく(ループ)
  MODEL_ANIME_PF_EAT_C,           ///かぶりつく(終了)
  MODEL_ANIME_PF_TOUCH,           ///タッチ
  ///フィールド用モーション      
  MODEL_ANIME_PF_FI_WAIT_A,       //移動専用待機01
  MODEL_ANIME_PF_FI_WAIT_B,       //移動専用待機02（ランダム再生）
  MODEL_ANIME_PF_WALK01,          //歩き
  MODEL_ANIME_PF_RUN01,           //走り
  MODEL_ANIME_PF_MERGE_WALK01,    //(待機＞歩き＞待機)
  MODEL_ANIME_PF_WAIT_WALK01,     //待機＞歩きへ移行
  MODEL_ANIME_PF_WALK_WAIT01,     //歩き＞待機へ移行
  MODEL_ANIME_PF_MERGE_RUN01,     //(待機＞走り＞待機)
  MODEL_ANIME_PF_WAIT_RUN01,      //待機＞走りへ移行
  MODEL_ANIME_PF_RUN_WAIT01,      //走り＞待機へ移行
  MODEL_ANIME_PF_MERGE_MOVE01,    //(歩き＞走り＞歩き)
  MODEL_ANIME_PF_WALK_RUN01,      //歩き＞走りへ移行
  MODEL_ANIME_PF_RUN_WALK01,      //走り＞歩きへ移行

  MODEL_ANIME_POKE_FINDER_NUM,

}MODEL_ANIME_POKE_FINDER;
GFL_NAMESPACE_END(PokeTool)

#endif // __POKE_MODEL_POKE_FINFER_MOTION_H__