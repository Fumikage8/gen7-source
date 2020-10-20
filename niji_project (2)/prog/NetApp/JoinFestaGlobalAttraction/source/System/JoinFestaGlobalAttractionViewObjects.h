// ============================================================================
/*
 * @file JoinFestaGlobalAttractionViewObjects.h
 * @brief グローバルアトラクションアプリのViewクラスを持っているクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTION_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTION_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetAppLib)

  GFL_NAMESPACE_BEGIN(UI)
    class NetAppMessageMenu;
  GFL_NAMESPACE_END(UI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)

GFL_NAMESPACE_BEGIN(View)
class JoinFestaGlobalAttractionBGUpperView;
class JoinFestaGlobalAttractionInfoUpperView;

class JoinFestaGlobalAttractionBGLowerView;
class JoinFestaGlobalAttractionSelectListLowerView;
class JoinFestaGlobalAttractionDetailLowerView;
class JoinFestaGlobalAttractionMessageMenuLowerView;
GFL_NAMESPACE_END(View)


GFL_NAMESPACE_BEGIN(System)

class JoinFestaGlobalAttractionWork;

class JoinFestaGlobalAttractionViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionViewObjects );
public:
  JoinFestaGlobalAttractionViewObjects();
  virtual~JoinFestaGlobalAttractionViewObjects();

  void CreateView(
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork,
    NetAppLib::UI::NetAppMessageMenu&                                         netAppMessageMenu );

  void InputDisableAll();

  bool IsDrawing();

  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionBGUpperView*              GetJoinFestaGlobalAttractionBGUpperView() { return m_pJoinFestaGlobalAttractionBGUpperView; }
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionInfoUpperView*            GetJoinFestaGlobalAttractionInfoUpperView() { return m_pJoinFestaGlobalAttractionInfoUpperView; }

  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionBGLowerView*              GetJoinFestaGlobalAttractionBGLowerView() { return m_pJoinFestaGlobalAttractionBGLowerView; }
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerView*      GetJoinFestaGlobalAttractionSelectListLowerView() { return m_pJoinFestaGlobalAttractionSelectListLowerView; }
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionDetailLowerView*          GetJoinFestaGlobalAttractionDetailLowerView() { return m_pJoinFestaGlobalAttractionDetailLowerView; }
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerView*     GetJoinFestaGlobalAttractionMessageMenuLowerView() { return m_pJoinFestaGlobalAttractionMessageMenuLowerView; }

private:

  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionBGUpperView*              m_pJoinFestaGlobalAttractionBGUpperView;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionInfoUpperView*            m_pJoinFestaGlobalAttractionInfoUpperView;

  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionBGLowerView*              m_pJoinFestaGlobalAttractionBGLowerView;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerView*      m_pJoinFestaGlobalAttractionSelectListLowerView;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionDetailLowerView*          m_pJoinFestaGlobalAttractionDetailLowerView;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerView*     m_pJoinFestaGlobalAttractionMessageMenuLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAGLOBALATTRACTION_VIEWOBJECTS_H_INCLUDE
