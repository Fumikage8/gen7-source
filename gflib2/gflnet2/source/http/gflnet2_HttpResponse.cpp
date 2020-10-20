#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpResponse.cpp
 * @brief  HTTP通信レスポンスクラス
 * @author h.suzuki
 * @date   2011.10.6
 */
//=============================================================================

#include <nn.h>
#include <nn/http.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "gflnet2/include/base/gflnet2_Base.h"

#include "gflnet2/include/http/gflnet2_HttpResponse.h"

#if GFL_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(http)


//------------------------------------------------------------------
/**
 * @brief   HttpResponseコンストラクタ
 */
//------------------------------------------------------------------
HttpResponse::HttpResponse( const nn::http::Connection *con )
{
  connectionPtr = const_cast<nn::http::Connection*>(con);
  statusCode    = 0;
  contentLength = 0;
  responseBody  = NULL;
}

//------------------------------------------------------------------
/**
 * @brief   HttpResponseデストラクタ
 */
//------------------------------------------------------------------
HttpResponse::~HttpResponse()
{
}

//------------------------------------------------------------------
 /**
  * @brief  ＨＴＴＰステータスコードの取得
  * @return ＨＴＴＰステータスコード。
  */
 //------------------------------------------------------------------
s32 HttpResponse::GetStatusCode( void ) const
{
  return statusCode;
}


//------------------------------------------------------------------
 /**
  * @brief  ＨＴＴＰヘッダ「Content-Length」を取得
  * @return 「Content-Length」の値。
  */
 //------------------------------------------------------------------
s32 HttpResponse::GetContentLength( void ) const
{
  return contentLength;
}

//------------------------------------------------------------------
 /**
  * @brief  ＨＴＴＰヘッダフィールドを取得
  *
  * @param  fieldName           ＨＴＴＰヘッダフィールド名
  * @param  pFieldBuf           ヘッダ情報格納先バッファ
  * @param  bufSize             ヘッダ情報格納先バッファサイズ
  * @param  pFieldLengthCourier ヘッダ格納バッファのサイズ。bufSizeより大きい場合は、バッファが不足していることを意味する。
  *
  * @return リザルトクラス
  */
 //------------------------------------------------------------------
nn::Result HttpResponse::GetHeaderField( const char* fieldName, char *pFieldBuf, size_t bufSize, size_t *pFieldLengthCourier )
{
  memset(pFieldBuf, 0, bufSize);

  return connectionPtr->GetHeaderField(fieldName, pFieldBuf, bufSize, pFieldLengthCourier);
}


//------------------------------------------------------------------
/**
 * @brief  ＨＴＴＰレスポンスボディを取得する
 *
 * @return レスポンスボディーバッファを返却。
 *         インスタンスの終了とともにメモリから開放されるので、必要な場合は
 *         GetContentLength()でサイズを取得し、取得したデータを独自のバッファにコピーすること。
 */
//------------------------------------------------------------------
u8* HttpResponse::GetBody() const
{
  return responseBody;
}



GFL_NAMESPACE_END(http)
GFL_NAMESPACE_END(gflnet2)

#endif // GFL_DEBUG
#endif
