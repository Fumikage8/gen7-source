//===================================================================
/**
 * @file    FieldEffectEmitterControl.h
 * @brief   エフェクトエミッタ操作クラス
 * @author  saita_kazuki
 * @date    2015.03.16
 */
//===================================================================

#if !defined( FIELD_EFFECT_CONTROLH_INCLUDED )
#define FIELD_EFFECT_CONTROLH_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

// Effect
#include <Effect/include/gfl2_EffectHandle.h>
#include <Effect/include/gfl2_EffectEmitterSet.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class EmitterControl
{
  GFL_FORBID_COPY_AND_ASSIGN(EmitterControl); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief       コンストラクタ
   */
  EmitterControl();

  /**
   * @brief       デストラクタ
   */
  virtual ~EmitterControl();

  /**
   * @brief       エミッタが有効かどうか
   * @param       エフェクトハンドル
   * @retval      true 有効
   * @retval      false 無効
   */
  static b8 IsValid( gfl2::Effect::Handle& handle);

  /**
   * @brief       位置の設定
   * @param       エフェクトハンドル
   * @param[in]   pos 位置
   */
  static void SetPos( gfl2::Effect::Handle& handle, gfl2::math::Vector3& pos);

  /**
   * @brief       位置の取得
   * @param       エフェクトハンドル
   * @param[out]  pos 位置
   */
  static void GetPos( gfl2::Effect::Handle& handle, gfl2::math::Vector3& pos);

  /**
   * @brief       エミッタの放出を停止
   * @param       エフェクトハンドル
   */
  void Fade( gfl2::Effect::Handle& handle);

  /**
   * @brief       エミッタの放出を停止するフラグが設定されたか
   * @param       エフェクトハンドル
   * @retval      true 設定された
   * @retval      false 設定されていない
   */
  static b8 IsFadeRequest( gfl2::Effect::Handle& handle);

  /**
   * @brief       フェードリクエストをキャンセル
   * @param       エフェクトハンドル
   */
  static void CancelFade( gfl2::Effect::Handle& handle);

  /**
   * @brief       アルファ値を設定
   * @param       エフェクトハンドル
   * @param[in]   alpha アルファ値 0.0f〜1.0f
   */
  static void SetAlpha( gfl2::Effect::Handle& handle, f32 alpha);

  /**
   * @brief       パーティクル放出レートのスケール値を設定
   * @param       エフェクトハンドル
   * @param[in]   emitterID エミッタID。0〜エミッタセット生成時の所属エミッタ数までが有効。
   *              エミッタセットヘッダファイルのエミッタIDが指定できる
   * @param[in]   ratio 放出レートのスケール値 0.0f〜1.0f
   */
  static void SetEmissionRatio( gfl2::Effect::Handle& handle, s32 emitterID, f32 ratio);

  /**
   * @brief       パーティクル放出間隔のスケール値を設定
   * @param       エフェクトハンドル
   * @param[in]   emitterID エミッタID。0〜エミッタセット生成時の所属エミッタ数までが有効。
   *              エミッタセットヘッダファイルのエミッタIDが指定できる
   * @param[in]   ratio 放出間隔のスケール値 0.0f〜1.0f
   */
  static void SetEmissionInterval( gfl2::Effect::Handle& handle, s32 emitterID, f32 ratio);

};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // FIELD_EFFECT_CONTROLH_INCLUDED
