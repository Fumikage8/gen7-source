// ============================================================================
/*
 * @file gfl2_CTR_AESCrypto.cpp
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_AESCrypto.h>
#include <crypto/source/ctr/gfl2_CTR_AESCryptoExecutor.h>
#include <crypto/include/gfl2_IAESCryptoEventListener.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


// --------------------------------------------------------------------------
/**
  * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを確保します。
  * @param  pHeap ライブラリ内で使用するヒープです。
  * @param  temporaryBufferSize 暗号化したデータや復号化したデータを格納しておく為のバッファサイズです。
  * @param  pKey 暗号化する時と復号化する時に使用する鍵です。サイズは16byte固定となります。
  * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
  */
// --------------------------------------------------------------------------
CTR_AESCrypto::CTR_AESCrypto( gfl2::heap::HeapBase* pHeap, u32 temporaryBufferSize, const void* pKey, int threadPriority ) :
  m_pKey( NULL ),
  m_pTemporaryBuffer( NULL ),
  m_pThreadController( NULL ),
  m_pAESCryptoThread( NULL ),
  m_pEventListener( NULL )
{
  // keyのサイズは16byte固定
  m_pKey = GFL_NEW( pHeap ) gfl2::crypto::AESKey( pHeap, pKey, 16 );
  m_pTemporaryBuffer = GFL_NEW( pHeap ) gfl2::crypto::TemporaryBuffer( pHeap, temporaryBufferSize );
  m_pThreadController = GFL_NEW( pHeap ) gfl2::crypto::CryptoThreadController( pHeap, threadPriority );
  m_pAESCryptoThread = GFL_NEW( pHeap ) gfl2::crypto::ctr::CTR_AESCryptoThread();
}


// --------------------------------------------------------------------------
/**
  * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを解放します。
  */
// --------------------------------------------------------------------------
CTR_AESCrypto::~CTR_AESCrypto()
{
  GFL_SAFE_DELETE( m_pAESCryptoThread );
  GFL_SAFE_DELETE( m_pThreadController );
  GFL_SAFE_DELETE( m_pTemporaryBuffer );
  GFL_SAFE_DELETE( m_pKey );
}


// --------------------------------------------------------------------------
/**
  * @brief  暗号化復号化処理を非同期で行う為の更新処理です。
  */
// --------------------------------------------------------------------------
void CTR_AESCrypto::Update()
{
  if( m_pEventListener != NULL )
  {
    bool bExecute = m_pThreadController->Update();
    if( !bExecute )
    {
      CTR_AESCryptoThread::CommandID eCommandID = m_pAESCryptoThread->GetCommandID();
      bool bSuccess = m_pAESCryptoThread->IsSuccess();

      switch( eCommandID )
      {
        case CTR_AESCryptoThread::COMMAND_ID_ENCRYPT:
        {
          if( bSuccess )
          {
            m_pEventListener->OnEncryptSuccess( m_pTemporaryBuffer->GetData(), m_pTemporaryBuffer->GetDataSize() );
          }
          else
          {
            m_pEventListener->OnEncryptFailure();
          }
        }
        break;

        case CTR_AESCryptoThread::COMMAND_ID_DECRYPT:
        {
          if( bSuccess )
          {
            m_pEventListener->OnDecryptSuccess( m_pTemporaryBuffer->GetData(), m_pTemporaryBuffer->GetDataSize() );
          }
          else
          {
            m_pEventListener->OnDecryptFailure();
          }
        }
        break;
      }

      m_pEventListener = NULL;
    }
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  暗号化復号化処理を実行中か調べます。
  * @retval true 実行中している
  * @retval false 実行中していない
  */
// --------------------------------------------------------------------------
bool CTR_AESCrypto::IsBusy()
{
  return m_pThreadController->IsBusy();
}


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
bool CTR_AESCrypto::EncryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize )
{
  if( m_pEventListener == NULL )
  {
    m_pEventListener = pEventListener;
    m_pAESCryptoThread->SetExecuteParameter( m_pTemporaryBuffer, CTR_AESCryptoThread::COMMAND_ID_ENCRYPT, pData, dataSize, m_pKey->GetData() );
    m_pThreadController->Start( m_pAESCryptoThread, false );
    return true;
  }
  return false;
}


// --------------------------------------------------------------------------
/**
  * @brief  暗号化処理を行います。
  * @param  pData 暗号化したいデータです
  * @param  dataSize 暗号化したいデータのサイズです
  * @return 暗号化後のデータを返します
  * @retval NULL 暗号化に失敗
  */
// --------------------------------------------------------------------------
gfl2::crypto::TemporaryBuffer* CTR_AESCrypto::Encrypt( const void* pData, u32 dataSize )
{
  if( m_pEventListener == NULL )
  {
    bool bSuccess = gfl2::crypto::ctr::CTR_AESCryptoExecutor::Encrypt( m_pTemporaryBuffer, pData, dataSize, m_pKey->GetData() );
    if( bSuccess )
    {
      return m_pTemporaryBuffer;
    }
  }
  return NULL;
}


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
bool CTR_AESCrypto::DecryptAsync( IAESCryptoEventListener* pEventListener, const void* pData, u32 dataSize )
{
  if( m_pEventListener == NULL )
  {
    m_pEventListener = pEventListener;
    m_pAESCryptoThread->SetExecuteParameter( m_pTemporaryBuffer, CTR_AESCryptoThread::COMMAND_ID_DECRYPT, pData, dataSize, m_pKey->GetData() );
    m_pThreadController->Start( m_pAESCryptoThread, false );
    return true;
  }
  return false;
}


// --------------------------------------------------------------------------
/**
  * @brief  復号化処理を行います。
  * @param  pData 復号化したいデータです(暗号化されたデータです)
  * @param  dataSize 復号化したいデータのサイズです(暗号化されたデータのサイズです)
  * @return 暗号化する前のデータを返します(元のデータを返します)
  * @retval NULL 復号化に失敗
  */
// --------------------------------------------------------------------------
gfl2::crypto::TemporaryBuffer* CTR_AESCrypto::Decrypt( const void* pData, u32 dataSize )
{
  if( m_pEventListener == NULL )
  {
    bool bSuccess = gfl2::crypto::ctr::CTR_AESCryptoExecutor::Decrypt( m_pTemporaryBuffer, pData, dataSize, m_pKey->GetData() );
    if( bSuccess )
    {
      return m_pTemporaryBuffer;
    }
  }
  return NULL;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
