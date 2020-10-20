// ============================================================================
/*
 * @file gflnet2_CTR_CecRecvMessageBox.h
 * @brief すれ違い通信の受信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_CECRECVMESSAGEBOX_H_INCLUDE )
#define GFLNET2_CTR_CECRECVMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <gflnet2/include/cec/gflnet2_ICecRecvMessageBox.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecControlObject.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_ICecControlObjectRegister;


class CTR_CecRecvMessageBox : public ICecRecvMessageBox, public CTR_CecControlObject
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_CecRecvMessageBox );

public:
  CTR_CecRecvMessageBox( CTR_ICecControlObjectRegister* pCecControlObjectRegister );
  virtual~CTR_CecRecvMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  初期化処理を行います。
   *         内部で持つ受信データのバッファの確保等を行っています。
   * @param  pHeap 内部のバッファを確保する為のヒープです。
   * @param  pInitParam 内部で持つバッファのサイズ等の情報です。
   */
  // --------------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* pHeap, const CecSystemInitParam* pInitParam );

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データ数を取得します。
   * @return 受信ボックス内にある受信データ数
   */
  // --------------------------------------------------------------------------
  virtual s32 GetMessageCount();

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データ数を取得します。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void GetMessageCountAsync( ICecMessageBoxEventListener* pEventListener );

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

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データを読み込みます。
   * @param  recvPacketCount 読み込んだ受信データの数
   * @return 読み込んだ受信データ
   */
  // --------------------------------------------------------------------------
  virtual const gflnet2::cec::CecRecvPacket* ReadMessage( u32& recvPacketCount );

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データを読み込みます。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void ReadMessageAsync( ICecMessageBoxEventListener* pEventListener );

  // --------------------------------------------------------------------------
  /**
   * @brief  受信データの読み込みに成功した時にコールされるイベントハンドラです。
   * @param  cecMessage 読み込んだ受信データ
   */
  // --------------------------------------------------------------------------
  virtual void OnReadMessageSuccess( nn::cec::Message& cecMessage );

  // --------------------------------------------------------------------------
  /**
   * @brief  受信データの読み込みに失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnReadMessageFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックスにある受信データを全て削除します。
   * @retval true 削除に成功
   * @retval false 削除に失敗
   */
  // --------------------------------------------------------------------------
  virtual bool DeleteAllMessage();

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックスにある受信データを全て削除します。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener );

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にあるデータの削除に成功した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteAllMessageSuccess();

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にあるデータの削除に失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteAllMessageFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  非同期で実行されたコマンドが終了した時にコールされるイベントハンドラです。
   * @param  eResultCode 実行結果です
   *         この実行結果を参照してアプリ側へ成功、失敗の通知を行います。
   */
  // --------------------------------------------------------------------------
  virtual void OnCommandResult( ResultCode eResultCode );

private:
  gflnet2::cec::Week convertToWeek( nn::fnd::Week nnWeek );

  CTR_ICecControlObjectRegister*    m_pCecControlObjectRegister;
  ICecMessageBoxEventListener*      m_pCecMessageBoxEventListener;

  CecRecvPacket*                    m_pRecvPackets;
  u32                               m_recvPacketCount;
  CecSystemInitParam                m_InitParam;

  s32                               m_messageCount;
  bool                              m_bDeleteAllMessageResult;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_CECRECVMESSAGEBOX_H_INCLUDE
#endif
