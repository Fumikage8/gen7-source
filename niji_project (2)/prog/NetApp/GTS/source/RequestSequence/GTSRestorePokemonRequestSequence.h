// ============================================================================
/*
 * @file GTSRestorePokemonRequestSequence.h
 * @brief 預けていたポケモンが戻って来た時のリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSRESTOREPOKEMONREQUESTSEQUENCE_H_INCLUDE )
#define GTSRESTOREPOKEMONREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#include "NetApp/GTS/source/RequestSequence/GTSRestorePokemonResponseListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSRestorePokemonRequestSequence :
  public NetApp::GTS::RequestSequence::GTSRequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSRestorePokemonRequestSequence );

public:

  GTSRestorePokemonRequestSequence();
  virtual~GTSRestorePokemonRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
#ifdef GF_PLATFORM_WIN32
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
#endif // GF_PLATFORM_WIN32
  virtual void RemoveListener(){ m_pRestorePokemonResponseListener = NULL; }
  void SetListener( GTSRestorePokemonResponseListener* pRestorePokemonResponseListener ){ m_pRestorePokemonResponseListener = pRestorePokemonResponseListener; }

#ifdef GF_PLATFORM_CTR

public:

protected:

  virtual void updateSequence();

  enum Sequence
  {
    SEQUENCE_SAVE_START = SEQUENCE_START,
    SEQUENCE_SAVE_WAIT,
    SEQUENCE_NOTIFICATION_SUCCESS,
  };

#endif // GF_PLATFORM_CTR

  GTSRestorePokemonResponseListener*           m_pRestorePokemonResponseListener;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSRESTOREPOKEMONREQUESTSEQUENCE_H_INCLUDE
