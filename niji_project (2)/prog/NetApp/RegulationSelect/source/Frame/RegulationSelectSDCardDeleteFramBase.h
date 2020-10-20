// ============================================================================
/*
 * @file RegulationSelectSDCardDeleteFramBase.h
 * @brief SDカードからレギュレーションデータを削除するクラスです。
 * @date 2015.12.08
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_SDCARDDELETEFRAMEBASE_H_INCLUDE )
#define REGULATIONSELECT_SDCARDDELETEFRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/RegulationSelect/source/RegulationSelectFrameBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "ExtSavedata/include/RegulationExtSaveData.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


class RegulationSelectSDCardDeleteFramBase : public NetApp::RegulationSelect::RegulationSelectFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectSDCardDeleteFramBase );

public:

  RegulationSelectSDCardDeleteFramBase( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork );
  virtual~RegulationSelectSDCardDeleteFramBase();

  // NetAppFrameBase
  virtual bool cleanup();

  // CellSuper
  virtual applib::frame::Result UpdateFunc( void );

protected:

  void deleteRegulationData( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo );

private:

  virtual void onDeleteRegulationDataSuccess() = 0;
  virtual void onDeleteRegulationDataFatalError() = 0;

private:
  
  enum UpdateSequence
  {
    UPDATE_SEQUENCE_IDLE,
    UPDATE_SEQUENCE_DELETE_START,
    UPDATE_SEQUENCE_DELETE_WAIT,
    UPDATE_SEQUENCE_NOTIFICATION_SUCCESS,
    UPDATE_SEQUENCE_NOTIFICATION_FATAL_ERROR
  };

  ExtSavedata::RegulationExtSaveData       m_RegulationExtSaveData;
  UpdateSequence                           m_eUpdateSequence;
  NetAppLib::UI::RegulationDrawInfo*       m_pDeleteRegulationDrawInfo;
  NetAppLib::Util::NetAppTimeout           m_Timer;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_SDCARDDELETEFRAMEBASE_H_INCLUDE
