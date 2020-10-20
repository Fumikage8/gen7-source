// ============================================================================
/*
 * @file RegulationSelectSDCardSaveFramBase.cpp
 * @brief SDカードにレギュレーションデータを書き込むクラスです。
 * @date 2015.07.01
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectSDCardSaveFramBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


RegulationSelectSDCardSaveFramBase::RegulationSelectSDCardSaveFramBase( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  NetApp::RegulationSelect::RegulationSelectFrameBase( pRegulationSelectWork ),
  m_RegulationExtSaveData( pRegulationSelectWork->GetDeviceHeap() ),
  m_eUpdateSequence( UPDATE_SEQUENCE_IDLE ),
  m_pSaveRegulationDrawInfo( NULL ),
  m_SaveRegulationType( ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_00 ),
  m_UseFileSlotCount( 0 ),
  m_eErrorID( ERROR_ID_SLOT_MAX )
{
}


RegulationSelectSDCardSaveFramBase::~RegulationSelectSDCardSaveFramBase()
{
}


void RegulationSelectSDCardSaveFramBase::saveRegulationData( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo )
{
  if( m_eUpdateSequence == UPDATE_SEQUENCE_IDLE )
  {
    m_pSaveRegulationDrawInfo = pRegulationDrawInfo;

    m_SaveRegulationType = ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_00;

    m_UseFileSlotCount = 0;

    m_eUpdateSequence = UPDATE_SEQUENCE_DELETE_SAME;
  }
}


bool RegulationSelectSDCardSaveFramBase::cleanup()
{
  bool bResult = false;

  bool bIsUpdate = m_RegulationExtSaveData.Update();

  if( !bIsUpdate )
  {
    gfl2::fs::ExtSaveFile::Result result = m_RegulationExtSaveData.GetResult();

    if( ExtSavedata::NijiExtSaveSystem::IsFatalError( result ) )
    {
      onSaveRegulationDataFatalError();
    }

    bResult = true;
  }

  return bResult;
}


applib::frame::Result RegulationSelectSDCardSaveFramBase::UpdateFunc( void )
{
  applib::frame::Result result = RegulationSelectFrameBase::UpdateFunc();

  bool bIsUpdate = m_RegulationExtSaveData.Update();
 
  switch( m_eUpdateSequence )
  {
    case UPDATE_SEQUENCE_DELETE_SAME:
    {
      // @fix NMCat[4266]配信レギュレーションで大会IDが同じデータが、ローカル側で別なデータとして扱われてしまう　の修正
      // 対処２：
      //      セーブ前に、同一大会IDのレギュレーションを全て削除する。その後、新規で最新レギュレーションが保存される。
      //

      m_eUpdateSequence = UPDATE_SEQUENCE_SEARCH_FREE_SLOT;

      gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList = m_pRegulationSelectWork->GetDownloadedList();
      gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = pDownloadedList->Begin();

      // SDカードに保存しているリストを全てなめる
      while( it != pDownloadedList->End() )
      {
        // 大会IDが同じ　かつ　バージョンが下のデータを削除する
        if( (*it)->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID) == m_pSaveRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID) &&
            (*it)->GetRegulation().GetVersion() < m_pSaveRegulationDrawInfo->GetRegulation().GetVersion() )
        {
          // SDカードから削除
          // ※result値は無視する。パスが存在しないパターンがエラーになるが、無視してよいエラー。
          //   フェイタルエラーはハンドリングする
          GFL_RELEASE_PRINT("NMCat[4266]:レギュレーション削除 大会ID:%d バージョン:%d \n", (*it)->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID), (*it)->GetRegulation().GetVersion());
          m_RegulationExtSaveData.DeleteRegulationData( static_cast<ExtSavedata::RegulationExtSaveData::REGULATION_TYPE>( (*it)->GetFileIndex() ) );

          if( m_RegulationExtSaveData.IsFatalError() )
          {
            m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR; // FatalErrorシーケンスへ上書き
            break;
          }

          // リストから削除
          NetAppLib::UI::RegulationDrawInfo* pDeleteRegulationDrawInfo = (*it);
          it = pDownloadedList->Erase( it );
          GFL_SAFE_DELETE( pDeleteRegulationDrawInfo );
        }
        else
        {
          ++it;
        }
      }
    }
    break;

    case UPDATE_SEQUENCE_SEARCH_FREE_SLOT:
    {
      if( m_RegulationExtSaveData.IsExistsRegulationData( static_cast<ExtSavedata::RegulationExtSaveData::REGULATION_TYPE>( m_SaveRegulationType ) ) )
      {
        m_UseFileSlotCount++;

        m_SaveRegulationType++;

        if( m_SaveRegulationType > ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_11 )
        {
          m_eErrorID = ERROR_ID_SLOT_MAX;
          m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_ERROR;
        }
      }
      else
      {
        m_eUpdateSequence = UPDATE_SEQUENCE_CREATE_START;
      }
    }
    break;

    case UPDATE_SEQUENCE_CREATE_START:
    {
#if PM_DEBUG
      GFL_PRINT( "====================================================================================\n" );
      GFL_PRINT( "------------------------------------------------------------------------------------\n" );
      GFL_PRINT( "--------------------------------- Save Regulation ----------------------------------\n" );
      GFL_PRINT( "------------------------------------------------------------------------------------\n" );
      GFL_PRINT( "====================================================================================\n" );

      GFL_PRINT( "m_SaveRegulationType = %d\n", m_SaveRegulationType );

      m_pSaveRegulationDrawInfo->GetRegulation().DebugPrint();
#endif // PM_DEBUG

      if( m_RegulationExtSaveData.StartCreateRegulationData( static_cast<ExtSavedata::RegulationExtSaveData::REGULATION_TYPE>( m_SaveRegulationType ), m_pSaveRegulationDrawInfo->GetRegulation() ) )
      {
        m_eUpdateSequence = UPDATE_SEQUENCE_CREATE_WAIT;
      }
      else
      {
        m_SaveRegulationType++;

        if( m_SaveRegulationType > ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_11 )
        {
          m_eErrorID = ERROR_ID_SLOT_MAX;
          m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_ERROR;
        }
      }
    }
    break;

    case UPDATE_SEQUENCE_CREATE_WAIT:
    {
      if( !bIsUpdate )
      {
        gfl2::fs::ExtSaveFile::Result result = m_RegulationExtSaveData.GetResult();

        if( result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
        {
#if PM_DEBUG
          m_pSaveRegulationDrawInfo->GetRegulation().DebugPrint();
#endif // PM_DEBUG

          m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_SUCCESS;
        }
        else
        if( result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE )
        {
          m_eErrorID = ERROR_ID_NO_SPACE;
          m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_ERROR;
        }
        else
        {
          if( result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST )
          {
            m_UseFileSlotCount++;
          }

          m_SaveRegulationType++;

          if( m_SaveRegulationType > ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_RESERVED_11 )
          {
            m_eErrorID = ERROR_ID_SLOT_MAX;
            m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_ERROR;
          }
        }

        if( ExtSavedata::NijiExtSaveSystem::IsFatalError( result ) )
        {
          m_eUpdateSequence = UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR;
        }

      }
    }
    break;

    case UPDATE_SEQUENCE_NOTIFICATION_SUCCESS:
    {
      onSaveRegulationDataSuccess( m_SaveRegulationType );
      m_eUpdateSequence = UPDATE_SEQUENCE_IDLE;
    }
    break;

    case UPDATE_SEQUENCE_NOTIFICATION_ERROR:
    {
      onSaveRegulationDataError( m_eErrorID, m_UseFileSlotCount );
      m_eUpdateSequence = UPDATE_SEQUENCE_IDLE;
    }
    break;

    case UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR:
    {
      onSaveRegulationDataFatalError();
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
