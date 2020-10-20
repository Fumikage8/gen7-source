// ============================================================================
/*
 * @file GTSRequestSequenceBase.h
 * @brief GTSの通信リクエストのベースクラスです。
 * @date 2015.04.27
 */
// ============================================================================
#if !defined( GTSREQUESTSEQUENCEBASE_H_INCLUDE )
#define GTSREQUESTSEQUENCEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClientListener.h>
#include "NetApp/GTS/source/NexGlobalTradeStationClient.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPokemonValidationUtility.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)

class GTSRequestSequenceBase :
#ifdef GF_PLATFORM_CTR
  public NetApp::GTS::NexGlobalTradeStationClientListener,
  public gflnet2::nex::NexDataStoreClientListener,
#endif // GF_PLATFORM_CTR
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSRequestSequenceBase );

private:

  static const u32 DEFAULT_NOTIFICATION_WAIT_FRAME = NetApp::GTS::GTS_DEFAULT_NOTIFICATION_WAIT_FRAME;

public:

  GTSRequestSequenceBase( u32 notificationWaitFrame = DEFAULT_NOTIFICATION_WAIT_FRAME );
  virtual~GTSRequestSequenceBase();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ){ m_pGTSWork = pGTSWork; }


#ifdef GF_PLATFORM_CTR

public:

  // NetApp::GTS::NexGlobalTradeStationClientListener
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode ){}

  // gflnet2::nex::NexDataStoreClientListener
  virtual void OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode ){}

private:

  void initialize();
  void terminate();
  
  virtual void notificationCancel(){}

protected:

  virtual void updateSequence() = 0;
  bool isNotification(){ return ( m_FrameCount >= m_NotificationWaitFrame ); }
  void setRandomWaitSequence( u32 nextSequenceNo );

  void setFirstSaveStartFlag( bool bFlag ){ m_bFirstSaveStartFlag = bFlag; }
  void setFirstSaveCompleteFlag( bool bFlag ){ m_bFirstSaveCompleteFlag = bFlag; }
  void setLastSaveStartFlag( bool bFlag ){ m_bLastSaveStartFlag = bFlag; }
  void setLastSaveCompleteFlag( bool bFlag ){ m_bLastSaveCompleteFlag = bFlag; }

  enum Sequence
  {
    SEQUENCE_INITIALIZE,
    SEQUENCE_TERMINATE,
    SEQUENCE_RESPONSE_WAIT,
    SEQUENCE_RANDOM_WAIT,
    SEQUENCE_START,
  };

  enum CancelSequence
  {
    CANCEL_SEQUENCE_GTS_CLIENT_CANCEL_WAIT,
    CANCEL_SEQUENCE_DATA_STORE_CLIENT_CANCEL_WAIT,

    CANCEL_SEQUENCE_POKEMON_VERIFY_WAIT,
    CANCEL_SEQUENCE_FIRST_SAVE_WAIT,
    CANCEL_SEQUENCE_FIRST_SAVE_CANCEL_WAIT,
    CANCEL_SEQUENCE_LAST_SAVE_WAIT,

    CANCEL_SEQUENCE_TERMINATE,
  };

  NetApp::GTS::NexGlobalTradeStationClient*           m_pNexGTSClient;
  NetAppLib::Util::NetAppPokemonValidationUtility*    m_pNetAppPokemonValidationUtility;

#endif // GF_PLATFORM_CTR

  NetApp::GTS::ApplicationSystem::GTSWork*            m_pGTSWork;
  u32                                                 m_FrameCount;
  u32                                                 m_NotificationWaitFrame;
  u32                                                 m_RandomFrameCount;
  u32                                                 m_RandomWaitFrame;
  u32                                                 m_RandomNextSequenceNo;
  u32                                                 m_CancelSequenceNo;
  bool                                                m_bFirstSaveStartFlag;
  bool                                                m_bFirstSaveCompleteFlag;
  bool                                                m_bLastSaveStartFlag;
  bool                                                m_bLastSaveCompleteFlag;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSREQUESTSEQUENCEBASE_H_INCLUDE
