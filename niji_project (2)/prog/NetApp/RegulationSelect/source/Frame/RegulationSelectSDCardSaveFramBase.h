// ============================================================================
/*
 * @file RegulationSelectSDCardSaveFramBase.h
 * @brief SDカードにレギュレーションデータを書き込むクラスです。
 * @date 2015.07.01
 */
// ============================================================================
#if !defined( REGULATIONSELECT_SDCARDSAVEFRAMEBASE_H_INCLUDE )
#define REGULATIONSELECT_SDCARDSAVEFRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/RegulationSelect/source/RegulationSelectFrameBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "ExtSavedata/include/RegulationExtSaveData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


class RegulationSelectSDCardSaveFramBase : public NetApp::RegulationSelect::RegulationSelectFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectSDCardSaveFramBase );

public:

  RegulationSelectSDCardSaveFramBase( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork );
  virtual~RegulationSelectSDCardSaveFramBase();

  // NetAppFrameBase
  virtual bool cleanup();

  // CellSuper
  virtual applib::frame::Result UpdateFunc( void );

protected:

  void saveRegulationData( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo );

  enum ErrorID
  {
    ERROR_ID_SLOT_MAX,
    ERROR_ID_NO_SPACE
  };

private:

  virtual void onSaveRegulationDataSuccess( int saveRegulationType ) = 0;
  virtual void onSaveRegulationDataError( ErrorID eErrorID, int useFileSlotCount ) = 0;
  virtual void onSaveRegulationDataFatalError() = 0;

private:
  
  enum UpdateSequence
  {
    UPDATE_SEQUENCE_IDLE,
    UPDATE_SEQUENCE_DELETE_SAME, // @fix NMCat[4266]配信レギュレーションで大会IDが同じデータが、ローカル側で別なデータとして扱われてしまう　の修正
    UPDATE_SEQUENCE_SEARCH_FREE_SLOT,
    UPDATE_SEQUENCE_CREATE_START,
    UPDATE_SEQUENCE_CREATE_WAIT,
    UPDATE_SEQUENCE_NOTIFICATION_SUCCESS,
    UPDATE_SEQUENCE_NOTIFICATION_ERROR,
    UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR
  };

  ExtSavedata::RegulationExtSaveData       m_RegulationExtSaveData;
  UpdateSequence                           m_eUpdateSequence;
  NetAppLib::UI::RegulationDrawInfo*       m_pSaveRegulationDrawInfo;
  int                                      m_SaveRegulationType;
  int                                      m_UseFileSlotCount;
  ErrorID                                  m_eErrorID;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_SDCARDSAVEFRAMEBASE_H_INCLUDE
