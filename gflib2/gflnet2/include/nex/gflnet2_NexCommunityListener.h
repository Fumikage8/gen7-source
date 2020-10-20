#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexCommunityListener.h
 * @brief  コミュニティ関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_COMMUNITY_LISTENER_H__
#define __GFLNET_NEX_COMMUNITY_LISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief コミュニティ関連通知イベントハンドラ
 *
 * @note コミュニティ関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexCommunityListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexCommunityListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
  * @brief コミュニティの作成完了通知
  *
  * @param[in] gatheringId 作成されたセッションのギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnCommunityCreateSuccess( const nn::nex::GatheringID& gatheringId ){};

  //------------------------------------------------------------------
  /**
  * @brief コミュニティの検索完了通知
  *
  * @param[in] communityList 検索されたコミュニティリスト
  */
  //------------------------------------------------------------------
  virtual void OnCommunitySearchSuccess( nn::nex::qList<nn::nex::Community>* communityList ){};

  //------------------------------------------------------------------
  /**
  * @brief コミュニティへの参加完了通知
  *
  * @param[in] gatheringId 参加したギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnCommunityJoinSuccess( const nn::nex::GatheringID& gatheringId ){};

  //------------------------------------------------------------------
  /**
  * @brief コミュニティの退出完了通知
  *
  * @param[in] gatheringId 退出したギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnCommunityLeaveSuccess( const nn::nex::GatheringID& gatheringId ){};

  //------------------------------------------------------------------
  /**
  * @brief コミュニティ関連のキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnNexCommunityCanceled( void ){};

  //------------------------------------------------------------------
  /**
  * @brief コミュニティ関連のエラー終了通知
  *
  * @param[in] result     エラー理由
  * @param[in] errorCode  resultからエラーコードに変換した値
  *
  * @note 以下のエラーをハンドリングする必要がある
  *       QERROR(RendezVous, PersistentGatheringCreationMax) 永続ギャザリングの登録上限を超過。
  *       QERROR(RendezVous, PersistentGatheringParticipationMax) 永続ギャザリングの参加上限を超過。
  *       QERROR(RendezVous, InvalidGatheringPassword) パスワード付永続ギャザリングでパスワードが合致していない。
  *       QERROR(RendezVous, WithoutParticipationPeriod) 参加可能期間外。
  */
  //------------------------------------------------------------------
  virtual void OnNexCommunityError( const nn::nex::qResult& result, u32 errorCode )=0;

};


} // nex
} // gflnet



#endif // __GFLNET_NEX_COMMUNITY_LISTENER_H__
#endif
