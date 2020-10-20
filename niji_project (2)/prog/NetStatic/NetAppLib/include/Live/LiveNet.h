#if !defined(LIVENET_H_INCLUDED)
#define LIVENET_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveNet.h
 * @date   2015/12/02 Wed. 18:03:47
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <heap/include/gfl2_Heap.h>
//#include  <util/include/gfl2_List.h>
#include  <pml/include/pmlib.h>
#include  <gflnet2/include/p2p/gflnet2_NetGame.h>
#include  <gflnet2/include/p2p/gflnet2_P2pDefine.h>


#include  "NetStatic/NetLib/include/P2P/P2PPacketTransfer.h"
#include  "Battle/Regulation/include/Regulation.h"
#include  "Battle/include/battle_SetupParam.h"
#include  "Savedata/include/MyStatus.h"


// 前方宣言
namespace gflnet2{
  namespace p2p{
    class NetGame;
  }
}

// =============================================================================
/**/
//==============================================================================


namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================

struct SerializedRegulation
{
  u8  data[Regulation::REGULATION_MAX_DATA_SIZE_LIVE];    /**<  受信バッファ：※ライブ大会専用サイズ   */
};

struct LiveBattleInfo
{
  SerializedRegulation            serializedRegulation;
  pml::PokeParty::SERIALIZE_DATA  serializedPokeParty;
};


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/12/02 Wed. 18:04:43
  */
//==============================================================================
class LiveNet
  : protected NetLib::P2P::P2pPacketTransferListener
{
  GFL_FORBID_COPY_AND_ASSIGN(LiveNet);
public:

  class IEventHandler
  {
  public:
    virtual void  OnReceive_Regulation(     LiveNet* pSender, const SerializedRegulation& rSerializedRegulation ) = 0;
    virtual void  OnReceive_LiveBattleInfo( LiveNet* pSender, const LiveBattleInfo& rMyLiveBattleInfo, const LiveBattleInfo& rOpponentLiveBattleInfo) = 0;

    virtual ~IEventHandler(){}
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler  = pHandler;}



  LiveNet(gfl2::heap::HeapBase* pHeapBase);
  virtual ~LiveNet();

  void  Initialize(void);
  void  Finalize(void);
  void  Update(void);


  /*
    connection
  */
  void  Connect(void);
  bool  IsConnected(void);

  void  Disconnect(void);
  bool  IsDisconnected(void);


  enum  Command
  {
    COMMAND_Regulation,
    COMMAND_BattleInfo,
    
    COMMAND_MAX
  };

  void  OpenCommandPort(const Command command);
  void  CloseCommandPort(const Command command = COMMAND_MAX);
  void  CloseAllCommandPort(void)                         {CloseCommandPort();}
  bool  IsCommandPortOpened(const Command command) const  {return (m_pPacketTransferrer[command]!=NULL);}



  /*
    utils
  */
  bool            ExchangeRegulation(const Regulation* pRegulation = NULL); /**<  regulationの交換（regulation受信用）  */
  bool            ExchangeBattleInfo(void);                                 /**<  battleInfoの交換（対戦用）            */

  void            SetupMyBattleInfo(const Regulation& rRegulation, const pml::PokeParty& rPokeParty);

  BTL_CLIENT_ID         GetClientID_Mine(void)        const;
  BTL_CLIENT_ID         GetClientID_Opponent(void)    const;
  int                   GetNetID_Mine(void)           const;
  int                   GetNetID_Opponent(void)       const;
  const LiveBattleInfo& GetBattleInfo_Mine(void)      const {return m_BattleInfo_Mine;}
  const LiveBattleInfo& GetBattleInfo_Opponent(void)  const {return m_BattleInfo_Opponent;}
  Savedata::MyStatus*   GetMyStatus_Mine(void)        const;
  Savedata::MyStatus*   GetMyStatus_Opponent(void)    const;

  static const BTL_CLIENT_ID  ClientID_1of2 = BTL_CLIENT_PLAYER;
  static const BTL_CLIENT_ID  ClientID_2of2 = BTL_CLIENT_ENEMY1;


#if PM_DEBUG
  void  Debug_RegulationFakeDataReceive(const Regulation::PRESET  preset = Regulation::PRESET_SINGLE_UNLIMITED);
#endif


private:
  class PacketTransferrer
    : public  NetLib::P2P::P2pPacketTransfer
  {
    GFL_FORBID_COPY_AND_ASSIGN(PacketTransferrer);
  public:
    PacketTransferrer(gfl2::heap::HeapBase* pLowerHeap, NetLib::P2P::P2pPacketTransferListener* pP2pPacketTransferListener)
      : NetLib::P2P::P2pPacketTransfer(pLowerHeap, pP2pPacketTransferListener)
      , m_bDone(false)
    {}
    virtual ~PacketTransferrer()  {}
    
    bool  IsDone(void) const  {return m_bDone;}
    void  Done(void)          {m_bDone  = true;}
  protected:
    bool  m_bDone;
  };


  bool  Send(void* pData, const u32 dataSize, const Command command);
  void  Recv(void);

  /*  implement NetLib::P2P::P2pPacketTransferListener  */
  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer );

  void  Event_ReceiveRegulation(void);
  void  Event_ReceiveBattleInfo(void);

  gfl2::heap::HeapBase*   m_pHeapBase;
  gflnet2::p2p::NetGame*  m_pNetGame;           /**<  送受信管理  */
  PacketTransferrer*      m_pPacketTransferrer[COMMAND_MAX];
  IEventHandler*          m_pEventHandler;
  u32                     m_State;

  union DataBuffer
  {
    SerializedRegulation  serializedRegulation;
    LiveBattleInfo        battleInfo;
  }mSendBuffer, mReceiveBuffer;
  static const u32    ReceiveBufferBytes = sizeof(DataBuffer);

  SerializedRegulation    m_SerializedRegulation;
  LiveBattleInfo          m_BattleInfo_Mine;
  LiveBattleInfo          m_BattleInfo_Opponent;
};



} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVENET_H_INCLUDED)  */
