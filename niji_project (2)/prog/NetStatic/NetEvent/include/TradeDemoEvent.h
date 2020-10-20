// ============================================================================
/*
 * @file TradeDemoEvent.h
 * @brief 交換デモ専用のイベント
 * @date 2016.01.05
 * @author Akira endo
 */
// ============================================================================
#if !defined( TRADEDEMOEVENT_H_INCLUDE )
#define TRADEDEMOEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
#include "App/AppDemo/include/AppDemoProcParam.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "NetStatic/NetEvent/include/EvolutionEvent.h"
#include "NetApp/P2PTrade/include/P2PTradeAppParam.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
class P2PTradeProc;
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)
class P2PTradeRequestClient;
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(AppDemo)
class AppDemoProc_GTS;
class AppDemoProc_MiracleTrade;
GFL_NAMESPACE_END(AppDemo)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(TradeDemo)


typedef struct
{
  App::AppDemo::AppDemoProcParam_GTS*            pGTSTradeDemoParam;
  app::event::ZukanRegisterEventParam*           pZukanRegisterEventParam;
  NetEvent::Evolution::EvolutionEventParam*      pEvolutionEventParam;
  NetApp::P2PTrade::APP_PARAM                    p2pTradeParam;
  bool                                           bCallZukanRegister;
  bool                                           bCallEvolution;
} TradeDemoEventParam_GTS;


typedef struct
{
  NetAppLib::P2PTrade::P2PTradeRequestClient*    pP2PTradeRequestClient;
  App::AppDemo::AppDemoProcParam_MiracleTrade*   pMiracleTradeDemoParam;
  app::event::ZukanRegisterEventParam*           pZukanRegisterEventParam;
  NetEvent::Evolution::EvolutionEventParam*      pEvolutionEventParam;
  NetApp::P2PTrade::APP_PARAM                    p2pTradeParam;
  bool                                           bCallZukanRegister;
  bool                                           bCallEvolution;
} TradeDemoEventParam_MiracleTrade;


typedef struct
{
  NetApp::P2PTrade::APP_RESULT                   p2pTradeResult;
} TradeDemoEventResult;


class TradeDemoEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( TradeDemoEvent );

public:

  static void StartEvent( GameSys::GameManager* pGameManager, TradeDemoEventParam_GTS& param, TradeDemoEventResult* pResult = NULL );
  static void StartEvent( GameSys::GameManager* pGameManager, TradeDemoEventParam_MiracleTrade& param, TradeDemoEventResult* pResult = NULL );

  TradeDemoEvent( gfl2::heap::HeapBase* pHeap );
  virtual~TradeDemoEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetParam( TradeDemoEventParam_GTS& param );
  void SetParam( TradeDemoEventParam_MiracleTrade& param );

  void SetResult( TradeDemoEventResult* pResult );

private:

  void copy( pml::pokepara::PokemonParam& dest, pml::pokepara::PokemonParam& src );
  void bootDemoProc();
  void bootZukanRegisterEvent( GameSys::GameManager* pGameManager );
  void bootEvolutionEvent( GameSys::GameManager* pGameManager );
  void bootSaveProc();

private:

  NetApp::P2PTrade::P2PTradeProc*                 m_pP2PTradeProc;
  App::AppDemo::AppDemoProc_GTS*                  m_pGTSDemoProc;
  App::AppDemo::AppDemoProc_MiracleTrade*         m_pMiracleTradeDemoProc;

  App::AppDemo::AppDemoProcParam_GTS*             m_pGTSTradeDemoParam;
  App::AppDemo::AppDemoProcParam_MiracleTrade*    m_pMiracleTradeDemoParam;
  app::event::ZukanRegisterEventParam*            m_pZukanRegisterEventParam;
  NetEvent::Evolution::EvolutionEventParam*       m_pEvolutionEventParam;
  NetApp::P2PTrade::APP_PARAM                     m_P2PTradeParam;

  TradeDemoEventResult*                           m_pTradeDemoEventResult;
  NetAppLib::P2PTrade::P2PTradeRequestClient*     m_pP2PTradeRequestClient;

  bool                                            m_bCallZukanRegister;
  bool                                            m_bCallEvolution;


};


GFL_NAMESPACE_END(TradeDemo)
GFL_NAMESPACE_END(NetEvent)

#endif // TRADEDEMOEVENT_H_INCLUDE
