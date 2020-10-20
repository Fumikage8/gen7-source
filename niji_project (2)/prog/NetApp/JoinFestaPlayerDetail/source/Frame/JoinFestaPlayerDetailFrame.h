// ============================================================================
/*
 * @file JoinFestaPlayerDetailFrame.h
 * @brief 詳細アプリの○○プロセスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_FRAME_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaPlayerDetail/source/Frame/JoinFestaPlayerDetailFrameBase.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerDetailFrame : public NetApp::JoinFestaPlayerDetail::Frame::JoinFestaPlayerDetailFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailFrame );
public:
  JoinFestaPlayerDetailFrame(
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork,
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor* pJoinFestaPlayerDetailViewSystemAccessor );
  virtual~JoinFestaPlayerDetailFrame();

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
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERDETAIL_FRAME_H_INCLUDE
