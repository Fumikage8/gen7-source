//==============================================================================
/**
 * @file	P2pConnectionManager.h
 * @brief	P2P通信接続マネージャー
 * @author	ichiraku_katsuhiko
 * @date	2015/04/01 19:39:50
 *
 *
 * @note  使用方法
 *
 *        1.Initialize実行
 *
 *          ～接続～
 *        2.ローカル通信の場合は、ConnectStart
 *          Wi-Fi通信の場合は、ConnectStartWifiを実行
 *        3.IsConnectSuccessで接続完了を待つ
 *
 *          ～切断～
 *        4.DisconnectStartを実行
 *        5.IsDisconnectSuccessで切断完了を待つ
 *
 *          ここでまた2をやり接続しても良いです。
 *
 *        6.Terminate実行
 *
 */
// =============================================================================
#ifndef __P2P_CONNECTION_MANAGER__
#define __P2P_CONNECTION_MANAGER__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>

#include "NetStatic/NetLib/include/NetDefine.h"
#include <gflnet2/include/p2p/gflnet2_P2pManager.h>
#include <gflnet2/include/p2p/gflnet2_P2pManagerEventListener.h> // P2pManagerリスナー
#include <gflnet2/include/p2p/gflnet2_NetGame.h>

// タイムアウト計測
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"

#include "../../source/P2P/NexRatingClient.h" // 非公開ヘッダ
#include "../../source/P2P/NexRatingClientListener.h" // 非公開ヘッダ
#include "../../source/P2P/P2pTimeout.h" // 非公開ヘッダ
#include "../../source/P2P/P2pTimeoutListener.h" // 非公開ヘッダ
#include "../../source/P2P/P2pCommonData.h" // 非公開ヘッダ


