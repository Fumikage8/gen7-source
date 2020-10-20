// ============================================================================
/*
 * @file GameSyncRequestFacade.cpp
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncCreateSaveDataRequestSequence.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncCreateSaveDataResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusRequestSequence.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncSaveDataSyncRequestSequence.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncSaveDataSyncResponseListener.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)


gflnet2::ServerClient::RequestHandle GameSyncRequestFacade::AddCreateSaveDataRequest( NetAppLib::GameSync::GameSyncCreateSaveDataResponseListener* pGameSyncCreateSaveDataResponseListener, NetAppLib::GameSync::GAMESYNC_PARAM* pWork )
{
#if defined(GF_PLATFORM_CTR)
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetAppLib::GameSync::GameSyncCreateSaveDataRequestSequence, NetAppLib::GameSync::GameSyncCreateSaveDataResponseListener >( pGameSyncCreateSaveDataResponseListener, pWork );
#else
  return 0;
#endif
}

gflnet2::ServerClient::RequestHandle GameSyncRequestFacade::AddConfirmPlayStatusRequest( NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener* pGameSyncConfirmPlayStatusResponseListener, NetAppLib::GameSync::GETPLAYSTATUS_PARAM* pWork )
{
#if defined(GF_PLATFORM_CTR)
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetAppLib::GameSync::GameSyncConfirmPlayStatusRequestSequence, NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener >( pGameSyncConfirmPlayStatusResponseListener, pWork );
#else
  return 0;
#endif
}

gflnet2::ServerClient::RequestHandle GameSyncRequestFacade::AddSaveDataSyncRequest( NetAppLib::GameSync::GameSyncSaveDataSyncResponseListener* pGameSyncSaveDataSyncResponseListener, NetAppLib::GameSync::GAMESYNC_PARAM* pWork )
{
#if defined(GF_PLATFORM_CTR)
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetAppLib::GameSync::GameSyncSaveDataSyncRequestSequence, NetAppLib::GameSync::GameSyncSaveDataSyncResponseListener >( pGameSyncSaveDataSyncResponseListener, pWork );
#else
  return 0;
#endif
}

GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)
