// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptApplicationSystem.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のアプリケーションシステムです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptApplicationSystem.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptViewList.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptViewObjects.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptViewSystemAccessor.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptBuilder.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/join_festa_playerlist.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)


// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_JoinFestaPlayerListSelectForScriptApplicationSystem
namespace FILELOCAL {
  enum  LoadSeq
  {
    LOADSEQ_Initial,
    LOADSEQ_Base,
    LOADSEQ_CommonIconData,
    LOADSEQ_Done,
    
    LOADSEQ_
  };
}

// =============================================================================
/**/
//==============================================================================
JoinFestaPlayerListSelectForScriptApplicationSystem::JoinFestaPlayerListSelectForScriptApplicationSystem() :
  m_pJoinFestaPlayerListSelectForScriptWork( NULL ),
  m_pJoinFestaPlayerListSelectForScriptViewList( NULL ),
  m_pJoinFestaPlayerListSelectForScriptViewObjects( NULL ),
  m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor( NULL ),
  m_LoadSeq(FILELOCAL::LOADSEQ_Initial),
  m_CommonIconData(),
  m_IsCommonIconDataTerminated(false)
{
}


JoinFestaPlayerListSelectForScriptApplicationSystem::~JoinFestaPlayerListSelectForScriptApplicationSystem()
{
  if( m_pJoinFestaPlayerListSelectForScriptViewList )
  {
    m_pJoinFestaPlayerListSelectForScriptViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectForScriptViewObjects );
  GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectForScriptViewList );
}


void JoinFestaPlayerListSelectForScriptApplicationSystem::Build( NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pJoinFestaPlayerListSelectForScriptWork->GetAppHeap();

  m_pJoinFestaPlayerListSelectForScriptWork               = pJoinFestaPlayerListSelectForScriptWork;
  m_pJoinFestaPlayerListSelectForScriptViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewList( pAppHeap );
  m_pJoinFestaPlayerListSelectForScriptViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewObjects();
  m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor( m_pJoinFestaPlayerListSelectForScriptViewList, m_pJoinFestaPlayerListSelectForScriptViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool JoinFestaPlayerListSelectForScriptApplicationSystem::Loading()
{
  switch(m_LoadSeq)
  {
    case FILELOCAL::LOADSEQ_Initial:
      m_LoadSeq = FILELOCAL::LOADSEQ_Base;
      /*  fall through  */

    case FILELOCAL::LOADSEQ_Base:
      if(!NetAppLib::System::ApplicationSystemBase::Loading())
      {
        m_LoadSeq = FILELOCAL::LOADSEQ_CommonIconData;
      }
      break;

    case FILELOCAL::LOADSEQ_CommonIconData:
      if(m_CommonIconData.IsReady())
      {
        m_LoadSeq = FILELOCAL::LOADSEQ_Done;
      }
      break;

    case FILELOCAL::LOADSEQ_Done:
    default:
      break;
  }


  return (m_LoadSeq != FILELOCAL::LOADSEQ_Done);
}


void JoinFestaPlayerListSelectForScriptApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT, ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST, print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA, print::GetMessageArcId(), GARC_message_jf_questiondata_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );

  /*  commonGrpIcon  */
  {
    m_CommonIconData.Initialize(m_pJoinFestaPlayerListSelectForScriptWork->GetAppHeap());
    m_pJoinFestaPlayerListSelectForScriptWork->SetCommonGrpIconData(&m_CommonIconData);
  }


  /*  font  */
  m_pResourceManager->FontLoadRequest(NetAppLib::System::ResourceManager::FONT_NUMBER);
}


void JoinFestaPlayerListSelectForScriptApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_playerlist_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_questiondata_DAT, m_pResourceManager->GetResourceBuffer( NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA ) );
  m_pJoinFestaPlayerListSelectForScriptWork->SetMessageUtility( m_pMessageUtility );
}


void JoinFestaPlayerListSelectForScriptApplicationSystem::CreateView()
{
  m_pJoinFestaPlayerListSelectForScriptViewObjects->CreateView( m_pJoinFestaPlayerListSelectForScriptWork, *m_pNetAppMessageMenu, *m_pNetAppTrainerIconUtility );
}


void JoinFestaPlayerListSelectForScriptApplicationSystem::UpdateView()
{
  m_pJoinFestaPlayerListSelectForScriptViewList->Update();
}


void JoinFestaPlayerListSelectForScriptApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pJoinFestaPlayerListSelectForScriptViewList->Draw( displayNo );
}


bool JoinFestaPlayerListSelectForScriptApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pJoinFestaPlayerListSelectForScriptViewObjects )
  {
    if( m_pJoinFestaPlayerListSelectForScriptViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  /*  commonGrpIcon  */
  if(!m_IsCommonIconDataTerminated)
  {
    m_CommonIconData.Terminate();
    m_IsCommonIconDataTerminated  = true;
  }

  return bExit;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool JoinFestaPlayerListSelectForScriptApplicationSystem::IsCreateViewComplete()
{
  bool  isEnd = true;
  
  
  isEnd &= m_pJoinFestaPlayerListSelectForScriptViewObjects->BattleFesUpperView_RadarChartInitFunc();
  isEnd &= m_pJoinFestaPlayerListSelectForScriptViewObjects->BattleFesUpperView_ObserveWordsetPreload();

  return isEnd;
}



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
