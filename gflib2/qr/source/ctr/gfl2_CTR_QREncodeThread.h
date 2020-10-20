// ============================================================================
/*
 * @file gfl2_CTR_QREncodeThread.h
 * @brief QRコードのエンコード処理を非同期で行う為のスレッドクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_QRENCODETHREAD_H_INCLUDE )
#define GFL2_CTR_QRENCODETHREAD_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <qr/include/gfl2_QREncodeData.h>
#include <qr/include/gfl2_QRRGBImageInfo.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_QREncodeThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_QREncodeThread );
public:
  CTR_QREncodeThread( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo );
  virtual~CTR_QREncodeThread();

  // --------------------------------------------------------------------------
  /**
   * @brief  スレッド内から呼ばれる関数です。QRコードのエンコード処理を実行します。
   */
  // --------------------------------------------------------------------------
  virtual void Function();

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理の実行に必要なパラメータの設定をします。
   * @param  pQREncodeData QRコードのエンコード情報です
   */
  // --------------------------------------------------------------------------
  void SetExecuteParameter( const gfl2::qr::QREncodeData* pQREncodeData );

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理の成否を返します。
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  bool IsSuccess() const;

private:
  gfl2::heap::HeapBase*             m_pHeap;
  gfl2::heap::HeapBase*             m_pDeviceHeap;
  gfl2::qr::QRRGBImageInfo*         m_pQRRGBImageInfo;
  const gfl2::qr::QREncodeData*     m_pQREncodeData;
  bool                              m_bIsSuccess;
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_QRENCODETHREAD_H_INCLUDE
#endif // GF_PLATFORM_CTR
