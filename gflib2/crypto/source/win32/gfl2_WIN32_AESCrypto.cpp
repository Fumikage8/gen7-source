// ============================================================================
/*
 * @file gfl2_WIN32_AESCrypto.cpp
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#include <crypto/source/win32/gfl2_WIN32_AESCrypto.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(win32)


WIN32_AESCrypto::WIN32_AESCrypto()
{
}


WIN32_AESCrypto::~WIN32_AESCrypto()
{
}


void WIN32_AESCrypto::Update()
{
}


bool WIN32_AESCrypto::IsBusy()
{
  return false;
}


bool WIN32_AESCrypto::EncryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize )
{
  return false;
}


gfl2::crypto::TemporaryBuffer* WIN32_AESCrypto::Encrypt( const void* pData, u32 dataSize )
{
  return NULL;
}


bool WIN32_AESCrypto::DecryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize )
{
  return false;
}


gfl2::crypto::TemporaryBuffer* WIN32_AESCrypto::Decrypt( const void* pData, u32 dataSize )
{
  return NULL;
}


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_WIN32
