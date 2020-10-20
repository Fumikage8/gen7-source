// ============================================================================
/*
 * @file JoinFestaSelectWordMainFrame.h
 * @brief 挨拶選択アプリの○○プロセスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDMAIN_FRAME_H_INCLUDE )
#define JOINFESTASELECTWORDMAIN_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaSelectWord/source/Frame/JoinFestaSelectWordFrameBase.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordLowerViewListener.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordMessageMenuLowerViewListener.h"
#include "niji_reference_files/script/JoinFestaPawnDefine.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSelectWordMainFrame : public NetApp::JoinFestaSelectWord::Frame::JoinFestaSelectWordFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordMainFrame );
public:
  JoinFestaSelectWordMainFrame(
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor );
  virtual~JoinFestaSelectWordMainFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  void sequenceSelectCategory();
  void sequenceSelectMessage();

  JoinFestaScript::PatternType getPatternType();

  void setupScreen( int sequenceNo );

private:

  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener                m_JoinFestaSelectWordLowerViewListener;
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordMessageMenuLowerViewListener     m_JoinFestaSelectWordMessageMenuLowerViewListener;
  u32                                                                                    m_SelectCategoryMessageID;
  bool                                                                                   m_bSequenceInitFlag;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDMAIN_FRAME_H_INCLUDE
