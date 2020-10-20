#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryIRSystem.cpp
 * @brief  赤外線配信システムクラス
 * @author Shin Kosaka
 * @date   2012.8.21
 */
//=============================================================================

#include <NetStatic/NetLib/source/Delivery/IR/DeliveryIRSystem.h>
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

#include "gflnet2/include/beacon/gflnet2_BeaconManager.h"

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorListener.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkError.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)


//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
DeliveryIRSystem::DeliveryIRSystem( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam , gflnet2::InitParameter* pIrInitParam , gflnet2::InitParameterFixed* pIrInitParamFixed )
: DeliverySystem( pHeap , pInitParam )
, m_pIrInitParam( pIrInitParam )
, m_pIrInitParamFixed( pIrInitParamFixed )
, m_pNetGame( NULL )
, m_pSendData(NULL)
, m_dataSize(0)
, m_deliveryID(0)
, m_step(IR_STATE_START)
, m_sendedSize(0)
, m_completeIndex(0)
, m_peerType(0)
, m_pP2pConnectionManager(GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager))
{
  for( u32 index = 0 ; index < MAX_RECV_INDEX ; ++index )
  {
    m_recvDataBuffer[index] = GflHeapAllocMemoryAlign4096( pHeap , DELIVERY_DATA_SIZE );
    memset( m_recvDataBuffer[index] , 0 , DELIVERY_DATA_SIZE );
  }

  memset( m_recvDataSize , 0 , sizeof( m_recvDataSize ) );
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
DeliveryIRSystem::~DeliveryIRSystem()
{
   Finalize();
}

//------------------------------------------------------------------
/**
 * @brief  DeliveryIRSystemの初期化
 * @result 	実行結果
 */
//------------------------------------------------------------------
bool DeliveryIRSystem::Initialize( const int threadPriority , const u16 deliveryID , const void* pSendData , const size_t dataSize )
{
  KOUSAKA_PRINT("DeliveryIRSystem::Initialize[%d][0x%x]\n",dataSize,pSendData);

  if( m_pP2pConnectionManager == NULL )
  {//P2P通信システムが無い
    GFL_ASSERT(0);//@fix
    return false;
  }

  if( !m_isInit )
  {//初期化
    m_pSendData = pSendData;
    m_dataSize = dataSize;
    m_deliveryID = deliveryID;


    m_pP2pConnectionManager->Initialize( static_cast<NetLib::P2P::ENUM_NIJI_P2P_ID>(m_pIrInitParam->GameID) );
    m_pP2pConnectionManager->SetThreadPriority( threadPriority );
    bool ret = m_pP2pConnectionManager->ConnectStart( gflnet2::p2p::AUTO );
    GFL_ASSERT( ret );//@fix
  }

  DeliverySystem::InitializeBase();

  return true;
}

//------------------------------------------------------------------
/**
 * @brief  DeliveryIRSystemの終了処理
 * @result 実行結果
 */
//------------------------------------------------------------------
void DeliveryIRSystem::Finalize( void )
{
  KOUSAKA_PRINT("DeliveryIRSystem::Finalize\n");

  //if( m_isInit )
  {
    if( m_pNetGame )
    {
      m_pNetGame->Finalize();
    }

    if( m_pP2pConnectionManager )
    {
      m_pP2pConnectionManager->Terminate();
    }
  }

  for( u32 index = 0 ; index < MAX_RECV_INDEX ; ++index )
  {
    if( m_recvDataBuffer[index] )
    {
      GflHeapFreeMemory(m_recvDataBuffer[index]);
      m_recvDataBuffer[index]=NULL;
    }
  }

  DeliverySystem::FinalizeBase();
}

//------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------
void DeliveryIRSystem::Update( DeliveryRecvListener* pRecvListener )
{
  if( m_pSendData && m_dataSize > 0 )
  {//送信モード
    UpdateSendInternal( pRecvListener );
  }
  else
  {//受信モード
    UpdateRecvInternal( pRecvListener );
  }
}

//------------------------------------------------------------------
/**
 * @brief  終了準備
 */
//------------------------------------------------------------------
void DeliveryIRSystem::PrepareFinalize( void )
{
  if( m_step < IR_STATE_END_START )
  {//終了シーケンスに入っていないなら終了シーケンスへ
    m_step = IR_STATE_END_START;
  }
}

//------------------------------------------------------------------
/**
 * @brief  停止確認
 */
//------------------------------------------------------------------
bool DeliveryIRSystem::IsCompretePrepareFinalize( void )
{
  return m_step == IR_STATE_SHUTDOWN;
}

//static u32 s_count = 0;

//------------------------------------------------------------------
/**
 * @brief   送信更新処理
 */
//------------------------------------------------------------------
void DeliveryIRSystem::UpdateSendInternal( DeliveryRecvListener* pRecvListener )
{
  switch( m_step )
  {
  case IR_STATE_START:
    if( m_pP2pConnectionManager->IsConnectSuccess() )
    {
      KOUSAKA_PRINT("DeliveryIRSystem::m_pAutoConnect->isConnect\n ");
      m_step = IR_STATE_CONNECT;
      m_pNetGame = GFL_NEW(m_pHeap) gflnet2::p2p::NetGame();
    }
    else if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) )
    {
      if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        if( pRecvListener )
        {//赤外線モジュールが壊れているとこのタイミングでエラーになる
          GFL_PRINT("IR fatal error\n");
          pRecvListener->OnNetworkError( nn::ResultSuccess() , 0 );
          PrepareFinalize();
        }
      }
    }
  break;
  case IR_STATE_CONNECT:
    {
      KOUSAKA_PRINT("DeliveryIRSystem::m_pAutoConnect->Initialize\n ");
      m_pNetGame->Initialize();
      m_step = IR_STATE_CONNECT_WAIT;

      //s_count = 0;
    }
  break;
  case IR_STATE_CONNECT_WAIT:
    if( m_pNetGame->IsInitialize() )
    {
      KOUSAKA_PRINT("DeliveryIRSystem::m_pNetGame->IsInitialize() == true\n ");
      m_step = IR_STATE_CONNECTION_CHECK;
    }
  break;
  case IR_STATE_CONNECTION_CHECK:
    {
      u32 sendData = PEER_TYPE_SEND;
      if( m_pNetGame->SendData( (char*)&sendData , sizeof(sendData) ) )
      {
        KOUSAKA_PRINT("DeliveryIRSystem::peer type send complete\n");
        m_step = IR_STATE_CONNECTION_CHECK_WAIT;
      }
    }
  break;
  case IR_STATE_CONNECTION_CHECK_WAIT:
    {
      m_peerType = 0;
      u16 recvSize = 0;
      if( m_pNetGame->RecvData( 1 - m_pNetGame->GetNetID() , (char*)&m_peerType , sizeof(m_peerType) , &recvSize ) )
      {
        KOUSAKA_PRINT("DeliveryIRSystem::recv peer type[%d]\n",m_peerType);

        m_pNetGame->TimingStart(CHECK_TIMING_NO);
        m_step = IR_STATE_CONNECTION_CHECK_TIMING;
      }
    }
  break;
  case IR_STATE_CONNECTION_CHECK_TIMING:
    {
      if( m_pNetGame->IsTimingEnd(CHECK_TIMING_NO) )
      {
        if( m_peerType == PEER_TYPE_RECV )
        {//続行
          m_step = IR_STATE_SEND_HEADER;
        }
        else
        {//送信と送信の接続なので終わる
          //pRecvListener->OnNetworkError( nn::Result() , 0 );
          m_step = IR_STATE_END_START;
        }
      }
    }
  break;
  case IR_STATE_SEND_HEADER:
    {
      IR_DATA_HEADER header;
      header.crc16 = 0;
      header.dataType = m_deliveryID;
      header.dataBodySize = m_dataSize;

      char* pSendPoint = (char*)(&header);
      u16 sendSize = sizeof(header);
      if( m_pNetGame->SendData( pSendPoint , sendSize ) )
      {
        KOUSAKA_PRINT("DeliveryIRSystem::send header complete[%d]\n",sendSize);
        m_step = IR_STATE_SEND_BODY;
      }
    }
  break;
  case IR_STATE_SEND_BODY:
    {
      char* pSendPoint = (char*)(const_cast<void*>(m_pSendData));
      pSendPoint += m_sendedSize;
      u16 sendSize = m_dataSize - m_sendedSize > MAX_SEND_SIZE ? MAX_SEND_SIZE : m_dataSize - m_sendedSize;
      if( m_pNetGame->SendData( pSendPoint , sendSize ) )
      {
        m_sendedSize += sendSize;
        KOUSAKA_PRINT("DeliveryIRSystem::send[%d][%d]\n",m_sendedSize,m_dataSize);
      }

      if( m_sendedSize == m_dataSize )
      {
        KOUSAKA_PRINT("DeliveryIRSystem::send complete[%d][%d]\n",m_sendedSize,m_dataSize);
        m_pNetGame->TimingStart(END_TIMING_NO);
        m_step = IR_STATE_TIMING_WAIT;
      }
    }
  break;
  case IR_STATE_TIMING_WAIT:
    {
      if( m_pNetGame->IsTimingEnd(END_TIMING_NO) )
      {
        m_step = IR_STATE_END_START;
      }
    }
  break;
  case IR_STATE_END_START:
    {
      KOUSAKA_PRINT("DeliveryIRSystem::m_pAutoConnect->End\n ");
      if( m_pNetGame )
      {
        m_pNetGame->Finalize();
      }
      GFL_SAFE_DELETE( m_pNetGame );
      m_pP2pConnectionManager->DisconnectStart();

      m_step = IR_STATE_CONNECT_END;
    }
  break;
  case IR_STATE_CONNECT_END:
    if( m_pP2pConnectionManager->IsDisconnectSuccess() )
    {
      m_pP2pConnectionManager->Terminate();
      m_step = IR_STATE_SHUTDOWN;
    }
  break;
  case IR_STATE_SHUTDOWN:
    {
    }
  break;
  }
}

