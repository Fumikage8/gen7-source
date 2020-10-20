// ============================================================================
/*
 * @file DisconnectionResponseListener.h
 * @brief インターネット切断の結果を受け取るレスポンスリスナーです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( DISCONNECTION_RESPONSE_LISTENER_H_INCLUDE )
#define DISCONNECTION_RESPONSE_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(RequestSequence)


class DisconnectionResponseListener
{
public:
  virtual void OnDisconnectionSuccess() = 0;
  virtual void OnDisconnectionError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // DISCONNECTION_RESPONSE_LISTENER_H_INCLUDE
