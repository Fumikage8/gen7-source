// ============================================================================
/*
 * @file gflnet2_CTR_CecSendMessageBox.h
 * @brief すれ違い通信の送信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_CECSENDMESSAGEBOX_H_INCLUDE )
#define GFLNET2_CTR_CECSENDMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <gflnet2/include/cec/gflnet2_ICecSendMessageBox.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecControlObject.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_ICecControlObjectRegister;


class CTR_CecSendMessageBox : public ICecSendMessageBox, public CTR_CecControlObject
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_CecSendMessageBox );

public:
  CTR_CecSendMessageBox( CTR_ICecControlObjectRegister* pCecControlObjectRegister );
  virtual~CTR_CecSendMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  初期化処理を行います。
   *         内部で持つ送信データのバッファの確保等を行っています。
   * @param  pHeap 内部のバッファを確保する為のヒープです。
   * @param  pInitParam 内部で持つバッファのサイズ等の情報です。
   */
  // --------------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* pHeap, const CecSystemInitParam* pInitParam );

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスにある送信データを全て削除します。
   * @retval true 削除に成功
   * @retval false 削除に失敗
   */
  // --------------------------------------------------------------------------
  virtual bool DeleteAllMessage();

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスにある送信データを全て削除します。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener );

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックス内にあるデータの削除に成功した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteAllMessageSuccess();

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックス内にあるデータの削除に失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteAllMessageFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスに送信データを書き込みます。
   *         書き込んだデータは自動的に相手に送信されます。
   * @param  pSendPacket 書き込む送信データです
   * @retval true 書き込みに成功
   * @retval false 書き込みに失敗
   */
  // --------------------------------------------------------------------------
  virtual bool WriteMessage( const CecSendPacket* pSendPacket );

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスに送信データを書き込みます。
   *         書き込んだデータは自動的に相手に送信されます。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pSendPacket 書き込む送信データです
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void WriteMessageAsync( const CecSendPacket* pSendPacket, ICecMessageBoxEventListener* pEventListener );

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
   * @brief  送信ボックスに設定する送信データを取得します。
   * @return 送信ボックスに設定する送信データ
   */
  // --------------------------------------------------------------------------
  virtual const CecSendPacket* GetCecSendPacket() const;

  // --------------------------------------------------------------------------
  /**
   * @brief  非同期で実行されたコマンドが終了した時にコールされるイベントハンドラです。
   * @param  eResultCode 実行結果です
   *         この実行結果を参照してアプリ側へ成功、失敗の通知を行います。
   */
  // --------------------------------------------------------------------------
  virtual void OnCommandResult( ResultCode eResultCode );

private:

  void copySendPacket( const CecSendPacket* pSendPacket );

  CTR_ICecControlObjectRegister*    m_pCecControlObjectRegister;
  ICecMessageBoxEventListener*      m_pCecMessageBoxEventListener;
  CecSendPacket                     m_CecSendPacket;
  CecSystemInitParam                m_InitParam;
  bool                              m_bDeleteAllMessageResult;
  bool                              m_bWriteMessageResult;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_CECSENDMESSAGEBOX_H_INCLUDE
#endif
