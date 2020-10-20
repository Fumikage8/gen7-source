// ============================================================================
/*
 * @file GTSStateConfirmFrame.h
 * @brief GTSアプリを起動した時にGTSの状態を確認するプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#if !defined( GTSSTATECONFIRMFRAME_H_INCLUDE )
#define GTSSTATECONFIRMFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/RequestSequence/GTSStartStateConfirmResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadTradePokemonResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSRestorePokemonResponseListener.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerViewListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)


GFL_NAMESPACE_BEGIN(StateConfirm)


class GTSStateConfirmFrame :
  public NetApp::GTS::GTSFrameBase,
  public NetApp::GTS::RequestSequence::GTSStartStateConfirmResponseListener,
  public NetApp::GTS::RequestSequence::GTSDownloadTradePokemonResponseListener,
  public NetApp::GTS::RequestSequence::GTSRestorePokemonResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSStateConfirmFrame );
public:
  GTSStateConfirmFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
    NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSStateConfirmFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  virtual void defaultSequence();

public:

  // GTSStartStateConfirmResponseListener
  virtual void OnStartStateConfirmPokemonNotUpload();
  virtual void OnStartStateConfirmPokemonUpload();
  virtual void OnStartStateConfrimPokemonTrade();
  virtual void OnStartStateConfirmPokemonDelete();
  virtual void OnStartStateConfirmError();

  // GTSDownloadTradePokemonResponseListener
  virtual void OnDownloadTradePokemonSuccess();
  virtual void OnDownloadTradePokemonError();

  // GTSRestorePokemonResponseListener
  virtual void OnRestorePokemonSuccess();

private:

  enum RequestState
  {
    REQUEST_STATE_NULL,
    REQUEST_STATE_EXECUTE,
    REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_SUCCESS,
    REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_ERROR,
    REQUEST_STATE_COMPLETE_RESTORE_SUCCESS
  };

  NetApp::GTS::ApplicationSystem::GTSWork*                        m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                 m_pGTSViewObjects;
  NetApp::GTS::Window::GTSMessageWindowLowerViewListener          m_GTSMessageWindowLowerViewListener;
  NetAppLib::Util::NetAppTimeout                                  m_Timer;
  RequestState                                                    m_eRequestState;

};


GFL_NAMESPACE_END(StateConfirm)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSTATECONFIRMFRAME_H_INCLUDE
