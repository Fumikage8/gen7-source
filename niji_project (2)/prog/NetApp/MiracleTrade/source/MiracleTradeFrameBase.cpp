// ============================================================================
/*
 * @file MiracleTradeFrameBase.cpp
 * @brief ミラクル交換のフレーム基底クラスです。
 * @date 2015.06.03
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/MiracleTradeFrameBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)


MiracleTradeFrameBase::MiracleTradeFrameBase( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ) :
  NetAppLib::System::NetAppFrameBase( pMiracleTradeWork ),
  m_pRootUpperView( pMiracleTradeWork->GetRootUpperView() ),
  m_pRootLowerView( pMiracleTradeWork->GetRootLowerView() ),
  m_pActiveUpperView( m_pRootUpperView ),
  m_pActiveLowerView( m_pRootLowerView )
{
}


MiracleTradeFrameBase::~MiracleTradeFrameBase()
{
}


void MiracleTradeFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  m_pActiveUpperView->SetInputEnabled( false );
  m_pActiveLowerView->SetInputEnabled( false );

  NetAppFrameBase::exitFrame( result );
}


void MiracleTradeFrameBase::addUpperViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  pView->SetInputEnabled( true );

  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void MiracleTradeFrameBase::addUpperViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  pView->SetInputEnabled( false );

  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void MiracleTradeFrameBase::removeUpperView()
{
  app::ui::UIView* pSuperView = m_pActiveUpperView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveUpperView->RemoveFromSuperView();
    m_pActiveUpperView = pSuperView;
  }
}


void MiracleTradeFrameBase::addLowerViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  pView->SetInputEnabled( true );

  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void MiracleTradeFrameBase::addLowerViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  pView->SetInputEnabled( false );

  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void MiracleTradeFrameBase::removeLowerView()
{
  app::ui::UIView* pSuperView = m_pActiveLowerView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveLowerView->RemoveFromSuperView();
    m_pActiveLowerView = pSuperView;
  }
}


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
