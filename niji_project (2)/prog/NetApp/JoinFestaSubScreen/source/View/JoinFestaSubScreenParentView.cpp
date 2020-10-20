// ============================================================================
/*
 * @file JoinFestaSubScreenParentView.cpp
 * @brief ジョインフェスタ下画面のViewクラスを管理するクラスです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"
#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"

// SubView
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenBgLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenMainLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenInvitationLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenAttractionLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenAlertLowerView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


JoinFestaSubScreenParentView::JoinFestaSubScreenParentView( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork ) :
  m_pBgLowerView( NULL )
  ,m_pMainLowerView( NULL )
  ,m_pSelectLowerView( NULL )
  ,m_pInvitationLowerView( NULL )
  ,m_pAttractionLowerView( NULL )
  ,m_pAlertLowerView( NULL )
  ,m_pJoinFestaSubScreenWork(pJoinFestaSubScreenWork)
{
}


JoinFestaSubScreenParentView::~JoinFestaSubScreenParentView()
{
  GFL_SAFE_DELETE( m_pBgLowerView );
  GFL_SAFE_DELETE( m_pMainLowerView );
  GFL_SAFE_DELETE( m_pSelectLowerView );
  GFL_SAFE_DELETE( m_pInvitationLowerView );
  GFL_SAFE_DELETE( m_pAttractionLowerView );
  GFL_SAFE_DELETE( m_pAlertLowerView );
}


void JoinFestaSubScreenParentView::Initialize( NetAppLib::UI::NetAppMessageMenu* pNetAppMessageMenu )
{
  app::util::Heap* pAppHeap = m_pJoinFestaSubScreenWork->GetAppHeap();

  m_pBgLowerView        = GFL_NEW( pAppHeap->GetDeviceHeap() )   JoinFestaSubScreenBgLowerView( m_pJoinFestaSubScreenWork );
  m_pMainLowerView      = GFL_NEW( pAppHeap->GetDeviceHeap() ) JoinFestaSubScreenMainLowerView( m_pJoinFestaSubScreenWork );
  m_pSelectLowerView    = GFL_NEW( pAppHeap->GetDeviceHeap() ) JoinFestaSubScreenSelectLowerView( m_pJoinFestaSubScreenWork );
  m_pInvitationLowerView= GFL_NEW( pAppHeap->GetDeviceHeap() ) JoinFestaSubScreenInvitationLowerView( m_pJoinFestaSubScreenWork );
  m_pAttractionLowerView= GFL_NEW( pAppHeap->GetDeviceHeap() ) JoinFestaSubScreenAttractionLowerView( m_pJoinFestaSubScreenWork );
  m_pAlertLowerView     = GFL_NEW( pAppHeap->GetDeviceHeap() ) JoinFestaSubScreenAlertLowerView( m_pJoinFestaSubScreenWork );


  // 背景は初期非表示
  m_pBgLowerView->SetVisible( false );
}


void JoinFestaSubScreenParentView::Update()
{
  m_pBgLowerView->Update();
  m_pMainLowerView->Update();
  m_pSelectLowerView->Update();
  m_pInvitationLowerView->Update();
  m_pAttractionLowerView->Update();
  m_pAlertLowerView->Update();
}


void JoinFestaSubScreenParentView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  // IsVisibleを中で見ていて丁度良いからDrawTreeを使う
  m_pBgLowerView->DrawTree( displayNo );
  m_pMainLowerView->DrawTree( displayNo );
  m_pSelectLowerView->DrawTree( displayNo );
  m_pInvitationLowerView->DrawTree( displayNo );
  m_pAttractionLowerView->DrawTree( displayNo );
  m_pAlertLowerView->DrawTree( displayNo );
}

//------------------------------------------------------------------
/**
 * @brief Viewを破棄出来るかどうか
 *
 * @retval true：破棄出来る
 * @retval false：破棄出来ない
 */
//------------------------------------------------------------------
bool JoinFestaSubScreenParentView::IsExitView()
{
  // 描画中は破棄出来ない
  if( ( m_pBgLowerView && m_pBgLowerView->IsDrawing() == true ) ||
      ( m_pMainLowerView && m_pMainLowerView->IsDrawing() == true ) ||
      ( m_pSelectLowerView && m_pSelectLowerView->IsDrawing() == true ) || 
      ( m_pInvitationLowerView && m_pInvitationLowerView->IsDrawing() == true ) ||
      ( m_pAttractionLowerView && m_pAttractionLowerView->IsDrawing() == true ) ||
      ( m_pAlertLowerView && m_pAlertLowerView->IsDrawing() == true )
    )
  {
    return false;
  }
  return true;
}

//------------------------------------------------------------------
/**
 * @brief 全てのViewを非表示
 */
//------------------------------------------------------------------
void JoinFestaSubScreenParentView::DisableAllView()
{
  if( m_pMainLowerView != NULL )
  {
    m_pMainLowerView->SetVisible( false );
  }
  if( m_pSelectLowerView != NULL )
  {
    m_pSelectLowerView->SetVisible( false );
  }
  if( m_pInvitationLowerView != NULL )
  {
    m_pInvitationLowerView->SetVisible( false );
  }
  if( m_pAttractionLowerView != NULL )
  {
    m_pAttractionLowerView->SetVisible( false );
  }
  if( m_pAlertLowerView != NULL )
  {
    m_pAlertLowerView->SetVisible( false );
  }
}

//------------------------------------------------------------------
/**
 * @brief 一時停止
 */
//------------------------------------------------------------------
void JoinFestaSubScreenParentView::Suspend()
{
  if( m_pMainLowerView != NULL )
  {
    m_pMainLowerView->Suspend();
  }
  if( m_pSelectLowerView != NULL )
  {
    m_pSelectLowerView->Suspend();
  }
  if( m_pInvitationLowerView != NULL )
  {
    m_pInvitationLowerView->Suspend();
  }
  if( m_pAttractionLowerView != NULL )
  {
    m_pAttractionLowerView->Suspend();
  }
  if( m_pAlertLowerView != NULL )
  {
    m_pAlertLowerView->Suspend();
  }
}

//------------------------------------------------------------------
/**
 * @brief 再起動
 */
//------------------------------------------------------------------
void JoinFestaSubScreenParentView::Resume()
{
  if( m_pMainLowerView != NULL )
  {
    m_pMainLowerView->Resume();
  }
  if( m_pSelectLowerView != NULL )
  {
    m_pSelectLowerView->Resume();
  }
  if( m_pInvitationLowerView != NULL )
  {
    m_pInvitationLowerView->Resume();
  }
  if( m_pAttractionLowerView != NULL )
  {
    m_pAttractionLowerView->Resume();
  }
  if( m_pAlertLowerView != NULL )
  {
    m_pAlertLowerView->Resume();
  }
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
