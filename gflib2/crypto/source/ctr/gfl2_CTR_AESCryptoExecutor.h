// ============================================================================
/*
 * @file gfl2_CTR_AESCryptoExecutor.h
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_AESCRYPTOEXECUTOR_H_INCLUDE )
#define GFL2_CTR_AESCRYPTOEXECUTOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_AESCryptoExecutor
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化処理を行います。
   * @param  pTemporaryBuffer 暗号化後のデータを格納する為のバッファです
   * @param  pData 暗号化したいデータです
   * @param  dataSize 暗号化したいデータのサイズです
   * @param  pKey 暗号化復号化キー
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  static bool Encrypt( gfl2::crypto::TemporaryBuffer* pTemporaryBuffer, const void* pData, u32 dataSize, const void* pKey );

  // --------------------------------------------------------------------------
  /**
   * @brief  復号化処理を行います。
   * @param  pTemporaryBuffer 復号化後のデータを格納する為のバッファです
   * @param  pData 復号化したいデータです(暗号化されたデータです)
   * @param  dataSize 復号化したいデータのサイズです(暗号化されたデータのサイズです)
   * @param  pKey 暗号化復号化キー
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  static bool Decrypt( gfl2::crypto::TemporaryBuffer* pTemporaryBuffer, const void* pData, u32 dataSize, const void* pKey );
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_AESCRYPTOEXECUTOR_H_INCLUDE
#endif // GF_PLATFORM_CTR
