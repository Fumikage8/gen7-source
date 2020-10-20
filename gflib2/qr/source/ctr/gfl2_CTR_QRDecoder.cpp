// ============================================================================
/*
 * @file gfl2_CTR_QRDecoder.cpp
 * @brief QRコードのデコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <qr/source/ctr/gfl2_CTR_QRDecoder.h>
#include <qr/source/ctr/gfl2_CTR_QRDecodeExecutor.h>
#include <qr/source/ctr/gfl2_CTR_QRDecodeThread.h>
#include <qr/include/gfl2_IQREventListener.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


CTR_QRDecoder::CTR_QRDecoder( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, int threadPriority ) :
  m_pHeap( pHeap ),
  m_pDeviceHeap( pDeviceHeap ),
  m_pQRDecodeData( NULL ),
  m_pDecodeThread( NULL ),
  m_pThread( NULL ),
  m_pQREventListener( NULL ),
  m_ThreadPriority( threadPriority )
{
  m_pQRDecodeData = GFL_NEW( m_pHeap ) gfl2::qr::QRDecodeData( m_pHeap );
  m_pDecodeThread = GFL_NEW( m_pHeap ) gfl2::qr::ctr::CTR_QRDecodeThread( m_pHeap, m_pDeviceHeap, m_pQRDecodeData );
}


CTR_QRDecoder::~CTR_QRDecoder()
{
  GFL_SAFE_DELETE( m_pDecodeThread );
  GFL_SAFE_DELETE( m_pQRDecodeData );
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を非同期で行います。
  * @param  pQREventListener デコード処理が完了された時に呼び出されるイベントリスナー
  * @param  pQRYUVImageInfo デコード対象の画像情報です
  */
// --------------------------------------------------------------------------
void CTR_QRDecoder::DecodeAsync( IQREventListener* pQREventListener, const QRYUVImageInfo* pQRYUVImageInfo )
{
  if( m_pThread == NULL )
  {
    m_pQREventListener = pQREventListener;

    m_pDecodeThread->SetExecuteParameter( pQRYUVImageInfo );

    m_pThread = GFL_NEW( m_pHeap ) gfl2::thread::Thread( m_pDecodeThread, m_pHeap, false, STACK_SIZE );
        
    m_pThread->Start( m_ThreadPriority );
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を行います。
  * @param  pQRYUVImageInfo デコード対象の画像情報です
  * @return QRコードのデコード結果情報を返します  
  */
// --------------------------------------------------------------------------
QRDecodeData* CTR_QRDecoder::Decode( const QRYUVImageInfo* pQRYUVImageInfo )
{
  gfl2::qr::ctr::CTR_QRDecodeExecutor* pExecutor = GFL_NEW( m_pHeap ) gfl2::qr::ctr::CTR_QRDecodeExecutor( m_pHeap, m_pDeviceHeap );

  bool bIsSuccess = pExecutor->Execute( m_pQRDecodeData, pQRYUVImageInfo );

  GFL_SAFE_DELETE( pExecutor );

  if( bIsSuccess )
  {
    return m_pQRDecodeData;
  }
  return NULL;
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を非同期で行う為の更新処理です。
  */
// --------------------------------------------------------------------------
void CTR_QRDecoder::Update()
{
  if( m_pThread != NULL )
  {
    bool bIsAlive = m_pThread->IsAlive();
        
    if( !bIsAlive )
    {
      m_pThread->Kill();
          
      GFL_SAFE_DELETE( m_pThread );

      if( m_pDecodeThread->IsSuccess() )
      {
        m_pQREventListener->OnDecodeSuccess( m_pQRDecodeData );
      }
      else
      {
        m_pQREventListener->OnDecodeFailure();
      }
    }
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を実行中か調べます。
  * @retval true 実行中している
  * @retval false 実行中していない
  */
// --------------------------------------------------------------------------
bool CTR_QRDecoder::IsBusy()
{
  if( m_pThread != NULL )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
