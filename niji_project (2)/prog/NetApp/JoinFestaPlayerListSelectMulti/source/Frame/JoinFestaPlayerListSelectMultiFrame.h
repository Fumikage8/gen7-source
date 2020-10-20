// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiFrame.h
 * @brief プレイヤー複数選択アプリの選択プロセスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_FRAME_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaPlayerListSelectMulti/source/Frame/JoinFestaPlayerListSelectMultiFrameBase.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerListSelectMultiFrame :
  public NetApp::JoinFestaPlayerListSelectMulti::Frame::JoinFestaPlayerListSelectMultiFrameBase,
  public NetAppLib::JoinFesta::JoinFestaPersonalListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiFrame );
public:
  JoinFestaPlayerListSelectMultiFrame(
    NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork,
    NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor* pJoinFestaPlayerListSelectMultiViewSystemAccessor );
  virtual~JoinFestaPlayerListSelectMultiFrame();

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

  void addMessageMenuView();
  void updateSelectList();
  bool isAllOnline();

  void setupList();

  void setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData );

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener        m_JoinFestaPlayerListSelectLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener   m_JoinFestaPlayerListMessageMenuLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList                            m_SelectOffLineList;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTMULTI_FRAME_H_INCLUDE
