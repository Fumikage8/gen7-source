//==============================================================================
/**
 * @file	NijiNetworkErrorDetectDefault.h
 * @brief	Niji通信エラーリスナーの検知クラス（デフォルト）
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 16:46:54
 */
// =============================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef __NIJI_NETWORK_ERROR_DETECT_DEFAULT_H__
#define __NIJI_NETWORK_ERROR_DETECT_DEFAULT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

// 各エラーリスナー
#include <gflnet2/include/wifi/gflnet2_WifiDisconnectListener.h>
#include <gflnet2/include/p2p/gflnet2_NetGameErrorListener.h>
#include <gflnet2/include/nex/gflnet2_NexErrorListener.h>
#include <gflnet2/include/dev/gflnet2_IrAdapterErrorListener.h>
#include "NetStatic/NetLib/include/P2P/P2pConnectionErrorListener.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkError.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "ExtSavedata/include/NijiExtSaveSystemListener.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   Niji通信エラー検知クラス
 *
 * @note    通信エラーのリスナーがある際はここに継承していく。
 *          但し、エラー以外の通知があるリスナーはその限りではない。
 */
//------------------------------------------------------------------
class NijiNetworkErrorDetectDefault : 
  public gflnet2::wifi::WifiDisconnectListener,
  public gflnet2::p2p::NetGameErrorListener,
  public gflnet2::nex::NexErrorListener,
  public gflnet2::dev::IrAdapterErrorListener,
  public NetLib::P2P::P2pConnectionErrorListener,
  public ExtSavedata::NijiExtSaveSystemListener
{
  GFL_FORBID_COPY_AND_ASSIGN(NijiNetworkErrorDetectDefault);
public:
  // コンストラクタ
  NijiNetworkErrorDetectDefault( NetLib::Error::NijiNetworkErrorManager* errorManager );
  // デストラクタ
  virtual ~NijiNetworkErrorDetectDefault();

  //=============================================================================
  /**
   *  WifiDisconnectListenerのリスナー
   */
  //=============================================================================
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
  virtual void OnWifiDisconnected( u32 errorCode );


  //=============================================================================
  /**
   *  NetGameErrorListenerのリスナー
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief P2P切断通知
   */
  //------------------------------------------------------------------
  virtual void OnNetGameSessionLeft();


  //=============================================================================
  /**
   *  NexErrorListenerのリスナー
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
  * @brief NEX内部で確保されたヒープメモリが枯渇したことを通知
  */
  //------------------------------------------------------------------
  virtual void OnNexErrorOutOfMemory( void );

  //------------------------------------------------------------------
  /**
  * @brief NEX内部のエラー通知
  *
  * @param[in] result     エラー値
  */
  //------------------------------------------------------------------
  virtual void OnNexErrorHandling( const nn::nex::qResult& result );

  //------------------------------------------------------------------
  /**
   * @brief ゲームサーバログイン後、ゲームサーバとの接続が切断されたことを通知
   */
  //------------------------------------------------------------------
  virtual void OnNexErrorGameServerShutdown( void );


  //=============================================================================
  /**
   *  IrAdapterErrorListenerのリスナー
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief 赤外線壊れている
   */
  //------------------------------------------------------------------
  virtual void OnIrAdapterErrorIrBroken();

  //------------------------------------------------------------------
  /**
   * @brief 繋いだタイプが違う
   */
  //------------------------------------------------------------------
  virtual void OnIrAdapterErrorMissmatch();

  //------------------------------------------------------------------
  /**
   * @brief IrAdapterの内部エラー
   */
  //------------------------------------------------------------------
  virtual void OnIrAdapterErrorHandling();


  //=============================================================================
  /**
   *  P2pConnectionErrorListenerのリスナー
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief システムエラー発生通知（ライブラリの失敗通知など）
   */
  //------------------------------------------------------------------
  virtual void OnP2pConnectionErrorSystem();

  //------------------------------------------------------------------
  /**
  * @brief Nexエラー発生通知
  *
  * @param[in] errorCode  エラーアプレット用エラーコード
  */
  //------------------------------------------------------------------
  virtual void OnP2pConnectionErrorNex( u32 errorCode );

  //------------------------------------------------------------------
  /**
   * @brief P2P切断通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pConnectionErrorSessionLeft();

  //------------------------------------------------------------------
  /**
   * @brief マッチメイクエラー通知（独自ハンドリング版）
   * 
   * @note  エラーアプレットは表示しない。
   */
  //------------------------------------------------------------------
  virtual void OnP2pConnectionErrorMatchMakeFailed();

  //------------------------------------------------------------------
  /**
  * @brief 内部で確保されたヒープメモリが枯渇したことを通知
  */
  //------------------------------------------------------------------
  virtual void OnP2pConnectionErrorOutOfMemory();

  //------------------------------------------------------------------
  /**
   * @brief タイムアウト発生通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pConnectionErrorTimeout();

  //------------------------------------------------------------------
  /**
   * @brief 拡張セーブのフェイタルエラー
   */
  //------------------------------------------------------------------
  virtual void OnExtSaveFatalError();

 protected:
   NetLib::Error::NijiNetworkError           m_error;        ///< エラー情報
   NetLib::Error::NijiNetworkErrorManager*   m_errorManager; ///< エラーマネージャ

};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_ERROR_DETECT_DEFAULT_H__
#endif // GF_PLATFORM_CTR
