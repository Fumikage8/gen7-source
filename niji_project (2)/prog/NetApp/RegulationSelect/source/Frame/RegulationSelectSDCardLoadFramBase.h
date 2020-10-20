// ============================================================================
/*
 * @file RegulationSelectSDCardLoadFramBase.h
 * @brief SDカードからレギュレーションデータを読み込むクラスです。
 * @date 2015.06.19
 */
// ============================================================================
#if !defined( REGULATIONSELECT_SDCARDLOADFRAMEBASE_H_INCLUDE )
#define REGULATIONSELECT_SDCARDLOADFRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include "NetApp/RegulationSelect/source/RegulationSelectFrameBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "ExtSavedata/include/RegulationExtSaveData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


class RegulationSelectSDCardLoadFramBase : public NetApp::RegulationSelect::RegulationSelectFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectSDCardLoadFramBase );

public:

  RegulationSelectSDCardLoadFramBase( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork );
  virtual~RegulationSelectSDCardLoadFramBase();

  // NetAppFrameBase
  virtual bool cleanup();

  // CellSuper
  virtual applib::frame::Result UpdateFunc( void );

protected:

  void loadRegulationData( bool bCheckRomCode );

private:

  virtual void onLoadRegulationDataSuccess() = 0;
  virtual void onLoadRegulationDataError() = 0;
  virtual void onLoadRegulationDataFatalError() = 0;

  bool checkRomCode();

private:
  
  enum UpdateSequence
  {
    UPDATE_SEQUENCE_IDLE,
    UPDATE_SEQUENCE_CLEAR_DOWNLOADED_LIST,
    UPDATE_SEQUENCE_LOAD_START,
    UPDATE_SEQUENCE_LOAD_WAIT,
    UPDATE_SEQUENCE_BROKEN_FILE_DELETE_START,
    UPDATE_SEQUENCE_BROKEN_FILE_DELETE_WAIT,
    UPDATE_SEQUENCE_NOTIFICATION_SUCCESS,
    UPDATE_SEQUENCE_NOTIFICATION_ERROR,
    UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR
  };


  gfl2::util::List<int>                    m_BrokenFileList;
  ExtSavedata::RegulationExtSaveData       m_RegulationExtSaveData;
  UpdateSequence                           m_eUpdateSequence;
  Regulation                               m_LoadRegulation;
  int                                      m_LoadRegulationType;
  bool                                     m_bCheckRomCode;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_SDCARDLOADFRAMEBASE_H_INCLUDE
