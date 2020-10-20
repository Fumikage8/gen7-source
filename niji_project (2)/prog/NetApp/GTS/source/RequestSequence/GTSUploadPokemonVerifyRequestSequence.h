// ============================================================================
/*
 * @file GTSUploadPokemonVerifyRequestSequence.h
 * @brief アップロードするポケモンの不正チェックをするリクエストを投げるシーケンスです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSUPLOADPOKEMONVERIFYREQUESTSEQUENCE_H_INCLUDE )
#define GTSUPLOADPOKEMONVERIFYREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonVerifyResponseListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSUploadPokemonVerifyRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSUploadPokemonVerifyRequestSequence );

public:

  GTSUploadPokemonVerifyRequestSequence();
  virtual~GTSUploadPokemonVerifyRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pUploadPokemonVerifyResponseListener = NULL; }
  void SetListener( GTSUploadPokemonVerifyResponseListener* pUploadPokemonVerifyResponseListener ){ m_pUploadPokemonVerifyResponseListener = pUploadPokemonVerifyResponseListener; }

#ifdef GF_PLATFORM_CTR

public:

  // gflnet2::nex::NexDataStoreClientListener
  virtual void OnNexDataStoreUploadSuccess( const nn::nex::qUnsignedInt64 dataId );
  virtual void OnNexDataStoreChangeMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results );
  virtual void OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode );

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_UPLOAD_PERSONAL_DATA = SEQUENCE_START,
    SEQUENCE_UPLOAD_POKEMON_VERIFY,
    SEQUENCE_WANT_POKEMON_VERIFY,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
  };

  u64                                             m_MyDataStoreID;

#endif // GF_PLATFORM_CTR

  GTSUploadPokemonVerifyResponseListener*         m_pUploadPokemonVerifyResponseListener;

  pml::pokepara::CoreParam*                       m_pDummyPokemon;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADPOKEMONVERIFYREQUESTSEQUENCE_H_INCLUDE