//------------------------------------------------------------------
/**
 * @brief   受信更新処理
 */
//------------------------------------------------------------------
void DeliveryIRSystem::UpdateRecvInternal( DeliveryRecvListener* pRecvListener )
{
  switch( m_step )
  {
  case IR_STATE_START:
    if( m_pP2pConnectionManager->IsConnectSuccess() )
    {
      KOUSAKA_PRINT("DeliveryIRSystem::m_pAutoConnect->isConnect\n ");
      m_step = IR_STATE_CONNECT;
      m_pNetGame = GFL_NEW(m_pHeap) gflnet2::p2p::NetGame();
    }
    else if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) )
    {
      if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        if( pRecvListener )
        {//赤外線モジュールが壊れているとこのタイミングでエラーになる
          GFL_PRINT("IR fatal error\n");
          pRecvListener->OnNetworkError( nn::ResultSuccess() , 0 );
          PrepareFinalize();
        }
      }
    }
  break;
  case IR_STATE_CONNECT:
    {
      KOUSAKA_PRINT("DeliveryIRSystem::m_pAutoConnect->Initialize\n ");
      m_pNetGame->Initialize();
      m_step = IR_STATE_CONNECT_WAIT;
    }
  break;
  case IR_STATE_CONNECT_WAIT:
    if( m_pNetGame->IsInitialize() )
    {
      KOUSAKA_PRINT("DeliveryIRSystem::m_pNetGame->IsInitialize() == true\n ");
      m_step = IR_STATE_CONNECTION_CHECK;
    }
  break;
  case IR_STATE_CONNECTION_CHECK:
    {
      u32 sendData = PEER_TYPE_RECV;
      if( m_pNetGame->SendData( (char*)&sendData , sizeof(sendData) ) )
      {
        KOUSAKA_PRINT("DeliveryIRSystem::peer type send complete\n");
        m_step = IR_STATE_CONNECTION_CHECK_WAIT;
      }
    }
  break;
  case IR_STATE_CONNECTION_CHECK_WAIT:
    {
      m_peerType = 0;
      u16 recvSize = 0;
      if( m_pNetGame->RecvData( 1 - m_pNetGame->GetNetID() , (char*)&m_peerType , sizeof(m_peerType) , &recvSize ) )
      {
        KOUSAKA_PRINT("DeliveryIRSystem::recv peer type[%d]\n",m_peerType);

        m_pNetGame->TimingStart(CHECK_TIMING_NO);
        m_step = IR_STATE_CONNECTION_CHECK_TIMING;
      }
    }
  break;
  case IR_STATE_CONNECTION_CHECK_TIMING:
    {
      if( m_pNetGame->IsTimingEnd(CHECK_TIMING_NO) )
      {
        if( m_peerType == PEER_TYPE_SEND )
        {//続行
          m_step = IR_STATE_RECV;
        }
        else
        {//受信と受信の接続なので終わる
          //pRecvListener->OnNetworkError( nn::Result() , 0 );
          m_step = IR_STATE_END_START;
        }
      }
    }
  break;
  case IR_STATE_RECV:
    {
      for( int i = 0 ; i < MAX_RECV_INDEX ; i++ )
      {
        u32 recvEnableSize = MAX_SEND_SIZE < DELIVERY_DATA_SIZE - m_recvDataSize[i] ? MAX_SEND_SIZE : DELIVERY_DATA_SIZE - m_recvDataSize[i];
        u16 recvSize = 0;
        if( recvEnableSize > 0 )
        {
          char* pRecvPoint = (char*)m_recvDataBuffer[i];
          pRecvPoint += m_recvDataSize[i];

          if( m_pNetGame->RecvData( i , pRecvPoint , recvEnableSize , &recvSize ) )
          {
            m_recvDataSize[i] += recvSize;
            KOUSAKA_PRINT("DeliveryIRSystem::recv[%d][%d][%d]\n",recvSize,m_recvDataSize[i],i);

            if( m_recvDataSize[i] >= sizeof(IR_DATA_HEADER) )
            {//ヘッダ分は受信した
              //受信完了しているかどうかチェック
              if( IsCompleteRecvDataInternal( i , pRecvListener ) )
              {//受信完了している
                //crcチェック
                m_completeIndex = i;
                m_step = IR_STATE_TIMING_WAIT;
                m_pNetGame->TimingStart(END_TIMING_NO);
                break;
              }
            }
          }
        }
      }
    }
  break;
  case IR_STATE_TIMING_WAIT:
    {
      if( m_pNetGame->IsTimingEnd(END_TIMING_NO) )
      {
        IR_DATA_HEADER* pHeader = reinterpret_cast<IR_DATA_HEADER*>( m_recvDataBuffer[m_completeIndex] );
        if( pHeader->dataBodySize >= sizeof(IR_DATA_HEADER) )
        {
          if( pRecvListener )
          {
            pRecvListener->OnUpdateProgress( 100 );//100%到達を通知
            pRecvListener->OnRecvComplete( &pHeader[1] , pHeader->dataBodySize );
          }
        }
        else
        {//エラー
          KOUSAKA_PRINT( "DeliveryIRSystem::pHeader->dataBodySize < sizeof(IR_DATA_HEADER) [%u]<[%u]\n" , pHeader->dataBodySize , sizeof(IR_DATA_HEADER) );
          if( pRecvListener )
          {
            pRecvListener->OnNetworkError( nn::Result() , 0 );
          }
        }

        m_step = IR_STATE_END_START;
      }
    }
  break;
  case IR_STATE_END_START:
    {
      KOUSAKA_PRINT("DeliveryIRSystem::m_pAutoConnect->End\n ");
      if( m_pNetGame )
      {
        m_pNetGame->Finalize();
      }
      GFL_SAFE_DELETE( m_pNetGame );
      m_pP2pConnectionManager->DisconnectStart();
      m_step = IR_STATE_CONNECT_END;
    }
  break;
  case IR_STATE_CONNECT_END:
    if( m_pP2pConnectionManager->IsDisconnectSuccess() )
    {

      m_pP2pConnectionManager->Terminate();
      m_step = IR_STATE_SHUTDOWN;
    }
  break;
  case IR_STATE_SHUTDOWN:
    {
    }
  break;
  }
}


