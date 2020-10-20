// ============================================================================
/*
 * @file CecTestMessageBoxEventListener.cpp
 * @brief すれ違い通信のテスト用イベントリスナーです。
 *        すれ違い通信の状態を更新しています。
 * @date 2015.03.03
 */
// ============================================================================
#if PM_DEBUG

#include "Test/CecTest/include/CecTestMessageBoxEventListener.h"
#include "Test/CecTest/include/CecTestStateAccessor.h"
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(CecTest)


CecTestMessageBoxEventListener::CecTestMessageBoxEventListener( CecTestStateAccessor* pStateAccessor ) :
  m_pStateAccessor( pStateAccessor )
{
}


CecTestMessageBoxEventListener::~CecTestMessageBoxEventListener()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスが作成されているか調べ終わった後にコールされるイベントハンドラです。
  * @param  bIsCreate [ true : 作成されている, false : 作成されていない ]
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnIsCreateMessageBox( const bool bIsCreate )
{
  if( bIsCreate )
  {
    m_pStateAccessor->SetState( Test::CecTest::CecTestStateDeleteAllSendMessage );
  }
  else
  {
    m_pStateAccessor->SetState( Test::CecTest::CecTestStateCreateMessageBox );
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスの作成に成功した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnCreateMessageBoxSuccess()
{
  m_pStateAccessor->SetState( Test::CecTest::CecTestStateDeleteAllSendMessage );
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスの作成に失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnCreateMessageBoxFailure()
{
  m_pStateAccessor->SetState( Test::CecTest::CecTestStateFailure );
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスの削除に成功した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnDeleteMessageBoxSuccess()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスの削除に失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnDeleteMessageBoxFailure()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  送受信ボックス内にあるデータの削除に成功した時にコールされるイベントハンドラです。
  * @param  eMessageBoxType メッセージボックスタイプです
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnDeleteAllMessageSuccess( const gflnet2::cec::MessageBoxType eMessageBoxType )
{
  if( eMessageBoxType == gflnet2::cec::RECV_BOX )
  {
    m_pStateAccessor->SetState( Test::CecTest::CecTestStateIdle );
  }
  else
  {
    m_pStateAccessor->SetState( Test::CecTest::CecTestStateWriteMessage );
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  送受信ボックス内にあるデータの削除に失敗した時にコールされるイベントハンドラです。
  * @param  eMessageBoxType メッセージボックスタイプです
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnDeleteAllMessageFailure( const gflnet2::cec::MessageBoxType eMessageBoxType )
{
  if( eMessageBoxType == gflnet2::cec::RECV_BOX )
  {
    // @todo : 将来的にはエラーメッセージを表示したりする
    //         アプリを終了させるかは要相談

    // サンプルではとりあえず何もしない
  }
  else
  {
    m_pStateAccessor->SetState( Test::CecTest::CecTestStateFailure );
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  送信データの書き込みに成功した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnWriteMessageSuccess()
{
  m_pStateAccessor->SetState( Test::CecTest::CecTestStateIdle );
}


// --------------------------------------------------------------------------
/**
  * @brief  送信データの書き込みに失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnWriteMessageFailure()
{
  // @todo : 将来的にはエラーメッセージを表示したりする
  //         アプリを終了させるかは要相談

  // サンプルなので終了させます 
  m_pStateAccessor->SetState( Test::CecTest::CecTestStateFailure );
}


// --------------------------------------------------------------------------
/**
  * @brief  受信データの読み込みに成功した時にコールされるイベントハンドラです。
  * @param  pCecRecvPackets 読み込んだ受信データ
  * @param  recvPacketCount 読み込んだ受信データの数
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnReadMessageSuccess( const gflnet2::cec::CecRecvPacket* pCecRecvPackets, const u32 recvPacketCount )
{
  m_pStateAccessor->SetState( Test::CecTest::CecTestStateIdle );

  GFL_PRINT( "recvPacketCount = %d\n", recvPacketCount );

  for( u32 i = 0; i < recvPacketCount; ++i )
  {
    GFL_PRINT( "recvSize = %d\n", pCecRecvPackets[i].recvSize );
    GFL_PRINT( "iconSize = %d\n", pCecRecvPackets[i].iconSize );
    GFL_PRINT( "infoTextSize = %d\n", pCecRecvPackets[i].infoTextSize );
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  受信データの読み込みに失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnReadMessageFailure()
{
  // @todo : 将来的にはエラーメッセージを表示したりする
  //         アプリを終了させるかは要相談

  // サンプルではとりあえず何もしない
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にある受信データ数の取得に成功した時にコールされるイベントハンドラです。
  * @param  messageCount 受信ボックス内にある受信データ数
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnGetMessageCountSuccess( const s32 messageCount )
{
  m_pStateAccessor->SetState( Test::CecTest::CecTestStateIdle );
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にある受信データ数の取得に失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CecTestMessageBoxEventListener::OnGetMessageCountFailure()
{
  // @todo : 将来的にはエラーメッセージを表示したりする
  //         アプリを終了させるかは要相談

  // サンプルではとりあえず何もしない
}


GFL_NAMESPACE_END(CecTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
