// ============================================================================
/*
 * @file gfl2_IRSACryptoEventListener.h
 * @brief 署名データの作成と検証の完了を受け取るイベントリスナーです。
 * @date 2015.03.11
 */
// ============================================================================
#if !defined( GFL2_IRSACRYPTOEVENTLISTENER_H_INCLUDE )
#define GFL2_IRSACRYPTOEVENTLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


class IRSACryptoEventListener
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成が成功した時に呼び出されるイベントハンドラです。
   * @param  pSignature 署名データ
   * @param  signatureSize 署名データのサイズ
   */
  // --------------------------------------------------------------------------
  virtual void OnCreateSignatureSuccess( const void* pSignature, u32 signatureSize ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成が失敗した時に呼び出されるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnCreateSignatureFailure() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの検証が成功した時に呼び出されるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnVerifySignatureSuccess() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの検証が失敗した時に呼び出されるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnVerifySignatureFailure() = 0;
};


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_IRSACRYPTOEVENTLISTENER_H_INCLUDE
