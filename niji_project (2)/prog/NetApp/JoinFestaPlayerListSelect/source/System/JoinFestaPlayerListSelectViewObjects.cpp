// ============================================================================
/*
 * @file JoinFestaPlayerListSelectViewObjects.cpp
 * @brief プレイヤー単体選択アプリのViewクラスを持っているクラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectViewObjects.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectWork.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectResourceID.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectViewObjects::JoinFestaPlayerListSelectViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaPlayerListPlayerInfoUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaPlayerListSelectLowerViewBase( NULL ),
  m_pJoinFestaPlayerListMessageMenuLowerView( NULL )
{
}


JoinFestaPlayerListSelectViewObjects::~JoinFestaPlayerListSelectViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectLowerViewBase );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListPlayerInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaPlayerListSelectViewObjects::RemoveFromSuperViewAll()
{
  m_pJoinFestaPlayerListBGUpperView->RemoveFromSuperView();
  m_pJoinFestaPlayerListPlayerInfoUpperView->RemoveFromSuperView();
  m_pJoinFestaPlayerListBGLowerView->RemoveFromSuperView();
  m_pJoinFestaPlayerListSelectLowerViewBase->RemoveFromSuperView();
  m_pJoinFestaPlayerListMessageMenuLowerView->RemoveFromSuperView();
}


void JoinFestaPlayerListSelectViewObjects::CreateView(
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork,
  NetAppLib::UI::NetAppMessageMenu&                                         netAppMessageMenu,
  NetAppLib::Util::NetAppTrainerIconUtility&                                netAppTrainerIconUtility )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerListSelectWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaPlayerListSelectWork, NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListPlayerInfoUpperView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView( pJoinFestaPlayerListSelectWork, NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_LAYOUT, netAppTrainerIconUtility );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaPlayerListSelectWork, NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListSelectLowerViewBase   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase( pJoinFestaPlayerListSelectWork, NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_MENU_CURSOR );
  m_pJoinFestaPlayerListMessageMenuLowerView  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView( pJoinFestaPlayerListSelectWork, netAppMessageMenu );

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetModal();

  m_pJoinFestaPlayerListBGLowerView->SetModal();
  m_pJoinFestaPlayerListSelectLowerViewBase->SetModal();
  m_pJoinFestaPlayerListMessageMenuLowerView->SetModal();
}


void JoinFestaPlayerListSelectViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetInputEnabled( false );

  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );
  m_pJoinFestaPlayerListSelectLowerViewBase->SetInputEnabled( false );
  m_pJoinFestaPlayerListMessageMenuLowerView->SetInputEnabled( false );
}


bool JoinFestaPlayerListSelectViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaPlayerListPlayerInfoUpperView->IsDrawing()
   || m_pJoinFestaPlayerListBGLowerView->IsDrawing()
   || m_pJoinFestaPlayerListSelectLowerViewBase->IsDrawing()
   || m_pJoinFestaPlayerListMessageMenuLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
