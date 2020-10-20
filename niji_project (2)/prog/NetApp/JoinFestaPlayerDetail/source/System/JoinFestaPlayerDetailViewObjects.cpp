// ============================================================================
/*
 * @file JoinFestaPlayerDetailViewObjects.cpp
 * @brief 詳細アプリのViewクラスを持っているクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailViewObjects.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailResourceID.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailWork.h"


#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerView.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerDetailViewObjects::JoinFestaPlayerDetailViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaPlayerListPlayerInfoUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaRecordSelectLowerView( NULL ),
  m_pJoinFestaRecordDetailLowerView( NULL ),
  m_pJoinFestaSurveyLowerView( NULL )
{
}


JoinFestaPlayerDetailViewObjects::~JoinFestaPlayerDetailViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaSurveyLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaRecordDetailLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaRecordSelectLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListPlayerInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaPlayerDetailViewObjects::CreateView(
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork,
  NetAppLib::Util::NetAppTrainerIconUtility&                        netAppTrainerIconUtility )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerDetailWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListPlayerInfoUpperView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView( pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_LAYOUT, netAppTrainerIconUtility );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_LAYOUT );
  m_pJoinFestaRecordSelectLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerView( pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaRecordDetailLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerView( pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaSurveyLowerView                 = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaSurveyLowerView( pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_MENU_CURSOR );

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetModal();

  m_pJoinFestaPlayerListBGLowerView->SetModal();
  m_pJoinFestaRecordSelectLowerView->SetModal();
  m_pJoinFestaRecordDetailLowerView->SetModal();
  m_pJoinFestaSurveyLowerView->SetModal();

}


void JoinFestaPlayerDetailViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetInputEnabled( false );

  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );
  m_pJoinFestaRecordSelectLowerView->SetInputEnabled( false );
  m_pJoinFestaRecordDetailLowerView->SetInputEnabled( false );
  m_pJoinFestaSurveyLowerView->SetInputEnabled( false );
}


bool JoinFestaPlayerDetailViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaPlayerListPlayerInfoUpperView->IsDrawing()
   || m_pJoinFestaPlayerListBGLowerView->IsDrawing()
   || m_pJoinFestaRecordSelectLowerView->IsDrawing()
   || m_pJoinFestaRecordDetailLowerView->IsDrawing()
   || m_pJoinFestaSurveyLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
