// ============================================================================
/*
 * @file RegulationSelectProc.cpp
 * @brief レギュレーション選択アプリのプロッククラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/include/RegulationSelectProc.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectBuilder.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectApplicationSystem.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)


RegulationSelectProc::RegulationSelectProc( NetApp::RegulationSelect::APP_PARAM* pAppParam ) :
  m_pAppParam( pAppParam ),
  m_pAppResult( NULL ),
  m_pAppResultLocal( NULL ),
  m_pRegulationSelectWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


RegulationSelectProc::~RegulationSelectProc()
{
}


gfl2::proc::Result RegulationSelectProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pRegulationSelectWork = GFL_NEW( pDevHeap ) NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork();
      m_pRegulationSelectWork->SetBootMode( static_cast<NetApp::RegulationSelect::BootMode>( m_pAppParam->bootMode ) );
      m_pRegulationSelectWork->SetBattleRule( static_cast<Regulation::BATTLE_RULE>( m_pAppParam->battleRule ) );
      m_pRegulationSelectWork->SetSingleNormalClearFlag( m_pAppParam->bSingleNormalClearFlag );
      m_pRegulationSelectWork->SetDoubleNormalClearFlag( m_pAppParam->bDoubleNormalClearFlag );
      m_pRegulationSelectWork->SetMultiNormalClearFlag( m_pAppParam->bMultiNormalClearFlag );
      m_pRegulationSelectWork->SetBattleRoyalNormalClearFlag( m_pAppParam->bBattleRoyalNormalClearFlag );
      m_pRegulationSelectWork->SetBattleRoyalSuperClearFlag( m_pAppParam->bBattleRoyalSuperClearFlag );
      m_pRegulationSelectWork->SetBattleRoyalHyperClearFlag( m_pAppParam->bBattleRoyalHyperClearFlag );
      m_pRegulationSelectWork->SetQRPartyFlag( m_pAppParam->bQRPartyFlag );
      if( m_pRegulationSelectWork->GetBootMode() == NetApp::RegulationSelect::BOOT_MODE_DETAIL_BATTLE_TEAM )
      {
        m_pRegulationSelectWork->SetDetailBootInfo( NetApp::RegulationSelect::DETAIL_BOOT_PROC_BATTLE_TEAM_SELECT, m_pAppParam->pRegulationDrawInfo );
      }

      app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();
      NetApp::RegulationSelect::ApplicationSystem::RegulationSelectBuilder netAppSkeletonBuilder( m_pRegulationSelectWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::ApplicationSystem::RegulationSelectApplicationSystem();
      m_pApplicationSystem->Build( m_pRegulationSelectWork, &netAppSkeletonBuilder );
      SetSubSeq( INIT_SEQUENCE_RESOURCE_LOADING );
    }
    break;

    case INIT_SEQUENCE_RESOURCE_LOADING:
    {
      if( !m_pApplicationSystem->Loading() )
      {
        result = gfl2::proc::RES_FINISH;
      }
    }
    break;
  }

  return result;
}


gfl2::proc::Result RegulationSelectProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    if( m_pAppResult )
    {
      if( m_pAppResult->pSelectRegulationDrawInfo )
      {
        NetAppLib::UI::RegulationDrawInfo* pSelectRegulationDrawInfo = m_pRegulationSelectWork->GetSelectRegulationDrawInfo();
        m_pAppResult->pSelectRegulationDrawInfo->CopyRegulation( pSelectRegulationDrawInfo );
        m_pAppResult->bIsSelectedRegulation = ( pSelectRegulationDrawInfo ) ? true : false;
      }
    }

    if( m_pAppResultLocal )
    {
      m_pAppResultLocal->bIsFatalError = m_pRegulationSelectWork->IsFatalError();
    }

    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pRegulationSelectWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result RegulationSelectProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void RegulationSelectProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void RegulationSelectProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
