// ============================================================================
/*
 * @file GTSRequestSequenceBase.cpp
 * @brief GTSの通信リクエストのベースクラスです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSRequestSequenceBase.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <system/include/GflUse.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSRequestSequenceBase::GTSRequestSequenceBase( u32 notificationWaitFrame ) :
#if defined( GF_PLATFORM_CTR )
  m_pNexGTSClient( NULL ),
  m_pNetAppPokemonValidationUtility( NULL ),
#endif // defined( GF_PLATFORM_CTR )
  m_pGTSWork( NULL ),
  m_FrameCount( 0 ),
  m_NotificationWaitFrame( notificationWaitFrame ),
  m_RandomFrameCount( 0 ),
  m_RandomWaitFrame( 0 ),
  m_RandomNextSequenceNo( 0 ),
  m_CancelSequenceNo( 0 ),
  m_bFirstSaveStartFlag( false ),
  m_bFirstSaveCompleteFlag( false ),
  m_bLastSaveStartFlag( false ),
  m_bLastSaveCompleteFlag( false )
{
#ifdef GF_PLATFORM_CTR
  SetSequenceNo( SEQUENCE_INITIALIZE );
#endif // GF_PLATFORM_CTR
}


GTSRequestSequenceBase::~GTSRequestSequenceBase()
{
}


#if defined( GF_PLATFORM_CTR )

void GTSRequestSequenceBase::initialize()
{
  gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  pNexManager->BindDataStoreClient();

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  m_pNexGTSClient = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::NexGlobalTradeStationClient();

  m_pNexGTSClient->Initialize( pNexManager->GetDataStoreClient()->GetDataStoreClient() );
  m_pNexGTSClient->RegistGlobalTradeStationClientListener( this );

  gflnet2::nex::NexDataStoreClient* pDataStoreClient = pNexManager->GetDataStoreClient();
  pDataStoreClient->RegistDataStoreClientListener( this );
}


void GTSRequestSequenceBase::terminate()
{
  GFL_SAFE_DELETE( m_pNetAppPokemonValidationUtility );

  gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  gflnet2::nex::NexDataStoreClient* pDataStoreClient = pNexManager->GetDataStoreClient();
  pDataStoreClient->UnregistDataStoreClientListener( this );

  m_pNexGTSClient->UnregistGlobalTradeStationClientListener( this );
  m_pNexGTSClient->Finalize();

  GFL_SAFE_DELETE( m_pNexGTSClient );

  pNexManager->UnbindDataStoreClient();
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSRequestSequenceBase::Update()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
  switch( GetSequenceNo() )
  {
    case SEQUENCE_INITIALIZE:
    {
      initialize();
      SetSequenceNo( SEQUENCE_START );
    }
    break;

    case SEQUENCE_TERMINATE:
    {
      bool bCanDestroy = true;

      if( m_pNetAppPokemonValidationUtility )
      {
        bCanDestroy = m_pNetAppPokemonValidationUtility->CanDestroy();
      }

      if( bCanDestroy )
      {
        terminate();
        result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }
    }
    break;

    case SEQUENCE_RESPONSE_WAIT:
    {
      // 派生先のリスナーのハンドラが呼ばれる
    }
    break;

    case SEQUENCE_RANDOM_WAIT:
    {
      if( m_RandomFrameCount >= m_RandomWaitFrame )
      {
        SetSequenceNo( m_RandomNextSequenceNo );
      }
      ++m_RandomFrameCount;
    }
    break;

    default:
    {
      updateSequence();
    }
    break;
  }

  ++m_FrameCount;

  return result;
}


void GTSRequestSequenceBase::setRandomWaitSequence( u32 nextSequenceNo )
{
  m_RandomFrameCount      = 0;
  m_RandomWaitFrame       = System::GflUse::GetPublicRand( NetApp::GTS::GTS_RANDOM_WAIT_FRAME );
  m_RandomNextSequenceNo  = nextSequenceNo;
  SetSequenceNo( SEQUENCE_RANDOM_WAIT );
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSRequestSequenceBase::UpdateCancel()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  switch( m_CancelSequenceNo )
  {
    case CANCEL_SEQUENCE_GTS_CLIENT_CANCEL_WAIT:
    {
      if( m_pNexGTSClient->IsConnecting() )
      {
        m_pNexGTSClient->CancelConnecting();
      }
      else
      {
        m_CancelSequenceNo = CANCEL_SEQUENCE_DATA_STORE_CLIENT_CANCEL_WAIT;
      }
    }
    break;

    case CANCEL_SEQUENCE_DATA_STORE_CLIENT_CANCEL_WAIT:
    {
      gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
      gflnet2::nex::NexDataStoreClient* pDataStoreClient = pNexManager->GetDataStoreClient();

      if( pDataStoreClient->IsConnecting() )
      {
        pDataStoreClient->CancelConnecting();
      }
      else
      {
        m_CancelSequenceNo = CANCEL_SEQUENCE_POKEMON_VERIFY_WAIT;
      }
    }
    break;

    case CANCEL_SEQUENCE_POKEMON_VERIFY_WAIT:
    {
      bool bCanDestroy = true;

      if( m_pNetAppPokemonValidationUtility )
      {
        bCanDestroy = m_pNetAppPokemonValidationUtility->CanDestroy();
      }

      if( bCanDestroy )
      {
        m_CancelSequenceNo = CANCEL_SEQUENCE_FIRST_SAVE_WAIT;
      }
    }
    break;

    case CANCEL_SEQUENCE_FIRST_SAVE_WAIT:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result saveResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

      if( m_bFirstSaveStartFlag == true && m_bFirstSaveCompleteFlag == false )
      {
        GFL_PRINT( "GTSRequestSequenceBase : pGameData->FirstSaveMain( &saveResult )\n" );
        if( pGameData->FirstSaveMain( &saveResult ) )
        {
          GFL_PRINT( "GTSRequestSequenceBase : setFirstSaveCompleteFlag( true );\n" );
          setFirstSaveCompleteFlag( true );

          if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
          {
            GFL_ASSERT_MSG( 0, "First save error = %d\n", saveResult );

            Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

            pPlayTime->PopSaveTime();
          }

          m_CancelSequenceNo = CANCEL_SEQUENCE_FIRST_SAVE_CANCEL_WAIT;
        }
      }
      else
      {
        m_CancelSequenceNo = CANCEL_SEQUENCE_FIRST_SAVE_CANCEL_WAIT;
      }
    }
    break;

    case CANCEL_SEQUENCE_FIRST_SAVE_CANCEL_WAIT:
    {
      if( m_bFirstSaveCompleteFlag == true && m_bLastSaveStartFlag == false )
      {
        GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

        GFL_PRINT( "GTSRequestSequenceBase : pGameData->FirstSaveCancel();\n" );
        pGameData->FirstSaveCancel();
      }

      m_CancelSequenceNo = CANCEL_SEQUENCE_LAST_SAVE_WAIT;
    }
    break;

    case CANCEL_SEQUENCE_LAST_SAVE_WAIT:
    {
      if( m_bLastSaveStartFlag == true && m_bLastSaveCompleteFlag == false )
      {
        GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

        gfl2::fs::FsCtrBackupSystem::Result saveResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

        GFL_PRINT( "GTSRequestSequenceBase : pGameData->LastSaveMain( &saveResult )\n" );
        if( pGameData->LastSaveMain( &saveResult ) )
        {
          GFL_PRINT( "GTSRequestSequenceBase : setLastSaveCompleteFlag( true );\n" );
          setLastSaveCompleteFlag( true );

          if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
          {
            GFL_ASSERT_MSG( 0, "Last save error = %d\n", saveResult );
          
            Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();
          
            pPlayTime->PopSaveTime();
          }

          m_CancelSequenceNo = CANCEL_SEQUENCE_TERMINATE;
        }
      }
      else
      {
        m_CancelSequenceNo = CANCEL_SEQUENCE_TERMINATE;
      }
    }
    break;

    case CANCEL_SEQUENCE_TERMINATE:
    {
      notificationCancel();
      terminate();
      result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
    break;
  }

  return result;
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSRequestSequenceBase::Update()
{
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSRequestSequenceBase::UpdateCancel()
{
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
