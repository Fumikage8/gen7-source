// ============================================================================
/*
 * @file RegulationSelectMainFrame.h
 * @brief レギュレーション選択アプリのレギュレーションデータを選択するプロセスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECTMAIN_FRAME_H_INCLUDE )
#define REGULATIONSELECTMAIN_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/RegulationSelect/source/RegulationSelectFrameBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerViewListener.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class RegulationSelectViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(Frame)


class RegulationSelectMainFrame : public NetApp::RegulationSelect::RegulationSelectFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectMainFrame );
public:
  RegulationSelectMainFrame(
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects );
  virtual~RegulationSelectMainFrame();

private:

  // RegulationSelectFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  bool isEntry( NetAppLib::UI::RegulationDrawInfo* pSelectRegulationDrawInfo );

private:

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*       m_pRegulationSelectViewObjects;
  NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener           m_RegulationSelectMainLowerViewListener;
  NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener    m_RegulationSelectMessageMenuLowerViewListener;
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>                                                   m_RegulationList;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTMAIN_FRAME_H_INCLUDE
