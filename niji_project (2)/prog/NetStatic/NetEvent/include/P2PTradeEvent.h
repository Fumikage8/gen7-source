// ============================================================================
/*
 * @file P2PTradeEvent.h
 * @brief P2P交換アプリのイベント
 * @date 2015.07.24
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADEEVENT_H_INCLUDE )
#define P2PTRADEEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"
#include "App/AppEvent/include/Box/BoxEventListener.h"
#include "NetApp/P2PTrade/include/P2PTradeAppParam.h"
#include "App/AppDemo/include/AppDemoProcParam.h"
#include "NetStatic/NetEvent/include/TradeDemoEvent.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
class P2PTradeProc;
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(P2PTrade)


class P2PTradeEvent :
  public NetEvent::NetEventBase,
  public App::Event::BoxEventListener

{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager );

  P2PTradeEvent( gfl2::heap::HeapBase* pHeap );
  virtual~P2PTradeEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  virtual void OnTermination( u8 outJumpMode, u8 outTray, u8 outPos );

private:

  void bootBoxEvent( GameSys::GameManager* pGameManager );
  void bootP2PTradeProc( u32 bootMode, NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient );
  void bootTradeDemoEvent( GameSys::GameManager* pGameManager );

private:

  enum Sequence
  {
    SEQUENCE_INITIALIZE,
    SEQUENCE_BOX,
    SEQUENCE_P2PTRADE_WARNING,
    SEQUENCE_TRADE_DEMO,
    SEQUENCE_FINISH
  };

  NetApp::P2PTrade::P2PTradeProc*               m_pP2PTradeProc;
  NetAppLib::P2PTrade::P2PTradeRequestClient*   m_pP2PTradeRequestClient;
  pml::pokepara::PokemonParam*                  m_pDemoSendPokemon;
  pml::pokepara::PokemonParam*                  m_pDemoRecvPokemon;
  u8                                            m_OutJumpMode;
  u8                                            m_OutTray;
  u8                                            m_OutPos;

  App::AppDemo::AppDemoProcParam_MiracleTrade   m_DemoParam;

  NetEvent::TradeDemo::TradeDemoEventResult     m_TradeDemoEventResult;

};


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetEvent)

#endif // P2PTRADEEVENT_H_INCLUDE
