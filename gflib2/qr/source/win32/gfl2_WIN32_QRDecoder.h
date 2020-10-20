// ============================================================================
/*
 * @file gfl2_WIN32_QRDecoder.h
 * @brief QRコードのデコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#if !defined( GFL2_WIN32_QRDECODER_H_INCLUDE )
#define GFL2_WIN32_QRDECODER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <qr/include/gfl2_IQRDecoder.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(win32)


class WIN32_QRDecoder : public gfl2::qr::IQRDecoder
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_QRDecoder );
public:
  WIN32_QRDecoder();
  virtual~WIN32_QRDecoder();

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を非同期で行います。
   * @param  pQREventListener デコード処理が完了された時に呼び出されるイベントリスナー
   * @param  pQRYUVImageInfo デコード対象の画像情報です
   */
  // --------------------------------------------------------------------------
  virtual void DecodeAsync( IQREventListener* pQREventListener, const QRYUVImageInfo* pQRYUVImageInfo );

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を行います。
   * @param  pQRYUVImageInfo デコード対象の画像情報です
   * @return QRコードのデコード結果情報を返します  
   */
  // --------------------------------------------------------------------------
  virtual QRDecodeData* Decode( const QRYUVImageInfo* pQRYUVImageInfo );

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update();

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy();

private:
  gfl2::qr::IQREventListener*             m_pQREventListener;

};


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_WIN32_QRDECODER_H_INCLUDE
#endif // GF_PLATFORM_WIN32
