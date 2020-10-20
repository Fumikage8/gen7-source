//======================================================================
/**
 * @file  NexHttpConnection.cpp
 *
 * @author  suzuki
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#include <NetStatic/NetLib/source/HTTP/NexHttpConnection.h>


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(HTTP)


//--------------------------------------------------------------------------------------------
/**
* @brief コンストラクタ
*/
//--------------------------------------------------------------------------------------------
NexHttpConnection::NexHttpConnection( void ) : m_setting()
{
}


//--------------------------------------------------------------------------------------------
/**
* @brief デストラクタ
*/
//--------------------------------------------------------------------------------------------
NexHttpConnection::~NexHttpConnection()
{
}

//--------------------------------------------------------------------------------------------
/**
* @brief HTTP設定情報を登録
*
* @param setting HTTP設定情報
*/
//--------------------------------------------------------------------------------------------
void NexHttpConnection::SetSetting( const HttpConnectSetting& setting )
{
  std::memcpy( &m_setting, &setting, sizeof(HttpConnectSetting) );
}


//--------------------------------------------------------------------------------------------
/**
* @brief HTTPレスポンスの取得
*
* @return HTTPレスポンス
*/
//--------------------------------------------------------------------------------------------
const NexHttpResponse& NexHttpConnection::GetHttpResponse( void )
{
  NexHttpResponse* response = this;
  return *response;
}


// HTTPリクエスト前に呼び出されます。パラメータ、ルートCA証明書のセットなどを行います。
//--------------------------------------------------------------------------------------------
/**
* @brief HTTPリクエスト前にコールされる。[callback]
*
* @note パラメータ、ルートCA証明書のセットなど本関数にて行うこと
*
* @caution  システム呼び出し用コールバック関数の為、ユーザー使用は禁止！
*
* @param conn  HTTPコネクションインスタンス
*/
//--------------------------------------------------------------------------------------------
// overwride
void NexHttpConnection::PrepareRequest( nn::nex::HttpConnection* conn )
{
  // 証明書設定
  if( (m_setting.certData != NULL) && (m_setting.certDataSize>0) ){
    conn->AddRootCa(m_setting.certData, m_setting.certDataSize);
  }

  // エンコードタイプ
  if( !m_setting.addHeaderField )
  {
    if( m_setting.postEncodeType ==  nn::http::ENCODING_TYPE_MULTIPART ){
      conn->AddHeaderField("Content-Type", "multipart/form-data");
    }
    else if( m_setting.postEncodeType ==  nn::http::ENCODING_TYPE_URL ){
      conn->AddHeaderField("Content-Type", "application/x-www-form-urlencoded");
    }
  }

  // HTTPヘッダ
  for( int i = 0; i < m_setting.addHeaderFieldNum; i++ ){
    conn->AddHeaderField( m_setting.addHeaderField[i].fieldName, m_setting.addHeaderField[i].fieldValue );
  }

  // POST BODY
  for( int i = 0; i < m_setting.addPostParamNum; i++ )
  {
    gflnet2::http::PostParam *param = &m_setting.addPostParam[i];

    if( param->type == gflnet2::http::E_POST_TYPE_ASCII )
    {
      conn->AddPostData( param->queryName, param->queryValue );

      if( std::strlen( param->queryValue ) > 128 ){
        GFL_PRINT( "post_param%d = [%s=**********]\n", i, param->queryName );
      }
      else{
        GFL_PRINT( "post_param%d = [%s=%s]\n", i, param->queryName, param->queryValue );
      }
    }
    else if( param->type == gflnet2::http::E_POST_TYPE_RAW ){
      conn->AddPostDataRaw( param->queryValue, param->queryValueSize );
    }
    else if( param->type == gflnet2::http::E_POST_TYPE_BINARY ){
      GFL_PRINT("gflnet2::http::E_POST_TYPE_BINARY is Not Support\n" );
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief HTTPリクエストヘッダの取得が完了した時にコールされる[callback]
*
* @caution システム呼び出し用コールバック関数の為、ユーザー使用は禁止！
*
* @param conn       HTTPコネクションインスタンス
* @param statusCode HTTPステータスコード
*/
//--------------------------------------------------------------------------------------------
// overwride
void NexHttpConnection::ProcessResponseHeader( nn::nex::HttpConnection *conn, nn::nex::qInt statusCode )
{
  GFL_PRINT( "NexHttpConnection::ProcessResponseHeader[statusCode= %u]\n", statusCode );

  m_statusCode = statusCode;
}


//--------------------------------------------------------------------------------------------
/**
* @brief HTTPレスポンスボディを取得した時にコールされる[callback]
*
* @caution システム呼び出し用コールバック関数の為、ユーザー使用は禁止！
*
* @param buffer     レスポンスボディ
* @param bufferSize レスポンスボディのサイズ
*
* @return データの受信処理が正常に完了したときは true を、完了できなかったときは false を返却する。
*         true を返した場合はダウンロード処理が継続される。
*         false を返した場合は次回の本関数呼び出しで再度同じデータが渡される。
*         (false を返し続けることによりダウンロード処理を一時中断することが可能。）
*/
//--------------------------------------------------------------------------------------------
// overwride
qBool NexHttpConnection::ProcessResponse( const nn::nex::qUnsignedInt8* buffer, qUnsignedInt32 bufferSize )
{
  if( m_receivedBuffer != NULL )
  {
    GFL_PRINT( "NexHttpConnection::ProcessResponse[response= %u byte]\n", bufferSize );

    const nn::nex::qByte* byteArray = static_cast<const nn::nex::qByte*>(buffer);
    m_receivedBuffer->insert(m_receivedBuffer->end(), byteArray, byteArray + bufferSize);
  }
  return true;
}





GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(netapp)
#endif
