// ============================================================================
/*
 * @file BattleTreeViewList.h
 * @brief バトルツリーアプリのViewリストです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/System/BattleTreeViewList.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  ViewList::ViewList( app::util::Heap* pAppHeap ) :
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


  ViewList::~ViewList()
  {
    RemoveAllView();

    GFL_SAFE_DELETE( m_pRootLowerView );
    GFL_SAFE_DELETE( m_pRootUpperView );
  }


  void ViewList::Update()
  {
    m_pRootUpperView->UpdateTree();
    m_pRootLowerView->UpdateTree();
  }


  void ViewList::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pRootUpperView->DrawTree( displayNo );
    m_pRootLowerView->DrawTree( displayNo );
  }


  void ViewList::AddUpperView( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
  }


  void ViewList::AddLowerView( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
  }


  void ViewList::AddUpperViewInputEnable( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
    pView->SetInputEnabled( true );
  }


  void ViewList::AddLowerViewInputEnable( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
    pView->SetInputEnabled( true );
  }


  void ViewList::AddUpperViewInputDisable( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
    pView->SetInputEnabled( false );
  }


  void ViewList::AddLowerViewInputDisable( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
    pView->SetInputEnabled( false );
  }

  void ViewList::RemoveAllView()
  {
    m_pRootUpperView->RemoveAllSubView();
    m_pRootLowerView->RemoveAllSubView();
    m_pActiveUpperView = m_pRootUpperView;
    m_pActiveLowerView = m_pRootLowerView;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
