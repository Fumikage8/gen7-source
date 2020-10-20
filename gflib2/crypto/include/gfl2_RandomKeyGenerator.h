// ============================================================================
/*
 * @file gfl2_RandomKeyGenerator.h
 * @brief ランダムなキーを生成するクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#if !defined( GFL2_RANDOMKEYGENERATOR_H_INCLUDE )
#define GFL2_RANDOMKEYGENERATOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


class RandomKeyGenerator
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  ランダムなキーを生成します。
   * @param  pRandomKey 生成したランダムなキーを格納するバッファのポインタ
   * @param  keySize 生成したランダムなキーを格納するバッファのサイズ
   * @retval true ランダムなキーの生成に成功
   * @retval false ランダムなキーの生成に失敗
   */
  // --------------------------------------------------------------------------
  static bool GenerateRandomKey( void* pRandomKey, u32 keySize );
};


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_RANDOMKEYGENERATOR_H_INCLUDE
