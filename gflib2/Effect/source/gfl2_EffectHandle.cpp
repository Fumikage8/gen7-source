//======================================================================
/**
 * @file gfl2_EffectHandle.cpp
 * @date 2016/03/15 15:29:55
 * @author miyachi_soichi
 * @brief エフェクトハンドル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// self
#include <effect/include/gfl2_EffectHandle.h>
#include <effect/include/gfl2_EffectEmitterSet.h>
#include <effect/include/gfl2_EffectSystem.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @brief  エミッターセットの取得
 *  @return 設定されているエミッターセットのハンドル
 *
 *  @note   ハンドルが無効な場合、NULLが返却されるので必ずNULL返却への対応が必要
 */
nw::eft::EmitterSet *Handle::GetEmitterSet()
{
  {
    // ハンドルの有効チェック
    if( !nw::eft::Handle::IsValid() ){ return NULL; }
  }

  return nw::eft::Handle::GetEmitterSet();
}

/**
 *  @brief  エミッターセットの取得：コンスト版
 *  @return 設定されているエミッターセットのハンドル
 *
 *  @note   ハンドルが無効な場合、NULLが返却されるので必ずNULL返却への対応が必要
 */
const nw::eft::EmitterSet *Handle::GetEmitterSet() const
{
  {
    // ハンドルの有効チェック
    if( !nw::eft::Handle::IsValid() ){ return NULL; }
  }

  return nw::eft::Handle::GetEmitterSet();
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(gfl2)
