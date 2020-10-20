// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptViewObjects.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のViewクラスを持っているクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptViewObjects.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptResourceID.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptWork.h"


#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListForBattleFesUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerView.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectForScriptViewObjects::JoinFestaPlayerListSelectForScriptViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaPlayerListPlayerInfoUpperView( NULL ),
  m_pJoinFestaPlayerListForBattleFesUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaPlayerListSelectLowerViewBase( NULL ),
  m_pJoinFestaPlayerListMessageMenuLowerView( NULL ),
  m_pJoinFestaRecordSelectLowerView( NULL ),
  m_pJoinFestaRecordDetailLowerView( NULL ),
  m_pJoinFestaSurveyLowerView( NULL ),
  m_pParameterChart( NULL ),
  m_pWordSetLoader_ForBFCUpper(NULL)
{
}


JoinFestaPlayerListSelectForScriptViewObjects::~JoinFestaPlayerListSelectForScriptViewObjects()
{
  GFL_SAFE_DELETE( m_pWordSetLoader_ForBFCUpper );
  GFL_SAFE_DELETE( m_pJoinFestaSurveyLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaRecordDetailLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaRecordSelectLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectLowerViewBase );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListForBattleFesUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListPlayerInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaPlayerListSelectForScriptViewObjects::CreateView(
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork,
  NetAppLib::UI::NetAppMessageMenu&                                                           netAppMessageMenu,
  NetAppLib::Util::NetAppTrainerIconUtility&                                                  netAppTrainerIconUtility )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerListSelectForScriptWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListPlayerInfoUpperView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView( pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT, netAppTrainerIconUtility );
  m_pJoinFestaPlayerListForBattleFesUpperView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView( pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT, netAppTrainerIconUtility );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListSelectLowerViewBase   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase( pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaPlayerListMessageMenuLowerView  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView( pJoinFestaPlayerListSelectForScriptWork, netAppMessageMenu );
  m_pJoinFestaRecordSelectLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView( pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaRecordDetailLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView( pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaSurveyLowerView                 = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView( pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MENU_CURSOR );

  m_pJoinFestaPlayerListForBattleFesUpperView->SetParameterChartDelegateObject(this);
  m_pJoinFestaPlayerListForBattleFesUpperView->SetCommonGrpIcon(pJoinFestaPlayerListSelectForScriptWork->GetCommonGrpIconData());

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetModal();
  m_pJoinFestaPlayerListForBattleFesUpperView->SetModal();

  m_pJoinFestaPlayerListBGLowerView->SetModal();
  m_pJoinFestaPlayerListSelectLowerViewBase->SetModal();
  m_pJoinFestaPlayerListMessageMenuLowerView->SetModal();
  m_pJoinFestaRecordSelectLowerView->SetModal();
  m_pJoinFestaRecordDetailLowerView->SetModal();
  m_pJoinFestaSurveyLowerView->SetModal();

  /*  wordsetloader  */
  {
    const u32 targetBits  = 0
                          | PRELOAD_ITEMNAME
                          | PRELOAD_WAZANAME
                          | PRELOAD_TYPENAME
                          | PRELOAD_TOKUSEI
                          | PRELOAD_SEIKAKU
                          ;

    gfl2::heap::HeapBase* pHeapBase = pAppHeap->GetDeviceHeap();
    m_pWordSetLoader_ForBFCUpper  = GFL_NEW(pHeapBase)  WordSetLoader(pHeapBase, targetBits);
    m_pWordSetLoader_ForBFCUpper->StartLoad(pHeapBase->GetLowerHandle());

    m_pJoinFestaPlayerListForBattleFesUpperView->GetG2DUtil()->SetWordSetLoader(m_pWordSetLoader_ForBFCUpper);
  }
}


void JoinFestaPlayerListSelectForScriptViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetInputEnabled( false );
  m_pJoinFestaPlayerListForBattleFesUpperView->SetInputEnabled( false );

  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );
  m_pJoinFestaPlayerListSelectLowerViewBase->SetInputEnabled( false );
  m_pJoinFestaPlayerListMessageMenuLowerView->SetInputEnabled( false );
  m_pJoinFestaRecordSelectLowerView->SetInputEnabled( false );
  m_pJoinFestaRecordDetailLowerView->SetInputEnabled( false );
  m_pJoinFestaSurveyLowerView->SetInputEnabled( false );
}


bool JoinFestaPlayerListSelectForScriptViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaPlayerListPlayerInfoUpperView->IsDrawing()
   || m_pJoinFestaPlayerListForBattleFesUpperView->IsDrawing()
   || (!m_pJoinFestaPlayerListForBattleFesUpperView->ParameterChart_EndFunc())
   || m_pJoinFestaPlayerListMessageMenuLowerView->IsDrawing()
   || m_pJoinFestaPlayerListBGLowerView->IsDrawing()
   || m_pJoinFestaPlayerListSelectLowerViewBase->IsDrawing()
   || m_pJoinFestaPlayerListMessageMenuLowerView->IsDrawing()
   || m_pJoinFestaRecordSelectLowerView->IsDrawing()
   || m_pJoinFestaRecordDetailLowerView->IsDrawing()
   || m_pJoinFestaSurveyLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}

