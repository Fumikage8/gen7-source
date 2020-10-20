#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpConnectionSetting.h
 * @brief  HTTP通信用設定ファイル
 * @author h.suzuki
 * @date   2011.10.7
 */
//=============================================================================

#ifndef __GFLNET_HTTP_CONNECTION_SETTING_H__
#define __GFLNET_HTTP_CONNECTION_SETTING_H__
#pragma once

#include <nn.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(http)

class HttpConnectionListener;

#define HTTP_SETTING_PARAM_SIZE  64

/**
* @brief POSTパラメータタイプ
*/
typedef enum
{
  E_POST_TYPE_ASCII,        //!< ASCII
  E_POST_TYPE_BINARY,       //!< Binary
  E_POST_TYPE_RAW,          //!< octed
}PostParamType;


/**
* @brief POSTパラメータ
*/
typedef struct
{
  char     queryName[64];   //!< クエリ名
  size_t   queryValueSize;  //!< データサイズ
  char*    queryValue;      //!< データポインタ
  s32      type;            //!< データタイプ(PostParamType)
}PostParam;


/**
* @brief Headerパラメータ
*/
typedef struct
{
  char*    fieldName;       //!< フィールド名
  char*    fieldValue;      //!< フィールド値
}HeaderParam;


/**
* @brief スレッドの動作を定義するパラメータ
*/
struct HttpConnectionSetting
{
#if GFL_DEBUG
  HttpConnectionListener *listener;                                //!< 通信完了・エラー通知用リスナ
#endif
  char                   *url;                                     //!< アクセスURL
  s32                     timeoutSeconds;                          //!< 通信タイムアウト時間(秒）
  s32                     method;                                  //!< GET or POST(nn::http::RequestMethod)
  size_t                  addPostParamNum;                         //!< Postパラメータ数
  PostParam               addPostParam[HTTP_SETTING_PARAM_SIZE];   //!< Postパラメータ
  size_t                  addHeaderFieldNum;                       //!< 送信時に追加するHttpヘッダ数
  HeaderParam             addHeaderField[HTTP_SETTING_PARAM_SIZE]; //!< Headerパラメータ
  nn::http::EncodingType  postEncodeType;                          //!< エンコードタイプ
  u8*                     certData;                                //!< 証明書へのポインタ
  size_t                  certDataSize;                            //!< 証明書のサイズ

  HttpConnectionSetting()
  {
    timeoutSeconds    = 30;
    addPostParamNum   = 0;
    addHeaderFieldNum = 0;
    postEncodeType    = nn::http::ENCODING_TYPE_AUTO;
    certData          = NULL;
    certDataSize      = 0;
  }
};



GFL_NAMESPACE_END(http)
GFL_NAMESPACE_END(gflnet2)


#endif

#endif
