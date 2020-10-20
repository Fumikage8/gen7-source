﻿//==============================================================================
/**
 * @file	LocalQuickMatchTest.h
 * @brief	LocalQuickMatchTestProc
 */
// =============================================================================

#ifdef  GF_PLATFORM_CTR
#ifndef __LOCAL_QUICK_MATCH_TEST__
#define __LOCAL_QUICK_MATCH_TEST__
#pragma once
#if PM_DEBUG

// gfl2
#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>
#include <gflnet2/include/p2p/gflnet2_P2pManager.h>
#include <gflnet2/include/p2p/gflnet2_P2pManagerEventListener.h>
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>
#include <gflnet2/include/p2p/gflnet2_NetPlayer.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>

// debug
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

// draw
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

#include <ExtSavedata/include/BattleRecorderSaveData.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(LocalQuickMatchTest)

class LocalQuickMatchTest : public GameSys::GameProc , public gflnet2::p2p::P2pManagerEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN(LocalQuickMatchTest); //コピーコンストラクタ＋代入禁止

  enum
  {
    MODE_NONE = 0,
    MODE_AUTO_MATCH,
    MODE_AUTO_MATCH_CONNECTED,
    MODE_AUTO_DISCONNECT,
  };

public:
  LocalQuickMatchTest(void);  //コンストラクタ
  virtual ~LocalQuickMatchTest(void);  //デストラクタ

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

public:

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
  virtual void OnP2pManagerError(){};

  //------------------------------------------------------------------
  /**
  * @brief P2pManager通信のエラー通知
  *
  * @param[in] result     実行結果
  * @param[in] errorCode  エラーアプレット用エラーコード
  */
  //------------------------------------------------------------------
  virtual void OnP2pManagerError( const nn::nex::qResult& result, u32 errorCode ){};

  //------------------------------------------------------------------
  /**
   * @brief P2P切断通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pManagerSessionLeft(){};

  //------------------------------------------------------------------
  /**
   * @brief マッチメイクエラー通知（独自ハンドリング版）
   */
  //------------------------------------------------------------------
  virtual void OnP2pManagerMatchMakeFailed(){};

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

private:
  //------------------------------------------------------------------
  /**
   * @brief 文字描画
   */
  //------------------------------------------------------------------
  void DrawText( void );

  //p2p初期化
  void P2pInitialize( u32 connectNum );


	gfl2::heap::NwAllocator*		m_pMemAllocator;
	gfl2::heap::NwAllocator*		m_pDevAllocator;
  System::nijiAllocator*      m_pNijiAllocator;

	gfl2::util::UtilTextRenderingPipeLine*		m_pRenderingPipeLine;

	gfl2::util::DrawUtilText* m_pDrawUtilText;

  // P2P系
  gflnet2::p2p::P2pManager* m_pP2pManager;  ///< P2P管理クラス
  gflnet2::base::BeaconManager* m_pBeaconManager; ///< ビーコンマネージャ
  gflnet2::p2p::NetGame*    m_pNetGame;     ///< 送受信管理クラス

  bool m_isConnectSuccess;
  bool m_isDisconnectSuccess;

  int m_mode;

  u64 m_waitTime;
  u64 m_DispTime;
};

GFL_NAMESPACE_END(LocalQuickMatchTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
#endif //__LOCAL_QUICK_MATCH_TEST__
#endif // GF_PLATFORM_CTR