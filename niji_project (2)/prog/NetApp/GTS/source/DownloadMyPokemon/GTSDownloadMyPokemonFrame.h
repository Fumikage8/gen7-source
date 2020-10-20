// ============================================================================
/*
 * @file GTSDownloadMyPokemonFrame.h
 * @brief ポケモンを引き取るプロセスです。
 * @date 2015.04.13
 */
// ============================================================================
#if !defined( GTSDOWNLOADMYPOKEMONFRAME_H_INCLUDE )
#define GTSDOWNLOADMYPOKEMONFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestHandle.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadStateConfirmResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadTradePokemonResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadPokemonResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSRestorePokemonResponseListener.h"
#include "NetApp/GTS/source/SelectMenu/GTSOneSelectMenuLowerViewListener.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerViewListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(DownloadMyPokemon)


class GTSDownloadMyPokemonFrame :
  public NetApp::GTS::GTSFrameBase,
  public NetApp::GTS::RequestSequence::GTSDownloadStateConfirmResponseListener,
  public NetApp::GTS::RequestSequence::GTSDownloadTradePokemonResponseListener,
  public NetApp::GTS::RequestSequence::GTSDownloadPokemonResponseListener,
  public NetApp::GTS::RequestSequence::GTSRestorePokemonResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSDownloadMyPokemonFrame );
public:
  GTSDownloadMyPokemonFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
    NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSDownloadMyPokemonFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  virtual void defaultSequence();

public:

  // GTSDownloadStateConfirmResponseListener
  virtual void OnDownloadStateConfirmPokemonTrade();
  virtual void OnDownloadStateConfirmPokemonNotTrade();
  virtual void OnDownloadStateConfirmPokemonDelete();
  virtual void OnDownloadStateConfirmError();

  // GTSDownloadTradePokemonResponseListener
  virtual void OnDownloadTradePokemonSuccess();
  virtual void OnDownloadTradePokemonError();

  // GTSDownloadPokemonResponseListener
  virtual void OnDownloadPokemonSuccess();
  virtual void OnDownloadPokemonError();

  // GTSRestorePokemonResponseListener
  virtual void OnRestorePokemonSuccess();

private:

  enum RequestState
  {
    REQUEST_STATE_NULL,
    REQUEST_STATE_EXECUTE,
    REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_SUCCESS,
    REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_ERROR,
    REQUEST_STATE_COMPLETE_DOWNLOAD_SUCCESS,
    REQUEST_STATE_COMPLETE_DOWNLOAD_ERROR,
    REQUEST_STATE_COMPLETE_RESTORE_SUCCESS
  };

  NetApp::GTS::ApplicationSystem::GTSWork*                            m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                     m_pGTSViewObjects;
  bool                                                                m_bDeletePokemon;
  NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerViewListener          m_GTSOneSelectMenuLowerViewListener;
  NetApp::GTS::Window::GTSMessageWindowLowerViewListener              m_GTSMessageWindowLowerViewListener;
  NetAppLib::Util::NetAppTimeout                                      m_Timer;
  RequestState                                                        m_eRequestState;

};


GFL_NAMESPACE_END(DownloadMyPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSDOWNLOADMYPOKEMONFRAME_H_INCLUDE
