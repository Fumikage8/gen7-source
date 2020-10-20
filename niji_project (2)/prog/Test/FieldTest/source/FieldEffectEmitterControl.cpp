//===================================================================
/**
 * @file    FieldEffectEmitterControl.cpp
 * @brief   エフェクトエミッタ操作クラス
 * @author  saita_kazuki
 * @date    2015.03.16
 */
//===================================================================

#include "Test/FieldTest/include/FieldEffectEmitterControl.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 * @brief       コンストラクタ
 */
EmitterControl::EmitterControl()
{
}

/**
 * @brief       デストラクタ
 */
EmitterControl::~EmitterControl()
{
}

/**
 * @brief       エミッタが有効かどうか
 * @param       エフェクトハンドル
 * @retval      true 有効
 * @retval      false 無効
 */
b8 EmitterControl::IsValid( gfl2::Effect::Handle& handle)
{
  return handle.IsValid();
}

/**
 * @brief       位置の設定
 * @param       エフェクトハンドル
 * @param[in]   pos 位置
 */
void EmitterControl::SetPos( gfl2::Effect::Handle& handle, gfl2::math::Vector3& pos)
{
  gfl2::Effect::EmitterSet::SetGFPos( handle.GetEmitterSet(), pos);
}

/**
 * @brief       位置の取得
 * @param       エフェクトハンドル
 * @param[out]  pos 位置
 */
void EmitterControl::GetPos( gfl2::Effect::Handle& handle, gfl2::math::Vector3& pos)
{
  gfl2::Effect::EmitterSet::GetGFPos( handle.GetEmitterSet(), pos);
}

/**
 * @brief       エミッタの放出を停止
 * @param       エフェクトハンドル
 */
void EmitterControl::Fade( gfl2::Effect::Handle& handle)
{
  handle.GetEmitterSet()->Fade();
}

/**
 * @brief       エミッタの放出を停止するフラグが設定されたか
 * @param       エフェクトハンドル
 * @retval      true 設定された
 * @retval      false 設定されていない
 */
b8 EmitterControl::IsFadeRequest( gfl2::Effect::Handle& handle)
{
  return handle.GetEmitterSet()->IsFadeRequest();
}

/**
 * @brief       フェードリクエストをキャンセル
 * @param       エフェクトハンドル
 */
void EmitterControl::CancelFade( gfl2::Effect::Handle& handle)
{
  handle.GetEmitterSet()->CancelFade();
}

/**
 * @brief       アルファ値を設定
 * @param       エフェクトハンドル
 * @param[in]   alpha アルファ値 0.0f〜1.0f
 */
void EmitterControl::SetAlpha( gfl2::Effect::Handle& handle, f32 alpha)
{
  handle.GetEmitterSet()->SetAlpha( alpha);
}

/**
 * @brief       パーティクル放出レートのスケール値を設定
 * @param       エフェクトハンドル
 * @param[in]   emitterID エミッタID。0〜エミッタセット生成時の所属エミッタ数までが有効。
 *              エミッタセットヘッダファイルのエミッタIDが指定できる
 * @param[in]   ratio 放出レートのスケール値 0.0f〜1.0f
 */
void EmitterControl::SetEmissionRatio( gfl2::Effect::Handle& handle, s32 emitterID, f32 ratio)
{
  handle.GetEmitterSet()->GetEmitterController( emitterID)->SetEmissionRatio( ratio);
}

/**
 * @brief       パーティクル放出間隔のスケール値を設定
 * @param       エフェクトハンドル
 * @param[in]   emitterID エミッタID。0〜エミッタセット生成時の所属エミッタ数までが有効。
 *              エミッタセットヘッダファイルのエミッタIDが指定できる
 * @param[in]   ratio 放出間隔のスケール値 0.0f〜1.0f
 */
void EmitterControl::SetEmissionInterval( gfl2::Effect::Handle& handle, s32 emitterID, f32 ratio)
{
  handle.GetEmitterSet()->GetEmitterController( emitterID)->SetEmissionInterval( ratio);
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
