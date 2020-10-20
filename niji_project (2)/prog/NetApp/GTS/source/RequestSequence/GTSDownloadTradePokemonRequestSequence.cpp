// ============================================================================
/*
 * @file GTSDownloadTradePokemonRequestSequence.cpp
 * @brief ポケモンの交換が成立していた時の受け取りリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSDownloadTradePokemonRequestSequence.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonTradeUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSDownloadTradePokemonRequestSequence::GTSDownloadTradePokemonRequestSequence() :
  NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
#ifdef GF_PLATFORM_CTR
  m_TradePartnerDataStoreID( nn::nex::DataStoreConstants::INVALID_DATAID ),
  m_TradePartnerPrincipalID( NN_FRIENDS_INVALID_PRINCIPAL_ID ),
#endif // GF_PLATFORM_CTR
  m_pDownloadTradePokemonResponseListener( NULL ),
  m_NetAppCommonSaveUtility()
{
}


GTSDownloadTradePokemonRequestSequence::~GTSDownloadTradePokemonRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )


void GTSDownloadTradePokemonRequestSequence::updateSequence()
{
  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // 切断検知フラグのセーブ開始
    case SEQUENCE_DISCONNECT_DETECTED_SAVE_START:
    {
      NetAppLib::Util::NetAppCommonSaveUtility::SetDisconnectDetectedFlag( true );

      SetSequenceNo( SEQUENCE_DISCONNECT_DETECTED_SAVE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 切断検知フラグのセーブ待ち
    case SEQUENCE_DISCONNECT_DETECTED_SAVE_WAIT:
    {
      if( m_NetAppCommonSaveUtility.Save() )
      {
        SetSequenceNo( SEQUENCE_DOWNLOAD_OTHER_POKEMON );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 相手のポケモンのダウンロード
    case SEQUENCE_DOWNLOAD_OTHER_POKEMON:
    {
      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

      NetApp::GTS::GTSUtility::SetCriticalSectionFlag( true );

      m_pNexGTSClient->DownloadOtherPokemon( pGTSSaveData->GetUploadKey() );

      SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 相手のパーソナルデータのダウンロード
    case SEQUENCE_DOWNLOAD_PERSONAL_DATA:
    {
      if( ( getJoinFestaPersonalData( m_TradePartnerPrincipalID ) == NULL )
       && ( m_TradePartnerDataStoreID != nn::nex::DataStoreConstants::INVALID_DATAID ) )
      {
        gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
        gflnet2::nex::NexDataStoreClient* pDataStoreClient = pNexManager->GetDataStoreClient();

        nn::nex::qVector<nn::nex::qUnsignedInt64> dataIds;
        dataIds.push_back( m_TradePartnerDataStoreID );
        pDataStoreClient->DownloadMetaData( dataIds, gflnet2::nex::NexDataStoreClient::DL_TARGET_METABINARY );

        SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
      }
      else
      {
        SetSequenceNo( SEQUENCE_FIRST_SAVE_START );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 99%セーブ開始
    case SEQUENCE_FIRST_SAVE_START:
    {
      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();
      pGTSSaveData->SetDepositFlag( false );

      {
        gfl2::heap::HeapBase*                                 pDeviceHeap              = m_pGTSWork->GetDeviceHeap();
        NetApp::GTS::ApplicationSystem::PokemonSelectInfo&    pokemonSelectInfo        = m_pGTSWork->GetPokemonSelectInfo();
        pml::pokepara::CoreParam&                             sendPokemonCoreParam     = m_pGTSWork->GetDemoSendPokemon();
        pml::pokepara::CoreParam&                             receivedPokemonCoreParam = m_pGTSWork->GetDemoRecvPokemon();
        bool&                                                 bCallZukanRegister       = m_pGTSWork->GetCallZukanRegister();
        bool&                                                 bCallEvolution           = m_pGTSWork->GetCallEvolution();
        app::event::ZukanRegisterEventParam&                  zukanRegisterEventParam  = m_pGTSWork->GetZukanRegisterEventParam();
        NetEvent::Evolution::EvolutionEventParam&             evolutionEventParam      = m_pGTSWork->GetEvolutionEventParam();

        NetAppLib::Util::NetAppCommonTradeUtility::UpdateSaveData( pDeviceHeap, NetAppLib::Util::TRADE_ID_GTS_DOWNLOAD, pokemonSelectInfo.inBox, pokemonSelectInfo.tray, pokemonSelectInfo.pos, &receivedPokemonCoreParam, &sendPokemonCoreParam, bCallZukanRegister, zukanRegisterEventParam, bCallEvolution, evolutionEventParam );
      }

      NetAppLib::Util::NetAppCommonSaveUtility::SetDisconnectDetectedFlag( false );

      // 通信経路をGTSに設定
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pUpdatePersonalData = getJoinFestaPersonalData( m_TradePartnerPrincipalID );
        if( pUpdatePersonalData )
        {
          pUpdatePersonalData->SetCommRoute( JoinFestaScript::COMM_ROUTE_GTS );
        }
      }

      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
      Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

      // 前回の保存日付を退避
      pPlayTime->PushSaveTime();
      pPlayTime->SetSaveTime( pGameData->GetGameTime() );

      setFirstSaveStartFlag( true );

      // Firstセーブの開始
      pGameData->FirstSaveStart( m_pGTSWork->GetDeviceHeap() );

      SetSequenceNo( SEQUENCE_FIRST_SAVE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 99%セーブ待ち
    case SEQUENCE_FIRST_SAVE_WAIT:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result saveResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

      if( pGameData->FirstSaveMain( &saveResult ) )
      {
        setFirstSaveCompleteFlag( true );

        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          setRandomWaitSequence( SEQUENCE_DELETE_POKEMON );
        }
        else
        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
        {
          GFL_ASSERT_MSG( 0, "First save error = %d\n", saveResult );

          Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

          pPlayTime->PopSaveTime();

          setRandomWaitSequence( SEQUENCE_DELETE_POKEMON );
        }
      }
    }
    break;

    // ---------------------------------------------------------------
    // サーバーにあるポケモンの削除
    case SEQUENCE_DELETE_POKEMON:
    {
      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

      m_pNexGTSClient->DeletePokemon( pGTSSaveData->GetUploadKey(), nn::nex::GlobalTradeStationConstants::DELETE_FLAG_TRADED );
      SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 1%セーブ開始
    case SEQUENCE_LAST_SAVE_START:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      setLastSaveStartFlag( true );

      pGameData->LastSaveStart( m_pGTSWork->GetDeviceHeap() );

      SetSequenceNo( SEQUENCE_LAST_SAVE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 1%セーブ待ち
    case SEQUENCE_LAST_SAVE_WAIT:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result saveResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

      if( pGameData->LastSaveMain( &saveResult ) )
      {
        setLastSaveCompleteFlag( true );

        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          NetApp::GTS::GTSUtility::SetCriticalSectionFlag( false );

          SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
        }
        else
        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
        {
          GFL_ASSERT_MSG( 0, "Last save error = %d\n", saveResult );
          
          Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();
          
          pPlayTime->PopSaveTime();

          SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
        }
      }
    }
    break;

    // ---------------------------------------------------------------
    // 成功を通知する
    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( isNotification() )
      {
        if( m_pDownloadTradePokemonResponseListener )
        {
          m_pDownloadTradePokemonResponseListener->OnDownloadTradePokemonSuccess();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // エラーを通知する
    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( isNotification() )
      {
        if( m_pDownloadTradePokemonResponseListener )
        {
          m_pDownloadTradePokemonResponseListener->OnDownloadTradePokemonError();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

  }
}


NetAppLib::JoinFesta::JoinFestaPersonalData* GTSDownloadTradePokemonRequestSequence::getJoinFestaPersonalData( u32 principalID )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  JoinFestaFriendKey friendKey;
  friendKey.initialize( principalID );

  return pJoinFestaPersonalDataManager->GetPersonalData( friendKey );
}


//-----------------------------------------------------------------------------
// NetApp::GTS::NexGlobalTradeStationClientListener
//-----------------------------------------------------------------------------
void GTSDownloadTradePokemonRequestSequence::OnDownloadOtherPokemonSuccess( const nn::nex::GlobalTradeStationTradePokemonResult& downloadResult )
{
  const nn::nex::GlobalTradeStationDownloadPokemonResult otherPokemonResult = downloadResult.GetResult();

  // 交換相手のポケモンデータ
  {
    NetApp::GTS::GtsSerializePokemonParam otherPokemonParam;
    nn::nex::qVector<u8> pokeData;
    otherPokemonResult.GetPokemonData( pokeData );
    ::std::memcpy( otherPokemonParam.data, reinterpret_cast<const u8*>(&pokeData[0]), pokeData.size() );

    pml::pokepara::CoreParam& demoRecvPokemon = m_pGTSWork->GetDemoRecvPokemon();

    demoRecvPokemon.Deserialize_Core( otherPokemonParam.data );

#if defined(GF_PLATFORM_CTR)
    NetLib::Validation::PokemonValidation::ReplaceMonsName( &demoRecvPokemon );
#endif // defined(GF_PLATFORM_CTR)

  }

  // 交換相手の名前
  {
    nn::nex::qVector<u8> indexData;
    otherPokemonResult.GetIndexData( indexData );
    NetApp::GTS::GtsIndexData* pIndexData = reinterpret_cast<NetApp::GTS::GtsIndexData*>(&indexData[0]);

    gfl2::str::StrBuf& tradePartnerName = m_pGTSWork->GetTradePartnerName();

    tradePartnerName.SetStr( reinterpret_cast<gfl2::str::STRCODE*>(pIndexData->TrainerName) );
  
    m_TradePartnerDataStoreID = pIndexData->dataStoreId;
    m_TradePartnerPrincipalID = pIndexData->principalId;
  }

  SetSequenceNo( SEQUENCE_DOWNLOAD_PERSONAL_DATA );
}


void GTSDownloadTradePokemonRequestSequence::OnDeletePokemonSuccess( void )
{
  SetSequenceNo( SEQUENCE_LAST_SAVE_START );
}


void GTSDownloadTradePokemonRequestSequence::OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


//-----------------------------------------------------------------------------
// gflnet2::nex::NexDataStoreClientListener
//-----------------------------------------------------------------------------
void GTSDownloadTradePokemonRequestSequence::OnNexDataStoreDownloadMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results, const nn::nex::qVector<nn::nex::DataStoreMetaInfo>& metas )
{
  nn::nex::qResult ret = results.at(0);

  if( (nn::nex::qBool)ret )
  {
    nn::nex::DataStoreMetaInfo       info = metas.at(0);
    nn::nex::qVector<nn::nex::qByte> binary;
    info.GetMetaBinary( binary );

    #if PM_DEBUG
    GFL_PRINT("OnNexDataStoreDownloadMetaDataFinish: success[%u]\n", binary.size() );
    #endif

    // 交換相手を登録
    {
      const NetAppLib::JoinFesta::JoinFestaPacketData* pJoinFestaPacketData = reinterpret_cast<const NetAppLib::JoinFesta::JoinFestaPacketData*>(&(binary[0]));

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );
      NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData = pJoinFestaPersonalDataManager->GetPersonalData( pJoinFestaPacketData->personalInfo.GetJoinFestaFriendKey() );

      bool bIsOnline = ( pJoinFestaPersonalData ) ? pJoinFestaPersonalData->IsOnline() : false;

      NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager );
      pJoinFestaPacketManager->UpdateWithJoinFestaPacketData( pJoinFestaPacketData, bIsOnline );
    }

    binary.clear();

    SetSequenceNo( SEQUENCE_FIRST_SAVE_START );
  }
  else
  {
    u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( ret );

    switch( errorCode )
    {
      case  61203:  //アクセス権限がない
      case  61204:  //指定したデータIDが見つからない
      case  61207:  //期限切れ
      {
        // ダウンロード失敗だが無視して先に進む
        SetSequenceNo( SEQUENCE_FIRST_SAVE_START );
      }
      break;

      default:
      {
        NetLib::Error::NijiNetworkError error;
        error.SetResultNex( ret );
        GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

        SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
      }
      break;
    }
  }
}


void GTSDownloadTradePokemonRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSDownloadTradePokemonRequestSequence::Update()
{
  if( m_pDownloadTradePokemonResponseListener )
  {
    m_pDownloadTradePokemonResponseListener->OnDownloadTradePokemonError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
