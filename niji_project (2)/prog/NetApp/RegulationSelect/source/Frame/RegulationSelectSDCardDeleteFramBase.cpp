// ============================================================================
/*
 * @file RegulationSelectSDCardDeleteFramBase.cpp
 * @brief SDカードからレギュレーションデータを削除するクラスです。
 * @date 2015.12.08
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectSDCardDeleteFramBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


RegulationSelectSDCardDeleteFramBase::RegulationSelectSDCardDeleteFramBase( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  NetApp::RegulationSelect::RegulationSelectFrameBase( pRegulationSelectWork ),
  m_RegulationExtSaveData( pRegulationSelectWork->GetDeviceHeap() ),
  m_eUpdateSequence( UPDATE_SEQUENCE_IDLE ),
  m_pDeleteRegulationDrawInfo( NULL ),
  m_Timer()
{
}


RegulationSelectSDCardDeleteFramBase::~RegulationSelectSDCardDeleteFramBase()
{
}


void RegulationSelectSDCardDeleteFramBase::deleteRegulationData( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo )
{
  if( m_eUpdateSequence == UPDATE_SEQUENCE_IDLE )
  {
    m_pDeleteRegulationDrawInfo = pRegulationDrawInfo;

    m_eUpdateSequence = UPDATE_SEQUENCE_DELETE_START;
  }
}


bool RegulationSelectSDCardDeleteFramBase::cleanup()
{
  bool bResult = false;

  bool bIsUpdate = m_RegulationExtSaveData.Update();

  if( !bIsUpdate )
  {
    gfl2::fs::ExtSaveFile::Result result = m_RegulationExtSaveData.GetResult();

    if( ExtSavedata::NijiExtSaveSystem::IsFatalError( result ) )
    {
      onDeleteRegulationDataFatalError();
    }

    bResult = true;
  }

  return bResult;
}


applib::frame::Result RegulationSelectSDCardDeleteFramBase::UpdateFunc( void )
{
  applib::frame::Result result = RegulationSelectFrameBase::UpdateFunc();

  bool bIsUpdate = m_RegulationExtSaveData.Update();
 
  switch( m_eUpdateSequence )
  {
    case UPDATE_SEQUENCE_DELETE_START:
    {
      m_Timer.TimeOutStart();

      int fileIndex = m_pDeleteRegulationDrawInfo->GetFileIndex();

      if( fileIndex > 0 && m_RegulationExtSaveData.StartDeleteRegulationData( static_cast<ExtSavedata::RegulationExtSaveData::REGULATION_TYPE>( fileIndex ) ) )
      {
        m_eUpdateSequence = UPDATE_SEQUENCE_DELETE_WAIT;
      }
      else
      {
        GFL_PRINT( "fileIndex = %d\n", fileIndex );
        GFL_ASSERT(0); // @fix
        // ここに来ることはないけど、来てしまった時に無限ループにならないような対応です
        m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_SUCCESS;
      }
    }
    break;

    case UPDATE_SEQUENCE_DELETE_WAIT:
    {
      if( !bIsUpdate )
      {
        gfl2::fs::ExtSaveFile::Result result = m_RegulationExtSaveData.GetResult();

        m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_SUCCESS;

        if( ExtSavedata::NijiExtSaveSystem::IsFatalError( result ) )
        {
          m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR;
        }
      }
    }
    break;

    case UPDATE_SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( m_Timer.IsTimeOut() )
      {
        onDeleteRegulationDataSuccess();
        m_eUpdateSequence = UPDATE_SEQUENCE_IDLE;
      }
    }
    break;

    case UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR:
    {
      onDeleteRegulationDataFatalError();
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


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
