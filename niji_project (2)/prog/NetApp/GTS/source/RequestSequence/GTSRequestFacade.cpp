// ============================================================================
/*
 * @file GTSRequestFacade.cpp
 * @brief GTSアプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.03.25
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSRequestFacade.h"
#include "NetApp/GTS/source/RequestSequence/GTSStartStateConfirmRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSStartStateConfirmResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSRestorePokemonRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSRestorePokemonResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadStateConfirmRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadStateConfirmResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadTradePokemonRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadTradePokemonResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadPokemonRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadPokemonResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSSearchPokemonRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSSearchPokemonResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonVerifyRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonVerifyResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonVerifyRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonVerifyResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonConfirmRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonConfirmResponseListener.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonRequestSequence.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonResponseListener.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)



gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddStartStateConfirmRequest( NetApp::GTS::RequestSequence::GTSStartStateConfirmResponseListener* pStartStateConfirmResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSStartStateConfirmRequestSequence, NetApp::GTS::RequestSequence::GTSStartStateConfirmResponseListener >( pStartStateConfirmResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddRestorePokemonRequest( NetApp::GTS::RequestSequence::GTSRestorePokemonResponseListener* pRestorePokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSRestorePokemonRequestSequence, NetApp::GTS::RequestSequence::GTSRestorePokemonResponseListener >( pRestorePokemonResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddDownloadStateConfirmRequest( NetApp::GTS::RequestSequence::GTSDownloadStateConfirmResponseListener* pDownloadStateConfirmResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSDownloadStateConfirmRequestSequence, NetApp::GTS::RequestSequence::GTSDownloadStateConfirmResponseListener >( pDownloadStateConfirmResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddDownloadTradePokemonRequest( NetApp::GTS::RequestSequence::GTSDownloadTradePokemonResponseListener* pDownloadTradePokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSDownloadTradePokemonRequestSequence, NetApp::GTS::RequestSequence::GTSDownloadTradePokemonResponseListener >( pDownloadTradePokemonResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddDownloadPokemonRequest( NetApp::GTS::RequestSequence::GTSDownloadPokemonResponseListener* pDownloadPokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSDownloadPokemonRequestSequence, NetApp::GTS::RequestSequence::GTSDownloadPokemonResponseListener >( pDownloadPokemonResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddSearchPokemonRequest( NetApp::GTS::RequestSequence::GTSSearchPokemonResponseListener* pSearchPokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSSearchPokemonRequestSequence, NetApp::GTS::RequestSequence::GTSSearchPokemonResponseListener >( pSearchPokemonResponseListener, pGTSWork );
}


void GTSRequestFacade::CancelSearchPokemonRequest( gflnet2::ServerClient::RequestHandle handle )
{
  gflnet2::ServerClient::ServerClientRequestManager::CancelRequest( handle );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddUploadPokemonVerifyRequest( NetApp::GTS::RequestSequence::GTSUploadPokemonVerifyResponseListener* pUploadPokemonVerifyResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSUploadPokemonVerifyRequestSequence, NetApp::GTS::RequestSequence::GTSUploadPokemonVerifyResponseListener >( pUploadPokemonVerifyResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddUploadPokemonRequest( NetApp::GTS::RequestSequence::GTSUploadPokemonResponseListener* pUploadPokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSUploadPokemonRequestSequence, NetApp::GTS::RequestSequence::GTSUploadPokemonResponseListener >( pUploadPokemonResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddTradePokemonVerifyRequest( NetApp::GTS::RequestSequence::GTSTradePokemonVerifyResponseListener* pTradePokemonVerifyResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSTradePokemonVerifyRequestSequence, NetApp::GTS::RequestSequence::GTSTradePokemonVerifyResponseListener >( pTradePokemonVerifyResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddTradePokemonConfirmRequest( NetApp::GTS::RequestSequence::GTSTradePokemonConfirmResponseListener* pTradePokemonConfirmResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSTradePokemonConfirmRequestSequence, NetApp::GTS::RequestSequence::GTSTradePokemonConfirmResponseListener >( pTradePokemonConfirmResponseListener, pGTSWork );
}


gflnet2::ServerClient::RequestHandle GTSRequestFacade::AddTradePokemonRequest( NetApp::GTS::RequestSequence::GTSTradePokemonResponseListener* pTradePokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::GTS::RequestSequence::GTSTradePokemonRequestSequence, NetApp::GTS::RequestSequence::GTSTradePokemonResponseListener >( pTradePokemonResponseListener, pGTSWork );
}


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
