// ============================================================================
/*
 * @file GameSyncThreeSelectMenuLowerView.cpp
 * @brief ゲームシンクアプリの下画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/View/GameSyncThreeSelectMenuLowerView.h"
#include "NetApp/GameSync/source/GameSyncResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/gamesync/gamesync.h>
#include <niji_conv_header/app/gamesync/gamesync_anim_list.h>
#include <niji_conv_header/app/gamesync/gamesync_pane.h>
#include <niji_conv_header/message/msg_gamesync.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)

static const u32 buttonPartsPaneIDTable[] =
{
  PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_00,
  PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_01,
  PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_02,
};

static const u32 cursorPaneIDTable[] =
{
  PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
  PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
  PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
};

GameSyncThreeSelectMenuLowerView::GameSyncThreeSelectMenuLowerView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork ) :
  NetAppLib::UI::NetAppCursorView( pGameSyncWork ),
  m_pGameSyncWork( pGameSyncWork ),
  m_pGameSyncThreeSelectMenuLowerViewListener( NULL ),
  m_WindowMessageID( 0xffffffff ),
  m_MenuMessageID1( 0xffffffff ),
  m_MenuMessageID2( 0xffffffff ),
  m_MenuMessageID3( 0xffffffff )
{
  Create2DOneLayout( pGameSyncWork,
                     NetApp::GameSync::GAMESYNC_RESOURCE_ID_LAYOUT,
                     LYTRES_GAMESYNC_GAMESYNC_SELECT_LOW_000_BFLYT,
                     LA_GAMESYNC_GAMESYNC_SELECT_LOW_000___NUM,
                     app::util::G2DUtil::SETUP_LOWER,
                     pGameSyncWork->GetMessageUtility(),
                     GARC_message_gamesync_DAT );

  setupLayout( LAYOUT_WORK_ID_LOWER, LA_GAMESYNC_GAMESYNC_SELECT_LOW_000_PATERN_01 );
  UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_LOWER, LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BG_WAVE_BLUE_UPP_00_BG_LOOP_00 );

  { // @fix GFMMCat[25]: Newアイコンのアニメを再生
    static const u32 tbl[] =
    {
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__NEWICON_00_LOOP,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__NEWICON_01_LOOP,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__NEWICON_02_LOOP,
    };
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_LOWER, tbl[i] );
    }
  }

  const app::tool::ButtonManager::ButtonId  buttonIDTable[] =
  {
    BUTTON_ID_BUTTON_00,
    BUTTON_ID_BUTTON_01,
    BUTTON_ID_BUTTON_02,
  };

  CreateCursor(NetApp::GameSync::GAMESYNC_RESOURCE_ID_MENU_CURSOR,
                LAYOUT_RESOURCE_ID_GAMESYNC,
                LAYOUT_WORK_ID_LOWER,
                buttonPartsPaneIDTable,
                cursorPaneIDTable,
                buttonIDTable,
                GFL_NELEMS( buttonPartsPaneIDTable ) );

  setupButton();

  SetInputListener( this );
}


GameSyncThreeSelectMenuLowerView::~GameSyncThreeSelectMenuLowerView()
{
  ReleaseCursor();
}


void GameSyncThreeSelectMenuLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BACKBUTTON_TOUCH,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BACKBUTTON_RELEASE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BACKBUTTON_CANSEL,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BACKBUTTON_ACTIVE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BUTTON_ID_BUTTON_00, NULL, NULL,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_00_TOUCH,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_00_RELEASE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_00_CANSEL,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_00_TOUCH_RELEASE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_00_ACTIVE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_00_PASSIVE,
    },
    {
      BUTTON_ID_BUTTON_01, NULL, NULL,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_01_TOUCH,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_01_RELEASE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_01_CANSEL,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_01_TOUCH_RELEASE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_01_ACTIVE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_01_PASSIVE,
    },
    {
      BUTTON_ID_BUTTON_02, NULL, NULL,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_02_TOUCH,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_02_RELEASE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_02_CANSEL,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_02_TOUCH_RELEASE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_02_ACTIVE,
      LA_GAMESYNC_GAMESYNC_SELECT_LOW_000__BUTTON_02_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GAMESYNC );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnS00 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_00 );
    buttonInfoTable[ BUTTON_ID_BUTTON_00 ].picture_pane = pBtnS00;
    buttonInfoTable[ BUTTON_ID_BUTTON_00 ].bound_pane = pLower->GetBoundingPane( pBtnS00, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnS01 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_01 );
    buttonInfoTable[ BUTTON_ID_BUTTON_01 ].picture_pane = pBtnS01;
    buttonInfoTable[ BUTTON_ID_BUTTON_01 ].bound_pane = pLower->GetBoundingPane( pBtnS01, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnS02 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_02 );
    buttonInfoTable[ BUTTON_ID_BUTTON_02 ].picture_pane = pBtnS02;
    buttonInfoTable[ BUTTON_ID_BUTTON_02 ].bound_pane = pLower->GetBoundingPane( pBtnS02, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pGameSyncWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BUTTON_00, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BUTTON_01, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BUTTON_02, SEQ_SE_DECIDE1 );

  RegisterShortcut_B( BUTTON_ID_BACK_BUTTON );
}


void GameSyncThreeSelectMenuLowerView::SetMessage( u32 windowMessageID, u32 button1MessageID, bool bShowNewIcon1, u32 button2MessageID, bool bShowNewIcon2, u32 button3MessageID, bool bShowNewIcon3 )
{
  m_WindowMessageID = windowMessageID;
  m_MenuMessageID1 = button1MessageID;
  m_MenuMessageID2 = button2MessageID;
  m_MenuMessageID3 = button3MessageID;

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GAMESYNC );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  
  app::tool::AppCommonGrpIconData* pAppCommonGrpIconData = m_pGameSyncWork->GetAppCommonGrpIconData();

  {
    gfl2::lyt::LytParts* pMsgL1 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_MESSAGE );
    gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMsgL1, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, &gtsResourceID );
    pTextBox->SetVisible( true );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, windowMessageID );
  }

  {
    gfl2::lyt::LytParts* pBtnS_00 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_00 );
    gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnS_00, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &gtsResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, button1MessageID );

    gfl2::lyt::LytParts* pNewIcon_00 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_NEWICON_00 );
    gfl2::lyt::LytPicture* pNewIconPicture = pLower->GetPicturePane( pNewIcon_00, PANENAME_COMMON_ICN_013_PANE_NEW_ICON_01, &gtsResourceID );
    pAppCommonGrpIconData->ReplacePaneTextureByNewIcon( pNewIconPicture );
    pNewIcon_00->SetVisible( bShowNewIcon1 );
  }

  {
    gfl2::lyt::LytParts* pBtnS_01 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_01 );
    gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnS_01, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &gtsResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, button2MessageID );

    gfl2::lyt::LytParts* pNewIcon_01 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_NEWICON_01 );
    gfl2::lyt::LytPicture* pNewIconPicture = pLower->GetPicturePane( pNewIcon_01, PANENAME_COMMON_ICN_013_PANE_NEW_ICON_01, &gtsResourceID );
    pAppCommonGrpIconData->ReplacePaneTextureByNewIcon( pNewIconPicture );
    pNewIcon_01->SetVisible( bShowNewIcon2 );
  }

  {
    gfl2::lyt::LytParts* pBtnS_02 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_BUTTON_02 );
    gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnS_02, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &gtsResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, button3MessageID );

    gfl2::lyt::LytParts* pNewIcon_02 = pLower->GetPartsPane( PANENAME_GAMESYNC_SELECT_LOW_000_PANE_NEWICON_02 );
    gfl2::lyt::LytPicture* pNewIconPicture = pLower->GetPicturePane( pNewIcon_02, PANENAME_COMMON_ICN_013_PANE_NEW_ICON_01, &gtsResourceID );
    pAppCommonGrpIconData->ReplacePaneTextureByNewIcon( pNewIconPicture );
    pNewIcon_02->SetVisible( bShowNewIcon3 );
  }
}


void GameSyncThreeSelectMenuLowerView::Update()
{
  NetAppLib::UI::NetAppCursorView::Update();
}


void GameSyncThreeSelectMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGameSyncWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  NetAppLib::UI::NetAppCursorView::Draw( displayNo );
}


::app::ui::UIInputListener::ListenerResult GameSyncThreeSelectMenuLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pGameSyncThreeSelectMenuLowerViewListener )
      {
        m_pGameSyncThreeSelectMenuLowerViewListener->TouchBackButton( m_WindowMessageID );
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BUTTON_00:
    {
      if( m_pGameSyncThreeSelectMenuLowerViewListener )
      {
        m_pGameSyncThreeSelectMenuLowerViewListener->TouchSelectMenu( m_WindowMessageID, m_MenuMessageID1 );
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BUTTON_01:
    {
      if( m_pGameSyncThreeSelectMenuLowerViewListener )
      {
        m_pGameSyncThreeSelectMenuLowerViewListener->TouchSelectMenu( m_WindowMessageID, m_MenuMessageID2 );
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BUTTON_02:
    {
      if( m_pGameSyncThreeSelectMenuLowerViewListener )
      {
        m_pGameSyncThreeSelectMenuLowerViewListener->TouchSelectMenu( m_WindowMessageID, m_MenuMessageID3 );
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
