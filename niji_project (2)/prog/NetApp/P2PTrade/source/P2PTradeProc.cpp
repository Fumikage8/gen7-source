// ============================================================================
/*
 * @file P2PTradeProc.cpp
 * @brief P2P交換アプリのプロッククラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/include/P2PTradeProc.h"
#include "NetApp/P2PTrade/source/System/P2PTradeBuilder.h"
#include "NetApp/P2PTrade/source/System/P2PTradeApplicationSystem.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)


P2PTradeProc::P2PTradeProc( NetApp::P2PTrade::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pAppResult( NULL ),
  m_pP2PTradeWork( NULL ),
  m_pApplicationSystem( NULL ),
  m_pP2PTradeRequestClient( NULL )
{
}


P2PTradeProc::~P2PTradeProc()
{
  if( m_pP2PTradeRequestClient )
  {
    m_pP2PTradeRequestClient->RemoveP2PTradeResponseListener();
  }
}


void P2PTradeProc::SetP2PTradeRequestClient( NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient )
{
  m_pP2PTradeRequestClient = pP2PTradeRequestClient;
}


void P2PTradeProc::SetAppResult( NetApp::P2PTrade::APP_RESULT* pAppResult )
{
  m_pAppResult = pAppResult;
}


gfl2::proc::Result P2PTradeProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pP2PTradeWork = GFL_NEW( pDevHeap ) NetApp::P2PTrade::System::P2PTradeWork();
      m_pP2PTradeWork->SetBootMode( static_cast<NetApp::P2PTrade::BootMode>( m_AppParam.bootMode ) );

      if( m_pP2PTradeRequestClient )
      {
        m_pP2PTradeWork->SetP2PTradeRequestClient( m_pP2PTradeRequestClient );

        GFL_PRINT( "P2PTradeProc::InitFunc\n" );
        GFL_PRINT( "m_pP2PTradeRequestClient->SetP2PTradeResponseListener( this );\n" );

        m_pP2PTradeRequestClient->SetP2PTradeResponseListener( this );
      }

      app::util::Heap* pAppHeap = m_pP2PTradeWork->GetAppHeap();
      NetApp::P2PTrade::System::P2PTradeBuilder netAppSkeletonBuilder( m_pP2PTradeWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::System::P2PTradeApplicationSystem();
      m_pApplicationSystem->Build( m_pP2PTradeWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result P2PTradeProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    if( m_pP2PTradeWork->IsSyncSaveCancel() )
    {
      if( m_pAppResult )
      {
        m_pAppResult->bIsSyncSaveCancel = m_pP2PTradeWork->IsSyncSaveCancel();
      }
    }

    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pP2PTradeWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result P2PTradeProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  if( m_pP2PTradeRequestClient )
  {
    m_pP2PTradeWork->SetFinalTradeSuccess( m_pP2PTradeRequestClient->IsFinalTradeSuccess() );
    m_pP2PTradeWork->SetFinalTradeComplete( m_pP2PTradeRequestClient->IsFinalTradeComplete() );
  }

  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void P2PTradeProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void P2PTradeProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


void P2PTradeProc::OnMessageIDReceived( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, void* pPokemonParamBuffer )
{
}


void P2PTradeProc::OnPokemonParamReceived( const pml::pokepara::PokemonParam& pokemonParam )
{
}


void P2PTradeProc::OnFirstTradeSuccess( const pml::pokepara::PokemonParam& pokemonParam )
{
}


void P2PTradeProc::OnFirstTradePokemonInvalid( bool bMyPokemonInvalid )
{
}


void P2PTradeProc::OnFirstTradeValidationError( bool bMyValidationError )
{
}


void P2PTradeProc::OnFirstTradeCancel()
{
}


void P2PTradeProc::OnFinalTradeSuccess()
{
  if( m_pP2PTradeWork )
  {
    m_pP2PTradeWork->SetFinalTradeSuccess( true );
  }
}


void P2PTradeProc::OnFinalTradeComplete()
{
  if( m_pP2PTradeWork )
  {
    m_pP2PTradeWork->SetFinalTradeComplete( true );
  }
}


void P2PTradeProc::OnFinalTradeCancel()
{
}


void P2PTradeProc::OnTradeCancelReceived()
{
}


void P2PTradeProc::OnTradeExitReceived()
{
}


void P2PTradeProc::OnTradeExitSuccess()
{
}


void P2PTradeProc::OnFinalSyncComplete()
{
  if( m_pP2PTradeWork )
  {
    m_pP2PTradeWork->SetFinalSyncComplete( true );
  }
}


void P2PTradeProc::OnFinalSyncCancelComplete()
{
  if( m_pP2PTradeWork )
  {
    m_pP2PTradeWork->SetFinalSyncCancelComplete( true );
  }
}


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)
