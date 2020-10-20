#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_WifiConnectThread.h
 * @author suzuki_hisanao
 */
//=============================================================================

#ifndef __GFLNET_WIFI_CONNECT_THREAD_H__
#define __GFLNET_WIFI_CONNECT_THREAD_H__
#pragma once

#include <nn.h>
#include <nn/ngc.h>
#include <gflnet2/include/wifi/gflnet2_WifiConnectListener.h>
#include <gflnet2/include/wifi/gflnet2_WifiSystem.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

namespace gflnet2 {
namespace wifi {



class WifiConnectListener;


/**
 * @brief  アクセスポイントへの接続・切断用スレッドクラス。<p>
 *         アクセスポイントへの接続および切断を非同期で行う為のスレッドクラスです。
 */
class WifiConnectThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(WifiConnectThread); //コピーコンストラクタ＋代入禁止

public:
  enum EXEC_MODE
  {
    E_CONNECT,          // 接続処理を実行
    E_DISCONNECT,       // 切断処理を実行
  };

private:
  EXEC_MODE                m_execMode;         //!< 動作モード
  WifiConnectListener*     m_listener;         //!< イベントリスナ
  WifiSystem*              m_wifiSys;          //!< WifiSystem
  nn::os::CriticalSection  m_criticalSection;  //!< クリティカルセクション
  gfl2::thread::Thread* m_pThread;

public:
  //--------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param[in] sys            WifiSystemへのポインタ
   * @param[in] heap           スレッドのスタック領域に使用するヒープ
   * @param[in] stack_size     スレッドのスタック領域のサイズ
   * @param[in] mode           動作モード
   * @param[in] listener       完了通知用リスナ
   */
  //--------------------------------------------------------------------------------
  WifiConnectThread( WifiSystem* sys,  gfl2::heap::HeapBase* heap, u32 stack_size, EXEC_MODE mode, WifiConnectListener *listener );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~WifiConnectThread();


  //--------------------------------------------------------------------------------
  /**
   * @brief 開始
   */
  //--------------------------------------------------------------------------------
  void Start();


  //--------------------------------------------------------------------------------
  /**
   * @brief 通知リスナの解除
   */
  //--------------------------------------------------------------------------------
  void RemoveListener( void );


public:

  void Kill( void ){ m_pThread->Kill(); };
  bool IsAlive( void ){ return m_pThread->IsAlive(); };

private:
  //--------------------------------------------------------------------------------
  /**
   * @brief WIFI接続処理
   */
  //--------------------------------------------------------------------------------
  void ConnectWifi( void );

  //--------------------------------------------------------------------------------
  /**
   * @brief WIFI切断処理
   */
  //--------------------------------------------------------------------------------
  void DisconnectWifi( void );

  //--------------------------------------------------------------------------------
  /**
   * @brief 接続成功通知
   */
  //--------------------------------------------------------------------------------
  void ReportConnectSuccess( void );

  //--------------------------------------------------------------------------------
  /**
   * @brief 切断成功通知
   */
  //--------------------------------------------------------------------------------
  void ReportDisconnectSuccess( void );

  //--------------------------------------------------------------------------------
  /**
   * @brief WIFI接続エラー処理
   *
   * @param[in] result 実行結果情報
   */
  //--------------------------------------------------------------------------------
  void ReportConnectingError( const nn::Result& result );

  //--------------------------------------------------------------------------------
  /**
   * @brief WIFI切断エラー処理
   *
   * @param[in] result 実行結果情報
   */
  //--------------------------------------------------------------------------------
  void ReportDisconnectingError( const nn::Result& result );

private:

  virtual void Function( void );
};




} // wifi
} // gflnet


#endif  // __GFLNET_WIFI_CONNECT_THREAD_H__

#endif
