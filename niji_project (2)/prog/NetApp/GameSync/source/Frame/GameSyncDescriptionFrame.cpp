// ============================================================================
/*
 * @file GameSyncDescriptionFrame.cpp
 * @brief ゲームシンクアプリの○○プロセスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/Frame/GameSyncDescriptionFrame.h"
#include "NetApp/GameSync/source/GameSyncFrameResult.h"

#include "NetApp/GameSync/source/View/GameSyncBGUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncUppCaptureUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncLowCaptureUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncMainUpperView.h"

#include "NetApp/GameSync/source/View/GameSyncBGLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncTwoSelectMenuLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncThreeSelectMenuLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncMessageMenuLowerView.h"

#include <niji_conv_header/app/gamesync/gamesync_anim_list.h>
#include <niji_conv_header/message/msg_gamesync.h>
#include <arc_index/message.gaix>

#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(Frame)


GameSyncDescriptionFrame::GameSyncDescriptionFrame(
  NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
  NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor ) :
  NetApp::GameSync::Frame::GameSyncFrameBase( pGameSyncWork, pGameSyncViewSystemAccessor ),
  m_pInfoWindowBookTypeLowerView( NULL )
{
}


GameSyncDescriptionFrame::~GameSyncDescriptionFrame()
{
  if( m_pInfoWindowBookTypeLowerView )
  {
    m_pInfoWindowBookTypeLowerView->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE( m_pInfoWindowBookTypeLowerView );
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool GameSyncDescriptionFrame::startup()
{
  m_pGameSyncViewObjects->InputDisableAll();
  m_pGameSyncViewList->RemoveAllView();

  m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncBGUpperView() );
  m_pGameSyncViewList->AddLowerView( m_pGameSyncViewObjects->GetGameSyncBGLowerView() );


  Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveDataDirect();

  app::util::Heap* pAppHeap = m_pGameSyncWork->GetAppHeap();

  u32 descriptionType = m_pGameSyncWork->GetDescriptionType();

  u32 pageMax = 0;

  switch( descriptionType )
  {
    case msg_gamesync_sel_04:
    {
      pageMax = 3;
      gameSyncSaveData.m_config.m_isDispedGuide1 = true;
    }
    break;

    case msg_gamesync_sel_05:
    {
      pageMax = 4;
      gameSyncSaveData.m_config.m_isDispedGuide2 = true;
    }
    break;

    case msg_gamesync_sel_06:
    {
      pageMax = 4;
      gameSyncSaveData.m_config.m_isDispedGuide3 = true;
    }
    break;
  }
  

  m_pInfoWindowBookTypeLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( pAppHeap );
  m_pInfoWindowBookTypeLowerView->SyncCreate( pAppHeap, m_pGameSyncWork->GetAppRenderingManager(), pageMax, NULL );

  m_pGameSyncViewObjects->GetGameSyncBGLowerView()->AddSubView( m_pInfoWindowBookTypeLowerView );

  m_pInfoWindowBookTypeLowerView->ResetContents( pageMax );
  OnChangePage(0);

  m_pInfoWindowBookTypeLowerView->StartOpen( this, gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM );

  return true;
}


bool GameSyncDescriptionFrame::cleanup()
{
  return m_pInfoWindowBookTypeLowerView->EndFunc();
}


void GameSyncDescriptionFrame::setListener()
{
}


void GameSyncDescriptionFrame::removeListener()
{
}


void GameSyncDescriptionFrame::updateSequence()
{
}


//-----------------------------------------------------------------------------
// InfoWindowBookTypeListener
//-----------------------------------------------------------------------------
void GameSyncDescriptionFrame::OnClose()
{
  exitFrame( NetApp::GameSync::DESCRIPTION_FRAME_RESULT_BACK );
}


void GameSyncDescriptionFrame::OnChangePage( u32 pageNo )
{
  u32 descriptionType = m_pGameSyncWork->GetDescriptionType();

  u32 messageID = 0xffffffff;

  switch( descriptionType )
  {
    case msg_gamesync_sel_04:
    {
      switch( pageNo )
      {
        case 0: messageID = msg_gamesync_tutorial_01_01; break;
        case 1: messageID = msg_gamesync_tutorial_01_02; break;
        case 2: messageID = msg_gamesync_tutorial_01_03; break;
      }
    }
    break;

    case msg_gamesync_sel_05:
    {
      switch( pageNo )
      {
        case 0: messageID = msg_gamesync_tutorial_02_01; break;
        case 1: messageID = msg_gamesync_tutorial_02_02; break;
        case 2: messageID = msg_gamesync_tutorial_02_03; break;
        case 3: messageID = msg_gamesync_tutorial_02_04; break;
      }
    }
    break;

    case msg_gamesync_sel_06:
    {
      switch( pageNo )
      {
        case 0: messageID = msg_gamesync_tutorial_03_01; break;
        case 1: messageID = msg_gamesync_tutorial_03_02; break;
        case 2: messageID = msg_gamesync_tutorial_03_03; break;
        case 3: messageID = msg_gamesync_tutorial_03_04; break;
      }
    }
    break;
  }


  if( messageID != 0xffffffff )
  {
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pGameSyncWork->GetMessageUtility();
    m_pInfoWindowBookTypeLowerView->SetTextContent( pageNo, &pMessageUtility->GetString( GARC_message_gamesync_DAT, messageID ), NULL );
  }
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
