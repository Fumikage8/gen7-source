// ============================================================================
/*
 * @file gfl2_IQRDecoder.h
 * @brief QRコードのデコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#if !defined( GFL2_IQRDECODER_H_INCLUDE )
#define GFL2_IQRDECODER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <qr/include/gfl2_QRYUVImageInfo.h>
#include <qr/include/gfl2_QRDecodeData.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)

class IQREventListener;

class IQRDecoder
{
public:

  virtual~IQRDecoder(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  QRDecoderを作成します。
   * @param  pHeap ライブラリ内で使用するヒープです。
   * @param  pDeviceHeap ライブラリ内で使用するデバイスヒープです。
   * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
   * @return QRデコーダー
   */
  // --------------------------------------------------------------------------
  static IQRDecoder* CreateQRDecoder( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, int threadPriority );

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を非同期で行います。
   * @param  pQREventListener デコード処理が完了された時に呼び出されるイベントリスナー
   * @param  pQRYUVImageInfo デコード対象の画像情報です
   */
  // --------------------------------------------------------------------------
  virtual void DecodeAsync( IQREventListener* pQREventListener, const QRYUVImageInfo* pQRYUVImageInfo ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を行います。
   * @param  pQRYUVImageInfo デコード対象の画像情報です
   * @return QRコードのデコード結果情報を返します  
   */
  // --------------------------------------------------------------------------
  virtual QRDecodeData* Decode( const QRYUVImageInfo* pQRYUVImageInfo ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  デコード処理を実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy() = 0;
};


GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_IQRDECODER_H_INCLUDE
