#if defined(GF_PLATFORM_CTR)

// ============================================================================
/*
 * @file GlobalAttractionOpenCheckRequestSequence.cpp
 * @brief 
 * @date 2016.01.19
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/GlobalAttraction/GlobalAttractionOpenCheckRequestSequence.h"
#include "NetStatic/NetAppLib/include/Util/NetAppDateTime.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"


#if defined( GF_PLATFORM_CTR )

#include <math/include/gfl2_math.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#endif // defined( GF_PLATFORM_CTR )

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GlobalAttraction)


static const u32 GLOBAL_ATTRACTION_INFO_FILE_SIZE = 496;


GlobalAttractionOpenCheckRequestSequence::GlobalAttractionOpenCheckRequestSequence() :
  m_pWork( NULL ),
  m_pGlobalAttractionOpenCheckResponseListener( NULL ),
  m_CancelSequenceNo( CANCEL_SEQUENCE_START ),
  m_SerialID( 0 ),
  m_bIsGlobalAttractionOpenNotEntry( false )
{
}


GlobalAttractionOpenCheckRequestSequence::~GlobalAttractionOpenCheckRequestSequence()
{
}

#if defined( GF_PLATFORM_CTR )

void GlobalAttractionOpenCheckRequestSequence::initialize()
{
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->SetAppHeap( m_pWork->GetDeviceHeap() );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->AddEventListener( this );
}


void GlobalAttractionOpenCheckRequestSequence::terminate()
{
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveEventListener( this );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveAppHeap();
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GlobalAttractionOpenCheckRequestSequence::Update()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  switch( GetSequenceNo() )
  {
    case SEQUENCE_INITIALIZE:
    {
      initialize();
      SetSequenceNo( SEQUENCE_START_DOWNLOAD_FILE );
    }
    break;

    case SEQUENCE_START_DOWNLOAD_FILE:
    {
      m_SerialID = 0;

      nn::Result nnResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->StartImmidiateTask( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1 , NetAppLib::Util::BossUtility::GetGlobalAttractionFileName() ,NULL,NetAppLib::Util::BossUtility::GetRegionURL() );

      if( nnResult.IsSuccess() )
      {
        SetSequenceNo( SEQUENCE_WAIT_DOWNLOAD_FILE );
      }
      else
      {
        SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
      }
    }
    break;

    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( m_pGlobalAttractionOpenCheckResponseListener )
      {
        m_pGlobalAttractionOpenCheckResponseListener->OnOpenCheckSuccess( m_bIsGlobalAttractionOpenNotEntry );
      }
      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( m_pGlobalAttractionOpenCheckResponseListener )
      {
        m_pGlobalAttractionOpenCheckResponseListener->OnOpenCheckError();
      }
      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    case SEQUENCE_TERMINATE:
    {
      terminate();
      result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
    break;
  }

  return result;
}




gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GlobalAttractionOpenCheckRequestSequence::UpdateCancel()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  switch( m_CancelSequenceNo )
  {
    case CANCEL_SEQUENCE_START:
    {
      GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->CancelImmidiateTask();

      m_CancelSequenceNo = CANCEL_SEQUENCE_WAIT;
    }
    break;

    case CANCEL_SEQUENCE_WAIT:
    {
      if( !GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->IsRunningThread() )
      {
        m_CancelSequenceNo = CANCEL_SEQUENCE_NOTIFICATION_CANCEL;
      }
    }
    break;

    case CANCEL_SEQUENCE_NOTIFICATION_CANCEL:
    {
      if( m_pGlobalAttractionOpenCheckResponseListener )
      {
        m_pGlobalAttractionOpenCheckResponseListener->OnOpenCheckCancel();
      }
      m_CancelSequenceNo = CANCEL_SEQUENCE_TERMINATE;
    }
    break;

    case CANCEL_SEQUENCE_TERMINATE:
    {
      terminate();
      result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
    break;
  }

  return result;
}


void GlobalAttractionOpenCheckRequestSequence::OnBossNewArrivalData( const u32 serialId , const bool isFinalData )
{
  if( m_SerialID != 0 )
  {
    return;
  }

  u32 serialMax = NetAppLib::Util::BossUtility::GetSerialIdMax( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_globalAttraction );
  u32 serialMin = NetAppLib::Util::BossUtility::GetSerialIdMin( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_globalAttraction );

  if( serialMax < serialId || serialId < serialMin )
  {
    if( isFinalData )
    {
      SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
    }
  }
  else
  {
    bool bResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RequestDataBody( serialId, gfl2::thread::Thread::THREAD_PRI_NORMAL + 1 );

    if( !bResult )
    {
      SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
    }
    else
    {
      m_SerialID = serialId;
    }
  }
}


void GlobalAttractionOpenCheckRequestSequence::OnBossNewArrivalDataEmpty( void )
{
  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


bool GlobalAttractionOpenCheckRequestSequence::isOpenGlobalAttraction( const u8* pDownloadData, u32& index )
{
  bool bIsOpenGlobalAttraction = false;

  gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager );

  nn::nex::DateTime nexDateTime;

  if( pNexManager->GetNgsFacade()->GetGameServerTime( &nexDateTime ) )
  {
    NetAppLib::Util::NetAppDateTime currentDateTime(
      nexDateTime.GetYear(),
      nexDateTime.GetMonth(),
      nexDateTime.GetDay(),
      nexDateTime.GetHour(),
      nexDateTime.GetMinute() );

    u64 currentTime = currentDateTime.GetTime();

    const int DATA_MAX = 12;

    Header header;
    GlobalAttractionInfo aGlobalAttractionInfos[ DATA_MAX ];

    gfl2::std::MemCopy( &pDownloadData[0], &header, sizeof( Header ) );
    gfl2::std::MemCopy( &pDownloadData[ sizeof( Header ) ], &aGlobalAttractionInfos[0], sizeof( aGlobalAttractionInfos ) );

    GFL_PRINT( "crc      = %d\n", header.crc );
    GFL_PRINT( "padding  = %d\n", header.padding );
    GFL_PRINT( "version  = %d\n", header.version );
    GFL_PRINT( "fileSize = %d\n", header.fileSize );
    GFL_PRINT( "reserve  = %d\n", header.reserve );

    for( u32 i = 0; i < DATA_MAX; ++i )
    {
      if( aGlobalAttractionInfos[i].enableData > 0 )
      {
        const u16 value2000 = 2000;

        u16 startDateTimeYear = value2000 + static_cast<u16>( aGlobalAttractionInfos[i].startDateTimeYear );
        u16 endDateTimeYear = value2000 + static_cast<u16>( aGlobalAttractionInfos[i].endDateTimeYear );

        NetAppLib::Util::NetAppDateTime startDateTime(
          startDateTimeYear,
          aGlobalAttractionInfos[i].startDateTimeMonth,
          aGlobalAttractionInfos[i].startDateTimeDay,
          aGlobalAttractionInfos[i].startDateTimeHour,
          aGlobalAttractionInfos[i].startDateTimeMinute );

        NetAppLib::Util::NetAppDateTime endDateTime(
          endDateTimeYear,
          aGlobalAttractionInfos[i].endDateTimeMonth,
          aGlobalAttractionInfos[i].endDateTimeDay,
          aGlobalAttractionInfos[i].endDateTimeHour,
          aGlobalAttractionInfos[i].endDateTimeMinute );

        u64 startTime = startDateTime.GetTime();

        u64 endTime = endDateTime.GetTime();

        if( startTime <= currentTime && currentTime <= endTime )
        {
          index = i;

          bIsOpenGlobalAttraction = true;
        }
      }
    }
  }

  return bIsOpenGlobalAttraction;
}


bool GlobalAttractionOpenCheckRequestSequence::isEntry( u32 index )
{
  bool bIsEntry = true;

  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionSaveData = pJoinFestaDataSave->GetGlobalAttractionData( static_cast<u8>( index ) );

  if( pGlobalAttractionSaveData )
  {
    if( pGlobalAttractionSaveData->participateState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_NULL )
    {
      bIsEntry = false;
    }
  }

  return bIsEntry;
}


bool GlobalAttractionOpenCheckRequestSequence::checkCrc( const size_t bodySize , const u8* pBody )
{
  bool bResult = false;

  GFL_PRINT( "bodySize = %d\n", bodySize );

  if( bodySize == GLOBAL_ATTRACTION_INFO_FILE_SIZE )
  {
    const size_t CRC_SIZE = sizeof( u16 );

    u16 crc = gfl2::math::Crc::Crc16( 0, &pBody[ CRC_SIZE ], bodySize - CRC_SIZE );

    u16 fileCrc = 0;

    std::memcpy( &fileCrc, &pBody[0], CRC_SIZE );

    GFL_PRINT( "crc = %d\n", crc );
    GFL_PRINT( "fileCrc = %d\n", fileCrc );

    if( crc == fileCrc )
    {
      bResult = true;
    }
  }

  return bResult;
}


bool GlobalAttractionOpenCheckRequestSequence::OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody )
{
  if( pHeader )
  {
    bool bCheckCrcResult = checkCrc( bodySize, pBody );

    if( pHeader->serialId == m_SerialID && bCheckCrcResult )
    {
      u32 index = 0;

      if( isOpenGlobalAttraction( pBody, index ) )
      {
        if( !isEntry( index ) )
        {
          m_bIsGlobalAttractionOpenNotEntry = true;
        }
      }

      SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
    }
    else
    {
      SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
    }
  }
  else
  {
    SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
  }

  return true;
}


void GlobalAttractionOpenCheckRequestSequence::OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{
  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


void GlobalAttractionOpenCheckRequestSequence::OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result )
{
  // @note : インターネット接続アプリの時はエラーアプレットを表示しない
//  NetLib::Error::NijiNetworkError error;
//  error.SetResultBossError( result );
//  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


void GlobalAttractionOpenCheckRequestSequence::OnBossError( const nn::Result& result )
{
  // @note : インターネット接続アプリの時はエラーアプレットを表示しない
//  NetLib::Error::NijiNetworkError error;
//  error.SetResultBossError( result );
//  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


void GlobalAttractionOpenCheckRequestSequence::OnBossTaskError( const char* pTaskName , const u32 errorCode )
{
  // @note : インターネット接続アプリの時はエラーアプレットを表示しない
//  NetLib::Error::NijiNetworkError error;
//  error.SetResultBossTaskError( errorCode );
//  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GlobalAttractionOpenCheckRequestSequence::Update()
{
  if( m_pGlobalAttractionOpenCheckResponseListener )
  {
    m_pGlobalAttractionOpenCheckResponseListener->OnOpenCheckError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GlobalAttractionOpenCheckRequestSequence::UpdateCancel()
{
  if( m_pGlobalAttractionOpenCheckResponseListener )
  {
    m_pGlobalAttractionOpenCheckResponseListener->OnOpenCheckCancel();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(GlobalAttraction)
GFL_NAMESPACE_END(NetAppLib)
#endif
