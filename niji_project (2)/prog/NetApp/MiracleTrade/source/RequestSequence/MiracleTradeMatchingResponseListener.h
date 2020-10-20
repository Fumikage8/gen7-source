// ============================================================================
/*
 * @file MiracleTradeMatchingResponseListener.h
 * @brief マッチングの結果を受け取るレスポンスリスナーです。
 * @date 2015.05.26
 */
// ============================================================================
#if !defined( MIRACLETRADEMATCHINGRESPONSELISTENER_H_INCLUDE )
#define MIRACLETRADEMATCHINGRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(RequestSequence)


class MiracleTradeMatchingResponseListener
{
public:
  virtual void OnMiracleTradeMatchingSuccess() = 0;
  virtual void OnMiracleTradeMatchingError() = 0;
  virtual void OnMiracleTradeMatchingTimeOut() = 0;
  virtual void OnMiracleTradeMatchingUpdateTime( s32 nTime ) = 0;
  virtual void OnMiracleTradeMatchingTimeCounterOff() = 0;
  virtual void OnMiracleTradeMatchingVerifyNG() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEMATCHINGRESPONSELISTENER_H_INCLUDE
