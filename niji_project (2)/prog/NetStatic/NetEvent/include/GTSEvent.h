// ============================================================================
/*
 * @file GTSEvent.h
 * @brief GTSイベント
 * @date 2015.03.17
 */
// ============================================================================
#if !defined( GTSEVENT_H_INCLUDE )
#define GTSEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <ro/include/gfl2_RoManager.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/GTS/include/GTSAppParam.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase
#include "App/AppEvent/include/Box/BoxEventListener.h"
#include "App/AppDemo/include/AppDemoProcParam.h"
#include "App/StrInput/include/StrInputAppParam.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
class GTSProc;
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#if PM_DEBUG
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDebugWin;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)
#endif // PM_DEBUG

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(GTS)


class GTSEvent :
  public NetEvent::NetEventBase,
  public App::Event::BoxEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager );

  GTSEvent( gfl2::heap::HeapBase* pHeap );
  virtual~GTSEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  virtual void OnTermination( u8 outJumpMode,u8 outTray, u8 outPos );

private:

  void bootGTSProc( NetAppLib::System::FrameID startFrameID );
  void shutdownGTSProc();
  void bootBoxEvent( GameSys::GameManager* pGameManager );
  void bootTradeDemoEvent( GameSys::GameManager* pGameManager );
  void bootNameInputEvent( GameSys::GameManager* pGameManager );

private:

  enum Sequence
  {
    SEQUENCE_GTS,
    SEQUENCE_BOX_SELECT_WAIT,
    SEQUENCE_BOX_SELECT_END,
    SEQUENCE_TRADE_DEMO,
    SEQUENCE_NAME_INPUT
  };

  gfl2::ro::Module*                             m_pBoxModule;
  gfl2::ro::Module*                             m_pRaderChartModule;
#if PM_DEBUG
  gfl2::ro::Module*                             m_pZukanModule;
  app::zukan::ZukanDebugWin*                    m_pZukanDebugWin;
#endif // PM_DEBUG
  gfl2::heap::HeapBase*                         m_pWorkHeap;
  NetApp::GTS::APP_PARAM                        m_AppParam;
  NetApp::GTS::ApplicationSystem::GTSWork*      m_pGTSWork;
  NetApp::GTS::GTSProc*                         m_pGTSProc;

  pml::pokepara::PokemonParam*                  m_pDemoSendPokemon;
  pml::pokepara::PokemonParam*                  m_pDemoRecvPokemon;

  App::AppDemo::AppDemoProcParam_GTS            m_DemoParam;
  App::StrInput::APP_PARAM                      m_StrInputAppParam;

};


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetEvent)

#endif // GTSEVENT_H_INCLUDE
