// ============================================================================
/*
 * @file MiracleTradeEvent.h
 * @brief ミラクル交換イベント
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEEVENT_H_INCLUDE )
#define MIRACLETRADEEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/MiracleTrade/include/MiracleTradeAppParam.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase
#include "App/AppEvent/include/Box/BoxEventListener.h"
#include "App/AppDemo/include/AppDemoProcParam.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
class MiracleTradeProc;
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(MiracleTrade)


class MiracleTradeEvent :
  public NetEvent::NetEventBase,
  public App::Event::BoxEventListener
{

  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeEvent );

public:

  static void StartEvent( GameSys::GameManager* pGameManager );

  MiracleTradeEvent( gfl2::heap::HeapBase* pHeap );
  virtual~MiracleTradeEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  virtual void OnTermination( u8 outJumpMode, u8 outTray,u8 outPos );

private:

  void bootMiracleTradeProc( NetAppLib::System::FrameID startFrameID );
  void bootBoxEvent( GameSys::GameManager* pGameManager );
  void bootTradeDemoEvent( GameSys::GameManager* pGameManager );

private:

  enum Sequence
  {
    SEQUENCE_MIRACLE_TRADE,
    SEQUENCE_BOX_SELECT_WAIT,
    SEQUENCE_BOX_SELECT_END,
    SEQUENCE_TRADE_DEMO
  };

  gfl2::heap::HeapBase*                                         m_pWorkHeap;
  NetApp::MiracleTrade::APP_PARAM                               m_AppParam;
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*    m_pMiracleTradeWork;
  NetApp::MiracleTrade::MiracleTradeProc*                       m_pMiracleTradeProc;

  pml::pokepara::PokemonParam*                                  m_pDemoSendPokemon;
  pml::pokepara::PokemonParam*                                  m_pDemoRecvPokemon;

  App::AppDemo::AppDemoProcParam_MiracleTrade                   m_DemoParam;

};


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetEvent)

#endif // MIRACLETRADEEVENT_H_INCLUDE