//------------------------------------------------------------------
/**
 * @brief   受信完了しているかどうかチェック
 */
//------------------------------------------------------------------
bool DeliveryIRSystem::IsCompleteRecvDataInternal( u32 index , DeliveryRecvListener* pRecvListener )
{
  IR_DATA_HEADER* pHeader = reinterpret_cast<IR_DATA_HEADER*>( m_recvDataBuffer[index] );

  if( pRecvListener )
  {//受信完了％通知
    u32 progress = static_cast<u32>( std::floor( ( ( ( m_recvDataSize[index] - sizeof(IR_DATA_HEADER) ) * 1.0f ) / ( pHeader->dataBodySize * 1.0f ) ) * 100.0f ) );

    GFL_PRINT("IsCompleteRecvDataInternal : IR progress[%d]\n",progress);

    pRecvListener->OnUpdateProgress( progress );
  }
 
  //データサイズ分受信した
  if( m_recvDataSize[index] - sizeof(IR_DATA_HEADER) >= pHeader->dataBodySize )//ヘッダ分引く
  {
    KOUSAKA_PRINT( "DeliveryIRSystem::Recv Complete[%u][%u]\n" , m_recvDataSize[index] - sizeof(IR_DATA_HEADER) , pHeader->dataBodySize );

    return true;
  }

  return false;
}

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)
#endif
