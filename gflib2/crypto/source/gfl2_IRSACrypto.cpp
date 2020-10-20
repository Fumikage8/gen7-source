// ============================================================================
/*
 * @file gfl2_IRSACrypto.cpp
 * @brief 署名データの作成と検証を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#include <crypto/include/gfl2_IRSACrypto.h>
#if defined( GF_PLATFORM_CTR )
#include <crypto/source/ctr/gfl2_CTR_RSACrypto.h>
#elif defined( GF_PLATFORM_WIN32 )
#include <crypto/source/win32/gfl2_WIN32_RSACrypto.h>
#endif


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


IRSACrypto* IRSACrypto::CreateRSACrypto( gfl2::heap::HeapBase* pHeap, const void* pPublicKey, u32 publicKeySize, const void* pPrivateKey, u32 privateKeySize, int threadPriority )
{
  IRSACrypto* pRSACrypto = NULL;

#if defined( GF_PLATFORM_CTR )
  pRSACrypto = GFL_NEW( pHeap ) gfl2::crypto::ctr::CTR_RSACrypto( pHeap, pPublicKey, publicKeySize, pPrivateKey, privateKeySize, threadPriority );
#elif defined( GF_PLATFORM_WIN32 )
  pRSACrypto = GFL_NEW( pHeap ) gfl2::crypto::win32::WIN32_RSACrypto();
#endif

  return pRSACrypto;
}


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
