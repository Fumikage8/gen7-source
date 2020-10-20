#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexMatchMakingClient.h
 *
 * @brief  nn::nex::MatchMakingClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_MATCH_MAKING_H__
#define __GFLNET_NEX_MATCH_MAKING_H__

#pragma once

#include <nex.h>
#include <nex_P2p.h>

#include <nn/friends.h>
#include <nw/ut.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/nex/gflnet2_NexMatchmakeListener.h>
#include <gflnet2/include/nex/gflnet2_NexSessionEventListener.h>
#include <gflnet2/include/nex/gflnet2_NexCommunityListener.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingRule.h>

#include <base/include/gfl2_Singleton.h>

namespace gflnet2{
namespace nex{

class NexMatchMakingNotificationListener;

//------------------------------------------------------------------
/**
*  @brief MatchMakingClientアクセス用クラス
*
*  @par       マッチングの手順は以下の3つがある。
*              - [オーナー] CreateSession() → OpenSession()
*              - [参加者  ] SearchSession() → JoinSession()
*              - [オーナー] AutoMatchmake() → OpenSession()
*              - [参加者  ] AutoMatchmake()
*
*  @attention 事前にゲームサーバへログインしておくこと
*  @attention 本関数は2つのスレッドで同時に呼び出すことはできない。
*
*  @note      MatchMakeとCommunityで使用されるギャザリングIDは共通利用不可能。
*  @note      Communityに参加したユーザー同士でマッチングする場合は、
*  @note      Communityで取得したギャザリングIDを用いてマッチメイクセッションを確立し
*  @note      P2P接続用のギャザリングIDを発行すること。
**/
//------------------------------------------------------------------
class NexMatchMakingClient : public gfl2::base::Singleton<NexMatchMakingClient>
{
  GFL_FORBID_COPY_AND_ASSIGN(NexMatchMakingClient); //コピーコンストラクタ＋代入禁止

  friend class NexMatchMakingNotificationListener;  //!< NEXマッチメイクイベント用リスナ

public:
  typedef nn::nex::GatheringID GatheringID;
  typedef nw::ut::LinkList<NexMatchMakingRule, offsetof(NexMatchMakingRule,node)> NexMatchMakingRuleList;

  /**
   * @brief マッチメイクタイプ
   */
  typedef enum
  {
    MATCHMAKE_TYPE_FRIEND    = nn::nex::MATCHMAKE_SYSTEM_TYPE_FRIEND,    //!< フレンドのみ
    MATCHMAKE_TYPE_COMMUNITY = nn::nex::MATCHMAKE_SYSTEM_TYPE_COMMUNITY, //!< コミュニティ
    MATCHMAKE_TYPE_ANYBODY   = nn::nex::MATCHMAKE_SYSTEM_TYPE_ANYBODY,   //!< 制限なし
  }E_MATCHMAKE_TYPE;

private:

// マッチメイクグループ
#if GFL_DEBUG
  static nn::nex::qUnsignedInt32 MATCHMAKE_GROUP_RANDOM_TRADE;
#else
  static const nn::nex::qUnsignedInt32 MATCHMAKE_GROUP_RANDOM_TRADE = 64;
#endif

public:
  NexMatchMakingClient( void );
  virtual ~NexMatchMakingClient();

public:
  //------------------------------------------------------------------
  /**
  * @brief  MatchMakingClientの初期化
  *
  * @param[in] pFascade     ゲームサーバーログイン情報
  *
  * @note マッチングの連続成立防止用履歴のクリア処理はデバッグビルド時のみ有効
  *
  * @return 初期化に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool Initialize( nn::nex::NgsFacade* pFascade );

  //------------------------------------------------------------------
  /**
  * @brief  MatchMakingClientの終了
  *
  * @attention 必ず事前にIsConnecting()がfalseであることを確認してください
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
   * @brief  MatchmakeExtensionClientインスタンスの取得
   *
   * @return MatchmakeExtensionClientインスタンス
   */
  //------------------------------------------------------------------
  nn::nex::MatchmakeExtensionClient* GetMatchmakeExtensionClient(void);

