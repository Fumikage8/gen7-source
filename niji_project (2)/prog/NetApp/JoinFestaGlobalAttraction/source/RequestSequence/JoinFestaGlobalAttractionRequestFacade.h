// ============================================================================
/*
 * @file JoinFestaGlobalAttractionRequestFacade.h
 * @brief レギュレーション選択アプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.06.30
 */
// ============================================================================
#if !defined( JOINFESTAGLOABALATTRACTIONREQUESTFACADE_H_INCLUDE )
#define JOINFESTAGLOABALATTRACTIONREQUESTFACADE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(RequestSequence)


class JoinFestaGlobalAttractionDownloadResponseListener;


class JoinFestaGlobalAttractionRequestFacade
{
public:

  static gflnet2::ServerClient::RequestHandle AddDownloadRequest( NetApp::JoinFestaGlobalAttraction::RequestSequence::JoinFestaGlobalAttractionDownloadResponseListener* pJoinFestaGlobalAttractionDownloadResponseListener, NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork );
  static void CancelDownloadRequest( gflnet2::ServerClient::RequestHandle handle );

};



GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOABALATTRACTIONREQUESTFACADE_H_INCLUDE
