#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexRankingClientListener.h
 * @brief  RankingClient関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_RANKING_CLIENT_LISTENER_H__
#define __GFLNET_NEX_RANKING_CLIENT_LISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief ランキング通知イベントハンドラ
 *
 * @note ランキング関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexRankingClientListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexRankingClientListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
  * @brief スコアアップロードの成功通知
  *
  * @param[in] category  対象カテゴリID
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingUploadScoreSuccess( const u32 category ){};

  //------------------------------------------------------------------
  /**
  * @brief スコア削除の成功通知
  *
  * @param[in] category  対象カテゴリID
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingDeleteScoreSuccess( const u32 category ){};

  //------------------------------------------------------------------
  /**
  * @brief 共通情報アップロードの成功通知
  *
  * @param[in] result    実行結果
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingUploadCommonDataSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief 共通情報削除の成功通知
  *
  * @param[in] result    実行結果
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingDeleteCommonDataSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief スコア属性変更の成功通知
  *
  * @param[in] category  対象カテゴリID
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingChageAttributesSuccess( const u32 category ){};

  //------------------------------------------------------------------
  /**
  * @brief スコア統計情報の取得成功通知
  *
  * @param[in] status スコアの統計
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingGetStatsSuccess( const nn::nex::RankingStats& status ){};

  //------------------------------------------------------------------
  /**
  * @brief ランキング共通データの取得成功通知
  *
  * @param[in] data ランキング共通データ
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingGetCommonDataSuccess( const nn::nex::qVector<qByte>& data ){};

  //------------------------------------------------------------------
  /**
  * @brief ランキングリストデータの取得成功通知
  *
  * @param[in] data ランキングリストデータ
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingGetListDataSuccess( const nn::nex::RatingRankData& data ){};

  //------------------------------------------------------------------
  /**
  * @brief 予想順位の取得成功通知
  *
  * @param[in] order 予想順位
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingGetApproxOrderSuccess( const u32 order ){};

  //------------------------------------------------------------------
  /**
  * @brief NexGlobalTradeStationClient通信のキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingCanceled( void ){};

  //------------------------------------------------------------------
  /**
  * @brief NexGlobalTradeStationClient通信のエラー通知
  *
  * @param[in] result     実行結果
  * @param[in] errorCode  エラーアプレット用エラーコード
  *
  * @note 以下のエラーをハンドリングする必要がある。
  *         QERROR(Ranking, NotFound) データが見つからない
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingClientError( const nn::nex::qResult& result, u32 errorCode ) = 0;
};


} // nex
} // gflnet



#endif // __GFLNET_NEX_RANKING_CLIENT_LISTENER_H__
#endif
