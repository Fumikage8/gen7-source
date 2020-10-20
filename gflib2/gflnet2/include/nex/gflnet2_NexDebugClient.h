#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexDebugClient.h
 *
 * @brief  nn::nex::DebugClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================

#ifdef ENABLE_NEX_DEBUG_CLIENT

#ifndef __GFLNET_NEX_DEBUG_CLIENT_H__
#define __GFLNET_NEX_DEBUG_CLIENT_H__
#pragma once


#include <nex.h>
#include <nex_P2p.h>

#include <nn/friends.h>
#include <RendezVous/Services/Messaging.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexDebugClientListener.h>


namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/**
*  @brief デバッグ補助機能（DebugClientアクセス）用クラス
*
*  @attention 事前にゲームサーバへログインしておくこと
*  @attention 全ての通信関数は排他制御となる為、同時に実行することはできない
*
**/
//------------------------------------------------------------------
class NexDebugClient
{
  GFL_FORBID_COPY_AND_ASSIGN(NexDebugClient); //コピーコンストラクタ＋代入禁止

public:
  NexDebugClient( void );
  virtual ~NexDebugClient();

public:
  //------------------------------------------------------------------
  /**
  * @brief  DebugClientの初期化
  *
  * @param[in]  credentials     ゲームサーバーログイン情報
  *
  * @return 初期化に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool Initialize( nn::nex::Credentials* credentials );

  //------------------------------------------------------------------
  /**
  * @brief  DebugClientの終了
  *
  * @attention 必ず事前にIsConnecting()がfalseであることを確認してください
  */
  //------------------------------------------------------------------
  void Finalize( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief DebugClient用リスナの登録
  *
  * @param[in] listener 登録対象のメッセージ用リスナ。
  *
  * @return 登録に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool RegistDebugClientListener( NexDebugClientListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief DebugClient用リスナの解除
  *
  * @param[in] listener 削除対象のメッセージ用リスナ。
  */
  //------------------------------------------------------------------
  void UnregistDebugClientListener( NexDebugClientListener* listener );

public:
  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのAPIレコード機能設定を変更する(非同期通信）
  *
  * @param[in] isEnable APIレコード機能を有効化するならtrueを指定
  *
  * @note  正常終了時は「NexDebugClientListener::OnNexDebugClientChangeStatusSuccess()」がコールされる。
  * @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool ChangeApiRecorderStatus( bool isEnable );

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのAPIレコード機能設定値を取得する(非同期通信）
  *
  * @note  正常終了時は「NexDebugClientListener::OnNexDebugClientGetStatusSuccess()」がコールされる。
  * @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetApiRecorderStatus( void );

  //------------------------------------------------------------------
  /**
  * @brief オートマッチの連続マッチ有効設定を変更する(非同期通信）
  *
  * @param[in] isEnable オートマッチで過去参加したマッチメイクセッションを参加対象外とするかどうか。<p>
  *            @arg true  過去に参加したマッチメイクセッションに参加しない(ゲームサーバのデフォルト設定)。<p>
  *            @arg false 過去に参加したマッチメイクセッションに参加する。
  *
  * @note  正常終了時は「NexDebugClientListener::OnNexDebugClientChangeExecludeJoinedMatchmakeSuccess()」がコールされる。
  * @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool ChangeExcludeJoinedMatchmake( bool isEnable );

  //------------------------------------------------------------------
  /**
  * @brief オートマッチの連続マッチ有効設定値を取得する
  *
  * @note  正常終了時は「NexDebugClientListener::OnNexDebugClientGetExecludeJoinedMatchmakeSuccess()」がコールされる。
  * @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetExecludeJoinedMatchmake( void );

  //------------------------------------------------------------------
  /**
  * @brief API呼び出し情報を取得する(非同期通信）
  *
  * @param[in] principalId 取得対象のプリンシパルID
  * @param[in] sinceTime   取得開始時刻
  * @param[in] untilTime   取得終了時刻
  *
  * @note  正常終了時は「NexDebugClientListener::OnNexDebugClientGetApiCallsSuccess()」がコールされる。
  * @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetApiCalls( nn::nex::PrincipalID principalId,
                    nn::nex::DateTime sinceTime=nn::nex::DateTime::Never,
                    nn::nex::DateTime untilTime=nn::nex::DateTime::Never );

  //------------------------------------------------------------------
  /**
  * @brief APIコール詳細情報を取得する(非同期通信）
  *
  * @param[in] principalId     取得対象のプリンシパルID
  * @param[in] sinceTime       取得開始時刻
  * @param[in] untilTime       取得終了時刻
  * @param[in] isLimitOverOnly 呼び出し頻度制限の目安を超えているAPIのみ取得するならtrueを指定
  *
  * @note  正常終了時は「NexDebugClientListener::OnNexDebugClientGetApiCallSummarySuccess()」がコールされる。
  * @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetApiCallSummary( nn::nex::PrincipalID principalId,
                          nn::nex::DateTime sinceTime=nn::nex::DateTime::Never,
                          nn::nex::DateTime untilTime=nn::nex::DateTime::Never,
                          bool isLimitOverOnly = true );

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
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   */
  //------------------------------------------------------------------
  void ChangeApiRecorderStatusCompletion(nn::nex::CallContext *pContext);
  void OnGetApiRecorderStatusCompletion(nn::nex::CallContext *pContext);
  void OnChangeExcludeJoinedMatchmakeCompletion(nn::nex::CallContext *pContext);
  void OnGetExecludeJoinedMatchmakeCompletion(nn::nex::CallContext *pContext);
  void OnGetApiCallsCompletion(nn::nex::CallContext *pContext);
  void OnGetApiCallSummaryCompletion(nn::nex::CallContext *pContext);


private:
  bool LockConnection( void );
  void UnlockConnection( void );
  void ReportFunctionError( void );
  void ReportCanceled( void );
  void ReportError( const nn::nex::qResult& result );

private:
  static const u32      LISTENER_NUM = 4;

private:
  bool                                m_isInit;                   //!< 初期化フラグ
  bool                                m_isSending;                //!< 送信中フラグ
  bool                                m_isEnableApiRecorder;      //!< APIレコーダー有効フラグ
  bool                                m_isAvoidPreviousGathering; //!< オートマッチメイク連続参加禁止フラグ
  //
  nn::nex::DebugClient*               m_debugClient;              //!< Debugクライアント
  nn::nex::ProtocolCallContext*       m_context;                  //!< カレントコールコンテキスト
  //
  nn::nex::qList<nn::nex::ApiCallData>*     m_apiCallData;        //!< APIコール情報
  nn::nex::qList<nn::nex::ApiCallSummary>*  m_apiCallSummary;     //!< APIコール集計情報
  //
  NexDebugClientListener*             m_listener[LISTENER_NUM];   //!< メッセージ用リスナ
};

} //nex
} //gflnet

#endif  // __GFLNET_NEX_DEBUG_CLIENT_H__

#endif
#endif
