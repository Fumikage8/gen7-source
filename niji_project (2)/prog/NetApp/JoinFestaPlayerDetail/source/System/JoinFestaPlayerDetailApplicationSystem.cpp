// ============================================================================
/*
 * @file JoinFestaPlayerDetailApplicationSystem.cpp
 * @brief 詳細アプリのアプリケーションシステムです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailApplicationSystem.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailViewList.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailViewObjects.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailViewSystemAccessor.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailBuilder.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerDetailApplicationSystem::JoinFestaPlayerDetailApplicationSystem() :
  m_pJoinFestaPlayerDetailWork( NULL ),
  m_pJoinFestaPlayerDetailViewList( NULL ),
  m_pJoinFestaPlayerDetailViewObjects( NULL ),
  m_pJoinFestaPlayerDetailViewSystemAccessor( NULL )
{
}


JoinFestaPlayerDetailApplicationSystem::~JoinFestaPlayerDetailApplicationSystem()
{
  if( m_pJoinFestaPlayerDetailViewList )
  {
    m_pJoinFestaPlayerDetailViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaPlayerDetailViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerDetailViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerDetailViewList );
}


void JoinFestaPlayerDetailApplicationSystem::Build( NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerDetailWork->GetAppHeap();

  m_pJoinFestaPlayerDetailWork               = pJoinFestaPlayerDetailWork;
  m_pJoinFestaPlayerDetailViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewList( pAppHeap );
  m_pJoinFestaPlayerDetailViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewObjects();
  m_pJoinFestaPlayerDetailViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor( m_pJoinFestaPlayerDetailViewList, m_pJoinFestaPlayerDetailViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaPlayerDetailViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaPlayerDetailApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST, print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA, print::GetMessageArcId(), GARC_message_jf_questiondata_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaPlayerDetailApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_playerlist_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_questiondata_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA ) );
  m_pJoinFestaPlayerDetailWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaPlayerDetailApplicationSystem::CreateView()
{
  m_pJoinFestaPlayerDetailViewObjects->CreateView( m_pJoinFestaPlayerDetailWork, *m_pNetAppTrainerIconUtility );
}


void JoinFestaPlayerDetailApplicationSystem::UpdateView()
{
  m_pJoinFestaPlayerDetailViewList->Update();
}


void JoinFestaPlayerDetailApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaPlayerDetailViewList->Draw( displayNo );
}


bool JoinFestaPlayerDetailApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaPlayerDetailViewObjects )
  {
    if( m_pJoinFestaPlayerDetailViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