// 前方宣言
namespace NetLib{
  namespace P2P{
    class P2pConnectionErrorListener;
    class P2pConnectionMigrationListener;
  }
}
namespace gflnet2{
  namespace base{
    class BeaconManager;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)

//-----------------------------------------------------------------------------
//  ゲームごとのP2Pサービス定義
//-----------------------------------------------------------------------------
typedef enum{
  // ジョインフェスタ
  NIJI_P2P_JF_QUICK_2   = 1,    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
  NIJI_P2P_JF_QUICK_4   = 2,    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
  NIJI_P2P_BATTLE_2_L   = 3,    ///< バトル（2人接続）（ローカル）
  NIJI_P2P_BATTLE_2_W   = 4,    ///< バトル（2人接続）（WiFi）
  NIJI_P2P_BATTLE_4_L   = 5,    ///< バトル（4人接続）（ローカル）
  NIJI_P2P_BATTLE_4_W   = 6,    ///< バトル（4人接続）（WiFi）
  NIJI_P2P_TRADE_L      = 7,    ///< 交換（ローカル)
  NIJI_P2P_TRADE_W      = 8,    ///< 交換（WiFi）
  NIJI_P2P_RANDOM_TRADE = 9,    ///< ランダム交換

  // バトル施設
  NIJI_P2P_BATTLE_TREE_2 = 10,  ///< バトルハウス（マルチ）
  NIJI_P2P_BATTLE_TREE_4 = 11,  ///< バトルハウス（バトルロイヤル）

  // ランダムマッチ
  /*
  NIJI_P2P_RNDRATE_S = 101,    ///< レーティング:シングル
  NIJI_P2P_RNDRATE_D = 102,    ///< レーティング:ダブル
  NIJI_P2P_RNDRATE_SP = 103,    ///< レーティング:スペシャル
  NIJI_P2P_RNDRATE_WCS = 104,    ///< レーティング:WCSルール
  NIJI_P2P_RNDFREE_S_LEGEND = 105,    ///< フリー：シングル(伝説あり)
  NIJI_P2P_RNDFREE_D_LEGEND = 106,    ///< フリー：ダブル(伝説あり)
  NIJI_P2P_RNDFREE_B_LEGEND = 107,    ///< フリー：バトルロイヤル(伝説あり)
  NIJI_P2P_RNDFREE_S = 108,    ///< フリー：シングル(伝説なし)
  NIJI_P2P_RNDFREE_D = 109,    ///< フリー：ダブル(伝説なし)
  NIJI_P2P_RNDFREE_B = 110,    ///< フリー：バトルロイヤル(伝説なし)
  NIJI_P2P_WIFI_CUP = 111,    ///< インターネット大会
  NIJI_P2P_GROUP_CUP = 112,    ///< 仲間大会
  */

  // momijiで互換切る為にID変更
  NIJI_P2P_RNDRATE_S  = 201,    ///< レーティング:シングル
  NIJI_P2P_RNDRATE_D  = 202,    ///< レーティング:ダブル
  NIJI_P2P_RNDRATE_SP = 203,    ///< レーティング:スペシャル
  NIJI_P2P_RNDRATE_WCS = 204,    ///< レーティング:WCSルール
  NIJI_P2P_RNDFREE_S_LEGEND  = 205,    ///< フリー：シングル(伝説あり)
  NIJI_P2P_RNDFREE_D_LEGEND  = 206,    ///< フリー：ダブル(伝説あり)
  NIJI_P2P_RNDFREE_B_LEGEND  = 207,    ///< フリー：バトルロイヤル(伝説あり)
  NIJI_P2P_RNDFREE_S  = 208,    ///< フリー：シングル(伝説なし)
  NIJI_P2P_RNDFREE_D  = 209,    ///< フリー：ダブル(伝説なし)
  NIJI_P2P_RNDFREE_B  = 210,    ///< フリー：バトルロイヤル(伝説なし)
  NIJI_P2P_WIFI_CUP   = 211,    ///< インターネット大会
  NIJI_P2P_GROUP_CUP  = 212,    ///< 仲間大会


  // 以下は、赤外線接続
  NIJI_P2P_LIVE_CUP   = 115,    ///< LIVE大会
  NIJI_P2P_GIFT       = 120,    ///< ふしぎなおくりもの

  //BeaconTraderが使用するID
  NIJI_BEACON_TRADE   = 200,

  NIJI_P2P_MAX  = 255,    ///< InitParameterのGameIDがu8なので255が最大値

} ENUM_NIJI_P2P_ID;


class P2pConnectionManager : public gfl2::base::Singleton<P2pConnectionManager>, public gflnet2::p2p::P2pManagerEventListener, public NexRatingClientListener, public P2pTimeoutListener
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pConnectionManager);

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
private:
  //-------------------------------------
  /// シーケンス定義
  //=====================================
  typedef enum{
    E_SEQ_NONE = 0,         ///< なし
    E_SEQ_CONNECT_WAIT,     ///< P2P接続待ち
    E_SEQ_RATING_INITIALIZE,///< レーティングクラス初期化
    E_SEQ_RATING_SESSION,   ///< レーティングセッション確立処理
    E_SEQ_CONNECT_TIMING,   ///< P2P接続完了同期
    E_SEQ_CONNECT_TIMING_WAIT,///< P2P接続完了同期待ち
    E_SEQ_COMMON_DATA_CREATE,///< 共通のデータモジュール作成
    E_SEQ_COMMON_DATA_START1,///< 共通のデータ送信
    E_SEQ_COMMON_DATA_WAIT1, ///< 共通のデータ受信待ち
    E_SEQ_COMMON_DATA_START2,///< 共通のデータ送信
    E_SEQ_COMMON_DATA_WAIT2, ///< 共通のデータ受信待ち
    E_SEQ_CONNECT_LOOP,     ///< P2P接続完了（空ループ）
    E_SEQ_DISCONNECT_TIMING,///< P2P切断同期
    E_SEQ_DISCONNECT_TIMING_WAIT,///< P2P切断同期待ち
    E_SEQ_DISCONNECT_EXE,   ///< P2P切断実行
    E_SEQ_DISCONNECT_START, ///< P2P切断開始
    E_SEQ_DISCONNECT_WAIT,  ///< P2P切断待ち
    E_SEQ_DISCONNECT_LOOP,  ///< P2P切断完了(空ループ)
  }E_SEQ;

  //! 接続完了同期番号
  static const int CONNECT_TIMING_NO = 136;
  //! 切断前同期番号
  static const int DISCONNECT_TIMING_NO = 137;

  //! ビーコンサイズ
  static const int BEACON_BUFFER_SIZE = 200;

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

