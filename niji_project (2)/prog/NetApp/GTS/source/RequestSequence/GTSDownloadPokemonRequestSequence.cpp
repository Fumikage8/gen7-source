// ============================================================================
/*
 * @file GTSDownloadPokemonRequestSequence.cpp
 * @brief GTSサーバーからポケモンを引き取るリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSDownloadPokemonRequestSequence.h"
#include "NetApp/GTS/source/GTSUtility.h"
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


GTSDownloadPokemonRequestSequence::GTSDownloadPokemonRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
  m_pDownloadPokemonResponseListener( NULL ),
  m_NetAppCommonSaveUtility()
{
}


GTSDownloadPokemonRequestSequence::~GTSDownloadPokemonRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )


void GTSDownloadPokemonRequestSequence::updateSequence()
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
        SetSequenceNo( SEQUENCE_FIRST_SAVE_START );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 99%セーブ開始
    case SEQUENCE_FIRST_SAVE_START:
    {
      NetApp::GTS::GTSUtility::SetCriticalSectionFlag( true );

      NetApp::GTS::GTSUtility::RestoreUploadPokemon( *m_pGTSWork );

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
      m_pNexGTSClient->DeletePokemon( pGTSSaveData->GetUploadKey(), nn::nex::GlobalTradeStationConstants::DELETE_FLAG_UPLOADED );
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
        if( m_pDownloadPokemonResponseListener )
        {
          m_pDownloadPokemonResponseListener->OnDownloadPokemonSuccess();
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
        if( m_pDownloadPokemonResponseListener )
        {
          m_pDownloadPokemonResponseListener->OnDownloadPokemonError();
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
void GTSDownloadPokemonRequestSequence::OnDeletePokemonSuccess( void )
{
  GFL_PRINT( "GTSDownloadPokemonRequestSequence::OnDeletePokemonSuccess()\n" );

  SetSequenceNo( SEQUENCE_LAST_SAVE_START );
}


void GTSDownloadPokemonRequestSequence::OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "GTSDownloadPokemonRequestSequence::OnGlobalTradeStationClientError()\n" );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );

}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSDownloadPokemonRequestSequence::Update()
{
  if( m_pDownloadPokemonResponseListener )
  {
    m_pDownloadPokemonResponseListener->OnDownloadPokemonError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
