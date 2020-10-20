// ============================================================================
/*
 * @file JoinFestaAttractionResultViewObjects.cpp
 * @brief アトラクション結果アプリのViewクラスを持っているクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultViewObjects.h"
#include "NetApp/JoinFestaAttractionResult/source/View/JoinFestaAttractionResultUpperView.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultResourceID.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultWork.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRankingLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionResultViewObjects::JoinFestaAttractionResultViewObjects() :
  m_pJoinFestaPlayerListBGUpperView( NULL ),
  m_pJoinFestaAttractionResultUpperView( NULL ),
  m_pJoinFestaPlayerListBGLowerView( NULL ),
  m_pJoinFestaPlayerListMessageMenuLowerView( NULL ),
  m_pJoinFestaRankingLowerView( NULL )
{
}


JoinFestaAttractionResultViewObjects::~JoinFestaAttractionResultViewObjects()
{
  GFL_SAFE_DELETE( m_pJoinFestaRankingLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGLowerView );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionResultUpperView );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListBGUpperView );
}


void JoinFestaAttractionResultViewObjects::CreateView(
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork,
  NetAppLib::UI::NetAppMessageMenu&                                         netAppMessageMenu )
{
  app::util::Heap* pAppHeap = pJoinFestaAttractionResultWork->GetAppHeap();

  m_pJoinFestaPlayerListBGUpperView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView( pJoinFestaAttractionResultWork, NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_LAYOUT );
  m_pJoinFestaAttractionResultUpperView       = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionResult::View::JoinFestaAttractionResultUpperView( pJoinFestaAttractionResultWork );

  m_pJoinFestaPlayerListBGLowerView           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView( pJoinFestaAttractionResultWork, NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_LAYOUT );
  m_pJoinFestaPlayerListMessageMenuLowerView  = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView( pJoinFestaAttractionResultWork, netAppMessageMenu );
  m_pJoinFestaRankingLowerView                = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::JoinFestaUI::JoinFestaRankingLowerView( pJoinFestaAttractionResultWork, NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_LAYOUT, NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MENU_CURSOR );

  m_pJoinFestaPlayerListBGUpperView->SetModal();
  m_pJoinFestaAttractionResultUpperView->SetModal();

  m_pJoinFestaPlayerListBGLowerView->SetModal();
  m_pJoinFestaPlayerListMessageMenuLowerView->SetModal();
  m_pJoinFestaRankingLowerView->SetModal();
}


void JoinFestaAttractionResultViewObjects::InputDisableAll()
{
  m_pJoinFestaPlayerListBGUpperView->SetInputEnabled( false );
  m_pJoinFestaAttractionResultUpperView->SetInputEnabled( false );

  m_pJoinFestaPlayerListBGLowerView->SetInputEnabled( false );
  m_pJoinFestaPlayerListMessageMenuLowerView->SetInputEnabled( false );
  m_pJoinFestaRankingLowerView->SetInputEnabled( false );
}


bool JoinFestaAttractionResultViewObjects::IsDrawing()
{
  if( m_pJoinFestaPlayerListBGUpperView->IsDrawing()
   || m_pJoinFestaAttractionResultUpperView->IsDrawing()
   || m_pJoinFestaPlayerListBGLowerView->IsDrawing()
   || m_pJoinFestaPlayerListMessageMenuLowerView->IsDrawing()
   || m_pJoinFestaRankingLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)
