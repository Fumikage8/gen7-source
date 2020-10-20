//=============================================================================
/**
 * @file   gflnet2_SessionPacketManager.cpp
 * @brief  セッション内パケット管理クラス
 * @author shin kosaka
 * @date   2014.9.29
 */
//=============================================================================
#include <gflnet2/include/Session/gflnet2_SessionPacketManager.h>


#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/Session/CTR/gflnet2_CTR_TCPPacketIO.h>
#include <gflnet2/source/Session/CTR/gflnet2_CTR_UDPPacketIO.h>
#endif

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <thread/include/gfl2_ThreadStatic.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)

//------------------------------------------------------------------
/**
* @brief   コンストラクタ
*/
//------------------------------------------------------------------
SessionPacketManager::SessionPacketManager()
: m_pHeap(NULL)
, m_messageManagerArray()
, m_pUDPPacketIO(NULL)
, m_pTCPPacketIO(NULL)
, m_UDPDatagram()
, m_TCPDataSizeArray()
, m_pRecvPacketBuffer(NULL)
{
}

//------------------------------------------------------------------
/**
* @brief   デストラクタ
*/
//------------------------------------------------------------------
SessionPacketManager::~SessionPacketManager()
{
  Finalize();
}

//------------------------------------------------------------------
/**
* @brief   初期化処理
* @param[in] pHeap ヒープ
* @param[in] pInitParam 初期化パラメータ
*/
//------------------------------------------------------------------
void SessionPacketManager::Initialize( gfl2::heap::HeapBase* pHeap , SessionPacketInitParam* pInitParam )
{
  if( m_pHeap )
  {//Initializeが2回呼ばれている
    GFL_ASSERT_MSG( 0 ,"aleady initialized" );
    return;
  }

  m_pHeap = pHeap;

  if( pInitParam )
  {
    for( u32 index = 0 ; index < pInitParam->UDPMessageSizeArray.size() ; ++index )
    {
      m_UDPDatagram.AddDataSize( pInitParam->UDPMessageSizeArray[index] );
    }
    m_UDPDatagram.CreateBuffer( m_pHeap );
    m_pRecvPacketBuffer = GflHeapAllocMemoryAlign( m_pHeap, m_UDPDatagram.GetTotalDataSize() , SessionPacketDatagram::DATAGRAM_ALIGN );

    for( u32 index = 0 ; index < pInitParam->TCPMessageSizeArray.size() ; ++index )
    {
      m_TCPDataSizeArray.push_back( pInitParam->UDPMessageSizeArray[index] );
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"pInitParam is NULL");
  }

#ifdef  GF_PLATFORM_CTR
  m_pUDPPacketIO = GFL_NEW( m_pHeap ) CTR_UDPPacketIO( m_pHeap );
  m_pTCPPacketIO = GFL_NEW( m_pHeap ) CTR_TCPPacketIO( m_pHeap );
#endif
}

//------------------------------------------------------------------
/**
* @brief   終了処理
*/
//------------------------------------------------------------------
void SessionPacketManager::Finalize( void )
{
  if( m_pHeap )
  {
    GFL_SAFE_DELETE( m_pUDPPacketIO );
    GFL_SAFE_DELETE( m_pTCPPacketIO );
    GflHeapFreeMemory( m_pRecvPacketBuffer );

    m_messageManagerArray.clear();
    m_TCPDataSizeArray.clear();
    m_UDPDatagram.DeleteBuffer();

    m_pHeap = NULL;
  }
}

