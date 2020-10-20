// ============================================================================
/*
 * @file GTSUploadPokemonRequestSequence.h
 * @brief GTSサーバーにポケモンをアップロードするリクエストを投げるシーケンスです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSUPLOADPOKEMONREQUESTSEQUENCE_H_INCLUDE )
#define GTSUPLOADPOKEMONREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonResponseListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSUploadPokemonRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSUploadPokemonRequestSequence );

public:

  GTSUploadPokemonRequestSequence();
  virtual~GTSUploadPokemonRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pUploadPokemonResponseListener = NULL; }
  void SetListener( GTSUploadPokemonResponseListener* pUploadPokemonResponseListener ){ m_pUploadPokemonResponseListener = pUploadPokemonResponseListener; }

#ifdef GF_PLATFORM_CTR

public:

  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnPrepareUploadPokemonSuccess( const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey );
  virtual void OnUploadPokemonSuccess( void );
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode );

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_DISCONNECT_DETECTED_SAVE_START = SEQUENCE_START,
    SEQUENCE_DISCONNECT_DETECTED_SAVE_WAIT,
    SEQUENCE_PREPARE_UPLOAD,
    SEQUENCE_FIRST_SAVE_START,
    SEQUENCE_FIRST_SAVE_WAIT,
    SEQUENCE_UPLOAD,
    SEQUENCE_LAST_SAVE_START,
    SEQUENCE_LAST_SAVE_WAIT,

    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
    SEQUENCE_FORCE_CANCEL_WAIT
  };

  NetApp::GTS::GtsSerializeRecordKey          m_PrepareUploadKey;
  NetApp::GTS::GtsSerializePokemonParam       m_UploadPokemonParam;

#endif // GF_PLATFORM_CTR

  GTSUploadPokemonResponseListener*           m_pUploadPokemonResponseListener;
  NetAppLib::Util::NetAppCommonSaveUtility    m_NetAppCommonSaveUtility;
  bool                                        m_bIsUploadPokemonError;

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADPOKEMONREQUESTSEQUENCE_H_INCLUDE
