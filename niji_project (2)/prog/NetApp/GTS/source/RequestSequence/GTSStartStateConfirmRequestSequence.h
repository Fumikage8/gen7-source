// ============================================================================
/*
 * @file GTSStartStateConfirmRequestSequence.h
 * @brief GTSアプリ起動時のGTSの状態を確認するリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSSTARTSTATECONFIRMREQUESTSEQUENCE_H_INCLUDE )
#define GTSSTARTSTATECONFIRMREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSStartStateConfirmResponseListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSStartStateConfirmRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSStartStateConfirmRequestSequence );

public:

  GTSStartStateConfirmRequestSequence();
  virtual~GTSStartStateConfirmRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pStartStateConfirmResponseListener = NULL; }
  void SetListener( GTSStartStateConfirmResponseListener* pStartStateConfirmResponseListener ){ m_pStartStateConfirmResponseListener = pStartStateConfirmResponseListener; }

#ifdef GF_PLATFORM_CTR

public:
  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnDownloadMyPokemonSuccess( const nn::nex::GlobalTradeStationDownloadMyPokemonResult& downloadResult );
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode );

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_CHECK_SAVE_DATA = SEQUENCE_START,
    SEQUENCE_DOWNLOAD_MY_POKEMON,
    SEQUENCE_NOTIFICATION_POKEMON_NOT_UPLOAD,
    SEQUENCE_NOTIFICATION_POKEMON_UPLOAD,
    SEQUENCE_NOTIFICATION_POKEMON_TRADE,
    SEQUENCE_NOTIFICATION_POKEMON_DELETE,
    SEQUENCE_NOTIFICATION_ERROR,
  };

#endif // GF_PLATFORM_CTR

  GTSStartStateConfirmResponseListener*           m_pStartStateConfirmResponseListener;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSTARTSTATECONFIRMREQUESTSEQUENCE_H_INCLUDE
