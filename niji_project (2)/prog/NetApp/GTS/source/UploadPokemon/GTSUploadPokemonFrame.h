// ============================================================================
/*
 * @file GTSUploadPokemonFrame.h
 * @brief ポケモンを預けるプロセスです。
 * @date 2015.04.08
 */
// ============================================================================
#if !defined( GTSUPLOADPOKEMONFRAME_H_INCLUDE )
#define GTSUPLOADPOKEMONFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestHandle.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonVerifyResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonResponseListener.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadPokemonLowerViewListener.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadMessageSelectLowerViewListener.h"
#include "NetApp/GTS/source/ScrollMenu/GTSScrollMenuLowerViewListener.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerViewListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(UploadPokemon)


class GTSUploadPokemonFrame :
  public NetApp::GTS::GTSFrameBase,
  public NetApp::GTS::RequestSequence::GTSUploadPokemonVerifyResponseListener,
  public NetApp::GTS::RequestSequence::GTSUploadPokemonResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSUploadPokemonFrame );
public:
  GTSUploadPokemonFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
    NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSUploadPokemonFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  virtual void defaultSequence();

public:

  // GTSUploadPokemonVerifyResponseListener
  virtual void OnUploadPokemonVerifySuccess();
  virtual void OnUploadPokemonVerifyError();

  // GTSUploadPokemonResponseListener
  virtual void OnUploadPokemonSuccess();
  virtual void OnUploadPokemonError();

private:

  enum RequestState
  {
    REQUEST_STATE_NULL,
    REQUEST_STATE_EXECUTE,
    REQUEST_STATE_COMPLETE_SUCCESS,
    REQUEST_STATE_COMPLETE_ERROR
  };

  NetApp::GTS::ApplicationSystem::GTSWork*                              m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                       m_pGTSViewObjects;
  NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener         m_GTSUploadPokemonLowerViewListener;
  NetApp::GTS::UploadPokemon::GTSUploadMessageSelectLowerViewListener   m_GTSUploadMessageSelectLowerViewListener;
  NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener               m_GTSScrollMenuLowerViewListener;
  NetApp::GTS::Window::GTSMessageWindowLowerViewListener                m_GTSMessageWindowLowerViewListener;
  NetAppLib::Util::NetAppTimeout                                        m_Timer;
  bool                                                                  m_bEnableSelectGenderMenu;
  RequestState                                                          m_eRequestState;

};


GFL_NAMESPACE_END(UploadPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADPOKEMONFRAME_H_INCLUDE
