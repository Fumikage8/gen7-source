#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexRankingClient.h
 *
 * @brief  nn::nex::RankingClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_RANKING_CLIENT_H__
#define __GFLNET_NEX_RANKING_CLIENT_H__
#pragma once

#include <nn.h>
#include <nex.h>

#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexRankingClientListener.h>
#include <gflnet2/include/friends/gflnet2_FriendManager.h>

namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/**
*  @brief ランキング（RankingClientアクセス）用クラス
*
*  @attention 事前にゲームサーバへログインしておくこと
*  @attention 各非同期通信は排他制御となる為、同時実行は不可。
*
*  @note  本クラス関数において[nn::nex::UniqueId]を指定することで
*         セーブデータ毎にランキング情報を管理することが可能。
*
**/
//------------------------------------------------------------------
class NexRankingClient
{
  GFL_FORBID_COPY_AND_ASSIGN(NexRankingClient); //コピーコンストラクタ＋代入禁止

public:
  NexRankingClient( void );
  virtual ~NexRankingClient();

public:
  //------------------------------------------------------------------
  /**
  * @brief  RankingClientの初期化
  *
  * @param[in]  credentials     ゲームサーバーログイン情報
  *
  * @return 初期化に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool Initialize( nn::nex::Credentials* credentials );

  //------------------------------------------------------------------
  /**
  * @brief  RankingClientの終了
  *
  * @attention 必ず事前にIsConnecting()がfalseであることを確認してください
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
  * @brief  RankingClientインスタンスの取得
  */
  //------------------------------------------------------------------
  nn::nex::RankingClient* GetRankingClient( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief ランキング用リスナの登録
  *
  * @param[in] listener 登録対象のランキング用リスナ。
  *
  * @return 登録に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool RegistRankingClientListener( NexRankingClientListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief ランキング用リスナの解除
  *
  * @param[in] listener 削除対象のランキング用リスナ。
  */
  //------------------------------------------------------------------
  void UnregistRankingClientListener( NexRankingClientListener* listener );

public:
  //------------------------------------------------------------------
  /**
  * @brief スコア情報を登録する(非同期関数)
  *
  * @param[in] scoreData    スコア情報
  * @param[in] nexUniqueId  NEXユニークID（省略可能）
  *
  * @attention 自分と同じプリンシパルIDのスコア情報のみ登録可能。
  *
  * @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingDeleteScoreFinish()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool UploadScore( const nn::nex::RankingScoreData& scoreData, nn::nex::UniqueId nexUniqueId );

  //------------------------------------------------------------------
  /**
  * @brief スコア情報を削除する(非同期関数)
  *
  * @param[in] categoryId   削除対象カテゴリ
  * @param[in] nexUniqueId  NEXユニークID（省略可能）
  *
  * @attention 自分と同じプリンシパルIDのスコア情報のみ削除可能。
  *
  * @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingDeleteScoreFinish()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool DeleteScore( u32 categoryId, nn::nex::UniqueId nexUniqueId = nn::nex::INVALID_UNIQUEID );

  //------------------------------------------------------------------
  /**
  * @brief 共通データを登録する(非同期関数)
  *
  * @param[in] pCommonData    共通情報へのポインタ
  * @param[in] commonDataSize 共通情報データサイズ(最大255バイト）
  * @param[in] nexUniqueId    NEXユニークID（省略可能）
  *
  * @attention 自分と同じプリンシパルIDの共通データのみ登録可能。
  *
  * @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingUploadCommonDataFinish()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool UploadCommonData( const void* pCommonData, qByte commonDataSize,
                         nn::nex::UniqueId nexUniqueId = nn::nex::INVALID_UNIQUEID );

  //------------------------------------------------------------------
  /**
  * @brief 共通データを削除する(非同期関数)
  *
  * @param[in] nexUniqueId  NEXユニークID（省略可能）
  *
  * @attention 自分と同じプリンシパルIDのスコア情報のみ削除可能。
  *
  * @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingDeleteScoreFinish()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool DeleteCommonData( nn::nex::UniqueId nexUniqueId = nn::nex::INVALID_UNIQUEID );

  //------------------------------------------------------------------
  /**
  * @brief スコアの属性値を変更する(非同期関数)
  *
  * @param[in] categorId    変更するカテゴリID
  * @param[in] changeParam  変更詳細情報
  * @param[in] nexUniqueId  NEXユニークID（省略可能）
  *
  * @attention 自分と同じプリンシパルIDのスコアの属性値のみ変更可能。
  *
  * @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingChageAttributesFinish()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool ChangeAttributes( u32 categorId, const nn::nex::RankingChangeAttributesParam& changeParam,
                         nn::nex::UniqueId nexUniqueId = nn::nex::INVALID_UNIQUEID );

  //------------------------------------------------------------------
  /**
  * @brief 共通データを取得する(非同期関数)
  *
  * @param[in] nexUniqueId  NEXユニークID（省略可能）
  *
  * @attention 自分と同じプリンシパルIDのスコアの属性値のみ変更可能。
  *
  * @note  正常終了時は「NexRankingClientListener::OnNexRankingGetCommonDataSuccess()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetCommonData( nn::nex::UniqueId nexUniqueId = nn::nex::INVALID_UNIQUEID );

  //------------------------------------------------------------------
  /**
  * @brief ランキングリストを取得する(非同期関数)
  *
  * @param[in] rankingMode  ランキングモード
  * @param[in] categoryId   取得対象カテゴリID
  * @param[in] orderParam   順位計算用パラメータ
  * @param[in] nexUniqueId  NEXユニークID（省略可能）
  * @param[in] principalId  プリンシバルID（省略可能）
  *
  * @note  正常終了時は「NexRankingClientListener::OnNexRankingGetListDataSuccess()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @attention 引数設定が複雑なので、nn::nex::nn::nex::RankingClient::GetRanking()のAPIリファレンスを
  * @attention 必ず確認しておくこと！
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetRankingList( nn::nex::RankingConstants::RankingMode rankingMode,
                       u32 categoryId, const nn::nex::RankingOrderParam& orderParam,
                       nn::nex::UniqueId nexUniqueId = nn::nex::INVALID_UNIQUEID,
                       gflnet2::friends::PrincipalId principalId = INVALID_PRINCIPALID );

  //------------------------------------------------------------------
  /**
  * @brief 指定したスコアを元にした予測順位を取得する(非同期関数)
  *
  * @param[in] categoryId   取得対象カテゴリID
  * @param[in] orderParam   順位計算用パラメータ
  * @param[in] score        スコア
  * @param[in] nexUniqueId  NEXユニークID（省略可能）
  * @param[in] principalId  プリンシバルID（省略可能）
  *
  * @note  正常終了時は「NexRankingClientListener::OnNexRankingGetApproxOrderSuccess()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @attention 引数設定が複雑なので、nn::nex::nn::nex::RankingClient::GetApproxOrder()のAPIリファレンスを
  * @attention 必ず確認しておくこと！
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetApproxOrder( u32 categoryId, const nn::nex::RankingOrderParam& orderParam, u32 score,
                       nn::nex::UniqueId nexUniqueId = nn::nex::INVALID_UNIQUEID,
                       gflnet2::friends::PrincipalId principalId = INVALID_PRINCIPALID );

  //------------------------------------------------------------------
  /**
  * @brief スコア統計情報を取得する(非同期関数)
  *
  * @param[in] categoryId   取得対象カテゴリID
  * @param[in] orderParam   順位計算用パラメータ
  * @param[in] flags        取得する統計値を指定するフラグ。「nn::nex::RankingConstants::StatsFlag」の論理和
  *
  * @note  正常終了時は「NexRankingClientListener::OnNexRankingGetStatsSuccess()」がコールされる。
  * @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetStats( u32 categoryId, const nn::nex::RankingOrderParam& orderParam, u32 flags );

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
  // 以下はコールバック関数にのみ開示するため、ユーザー使用禁止！
  //------------------------------------------------------------------
  void OnUploadScoreCompletion(nn::nex::CallContext *pContext);
  void OnUploadCommonDataCompletion(nn::nex::CallContext *pContext);
  void OnDeleteScoreCompletion(nn::nex::CallContext *pContext);
  void OnDeleteCommonDataCompletion(nn::nex::CallContext *pContext);
  void OnChangeAttributesCompletion(nn::nex::CallContext *pContext);
  void OnGetCommonDataCompletion(nn::nex::CallContext *pContext);
  void OnGetRankingListCompletion(nn::nex::CallContext *pContext);
  void OnGetStatsCompletion(nn::nex::CallContext *pContext);
  void OnGetApproxOrderCompletion(nn::nex::CallContext *pContext);

private:
  //------------------------------------------------------------------
  /**
  * @brief 通信ロックを行う
  *
  * @return すでに通信接続ロック中であればfalseを返却
  */
  //------------------------------------------------------------------
  bool LockConnection( void );

  //------------------------------------------------------------------
  /**
  * @brief 通信ロックを解除する
  */
  //------------------------------------------------------------------
  void UnlockConnection( void );

  //------------------------------------------------------------------
  /**
   * @brief エラー報告処理
   *
   * @param result 実行結果
   */
  //------------------------------------------------------------------
  void ReportError( const nn::nex::qResult& result );

  //------------------------------------------------------------------
  /**
   * @brief NEX関数実行エラー報告処理
   *
   * @param result 実行結果
   */
  //------------------------------------------------------------------
  void ReportFunctionError( void );

  //------------------------------------------------------------------
  /**
   * @brief キャンセル報告処理
   */
  //------------------------------------------------------------------
  void ReportCanceled( void );

private:
  static const u32                    LISTENER_NUM = 4;         //!< リスナ最大登録可能数

private:
  bool                                m_isInit;                 //!< 初期化フラグ
  bool                                m_isAccess;               //!< 通信中フラグ
  u32                                 m_appproxOrder;           //!< 推定順位
  u32                                 m_categoryId;             //!< カテゴリID
  //
  nn::nex::RankingClient*             m_rankingClient;          //!< Rankingクライアント
  nn::nex::RatingClient*             m_ratingClient;          //!< Rankingクライアント
  nn::nex::ProtocolCallContext*       m_context;                //!< カレントコールコンテキスト
  nn::nex::qVector<qByte>*            m_commonData;             //!< 共通情報
  nn::nex::RankingResult*             m_rankResultInf;          //!< ランキングリスト情報
  nn::nex::RatingRankData*            m_rankData;
  nn::nex::RankingStats*              m_rankStats;              //!< スコア統計情報
  //
  NexRankingClientListener*           m_listener[LISTENER_NUM]; //!< ランキング用リスナ
};

} // nex
} // gflnet

#endif  // __GFLNET_NEX_RANKING_CLIENT_H__
#endif
