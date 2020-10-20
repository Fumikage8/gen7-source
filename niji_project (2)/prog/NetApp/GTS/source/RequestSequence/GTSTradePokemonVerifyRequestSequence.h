// ============================================================================
/*
 * @file GTSTradePokemonVerifyRequestSequence.h
 * @brief 交換するポケモンの不正チェックをするリクエストを投げるシーケンスです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSTRADEPOKEMONVERIFYREQUESTSEQUENCE_H_INCLUDE )
#define GTSTRADEPOKEMONVERIFYREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonVerifyResponseListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSTradePokemonVerifyRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTradePokemonVerifyRequestSequence );

public:

  GTSTradePokemonVerifyRequestSequence();
  virtual~GTSTradePokemonVerifyRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pTradePokemonVerifyResponseListener = NULL; }
  void SetListener( GTSTradePokemonVerifyResponseListener* pTradePokemonVerifyResponseListener ){ m_pTradePokemonVerifyResponseListener = pTradePokemonVerifyResponseListener; }


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
    SEQUENCE_TRADE_POKEMON_VERIFY,
    SEQUENCE_NOTIFICATION_OK,
    SEQUENCE_NOTIFICATION_NG,
    SEQUENCE_NOTIFICATION_ERROR,
  };

  u64                                                 m_MyDataStoreID;

#endif // GF_PLATFORM_CTR

  GTSTradePokemonVerifyResponseListener*              m_pTradePokemonVerifyResponseListener;

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPOKEMONVERIFYREQUESTSEQUENCE_H_INCLUDE
