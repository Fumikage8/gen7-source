// ============================================================================
/*
 * @file JoinFestaProfileViewObjects.h
 * @brief プロフィールアプリのViewクラスを持っているクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAPROFILE_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetAppLib)

  GFL_NAMESPACE_BEGIN(UI)
    class NetAppMessageMenu;
  GFL_NAMESPACE_END(UI)

  GFL_NAMESPACE_BEGIN(JoinFestaUI)
    class JoinFestaPlayerListBGUpperView;

    class JoinFestaPlayerListBGLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)

GFL_NAMESPACE_BEGIN(View)
class JoinFestaProfileUpperView;
class JoinFestaProfileLowerView;
class JoinFestaProfileMessageMenuLowerView;
GFL_NAMESPACE_END(View)


GFL_NAMESPACE_BEGIN(System)

class JoinFestaProfileWork;

class JoinFestaProfileViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileViewObjects );
public:
  JoinFestaProfileViewObjects();
  virtual~JoinFestaProfileViewObjects();

  void CreateView(
    NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
    NetAppLib::UI::NetAppMessageMenu&                       netAppMessageMenu );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                   GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                   GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }

  NetApp::JoinFestaProfile::View::JoinFestaProfileUpperView*                GetJoinFestaProfileUpperView() { return m_pJoinFestaProfileUpperView; }
  NetApp::JoinFestaProfile::View::JoinFestaProfileLowerView*                GetJoinFestaProfileLowerView() { return m_pJoinFestaProfileLowerView; }
  NetApp::JoinFestaProfile::View::JoinFestaProfileMessageMenuLowerView*     GetJoinFestaProfileMessageMenuLowerView() { return m_pJoinFestaProfileMessageMenuLowerView; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                   m_pJoinFestaPlayerListBGUpperView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                   m_pJoinFestaPlayerListBGLowerView;

  NetApp::JoinFestaProfile::View::JoinFestaProfileUpperView*                m_pJoinFestaProfileUpperView;
  NetApp::JoinFestaProfile::View::JoinFestaProfileLowerView*                m_pJoinFestaProfileLowerView;
  NetApp::JoinFestaProfile::View::JoinFestaProfileMessageMenuLowerView*     m_pJoinFestaProfileMessageMenuLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPROFILE_VIEWOBJECTS_H_INCLUDE
