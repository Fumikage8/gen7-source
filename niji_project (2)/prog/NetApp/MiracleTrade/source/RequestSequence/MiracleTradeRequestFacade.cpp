// ============================================================================
/*
 * @file MiracleTradeRequestFacade.cpp
 * @brief ミラクル交換アプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.05.26
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeRequestFacade.h"
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeVerifyRequestSequence.h"
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeVerifyResponseListener.h"
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeMatchingRequestSequence.h"
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeMatchingResponseListener.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(RequestSequence)



gflnet2::ServerClient::RequestHandle MiracleTradeRequestFacade::AddVerifyRequest( NetApp::MiracleTrade::RequestSequence::MiracleTradeVerifyResponseListener* pVerifyResponseListener, NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::MiracleTrade::RequestSequence::MiracleTradeVerifyRequestSequence, NetApp::MiracleTrade::RequestSequence::MiracleTradeVerifyResponseListener >( pVerifyResponseListener, pMiracleTradeWork );
}


gflnet2::ServerClient::RequestHandle MiracleTradeRequestFacade::AddMatchingRequest( NetApp::MiracleTrade::RequestSequence::MiracleTradeMatchingResponseListener* pMatchingResponseListener, NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::MiracleTrade::RequestSequence::MiracleTradeMatchingRequestSequence, NetApp::MiracleTrade::RequestSequence::MiracleTradeMatchingResponseListener >( pMatchingResponseListener, pMiracleTradeWork );
}


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
