// ============================================================================
/*
 * @file CecTestStateAccessor.h
 * @brief すれ違い通信の状態の設定と取得をするクラスです。
 * @date 2015.03.03
 */
// ============================================================================
#if PM_DEBUG

#if !defined( CECTESTSTATEACCESSOR_H_INCLUDED )
#define CECTESTSTATEACCESSOR_H_INCLUDED
#pragma once


#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(CecTest)

enum CecTestState
{
  CecTestStateNone,
  CecTestStateCreateMessageBoxCheck,      // メッセージボックスが作成されているか調べる
  CecTestStateCreateMessageBoxCheckWait,  // メッセージボックスが作成されているか調べるのを待つ
  CecTestStateCreateMessageBox,           // メッセージボックスの作成をする
  CecTestStateCreateMessageBoxWait,       // メッセージボックスが作成されるのを待つ
  CecTestStateDeleteAllSendMessage,       // 送信ボックス内のデータを削除する
  CecTestStateDeleteAllSendMessageWait,   // 送信ボックス内のデータが削除されるのを待つ
  CecTestStateWriteMessage,               // 送信ボックス内に送信データを書き込む
  CecTestStateWriteMessageWait,           // 送信ボックス内に送信データが書き込まれるのを待つ

  CecTestStateIdle,                       // すれ違い通信のセットアップが完了され何もしていない状態

  CecTestStateGetMessageCountWait,        // 受信ボックス内にあるデータ数の取得待ち
  CecTestStateReadMessageWait,            // 受信ボックス内にあるデータの読み込み待ち
  CecTestStateDeleteAllRecvMessageWait,   // 受信ボックス内のデータが削除されるのを待つ

  CecTestStateFailure
};

class CecTestStateAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( CecTestStateAccessor );

public:
  CecTestStateAccessor();
  virtual~CecTestStateAccessor();

  CecTestState GetState() const;
  void SetState( CecTestState eState );

private:
  CecTestState  m_eState;

};


GFL_NAMESPACE_END(CecTest)
GFL_NAMESPACE_END(Test)

#endif // CECTESTSTATEACCESSOR_H_INCLUDED

#endif // PM_DEBUG
