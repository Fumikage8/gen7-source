#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpConnectionThread.h
 * @author suzuki_hisanao
 */
//=============================================================================

#ifndef __GFLNET_HTTP_CONNECTION_THREAD_H__
#define __GFLNET_HTTP_CONNECTION_THREAD_H__
#pragma once

#include <nn.h>
#include <nn/fnd/fnd_TimeSpan.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/http/gflnet2_HttpResponse.h>
#include <gflnet2/source/http/gflnet2_HttpConnection.h>
#include <gflnet2/include/http/gflnet2_HttpConnectionSetting.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_Threadstatic.h>

#if GFL_DEBUG

namespace gflnet2{
namespace http{


/**
 * @brief  HTTP通信スレッド用クラス。
 *         HTTP通信を非同期で行う為のスレッドクラスです。
 */
class HttpConnectionThread : public gfl2::thread::ThreadInterface
{
private:
  HttpConnectionSetting   m_setting;       //!< 通信設定
  HttpConnection          m_connection;    //!< HTTPコネクションクラス
  gfl2::thread::Thread* m_pThread;

public:
  //--------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param[in] heap        スレッドのスタック領域に使用するヒープ
   * @param[in] stack_size  スレッドのスタック領域のサイズ
   * @param[in] setting     スレッドの動作を定義するパラメータ
   */
  //--------------------------------------------------------------------------------
  HttpConnectionThread( gfl2::heap::HeapBase* heap, u32 stack_size, const HttpConnectionSetting& setting );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~HttpConnectionThread();

  void Kill(void){ m_pThread->Kill(); };
  void Start(void) { m_pThread->Start(); };
  bool IsAlive(void) { return m_pThread->IsAlive(); };

private:

  virtual void Function( void );
};


} // http
} // gflnet


#endif // GFL_DEBUG
#endif // __GFLNET_HTTP_CONNECTION_THREAD_H__

#endif
