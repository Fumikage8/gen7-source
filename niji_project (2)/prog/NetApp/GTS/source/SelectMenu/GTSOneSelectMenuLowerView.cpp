// ============================================================================
/*
 * @file GTSOneSelectMenuLowerView.cpp
 * @brief 一つの選択ボタンと戻るボタンを表示するクラスです。
 * @date 2015.05.12
 */
// ============================================================================
#include "NetApp/GTS/source/SelectMenu/GTSOneSelectMenuLowerView.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SelectMenu)

static const u32 buttonPartsPaneIDTable[] =
{
  PANENAME_GTS_SLC_LOW_000_PANE_BTNS_00,
};

static const u32 cursorPaneIDTable[] =
{
  PANENAME_GTS_BTN_LOW_001_PANE_NULL_CUR,
};

GTSOneSelectMenuLowerView::GTSOneSelectMenuLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pOneSelectMenuDrawListener( NULL )
{
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_SLC_LOW_000_PATERN_00 );

  const app::tool::ButtonManager::ButtonId  buttonIDTable[] =
  {
    BUTTON_ID_BTNS_00,
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


GTSOneSelectMenuLowerView::~GTSOneSelectMenuLowerView()
{
  ReleaseCursor();
}


void GTSOneSelectMenuLowerView::setupButton()
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
      BUTTON_ID_BTNS_00, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_PASSIVE,
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
    gfl2::lyt::LytParts* pBtnS_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNS_00 );
    buttonInfoTable[ BUTTON_ID_BTNS_00 ].picture_pane = pBtnS_00;
    buttonInfoTable[ BUTTON_ID_BTNS_00 ].bound_pane = pLower->GetBoundingPane( pBtnS_00, PANENAME_GTS_BTN_LOW_001_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_BACK, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNS_00, SEQ_SE_DECIDE1 );

  RegisterShortcut_B( BUTTON_ID_BTN_BACK );
}


void GTSOneSelectMenuLowerView::SetMessage( u32 windowMessageID, u32 buttonMessageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pMsgL1 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_MSG_L1 );
    gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMsgL1, PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, &gtsResourceID );
    pTextBox->SetVisible( true );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, windowMessageID );
  }

  {
    gfl2::lyt::LytParts* pBtnS_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNS_00 );
    gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnS_00, PANENAME_GTS_BTN_LOW_001_PANE_TEXTBOX_00, &gtsResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, buttonMessageID );
  }
}


void GTSOneSelectMenuLowerView::SetMessageFromDownloadMyPokemon()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

  pml::pokepara::CoreParam uploadPokemonParam( pAppHeap->GetDeviceHeap() );

  pGTSSaveData->GetBackupPokemonCoreParam( &uploadPokemonParam );


  // 下画面のTextを置き換え
  {
    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );


    {
      gfl2::lyt::LytParts* pMsgL1 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_MSG_L1 );
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMsgL1, PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, &gtsResourceID );
      pTextBox->SetVisible( true );
      pG2DUtil->SetRegisterPokeNickName( 0, &uploadPokemonParam );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, msg_gts_win_19 );
    }

    {
      gfl2::lyt::LytParts* pBtnS_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNS_00 );
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnS_00, PANENAME_GTS_BTN_LOW_001_PANE_TEXTBOX_00, &gtsResourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_36 );
    }
  }
}


//-----------------------------------------------------------------------------
// UIView
//-----------------------------------------------------------------------------
void GTSOneSelectMenuLowerView::OnAddChild()
{
  SetCursorVisible( false );
}


void GTSOneSelectMenuLowerView::OnAddedToParent()
{
  SetCursorVisible( true );
}


void GTSOneSelectMenuLowerView::OnRemoveChild()
{
  SetCursorVisible( true );
}


void GTSOneSelectMenuLowerView::OnRemovedFromParent()
{
  SetCursorVisible( false );
}


void GTSOneSelectMenuLowerView::Update( void )
{
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Update();
}


void GTSOneSelectMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Draw( displayNo );
}


//-----------------------------------------------------------------------------
// UIInputListener
//-----------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult GTSOneSelectMenuLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    //『戻る』ボタン
    case BUTTON_ID_BTN_BACK:
    {
      if( m_pOneSelectMenuDrawListener )
      {
        m_pOneSelectMenuDrawListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BTNS_00:
    {
      if( m_pOneSelectMenuDrawListener )
      {
        m_pOneSelectMenuDrawListener->TouchSelectButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

  }

  return result;
}


GFL_NAMESPACE_END(SelectMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
