// ============================================================================
/*
 * @file GTSDownloadTradePokemonRequestSequence.h
 * @brief ポケモンの交換が成立していた時の受け取りリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSDOWNLOADTRADEPOKEMONREQUESTSEQUENCE_H_INCLUDE )
#define GTSDOWNLOADTRADEPOKEMONREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSDownloadTradePokemonResponseListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSDownloadTradePokemonRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSDownloadTradePokemonRequestSequence );

public:

  GTSDownloadTradePokemonRequestSequence();
  virtual~GTSDownloadTradePokemonRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pDownloadTradePokemonResponseListener = NULL; }
  void SetListener( GTSDownloadTradePokemonResponseListener* pDownloadTradePokemonResponseListener ){ m_pDownloadTradePokemonResponseListener = pDownloadTradePokemonResponseListener; }

#ifdef GF_PLATFORM_CTR

public:

  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnDownloadOtherPokemonSuccess( const nn::nex::GlobalTradeStationTradePokemonResult& downloadResult );
  virtual void OnDeletePokemonSuccess( void );
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode );

  // gflnet2::nex::NexDataStoreClientListener
  virtual void OnNexDataStoreDownloadMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results, const nn::nex::qVector<nn::nex::DataStoreMetaInfo>& metas );
  virtual void OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode );

private:

  NetAppLib::JoinFesta::JoinFestaPersonalData* getJoinFestaPersonalData( u32 principalID );

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_DISCONNECT_DETECTED_SAVE_START = SEQUENCE_START,
    SEQUENCE_DISCONNECT_DETECTED_SAVE_WAIT,
    SEQUENCE_DOWNLOAD_OTHER_POKEMON,
    SEQUENCE_DOWNLOAD_PERSONAL_DATA,
    SEQUENCE_FIRST_SAVE_START,
    SEQUENCE_FIRST_SAVE_WAIT,
    SEQUENCE_DELETE_POKEMON,
    SEQUENCE_LAST_SAVE_START,
    SEQUENCE_LAST_SAVE_WAIT,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
  };

  u64                                               m_TradePartnerDataStoreID;
  u32                                               m_TradePartnerPrincipalID;

#endif // GF_PLATFORM_CTR

  GTSDownloadTradePokemonResponseListener*          m_pDownloadTradePokemonResponseListener;
  NetAppLib::Util::NetAppCommonSaveUtility          m_NetAppCommonSaveUtility;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSDOWNLOADTRADEPOKEMONREQUESTSEQUENCE_H_INCLUDE
