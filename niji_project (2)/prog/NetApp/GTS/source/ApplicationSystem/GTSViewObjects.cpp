// ============================================================================
/*
 * @file GTSViewObjects.cpp
 * @brief GTSのDrawクラスを管理するクラスです。
 * @date 2015.06.01
 */
// ============================================================================
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetApp/GTS/source/ScrollMenu/GTSScrollMenuLowerView.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchPokemonLowerView.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchOptionLowerView.h"
#include "NetApp/GTS/source/SelectMenu/GTSOneSelectMenuLowerView.h"
#include "NetApp/GTS/source/SelectMenu/GTSTwoSelectMenuLowerView.h"
#include "NetApp/GTS/source/SelectMenu/GTSThreeSelectMenuLowerView.h"
#include "NetApp/GTS/source/TopMenu/GTSTopMenuLowerView.h"
#include "NetApp/GTS/source/TradePersonRefineSearch/GTSTradePersonRefineSearchLowerView.h"
#include "NetApp/GTS/source/TradePersonSelect/GTSTradePersonSelectLowerView.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadPokemonLowerView.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadMessageSelectLowerView.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSRequestInfoWindowUpperView.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowUpperView.h"
#include "NetApp/GTS/source/Window/GTSSmallMessageWindowUpperView.h"
#include "NetApp/GTS/source/Window/GTSWarningMessageWindowLowerView.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/GTSResourceID.h"

#include "App/Box/source/BoxUppPokeStatusDraw.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include <arc_index/message.gaix>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


GTSViewObjects::GTSViewObjects() :
  m_pGTSWork( NULL ),
  m_pGTSScrollMenuLowerView( NULL ),
  m_pGTSSearchPokemonLowerView( NULL ),
  m_pGTSSearchOptionLowerView( NULL ),
  m_pGTSOneSelectMenuLowerView( NULL ),
  m_pGTSTwoSelectMenuLowerView( NULL ),
  m_pGTSThreeSelectMenuLowerView( NULL ),
  m_pGTSTopMenuLowerView( NULL ),
  m_pGTSTradePersonRefineSearchLowerView( NULL ),
  m_pGTSTradePersonSelectLowerView( NULL ),
  m_pGTSUploadPokemonLowerView( NULL ),
  m_pGTSUploadMessageSelectLowerView( NULL ),
  m_pGTSMessageWindowLowerView( NULL ),
  m_pGTSRequestInfoWindowUpperView( NULL ),
  m_pGTSMessageWindowUpperView( NULL ),
  m_pGTSSmallMessageWindowUpperView( NULL ),
  m_pGTSWarningMessageWindowLowerView( NULL ),
  m_pAppCommonGrpIconData( NULL ),
  m_pBoxPokeStatusUpperView( NULL ),
  m_InitSequence( 0 )
{
}


GTSViewObjects::~GTSViewObjects()
{
  GFL_SAFE_DELETE( m_pBoxPokeStatusUpperView );
  if( m_pAppCommonGrpIconData )
  {
    m_pAppCommonGrpIconData->Terminate();
  }
  GFL_SAFE_DELETE( m_pAppCommonGrpIconData );
  GFL_SAFE_DELETE( m_pGTSWarningMessageWindowLowerView );
  GFL_SAFE_DELETE( m_pGTSSmallMessageWindowUpperView );
  GFL_SAFE_DELETE( m_pGTSMessageWindowUpperView );
  GFL_SAFE_DELETE( m_pGTSRequestInfoWindowUpperView );
  GFL_SAFE_DELETE( m_pGTSMessageWindowLowerView );
  GFL_SAFE_DELETE( m_pGTSUploadMessageSelectLowerView );
  GFL_SAFE_DELETE( m_pGTSUploadPokemonLowerView );
  GFL_SAFE_DELETE( m_pGTSTradePersonSelectLowerView );
  GFL_SAFE_DELETE( m_pGTSTradePersonRefineSearchLowerView );
  GFL_SAFE_DELETE( m_pGTSTopMenuLowerView );
  GFL_SAFE_DELETE( m_pGTSThreeSelectMenuLowerView );
  GFL_SAFE_DELETE( m_pGTSTwoSelectMenuLowerView );
  GFL_SAFE_DELETE( m_pGTSOneSelectMenuLowerView );
  GFL_SAFE_DELETE( m_pGTSSearchOptionLowerView );
  GFL_SAFE_DELETE( m_pGTSSearchPokemonLowerView );
  GFL_SAFE_DELETE( m_pGTSScrollMenuLowerView );
}


