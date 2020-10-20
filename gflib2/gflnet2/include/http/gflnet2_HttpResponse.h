#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpResponse.h
 * @brief  Http通信レスポンスクラス。ユーザーが本クラスを生成してはならない。
 * @author h.suzuki
 * @date   2011.10.6
 */
//=============================================================================

#ifndef __GFLNET_HTTP_HTTP_RESPONSE_H__
#define __GFLNET_HTTP_HTTP_RESPONSE_H__
#pragma once

#include <nn.h>

#include <nn/http.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>

#if GFL_DEBUG

namespace gflnet2{
namespace http{

/**
 * @brief HTTP通信レスポンスデータ用クラス
 */
class HttpResponse
{
//  GFL_FORBID_COPY_AND_ASSIGN(HttpResponse); //コピーコンストラクタ＋代入禁止

protected:
  nn::http::Connection* connectionPtr;  //!< コネクションインスタンス
  s32                   statusCode;     //!< ステータスコード
  int                   contentLength;  //!< ＨＴＴＰヘッダ「Content-Length」
  u8*                   responseBody;   //!< レスポンスBody

public:
  //------------------------------------------------------------------
  /**
   * @brief   HttpResponseコンストラクタ＆デストラクタ
   */
  //------------------------------------------------------------------
  HttpResponse( const nn::http::Connection *con );
  virtual ~HttpResponse();

public:
  //------------------------------------------------------------------
  /**
   * @brief  ＨＴＴＰステータスコードの取得
   * @return ＨＴＴＰステータスコード。エラー時は−１を返却。
   */
  //------------------------------------------------------------------
  s32 GetStatusCode( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief  ＨＴＴＰヘッダ「Content-Length」を取得
   * @return 「Content-Length」の値。エラー時は−１、ヘッダが不在の場合は０を返却。
   */
  //------------------------------------------------------------------
  s32 GetContentLength( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief  ＨＴＴＰヘッダフィールドを取得
   *
   * @param[out]  fieldName           ＨＴＴＰヘッダフィールド名
   * @param[out]  pFieldBuf           ヘッダ情報格納先バッファ
   * @param[in]   bufSize             ヘッダ情報格納先バッファサイズ
   * @param[out]  pFieldLengthCourier ヘッダ格納バッファのサイズ。
   * @param[out]  pFieldLengthCourier bufSizeより大きい場合は、バッファが不足していることを意味する。
   *
   * @return リザルトクラス
   */
  //------------------------------------------------------------------
  nn::Result GetHeaderField( const char* fieldName, char *pFieldBuf, size_t bufSize, size_t *pFieldLengthCourier );

  //------------------------------------------------------------------
  /**
   * @brief  ＨＴＴＰレスポンスボディを取得する
   *
   * @return レスポンスボディーバッファを返却。
   * @return インスタンスの終了とともにメモリから開放される。
   * @return 必要な場合はGetContentLength()でサイズを取得し、取得したデータを独自のバッファにコピーすること。
   */
  //------------------------------------------------------------------
  u8* GetBody() const;

};

} // http
} // gflnet

#endif // GFL_DEBUG

#endif // __GFLNET_HTTP_HTTP_RESPONSE_H__

#endif
