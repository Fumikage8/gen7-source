// ============================================================================
/*
 * @file JoinFestaAttractionListSelectEntryFrame.h
 * @brief アトラクション開催参加アプリの○○プロセスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECTENTRY_FRAME_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECTENTRY_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectFrameBase.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaAttractionListSelectEntryFrame :
  public NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectFrameBase,
  public NetAppLib::JoinFesta::JoinFestaPersonalListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectEntryFrame );
public:
  JoinFestaAttractionListSelectEntryFrame(
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor );
  virtual~JoinFestaAttractionListSelectEntryFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();
  virtual void OnWLANSwitchDisable();

  // JoinFestaPersonalListener
  virtual void OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );
  virtual void OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate );
  virtual void OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

  void defaultProcedure();

private:

  NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener        m_JoinFestaAttractionSelectLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener   m_JoinFestaPlayerListMessageMenuLowerViewListener;
  NetAppLib::Util::NetAppTimeout                                            m_Timer;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECTENTRY_FRAME_H_INCLUDE
