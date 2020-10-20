// ============================================================================
/*
 * @file RegulationSelectViewObjects.cpp
 * @brief レギュレーション選択アプリのDrawクラスを管理するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainUpperView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectBGUpperView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerView.h"
#include "NetApp/RegulationSelect/source/View/RegulationDetailLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectResourceID.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


RegulationSelectViewObjects::RegulationSelectViewObjects() :
  m_pRegulationSelectMainUpperView( NULL ),
  m_pRegulationSelectBGUpperView( NULL ),
  m_pRegulationSelectMainLowerView( NULL ),
  m_pRegulationSelectMessageMenuLowerView( NULL ),
  m_pRegulationDetailLowerView( NULL )
{
}


RegulationSelectViewObjects::~RegulationSelectViewObjects()
{
  GFL_SAFE_DELETE( m_pRegulationDetailLowerView );
  GFL_SAFE_DELETE( m_pRegulationSelectMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pRegulationSelectMainLowerView );
  GFL_SAFE_DELETE( m_pRegulationSelectBGUpperView );
  GFL_SAFE_DELETE( m_pRegulationSelectMainUpperView );
}


void RegulationSelectViewObjects::RemoveFromSuperViewAll()
{
  m_pRegulationSelectMainUpperView->RemoveFromSuperView();
  m_pRegulationSelectBGUpperView->RemoveFromSuperView();
  m_pRegulationSelectMainLowerView->RemoveFromSuperView();
  m_pRegulationSelectMessageMenuLowerView->RemoveFromSuperView();
  m_pRegulationDetailLowerView->RemoveFromSuperView();
}


void RegulationSelectViewObjects::CreateDraw( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork, NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu )
{
  app::util::Heap* pAppHeap = pRegulationSelectWork->GetAppHeap();

  m_pRegulationSelectMainUpperView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Draw::RegulationSelectMainUpperView( pRegulationSelectWork );
  m_pRegulationSelectBGUpperView          = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Draw::RegulationSelectBGUpperView( pRegulationSelectWork );
  m_pRegulationSelectMainLowerView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Draw::RegulationSelectMainLowerView( pRegulationSelectWork );
  m_pRegulationSelectMessageMenuLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerView( pRegulationSelectWork, netAppMessageMenu );
  m_pRegulationDetailLowerView            = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Draw::RegulationDetailLowerView( pRegulationSelectWork );

  m_pRegulationSelectMainUpperView->SetModal();
  m_pRegulationSelectBGUpperView->SetModal();
  m_pRegulationSelectMainLowerView->SetModal();
  m_pRegulationSelectMessageMenuLowerView->SetModal();
  m_pRegulationDetailLowerView->SetModal();
}


void RegulationSelectViewObjects::InputDisableAll()
{
  m_pRegulationSelectMainUpperView->SetInputEnabled( false );
  m_pRegulationSelectBGUpperView->SetInputEnabled( false );
  m_pRegulationSelectMainLowerView->SetInputEnabled( false );
  m_pRegulationSelectMessageMenuLowerView->SetInputEnabled( false );
  m_pRegulationDetailLowerView->SetInputEnabled( false );
}


bool RegulationSelectViewObjects::IsDrawing()
{
  if( m_pRegulationSelectMainUpperView->IsDrawing()
   || m_pRegulationSelectBGUpperView->IsDrawing()
   || m_pRegulationSelectMainLowerView->IsDrawing()
   || m_pRegulationSelectMessageMenuLowerView->IsDrawing()
   || m_pRegulationDetailLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
