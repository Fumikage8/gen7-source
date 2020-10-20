// ============================================================================
/*
 * @file RegulationDLDeleteFrame.h
 * @brief レギュレーション選択アプリのダウンロードしたレギュレーションデータを削除するプロセスです。
 * @date 2015.07.01
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONDLDELETE_FRAME_H_INCLUDE )
#define REGULATIONDLDELETE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectSDCardDeleteFramBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerViewListener.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class RegulationSelectViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(Frame)


class RegulationDLDeleteFrame : public NetApp::RegulationSelect::Frame::RegulationSelectSDCardDeleteFramBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationDLDeleteFrame );
public:
  RegulationDLDeleteFrame(
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects );
  virtual~RegulationDLDeleteFrame();

private:

  // RegulationSelectFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  // RegulationSelectSDCardDeleteFramBase
  virtual void onDeleteRegulationDataSuccess();
  virtual void onDeleteRegulationDataFatalError();

  void deleteRegulation();

private:

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*       m_pRegulationSelectViewObjects;
  NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener           m_RegulationSelectMainLowerViewListener;
  NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener    m_RegulationSelectMessageMenuLowerViewListener;


};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDLDELETE_FRAME_H_INCLUDE
