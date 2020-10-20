#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_WifiDisconnectListener.h
 * @brief  Wifiアクセスポイント切断通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_WIFI_DISCONNECT_LISTENER_H__
#define __GFLNET_WIFI_DISCONNECT_LISTENER_H__
#pragma once

#include <nn.h>


namespace gflnet2{
namespace wifi{

/*!
 * @brief WIFIアクセスポイント接続関連通知イベントハンドラ
 *
 * @note WIFIの切断通知を受け取る場合は、本クラスを継承し仮想関数を実装してください。
 */
class WifiDisconnectListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(WifiDisconnectListener); //コピーコンストラクタ＋代入禁止
public:
  //------------------------------------------------------------------
  /**
  * @brief 正規手続きを踏まないWIFI切断の通知
  *
  * @note  スレッドセーフ関数です
  *
  * @param[in] errorCode 最後に通知されたネットワークエラーコード
  *
  * @note WifiSystem::Close()を経ないＡＰ切断が行われた際に通知される
  */
  //------------------------------------------------------------------
  virtual void OnWifiDisconnected( u32 errorCode ) = 0;

};


} // wifi
} // gflnet



#endif // __GFLNET_WIFI_DISCONNECT_LISTENER_H__
#endif
