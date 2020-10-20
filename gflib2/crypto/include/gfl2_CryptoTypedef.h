// ============================================================================
/*
 * @file gfl2_CryptoTypedef.h
 * @brief Cryptoライブラリの型定義
 * @date 2015.03.11
 */
// ============================================================================
#if !defined( GFL2_CRYPTOTYPEDEF_H_INCLUDE )
#define GFL2_CRYPTOTYPEDEF_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)

typedef gfl2::crypto::TemporaryBuffer Signature;
typedef gfl2::crypto::TemporaryBuffer RSAKey;
typedef gfl2::crypto::TemporaryBuffer AESKey;
typedef gfl2::crypto::TemporaryBuffer SHA256Hash;

typedef struct
{
  Signature*      pSignature;       // 作成した署名データを格納する為のバッファ
  const RSAKey*   pPrivateKey;      // 署名を作成する為の秘密鍵
  const void*     pSHA256Hash;      // 署名データに入る SHA256 のハッシュ
} RSACreateSignatureCommandParam;

typedef struct
{
  const RSAKey*   pPublicKey;       // 署名を検証する為の公開鍵
  const void*     pSHA256Hash;      // 検証する SHA256 ハッシュ
  const void*     pSignature;       // 署名データ
  u32             signatureSize;    // 署名データのサイズ
} RSAVerifySignatureCommandParam;

GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CRYPTOTYPEDEF_H_INCLUDE
