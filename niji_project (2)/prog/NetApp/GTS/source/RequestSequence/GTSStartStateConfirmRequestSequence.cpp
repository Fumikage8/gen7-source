﻿// ============================================================================
/*
 * @file GTSStartStateConfirmRequestSequence.cpp
 * @brief GTSアプリ起動時のGTSの状態を確認するリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSStartStateConfirmRequestSequence.h"
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


GTSStartStateConfirmRequestSequence::GTSStartStateConfirmRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
  m_pStartStateConfirmResponseListener( NULL )
{
}


GTSStartStateConfirmRequestSequence::~GTSStartStateConfirmRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )


void GTSStartStateConfirmRequestSequence::updateSequence()
{
  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // セーブデータをチェック
    case SEQUENCE_CHECK_SAVE_DATA:
    {
      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

      if( pGTSSaveData->GetDepositFlag() )
      {
        SetSequenceNo( SEQUENCE_DOWNLOAD_MY_POKEMON );
      }
      else
      {
        SetSequenceNo( SEQUENCE_NOTIFICATION_POKEMON_NOT_UPLOAD );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 自分のポケモンのダウンロード
    case SEQUENCE_DOWNLOAD_MY_POKEMON:
    {
      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

      pml::pokepara::CoreParam& demoSendPokemon = m_pGTSWork->GetDemoSendPokemon();

      // 進化判定用にアップロードした自分のポケモンのパラメータをワークに入れておく必要がある
      pGTSSaveData->GetBackupPokemonCoreParam( &demoSendPokemon );

      m_pNexGTSClient->DownloadMyPokemon( pGTSSaveData->GetUploadKey() );

      SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // ポケモンがアップロードされていないを通知する
    case SEQUENCE_NOTIFICATION_POKEMON_NOT_UPLOAD:
    {
      if( isNotification() )
      {
        if( m_pStartStateConfirmResponseListener )
        {
          m_pStartStateConfirmResponseListener->OnStartStateConfirmPokemonNotUpload();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // ポケモンがアップロードされているを通知する
    case SEQUENCE_NOTIFICATION_POKEMON_UPLOAD:
    {
      if( isNotification() )
      {
        if( m_pStartStateConfirmResponseListener )
        {
          m_pStartStateConfirmResponseListener->OnStartStateConfirmPokemonUpload();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // ポケモンの交換が行われているを通知する
    case SEQUENCE_NOTIFICATION_POKEMON_TRADE:
    {
      if( isNotification() )
      {
        if( m_pStartStateConfirmResponseListener )
        {
          m_pStartStateConfirmResponseListener->OnStartStateConfrimPokemonTrade();
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
        if( m_pStartStateConfirmResponseListener )
        {
          m_pStartStateConfirmResponseListener->OnStartStateConfirmPokemonDelete();
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
        if( m_pStartStateConfirmResponseListener )
        {
          m_pStartStateConfirmResponseListener->OnStartStateConfirmError();
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
void GTSStartStateConfirmRequestSequence::OnDownloadMyPokemonSuccess( const nn::nex::GlobalTradeStationDownloadMyPokemonResult& downloadResult )
{
  if( downloadResult.IsTraded() )
  {
    SetSequenceNo( SEQUENCE_NOTIFICATION_POKEMON_TRADE );
  }
  else
  {
    SetSequenceNo( SEQUENCE_NOTIFICATION_POKEMON_UPLOAD );
  }
}


void GTSStartStateConfirmRequestSequence::OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode )
{
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

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSStartStateConfirmRequestSequence::Update()
{
  if( m_pStartStateConfirmResponseListener )
  {
    m_pStartStateConfirmResponseListener->OnStartStateConfirmError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