bool  JoinFestaPlayerListSelectForScriptViewObjects::BattleFesUpperView_RadarChartInitFunc(void)
{
  return m_pJoinFestaPlayerListForBattleFesUpperView->ParameterChart_InitFunc();
}

bool  JoinFestaPlayerListSelectForScriptViewObjects::BattleFesUpperView_RadarChartEndFunc(void)
{
  return m_pJoinFestaPlayerListForBattleFesUpperView->ParameterChart_EndFunc();
}

bool  JoinFestaPlayerListSelectForScriptViewObjects::BattleFesUpperView_ObserveWordsetPreload(void)
{
  return m_pWordSetLoader_ForBFCUpper->WaitLoad();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListSelectForScriptViewObjects::ParameterChart_OnCreate(const NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView::IParameterChartDelegateObject::CreateParam&  rParam)
{
  gfl2::heap::HeapBase* pHeapBase = rParam.heap->GetDeviceHeap();

  m_pParameterChart = GFL_NEW(pHeapBase) ParameterChart(rParam);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListSelectForScriptViewObjects::ParameterChart_OnRelease(void)
{
  GFL_SAFE_DELETE(m_pParameterChart);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListSelectForScriptViewObjects::ParameterChart_OnUpdate(void)
{
  if(m_pParameterChart)
  {
    m_pParameterChart->Update();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListSelectForScriptViewObjects::ParameterChart_OnSetPokepara(const pml::pokepara::CoreParam* pCoreParam)
{
  if(m_pParameterChart)
  {
    m_pParameterChart->Setup(pCoreParam);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListSelectForScriptViewObjects::ParameterChart_OnSetVisibility(const bool isVisible)
{
  if(m_pParameterChart)
  {
    if(isVisible) m_pParameterChart->VisibleRadarChart();
    else          m_pParameterChart->InvisibleRadarChart();
  }
  
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  JoinFestaPlayerListSelectForScriptViewObjects::ParameterChart_OnInitFunc(void)
{
  bool  isEnd = false;

  if(m_pParameterChart)
  {
    isEnd = m_pParameterChart->InitFunc();
    m_pParameterChart->Update();
    
//    if(isEnd) m_pParameterChart->VisibleRadarChart();
  }
  else
  {
    isEnd = true;
  }

  return isEnd;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  JoinFestaPlayerListSelectForScriptViewObjects::ParameterChart_OnEndFunc(void)
{
  bool  isEnd = false;

  if(m_pParameterChart)
  {
    isEnd = m_pParameterChart->EndFunc();
    m_pParameterChart->Update();
  }
  else
  {
    isEnd = true;

  }
  return isEnd;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
