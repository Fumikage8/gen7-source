//======================================================================
/**
 * @file FieldGimmickEncountData.h
 * @date 2015/08/20 11:54:38
 * @author saita_kazuki
 * @brief ギミックエンカウントバイナリデータフォーマット
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_DATA_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_DATA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 *  @brief 待機型データ
 */
struct StandbyData
{
  u32   actionSEID;       ///< 予備動作SEラベル
  u32   jumpSEID;         ///< 飛び出しSEラベル
  u32   jumpMoveSEID;     ///< 飛び出し移動時SEラベル
  u32   scriptID;         ///< スクリプトID
  u8    prob;             ///< ギミック発生確率
  b8    isAllwaysDraw;    ///< 常時表示フラグ
  u8    padding[2];
};

/**
 *  @brief 接近型データ
 */
struct ChaseData
{
  f32   moveSpeedRate;    ///< 移動速度割合
  u32   standbySEID;      ///< 待機SEラベル
  u32   moveSEID;         ///< 移動SEラベル
  u32   jumpMoveSEID;     ///< 飛び出し移動時SEラベル
  u32   scriptID;         ///< スクリプトID
  u8    prob;             ///< ギミック発生確率
  u8    padding[3];
};

/**
 *  @brief 巡回型データ
 */
struct PatrolData
{
  f32   moveSpeedRate;    ///< 移動速度割合
  u32   standbySEID;      ///< 待機SEラベル
  u32   moveSEID;         ///< 移動SEラベル
  u32   jumpMoveSEID;     ///< 飛び出し移動時SEラベル
  u32   scriptID;         ///< スクリプトID
  u8    prob;             ///< ギミック発生確率
  u8    isJumpMove;       ///< 飛び出し演出をするか
  u8    padding[2];
};

/**
 *  @brief 逃走型データ
 */
struct EscapeData
{
  f32   moveSpeedRate;    ///< 移動速度割合
  u32   standbySEID;      ///< 待機SEラベル
  u32   moveSEID;         ///< 移動SEラベル
  u32   jumpMoveSEID;     ///< 飛び出し移動時SEラベル
  u32   scriptID;         ///< スクリプトID
  u8    prob;             ///< ギミック発生確率
  u8    padding[3];
};

/**
 *  @brief ポケモンデータ
 */
struct PokeData
{
  f32   motionStartFrame; ///< モーション開始フレーム
  f32   motionStepFrame;  ///< モーションステップフレーム
  f32   moveCurve;        ///< 移動時カーブ値
  u32   dropItemID;       ///< ドロップアイテムID
  u16   monsNo;           ///< モンスターID
  u8    motionType;       ///< モーションタイプ(バトルorフィールド)
  u8    motionID;         ///< モーションID
};

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_ENCOUNT_DATA_H_INCLUDED__