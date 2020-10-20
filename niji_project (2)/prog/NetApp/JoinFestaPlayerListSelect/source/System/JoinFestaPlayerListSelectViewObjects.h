// ============================================================================
/*
 * @file JoinFestaPlayerListSelectViewObjects.h
 * @brief プレイヤー単体選択アプリのViewクラスを持っているクラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_VIEWOBJECTS_H_INCLUDE
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
    class JoinFestaPlayerListSelectLowerViewBase;
    class JoinFestaPlayerListMessageMenuLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectWork;

class JoinFestaPlayerListSelectViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectViewObjects );
public:
  JoinFestaPlayerListSelectViewObjects();
  virtual~JoinFestaPlayerListSelectViewObjects();

  void RemoveFromSuperViewAll();

  void CreateView(
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork,
    NetAppLib::UI::NetAppMessageMenu&                                         netAppMessageMenu,
    NetAppLib::Util::NetAppTrainerIconUtility&                                netAppTrainerIconUtility );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                   GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*           GetJoinFestaPlayerListPlayerInfoUpperView() { return m_pJoinFestaPlayerListPlayerInfoUpperView; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                   GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase*           GetJoinFestaPlayerListSelectLowerViewBase() { return m_pJoinFestaPlayerListSelectLowerViewBase; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*          GetJoinFestaPlayerListMessageMenuLowerView() { return m_pJoinFestaPlayerListMessageMenuLowerView; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                   m_pJoinFestaPlayerListBGUpperView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*           m_pJoinFestaPlayerListPlayerInfoUpperView;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                   m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase*           m_pJoinFestaPlayerListSelectLowerViewBase;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*          m_pJoinFestaPlayerListMessageMenuLowerView;


};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECT_VIEWOBJECTS_H_INCLUDE
