// ============================================================================
/*
 * @file gfl2_IAESCrypto.cpp
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#include <crypto/include/gfl2_IAESCrypto.h>
#if defined( GF_PLATFORM_CTR )
#include <crypto/source/ctr/gfl2_CTR_AESCrypto.h>
#elif defined( GF_PLATFORM_WIN32 )
#include <crypto/source/win32/gfl2_WIN32_AESCrypto.h>
#endif

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


IAESCrypto* IAESCrypto::CreateAESCrypto( gfl2::heap::HeapBase* pHeap, u32 temporaryBufferSize, const void* pKey, int threadPriority )
{
  IAESCrypto* pAESCrypto = NULL;

#if defined( GF_PLATFORM_CTR )
  pAESCrypto = GFL_NEW( pHeap ) gfl2::crypto::ctr::CTR_AESCrypto( pHeap, temporaryBufferSize, pKey, threadPriority );
#elif defined( GF_PLATFORM_WIN32 )
  pAESCrypto = GFL_NEW( pHeap ) gfl2::crypto::win32::WIN32_AESCrypto();
#endif

  return pAESCrypto;
}


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
