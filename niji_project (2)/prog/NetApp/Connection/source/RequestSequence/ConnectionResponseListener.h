// ============================================================================
/*
 * @file ConnectionResponseListener.h
 * @brief インターネット接続の結果を受け取るレスポンスリスナーです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( CONNECTION_RESPONSE_LISTENER_H_INCLUDE )
#define CONNECTION_RESPONSE_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(RequestSequence)


class ConnectionResponseListener
{
public:
  virtual void OnConnectionSuccess() = 0;
  virtual void OnConnectionError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_RESPONSE_LISTENER_H_INCLUDE
