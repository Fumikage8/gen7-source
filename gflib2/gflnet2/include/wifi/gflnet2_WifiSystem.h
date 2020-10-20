#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_WifiSystem.h
 * @brief  nn::acのラッパークラス。Wifiアクセスポイントを管理する
 * @author h.suzuki
 * @date   2011.10.20
 */
//=============================================================================

#ifndef __GFLNET_WIFI_SYSTEM_H__
#define __GFLNET_WIFI_SYSTEM_H__
#pragma once

#include <nn/init.h>
#include <nn.h>
#include <nn/http.h>
#include <nn/ac.h>
#include <nn/ac/ac_Result.h>
//#include <ro/gfl_RoManager.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/wifi/gflnet2_WifiConnectListener.h>
#include <gflnet2/include/wifi/gflnet2_WifiDisconnectListener.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <base/include/gfl2_Singleton.h>

namespace gflnet2{

/**
 * @namespace gflnet2::wifi
 * @brief WIFIアクセスポイント接続関連
 */
namespace wifi{


class WifiConnectThread;

/**
 * @brief WIFIアクセスポイント接続管理用クラス
 *
 * @note  本関数はシングルトンインスタンスです。
 */
class WifiSystem : public gfl2::base::Singleton<WifiSystem>
{
  GFL_FORBID_COPY_AND_ASSIGN(WifiSystem); //コピーコンストラクタ＋代入禁止

public:
  static const u32 EULA_DISPLAY_ERROR_CODE       = 30399; //!< EULAエラー時に表示させるネットワークエラーコード(ガイドライン指定）
  static const u32 DISCONNECT_DISPLAY_ERROR_CODE = 34099; //!< AP切断時のネットワークエラーコード

public:
  //------------------------------------------------------------------
  /**
   * @brief   WifiSystemコンストラクタ
   */
  //------------------------------------------------------------------
  WifiSystem();

  //------------------------------------------------------------------
  /**
   * @brief   WifiSystemデストラクタ
   */
  //------------------------------------------------------------------
  virtual ~WifiSystem();


public:

  
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   heap 内部スレッドを生成する際に使用されるヒープ。LOWから確保される。
   */
  //------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase *heap );

  //------------------------------------------------------------------
  /**
   * @brief   終了
   */
  //------------------------------------------------------------------
  void Terminate( void );

  //------------------------------------------------------------------
  /**
   * @brief  WifiSystemの初期化する
   *
   * @note   事前にfsを初期化する必要があります！
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result InitializeAC( void );

  //------------------------------------------------------------------
  /**
   * @brief  ＡＰへの接続を開始する(同期接続）
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result Connect( void );

  //------------------------------------------------------------------
  /**
   * @brief  ＡＰへの接続を終了する
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result Close( void );

  //------------------------------------------------------------------
  /**
   * @brief  WifiSystemの終了処理
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result FinalizeAC( void );

  //------------------------------------------------------------------
  /**
   * @brief 更新処理。ポーリングしてください。
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief 接続を確認する
   *
   * @return 接続中ならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsConnect( void );

  //------------------------------------------------------------------
  /**
   * @brief ＡＰ不正切断通知用リスナを登録する
   *
   * @param[in,out] listener 通知先のリスナ。NULLを指定することで登録を解除できる。
   */
  //------------------------------------------------------------------
  void SetDisconnectingListener( WifiDisconnectListener* listener );

public:
  //------------------------------------------------------------------
  /**
   * @brief  スレッドにて非同期で[Initialize(), Connect()]を実行する
   *
   * @attention 通信終了イベント通知後、必ずDeleteAsync()をコールしてください。
   *
   * @param[in]  listener          接続結果通知用イベントリスナ
   * @param[in]  thread_priority   スレッドの実行優先度
   *
   * @retval true  スレッドの開始に成功
   * @retval false スレッドの開始に失敗
   */
  //------------------------------------------------------------------
  bool StartWifiASync( WifiConnectListener *listener, int thread_priority );

  //------------------------------------------------------------------
  /**
   * @brief  スレッドにて非同期で[Close(), Finalize()]を実行する
   *
   * @attention 通信終了イベント通知後、必ずDeleteAsync()をコールしてください。
   *
   * @param[in]  listener          切断結果通知用イベントリスナ
   * @param[in]  thread_priority   スレッドの実行優先度
   *
   * @retval true  スレッドの開始に成功
   * @retval false スレッドの開始に失敗
   */
  //------------------------------------------------------------------
  bool EndWifiASync( WifiConnectListener *listener, int thread_priority );

  //------------------------------------------------------------------
  /**
   * @brief 非同期処理スレッドを破棄する
   */
  //------------------------------------------------------------------
  void DeleteAsync( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief  最終エラーコードを取得する(デバッグ関数)
   *
   * @param[out]  errorCode エラーコード格納先
   *
   * @return  エラーコード取得結果
   */
  //------------------------------------------------------------------
  static nn::Result GetLastErrorCode( u32* errorCode );

  //------------------------------------------------------------------
  /**
   * @brief  エラー内容をコンソールに出力する(デバッグ関数)
   *
   * @param[in]  result 出力対象となるnn::Result
   */
  //------------------------------------------------------------------
  static void PrintError( const nn::Result &result );

  //------------------------------------------------------------------
  /**
   * @brief  ネットワーク接続要求のレベルを指定(デバッグ関数)
   *
   * @param[in]  result 出力対象となるnn::Result
   */
  //------------------------------------------------------------------
  nn::Result DebugSetNetworkArea( nn::ac::CTR::NetworkArea netArea );

private:
  nn::ac::Config                     m_config;            //!< 接続設定
  nn::os::Event                      m_event;             //!< 切断検知用イベント
  gflnet2::wifi::WifiConnectThread*   m_thread;            //!< 接続処理用スレッド
  gfl2::heap::HeapBase*               m_heapBase;          //!< ヒープベース
  bool                               m_isConnect;         //!< 接続フラグ
  WifiDisconnectListener*            m_disconnetListener; //!< 切断通知リスナ
  nn::os::CriticalSection            m_criticalSection;   //!< クリティカルセクション

  static const u32 THREAD_STACK_SIZE = 8192;              //!< スレッドスタック用サイズ

private:
  //-------------------------------------------------------------------------------
  /**
   * @brief 非同期通信が実行中かどうかを調べる
   *
   * @retval true   実行中
   * @retval false  実行していない
   */
  //-------------------------------------------------------------------------------
  bool CheckAsync( void ) const;

  //-------------------------------------------------------------------------------
  /**
   * @brief 実行中の非同期通信をキャンセルする
   *
   * @note CheckAsync()で非同期処理の終了を確認した後にEndAsync()を呼んでください。
   */
  //-------------------------------------------------------------------------------
  void CancelAsync( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief スレッドの終了待ちを行う
   */
  //-------------------------------------------------------------------------------
  void WaitAsyncFinished( void );
};



}   // wifi
}   // gflnet

#endif // __GFLNET_WIFI_SYSTEM_H__

#endif
