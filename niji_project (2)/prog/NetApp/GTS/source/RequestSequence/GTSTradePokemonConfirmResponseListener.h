// ============================================================================
/*
 * @file GTSTradePokemonConfirmResponseListener.h
 * @brief 交換相手のポケモンが存在するか確認した結果を受け取るレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSTRADEPOKEMONCONFIRMRESPONSELISTENER_H_INCLUDE )
#define GTSTRADEPOKEMONCONFIRMRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSTradePokemonConfirmResponseListener
{
public:
  virtual void OnTradePokemonAlreadyTradePokemon() = 0;
  virtual void OnTradePokemonConfirmSuccess() = 0;
  virtual void OnTradePokemonConfirmError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPOKEMONCONFIRMRESPONSELISTENER_H_INCLUDE