void GTSViewObjects::RemoveFromSuperViewAll()
{
  m_pGTSScrollMenuLowerView->RemoveFromSuperView();
  m_pGTSSearchPokemonLowerView->RemoveFromSuperView();
  m_pGTSSearchOptionLowerView->RemoveFromSuperView();
  m_pGTSOneSelectMenuLowerView->RemoveFromSuperView();
  m_pGTSTwoSelectMenuLowerView->RemoveFromSuperView();
  m_pGTSThreeSelectMenuLowerView->RemoveFromSuperView();
  m_pGTSTopMenuLowerView->RemoveFromSuperView();
  m_pGTSTradePersonRefineSearchLowerView->RemoveFromSuperView();
  m_pGTSTradePersonSelectLowerView->RemoveFromSuperView();
  m_pGTSUploadPokemonLowerView->RemoveFromSuperView();
  m_pGTSUploadMessageSelectLowerView->RemoveFromSuperView();
  m_pGTSMessageWindowLowerView->RemoveFromSuperView();
  m_pGTSRequestInfoWindowUpperView->RemoveFromSuperView();
  m_pGTSMessageWindowUpperView->RemoveFromSuperView();
  m_pGTSSmallMessageWindowUpperView->RemoveFromSuperView();
  m_pGTSWarningMessageWindowLowerView->RemoveFromSuperView();
  m_pBoxPokeStatusUpperView->RemoveFromSuperView();
}


void GTSViewObjects::CreateView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  m_pGTSWork = pGTSWork;

  gfl2::heap::HeapBase* pDeviceHeap = pGTSWork->GetDeviceHeap();

  m_pGTSScrollMenuLowerView                 = GFL_NEW( pDeviceHeap ) NetApp::GTS::ScrollMenu::GTSScrollMenuLowerView( pGTSWork );
  m_pGTSSearchPokemonLowerView              = GFL_NEW( pDeviceHeap ) NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerView( pGTSWork );
  m_pGTSSearchOptionLowerView               = GFL_NEW( pDeviceHeap ) NetApp::GTS::SearchPokemon::GTSSearchOptionLowerView( pGTSWork );
  m_pGTSOneSelectMenuLowerView              = GFL_NEW( pDeviceHeap ) NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerView( pGTSWork );
  m_pGTSTwoSelectMenuLowerView              = GFL_NEW( pDeviceHeap ) NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerView( pGTSWork );
  m_pGTSThreeSelectMenuLowerView            = GFL_NEW( pDeviceHeap ) NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerView( pGTSWork );
  m_pGTSTopMenuLowerView                    = GFL_NEW( pDeviceHeap ) NetApp::GTS::TopMenu::GTSTopMenuLowerView( pGTSWork );
  m_pGTSTradePersonRefineSearchLowerView    = GFL_NEW( pDeviceHeap ) NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerView( pGTSWork );
  m_pGTSTradePersonSelectLowerView          = GFL_NEW( pDeviceHeap ) NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerView(  pGTSWork );
  m_pGTSUploadPokemonLowerView              = GFL_NEW( pDeviceHeap ) NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerView( pGTSWork );
  m_pGTSUploadMessageSelectLowerView        = GFL_NEW( pDeviceHeap ) NetApp::GTS::UploadPokemon::GTSUploadMessageSelectLowerView( pGTSWork );
  m_pGTSMessageWindowLowerView              = GFL_NEW( pDeviceHeap ) NetApp::GTS::Window::GTSMessageWindowLowerView( pGTSWork );
  m_pGTSRequestInfoWindowUpperView          = GFL_NEW( pDeviceHeap ) NetApp::GTS::Window::GTSRequestInfoWindowUpperView( pGTSWork );
  m_pGTSMessageWindowUpperView              = GFL_NEW( pDeviceHeap ) NetApp::GTS::Window::GTSMessageWindowUpperView( pGTSWork );
  m_pGTSSmallMessageWindowUpperView         = GFL_NEW( pDeviceHeap ) NetApp::GTS::Window::GTSSmallMessageWindowUpperView( pGTSWork );
  m_pGTSWarningMessageWindowLowerView       = GFL_NEW( pDeviceHeap ) NetApp::GTS::Window::GTSWarningMessageWindowLowerView( pGTSWork );

  m_pGTSScrollMenuLowerView->SetModal();
  m_pGTSSearchPokemonLowerView->SetModal();
  m_pGTSSearchOptionLowerView->SetModal();
  m_pGTSOneSelectMenuLowerView->SetModal();
  m_pGTSTwoSelectMenuLowerView->SetModal();
  m_pGTSThreeSelectMenuLowerView->SetModal();
  m_pGTSTopMenuLowerView->SetModal();
  m_pGTSTradePersonRefineSearchLowerView->SetModal();
  m_pGTSTradePersonSelectLowerView->SetModal();
  m_pGTSUploadPokemonLowerView->SetModal();
  m_pGTSUploadMessageSelectLowerView->SetModal();
  m_pGTSMessageWindowLowerView->SetModal();
  m_pGTSRequestInfoWindowUpperView->SetModal();
  m_pGTSMessageWindowUpperView->SetModal();
  m_pGTSSmallMessageWindowUpperView->SetModal();
  m_pGTSWarningMessageWindowLowerView->SetModal();

}


