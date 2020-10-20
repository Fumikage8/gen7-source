#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpConnector.cpp
 * @brief  HTTP通信アクセサクラス
 * @author h.suzuki
 * @date   2011.10.7
 */
//=============================================================================


#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <gflnet2/include/http/gflnet2_HttpConnectionListener.h>
#include <gflnet2/include/http/gflnet2_HttpConnector.h>

#include "gflnet2_HttpConnectionThread.h"

#if GFL_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(http)



//------------------------------------------------------------------
/**
 * @brief   HttpConnectorコンストラクタ＆デストラクタ
 */
//------------------------------------------------------------------
HttpConnector::HttpConnector() : m_accessThread(NULL)
{
}

HttpConnector::~HttpConnector()
{
  this->CancelAsyncAccess();
  this->DeleteHttpConnectionThread();
}

//-------------------------------------------------------------------------------
/**
 * @brief 非同期通信の開始
 *
 * @param[in] heap            スレッドの生成に使用するヒープ
 * @param[in] setting         HTTP通信用設定情報
 * @param[in] thread_priority スレッドの優先度( 高 0 〜 31 低 )
 *
 * @return 通信が開始できたかどうか
 * @retval true   開始できた
 * @retval false  開始できなかった
 *
 * @note
 * CheckAsyncAccess()で書き込みの終了を待った後,
 * EndAsyncAccess()を呼んでください。
 */
//-------------------------------------------------------------------------------
bool HttpConnector::StartASyncAccess( gfl2::heap::HeapBase* heap, const HttpConnectionSetting &setting, int thread_priority )
{
  if( this->CheckAsyncAccess() ) {
    return false;  // 非同期処理実行中
  }

  m_accessThread = GFL_NEW_LOW( heap ) HttpConnectionThread( heap, HTTP_THREAD_STACK_SIZE, setting );

  // スレッド開始
  m_accessThread->Start();

  return true;
}

//-------------------------------------------------------------------------------
/**
 * @brief 非同期通信が実行中かどうかを調べる
 * @retval true   実行中
 * @retval false  実行していない
 */
//-------------------------------------------------------------------------------
bool HttpConnector::CheckAsyncAccess( void ) const
{
  return ( m_accessThread && m_accessThread->IsAlive() );
}

//-------------------------------------------------------------------------------
/**
 * @brief 実行中の非同期通信をキャンセルする
 *
 * @note
 * CheckAsyncAccess()で非同期処理の終了を確認した後,
 * EndAsyncAccess()を呼んでください。
 */
//-------------------------------------------------------------------------------
void HttpConnector::CancelAsyncAccess( void )
{
  if( this->CheckAsyncAccess() ) {
    m_accessThread->Kill();
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief 非同期通信を完了する
 *
 * @note
 * スレッドの終了待ちを行います。
 * CheckAsyncAccess()でスレッドの終了を待ってから呼んでください。
 * CancelAsyncAccess()を呼んだ場合も同様です。
 */
//-------------------------------------------------------------------------------
void HttpConnector::EndAsyncAccess( void )
{
  this->DeleteHttpConnectionThread();
}

//-------------------------------------------------------------------------------
/**
 * @brief スレッドの終了待ちを行います。
 */
//-------------------------------------------------------------------------------
void HttpConnector::waitAsyncFinished()
{
  while( this->CheckAsyncAccess() ) {
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
}


/**
 * @brief 非同期処理スレッドを破棄する
 */
void HttpConnector::DeleteHttpConnectionThread( void )
{
  this->waitAsyncFinished();

  if( m_accessThread ) {
    GFL_DELETE m_accessThread;
    m_accessThread = NULL;
  }
}


GFL_NAMESPACE_END(http)
GFL_NAMESPACE_END(gflnet2)

#endif
#endif
