// ============================================================================
/*
 * @file ConnectionViewList.cpp
 * @brief インターネット接続アプリのViewリストです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/System/ConnectionViewList.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


ConnectionViewList::ConnectionViewList( app::util::Heap* pAppHeap ) :
  m_pRootUpperView( NULL ),
  m_pRootLowerView( NULL ),
  m_pActiveUpperView( NULL ),
  m_pActiveLowerView( NULL )
{
  m_pRootUpperView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppEmptyView( pAppHeap );
  m_pRootLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppEmptyView( pAppHeap );
  m_pActiveUpperView = m_pRootUpperView;
  m_pActiveLowerView = m_pRootLowerView;
}


ConnectionViewList::~ConnectionViewList()
{
  GFL_SAFE_DELETE( m_pRootLowerView );
  GFL_SAFE_DELETE( m_pRootUpperView );
}


void ConnectionViewList::Update()
{
  m_pRootUpperView->UpdateTree();
  m_pRootLowerView->UpdateTree();
}


void ConnectionViewList::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pRootUpperView->DrawTree( displayNo );
  m_pRootLowerView->DrawTree( displayNo );
}


void ConnectionViewList::AddUpperView( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void ConnectionViewList::AddLowerView( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void ConnectionViewList::AddUpperViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
  pView->SetInputEnabled( true );
}


void ConnectionViewList::AddLowerViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
  pView->SetInputEnabled( true );
}


void ConnectionViewList::AddUpperViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
  pView->SetInputEnabled( false );
}


void ConnectionViewList::AddLowerViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
  pView->SetInputEnabled( false );
}


void ConnectionViewList::RemoveUpperView()
{
  app::ui::UIView* pSuperView = m_pActiveUpperView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveUpperView->RemoveFromSuperView();
    m_pActiveUpperView = pSuperView;
  }
}


void ConnectionViewList::RemoveLowerView()
{
  app::ui::UIView* pSuperView = m_pActiveLowerView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveLowerView->RemoveFromSuperView();
    m_pActiveLowerView = pSuperView;
  }
}


void ConnectionViewList::RemoveAllView()
{
  m_pRootUpperView->RemoveAllSubView();
  m_pRootLowerView->RemoveAllSubView();
  m_pActiveUpperView = m_pRootUpperView;
  m_pActiveLowerView = m_pRootLowerView;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)
