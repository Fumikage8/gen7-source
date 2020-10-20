// ============================================================================
/*
 * @file MiracleTradeRequestFacade.h
 * @brief ミラクル交換アプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.05.26
 */
// ============================================================================
#if !defined( MIRACLETRADEREQUESTFACADE_H_INCLUDE )
#define MIRACLETRADEREQUESTFACADE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(RequestSequence)


class MiracleTradeVerifyResponseListener;
class MiracleTradeMatchingResponseListener;



class MiracleTradeRequestFacade
{
public:
  static gflnet2::ServerClient::RequestHandle AddVerifyRequest( NetApp::MiracleTrade::RequestSequence::MiracleTradeVerifyResponseListener* pVerifyResponseListener, NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork );
  static gflnet2::ServerClient::RequestHandle AddMatchingRequest( NetApp::MiracleTrade::RequestSequence::MiracleTradeMatchingResponseListener* pMatchingResponseListener, NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork );


};



GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEREQUESTFACADE_H_INCLUDE
