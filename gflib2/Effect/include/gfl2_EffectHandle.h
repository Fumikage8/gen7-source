//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_EffectHandle.h
 *	@brief  エフェクトハンドル
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __GFL2_EFFECT_HANDLE_H__ )
#define __GFL2_EFFECT_HANDLE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <nw/eft/eft_Handle.h>

// 前方宣言
namespace nw
{
  namespace eft
  {
    class EmitterSet;
  } // namespace eft
} // namespace nw

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Effect )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
/**
 *  @class  Handle
 *  @brief エフェクトハンドル再定義
 *  Handleから先の詳細な操作はNW_Cafe NW_CTRの物を使用
 *
 *  GetEmitterSetをオーバーライドして無効なハンドルならNullPtrにて返却を行う
 */
class Handle : public nw::eft::Handle
{
public:
  /**
   *  @brief  エミッターセットの取得
   *  @return 設定されているエミッターセットのハンドル
   *
   *  @note   ハンドルが無効な場合、NULLが返却されるので必ずNULL返却への対応が必要
   */
  nw::eft::EmitterSet *GetEmitterSet();

  /**
   *  @brief  エミッターセットの取得：コンスト版
   *  @return 設定されているエミッターセットのハンドル
   *
   *  @note   ハンドルが無効な場合、NULLが返却されるので必ずNULL返却への対応が必要
   */
  const nw::eft::EmitterSet *GetEmitterSet() const;
};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( gfl2 )

#endif	// __GFL2_EFFECT_HANDLE_H__

