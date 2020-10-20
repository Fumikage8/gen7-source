// ============================================================================
/*
 * @file GTSSearchPokemonRequestSequence.h
 * @brief 探したいポケモンを検索するリクエストを投げるシーケンスです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSSEARCHPOKEMONREQUESTSEQUENCE_H_INCLUDE )
#define GTSSEARCHPOKEMONREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSSearchPokemonResponseListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSSearchPokemonRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSSearchPokemonRequestSequence );

public:

  GTSSearchPokemonRequestSequence();
  virtual~GTSSearchPokemonRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pSearchPokemonResponseListener = NULL; }
  void SetListener( GTSSearchPokemonResponseListener* pSearchPokemonResponseListener ){ m_pSearchPokemonResponseListener = pSearchPokemonResponseListener; }

#ifdef GF_PLATFORM_CTR

public:

  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnSearchPokemonSuccess( const nn::nex::GlobalTradeStationSearchPokemonResult& searchResult );
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode );

private:
  virtual void notificationCancel();

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_SEARCH_POKEMON = SEQUENCE_START,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
  };

#endif // GF_PLATFORM_CTR

  GTSSearchPokemonResponseListener*           m_pSearchPokemonResponseListener;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // SEARCHPOKEMONREQUESTSEQUENCE_H_INCLUDE
