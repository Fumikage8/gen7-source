#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexMatchmakeListener.h
 * @brief  マッチメイクセッション関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_MATCHMAKE_LISTENER_H__
#define __GFLNET_NEX_MATCHMAKE_LISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief マッチメイクセッション関連通知イベントハンドラ
 *
 * @note マッチメイクセッション関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexMatchmakeListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexMatchmakeListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションの作成完了通知
  *
  * @param[in] gatheringId 作成されたセッションのギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingCreateSuccess( const nn::nex::GatheringID& gatheringId ){};

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションの検索完了通知
  *
  * @param[in] gatheringList 検索されたセッションのギャザリングIDリスト
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingSearchSuccess( nn::nex::qList<nn::nex::GatheringHolder>* gatheringList ){};

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションへの参加完了通知
  *
  * @param[in] gatheringId 参加したギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingJoinSuccess( const nn::nex::GatheringID& gatheringId ){};

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションの正常終了通知。
  *        自身がマッチメイクセッションから正常離脱した旨を検知できる。
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingEndSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションのオープン正常終了通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingOpenSessionSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションのオーナー変更通知
  *
  * @param[in] newOwnerId 新たにオーナーとなったユーザーのプリンシバルID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingOwnerChange( nn::nex::PrincipalID newOwnerId ){};

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションの削除通知
  *
  * @param[in] gatheringId 削除されたセッションのギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingUnregisterd( nn::nex::GatheringID gatheringId ){};

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションのキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingCanceled( void ){};

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
  *
  * @note 以下のエラーをハンドリングする必要がある
  *        QERROR(RendezVous, SessionVoid) ギャザリングが存在しない。
  *        QERROR(RendezVous, SessionFull) すでに最大参加人数に達している。
  *        QERROR(RendezVous, SessionClosed) ギャザリングが締め切られている。
  */
  //------------------------------------------------------------------
  virtual void OnNexMatchmakeError( const nn::nex::qResult& result, u32 errorCode ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief ホスト更新処理の成功通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingUpdateSessionHostSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief SimpleMatchmake処理の成功通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingSimpleMatchmakeSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief EntrySimpleMatchmake処理の成功通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingEntrySimpleMatchmakeSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief CancelSimpleMatchmakeEntry処理の成功通知
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingCancelSimpleMatchmakeEntrySuccess( void ){};


};


} // nex
} // gflnet



#endif // __GFLNET_NEX_MATCHMAKE_LISTENER_H__
#endif
