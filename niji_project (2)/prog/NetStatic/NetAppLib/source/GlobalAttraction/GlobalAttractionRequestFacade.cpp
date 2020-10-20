// ============================================================================
/*
 * @file GlobalAttractionRequestFacade.cpp
 * @brief 
 * @date 2016.01.19
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/GlobalAttraction/GlobalAttractionRequestFacade.h"
#include "NetStatic/NetAppLib/include/GlobalAttraction/GlobalAttractionOpenCheckRequestSequence.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GlobalAttraction)

gflnet2::ServerClient::RequestHandle GlobalAttractionRequestFacade::AddOpenCheckRequest( NetAppLib::GlobalAttraction::GlobalAttractionOpenCheckResponseListener* pGlobalAttractionOpenCheckResponseListener, NetAppLib::System::ApplicationWorkBase* pWork )
{
#if defined(GF_PLATFORM_CTR)
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetAppLib::GlobalAttraction::GlobalAttractionOpenCheckRequestSequence, NetAppLib::GlobalAttraction::GlobalAttractionOpenCheckResponseListener >( pGlobalAttractionOpenCheckResponseListener, pWork );
#else
  return 0;
#endif
}


GFL_NAMESPACE_END(GlobalAttraction)
GFL_NAMESPACE_END(NetAppLib)
