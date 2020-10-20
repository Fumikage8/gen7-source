//======================================================================
/**
 * @file  NexHttpConnection.h
 *
 * @author suzuki_hisanao
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef __NETAPP_NEX_HTTP_CONNECTION_H__
#define __NETAPP_NEX_HTTP_CONNECTION_H__

#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>


#include <NetStatic/NetLib/include/HTTP/NexHttpClient.h>
#include <OnlineCore/src/NintendoHTTP/HttpEventListener.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(HTTP)


//--------------------------------------------------------------------------------------------
/**
 * @brief NEX拡張HTTPライブラリ用コネクション管理クラス
 *
 * @attention 本クラスの生成は、qNewにて行ってください
 *
 * @note 本クラスはレスポンスデータの管理のみを行います
 */
//--------------------------------------------------------------------------------------------
class NexHttpConnection : public nn::nex::HttpEventListener, public NexHttpResponse, public nn::nex::RootObject
{
public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
   NexHttpConnection( void );

   //--------------------------------------------------------------------------------------------
   /**
    * @brief デストラクタ
    */
   //--------------------------------------------------------------------------------------------
   virtual ~NexHttpConnection();

public:
   //--------------------------------------------------------------------------------------------
   /**
   * @brief HTTP設定情報を登録
   *
   * @param setting HTTP設定情報
   */
   //--------------------------------------------------------------------------------------------
   void SetSetting( const HttpConnectSetting& setting );

   //--------------------------------------------------------------------------------------------
   /**
   * @brief HTTPレスポンスの取得
   *
   * @return HTTPレスポンス
   */
   //--------------------------------------------------------------------------------------------
   const NexHttpResponse& GetHttpResponse( void );

public:
   //--------------------------------------------------------------------------------------------
   /*
   * 以下、ユーザーの利用禁止
   */
   //--------------------------------------------------------------------------------------------
   // HTTPリクエストヘッダの取得が完了した時に呼び出されます。
   virtual void ProcessResponseHeader(nn::nex::HttpConnection*, nn::nex::qInt statusCode);
   // HTTPレスポンスボディを取得した時に実行されます。
   virtual qBool ProcessResponse(const nn::nex::qUnsignedInt8* buffer, qUnsignedInt32 bufferSize);
   // HTTPリクエスト前に呼び出されます。パラメータ、ルートCA証明書のセットなどを行います。
   virtual void PrepareRequest(nn::nex::HttpConnection* httpConnection);
private:

   HttpConnectSetting m_setting;
};


GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(netapp)



#endif  // __NETAPP_NEX_HTTP_CONNECTION_H__
#endif