// ============================================================================
/*
 * @file JoinFestaGlobalAttractionDownloadRequestSequence.cpp
 * @brief レギュレーションデータを探す通信リクエストです。
 * @date 2016.02.29
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/RequestSequence/JoinFestaGlobalAttractionDownloadRequestSequence.h"

#if defined( GF_PLATFORM_CTR )

#include <math/include/gfl2_math.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#endif // defined( GF_PLATFORM_CTR )

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(RequestSequence)


static const u32 GLOBAL_ATTRACTION_INFO_FILE_SIZE = 496;


JoinFestaGlobalAttractionDownloadRequestSequence::JoinFestaGlobalAttractionDownloadRequestSequence() :
  m_pJoinFestaGlobalAttractionWork( NULL ),
  m_pJoinFestaGlobalAttractionDownloadResponseListener( NULL ),
  m_CancelSequenceNo( CANCEL_SEQUENCE_START ),
  m_SerialID( 0 )
{
}


JoinFestaGlobalAttractionDownloadRequestSequence::~JoinFestaGlobalAttractionDownloadRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )

void JoinFestaGlobalAttractionDownloadRequestSequence::initialize()
{
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->SetAppHeap( m_pJoinFestaGlobalAttractionWork->GetDeviceHeap() );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->AddEventListener( this );
}


void JoinFestaGlobalAttractionDownloadRequestSequence::terminate()
{
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveEventListener( this );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveAppHeap();
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE JoinFestaGlobalAttractionDownloadRequestSequence::Update()
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

      nn::Result nnResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->StartImmidiateTask( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1, NetAppLib::Util::BossUtility::GetGlobalAttractionFileName() , NULL , NetAppLib::Util::BossUtility::GetRegionURL() );

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
      if( m_pJoinFestaGlobalAttractionDownloadResponseListener )
      {
        m_pJoinFestaGlobalAttractionDownloadResponseListener->OnDownloadSuccess();
      }
      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( m_pJoinFestaGlobalAttractionDownloadResponseListener )
      {
        m_pJoinFestaGlobalAttractionDownloadResponseListener->OnDownloadError();
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




gflnet2::ServerClient::REQUEST_SEQUENCE_STATE JoinFestaGlobalAttractionDownloadRequestSequence::UpdateCancel()
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
      if( m_pJoinFestaGlobalAttractionDownloadResponseListener )
      {
        m_pJoinFestaGlobalAttractionDownloadResponseListener->OnDownloadCancel();
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


void JoinFestaGlobalAttractionDownloadRequestSequence::OnBossNewArrivalData( const u32 serialId , const bool isFinalData )
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


void JoinFestaGlobalAttractionDownloadRequestSequence::OnBossNewArrivalDataEmpty( void )
{
  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


bool JoinFestaGlobalAttractionDownloadRequestSequence::checkCrc( const size_t bodySize , const u8* pBody )
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


bool JoinFestaGlobalAttractionDownloadRequestSequence::OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody )
{
  if( pHeader )
  {
    bool bCheckCrcResult = checkCrc( bodySize, pBody );

    if( pHeader->serialId == m_SerialID && bCheckCrcResult )
    {
      gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager );

      NetAppLib::Util::NetAppDateTime currentDateTime;

      nn::nex::DateTime nexDateTime;

      if( pNexManager->GetNgsFacade()->GetGameServerTime( &nexDateTime ) )
      {
        currentDateTime = NetAppLib::Util::NetAppDateTime(
          nexDateTime.GetYear(),
          nexDateTime.GetMonth(),
          nexDateTime.GetDay(),
          nexDateTime.GetHour(),
          nexDateTime.GetMinute() );
      }

      if( m_pJoinFestaGlobalAttractionWork )
      {
        m_pJoinFestaGlobalAttractionWork->SetDownloadData( pBody );
        m_pJoinFestaGlobalAttractionWork->SetCurrentDateTime( currentDateTime );
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


void JoinFestaGlobalAttractionDownloadRequestSequence::OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{
  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


void JoinFestaGlobalAttractionDownloadRequestSequence::OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossError( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


void JoinFestaGlobalAttractionDownloadRequestSequence::OnBossError( const nn::Result& result )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossError( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


void JoinFestaGlobalAttractionDownloadRequestSequence::OnBossTaskError( const char* pTaskName , const u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossTaskError( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE JoinFestaGlobalAttractionDownloadRequestSequence::Update()
{
  if( m_pJoinFestaGlobalAttractionDownloadResponseListener )
  {
    m_pJoinFestaGlobalAttractionDownloadResponseListener->OnDownloadError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE JoinFestaGlobalAttractionDownloadRequestSequence::UpdateCancel()
{
  if( m_pJoinFestaGlobalAttractionDownloadResponseListener )
  {
    m_pJoinFestaGlobalAttractionDownloadResponseListener->OnDownloadCancel();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
