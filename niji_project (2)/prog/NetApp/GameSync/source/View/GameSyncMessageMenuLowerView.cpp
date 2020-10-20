// ============================================================================
/*
 * @file GameSyncMessageMenuLowerView.cpp
 * @brief ゲームシンクアプリの下画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/View/GameSyncMessageMenuLowerView.h"
#include "NetApp/GameSync/source/GameSyncResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/gamesync/gamesync.h>
#include <niji_conv_header/app/gamesync/gamesync_anim_list.h>
#include <niji_conv_header/app/gamesync/gamesync_pane.h>
#include <niji_conv_header/message/msg_gamesync.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)

GameSyncMessageMenuLowerView::GameSyncMessageMenuLowerView(
  NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
  NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu ) :
  NetAppLib::System::NetApplicationViewBase(
    pGameSyncWork,
    NetApp::GameSync::GAMESYNC_RESOURCE_ID_LAYOUT,
    LYTRES_GAMESYNC_GAMESYNC_SELECT_LOW_000_BFLYT,
    LA_GAMESYNC_GAMESYNC_SELECT_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pGameSyncWork->GetMessageUtility(),
    GARC_message_gamesync_DAT ),
  m_pGameSyncWork( pGameSyncWork ),
  m_pGameSyncMessageMenuLowerViewListener( NULL ),
  m_NetAppMessageMenu( netAppMessageMenu ),
  m_MessageID( 0xffffffff )
{
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_GAMESYNC_GAMESYNC_SELECT_LOW_000_PATERN_02 );
  UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_LOWER, LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BG_WAVE_BLUE_UPP_00_BG_LOOP_00 );

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytPane* pMessage = pLower->GetPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_MESSAGE );
  gfl2::lyt::LytPane* BG0 = pLower->GetPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BG_WAVE_BLUE_UPP_00 );
  gfl2::lyt::LytPane* BG1 = pLower->GetPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_MENUBG );

  pMessage->SetVisible( false );
  BG0->SetVisible( false );
  BG1->SetVisible( false );

  m_NetAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_NetAppMessageMenu );
}


GameSyncMessageMenuLowerView::~GameSyncMessageMenuLowerView()
{
  m_NetAppMessageMenu.RemoveFromSuperView();
  m_NetAppMessageMenu.RemoveEventHandler();
}


void GameSyncMessageMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGameSyncWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


void GameSyncMessageMenuLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  switch( eEventCode )
  {
    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      if( m_pGameSyncMessageMenuLowerViewListener )
      {
        m_pGameSyncMessageMenuLowerViewListener->EndStreamMessage( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedYes:
    {
      if( m_pGameSyncMessageMenuLowerViewListener )
      {
        m_pGameSyncMessageMenuLowerViewListener->TouchYesButton( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedNo:
    {
      if( m_pGameSyncMessageMenuLowerViewListener )
      {
        m_pGameSyncMessageMenuLowerViewListener->TouchNoButton( m_MessageID );
      }
    }
    break;
  }
}


void GameSyncMessageMenuLowerView::SetMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, true, isShowTimerIcon, true );
}


void GameSyncMessageMenuLowerView::SetStreamMessage( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, false );
}


void GameSyncMessageMenuLowerView::SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, true );
}


void GameSyncMessageMenuLowerView::SetStreamMessageYesNoBlackFilter( u32 messageID )
{
  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( GARC_message_gamesync_DAT, messageID, false, GARC_message_gamesync_DAT, msg_gamesync_sel_07, msg_gamesync_sel_08 );
  m_MessageID = messageID;
}


void GameSyncMessageMenuLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pGameSyncWork->GetMessageUtility();
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetString( GARC_message_gamesync_DAT, messageID ), bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
