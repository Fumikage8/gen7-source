﻿#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexMessageClient.h
 *
 * @brief  nn::nex::MessageClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_MESSAGE_CLIENT_H__
#define __GFLNET_NEX_MESSAGE_CLIENT_H__
#pragma once


#include <nex.h>
#include <nex_P2p.h>

#include <nn/friends.h>
#include <RendezVous/Services/Messaging.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexMessageClientListener.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_StrSys.h>

GFL_NAMESPACE_BEGIN(gflnet2) 
GFL_NAMESPACE_BEGIN(nex)

//------------------------------------------------------------------
/**
*  @brief インスタントメッセージ（MessageClientアクセス）用クラス
*
*  @attention 事前にゲームサーバへログインしておくこと
*  @attention 全ての通信関数は排他制御となる為、同時に実行することはできない
*
**/
//------------------------------------------------------------------
class NexMessageClient : public nn::nex::MessagingNotificationHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(NexMessageClient); //コピーコンストラクタ＋代入禁止

public:
  NexMessageClient( void );
  virtual ~NexMessageClient();

public:
  //------------------------------------------------------------------
  /**
  * @brief  MessageClientの初期化
  *
  * @param[in]  credentials     ゲームサーバーログイン情報
  *
  * @return 初期化に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool Initialize( nn::nex::Credentials* credentials );

  //------------------------------------------------------------------
  /**
  * @brief  MessageClientの終了
  *
  * @attention 必ず事前にIsConnecting()がfalseであることを確認してください
  */
  //------------------------------------------------------------------
  void Finalize( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief メッセージ用リスナの登録
  *
  * @param[in] listener 登録対象のメッセージ用リスナ。
  *
  * @return 登録に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool RegistMessageClientListener( NexMessageClientListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief メッセージ用リスナの解除
  *
  * @param[in] listener 削除対象のメッセージ用リスナ。
  */
  //------------------------------------------------------------------
  void UnregistMessageClientListener( NexMessageClientListener* listener );

public:
  //------------------------------------------------------------------
  /**
  * @brief 指定ユーザーにメッセージを送信する(非同期通信）
  *
  * @param[in] pid         送信対象ユーザーのプリンシバルID
  * @param[in] subject     タイトル文字列。文字列は内部ワークにコピーされる。
  * @param[in] messageBody 送信するメッセージ文字列。文字列は内部ワークにコピーされる。
  *
  * @note  正常終了時は「NexMessageClientListener::OnNexMessageSendingSuccess()」がコールされる。
  * @note  エラー時は「NexMessageClientListener::OnNexMessageClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool SendInstant( const nn::nex::PrincipalID pid, const gfl2::str::STRCODE * subject, const gfl2::str::STRCODE* messageBody );


  //------------------------------------------------------------------
  /**
  * @brief 複数指定ユーザーにメッセージを送信する
  */
  //------------------------------------------------------------------
  bool SendInstantMultiTarget( const nn::nex::PrincipalID* pidArray, const gfl2::str::STRCODE* subject, const gfl2::str::STRCODE* messageBody );

  //------------------------------------------------------------------
  /**
  * @brief マッチメイク参加ユーザー（自分も含む）にメッセージを送信する（非同期通信）
  *
  * @param[in] gid    送信対象マッチメイクのギャザリングID
  * @param[in] subject     タイトル文字列。文字列は内部ワークにコピーされる。
  * @param[in] messageBody 送信するメッセージ文字列。文字列は内部ワークにコピーされる。
  *
  * @note  正常終了時は「NexMessageClientListener::OnNexMessageSendingSuccess()」がコールされる。
  * @note  エラー時は「NexMessageClientListener::OnNexMessageClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool SendChat( const nn::nex::GatheringID gid, const gfl2::str::STRCODE* subject, const gfl2::str::STRCODE* messageBody );

  //------------------------------------------------------------------
  /**
  * @brief 通信が可能か判定
  *
  * @return 通信可能ならtrueを返却する
  */
  //------------------------------------------------------------------
  bool IsEnableSending( void ) const;

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
   * @brief  メッセージ送信非同期処理の完了通知検知用関数
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void OnSendingCompletion(nn::nex::CallContext *pContext);

  //------------------------------------------------------------------
  /**
   * @brief  MessagingNotificationHandler用Message受信イベント
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in]  hMsg 受信メッセージ
   */
  //------------------------------------------------------------------
  virtual void DeliverInstantMessage( const nn::nex::DataHolder &hMsg );


private:
  void SetupMessageHolder( const gfl2::str::STRCODE* subject, const gfl2::str::STRCODE* messageBody );
  bool LockConnection( void );
  void UnlockConnection( void );
  void ReportCanceled( void );
  void ReportFunctionError( void );
  void ReportError( const nn::nex::qResult& result );

private:
  static const u32      LISTENER_NUM = 4;
private:
  bool                                m_isInit;                 //!< 初期化フラグ
  bool                                m_isSending;              //!< 送信中フラグ
  //
  nn::nex::MessagingClient*           m_messageClient;          //!< メッセージクライアント
  nn::nex::ProtocolCallContext*       m_context;                //!< カレントコールコンテキスト
  nn::nex::UserMessageDataHolder*     m_messageHolder;          //!< メッセージホルダ
  nn::nex::String*                    m_messageSubject;         //!< 送信タイトル文字列
  nn::nex::String*                    m_messageBody;            //!< 送信メッセージ文字列
  //
  NexMessageClientListener*           m_listener[LISTENER_NUM]; //!< メッセージ用リスナ
};

GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(gflnet2)

#endif  // __GFLNET_NEX_MESSAGE_CLIENT_H__
#endif
