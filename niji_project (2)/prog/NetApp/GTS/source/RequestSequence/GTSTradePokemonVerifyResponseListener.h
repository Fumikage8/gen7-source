// ============================================================================
/*
 * @file GTSTradePokemonVerifyResponseListener.h
 * @brief 交換するポケモンの不正チェックの結果を受け取るレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSTRADEPOKEMONVERIFYRESPONSELISTENER_H_INCLUDE )
#define GTSTRADEPOKEMONVERIFYRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSTradePokemonVerifyResponseListener
{
public:
  virtual void OnTradePokemonVerifyOK() = 0;
  virtual void OnTradePokemonVerifyNG() = 0;
  virtual void OnTradePokemonVerifyError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPOKEMONVERIFYRESPONSELISTENER_H_INCLUDE
