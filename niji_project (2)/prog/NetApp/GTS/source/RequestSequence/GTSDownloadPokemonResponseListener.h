// ============================================================================
/*
 * @file GTSDownloadPokemonResponseListener.h
 * @brief GTSサーバーからポケモンを引き取るリクエストのレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSDOWNLOADPOKEMONRESPONSELISTENER_H_INCLUDE )
#define GTSDOWNLOADPOKEMONRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSDownloadPokemonResponseListener
{
public:
  virtual void OnDownloadPokemonSuccess() = 0;
  virtual void OnDownloadPokemonError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSDOWNLOADPOKEMONRESPONSELISTENER_H_INCLUDE
