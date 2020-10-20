// ============================================================================
/*
 * @file GTSDownloadStateConfirmRequestSequence.cpp
 * @brief 引き取り時のGTSの状態を確認するリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSDownloadStateConfirmRequestSequence.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSDownloadStateConfirmRequestSequence::GTSDownloadStateConfirmRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
  m_pDownloadStateConfirmResponseListener( NULL )
{
}


GTSDownloadStateConfirmRequestSequence::~GTSDownloadStateConfirmRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )


void GTSDownloadStateConfirmRequestSequence::updateSequence()
{
  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // 自分のポケモンのダウンロード
    case SEQUENCE_DOWNLOAD_MY_POKEMON:
    {
      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

      pml::pokepara::CoreParam& demoSendPokemon = m_pGTSWork->GetDemoSendPokemon();

      // 進化判定用にアップロードした自分のポケモンをdemoSendPokeに入れておく
      pGTSSaveData->GetBackupPokemonCoreParam( &demoSendPokemon );

      m_pNexGTSClient->DownloadMyPokemon( pGTSSaveData->GetUploadKey() );

      SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // ポケモンの交換が行われているを通知する
    case SEQUENCE_NOTIFICATION_POKEMON_TRADE:
    {
      if( isNotification() )
      {
        if( m_pDownloadStateConfirmResponseListener )
        {
          m_pDownloadStateConfirmResponseListener->OnDownloadStateConfirmPokemonTrade();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // ポケモンの交換が行われていないを通知する
    case SEQUENCE_NOTIFICATION_POKEMON_NOT_TRADE:
    {
      if( isNotification() )
      {
        if( m_pDownloadStateConfirmResponseListener )
        {
          m_pDownloadStateConfirmResponseListener->OnDownloadStateConfirmPokemonNotTrade();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // ポケモンが消えている通知する
    case SEQUENCE_NOTIFICATION_POKEMON_DELETE:
    {
      if( isNotification() )
      {
        if( m_pDownloadStateConfirmResponseListener )
        {
          m_pDownloadStateConfirmResponseListener->OnDownloadStateConfirmPokemonDelete();
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
        if( m_pDownloadStateConfirmResponseListener )
        {
          m_pDownloadStateConfirmResponseListener->OnDownloadStateConfirmError();
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
void GTSDownloadStateConfirmRequestSequence::OnDownloadMyPokemonSuccess( const nn::nex::GlobalTradeStationDownloadMyPokemonResult& downloadResult )
{
  GFL_PRINT( "GTSDownloadStateConfirmRequestSequence::OnDownloadMyPokemonSuccess() downloadResult.IsTraded() = %d \n", downloadResult.IsTraded() );
  if( downloadResult.IsTraded() )
  {
    SetSequenceNo( SEQUENCE_NOTIFICATION_POKEMON_TRADE );
  }
  else
  {
    SetSequenceNo( SEQUENCE_NOTIFICATION_POKEMON_NOT_TRADE );
  }
}


void GTSDownloadStateConfirmRequestSequence::OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "GTSDownloadStateConfirmRequestSequence::OnGlobalTradeStationClientError()\n" );

  // 有効期限が切れたことによるポケモンの消失はポケモンが戻ってきた事にする
  if( errorCode == NetApp::GTS::NexGlobalTradeStationClient::E_ERR_NOT_FOUND
   || errorCode == NetApp::GTS::NexGlobalTradeStationClient::E_ERR_POKEMON_NOT_EXIST )
  {
    SetSequenceNo( SEQUENCE_NOTIFICATION_POKEMON_DELETE );
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

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSDownloadStateConfirmRequestSequence::Update()
{
  if( m_pDownloadStateConfirmResponseListener )
  {
    m_pDownloadStateConfirmResponseListener->OnDownloadStateConfirmError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
