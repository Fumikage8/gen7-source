// ============================================================================
/*
 * @file gfl2_IQRDecoder.cpp
 * @brief QRコードのデコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#include <qr/include/gfl2_IQRDecoder.h>
#if defined( GF_PLATFORM_CTR )
#include <qr/source/ctr/gfl2_CTR_QRDecoder.h>
#elif defined( GF_PLATFORM_WIN32 )
#include <qr/source/win32/gfl2_WIN32_QRDecoder.h>
#endif

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


IQRDecoder* IQRDecoder::CreateQRDecoder( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, int threadPriority )
{
  IQRDecoder* pQRDecoder = NULL;

#if defined( GF_PLATFORM_CTR )
  pQRDecoder = GFL_NEW( pHeap ) gfl2::qr::ctr::CTR_QRDecoder( pHeap, pDeviceHeap, threadPriority );
#elif defined( GF_PLATFORM_WIN32 )
  pQRDecoder = GFL_NEW( pHeap ) gfl2::qr::win32::WIN32_QRDecoder();
#endif

  return pQRDecoder;
}



GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
