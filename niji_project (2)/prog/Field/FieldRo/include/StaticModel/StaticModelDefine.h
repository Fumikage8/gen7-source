//===================================================================
/**
 * @file    StaticModelDefine.h
 * @brief   管理クラス、モデルクラスで共有する定義
 * @author  saita_kazuki
 * @date    2015.04.08
 */
//===================================================================

#if !defined( STATIC_MODEL_DEFINE_H_INCLUDED )
#define STATIC_MODEL_DEFINE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

 ///< モデルの種類
 enum ModelType
 {
   MODEL_TYPE_HIGH,     // ハイモデル。地形ブロックと同タイミングで生成、破棄
   MODEL_TYPE_MIDDLE,   // ミドルモデル。常駐させる
   MODEL_TYPE_LOW,      // ローモデル。常駐させる

   MODEL_TYPE_MAX,

   MODEL_TYPE_ALL   = MODEL_TYPE_MAX, // 全て
   MODEL_TYPE_NONE  = MODEL_TYPE_MAX, // なし
 };

 ///< アニメーションの種類
 enum AnimationType
 {
   ANIMATION_TYPE_LOOP    ,  ///< ループアニメ。ループするだけ
   ANIMATION_TYPE_ALL_TIME,  ///< 24時間アニメ。全ての時間帯で動くアニメーション
   ANIMATION_TYPE_EVENT,     ///< イベントアニメ。イベントやスクリプトから起動するアニメーション
   ANIMATION_TYPE_TIME,      ///< 時間アニメ。指定された時間帯に起動するアニメーション
   ANIMATION_TYPE_BB,        ///< バウンディングボックスアニメーション。専用のコリジョンに当たったときに起動するアニメーション

   ANIMATION_TYPE_MAX,
 };

///< アニメーション番号 FieldPawnTypes.h ScrStaticModelAnimationSlotと同期をとること
enum AnimationIndex
{
  ANIMATION_INDEX_LOOP_0,         ///< ループアニメ

  ANIMATION_INDEX_ALL_TIME_0,     ///< 24時間アニメ

  ANIMATION_INDEX_EVENT_0,        ///< イベントアニメ
  ANIMATION_INDEX_EVENT_1,
  ANIMATION_INDEX_EVENT_2,
  ANIMATION_INDEX_EVENT_3,

  ANIMATION_INDEX_TIME_0,         ///< 時間アニメ。フレーム数で開始、ループ、終了の3つのアニメを制御する

  ANIMATION_INDEX_BB_0,           ///< バウンディングボックスアニメ。フレーム数で開始、ループ、終了の3つのアニメを制御する

  ANIMATION_INDEX_MAX,

  ANIMATION_INDEX_NONE = ANIMATION_INDEX_MAX,
};

///< イベントアニメーション番号
enum EventAnimationIndex
{
  EVENT_ANIMATION_INDEX_0,
  EVENT_ANIMATION_INDEX_1,
  EVENT_ANIMATION_INDEX_2,
  EVENT_ANIMATION_INDEX_3,

  EVENT_ANIMATION_INDEX_MAX,
};

///< バウンディングボックスアニメーションコリジョンの種類
enum BBAnimationCollisionType
{
  BB_ANIMATION_COLLISION_TYPE_CYLINDER,   ///< シリンダー
  BB_ANIMATION_COLLISION_TYPE_BOX,        ///< ボックス

  BB_ANIMATION_COLLISION_TYPE_NONE = 0xFF ///< なし
};

///< カリングの種類
enum CullingType
{
  CULLING_TYPE_ORIGIN,    ///< モデル原点からAABBを作成してカリングを行う
  CULLING_TYPE_AABB,      ///< モデルAABBを使用してカリングを行う
  CULLING_TYPE_NONE,      ///< カリングを行わない
};

///< LOD挙動の種類
enum LODType
{
  LOD_TYPE_VISIBLE_OFF,     ///< 表示をOFFにする
  LOD_TYPE_VISIBLE_KEEP,    ///< 表示を継続する(見た目を切り替えるかはLODの登録による)
};

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )

#endif // STATIC_MODEL_DEFINE_H_INCLUDED
