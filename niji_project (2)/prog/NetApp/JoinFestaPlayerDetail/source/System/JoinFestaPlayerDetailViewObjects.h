// ============================================================================
/*
 * @file JoinFestaPlayerDetailViewObjects.h
 * @brief 詳細アプリのViewクラスを持っているクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetAppLib)

  GFL_NAMESPACE_BEGIN(Util)
    class NetAppTrainerIconUtility;
  GFL_NAMESPACE_END(Util)

  GFL_NAMESPACE_BEGIN(JoinFestaUI)
    class JoinFestaPlayerListBGUpperView;
    class JoinFestaPlayerListPlayerInfoUpperView;

    class JoinFestaPlayerListBGLowerView;
    class JoinFestaRecordSelectLowerView;
    class JoinFestaRecordDetailLowerView;
    class JoinFestaSurveyLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerDetailWork;

class JoinFestaPlayerDetailViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailViewObjects );
public:
  JoinFestaPlayerDetailViewObjects();
  virtual~JoinFestaPlayerDetailViewObjects();

  void CreateView(
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork,
    NetAppLib::Util::NetAppTrainerIconUtility&                        netAppTrainerIconUtility );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*                                   GetJoinFestaPlayerListPlayerInfoUpperView() { return m_pJoinFestaPlayerListPlayerInfoUpperView; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView*                                           GetJoinFestaRecordSelectLowerView() { return m_pJoinFestaRecordSelectLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView*                                           GetJoinFestaRecordDetailLowerView() { return m_pJoinFestaRecordDetailLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView*                                                 GetJoinFestaSurveyLowerView() { return m_pJoinFestaSurveyLowerView; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           m_pJoinFestaPlayerListBGUpperView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*                                   m_pJoinFestaPlayerListPlayerInfoUpperView;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView*                                           m_pJoinFestaRecordSelectLowerView;
  NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView*                                           m_pJoinFestaRecordDetailLowerView;
  NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView*                                                 m_pJoinFestaSurveyLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERDETAIL_VIEWOBJECTS_H_INCLUDE
