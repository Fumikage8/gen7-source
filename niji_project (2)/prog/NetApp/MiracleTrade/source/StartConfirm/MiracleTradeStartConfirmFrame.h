// ============================================================================
/*
 * @file MiracleTradeStartConfirmFrame.h
 * @brief ミラクル交換を開始するか確認するプロセスです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADESTARTCONFIRMFRAME_H_INCLUDE )
#define MIRACLETRADESTARTCONFIRMFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/MiracleTrade/source/MiracleTradeFrameBase.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class MiracleTradeViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(StartConfirm)


class MiracleTradeStartConfirmFrame :
  public NetApp::MiracleTrade::MiracleTradeFrameBase,
  public NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmLowerViewListener
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeStartConfirmFrame );
public:
  MiracleTradeStartConfirmFrame(
    NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork,
    NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects* pMiracleTradeViewObjects );
  virtual~MiracleTradeStartConfirmFrame();

private:

  // MiracleTradeFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();

public:

  // MiracleTradeStartConfirmLowerViewListener
  virtual void OnTouchBackButtonFromMiracleTradeStartConfirmLower();
  virtual void OnTouchStartButton();

private:

  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*                       m_pMiracleTradeWork;
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects*      m_pMiracleTradeViewObjects;

};


GFL_NAMESPACE_END(StartConfirm)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADESTARTCONFIRMFRAME_H_INCLUDE
