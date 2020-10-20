// ============================================================================
/*
 * @file MiracleTradeMatchingFrame.h
 * @brief ミラクル交換のマッチングプロセスです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEMATCHINGFRAME_H_INCLUDE )
#define MIRACLETRADEMATCHINGFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/MiracleTrade/source/MiracleTradeFrameBase.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradePlayerInfoLowerViewListener.h"
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeVerifyResponseListener.h"
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeMatchingResponseListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class MiracleTradeViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(Matching)


class MiracleTradeMatchingFrame :
  public NetApp::MiracleTrade::MiracleTradeFrameBase,
  public NetApp::MiracleTrade::RequestSequence::MiracleTradeVerifyResponseListener,
  public NetApp::MiracleTrade::RequestSequence::MiracleTradeMatchingResponseListener,
  public NetApp::MiracleTrade::Matching::MiracleTradePlayerInfoLowerViewListener
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeMatchingFrame );
public:
  MiracleTradeMatchingFrame(
    NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork,
    NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects* pMiracleTradeViewObjects );
  virtual~MiracleTradeMatchingFrame();

private:

  // MiracleTradeFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();


public:

  // MiracleTradeVerifyResponseListener
  virtual void OnMiracleTradeVerifyOK();
  virtual void OnMiracleTradeVerifyNG();
  virtual void OnMiracleTradeVerifyError();

  // MiracleTradeMatchingResponseListener
  virtual void OnMiracleTradeMatchingSuccess();
  virtual void OnMiracleTradeMatchingError();
  virtual void OnMiracleTradeMatchingTimeOut();
  virtual void OnMiracleTradeMatchingUpdateTime( s32 nTime );
  virtual void OnMiracleTradeMatchingTimeCounterOff();
  virtual void OnMiracleTradeMatchingVerifyNG();

  // MiracleTradePlayerInfoLowerViewListener
  virtual void OnTouchScreen();

private:

  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*             m_pMiracleTradeWork;
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects*      m_pMiracleTradeViewObjects;
  NetAppLib::Util::NetAppTimeout                                         m_ExitTimer;
  bool                                                                   m_bExitMatchingSuccess;

};


GFL_NAMESPACE_END(Matching)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEMATCHINGFRAME_H_INCLUDE
