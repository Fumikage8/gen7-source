// ============================================================================
/*
 * @file GTSTradePokemonConfirmRequestSequence.h
 * @brief 交換相手のポケモンが存在するか確認するリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSTRADEPOKEMONCONFIRMREQUESTSEQUENCE_H_INCLUDE )
#define GTSTRADEPOKEMONCONFIRMREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonConfirmResponseListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSTradePokemonConfirmRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTradePokemonConfirmRequestSequence );

public:

  GTSTradePokemonConfirmRequestSequence();
  virtual~GTSTradePokemonConfirmRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pTradePokemonConfirmResponseListener = NULL; }
  void SetListener( GTSTradePokemonConfirmResponseListener* pTradePokemonConfirmResponseListener ){ m_pTradePokemonConfirmResponseListener = pTradePokemonConfirmResponseListener; }


#ifdef GF_PLATFORM_CTR

public:

  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnPrepareTradePokemonSuccess( const nn::nex::GlobalTradeStationDownloadPokemonResult& preTradeResult, const NetApp::GTS::GtsSerializeRecordKey& prepareTradeKey );
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode );

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_PREPARE_TRADE_POKEMON = SEQUENCE_START,
    SEQUENCE_NOTIFICATION_ALREADY_TRADE_POKEMON,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
  };


#endif // GF_PLATFORM_CTR

  GTSTradePokemonConfirmResponseListener*            m_pTradePokemonConfirmResponseListener;

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPOKEMONCONFIRMREQUESTSEQUENCE_H_INCLUDE
