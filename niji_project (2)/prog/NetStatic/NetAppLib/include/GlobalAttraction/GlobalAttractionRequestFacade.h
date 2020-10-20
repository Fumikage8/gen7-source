// ============================================================================
/*
 * @file GlobalAttractionRequestFacade.h
 * @brief 
 * @date 2016.01.19
 */
// ============================================================================
#if !defined( GLOBAL_ATTRACTION_REQUEST_FACADE_H_INCLUDE )
#define GLOBAL_ATTRACTION_REQUEST_FACADE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GlobalAttraction)


class GlobalAttractionOpenCheckResponseListener;


class GlobalAttractionRequestFacade
{
public:
  static gflnet2::ServerClient::RequestHandle AddOpenCheckRequest( NetAppLib::GlobalAttraction::GlobalAttractionOpenCheckResponseListener* pGlobalAttractionOpenCheckResponseListener, NetAppLib::System::ApplicationWorkBase* pWork );

};


GFL_NAMESPACE_END(GlobalAttraction)
GFL_NAMESPACE_END(NetAppLib)

#endif // GLOBAL_ATTRACTION_REQUEST_FACADE_H_INCLUDE
