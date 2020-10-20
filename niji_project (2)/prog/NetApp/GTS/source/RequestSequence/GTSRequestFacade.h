// ============================================================================
/*
 * @file GTSRequestFacade.h
 * @brief GTSアプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.03.25
 */
// ============================================================================
#if !defined( GTSREQUESTFACADE_H_INCLUDE )
#define GTSREQUESTFACADE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSStartStateConfirmResponseListener;
class GTSRestorePokemonResponseListener;
class GTSDownloadStateConfirmResponseListener;
class GTSDownloadTradePokemonResponseListener;
class GTSDownloadPokemonResponseListener;
class GTSUploadPokemonVerifyResponseListener;
class GTSUploadPokemonResponseListener;
class GTSSearchPokemonResponseListener;
class GTSTradePokemonVerifyResponseListener;
class GTSTradePokemonConfirmResponseListener;
class GTSTradePokemonResponseListener;


class GTSRequestFacade
{
public:
  static gflnet2::ServerClient::RequestHandle AddStartStateConfirmRequest( NetApp::GTS::RequestSequence::GTSStartStateConfirmResponseListener* pStartStateConfirmResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  static gflnet2::ServerClient::RequestHandle AddRestorePokemonRequest( NetApp::GTS::RequestSequence::GTSRestorePokemonResponseListener* pRestorePokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );

  static gflnet2::ServerClient::RequestHandle AddDownloadStateConfirmRequest( NetApp::GTS::RequestSequence::GTSDownloadStateConfirmResponseListener* pDownloadStateConfirmResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  static gflnet2::ServerClient::RequestHandle AddDownloadTradePokemonRequest( NetApp::GTS::RequestSequence::GTSDownloadTradePokemonResponseListener* pDownloadTradePokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  static gflnet2::ServerClient::RequestHandle AddDownloadPokemonRequest( NetApp::GTS::RequestSequence::GTSDownloadPokemonResponseListener* pDownloadPokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );

  static gflnet2::ServerClient::RequestHandle AddSearchPokemonRequest( NetApp::GTS::RequestSequence::GTSSearchPokemonResponseListener* pSearchPokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  static void CancelSearchPokemonRequest( gflnet2::ServerClient::RequestHandle handle );

  static gflnet2::ServerClient::RequestHandle AddUploadPokemonVerifyRequest( NetApp::GTS::RequestSequence::GTSUploadPokemonVerifyResponseListener* pUploadPokemonVerifyResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  static gflnet2::ServerClient::RequestHandle AddUploadPokemonRequest( NetApp::GTS::RequestSequence::GTSUploadPokemonResponseListener* pUploadPokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );

  static gflnet2::ServerClient::RequestHandle AddTradePokemonVerifyRequest( NetApp::GTS::RequestSequence::GTSTradePokemonVerifyResponseListener* pTradePokemonVerifyResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  static gflnet2::ServerClient::RequestHandle AddTradePokemonConfirmRequest( NetApp::GTS::RequestSequence::GTSTradePokemonConfirmResponseListener* pTradePokemonConfirmResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  static gflnet2::ServerClient::RequestHandle AddTradePokemonRequest( NetApp::GTS::RequestSequence::GTSTradePokemonResponseListener* pTradePokemonResponseListener, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );


};



GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSREQUESTFACADE_H_INCLUDE
