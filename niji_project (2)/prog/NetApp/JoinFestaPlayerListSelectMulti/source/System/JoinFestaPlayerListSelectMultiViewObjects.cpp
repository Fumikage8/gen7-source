// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiViewObjects.cpp
 * @brief プレイヤー複数選択アプリのViewクラスを持っているクラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiViewObjects.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiWork.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiResourceID.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/View/JoinFestaPlayerListSelectMultiLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectMultiViewObjects::JoinFestaPlayerListSelectMultiViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaPlayerListPlayerInfoUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaPlayerListMessageMenuLowerView( NULL ),
  m_pJoinFestaPlayerListSelectMultiLowerView( NULL )
{
}


JoinFestaPlayerListSelectMultiViewObjects::~JoinFestaPlayerListSelectMultiViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectMultiLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListPlayerInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaPlayerListSelectMultiViewObjects::RemoveFromSuperViewAll()
{
  m_pJoinFestaPlayerListBGUpperView->RemoveFromSuperView();
  m_pJoinFestaPlayerListPlayerInfoUpperView->RemoveFromSuperView();
  m_pJoinFestaPlayerListBGLowerView->RemoveFromSuperView();
  m_pJoinFestaPlayerListMessageMenuLowerView->RemoveFromSuperView();
  m_pJoinFestaPlayerListSelectMultiLowerView->RemoveFromSuperView();
}


void JoinFestaPlayerListSelectMultiViewObjects::CreateView(
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork,
  NetAppLib::UI::NetAppMessageMenu&                                                   netAppMessageMenu,
  NetAppLib::Util::NetAppTrainerIconUtility&                                          netAppTrainerIconUtility )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerListSelectMultiWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaPlayerListSelectMultiWork, NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListPlayerInfoUpperView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListPlayerInfoUpperView( pJoinFestaPlayerListSelectMultiWork, NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_LAYOUT, netAppTrainerIconUtility );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaPlayerListSelectMultiWork, NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListMessageMenuLowerView  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView( pJoinFestaPlayerListSelectMultiWork, netAppMessageMenu );

  m_pJoinFestaPlayerListSelectMultiLowerView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectMulti::View::JoinFestaPlayerListSelectMultiLowerView( pJoinFestaPlayerListSelectMultiWork );

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetModal();

  m_pJoinFestaPlayerListBGLowerView->SetModal();
  m_pJoinFestaPlayerListMessageMenuLowerView->SetModal();

  m_pJoinFestaPlayerListSelectMultiLowerView->SetModal();
}


void JoinFestaPlayerListSelectMultiViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaPlayerListPlayerInfoUpperView->SetInputEnabled( false );

  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );
  m_pJoinFestaPlayerListMessageMenuLowerView->SetInputEnabled( false );

  m_pJoinFestaPlayerListSelectMultiLowerView->SetInputEnabled( false );
}


bool JoinFestaPlayerListSelectMultiViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaPlayerListPlayerInfoUpperView->IsDrawing()
   || m_pJoinFestaPlayerListBGLowerView->IsDrawing()
   || m_pJoinFestaPlayerListMessageMenuLowerView->IsDrawing()
   || m_pJoinFestaPlayerListSelectMultiLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)
