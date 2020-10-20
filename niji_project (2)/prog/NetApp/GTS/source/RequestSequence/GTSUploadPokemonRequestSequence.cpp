// ============================================================================
/*
 * @file GTSUploadPokemonRequestSequence.cpp
 * @brief GTSサーバーにポケモンをアップロードするリクエストを投げるシーケンスです。
 * @date 2015..04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonRequestSequence.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include <Savedata/include/PokeDiarySave.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSUploadPokemonRequestSequence::GTSUploadPokemonRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
  m_pUploadPokemonResponseListener( NULL ),
  m_NetAppCommonSaveUtility(),
  m_bIsUploadPokemonError( false )
{
#ifdef GF_PLATFORM_CTR
  gfl2::std::MemClear( &m_PrepareUploadKey, sizeof( NetApp::GTS::GtsSerializeRecordKey ) );
  gfl2::std::MemClear( &m_UploadPokemonParam, sizeof( NetApp::GTS::GtsSerializePokemonParam ) );
  SetSequenceNo( SEQUENCE_INITIALIZE );
#endif // GF_PLATFORM_CTR
}


GTSUploadPokemonRequestSequence::~GTSUploadPokemonRequestSequence()
{
  if( m_bIsUploadPokemonError )
  {
    NetApp::GTS::GTSUtility::RestoreUploadPokemon( *m_pGTSWork );
  }
}


#if defined( GF_PLATFORM_CTR )


void GTSUploadPokemonRequestSequence::updateSequence()
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
        SetSequenceNo( SEQUENCE_PREPARE_UPLOAD );
      }
    }
    break;

    // ---------------------------------------------------------------
    // アップロード準備
    case SEQUENCE_PREPARE_UPLOAD:
    {
      pml::pokepara::CoreParam& uploadPokemonParam = m_pGTSWork->GetSelectPokemonCoreParam();

      uploadPokemonParam.Serialize_Core( m_UploadPokemonParam.data );

      NetApp::GTS::GTSUtility::BackupUploadPokemon( *m_pGTSWork );

      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

      pml::pokepara::CoreParam& demoSendPokemon = m_pGTSWork->GetDemoSendPokemon();

      // デモ用にCoreParamを隠したセーブデータからコピーしておく
      pGTSSaveData->GetBackupPokemonCoreParam( &demoSendPokemon );

      const NetApp::GTS::GtsIndexData& uploadGTSIndexData = m_pGTSWork->GetUploadGTSIndexData();

      pGTSSaveData->SetUploadGtsIndexData( uploadGTSIndexData );

      m_pNexGTSClient->PrepareUploadPokemon();

      {
        //ダイアリー記録
        PokeDiary::Set( PDID_COUNT_GTS_POKEPUT , uploadPokemonParam.GetMonsNo() );

        Savedata::IncRecord( Savedata::Record::RECID_GTS_PUT );
      }

      SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 99%セーブ開始
    case SEQUENCE_FIRST_SAVE_START:
    {
      NetAppLib::Util::NetAppCommonSaveUtility::SetDisconnectDetectedFlag( false );

      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
      Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

      // 前回の保存日付を退避
      pPlayTime->PushSaveTime();
      pPlayTime->SetSaveTime( pGameData->GetGameTime() );

      GFL_PRINT( "GTSUploadPokemonRequestSequence : setFirstSaveStartFlag( true );\n" );
      setFirstSaveStartFlag( true );

      // Firstセーブの開始
      GFL_PRINT( "GTSUploadPokemonRequestSequence : pGameData->FirstSaveStart( m_pGTSWork->GetDeviceHeap() );\n" );
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

      GFL_PRINT( "GTSUploadPokemonRequestSequence : pGameData->FirstSaveMain( &saveResult )\n" );
      if( pGameData->FirstSaveMain( &saveResult ) )
      {
        GFL_PRINT( "GTSUploadPokemonRequestSequence : setFirstSaveCompleteFlag( true );\n" );
        setFirstSaveCompleteFlag( true );

        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          setRandomWaitSequence( SEQUENCE_UPLOAD );
        }
        else
        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
        {
          GFL_ASSERT_MSG( 0, "First save error = %d\n", saveResult );

          Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

          pPlayTime->PopSaveTime();

          setRandomWaitSequence( SEQUENCE_UPLOAD );
        }
      }
    }
    break;

    // ---------------------------------------------------------------
    // アップロード
    case SEQUENCE_UPLOAD:
    {
      const NetApp::GTS::GtsIndexData& uploadGTSIndexData = m_pGTSWork->GetUploadGTSIndexData();
      const NetApp::GTS::GtsSignature& signature = m_pGTSWork->GetSignature();

      m_pNexGTSClient->UploadPokemon( m_PrepareUploadKey, &uploadGTSIndexData, &m_UploadPokemonParam, signature.data, signature.size );

      SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 1%セーブ開始
    case SEQUENCE_LAST_SAVE_START:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      GFL_PRINT( "GTSUploadPokemonRequestSequence : setLastSaveStartFlag( true );\n" );
      setLastSaveStartFlag( true );

      GFL_PRINT( "GTSUploadPokemonRequestSequence : pGameData->LastSaveStart( m_pGTSWork->GetDeviceHeap() );\n" );
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

      GFL_PRINT( "GTSUploadPokemonRequestSequence : pGameData->LastSaveMain( &saveResult )\n" );
      if( pGameData->LastSaveMain( &saveResult ) )
      {
        GFL_PRINT( "GTSUploadPokemonRequestSequence : setLastSaveCompleteFlag( true );\n" );
        setLastSaveCompleteFlag( true );

        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
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
        if( m_pUploadPokemonResponseListener )
        {
          m_pUploadPokemonResponseListener->OnUploadPokemonSuccess();
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
        if( m_pUploadPokemonResponseListener )
        {
          m_pUploadPokemonResponseListener->OnUploadPokemonError();
        }

        // エラーが発生した時は強制キャンセル処理が走るのを待つ
        SetSequenceNo( SEQUENCE_FORCE_CANCEL_WAIT );
      }
    }
    break;

  }
}


//-----------------------------------------------------------------------------
// NetApp::GTS::NexGlobalTradeStationClientListener
//-----------------------------------------------------------------------------
void GTSUploadPokemonRequestSequence::OnPrepareUploadPokemonSuccess( const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey )
{
  GFL_PRINT( "GTSUploadPokemonRequestSequence::OnPrepareUploadPokemonSuccess() \n" );

  m_PrepareUploadKey = prepareUploadKey;

  // アップロードキーを保存
  Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();
  pGTSSaveData->SetUploadKey( m_PrepareUploadKey );
  
  SetSequenceNo( SEQUENCE_FIRST_SAVE_START );
}


void GTSUploadPokemonRequestSequence::OnUploadPokemonSuccess( void )
{
  GFL_PRINT( "GTSUploadPokemonRequestSequence::OnUploadPokemonSuccess() \n" );

  SetSequenceNo( SEQUENCE_LAST_SAVE_START );
}


void GTSUploadPokemonRequestSequence::OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "GTSUploadPokemonRequestSequence::OnGlobalTradeStationClientError()\n" );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_bIsUploadPokemonError = true;

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );

}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSUploadPokemonRequestSequence::Update()
{
  if( m_pUploadPokemonResponseListener )
  {
    m_pUploadPokemonResponseListener->OnUploadPokemonError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
