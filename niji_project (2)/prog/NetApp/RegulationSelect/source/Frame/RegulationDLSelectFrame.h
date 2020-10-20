// ============================================================================
/*
 * @file RegulationDLSelectFrame.h
 * @brief レギュレーション選択アプリのダウンロードしたレギュレーションデータを選択するプロセスです。
 * @date 2015.07.01
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONDLSELECT_FRAME_H_INCLUDE )
#define REGULATIONDLSELECT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectSDCardSaveFramBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerViewListener.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class RegulationSelectViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(Frame)


class RegulationDLSelectFrame : public NetApp::RegulationSelect::Frame::RegulationSelectSDCardSaveFramBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationDLSelectFrame );
public:
  RegulationDLSelectFrame(
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects );
  virtual~RegulationDLSelectFrame();

private:

  // RegulationSelectFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  // RegulationSelectSDCardSaveFramBase
  virtual void onSaveRegulationDataSuccess( int saveRegulationType );
  virtual void onSaveRegulationDataError( ErrorID eErrorID, int useFileSlotCoun );
  virtual void onSaveRegulationDataFatalError();

  bool isDownloaded( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo );

private:

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*       m_pRegulationSelectViewObjects;
  NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener           m_RegulationSelectMainLowerViewListener;
  NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener    m_RegulationSelectMessageMenuLowerViewListener;


};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDLSELECT_FRAME_H_INCLUDE
