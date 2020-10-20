// ============================================================================
/*
 * @file gfl2_CTR_QRDecodeThread.h
 * @brief QRコードのデコード処理を非同期で行う為のスレッドクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_QRDECODETHREAD_H_INCLUDE )
#define GFL2_CTR_QRDECODETHREAD_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <qr/include/gfl2_QRDecodeData.h>
#include <qr/include/gfl2_QRYUVImageInfo.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_QRDecodeThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_QRDecodeThread );
public:
  CTR_QRDecodeThread( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::qr::QRDecodeData* pQRDecodeData );
  virtual~CTR_QRDecodeThread();

  // --------------------------------------------------------------------------
  /**
   * @brief  スレッド内から呼ばれる関数です。QRコードのデコード処理を実行します。
   */
  // --------------------------------------------------------------------------
  virtual void Function();

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理の実行に必要なパラメータの設定をします。
   * @param  pQRYUVImageInfo デコード対象の画像情報です
   */
  // --------------------------------------------------------------------------
  void SetExecuteParameter( const gfl2::qr::QRYUVImageInfo* pQRYUVImageInfo );

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理の成否を返します。
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  bool IsSuccess() const;

private:
  gfl2::heap::HeapBase*             m_pHeap;
  gfl2::heap::HeapBase*             m_pDeviceHeap;
  gfl2::qr::QRDecodeData*           m_pQRDecodeData;
  const gfl2::qr::QRYUVImageInfo*   m_pQRYUVImageInfo;
  bool                              m_bIsSuccess;
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_QRDECODETHREAD_H_INCLUDE
#endif // GF_PLATFORM_CTR
