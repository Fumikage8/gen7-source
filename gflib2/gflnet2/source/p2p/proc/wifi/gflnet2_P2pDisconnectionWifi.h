#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pDisconnectionWifi.h
 * @brief	P2Pプロセス：WIfi切断
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:30:21
 */
// =============================================================================

#ifndef __GFLNET2_P2P_DISCONNECTION_WIFI__
#define __GFLNET2_P2P_DISCONNECTION_WIFI__
#pragma once

#include <nex_P2p.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/nex/gflnet2_NexMatchmakeListener.h>

#include "gflnet2/include/p2p/gflnet2_P2pDisconnectionInterface.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class P2pDisconnectionWifi : public P2pDisconnectionInterface, public gflnet2::nex::NexMatchmakeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pDisconnectionWifi); //コピーコンストラクタ＋代入禁止

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

private:
  gflnet2::nex::NexManager* m_nexManager;
  gflnet2::nex::NexMatchMakingClient* m_nexMatchMakeClient;   ///< マッチメイククライアント
  bool  m_isOnMatchmakingEndSuccess;  ///< リスナーのフラグ

public:
  P2pDisconnectionWifi() :
    m_nexManager( NULL ),
    m_nexMatchMakeClient( NULL ),
    m_isOnMatchmakingEndSuccess( false ),
    gflnet2::nex::NexMatchmakeListener()
  {
  };
  virtual ~P2pDisconnectionWifi(){};

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
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

  // NexMatchmakeListenerの実装
  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションの正常終了通知。
  *        自身がマッチメイクセッションから正常離脱した旨を検知できる。
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingEndSuccess( void );

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションのエラー終了通知
  *
  * @param[in] result エラー理由
  * @param[in] result エラーアプレットに渡されるエラーコード
  *
  * @par  下記関数のエラー時に発行される。
  *       - NexMatchMakingClient::DeleteParticipationAsync()
  *       - NexMatchMakingClient::EndParticipationAsync()
  */
  //------------------------------------------------------------------
  virtual void OnNexMatchmakeError( const nn::nex::qResult& result, u32 errorCode );


  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   NATトラバーサルのUnbind
   */
  //------------------------------------------------------------------
  void UnbindNatTraversal( void );

  //------------------------------------------------------------------
  /**
   * @brief   NATトラバーサルの終了
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool EndNatTraversal( void );

  //------------------------------------------------------------------
  /**
   * @brief   NetZの削除
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool DeleteNetZ( void );

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクのキャンセルリクエスト発行
   */
  //------------------------------------------------------------------
  void CancelMatchmake( void );

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクのキャンセル待ち
   */
  //------------------------------------------------------------------
  bool IsCancelMatchmake( void );

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクの終了
   */
  //------------------------------------------------------------------
  void EndMatchmake( void );

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクの終了完了待ち
   */
  //------------------------------------------------------------------
  bool IsEndMatchmake( void );

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクの削除
   */
  //------------------------------------------------------------------
  void DeleteMatchmake( void );

};

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // __GFLNET2_P2P_DISCONNECTION_WIFI__
#endif // GF_PLATFORM_CTR