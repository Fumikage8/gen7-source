// ============================================================================
/*
 * @file GTSTradePokemonRequestSequence.cpp
 * @brief ポケモンを交換するリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonRequestSequence.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonTradeUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <util/include/gfl2_std_string.h>
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSTradePokemonRequestSequence::GTSTradePokemonRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
  m_pTradePokemonResponseListener( NULL ),
  m_NetAppCommonSaveUtility()
{
#ifdef GF_PLATFORM_CTR
  gfl2::std::MemClear( &m_UploadPokemonParam, sizeof( NetApp::GTS::GtsSerializePokemonParam ) );
#endif // GF_PLATFORM_CTR
}


GTSTradePokemonRequestSequence::~GTSTradePokemonRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )

void GTSTradePokemonRequestSequence::updateSequence()
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
        SetSequenceNo( SEQUENCE_DOWNLOAD_PERSONAL_DATA );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 相手のパーソナルデータのダウンロード
    case SEQUENCE_DOWNLOAD_PERSONAL_DATA:
    {
      const NetApp::GTS::GtsSearchResultInfo& tradePartnerInfo = m_pGTSWork->GetTradePartnerInfo();

      if( ( getJoinFestaPersonalData( tradePartnerInfo.indexData.principalId ) == NULL )
       && ( tradePartnerInfo.indexData.dataStoreId != nn::nex::DataStoreConstants::INVALID_DATAID ) )
      {
        gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
        gflnet2::nex::NexDataStoreClient* pDataStoreClient = pNexManager->GetDataStoreClient();

        nn::nex::qVector<nn::nex::qUnsignedInt64> dataIds;
        dataIds.push_back( tradePartnerInfo.indexData.dataStoreId );
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
      NetApp::GTS::GTSUtility::SetCriticalSectionFlag( true );

      pml::pokepara::CoreParam& uploadPokemonParam = m_pGTSWork->GetSelectPokemonCoreParam();

      uploadPokemonParam.Serialize_Core( m_UploadPokemonParam.data );

      pml::pokepara::CoreParam& sendPokemon = m_pGTSWork->GetDemoSendPokemon();

      sendPokemon.Deserialize_Core( m_UploadPokemonParam.data );

      {
        gfl2::heap::HeapBase*                                 pDeviceHeap              = m_pGTSWork->GetDeviceHeap();
        NetApp::GTS::ApplicationSystem::PokemonSelectInfo&    pokemonSelectInfo        = m_pGTSWork->GetPokemonSelectInfo();
        pml::pokepara::CoreParam&                             selectPokemonCoreParam   = m_pGTSWork->GetSelectPokemonCoreParam();
        pml::pokepara::CoreParam&                             receivedPokemonCoreParam = m_pGTSWork->GetDemoRecvPokemon();
        bool&                                                 bCallZukanRegister       = m_pGTSWork->GetCallZukanRegister();
        bool&                                                 bCallEvolution           = m_pGTSWork->GetCallEvolution();
        app::event::ZukanRegisterEventParam&                  zukanRegisterEventParam  = m_pGTSWork->GetZukanRegisterEventParam();
        NetEvent::Evolution::EvolutionEventParam&             evolutionEventParam      = m_pGTSWork->GetEvolutionEventParam();

        NetAppLib::Util::NetAppCommonTradeUtility::UpdateSaveData( pDeviceHeap, NetAppLib::Util::TRADE_ID_GTS_SEARCH, pokemonSelectInfo.inBox, pokemonSelectInfo.tray, pokemonSelectInfo.pos, &receivedPokemonCoreParam, &selectPokemonCoreParam, bCallZukanRegister, zukanRegisterEventParam, bCallEvolution, evolutionEventParam );
      }

      // 通信経路をGTSに設定
      {
        const NetApp::GTS::GtsSearchResultInfo& tradePartnerInfo = m_pGTSWork->GetTradePartnerInfo();
        NetAppLib::JoinFesta::JoinFestaPersonalData* pUpdatePersonalData = getJoinFestaPersonalData( tradePartnerInfo.indexData.principalId );
        if( pUpdatePersonalData )
        {
          pUpdatePersonalData->SetCommRoute( JoinFestaScript::COMM_ROUTE_GTS );
        }
      }

      
      NetAppLib::Util::NetAppCommonSaveUtility::SetDisconnectDetectedFlag( false );
      

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
          setRandomWaitSequence( SEQUENCE_TRADE_POKEMON );
        }
        else
        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
        {
          GFL_ASSERT_MSG( 0, "First save error = %d\n", saveResult );

          Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

          pPlayTime->PopSaveTime();

          setRandomWaitSequence( SEQUENCE_TRADE_POKEMON );
        }
      }
    }
    break;

    // ---------------------------------------------------------------
    // 交換
    case SEQUENCE_TRADE_POKEMON:
    {
      const NetApp::GTS::GtsSearchResultInfo& tradePartnerInfo = m_pGTSWork->GetTradePartnerInfo();
      const NetApp::GTS::GtsSerializeRecordKey& prepareTradeKey = m_pGTSWork->GetPrepareTradeKey();
      const NetApp::GTS::GtsIndexData& uploadGTSIndexData = m_pGTSWork->GetUploadGTSIndexData();
      const NetApp::GTS::GtsSignature& signature = m_pGTSWork->GetSignature();
      
      NetApp::GTS::GtsSerializeRecordKey prepareUploadKey;
      gfl2::std::MemClear( &prepareUploadKey, sizeof( NetApp::GTS::GtsSerializeRecordKey ) );

      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();
      if( pGTSSaveData->GetDepositFlag() )
      {
        prepareUploadKey = pGTSSaveData->GetUploadKey();
      }

      m_pNexGTSClient->TradePokemon( tradePartnerInfo.serializeTradeKey, prepareTradeKey, prepareUploadKey, &uploadGTSIndexData, &m_UploadPokemonParam, signature.data, signature.size );

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
        if( m_pTradePokemonResponseListener )
        {
          m_pTradePokemonResponseListener->OnTradePokemonSuccess();
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
        if( m_pTradePokemonResponseListener )
        {
          m_pTradePokemonResponseListener->OnTradePokemonError();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;
  }
}


NetAppLib::JoinFesta::JoinFestaPersonalData* GTSTradePokemonRequestSequence::getJoinFestaPersonalData( u32 principalID )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  JoinFestaFriendKey friendKey;
  friendKey.initialize( principalID );

  return pJoinFestaPersonalDataManager->GetPersonalData( friendKey );
}


//-----------------------------------------------------------------------------
// gflnet2::nex::NexDataStoreClientListener
//-----------------------------------------------------------------------------
void GTSTradePokemonRequestSequence::OnNexDataStoreDownloadMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results, const nn::nex::qVector<nn::nex::DataStoreMetaInfo>& metas )
{
  GFL_PRINT( "GTSTradePokemonRequestSequence::OnNexDataStoreDownloadMetaDataFinish() \n" );
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


void GTSTradePokemonRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "GTSTradePokemonRequestSequence::OnNexDataStoreClientError() \n" );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


//-----------------------------------------------------------------------------
// NetApp::GTS::NexGlobalTradeStationClientListener
//-----------------------------------------------------------------------------
void GTSTradePokemonRequestSequence::OnTradePokemonSuccess( const nn::nex::GlobalTradeStationTradePokemonResult& tradeResult )
{
  GFL_PRINT( "GTSTradePokemonRequestSequence::OnTradePokemonSuccess() \n" );
  SetSequenceNo( SEQUENCE_LAST_SAVE_START );
}


void GTSTradePokemonRequestSequence::OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "GTSTradePokemonRequestSequence::OnGlobalTradeStationClientError() \n" );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );

}

#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSTradePokemonRequestSequence::Update()
{
  if( m_pTradePokemonResponseListener )
  {
    m_pTradePokemonResponseListener->OnTradePokemonError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
