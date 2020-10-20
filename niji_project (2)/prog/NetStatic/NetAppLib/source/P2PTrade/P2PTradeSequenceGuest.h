// ============================================================================
/*
 * @file P2PTradeSequenceGuest.h
 * @brief P2P交換アプリで使用する通信モジュールです。
 * @date 2015.09.28
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADESEQUENCEGUEST_H_INCLUDE )
#define P2PTRADESEQUENCEGUEST_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeSequenceBase.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


class P2PTradeSequenceGuest : public NetAppLib::P2PTrade::P2PTradeSequenceBase
{
public:

  P2PTradeSequenceGuest(
    gfl2::heap::HeapBase*                         pHeap,
    bool&                                         bOutCallZukanRegister,
    app::event::ZukanRegisterEventParam&          outZukanRegisterEventParam,
    bool&                                         bOutCallEvolution,
    NetEvent::Evolution::EvolutionEventParam&     outEvolutionEventParam );
  virtual~P2PTradeSequenceGuest();

  virtual void Update( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener );
  virtual void OnRecvDataCommand( void* pRecvData, u8 commandID );
  virtual void AddFirstTradeRequest( const pml::pokepara::PokemonParam& pokemonParam, u8 tray, u8 pos );
  virtual void CancelFirstTradeRequest();
  virtual void AddFinalTradeRequest();
  virtual void CancelFinalTradeRequest();
  virtual void AddSendTradeCancelRequest();
  virtual void AddTradeExitRequest();
  virtual void AddFinalSyncRequest();
  virtual void AddFinalSyncCancelRequest();

protected:

  virtual void resetFlag();

  virtual void tradeCancel( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener );
  virtual void tradeExit( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener );
  virtual void finalSync( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener );

public:

  virtual void DebugPrint();

private:

  bool                m_bRecvVerifyStartFlag;
  bool                m_bRecvFirstTradeResultFlag;
  bool                m_bSendCancelFirstTradeFlag;
  bool                m_bRecvFinalTradeResultFlag;
  bool                m_bSendCancelFinalTradeFlag;
  bool                m_bSendFinalSyncCommandFlag;
  bool                m_bRecvFinalSyncCompleteCommandFlag;
  bool                m_bSendFinalSyncCancelCommandFlag;
  bool                m_bRecvFinalSyncCancelCompleteCommandFlag;

};



GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

#endif // P2PTRADESEQUENCEGUEST_H_INCLUDE
