// ============================================================================
/*
 * @file gfl2_CTR_QRDecoder.h
 * @brief QRコードのデコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_QRDECODER_H_INCLUDE )
#define GFL2_CTR_QRDECODER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <qr/include/gfl2_IQRDecoder.h>
#include <thread/include/gfl2_Thread.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)

class CTR_QRDecodeThread;

class CTR_QRDecoder : public gfl2::qr::IQRDecoder
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_QRDecoder );
public:
  CTR_QRDecoder( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, int threadPriority );
  virtual~CTR_QRDecoder();

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
  gfl2::heap::HeapBase*                   m_pHeap;
  gfl2::heap::HeapBase*                   m_pDeviceHeap;
  gfl2::qr::QRDecodeData*                 m_pQRDecodeData;
  gfl2::qr::ctr::CTR_QRDecodeThread*      m_pDecodeThread;
  gfl2::thread::Thread*                   m_pThread;
  gfl2::qr::IQREventListener*             m_pQREventListener;
  int                                     m_ThreadPriority;
  static const size_t STACK_SIZE = 4096;       ///< デフォルトスタックサイズ
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_QRDECODER_H_INCLUDE
#endif // GF_PLATFORM_CTR
