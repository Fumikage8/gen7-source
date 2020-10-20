// ============================================================================
/*
 * @file MiracleTradeMatchingRequestSequence.cpp
 * @brief マッチングリクエストを投げるシーケンスです。
 * @date 2015.05.26
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeMatchingRequestSequence.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonTradeUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>

#include <pml/include/item/item.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(RequestSequence)


MiracleTradeMatchingRequestSequence::MiracleTradeMatchingRequestSequence() :
  m_pMiracleTradeWork( NULL ),
  m_pMatchingResponseListener( NULL ),
  m_FrameCount( 0 ),
  m_SendData(),
  m_CountDownTimer(),
  m_pP2pPacketTransfer( NULL ),
  m_pP2pSyncSave( NULL ),
#if defined( GF_PLATFORM_CTR )
  m_pNetGame( NULL ),
  m_pNetAppPokemonValidationUtility( NULL ),
#endif // defiend( GF_PLATFORM_CTR )
  m_CancelSequenceNo( 0 ),
  m_CounterOffCount( 0 ),
  m_bReceivedTradeInfo( false ),
  m_bIsTimeOut( false ),
  m_bCountDownStartFlag( false ),
  m_bVerifyResult( false )
{
}


MiracleTradeMatchingRequestSequence::~MiracleTradeMatchingRequestSequence()
{
}

#if defined( GF_PLATFORM_CTR )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE MiracleTradeMatchingRequestSequence::Update()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  
  if( m_pP2pPacketTransfer )
  {
    m_pP2pPacketTransfer->Update();
  }

  bool bIsTimeOut = false;

  if( m_bCountDownStartFlag )
  {
    u32 unElapsedTime = 0;
    bIsTimeOut = m_CountDownTimer.IsTimeOut( &unElapsedTime );

    s32 nElapsedTime = static_cast<s32>( unElapsedTime );
    s32 nTimeOut = static_cast<s32>( MIRACLETRADE_TIME_OUT );
    s32 nTime = ( nTimeOut - ( nElapsedTime / 1000 ) );

    if( m_pMatchingResponseListener )
    {
      if( m_CounterOffCount > 0 )
      {
        ++m_CounterOffCount;
      }

      if( nTime >= 0 )
      {
        if( m_CounterOffCount == 0 )
        {
          m_pMatchingResponseListener->OnMiracleTradeMatchingUpdateTime( nTime );

          if( nTime == 0 )
          {
            ++m_CounterOffCount;
          }
        }
      }

      if( m_CounterOffCount >= 30 )
      {
        m_pMatchingResponseListener->OnMiracleTradeMatchingTimeCounterOff();
      }
    }
  }

  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // 
    case SEQUENCE_INITIALIZE:
    {
      pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_RANDOM_TRADE );

      SetSequenceNo( SEQUENCE_CONNECT_START );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_CONNECT_START:
    {
      gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
      gflnet2::nex::NexMatchMakingRule rule1( 0, 0, 0, 0 );
      list.push_back( &rule1 );

      bool ret = pP2pConnectionManager->ConnectStartWifi( gflnet2::p2p::AUTO, &list );
      if( ret )
      {
        m_CountDownTimer.TimeOutStart( MIRACLETRADE_TIME_OUT * 1000 );
        m_bCountDownStartFlag = true;

        SetSequenceNo( SEQUENCE_CONNECT_WAIT );
      }
      else
      {
        SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_CONNECT_WAIT:
    {
      if( pP2pConnectionManager->IsConnectSuccess() )
      {
        SetSequenceNo( SEQUENCE_CREATE_P2P_PACKET_TRANSFER );
      }
      else
      if( bIsTimeOut )
      {
        GFL_PRINT( "MiracleTradeMatchingRequestSequence : Time out cancel request.\n" );
        Cancel();
        m_bIsTimeOut = bIsTimeOut;
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_CREATE_P2P_PACKET_TRANSFER:
    {
      gfl2::heap::HeapBase* pDeviceHeap = m_pMiracleTradeWork->GetDeviceHeap()->GetLowerHandle();

      m_pP2pPacketTransfer = GFL_NEW( pDeviceHeap ) NetLib::P2P::P2pPacketTransfer( pDeviceHeap, this );

      SetSequenceNo( SEQUENCE_SEND_TRADE_INFO );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_SEND_TRADE_INFO:
    {
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );
      NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData = pJoinFestaPersonalDataManager->GetMyData();
      NetAppLib::JoinFesta::JoinFestaPacketData joinFestaPacketData;
      pJoinFestaPersonalData->OutputPacketData( &joinFestaPacketData );

      // pokemon core param
      pml::pokepara::CoreParam& selectPokemonCoreParam = m_pMiracleTradeWork->GetSelectPokemonCoreParam();
      selectPokemonCoreParam.Serialize_Core( m_SendData.pokemonCoreParam );

      // my status
      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
      pMyStatus->Serialize( m_SendData.myStatus );
  
      // signature
      gfl2::std::MemCopy( m_pMiracleTradeWork->GetSignature(), m_SendData.signature, SIGNATRUE_SIZE );

      // signature size
      u32 signatureSize = m_pMiracleTradeWork->GetSignatureSize();
      gfl2::std::MemCopy( &signatureSize, m_SendData.signatureSize, SIGNATRUE_SIZE_SIZE );

      // personal data
      gfl2::std::MemCopy( &joinFestaPacketData, m_SendData.personalData, sizeof( NetAppLib::JoinFesta::JoinFestaPacketData ) );

      // message id
      Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

      u32 hello = static_cast<u32>( pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_HELLO ) );
      u32 emotion = static_cast<u32>( pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_EMOTION ) );

      gfl2::std::MemCopy( &hello, &m_SendData.phraseMessageID[0], sizeof( u32 ) );
      gfl2::std::MemCopy( &emotion, &m_SendData.phraseMessageID[ sizeof( u32 ) ], sizeof( u32 ) );

      if( m_pP2pPacketTransfer )
      {
        m_pP2pPacketTransfer->SendDataCommand( &m_SendData, TRADE_INFO_SIZE, COMMAND_ID_SEND_TRADE_INFO );
      }

      SetSequenceNo( SEQUENCE_RECV_TRADE_INFO );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_RECV_TRADE_INFO:
    {
      if( m_bReceivedTradeInfo )
      {
        SetSequenceNo( SEQUENCE_VERIFY );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_VERIFY:
    {
      if( m_pNetAppPokemonValidationUtility == NULL )
      {
        gfl2::heap::HeapBase* pHeap = m_pMiracleTradeWork->GetDeviceHeap();

        m_pNetAppPokemonValidationUtility = GFL_NEW( pHeap ) NetAppLib::Util::NetAppPokemonValidationUtility( pHeap );
      }

      u8* pSignature = m_pMiracleTradeWork->GetTradePersonSignature();
      u32 signatureSize = m_pMiracleTradeWork->GetTradePersonSignatureSize();
      pml::pokepara::CoreParam& receivedPokemonCoreParam = m_pMiracleTradeWork->GetReceivedPokemonCoreParam();

      if( m_pNetAppPokemonValidationUtility->VerifySignature( m_bVerifyResult, NetLib::Validation::PokemonValidation::E_GAME_MODE_TRADE_M, pSignature, signatureSize, &receivedPokemonCoreParam ) )
      {
        if( m_bVerifyResult )
        {
          SetSequenceNo( SEQUENCE_NET_GAME_INITIALIZE );
        }
        else
        {
          SetSequenceNo( SEQUENCE_DELETE_P2P_PACKET_TRANSFER );
        }
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NET_GAME_INITIALIZE:
    {
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SaveCriticalSpanOn();

      gfl2::heap::HeapBase* pDeviceHeap = m_pMiracleTradeWork->GetDeviceHeap();

      m_pNetGame = GFL_NEW( pDeviceHeap ) gflnet2::p2p::NetGame();
      
      m_pNetGame->Initialize();

      SetSequenceNo( SEQUENCE_NET_GAME_INITIALIZE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NET_GAME_INITIALIZE_WAIT:
    {
      if( m_pNetGame->IsInitialize() )
      {
        SetSequenceNo( SEQUENCE_SAVE_START );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_SAVE_START:
    {
      gfl2::heap::HeapBase*                                         pDeviceHeap              = m_pMiracleTradeWork->GetDeviceHeap();
      NetApp::MiracleTrade::ApplicationSystem::PokemonSelectInfo&   pokemonSelectInfo        = m_pMiracleTradeWork->GetPokemonSelectInfo();
      pml::pokepara::CoreParam&                                     selectPokemonCoreParam   = m_pMiracleTradeWork->GetSelectPokemonCoreParam();
      pml::pokepara::CoreParam&                                     receivedPokemonCoreParam = m_pMiracleTradeWork->GetReceivedPokemonCoreParam();
      bool&                                                         bCallZukanRegister       = m_pMiracleTradeWork->GetCallZukanRegister();
      bool&                                                         bCallEvolution           = m_pMiracleTradeWork->GetCallEvolution();
      app::event::ZukanRegisterEventParam&                          zukanRegisterEventParam  = m_pMiracleTradeWork->GetZukanRegisterEventParam();
      NetEvent::Evolution::EvolutionEventParam&                     evolutionEventParam      = m_pMiracleTradeWork->GetEvolutionEventParam();

#if defined(GF_PLATFORM_CTR)
      NetLib::Validation::PokemonValidation::ReplaceMonsName( &receivedPokemonCoreParam );
#endif

      if( item::ITEM_CheckPiece( receivedPokemonCoreParam.GetItem() ) )
      {
        receivedPokemonCoreParam.RemoveItem();
      }

      NetAppLib::Util::NetAppCommonTradeUtility::UpdateSaveData( pDeviceHeap, NetAppLib::Util::TRADE_ID_MIRACLE, pokemonSelectInfo.inBox, pokemonSelectInfo.tray, pokemonSelectInfo.pos, &receivedPokemonCoreParam, &selectPokemonCoreParam, bCallZukanRegister, zukanRegisterEventParam, bCallEvolution, evolutionEventParam );

      NetLib::P2P::P2pSyncSave::PARAM param;
      param.gameData  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
      param.netGame   = m_pNetGame;
      param.SyncNo1   = 87;
      param.SyncNo2   = 88;
      param.SyncNo3   = 89;
      param.pHeap     = pDeviceHeap;
      param.StartWait = 100;
      param.sendMonsNo = selectPokemonCoreParam.GetMonsNo();
      param.sendFormNo = selectPokemonCoreParam.GetFormNo();
      param.recvMonsNo = receivedPokemonCoreParam.GetMonsNo();
      param.recvFormNo = receivedPokemonCoreParam.GetFormNo();

      m_pP2pSyncSave = GFL_NEW( pDeviceHeap ) NetLib::P2P::P2pSyncSave( &param );

      SetSequenceNo( SEQUENCE_SAVE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_SAVE_WAIT:
    {
      if( m_pP2pSyncSave->UpdateSyncSave() )
      {
        GFL_SAFE_DELETE( m_pP2pSyncSave );

        if( m_pNetGame )
        {
          m_pNetGame->Finalize();
          GFL_SAFE_DELETE( m_pNetGame );
        }

        GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SaveCriticalSpanOff();

        SetSequenceNo( SEQUENCE_DELETE_P2P_PACKET_TRANSFER );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_DELETE_P2P_PACKET_TRANSFER:
    {
      GFL_SAFE_DELETE( m_pP2pPacketTransfer );

      SetSequenceNo( SEQUENCE_DISCONNECT_START );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_DISCONNECT_START:
    {
      pP2pConnectionManager->DisconnectStart();
      SetSequenceNo( SEQUENCE_DISCONNECT_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_DISCONNECT_WAIT:
    {
      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        if( m_bVerifyResult )
        {
          SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
        }
        else
        {
          SetSequenceNo( SEQUENCE_NOTIFICATION_VERIFY_NG );
        }
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( isNotification() )
      {
        if( m_pMatchingResponseListener )
        {
          m_pMatchingResponseListener->OnMiracleTradeMatchingSuccess();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_VERIFY_NG:
    {
      if( isNotification() )
      {
        if( m_pMatchingResponseListener )
        {
          m_pMatchingResponseListener->OnMiracleTradeMatchingVerifyNG();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( isNotification() )
      {
        if( m_pMatchingResponseListener )
        {
          m_pMatchingResponseListener->OnMiracleTradeMatchingError();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_TERMINATE:
    {
      bool bCanDestroy = true;

      if( m_pNetAppPokemonValidationUtility )
      {
        bCanDestroy = m_pNetAppPokemonValidationUtility->CanDestroy();
      }

      if( bCanDestroy )
      {
        GFL_SAFE_DELETE( m_pNetAppPokemonValidationUtility );

        pP2pConnectionManager->Terminate();
        result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }
    }
    break;
  }

  ++m_FrameCount;
  
  return result;
}


void MiracleTradeMatchingRequestSequence::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
  u32 receivedDataCount = pP2pPacketTransfer->GetReceivedDataCount();

  for( u32 i = 0; i < receivedDataCount; ++i )
  {
    int netID;
    
    TradeInfo recvData;
    
    bool bResult = pP2pPacketTransfer->GetReceivedData( i, COMMAND_ID_SEND_TRADE_INFO, netID, recvData.buffer, TRADE_INFO_SIZE );
    
    if( bResult )
    {
      // pokemon core param
      pml::pokepara::CoreParam& receivedPokemonCoreParam = m_pMiracleTradeWork->GetReceivedPokemonCoreParam();
      receivedPokemonCoreParam.Deserialize_Core( recvData.pokemonCoreParam );
      
      // my status
      Savedata::MyStatus& tradePersonStatus = m_pMiracleTradeWork->GetTradePersonStatus();
      tradePersonStatus.Deserialize( recvData.myStatus );
      // @fix MMCat[337]：相手がnijiの場合フラグをたてる。
      if( tradePersonStatus.GetRomCode() == VERSION_SUN ||
          tradePersonStatus.GetRomCode() == VERSION_MOON )
      {
        tradePersonStatus.SetDressUpParamRomVersionNiji();
      }

      // signature
      gfl2::std::MemCopy( recvData.signature, m_pMiracleTradeWork->GetTradePersonSignature(), SIGNATRUE_SIZE );

      // signature size
      u32 signatureSize = 0;
      gfl2::std::MemCopy( recvData.signatureSize, &signatureSize, SIGNATRUE_SIZE_SIZE );
      m_pMiracleTradeWork->SetTradePersonSignatureSize( signatureSize );

      // personal data
      NetAppLib::JoinFesta::JoinFestaPacketData& tradePersonJoinFestaPacketData = m_pMiracleTradeWork->GetTradePersonJoinFestaPacketData();
      gfl2::std::MemCopy( recvData.personalData, &tradePersonJoinFestaPacketData, sizeof( NetAppLib::JoinFesta::JoinFestaPacketData ) );

      // message id
      u32 hello = 0;
      u32 emotion = 0;

      gfl2::std::MemCopy( &recvData.phraseMessageID[0], &hello, sizeof( u32 ) );
      gfl2::std::MemCopy( &recvData.phraseMessageID[ sizeof( u32 ) ], &emotion, sizeof( u32 ) );

      m_pMiracleTradeWork->SetHelloPhraseMessageID( hello );
      m_pMiracleTradeWork->SetEmotionPhraseMessageID( emotion );
    }
  }

  m_bReceivedTradeInfo = true;
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE MiracleTradeMatchingRequestSequence::UpdateCancel()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  switch( m_CancelSequenceNo )
  {
    case CANCEL_SEQUENCE_NET_GAME_DELETE:
    {
      GFL_PRINT( "MiracleTradeMatchingRequestSequence : Cancel start. m_bIsTimeOut = %d\n", m_bIsTimeOut );
      
      GFL_SAFE_DELETE( m_pP2pSyncSave );

      if( m_pNetGame )
      {
        m_pNetGame->Finalize();
        GFL_SAFE_DELETE( m_pNetGame );
      }

      GFL_SAFE_DELETE( m_pP2pPacketTransfer );

      m_CancelSequenceNo = CANCEL_SEQUENCE_DISCONNECT_START;
    }
    break;

    case CANCEL_SEQUENCE_DISCONNECT_START:
    {
      pP2pConnectionManager->DisconnectStart();
      m_CancelSequenceNo = CANCEL_SEQUENCE_DISCONNECT_WAIT;
    }
    break;

    case CANCEL_SEQUENCE_DISCONNECT_WAIT:
    {
      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        m_CancelSequenceNo = CANCEL_SEQUENCE_TERMINATE;
      }
    }
    break;

    case CANCEL_SEQUENCE_TERMINATE:
    {
      bool bCanDestroy = true;

      if( m_pNetAppPokemonValidationUtility )
      {
        bCanDestroy = m_pNetAppPokemonValidationUtility->CanDestroy();
      }

      if( bCanDestroy )
      {
        GFL_SAFE_DELETE( m_pNetAppPokemonValidationUtility );

        if( m_bIsTimeOut )
        {
          if( m_pMatchingResponseListener )
          {
            m_pMatchingResponseListener->OnMiracleTradeMatchingTimeOut();
          }
        }

        pP2pConnectionManager->Terminate();
        result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }
    }
    break;
  }

  return result;
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE MiracleTradeMatchingRequestSequence::Update()
{
  if( m_pMatchingResponseListener )
  {
    m_pMatchingResponseListener->OnMiracleTradeMatchingError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE MiracleTradeMatchingRequestSequence::UpdateCancel()
{
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

void MiracleTradeMatchingRequestSequence::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
}

#endif



GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
