// ============================================================================
/*
 * @file gfl2_CTR_QRDecodeExecutor.h
 * @brief QRコードのデコード処理を実行するクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_QRDECODEEXECUTOR_H_INCLUDE )
#define GFL2_CTR_QRDECODEEXECUTOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <qr/include/gfl2_QRYUVImageInfo.h>
#include <qr/include/gfl2_QRDecodeData.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_QRDecodeExecutor
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_QRDecodeExecutor );
public:
  CTR_QRDecodeExecutor( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap );
  virtual~CTR_QRDecodeExecutor();

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を実行します。
   * @param  pQRDecodeData QRコードのデコード結果です
   * @param  pQRYUVImageInfo デコード対象の画像情報です
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  bool Execute( gfl2::qr::QRDecodeData* pQRDecodeData, const gfl2::qr::QRYUVImageInfo* pQRYUVImageInfo );

private:

  u32* createDecodeBuffer( u32 decodeBufferSize );

  gfl2::heap::HeapBase*     m_pHeap;
  gfl2::heap::HeapBase*     m_pDeviceHeap;
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_QRDECODEEXECUTOR_H_INCLUDE
#endif // GF_PLATFORM_CTR
