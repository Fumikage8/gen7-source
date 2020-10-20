#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pConnectionWireless.h
 * @brief	P2Pプロセス：Wireless接続
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:33:38
 */
// =============================================================================
#ifndef __GFLNET2_P2P_CONNECTION_WIRELESS__
#define __GFLNET2_P2P_CONNECTION_WIRELESS__
#pragma once

#include "gflnet2/include/p2p/gflnet2_P2pConnectionInterface.h"
#include "gflnet2/include/beacon/gflnet2_PiaBeaconSender.h"
#include "gflnet2/source/p2p/thread/gflnet2_P2pAutoScanThread.h"
#include "gflnet2/source/p2p/thread/gflnet2_LocalAutoMatchingThread.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class P2pAutoScanThread;

class P2pConnectionWireless : public P2pConnectionInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pConnectionWireless); //コピーコンストラクタ＋代入禁止

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
private:
  
  enum
  {
    REGIST_GATHERING_TRY_COUNT_MAX = 30, ///< ギャザリング構築トライ最大回数
    CALLFAILURE_DOWNCOUNT = 300,         ///< CONTEXTのFAILUE許容回数
    CALLFAILURE_DOWNCOUNT_AUTOMATCH = 30         ///< CONTEXTのFAILUE許容回数
  };

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

private:

  // 親用
  gflnet2::wireless::PiaBeaconSender*   m_pBeaconSender; ///< ビーコンセンダー
  int                                   m_registerGatheringTryCount;  ///< ギャザリング構築試作回数

  // 子用
  gflnet2::p2p::P2pAutoScanThread*      m_thread;   ///< 起動スレッド
  nn::nex::ProtocolCallContext*         m_context;  ///< カレントコールコンテキスト
  int                                   m_contextFailureCount;  ///< ContextのFailure回数

  // 自動マッチング用
  gflnet2::p2p::LocalAutoMatchingThread* m_pAutoMatchingThread;

  //ビーコンデータ
  const void* m_pBeaconData;
  u32 m_beaconSize;
  u8 m_beaconGameID;
  u8 m_beaconModeID;

public:
  P2pConnectionWireless();
  virtual ~P2pConnectionWireless(){};

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================

  //------------------------------------------------------------------
  /**
    * @brief   ビーコン設定（ローカルの親のみ）
    */
  //------------------------------------------------------------------
  #ifdef  GF_PLATFORM_CTR
  void SetLocalP2PMasterBeacon( const void* pBeaconData , u32 beaconSize , u8 gameID , u8 modeID );
  #endif // GF_PLATFORM_CTR 

  //---------------------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   Manager  管理クラス
   * @return  RES_FINISHで終了
   */
  //---------------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* );
  //---------------------------------------------------------------------------------
  /**
   * @brief   実行処理
   * @param   Manager  管理クラス
   * @return  RES_FINISHで終了
   */
  //---------------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager*);
  //---------------------------------------------------------------------------------
  /**
   * @brief   終了処理
   * @param   Manager  管理クラス
   * @return  RES_FINISHで終了
   */
  //---------------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager*);


  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================

  //------------------------------------------------------------------
  /**
   * @brief   親更新
   * @return  trueで終了
   */
  //------------------------------------------------------------------
  bool UpdateParent( void );

  //------------------------------------------------------------------
  /**
   * @brief   子更新
   * @return  trueで終了
   */
  //------------------------------------------------------------------
  bool UpdateChild( void );

  //------------------------------------------------------------------
  /**
   * @brief   自動マッチング更新
   * @return  trueで終了
   */
  //------------------------------------------------------------------
  bool UpdateAutoMatching( void );

  //------------------------------------------------------------------
  /**
   * @brief   ビーコンスキャンスレッド作成
   */
  //------------------------------------------------------------------
  void BeaconScanThreadStart( void );

  //------------------------------------------------------------------
  /**
   * @brief   ビーコンスキャンスレッド終了待ち
   * @return  trueでスレッド終了
   */
  //------------------------------------------------------------------
  bool BeaconScanThreadEndCheck( void );

  //------------------------------------------------------------------
  /**
   * @brief   相手のSessionにJoin開始
   */
  //------------------------------------------------------------------
  void JoinSessionStart( void );

  //------------------------------------------------------------------
  /**
   * @brief   相手のSessionにJoin待ち
   * @return  trueで終了
   */
  //------------------------------------------------------------------
  bool CheckJoinSession( void );

};

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // __GFLNET2_P2P_CONNECTION_WIRELESS__
#endif // GF_PLATFORM_CTR
