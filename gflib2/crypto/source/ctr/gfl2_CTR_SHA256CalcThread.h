// ============================================================================
/*
 * @file gfl2_CTR_SHA256CalcThread.h
 * @brief shaによるハッシュ値を計算するスレッドです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_SHA256CALCTHREAD_H_INCLUDE )
#define GFL2_CTR_SHA256CALCTHREAD_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>
#include <crypto/include/gfl2_CryptoTypedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_SHA256CalcThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_SHA256CalcThread );
public:

  CTR_SHA256CalcThread();
  virtual~CTR_SHA256CalcThread();

  // --------------------------------------------------------------------------
  /**
   * @brief  スレッドから呼び出される関数です。
   *         この関数内でハッシュの計算が行われます。
   */
  // --------------------------------------------------------------------------
  virtual void Function();

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算に必要なパラメータを設定します。
   * @param  pSHA256Hash 計算したハッシュを格納する為のバッファです
   * @param  pData ハッシュを計算する為のデータ
   * @param  dataSize ハッシュを計算する為のデータのサイズです
   */
  // --------------------------------------------------------------------------
  void SetExecuteParameter( gfl2::crypto::SHA256Hash* pSHA256Hash, const void* pData, u32 dataSize );

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算の成否を返します。
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  bool IsSuccess() const;

private:
  bool                          m_bSuccess;
  gfl2::crypto::SHA256Hash*     m_pSHA256Hash;
  const void*                   m_pData;
  u32                           m_DataSize;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_SHA256CALCTHREAD_H_INCLUDE
#endif // GF_PLATFORM_CTR
