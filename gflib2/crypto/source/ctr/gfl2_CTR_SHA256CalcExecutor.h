// ============================================================================
/*
 * @file gfl2_CTR_SHA256CalcExecutor.h
 * @brief shaによるハッシュ値を計算するクラスです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_SHA256CALCEXECUTOR_H_INCLUDE )
#define GFL2_CTR_SHA256CALCEXECUTOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>
#include <crypto/include/gfl2_CryptoTypedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_SHA256CalcExecutor
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理を行います。
   * @param  pSHA256Hash 計算したハッシュを格納する為のバッファです
   * @param  pData ハッシュを計算する為のデータ
   * @param  dataSize ハッシュを計算する為のデータのサイズです
   */
  // --------------------------------------------------------------------------
  static bool CalcHash( gfl2::crypto::SHA256Hash* pSHA256Hash, const void* pData, u32 dataSize );
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_SHA256CALCEXECUTOR_H_INCLUDE
#endif // GF_PLATFORM_CTR
