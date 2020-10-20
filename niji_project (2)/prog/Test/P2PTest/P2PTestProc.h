//==============================================================================
/**
 * @file	P2PTestProc.h
 * @brief	P2Pテスト用Proc
 * @author	ichiraku_katsuhiko
 * @date	2015/03/10 15:21:30
 */
// =============================================================================

#ifdef  GF_PLATFORM_CTR
#ifndef __P2P_TEST_PROC__
#define __P2P_TEST_PROC__
#pragma once
#if PM_DEBUG

// gfl2
#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>
#include <gflnet2/include/p2p/gflnet2_P2pManager.h>
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>
#include <gflnet2/include/p2p/gflnet2_NetPlayer.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorListener.h"

// debug
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

// draw
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

// P2pManagerリスナー
#include <gflnet2/include/p2p/gflnet2_P2pManagerEventListener.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)

  
  GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(P2PTestProc)

class Proc : public GameSys::GameProc, public gflnet2::p2p::P2pManagerEventListener, public NetLib::Error::NijiNetworkErrorListener
{
  GFL_FORBID_COPY_AND_ASSIGN(Proc); //コピーコンストラクタ＋代入禁止

public:
  Proc(void);  //コンストラクタ
  virtual ~Proc(void);  //デストラクタ

  //-----------------------------------------------------------------------------
  /**
  *  定数
  */
  //-----------------------------------------------------------------------------

private:
  //-------------------------------------
  /// メインシーケンス定義
  //=====================================
  typedef enum {
    MAIN_SEQ_KEYWAIT = 0, // キー待ち状態
    MAIN_SEQ_P2P,         // P2P接続
    MAIN_SEQ_END,         // 終了

  } ENUM_MAIN_SEQ;

  //-------------------------------------
  /// 接続種別定義
  //=====================================
  enum{
    WIRELESS_PARENT,
    WIRELESS_CHILD,
    WIFI_PARENT,
    WIFI_CHILD,
    WIFI_AUTO,
    IR,
    NET_MAX
  };

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

public:
  //=============================================================================
  /**
  *  publicメソッド
  */
  //=============================================================================

  //------------------------------------------------------------------
  /**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
  * @brief	  PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //------------------------------------------------------------------
  /**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);


  //=============================================================================
  /**
  *  P2pManagerリスナー
  */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   P2P接続成功通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pManagerConnectSuccess();

  //------------------------------------------------------------------
  /**
   * @brief   P2P切断成功通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pManagerDisconnectSuccess();

  //------------------------------------------------------------------
  /**
  * @brief P2pManager通信のエラー通知
  */
  //------------------------------------------------------------------
  virtual void OnP2pManagerError();

  //------------------------------------------------------------------
  /**
  * @brief P2pManager通信のエラー通知
  *
  * @param[in] result     実行結果
  * @param[in] errorCode  エラーアプレット用エラーコード
  */
  //------------------------------------------------------------------
  virtual void OnP2pManagerError( const nn::nex::qResult& result, u32 errorCode );

  //------------------------------------------------------------------
  /**
   * @brief P2P切断通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pManagerSessionLeft();

  //------------------------------------------------------------------
  /**
   * @brief マッチメイクエラー通知（独自ハンドリング版）
   */
  //------------------------------------------------------------------
  virtual void OnP2pManagerMatchMakeFailed();

  //------------------------------------------------------------------
  /**
  * @brief 内部で確保されたヒープメモリが枯渇したことを通知
  */
  //------------------------------------------------------------------
  virtual void OnP2pManagerErrorOutOfMemory(){};

  //------------------------------------------------------------------
  /**
   * @brief マイグレーション成功
   */
  //------------------------------------------------------------------
  virtual void OnP2pManagerMigrationSuccess(){};

  //------------------------------------------------------------------
  /**
  * @brief 通信エラー通知
  */
  //------------------------------------------------------------------
  virtual void OnNijiNetworkError(){}

private:
  //=============================================================================
  /**
  *  privateメソッド
  */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief 文字描画
   */
  //------------------------------------------------------------------
  void DrawText( void );

  // シーケンス関数
  void MainSeq_KeyWait();
  void MainSeq_P2P();

  // メインシーケンス設定
  void SetMainSeq( ENUM_MAIN_SEQ seq ){ m_mainSeq = seq; m_subSeq = 0; }
  // シーケンス実行
  bool DoSeqProcess( void );

private:

  // P2P初期化
  void P2pInitialize();
  // P2P接続開始
  bool P2pConnectStart();
  // P2P破棄
  void P2pFinalize();

private:
	gfl2::heap::NwAllocator*		m_pMemAllocator;
	gfl2::heap::NwAllocator*		m_pDevAllocator;
  System::nijiAllocator*      m_pNijiAllocator;

	gfl2::util::UtilTextRenderingPipeLine*		m_pRenderingPipeLine;

	gfl2::util::DrawUtilText* m_pDrawUtilText;

  int m_selectP2pType;
  ENUM_MAIN_SEQ m_mainSeq;
  u8 m_subSeq;

  // P2P系
  gflnet2::p2p::P2pManager* m_pP2pManager;  ///< P2P管理クラス
  gflnet2::base::BeaconManager* m_pBeaconManager; ///< ビーコンマネージャ
  gflnet2::p2p::NetGame*    m_pNetGame;     ///< 送受信管理クラス

  bool m_isConnectSuccess;
  bool m_isDisconnectSuccess;
};

GFL_NAMESPACE_END(P2PTestProc)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
#endif //__P2P_TEST_PROC__
#endif // GF_PLATFORM_CTR