public:
  // コンストラクタ
  P2pConnectionManager();
  // デストラクタ
  virtual ~P2pConnectionManager();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   p2pId           ゲームごとのP2PサービスID
   * @return  trueで開始成功
   */
  //------------------------------------------------------------------
  void Initialize( const ENUM_NIJI_P2P_ID p2pId );

  //------------------------------------------------------------------
  /**
   * @brief   ビーコン設定（ローカルの親のみ）
   */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  void SetLocalP2PMasterBeacon( const void* pBeaconData , u32 beaconSize );
#endif // GF_PLATFORM_CTR 

  //------------------------------------------------------------------
  /**
   * @brief   接続開始
   *
   * @param   connectType     接続種類（自動、親、子）
   * @param   transferdIdList 子の場合のみ親のTransferdIDリスト（親、自動なら省略）
   * @return  trueで開始成功
   */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  bool ConnectStart( const gflnet2::p2p::CONNECT_TYPE connectType, bit64 transferdId = 0 );
#endif // GF_PLATFORM_CTR 

  //------------------------------------------------------------------
  /**
   * @brief   接続開始(Wifi接続時のランダムマッチング専用）
   *
   * @param   connectType     接続種類（自動、親、子）
   * @param   ruleLest        マッチング条件リスト
   * @return  trueで開始成功
   */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  bool ConnectStartWifi( const gflnet2::p2p::CONNECT_TYPE connectType, gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList* ruleList );
