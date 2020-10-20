#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpConnection.h
 * @author suzuki_hisanao
 */
//=============================================================================

#ifndef __GFLNET_HTTP_CONNECTION_H__
#define __GFLNET_HTTP_CONNECTION_H__
#pragma once


#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <gflnet2/include/http/gflnet2_HttpResponse.h>
#include <gflnet2/include/http/gflnet2_HttpConnectionListener.h>

#if GFL_DEBUG

namespace gflnet2{
namespace http{


/**
 * @brief  HTTP通信クラス(隠蔽ライブラリクラス）
 */
class HttpConnection : public HttpResponse
{
  GFL_FORBID_COPY_AND_ASSIGN(HttpConnection); // コピーコンストラクタ＋代入禁止

private:
  nn::http::Connection    connection;         //!< コネクションインスタンス
  HttpConnectionListener* finishListener;     //!< 通信終了通知用リスナ
  char                    accessUrl[512];     //!< アクセスＵＲＬ
  gfl2::heap::HeapBase*    m_heapBase;         //!< ヒープベースアドレス

public:
  //------------------------------------------------------------------
  /**
   * @brief   HttpConnectionコンストラクタ＆デストラクタ
   */
  //------------------------------------------------------------------
  HttpConnection( gfl2::heap::HeapBase* heap );
  virtual ~HttpConnection();

public:
  //------------------------------------------------------------------
  /**
   * @brief  セッションの確立(ブロックAPI)
   *
   * @param[in] url      アクセス先ＵＲＬ
   * @param[in] method   アクセス方法（GET/POST)
   * @param[in] listener 通信結果通知用リスナの実態へのポインタ。
   *                     通信の終了、失敗のタイミングでハンドライベントがコールされる。
   */
  //------------------------------------------------------------------
  void Open( const char *url, nn::http::RequestMethod method, const HttpConnectionListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   HttpConnectionのクローズ。使用後は必ずコールすること！
   */
  //------------------------------------------------------------------
  void Close();

  //------------------------------------------------------------------
  /**
   * @brief   ＨＴＴＰヘッダフィールドに値を追加する。
   *
   * @note    コール回数分のヘッダ情報が追加される。
   *
   * @param[in] fieldName  追加フィールド名
   * @param[in] fieldValue 追加フィールドの値
   *
   * @return 追加結果
   */
  //------------------------------------------------------------------
  nn::Result AddHeader( const char* fieldName, const char *fieldValue );

  //------------------------------------------------------------------
  /**
   * @brief   証明書を設定する
   *
   * @param[in]   pCertData 証明書へのポインタ
   * @param[in]   dataSize  証明書のサイズ
   */
  //------------------------------------------------------------------
  nn::Result SetRootCA( const u8* pCertData, size_t dataSize );

  //------------------------------------------------------------------
  /**
   * @brief   エンコード方式を設定する
   *
   * @note    POSTデータを設定する前に本関数をコールすること！
   *
   * @param[in]   encodeType エンコード方式
   */
  //------------------------------------------------------------------
  nn::Result SetPostDataEncoding( nn::http::EncodingType encodeType );

  //------------------------------------------------------------------
  /**
   * @brief   送信対象となるPostデータ[ASCII文字列]を追加する
   *
   * @brief[in] pValue     データへのポインタ
   * @brief[in] valueSize  データサイズ
   *
   * @attention 通信が完了するまで、データを解放してはならない
   */
  //------------------------------------------------------------------
  nn::Result AddPostDataAscii( const char * pLabel, const char * pValue );

  //------------------------------------------------------------------
  /**
   * @brief   送信対象となるPostデータ[バイナリデータ]を追加する
   *
   * @brief[in] pValue     データへのポインタ
   * @brief[in] valueSize  データサイズ
   *
   * @attention 通信が完了するまで、データを解放してはならない
   */
  //------------------------------------------------------------------
  nn::Result AddPostDataBinary( const char * pLabel, const void * pValue, size_t valueSize );

  //------------------------------------------------------------------
  /**
   * @brief   送信対象となるPostデータ[RAWデータ]を追加する
   *
   * @brief[in] pValue     データへのポインタ
   * @brief[in] valueSize  データサイズ
   *
   * @attention 通信が完了するまで、データを解放してはならない
   */
  //------------------------------------------------------------------
  nn::Result AddPostDataRaw( const void * pValue, size_t valueSize );

  //------------------------------------------------------------------
  /**
   * @brief   データの受信を開始(ブロックAPI)。
   *
   * @note    ヘッダの設定やPostBodyの設定は、このメソッドをコールする前に行うこと！
   *
   * @param[in] timeout 通信タイムアウトまでの時間
   *
   * @return 接続に成功ならtrueを返却
   */
  //------------------------------------------------------------------
  bool Connect( const nn::fnd::TimeSpan & timeout );

  //------------------------------------------------------------------
  /**
   * @brief   通信キャンセル
   */
  //------------------------------------------------------------------
  void Cancel();


private:
  //------------------------------------------------------------------
  /**
   * @brief   受信データの解析
   *
   * @param[in] timeout 通信タイムアウトまでの時間
   */
  //------------------------------------------------------------------
  void GetResponse( const nn::fnd::TimeSpan & timeout );

  //------------------------------------------------------------------
   /**
    * @brief  エラー通知処理
    *
    * @param[in]  result   エラー情報
    * @param[in]  function エラー発生関数名
    */
   //------------------------------------------------------------------
  void DoError( const nn::Result &result, const char *functionName );

  //------------------------------------------------------------------
  /**
   * @brief   レスポンスボディーの解放
   */
  //------------------------------------------------------------------
  void ReleaseResponseBody( void );

};




} // http
} // gflnet

#endif // GFL_DEBUG

#endif // __GFLNET_HTTP_CONNECTION_H__


#endif
