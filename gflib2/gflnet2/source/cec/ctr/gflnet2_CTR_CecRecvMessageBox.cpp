// ============================================================================
/*
 * @file gflnet2_CTR_CecRecvMessageBox.cpp
 * @brief すれ違い通信の受信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecRecvMessageBox.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_ICecControlObjectRegister.h>
#include <gflnet2/include/cec/gflnet2_ICecMessageBoxEventListener.h>
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecRecvMessageBox::CTR_CecRecvMessageBox( CTR_ICecControlObjectRegister* pCecControlObjectRegister ) :
  m_pCecControlObjectRegister( pCecControlObjectRegister ),
  m_pCecMessageBoxEventListener( NULL ),
  m_pRecvPackets( NULL ),
  m_recvPacketCount( 0 ),
  m_messageCount( 0 ),
  m_bDeleteAllMessageResult( false )
{
}


CTR_CecRecvMessageBox::~CTR_CecRecvMessageBox()
{
  for( u32 i = 0; i < m_InitParam.recvPacketLength; ++i )
  {
    GFL_SAFE_DELETE_ARRAY( m_pRecvPackets[i].pRecvData );
    GFL_SAFE_DELETE_ARRAY( m_pRecvPackets[i].pIconData );
    GFL_SAFE_DELETE_ARRAY( m_pRecvPackets[i].pInfoTextData );
  }

  GFL_SAFE_DELETE_ARRAY( m_pRecvPackets );
}


// --------------------------------------------------------------------------
/**
  * @brief  初期化処理を行います。
  *         内部で持つ受信データのバッファの確保等を行っています。
  * @param  pHeap 内部のバッファを確保する為のヒープです。
  * @param  pInitParam 内部で持つバッファのサイズ等の情報です。
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::Initialize( gfl2::heap::HeapBase* pHeap, const CecSystemInitParam* pInitParam )
{
  gfl2::std::MemCopy( pInitParam, &m_InitParam, sizeof( CecSystemInitParam ) );

  m_pRecvPackets = GFL_NEW_ARRAY( pHeap ) gflnet2::cec::CecRecvPacket[ m_InitParam.recvPacketLength ];

  for( u32 i = 0; i < m_InitParam.recvPacketLength; ++i )
  {

    m_pRecvPackets[i].pRecvData      = GFL_NEW_ARRAY( pHeap ) u8[ m_InitParam.recvDataSize ];
    m_pRecvPackets[i].recvSize       = 0;
    m_pRecvPackets[i].pIconData      = GFL_NEW_ARRAY( pHeap ) u8[ m_InitParam.recvIconSize ];
    m_pRecvPackets[i].iconSize       = 0;
    m_pRecvPackets[i].pInfoTextData  = reinterpret_cast<wchar_t*>( GFL_NEW_ARRAY( pHeap ) u8[ m_InitParam.recvInfoTextSize ] );
    m_pRecvPackets[i].infoTextSize   = 0;

    m_pRecvPackets[i].createDateTime.year         = 0;
    m_pRecvPackets[i].createDateTime.month        = 0;
    m_pRecvPackets[i].createDateTime.day          = 0;
    m_pRecvPackets[i].createDateTime.week         = gflnet2::cec::WEEK_MAX;
    m_pRecvPackets[i].createDateTime.hour         = 0;
    m_pRecvPackets[i].createDateTime.minute       = 0;
    m_pRecvPackets[i].createDateTime.second       = 0;
    m_pRecvPackets[i].createDateTime.milliSecond  = 0;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にある受信データ数を取得します。
  * @return 受信ボックス内にある受信データ数
  */
