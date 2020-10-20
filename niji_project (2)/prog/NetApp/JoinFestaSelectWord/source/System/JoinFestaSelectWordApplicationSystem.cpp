// ============================================================================
/*
 * @file JoinFestaSelectWordApplicationSystem.cpp
 * @brief 挨拶選択アプリのアプリケーションシステムです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordApplicationSystem.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordViewList.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordViewObjects.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordViewSystemAccessor.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordBuilder.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_select_word.gaix>
#include <arc_index/JoinFestaSelectWordBin.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


JoinFestaSelectWordApplicationSystem::JoinFestaSelectWordApplicationSystem() :
  m_pJoinFestaSelectWordWork( NULL ),
  m_pJoinFestaSelectWordViewList( NULL ),
  m_pJoinFestaSelectWordViewObjects( NULL ),
  m_pJoinFestaSelectWordViewSystemAccessor( NULL )
{
}


JoinFestaSelectWordApplicationSystem::~JoinFestaSelectWordApplicationSystem()
{
  if( m_pJoinFestaSelectWordViewList )
  {
    m_pJoinFestaSelectWordViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaSelectWordViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaSelectWordViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaSelectWordViewList );
}


void JoinFestaSelectWordApplicationSystem::Build( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork, NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaSelectWordWork->GetAppHeap();

  m_pJoinFestaSelectWordWork               = pJoinFestaSelectWordWork;
  m_pJoinFestaSelectWordViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewList( pAppHeap );
  m_pJoinFestaSelectWordViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewObjects();
  m_pJoinFestaSelectWordViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor( m_pJoinFestaSelectWordViewList, m_pJoinFestaSelectWordViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaSelectWordViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void JoinFestaSelectWordApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_SELECT_WORD, GARC_join_festa_select_word_join_festa_select_word_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_MESSAGE_SET_PHRASE, print::GetMessageArcId(), GARC_message_setphrase_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_MESSAGE_JF_PHRASE, print::GetMessageArcId(), GARC_message_jf_phrase_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_INFO_TABLE, ARCID_JOIN_FESTA_SELECT_WORD_BIN, GARC_JoinFestaSelectWordBin_JoinFestaSelectWordTable_SelectWordInfoTable_BIN, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_CATEGORY_TABLE, ARCID_JOIN_FESTA_SELECT_WORD_BIN, GARC_JoinFestaSelectWordBin_JoinFestaSelectWordTable_CategoryTable_BIN, false, true );
}


void JoinFestaSelectWordApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_setphrase_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_MESSAGE_SET_PHRASE ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_phrase_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_MESSAGE_JF_PHRASE ) );
  m_pJoinFestaSelectWordWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaSelectWordApplicationSystem::CreateView()
{
  m_pJoinFestaSelectWordWork->SetupTable();

  m_pJoinFestaSelectWordViewObjects->CreateView( m_pJoinFestaSelectWordWork, *m_pNetAppMessageMenu );
}


void JoinFestaSelectWordApplicationSystem::UpdateView()
{
  m_pJoinFestaSelectWordViewList->Update();
}


void JoinFestaSelectWordApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaSelectWordViewList->Draw( displayNo );
}


bool JoinFestaSelectWordApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaSelectWordViewObjects )
  {
    if( m_pJoinFestaSelectWordViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
