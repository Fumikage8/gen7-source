//==============================================================================
/**
 * @file	P2pConnectionManagerTestProc.h
 * @brief	P2pConnectionManagerクラスのテスト
 * @author	ichiraku_katsuhiko
 * @date	2015/04/16 12:45:16
 */
// =============================================================================


#ifdef  GF_PLATFORM_CTR
#ifndef __P2P_CONNECTIONMANAGER_TEST_PROC__
#define __P2P_CONNECTIONMANAGER_TEST_PROC__
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

// debug
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

// draw
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

// P2pConnectionManager
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(P2pConnectionManagerTestProc)

class Proc : public GameSys::GameProc
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
    WIRELESS_AUTO,
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
	gfl2::heap::NwAllocator*		m_pMemAllocator;
	gfl2::heap::NwAllocator*		m_pDevAllocator;
  System::nijiAllocator*      m_pNijiAllocator;

	gfl2::util::UtilTextRenderingPipeLine*		m_pRenderingPipeLine;

	gfl2::util::DrawUtilText* m_pDrawUtilText;

  int m_selectP2pType;
  ENUM_MAIN_SEQ m_mainSeq;
  u8 m_subSeq;

  int m_sleepCount;
};

GFL_NAMESPACE_END(P2pConnectionManagerTestProc)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
#endif //__P2P_CONNECTIONMANAGER_TEST_PROC__
#endif // GF_PLATFORM_CTR