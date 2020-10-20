// ============================================================================
/*
 * @file P2PTradeProc.h
 * @brief P2P交換アプリのプロッククラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_PROC_H_INCLUDE )
#define P2PTRADE_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/P2PTrade/include/P2PTradeAppParam.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeResponseListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)

GFL_NAMESPACE_BEGIN(System)
class P2PTradeApplicationSystem;
class P2PTradeWork;
GFL_NAMESPACE_END(System)


class P2PTradeProc :
  public GameSys::GameProc,
  NetAppLib::P2PTrade::P2PTradeResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeProc );
public:
  P2PTradeProc( NetApp::P2PTrade::APP_PARAM* pAppParam );
  virtual~P2PTradeProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

  void SetP2PTradeRequestClient( NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient );

  void SetAppResult( NetApp::P2PTrade::APP_RESULT* pAppResult );

  // P2PTradeResponseListener
  virtual void OnMessageIDReceived( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, void* pPokemonParamBuffer );
  virtual void OnPokemonParamReceived( const pml::pokepara::PokemonParam& pokemonParam );
  virtual void OnFirstTradeSuccess( const pml::pokepara::PokemonParam& pokemonParam );
  virtual void OnFirstTradePokemonInvalid( bool bMyPokemonInvalid );
  virtual void OnFirstTradeValidationError( bool bMyValidationError );
  virtual void OnFirstTradeCancel();
  virtual void OnFinalTradeSuccess();
  virtual void OnFinalTradeComplete();
  virtual void OnFinalTradeCancel();
  virtual void OnTradeCancelReceived();
  virtual void OnTradeExitReceived();
  virtual void OnTradeExitSuccess();
  virtual void OnFinalSyncComplete();
  virtual void OnFinalSyncCancelComplete();

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::P2PTrade::APP_PARAM                              m_AppParam;
  NetApp::P2PTrade::APP_RESULT*                            m_pAppResult;
  NetApp::P2PTrade::System::P2PTradeWork*                  m_pP2PTradeWork;
  NetApp::P2PTrade::System::P2PTradeApplicationSystem*     m_pApplicationSystem;
  NetAppLib::P2PTrade::P2PTradeRequestClient*              m_pP2PTradeRequestClient;

};


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADE_PROC_H_INCLUDE
