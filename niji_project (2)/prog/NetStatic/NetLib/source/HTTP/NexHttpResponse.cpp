//======================================================================
/**
 * @file  NexHttpResponse.cpp
 *
 * @author  suzuki
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#include <NetStatic/NetLib/include/HTTP/NexHttpResponse.h>

#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(HTTP)


//--------------------------------------------------------------------------------------------
/**
* @brief コンストラクタ
*/
//--------------------------------------------------------------------------------------------
NexHttpResponse::NexHttpResponse( void )
{
  m_receivedBuffer = qNew nn::nex::qVector<nn::nex::qByte>;
  m_statusCode     = 0;
}


//--------------------------------------------------------------------------------------------
/**
* @brief デストラクタ
*/
//--------------------------------------------------------------------------------------------
NexHttpResponse::~NexHttpResponse()
{
  gflnet2::nex::NexFreeCollection( &m_receivedBuffer );
}


//--------------------------------------------------------------------------------------------
/**
* @brief HTTPステータスコードを取得
*
* @return HTTPステータスコード
*/
//--------------------------------------------------------------------------------------------
nn::nex::qInt NexHttpResponse::GetStatusCode() const
{
  return m_statusCode;
}


//--------------------------------------------------------------------------------------------
/**
* @brief HTTPレスポンスバッファを取得
*
* @return HTTPレスポンスバッファ
*/
//--------------------------------------------------------------------------------------------
const nn::nex::qVector<nn::nex::qByte>& NexHttpResponse::GetReceiveBuffer() const
{
  return *m_receivedBuffer;
}


//--------------------------------------------------------------------------------------------
/**
* @brief HTTPレスポンスバッファをクリアする
*/
//--------------------------------------------------------------------------------------------
void NexHttpResponse::ResetReceiveBuffer()
{
  m_receivedBuffer->clear();
}



GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(netapp)
#endif