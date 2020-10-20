// ============================================================================
/*
 * @file JoinFestaOpenListViewList.cpp
 * @brief 開催選択アプリのViewリストです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewList.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


JoinFestaOpenListViewList::JoinFestaOpenListViewList( app::util::Heap* pAppHeap ) :
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


JoinFestaOpenListViewList::~JoinFestaOpenListViewList()
{
  GFL_SAFE_DELETE( m_pRootLowerView );
  GFL_SAFE_DELETE( m_pRootUpperView );
}


void JoinFestaOpenListViewList::Update()
{
  m_pRootUpperView->UpdateTree();
  m_pRootLowerView->UpdateTree();
}


void JoinFestaOpenListViewList::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pRootUpperView->DrawTree( displayNo );
  m_pRootLowerView->DrawTree( displayNo );
}


void JoinFestaOpenListViewList::AddUpperView( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void JoinFestaOpenListViewList::AddLowerView( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void JoinFestaOpenListViewList::AddUpperViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
  pView->SetInputEnabled( true );
}


void JoinFestaOpenListViewList::AddLowerViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
  pView->SetInputEnabled( true );
}


void JoinFestaOpenListViewList::AddUpperViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
  pView->SetInputEnabled( false );
}


void JoinFestaOpenListViewList::AddLowerViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
  pView->SetInputEnabled( false );
}


void JoinFestaOpenListViewList::RemoveUpperView()
{
  app::ui::UIView* pSuperView = m_pActiveUpperView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveUpperView->RemoveFromSuperView();
    m_pActiveUpperView = pSuperView;
  }
}


void JoinFestaOpenListViewList::RemoveLowerView()
{
  app::ui::UIView* pSuperView = m_pActiveLowerView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveLowerView->RemoveFromSuperView();
    m_pActiveLowerView = pSuperView;
  }
}


void JoinFestaOpenListViewList::RemoveAllView()
{
  while( m_pRootUpperView != m_pActiveUpperView )
  {
    RemoveUpperView();
  }

  while( m_pRootLowerView != m_pActiveLowerView )
  {
    RemoveLowerView();
  }
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
