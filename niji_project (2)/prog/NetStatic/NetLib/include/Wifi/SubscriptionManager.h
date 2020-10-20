#if defined(GF_PLATFORM_CTR)
//======================================================================
/**
 * @file	SubscriptionManager.h
 *
 * @author	suzuki
 */
//======================================================================
#ifndef _SUBSCRIPTION_MANAGER_H_
#define _SUBSCRIPTION_MANAGER_H_
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/nex/gflnet2_NexMessageClient.h>

#include <NetStatic/NetLib/include/Wifi/SubscriptionNotificationListener.h>
#include <NetStatic/NetLib/include/Wifi/SubscriptionPacketListener.h>
#include <NetStatic/NetLib/include/NetDefine.h>

namespace NetAppLib{
  namespace JoinFesta{
  class JoinFestaPersonalData;
  class JoinFestaPersonalDataManager;
  }
} 

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Wifi)

/**
 * @namespace NetLib::subscription
 * @brief     NEXゲームサーバ用ユーザー通知イベント関連。<p>
 *            インターネット接続時のユーザーアクションを本機能を使用して他者に共有できます。
 */

//--------------------------------------------------------------------------------------------
/**
 * @brief インターネット接続時におけるＰＳＳユーザー間の情報共有機能を提供するためのクラス。<p>
 *        本機能によるリクエストはサーバへの負荷が大きいので、影響を考慮した関数呼び出しを行ってください。
 *
 * @note  本機能の利用には、ＮＥＸサーバおよびフレンドサーバへの事前ログインが必須となります。
 *
 * @attention  本機能はKUJIRA専用に導入されたカスタム機能を使用しているため他のアプリでは使用できません。
 */
