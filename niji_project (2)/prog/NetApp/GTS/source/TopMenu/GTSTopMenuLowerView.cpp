// ============================================================================
/*
 * @file GTSTopMenuLowerView.cpp
 * @brief GTSのトップメニューを表示するクラスです。
 * @date 2015.03.18
 */
// ============================================================================
#include "NetApp/GTS/source/TopMenu/GTSTopMenuLowerView.h"
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
GFL_NAMESPACE_BEGIN(TopMenu)


static const u32 buttonPartsPaneIDTable[] =
{
  PANENAME_GTS_SLC_LOW_000_PANE_BTNSEARCH_00,
  PANENAME_GTS_SLC_LOW_000_PANE_BTNSEARCH_01,
};

static const u32 cursorPaneIDTable[] =
{
  PANENAME_COMMON_BTN_LOW_004_PANE_NULL_CUR,
  PANENAME_COMMON_BTN_LOW_004_PANE_NULL_CUR,
};


GTSTopMenuLowerView::GTSTopMenuLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pTopMenuDrawListener( NULL )
{
  setupTextMessage();

  const app::tool::ButtonManager::ButtonId  buttonIDTable[] =
  {
    BUTTON_ID_BTNSEARCH_00,
    BUTTON_ID_BTNSEARCH_01,
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


GTSTopMenuLowerView::~GTSTopMenuLowerView()
{
  ReleaseCursor();
}


void GTSTopMenuLowerView::setupTextMessage()
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  // 下画面のTextを置き換え
  {
    // アニメーションのIDはLayoutEditorで確認する必要がある
    setupLayout( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_SLC_LOW_000_PATERN_03 );

    // 画面上のテキスト
    {
      gfl2::lyt::LytParts* pMsgL2 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_MSG_L2 );
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMsgL2, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, &gtsResourceID );
      pTextBox->SetVisible( true );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, msg_gts_win_03 );
    }

    // ボタン内のText置き換え
    {
      gfl2::lyt::LytParts* pSearchButton1 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNSEARCH_00 );
      gfl2::lyt::LytParts* pSearchButton2 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNSEARCH_01 );

      gfl2::lyt::LytTextBox* pSearchButton1TextBox = pLower->GetTextBoxPane( pSearchButton1, PANENAME_COMMON_BTN_LOW_004_PANE_TEXTBOX_00, &gtsResourceID );
      gfl2::lyt::LytTextBox* pSearchButton2TextBox = pLower->GetTextBoxPane( pSearchButton2, PANENAME_COMMON_BTN_LOW_004_PANE_TEXTBOX_00, &gtsResourceID );

      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

      pG2DUtil->SetTextBoxPaneStringExpand( pSearchButton1TextBox, msg_gts_sel_01 ); // ポケモンを探す

      if( pGTSSaveData->GetDepositFlag() )
      {
        pml::pokepara::CoreParam uploadPokemonParam( pAppHeap->GetDeviceHeap() );
        pGTSSaveData->GetBackupPokemonCoreParam( &uploadPokemonParam );
        pG2DUtil->SetRegisterPokeNickName( 0, &uploadPokemonParam );
        pG2DUtil->SetTextBoxPaneStringExpand( pSearchButton2TextBox, msg_gts_sel_03 ); // ○○の様子を見る
      }
      else
      {
        pG2DUtil->SetTextBoxPaneStringExpand( pSearchButton2TextBox, msg_gts_sel_02 ); // ポケモンを預ける
      }
    }
  }
}


void GTSTopMenuLowerView::setupButton()
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
      BUTTON_ID_BTNSEARCH_00, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_00_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_00_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_00_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_00_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_00_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_00_PASSIVE,
    },
    {
      BUTTON_ID_BTNSEARCH_01, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_01_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_01_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_01_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_01_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_01_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNSEARCH_01_PASSIVE,
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
    gfl2::lyt::LytParts* pBtnSearch00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNSEARCH_00 );
    buttonInfoTable[ BUTTON_ID_BTNSEARCH_00 ].picture_pane = pBtnSearch00;
    buttonInfoTable[ BUTTON_ID_BTNSEARCH_00 ].bound_pane = pLower->GetBoundingPane( pBtnSearch00, PANENAME_COMMON_BTN_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnSearch01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNSEARCH_01 );
    buttonInfoTable[ BUTTON_ID_BTNSEARCH_01 ].picture_pane = pBtnSearch01;
    buttonInfoTable[ BUTTON_ID_BTNSEARCH_01 ].bound_pane = pLower->GetBoundingPane( pBtnSearch01, PANENAME_COMMON_BTN_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_BACK, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNSEARCH_00, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNSEARCH_01, SEQ_SE_DECIDE1 );

  RegisterShortcut_B( BUTTON_ID_BTN_BACK );
}


void GTSTopMenuLowerView::Update( void )
{
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Update();
}


void GTSTopMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Draw( displayNo );
}


::app::ui::UIInputListener::ListenerResult GTSTopMenuLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BTN_BACK:
    {
      if( m_pTopMenuDrawListener )
      {
        m_pTopMenuDrawListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BTNSEARCH_00:
    {
      if( m_pTopMenuDrawListener )
      {
        m_pTopMenuDrawListener->TouchSearchPokemonButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BTNSEARCH_01:
    {
      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();
      if( pGTSSaveData->GetDepositFlag() )
      {
        if( m_pTopMenuDrawListener )
        {
          m_pTopMenuDrawListener->TouchDownloadMyPokemonButton();
        }
      }
      else
      {
        if( m_pTopMenuDrawListener )
        {
          m_pTopMenuDrawListener->TouchUploadPokemonButton();
        }
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


GFL_NAMESPACE_END(TopMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
