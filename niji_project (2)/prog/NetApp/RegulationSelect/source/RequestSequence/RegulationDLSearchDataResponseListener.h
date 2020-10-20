// ============================================================================
/*
 * @file RegulationDLSearchDataResponseListener.h
 * @brief レギュレーションデータを探す通信リクエストのレスポンスリスナーです。
 * @date 2015.06.30
 */
// ============================================================================
#if !defined( REGULATIONDLSEARCHDATARESPONSELISTENER_H_INCLUDE )
#define REGULATIONDLSEARCHDATARESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(RequestSequence)


class RegulationDLSearchDataResponseListener
{
public:
  virtual void OnSearchDataSuccess() = 0;
  virtual void OnSearchDataError() = 0;
  virtual void OnSearchDataError2() = 0;
  virtual void OnSearchDataCancel() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDLSEARCHDATARESPONSELISTENER_H_INCLUDE
