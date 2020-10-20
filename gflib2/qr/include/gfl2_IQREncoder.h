// ============================================================================
/*
 * @file gfl2_IQREncoder.h
 * @brief QRコードのエンコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#if !defined( GFL2_IQRENCODER_H_INCLUDE )
#define GFL2_IQRENCODER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <qr/include/gfl2_QRRGBImageInfo.h>
#include <qr/include/gfl2_QREncodeData.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)

class IQREventListener;

class IQREncoder
{
public:

  virtual~IQREncoder(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  QREncoderを作成します。
   * @param  pHeap ライブラリ内で使用するヒープです。
   * @param  pDeviceHeap ライブラリ内で使用するデバイスヒープです。
   * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
   * @return QRエンコーダー
   */
  // --------------------------------------------------------------------------
  static IQREncoder* CreateQREncoder( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, int threadPriority );

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を非同期で行います。
   * @param  pQREventListener エンコード処理が完了された時に呼び出されるイベントリスナー
   * @param  pQREncodeData QRコードのエンコード情報です
   */
  // --------------------------------------------------------------------------
  virtual void EncodeAsync( IQREventListener* pQREventListener, const QREncodeData* pQREncodeData ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を行います。
   * @param  pQREncodeData QRコードのエンコード情報です
   * @return QRコードのイメージ情報を返します
   */
  // --------------------------------------------------------------------------
  virtual QRRGBImageInfo* Encode( const QREncodeData* pQREncodeData ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  エンコード処理を実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy() = 0;
};


GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_IQRENCODER_H_INCLUDE
