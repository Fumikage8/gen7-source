// ============================================================================
/*
 * @file EndRollEndRollViewList.cpp
 * @brief エンドロールアプリのViewリストです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/System/EndRollViewList.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)

  EndRollViewList::EndRollViewList( app::util::Heap* pAppHeap ) :
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


  EndRollViewList::~EndRollViewList()
  {
    RemoveAllView();

    GFL_SAFE_DELETE( m_pRootLowerView );
    GFL_SAFE_DELETE( m_pRootUpperView );
  }


  void EndRollViewList::Update()
  {
    m_pRootUpperView->UpdateTree();
    m_pRootLowerView->UpdateTree();
  }


  void EndRollViewList::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pRootUpperView->DrawTree( displayNo );
    m_pRootLowerView->DrawTree( displayNo );
  }


  void EndRollViewList::AddUpperView( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
  }


  void EndRollViewList::AddLowerView( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
  }


  void EndRollViewList::AddUpperViewInputEnable( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
    pView->SetInputEnabled( true );
  }


  void EndRollViewList::AddLowerViewInputEnable( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
    pView->SetInputEnabled( true );
  }


  void EndRollViewList::AddUpperViewInputDisable( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
    pView->SetInputEnabled( false );
  }


  void EndRollViewList::AddLowerViewInputDisable( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
    pView->SetInputEnabled( false );
  }

  void EndRollViewList::RemoveAllView()
  {
    m_pRootUpperView->RemoveAllSubView();
    m_pRootLowerView->RemoveAllSubView();
    m_pActiveUpperView = m_pRootUpperView;
    m_pActiveLowerView = m_pRootLowerView;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)
