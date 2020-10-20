#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexError.h
 *
 * @brief  nn::nexのエラー管理クラス
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_ERROR_H__
#define __GFLNET_NEX_ERROR_H__

#pragma once

#include <nex.h>

#include <OnlineCore/src/Platform/Core/SystemErrorResult.h>


namespace gflnet2{
namespace nex{

/**
 * @brief NEXエラー変換用ユーティリティクラス
 */
class NexError
{
  private:
             NexError( void );
    virtual ~NexError( void );
  public:
    //------------------------------------------------------------------
    /**
     * @brief  プロトコルコールコンテキストのエラーをコンソール出力する（デバッグ用）
     *
     * @param[in]  context    ProtocolCallContext オブジェクト
     */
    //------------------------------------------------------------------
    static void ReportProtocolCallError( const nn::nex::ProtocolCallContext& context );

    //------------------------------------------------------------------
    /**
     * @brief  nn::Result のエラーからNEXのエラーコードを算出し、コンソール出力する（デバッグ用）
     *
     * @param[in]  result リザルトコード
     */
    //------------------------------------------------------------------
    static void ReportResultError( const nn::Result& result );

    //------------------------------------------------------------------
    /**
     * @brief  NEX API がエラーを返したときの詳細情報をコンソール出力する（デバッグ用）
     */
    //------------------------------------------------------------------
    static void ReportSystemError( void );

    //------------------------------------------------------------------
    /**
     * @brief  Nex関数がfalseを返却した際の通信エラーコードを取得する
     *
     * @return  通信エラーコード
     */
    //------------------------------------------------------------------
    static nn::nex::qUnsignedInt32 GetNetworkFunctionErrorCode( void );

    //------------------------------------------------------------------
    /**
     * @brief  nn::nex::qResult のエラーから通信エラーコードへ変換する
     *
     * @param[in]  result エラー情報
     *
     * @return  通信エラーコード
     */
    //------------------------------------------------------------------
    static nn::nex::qUnsignedInt32 GetNetworkErrorCode( const nn::nex::qResult& result );
};


} //nex
} //gflnet




#endif // __GFLNET_NEX_ERROR_H__
#endif
