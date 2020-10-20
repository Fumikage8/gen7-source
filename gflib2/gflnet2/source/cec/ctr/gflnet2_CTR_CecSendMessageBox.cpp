// ============================================================================
/*
 * @file gflnet2_CTR_CecSendMessageBox.cpp
 * @brief すれ違い通信の送信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecSendMessageBox.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_ICecControlObjectRegister.h>
#include <gflnet2/include/cec/gflnet2_ICecMessageBoxEventListener.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecSendMessageBox::CTR_CecSendMessageBox( CTR_ICecControlObjectRegister* pCecControlObjectRegister ) :
  m_pCecControlObjectRegister( pCecControlObjectRegister ),
  m_pCecMessageBoxEventListener( NULL ),
  m_bDeleteAllMessageResult( false ),
  m_bWriteMessageResult( false )
{
}


CTR_CecSendMessageBox::~CTR_CecSendMessageBox()
{
  GFL_SAFE_DELETE_ARRAY( m_CecSendPacket.pInfoTextData );
  GFL_SAFE_DELETE_ARRAY( m_CecSendPacket.pIconData );
  GFL_SAFE_DELETE_ARRAY( m_CecSendPacket.pSendData );

}


// --------------------------------------------------------------------------
/**
  * @brief  初期化処理を行います。
  *         内部で持つ送信データのバッファの確保等を行っています。
  * @param  pHeap 内部のバッファを確保する為のヒープです。
  * @param  pInitParam 内部で持つバッファのサイズ等の情報です。
  */
// --------------------------------------------------------------------------
void CTR_CecSendMessageBox::Initialize( gfl2::heap::HeapBase* pHeap, const CecSystemInitParam* pInitParam )
{
  gfl2::std::MemCopy( pInitParam, &m_InitParam, sizeof( CecSystemInitParam ) );

  m_CecSendPacket.pSendData = GFL_NEW_ARRAY( pHeap ) u8[ pInitParam->sendDataSize ];
  m_CecSendPacket.sendSize = pInitParam->sendDataSize;

  m_CecSendPacket.pIconData = GFL_NEW_ARRAY( pHeap ) u8[ pInitParam->sendIconSize ];
  m_CecSendPacket.iconSize = pInitParam->sendIconSize;

  m_CecSendPacket.pInfoTextData = reinterpret_cast<wchar_t*>( GFL_NEW_ARRAY( pHeap ) u8[ pInitParam->sendInfoTextSize ] );
  m_CecSendPacket.infoTextSize = pInitParam->sendInfoTextSize;

}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックスにある送信データを全て削除します。
  * @retval true 削除に成功
  * @retval false 削除に失敗
  */
// --------------------------------------------------------------------------
bool CTR_CecSendMessageBox::DeleteAllMessage()
{
  SetCecCommandExecuteParameters( gflnet2::cec::DELETE_ALL_MESSAGE, gflnet2::cec::SEND_BOX );
  m_pCecControlObjectRegister->QuickExecute( this );
  return m_bDeleteAllMessageResult;
}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックスにある送信データを全て削除します。
  *         非同期で行われるので結果はイベントリスナーに通知されます。
  * @param  pEventListener 結果が通知されるイベントリスナーを指定します
  */
// --------------------------------------------------------------------------
void CTR_CecSendMessageBox::DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pCecMessageBoxEventListener = pEventListener;
  SetCecCommandExecuteParameters( gflnet2::cec::DELETE_ALL_MESSAGE, gflnet2::cec::SEND_BOX );
  m_pCecControlObjectRegister->RegisterControlObject( this );
}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックス内にあるデータの削除に成功した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecSendMessageBox::OnDeleteAllMessageSuccess()
{
  m_bDeleteAllMessageResult = true;
}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックス内にあるデータの削除に失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecSendMessageBox::OnDeleteAllMessageFailure()
{
  m_bDeleteAllMessageResult = false;
}


