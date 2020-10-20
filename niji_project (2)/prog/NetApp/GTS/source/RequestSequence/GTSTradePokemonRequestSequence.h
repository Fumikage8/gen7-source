// ============================================================================
/*
 * @file GTSTradePokemonRequestSequence.h
 * @brief ポケモンを交換するリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSTRADEPOKEMONREQUESTSEQUENCE_H_INCLUDE )
#define GTSTRADEPOKEMONREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonResponseListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSTradePokemonRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTradePokemonRequestSequence );

public:

  GTSTradePokemonRequestSequence();
  virtual~GTSTradePokemonRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pTradePokemonResponseListener = NULL; }
  void SetListener( GTSTradePokemonResponseListener* pTradePokemonResponseListener ){ m_pTradePokemonResponseListener = pTradePokemonResponseListener; }


#ifdef GF_PLATFORM_CTR

public:

  // gflnet2::nex::NexDataStoreClientListener
  virtual void OnNexDataStoreDownloadMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results, const nn::nex::qVector<nn::nex::DataStoreMetaInfo>& metas );
  virtual void OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode );

  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnTradePokemonSuccess( const nn::nex::GlobalTradeStationTradePokemonResult& tradeResult );
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode );

private:

  NetAppLib::JoinFesta::JoinFestaPersonalData* getJoinFestaPersonalData( u32 principalID );

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_DISCONNECT_DETECTED_SAVE_START = SEQUENCE_START,
    SEQUENCE_DISCONNECT_DETECTED_SAVE_WAIT,
    SEQUENCE_DOWNLOAD_PERSONAL_DATA,
    SEQUENCE_FIRST_SAVE_START,
    SEQUENCE_FIRST_SAVE_WAIT,
    SEQUENCE_TRADE_POKEMON,
    SEQUENCE_LAST_SAVE_START,
    SEQUENCE_LAST_SAVE_WAIT,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
  };


  NetApp::GTS::GtsSerializePokemonParam       m_UploadPokemonParam;

#endif // GF_PLATFORM_CTR

  GTSTradePokemonResponseListener*            m_pTradePokemonResponseListener;
  NetAppLib::Util::NetAppCommonSaveUtility    m_NetAppCommonSaveUtility;

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPOKEMONREQUESTSEQUENCE_H_INCLUDE
