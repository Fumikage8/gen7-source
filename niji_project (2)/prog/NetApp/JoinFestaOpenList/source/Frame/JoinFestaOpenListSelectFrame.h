// ============================================================================
/*
 * @file JoinFestaOpenListSelectFrame.h
 * @brief 開催選択アプリの○○プロセスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLISTSELECT_FRAME_H_INCLUDE )
#define JOINFESTAOPENLISTSELECT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaOpenList/source/Frame/JoinFestaOpenListFrameBase.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAlertLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaOpenListSelectFrame :
  public NetApp::JoinFestaOpenList::Frame::JoinFestaOpenListFrameBase,
  public NetAppLib::JoinFesta::JoinFestaPersonalListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListSelectFrame );

public:

  JoinFestaOpenListSelectFrame(
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork,
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor* pJoinFestaOpenListViewSystemAccessor );
  virtual~JoinFestaOpenListSelectFrame();

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

  void setupList();

  void attractionSelectEventProcedure();
  void messageEventProcedure();
  void alertEventProcedure();

private:

  NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener        m_JoinFestaAttractionSelectLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener   m_JoinFestaPlayerListMessageMenuLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaAlertLowerViewListener                   m_JoinFestaAlertLowerViewListener;
  NetAppLib::Util::NetAppTimeout                                            m_Timer;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLISTSELECT_FRAME_H_INCLUDE
