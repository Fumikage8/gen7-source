// ============================================================================
/*
 * @file JoinFestaAttractionListSelectViewObjects.cpp
 * @brief アトラクション開催参加アプリのViewクラスを持っているクラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectViewObjects.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectResourceID.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectWork.h"


#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAlertLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionListSelectViewObjects::JoinFestaAttractionListSelectViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaAttractionInfoUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaPlayerListMessageMenuLowerView( NULL ),
  m_pJoinFestaAttractionSelectLowerView( NULL ),
  m_pJoinFestaAlertLowerView( NULL )
{
}


JoinFestaAttractionListSelectViewObjects::~JoinFestaAttractionListSelectViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaAlertLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionSelectLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaAttractionListSelectViewObjects::CreateView(
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
  NetAppLib::UI::NetAppMessageMenu&                                                 netAppMessageMenu )
{
  app::util::Heap* pAppHeap = pJoinFestaAttractionListSelectWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaAttractionListSelectWork, NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_LAYOUT );
  m_pJoinFestaAttractionInfoUpperView         = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaAttractionInfoUpperView( pJoinFestaAttractionListSelectWork, NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_LAYOUT );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaAttractionListSelectWork, NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListMessageMenuLowerView  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView( pJoinFestaAttractionListSelectWork, netAppMessageMenu );
  m_pJoinFestaAttractionSelectLowerView       = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerView( pJoinFestaAttractionListSelectWork, NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_LAYOUT, NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaAlertLowerView                  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaAlertLowerView( pJoinFestaAttractionListSelectWork, NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_LAYOUT );

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaAttractionInfoUpperView->SetModal();

  m_pJoinFestaPlayerListBGLowerView->SetModal();
  m_pJoinFestaPlayerListMessageMenuLowerView->SetModal();
  m_pJoinFestaAttractionSelectLowerView->SetModal();
  m_pJoinFestaAlertLowerView->SetModal();

}


void JoinFestaAttractionListSelectViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaAttractionInfoUpperView->SetInputEnabled( false );

  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );
  m_pJoinFestaPlayerListMessageMenuLowerView->SetInputEnabled( false );
  m_pJoinFestaAttractionSelectLowerView->SetInputEnabled( false );
  m_pJoinFestaAlertLowerView->SetInputEnabled( false );
}


bool JoinFestaAttractionListSelectViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaAttractionInfoUpperView->IsDrawing()
   || m_pJoinFestaPlayerListBGLowerView->IsDrawing()
   || m_pJoinFestaPlayerListMessageMenuLowerView->IsDrawing()
   || m_pJoinFestaAttractionSelectLowerView->IsDrawing()
   || m_pJoinFestaAlertLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