void CTR_CecSendMessageBox::copySendPacket( const CecSendPacket* pSendPacket )
{
  if( m_InitParam.sendDataSize < pSendPacket->sendSize )
  {
    GFL_ASSERT_MSG( NULL, "Error : send data size over." );
  }
  if( m_InitParam.sendIconSize < pSendPacket->iconSize )
  {
    GFL_ASSERT_MSG( NULL, "Error : send icon size over." );
  }
  if( m_InitParam.sendInfoTextSize < pSendPacket->infoTextSize )
  {
    GFL_ASSERT_MSG( NULL, "Error : send info text size over." );
  }

  gfl2::std::MemCopy( pSendPacket->pSendData, m_CecSendPacket.pSendData, pSendPacket->sendSize );
  gfl2::std::MemCopy( pSendPacket->pIconData, m_CecSendPacket.pIconData, pSendPacket->iconSize );
  gfl2::std::MemCopy( pSendPacket->pInfoTextData, m_CecSendPacket.pInfoTextData, pSendPacket->infoTextSize );

  m_CecSendPacket.sendSize = pSendPacket->sendSize;
  m_CecSendPacket.iconSize = pSendPacket->iconSize;
  m_CecSendPacket.infoTextSize = pSendPacket->infoTextSize;

}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックスに送信データを書き込みます。
  *         書き込んだデータは自動的に相手に送信されます。
  * @param  pSendPacket 書き込む送信データです
  * @retval true 書き込みに成功
  * @retval false 書き込みに失敗
  */
// --------------------------------------------------------------------------
bool CTR_CecSendMessageBox::WriteMessage( const CecSendPacket* pSendPacket )
{
  copySendPacket( pSendPacket );
  SetCecCommandExecuteParameters( gflnet2::cec::WRITE_MESSAGE, gflnet2::cec::SEND_BOX );
  m_pCecControlObjectRegister->QuickExecute( this );
  return m_bWriteMessageResult;
}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックスに送信データを書き込みます。
  *         書き込んだデータは自動的に相手に送信されます。
  *         非同期で行われるので結果はイベントリスナーに通知されます。
  * @param  pSendPacket 書き込む送信データです
  * @param  pEventListener 結果が通知されるイベントリスナーを指定します
  */
// --------------------------------------------------------------------------
void CTR_CecSendMessageBox::WriteMessageAsync( const CecSendPacket* pSendPacket, ICecMessageBoxEventListener* pEventListener )
{
  copySendPacket( pSendPacket );
  m_pCecMessageBoxEventListener = pEventListener;
  SetCecCommandExecuteParameters( gflnet2::cec::WRITE_MESSAGE, gflnet2::cec::SEND_BOX );
  m_pCecControlObjectRegister->RegisterControlObject( this );
}


// --------------------------------------------------------------------------
/**
  * @brief  送信データの書き込みに成功した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecSendMessageBox::OnWriteMessageSuccess()
{
  m_bWriteMessageResult = true;
}


// --------------------------------------------------------------------------
/**
  * @brief  送信データの書き込みに失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecSendMessageBox::OnWriteMessageFailure()
{
  m_bWriteMessageResult = false;
}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックスに設定する送信データを取得します。
  * @return 送信ボックスに設定する送信データ
  */
// --------------------------------------------------------------------------
const CecSendPacket* CTR_CecSendMessageBox::GetCecSendPacket() const
{
  return &m_CecSendPacket;
}


// --------------------------------------------------------------------------
/**
  * @brief  非同期で実行されたコマンドが終了した時にコールされるイベントハンドラです。
  * @param  eResultCode 実行結果です
  *         この実行結果を参照してアプリ側へ成功、失敗の通知を行います。
  */
// --------------------------------------------------------------------------
void CTR_CecSendMessageBox::OnCommandResult( ResultCode eResultCode )
{
  CommandID eCommandID = GetCommandID();
  switch( eCommandID )
  {
    case gflnet2::cec::DELETE_ALL_MESSAGE:
      if( eResultCode == gflnet2::cec::SUCCESS )
      {
        m_pCecMessageBoxEventListener->OnDeleteAllMessageSuccess( gflnet2::cec::SEND_BOX );
      }
      else
      {
        m_pCecMessageBoxEventListener->OnDeleteAllMessageFailure( gflnet2::cec::SEND_BOX );
      }
    break;

    case gflnet2::cec::WRITE_MESSAGE:
      if( eResultCode == gflnet2::cec::SUCCESS )
      {
        m_pCecMessageBoxEventListener->OnWriteMessageSuccess();
      }
      else
      {
        m_pCecMessageBoxEventListener->OnWriteMessageFailure();
      }
    break;
  }
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
