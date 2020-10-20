// ============================================================================
/*
 * @file GTSUploadPokemonLowerView.cpp
 * @brief ポケモンを預ける画面を表示するクラスです。
 * @date 2015.04.08
 */
// ============================================================================
#include "NetApp/GTS/source/UploadPokemon/GTSUploadPokemonLowerView.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(UploadPokemon)


static const u32 buttonPartsPaneIDTable[] =
{
  PANENAME_GTS_SLC_LOW_000_PANE_BTNM_00,
  PANENAME_GTS_SLC_LOW_000_PANE_BTNM_01,
  PANENAME_GTS_SLC_LOW_000_PANE_BTNM_02,
  PANENAME_GTS_SLC_LOW_000_PANE_BTNL_02,
  PANENAME_GTS_SLC_LOW_000_PANE_BTN_GREEN,
};

static const u32 cursorPaneIDTable[] =
{
  PANENAME_GTS_BTN_LOW_004_PANE_NULL_CUR,
  PANENAME_GTS_BTN_LOW_004_PANE_NULL_CUR,
  PANENAME_GTS_BTN_LOW_004_PANE_NULL_CUR,
  PANENAME_GTS_BTN_LOW_005_PANE_NULL_CUR,
  PANENAME_GTS_BTN_LOW_001_PANE_NULL_CUR,
};


GTSUploadPokemonLowerView::GTSUploadPokemonLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pUploadPokemonDrawListener( NULL ),
  m_SpaceString( 4, pGTSWork->GetDeviceHeap() )
{
  setupTextMessage();

  const app::tool::ButtonManager::ButtonId  buttonIDTable[] =
  {
    BUTTON_ID_BTNM_00,    //『欲しいポケモン』ボタン
    BUTTON_ID_BTNM_01,    //『せいべつ』ボタン
    BUTTON_ID_BTNM_02,    //『レベル』ボタン
    BUTTON_ID_BTNL_02,    //『メッセージ』ボタン
    BUTTON_ID_BTN_GREEN,  //『預ける』ボタン
  };

  CreateCursor( NetApp::GTS::GTS_RESOURCE_ID_MENU_CURSOR,
                LAYOUT_RESOURCE_ID_GTS,
                LAYOUT_WORK_ID_LOWER,
                buttonPartsPaneIDTable,
                cursorPaneIDTable,
                buttonIDTable,
                GFL_NELEMS( buttonPartsPaneIDTable ) );

  setupButton();

  SetVisibleBackButton( true );
  SetVisibleBlackFilter( false );
  SetInputListener( this );
}


GTSUploadPokemonLowerView::~GTSUploadPokemonLowerView()
{
  ReleaseCursor();
}


void GTSUploadPokemonLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  // 下画面のTextを置き換え
  {
    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

    setupLayout( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_SLC_LOW_000_PATERN_04 );

    // 『ほしいポケモン』ボタン
    {
      gfl2::lyt::LytParts* pBtnM_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_00 );
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnM_00, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_00, &gtsResourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_04 );
    }

    // 『せいべつ』ボタン
    {
      gfl2::lyt::LytParts* pBtnM_01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_01 );
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnM_01, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_00, &gtsResourceID );
      gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnM_01, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01, &gtsResourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_06 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, msg_gts_sel_12 );
    }

    // 『レベル』ボタン
    {
      gfl2::lyt::LytParts* pBtnM_02 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_02 );
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnM_02, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_00, &gtsResourceID );
      gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnM_02, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01, &gtsResourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_07 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, msg_gts_sel_12 );
    }

    // 『メッセージ』ボタン
    {
      gfl2::lyt::LytParts* pBtnL_02 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_02 );
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnL_02, PANENAME_GTS_BTN_LOW_005_PANE_TEXTBOX_00, &gtsResourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_introtext_01 );
    }

    // 『この条件で預ける』ボタン
    {
      gfl2::lyt::LytParts* pBtn_Green = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTN_GREEN );
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtn_Green, PANENAME_GTS_BTN_LOW_001_PANE_TEXTBOX_00, &gtsResourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_02 );
    }

  }
}


void GTSUploadPokemonLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BTN_BACK, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_PASSIVE,
    },
    {
      BUTTON_ID_BTNM_00, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNM_00_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNM_00_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_00_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNM_00_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_00_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_00_PASSIVE,
    },
    {
      BUTTON_ID_BTNM_01, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNM_01_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNM_01_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_01_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNM_01_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_01_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_01_PASSIVE,
    },
    {
      BUTTON_ID_BTNM_02, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNM_02_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNM_02_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_02_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNM_02_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_02_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNM_02_PASSIVE,
    },
    {
      BUTTON_ID_BTNL_02, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNL_02_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNL_02_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_02_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNL_02_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_02_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_02_PASSIVE,
    },
    {
      BUTTON_ID_BTN_GREEN, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTN_BACK );
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnM00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_00 );
    buttonInfoTable[ BUTTON_ID_BTNM_00 ].picture_pane = pBtnM00;
    buttonInfoTable[ BUTTON_ID_BTNM_00 ].bound_pane = pLower->GetBoundingPane( pBtnM00, PANENAME_GTS_BTN_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnM01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_01 );
    buttonInfoTable[ BUTTON_ID_BTNM_01 ].picture_pane = pBtnM01;
    buttonInfoTable[ BUTTON_ID_BTNM_01 ].bound_pane = pLower->GetBoundingPane( pBtnM01, PANENAME_GTS_BTN_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnM02 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_02 );
    buttonInfoTable[ BUTTON_ID_BTNM_02 ].picture_pane = pBtnM02;
    buttonInfoTable[ BUTTON_ID_BTNM_02 ].bound_pane = pLower->GetBoundingPane( pBtnM02, PANENAME_GTS_BTN_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnL02 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_02 );
    buttonInfoTable[ BUTTON_ID_BTNL_02 ].picture_pane = pBtnL02;
    buttonInfoTable[ BUTTON_ID_BTNL_02 ].bound_pane = pLower->GetBoundingPane( pBtnL02, PANENAME_GTS_BTN_LOW_005_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnGreen = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTN_GREEN );
    buttonInfoTable[ BUTTON_ID_BTN_GREEN ].picture_pane = pBtnGreen;
    buttonInfoTable[ BUTTON_ID_BTN_GREEN ].bound_pane = pLower->GetBoundingPane( pBtnGreen, PANENAME_GTS_BTN_LOW_001_PANE_BOUND_00, &gtsResourceID );
  }

  // 不要なボタンを非表示にする
  {
    gfl2::lyt::LytParts* pBtnS03 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNS_03 );
    pBtnS03->SetVisible( false );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_BACK, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNM_00, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNM_01, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNM_02, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNL_02, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_GREEN, SEQ_SE_DECIDE1 );

  RegisterShortcut_B( BUTTON_ID_BTN_BACK );
}


//-----------------------------------------------------------------------------
// UIView
//-----------------------------------------------------------------------------
void GTSUploadPokemonLowerView::OnAddChild()
{
  SetCursorVisible( false );
}


void GTSUploadPokemonLowerView::OnAddedToParent()
{
  SetCursorVisible( true );
}


void GTSUploadPokemonLowerView::OnRemoveChild()
{
  SetCursorVisible( true );
}


void GTSUploadPokemonLowerView::OnRemovedFromParent()
{
  SetCursorVisible( false );
}


void GTSUploadPokemonLowerView::Update( void )
{
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Update();
}


void GTSUploadPokemonLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Draw( displayNo );
}


//-----------------------------------------------------------------------------
// UIInputListener
//-----------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult GTSUploadPokemonLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    //『戻る』ボタン
    case BUTTON_ID_BTN_BACK:
    {
      if( m_pUploadPokemonDrawListener )
      {
        m_pUploadPokemonDrawListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    //『欲しいポケモン』ボタン
    case BUTTON_ID_BTNM_00:
    {
      if( m_pUploadPokemonDrawListener )
      {
        m_pUploadPokemonDrawListener->TouchPokemonButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    //『せいべつ』ボタン
    case BUTTON_ID_BTNM_01:
    {
      if( m_pUploadPokemonDrawListener )
      {
        m_pUploadPokemonDrawListener->TouchGenderButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    //『レベル』ボタン
    case BUTTON_ID_BTNM_02:
    {
      if( m_pUploadPokemonDrawListener )
      {
        m_pUploadPokemonDrawListener->TouchLevelButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    //『メッセージ』ボタン
    case BUTTON_ID_BTNL_02:
    {
      if( m_pUploadPokemonDrawListener )
      {
        m_pUploadPokemonDrawListener->TouchMessageButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    //『預ける』ボタン
    case BUTTON_ID_BTN_GREEN:
    {
      if( m_pUploadPokemonDrawListener )
      {
        m_pUploadPokemonDrawListener->TouchUploadButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


void GTSUploadPokemonLowerView::SetPokemonName( gfl2::str::StrBuf& inputName )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pBtnM_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_00 );
  gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnM_00, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01, &gtsResourceID );
  pG2DUtil->SetTextBoxPaneString( pTextBox_01, &inputName );
}


void GTSUploadPokemonLowerView::SetPokemon( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pBtnM_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_00 );
  gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnM_00, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01, &gtsResourceID );
  MonsNo monsno = static_cast<MonsNo>( messageID );
  if( monsno == MONSNO_NULL )
  {
    pG2DUtil->SetTextBoxPaneString( pTextBox_01, &m_SpaceString );
  }
  else
  {
    pG2DUtil->SetRegisterMonsNameNo( 0, monsno );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, msg_gts_sel_05 );
  }
}


void GTSUploadPokemonLowerView::SetGender( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pBtnM_01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_01 );
  gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnM_01, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01, &gtsResourceID );
  pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, messageID );
}


void GTSUploadPokemonLowerView::SetLevelBand( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pBtnM_02 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNM_02 );
  gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnM_02, PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01, &gtsResourceID );
  pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, messageID );
}


void GTSUploadPokemonLowerView::SetMessage( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pBtnL_02 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_02 );
  gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnL_02, PANENAME_GTS_BTN_LOW_005_PANE_TEXTBOX_00, &gtsResourceID );
  pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, messageID );
}


void GTSUploadPokemonLowerView::SetGenderButtonSE( bool bEnableSelectGender )
{
  app::tool::ButtonManager* pButtonManager = GetButtonManager();

  if( pButtonManager )
  {
    u32 seid = ( bEnableSelectGender ) ? SEQ_SE_DECIDE1 : SEQ_SE_CANCEL1;

    pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNM_01, seid );
  }
}


GFL_NAMESPACE_END(UploadPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
