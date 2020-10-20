// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptDetailFrame.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)の○○プロセスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPTDETAIL_FRAME_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPTDETAIL_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaPlayerListSelectForScript/source/Frame/JoinFestaPlayerListSelectForScriptFrameBase.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerListSelectForScriptDetailFrame : public NetApp::JoinFestaPlayerListSelectForScript::Frame::JoinFestaPlayerListSelectForScriptFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptDetailFrame );
public:
  JoinFestaPlayerListSelectForScriptDetailFrame(
    NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork,
    NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor* pJoinFestaPlayerListSelectForScriptViewSystemAccessor );
  virtual~JoinFestaPlayerListSelectForScriptDetailFrame();

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
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPTDETAIL_FRAME_H_INCLUDE
