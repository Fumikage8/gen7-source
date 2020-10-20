// ============================================================================
/*
 * @file gfl2_CTR_SHA256Calculator.h
 * @brief shaによるハッシュ値を計算するクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_SHA256CALCULATOR_H_INCLUDE )
#define GFL2_CTR_SHA256CALCULATOR_H_INCLUDE
#pragma once

#include <crypto/include/gfl2_ISHA256Calculator.h>
#include <crypto/source/gfl2_CryptoThreadController.h>
#include <crypto/source/ctr/gfl2_CTR_SHA256CalcThread.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_SHA256Calculator : public gfl2::crypto::ISHA256Calculator
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_SHA256Calculator );
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを確保します。
   * @param  hashBufferSize 計算したハッシュを格納しておく為のバッファサイズです
   * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
   */
  // --------------------------------------------------------------------------
  CTR_SHA256Calculator( gfl2::heap::HeapBase* pHeap, u32 hashBufferSize, int threadPriority );

  // --------------------------------------------------------------------------
  /**
   * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを解放します。
   */
  // --------------------------------------------------------------------------
  virtual~CTR_SHA256Calculator();

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update();

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理を実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy();

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
  virtual bool CalcHashAsync( ISHA256CalcEventListener* pEventListener, const void* pData, u32 dataSize );

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理を行います。
   * @param  pData ハッシュを計算する為のデータ
   * @param  dataSize ハッシュを計算する為のデータのサイズです
   * @return SHA256ハッシュが入っているデータのバッファ
   * @retval NULL ハッシュの計算に失敗
   */
  // --------------------------------------------------------------------------
  virtual gfl2::crypto::SHA256Hash* CalcHash( const void* pData, u32 dataSize );

private:
  gfl2::crypto::SHA256Hash*                  m_pSHA256Hash;
  gfl2::crypto::CryptoThreadController*      m_pThreadController;
  gfl2::crypto::ctr::CTR_SHA256CalcThread*   m_pSHA256CalcThread;
  gfl2::crypto::ISHA256CalcEventListener*    m_pEventListener;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_SHA256CALCULATOR_H_INCLUDE
#endif // GF_PLATFORM_CTR
