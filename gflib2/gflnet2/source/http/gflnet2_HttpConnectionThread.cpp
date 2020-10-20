#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpConnectionThread.cpp
 * @brief  HTTP通信スレッド処理用クラス(隠蔽ライブラリクラス）
 * @author h.suzuki
 * @date   2011.10.7
 */
//=============================================================================

#include <nn.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include "gflnet2/source/http/gflnet2_HttpConnectionThread.h"

#if GFL_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(http)

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 * @param setting     スレッドの動作を定義するパラメータ
 */
//--------------------------------------------------------------------------------
HttpConnectionThread::HttpConnectionThread( gfl2::heap::HeapBase* heap, u32 stack_size, const HttpConnectionSetting& setting ) :
  m_connection(heap)
, m_pThread(NULL)
{
  m_setting = setting;

  m_pThread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, true, stack_size);
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
HttpConnectionThread::~HttpConnectionThread()
{
  GFL_SAFE_DELETE( m_pThread );
  m_connection.Close();
}


//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理
 */
//--------------------------------------------------------------------------------
void HttpConnectionThread::Function( void )
{
  m_connection.Open( m_setting.url, static_cast<nn::http::RequestMethod>(m_setting.method), m_setting.listener );

  // Certify
  if( (m_setting.certData !=NULL) && (m_setting.certDataSize>0) ){
    m_connection.SetRootCA( m_setting.certData, m_setting.certDataSize );
  }

  // Set Http Header
  for( int i = 0; i < m_setting.addHeaderFieldNum; i++ ){
    m_connection.AddHeader( m_setting.addHeaderField[i].fieldName, m_setting.addHeaderField[i].fieldValue );
  }

  // Post Encoding
  m_connection.SetPostDataEncoding( m_setting.postEncodeType );

  // Set Post Body
  if( m_setting.method == nn::http::REQUEST_METHOD_POST )
  {
    for( int i = 0; i < m_setting.addPostParamNum; i++ )
    {
      PostParam *param = &m_setting.addPostParam[i];

      if( param->type == E_POST_TYPE_ASCII ){
        m_connection.AddPostDataAscii( param->queryName, param->queryValue );
      }
      else if( param->type == E_POST_TYPE_RAW ){
        m_connection.AddPostDataRaw( param->queryValue, param->queryValueSize );
      }
      else if( param->type == E_POST_TYPE_BINARY ){
        m_connection.AddPostDataBinary( param->queryName, param->queryValue, param->queryValueSize );
      }
    }
  }

  m_connection.Connect( nn::fnd::TimeSpan::FromSeconds( m_setting.timeoutSeconds ) );

  m_connection.Close();

  m_pThread->Kill();
}

GFL_NAMESPACE_END(http)
GFL_NAMESPACE_END(gflnet2)

#endif

#endif