  //------------------------------------------------------------------
  /**
   * @brief  ホストマイグレーション拡張機能の有効化
   */
  //------------------------------------------------------------------
  void EnableHostMigration(void);

  //------------------------------------------------------------------
  /**
   * @brief  ホストマイグレーションコールバックの登録
   */
  //------------------------------------------------------------------
  void RegisterHostMigrationCallback(void);

  //------------------------------------------------------------------
  /**
  * @brief  ホスト孤立状態かどうか
  */
  //------------------------------------------------------------------
  bool SessionHostIsOrphan(void);


#if GFL_DEBUG
  //------------------------------------------------------------------
  /**
  * @brief マッチング履歴のクリアフラグをセット
  *
  * @param[in] flag trueを指定するとInitialize時にマッチング履歴がクリアされる。
  */
  //------------------------------------------------------------------
  void SetClearCacheFlag( const bool flag ){ m_debugIsClearCache = flag; };
  bool GetClearCacheFlag( void ){ return m_debugIsClearCache; };

  // デバッグ用マッチングを分けるグループ
  void SetDebugMatchmakeGroup( const u32 debugMatchmakeGroup )
  {
    m_debugMatchmakeGroup = debugMatchmakeGroup;
    MATCHMAKE_GROUP_RANDOM_TRADE = debugMatchmakeGroup;
  }
#endif // GFL_DEBUG

public:
  //------------------------------------------------------------------
  /**
  * @brief マッチメイク用リスナの登録
  *
  * @param[in] listener マッチメイク用リスナ。NULLを指定することで登録を解除できる。
  */
  //------------------------------------------------------------------
  void SetMatchmakeListener( NexMatchmakeListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief マッチメイク用リスナの取得
  *
  * @return マッチメイク用リスナへのポインタ
  */
  //------------------------------------------------------------------
  NexMatchmakeListener* GetMatchmakeListener( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief セッションイベント用リスナの登録
  *
  * @param[in] listener セッションイベント用リスナ。NULLを指定することで登録を解除できる。
  */
  //------------------------------------------------------------------
  void SetSessionEventListener( NexSessionEventListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief セッションイベント用リスナの取得
  *
  * @return セッションイベント用リスナへのポインタ
  */
  //------------------------------------------------------------------
  NexSessionEventListener* GetSessionEventListener( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief コミュニティ用リスナの登録
  *
  * @param[in] listener コミュニティ用リスナ。NULLを指定することで登録を解除できる。
  */
  //------------------------------------------------------------------
  void SetCommunityListener( NexCommunityListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief コミュニティ用リスナの取得
  *
  * @return コミュニティ用リスナへのポインタ
  */
  //------------------------------------------------------------------
  NexCommunityListener* GetCommunityListener( void ) const;

public:
  //------------------------------------------------------------------
  /**
  * @brief 自身をオーナーとしてマッチメイクセッションを作成し、参加する(非同期関数)
  *
  * @attention 本関数実行後、必ずOpenSession()をコールすること。
  *
  * @note   非同期通信の結果
  * @note   正常終了時は「NexMatchmakeListener::OnMatchmakingCreateSuccess()」がコールされる。
  * @note   エラー時は「NexMatchmakeListener::OnNexMatchmakeError()」がコールされる。
  *
  * @param[in] type             参加タイプ
  * @param[in] joinNum          マッチメイクに参加できる最大人数
  * @param[in] gameMode         ゲームモード（任意に設定可能）
  * @param[in] ruleList         抽出条件として使用するルールリスト(NULL指定も可能）
  * @param[in] isMigrateOwner   オーナーがログアウトしたときに他のユーザーにOwnerの委譲を行うならtrueを指定
  * @param[in] isUsingBlackList ブラックリストを適応するならtrueを指定
  *
  * @return 非同期処理の開始に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool CreateSessionAsync( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode,
                           NexMatchMakingRuleList* ruleList,
                           bool isMigrateOwner, bool isUsingBlackList );

  //------------------------------------------------------------------
  /**
   * @brief マッチメイクセッションのホストを自分自身として更新する
   *
   * @note  非同期通信の結果
   *        正常終了時は「NexMatchmakeListener::OnMatchmakingUpdateHostSuccess()」が
   *        コールされる。
   *        本関数がfalseを返却した際は、上記イベントは通知されない。
   *
   * @return 非同期処理の開始に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool UpdateSessionHost( void );

  //------------------------------------------------------------------
  /**
  * @brief  セッションを再開する(非同期関数)
  *
  * @note   正常成功時は「NexMatchmakeListener::OnMatchmakingOpenSessionSuccess()」がコールされる。
  * @note   エラー時は  「NexMatchmakeListener::OnNexMatchmakeError()」がコールされる。
  *
  * @param  gatheringID 再会対象となるセッションのギャザリングID
  *
  * @return 実行に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool OpenSessionAsync( nn::nex::GatheringID gatheringID );

  //------------------------------------------------------------------
  /**
  * @brief  マッチメイクセッションを検索する(非同期関数)
  *
  * @attention オーナーユーザーは「CreateSession()」を使用すること
  *
  * @note      非同期通信の結果
  * @note      正常成功時は「NexMatchmakeListener::OnMatchmakingSearchSuccess()」がコールされる。
  * @note      エラー時は  「NexMatchmakeListener::OnNexMatchmakeError()」がコールされる。
  *
  * @param[in] type          参加タイプ
  * @param[in] joinNum       マッチメイクに参加できる最大人数
  * @param[in] gameMode      ゲームモード（任意に設定可能）
  * @param[in] ruleList      抽出条件として使用するルールリスト(NULL指定も可能）
  *
  * @return 非同期処理の開始に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool SearchSessionAsync( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode, NexMatchMakingRuleList* ruleList );

  //------------------------------------------------------------------
  /**
  * @brief  マッチメイクセッションへ参加する(非同期関数)
  *
  * @attention  事前にSearchSession()を実行すること。
  * @attention  ホストユーザーは「CreateSession()」を使用すること。
  *
  * @note       非同期通信の結果
  * @note       正常成功時は「NexMatchmakeListener::OnMatchmakingJoinSuccess()」がコールされる。
  * @note       エラー時は  「NexMatchmakeListener::OnNexMatchmakeError()」がコールされる。
  *
  * @param[in]  gathering   参加対象となるギャザリングホルダー
  *
  * @return 非同期処理の開始に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool JoinSessionAsync( const nn::nex::GatheringHolder& gathering );

  //------------------------------------------------------------------
  /**
  * @brief 条件に合うマッチメイクセッションを検索・作成・参加する（非同期関数）
  *
  * @note  自身がオーナーとなった場合は「NexMatchmakeListener::OnMatchmakingCreateSuccess()」がコールされる。
  * @note  一般参加者の場合は「NexMatchmakeListener::OnMatchmakingSearchSuccess()」がコールされる。
  * @note  エラー時は「NexMatchmakeListener::OnNexMatchmakeError()」がコールされる。
  *
  * @attention 本関数を使用してマッチメイクセッションに参加した場合は
  *            CreateSession(), JoinSession()をコールしてはならない。
  *
  * @param[in] type             参加タイプ
  * @param[in] joinNum          マッチメイクに参加できる最大人数
  * @param[in] gameMode         ゲームモード（任意に設定可能）
  * @param[in] ruleList         抽出条件として使用するルールリスト(NULL指定も可能）
  * @param[in] isMigrateOwner   オーナーがログアウトしたときに他のユーザーにOwnerの委譲を行うならtrueを指定
  * @param[in] isUsingBlackList ブラックリストを適応するならtrueを指定
  *
  * @return 非同期処理の開始に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool AutoMatchmakeAsync( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode,
                           NexMatchMakingRuleList* ruleList,
                           bool isMigrateOwner, bool isUsingBlackList );

  //------------------------------------------------------------------
  /**
  * @brief  現在接続しているセッションのギャザリングIDを取得する
  *
  * @attention CreateSession()またはJoinSession()にてセッションへ
  *            接続しておく必要がある。
  *
  * @return ギャザリングID
  */
  //------------------------------------------------------------------
  GatheringID GetGatheringID( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief  参加予定のセッションの最大参加人数を取得する
  *
  * @attention CreateSession(), SearchSession(), AutoMatchmake()を事前に実行する必要がある。
  *
  * @return 最大参加人数
  */
  //------------------------------------------------------------------
  u32 GetJoinMaxNum( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief  自身がマッチメイクセッションのオーナーであるか判定
  *
  * @attention マッチメイクセッションに接続していない場合は正しく動作しない
  *
  * @return 自身がマッチメイクセッションのオーナーであればtrueを返却する
  */
  //------------------------------------------------------------------
  bool IsOwner( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief 通信中か判定
   *
   * @return 通信中ならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsConnecting( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する
   * @brief キャンセルが実施された場合、完了時にOnNexRatingClientCanceled()がコールされる。
   *
   * @note  通信状態にかかわらず、コールしても良い
   *
   * @return キャンセル処理が実施されたならtrueを返却する
   */
  //------------------------------------------------------------------
  bool CancelConnecting( void );

public:

  //------------------------------------------------------------------
  /**
   * @brief  コミュニティを作成・参加する(非同期関数)
   *
   * @attention  ユーザーはコミュニティのオーナーとなる。
   * @attention  ユーザーが作成可能なコミュニティ数は4まで。
   * @attention  コミュニティは参加者が 0 になる、またはコミュニティのオーナーが明示的に
   * @attention  DeleteParticipationAsync() によって削除しない限り、永続的にゲームサーバに残る。
   *
   * @note       非同期通信の結果
   *             正常成功時は「NexCommunityListener::OnCommunityCreateSuccess()」が
   *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
   *             コールされる。
   *             本関数がfalseを返却した際は、上記イベントは通知されない。
   *
   * @param[in]  community      作成するコミュニティ情報
   * @param[in]  strMessage     参加者が固有に登録できるメッセージ。最大 nn::nex::MATCHMAKE_STRING_MAX_LENGTH 文字まで。
   *
   * @return 非同期処理の開始に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool CreateCommunity( const nn::nex::Community& community, const nn::nex::String& strMessage = _T("") );

  //------------------------------------------------------------------
  /**
   * @brief  コミュニティに参加する(非同期関数)
   *
   * @attention  ユーザーが参加可能なコミュニティ数は16まで。
   * @attention  EndParticipation() で明示的に退出しない限りコミュニティへの参加状態は残り続ける
   *
   * @note       非同期通信の結果
   *             正常成功時は「NexCommunityListener::OnCommunityJoinSuccess()」が
   *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
   *             コールされる。
   *             本関数がfalseを返却した際は、上記イベントは通知されない。
   *
   * @param[in]  gatheringId    参加対象となるコミュニティ用ギャザリングID。
   * @param[in]  strMessage     参加者が固有に登録できるメッセージ。最大 nn::nex::MATCHMAKE_STRING_MAX_LENGTH 文字まで。
   * @param[in]  strPassword    コミュニティのパスワード。コミュニティタイプがPasswordLockの場合のみ有効。
   *
   * @return 非同期処理の開始に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool JoinCommunity( nn::nex::GatheringID gatheringId, const nn::nex::String& strMessage = _T(""), const nn::nex::String& strPassword = _T("") );

  //------------------------------------------------------------------
  /**
   * @brief  コミュニティを参加者のPrincipalIdで検索する(非同期関数)
   *
   * @attention  FindCommunityByGatheringId(), FindOfficialCommunity()とは排他。
   *
   * @note       非同期通信の結果
   *             正常成功時は「NexCommunityListener::OnCommunitySearchSuccess()」が
   *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
   *             コールされる。
   *             本関数がfalseを返却した際は、上記イベントは通知されない。
   *
   * @param[in]  principalId    検索対象となるプリンシバルID
   * @param[in]  resultRange    検索範囲条件
   *
   * @return 非同期処理の開始に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool FindCommunityByParticipant( nn::nex::PrincipalID principalId, const nn::nex::ResultRange & resultRange );

  //------------------------------------------------------------------
  /**
   * @brief  コミュニティをコミュニティ用ギャザリングIDで検索する(非同期関数)
   *
   * @attention  FindCommunityByParticipant(), FindOfficialCommunity()とは排他。
   *
   * @note       非同期通信の結果
   *             正常成功時は「NexCommunityListener::OnCommunitySearchSuccess()」が
   *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
   *             コールされる。
   *             本関数がfalseを返却した際は、上記イベントは通知されない。
   *
   * @attention  指定できるコミュニティ用ギャザリングID数は100件まで
   *
   * @param[in]  gatheringIdList 検索対象となるコミュニティ用ギャザリングID
   *
   * @return 非同期処理の開始に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool FindCommunityByGatheringId( const nn::nex::qList< nn::nex::GatheringID >& gatheringIdList );

  //------------------------------------------------------------------
  /**
   * @brief  公式コミュニティを検索する(非同期関数)
   *
   * @attention  FindCommunityByParticipant(), FindCommunityByGatheringId()とは排他。
   *
   * @note       非同期通信の結果
   *             正常成功時は「NexCommunityListener::OnCommunitySearchSuccess()」が
   *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
   *             コールされる。
   *             本関数がfalseを返却した際は、上記イベントは通知されない。
   *
   * @param[in]  isAvailableOnly 参加可能なコミュニティのみ検索対象に含めるならtrueを指定
   * @param[in]  resultRange     検索範囲条件
   *
   * @return 非同期処理の開始に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool FindOfficialCommunity( bool isAvailableOnly, const nn::nex::ResultRange& resultRange );

  //------------------------------------------------------------------
  /**
   * @brief  コミュニティから退出する(非同期通信）
   *
   * @param  gateringId 退出対象のコミュニティギャザリングID
   *
   * @note       非同期通信の結果
   *             正常成功時は「NexCommunityListener::OnCommunityLeaveSuccess()」が
   *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
   *             コールされる。
   *             本関数がfalseを返却した際は、上記イベントは通知されない。
   *
   * @return 非同期処理の開始に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool LeaveCommunity( nn::nex::GatheringID gateringId );

  //------------------------------------------------------------------
  /**
   * @brief  コミュニティ用ギャザリングIDをコミュニティコードへ変換する
   *
   * @param gatheringId    変換元となるコミュニティ用ギャザリングID
   * @param communityCode  コミュニティコード格納先へのポインタ
   *
   * @return 変換に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool GatheringIdToPersistentGatheringCode( nn::nex::GatheringID gatheringId, u64 *communityCode );

  //------------------------------------------------------------------
  /**
   * @brief  コミュニティコードをコミュニティ用ギャザリングIDへ変換する
   *
   * @param communityCode  変換元となるコミュニティコード
   * @param gatheringId    コミュニティ用ギャザリングID格納先へのポインタ
   *
   * @return 変換に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool PersistentGatheringCodeToGatheringId( u64 communityCode, nn::nex::GatheringID *gatheringId );

public:
  //------------------------------------------------------------------
  /**
  * @brief  マッチメイクセッションを削除する（非同期関数）
  *
  * @note   自身がオーナーのときのみ実行できる
  * @note   参加ユーザーについては「EndParticipation()」をコールすること。
  * @note   コミュニティマッチングかつオーナーである場合はコミュニティを削除する。
  *
  * @note   正常成功時は「NexCommunityListener::OnMatchmakingEndSuccess()」が
  * @note   エラー時は  「NexCommunityListener::OnNexCommunityError()」が
  * @note   返却される
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool DeleteParticipationAsync( void );
  
  //------------------------------------------------------------------
  /**
   * @brief  マッチメイクセッションから離脱する（非同期関数）
   *
   * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
   *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
   *
   * @note   自身がホストの場合は離脱は実行されない。
   *         別途「DeleteParticipationAsync()」をコールしてください。
   *         セッション離脱成功時は「NexMatchmakeListener::OnMatchmakingEndSuccess()」
   *         がコールされます。
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  bool EndParticipationAsync( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief  ランダムな1人のユーザーとマッチメイクする（非同期関数）
   *
   * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
   *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
   *
   * @note   ミラクル交換専用です。AutoMatchmakeの代わりに使います。
   * 
   *         セッション離脱成功時は「NexMatchmakeListener::OnMatchmakingEndSuccess()」
   *         がコールされます。
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  bool SimpleMatchmakeAsync( void );

  //------------------------------------------------------------------
  /**
   * @brief SimpleMatchmake用のP2Pセッション署名キーを登録する
   */
  //------------------------------------------------------------------
  void SetSimpleMatchmakeP2pSessionKey( void );

  //------------------------------------------------------------------
  /**
   * @brief  ランダムな1人のユーザーとのマッチメイクにエントリーする（非同期関数）
   *
   * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
   *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
   *
   * @note   ミラクル交換専用です。AutoMatchmakeの代わりに使います。
   * 
   *         セッション離脱成功時は「NexMatchmakeListener::OnMatchmakingEntrySimpleMatchmakeSuccess()」
   *         がコールされます。
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  bool EntrySimpleMatchmakeAsync( void );

  //------------------------------------------------------------------
  /**
   * @brief  ランダムな1人のユーザーとのマッチメイクにエントリーをキャンセルする（非同期関数）
   *
   * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
   *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
   *
   * @note   ミラクル交換専用です。AutoMatchmakeの代わりに使います。
   * 
   *         セッション離脱成功時は「NexMatchmakeListener::OnMatchmakingCancelSimpleMatchmakeEntrySuccess()」
   *         がコールされます。
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  bool CancelSimpleMatchmakeEntryAsync( void );

  //------------------------------------------------------------------
  /**
   * @brief  シンプルマッチメイク用のホスト情報を取得
   * @return P2P接続の為の情報
   */
  //------------------------------------------------------------------
  nn::nex::SimpleMatchmakeHostInfo* GetSimpleMatchmakeHostInfo(){ return m_pSimpleMatchmakeHostInfo; }

  //------------------------------------------------------------------
  /**
   * @brief  シンプルマッチメイクモードか
   * @return シンプルマッチメイクモードか
   */
  //------------------------------------------------------------------
  bool IsSimpleMatchmake( void ){ return m_isSimpleMatchmake; }

private:
  //------------------------------------------------------------------
  /**
  * @brief  オーナーのプリンシバルIDを設定する
  *
  * @attention コールバック関数用に準備している為、ユーザー操作禁止！
  *
  * @return none
  */
  //------------------------------------------------------------------
  void SetOwner( nn::nex::PrincipalID pid );

  //------------------------------------------------------------------
  /**
   * @brief  マッチメイクセッション検索条件を設定する
   *
   * @param type         参加タイプ
   * @param joinNum      マッチメイクに参加できる最大人数
   * @param gameMode     ゲームモード（任意に設定可能）
   * @param ruleList     抽出条件として使用するルールリスト
   *
   * @return none
   */
  //------------------------------------------------------------------
  void SetupCriteria( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode, NexMatchMakingRuleList* ruleList );

  //------------------------------------------------------------------
  /**
   * @brief  マッチメイクセッション作成条件を設定する
   *
   * @param type             参加タイプ
   * @param joinNum          マッチメイクに参加できる最大人数
   * @param gameMode         ゲームモード（任意に設定可能）
   * @param ruleList         抽出条件として使用するルール
   * @param isMigrateOwner   ホストがログアウトしたときに他のユーザーにOwnerの委譲を行うならtrueを指定
   * @param isUsingBlackList ブラックリストを適応するならtrueを指定
   *
   * @return none
   */
  //------------------------------------------------------------------
  void SetupMatchmakeSession( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode,
                              NexMatchMakingRuleList* ruleList, bool isMigrateOwner, bool isUsingBlackList );

  //------------------------------------------------------------------
  /**
  * @brief  ギャザリングIDをもとに、セッション参加判定用パスワードを設定する
  *
  * @note   誤って別のマッチメイクセッションに参加しているクライアントが P2P通信に参加してしまわないよう、
  *         SetLocalIdentificationToken() で参加したギャザリングIDをセットする。
  * @note   ここでセットした値を CheckJoiningProcesses() でチェックしている
  *
  * @param  gatheringId  参加対象のギャザリングID
  *
  * @return none
  */
  //------------------------------------------------------------------
  void SetPasswordWithGatheringID( GatheringID gatheringId );

  //------------------------------------------------------------------
  /**
   * @brief  P2Pセッションの署名キーを登録する
   *
   * @note   マッチメイクセッションを確立した後にコールすること
   *
   * @param  sessionKey セッションキー
   */
  //------------------------------------------------------------------
  void SetSessionSignatureKey( const nn::nex::qVector<qByte>& sessionKey );

  //------------------------------------------------------------------
  /**
   * @brief  通信アクセスをロックする
   *
   * @return アクセスロック状態にあるならtrueを返却
   */
  //------------------------------------------------------------------
  bool LockConnection( void );

  //------------------------------------------------------------------
  /**
   * @brief  通信アクセスロックを解除する
   */
  //------------------------------------------------------------------
  void UnlockConnection( void );

  //------------------------------------------------------------------
  /**
   * @brief コミュニティーNEX関数実行エラー報告処理
   */
  //------------------------------------------------------------------
  void ReportMatchmakeError( const nn::nex::qResult& result );

  //------------------------------------------------------------------
  /**
   * @brief コミュニティーNEX関数実行エラー報告処理
   */
  //------------------------------------------------------------------
  void ReportMatchmakeFunctionError( void );

  //------------------------------------------------------------------
  /**
   * @brief コミュニティーNEX関数キャンセル報告処理
   */
  //------------------------------------------------------------------
  void ReportMatchmakeCanceled( void );

  //------------------------------------------------------------------
  /**
   * @brief  コミュニティー関連のエラー報告処理
   *
   * @param[in] result 実行結果
   */
  //------------------------------------------------------------------
  void ReportCommunityError( const nn::nex::qResult& result );

  //------------------------------------------------------------------
  /**
   * @brief コミュニティーNEX関数実行エラー報告処理
   */
  //------------------------------------------------------------------
  void ReportCommunityFunctionError( void );

  //------------------------------------------------------------------
  /**
   * @brief コミュニティーNEX関数キャンセル報告処理
   */
  //------------------------------------------------------------------
  void ReportCommunityCanceled( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief  非同期処理の完了検知用関数
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   */
  //------------------------------------------------------------------
  void OnCreateSessionCompletion(nn::nex::CallContext *pContext);
  void OnOpenSessionCompletion(nn::nex::CallContext *pContext);
  void OnSearchSessionCompletion(nn::nex::CallContext *pContext);
  void OnJoinSessionCompletion(nn::nex::CallContext *pContext);
  void OnAutoMatchmakeCompletion(nn::nex::CallContext *pContext);
  void OnCreateCommunityCompletion(nn::nex::CallContext *pContext);
  void OnJoinCommunityCompletion(nn::nex::CallContext *pContext);
  void OnFindCommunityCompletion(nn::nex::CallContext *pContext);
  void OnLeaveCommunityCompletion(nn::nex::CallContext *pContext);
  void OnEndParticipationCompletion(nn::nex::CallContext *pContext);
  void OnDeleteParticipationCompletion(nn::nex::CallContext *pContext);
  void OnUpdateSessionHostCompletion(nn::nex::CallContext *pContext);
  void OnSimpleMatchmakeCompletion(nn::nex::CallContext *pContext);
  void OnEntrySimpleMatchmakeCompletion(nn::nex::CallContext *pContext);
  void OnCancelSimpleMatchmakeEntryCompletion(nn::nex::CallContext *pContext);

private:
  bool                                                     m_isInit;            //!< 初期化フラグ
  bool                                                     m_isAccess;          //!< 通信中フラグ
  bool                                                     m_isEncryption;      //!< P2Pセッションの暗号化フラグ
  //
  GatheringID                                              m_gatheringID;       //!< 参加しているギャザリングID
  GatheringID                                              m_commGatheringID;   //!< 参加しているコミュニティ用ギャザリングID
  u32                                                      m_joinMaxNum;        //!< 参加可能最大人数
  //
  NexMatchmakeListener*                                    m_matchListener;     //!< マッチメイク用リスナ
  NexSessionEventListener*                                 m_sessionListener;   //!< セッション用リスナ
  NexCommunityListener*                                    m_communityListener; //!< コミュニティ用リスナ
  //
  nn::nex::PrincipalID                                     m_ownerPID;          //!< オーナーのプリンシバルID
  nn::nex::NgsFacade*                                      m_fascade;           //!< ゲームサーバログイン情報
  nn::nex::ProtocolCallContext*                            m_context;           //!< プロトコルコンテキスト
  nn::nex::MatchmakeExtensionClient*                       m_matchingClient;    //!< マッチメイククライアント
  nn::nex::qList<nn::nex::GatheringHolder>*                m_gatheringList;     //!< ギャザリングリスト
  nn::nex::qList<nn::nex::MatchmakeSessionSearchCriteria>* m_criteriaList;      //!< 条件リスト
  nn::nex::qVector<nn::nex::qByte>*                        m_sessionKey;        //!< セッションキー
  nn::nex::MatchmakeSessionSearchCriteria*                 m_criteriaInfo;      //!< 条件情報
  nn::nex::GatheringHolder*                                m_sessionInfo;       //!< セッション情報
  nn::nex::GatheringHolder*                                m_autoSessionInfo;   //!< 自動セッション情報
  nn::nex::qList<nn::nex::Community>*                      m_communityList;     //!< コミュニティーリスト
  // 
  // SimpleMatchmake用
  nn::nex::qBool                                           m_isFound;           //!< 相手が見つかったかどうか
  nn::nex::SimpleMatchmakeHostInfo*                        m_pSimpleMatchmakeHostInfo; //!< P2P接続の為の情報
  nn::nex::qBool                                           m_isSucceed;         //!< キャンセル成功したか
  bool                                                     m_isSimpleMatchmake; //!< シンプルマッチメイクモードか

  //
  nn::nex::HostMigrationCallback* m_pHostMigrationCallback;//!<ホストマイグレーションコールバッククラス

#if GFL_DEBUG
  bool  m_debugIsClearCache;
  u32   m_debugMatchmakeGroup;    // デバッグ用マッチング分けたい
#endif // GFL_DEBUG

};


} // nex
} // gflnet

#endif  // __GFLNET_NEX_MATCH_MAKING_H__
#endif
