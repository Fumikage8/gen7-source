// ============================================================================
/*
 * @file gfl2_CTR_QRDecodeThread.cpp
 * @brief QRコードのデコード処理を非同期で行う為のスレッドクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <qr/source/ctr/gfl2_CTR_QRDecodeThread.h>
#include <qr/source/ctr/gfl2_CTR_QRDecodeExecutor.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


CTR_QRDecodeThread::CTR_QRDecodeThread( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::qr::QRDecodeData* pQRDecodeData ) :
  m_pHeap( pHeap ),
  m_pDeviceHeap( pDeviceHeap ),
  m_pQRDecodeData( pQRDecodeData ),
  m_pQRYUVImageInfo( NULL ),
  m_bIsSuccess( false )
{
}


CTR_QRDecodeThread::~CTR_QRDecodeThread()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  スレッド内から呼ばれる関数です。QRコードのデコード処理を実行します。
  */
// --------------------------------------------------------------------------
void CTR_QRDecodeThread::Function()
{
  gfl2::qr::ctr::CTR_QRDecodeExecutor* pExecutor = GFL_NEW( m_pHeap ) gfl2::qr::ctr::CTR_QRDecodeExecutor( m_pHeap, m_pDeviceHeap );

  m_bIsSuccess = pExecutor->Execute( m_pQRDecodeData, m_pQRYUVImageInfo );

  GFL_SAFE_DELETE( pExecutor );
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理の実行に必要なパラメータの設定をします。
  * @param  pQRYUVImageInfo デコード対象の画像情報です
  */
// --------------------------------------------------------------------------
void CTR_QRDecodeThread::SetExecuteParameter( const gfl2::qr::QRYUVImageInfo* pQRYUVImageInfo )
{
  m_pQRYUVImageInfo = pQRYUVImageInfo;
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理の成否を返します。
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_QRDecodeThread::IsSuccess() const
{
  return m_bIsSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
