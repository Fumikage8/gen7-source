#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pConnectionWifi.h
 * @brief	P2Pプロセス：WIfi接続
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:28:01
 */
// =============================================================================

#ifndef __GFLNET2_P2P_CONNECTION_WIFI__
#define __GFLNET2_P2P_CONNECTION_WIFI__
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

class P2pConnectionWifi : public P2pConnectionInterface, public gflnet2::nex::NexMatchmakeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pConnectionWifi); //コピーコンストラクタ＋代入禁止

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
  gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList m_wifiMatchRuleList; ///< WiFiマッチング時の条件リスト
  gflnet2::nex::NexMatchMakingRule m_rule[ gflnet2::nex::NexMatchMakingRule::RULE_MAX_NUM ]; ///< WiFiマッチング時の条件リスト

  nn::nex::CallContext      m_context; ///< コンテキスト
  nn::nex::CallContext      m_natContext; ///< NAT用コンテキスト

  // ギャザリング
  nn::nex::GatheringHolder* m_gatheringHolder;
  u32                       m_gatheringListSize;
  // 各リスナーのフラグ
  bool m_isOnMatchmakingCreateSuccess;
  bool m_isOnMatchmakingSearchSuccess;
  bool m_isOnMatchmakingJoinSuccess;
  bool m_isOnMatchmakingOpenSessionSuccess;
  bool m_isOnMatchmakingUpdateHostSuccess;
  bool m_isMatchMakeFailed;

public:
  P2pConnectionWifi() :
    m_nexManager( NULL ),
    m_nexMatchMakeClient( NULL ),
    m_wifiMatchRuleList(),
    m_rule(),
    m_context(),
    m_natContext(),
    m_gatheringHolder( NULL ),
    m_gatheringListSize( 0 ),
    m_isOnMatchmakingCreateSuccess( false ),
    m_isOnMatchmakingSearchSuccess( false ),
    m_isOnMatchmakingJoinSuccess( false ),
    m_isOnMatchmakingOpenSessionSuccess( false ),
    m_isOnMatchmakingUpdateHostSuccess( false ),
    m_isMatchMakeFailed( false ),
    gflnet2::nex::NexMatchmakeListener()
  {
  };
  virtual ~P2pConnectionWifi(){};

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

  //------------------------------------------------------------------
  /**
   * @brief   マッチング条件を設定
   */
  //------------------------------------------------------------------
  void SetMatchRuleList( gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList* ruleList );

  // NexMatchmakeListenerの実装
  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションの作成完了通知
  *
  * @param[in] gatheringId 作成されたセッションのギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingCreateSuccess( const nn::nex::GatheringID& gatheringId );

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションの検索完了通知
  *
  * @param[in] gatheringList 検索されたセッションのギャザリングIDリスト
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingSearchSuccess( nn::nex::qList<nn::nex::GatheringHolder>* gatheringList );

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションへの参加完了通知
  *
  * @param[in] gatheringId 参加したギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingJoinSuccess( const nn::nex::GatheringID& gatheringId );

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションのオープン正常終了通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingOpenSessionSuccess( void );

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
  * @brief ホスト更新処理の成功通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingUpdateSessionHostSuccess( void );

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションのオーナー変更通知
  *
  * @param[in] newOwnerId 新たにオーナーとなったユーザーのプリンシバルID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingOwnerChange( nn::nex::PrincipalID newOwnerId );

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================

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
   * @brief   マッチメイクの開始
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool StartMatchmake( void );

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクへの参加
   * @return  実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool JoinMatchmake( void );

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

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクセッションの参加者のURLを取得（サンプルから移植）
   * @return  trueで成功
   */
  //------------------------------------------------------------------
  bool GetSessionURLs( nn::nex::qList<nn::nex::StationURL>& listURL );

};

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // __GFLNET2_P2P_CONNECTION_WIFI__
#endif // GF_PLATFORM_CTR
