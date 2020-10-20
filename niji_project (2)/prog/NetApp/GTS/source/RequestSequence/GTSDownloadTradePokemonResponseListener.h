// ============================================================================
/*
 * @file GTSDownloadTradePokemonResponseListener.h
 * @brief ポケモンの交換が成立していた時の受け取りリクエストのレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSDOWNLOADTRADEPOKEMONRESPONSELISTENER_H_INCLUDE )
#define GTSDOWNLOADTRADEPOKEMONRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSDownloadTradePokemonResponseListener
{
public:
  virtual void OnDownloadTradePokemonSuccess() = 0;
  virtual void OnDownloadTradePokemonError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSDOWNLOADTRADEPOKEMONRESPONSELISTENER_H_INCLUDE
