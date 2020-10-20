// ============================================================================
/*
 * @file gfl2_CTR_RSACryptoThread.cpp
 * @brief 署名データの作成と検証を行うスレッドです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_RSACryptoThread.h>
#include <crypto/source/ctr/gfl2_CTR_RSACryptoExecutor.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


CTR_RSACryptoThread::CTR_RSACryptoThread() :
  m_eCommandID( COMMAND_ID_CREATE_SIGNATURE ),
  m_bSuccess( false )
{
}


CTR_RSACryptoThread::~CTR_RSACryptoThread()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  スレッドから呼び出される関数です。
  *         この関数内で署名の作成や検証が行われます。
  */
// --------------------------------------------------------------------------
void CTR_RSACryptoThread::Function()
{
  switch( m_eCommandID )
  {
    case COMMAND_ID_CREATE_SIGNATURE:
      m_bSuccess = gfl2::crypto::ctr::CTR_RSACryptoExecutor::CreateSignature( m_CreateSignatureCommandParam );
    break;

    case COMMAND_ID_VERIFY_SIGNATURE:
      m_bSuccess = gfl2::crypto::ctr::CTR_RSACryptoExecutor::VerifySignature( m_VerifySignatureCommandParam );
    break;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  署名データを作成する為に必要なパラメータを設定します。
  * @param  param 署名データを作成する為に必要なパラメータです。
  */
// --------------------------------------------------------------------------
void CTR_RSACryptoThread::SetExecuteParameter( gfl2::crypto::RSACreateSignatureCommandParam& param )
{
  m_eCommandID = COMMAND_ID_CREATE_SIGNATURE;
  m_bSuccess   = false;

  m_CreateSignatureCommandParam.pPrivateKey = param.pPrivateKey;
  m_CreateSignatureCommandParam.pSHA256Hash = param.pSHA256Hash;
  m_CreateSignatureCommandParam.pSignature  = param.pSignature;
}


// --------------------------------------------------------------------------
/**
  * @brief  署名データを検証する為に必要なパラメータを設定します。
  * @param  param 署名データを検証する為に必要なパラメータです。
  */
// --------------------------------------------------------------------------
void CTR_RSACryptoThread::SetExecuteParameter( gfl2::crypto::RSAVerifySignatureCommandParam& param )
{
  m_eCommandID = COMMAND_ID_VERIFY_SIGNATURE;
  m_bSuccess   = false;

  m_VerifySignatureCommandParam.pPublicKey    = param.pPublicKey;
  m_VerifySignatureCommandParam.pSHA256Hash   = param.pSHA256Hash;
  m_VerifySignatureCommandParam.pSignature    = param.pSignature;
  m_VerifySignatureCommandParam.signatureSize = param.signatureSize;
}


// --------------------------------------------------------------------------
/**
  * @brief  実行したコマンドのIDを取得します。
  * @return 実行したコマンドのID
  */
// --------------------------------------------------------------------------
gfl2::crypto::ctr::CTR_RSACryptoThread::CommandID CTR_RSACryptoThread::GetCommandID() const
{
  return m_eCommandID;
}


// --------------------------------------------------------------------------
/**
  * @brief  実行したコマンドの成否を返します。
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_RSACryptoThread::IsSuccess() const
{
  return m_bSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
