// ============================================================================
/*
 * @file JoinFestaAttractionResultViewObjects.h
 * @brief アトラクション結果アプリのViewクラスを持っているクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetAppLib)

  GFL_NAMESPACE_BEGIN(UI)
    class NetAppMessageMenu;
  GFL_NAMESPACE_END(UI)

  GFL_NAMESPACE_BEGIN(JoinFestaUI)
    class JoinFestaPlayerListBGUpperView;

    class JoinFestaPlayerListBGLowerView;
    class JoinFestaPlayerListMessageMenuLowerView;
    class JoinFestaRankingLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)

GFL_NAMESPACE_BEGIN(View)
class JoinFestaAttractionResultUpperView;
GFL_NAMESPACE_END(View)


GFL_NAMESPACE_BEGIN(System)

class JoinFestaAttractionResultWork;

class JoinFestaAttractionResultViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultViewObjects );
public:
  JoinFestaAttractionResultViewObjects();
  virtual~JoinFestaAttractionResultViewObjects();

  void CreateView(
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork,
    NetAppLib::UI::NetAppMessageMenu&                                         netAppMessageMenu );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetApp::JoinFestaAttractionResult::View::JoinFestaAttractionResultUpperView*                      GetJoinFestaAttractionResultUpperView() { return m_pJoinFestaAttractionResultUpperView; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  GetJoinFestaPlayerListMessageMenuLowerView() { return m_pJoinFestaPlayerListMessageMenuLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaRankingLowerView*                                                GetJoinFestaRankingLowerView() { return m_pJoinFestaRankingLowerView; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           m_pJoinFestaPlayerListBGUpperView;
  NetApp::JoinFestaAttractionResult::View::JoinFestaAttractionResultUpperView*                      m_pJoinFestaAttractionResultUpperView;
  
  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  m_pJoinFestaPlayerListMessageMenuLowerView;
  NetAppLib::JoinFestaUI::JoinFestaRankingLowerView*                                                m_pJoinFestaRankingLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAATTRACTIONRESULT_VIEWOBJECTS_H_INCLUDE