bool GTSViewObjects::IsCreateViewComplete()
{
  bool isComplete = false;

  switch( m_InitSequence )
  {
    case 0:
    {
      app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();
      m_pAppCommonGrpIconData = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::AppCommonGrpIconData();
      m_pAppCommonGrpIconData->Initialize( pAppHeap );
      
      m_InitSequence++;
    }
    break;

    case 1:
    {
      if( m_pAppCommonGrpIconData->IsReady() )
      {
        m_InitSequence++;
      }
    }
    break;
    
    case 2:
    {
      App::Box::BoxUppPokeStatusDraw::INIT_PARAM initParam;

      initParam.pLytBinData            = m_pGTSWork->GetResourceManager()->GetResourceBuffer( NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_BOX );
      initParam.pRenderinManager       = m_pGTSWork->GetAppRenderingManager(1);
      initParam.pAsyncFileManager      = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      initParam.pHeap                  = m_pGTSWork->GetAppHeap();
      initParam.pBoxMsgData            = m_pGTSWork->GetMessageUtility()->GetData( GARC_message_box_DAT );
      initParam.pZukanSaveData         = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
      initParam.pAppCommonGrapIconData = m_pAppCommonGrpIconData;
      initParam.pokeInFrameLayerNo     = 0;

      m_pBoxPokeStatusUpperView = GFL_NEW( m_pGTSWork->GetDeviceHeap() ) App::Box::BoxUppPokeStatusDraw( initParam );

      m_InitSequence++;
    }
    break;
    
    case 3:
    {
      if( m_pBoxPokeStatusUpperView->InitFunc() )
      {
        isComplete = true;
      }
    }
    break;
  }

  return isComplete;
}


void GTSViewObjects::InputDisableAll()
{
  m_pGTSScrollMenuLowerView->SetInputEnabled( false );
  m_pGTSSearchPokemonLowerView->SetInputEnabled( false );
  m_pGTSSearchOptionLowerView->SetInputEnabled( false );
  m_pGTSOneSelectMenuLowerView->SetInputEnabled( false );
  m_pGTSTwoSelectMenuLowerView->SetInputEnabled( false );
  m_pGTSThreeSelectMenuLowerView->SetInputEnabled( false );
  m_pGTSTopMenuLowerView->SetInputEnabled( false );
  m_pGTSTradePersonRefineSearchLowerView->SetInputEnabled( false );
  m_pGTSTradePersonSelectLowerView->SetInputEnabled( false );
  m_pGTSUploadPokemonLowerView->SetInputEnabled( false );
  m_pGTSUploadMessageSelectLowerView->SetInputEnabled( false );
  m_pGTSMessageWindowLowerView->SetInputEnabled( false );
  m_pGTSRequestInfoWindowUpperView->SetInputEnabled( false );
  m_pGTSMessageWindowUpperView->SetInputEnabled( false );
  m_pGTSSmallMessageWindowUpperView->SetInputEnabled( false );
  m_pGTSWarningMessageWindowLowerView->SetInputEnabled( false );
}


bool GTSViewObjects::IsDrawing()
{
  bool isDrawing = false;

  if( m_pBoxPokeStatusUpperView->EndFunc() == false )
  {
    isDrawing = true;
  }

  if( m_pGTSScrollMenuLowerView->IsDrawing()
   || m_pGTSSearchPokemonLowerView->IsDrawing()
   || m_pGTSSearchOptionLowerView->IsDrawing()
   || m_pGTSOneSelectMenuLowerView->IsDrawing()
   || m_pGTSTwoSelectMenuLowerView->IsDrawing()
   || m_pGTSThreeSelectMenuLowerView->IsDrawing()
   || m_pGTSTopMenuLowerView->IsDrawing()
   || m_pGTSTradePersonRefineSearchLowerView->IsDrawing()
   || m_pGTSTradePersonSelectLowerView->IsDrawing()
   || m_pGTSUploadPokemonLowerView->IsDrawing()
   || m_pGTSUploadMessageSelectLowerView->IsDrawing()
   || m_pGTSMessageWindowLowerView->IsDrawing()
   || m_pGTSRequestInfoWindowUpperView->IsDrawing()
   || m_pGTSMessageWindowUpperView->IsDrawing()
   || m_pGTSSmallMessageWindowUpperView->IsDrawing()
   || m_pGTSWarningMessageWindowLowerView->IsDrawing()
   || m_pBoxPokeStatusUpperView->IsDrawing() )
  {
    isDrawing = true;
  }

  return isDrawing;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
