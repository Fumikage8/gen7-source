// ============================================================================
/*
 * @file gflnet2_CTR_CecMessageBoxAccessor.cpp
 * @brief メッセージボックスにアクセスする為のクラスです。
 *        nn::cec::MessageBoxのラッパークラス的なクラスとなっています。
 *        実際に必要な機能だけ実装しています。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <debug/include/gfl2_DebugPrint.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecMessageBoxAccessor.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecAllocator.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecControlObject.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecMessageBoxAccessor::CTR_CecMessageBoxAccessor( CTR_CecAllocator* pCecAllocator ) :
  m_pCecAllocator( pCecAllocator ),
  m_pMessageBox( NULL ),
  m_pCecControlObject( NULL ),
  m_cecTitleID( 0 ),
  m_privateID( 0 )
{
}


CTR_CecMessageBoxAccessor::~CTR_CecMessageBoxAccessor()
{
  GFL_SAFE_DELETE( m_pMessageBox );
}


// --------------------------------------------------------------------------
/**
  * @brief  初期化処理を行います。
  * @param  pHeap メッセージボックスを作成する為に必要なヒープです。
  * @param  pInitParam メッセージボックスにアクセスする為に必要なパラメータです。
  */
// --------------------------------------------------------------------------
void CTR_CecMessageBoxAccessor::Initialize( gfl2::heap::HeapBase* pHeap, const gflnet2::cec::CecSystemInitParam* pInitParam )
{
  m_pMessageBox = GFL_NEW( pHeap ) nn::cec::MessageBox();

  m_cecTitleID = nn::cec::MakeCecTitleId( pInitParam->titleID );
  m_privateID = pInitParam->titleID;
  
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージボックスへアクセスします。
  * @param  pCecControlObject 実行オブジェクトです。
  * @return 実行結果を返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::Execute( CTR_CecControlObject* pCecControlObject )
{
  m_pCecControlObject = pCecControlObject;

  CommandID eCommandID = m_pCecControlObject->GetCommandID();

  ResultCode eResultCode = gflnet2::cec::FAILURE;

  switch( eCommandID )
  {
    case gflnet2::cec::IS_CREATE_MESSAGE_BOX:
      eResultCode = isCreateMessageBox();
    break;

    case gflnet2::cec::CREATE_MESSAGE_BOX:
      eResultCode = createMessageBox();
    break;

    case gflnet2::cec::DELETE_MESSAGE_BOX:
      eResultCode = deleteMessageBox();
    break;

    case gflnet2::cec::DELETE_ALL_MESSAGE:
      eResultCode = deleteAllMessage();
    break;

    case gflnet2::cec::WRITE_MESSAGE:
      eResultCode = writeMessage();
    break;

    case gflnet2::cec::READ_MESSAGE:
      eResultCode = readMessage();
    break;

    case gflnet2::cec::GET_MESSAGE_COUNT:
      eResultCode = getMessageCount();
    break;
  }

  return eResultCode;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージボックスが作成されているか調べます。
  * @return 実行結果を返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::isCreateMessageBox()
{
  nn::Result result = m_pMessageBox->OpenMessageBox( m_cecTitleID, m_privateID );

  if( result.IsSuccess() )
  {
    m_pCecControlObject->OnIsCreateMessageBoxSuccess();
  }
  else
  {
    m_pCecControlObject->OnIsCreateMessageBoxFailure();
  }

  return gflnet2::cec::SUCCESS;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージボックスの作成を行います。
  * @return 実行結果を返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::createMessageBox()
{
  const CecMessageBoxCreateCoreParam* pCreateCoreParam = m_pCecControlObject->GetCecMessageBoxCreateCoreParam();

  nn::Result result = m_pMessageBox->CreateMessageBox(
    m_cecTitleID, m_privateID, pCreateCoreParam->hmackey,
    pCreateCoreParam->icon, pCreateCoreParam->iconSize, pCreateCoreParam->name, pCreateCoreParam->nameSize,
    pCreateCoreParam->recvBoxSize, pCreateCoreParam->sendBoxSize,
    pCreateCoreParam->recvMessageCount, pCreateCoreParam->sendMessageCount,
    nn::cec::CEC_MESSSIZEMAX_DEFAULT );

  ResultCode eResultCode = convertToResultCode( result );

  if( eResultCode == gflnet2::cec::SUCCESS )
  {
    m_pCecControlObject->OnCreateMessageBoxSuccess();
  }
  else
  {
    GFL_PRINT( "Error : create message box failed[0x%x].\n", result.GetPrintableBits() );
    m_pCecControlObject->OnCreateMessageBoxFailure();
  }

  return eResultCode;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージボックスの削除を行います。
  * @return 実行結果を返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::deleteMessageBox()
{
  nn::Result result = m_pMessageBox->DeleteMessageBox( m_cecTitleID );

  ResultCode eResultCode = convertToResultCode( result );

  if( eResultCode == gflnet2::cec::SUCCESS )
  {
    m_pCecControlObject->OnDeleteMessageBoxSuccess();
  }
  else
  {
    GFL_PRINT( "Error : delete message box failed[0x%x].\n", result.GetPrintableBits() );
    m_pCecControlObject->OnDeleteMessageBoxFailure();
  }

  return eResultCode;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージの全削除を行います。
  * @return 実行結果を返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::deleteAllMessage()
{
  nn::Result result = m_pMessageBox->OpenMessageBox( m_cecTitleID, m_privateID );

  ResultCode eResultCode = convertToResultCode( result );

  if( eResultCode == gflnet2::cec::SUCCESS )
  {
    m_pMessageBox->DeleteAllMessages( convertToCecBoxType( m_pCecControlObject->GetMessageBoxType() ) );
    m_pMessageBox->CloseMessageBox();

    m_pCecControlObject->OnDeleteAllMessageSuccess();
  }
  else
  {
    GFL_PRINT( "Error : open message box failed[0x%x].\n", result.GetPrintableBits() );
    m_pCecControlObject->OnDeleteAllMessageFailure();
  }

  return eResultCode;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージの書き込みを行います。
  * @return 実行結果を返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::writeMessage()
{
  nn::cec::Message message;
  nn::cec::MessageId messageID;
  bool bResult = setupSendMessage( message );
  if( !bResult )
  {
    m_pCecControlObject->OnWriteMessageFailure();
    return gflnet2::cec::FAILURE;
  }

  nn::Result result = m_pMessageBox->OpenMessageBox( m_cecTitleID, m_privateID );
  if( result.IsFailure() )
  {
    GFL_PRINT( "Error : open message box failed[0x%x].\n", result.GetPrintableBits() );
    m_pCecControlObject->OnWriteMessageFailure();
    return gflnet2::cec::FAILURE;
  }

  result = m_pMessageBox->WriteMessage( message, convertToCecBoxType( m_pCecControlObject->GetMessageBoxType() ), messageID );

  m_pMessageBox->CloseMessageBox();

  ResultCode eResultCode = convertToResultCode( result );

  if( eResultCode == gflnet2::cec::SUCCESS )
  {
    m_pCecControlObject->OnWriteMessageSuccess();
  }
  else
  {
    GFL_PRINT( "Error : write message failed[0x%x].\n", result.GetPrintableBits() );
    m_pCecControlObject->OnWriteMessageFailure();
  }

  return eResultCode;

}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージの読み込みを行います。
  * @return 実行結果を返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::readMessage()
{
  nn::Result result = m_pMessageBox->OpenMessageBox( m_cecTitleID, m_privateID );
  if( result.IsFailure() )
  {
    GFL_PRINT( "Error : open message box failed[0x%x].\n", result.GetPrintableBits() );
    m_pCecControlObject->OnReadMessageFailure();
    return gflnet2::cec::FAILURE;
  }

  // 読み込み処理
  {
    nn::cec::CTR::CecBoxType cecBoxType = convertToCecBoxType( m_pCecControlObject->GetMessageBoxType() );

    u32 messageCount = m_pMessageBox->GetBoxMessageNum( cecBoxType );

    for( u32 i = 0; i < messageCount; ++i )
    {
      nn::cec::MessageId  cecMessageID;
      nn::cec::Message    cecMessage;
      size_t              cecMessageSize;
      u8*                 pBuffer;

      m_pMessageBox->GetMessageId( &cecMessageID, cecBoxType, i );
      
      cecMessageSize = m_pMessageBox->GetMessageSize( cecBoxType, i );
      
      pBuffer = reinterpret_cast<u8*>( m_pCecAllocator->Allocate( cecMessageSize ) );
      if( pBuffer == NULL )
      {
        GFL_PRINT( "Error : memory allocation failed.\n" );
        continue;
      }

      result = m_pMessageBox->ReadMessage( cecMessage, pBuffer, cecMessageSize, cecBoxType, cecMessageID );
      if( result.IsFailure() )
      {
        GFL_PRINT( "Error : read message failed[0x%x].\n", result.GetPrintableBits() );
        m_pCecAllocator->Free( pBuffer );
        continue;
      }

      m_pCecControlObject->OnReadMessageSuccess( cecMessage );

      m_pCecAllocator->Free( pBuffer );
    }
  }

  m_pMessageBox->CloseMessageBox();

  return gflnet2::cec::SUCCESS;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージ数の取得を行います。
  * @return 実行結果を返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::getMessageCount()
{
  nn::Result result = m_pMessageBox->OpenMessageBox( m_cecTitleID, m_privateID );

  ResultCode eResultCode = convertToResultCode( result );

  if( eResultCode == gflnet2::cec::SUCCESS )
  {
    s32 messageCount = m_pMessageBox->GetBoxMessageNum( convertToCecBoxType( m_pCecControlObject->GetMessageBoxType() ) );

    m_pMessageBox->CloseMessageBox();

    m_pCecControlObject->OnGetMessageCountSuccess( messageCount );
  }
  else
  {
    GFL_PRINT( "Error : open message box failed[0x%x].\n", result.GetPrintableBits() );
    m_pCecControlObject->OnGetMessageCountFailure();
  }

  return eResultCode;
}


// --------------------------------------------------------------------------
/**
  * @brief  リザルトコードの変換を行います。
  * @param  result nnのリザルトコード
  * @return ライブラリで定義してあるリザルトコードを返します。
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessor::convertToResultCode( nn::Result result )
{
  ResultCode eResultCode;

  if( result.IsSuccess()
   || result == nn::cec::ResultBoxAlreadyExists() )  // メッセージボックスが既に作成されている場合も成功と見なします
  {
    eResultCode = gflnet2::cec::SUCCESS;
  }
  else
  {
    eResultCode = gflnet2::cec::FAILURE;
  }

  return eResultCode;
}


// --------------------------------------------------------------------------
/**
  * @brief  ライブラリ内で使用しているボックスタイプをnnのボックスタイプに変換します。
  * @param  eMessageBoxType ライブラリ内で使用しているボックスタイプ
  * @return nnのボックスタイプ
  */
// --------------------------------------------------------------------------
nn::cec::CTR::CecBoxType CTR_CecMessageBoxAccessor::convertToCecBoxType( const MessageBoxType eMessageBoxType ) const
{
  nn::cec::CTR::CecBoxType eCecBoxType = nn::cec::CEC_BOXTYPE_INBOX;
  if( eMessageBoxType == gflnet2::cec::SEND_BOX )
  {
    eCecBoxType = nn::cec::CEC_BOXTYPE_OUTBOX;
  }
  return eCecBoxType;
}


// --------------------------------------------------------------------------
/**
  * @brief  送信メッセージのセットアップを行います。
  *         送信するデータ、送信するアイコン、送信する情報テキストを設定します。
  * @param  message セットアップを行うメッセージ
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_CecMessageBoxAccessor::setupSendMessage( nn::cec::Message& message ) const
{
  const CecSendPacket* pCecSendPacket = m_pCecControlObject->GetCecSendPacket();

  nn::Result result;

  result = message.NewMessage( m_cecTitleID, 0, nn::cec::MESSAGE_TYPEFLAG_ANYONE, nn::cec::CEC_SENDMODE_SENDRECV, nn::cec::MESSAGE_SENDCOUNT_UNLIMITED, nn::cec::MESSAGE_PROPAGATIONCOUNT_ONCE );
  if( result.IsFailure() )
  {
    GFL_PRINT( "Error : new message failed[0x%x].\n", result.GetPrintableBits() );
    return false;
  }

  result = message.SetMessageBody( pCecSendPacket->pSendData, pCecSendPacket->sendSize );
  if( result.IsFailure() )
  {
    GFL_PRINT( "Error : set send data failed[0x%x].\n", result.GetPrintableBits() );
    return false;
  }

  result = message.SetIcon( pCecSendPacket->pIconData, pCecSendPacket->iconSize );
  if( result.IsFailure() )
  {
    GFL_PRINT( "Error : set icon data failed[0x%x].\n", result.GetPrintableBits() );
    return false;
  }

  if( pCecSendPacket->pInfoTextData != NULL && pCecSendPacket->infoTextSize > 0 )
  {
    result = message.SetInfoText( pCecSendPacket->pInfoTextData, pCecSendPacket->infoTextSize );
    if( result.IsFailure() )
    {
      GFL_PRINT( "Error : set info text data failed[0x%x].\n", result.GetPrintableBits() );
      return false;
    }
  }

  return true;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
