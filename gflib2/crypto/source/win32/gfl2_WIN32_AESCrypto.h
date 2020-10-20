// ============================================================================
/*
 * @file gfl2_WIN32_AESCrypto.h
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#if !defined( GFL2_WIN32_AESCRYPTO_H_INCLUDE )
#define GFL2_WIN32_AESCRYPTO_H_INCLUDE
#pragma once

#include <crypto/include/gfl2_IAESCrypto.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(win32)


class WIN32_AESCrypto : public gfl2::crypto::IAESCrypto
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_AESCrypto );
public:
  WIN32_AESCrypto();
  virtual~WIN32_AESCrypto();
  virtual void Update();
  virtual bool IsBusy();
  virtual bool EncryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize );
  virtual gfl2::crypto::TemporaryBuffer* Encrypt( const void* pData, u32 dataSize );
  virtual bool DecryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize );
  virtual gfl2::crypto::TemporaryBuffer* Decrypt( const void* pData, u32 dataSize );
};


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_WIN32_AESCRYPTO_H_INCLUDE
#endif // GF_PLATFORM_WIN32
