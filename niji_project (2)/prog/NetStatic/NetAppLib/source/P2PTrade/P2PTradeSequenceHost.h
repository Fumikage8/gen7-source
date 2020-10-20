// ============================================================================
/*
 * @file P2PTradeSequenceHost.h
 * @brief P2P交換アプリで使用する通信モジュールです。
 * @date 2015.09.28
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADESEQUENCEHOST_H_INCLUDE )
#define P2PTRADESEQUENCEHOST_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeSequenceBase.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


class P2PTradeSequenceHost : public NetAppLib::P2PTrade::P2PTradeSequenceBase
{
public:

  P2PTradeSequenceHost(
    gfl2::heap::HeapBase*                         pHeap,
    bool&                                         bOutCallZukanRegister,
    app::event::ZukanRegisterEventParam&          outZukanRegisterEventParam,
    bool&                                         bOutCallEvolution,
    NetEvent::Evolution::EvolutionEventParam&     outEvolutionEventParam );
  virtual~P2PTradeSequenceHost();

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
  
  VerifyResult        m_eRecvVerifyResult;
  bool                m_bRecvVerifyResultFlag;
  bool                m_bRecvFinalTradeFlag;
  bool                m_bExitRunning;
  bool                m_bSendFinalSyncCompleteFlag;
  bool                m_bRecvFinalSyncCommandFlag;
  bool                m_bSendFinalSyncCancelCompleteFlag;
  bool                m_bRecvFinalSyncCancelCommandFlag;
  bool                m_bCallFinalSyncCancelReceivedHandler;

};



GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

#endif // P2PTRADESEQUENCEHOST_H_INCLUDE
