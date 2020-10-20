// ============================================================================
/*
 * @file RegulationDLRequestFacade.cpp
 * @brief レギュレーション選択アプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.03.25
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/RequestSequence/RegulationDLRequestFacade.h"
#include "NetApp/RegulationSelect/source/RequestSequence/RegulationDLSearchDataRequestSequence.h"
#include "NetApp/RegulationSelect/source/RequestSequence/RegulationDLSearchDataResponseListener.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(RequestSequence)


gflnet2::ServerClient::RequestHandle RegulationDLRequestFacade::AddSearchDataRequest( NetApp::RegulationSelect::RequestSequence::RegulationDLSearchDataResponseListener* pRegulationDLSearchDataResponseListener, NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::RegulationSelect::RequestSequence::RegulationDLSearchDataRequestSequence, NetApp::RegulationSelect::RequestSequence::RegulationDLSearchDataResponseListener >( pRegulationDLSearchDataResponseListener, pRegulationSelectWork );
}


void RegulationDLRequestFacade::CancelSearchDataRequest( gflnet2::ServerClient::RequestHandle handle )
{
  gflnet2::ServerClient::ServerClientRequestManager::CancelRequest( handle );
}


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
