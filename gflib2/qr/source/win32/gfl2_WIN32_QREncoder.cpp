// ============================================================================
/*
 * @file gfl2_WIN32_QREncoder.cpp
 * @brief QRコードのでコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#include <qr/source/win32/gfl2_WIN32_QREncoder.h>
#include <qr/include/gfl2_IQREventListener.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(win32)


WIN32_QREncoder::WIN32_QREncoder() :
  m_pQREventListener( NULL )
{
}


WIN32_QREncoder::~WIN32_QREncoder()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を非同期で行います。
  * @param  pQREventListener エンコード処理が完了された時に呼び出されるイベントリスナー
  * @param  pQREncodeData QRコードのエンコード情報です
  */
// --------------------------------------------------------------------------
void WIN32_QREncoder::EncodeAsync( IQREventListener* pQREventListener, const QREncodeData* pQREncodeData )
{
  if( m_pQREventListener == NULL )
  {
    m_pQREventListener = pQREventListener;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を行います。
  * @param  pQREncodeData QRコードのエンコード情報です
  * @return QRコードのイメージ情報を返します
  */
// --------------------------------------------------------------------------
QRRGBImageInfo* WIN32_QREncoder::Encode( const QREncodeData* pQREncodeData )
{
  return NULL;
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を非同期で行う為の更新処理です。
  */
// --------------------------------------------------------------------------
void WIN32_QREncoder::Update()
{
  if( m_pQREventListener != NULL )
  {
    m_pQREventListener->OnEncodeFailure();
    m_pQREventListener = NULL;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を実行中か調べます。
  * @retval true 実行中している
  * @retval false 実行中していない
  */
// --------------------------------------------------------------------------
bool WIN32_QREncoder::IsBusy()
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
