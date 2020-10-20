// ============================================================================
/*
 * @file gfl2_IQREncoder.cpp
 * @brief QRコードのエンコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#include <qr/include/gfl2_IQREncoder.h>
#if defined( GF_PLATFORM_CTR )
#include <qr/source/ctr/gfl2_CTR_QREncoder.h>
#elif defined( GF_PLATFORM_WIN32 )
#include <qr/source/win32/gfl2_WIN32_QREncoder.h>
#endif

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


IQREncoder* IQREncoder::CreateQREncoder( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, int threadPriority )
{
  IQREncoder* pQREncoder = NULL;

#if defined( GF_PLATFORM_CTR )
  pQREncoder = GFL_NEW( pHeap ) gfl2::qr::ctr::CTR_QREncoder( pHeap, pDeviceHeap, threadPriority );
#elif defined( GF_PLATFORM_WIN32 )
  pQREncoder = GFL_NEW( pHeap ) gfl2::qr::win32::WIN32_QREncoder();
#endif

  return pQREncoder;
}



GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
