// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiViewObjects.h
 * @brief プレイヤー複数選択アプリのViewクラスを持っているクラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>



GFL_NAMESPACE_BEGIN(NetAppLib)

  GFL_NAMESPACE_BEGIN(UI)
    class NetAppMessageMenu;
  GFL_NAMESPACE_END(UI)

  GFL_NAMESPACE_BEGIN(Util)
    class NetAppTrainerIconUtility;
  GFL_NAMESPACE_END(Util)

  GFL_NAMESPACE_BEGIN(JoinFestaUI)
    class JoinFestaPlayerListBGUpperView;
    class JoinFestaPlayerListPlayerInfoUpperView;

    class JoinFestaPlayerListBGLowerView;
    class JoinFestaPlayerListMessageMenuLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)

GFL_NAMESPACE_BEGIN(View)
class JoinFestaPlayerListSelectMultiLowerView;
GFL_NAMESPACE_END(View)


GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectMultiWork;

class JoinFestaPlayerListSelectMultiViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiViewObjects );
public:
  JoinFestaPlayerListSelectMultiViewObjects();
  virtual~JoinFestaPlayerListSelectMultiViewObjects();

  void RemoveFromSuperViewAll();

  void CreateView(
    NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork,
    NetAppLib::UI::NetAppMessageMenu&                                                   netAppMessageMenu,
    NetAppLib::Util::NetAppTrainerIconUtility&                                          netAppTrainerIconUtility );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*                                   GetJoinFestaPlayerListPlayerInfoUpperView() { return m_pJoinFestaPlayerListPlayerInfoUpperView; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  GetJoinFestaPlayerListMessageMenuLowerView() { return m_pJoinFestaPlayerListMessageMenuLowerView; }

  NetApp::JoinFestaPlayerListSelectMulti::View::JoinFestaPlayerListSelectMultiLowerView*            GetJoinFestaPlayerListSelectMultiLowerView() { return m_pJoinFestaPlayerListSelectMultiLowerView; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           m_pJoinFestaPlayerListBGUpperView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*                                   m_pJoinFestaPlayerListPlayerInfoUpperView;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  m_pJoinFestaPlayerListMessageMenuLowerView;

  NetApp::JoinFestaPlayerListSelectMulti::View::JoinFestaPlayerListSelectMultiLowerView*            m_pJoinFestaPlayerListSelectMultiLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECTMULTI_VIEWOBJECTS_H_INCLUDE
