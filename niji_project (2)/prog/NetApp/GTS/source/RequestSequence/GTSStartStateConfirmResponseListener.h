// ============================================================================
/*
 * @file GTSStartStateConfirmResponseListener.h
 * @brief GTSアプリ起動時のGTSの状態を確認するレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSSTARTSTATECONFIRMRESPONSELISTENER_H_INCLUDE )
#define GTSSTARTSTATECONFIRMRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSStartStateConfirmResponseListener
{
public:
  virtual void OnStartStateConfirmPokemonNotUpload() = 0;
  virtual void OnStartStateConfirmPokemonUpload() = 0;
  virtual void OnStartStateConfrimPokemonTrade() = 0;
  virtual void OnStartStateConfirmPokemonDelete() = 0;
  virtual void OnStartStateConfirmError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSTARTSTATECONFIRMRESPONSELISTENER_H_INCLUDE