//------------------------------------------------------------------
/**
* @brief  更新処理。
*
* @note   	メインループ毎にポーリングすること
*			      リスナーに通知されるイベントはすべてこの関数内で実行される
*/
//------------------------------------------------------------------
void SessionPacketManager::Update( void )
{
  if( m_pTCPPacketIO )
  {//TCP受信
    m_pTCPPacketIO->Update();
    for ever
    {
      u32 messageID = m_pTCPPacketIO->OutputPacket( static_cast<u8*>( m_pRecvPacketBuffer ) , m_UDPDatagram.GetTotalDataSize() );
      if( messageID == INVALID_MESSAGE_ID )
      {
        break;
      }

      for( u32 index = 0 ; index < m_messageManagerArray.size() ; ++index )
      {
        if( m_messageManagerArray[index] == NULL )
        {
          continue;
        }

        if( m_messageManagerArray[index]->SetRecvTCPMessage( messageID , static_cast<u8*>( m_pRecvPacketBuffer ) , m_TCPDataSizeArray[messageID] ) )//メッセージIDを全てモジュール内で管理できるようにする
        {//見つかったので終わり（メッセージIDは重複して管理されない）
          break;
        }
      }

    }
  }

  if( m_pUDPPacketIO )
  {//UDP受信
    m_pUDPPacketIO->InputPacket( SESSION_UDP_MESSAGE_ID , m_UDPDatagram.GetDataAddr() , m_UDPDatagram.GetTotalDataSize() );
    m_pUDPPacketIO->Update();
    m_pUDPPacketIO->OutputPacket( static_cast<u8*>( m_pRecvPacketBuffer ) , m_UDPDatagram.GetTotalDataSize() );

    u32 dataSizeNum = m_UDPDatagram.GetDataSizeNum();
    for( u32 dataIndex = 0 ; dataIndex < dataSizeNum ; ++dataIndex )
    {
      u8* pRecvData = m_UDPDatagram.GetRecvDataAddr( static_cast<u8*>( m_pRecvPacketBuffer ) , dataIndex );

      for( u32 index = 0 ; index < m_messageManagerArray.size() ; ++index )
      {
        if( m_messageManagerArray[index] == NULL )
        {
          continue;
        }

        if( m_messageManagerArray[index]->SetRecvUDPMessage( ConvertUDPDataIndexToMessageID( dataIndex ) , pRecvData , m_UDPDatagram.GetDataSize(dataIndex) ) )
        {//見つかったので終わり（メッセージIDは重複して管理されない）
          break;
        }
      }
    }
  }

#if GFL_DEBUG
#endif
}

//------------------------------------------------------------------
/**
* @brief  メッセージ管理クラスを登録
*
* @retval true： 登録成功
* @retval false: 登録失敗
*/
//------------------------------------------------------------------
bool SessionPacketManager::AddMessageManager( SessionMessageManagerBase* pManager )
{
  if( pManager == NULL )
  {GFL_ASSERT_MSG(0,"pManager is NULL");
    return false;
  }

  if( m_messageManagerArray.free_size() <= 0 )
  {//一杯
    GFL_ASSERT_MSG(0,"size over");
    return false;
  }

  for( u32 index = 0 ; index < m_messageManagerArray.size() ; ++index )
  {//すでに登録済み
    if( m_messageManagerArray[index] == pManager )
    {
      GFL_ASSERT_MSG(0,"aleady add pManager");
      return false;
    }
  }

  m_messageManagerArray.push_back( pManager );

  return true;
}


//------------------------------------------------------------------
/**
* @brief  メッセージ管理クラスを削除
*/
//------------------------------------------------------------------
void SessionPacketManager::RemoveMessageManager( SessionMessageManagerBase* pManager )
{
  for( u32 index = 0 ; index < m_messageManagerArray.size() ; ++index )
  {
    if( m_messageManagerArray[index] == pManager )
    {
      m_messageManagerArray.remove( m_messageManagerArray[index] );
      break;
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  TCPメッセージ送信
* @param[in] messageID メッセージID
* @param[in] pMessage  メッセージ本体
* @param[in] messageSize メッセージサイズ
*/
//------------------------------------------------------------------
bool SessionPacketManager::SendTCPMessage( u32 messageID , const u8* pMessage , u16 messageSize )
{
  if( m_pTCPPacketIO )
  {
    return m_pTCPPacketIO->InputPacket( messageID , pMessage , messageSize );
  }

  return false;
}

//------------------------------------------------------------------
/**
* @brief  UDPメッセージ送信
* @param[in] messageID メッセージID
* @param[in] pMessage  メッセージ本体
* @param[in] messageSize メッセージサイズ
*/
//------------------------------------------------------------------
bool SessionPacketManager::SendUDPMessage( u32 messageID , const u8* pMessage , u16 messageSize )
{
  if( m_pUDPPacketIO )
  {
    return m_UDPDatagram.SetData( messageID , pMessage , messageSize );
  }

  return false;
}

//------------------------------------------------------------------
/**
* @brief  UDPデータ番号からメッセージIDに変換
* @param[in] dataIndex UDPデータIndex
* @retval messageID メッセージID
*/
//------------------------------------------------------------------
u16 SessionPacketManager::ConvertUDPDataIndexToMessageID( u16 dataIndex )
{
  return SESSION_UDP_INNER_MESSAGE_ID_START + dataIndex;
}

GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)

