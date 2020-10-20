// ============================================================================
/*
 * @file RegulationDLMainFrame.h
 * @brief レギュレーション選択アプリのレギュレーションデータをダウンロードするプロセスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONDLMAIN_FRAME_H_INCLUDE )
#define REGULATIONDLMAIN_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestHandle.h>
#include "NetApp/RegulationSelect/source/RegulationSelectFrameBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerViewListener.h"
#include "NetApp/RegulationSelect/source/RequestSequence/RegulationDLSearchDataResponseListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class RegulationSelectViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(Frame)


class RegulationDLMainFrame :
  public NetApp::RegulationSelect::RegulationSelectFrameBase,
  public NetApp::RegulationSelect::RequestSequence::RegulationDLSearchDataResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationDLMainFrame );
public:
  RegulationDLMainFrame(
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects );
  virtual~RegulationDLMainFrame();

private:

  // RegulationSelectFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();
public:

  // RegulationDLSearchDataResponseListener
  virtual void OnSearchDataSuccess();
  virtual void OnSearchDataError();
  virtual void OnSearchDataError2();
  virtual void OnSearchDataCancel();

private:

  enum UpdateSequence
  {
    UPDATE_SEQUENCE_DATA_DOWNLOAD,
    UPDATE_SEQUENCE_NOT_FOUND_DATA,
    UPDATE_SEQUENCE_CANCEL_CONFIRM,
    UPDATE_SEQUENCE_CANCEL_WAIT
  };

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*       m_pRegulationSelectViewObjects;
  NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener    m_RegulationSelectMessageMenuLowerViewListener;
  gflnet2::ServerClient::RequestHandle                                            m_RequestHandle;
  bool                                                                            m_bRecvSearchDataSuccess;
  bool                                                                            m_bRecvSearchDataError;
  bool                                                                            m_bRecvSearchDataError2;
  bool                                                                            m_bRecvSearchDataCancel;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDLMAIN_FRAME_H_INCLUDE
