// ============================================================================
/*
 * @file gfl2_CTR_RSACryptoExecutor.h
 * @brief 署名データの作成と検証を行うクラスです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_RSACRYPTOEXECUTOR_H_INCLUDE )
#define GFL2_CTR_RSACRYPTOEXECUTOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <crypto/include/gfl2_CryptoTypedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_RSACryptoExecutor
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成を行います。
   * @param  param 署名データを作成する為に必要なパラメータです
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  static bool CreateSignature( RSACreateSignatureCommandParam& param );

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの検証を行います。
   * @param  param 署名データを検証する為に必要なパラメータです
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  static bool VerifySignature( RSAVerifySignatureCommandParam& param );
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_RSACRYPTOEXECUTOR_H_INCLUDE
#endif // GF_PLATFORM_CTR
