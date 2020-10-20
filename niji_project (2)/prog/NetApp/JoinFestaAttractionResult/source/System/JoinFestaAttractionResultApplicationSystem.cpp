// ============================================================================
/*
 * @file JoinFestaAttractionResultApplicationSystem.cpp
 * @brief アトラクション結果アプリのアプリケーションシステムです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultApplicationSystem.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultViewList.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultViewObjects.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultViewSystemAccessor.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultBuilder.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionResultApplicationSystem::JoinFestaAttractionResultApplicationSystem() :
  m_pJoinFestaAttractionResultWork( NULL ),
  m_pJoinFestaAttractionResultViewList( NULL ),
  m_pJoinFestaAttractionResultViewObjects( NULL ),
  m_pJoinFestaAttractionResultViewSystemAccessor( NULL )
{
}


JoinFestaAttractionResultApplicationSystem::~JoinFestaAttractionResultApplicationSystem()
{
  if( m_pJoinFestaAttractionResultViewList )
  {
    m_pJoinFestaAttractionResultViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaAttractionResultViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionResultViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaAttractionResultViewList );
}


void JoinFestaAttractionResultApplicationSystem::Build( NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork, NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaAttractionResultWork->GetAppHeap();

  m_pJoinFestaAttractionResultWork               = pJoinFestaAttractionResultWork;
  m_pJoinFestaAttractionResultViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewList( pAppHeap );
  m_pJoinFestaAttractionResultViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewObjects();
  m_pJoinFestaAttractionResultViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor( m_pJoinFestaAttractionResultViewList, m_pJoinFestaAttractionResultViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaAttractionResultViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaAttractionResultApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST, print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION, print::GetMessageArcId(), GARC_message_jf_attraction_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaAttractionResultApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_playerlist_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_attraction_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION ) );
  m_pJoinFestaAttractionResultWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaAttractionResultApplicationSystem::CreateView()
{
  m_pJoinFestaAttractionResultViewObjects->CreateView( m_pJoinFestaAttractionResultWork, *m_pNetAppMessageMenu );
}


void JoinFestaAttractionResultApplicationSystem::UpdateView()
{
  m_pJoinFestaAttractionResultViewList->Update();
}


void JoinFestaAttractionResultApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaAttractionResultViewList->Draw( displayNo );
}


bool JoinFestaAttractionResultApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaAttractionResultViewObjects )
  {
    if( m_pJoinFestaAttractionResultViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)
