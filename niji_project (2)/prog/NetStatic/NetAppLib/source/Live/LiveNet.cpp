//==============================================================================
/**
 * @file   LiveNet.cpp
 * @date   2015/12/02 Wed. 18:18:00
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_DebugPrint.h>

#include  "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include  "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

#include  "NetStatic/NetAppLib/include/Live/LiveNet.h"
// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  State
  {
    STATE_Initial,
    STATE_Connecting,
    STATE_NetGameInitializing,
    STATE_Connected,
    STATE_Disconnecting,

    STATE_
  };




  inline  NetLib::P2P::P2pConnectionManager*  GetP2P(void)
  {
    return GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  }
}


namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LiveNet::LiveNet(gfl2::heap::HeapBase* pHeapBase)
  : m_pHeapBase(pHeapBase)
  , m_pNetGame(NULL)
  , m_pEventHandler(NULL)
  , m_State(STATE_Initial)
{
  for(u32 command = 0; command < COMMAND_MAX; ++command)
  {
    m_pPacketTransferrer[command]  = NULL;
  }

#define LIVENET_BUF_CLEAR(buf)  memset(&(buf), 0, sizeof(buf))
  LIVENET_BUF_CLEAR(mSendBuffer);
  LIVENET_BUF_CLEAR(mReceiveBuffer);
  LIVENET_BUF_CLEAR(m_BattleInfo_Mine);
  LIVENET_BUF_CLEAR(m_BattleInfo_Opponent);
#undef  LIVENET_BUF_CLEAR

}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LiveNet::~LiveNet()
{
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveNet::Initialize(void)
{
  GFL_ASSERT(m_State == STATE_Initial);

  NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveNet::Finalize(void)
{
  Disconnect();
  NetLib::P2P::P2pConnectionManager*  pP2P  = GetP2P();
  NetLib::NijiNetworkSystem::TerminateNetwork();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveNet::Update(void)
{
  NetLib::P2P::P2pConnectionManager*  pP2P  = GetP2P();


  switch(m_State)
  {
    case  STATE_Initial:
      break;

    case  STATE_Connecting:
      if(pP2P->IsConnectSuccess())
      {
        m_pNetGame->Initialize();
        m_State  = STATE_NetGameInitializing;
      }
      break;

    case  STATE_NetGameInitializing:
      if(m_pNetGame->IsInitialize())
      {
        m_State = STATE_Connected;
      }
      break;

    case  STATE_Connected:
      Recv();
      break;

    case  STATE_Disconnecting:
      if(pP2P->IsDisconnectSuccess())
      {
        pP2P->Terminate();
        m_State = STATE_Initial;
      }
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LiveNet::Connect(void)
{
  GFL_ASSERT(m_State == STATE_Initial);
  if(m_State != STATE_Initial)  return;

  NetLib::P2P::P2pConnectionManager*  pP2P  = GetP2P();

  pP2P->Initialize(NetLib::P2P::NIJI_P2P_LIVE_CUP);
#ifdef  GF_PLATFORM_CTR
  pP2P->ConnectStart(gflnet2::p2p::AUTO);
  m_pNetGame  = GFL_NEW(m_pHeapBase) gflnet2::p2p::NetGame();
#endif // GF_PLATFORM_CTR 

  m_State = STATE_Connecting;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  LiveNet::IsConnected(void)
{
  return (m_State == STATE_Connected);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LiveNet::Disconnect(void)
{
  if(m_State == STATE_Initial)        return;
  if(m_State == STATE_Disconnecting)  return;

  CloseCommandPort();
  if(m_pNetGame)
  {
    m_pNetGame->Finalize();
    GFL_SAFE_DELETE(m_pNetGame);
  }

  /*
    GFNMcat[5644]   「ライブ大会」相手と接続する直前にBキャンセルするとアサート
    p2p初期化中にDisconnectすると接続Timing処理中に切断Timing処理が走り出し、Assert
    Irは相手側に切断を送信する必要もないため、Timing処理をSkipする
  */
  GetP2P()->DisconnectStart(true);

  m_State = STATE_Disconnecting;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  LiveNet::IsDisconnected(void)
{
  return (m_State == STATE_Initial);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LiveNet::OpenCommandPort(const Command command)
{
  const bool  isOpened  = IsCommandPortOpened(command);

  GFL_ASSERT(!isOpened);
  if(isOpened)  return;

  m_pPacketTransferrer[command]  = GFL_NEW(m_pHeapBase)  PacketTransferrer(m_pHeapBase->GetLowerHandle(), this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LiveNet::CloseCommandPort(const Command command)
{
  if(command >= COMMAND_MAX)
  {
    /*  全てClose  */
    for(u32 commandID = 0; commandID < COMMAND_MAX; ++commandID)
    {
      CloseCommandPort(static_cast<Command>(commandID));
    }
    return;
  }

  const bool  isOpened  = IsCommandPortOpened(command);

//  GFL_ASSERT(isOpened);
  if(!isOpened)  return;

  GFL_SAFE_DELETE(m_pPacketTransferrer[command]);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  LiveNet::ExchangeRegulation(const Regulation* pRegulation)
{
  const Command   commandID     = COMMAND_Regulation;
  void*           pSendData     = &mSendBuffer.serializedRegulation;
  const u32       sendDataBytes = sizeof(mSendBuffer.serializedRegulation);
  
  
  memset(pSendData, 0, sendDataBytes);
#if PM_DEBUG
  /*  Regulation送信ツール用  */
  if(pRegulation)
  {
    pRegulation->Serialize(&mSendBuffer.serializedRegulation);
  }
#endif

  CloseCommandPort(commandID);
  OpenCommandPort(commandID);
  return Send(pSendData, sendDataBytes, commandID);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  LiveNet::ExchangeBattleInfo(void)
{
  const Command   command       = COMMAND_BattleInfo;
  void*           pSendData     = &mSendBuffer.battleInfo;
  const u32       sendDataBytes = sizeof(mSendBuffer.battleInfo);
  
  mSendBuffer.battleInfo  = m_BattleInfo_Mine;

  CloseCommandPort(command);
  OpenCommandPort(command);
  return Send(pSendData, sendDataBytes, command);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveNet::SetupMyBattleInfo(const Regulation& rRegulation, const pml::PokeParty& rPokeParty)
{
  rRegulation.Serialize(&m_BattleInfo_Mine.serializedRegulation.data);
  rPokeParty.SerializeCore(&m_BattleInfo_Mine.serializedPokeParty);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  LiveNet::Send(void* pData, const u32 dataSize, const Command command)
{
  GFL_ASSERT(IsCommandPortOpened(command));
  if(!IsCommandPortOpened(command))           return false;

  m_pPacketTransferrer[command]->SendDataCommand(pData, dataSize, command);
  return true;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveNet::Recv(void)
{
  for(u32 commandID = 0; commandID < COMMAND_MAX; ++commandID)
  {
    const Command command = static_cast<Command>(commandID);

    if(IsCommandPortOpened(command))
    {
      if(m_pPacketTransferrer[command]->IsDone())
      {
        CloseCommandPort(command);
      }
      else
      {
        m_pPacketTransferrer[command]->Update();
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LiveNet::OnPacketTransferSuccess(NetLib::P2P::P2pPacketTransfer* pP2PTrans)
{
  PacketTransferrer*  pPacketTransferrer  = NULL;
  u32                 command;

  for(command = 0; command < COMMAND_MAX; ++command)
  {
    if(m_pPacketTransferrer[command] == pP2PTrans)
    {
      pPacketTransferrer  = static_cast<PacketTransferrer*>(m_pPacketTransferrer[command]);
      break;
    }
  }

  if(!pPacketTransferrer)
  {
    /*  not found (想定外)  */
    GFL_ASSERT(false);
    return;
  }

  pPacketTransferrer->Done();
  {
    const u32 dataNum   = pP2PTrans->GetReceivedDataCount();
    int       recvNetID;

    switch(command)
    {
      case  COMMAND_Regulation:
      {
        /*  データバッファのクリア  */
        memset(&m_SerializedRegulation, 0, sizeof(m_SerializedRegulation));

        for(u32 index = 0; index < dataNum; ++index)
        {
          if(pPacketTransferrer->GetReceivedData(index, command, recvNetID, &(mReceiveBuffer.serializedRegulation), sizeof(mReceiveBuffer.serializedRegulation)))
          {
            /*  memcpy  */
            if(recvNetID != m_pNetGame->GetNetID()) 
            {
              m_SerializedRegulation  = mReceiveBuffer.serializedRegulation;
              break;
            }
          }
        }
        Event_ReceiveRegulation();
      }
        break;

      case  COMMAND_BattleInfo:
      {
        /*  データバッファのクリア  */
        memset(&m_BattleInfo_Opponent,  0, sizeof(m_BattleInfo_Opponent));

        for(u32 index = 0; index < dataNum; ++index)
        {
          if(pPacketTransferrer->GetReceivedData(index, command, recvNetID, &(mReceiveBuffer.battleInfo), sizeof(mReceiveBuffer.battleInfo)))
          {
            /*  memcpy  */
            if(recvNetID != m_pNetGame->GetNetID())
            {
              m_BattleInfo_Opponent = mReceiveBuffer.battleInfo;
              break;
            }
          }
        }
        Event_ReceiveBattleInfo();
      }
        break;
    }
  }
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
BTL_CLIENT_ID   LiveNet::GetClientID_Mine(void) const
{
  return (m_pNetGame->IsMaster() ? BTL_CLIENT_PLAYER : BTL_CLIENT_ENEMY1);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
BTL_CLIENT_ID   LiveNet::GetClientID_Opponent(void) const
{
  return (m_pNetGame->IsMaster() ? BTL_CLIENT_ENEMY1 : BTL_CLIENT_PLAYER);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
int LiveNet::GetNetID_Mine(void)  const
{
  return GetP2P()->GetNetID();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
int LiveNet::GetNetID_Opponent(void)  const
{
  return (!GetP2P()->GetNetID());
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
Savedata::MyStatus*   LiveNet::GetMyStatus_Mine(void)  const
{
  const int netID = GetNetID_Mine();
  
  GFL_ASSERT(0<=netID);   /*  offline時は-1が返る  */
  
  return  GetP2P()->GetMyStatus(netID);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
Savedata::MyStatus*   LiveNet::GetMyStatus_Opponent(void)  const
{
  const int netID = GetNetID_Opponent();

  GFL_ASSERT(0<=netID);   /*  offline時は-1が返る  */

  return  GetP2P()->GetMyStatus(netID);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
#if PM_DEBUG
  void  LiveNet::Debug_RegulationFakeDataReceive(const Regulation::PRESET  preset)
  {
    Regulation  testRegulation(m_pHeapBase);
    
    testRegulation.LoadData(preset);
    testRegulation.Serialize(m_SerializedRegulation.data);
    Event_ReceiveRegulation();
  }
#endif


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveNet::Event_ReceiveRegulation(void)
{
  if(!m_pEventHandler) return;
  
  m_pEventHandler->OnReceive_Regulation(this, m_SerializedRegulation);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LiveNet::Event_ReceiveBattleInfo(void)
{
  if(!m_pEventHandler) return;

  m_pEventHandler->OnReceive_LiveBattleInfo(this, m_BattleInfo_Mine, m_BattleInfo_Opponent);
}


} /*  namespace Live    */
} /*  namespace NetApp  */
