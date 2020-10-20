// ============================================================================
/*
 * @file gfl2_CTR_RSACryptoThread.h
 * @brief 署名データの作成と検証を行うスレッドです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#if !defined( GFL2_CTR_RSACRYPTOTHREAD_H_INCLUDE )
#define GFL2_CTR_RSACRYPTOTHREAD_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <crypto/include/gfl2_TemporaryBuffer.h>
#include <crypto/include/gfl2_CryptoTypedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_RSACryptoThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_RSACryptoThread );
public:

  enum CommandID
  {
    COMMAND_ID_CREATE_SIGNATURE,
    COMMAND_ID_VERIFY_SIGNATURE
  };

  CTR_RSACryptoThread();
  virtual~CTR_RSACryptoThread();

  // --------------------------------------------------------------------------
  /**
   * @brief  スレッドから呼び出される関数です。
   *         この関数内で署名の作成や検証が行われます。
   */
  // --------------------------------------------------------------------------
  virtual void Function();

  // --------------------------------------------------------------------------
  /**
   * @brief  署名データを作成する為に必要なパラメータを設定します。
   * @param  param 署名データを作成する為に必要なパラメータです。
   */
  // --------------------------------------------------------------------------
  void SetExecuteParameter( gfl2::crypto::RSACreateSignatureCommandParam& param );


  // --------------------------------------------------------------------------
  /**
   * @brief  署名データを検証する為に必要なパラメータを設定します。
   * @param  param 署名データを検証する為に必要なパラメータです。
   */
  // --------------------------------------------------------------------------
  void SetExecuteParameter( gfl2::crypto::RSAVerifySignatureCommandParam& param );

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
  CommandID                                       m_eCommandID;
  bool                                            m_bSuccess;
  gfl2::crypto::RSACreateSignatureCommandParam    m_CreateSignatureCommandParam;
  gfl2::crypto::RSAVerifySignatureCommandParam    m_VerifySignatureCommandParam;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CTR_RSACRYPTOTHREAD_H_INCLUDE
#endif // GF_PLATFORM_CTR
