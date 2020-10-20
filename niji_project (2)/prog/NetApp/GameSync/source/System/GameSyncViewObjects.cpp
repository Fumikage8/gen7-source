// ============================================================================
/*
 * @file GameSyncViewObjects.cpp
 * @brief ゲームシンクアプリのViewクラスを持っているクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/System/GameSyncViewObjects.h"

#include "NetApp/GameSync/source/View/GameSyncBGUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncUppCaptureUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncLowCaptureUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncMainUpperView.h"

#include "NetApp/GameSync/source/View/GameSyncBGLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncTwoSelectMenuLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncThreeSelectMenuLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncMessageMenuLowerView.h"

#include "NetApp/GameSync/source/GameSyncResourceID.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


GameSyncViewObjects::GameSyncViewObjects() :
  m_pGameSyncBGUpperView( NULL ),
  m_pGameSyncUppCaptureUpperView( NULL ),
  m_pGameSyncLowCaptureUpperView( NULL ),
  m_pGameSyncMainUpperView( NULL ),
  m_pGameSyncBGLowerView( NULL ),
  m_pGameSyncTwoSelectMenuLowerView( NULL ),
  m_pGameSyncThreeSelectMenuLowerView( NULL ),
  m_pGameSyncMessageMenuLowerView( NULL )
{
}


GameSyncViewObjects::~GameSyncViewObjects()
{
  GFL_SAFE_DELETE( m_pGameSyncMessageMenuLowerView );
  GFL_SAFE_DELETE( m_pGameSyncThreeSelectMenuLowerView );
  GFL_SAFE_DELETE( m_pGameSyncTwoSelectMenuLowerView );
  GFL_SAFE_DELETE( m_pGameSyncBGLowerView );
  GFL_SAFE_DELETE( m_pGameSyncMainUpperView );
  GFL_SAFE_DELETE( m_pGameSyncLowCaptureUpperView );
  GFL_SAFE_DELETE( m_pGameSyncUppCaptureUpperView );
  GFL_SAFE_DELETE( m_pGameSyncBGUpperView );
}


void GameSyncViewObjects::CreateView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork, NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu )
{
  app::util::Heap* pAppHeap = pGameSyncWork->GetAppHeap();

  m_pGameSyncBGUpperView                = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::View::GameSyncBGUpperView( pGameSyncWork );
  m_pGameSyncUppCaptureUpperView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::View::GameSyncUppCaptureUpperView( pGameSyncWork );
  m_pGameSyncLowCaptureUpperView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::View::GameSyncLowCaptureUpperView( pGameSyncWork );
  m_pGameSyncMainUpperView              = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::View::GameSyncMainUpperView( pGameSyncWork );
  m_pGameSyncBGLowerView                = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::View::GameSyncBGLowerView( pGameSyncWork );
  m_pGameSyncTwoSelectMenuLowerView     = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::View::GameSyncTwoSelectMenuLowerView( pGameSyncWork );
  m_pGameSyncThreeSelectMenuLowerView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::View::GameSyncThreeSelectMenuLowerView( pGameSyncWork );
  m_pGameSyncMessageMenuLowerView       = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::View::GameSyncMessageMenuLowerView( pGameSyncWork, netAppMessageMenu );

  m_pGameSyncBGUpperView->SetModal();
  m_pGameSyncUppCaptureUpperView->SetModal();
  m_pGameSyncLowCaptureUpperView->SetModal();
  m_pGameSyncMainUpperView->SetModal();
  m_pGameSyncBGLowerView->SetModal();
  m_pGameSyncTwoSelectMenuLowerView->SetModal();
  m_pGameSyncThreeSelectMenuLowerView->SetModal();
  m_pGameSyncMessageMenuLowerView->SetModal();
}


void GameSyncViewObjects::InputDisableAll()
{
  m_pGameSyncBGUpperView->SetInputEnabled( false );
  m_pGameSyncUppCaptureUpperView->SetInputEnabled( false );
  m_pGameSyncLowCaptureUpperView->SetInputEnabled( false );
  m_pGameSyncMainUpperView->SetInputEnabled( false );
  m_pGameSyncBGLowerView->SetInputEnabled( false );
  m_pGameSyncTwoSelectMenuLowerView->SetInputEnabled( false );
  m_pGameSyncThreeSelectMenuLowerView->SetInputEnabled( false );
  m_pGameSyncMessageMenuLowerView->SetInputEnabled( false );
}


bool GameSyncViewObjects::IsDrawing()
{
  if( m_pGameSyncBGUpperView->IsDrawing()
   || m_pGameSyncUppCaptureUpperView->IsDrawing()
   || m_pGameSyncLowCaptureUpperView->IsDrawing()
   || m_pGameSyncMainUpperView->IsDrawing()
   || m_pGameSyncBGLowerView->IsDrawing()
   || m_pGameSyncTwoSelectMenuLowerView->IsDrawing()
   || m_pGameSyncThreeSelectMenuLowerView->IsDrawing()
   || m_pGameSyncMessageMenuLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
