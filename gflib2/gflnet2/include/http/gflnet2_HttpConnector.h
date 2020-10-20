#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpConnector.h
 * @brief  Http通信アクセサクラス
 * @author h.suzuki
 * @date   2011.10.6
 */
//=============================================================================

#ifndef __GFLNET_HTTP_CONNECTOR_H__
#define __GFLNET_HTTP_CONNECTOR_H__
#pragma once


#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <gflnet2/include/http/gflnet2_HttpResponse.h>
#include <gflnet2/include/http/gflnet2_HttpConnectionListener.h>
#include <gflnet2/include/http/gflnet2_HttpConnectionSetting.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#if GFL_DEBUG

namespace gflnet2{
namespace http{


class HttpConnectionThread;

/**
 * @brief HTTP通信コネクション管理用クラス
 *
 * @note 本クラスはデバッグ用です。製品に使用する場合は任天堂様の承認が必要です。
 */
class HttpConnector
{
  GFL_FORBID_COPY_AND_ASSIGN(HttpConnector); //コピーコンストラクタ＋代入禁止

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  HttpConnector();

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~HttpConnector();


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
    * @note CheckAsyncAccess()で書き込みの終了を待った後、EndAsyncAccess()を呼んでください。
    */
   //-------------------------------------------------------------------------------
   virtual bool StartASyncAccess( gfl2::heap::HeapBase* heap, const gflnet2::http::HttpConnectionSetting &setting, int thread_priority );

  //-------------------------------------------------------------------------------
   /**
    * @brief 非同期通信が実行中かどうかを調べる
    * @retval true   実行中
    * @retval false  実行していない
    */
   //-------------------------------------------------------------------------------
   virtual bool CheckAsyncAccess( void ) const;

   //-------------------------------------------------------------------------------
   /**
    * @brief 実行中の非同期通信をキャンセルする
    *
    * @note CheckAsyncAccess()で非同期処理の終了を確認した後、EndAsyncAccess()を呼んでください。
    */
   //-------------------------------------------------------------------------------
   virtual void CancelAsyncAccess( void );

   //-------------------------------------------------------------------------------
   /**
    * @brief 非同期通信を完了する
    *
    * @note  スレッドの終了待ちを行います。
    * @note  CheckAsyncAccess()でスレッドの終了を待ってから呼んでください。
    * @note  CancelAsyncAccess()を呼んだ場合も同様です。
    */
   //-------------------------------------------------------------------------------
   virtual void EndAsyncAccess( void );


private:
   HttpConnectionThread* m_accessThread;  //!< 非同期アクセススレッド

   static const u32 HTTP_THREAD_STACK_SIZE = 8192;   //!< 通信スレッドで使用されるスタックサイズ

   //-------------------------------------------------------------------------------
   /**
    * @brief スレッドの終了待ちを行います。
    */
   //-------------------------------------------------------------------------------
   void waitAsyncFinished();

   //-------------------------------------------------------------------------------
   /**
    * @brief 非同期処理スレッドを破棄する
    */
   //-------------------------------------------------------------------------------
   void DeleteHttpConnectionThread( void );

};


} // http
} // gflnet


#endif // GF_DEBUG

#endif // __GFLNET_HTTP_CONNECTOR_H__


#endif
