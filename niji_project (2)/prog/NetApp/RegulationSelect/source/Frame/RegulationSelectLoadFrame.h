﻿// ============================================================================
/*
 * @file RegulationSelectLoadFrame.h
 * @brief レギュレーション選択アプリのSDカードからレギュレーションデータをロードするプロセスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECTLOAD_FRAME_H_INCLUDE )
#define REGULATIONSELECTLOAD_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectSDCardLoadFramBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class RegulationSelectViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(Frame)


class RegulationSelectLoadFrame : public NetApp::RegulationSelect::Frame::RegulationSelectSDCardLoadFramBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectLoadFrame );
public:
  RegulationSelectLoadFrame(
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects );
  virtual~RegulationSelectLoadFrame();

private:

  // RegulationSelectFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  // RegulationSelectSDCardLoadFramBase
  virtual void onLoadRegulationDataSuccess();
  virtual void onLoadRegulationDataError();
  virtual void onLoadRegulationDataFatalError();

private:

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*     m_pRegulationSelectViewObjects;
  NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener  m_RegulationSelectMessageMenuLowerViewListener;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTLOAD_FRAME_H_INCLUDE
