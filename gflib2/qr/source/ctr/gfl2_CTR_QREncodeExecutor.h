// ============================================================================
/*
 * @file gfl2_CTR_QREncodeExecutor.h
 * @brief QRコードのエンコード処理を実行するクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_QRENCODEEXECUTOR_H_INCLUDE )
#define GFL2_CTR_QRENCODEEXECUTOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <qr/include/gfl2_QRRGBImageInfo.h>
#include <qr/include/gfl2_QREncodeData.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_QREncodeExecutor
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_QREncodeExecutor );
public:
  CTR_QREncodeExecutor( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap );
  virtual~CTR_QREncodeExecutor();

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を実行します。
   * @param  pQRRGBImageInfo QRコードのイメージ情報です
   * @param  pQREncodeData QRコードのエンコード情報です
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  bool Execute( gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo, const gfl2::qr::QREncodeData* pQREncodeData );

private:
  gfl2::heap::HeapBase*     m_pHeap;
  gfl2::heap::HeapBase*     m_pDeviceHeap;
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_QRENCODEEXECUTOR_H_INCLUDE
#endif // GF_PLATFORM_CTR
