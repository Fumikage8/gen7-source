// ============================================================================
/*
 * @file gfl2_IAESCryptoEventListener.h
 * @brief AES暗号方式による暗号化、復号化の完了を受け取るイベントリスナーです。
 * @date 2015.03.11
 */
// ============================================================================
#if !defined( GFL2_IAESCRYPTOEVENTLISTENER_H_INCLUDE )
#define GFL2_IAESCRYPTOEVENTLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


class IAESCryptoEventListener
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化処理が成功した時に呼び出されるイベントハンドラです。
   * @param  pData 暗号化したデータです
   * @param  dataSize 暗号化したデータのサイズです
   */
  // --------------------------------------------------------------------------
  virtual void OnEncryptSuccess( const void* pData, u32 dataSize ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化処理が失敗した時に呼び出されるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnEncryptFailure() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  復号化処理が成功した時に呼び出されるイベントハンドラです。
   * @param  pData 暗号化する前のデータです
   * @param  dataSize 暗号化する前のデータのサイズです
   */
  // --------------------------------------------------------------------------
  virtual void OnDecryptSuccess( const void* pData, u32 dataSize ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  復号化処理が失敗した時に呼び出されるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDecryptFailure() = 0;
};


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_IAESCRYPTOEVENTLISTENER_H_INCLUDE
