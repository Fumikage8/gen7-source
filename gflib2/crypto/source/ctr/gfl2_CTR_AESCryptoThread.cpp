// ============================================================================
/*
 * @file gfl2_CTR_AESCryptoThread.cpp
 * @brief AES暗号方式による暗号化、復号化を行うスレッドです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_AESCryptoThread.h>
#include <crypto/source/ctr/gfl2_CTR_AESCryptoExecutor.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


CTR_AESCryptoThread::CTR_AESCryptoThread() :
  m_pTemporaryBuffer( NULL ),
  m_eCommandID( COMMAND_ID_ENCRYPT ),
  m_pData( NULL ),
  m_DataSize( 0 ),
  m_pKey( NULL ),
  m_bSuccess( false )
{
}


CTR_AESCryptoThread::~CTR_AESCryptoThread()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  スレッドから呼び出される関数です。
  *         この関数内で暗号化処理と復号化処理が行われます。
  */
// --------------------------------------------------------------------------
void CTR_AESCryptoThread::Function()
{
  switch( m_eCommandID )
  {
    case COMMAND_ID_ENCRYPT:
      m_bSuccess = gfl2::crypto::ctr::CTR_AESCryptoExecutor::Encrypt( m_pTemporaryBuffer, m_pData, m_DataSize, m_pKey );
    break;

    case COMMAND_ID_DECRYPT:
      m_bSuccess = gfl2::crypto::ctr::CTR_AESCryptoExecutor::Decrypt( m_pTemporaryBuffer, m_pData, m_DataSize, m_pKey );
    break;
  }
}


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
void CTR_AESCryptoThread::SetExecuteParameter( gfl2::crypto::TemporaryBuffer* pTemporaryBuffer, CommandID eCommandID, const void* pData, u32 dataSize, const void* pKey )
{
  m_pTemporaryBuffer = pTemporaryBuffer;
  m_eCommandID       = eCommandID;
  m_pData            = pData;
  m_DataSize         = dataSize;
  m_pKey             = pKey;
  m_bSuccess         = false;
}


// --------------------------------------------------------------------------
/**
  * @brief  実行したコマンドのIDを取得します。
  * @return 実行したコマンドのID
  */
// --------------------------------------------------------------------------
gfl2::crypto::ctr::CTR_AESCryptoThread::CommandID CTR_AESCryptoThread::GetCommandID() const
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
bool CTR_AESCryptoThread::IsSuccess() const
{
  return m_bSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
