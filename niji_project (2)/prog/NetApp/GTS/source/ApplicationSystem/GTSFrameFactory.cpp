// ============================================================================
/*
 * @file GTSFrameFactory.cpp
 * @brief GTSアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.03.26
 */
// ============================================================================
#include "NetApp/GTS/source/ApplicationSystem/GTSFrameFactory.h"
#include "NetApp/GTS/source/GTSFrameID.h"
#include "NetApp/GTS/source/StateConfirm/GTSStateConfirmFrame.h"
#include "NetApp/GTS/source/TopMenu/GTSTopMenuFrame.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchPokemonFrame.h"
#include "NetApp/GTS/source/TradePersonSelect/GTSTradePersonSelectFrame.h"
#include "NetApp/GTS/source/TradePersonRefineSearch/GTSTradePersonRefineSearchFrame.h"
#include "NetApp/GTS/source/TradePokemon/GTSTradePokemonFrame.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadPokemonFrame.h"
#include "NetApp/GTS/source/DownloadMyPokemon/GTSDownloadMyPokemonFrame.h"
#include "NetApp/GTS/source/NameInput/GTSNameInputFrame.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


GTSFrameFactory::GTSFrameFactory( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork, NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) :
  m_pGTSWork( pGTSWork ),
  m_pGTSViewObjects( pGTSViewObjects )
{
}


GTSFrameFactory::~GTSFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* GTSFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::GTS::GTS_FRAME_ID_STATE_CONFIRM:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::StateConfirm::GTSStateConfirmFrame( m_pGTSWork, m_pGTSViewObjects );
    break;

    case NetApp::GTS::GTS_FRAME_ID_TOP_MENU:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::TopMenu::GTSTopMenuFrame( m_pGTSWork, m_pGTSViewObjects );
    break;

    case NetApp::GTS::GTS_FRAME_ID_SEARCH_POKEMON:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::SearchPokemon::GTSSearchPokemonFrame( m_pGTSWork, m_pGTSViewObjects );
    break;

    case NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_SELECT:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::TradePersonSelect::GTSTradePersonSelectFrame( m_pGTSWork, m_pGTSViewObjects );
    break;

    case NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_REFINE_SEARCH:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchFrame( m_pGTSWork, m_pGTSViewObjects );
    break;

    case NetApp::GTS::GTS_FRAME_ID_TRADE_POKEMON:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::TradePokemon::GTSTradePokemonFrame( m_pGTSWork, m_pGTSViewObjects );
    break;

    case NetApp::GTS::GTS_FRAME_ID_UPLOAD_POKEMON:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::UploadPokemon::GTSUploadPokemonFrame( m_pGTSWork, m_pGTSViewObjects );
    break;

    case NetApp::GTS::GTS_FRAME_ID_DOWNLOAD_MY_POKEMON:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::DownloadMyPokemon::GTSDownloadMyPokemonFrame( m_pGTSWork, m_pGTSViewObjects );
    break;

    case NetApp::GTS::GTS_FRAME_ID_NAME_INPUT:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::NameInput::GTSNameInputFrame( m_pGTSWork );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
