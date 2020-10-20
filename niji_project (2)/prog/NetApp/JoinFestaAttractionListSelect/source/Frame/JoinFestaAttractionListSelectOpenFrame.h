// ============================================================================
/*
 * @file JoinFestaAttractionListSelectOpenFrame.h
 * @brief アトラクション開催参加アプリの○○プロセスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECTOPEN_FRAME_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECTOPEN_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectFrameBase.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAlertLowerViewListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaAttractionListSelectOpenFrame :
  public NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectOpenFrame );

public:

  JoinFestaAttractionListSelectOpenFrame(
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor );

  virtual~JoinFestaAttractionListSelectOpenFrame();


private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  void defaultProcedure();

private:

  NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener        m_JoinFestaAttractionSelectLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener   m_JoinFestaPlayerListMessageMenuLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaAlertLowerViewListener                   m_JoinFestaAlertLowerViewListener;
  NetAppLib::Util::NetAppCommonSaveUtility                                  m_NetAppCommonSaveUtility;
  NetAppLib::Util::NetAppTimeout                                            m_Timer;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECTOPEN_FRAME_H_INCLUDE
