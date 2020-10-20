//=============================================================================
/**
 * @file   NexHttpClientListener.h
 * @author suzuki_hisanao
 *
 * @note 通信の完了通知を受け取る場合は本クラスを継承し、仮想関数を実装してください。
 */
//=============================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef __NEX_HTTP_CLIENT_LISTENER_H__
#define __NEX_HTTP_CLIENT_LISTENER_H__
#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>


#include <NetStatic/NetLib/include/HTTP/NexHttpResponse.h>

namespace NetLib{
namespace HTTP{

/**
* @brief Http通信イベントハンドラ用クラス<BR>
*        イベントを検知したい場合は本クラスを継承し、virtual関数を実装すること
*/
class NexHttpClientListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(HttpConnectionListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
   * @brief   Http通信の成功イベント<BR>
   *          ただし、サーバからのステータスコードについては必ず確認すること。
   *
   * @param  response  サーバから返却されたレスポンス情報
   */
  //------------------------------------------------------------------
   virtual void OnNexHttpConnectionSuccess( const NetLib::HTTP::NexHttpResponse& response ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   Http通信のエラーイベント
   *
   * @param   result    エラーコード
   * @param   errorCode ネットワークエラーコード
   */
  //------------------------------------------------------------------
  virtual void OnNexHttpConnectionError( const nn::nex::qResult &result, u32 errorCode ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   Http通信のキャンセルイベント
   */
  //------------------------------------------------------------------
  virtual void OnNexHttpConnectionCanceled( void ) = 0;
};


} // http
} // netapp



#endif // __NEX_HTTP_CLIENT_LISTENER_H__
#endif