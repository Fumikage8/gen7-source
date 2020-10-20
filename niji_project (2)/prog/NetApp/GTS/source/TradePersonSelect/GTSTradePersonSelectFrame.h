// ============================================================================
/*
 * @file GTSTradePersonSelectFrame.h
 * @brief 交換相手を選択するプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#if !defined( GTSTRADEPERSONSELECTFRAME_H_INCLUDE )
#define GTSTRADEPERSONSELECTFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestHandle.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/RequestSequence/GTSSearchPokemonResponseListener.h"
#include "NetApp/GTS/source/TradePersonSelect/GTSTradePersonSelectLowerViewListener.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(TradePersonSelect)


class GTSTradePersonSelectFrame :
  public NetApp::GTS::GTSFrameBase,
  public NetApp::GTS::RequestSequence::GTSSearchPokemonResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTradePersonSelectFrame );
public:
  GTSTradePersonSelectFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
    NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSTradePersonSelectFrame();

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

  NetApp::GTS::ApplicationSystem::GTSWork*                                m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                         m_pGTSViewObjects;
  bool                                                                    m_bBegin;
  NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener   m_GTSTradePersonSelectLowerViewListener;
  NetApp::GTS::Window::GTSMessageWindowLowerViewListener                  m_GTSMessageWindowLowerViewListener;
  u32                                                                     m_RetrySearchCount;
  bool                                                                    m_bIsSearchPokemonRequest;
  bool                                                                    m_bCursorVisibleFlag;

};


GFL_NAMESPACE_END(TradePersonSelect)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPERSONSELECTFRAME_H_INCLUDE