#endif // GF_PLATFORM_CTR 

  //------------------------------------------------------------------
  /**
   * @brief   接続成功取得
   * @return  trueで接続成功している
   */
  //------------------------------------------------------------------
  bool IsConnectSuccess();

  //------------------------------------------------------------------
  /**
   * @brief   切断開始
   * @param   isSkipDisconnectTiming     切断同期をスキップするか（デフォルトfalse）
   */
  //------------------------------------------------------------------
  void DisconnectStart( bool isSkipDisconnectTiming = false );

  //------------------------------------------------------------------
  /**
   * @brief   切断成功取得
   * @return  trueで切断成功している
   */
  //------------------------------------------------------------------
  bool IsDisconnectSuccess();

  //------------------------------------------------------------------
  /**
   * @brief   終了
   */
  //------------------------------------------------------------------
  void Terminate();

  //------------------------------------------------------------------
  /**
   * @brief   同期開始
   * @param   timingUniqueNo    同期番号 0以外
   * @param   timeou            タイムアウト有り無し  trueであり
   */
  //------------------------------------------------------------------
  void TimingStart( int timingUniqueNo, bool timeout );
  //------------------------------------------------------------------
  /**
   * @brief   同期完了チェック
   * @param   timingUniqueNo    同期番号 0以外
   * @return  trueで完了
   */
  //------------------------------------------------------------------
  bool IsTimingEnd( int timingUniqueNo );

  //------------------------------------------------------------------
  /**
   * @brief   接続相手のMyStatus取得（P2P接続成功後に取得可能）
   * @param[in] netId   通信ID（NetGame::GetNetID)
   * @return  MyStatus
   */
  //------------------------------------------------------------------
  Savedata::MyStatus* GetMyStatus( const int netId ) const;

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief PokePartyデータ送受信開始
   * @param[in] pPokeParty    ポケパーティ
   * @param[in] pCerdData     署名データ（必要ないなら省略可）
   * @return  送信準備完了したらtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  bool SendPokeParty( const pml::PokeParty* pPokeParty, const void* pCerdData );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief PokeParty送受信終了したか
   * @return 終了していたらtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsRecvPokePartyEnd();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   ポケモン改ざんチェックの結果取得
   * @param[in] netId   通信ID（NetGame::GetNetID)
   * @return  改ざんが行われていないならtrueを返却する
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsValidationCheckOk( const int netId );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   接続相手のPokeParty取得（IsRecvPokePartyEnd成功後に取得可能）
   * @param[in] netId   通信ID（NetGame::GetNetID)
   * @return  PokeParty
   */
  /* ----------------------------------------------------------------------------------------- */
  pml::PokeParty* GetPokeParty( const int netId ) const;

  //------------------------------------------------------------------
  /**
   * @brief   SetThreadPriority設定
   * @param   SetThreadPriority    
   */
  //------------------------------------------------------------------
  void SetThreadPriority( int threadPriority );

  //------------------------------------------------------------------
  /**
   * @brief   接続順番 = NetIDをかえす
   * @return  NetID
   */
  //------------------------------------------------------------------
  int GetNetID() const;
 
  //------------------------------------------------------------------
  /**
   * @brief   インターネット接続かどうか
   * @return  trueでインターネット接続
   */
  //------------------------------------------------------------------
  bool IsWifi() const;

  //------------------------------------------------------------------
  /**
   * @brief   赤外線接続かどうか
   * @return  trueで赤外線接続
   */
  //------------------------------------------------------------------
  bool IsIr() const;

  //------------------------------------------------------------------
  /**
   * @brief   マイグレーション時電源OFF切断設定
   */
  //------------------------------------------------------------------
  void SetupMigrationPowerOff();

  //@fix #4127   インターネット通信を介したバトルロイヤル中に親機がLANケーブルの挿抜を行うと子機がはまる【ホストマイグレーション】
  //------------------------------------------------------------------
  /**
   * @brief   マイグレーションコールバック有効化
   */
  //------------------------------------------------------------------
  void SetupMigrationCallBackEnable();

  //------------------------------------------------------------------
  /**
   * @brief   レーティングクライアント取得
   */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  NexRatingClient* GetRatingClient() { return m_NexRatingClient; };