// --------------------------------------------------------------------------
s32 CTR_CecRecvMessageBox::GetMessageCount()
{
  m_messageCount = 0;
  SetCecCommandExecuteParameters( gflnet2::cec::GET_MESSAGE_COUNT, gflnet2::cec::RECV_BOX );
  m_pCecControlObjectRegister->QuickExecute( this );
  return m_messageCount;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にある受信データ数を取得します。
  *         非同期で行われるので結果はイベントリスナーに通知されます。
  * @param  pEventListener 結果が通知されるイベントリスナーを指定します
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::GetMessageCountAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_messageCount = 0;
  m_pCecMessageBoxEventListener = pEventListener;
  SetCecCommandExecuteParameters( gflnet2::cec::GET_MESSAGE_COUNT, gflnet2::cec::RECV_BOX );
  m_pCecControlObjectRegister->RegisterControlObject( this );
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にある受信データ数の取得に成功した時にコールされるイベントハンドラです。
  * @param  messageCount 受信ボックス内にある受信データ数
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::OnGetMessageCountSuccess( const s32 messageCount )
{
  m_messageCount = messageCount;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にある受信データ数の取得に失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::OnGetMessageCountFailure()
{
  m_messageCount = 0;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にある受信データを読み込みます。
  * @param  recvPacketCount 読み込んだ受信データの数
  * @return 読み込んだ受信データ
  */
// --------------------------------------------------------------------------
const gflnet2::cec::CecRecvPacket* CTR_CecRecvMessageBox::ReadMessage( u32& recvPacketCount )
{
  m_recvPacketCount = 0;

  SetCecCommandExecuteParameters( gflnet2::cec::READ_MESSAGE, gflnet2::cec::RECV_BOX );
  m_pCecControlObjectRegister->QuickExecute( this );

  recvPacketCount = m_recvPacketCount;
  return m_pRecvPackets;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にある受信データを読み込みます。
  *         非同期で行われるので結果はイベントリスナーに通知されます。
  * @param  pEventListener 結果が通知されるイベントリスナーを指定します
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::ReadMessageAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_recvPacketCount = 0;

  m_pCecMessageBoxEventListener = pEventListener;
  SetCecCommandExecuteParameters( gflnet2::cec::READ_MESSAGE, gflnet2::cec::RECV_BOX );
  m_pCecControlObjectRegister->RegisterControlObject( this );
}


// --------------------------------------------------------------------------
/**
  * @brief  受信データの読み込みに成功した時にコールされるイベントハンドラです。
  * @param  cecMessage 読み込んだ受信データ
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::OnReadMessageSuccess( nn::cec::Message& cecMessage )
{
  if( m_recvPacketCount >= m_InitParam.recvPacketLength )
  {
    GFL_PRINT( "Error : recv packet buffer over. m_recvPacketCount = %d, m_InitParam.recvPacketLength = %d\n", m_recvPacketCount, m_InitParam.recvPacketLength );
    return;
  }

  CecRecvPacket* pRecvPacket = &m_pRecvPackets[ m_recvPacketCount ];

  nn::Result result;

  {
    void*   pIcon;
    size_t  iconSize;

    result = cecMessage.GetIcon( &pIcon, &iconSize );
    if( result.IsFailure() )
    {
      GFL_PRINT( "Error : cec recv message get icon failed[0x%x].\n", result.GetPrintableBits() );
    }

    if( result.IsSuccess() )
    {
      if( iconSize <= m_InitParam.recvIconSize )
      {
        gfl2::std::MemCopy( pIcon, pRecvPacket->pIconData, iconSize );
        pRecvPacket->iconSize = iconSize;
      }
      else
      {
        GFL_PRINT( "Error : recv icon size over. size = %d\n", iconSize );
      }
    }
  }

  {
    const wchar_t*  pInfoText;
    size_t          infoTextSize;

    result = cecMessage.GetInfoText( &pInfoText, &infoTextSize );
    if( result.IsFailure() )
    {
      GFL_PRINT( "Error : cec recv message get info text failed[0x%x].\n", result.GetPrintableBits() );
    }

    if( result.IsSuccess() )
    {
      if( infoTextSize <= m_InitParam.recvInfoTextSize )
      {
        gfl2::std::MemCopy( pInfoText, pRecvPacket->pInfoTextData, infoTextSize );
        pRecvPacket->infoTextSize = infoTextSize;
      }
      else
      {
        GFL_PRINT( "Error : recv info text size over. size = %d\n", infoTextSize );
      }
    }
  }

  {
    void*   pBody;
    size_t  bodySize;

    cecMessage.GetMessageBodyPointer( &pBody, &bodySize );
    if( bodySize > 0 )
    {
      if( bodySize <= m_InitParam.recvDataSize )
      {
        gfl2::std::MemCopy( pBody, pRecvPacket->pRecvData, bodySize );
        pRecvPacket->recvSize = bodySize;
      }
      else
      {
        GFL_PRINT( "Error : recv data size over. size = %d\n", bodySize );
      }
    }
  }

  nn::fnd::DateTimeParameters createDateTime = cecMessage.GetCreateDate();
  pRecvPacket->createDateTime.year        = createDateTime.year;
  pRecvPacket->createDateTime.month       = createDateTime.month;
  pRecvPacket->createDateTime.day         = createDateTime.day;
  pRecvPacket->createDateTime.week        = convertToWeek( createDateTime.week );
  pRecvPacket->createDateTime.hour        = createDateTime.hour;
  pRecvPacket->createDateTime.minute      = createDateTime.minute;
  pRecvPacket->createDateTime.second      = createDateTime.second;
  pRecvPacket->createDateTime.milliSecond = createDateTime.milliSecond;

  m_recvPacketCount++;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信データの読み込みに失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::OnReadMessageFailure()
{
}


gflnet2::cec::Week CTR_CecRecvMessageBox::convertToWeek( nn::fnd::Week nnWeek )
{
  gflnet2::cec::Week week = gflnet2::cec::WEEK_MAX;

  switch( nnWeek )
  {
    case nn::fnd::WEEK_SUNDAY:    week = gflnet2::cec::WEEK_SUNDAY;    break;
    case nn::fnd::WEEK_MONDAY:    week = gflnet2::cec::WEEK_MONDAY;    break;
    case nn::fnd::WEEK_TUESDAY:   week = gflnet2::cec::WEEK_TUESDAY;   break;
    case nn::fnd::WEEK_WEDNESDAY: week = gflnet2::cec::WEEK_WEDNESDAY; break;
    case nn::fnd::WEEK_THURSDAY:  week = gflnet2::cec::WEEK_THURSDAY;  break;
    case nn::fnd::WEEK_FRIDAY:    week = gflnet2::cec::WEEK_FRIDAY;    break;
    case nn::fnd::WEEK_SATURDAY:  week = gflnet2::cec::WEEK_SATURDAY;  break;
  }

  return week;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックスにある受信データを全て削除します。
  * @retval true 削除に成功
  * @retval false 削除に失敗
  */
// --------------------------------------------------------------------------
bool CTR_CecRecvMessageBox::DeleteAllMessage()
{
  SetCecCommandExecuteParameters( gflnet2::cec::DELETE_ALL_MESSAGE, gflnet2::cec::RECV_BOX );
  m_pCecControlObjectRegister->QuickExecute( this );
  return m_bDeleteAllMessageResult;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックスにある受信データを全て削除します。
  *         非同期で行われるので結果はイベントリスナーに通知されます。
  * @param  pEventListener 結果が通知されるイベントリスナーを指定します
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pCecMessageBoxEventListener = pEventListener;
  SetCecCommandExecuteParameters( gflnet2::cec::DELETE_ALL_MESSAGE, gflnet2::cec::RECV_BOX );
  m_pCecControlObjectRegister->RegisterControlObject( this );
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にあるデータの削除に成功した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::OnDeleteAllMessageSuccess()
{
  m_bDeleteAllMessageResult = true;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックス内にあるデータの削除に失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::OnDeleteAllMessageFailure()
{
  m_bDeleteAllMessageResult = false;
}


// --------------------------------------------------------------------------
/**
  * @brief  非同期で実行されたコマンドが終了した時にコールされるイベントハンドラです。
  * @param  eResultCode 実行結果です
  *         この実行結果を参照してアプリ側へ成功、失敗の通知を行います。
  */
// --------------------------------------------------------------------------
void CTR_CecRecvMessageBox::OnCommandResult( ResultCode eResultCode )
{
  CommandID eCommandID = GetCommandID();
  switch( eCommandID )
  {
  case gflnet2::cec::GET_MESSAGE_COUNT:
      if( eResultCode == gflnet2::cec::SUCCESS )
      {
        m_pCecMessageBoxEventListener->OnGetMessageCountSuccess( m_messageCount );
      }
      else
      {
        m_pCecMessageBoxEventListener->OnGetMessageCountFailure();
      }
    break;

  case gflnet2::cec::READ_MESSAGE:
      if( eResultCode == gflnet2::cec::SUCCESS )
      {
        m_pCecMessageBoxEventListener->OnReadMessageSuccess( m_pRecvPackets, m_recvPacketCount );
      }
      else
      {
        m_pCecMessageBoxEventListener->OnReadMessageFailure();
      }
    break;

    case gflnet2::cec::DELETE_ALL_MESSAGE:
      if( eResultCode == gflnet2::cec::SUCCESS )
      {
        m_pCecMessageBoxEventListener->OnDeleteAllMessageSuccess( gflnet2::cec::RECV_BOX );
      }
      else
      {
        m_pCecMessageBoxEventListener->OnDeleteAllMessageFailure( gflnet2::cec::RECV_BOX );
      }
    break;
  }
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
