// ============================================================================
/*
 * @file gfl2_CTR_AESCryptoThread.h
 * @brief AES暗号方式による暗号化、復号化を行うスレッドです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_AESCRYPTOTHREAD_H_INCLUDE )
#define GFL2_CTR_AESCRYPTOTHREAD_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_AESCryptoThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_AESCryptoThread );
public:

  enum CommandID
  {
    COMMAND_ID_ENCRYPT,
    COMMAND_ID_DECRYPT
  };

  CTR_AESCryptoThread();
  virtual~CTR_AESCryptoThread();

  // --------------------------------------------------------------------------
  /**
   * @brief  スレッドから呼び出される関数です。
   *         この関数内で暗号化処理と復号化処理が行われます。
   */
  // --------------------------------------------------------------------------
  virtual void Function();

  // --------------------------------------------------------------------------
  /**
   * @brief  暗号化、復号化に必要なパラメータを設定します。
   * @param  pTemporaryBuffer 暗号化、復号化後のデータを格納する為のバッファです
   * @param  eCommandID 暗号化、復号化を指定する為のコマンドIDです
   * @param  pData 対象データです
   * @param  dataSize 対象データのサイズです
   * @param  pKey 暗号化復号化キー
   */
  // --------------------------------------------------------------------------
  void SetExecuteParameter( gfl2::crypto::TemporaryBuffer* pTemporaryBuffer, CommandID eCommandID, const void* pData, u32 dataSize, const void* pKey );

  // --------------------------------------------------------------------------
  /**
   * @brief  実行したコマンドのIDを取得します。
   * @return 実行したコマンドのID
   */
  // --------------------------------------------------------------------------
  CommandID GetCommandID() const;

  // --------------------------------------------------------------------------
  /**
   * @brief  実行したコマンドの成否を返します。
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  bool IsSuccess() const;

private:
  gfl2::crypto::TemporaryBuffer*            m_pTemporaryBuffer;
  CommandID                                 m_eCommandID;
  const void*                               m_pData;
  u32                                       m_DataSize;
  const void*                               m_pKey;
  bool                                      m_bSuccess;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_AESCRYPTOTHREAD_H_INCLUDE
#endif // GF_PLATFORM_CTR
