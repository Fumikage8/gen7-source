// ============================================================================
/*
 * @file GTSDownloadStateConfirmResponseListener.h
 * @brief 引き取り時のGTSの状態を確認するレスポンスリスナーです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSDOWNLOADSTATECONFIRMRESPONSELISTENER_H_INCLUDE )
#define GTSDOWNLOADSTATECONFIRMRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


class GTSDownloadStateConfirmResponseListener
{
public:
  virtual void OnDownloadStateConfirmPokemonTrade() = 0;
  virtual void OnDownloadStateConfirmPokemonNotTrade() = 0;
  virtual void OnDownloadStateConfirmPokemonDelete() = 0;
  virtual void OnDownloadStateConfirmError() = 0;
};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSDOWNLOADSTATECONFIRMRESPONSELISTENER_H_INCLUDE
