// ============================================================================
/*
 * @file gfl2_WIN32_QRDecoder.cpp
 * @brief QRコードのデコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#include <qr/source/win32/gfl2_WIN32_QRDecoder.h>
#include <qr/include/gfl2_IQREventListener.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(win32)


WIN32_QRDecoder::WIN32_QRDecoder() :
  m_pQREventListener( NULL )
{
}


WIN32_QRDecoder::~WIN32_QRDecoder()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を非同期で行います。
  * @param  pQREventListener デコード処理が完了された時に呼び出されるイベントリスナー
  * @param  pQRYUVImageInfo デコード対象の画像情報です
  */
// --------------------------------------------------------------------------
void WIN32_QRDecoder::DecodeAsync( IQREventListener* pQREventListener, const QRYUVImageInfo* pQRYUVImageInfo )
{
  if( m_pQREventListener == NULL )
  {
    m_pQREventListener = pQREventListener;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を行います。
  * @param  pQRYUVImageInfo デコード対象の画像情報です
  * @return QRコードのデコード結果情報を返します  
  */
// --------------------------------------------------------------------------
QRDecodeData* WIN32_QRDecoder::Decode( const QRYUVImageInfo* pQRYUVImageInfo )
{
  return NULL;
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を非同期で行う為の更新処理です。
  */
// --------------------------------------------------------------------------
void WIN32_QRDecoder::Update()
{
  if( m_pQREventListener != NULL )
  {
    m_pQREventListener->OnDecodeFailure();
    m_pQREventListener = NULL;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を実行中か調べます。
  * @retval true 実行中している
  * @retval false 実行中していない
  */
// --------------------------------------------------------------------------
bool WIN32_QRDecoder::IsBusy()
{
  if( m_pQREventListener != NULL )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_WIN32
