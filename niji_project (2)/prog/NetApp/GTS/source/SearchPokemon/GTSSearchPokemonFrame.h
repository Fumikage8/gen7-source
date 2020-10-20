// ============================================================================
/*
 * @file GTSSearchPokemonFrame.h
 * @brief 探したいポケモンを選択するプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#if !defined( GTSSEARCHPOKEMONFRAME_H_INCLUDE )
#define GTSSEARCHPOKEMONFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestHandle.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/RequestSequence/GTSSearchPokemonResponseListener.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchPokemonLowerViewListener.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchOptionLowerViewListener.h"
#include "NetApp/GTS/source/ScrollMenu/GTSScrollMenuLowerViewListener.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerViewListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(SearchPokemon)


class GTSSearchPokemonFrame :
  public NetApp::GTS::GTSFrameBase,
  public NetApp::GTS::RequestSequence::GTSSearchPokemonResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSSearchPokemonFrame );
public:
  GTSSearchPokemonFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
    NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSSearchPokemonFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

public:

  // GTSSearchPokemonResponseListener
  virtual void OnSearchPokemonSuccess();
  virtual void OnSearchPokemonCancel();
  virtual void OnSearchPokemonError();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                        m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                 m_pGTSViewObjects;
  gflnet2::ServerClient::RequestHandle                            m_SearchPokemonRequestHandle;
  NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener         m_GTSScrollMenuLowerViewListener;
  NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener   m_GTSSearchPokemonLowerViewListener;
  NetApp::GTS::SearchPokemon::GTSSearchOptionLowerViewListener    m_GTSSearchOptionLowerViewListener;
  NetApp::GTS::Window::GTSMessageWindowLowerViewListener          m_GTSMessageWindowLowerViewListener;
  bool                                                            m_bEnableSelectGenderMenu;

};


GFL_NAMESPACE_END(SearchPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSEARCHPOKEMONFRAME_H_INCLUDE
