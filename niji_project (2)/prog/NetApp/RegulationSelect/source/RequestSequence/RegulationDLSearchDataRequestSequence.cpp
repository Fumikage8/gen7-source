// ============================================================================
/*
 * @file RegulationDLSearchDataRequestSequence.cpp
 * @brief レギュレーションデータを探す通信リクエストです。
 * @date 2015.06.30
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/RequestSequence/RegulationDLSearchDataRequestSequence.h"

#if defined( GF_PLATFORM_CTR )

#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPackFile.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#endif // defined( GF_PLATFORM_CTR )

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(RequestSequence)


RegulationDLSearchDataRequestSequence::RegulationDLSearchDataRequestSequence() :
  m_pRegulationSelectWork( NULL ),
  m_pRegulationDLSearchDataResponseListener( NULL ),
  m_CancelSequenceNo( CANCEL_SEQUENCE_START ),
  m_SerialID( 0 ),
  m_WorkBuffer()
{
}


RegulationDLSearchDataRequestSequence::~RegulationDLSearchDataRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )

void RegulationDLSearchDataRequestSequence::initialize()
{
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->SetAppHeap( m_pRegulationSelectWork->GetDeviceHeap() );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->AddEventListener( this );
}


void RegulationDLSearchDataRequestSequence::terminate()
{
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveEventListener( this );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveAppHeap();
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE RegulationDLSearchDataRequestSequence::Update()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  switch( GetSequenceNo() )
  {
    case SEQUENCE_INITIALIZE:
    {
      initialize();
      SetSequenceNo( SEQUENCE_START_DOWNLOAD_FILE_LIST );
    }
    break;

    case SEQUENCE_START_DOWNLOAD_FILE_LIST:
    {
      char attribute1[ NetAppLib::Util::BossUtility::ATTRIBUTE_BUFFER_SIZE ];

      gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES attributes;
      
      NetAppLib::Util::BossUtility::CreateXYBossAttribute1( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_regulationDownload, attribute1 );

      attributes.pAttribute1 = attribute1;
      
      bool bResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->StartDownloadFileList( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1, NULL, &attributes );

      if( bResult )
      {
        SetSequenceNo( SEQUENCE_WAIT_DOWNLOAD_FILE_LIST );
      }
      else
      {
        SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
      }
    }
    break;

    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( m_pRegulationDLSearchDataResponseListener )
      {
        m_pRegulationDLSearchDataResponseListener->OnSearchDataSuccess();
      }
      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( m_pRegulationDLSearchDataResponseListener )
      {
        m_pRegulationDLSearchDataResponseListener->OnSearchDataError();
      }
      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    case SEQUENCE_NOTIFICATION_ERROR2:
    {
      if( m_pRegulationDLSearchDataResponseListener )
      {
        m_pRegulationDLSearchDataResponseListener->OnSearchDataError2();
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




gflnet2::ServerClient::REQUEST_SEQUENCE_STATE RegulationDLSearchDataRequestSequence::UpdateCancel()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  switch( m_CancelSequenceNo )
  {
    case CANCEL_SEQUENCE_START:
    {
      GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->CancelDownloadFileList();
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
      if( m_pRegulationDLSearchDataResponseListener )
      {
        m_pRegulationDLSearchDataResponseListener->OnSearchDataCancel();
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


void RegulationDLSearchDataRequestSequence::OnBossNewArrivalData( const u32 serialId , const bool isFinalData )
{
  if( m_SerialID != 0 )
  {
    return;
  }

  u32 serialMax = NetAppLib::Util::BossUtility::GetSerialIdMax( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_regulationDownload );
  u32 serialMin = NetAppLib::Util::BossUtility::GetSerialIdMin( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_regulationDownload );

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


void RegulationDLSearchDataRequestSequence::OnBossNewArrivalDataEmpty( void )
{
  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


bool RegulationDLSearchDataRequestSequence::OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody )
{
  if( pHeader )
  {
    if( pHeader->serialId == m_SerialID )
    {
      gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadList = m_pRegulationSelectWork->GetDownloadList();
      m_pRegulationSelectWork->ClearList( pDownloadList );

      NetAppLib::Util::PackFile packFile( pBody );

      bool bIsBrokenRegulationPackFileResult = packFile.IsBrokenRegulationPackFile( 0, 32, bodySize );

      if( bIsBrokenRegulationPackFileResult )
      {
        SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );

        return true;
      }

      u32 fileCount = packFile.GetFileCount();

      for( u32 i = 0; i < fileCount; ++i )
      {
        const void* pBuffer = packFile.GetFileBuffer(i);
        u32 fileSize = packFile.GetFileSize(i);

#if PM_DEBUG
        Regulation regulation( m_pRegulationSelectWork->GetDeviceHeap() );
        regulation.SetData( pBuffer, fileSize );
        regulation.DebugPrint();
#endif // PM_DEBUG


        gfl2::std::MemClear( m_WorkBuffer, sizeof( m_WorkBuffer ) );
        gfl2::std::MemCopy( pBuffer, m_WorkBuffer, fileSize );

        m_pRegulationSelectWork->AddRegulation( pDownloadList, m_WorkBuffer, sizeof( m_WorkBuffer ) );
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


void RegulationDLSearchDataRequestSequence::OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{
  if( fileNum != 1 )
  {
    SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
  }
  else
  {
    m_SerialID = 0;

    nn::Result nnResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->StartImmidiateTask( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1, pFileList[0].fileName , NULL , NetAppLib::Util::BossUtility::GetRegionURL() );

    if( !nnResult.IsSuccess() )
    {
      SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
    }
  }
}


void RegulationDLSearchDataRequestSequence::OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result )
{
  if( !IsCancelRequested() )
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultBossError( result );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
  }

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR2 );
}


void RegulationDLSearchDataRequestSequence::OnBossError( const nn::Result& result )
{
  if( !IsCancelRequested() )
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultBossError( result );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
  }

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR2 );
}


void RegulationDLSearchDataRequestSequence::OnBossTaskError( const char* pTaskName , const u32 errorCode )
{
  if( !IsCancelRequested() )
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultBossTaskError( errorCode );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
  }

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR2 );
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE RegulationDLSearchDataRequestSequence::Update()
{
  if( m_pRegulationDLSearchDataResponseListener )
  {
    m_pRegulationDLSearchDataResponseListener->OnSearchDataError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


gflnet2::ServerClient::REQUEST_SEQUENCE_STATE RegulationDLSearchDataRequestSequence::UpdateCancel()
{
  if( m_pRegulationDLSearchDataResponseListener )
  {
    m_pRegulationDLSearchDataResponseListener->OnSearchDataCancel();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
