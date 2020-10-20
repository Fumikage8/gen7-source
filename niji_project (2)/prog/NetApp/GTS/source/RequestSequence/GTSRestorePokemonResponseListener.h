// ============================================================================
/*
 * @file GTSRestorePokemonResponseListener.h
 * @brief 預けていたポケモンが戻って来た時のレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSRESTOREPOKEMONRESPONSELISTENER_H_INCLUDE )
#define GTSRESTOREPOKEMONRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSRestorePokemonResponseListener
{
public:
  virtual void OnRestorePokemonSuccess() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSRESTOREPOKEMONRESPONSELISTENER_H_INCLUDE
