// ============================================================================
/*
 * @file RegulationSelectSDCardLoadFramBase.cpp
 * @brief SDカードからレギュレーションデータを読み込むクラスです。
 * @date 2015.06.19
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectSDCardLoadFramBase.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)

static const u32 REGULATION_MAX = 12; // 配信レギュレーションを保存できる最大数

RegulationSelectSDCardLoadFramBase::RegulationSelectSDCardLoadFramBase( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  NetApp::RegulationSelect::RegulationSelectFrameBase( pRegulationSelectWork ),
  m_BrokenFileList( pRegulationSelectWork->GetDeviceHeap(), REGULATION_MAX ),
  m_RegulationExtSaveData( pRegulationSelectWork->GetDeviceHeap() ),
  m_eUpdateSequence( UPDATE_SEQUENCE_IDLE ),
  m_LoadRegulation( pRegulationSelectWork->GetDeviceHeap() ),
  m_LoadRegulationType( ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_00 ),
  m_bCheckRomCode( false )
{
}


RegulationSelectSDCardLoadFramBase::~RegulationSelectSDCardLoadFramBase()
{
}


void RegulationSelectSDCardLoadFramBase::loadRegulationData( bool bCheckRomCode )
{
  if( m_eUpdateSequence == UPDATE_SEQUENCE_IDLE )
  {
    m_bCheckRomCode = bCheckRomCode;
    m_eUpdateSequence = UPDATE_SEQUENCE_CLEAR_DOWNLOADED_LIST;
  }
}


bool RegulationSelectSDCardLoadFramBase::cleanup()
{
  bool bResult = false;

  bool bIsUpdate = m_RegulationExtSaveData.Update();

  if( !bIsUpdate )
  {
    gfl2::fs::ExtSaveFile::Result result = m_RegulationExtSaveData.GetResult();

    if( ExtSavedata::NijiExtSaveSystem::IsFatalError( result ) )
    {
      onLoadRegulationDataFatalError();
    }

    bResult = true;
  }

  return bResult;
}


applib::frame::Result RegulationSelectSDCardLoadFramBase::UpdateFunc( void )
{
  applib::frame::Result result = RegulationSelectFrameBase::UpdateFunc();
  
  bool bIsUpdate = m_RegulationExtSaveData.Update();

  switch( m_eUpdateSequence )
  {
    case UPDATE_SEQUENCE_CLEAR_DOWNLOADED_LIST:
    {
      gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList = m_pRegulationSelectWork->GetDownloadedList();
      m_pRegulationSelectWork->ClearList( pDownloadedList );

      m_LoadRegulationType = ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_00;

      m_BrokenFileList.Erase();

      m_eUpdateSequence = UPDATE_SEQUENCE_LOAD_START;
    }
    break;

    case UPDATE_SEQUENCE_LOAD_START:
    {
      if( m_RegulationExtSaveData.StartLoadRegulationData( static_cast<ExtSavedata::RegulationExtSaveData::REGULATION_TYPE>( m_LoadRegulationType ), m_LoadRegulation ) )
      {
        m_eUpdateSequence = UPDATE_SEQUENCE_LOAD_WAIT;
      }
      else
      {
        m_LoadRegulationType++;

        if( m_LoadRegulationType > ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_11 )
        {
          m_eUpdateSequence = ( m_BrokenFileList.Size() > 0 ) ? UPDATE_SEQUENCE_BROKEN_FILE_DELETE_START : UPDATE_SEQUENCE_NOTIFICATION_SUCCESS;
        }
      }
    }
    break;

    case UPDATE_SEQUENCE_LOAD_WAIT:
    {
      if( !bIsUpdate )
      {
        gfl2::fs::ExtSaveFile::Result result = m_RegulationExtSaveData.GetResult();

        if( result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
        {
#if PM_DEBUG
          GFL_PRINT( "====================================================================================\n" );
          GFL_PRINT( "------------------------------------------------------------------------------------\n" );
          GFL_PRINT( "--------------------------------- Load Regulation ----------------------------------\n" );
          GFL_PRINT( "------------------------------------------------------------------------------------\n" );
          GFL_PRINT( "====================================================================================\n" );

          GFL_PRINT( "m_LoadRegulationType = %d\n", m_LoadRegulationType );

          m_LoadRegulation.DebugPrint();
#endif // PM_DEBUG

          if( checkRomCode() )
          {
            gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList = m_pRegulationSelectWork->GetDownloadedList();

            u32 size = m_LoadRegulation.GetSerializeDataSize();

            gfl2::heap::HeapBase* pHeap = m_pRegulationSelectWork->GetDeviceHeap();

            u8* pTemp = GFL_NEW_ARRAY( pHeap->GetLowerHandle() ) u8[ size ];
          
            m_LoadRegulation.Serialize( pTemp );

            m_pRegulationSelectWork->AddRegulation( pDownloadedList, pTemp, size, m_LoadRegulationType );

            GFL_SAFE_DELETE_ARRAY( pTemp );
          }
        }
        else
        if( result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA )
        {
          m_BrokenFileList.PushBack( m_LoadRegulationType );
        }

        m_LoadRegulationType++;

        if( m_LoadRegulationType > ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_11 )
        {
          m_eUpdateSequence = ( m_BrokenFileList.Size() > 0 ) ? UPDATE_SEQUENCE_BROKEN_FILE_DELETE_START : UPDATE_SEQUENCE_NOTIFICATION_SUCCESS;
        }
        else
        {
          m_eUpdateSequence = UPDATE_SEQUENCE_LOAD_START;
        }

        if( ExtSavedata::NijiExtSaveSystem::IsFatalError( result ) )
        {
          m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR;
        }
      }
    }
    break;

    case UPDATE_SEQUENCE_BROKEN_FILE_DELETE_START:
    {
      gfl2::util::List<int>::Iterator it = m_BrokenFileList.Begin();
      
      if( it != m_BrokenFileList.End() )
      {
        m_RegulationExtSaveData.StartDeleteRegulationData( static_cast<ExtSavedata::RegulationExtSaveData::REGULATION_TYPE>( *it ) );

        m_eUpdateSequence = UPDATE_SEQUENCE_BROKEN_FILE_DELETE_WAIT;
      }
      else
      {
        m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_ERROR;
      }
    }
    break;

    case UPDATE_SEQUENCE_BROKEN_FILE_DELETE_WAIT:
    {
      if( !bIsUpdate )
      {
        gfl2::fs::ExtSaveFile::Result result = m_RegulationExtSaveData.GetResult();

        m_BrokenFileList.PopFront();

        m_eUpdateSequence = UPDATE_SEQUENCE_BROKEN_FILE_DELETE_START;

        if( ExtSavedata::NijiExtSaveSystem::IsFatalError( result ) )
        {
          m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR;
        }
      }
    }
    break;

    case UPDATE_SEQUENCE_NOTIFICATION_SUCCESS:
    {
      onLoadRegulationDataSuccess();
      m_eUpdateSequence = UPDATE_SEQUENCE_IDLE;
    }
    break;

    case UPDATE_SEQUENCE_NOTIFICATION_ERROR:
    {
      onLoadRegulationDataError();
      m_eUpdateSequence = UPDATE_SEQUENCE_IDLE;
    }
    break;

    case UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR:
    {
      onLoadRegulationDataFatalError();
      m_eUpdateSequence = UPDATE_SEQUENCE_IDLE;
    }
    break;
  }

  if( m_eUpdateSequence != UPDATE_SEQUENCE_IDLE )
  {
    result = applib::frame::RES_CONTINUE;
  }

  return result;
}


bool RegulationSelectSDCardLoadFramBase::checkRomCode()
{
  bool bResult = ( m_bCheckRomCode ) ? false : true;

  if( m_bCheckRomCode )
  {
    GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
    Savedata::MyStatus* pMyStatus = pGameData->GetPlayerStatus();

    bResult = m_LoadRegulation.CheckParamRom( static_cast<int>( pMyStatus->GetRomCode() ) );
  }

  return bResult;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
