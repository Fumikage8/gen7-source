// ============================================================================
/*
 * @file JoinFestaPlayerListShowDetailFrame.h
 * @brief 閲覧アプリのSDカード読み込みプロセスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOWDETAIL_FRAME_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOWDETAIL_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowFrameBase.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerListShowDetailFrame : public NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowDetailFrame );
public:
  JoinFestaPlayerListShowDetailFrame(
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor );
  virtual~JoinFestaPlayerListShowDetailFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  void recordSelectEventProcedure();
  void recordDetailEventProcedure();
  void surveyEventProcedure();

private:

  NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerViewListener      m_JoinFestaRecordSelectLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerViewListener      m_JoinFestaRecordDetailLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaSurveyLowerViewListener            m_JoinFestaSurveyLowerViewListener;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOWDETAIL_FRAME_H_INCLUDE
