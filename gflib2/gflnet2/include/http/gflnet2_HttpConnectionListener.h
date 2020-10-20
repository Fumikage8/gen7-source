#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpConnectionListener.h
 * @brief  Http通信イベントハンドラ用クラス
 * @author h.suzuki
 * @date   2011.10.6
 */
//=============================================================================

#ifndef __GFLNET_HTTP_CONNECTION_LISTENER_H__
#define __GFLNET_HTTP_CONNECTION_LISTENER_H__
#pragma once

#include <nn.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/http/gflnet2_HttpResponse.h>

#if GFL_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(http)

/**
* @brief Http通信イベントハンドラ用クラス
* @note  イベントを検知したい場合は本クラスを継承し、virtual関数を実装すること
*/
class HttpConnectionListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(HttpConnectionListener); //コピーコンストラクタ＋代入禁止
public:
  virtual ~HttpConnectionListener(void){}
public:

  //------------------------------------------------------------------
  /**
   * @brief   Http通信の成功イベント。
   * @brief   ただしサーバからのステータスコードについては必ず確認すること。
   *
   * @param[in]  response  サーバから返却されたレスポンス情報
   */
  //------------------------------------------------------------------
   virtual void OnHttpConnectionSuccess( const gflnet2::http::HttpResponse& response ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   Http通信のエラーイベント
   *
   * @param[in]   result       エラーコード
   * @param[in]   functionName エラー発生箇所の関数名
   */
  //------------------------------------------------------------------
  virtual void OnHttpConnectionError( const nn::Result &result, const char *functionName ) = 0;
};


GFL_NAMESPACE_END(http)
GFL_NAMESPACE_END(gflnet2)

#endif // GFL_DEBUG

#endif // __GFLNET_HTTP_CONNECTION_LISTENER_H__
#endif
