// ============================================================================
/*
 * @file gfl2_IRSACrypto.h
 * @brief 署名データの作成と検証を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#if !defined( GFL2_IRSACRYPTO_H_INCLUDE )
#define GFL2_IRSACRYPTO_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>
#include <crypto/include/gfl2_CryptoTypedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


class IRSACryptoEventListener;


class IRSACrypto
{
public:

  virtual~IRSACrypto(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成と検証を行うクラスを作成します。
   * @param  pHeap ライブラリ内で使用するヒープです。
   * @param  pPublicKey 公開鍵です
   * @param  publicKeySize 公開鍵のサイズです
   * @param  pPrivateKey 秘密鍵です
   * @param  privateKeySize 秘密鍵のサイズです
   * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
   * @return 署名データの作成と検証を行うクラス
   */
  // --------------------------------------------------------------------------
  static IRSACrypto* CreateRSACrypto( gfl2::heap::HeapBase* pHeap, const void* pPublicKey, u32 publicKeySize, const void* pPrivateKey, u32 privateKeySize, int threadPriority );

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成と検証を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成処理や検証処理が実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成を非同期で行います。
   * @param  pEventListener 署名データの作成が完了された時に呼び出されるイベントリスナー
   * @param  pSHA256Hash 署名データに入る SHA256 のハッシュが格納されているポインタ
   * @retval true 非同期処理の開始に成功した
   * @retval false 非同期処理の開始に失敗した
   */
  // --------------------------------------------------------------------------
  virtual bool CreateSignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成を行います。
   * @param  pSHA256Hash 署名データに入る SHA256 のハッシュが格納されているポインタ
   * @return 署名データ
   * @retval NULL 署名データの作成に失敗
   */
  // --------------------------------------------------------------------------
  virtual gfl2::crypto::Signature* CreateSignature( const void* pSHA256Hash ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの検証を非同期で行います。
   * @param  pEventListener 署名データの検証が完了された時に呼び出されるイベントリスナー
   * @param  pSHA256Hash 検証する SHA256 ハッシュが格納されているポインタ
   * @param  pSignature 署名データ
   * @param  signatureSize 署名データのサイズ
   * @retval true 非同期処理の開始に成功した
   * @retval false 非同期処理の開始に失敗した
   */
  // --------------------------------------------------------------------------
  virtual bool VerifySignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash, const void* pSignature, u32 signatureSize ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの検証を行います。
   * @param  pSHA256Hash 検証する SHA256 ハッシュが格納されているポインタ
   * @param  pSignature 署名データ
   * @param  signatureSize 署名データのサイズ
   * @retval true 検証に成功した(検証した結果問題ない)
   * @retval false 検証に失敗した(検証した結果問題あり)
   */
  // --------------------------------------------------------------------------
  virtual bool VerifySignature( const void* pSHA256Hash, const void* pSignature, u32 signatureSize ) = 0;
};


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_IRSACRYPTO_H_INCLUDE
