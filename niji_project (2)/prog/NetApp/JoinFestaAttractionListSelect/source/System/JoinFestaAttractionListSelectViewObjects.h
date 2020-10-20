// ============================================================================
/*
 * @file JoinFestaAttractionListSelectViewObjects.h
 * @brief アトラクション開催参加アプリのViewクラスを持っているクラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetAppLib)

  GFL_NAMESPACE_BEGIN(UI)
    class NetAppMessageMenu;
  GFL_NAMESPACE_END(UI)

  GFL_NAMESPACE_BEGIN(JoinFestaUI)
    class JoinFestaPlayerListBGUpperView;
    class JoinFestaAttractionInfoUpperView;

    class JoinFestaPlayerListBGLowerView;
    class JoinFestaPlayerListMessageMenuLowerView;
    class JoinFestaAttractionSelectLowerView;
    class JoinFestaAlertLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaAttractionListSelectWork;

class JoinFestaAttractionListSelectViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectViewObjects );
public:
  JoinFestaAttractionListSelectViewObjects();
  virtual~JoinFestaAttractionListSelectViewObjects();

  void CreateView(
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
    NetAppLib::UI::NetAppMessageMenu&                                                 netAppMessageMenu );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaAttractionInfoUpperView*                                         GetJoinFestaAttractionInfoUpperView() { return m_pJoinFestaAttractionInfoUpperView; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  GetJoinFestaPlayerListMessageMenuLowerView() { return m_pJoinFestaPlayerListMessageMenuLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerView*                                       GetJoinFestaAttractionSelectLowerView() { return m_pJoinFestaAttractionSelectLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaAlertLowerView*                                                  GetJoinFestaAlertLowerView() { return m_pJoinFestaAlertLowerView; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           m_pJoinFestaPlayerListBGUpperView;
  NetAppLib::JoinFestaUI::JoinFestaAttractionInfoUpperView*                                         m_pJoinFestaAttractionInfoUpperView;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  m_pJoinFestaPlayerListMessageMenuLowerView;
  NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerView*                                       m_pJoinFestaAttractionSelectLowerView;
  NetAppLib::JoinFestaUI::JoinFestaAlertLowerView*                                                  m_pJoinFestaAlertLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAATTRACTIONLISTSELECT_VIEWOBJECTS_H_INCLUDE
