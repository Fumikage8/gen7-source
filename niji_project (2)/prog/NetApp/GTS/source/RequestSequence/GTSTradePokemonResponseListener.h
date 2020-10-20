// ============================================================================
/*
 * @file GTSTradePokemonResponseListener.h
 * @brief ポケモンの交換リクエストのレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSTRADEPOKEMONRESPONSELISTENER_H_INCLUDE )
#define GTSTRADEPOKEMONRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSTradePokemonResponseListener
{
public:
  virtual void OnTradePokemonSuccess() = 0;
  virtual void OnTradePokemonError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPOKEMONRESPONSELISTENER_H_INCLUDE
