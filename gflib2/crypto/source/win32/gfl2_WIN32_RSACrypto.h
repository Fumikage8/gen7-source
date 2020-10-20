// ============================================================================
/*
 * @file gfl2_WIN32_RSACrypto.h
 * @brief 署名データの作成と検証を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#if !defined( GFL2_WIN32_RSACRYPTO_H_INCLUDE )
#define GFL2_WIN32_RSACRYPTO_H_INCLUDE
#pragma once

#include <crypto/include/gfl2_IRSACrypto.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(win32)


class WIN32_RSACrypto : public gfl2::crypto::IRSACrypto
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_RSACrypto );
public:
  WIN32_RSACrypto();
  virtual~WIN32_RSACrypto();
  virtual void Update();
  virtual bool IsBusy();
  virtual bool CreateSignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash );
  virtual gfl2::crypto::Signature* CreateSignature( const void* pSHA256Hash );
  virtual bool VerifySignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash, const void* pSignature, u32 signatureSize );
  virtual bool VerifySignature( const void* pSHA256Hash, const void* pSignature, u32 signatureSize );
};


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_WIN32_RSACRYPTO_H_INCLUDE
#endif // GF_PLATFORM_WIN32
