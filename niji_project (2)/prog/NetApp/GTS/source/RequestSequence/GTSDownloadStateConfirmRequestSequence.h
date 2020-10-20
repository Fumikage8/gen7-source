// ============================================================================
/*
 * @file GTSDownloadStateConfirmRequestSequence.h
 * @brief 引き取り時のGTSの状態を確認するリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSDOWNLOADSTATECONFIRMREQUESTSEQUENCE_H_INCLUDE )
#define GTSDOWNLOADSTATECONFIRMREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadStateConfirmResponseListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSDownloadStateConfirmRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSDownloadStateConfirmRequestSequence );

public:

  GTSDownloadStateConfirmRequestSequence();
  virtual~GTSDownloadStateConfirmRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pDownloadStateConfirmResponseListener = NULL; }
  void SetListener( GTSDownloadStateConfirmResponseListener* pDownloadStateConfirmResponseListener ){ m_pDownloadStateConfirmResponseListener = pDownloadStateConfirmResponseListener; }

#ifdef GF_PLATFORM_CTR

public:
  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnDownloadMyPokemonSuccess( const nn::nex::GlobalTradeStationDownloadMyPokemonResult& downloadResult );
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode );

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_DOWNLOAD_MY_POKEMON = SEQUENCE_START,
    SEQUENCE_NOTIFICATION_POKEMON_TRADE,
    SEQUENCE_NOTIFICATION_POKEMON_NOT_TRADE,
    SEQUENCE_NOTIFICATION_POKEMON_DELETE,
    SEQUENCE_NOTIFICATION_ERROR,
  };

#endif // GF_PLATFORM_CTR

  GTSDownloadStateConfirmResponseListener*           m_pDownloadStateConfirmResponseListener;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSDOWNLOADSTATECONFIRMREQUESTSEQUENCE_H_INCLUDE
