// ============================================================================
/*
 * @file gfl2_WIN32_QREncoder.h
 * @brief QRコードのエンコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#if !defined( GFL2_WIN32_QRENCODER_H_INCLUDE )
#define GFL2_WIN32_QRENCODER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <qr/include/gfl2_IQREncoder.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(win32)


class WIN32_QREncoder : public gfl2::qr::IQREncoder
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_QREncoder );
public:
  WIN32_QREncoder();
  virtual~WIN32_QREncoder();

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を非同期で行います。
   * @param  pQREventListener エンコード処理が完了された時に呼び出されるイベントリスナー
   * @param  pQREncodeData QRコードのエンコード情報です
   */
  // --------------------------------------------------------------------------
  virtual void EncodeAsync( IQREventListener* pQREventListener, const QREncodeData* pQREncodeData );

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を行います。
   * @param  pQREncodeData QRコードのエンコード情報です
   * @return QRコードのイメージ情報を返します
   */
  // --------------------------------------------------------------------------
  virtual QRRGBImageInfo* Encode( const QREncodeData* pQREncodeData );

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update();

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を実行中か調べます。
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

#endif // GFL2_WIN32_QRENCODER_H_INCLUDE
#endif // GF_PLATFORM_WIN32
