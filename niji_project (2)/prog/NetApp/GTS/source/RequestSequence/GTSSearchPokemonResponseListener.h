// ============================================================================
/*
 * @file GTSSearchPokemonResponseListener.h
 * @brief 探したいポケモンを検索した結果を受け取るレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSSEARCHPOKEMONRESPONSELISTENER_H_INCLUDE )
#define GTSSEARCHPOKEMONRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSSearchPokemonResponseListener
{
public:
  virtual void OnSearchPokemonSuccess() = 0;
  virtual void OnSearchPokemonCancel() = 0;
  virtual void OnSearchPokemonError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSEARCHPOKEMONRESPONSELISTENER_H_INCLUDE
