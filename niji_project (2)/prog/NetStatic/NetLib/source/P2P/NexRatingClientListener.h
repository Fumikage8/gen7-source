//=============================================================================
/**
 * @file   NexRatingClientListener.h
 * @brief  RatingClient関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================
#ifndef __NEX_RATING_CLIENT_LISTENER_H__
#define __NEX_RATING_CLIENT_LISTENER_H__
#pragma once

#if defined(GF_PLATFORM_CTR)
#include <nex.h>
#endif // GF_PLATFORM_CTR
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)

//------------------------------------------------------------------
/*!
 * @brief RatingClient通知イベントハンドラ
 *
 * @note レーティング関連の通知を受け取る場合は本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexRatingClientListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexRatingClientListener); //コピーコンストラクタ＋代入禁止
public:
  //------------------------------------------------------------------
  /**
  * @brief Ratingセッション作成の成功通知
  *
  * @param[in] sessionKey 生成されたセッションキー
  */
  //------------------------------------------------------------------
#if defined(GF_PLATFORM_CTR)
  virtual void OnCreateRatingSessionSuccess( const nn::nex::qVector<qByte>& sessionKey ){}
#endif // GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
  * @brief Ratingセッション参加終了通知
  */
  //------------------------------------------------------------------
  virtual void OnJoinRatingSessionSuccess( void ){}

  //------------------------------------------------------------------
  /**
  * @brief Ratingサーバへの結果送信終了通知
  */
  //------------------------------------------------------------------
  virtual void OnRatingReportSuccess( void ){}

  //------------------------------------------------------------------
  /**
  * @brief Ratingサーバ通信のエラー通知
  *
  * @param[in] result    実行結果
  * @param[in] errorCode 通信エラーコード
  */
  //------------------------------------------------------------------
#if defined(GF_PLATFORM_CTR)
  virtual void OnNexRatingClientError( const nn::nex::qResult& result, u32 errorCode ) = 0;
#endif // GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
  * @brief Ratingサーバ通信のキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnNexRatingClientCanceled( void ){};
};

GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

#endif // __NEX_RATING_CLIENT_LISTENER_H__
