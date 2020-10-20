// ============================================================================
/*
 * @file GTSFrameBase.cpp
 * @brief GTSのフレーム基底クラスです。
 * @date 2015.05.28
 */
// ============================================================================
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


GTSFrameBase::GTSFrameBase( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetAppLib::System::NetAppFrameBase( pGTSWork ),
  m_pRootUpperView( pGTSWork->GetRootUpperView() ),
  m_pRootLowerView( pGTSWork->GetRootLowerView() ),
  m_pActiveUpperView( m_pRootUpperView ),
  m_pActiveLowerView( m_pRootLowerView )
{
}


GTSFrameBase::~GTSFrameBase()
{
}


void GTSFrameBase::addUpperViewInputEnable( app::ui::UIView* pView )
{
  pView->SetInputEnabled( true );

  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void GTSFrameBase::addUpperViewInputDisable( app::ui::UIView* pView )
{
  pView->SetInputEnabled( false );

  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void GTSFrameBase::removeUpperView()
{
  app::ui::UIView* pSuperView = m_pActiveUpperView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveUpperView->RemoveFromSuperView();
    m_pActiveUpperView = pSuperView;
  }
}


void GTSFrameBase::addLowerViewInputEnable( app::ui::UIView* pView )
{
  pView->SetInputEnabled( true );

  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void GTSFrameBase::addLowerViewInputDisable( app::ui::UIView* pView )
{
  pView->SetInputEnabled( false );

  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void GTSFrameBase::removeLowerView()
{
  app::ui::UIView* pSuperView = m_pActiveLowerView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveLowerView->RemoveFromSuperView();
    m_pActiveLowerView = pSuperView;
  }
}


void GTSFrameBase::removeAllView()
{
  while( m_pRootUpperView != m_pActiveUpperView )
  {
    removeUpperView();
  }

  while( m_pRootLowerView != m_pActiveLowerView )
  {
    removeLowerView();
  }
}


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
