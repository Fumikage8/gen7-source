// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptViewObjects.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のViewクラスを持っているクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_VIEWOBJECTS_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

#include  <Print/include/WordSetLoader.h>
#include  <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListForBattleFesUpperView.h>
#include  "../View/JoinFestaPlayerListForBattleFesParameterChart.h"



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
    class JoinFestaPlayerListForBattleFesUpperView;

    class JoinFestaPlayerListBGLowerView;
    class JoinFestaPlayerListSelectLowerViewBase;
    class JoinFestaPlayerListMessageMenuLowerView;
    class JoinFestaRecordSelectLowerView;
    class JoinFestaRecordDetailLowerView;
    class JoinFestaSurveyLowerView;
  GFL_NAMESPACE_END(JoinFestaUI)
  
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectForScriptWork;

class JoinFestaPlayerListSelectForScriptViewObjects
  : protected NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView::IParameterChartDelegateObject
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptViewObjects );
public:
  JoinFestaPlayerListSelectForScriptViewObjects();
  virtual~JoinFestaPlayerListSelectForScriptViewObjects();

  void CreateView(
    NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork,
    NetAppLib::UI::NetAppMessageMenu&                                                           netAppMessageMenu,
    NetAppLib::Util::NetAppTrainerIconUtility&                                                  netAppTrainerIconUtility );

  void InputDisableAll();

  bool IsDrawing();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*                                           GetJoinFestaPlayerListBGUpperView() { return m_pJoinFestaPlayerListBGUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*                                   GetJoinFestaPlayerListPlayerInfoUpperView() { return m_pJoinFestaPlayerListPlayerInfoUpperView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView*                                 GetJoinFestaPlayerListForBattleFesUpperView() { return m_pJoinFestaPlayerListForBattleFesUpperView; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*                                           GetJoinFestaPlayerListBGLowerView() { return m_pJoinFestaPlayerListBGLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase*                                   GetJoinFestaPlayerListSelectLowerViewBase() { return m_pJoinFestaPlayerListSelectLowerViewBase; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*                                  GetJoinFestaPlayerListMessageMenuLowerView() { return m_pJoinFestaPlayerListMessageMenuLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView*                                           GetJoinFestaRecordSelectLowerView() { return m_pJoinFestaRecordSelectLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView*                                           GetJoinFestaRecordDetailLowerView() { return m_pJoinFestaRecordDetailLowerView; }
  NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView*                                                 GetJoinFestaSurveyLowerView() { return m_pJoinFestaSurveyLowerView; }

  /*    */
  bool  BattleFesUpperView_RadarChartInitFunc(void);
  bool  BattleFesUpperView_RadarChartEndFunc(void);
  bool  BattleFesUpperView_ObserveWordsetPreload(void);

private:
  /*
    implemente NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView::IParameterChartDelegateObject

  */
  virtual void  ParameterChart_OnCreate(const NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView::IParameterChartDelegateObject::CreateParam&  rParam);
  virtual void  ParameterChart_OnRelease(void);

  virtual void  ParameterChart_OnUpdate(void);
  virtual void  ParameterChart_OnSetPokepara(const pml::pokepara::CoreParam* pCoreParam);
  virtual void  ParameterChart_OnSetVisibility(const bool isVisible);

  virtual bool  ParameterChart_OnInitFunc(void);
  virtual bool  ParameterChart_OnEndFunc(void);


  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView*           m_pJoinFestaPlayerListBGUpperView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView*   m_pJoinFestaPlayerListPlayerInfoUpperView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView* m_pJoinFestaPlayerListForBattleFesUpperView;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView*           m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase*   m_pJoinFestaPlayerListSelectLowerViewBase;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*  m_pJoinFestaPlayerListMessageMenuLowerView;
  NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView*           m_pJoinFestaRecordSelectLowerView;
  NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView*           m_pJoinFestaRecordDetailLowerView;
  NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView*                 m_pJoinFestaSurveyLowerView;

  ParameterChart*                                                   m_pParameterChart;
  WordSetLoader*                                                    m_pWordSetLoader_ForBFCUpper;   /**<  BFC上画面用  */
};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPT_VIEWOBJECTS_H_INCLUDE
