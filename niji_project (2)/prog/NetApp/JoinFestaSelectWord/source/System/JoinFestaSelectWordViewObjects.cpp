// ============================================================================
/*
 * @file JoinFestaSelectWordViewObjects.cpp
 * @brief 挨拶選択アプリのViewクラスを持っているクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordViewObjects.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordBGUpperView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordUpperView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordBGLowerView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordLowerView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordMessageMenuLowerView.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordResourceID.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


JoinFestaSelectWordViewObjects::JoinFestaSelectWordViewObjects() :
  m_pJoinFestaSelectWordBGUpperView( NULL ),
  m_pJoinFestaSelectWordUpperView( NULL ),
  m_pJoinFestaSelectWordBGLowerView( NULL ),
  m_pJoinFestaSelectWordLowerView( NULL ),
  m_pJoinFestaSelectWordMessageMenuLowerView( NULL )
{
}


JoinFestaSelectWordViewObjects::~JoinFestaSelectWordViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaSelectWordMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaSelectWordLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaSelectWordBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaSelectWordUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaSelectWordBGUpperView );
}


void JoinFestaSelectWordViewObjects::CreateView(
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
  NetAppLib::UI::NetAppMessageMenu&                             netAppMessageMenu )
{
  app::util::Heap* pAppHeap = pJoinFestaSelectWordWork->GetAppHeap();

  m_pJoinFestaSelectWordBGUpperView             = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordBGUpperView( pJoinFestaSelectWordWork );
  m_pJoinFestaSelectWordUpperView               = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordUpperView( pJoinFestaSelectWordWork );
  m_pJoinFestaSelectWordBGLowerView             = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordBGLowerView( pJoinFestaSelectWordWork );
  m_pJoinFestaSelectWordLowerView               = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerView( pJoinFestaSelectWordWork );
  m_pJoinFestaSelectWordMessageMenuLowerView    = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordMessageMenuLowerView( pJoinFestaSelectWordWork, netAppMessageMenu );

  m_pJoinFestaSelectWordBGUpperView->SetModal();
  m_pJoinFestaSelectWordUpperView->SetModal();
  m_pJoinFestaSelectWordBGLowerView->SetModal();
  m_pJoinFestaSelectWordLowerView->SetModal();
  m_pJoinFestaSelectWordMessageMenuLowerView->SetModal();
}


void JoinFestaSelectWordViewObjects::InputDisableAll()
{
  m_pJoinFestaSelectWordBGUpperView->SetInputEnabled( false );
  m_pJoinFestaSelectWordUpperView->SetInputEnabled( false );
  m_pJoinFestaSelectWordBGLowerView->SetInputEnabled( false );
  m_pJoinFestaSelectWordLowerView->SetInputEnabled( false );
  m_pJoinFestaSelectWordMessageMenuLowerView->SetInputEnabled( false );
}


bool JoinFestaSelectWordViewObjects::IsDrawing()
{
  if( m_pJoinFestaSelectWordBGUpperView->IsDrawing()
   || m_pJoinFestaSelectWordUpperView->IsDrawing()
   || m_pJoinFestaSelectWordBGLowerView->IsDrawing()
   || m_pJoinFestaSelectWordLowerView->IsDrawing()
   || m_pJoinFestaSelectWordMessageMenuLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
