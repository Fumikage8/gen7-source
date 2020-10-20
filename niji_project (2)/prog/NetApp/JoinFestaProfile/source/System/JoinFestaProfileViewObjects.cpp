// ============================================================================
/*
 * @file JoinFestaProfileViewObjects.cpp
 * @brief プロフィールアプリのViewクラスを持っているクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewObjects.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileUpperView.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileLowerView.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileMessageMenuLowerView.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileResourceID.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


JoinFestaProfileViewObjects::JoinFestaProfileViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaProfileUpperView( NULL ),
  m_pJoinFestaProfileLowerView( NULL ),
  m_pJoinFestaProfileMessageMenuLowerView( NULL )
{
}


JoinFestaProfileViewObjects::~JoinFestaProfileViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaProfileMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaProfileLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaProfileUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaProfileViewObjects::CreateView(
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
  NetAppLib::UI::NetAppMessageMenu&                       netAppMessageMenu )
{
  app::util::Heap* pAppHeap = pJoinFestaProfileWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView          = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaProfileWork, NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_LAYOUT_PLAYER_LIST );
  m_pJoinFestaPlayerListBGLowerView          = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaProfileWork, NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_LAYOUT_PLAYER_LIST );

  m_pJoinFestaProfileUpperView               = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::View::JoinFestaProfileUpperView( pJoinFestaProfileWork );
  m_pJoinFestaProfileLowerView               = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::View::JoinFestaProfileLowerView( pJoinFestaProfileWork );
  m_pJoinFestaProfileMessageMenuLowerView    = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::View::JoinFestaProfileMessageMenuLowerView( pJoinFestaProfileWork, netAppMessageMenu );

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaPlayerListBGLowerView->SetModal();

  m_pJoinFestaProfileUpperView->SetModal();
  m_pJoinFestaProfileLowerView->SetModal();
  m_pJoinFestaProfileMessageMenuLowerView->SetModal();
}


void JoinFestaProfileViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );

  m_pJoinFestaProfileUpperView->SetInputEnabled( false );
  m_pJoinFestaProfileLowerView->SetInputEnabled( false );
  m_pJoinFestaProfileMessageMenuLowerView->SetInputEnabled( false );
}


bool JoinFestaProfileViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaPlayerListBGLowerView->IsDrawing()
   || m_pJoinFestaProfileUpperView->IsDrawing()
   || m_pJoinFestaProfileLowerView->IsDrawing()
   || m_pJoinFestaProfileMessageMenuLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
