// ============================================================================
/*
 * @file JoinFestaPlayerListShowViewObjects.h
 * @brief 閲覧アプリのViewクラスを持っているクラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_VIEWOBJECTS_H_INCLUDE
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
    class JoinFestaRecordSelectLowerView;
    class JoinFestaRecordDetailLowerView;
    class JoinFestaSurveyLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListShowWork;

class JoinFestaPlayerListShowViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowViewObjects );
public:
  JoinFestaPlayerListShowViewObjects();
  virtual~JoinFestaPlayerListShowViewObjects();

  void CreateView(
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
    NetAppLib::UI::NetAppMessageMenu&                                     netAppMessageMenu,
    NetAppLib::Util::NetAppTrainerIconUtility&                            netAppTrainerIconUtility );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*                                   GetJoinFestaPlayerListPlayerInfoUpperView() { return m_pJoinFestaPlayerListPlayerInfoUpperView; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase*                                   GetJoinFestaPlayerListSelectLowerViewBase() { return m_pJoinFestaPlayerListSelectLowerViewBase; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  GetJoinFestaPlayerListMessageMenuLowerView() { return m_pJoinFestaPlayerListMessageMenuLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView*                                           GetJoinFestaRecordSelectLowerView() { return m_pJoinFestaRecordSelectLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView*                                           GetJoinFestaRecordDetailLowerView() { return m_pJoinFestaRecordDetailLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView*                                                 GetJoinFestaSurveyLowerView() { return m_pJoinFestaSurveyLowerView; }


private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           m_pJoinFestaPlayerListBGUpperView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*                                   m_pJoinFestaPlayerListPlayerInfoUpperView;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase*                                   m_pJoinFestaPlayerListSelectLowerViewBase;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  m_pJoinFestaPlayerListMessageMenuLowerView;
  NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView*                                           m_pJoinFestaRecordSelectLowerView;
  NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView*                                           m_pJoinFestaRecordDetailLowerView;
  NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView*                                                 m_pJoinFestaSurveyLowerView;


};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSHOW_VIEWOBJECTS_H_INCLUDE
