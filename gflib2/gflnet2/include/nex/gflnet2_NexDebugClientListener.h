﻿#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexDebugClientListener.h
 * @brief  DebugClient関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifdef ENABLE_NEX_DEBUG_CLIENT

#ifndef __GFLNET_NEX_DEBUG_CLIENT_LISTENER_H__
#define __GFLNET_NEX_DEBUG_CLIENT_LISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>

#include <RendezVous/Services/Jugem/Debug/src/Client/DebugClient.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2) 
GFL_NAMESPACE_BEGIN(nex)

//------------------------------------------------------------------
/*!
 * @brief デバッグ補助機能通信用イベントハンドラ
 *
 * @note NexDebugClient関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexDebugClientListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexDebugClientListener); //コピーコンストラクタ＋代入禁止
public:
  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのAPI記録設定状態取得通信の完了通知
  *
  * @param[in] isApiRecorderEnabled API記録機能が有効状態ならtrueを返却
  */
  //------------------------------------------------------------------
  virtual void OnNexDebugClientGetStatusSuccess( bool isApiRecorderEnabled ){};

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのAPI記録設定変更通信の完了通知
  */
  //------------------------------------------------------------------
  virtual void OnNexDebugClientChangeStatusSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバ連続オートマッチ回避設定変更通信の完了通知
  *
  * @param[in] オートマッチで過去３回連続で参加したマッチメイクセッションを参加対象外とするならtrueを指定
  */
  //------------------------------------------------------------------
  virtual void OnNexDebugClientGetExecludeJoinedMatchmakeSuccess( bool isApiRecorderEnabled ){};

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバ連続オートマッチ回避設定取得通信の完了通知
  *
  * @param[in] isApiRecorderEnabled API記録機能が有効状態ならtrueを返却
  */
  //------------------------------------------------------------------
  virtual void OnNexDebugClientChangeExecludeJoinedMatchmakeSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのAPI記録設定変更通信の完了通知
  *
  * @param[in] pApiCallDataList API呼び出し履歴情報
  */
  //------------------------------------------------------------------
  virtual void OnNexDebugClientGetApiCallsSuccess( const nn::nex::qList<nn::nex::ApiCallData>* pApiCallDataList ){};

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバ連続オートマッチ回避設定取得通信の完了通知
  *
  * @param[in] pApiCallSummaryList API呼び出し履歴の集計情報
  */
  //------------------------------------------------------------------
  virtual void OnNexDebugClientGetApiCallSummarySuccess( nn::nex::qList<nn::nex::ApiCallSummary>* pApiCallSummaryList ){};

  //------------------------------------------------------------------
  /**
  * @brief NexDebugClient通信のキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnNexDebugClientCanceled( void ){};

  //------------------------------------------------------------------
  /**
  * @brief NexDebugClient通信のエラー通知
  *
  * @param[in] result     実行結果
  * @param[in] errorCode  エラーアプレット用エラーコード
  */
  //------------------------------------------------------------------
  virtual void OnNexDebugClientError( const nn::nex::qResult& result, u32 errorCode ) = 0;
};


GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_NEX_DEBUG_CLIENT_LISTENER_H__
#endif // ENABLE_NEX_DEBUG_CLIENT
#endif
