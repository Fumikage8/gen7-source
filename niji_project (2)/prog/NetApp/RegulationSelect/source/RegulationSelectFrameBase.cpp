// ============================================================================
/*
 * @file RegulationSelectFrameBase.cpp
 * @brief レギュレーション選択アプリのフレーム基底クラスです。
 * @date 2015.06.12
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/RegulationSelectFrameBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)


RegulationSelectFrameBase::RegulationSelectFrameBase( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  NetAppLib::System::NetAppFrameBase( pRegulationSelectWork ),
  m_pRegulationSelectWork( pRegulationSelectWork ),
  m_pRootUpperView( pRegulationSelectWork->GetRootUpperView() ),
  m_pRootLowerView( pRegulationSelectWork->GetRootLowerView() ),
  m_pActiveUpperView( m_pRootUpperView ),
  m_pActiveLowerView( m_pRootLowerView )
{
}


RegulationSelectFrameBase::~RegulationSelectFrameBase()
{
}


void RegulationSelectFrameBase::addUpperViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  pView->SetInputEnabled( true );

  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void RegulationSelectFrameBase::addUpperViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  pView->SetInputEnabled( false );

  m_pActiveUpperView->AddSubView( pView );
  m_pActiveUpperView = pView;
}


void RegulationSelectFrameBase::removeUpperView()
{
  app::ui::UIView* pSuperView = m_pActiveUpperView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveUpperView->RemoveFromSuperView();
    m_pActiveUpperView = pSuperView;
  }
}


void RegulationSelectFrameBase::addLowerViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView )
{
  pView->SetInputEnabled( true );

  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void RegulationSelectFrameBase::addLowerViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView )
{
  pView->SetInputEnabled( false );

  m_pActiveLowerView->AddSubView( pView );
  m_pActiveLowerView = pView;
}


void RegulationSelectFrameBase::removeLowerView()
{
  app::ui::UIView* pSuperView = m_pActiveLowerView->GetSuperView();
  if( pSuperView )
  {
    m_pActiveLowerView->RemoveFromSuperView();
    m_pActiveLowerView = pSuperView;
  }
}


GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
