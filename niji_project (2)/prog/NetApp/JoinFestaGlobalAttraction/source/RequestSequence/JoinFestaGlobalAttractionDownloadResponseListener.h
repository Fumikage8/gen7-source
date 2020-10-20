// ============================================================================
/*
 * @file JoinFestaGlobalAttractionDownloadResponseListener.h
 * @brief レギュレーションデータを探す通信リクエストのレスポンスリスナーです。
 * @date 2016.02.29
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONDOWNLOADRESPONSELISTENER_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONDOWNLOADRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(RequestSequence)


class JoinFestaGlobalAttractionDownloadResponseListener
{
public:
  virtual void OnDownloadSuccess() = 0;
  virtual void OnDownloadError() = 0;
  virtual void OnDownloadCancel() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONDOWNLOADRESPONSELISTENER_H_INCLUDE
