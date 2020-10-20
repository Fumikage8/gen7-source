// ============================================================================
/*
 * @file GTSTradePokemonConfirmRequestSequence.cpp
 * @brief 交換相手のポケモンが存在するか確認するリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonConfirmRequestSequence.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <util/include/gfl2_std_string.h>
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSTradePokemonConfirmRequestSequence::GTSTradePokemonConfirmRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
  m_pTradePokemonConfirmResponseListener( NULL )
{
}


GTSTradePokemonConfirmRequestSequence::~GTSTradePokemonConfirmRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )

void GTSTradePokemonConfirmRequestSequence::updateSequence()
{
  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // 交換準備
    case SEQUENCE_PREPARE_TRADE_POKEMON:
    {
      NetApp::GTS::GtsSerializeRecordKey prepareUploadKey;
      gfl2::std::MemClear( &prepareUploadKey, sizeof( NetApp::GTS::GtsSerializeRecordKey ) );

      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();
      if( pGTSSaveData->GetDepositFlag() )
      {
        prepareUploadKey = pGTSSaveData->GetUploadKey();
      }

      const NetApp::GTS::GtsSearchResultInfo& tradePartnerInfo = m_pGTSWork->GetTradePartnerInfo();

      m_pNexGTSClient->PrepareTradePokemon( prepareUploadKey, tradePartnerInfo.serializeTradeKey );

      SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 既にポケモンが交換されていたを通知する
    case SEQUENCE_NOTIFICATION_ALREADY_TRADE_POKEMON:
    {
      if( isNotification() )
      {
        if( m_pTradePokemonConfirmResponseListener )
        {
          m_pTradePokemonConfirmResponseListener->OnTradePokemonAlreadyTradePokemon();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 成功を通知する
    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( isNotification() )
      {
        if( m_pTradePokemonConfirmResponseListener )
        {
          m_pTradePokemonConfirmResponseListener->OnTradePokemonConfirmSuccess();
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
        if( m_pTradePokemonConfirmResponseListener )
        {
          m_pTradePokemonConfirmResponseListener->OnTradePokemonConfirmError();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;
  }
}


//-----------------------------------------------------------------------------
// NetApp::GTS::NexGlobalTradeStationClientListener
//-----------------------------------------------------------------------------
void GTSTradePokemonConfirmRequestSequence::OnPrepareTradePokemonSuccess( const nn::nex::GlobalTradeStationDownloadPokemonResult& preTradeResult, const NetApp::GTS::GtsSerializeRecordKey& prepareTradeKey )
{
  GFL_PRINT( "GTSTradePokemonConfirmRequestSequence::OnPrepareTradePokemonSuccess() \n" );
  nn::nex::qVector<u8> pokeData;
  preTradeResult.GetPokemonData( pokeData );

  NetApp::GTS::GtsSerializePokemonParam serializePokemonParam;
  gfl2::std::MemClear( &serializePokemonParam, sizeof( NetApp::GTS::GtsSerializePokemonParam ) );
  GFL_ASSERT( pokeData.size() <= sizeof( serializePokemonParam.data ) );
  gfl2::std::MemCopy( reinterpret_cast<const u8*>(&pokeData[0]), serializePokemonParam.data, pokeData.size() );

  pml::pokepara::CoreParam& demoRecvPokemon = m_pGTSWork->GetDemoRecvPokemon();

  demoRecvPokemon.Deserialize_Core( serializePokemonParam.data );

#if defined(GF_PLATFORM_CTR)
    NetLib::Validation::PokemonValidation::ReplaceMonsName( &demoRecvPokemon );
#endif // defined(GF_PLATFORM_CTR)

  m_pGTSWork->SetPrepareTradeKey( prepareTradeKey );

  SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
}

void GTSTradePokemonConfirmRequestSequence::OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "GTSTradePokemonConfirmRequestSequence::OnGlobalTradeStationClientError() \n" );

  if( errorCode == NetApp::GTS::NexGlobalTradeStationClient::E_ERR_NOT_FOUND
   || errorCode == NetApp::GTS::NexGlobalTradeStationClient::E_ERR_POKEMON_NOT_EXIST )
  {
    SetSequenceNo( SEQUENCE_NOTIFICATION_ALREADY_TRADE_POKEMON );
  }
  else
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultErrorCode( errorCode );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
  }
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSTradePokemonConfirmRequestSequence::Update()
{
  if( m_pTradePokemonConfirmResponseListener )
  {
    m_pTradePokemonConfirmResponseListener->OnTradePokemonConfirmError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
