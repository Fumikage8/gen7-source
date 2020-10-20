// ============================================================================
/*
 * @file JoinFestaSubScreenApplicationSystem.cpp
 * @brief ジョインフェスタ下画面のアプリケーションシステムです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenApplicationSystem.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenBuilder.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>

#include <arc_index/join_festa_subscreen.gaix>
#include <arc_index/rotom_fade.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


JoinFestaSubScreenApplicationSystem::JoinFestaSubScreenApplicationSystem() :
  m_pJoinFestaSubScreenParentView( NULL )
{
}


JoinFestaSubScreenApplicationSystem::~JoinFestaSubScreenApplicationSystem()
{
  GFL_SAFE_DELETE( m_pJoinFestaSubScreenParentView );
}


void JoinFestaSubScreenApplicationSystem::Build( app::util::Heap* pAppHeap, NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenBuilder* pApplicationSystemBuilder )
{
  m_pJoinFestaSubScreenParentView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView( pApplicationSystemBuilder->GetWork() );
  pApplicationSystemBuilder->SetParentView( m_pJoinFestaSubScreenParentView );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}

//------------------------------------------------------------------
/**
 * @brief 一時停止
 */
//------------------------------------------------------------------
// @override
void JoinFestaSubScreenApplicationSystem::Suspend()
{
  ApplicationSystemBase::Suspend();
  if( m_pJoinFestaSubScreenParentView != NULL )
  {
    m_pJoinFestaSubScreenParentView->Suspend();
  }
}

//------------------------------------------------------------------
/**
 * @brief 再起動
 */
//------------------------------------------------------------------
// @override
void JoinFestaSubScreenApplicationSystem::Resume()
{
  ApplicationSystemBase::Resume();
  if( m_pJoinFestaSubScreenParentView != NULL )
  {
    m_pJoinFestaSubScreenParentView->Resume();
  }
}

void JoinFestaSubScreenApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( JOINFESTASUBSCREEN_RESOURCE_ID_LAYOUT,  ARCID_JOIN_FESTA_SUB_SCREEN, GARC_join_festa_subscreen_join_festa_subscreen_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( JOINFESTASUBSCREEN_RESOURCE_ID_ROTOM_FADE,  ARCID_ROTOM_FADE, GARC_rotom_fade_rotom_fade_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( JOINFESTASUBSCREEN_RESOURCE_ID_MESSAGE, print::GetMessageArcId(), GARC_message_jf_menu_DAT, false, true );
}

void JoinFestaSubScreenApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_menu_DAT, m_pResourceManager->GetResourceBuffer( JOINFESTASUBSCREEN_RESOURCE_ID_MESSAGE ) );
}

void JoinFestaSubScreenApplicationSystem::CreateView()
{
  m_pJoinFestaSubScreenParentView->Initialize( NULL );
  m_pJoinFestaSubScreenParentView->DisableAllView();
}

void JoinFestaSubScreenApplicationSystem::UpdateView()
{
  m_pJoinFestaSubScreenParentView->Update();
}


void JoinFestaSubScreenApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaSubScreenParentView->Draw( displayNo );
}


bool JoinFestaSubScreenApplicationSystem::IsExitView()
{
  // 破棄出来ない場合はfalseを返す
  if( m_pJoinFestaSubScreenParentView && m_pJoinFestaSubScreenParentView->IsExitView() == false )
  {
    return false;
  }

  return true;
}




GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
