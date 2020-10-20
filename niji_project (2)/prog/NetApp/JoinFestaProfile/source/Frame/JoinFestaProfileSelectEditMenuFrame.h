// ============================================================================
/*
 * @file JoinFestaProfileSelectEditMenuFrame.h
 * @brief 編集メニューを選択するプロセスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILESELECTEDITMENU_FRAME_H_INCLUDE )
#define JOINFESTAPROFILESELECTEDITMENU_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileFrameBase.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileLowerViewListener.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaProfileSelectEditMenuFrame : public NetApp::JoinFestaProfile::Frame::JoinFestaProfileFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileSelectEditMenuFrame );
public:
  JoinFestaProfileSelectEditMenuFrame(
    NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
    NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor );
  virtual~JoinFestaProfileSelectEditMenuFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  void onSelectYes( NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu );
  void onSelectNo( NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu );

private:

  NetApp::JoinFestaProfile::View::JoinFestaProfileLowerViewListener                  m_JoinFestaProfileLowerViewListener;
  NetApp::JoinFestaProfile::View::JoinFestaProfileMessageMenuLowerViewListener       m_JoinFestaProfileMessageMenuLowerViewListener;


};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILESELECTEDITMENU_FRAME_H_INCLUDE
