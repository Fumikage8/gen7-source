// ============================================================================
/*
 * @file JoinFestaGlobalAttractionViewObjects.cpp
 * @brief グローバルアトラクションアプリのViewクラスを持っているクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionViewObjects.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionBGUpperView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionInfoUpperView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionBGLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionSelectListLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionDetailLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionMessageMenuLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionResourceID.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


JoinFestaGlobalAttractionViewObjects::JoinFestaGlobalAttractionViewObjects() :
  m_pJoinFestaGlobalAttractionBGUpperView( NULL ),
  m_pJoinFestaGlobalAttractionInfoUpperView( NULL ),
  m_pJoinFestaGlobalAttractionBGLowerView( NULL ),
  m_pJoinFestaGlobalAttractionSelectListLowerView( NULL ),
  m_pJoinFestaGlobalAttractionDetailLowerView( NULL ),
  m_pJoinFestaGlobalAttractionMessageMenuLowerView( NULL )
{
}


JoinFestaGlobalAttractionViewObjects::~JoinFestaGlobalAttractionViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionDetailLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionSelectListLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionInfoUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionBGUpperView );
}


void JoinFestaGlobalAttractionViewObjects::CreateView(
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork,
  NetAppLib::UI::NetAppMessageMenu&                                         netAppMessageMenu )
{
  app::util::Heap* pAppHeap = pJoinFestaGlobalAttractionWork->GetAppHeap();

  m_pJoinFestaGlobalAttractionBGUpperView            = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionBGUpperView( pJoinFestaGlobalAttractionWork );
  m_pJoinFestaGlobalAttractionInfoUpperView          = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionInfoUpperView( pJoinFestaGlobalAttractionWork );

  m_pJoinFestaGlobalAttractionBGLowerView            = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionBGLowerView( pJoinFestaGlobalAttractionWork );
  m_pJoinFestaGlobalAttractionSelectListLowerView    = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerView( pJoinFestaGlobalAttractionWork );
  m_pJoinFestaGlobalAttractionDetailLowerView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionDetailLowerView( pJoinFestaGlobalAttractionWork );
  m_pJoinFestaGlobalAttractionMessageMenuLowerView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerView( pJoinFestaGlobalAttractionWork, netAppMessageMenu );

  m_pJoinFestaGlobalAttractionBGUpperView->SetModal();
  m_pJoinFestaGlobalAttractionInfoUpperView->SetModal();

  m_pJoinFestaGlobalAttractionBGLowerView->SetModal();
  m_pJoinFestaGlobalAttractionSelectListLowerView->SetModal();
  m_pJoinFestaGlobalAttractionDetailLowerView->SetModal();
  m_pJoinFestaGlobalAttractionMessageMenuLowerView->SetModal();
}


void JoinFestaGlobalAttractionViewObjects::InputDisableAll()
{
  m_pJoinFestaGlobalAttractionBGUpperView->SetInputEnabled( false );
  m_pJoinFestaGlobalAttractionInfoUpperView->SetInputEnabled( false );

  m_pJoinFestaGlobalAttractionBGLowerView->SetInputEnabled( false );
  m_pJoinFestaGlobalAttractionSelectListLowerView->SetInputEnabled( false );
  m_pJoinFestaGlobalAttractionDetailLowerView->SetInputEnabled( false );
  m_pJoinFestaGlobalAttractionMessageMenuLowerView->SetInputEnabled( false );
}


bool JoinFestaGlobalAttractionViewObjects::IsDrawing()
{
  if( m_pJoinFestaGlobalAttractionBGUpperView->IsDrawing()
   || m_pJoinFestaGlobalAttractionInfoUpperView->IsDrawing()
   || m_pJoinFestaGlobalAttractionBGLowerView->IsDrawing()
   || m_pJoinFestaGlobalAttractionSelectListLowerView->IsDrawing()
   || m_pJoinFestaGlobalAttractionDetailLowerView->IsDrawing()
   || m_pJoinFestaGlobalAttractionMessageMenuLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
