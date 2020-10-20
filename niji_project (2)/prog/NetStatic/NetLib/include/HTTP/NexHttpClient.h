//======================================================================
/**
 * @file  NexHttpClient.h
 *
 * @author  suzuki_hisanao
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef __NETAPP_NEX_HTTP_CLIENT_H__
#define __NETAPP_NEX_HTTP_CLIENT_H__
#pragma once
#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <gflnet2/include/http/gflnet2_HttpConnector.h>

#include <NetStatic/NetLib/include/HTTP/NexHttpClientListener.h>
#include <NetStatic/NetLib/include/HTTP/NexHttpResponse.h>
#include <OnlineCore/src/NintendoHTTP/SimpleHttpClient.h>

GFL_NAMESPACE_BEGIN(NetLib)

/**
 * @namespace NetLib::http
 * @brief     NEX拡張HTTPライブラリ
 */
GFL_NAMESPACE_BEGIN(HTTP)


class NexHttpConnection;

//------------------------------------------------------------------
/**
* @brief HTTP通信定義用構造体
*
* @note 通信リクエストの各種パラメータを本構造体で設定できます。<p>
*       デフォルトのタイムアウト時間は30secです。
*/
//------------------------------------------------------------------
struct HttpConnectSetting
{
  char*                     url;                                     //!< アクセスURL
  s32                       timeoutMillSeconds;                      //!< 通信タイムアウト時間(ミリ秒）
  size_t                    addPostParamNum;                         //!< Postパラメータ数
  gflnet2::http::PostParam   addPostParam[HTTP_SETTING_PARAM_SIZE];   //!< Postパラメータ
  size_t                    addHeaderFieldNum;                       //!< 送信時に追加するHttpヘッダ数
  gflnet2::http::HeaderParam addHeaderField[HTTP_SETTING_PARAM_SIZE]; //!< Headerパラメータ
  nn::http::EncodingType    postEncodeType;                          //!< エンコードタイプ
  u8*                       certData;                                //!< 証明書へのポインタ
  size_t                    certDataSize;                            //!< 証明書のサイズ

  void Initialize( void )
  {
    url                = NULL;
    timeoutMillSeconds = 30 * 1000;
    addPostParamNum    = 0;
    addHeaderFieldNum  = 0;
    postEncodeType     = nn::http::ENCODING_TYPE_AUTO;
    certData           = NULL;
    certDataSize       = 0;
    ::std::memset( addPostParam,   0, sizeof(addPostParam)   );
    ::std::memset( addHeaderField, 0, sizeof(addHeaderField) );
  }

  HttpConnectSetting()
  {
    Initialize();
  }
};


//------------------------------------------------------------------
/**
 * @brief NEX拡張HTTPライブラリ用ラッパークラス
 *
 * @note  NEXの機能を利用してHTTP通信を行います。<p>
 *        トランスポートおよびディスパッチ処理については、NEX起動時に指定したスレッドによって実行されます。
 */
//------------------------------------------------------------------
class NexHttpClient
{
public:
  //------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //------------------------------------------------------------------
   NexHttpClient( void );

   //------------------------------------------------------------------
   /**
    * @brief デストラクタ
    */
   //------------------------------------------------------------------
   virtual ~NexHttpClient();

public:
  //------------------------------------------------------------------
  /**
  * @brief NexHttpClient用リスナの登録
  *
  * @param[in] listener 登録対象のNexHttpClient用リスナ。
  *
  * @return 登録に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool RegistNexHttpClientListener( NexHttpClientListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief NexHttpClient用リスナの解除
  *
  * @param[in] listener 削除対象のNexHttpClient用リスナ。
  */
  //------------------------------------------------------------------
  void UnregistNexHttpClientListener( NexHttpClientListener* listener );

public:
  //------------------------------------------------------------------
  /**
  * @brief 初期化
  *
  * @return 初期化に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool Initialize( void );

  //------------------------------------------------------------------
  /**
  * @brief 終了処理
  */
  //------------------------------------------------------------------
  void Finalize( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief Post通信の開始
  *
  * @param[in] setting         アクセス設定情報
  * @param[in] relayBufferSize 一度にバッファに読み書きする最大サイズ。 この関数に渡される最大バッファサイズを指定します。
  *
  * @note 非同期通信が正常終了した場合はNexHttpClientListener::OnNexHttpConnectionSuccess()がコールされる。
  * @note 非同期通信が異常終了した場合はNexHttpClientListener::OnNexHttpConnectionError()がコールされる。
  *
  * @retval true  非同期通信の開始に成功
  * @retval false 非同期通信の開始に失敗
  */
  //------------------------------------------------------------------
  bool StartPost( const HttpConnectSetting &setting, qUnsignedInt32 relayBufferSize = nn::nex::HttpConnection::DEFAULT_RELAY_BUFFER_SIZE );

  //------------------------------------------------------------------
  /**
  * @brief Get通信の開始
  *
  * @param[in] setting         アクセス設定情報
  * @param[in] relayBufferSize 一度にバッファに読み書きする最大サイズ。 この関数に渡される最大バッファサイズを指定します。
  *
  * @note 非同期通信が正常終了した場合はNexHttpClientListener::OnNexHttpConnectionSuccess()がコールされる。
  * @note 非同期通信が異常終了した場合はNexHttpClientListener::OnNexHttpConnectionError()がコールされる。
  *
  * @retval true  非同期通信の開始に成功
  * @retval false 非同期通信の開始に失敗
  */
  //------------------------------------------------------------------
  bool StartGet( const HttpConnectSetting &setting, qUnsignedInt32 relayBufferSize = nn::nex::HttpConnection::DEFAULT_RELAY_BUFFER_SIZE );

  //------------------------------------------------------------------
  /**
   * @brief 通信中か判定
   *
   * @return 通信中ならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsConnecting( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する
   * @brief キャンセルが実施された場合、完了時にOnNexHttpConnectionCanceled()がコールされる。
   *
   * @note  通信状態にかかわらず、何度でもコールしても良い
   *
   * @return キャンセル処理が実施されたならtrueを返却する
   */
  //------------------------------------------------------------------
  bool CancelConnecting( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief  通信完了通知検知用関数
   *
   * @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void OnConnectCompletion(nn::nex::CallContext *pContext);

private:
  //------------------------------------------------------------------
  /**
   * @brief キャンセルイベントリスナ通知
   */
  //------------------------------------------------------------------
  void ReportCanceled( void );

  //------------------------------------------------------------------
  /**
   * @brief エラーのイベントリスナ通知
   *
   * @param[in] result 実行結果
   */
  //------------------------------------------------------------------
  void ReportError( const nn::nex::qResult& result );

  //------------------------------------------------------------------
  /**
   * @brief NEX関数呼び出しエラーのイベントリスナ通知
   */
  //------------------------------------------------------------------
  void ReportFunctionError( void );

private:
  static const u32                     LISTENER_NUM = 4;          //!< リスナ最大登録可能数

private:
  bool                                 m_isInit;                  //!< 初期化フラグ
  bool                                 m_isAccess;                //!< アクセスフラグ
  //
  nn::nex::ProtocolCallContext*        m_context;                 //!< コンテキスト
  nn::nex::SimpleHttpClient*           m_httpClient;              //!< HTTPクライアント
  //
  NetLib::HTTP::NexHttpClientListener* m_listener[LISTENER_NUM];  //!< レーティング用リスナ
  NetLib::HTTP::NexHttpConnection*     m_connection;              //!< HTTPコネクション
};


GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(netapp)

#endif  // __NETAPP_NEX_HTTP_CLIENT_H__
#endif
