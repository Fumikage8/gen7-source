// ============================================================================
/*
 * @file JoinFestaSelectWordViewObjects.h
 * @brief 挨拶選択アプリのViewクラスを持っているクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTASELECTWORD_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetAppLib)

  GFL_NAMESPACE_BEGIN(UI)
    class NetAppMessageMenu;
  GFL_NAMESPACE_END(UI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)

GFL_NAMESPACE_BEGIN(View)
class JoinFestaSelectWordBGUpperView;
class JoinFestaSelectWordUpperView;
class JoinFestaSelectWordBGLowerView;
class JoinFestaSelectWordLowerView;
class JoinFestaSelectWordMessageMenuLowerView;
GFL_NAMESPACE_END(View)


GFL_NAMESPACE_BEGIN(System)

class JoinFestaSelectWordWork;

class JoinFestaSelectWordViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordViewObjects );
public:
  JoinFestaSelectWordViewObjects();
  virtual~JoinFestaSelectWordViewObjects();

  void CreateView(
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
    NetAppLib::UI::NetAppMessageMenu&                             netAppMessageMenu );

  void InputDisableAll();

  bool IsDrawing();

  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordBGUpperView*                     GetJoinFestaSelectWordBGUpperView() { return m_pJoinFestaSelectWordBGUpperView; }
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordUpperView*                       GetJoinFestaSelectWordUpperView() { return m_pJoinFestaSelectWordUpperView; }
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordBGLowerView*                     GetJoinFestaSelectWordBGLowerView() { return m_pJoinFestaSelectWordBGLowerView; }
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerView*                       GetJoinFestaSelectWordLowerView() { return m_pJoinFestaSelectWordLowerView; }
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordMessageMenuLowerView*            GetJoinFestaSelectWordMessageMenuLowerView() { return m_pJoinFestaSelectWordMessageMenuLowerView; }

private:

  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordBGUpperView*                     m_pJoinFestaSelectWordBGUpperView;
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordUpperView*                       m_pJoinFestaSelectWordUpperView;
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordBGLowerView*                     m_pJoinFestaSelectWordBGLowerView;
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerView*                       m_pJoinFestaSelectWordLowerView;
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordMessageMenuLowerView*            m_pJoinFestaSelectWordMessageMenuLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTASELECTWORD_VIEWOBJECTS_H_INCLUDE
