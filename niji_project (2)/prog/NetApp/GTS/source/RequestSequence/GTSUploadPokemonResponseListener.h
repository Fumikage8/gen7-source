// ============================================================================
/*
 * @file GTSUploadPokemonResponseListener.h
 * @brief GTSサーバーにポケモンをアップーロードした結果を受け取るレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSUPLOADPOKEMONRESPONSELISTENER_H_INCLUDE )
#define GTSUPLOADPOKEMONRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSUploadPokemonResponseListener
{
public:
  virtual void OnUploadPokemonSuccess() = 0;
  virtual void OnUploadPokemonError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADPOKEMONRESPONSELISTENER_H_INCLUDE
