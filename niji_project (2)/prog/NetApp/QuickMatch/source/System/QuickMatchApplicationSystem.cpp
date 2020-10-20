// ============================================================================
/*
 * @file QuickMatchApplicationSystem.cpp
 * @brief クイックマッチアプリのアプリケーションシステムです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/System/QuickMatchApplicationSystem.h"
#include "NetApp/QuickMatch/source/System/QuickMatchBuilder.h"
#include "NetApp/QuickMatch/source/QuickMatchResourceID.h"

#include <arc_def_index/arc_def.h>

#include "arc_index/QuickMatch.gaix"
#include "arc_index/message.gaix"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)


QuickMatchApplicationSystem::QuickMatchApplicationSystem()
: m_pQuickMatchWork( NULL )
, m_pQuickMatchUpperView( NULL )
, m_pQuickMatchLowerView( NULL )
{
}


QuickMatchApplicationSystem::~QuickMatchApplicationSystem()
{
  GFL_SAFE_DELETE( m_pQuickMatchUpperView );
  GFL_SAFE_DELETE( m_pQuickMatchLowerView );
}


void QuickMatchApplicationSystem::Build( NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork , NetApp::QuickMatch::System::QuickMatchBuilder* pApplicationSystemBuilder )
{
  m_pQuickMatchWork = pQuickMatchWork;
  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void QuickMatchApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::QuickMatch::QUICKMATCH_RESOURCE_ID_LAYOUT, ARCID_QUICKMATCH_LYT, GARC_QuickMatch_QuickMatch_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::QuickMatch::QUICKMATCH_RESOURCE_ID_MSG, print::GetMessageArcId(), GARC_message_quickmatch_DAT, false, true );
}

void QuickMatchApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_quickmatch_DAT, m_pResourceManager->GetResourceBuffer( NetApp::QuickMatch::QUICKMATCH_RESOURCE_ID_MSG ) );
  m_pQuickMatchWork->SetMessageUtility( m_pMessageUtility );
}

void QuickMatchApplicationSystem::CreateView()
{
  app::util::Heap* pAppHeap = m_pQuickMatchWork->GetAppHeap();

  m_pQuickMatchWork->SetTrainerIconUtility( m_pNetAppTrainerIconUtility );

  m_pQuickMatchUpperView = GFL_NEW( pAppHeap->GetDeviceHeap() ) View::QuickMatchUpperView( m_pQuickMatchWork , pAppHeap );
  m_pQuickMatchLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) View::QuickMatchLowerView( m_pQuickMatchWork , *m_pNetAppMessageMenu , pAppHeap , m_pQuickMatchUpperView );

  m_pQuickMatchWork->SetView( m_pQuickMatchUpperView , m_pQuickMatchLowerView );
}

void QuickMatchApplicationSystem::UpdateView()
{
  m_pQuickMatchUpperView->UpdateTree();
  m_pQuickMatchLowerView->UpdateTree();
}

void QuickMatchApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pQuickMatchUpperView->DrawTree( displayNo );
  m_pQuickMatchLowerView->DrawTree( displayNo );
}

bool QuickMatchApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pQuickMatchUpperView )
  {
    if( m_pQuickMatchUpperView->IsDrawing() )
    {
      bExit = false;
    }
  }

  if( m_pQuickMatchLowerView )
  {
    if( m_pQuickMatchLowerView->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
