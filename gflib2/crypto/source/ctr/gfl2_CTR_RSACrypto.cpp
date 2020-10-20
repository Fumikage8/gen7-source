// ============================================================================
/*
 * @file gfl2_CTR_RSACrypto.cpp
 * @brief 署名データの作成と検証を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_RSACrypto.h>
#include <crypto/source/ctr/gfl2_CTR_RSACryptoExecutor.h>
#include <crypto/include/gfl2_IRSACryptoEventListener.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


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
CTR_RSACrypto::CTR_RSACrypto( gfl2::heap::HeapBase* pHeap, const void* pPublicKey, u32 publicKeySize, const void* pPrivateKey, u32 privateKeySize, int threadPriority ) :
  m_pPublicKey( NULL ),
  m_pPrivateKey( NULL ),
  m_pSignature( NULL ),
  m_pThreadController( NULL ),
  m_pRSACryptoThread( NULL ),
  m_pEventListener( NULL )
{
  m_pPublicKey = GFL_NEW( pHeap ) gfl2::crypto::RSAKey( pHeap, pPublicKey, publicKeySize );
  m_pPrivateKey = GFL_NEW( pHeap ) gfl2::crypto::RSAKey( pHeap, pPrivateKey, privateKeySize );
  m_pSignature = GFL_NEW( pHeap ) gfl2::crypto::Signature( pHeap, 1024 );
  m_pThreadController = GFL_NEW( pHeap ) gfl2::crypto::CryptoThreadController( pHeap, threadPriority );
  m_pRSACryptoThread = GFL_NEW( pHeap ) gfl2::crypto::ctr::CTR_RSACryptoThread();
}


// --------------------------------------------------------------------------
/**
  * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを解放します。
  */
// --------------------------------------------------------------------------
CTR_RSACrypto::~CTR_RSACrypto()
{
  GFL_SAFE_DELETE( m_pRSACryptoThread );
  GFL_SAFE_DELETE( m_pThreadController );
  GFL_SAFE_DELETE( m_pSignature );
  GFL_SAFE_DELETE( m_pPrivateKey );
  GFL_SAFE_DELETE( m_pPublicKey );
}


// --------------------------------------------------------------------------
/**
  * @brief  署名データの作成と検証を非同期で行う為の更新処理です。
  */
// --------------------------------------------------------------------------
void CTR_RSACrypto::Update()
{
  if( m_pEventListener != NULL )
  {
    bool bExecute = m_pThreadController->Update();
    if( !bExecute )
    {
      CTR_RSACryptoThread::CommandID eCommandID = m_pRSACryptoThread->GetCommandID();
      bool bSuccess = m_pRSACryptoThread->IsSuccess();

      switch( eCommandID )
      {
        case CTR_RSACryptoThread::COMMAND_ID_CREATE_SIGNATURE:
        {
          if( bSuccess )
          {
            m_pEventListener->OnCreateSignatureSuccess( m_pSignature->GetData(), m_pSignature->GetDataSize() );
          }
          else
          {
            m_pEventListener->OnCreateSignatureFailure();
          }
        }
        break;

        case CTR_RSACryptoThread::COMMAND_ID_VERIFY_SIGNATURE:
        {
          if( bSuccess )
          {
            m_pEventListener->OnVerifySignatureSuccess();
          }
          else
          {
            m_pEventListener->OnVerifySignatureFailure();
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
  * @brief  署名データの作成処理や検証処理が実行中か調べます。
  * @retval true 実行中している
  * @retval false 実行中していない
  */
// --------------------------------------------------------------------------
bool CTR_RSACrypto::IsBusy()
{
  return m_pThreadController->IsBusy();
}


// --------------------------------------------------------------------------
/**
  * @brief  署名データの作成を非同期で行います。
  * @param  pEventListener 署名データの作成が完了された時に呼び出されるイベントリスナー
  * @param  pSHA256Hash 署名データに入る SHA256 のハッシュが格納されているポインタ
  * @retval true 非同期処理の開始に成功した
  * @retval false 非同期処理の開始に失敗した
  */
// --------------------------------------------------------------------------
bool CTR_RSACrypto::CreateSignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash )
{
  if( m_pEventListener == NULL )
  {
    gfl2::crypto::RSACreateSignatureCommandParam param;
  
    param.pSignature  = m_pSignature;
    param.pPrivateKey = m_pPrivateKey;
    param.pSHA256Hash = pSHA256Hash;

    m_pEventListener = pEventListener;
    m_pRSACryptoThread->SetExecuteParameter( param );
    m_pThreadController->Start( m_pRSACryptoThread, false );
    return true;
  }
  return false;
}


// --------------------------------------------------------------------------
/**
  * @brief  署名データの作成を行います。
  * @param  pSHA256Hash 署名データに入る SHA256 のハッシュが格納されているポインタ
  * @return 署名データ
  * @retval NULL 署名データの作成に失敗
  */
// --------------------------------------------------------------------------
gfl2::crypto::Signature* CTR_RSACrypto::CreateSignature( const void* pSHA256Hash )
{
  if( m_pEventListener == NULL )
  {
    gfl2::crypto::RSACreateSignatureCommandParam param;
  
    param.pSignature  = m_pSignature;
    param.pPrivateKey = m_pPrivateKey;
    param.pSHA256Hash = pSHA256Hash;

    bool bSuccess = gfl2::crypto::ctr::CTR_RSACryptoExecutor::CreateSignature( param );

    if( bSuccess )
    {
      return m_pSignature;
    }
  }  
  return NULL;
}


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
bool CTR_RSACrypto::VerifySignatureAsync( IRSACryptoEventListener* pEventListener, const void* pSHA256Hash, const void* pSignature, u32 signatureSize )
{
  if( m_pEventListener == NULL )
  {
    gfl2::crypto::RSAVerifySignatureCommandParam param;
  
    param.pPublicKey    = m_pPublicKey;
    param.pSHA256Hash   = pSHA256Hash;
    param.pSignature    = pSignature;
    param.signatureSize = signatureSize;

    m_pEventListener = pEventListener;
    m_pRSACryptoThread->SetExecuteParameter( param );
    m_pThreadController->Start( m_pRSACryptoThread, false );
    return true;
  }
  return false;
}


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
bool CTR_RSACrypto::VerifySignature( const void* pSHA256Hash, const void* pSignature, u32 signatureSize )
{
  bool bSuccess = false;
  if( m_pEventListener == NULL )
  {
    gfl2::crypto::RSAVerifySignatureCommandParam param;
  
    param.pPublicKey    = m_pPublicKey;
    param.pSHA256Hash   = pSHA256Hash;
    param.pSignature    = pSignature;
    param.signatureSize = signatureSize;
  
    bSuccess = gfl2::crypto::ctr::CTR_RSACryptoExecutor::VerifySignature( param );
  }
  return bSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
