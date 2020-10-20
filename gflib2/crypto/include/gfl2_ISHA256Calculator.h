// ============================================================================
/*
 * @file gfl2_ISHA256Calculator.h
 * @brief shaによるハッシュ値を計算するクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#if !defined( GFL2_ISHA256CALCULATOR_H_INCLUDE )
#define GFL2_ISHA256CALCULATOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <crypto/include/gfl2_CryptoTypedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


class ISHA256CalcEventListener;


class ISHA256Calculator
{
public:

  virtual~ISHA256Calculator(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  SHA256ハッシュを計算するクラスを作成します。
   * @param  pHeap ライブラリ内で使用するヒープです。
   * @param  hashBufferSize 計算したハッシュを格納しておく為のバッファサイズです
   * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
   * @return SHA256ハッシュを計算するクラス
   */
  // --------------------------------------------------------------------------
  static ISHA256Calculator* CreateSHA256Calculator( gfl2::heap::HeapBase* pHeap, u32 hashBufferSize, int threadPriority );

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理を実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理を非同期で行います。
   * @param  pEventListener ハッシュの計算処理が完了された時に呼び出されるイベントリスナー
   * @param  pData ハッシュを計算する為のデータ
   * @param  dataSize ハッシュを計算する為のデータのサイズです
   * @retval true 非同期処理の開始に成功した
   * @retval false 非同期処理の開始に失敗した
   */
  // --------------------------------------------------------------------------
  virtual bool CalcHashAsync( ISHA256CalcEventListener* pEventListener, const void* pData, u32 dataSize ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理を行います。
   * @param  pData ハッシュを計算する為のデータ
   * @param  dataSize ハッシュを計算する為のデータのサイズです
   * @return SHA256ハッシュが入っているデータのバッファ
   * @retval NULL ハッシュの計算に失敗
   */
  // --------------------------------------------------------------------------
  virtual gfl2::crypto::SHA256Hash* CalcHash( const void* pData, u32 dataSize ) = 0;
};


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_ISHA256CALCULATOR_H_INCLUDE
