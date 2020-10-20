// ============================================================================
/*
 * @file gfl2_ISHA256Calculator.cpp
 * @brief shaによるハッシュ値を計算するクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#include <crypto/include/gfl2_ISHA256Calculator.h>
#if defined( GF_PLATFORM_CTR )
#include <crypto/source/ctr/gfl2_CTR_SHA256Calculator.h>
#elif defined( GF_PLATFORM_WIN32 )
#include <crypto/source/win32/gfl2_WIN32_SHA256Calculator.h>
#endif

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


ISHA256Calculator* ISHA256Calculator::CreateSHA256Calculator( gfl2::heap::HeapBase* pHeap, u32 hashBufferSize, int threadPriority )
{
  ISHA256Calculator* pSHA256Calculator = NULL;

#if defined( GF_PLATFORM_CTR )
  pSHA256Calculator = GFL_NEW( pHeap ) gfl2::crypto::ctr::CTR_SHA256Calculator( pHeap, hashBufferSize, threadPriority );
#elif defined( GF_PLATFORM_WIN32 )
  pSHA256Calculator = GFL_NEW( pHeap ) gfl2::crypto::win32::WIN32_SHA256Calculator();
#endif

  return pSHA256Calculator;
}


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
