//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_EffectConfig.h
 *	@brief  エフェクトシステム生成用コンフィグ情報
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __GFL2_EFFECT_CONFIG_H__ )
#define __GFL2_EFFECT_CONFIG_H__
#pragma once

#include <effect/include/gfl2_EffectHeap.h>


#if defined(GF_PLATFORM_CTR)
#include <nw/eft.h>
#elif defined(GF_PLATFORM_WIN32)
#include <nw/effect.h>
#endif

#include <nw/eft/eft_Config.h>

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Effect )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#if defined(GF_PLATFORM_CTR)
typedef nw::eft::BufferLocation BufferLocation;  // メモリロケーション
#endif



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

#if defined(GF_PLATFORM_CTR)
typedef nw::eft::EnvInfo EnvInfo;  // 環境設定用構造体
#elif defined(GF_PLATFORM_WIN32)
typedef u32 EnvInfo;  // 環境設定用構造体(Win32では未使用)
#endif

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
/**
 *  @brief エフェクトコンフィグクラス
 *
 */
class Config : public nw::eft::Config
{
public:

  /**
   * @brief コンストラクタ
   */
  Config() : 
    nw::eft::Config()
  {
  }

private:
};


GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( gfl2 )

#endif	// __GFL2_EFFECT_CONFIG_H__

