// ============================================================================
/*
 * @file MiracleTradeMatchingRequestSequence.h
 * @brief マッチングリクエストを投げるシーケンスです。
 * @date 2015.05.26
 */
// ============================================================================
#if !defined( MIRACLETRADEMATCHINGREQUESTSEQUENCE_H_INCLUDE )
#define MIRACLETRADEMATCHINGREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeMatchingResponseListener.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "NetApp/MiracleTrade/source/MiracleTradeDefine.h"
#include "NetStatic/NetLib/include/P2P/P2pPacketTransfer.h"
#include "NetStatic/NetLib/include/P2P/P2pSyncSave.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketDefine.h"
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPokemonValidationUtility.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(RequestSequence)

class MiracleTradeMatchingRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase,
  public NetLib::P2P::P2pPacketTransferListener
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeMatchingRequestSequence );

private:

  static const u32 DEFAULT_NOTIFICATION_WAIT_FRAME = NetApp::MiracleTrade::MIRACLETRADE_DEFAULT_NOTIFICATION_WAIT_FRAME;

public:

  MiracleTradeMatchingRequestSequence();
  virtual~MiracleTradeMatchingRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ){ m_pMiracleTradeWork = pMiracleTradeWork; }
  virtual void RemoveListener(){ m_pMatchingResponseListener = NULL; }
  void SetListener( MiracleTradeMatchingResponseListener* pMatchingResponseListener ){ m_pMatchingResponseListener = pMatchingResponseListener; }

  // P2pPacketTransferListener
  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer );

private:

  bool isNotification(){ return ( m_FrameCount >= DEFAULT_NOTIFICATION_WAIT_FRAME ); }

private:

  enum Sequence
  {
    SEQUENCE_INITIALIZE,
    
    SEQUENCE_CONNECT_START,
    SEQUENCE_CONNECT_WAIT,

    SEQUENCE_CREATE_P2P_PACKET_TRANSFER,

    SEQUENCE_SEND_TRADE_INFO,
    SEQUENCE_RECV_TRADE_INFO,

    SEQUENCE_VERIFY,

    SEQUENCE_NET_GAME_INITIALIZE,
    SEQUENCE_NET_GAME_INITIALIZE_WAIT,
    SEQUENCE_SAVE_START,
    SEQUENCE_SAVE_WAIT,

    SEQUENCE_DELETE_P2P_PACKET_TRANSFER,

    SEQUENCE_DISCONNECT_START,
    SEQUENCE_DISCONNECT_WAIT,

    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_VERIFY_NG,
    SEQUENCE_NOTIFICATION_ERROR,
    
    SEQUENCE_TERMINATE,
  };

  enum CancelSequence
  {
    CANCEL_SEQUENCE_NET_GAME_DELETE,
    CANCEL_SEQUENCE_DISCONNECT_START,
    CANCEL_SEQUENCE_DISCONNECT_WAIT,
    CANCEL_SEQUENCE_TERMINATE,
  };

  enum CommandID
  {
    COMMAND_ID_SEND_TRADE_INFO
  };

  static const u32 SIGNATRUE_SIZE              = NetApp::MiracleTrade::SIGNATRUE_SIZE;
  static const u32 SIGNATRUE_SIZE_SIZE         = sizeof( u32 );
  static const u32 PERSONAL_DATA_SIZE          = sizeof( NetAppLib::JoinFesta::JoinFestaPacketData );
  static const u32 PHRASE_MESSAGE_ID_SIZE      = sizeof( u32 ) * 2;

  static const u32 TRADE_INFO_SIZE =
    pml::pokepara::CoreParam::DATASIZE +
    Savedata::MyStatus::SERIALIZE_DATA_SIZE +
    SIGNATRUE_SIZE +
    SIGNATRUE_SIZE_SIZE +
    PERSONAL_DATA_SIZE +
    PHRASE_MESSAGE_ID_SIZE;

  typedef union
  {
    struct
    {
      u8  pokemonCoreParam[ pml::pokepara::CoreParam::DATASIZE ];
      u8  myStatus[ Savedata::MyStatus::SERIALIZE_DATA_SIZE ];
      u8  signature[ SIGNATRUE_SIZE ];
      u8  signatureSize[ SIGNATRUE_SIZE_SIZE ];
      u8  personalData[ PERSONAL_DATA_SIZE ];
      u8  phraseMessageID[ PHRASE_MESSAGE_ID_SIZE ];
    };
    struct
    {
      u8  buffer[ TRADE_INFO_SIZE ];
    };
  } TradeInfo;


  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*    m_pMiracleTradeWork;
  MiracleTradeMatchingResponseListener*                         m_pMatchingResponseListener;
  u32                                                           m_FrameCount;
  TradeInfo                                                     m_SendData;
  NetAppLib::Util::NetAppTimeout                                m_CountDownTimer;
  NetLib::P2P::P2pPacketTransfer*                               m_pP2pPacketTransfer;
  NetLib::P2P::P2pSyncSave*                                     m_pP2pSyncSave;
#if defined( GF_PLATFORM_CTR )
  gflnet2::p2p::NetGame*                                        m_pNetGame;
  NetAppLib::Util::NetAppPokemonValidationUtility*              m_pNetAppPokemonValidationUtility;
#endif // defiend( GF_PLATFORM_CTR )
  u32                                                           m_CancelSequenceNo;
  s32                                                           m_CounterOffCount;
  bool                                                          m_bReceivedTradeInfo;
  bool                                                          m_bIsTimeOut;
  bool                                                          m_bCountDownStartFlag;
  bool                                                          m_bVerifyResult;

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEMATCHINGREQUESTSEQUENCE_H_INCLUDE
