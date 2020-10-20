// ============================================================================
/*
 * @file RegulationDLRequestFacade.h
 * @brief レギュレーション選択アプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.06.30
 */
// ============================================================================
#if !defined( REGULATIONDLREQUESTFACADE_H_INCLUDE )
#define REGULATIONDLREQUESTFACADE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(RequestSequence)


class RegulationDLSearchDataResponseListener;


class RegulationDLRequestFacade
{
public:

  static gflnet2::ServerClient::RequestHandle AddSearchDataRequest( NetApp::RegulationSelect::RequestSequence::RegulationDLSearchDataResponseListener* pRegulationDLSearchDataResponseListener, NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork );
  static void CancelSearchDataRequest( gflnet2::ServerClient::RequestHandle handle );

};



GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDLREQUESTFACADE_H_INCLUDE
