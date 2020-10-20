// ============================================================================
/*
 * @file JoinFestaPlayerListShowViewObjects.cpp
 * @brief 閲覧アプリのViewクラスを持っているクラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowViewObjects.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowResourceID.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowWork.h"


#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerView.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListShowViewObjects::JoinFestaPlayerListShowViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaPlayerListPlayerInfoUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaPlayerListSelectLowerViewBase( NULL ),
  m_pJoinFestaPlayerListMessageMenuLowerView( NULL ),
  m_pJoinFestaRecordSelectLowerView( NULL ),
  m_pJoinFestaRecordDetailLowerView( NULL ),
  m_pJoinFestaSurveyLowerView( NULL )
{
}


JoinFestaPlayerListShowViewObjects::~JoinFestaPlayerListShowViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaSurveyLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaRecordDetailLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaRecordSelectLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectLowerViewBase );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListPlayerInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaPlayerListShowViewObjects::CreateView(
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
  NetAppLib::UI::NetAppMessageMenu&                                     netAppMessageMenu,
  NetAppLib::Util::NetAppTrainerIconUtility&                            netAppTrainerIconUtility )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerListShowWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListPlayerInfoUpperView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView( pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT, netAppTrainerIconUtility );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListSelectLowerViewBase   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase( pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaPlayerListMessageMenuLowerView  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView( pJoinFestaPlayerListShowWork, netAppMessageMenu );
  m_pJoinFestaRecordSelectLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView( pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaRecordDetailLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView( pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaSurveyLowerView                 = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView( pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MENU_CURSOR );

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetModal();

  m_pJoinFestaPlayerListBGLowerView->SetModal();
  m_pJoinFestaPlayerListSelectLowerViewBase->SetModal();
  m_pJoinFestaPlayerListMessageMenuLowerView->SetModal();
  m_pJoinFestaRecordSelectLowerView->SetModal();
  m_pJoinFestaRecordDetailLowerView->SetModal();
  m_pJoinFestaSurveyLowerView->SetModal();

}


void JoinFestaPlayerListShowViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetInputEnabled( false );

  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );
  m_pJoinFestaPlayerListSelectLowerViewBase->SetInputEnabled( false );
  m_pJoinFestaPlayerListMessageMenuLowerView->SetInputEnabled( false );
  m_pJoinFestaRecordSelectLowerView->SetInputEnabled( false );
  m_pJoinFestaRecordDetailLowerView->SetInputEnabled( false );
  m_pJoinFestaSurveyLowerView->SetInputEnabled( false );
}


bool JoinFestaPlayerListShowViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaPlayerListPlayerInfoUpperView->IsDrawing()
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


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