#endif

  //------------------------------------------------------------------
  /**
   * @brief   P2PID取得
   */
  //------------------------------------------------------------------
  NetLib::P2P::ENUM_NIJI_P2P_ID GetP2PID(){ return m_p2pId; };


  //------------------------------------------------------------------
  /**
   * @brief   接続相手にnijiが混ざっているかどうか
   * @return  trueでnijiが混ざっている
   */
  //------------------------------------------------------------------
  bool IsNijiConnect();


  /************************************************************************/
  /* 
   * 以下、システム用メソッド。
   *                                                                 */
  /************************************************************************/

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update();

  //------------------------------------------------------------------
  /**
   * @brief   エラ－リスナー登録
   * @param   listener   通知を受け取るP2pConnectionErrorListener
   */
  //------------------------------------------------------------------
  void RegistErrorListener( NetLib::P2P::P2pConnectionErrorListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   エラーリスナー解除
   */
  //------------------------------------------------------------------
  void UnregistErrorListener( void );


  //------------------------------------------------------------------
  /**
   * @brief   マイグレーションリスナー登録
   * @param   listener   通知を受け取るP2pConnectionMigrationListener
   */
  //------------------------------------------------------------------
  void RegistMigrationListener( NetLib::P2P::P2pConnectionMigrationListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   マイグレーションリスナー解除
   */
  //------------------------------------------------------------------
  void UnregistMigrationListener( void );

  //------------------------------------------------------------------
  /**
   * @brief   クイックマッチ専用のP2P接続成功
   * @return  P2P接続成功したらtrue
   *
   * @note    クイックマッチはP2P接続直後のバックグラウンドのデータやり取りがいらないので、
   *          P2Pが繋がっただけを知る必要があった。
   */
  //------------------------------------------------------------------
  bool IsQuickMatchSuccess();

  //------------------------------------------------------------------
  /**
   * @brief   マッチメイクセッション作成成功
   * @return  成功してたらtrue
   *
   * @note    NCL三木さん要望：ml-pokenet:397
   *          子の再検索を止めて欲しいとのことで、
   *          親がマッチメイクセッション作成してから子が検索しにいく必要があった。
   *          その為に、マッチメイクセッション作成したどうかを取得する必要がある。
   */
  //------------------------------------------------------------------
  bool IsMatchmakingCreateSuccess();

  //------------------------------------------------------------------
  /**
    * @brief   シーケンス番号設定
    */
  //------------------------------------------------------------------
  void SetSequence( NetLib::P2P::P2pConnectionManager::E_SEQ seq );

  //@fix #5671   「ミラクル交換」交換同期セーブ中に通信切断し電源ボタンを押すとハマリおよび致命的エラー発生する
  //------------------------------------------------------------------
  /**
    * @brief   強制切断成功フラグ設定
    */
  //------------------------------------------------------------------
  void SetForceDisconnectSuccessFlag( void ){ m_isForceDisconnectSuccessFlag = true; };

#if PM_DEBUG
  //! エラー対処正常終了チェック
  void DEBUG_CheckErrorHandling();
#endif // PM_DEBUG

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
#ifdef  GF_PLATFORM_CTR
  virtual void OnP2pManagerError( const nn::nex::qResult& result, u32 errorCode );
#endif // GF_PLATFORM_CTR 

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
  virtual void OnP2pManagerErrorOutOfMemory();

  //------------------------------------------------------------------
  /**
   * @brief マイグレーション成功
   */
  //------------------------------------------------------------------
  virtual void OnP2pManagerMigrationSuccess();

#ifdef  GF_PLATFORM_CTR
  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションの作成完了通知
  *
  * @param[in] gatheringId 作成されたセッションのギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingCreateSuccess( const nn::nex::GatheringID& gatheringId );
#endif // GF_PLATFORM_CTR

  //=============================================================================
  /**
  *  NexRatingClientリスナー
  */
  //=============================================================================
  //------------------------------------------------------------------
  /**
  * @brief Ratingセッション作成の成功通知
  *
  * @param[in] sessionKey 生成されたセッションキー
  */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  virtual void OnCreateRatingSessionSuccess( const nn::nex::qVector<qByte>& sessionKey );
#endif // GF_PLATFORM_CTR 

  //------------------------------------------------------------------
  /**
  * @brief Ratingセッション参加終了通知
  */
  //------------------------------------------------------------------
  virtual void OnJoinRatingSessionSuccess( void );

  //------------------------------------------------------------------
  /**
  * @brief Ratingサーバ通信のエラー通知
  *
  * @param[in] result    実行結果
  * @param[in] errorCode 通信エラーコード
  */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  virtual void OnNexRatingClientError( const nn::nex::qResult& result, u32 errorCode );
#endif // GF_PLATFORM_CTR 

  //------------------------------------------------------------------
  /**
  * @brief Ratingサーバ通信のキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnNexRatingClientCanceled( void );



  //=============================================================================
  /**
  *  P2pTimeoutリスナー
  */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief タイムアウト発生通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pTimeoutError();





  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:

  //------------------------------------------------------------------
  /**
   * @brief   切断実行  このクラスで作ったもの系の破棄
   * @return   trueで完了
   */
  //------------------------------------------------------------------
  bool DisconnectExe();

  //------------------------------------------------------------------
  /**
   * @brief   切断前同期開始
   */
  //------------------------------------------------------------------
  void DissconnetTimingStart();

  //------------------------------------------------------------------
  /**
   * @brief   切断前同期完了チェック
   * @return   trueで完了
   */
  //------------------------------------------------------------------
  bool DissconnetIsTimingEnd();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief P2P通信IDから通信経路定義を取得
   * @return  通信経路定義
   */
  /* ----------------------------------------------------------------------------------------- */
  JoinFestaScript::CommRouteType GetCommRouteTypeForP2pID() const;

  //------------------------------------------------------------------
  /**
   * @brief   レーティング接続かチェック
   * @return  trueでレーティングである
   */
  //------------------------------------------------------------------
  bool IsRating(); 

  // NexRatingClientの簡易ラッパー
  void RatingClientCreate();
  bool RatingClientInitialize();
  bool RatingSessionSequence();
  bool RatingCreateSession();
  bool RatingJoinSession();
  bool RatingClientTerminate();//   * @return   trueで完了
  void RatingClientDelete();

private:
  gfl2::heap::HeapBase*     m_SysHeap;      ///< システムヒープ
  gfl2::heap::HeapBase*     m_DevHeap;      ///< デバイスヒープ
  E_SEQ                     m_seq;          ///< シーケンス
  gflnet2::p2p::P2pManager* m_P2pManager;   ///< P2P管理クラス
  gflnet2::base::BeaconManager* m_BeaconManager; ///< ビーコンマネージャ
  gflnet2::p2p::NetGame*    m_NetGame;      ///< 送受信管理クラス
  bool                      m_isInitialize; ///< 初期化処理完了フラグ（trueで初期化済み）
  bool                      m_isConnectStartCall; ///< 接続開始関数呼び出したかどうか（trueで呼び出した)
  ENUM_NIJI_P2P_ID          m_p2pId;        ///< 通信サービス種類

  //=====================================
  // Rating用
#ifdef  GF_PLATFORM_CTR
  static const int          RATING_SESSION_KEY_SIZE = 32;
  NexRatingClient*          m_NexRatingClient;  ///< Ratingセッション用
  u16                       m_ratingSessionKeySize;
  u8                        m_ratingSessionKeyBuffer[ RATING_SESSION_KEY_SIZE ];
  nn::nex::qVector<qByte>*  m_ratingSessionKey; // レーティングセッション用キー
  bool                      m_isRatingListenerSuccess;  // レーティングリスナー成功通知
  int                       m_ratingSeq;
#endif // GF_PLATFORM_CTR 

  // タイムアウト
  P2pTimeout                m_timeout;

  // 共通データ送受信モジュール
  P2pCommonData*            m_pP2pCommonData;
  
  // エラー通知リスナー
  NetLib::P2P::P2pConnectionErrorListener*  m_errorListener;  ///< エラー通知リスナー
  // マイグレーション通知リスナー
  NetLib::P2P::P2pConnectionMigrationListener*  m_migrationListener;  ///< マイグレーション通知リスナー

  NetAppLib::Util::NetAppTimeout  m_disconnectTimingTimeOut;  //! 切断同期専用

  u8 m_localP2PMasterBeaconBuffer[BEACON_BUFFER_SIZE];
  u32 m_localP2PMasterBeaconSize;

  // NCL三木さん要望：ml-pokenet:397
  bool m_isMatchmakingCreateSuccess;

  //@fix #5671   「ミラクル交換」交換同期セーブ中に通信切断し電源ボタンを押すとハマリおよび致命的エラー発生する
  bool m_isForceDisconnectSuccessFlag;

  // デバッグ機能
#if PM_DEBUG
public:
  static bool m_debugConnectError; //! 必ず切断失敗フラグ
#endif // PM_DEBUG

};

GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

#endif // __P2P_CONNECTION_MANAGER__
