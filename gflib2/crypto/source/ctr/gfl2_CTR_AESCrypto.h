// ============================================================================
/*
 * @file gfl2_CTR_AESCrypto.h
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_AESCRYPTO_H_INCLUDE )
#define GFL2_CTR_AESCRYPTO_H_INCLUDE
#pragma once

#include <crypto/include/gfl2_IAESCrypto.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>
#include <crypto/source/gfl2_CryptoThreadController.h>
#include <crypto/source/ctr/gfl2_CTR_AESCryptoThread.h>
#include <crypto/include/gfl2_CryptoTypedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_AESCrypto : public gfl2::crypto::IAESCrypto
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_AESCrypto );
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを確保します。
   * @param  pHeap ライブラリ内で使用するヒープです。
   * @param  temporaryBufferSize 暗号化したデータや復号化したデータを格納しておく為のバッファサイズです。
   * @param  pKey 暗号化する時と復号化する時に使用する鍵です。サイズは16byte固定となります。
   * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
   */
  // --------------------------------------------------------------------------
  CTR_AESCrypto( gfl2::heap::HeapBase* pHeap, u32 temporaryBufferSize, const void* pKey, int threadPriority );

  // --------------------------------------------------------------------------
  /**
   * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを解放します。
   */
  // --------------------------------------------------------------------------
  virtual~CTR_AESCrypto();

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化復号化処理を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update();

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化復号化処理を実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy();

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化処理を非同期で行います。
   * @param  pEventListener 暗号化処理が完了された時に呼び出されるイベントリスナー
   * @param  pData 暗号化したいデータです
   * @param  dataSize 暗号化したいデータのサイズです
   * @retval true 非同期処理の開始に成功した
   * @retval false 非同期処理の開始に失敗した
   */
  // --------------------------------------------------------------------------
  virtual bool EncryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize );

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化処理を行います。
   * @param  pData 暗号化したいデータです
   * @param  dataSize 暗号化したいデータのサイズです
   * @return 暗号化後のデータを返します
   * @retval NULL 暗号化に失敗
   */
  // --------------------------------------------------------------------------
  virtual gfl2::crypto::TemporaryBuffer* Encrypt( const void* pData, u32 dataSize );

  // --------------------------------------------------------------------------
  /**
   * @brief  復号化処理を非同期で行います。
   * @param  pEventListener 復号化処理が完了された時に呼び出されるイベントリスナー
   * @param  pData 復号化したいデータです(暗号化されたデータです)
   * @param  dataSize 復号化したいデータのサイズです(暗号化されたデータのサイズです)
   * @retval true 非同期処理の開始に成功した
   * @retval false 非同期処理の開始に失敗した
   */
  // --------------------------------------------------------------------------
  virtual bool DecryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize );

  // --------------------------------------------------------------------------
  /**
   * @brief  復号化処理を行います。
   * @param  pData 復号化したいデータです(暗号化されたデータです)
   * @param  dataSize 復号化したいデータのサイズです(暗号化されたデータのサイズです)
   * @return 暗号化する前のデータを返します(元のデータを返します)
   * @retval NULL 復号化に失敗
   */
  // --------------------------------------------------------------------------
  virtual gfl2::crypto::TemporaryBuffer* Decrypt( const void* pData, u32 dataSize );

private:
  gfl2::crypto::AESKey*                     m_pKey;
  gfl2::crypto::TemporaryBuffer*            m_pTemporaryBuffer;
  gfl2::crypto::CryptoThreadController*     m_pThreadController;
  gfl2::crypto::ctr::CTR_AESCryptoThread*   m_pAESCryptoThread;
  gfl2::crypto::IAESCryptoEventListener*    m_pEventListener;
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_AESCRYPTO_H_INCLUDE
#endif // GF_PLATFORM_CTR
