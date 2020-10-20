//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldFootSoundAccessor.h
 *	@brief  足音データへのアクセサー
 *	@author	 tomoya takahashi
 *	@date		2015.07.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_FOOTSOUND_ACCESSOR_H__ )
#define __FIELD_FOOTSOUND_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>  // for b32

// field
#include "niji_conv_header/field/footsound/footsound.h"

GFL_NAMESPACE_BEGIN(Field)

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
 *  @class  足音アクセサー
 */
class FootSoundAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FootSoundAccessor );
public:

public:
  /**
   *  @brief コンストラクタ
   */
  FootSoundAccessor();
  /**
   *  @brief デストラクタ
   */
  virtual ~FootSoundAccessor();

  /**
   *  @brief 初期化
   *  @param  cpBuff バッファ
   *  @param  is_man  男か　　true:男 false:女
   */
  void Initialize( const void * cpBuff, bool is_man );

  /**
   *  @brief SEの再生
   *
   *  @param   before         ビフォアフレーム(before+1.0fからチェックします。)
   *  @param   after          アフターフレーム
   *  @param   animation_id   アニメーションID
   *  @param   attr           足元のアトリビュート
   */
  void Update( f32 before, f32 after, u32 animation_id, u32 attr ) const;

private:
  const void * m_cpBuff;
  bool m_isMan;
};


GFL_NAMESPACE_END(Field)

#endif	// __FIELD_FOOTSOUND_ACCESSOR_H__

