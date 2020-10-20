// ============================================================================
/*
 * @file JoinFestaOpenListViewObjects.h
 * @brief 開催選択アプリのViewクラスを持っているクラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAOPENLIST_VIEWOBJECTS_H_INCLUDE
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
    class JoinFestaAttractionInfoUpperView;

    class JoinFestaPlayerListBGLowerView;
    class JoinFestaPlayerListMessageMenuLowerView;
    class JoinFestaAttractionSelectLowerView;
    class JoinFestaAlertLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaOpenListWork;

class JoinFestaOpenListViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListViewObjects );
public:
  JoinFestaOpenListViewObjects();
  virtual~JoinFestaOpenListViewObjects();

  void CreateView(
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork*   pJoinFestaOpenListWork,
    NetAppLib::UI::NetAppMessageMenu&                           netAppMessageMenu,
    NetAppLib::Util::NetAppTrainerIconUtility&                  netAppTrainerIconUtility );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                   GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*           GetJoinFestaPlayerListPlayerInfoUpperView() { return m_pJoinFestaPlayerListPlayerInfoUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaAttractionInfoUpperView*                 GetJoinFestaAttractionInfoUpperView() { return m_pJoinFestaAttractionInfoUpperView; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                   GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*          GetJoinFestaPlayerListMessageMenuLowerView() { return m_pJoinFestaPlayerListMessageMenuLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerView*               GetJoinFestaAttractionSelectLowerView() { return m_pJoinFestaAttractionSelectLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaAlertLowerView*                          GetJoinFestaAlertLowerView() { return m_pJoinFestaAlertLowerView; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                   m_pJoinFestaPlayerListBGUpperView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*           m_pJoinFestaPlayerListPlayerInfoUpperView;
  NetAppLib::JoinFestaUI::JoinFestaAttractionInfoUpperView*                 m_pJoinFestaAttractionInfoUpperView;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                   m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*          m_pJoinFestaPlayerListMessageMenuLowerView;
  NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerView*               m_pJoinFestaAttractionSelectLowerView;
  NetAppLib::JoinFestaUI::JoinFestaAlertLowerView*                          m_pJoinFestaAlertLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAOPENLIST_VIEWOBJECTS_H_INCLUDE
