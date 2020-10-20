// ============================================================================
/*
 * @file gfl2_CTR_SHA256Calculator.cpp
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_SHA256Calculator.h>
#include <crypto/source/ctr/gfl2_CTR_SHA256CalcExecutor.h>
#include <crypto/include/gfl2_ISHA256CalcEventListener.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


// --------------------------------------------------------------------------
/**
  * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを確保します。
  * @param  hashBufferSize 計算したハッシュを格納しておく為のバッファサイズです
  * @param  threadPriority ライブラリ内で使用するスレッドのプライオリティです。
  */
// --------------------------------------------------------------------------
CTR_SHA256Calculator::CTR_SHA256Calculator( gfl2::heap::HeapBase* pHeap, u32 hashBufferSize, int threadPriority ) :
  m_pSHA256Hash( NULL ),
  m_pThreadController( NULL ),
  m_pSHA256CalcThread( NULL ),
  m_pEventListener( NULL )
{
  m_pSHA256Hash = GFL_NEW( pHeap ) gfl2::crypto::SHA256Hash( pHeap, hashBufferSize );
  m_pThreadController = GFL_NEW( pHeap ) gfl2::crypto::CryptoThreadController( pHeap, threadPriority );
  m_pSHA256CalcThread = GFL_NEW( pHeap ) gfl2::crypto::ctr::CTR_SHA256CalcThread();
}


// --------------------------------------------------------------------------
/**
  * @brief  ライブラリ内で使用するクラスや、テンポラリバッファ等のメモリを解放します。
  */
// --------------------------------------------------------------------------
CTR_SHA256Calculator::~CTR_SHA256Calculator()
{
  GFL_SAFE_DELETE( m_pSHA256CalcThread );
  GFL_SAFE_DELETE( m_pThreadController );
  GFL_SAFE_DELETE( m_pSHA256Hash );
}


// --------------------------------------------------------------------------
/**
  * @brief  ハッシュの計算処理を非同期で行う為の更新処理です。
  */
// --------------------------------------------------------------------------
void CTR_SHA256Calculator::Update()
{
  if( m_pEventListener != NULL )
  {
    bool bExecute = m_pThreadController->Update();
    if( !bExecute )
    {
      bool bSuccess = m_pSHA256CalcThread->IsSuccess();

      if( bSuccess )
      {
        m_pEventListener->OnCalcHashSuccess( m_pSHA256Hash->GetData(), m_pSHA256Hash->GetDataSize() );
      }
      else
      {
        m_pEventListener->OnCalcHashFailure();
      }

      m_pEventListener = NULL;
    }
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  ハッシュの計算処理を実行中か調べます。
  * @retval true 実行中している
  * @retval false 実行中していない
  */
// --------------------------------------------------------------------------
bool CTR_SHA256Calculator::IsBusy()
{
  return m_pThreadController->IsBusy();
}


// --------------------------------------------------------------------------
/**
  * @brief  ハッシュの計算処理を非同期で行います。
  * @param  pEventListener ハッシュの計算処理が完了された時に呼び出されるイベントリスナー
  * @param  pData ハッシュを計算する為のデータ
  * @param  dataSize ハッシュを計算する為のデータのサイズです
  * @retval true 非同期処理の開始に成功した
  * @retval false 非同期処理の開始に失敗した
  */
// --------------------------------------------------------------------------
bool CTR_SHA256Calculator::CalcHashAsync( ISHA256CalcEventListener* pEventListener, const void* pData, u32 dataSize )
{
  if( m_pEventListener == NULL )
  {
    m_pEventListener = pEventListener;
    m_pSHA256CalcThread->SetExecuteParameter( m_pSHA256Hash, pData, dataSize );
    m_pThreadController->Start( m_pSHA256CalcThread, false );
    return true;
  }
  return false;
}


// --------------------------------------------------------------------------
/**
  * @brief  ハッシュの計算処理を行います。
  * @param  pData ハッシュを計算する為のデータ
  * @param  dataSize ハッシュを計算する為のデータのサイズです
  * @return SHA256ハッシュが入っているデータのバッファ
  * @retval NULL ハッシュの計算に失敗
  */
// --------------------------------------------------------------------------
gfl2::crypto::SHA256Hash* CTR_SHA256Calculator::CalcHash( const void* pData, u32 dataSize )
{
  if( m_pEventListener == NULL )
  {
    bool bSuccess = gfl2::crypto::ctr::CTR_SHA256CalcExecutor::CalcHash( m_pSHA256Hash, pData, dataSize );
    if( bSuccess )
    {
      return m_pSHA256Hash;
    }
  }
  return NULL;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
