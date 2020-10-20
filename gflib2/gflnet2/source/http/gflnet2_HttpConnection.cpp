#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpConnection.cpp
 * @brief  HTTP通信クラス(隠蔽ライブラリクラス）
 * @author h.suzuki
 * @date   2011.10.6
 */
//=============================================================================

#include <nn/ac/ac_Result.h>


#include <gflnet2/include/base/gflnet2_Base.h>

#include "gflnet2/source/http/gflnet2_HttpConnection.h"


#if GFL_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(http)


//------------------------------------------------------------------
/**
 * @brief   HttpConnectionコンストラクタ
 */
//------------------------------------------------------------------
HttpConnection::HttpConnection( gfl2::heap::HeapBase* heap ) : HttpResponse( &connection )
{
  m_heapBase = heap;
}


//------------------------------------------------------------------
/**
 * @brief   HttpConnectionデストラクタ
 */
//------------------------------------------------------------------
HttpConnection::~HttpConnection()
{
  Close();
}


//------------------------------------------------------------------
/**
 * @brief  セッションの確立
 *
 * @param url      アクセス先ＵＲＬ
 * @param method   アクセス方法（GET/POST)
 * @param listener 通信結果通知用リスナの実態へのポインタ
 */
//------------------------------------------------------------------
void HttpConnection::Open( const char *url, nn::http::RequestMethod method, const HttpConnectionListener* listener )
{
  Close();

  finishListener = const_cast<HttpConnectionListener*>(listener);
  ::std::strcpy( accessUrl, url );

  connection.Initialize( accessUrl, method, true );
}


//------------------------------------------------------------------
/**
 * @brief   ＨＴＴＰヘッダフィールドに値を追加する
 *
 * @fieldName 追加フィールド名
 * @fieldName 追加フィールドの値
 *
 * @return 追加結果
 */
//------------------------------------------------------------------
nn::Result HttpConnection::AddHeader( const char* fieldName, const char *fieldValue )
{
  return connection.AddHeaderField( fieldName, fieldValue );
}


//------------------------------------------------------------------
/**
 * @brief   送信対象となるPostデータを追加する
 *
 * @attention 通信が完了するまで、データを解放してはならない
 */
//------------------------------------------------------------------
nn::Result HttpConnection::AddPostDataAscii( const char * pLabel, const char * pValue )
{
  return connection.AddPostDataAscii( pLabel, pValue );
}
nn::Result HttpConnection::AddPostDataBinary( const char * pLabel, const void * pValue, size_t valueSize )
{
  return connection.AddPostDataBinary( pLabel, pValue, valueSize );
}
nn::Result HttpConnection::AddPostDataRaw( const void * pValue, size_t valueSize )
{
  return connection.AddPostDataRaw( pValue, valueSize );
}


//------------------------------------------------------------------
/**
 * @brief   証明書を設定する
 *
 * @note    通信が完了するまで、証明書を解放してはならない。
 * @note    証明書データ格納バッファは4バイトアライメントされていること。
 *
 * @param   pCertData 証明書へのポインタ。
 * @param   dataSize  証明書のサイズ
 */
//------------------------------------------------------------------
nn::Result HttpConnection::SetRootCA( const u8 * pCertData, size_t dataSize )
{
  return connection.SetRootCa( pCertData, dataSize );
}


//------------------------------------------------------------------
/**
 * @brief   エンコード方式を設定する
 *
 * @note    POSTデータを設定する前に本関数をコールすること！
 *
 * @param   encodeType エンコード方式
 */
//------------------------------------------------------------------
nn::Result HttpConnection::SetPostDataEncoding( nn::http::EncodingType encodeType )
{
  return connection.SetPostDataEncoding( encodeType );
}


//------------------------------------------------------------------
/**
 * @brief   データの受信を開始。
 *          ヘッダの設定やPostBodyの設定は、このメソッドをコールする前に行うこと！
 *
 * @param   timeout 通信タイムアウトまでの時間
 */
//------------------------------------------------------------------
bool HttpConnection::Connect( const nn::fnd::TimeSpan & timeout )
{
  nn::Result result = connection.Connect();

  if( !result.IsSuccess() ){
    DoError( result, "nn::http::Connection::Connect" );
    return false;
  }

  GetResponse( timeout );

  return true;
}

//------------------------------------------------------------------
/**
 * @brief   HttpConnectionのクローズ。使用後は必ずコールすること！
 */
//------------------------------------------------------------------
void HttpConnection::Close()
{
  ReleaseResponseBody();
  connection.Finalize();
}

//------------------------------------------------------------------
/**
 * @brief   レスポンスボディーの解放
 */
//------------------------------------------------------------------
void HttpConnection::ReleaseResponseBody( void )
{
  if( NULL != responseBody )
  {
    GflHeapFreeMemory( responseBody );
    responseBody = NULL;
  }
}

//------------------------------------------------------------------
/**
 * @brief   通信キャンセル
 */
//------------------------------------------------------------------
void HttpConnection::Cancel()
{
  connection.Cancel();
  Close();
}


//------------------------------------------------------------------
/**
 * @brief   受信データの解析
 *
 * @param timeout 通信タイムアウトまでの時間
 */
//------------------------------------------------------------------
void HttpConnection::GetResponse( const nn::fnd::TimeSpan &timeout )
{
  nn::Result result;

  // Check Status Code-------------------------------------
  statusCode = 0;
  result = connection.GetStatusCode( &statusCode );

  if( !result.IsSuccess() )
  {
    char errorMes[128];
    ::std::sprintf( errorMes, "nn::http::Connection::GetStatusCode[%d]", result.GetDescription() );

    DoError( result, errorMes );
    return;
  }

  // Get Content-Length------------------------------------
  char   headerFields[256];
  result = GetHeaderField("Content-Length", headerFields, sizeof(headerFields), NULL);

  if( !result.IsSuccess() ){
    DoError( result, "nn::http::Connection::GetHeaderField" );
    return;
  }
  contentLength = std::atoi(headerFields);

  // Get Body----------------------------------------------
  if( contentLength > 0 )
  {
    ReleaseResponseBody();
    responseBody = (u8*)GflHeapAllocMemory(m_heapBase, contentLength);

    if( responseBody != NULL){
      result = connection.Read( responseBody, contentLength, timeout );
    }
    else{
      DoError( nn::ac::ResultOutOfMemory(), "gflnet2::http::HttpConnection::getResponse" );
      return;
    }
    if( !result.IsSuccess() ){
      DoError( result, "nn::http::Connection::Read" );
      return;
    }
  }

  // success!!----------------------------------------------
  if( finishListener != NULL )
  {
    finishListener->OnHttpConnectionSuccess( *this );
  }
}


//------------------------------------------------------------------
 /**
  * @brief  エラー通知処理
  * @param  result エラー情報
  * @param  function エラー発生関数名
  */
 //------------------------------------------------------------------
void HttpConnection::DoError( const nn::Result &result, const char *functionName )
{
  if( finishListener != NULL )
  {
    finishListener->OnHttpConnectionError( result, functionName );
  }
}



GFL_NAMESPACE_END(http)
GFL_NAMESPACE_END(gflnet2)

#endif // GF_DEBUG
#endif
