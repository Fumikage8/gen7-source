//===================================================================
/**
 * @file    StaticModelPackAccessor.h
 * @brief   配置モデルクラス
 * @author  saita_kazuki
 * @date    2015.04.13
 */
//===================================================================

#if !defined( STATIC_MODEL_PACK_ACCESSOR_H_INCLUDED )
#define STATIC_MODEL_PACK_ACCESSOR_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include "Field/FieldRo/include/StaticModel/StaticModelDefine.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @class 1つの配置モデルパックのアクセサー
 */
class StaticModelPackAccessor
{
public:

  ///< 1ファイルで複数アニメーションをフレームで制御するアニメーション用のワーク
  struct FrameControlAnimationWork
  {
    f32 start_StartFrame;  ///< 開始アニメの開始フレーム
    f32 start_EndFrame;    ///< 開始アニメの終了フレーム
    f32 loop_StartFrame;   ///< ループアニメの開始フレーム
    f32 loop_EndFrame;     ///< ループアニメの終了フレーム
    f32 end_StartFrame;    ///< 終了アニメの開始フレーム
    f32 end_EndFrame;      ///< 終了アニメの開始フレーム
  };

  ///< 配置モデル情報
  ///< ここを変更するときはコンバーターも変更する必要がある
  struct StaticModelInfo
  {
    u16   modelID;                    ///< 配置モデルID
    u8    programID;                  ///< プログラム指定
    b8    isAnimeExist;               ///< 配置モデルにアニメーションが含まれているか
    b8    isDoor;                     ///< ドアか
    b8    lodType;                    ///< LOD挙動
    u8    padding0[2];
    f32   timeAnimeStart_StartFrame;  ///< 時間アニメ開始の開始フレーム
    f32   timeAnimeStart_EndFrame;    ///< 時間アニメ開始の終了フレーム
    f32   timeAnimeLoop_StartFrame;   ///< 時間アニメループの開始フレーム
    f32   timeAnimeLoop_EndFrame;     ///< 時間アニメループの終了フレーム
    f32   timeAnimeEnd_StartFrame;    ///< 時間アニメ終了の開始フレーム
    f32   timeAnimeEnd_EndFrame;      ///< 時間アニメ終了の開始フレーム
    s32   timeAnimeStartHour;         ///< 時間アニメの開始時間
    s32   timeAnimeStartMinute;       ///< 時間アニメの開始分
    s32   timeAnimeEndHour;           ///< 時間アニメの終了時間
    s32   timeAnimeEndMinute;         ///< 時間アニメの終了分
    u8    startTimeZone;              ///< 時間アニメ開始タイムゾーン(TimeZone::Zoneとして扱う)
    u8    endTimeZone;                ///< 時間アニメ終了タイムゾーン(TimeZone::Zoneとして扱う)
    u8    cullingType;                              ///< カリング種類
    u8                        BBAnimationType;      ///< バウンディングボックスアニメーションコリジョン種類
    FrameControlAnimationWork BBAnimationFrameWork; ///< バウンディングボックスアニメーションのフレーム
  };

public:

  /**
   * @brief コンストラクタ
   */
  StaticModelPackAccessor();

  /**
   * @brief デストラクタ
   */
  virtual ~StaticModelPackAccessor();

  /**
   * @brief 配置モデル1つのパックバイナリをセット
   */
  void SetData( void* pData);

  /**
   * @brief 配置モデル情報バイナリを取得
   * @return 配置モデル情報バイナリ。アニメーション情報などが格納されている
   */
  void* GetStaticModelInfo() const ;

  /**
   * @brief モデルリソースバイナリを取得
   * @param modelType どのモデルのリソースか。ハイ、ミドル、ローから選択
   * @return モデルリソースバイナリ
   */
  void* GetModelBinary( ModelType modelType) const ;

  /**
   * @brief モーションリソースバイナリを取得
   * @param modelType どのモデルのリソースか。ハイ、ミドル、ローから選択
   * @return モーションリソース
   */
  void* GetMotionBinary( ModelType modelType, u32 index) const ;

  /**
   * @brief バウンディングボックスアニメーションコリジョンモデル取得
   * @return  バウンディングボックスアニメーションコリジョンモデル。なければNULL
   */
  void* GetBBAnimationCollisionModel() const ;

private:

  ///< 1つの配置モデルデータパックのインデックス
  enum PackIndex
  {
    // 配置モデル情報(アニメーション情報など)
    PACK_INDEX_INFO,

    // ハイモデルのリソース
    PACK_INDEX_HIGH_MODEL,
    PACK_INDEX_HIGH_ANIMATION_LOOP_0,
    PACK_INDEX_HIGH_ANIMATION_ALL_TIME_0,
    PACK_INDEX_HIGH_ANIMATION_EVENT_0,
    PACK_INDEX_HIGH_ANIMATION_EVENT_1,
    PACK_INDEX_HIGH_ANIMATION_EVENT_2,
    PACK_INDEX_HIGH_ANIMATION_EVENT_3,
    PACK_INDEX_HIGH_ANIMATION_TIME_0,
    PACK_INDEX_HIGH_ANIMATION_BB_0,

    // ミドルモデルのリソース
    PACK_INDEX_MIDDLE_MODEL,
    PACK_INDEX_MIDDLE_ANIMATION_LOOP_0,
    PACK_INDEX_MIDDLE_ANIMATION_ALL_TIME_0,
    PACK_INDEX_MIDDLE_ANIMATION_EVENT_0,
    PACK_INDEX_MIDDLE_ANIMATION_EVENT_1,
    PACK_INDEX_MIDDLE_ANIMATION_EVENT_2,
    PACK_INDEX_MIDDLE_ANIMATION_EVENT_3,
    PACK_INDEX_MIDDLE_ANIMATION_TIME_0,
    PACK_INDEX_MIDDLE_ANIMATION_BB_0,

    // ローモデルのリソース
    PACK_INDEX_LOW_MODEL,
    PACK_INDEX_LOW_ANIMATION_LOOP_0,
    PACK_INDEX_LOW_ANIMATION_ALL_TIME_0,
    PACK_INDEX_LOW_ANIMATION_EVENT_0,
    PACK_INDEX_LOW_ANIMATION_EVENT_1,
    PACK_INDEX_LOW_ANIMATION_EVENT_2,
    PACK_INDEX_LOW_ANIMATION_EVENT_3,
    PACK_INDEX_LOW_ANIMATION_TIME_0,
    PACK_INDEX_LOW_ANIMATION_BB_0,

    // バウンディングボックスアニメーションコリジョンモデル
    PACK_INDEX_BB_ANIMATION_COLLISION_MODEL,

    PACK_INDEX_MAX,

    // ループアクセス用定義
    PACK_INDEX_HIGH_ANIMATION_START   = PACK_INDEX_HIGH_ANIMATION_LOOP_0,
    PACK_INDEX_MIDDLE_ANIMATION_START = PACK_INDEX_MIDDLE_ANIMATION_LOOP_0,
    PACK_INDEX_LOW_ANIMATION_START    = PACK_INDEX_LOW_ANIMATION_LOOP_0,
  };

private:

  void* m_pStaticModelPack;   ///< 配置モデル1つ分のリソースのパック
};

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )

#endif // STATIC_MODEL_PACK_ACCESSOR_H_INCLUDED