//--------------------------------------------------------------------------------------------
class SubscriptionManager : public gfl2::base::Singleton<SubscriptionManager>,
                            public gflnet2::nex::NexMessageClientListener,
                            public gflnet2::nex::NexDispatchThreadListener
{
public:
  /**
   * @brief 更新リクエスト
   */
  typedef enum
  {
    E_UPDATE_FRIEND,           //!< 友達ユーザ更新
    E_UPDATE_ACQUAINTANCE,     //!< 知り合いユーザ更新
    E_UPDATE_STRANGERUPDATING, //!< 通りすがりユーザ更新
    E_UPDATE_ALL,              //!< 友達・知り合い・通りすがりユーザ更新
  }UPDATE_REQUEST;

  static const u32 ACQUAINTANCE_TARGET_MAX = 6;             //!< 更新対象「知り合い」の最大人数

private:

  enum
  {
    MESSAGE_WORK_SIZE = 256,
  };

  static const qUnsignedInt32 MYGROUP_PLAYER_MAX     = 25;  //!< 通りすがり最大人数(自グループ）
  static const qUnsignedInt32 OTHERGROUP_PLAYER_MAX  = 25;  //!< 通りすがり最大人数(他グループ）
  static const qUnsignedInt32 UPDATE_ONCE_USER_MAX   = 50;  //!< 1回の更新リクエストで取得する最大ユーザ人数
  //
  static const qUnsignedInt32 MY_CONTEXT_SIZE        = 16;  //!< 自身の更新リクエストコンテキストバッファ

  typedef nn::nex::qVector<nn::nex::SubscriptionData>             SubscriptionVector;
  typedef nn::nex::qVector<nn::nex::ActivePlayerSubscriptionData> ActiveSubscriptionVector;
  typedef nn::nex::qVector<nn::nex::PrincipalID>                  PidVector;
public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
   SubscriptionManager( void );

   //--------------------------------------------------------------------------------------------
   /**
    * @brief デストラクタ
    */
   //--------------------------------------------------------------------------------------------
   virtual ~SubscriptionManager();

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 初期化処理
   *
   * @param[in] nexManager NEXマネージャ
   *
   * @return 成功ならtrue、失敗したならfalseを返却
   */
  //--------------------------------------------------------------------------------------------
  qBool Initialize( gflnet2::nex::NexManager *nexMgr );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 終了処理
   *
   * @caution 終了前に必ずIsConnecting()で通信が完了していることを確認すること。
   */
  //--------------------------------------------------------------------------------------------
  void Finalize();

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief データ更新処理を予約
   *
   * @param[in] request 更新リクエストタイプ
   */
  //--------------------------------------------------------------------------------------------
  void ReserveUpdating( SubscriptionManager::UPDATE_REQUEST request );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 状態変化を検知する[知り合いユーザー]を更新登録する
   *
   * @note  以前に登録されていたユーザーは登録から除外される。
   * @note  登録されたユーザーのみ、状態変化を検知できる。
   *
   * @param[in]  lstPid 更新対象に登録するPIDのリスト。最大6人まで登録可能。
   *
   * @return 成功ならtrue、失敗したならfalseを返却
   */
  //--------------------------------------------------------------------------------------------
  qBool UpdateAcquaintanceTarget( PidVector& lstPid );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 自身の情報をSubscriptionサーバに通知
   *
   * @param[in] myData 通知対象データ。
   *
   * @return 成功ならtrue、失敗したならfalseを返却
   */
  //--------------------------------------------------------------------------------------------
  qBool UpdateMySubscriptionData( nn::nex::SubscriptionData& myData );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 自分の通知データが更新されたことをメッセージキューに登録する
   *
   * @param[in] pid 通知先ユーザーのプリンシバルID
   *
   * @note リクエストキューに積み上げられ、順次実行される
   */
  //--------------------------------------------------------------------------------------------
  void PushUpdatingMessageRequest( nn::nex::PrincipalID pid );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief パケットデータ更新通信用イベントリスナの登録
   *
   * @param[in] listener 登録するイベントリスナ。NULL指定で登録を解除
   */
  //--------------------------------------------------------------------------------------------
  void SetSubscriptionPacketListener( SubscriptionPacketListener* listener );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief PssPersonalDataをMessageClient経由で送信する
   *
   * @param[in] targetPID    送信相手のプリンシパルID
   * @param[in] personalData PSSパーソナルデータ
   *
   * @return 非同期通信が開始されたならtrueを返却
   */
  //--------------------------------------------------------------------------------------------
  bool SendMessageWithJoinFestaPersonalData( const nn::nex::PrincipalID targetPID, const NetAppLib::JoinFesta::JoinFestaPersonalData* personalData );

  //------------------------------------------------------------------
  /**
   * @brief PssPersonalDataを複数ターゲットにMessageClient経由で送信する
   */
  //------------------------------------------------------------------
  bool SendMessageMultiTargetWithJoinFestaPersonalData( const nn::nex::PrincipalID* targetPIDs, const NetAppLib::JoinFesta::JoinFestaPersonalData* personalData );

  //------------------------------------------------------------------
  /**
   * @brief PssPersonalDataをギャザリングを対象に送信する
   */
  //------------------------------------------------------------------
  bool SendMessageGatheringWithJoinFestaPersonalData( const NetAppLib::JoinFesta::JoinFestaPersonalData* personalData );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する
   *
   * @note  通信状態にかかわらず、何度でもコールしても良い
   *
   * @return キャンセル処理が実施されたならtrueを返却する
   */
  //--------------------------------------------------------------------------------------------
  bool CancelConnecting( void );

  //------------------------------------------------------------------
  /**
   * @brief Messaging通信キャンセルリクエストを発行する
   *
   * @note  通信状態にかかわらず、何度でもコールしても良い
   *
   * @return キャンセル中ならtrue キャンセル完了ならfalse
   */
  //------------------------------------------------------------------
  bool CancelMessagingConnecting( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 更新処理
   */
  //--------------------------------------------------------------------------------------------
  void Update( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief アトラクション更新
   */
  //--------------------------------------------------------------------------------------------
  void UpdateAttraction( void );

  //------------------------------------------------------------------
  /**
   * @brief アトラクション開催
   */
  //------------------------------------------------------------------
  void OpenAttractionSession( void );

  //------------------------------------------------------------------
  /**
   * @brief アトラクション参加
   */
  //------------------------------------------------------------------
  void JoinAttractionSession( void );


  //------------------------------------------------------------------
  /**
   * @brief アトラクション離脱
   */
  //------------------------------------------------------------------
  void ExitAttractionSession( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief リザルトコードをコンソールに出力する（デバッグ用）
   *
   * @param[in] result 出力対象となるリザルト値
   */
  //--------------------------------------------------------------------------------------------
  static void  PrintResult(const nn::nex::qResult& result);

private:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 自身更新用コンテキストを保管する
   *
   * @param[in] context 登録するコンテキスト
   *
   * @return 登録されたならtrueを返却する
   */
  //--------------------------------------------------------------------------------------------
  bool PushMyContext( nn::nex::ProtocolCallContext* context );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 自身更新用コンテキストを排除する
   *
   * @param[in] context 排除するコンテキスト
   *
   * @return 排除されたならtrueを返却する
   */
  //--------------------------------------------------------------------------------------------
  bool PopMyContext( nn::nex::ProtocolCallContext* context );

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 通信中か判定
   *
   * @return 通信中ならtrueを返却する
   */
  //--------------------------------------------------------------------------------------------
  bool IsConnecting( void ) const;


  

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 稼働状態判定
   *
   * @return 稼働状態にあるならtrue、非稼働状態ならfalseを返却
   */
  //--------------------------------------------------------------------------------------------
  bool IsEnable( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief アトラクションを開催できるか判定
   * @note  アトラクション選択アプリで使用する関数
   * @return trueで開催できる
   */
  //--------------------------------------------------------------------------------------------
  bool CanOpenAttraction( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 前回開催してからの残り待ち時間を取得する
   * @note  待つ時間は30分
   * @return 前回開催してからの残り待ち時間(残り待ち時間が0で開催できる)
   */
  //--------------------------------------------------------------------------------------------
  u32 GetOpenAttractionPreviousWaitTime( void ) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief 更新処理のブロック設定
   *
   * @param[in] isBlock 更新処理を抑制するならtrueを指定する
   */
  //------------------------------------------------------------------
  void SetUpdatingBlock( bool isBlock );

  bool IsInitRequestEnd(void){ return m_initRequestEnd; };

  bool IsOpenAttraction(void){ bool ret = m_isOpenAttraction; m_isOpenAttraction = false; return ret; };
  bool IsJoinAttraction(void){ bool ret = m_isJoinAttraction; m_isJoinAttraction = false; return ret; };
  bool IsExitAttraction(void){ bool ret = m_isExitAttraction; m_isExitAttraction = false; return ret; };
  bool IsOpenAttractionFailed(void){ bool ret = m_isOpenAttractionFailed; m_isOpenAttractionFailed = false; return ret; };
  bool IsJoinAttractionFailed(void){ bool ret = m_isJoinAttractionFailed; m_isJoinAttractionFailed = false; return ret; };


  u32 GetGatheringID(void){ return m_gatheringID; };
  void SetGatheringID( u32 id ){ m_gatheringID = id; };

  u16 GetInviteParam(void){ return m_attractionStatus->GetActivePlayerInviteParam(); };
  u16 GetJoinParam(void){ return m_attractionStatus->GetActivePlayerJoinParam(); };

public:
  #if PM_DEBUG
  //------------------------------------------------------------------
  /**
   * @brief Subscriptionサーバ情報を取得する（デバッグ用）
   *
   * @return 成功ならtrue、失敗したならfalseを返却
   */
  //------------------------------------------------------------------
  qBool GetServerInformation( void );
  #endif

public:
  // 以下はイベントリスナ用関数の為、ユーザコールは禁止
  void OnGetOnlineSubscriptionListCompletion(nn::nex::CallContext *pContext);
  void OnGetOnlineSubscriptionListCompletionForVip(nn::nex::CallContext *pContext);
  void OnGetStrangerPlayerDataCompletion(nn::nex::CallContext *pContext);
  void OnGetSubscriptionDataCompletion(nn::nex::CallContext *pContext);
  void OnAddTargetCompletion(nn::nex::CallContext *pContext);
  void OnUpdateMySubscriptionDataCompletion(nn::nex::CallContext *pContext);

  //
  void OnGetAttractionStatus(nn::nex::CallContext *pContext);
  void OnOpenAttractionSession(nn::nex::CallContext *pContext);
  void OnJoinAttractionSession(nn::nex::CallContext *pContext);
  void OnExitAttractionSession(nn::nex::CallContext *pContext);
  //
  #if PM_DEBUG
  void OnGetAllActivePlayerSubscriptionDataCompletion(nn::nex::CallContext *pContext);
  #endif
  //
  virtual void OnNexMessageReceived( nn::nex::TextMessage* message );
  virtual void OnNexMessageSendingSuccess( nn::nex::TextMessage* message );
  virtual void OnNexMessageClientError( const nn::nex::qResult& result, u32 errorCode );
  //
  virtual void OnNexDispatchPollingEvent( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief フレンド関係にあるオンラインユーザー情報を取得
   *
   * @return 成功ならtrue、失敗したならfalseを返却
   */
  //------------------------------------------------------------------
  qBool GetFriendPlayerSubscriptionData( void );

  //------------------------------------------------------------------
  /**
   * @brief 通りすがりプレイヤーの通知データを取得
   *
   * @return 成功ならtrue、失敗したならfalseを返却
   */
  //------------------------------------------------------------------
  qBool GetStrangerPlayerSubscriptionData( void );

  //------------------------------------------------------------------
  /**
   * @brief 知り合いプレイヤーの通知データを取得
   *
   * @return 成功ならtrue、失敗したならfalseを返却
   */
  //------------------------------------------------------------------
  qBool GetAcquaintancePlayerSubscriptionData( void );

  //------------------------------------------------------------------
  /**
   * @brief 更新通知対象となるしりあいユーザをSubscriptionサーバに登録する
   *
   * @return 登録に成功ならtrue、失敗したならfalseを返却
   */
  //------------------------------------------------------------------
  qBool AddTarget( void );

  //------------------------------------------------------------------
  /**
   * @brief ユーザーのステータス更新イベントを取得
   *
   * @return 更新イベントが存在したならtrueを返却
   */
  //------------------------------------------------------------------
  qBool GetNotifiedData( void );

  //------------------------------------------------------------------
  /**
   * @brief 更新通知されたユーザーの情報を取得する
   *
   * @return 更新されたならtrueを返却
   */
  //------------------------------------------------------------------
  qBool GetUpdatingSubscriptionData( void );

  //------------------------------------------------------------------
  /**
   * @brief アトラクション情報取得
   */
  //------------------------------------------------------------------
  qBool GetAttractionStatus( void );



  //------------------------------------------------------------------
  /**
   * @brief アクセスロック状態であるか判定
   *
   * @note  アクセスロック判定と同時にロック状態にする
   *
   * @return アクセスロック状態であればfalseを返却する
   */
  //------------------------------------------------------------------
  qBool CheckConnectingLock( void );

  //------------------------------------------------------------------
  /**
   * @brief アクセスロックの解除
   */
  //------------------------------------------------------------------
  void  UnlockConnectingBlock( void );

  //------------------------------------------------------------------
  /**
   * @brief 更新通知キューからリクエストを取出し、メッセージとして送信する
   */
  //------------------------------------------------------------------
  void  SendUpdatingMessage( void );

  //------------------------------------------------------------------
  /**
   * @brief メンバ変数の初期化
   */
  //------------------------------------------------------------------
  void  Reset( void );

  //------------------------------------------------------------------
  /**
   * @brief グループ情報の設定
   */
  //------------------------------------------------------------------
  void  SetupGroup( void );

  //------------------------------------------------------------------
  /**
   * @brief NEX関数実行エラー報告処理
   */
  //------------------------------------------------------------------
  void  ReportFunctionError( void );

  //------------------------------------------------------------------
  /**
   * @brief エラーマネージャへのエラー報告
   *
   * @param result 実行結果
   */
  //------------------------------------------------------------------
  void  ReportError( nn::nex::qResult& result );

  //------------------------------------------------------------------
  /**
   * @brief エラーマネージャへのエラー報告
   *
   * @param errorCode ネットワークエラーコード
   */
  //------------------------------------------------------------------
  void  ReportError( u32 errorCode );

private:
  bool                                         m_isUpdatiingBlock;          //!< 更新処理をブロックする
  bool                                         m_isConnecting;              //!< 通信フラグ
  bool                                         m_isCreateMyData;            //!< 自身のデータ生成フラグ
  bool                                         m_isInit;                    //!< 初期化フラグ
  bool                                         m_requestFriendUpdating;     //!< 友達ユーザー更新許可フラグ
  bool                                         m_requestAcquaintance;       //!< 通りすがりユーザー更新許可フラグ
  bool                                         m_requestStrangerUpdating;   //!< すれ違いユーザー更新許可フラグ
  //
  PidVector*                                   m_lstAddTarget;              //!< 追加ターゲットリスト
  PidVector*                                   m_lstVip;                    //!< niji追加：VIPユーザーリスト
  PidVector*                                   m_lstUpdatedUserPID;         //!< 更新されたユーザーPID
  PidVector*                                   m_lstRequestMessage;         //!< メッセージ送信リクエストリスト
  SubscriptionVector*                          m_lstGetSubscriptionData;    //!< 取得対象ユーザー情報
  ActiveSubscriptionVector*                    m_lstActiveSubscriptionData; //!< 通りすがりユーザの更新情報
  nn::nex::SubscriptionClient*                 m_subscriptionClient;        //!< SubscriptionClient
  nn::nex::qList<nn::nex::NotificationEvent>*  m_notificationList;          //!< ユーザーステータスの更新情報
  nn::nex::SubscriptionData*                   m_playerData;                //!< プレイヤー情報
  gflnet2::nex::NexManager*                     m_nexManager;                //!< NEXマネージャ
  gflnet2::nex::NexMessageClient*               m_messageClient;             //!< メッセージクライアント
  nn::nex::MatchmakeExtensionClient*           m_matchingClient;             //!< マッチメイククライアント
  u16                                          m_attractionInterval;         //!< アトラクション情報取得インターバル
  nn::nex::AttractionStatus*                   m_attractionStatus;           //!< アトラクションステータス
  
  //
  gfl2::str::STRCODE                           m_messageBase64[MESSAGE_WORK_SIZE];        //!< BASE64メッセージ
  u8                                           m_personalBuff[MESSAGE_WORK_SIZE];         //!< パーソナルデータバッファ(4バイトアライメントであること)
  //
  SubscriptionNotificationListener*            m_subscriptionNotify;        //!< ゲームサーバ更新イベント
  SubscriptionPacketListener*                  m_listener;                  //!< イベントリスナ
  //
  nn::nex::ProtocolCallContext*                m_myContext[MY_CONTEXT_SIZE];//!< カレントコールコンテキスト(自身更新用）
  nn::nex::ProtocolCallContext*                m_context;                   //!< カレントコールコンテキスト
  //
  u32                                          m_myGroup;                   //!< 自身のグループID(リージョン）
  //
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager*         m_personalMgr;               //!< PSSパーソナルマネージャ

  //
  bool m_initRequestEnd;
  //
  bool m_isOpenAttraction;
  bool m_isJoinAttraction;
  bool m_isExitAttraction;
  bool m_isOpenAttractionFailed;
  bool m_isJoinAttractionFailed;
  u32 m_count;
  nn::nex::GatheringHolder*                                m_sessionInfo;       //!< セッション情報
  nn::nex::GatheringID                                              m_gatheringID;       //!< 参加しているギャザリングID

  u64 m_openAttractionMsec;

  #if PM_DEBUG
  nn::nex::String*                             m_serverId;                  //!< 接続中のサーバID
  nn::nex::qMap<u32,ActiveSubscriptionVector>* m_allPlayerList;             //!< 接続中サーバの全ユーザ情報
  #endif
};


GFL_NAMESPACE_END(Wifi)
GFL_NAMESPACE_END(NetLib)

#endif  // _SUBSCRIPTION_MANAGER_H_
#endif
