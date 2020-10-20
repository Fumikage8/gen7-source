// ============================================================================
/*
 * @file JoinFestaGlobalAttractionApplicationSystem.cpp
 * @brief グローバルアトラクションアプリのアプリケーションシステムです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionApplicationSystem.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionViewList.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionViewObjects.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionViewSystemAccessor.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionBuilder.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_attraction.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


JoinFestaGlobalAttractionApplicationSystem::JoinFestaGlobalAttractionApplicationSystem() :
  m_pJoinFestaGlobalAttractionWork( NULL ),
  m_pJoinFestaGlobalAttractionViewList( NULL ),
  m_pJoinFestaGlobalAttractionViewObjects( NULL ),
  m_pJoinFestaGlobalAttractionViewSystemAccessor( NULL )
{
}


JoinFestaGlobalAttractionApplicationSystem::~JoinFestaGlobalAttractionApplicationSystem()
{
  if( m_pJoinFestaGlobalAttractionViewList )
  {
    m_pJoinFestaGlobalAttractionViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionViewList );
}


void JoinFestaGlobalAttractionApplicationSystem::Build( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork, NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaGlobalAttractionWork->GetAppHeap();

  m_pJoinFestaGlobalAttractionWork               = pJoinFestaGlobalAttractionWork;
  m_pJoinFestaGlobalAttractionViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewList( pAppHeap );
  m_pJoinFestaGlobalAttractionViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewObjects();
  m_pJoinFestaGlobalAttractionViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor( m_pJoinFestaGlobalAttractionViewList, m_pJoinFestaGlobalAttractionViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaGlobalAttractionViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaGlobalAttractionApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_ATTRACTION, GARC_join_festa_attraction_join_festa_attraction_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_ID_MESSAGE_JOINFESTAGLOBALATTRACTION, print::GetMessageArcId(), GARC_message_jf_attraction_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void JoinFestaGlobalAttractionApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_attraction_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_ID_MESSAGE_JOINFESTAGLOBALATTRACTION ) );
  m_pJoinFestaGlobalAttractionWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaGlobalAttractionApplicationSystem::CreateView()
{
  m_pJoinFestaGlobalAttractionViewObjects->CreateView( m_pJoinFestaGlobalAttractionWork, *m_pNetAppMessageMenu );
}


void JoinFestaGlobalAttractionApplicationSystem::UpdateView()
{
  m_pJoinFestaGlobalAttractionViewList->Update();
}


void JoinFestaGlobalAttractionApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaGlobalAttractionViewList->Draw( displayNo );
}


bool JoinFestaGlobalAttractionApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaGlobalAttractionViewObjects )
  {
    if( m_pJoinFestaGlobalAttractionViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
