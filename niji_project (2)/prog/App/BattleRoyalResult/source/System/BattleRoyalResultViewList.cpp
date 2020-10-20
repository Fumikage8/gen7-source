// ============================================================================
/*
 * @file BattleRoyalResultBattleRoyalResultViewList.cpp
 * @brief バトルロイヤル勝敗アプリのViewリストです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "App/BattleRoyalResult/source/System/BattleRoyalResultViewList.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)

  BattleRoyalResultViewList::BattleRoyalResultViewList( app::util::Heap* pAppHeap ) :
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


  BattleRoyalResultViewList::~BattleRoyalResultViewList()
  {
    RemoveAllView();

    GFL_SAFE_DELETE( m_pRootLowerView );
    GFL_SAFE_DELETE( m_pRootUpperView );
  }


  void BattleRoyalResultViewList::Update()
  {
    m_pRootUpperView->UpdateTree();
    m_pRootLowerView->UpdateTree();
  }


  void BattleRoyalResultViewList::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pRootUpperView->DrawTree( displayNo );
    m_pRootLowerView->DrawTree( displayNo );
  }


  void BattleRoyalResultViewList::AddUpperView( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
  }


  void BattleRoyalResultViewList::AddLowerView( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
  }


  void BattleRoyalResultViewList::AddUpperViewInputEnable( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
    pView->SetInputEnabled( true );
  }


  void BattleRoyalResultViewList::AddLowerViewInputEnable( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
    pView->SetInputEnabled( true );
  }


  void BattleRoyalResultViewList::AddUpperViewInputDisable( app::ui::UIView* pView )
  {
    m_pActiveUpperView->AddSubView( pView );
    pView->SetInputEnabled( false );
  }


  void BattleRoyalResultViewList::AddLowerViewInputDisable( app::ui::UIView* pView )
  {
    m_pActiveLowerView->AddSubView( pView );
    pView->SetInputEnabled( false );
  }

  void BattleRoyalResultViewList::RemoveAllView()
  {
    m_pRootUpperView->RemoveAllSubView();
    m_pRootLowerView->RemoveAllSubView();
    m_pActiveUpperView = m_pRootUpperView;
    m_pActiveLowerView = m_pRootLowerView;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
