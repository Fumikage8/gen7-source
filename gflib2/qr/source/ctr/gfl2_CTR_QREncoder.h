// ============================================================================
/*
 * @file gfl2_CTR_QREncoder.h
 * @brief QRコードのエンコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_QRENCODER_H_INCLUDE )
#define GFL2_CTR_QRENCODER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <qr/include/gfl2_IQREncoder.h>
#include <thread/include/gfl2_Thread.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)

class CTR_QREncodeThread;

class CTR_QREncoder : public gfl2::qr::IQREncoder
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_QREncoder );
public:
  CTR_QREncoder( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, int threadPriority );
  virtual~CTR_QREncoder();

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
  u8* createNativeTexture( gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo );

private:
  gfl2::heap::HeapBase*                   m_pHeap;
  gfl2::heap::HeapBase*                   m_pDeviceHeap;
  gfl2::qr::QRRGBImageInfo*               m_pQRRGBImageInfo;
  gfl2::qr::ctr::CTR_QREncodeThread*      m_pEncodeThread;
  gfl2::thread::Thread*                   m_pThread;
  gfl2::qr::IQREventListener*             m_pQREventListener;
  u8*                                     m_pTextureBuffer;
  int                                     m_ThreadPriority;
  static const size_t STACK_SIZE = 4096;       ///< デフォルトスタックサイズ
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_QRENCODER_H_INCLUDE
#endif // GF_PLATFORM_CTR
