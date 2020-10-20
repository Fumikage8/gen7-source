// ============================================================================
/*
 * @file gfl2_IQREventListener.h
 * @brief QRコードのエンコード処理とデコード処理の完了を受け取るイベントリスナーです。
 * @date 2015.03.05
 */
// ============================================================================
#if !defined( GFL2_IQREVENTLISTENER_H_INCLUDE )
#define GFL2_IQREVENTLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <qr/include/gfl2_QRRGBImageInfo.h>
#include <qr/include/gfl2_QRDecodeData.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


class IQREventListener
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理が成功した時に呼び出されるイベントハンドラです。
   * @param  pQRRGBImageInfo QRコードのイメージ情報です
   * @param  pTextureBuffer 3DSで使用出来るテクスチャフォーマットのテクスチャバッファです
   * @note   WIN32版の場合は pTextureBuffer は NULL となります
   */
  // --------------------------------------------------------------------------
  virtual void OnEncodeSuccess( const gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo, const u8* pTextureBuffer ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理が失敗した時に呼び出されるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnEncodeFailure() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理が成功した時に呼び出されるイベントハンドラです。
   * @param  pQRDecodeData QRコードのデコード結果情報です
   */
  // --------------------------------------------------------------------------
  virtual void OnDecodeSuccess( const QRDecodeData* pQRDecodeData ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理が失敗した時に呼び出されるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDecodeFailure() = 0;
};


GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_IQREVENTLISTENER_H_INCLUDE
