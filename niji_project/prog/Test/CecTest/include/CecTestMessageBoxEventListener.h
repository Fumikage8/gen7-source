// ============================================================================
/*
 * @file CecTestMessageBoxEventListener.h
 * @brief すれ違い通信のテスト用イベントリスナーです。
 *        すれ違い通信の状態を更新しています。
 * @date 2015.03.03
 */
// ============================================================================
#if PM_DEBUG

#if !defined( CECTESTMESSAGEBOXEVENTLISTENER_H_INCLUDED )
#define CECTESTMESSAGEBOXEVENTLISTENER_H_INCLUDED
#pragma once


#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/cec/gflnet2_ICecMessageBoxEventListener.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(CecTest)

class CecTestStateAccessor;

class CecTestMessageBoxEventListener : public gflnet2::cec::ICecMessageBoxEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN( CecTestMessageBoxEventListener );

public:
  CecTestMessageBoxEventListener( CecTestStateAccessor* pStateAccessor );
  virtual~CecTestMessageBoxEventListener();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスが作成されているか調べ終わった後にコールされるイベントハンドラです。
   * @param  bIsCreate [ true : 作成されている, false : 作成されていない ]
   */
  // --------------------------------------------------------------------------
  virtual void OnIsCreateMessageBox( const bool bIsCreate );

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスの作成に成功した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnCreateMessageBoxSuccess();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスの作成に失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnCreateMessageBoxFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスの削除に成功した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteMessageBoxSuccess();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスの削除に失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteMessageBoxFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  送受信ボックス内にあるデータの削除に成功した時にコールされるイベントハンドラです。
   * @param  eMessageBoxType メッセージボックスタイプです
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteAllMessageSuccess( const gflnet2::cec::MessageBoxType eMessageBoxType );

  // --------------------------------------------------------------------------
  /**
   * @brief  送受信ボックス内にあるデータの削除に失敗した時にコールされるイベントハンドラです。
   * @param  eMessageBoxType メッセージボックスタイプです
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteAllMessageFailure( const gflnet2::cec::MessageBoxType eMessageBoxType );

  // --------------------------------------------------------------------------
  /**
   * @brief  送信データの書き込みに成功した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnWriteMessageSuccess();

  // --------------------------------------------------------------------------
  /**
   * @brief  送信データの書き込みに失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnWriteMessageFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  受信データの読み込みに成功した時にコールされるイベントハンドラです。
   * @param  pCecRecvPackets 読み込んだ受信データ
   * @param  recvPacketCount 読み込んだ受信データの数
   */
  // --------------------------------------------------------------------------
  virtual void OnReadMessageSuccess( const gflnet2::cec::CecRecvPacket* pCecRecvPackets, const u32 recvPacketCount );

  // --------------------------------------------------------------------------
  /**
   * @brief  受信データの読み込みに失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnReadMessageFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データ数の取得に成功した時にコールされるイベントハンドラです。
   * @param  messageCount 受信ボックス内にある受信データ数
   */
  // --------------------------------------------------------------------------
  virtual void OnGetMessageCountSuccess( const s32 messageCount );

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データ数の取得に失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnGetMessageCountFailure();

private:
  CecTestStateAccessor*   m_pStateAccessor;

};


GFL_NAMESPACE_END(CecTest)
GFL_NAMESPACE_END(Test)

#endif // CECTESTMESSAGEBOXEVENTLISTENER_H_INCLUDED

#endif // PM_DEBUG
