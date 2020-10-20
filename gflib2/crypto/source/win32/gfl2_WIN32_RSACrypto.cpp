// ============================================================================
/*
 * @file gfl2_WIN32_RSACrypto.cpp
 * @brief 署名データの作成と検証を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#include <crypto/source/win32/gfl2_WIN32_RSACrypto.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(win32)


WIN32_RSACrypto::WIN32_RSACrypto()
{
}


WIN32_RSACrypto::~WIN32_RSACrypto()
{
}


void WIN32_RSACrypto::Update()
{
}


bool WIN32_RSACrypto::IsBusy()
{
  return false;
}


bool WIN32_RSACrypto::CreateSignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash )
{
  return false;
}


gfl2::crypto::Signature* WIN32_RSACrypto::CreateSignature( const void* pSHA256Hash )
{
  return NULL;
}


bool WIN32_RSACrypto::VerifySignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash, const void* pSignature, u32 signatureSize )
{
  return false;
}


bool WIN32_RSACrypto::VerifySignature( const void* pSHA256Hash, const void* pSignature, u32 signatureSize )
{
  return false;
}


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_WIN32
