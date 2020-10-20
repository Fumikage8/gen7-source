// ============================================================================
/*
 * @file GTSUploadPokemonVerifyResponseListener.h
 * @brief アップロードするポケモンの不正チェックの結果を受け取るレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSUPLOADPOKEMONVERIFYRESPONSELISTENER_H_INCLUDE )
#define GTSUPLOADPOKEMONVERIFYRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSUploadPokemonVerifyResponseListener
{
public:
  virtual void OnUploadPokemonVerifySuccess() = 0;
  virtual void OnUploadPokemonVerifyError() = 0; 
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADPOKEMONVERIFYRESPONSELISTENER_H_INCLUDE
