// ============================================================================
/*
 * @file GTSDownloadPokemonRequestSequence.h
 * @brief GTSサーバーからポケモンを引き取るリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSDOWNLOADPOKEMONREQUESTSEQUENCE_H_INCLUDE )
#define GTSDOWNLOADPOKEMONREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadPokemonResponseListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSDownloadPokemonRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSDownloadPokemonRequestSequence );

public:

  GTSDownloadPokemonRequestSequence();
  virtual~GTSDownloadPokemonRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pDownloadPokemonResponseListener = NULL; }
  void SetListener( GTSDownloadPokemonResponseListener* pDownloadPokemonResponseListener ){ m_pDownloadPokemonResponseListener = pDownloadPokemonResponseListener; }

#ifdef GF_PLATFORM_CTR

public:

  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnDeletePokemonSuccess( void );
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode );

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_DISCONNECT_DETECTED_SAVE_START = SEQUENCE_START,
    SEQUENCE_DISCONNECT_DETECTED_SAVE_WAIT,
    SEQUENCE_FIRST_SAVE_START,
    SEQUENCE_FIRST_SAVE_WAIT,
    SEQUENCE_DELETE_POKEMON,
    SEQUENCE_LAST_SAVE_START,
    SEQUENCE_LAST_SAVE_WAIT,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
  };

#endif // GF_PLATFORM_CTR

  GTSDownloadPokemonResponseListener*           m_pDownloadPokemonResponseListener;
  NetAppLib::Util::NetAppCommonSaveUtility      m_NetAppCommonSaveUtility;

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSDOWNLOADPOKEMONREQUESTSEQUENCE_H_INCLUDE
