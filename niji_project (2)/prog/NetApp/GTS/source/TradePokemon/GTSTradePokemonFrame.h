// ============================================================================
/*
 * @file GTSTradePokemonFrame.h
 * @brief ポケモンを交換するプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#if !defined( GTSTRADEPOKEMONFRAME_H_INCLUDE )
#define GTSTRADEPOKEMONFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestHandle.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonVerifyResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonConfirmResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonResponseListener.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerViewListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(TradePokemon)


class GTSTradePokemonFrame :
  public NetApp::GTS::GTSFrameBase,
  public NetApp::GTS::RequestSequence::GTSTradePokemonVerifyResponseListener,
  public NetApp::GTS::RequestSequence::GTSTradePokemonConfirmResponseListener,
  public NetApp::GTS::RequestSequence::GTSTradePokemonResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTradePokemonFrame );
public:
  GTSTradePokemonFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
    NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSTradePokemonFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  virtual void defaultSequence();

public:

  // GTSTradePokemonVerifyResponseListener
  virtual void OnTradePokemonVerifyOK();
  virtual void OnTradePokemonVerifyNG();
  virtual void OnTradePokemonVerifyError();

  // GTSTradePokemonConfirmResponseListener
  virtual void OnTradePokemonAlreadyTradePokemon();
  virtual void OnTradePokemonConfirmSuccess();
  virtual void OnTradePokemonConfirmError();

  // GTSTradePokemonResponseListener
  virtual void OnTradePokemonSuccess();
  virtual void OnTradePokemonError();

private:

  enum RequestState
  {
    REQUEST_STATE_NULL,
    REQUEST_STATE_EXECUTE,
    REQUEST_STATE_COMPLETE_TRADE_SUCCESS,
    REQUEST_STATE_COMPLETE_TRADE_ERROR
  };

  NetApp::GTS::ApplicationSystem::GTSWork*                            m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                     m_pGTSViewObjects;
  NetApp::GTS::Window::GTSMessageWindowLowerViewListener              m_GTSMessageWindowLowerViewListener;
  NetAppLib::Util::NetAppTimeout                                      m_Timer;
  RequestState                                                        m_eRequestState;

};


GFL_NAMESPACE_END(TradePokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPOKEMONFRAME_H_INCLUDE
