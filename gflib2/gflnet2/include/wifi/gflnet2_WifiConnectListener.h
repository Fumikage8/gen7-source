#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_WifiConnectListener.h
 * @brief  Wifiアクセスポイント接続関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_WIFI_CONNECT_LISTENER_H__
#define __GFLNET_WIFI_CONNECT_LISTENER_H__
#pragma once

#include <nn.h>


namespace gflnet2{
namespace wifi{

/*!
 * @brief WIFIアクセスポイント接続関連通知イベントハンドラ
 *
 * @note  WIFI接続関連の通知を受け取る場合は、本クラスを継承し仮想関数を実装してください。
 */
class WifiConnectListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(WifiConnectListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
   * @brief WIFI接続の成功通知
   *
   * @note  イベントはスレッド内からコールされるため
   * @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
   */
  //------------------------------------------------------------------
   virtual void OnWifiConnectSuccess( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief WIFI接続の失敗通知
   *
   * @note  イベントはスレッド内からコールされるため
   * @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
   *
   * @param[in] result    実行結果
   * @param[in] errorCode 発生した通信エラーコード
   */
  //------------------------------------------------------------------
   virtual void OnWifiConnectFailed( const nn::Result& result, u32 errorCode ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief WIFI切断の成功通知
   *
   * @note  イベントはスレッド内からコールされるため
   * @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
   */
  //------------------------------------------------------------------
   virtual void OnWifiDisconnectSuccess( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief WIFI切断の失敗通知
   *
   * @note  イベントはスレッド内からコールされるため
   * @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
   *
   * @param[in] result    実行結果
   * @param[in] errorCode 発生した通信エラーコード
   */
  //------------------------------------------------------------------
   virtual void OnWifiDisconnecFailed( const nn::Result& result, u32 errorCode ) = 0;
};


} // wifi
} // gflnet



#endif // __GFLNET_WIFI_CONNECT_LISTENER_H__
#endif
