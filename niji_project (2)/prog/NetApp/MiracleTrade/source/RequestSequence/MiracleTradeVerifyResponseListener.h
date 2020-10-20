// ============================================================================
/*
 * @file MiracleTradeVerifyResponseListener.h
 * @brief 交換するポケモンの不正チェックの結果を受け取るレスポンスリスナーです。
 * @date 2015.05.26
 */
// ============================================================================
#if !defined( MIRACLETRADEVERIFYRESPONSELISTENER_H_INCLUDE )
#define MIRACLETRADEVERIFYRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(RequestSequence)


class MiracleTradeVerifyResponseListener
{
public:
  virtual void OnMiracleTradeVerifyOK() = 0;
  virtual void OnMiracleTradeVerifyNG() = 0;
  virtual void OnMiracleTradeVerifyError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEVERIFYRESPONSELISTENER_H_INCLUDE
