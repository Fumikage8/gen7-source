#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pSimpleConnectionWifi.h
 * @brief	P2Pプロセス：WIfi接続（シンプルマッチメイク版）ミラクル交換専用
 * @author	ichiraku_katsuhiko
 * @date	2016/01/20　23:05
 */
// =============================================================================

#ifndef __GFLNET2_P2P_SIMPLE_CONNECTION_WIFI__
#define __GFLNET2_P2P_SIMPLE_CONNECTION_WIFI__
#pragma once

#include <nex_P2p.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/nex/gflnet2_NexMatchmakeListener.h>

#include "gflnet2/include/p2p/gflnet2_P2pConnectionInterface.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class P2pSimpleConnectionWifi : public P2pConnectionInterface, public gflnet2::nex::NexMatchmakeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pSimpleConnectionWifi); //コピーコンストラクタ＋代入禁止

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
private:
  // ディスパッチの待ち時間
  static const qUnsignedInt32 SAMPLE_DISPATCH_TIMEOUT = 3;

  //-------------------------------------
  /// 処理状態定義
  //=====================================
  typedef enum{
    E_STATUS_PROGRESS,  ///< 実行中
    E_STATUS_SUCCESS,   ///< 成功
    E_STATUS_FAILURE,   ///< 失敗
  }E_STATUS;

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
private:
  gflnet2::nex::NexManager* m_nexManager;
  gflnet2::nex::NexMatchMakingClient* m_nexMatchMakeClient;   ///< マッチメイククライアント

  nn::nex::CallContext      m_context; ///< コンテキスト
  nn::nex::CallContext      m_natContext; ///< NAT用コンテキスト

  // 各リスナーのフラグ
  bool m_isOnMatchmakingSimpleMatchmakeSuccess;
  bool m_isOnMatchmakingEntrySimpleMatchmakeSuccess;
  bool m_isOnMatchmakingCancelSimpleMatchmakeSuccess;
  bool m_isMatchMakeFailed;
  bool m_isCancelSimpleMatchmake; //!< マッチメイクキャンセルするかどうか
  bool m_isMatchmakeError;        //!< GFNMCat[857]：エラーをポーリングする必要があった。

public:
  P2pSimpleConnectionWifi() :
    m_nexManager( NULL ),
    m_nexMatchMakeClient( NULL ),
    m_context(),
    m_natContext(),
    m_isOnMatchmakingSimpleMatchmakeSuccess( false ),
    m_isOnMatchmakingEntrySimpleMatchmakeSuccess( false ),
    m_isOnMatchmakingCancelSimpleMatchmakeSuccess( false ),
    m_isMatchMakeFailed( false ),
    m_isCancelSimpleMatchmake( false ),
    m_isMatchmakeError(false),
    gflnet2::nex::NexMatchmakeListener()
  {
  };
  virtual ~P2pSimpleConnectionWifi(){};

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
  * @brief マッチメイクセッションのエラー終了通知
  *
  * @param[in] result エラー理由
  * @param[in] result エラーアプレットに渡されるエラーコード
  *
  * @par  下記関数のエラー時に発行される。
  *       - NexMatchMakingClient::CreateSession()
  *       - NexMatchMakingClient::SearchSession()
  *       - NexMatchMakingClient::AutoMatchmake()
  */
  //------------------------------------------------------------------
  virtual void OnNexMatchmakeError( const nn::nex::qResult& result, u32 errorCode );

  //------------------------------------------------------------------
  /**
  * @brief SimpleMatchmake処理の成功通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingSimpleMatchmakeSuccess( void );

  //------------------------------------------------------------------
  /**
  * @brief EntrySimpleMatchmake処理の成功通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingEntrySimpleMatchmakeSuccess( void );

  //------------------------------------------------------------------
  /**
  * @brief CancelSimpleMatchmakeEntry処理の成功通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingCancelSimpleMatchmakeEntrySuccess( void );

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   SimpleMatchmake実行
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool SimpleMatchmake( void );

  //------------------------------------------------------------------
  /**
   * @brief   EntrySimpleMatchmake実行
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool EntrySimpleMatchmake( void );

  //------------------------------------------------------------------
  /**
   * @brief   CancelSimpleMatchmakeEntry実行
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool CancelSimpleMatchmakeEntry( void );

  //------------------------------------------------------------------
  /**
   * @brief   NATトラバーサルのBind
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool BindNatTraversal( void );

  //------------------------------------------------------------------
  /**
   * @brief   NATトラバーサルの開始
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool StartNatTraversal( void );

  //------------------------------------------------------------------
  /**
   * @brief   Natトラバーサルの開始待ち
   * @return  E_STATUS定義
   */
  //------------------------------------------------------------------
  E_STATUS WaitNatTraversal( void );

  //------------------------------------------------------------------
  /**
   * @brief   NetZの作成
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool CreateNetZ( void );

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクの作成
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool CreateMatchmake( void );

  //------------------------------------------------------------------
  /**
   * @brief   P2Pセッションを作成する（親）
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool CreateSession( void );

  //------------------------------------------------------------------
  /**
   * @brief   P2Pセッションに参加する（子）
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool JoinSession( void );

  //------------------------------------------------------------------
  /**
   * @brief   P2Pセッションに参加待ち（子）
   * @return  E_STATUS_SUCCESSでセッションに参加成功した
   */
  //------------------------------------------------------------------
  E_STATUS IsJoinSession( void );

};

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // __GFLNET2_P2P_SIMPLE_CONNECTION_WIFI__
#endif // GF_PLATFORM_CTR
