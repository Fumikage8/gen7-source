// ============================================================================
/*
 * @file gfl2_CTR_RSACrypto.h
 * @brief 署名データの作成と検証を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_RSACRYPTO_H_INCLUDE )
#define GFL2_CTR_RSACRYPTO_H_INCLUDE
#pragma once

#include <crypto/include/gfl2_IRSACrypto.h>
#include <crypto/source/gfl2_CryptoThreadController.h>
#include <crypto/source/ctr/gfl2_CTR_RSACryptoThread.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_RSACrypto : public gfl2::crypto::IRSACrypto
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_RSACrypto );
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを確保します。
   * @param  pHeap ライブラリ内で使用するヒープです。
   * @param  pPublicKey 公開鍵です
   * @param  publicKeySize 公開鍵のサイズです
   * @param  pPrivateKey 秘密鍵です
   * @param  privateKeySize 秘密鍵のサイズです
   * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
   */
  // --------------------------------------------------------------------------
  CTR_RSACrypto( gfl2::heap::HeapBase* pHeap, const void* pPublicKey, u32 publicKeySize, const void* pPrivateKey, u32 privateKeySize, int threadPriority );

  // --------------------------------------------------------------------------
  /**
   * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを解放します。
   */
  // --------------------------------------------------------------------------
  virtual~CTR_RSACrypto();

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成と検証を非同期で行う為の更新処理です。
   */
  // --------------------------------------------------------------------------
  virtual void Update();

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成処理や検証処理が実行中か調べます。
   * @retval true 実行中している
   * @retval false 実行中していない
   */
  // --------------------------------------------------------------------------
  virtual bool IsBusy();

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成を非同期で行います。
   * @param  pEventListener 署名データの作成が完了された時に呼び出されるイベントリスナー
   * @param  pSHA256Hash 署名データに入る SHA256 のハッシュが格納されているポインタ
   * @retval true 非同期処理の開始に成功した
   * @retval false 非同期処理の開始に失敗した
   */
  // --------------------------------------------------------------------------
  virtual bool CreateSignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash );

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データの作成を行います。
   * @param  pSHA256Hash 署名データに入る SHA256 のハッシュが格納されているポインタ
   * @return 署名データ
   * @retval NULL 署名データの作成に失敗
   */
  // --------------------------------------------------------------------------
  virtual gfl2::crypto::Signature* CreateSignature( const void* pSHA256Hash );

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
  virtual bool VerifySignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash, const void* pSignature, u32 signatureSize );

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
  virtual bool VerifySignature( const void* pSHA256Hash, const void* pSignature, u32 signatureSize );

private:
  gfl2::crypto::RSAKey*                     m_pPublicKey;
  gfl2::crypto::RSAKey*                     m_pPrivateKey;
  gfl2::crypto::Signature*                  m_pSignature;
  gfl2::crypto::CryptoThreadController*     m_pThreadController;
  gfl2::crypto::ctr::CTR_RSACryptoThread*   m_pRSACryptoThread;
  gfl2::crypto::IRSACryptoEventListener*    m_pEventListener;
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_RSACRYPTO_H_INCLUDE
#endif // GF_PLATFORM_CTR
