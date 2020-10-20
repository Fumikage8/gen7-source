// ============================================================================
/*
 * @file QRTestEventListener.cpp
 * @brief QRCodeのテスト用イベントリスナーです。
 * @date 2015.03.06
 */
// ============================================================================
#if PM_DEBUG

#include "Test/QRCodeTest/include/QRTestEventListener.h"
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(QRCodeTest)



QRTestEventListener::QRTestEventListener()
{
}


QRTestEventListener::~QRTestEventListener()
{
}


void QRTestEventListener::OnEncodeSuccess( const gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo, const u8* pTextureBuffer )
{
  GFL_PRINT( "QRTestEventListener::OnEncodeSuccess \n" );
}


void QRTestEventListener::OnEncodeFailure()
{
  GFL_PRINT( "QRTestEventListener::OnEncodeFailure \n" );
}


void QRTestEventListener::OnDecodeSuccess( const gfl2::qr::QRDecodeData* pQRDecodeData )
{
  GFL_PRINT( "QRTestEventListener::OnDecodeSuccess \n" );
}


void QRTestEventListener::OnDecodeFailure()
{
  GFL_PRINT( "QRTestEventListener::OnDecodeFailure \n" );
}


GFL_NAMESPACE_END(QRCodeTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
