// ============================================================================
/*
 * @file QRTestEventListener.h
 * @brief QRCodeのテスト用イベントリスナーです。
 * @date 2015.03.06
 */
// ============================================================================
#if PM_DEBUG

#if !defined( QRCODETESTEVENTLISTENER_H_INCLUDED )
#define QRCODETESTEVENTLISTENER_H_INCLUDED
#pragma once


#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <qr/include/gfl2_IQREventListener.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(QRCodeTest)


class QRTestEventListener : public gfl2::qr::IQREventListener
{
	GFL_FORBID_COPY_AND_ASSIGN( QRTestEventListener );

public:
  QRTestEventListener();
  virtual~QRTestEventListener();

  virtual void OnEncodeSuccess( const gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo, const u8* pTextureBuffer );
  virtual void OnEncodeFailure();
  virtual void OnDecodeSuccess( const gfl2::qr::QRDecodeData* pQRDecodeData );
  virtual void OnDecodeFailure();

};


GFL_NAMESPACE_END(QRCodeTest)
GFL_NAMESPACE_END(Test)

#endif // QRCODETESTEVENTLISTENER_H_INCLUDED

#endif // PM_DEBUG
