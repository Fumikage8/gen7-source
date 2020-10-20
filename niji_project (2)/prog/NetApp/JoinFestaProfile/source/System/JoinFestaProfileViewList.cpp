// ============================================================================
/*
 * @file JoinFestaProfileViewList.cpp
 * @brief プロフィールアプリのViewリストです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewList.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


JoinFestaProfileViewList::JoinFestaProfileViewList( app::util::Heap* pAppHeap ) :
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


JoinFestaProfileViewList::~JoinFestaProfileViewList()
{
  GFL_SAFE_DELETE( m_pRootLowerView );
  GFL_SAFE_DELETE( m_pRootUpperView );
}


void JoinFestaProfileViewList::Update()
{
  m_pRootUpperView->UpdateTree();
  m_pRootLowerView->UpdateTree();
}


void JoinFestaProfileViewList::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pRootUpperView->DrawTree( displayNo );
  m_pRootLowerView->DrawTree( displayNo );
}


void JoinFestaProfileViewList::AddUpperView( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void JoinFestaProfileViewList::AddLowerView( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void JoinFestaProfileViewList::AddUpperViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
  pView->SetInputEnabled( true );
}


void JoinFestaProfileViewList::AddLowerViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
  pView->SetInputEnabled( true );
}


void JoinFestaProfileViewList::AddUpperViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
  pView->SetInputEnabled( false );
}


void JoinFestaProfileViewList::AddLowerViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
  pView->SetInputEnabled( false );
}


void JoinFestaProfileViewList::RemoveUpperView()
{
  app::ui::UIView* pSuperView = m_pActiveUpperView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveUpperView->RemoveFromSuperView();
    m_pActiveUpperView = pSuperView;
  }
}


void JoinFestaProfileViewList::RemoveLowerView()
{
  app::ui::UIView* pSuperView = m_pActiveLowerView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveLowerView->RemoveFromSuperView();
    m_pActiveLowerView = pSuperView;
  }
}


void JoinFestaProfileViewList::RemoveAllView()
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
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
