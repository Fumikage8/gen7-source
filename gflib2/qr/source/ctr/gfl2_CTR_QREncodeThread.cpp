// ============================================================================
/*
 * @file gfl2_CTR_QREncodeThread.cpp
 * @brief QRコードのエンコード処理を非同期で行う為のスレッドクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <qr/source/ctr/gfl2_CTR_QREncodeThread.h>
#include <qr/source/ctr/gfl2_CTR_QREncodeExecutor.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


CTR_QREncodeThread::CTR_QREncodeThread( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo ) :
  m_pHeap( pHeap ),
  m_pDeviceHeap( pDeviceHeap ),
  m_pQRRGBImageInfo( pQRRGBImageInfo ),
  m_pQREncodeData( NULL ),
  m_bIsSuccess( false )
{
}


CTR_QREncodeThread::~CTR_QREncodeThread()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  スレッド内から呼ばれる関数です。QRコードのエンコード処理を実行します。
  */
// --------------------------------------------------------------------------
void CTR_QREncodeThread::Function()
{
  gfl2::qr::ctr::CTR_QREncodeExecutor* pExecutor = GFL_NEW( m_pHeap ) gfl2::qr::ctr::CTR_QREncodeExecutor( m_pHeap, m_pDeviceHeap );

  m_bIsSuccess = pExecutor->Execute( m_pQRRGBImageInfo, m_pQREncodeData );

  GFL_SAFE_DELETE( pExecutor );
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理の実行に必要なパラメータの設定をします。
  * @param  pQREncodeData QRコードのエンコード情報です
  */
// --------------------------------------------------------------------------
void CTR_QREncodeThread::SetExecuteParameter( const gfl2::qr::QREncodeData* pQREncodeData )
{
  m_pQREncodeData = pQREncodeData;
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理の成否を返します。
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_QREncodeThread::IsSuccess() const
{
  return m_bIsSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
