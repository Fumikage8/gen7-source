// ============================================================================
/*
 * @file JoinFestaGlobalAttractionRequestFacade.cpp
 * @brief レギュレーション選択アプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.03.25
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/RequestSequence/JoinFestaGlobalAttractionRequestFacade.h"
#include "NetApp/JoinFestaGlobalAttraction/source/RequestSequence/JoinFestaGlobalAttractionDownloadRequestSequence.h"
#include "NetApp/JoinFestaGlobalAttraction/source/RequestSequence/JoinFestaGlobalAttractionDownloadResponseListener.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(RequestSequence)


gflnet2::ServerClient::RequestHandle JoinFestaGlobalAttractionRequestFacade::AddDownloadRequest( NetApp::JoinFestaGlobalAttraction::RequestSequence::JoinFestaGlobalAttractionDownloadResponseListener* pJoinFestaGlobalAttractionDownloadResponseListener, NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::JoinFestaGlobalAttraction::RequestSequence::JoinFestaGlobalAttractionDownloadRequestSequence, NetApp::JoinFestaGlobalAttraction::RequestSequence::JoinFestaGlobalAttractionDownloadResponseListener >( pJoinFestaGlobalAttractionDownloadResponseListener, pJoinFestaGlobalAttractionWork );
}


void JoinFestaGlobalAttractionRequestFacade::CancelDownloadRequest( gflnet2::ServerClient::RequestHandle handle )
{
  gflnet2::ServerClient::ServerClientRequestManager::CancelRequest( handle );
}


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
