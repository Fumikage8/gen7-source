// ============================================================================
/*
 * @file JoinFestaOpenListViewObjects.cpp
 * @brief 開催選択アプリのViewクラスを持っているクラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewObjects.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListResourceID.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListWork.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAlertLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


JoinFestaOpenListViewObjects::JoinFestaOpenListViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaPlayerListPlayerInfoUpperView( NULL ),
  m_pJoinFestaAttractionInfoUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaPlayerListMessageMenuLowerView( NULL ),
  m_pJoinFestaAttractionSelectLowerView( NULL ),
  m_pJoinFestaAlertLowerView( NULL )
{
}


JoinFestaOpenListViewObjects::~JoinFestaOpenListViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaAlertLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionSelectLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListPlayerInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaOpenListViewObjects::CreateView(
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork*   pJoinFestaOpenListWork,
  NetAppLib::UI::NetAppMessageMenu&                           netAppMessageMenu,
  NetAppLib::Util::NetAppTrainerIconUtility&                  netAppTrainerIconUtility )
{
  app::util::Heap* pAppHeap = pJoinFestaOpenListWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListPlayerInfoUpperView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView( pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_LAYOUT, netAppTrainerIconUtility );
  m_pJoinFestaAttractionInfoUpperView         = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaAttractionInfoUpperView( pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_LAYOUT );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListMessageMenuLowerView  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView( pJoinFestaOpenListWork, netAppMessageMenu );
  m_pJoinFestaAttractionSelectLowerView       = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerView( pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_LAYOUT, NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaAlertLowerView                  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaAlertLowerView( pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_RESOURCE_ID_LAYOUT );

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetModal();
  m_pJoinFestaAttractionInfoUpperView->SetModal();

  m_pJoinFestaPlayerListBGLowerView->SetModal();
  m_pJoinFestaPlayerListMessageMenuLowerView->SetModal();
  m_pJoinFestaAttractionSelectLowerView->SetModal();
  m_pJoinFestaAlertLowerView->SetModal();
}


void JoinFestaOpenListViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetInputEnabled( false );
  m_pJoinFestaAttractionInfoUpperView->SetInputEnabled( false );

  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );
  m_pJoinFestaPlayerListMessageMenuLowerView->SetInputEnabled( false );
  m_pJoinFestaAttractionSelectLowerView->SetInputEnabled( false );
  m_pJoinFestaAlertLowerView->SetInputEnabled( false );
}


bool JoinFestaOpenListViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaPlayerListPlayerInfoUpperView->IsDrawing()
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
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
