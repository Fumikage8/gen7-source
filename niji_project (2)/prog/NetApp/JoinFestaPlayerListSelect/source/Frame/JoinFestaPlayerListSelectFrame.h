// ============================================================================
/*
 * @file JoinFestaPlayerListSelectFrame.h
 * @brief プレイヤー単体選択アプリの選択プロセスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_FRAME_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaPlayerListSelect/source/Frame/JoinFestaPlayerListSelectFrameBase.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerListSelectFrame :
  public NetApp::JoinFestaPlayerListSelect::Frame::JoinFestaPlayerListSelectFrameBase,
  public NetAppLib::JoinFesta::JoinFestaPersonalListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectFrame );
public:
  JoinFestaPlayerListSelectFrame(
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork,
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor* pJoinFestaPlayerListSelectViewSystemAccessor );
  virtual~JoinFestaPlayerListSelectFrame();

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

  void setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData );

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener        m_JoinFestaPlayerListSelectLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener   m_JoinFestaPlayerListMessageMenuLowerViewListener;


};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECT_FRAME_H_INCLUDE
