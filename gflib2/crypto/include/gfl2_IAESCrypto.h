// ============================================================================
/*
 * @file gfl2_IAESCrypto.h
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#if !defined( GFL2_IAESCRYPTO_H_INCLUDE )
#define GFL2_IAESCRYPTO_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


class IAESCryptoEventListener;


class IAESCrypto
{
public:

  virtual~IAESCrypto(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  AES暗号化復号化クラスを作成します。
   * @param  pHeap ライブラリ内で使用するヒープです。
   * @param  temporaryBufferSize 暗号化したデータや復号化したデータを格納しておく為のバッファサイズです。
   * @param  pKey 暗号化する時と復号化する時に使用する鍵です。サイズは16byte固定となります。
   * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
   * @return AES暗号化復号化クラス
   */
  // --------------------------------------------------------------------------
  static IAESCrypto* CreateAESCrypto( gfl2::heap::HeapBase* pHeap, u32 temporaryBufferSize, const void* pKey, int threadPriority );

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化復号化処理を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化復号化処理を実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy() = 0;

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
  virtual bool EncryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化処理を行います。
   * @param  pData 暗号化したいデータです
   * @param  dataSize 暗号化したいデータのサイズです
   * @return 暗号化後のデータを返します
   * @retval NULL 暗号化に失敗
   */
  // --------------------------------------------------------------------------
  virtual gfl2::crypto::TemporaryBuffer* Encrypt( const void* pData, u32 dataSize ) = 0;

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
  virtual bool DecryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  復号化処理を行います。
   * @param  pData 復号化したいデータです(暗号化されたデータです)
   * @param  dataSize 復号化したいデータのサイズです(暗号化されたデータのサイズです)
   * @return 暗号化する前のデータを返します(元のデータを返します)
   * @retval NULL 復号化に失敗
   */
  // --------------------------------------------------------------------------
  virtual gfl2::crypto::TemporaryBuffer* Decrypt( const void* pData, u32 dataSize ) = 0;
};


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_IAESCRYPTO_H_INCLUDE
