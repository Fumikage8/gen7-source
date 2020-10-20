// ============================================================================
/*
 * @file GTSRestorePokemonRequestSequence.cpp
 * @brief 預けていたポケモンが戻って来た時のリクエストです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSRestorePokemonRequestSequence.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSRestorePokemonRequestSequence::GTSRestorePokemonRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
  m_pRestorePokemonResponseListener( NULL )
{
}


GTSRestorePokemonRequestSequence::~GTSRestorePokemonRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )


void GTSRestorePokemonRequestSequence::updateSequence()
{
  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // セーブ開始
    case SEQUENCE_SAVE_START:
    {
      // 隠したポケモンを戻す
      NetApp::GTS::GTSUtility::RestoreUploadPokemon( *m_pGTSWork );

      // MyStatusのDataStoreIDを無効なデータとしてクリア
      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
      pMyStatus->SetPssPersonalDataStoreId( nn::nex::DataStoreConstants::INVALID_DATAID );

      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
      Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

      // 前回の保存日付を退避
      pPlayTime->PushSaveTime();
      pPlayTime->SetSaveTime( pGameData->GetGameTime() );

      // セーブ開始
      pGameData->SaveStart( m_pGTSWork->GetDeviceHeap() );

      SetSequenceNo( SEQUENCE_SAVE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // セーブ待ち
    case SEQUENCE_SAVE_WAIT:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result saveResult = pGameData->SaveMain();

      if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
      {
        SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
      }
      else
      if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
      {
        GFL_ASSERT_MSG( 0, "Save error = %d\n", saveResult );

        Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

        pPlayTime->PopSaveTime();

        SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 成功を通知する
    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( isNotification() )
      {
        if( m_pRestorePokemonResponseListener )
        {
          m_pRestorePokemonResponseListener->OnRestorePokemonSuccess();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

  }
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSRestorePokemonRequestSequence::Update()
{
  if( m_pRestorePokemonResponseListener )
  {
    m_pRestorePokemonResponseListener->OnRestorePokemonSuccess();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
