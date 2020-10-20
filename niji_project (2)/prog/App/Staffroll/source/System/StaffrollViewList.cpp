// ============================================================================
/*
 * @file StaffrollStaffrollViewList.cpp
 * @brief スタッフロールアプリのViewリストです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/System/StaffrollViewList.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)

  StaffrollViewList::StaffrollViewList( app::util::Heap* pAppHeap ) :
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


  StaffrollViewList::~StaffrollViewList()
  {
    RemoveAllView();

    GFL_SAFE_DELETE( m_pRootLowerView );
    GFL_SAFE_DELETE( m_pRootUpperView );
  }


  void StaffrollViewList::Update()
  {
    m_pRootUpperView->UpdateTree();
    m_pRootLowerView->UpdateTree();
  }


  void StaffrollViewList::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pRootUpperView->DrawTree( displayNo );
    m_pRootLowerView->DrawTree( displayNo );
  }


  void StaffrollViewList::AddUpperView( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
  }


  void StaffrollViewList::AddLowerView( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
  }


  void StaffrollViewList::AddUpperViewInputEnable( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
    pView->SetInputEnabled( true );
  }


  void StaffrollViewList::AddLowerViewInputEnable( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
    pView->SetInputEnabled( true );
  }


  void StaffrollViewList::AddUpperViewInputDisable( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
    pView->SetInputEnabled( false );
  }


  void StaffrollViewList::AddLowerViewInputDisable( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
    pView->SetInputEnabled( false );
  }

  void StaffrollViewList::RemoveAllView()
  {
    m_pRootUpperView->RemoveAllSubView();
    m_pRootLowerView->RemoveAllSubView();
    m_pActiveUpperView = m_pRootUpperView;
    m_pActiveLowerView = m_pRootLowerView;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
