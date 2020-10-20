// ============================================================================
/*
 * @file JoinFestaProfileLowerView.cpp
 * @brief プロフィールアプリの下画面を表示するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileLowerView.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_profile/join_festa_profile.h>
#include <niji_conv_header/app/join_festa_profile/join_festa_profile_anim_list.h>
#include <niji_conv_header/app/join_festa_profile/join_festa_profile_pane.h>
#include <niji_conv_header/message/msg_jf_myprofile.h>
#include <niji_conv_header/message/msg_jf_phrase.h>

#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(View)


static const gfl2::lyt::LytPaneIndex btns_tbl[JoinFestaProfileLowerView::LIST_ITEM_PANE_MAX] =
{
  PANENAME_JFP_MIN_LOW_000_PANE_PARTS_13,
  PANENAME_JFP_MIN_LOW_000_PANE_PARTS_14,
  PANENAME_JFP_MIN_LOW_000_PANE_PARTS_15,
  PANENAME_JFP_MIN_LOW_000_PANE_PARTS_16,
  PANENAME_JFP_MIN_LOW_000_PANE_PARTS_17
};

static const u32 anm[JoinFestaProfileLowerView::LIST_ITEM_PANE_MAX][4] =
{
  {
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_13_TOUCH,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_13_RELEASE,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_13_CANSEL,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_13_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_14_TOUCH,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_14_RELEASE,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_14_CANSEL,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_14_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_15_TOUCH,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_15_RELEASE,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_15_CANSEL,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_15_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_16_TOUCH,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_16_RELEASE,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_16_CANSEL,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_16_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_17_TOUCH,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_17_RELEASE,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_17_CANSEL,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_17_TOUCH_RELEASE
  },
};


JoinFestaProfileLowerView::JoinFestaProfileLowerView( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaProfileWork,
    NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000_BFLYT,
    LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaProfileWork->GetMessageUtility(),
    GARC_message_jf_myprofile_DAT ),
  m_pJoinFestaProfileWork( pJoinFestaProfileWork ),
  m_pJoinFestaProfileLowerViewListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_pMyPaneListData( NULL ),
  m_MenuCursor( pJoinFestaProfileWork ),
  m_NullString( 4, pJoinFestaProfileWork->GetDeviceHeap() ),
  m_pLikePokemonTextPane( NULL ),
  m_pHonorTextPane( NULL ),
  m_pFestaNameTextPane( NULL ),
  m_SelectListIndex( 0 ),
  m_bPaneListInputEnable( false )
{
  m_MenuCursor.Create( GetG2DUtil(), LAYOUT_RESOURCE_ID_JOINFESTAPROFILE, LAYOUT_WORK_ID_LOWER, NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_MENU_CURSOR );

  setupTextMessage();
  setupButton();

  createPaneList();

  SetInputListener( this );

  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFP_MIN_LOW_000_PANE_INFOBUTTON );
    pButton->SetVisible( true );
  }
}


JoinFestaProfileLowerView::~JoinFestaProfileLowerView()
{
  deletePaneList();

  m_MenuCursor.Destroy();
}


bool JoinFestaProfileLowerView::IsDrawing(void) const
{
  bool result = false;
  
  if( !m_MenuCursor.CanDestroy() )
  {
    result = true;
  }

  if( UIView::IsDrawing() )
  {
    result = true;
  }

  return result;
}


void JoinFestaProfileLowerView::OnAddChild( void )
{
  setInputEnablePaneList( false );
}


void JoinFestaProfileLowerView::OnRemoveChild( void )
{
  setInputEnablePaneList( true );
}


void JoinFestaProfileLowerView::Update()
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_MenuCursor.Update();
}


void JoinFestaProfileLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaProfileWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  m_MenuCursor.Draw( displayNo );
}


void JoinFestaProfileLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaProfileWork->GetMessageUtility();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

  // [プレイヤー名]のプロフィール
  {
    gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( PANENAME_JFP_MIN_LOW_000_PANE_PAGE_TITLE_00 );
    pG2DUtil->SetRegisterPlayerName( 0, pMyStatus );
    pG2DUtil->SetTextBoxPaneStringExpand( pText, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_11 ) );
  }
}


void JoinFestaProfileLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_00_TOUCH,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_00_RELEASE,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_00_CANSEL,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_00_TOUCH_RELEASE,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_00_ACTIVE,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__PARTS_00_PASSIVE,
    },

    {
      BUTTON_ID_INFO_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__INFOBUTTON_TOUCH,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__INFOBUTTON_RELEASE,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__INFOBUTTON_CANSEL,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__INFOBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__INFOBUTTON_ACTIVE,
      LA_JOIN_FESTA_PROFILE_JFP_MIN_LOW_000__INFOBUTTON_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPROFILE );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_JFP_MIN_LOW_000_PANE_PARTS_00 );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pInfoButton = pLower->GetPartsPane( PANENAME_JFP_MIN_LOW_000_PANE_INFOBUTTON );
    buttonInfoTable[ BUTTON_ID_INFO_BUTTON ].picture_pane = pInfoButton;
    buttonInfoTable[ BUTTON_ID_INFO_BUTTON ].bound_pane = pLower->GetBoundingPane( pInfoButton, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pButtonA = pLower->GetPartsPane( PANENAME_JFP_MIN_LOW_000_PANE_PARTS_01 );
    pButtonA->SetVisible( false );
  }

  app::util::Heap* pAppHeap = m_pJoinFestaProfileWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_INFO_BUTTON, SEQ_SE_DECIDE1 );
}


void JoinFestaProfileLowerView::createPaneList()
{
  app::util::Heap* pAppHeap = m_pJoinFestaProfileWork->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPROFILE );

  // -----------------------------------------------------------------------
  { // m_pPaneListData 初期化 ここから

    m_pPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];
    m_pMyPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) MyPaneData[ LIST_ITEM_PANE_MAX ];
   
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      gfl2::lyt::LytParts* pBTNS = pLower->GetPartsPane( btns_tbl[i] );
      m_pPaneListData[i].base       = pBTNS;
      m_pPaneListData[i].text       = pLower->GetTextBoxPane( pBTNS, PANENAME_FESTA_BTN_LOW_005_PANE_TEXTBOX_00, &resourceID );
      m_pPaneListData[i].bound      = pLower->GetBoundingPane( pBTNS, PANENAME_FESTA_BTN_LOW_005_PANE_BOUND_00, &resourceID );
      m_pPaneListData[i].cursor_pos = pLower->GetPane( pBTNS, PANENAME_FESTA_BTN_LOW_005_PANE_NULL_CUR, &resourceID );

      m_pPaneListData[i].holdAnime    = anm[i][0];
      m_pPaneListData[i].releaseAnime = anm[i][1];
      m_pPaneListData[i].cancelAnime  = anm[i][2];
      m_pPaneListData[i].selectAnime  = anm[i][3];

      m_pMyPaneListData[i].pTextBox00 = pLower->GetTextBoxPane( pBTNS, PANENAME_FESTA_BTN_LOW_005_PANE_TEXTBOX_00, &resourceID );
      m_pMyPaneListData[i].pTextBox01 = pLower->GetTextBoxPane( pBTNS, PANENAME_FESTA_BTN_LOW_005_PANE_TEXTBOX_01, &resourceID );
    }
  } // m_pPaneListData 初期化 ここまで
  // -----------------------------------------------------------------------

  // -----------------------------------------------------------------------
  { // m_pPaneListView 初期化 ここから

    NetApp::JoinFestaProfile::System::JoinFestaProfileWork::PaneListCursorInfo& paneListCursorInfo = m_pJoinFestaProfileWork->GetPaneListCursorInfo();

    app::tool::PaneList::SETUP_PARAM list_setup;
    setPaneListParam( &list_setup, paneListCursorInfo.pos, paneListCursorInfo.scroll );

    app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
    {
      pAppHeap->GetDeviceHeap(),
      app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
      list_setup.valMax-list_setup.listPosMax,

      pLower,
      resourceID,
      PANENAME_JFP_MIN_LOW_000_PANE_SCROLL_BAR,
      PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

      NULL,
      NULL,
      NULL,
      NULL
    };

    m_pPaneListView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::PaneListView( pAppHeap, true );
    m_pPaneListView->CreatePaneList( &list_setup );
    m_pPaneListView->CreateScrollBar( &scroll_bar_setup );
    m_pPaneListView->SetListener( this );
    setPaneListConfig( &list_setup );
    AddSubView( m_pPaneListView );
    setInputEnablePaneList( true );

  } // m_pPaneListView 初期化 ここまで
  // -----------------------------------------------------------------------
}


void JoinFestaProfileLowerView::deletePaneList()
{
  {
    m_pPaneListView->RemoveFromSuperView();
    m_pPaneListView->DeleteScrollBar();
    m_pPaneListView->DeletePaneList();
    GFL_SAFE_DELETE( m_pPaneListView );
  }

  {
    GFL_SAFE_DELETE_ARRAY( m_pMyPaneListData );
    GFL_SAFE_DELETE_ARRAY( m_pPaneListData );
  }
}


void JoinFestaProfileLowerView::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll )
{
  app::util::Heap* pAppHeap = m_pJoinFestaProfileWork->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  gfl2::util::List<NetApp::JoinFestaProfile::System::EditMenuInfo>& editMenuList = m_pJoinFestaProfileWork->GetEditMenuList();

  pParam->valMax = editMenuList.Size();					// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = 3;			// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 62.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                          //   この座標で指定した場所にあるペインが描画開始位置のペインになる
  pParam->defSX = 308.0f;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  pParam->defSY = 50.0f;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  pParam->writeOffsetY = -1;		// 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  pParam->listStartPane = NULL;   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  pParam->cbFunc = this;	// コールバック関数
  pParam->cbWork = NULL;			// コールバック関数に渡すワーク

  pParam->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  pParam->menuCursor = m_MenuCursor.GetAppToolMenuCursor();		// カーソル
}


void JoinFestaProfileLowerView::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
{
  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_BUTTON, true, false, true, false );
  pPaneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );

  { // タッチ範囲
    gfl2::math::VEC3 pos = m_pPaneListData[0].bound->GetTranslate();
    u32 pos_max = pParam->listPosMax;
    if( pParam->valMax < pos_max )
    {
      pos_max = pParam->valMax;
    }

    f32 lx = 5.0f - pParam->defSX / 2.0f;
    f32 rx = lx + pParam->defSX;
    f32 uy = pParam->defPY + pos.y + pParam->defSY / 2.0f;
    f32 dy = uy - pParam->defSY * pos_max;
    pPaneList->MakeTouchArea( lx, rx, uy, dy );
  }

}


void JoinFestaProfileLowerView::setInputEnablePaneList( bool bInputEnable )
{
  m_bPaneListInputEnable = bInputEnable;

  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      pPaneList->SetInputEnabled( bInputEnable );
    }
  }
}


void JoinFestaProfileLowerView::backupPaneListCursorInfo()
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    
    if( pPaneList )
    {
      NetApp::JoinFestaProfile::System::JoinFestaProfileWork::PaneListCursorInfo& paneListCursorInfo = m_pJoinFestaProfileWork->GetPaneListCursorInfo();

      pPaneList->GetCursorData( &paneListCursorInfo.pos, &paneListCursorInfo.scroll );
    }
  }
}


::app::ui::UIInputListener::ListenerResult JoinFestaProfileLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaProfileLowerViewListener )
      {
        m_pJoinFestaProfileLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_INFO_BUTTON:
    {
      if( m_pJoinFestaProfileLowerViewListener )
      {
        m_pJoinFestaProfileLowerViewListener->TouchInfoButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaProfileLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_BACK_BUTTON );
    result = DISABLE_ACCESS;
  }

  return result;
}


void JoinFestaProfileLowerView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
}


void JoinFestaProfileLowerView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void JoinFestaProfileLowerView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaProfileWork->GetMessageUtility();

  NetApp::JoinFestaProfile::System::EditMenuInfo* pEditMenuInfo = getEditMenuInfo( pos );

  if( pEditMenuInfo )
  {
    Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

    pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, pEditMenuInfo->editMenuMessageID ) );

    switch( pEditMenuInfo->editMenuID )
    {
      // -------------------------------------------------------
      // 出会いの挨拶
      case EDIT_MENU_ID_MEETING_MESSAGE:
      {
        u32 messageID = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_HELLO );
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_phrase_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // 感動した時
      case EDIT_MENU_ID_THROB:
      {
        u32 messageID = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_EMOTION );
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_phrase_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // 残念な時
      case EDIT_MENU_ID_REGRETFUL:
      {
        u32 messageID = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_REGRET );
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_phrase_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // 別れの挨拶
      case EDIT_MENU_ID_GOODBY_MESSAGE:
      {
        u32 messageID = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_PARTING );
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_phrase_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // 私の秘密
      case EDIT_MENU_ID_MY_SECRET:
      {
        u32 messageID = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_SECRET );
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_phrase_DAT, messageID ) );
      }
      break;

#if defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )
      // -------------------------------------------------------
      // 好きなポケモン
      case EDIT_MENU_ID_LIKE_POKEMON:
      {
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &m_pJoinFestaProfileWork->GetLikePokemonName() );
        m_pLikePokemonTextPane = m_pMyPaneListData[ pane_index ].pTextBox01;
      }
      break;

      // -------------------------------------------------------
      // 肩書き
      case EDIT_MENU_ID_HONOR:
      {
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &m_pJoinFestaProfileWork->GetHonorString() );
        m_pHonorTextPane = m_pMyPaneListData[ pane_index ].pTextBox01;
      }
      break;
#endif // defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )

      // -------------------------------------------------------
      // ジョインフェスタの名前
      case EDIT_MENU_ID_NAME:
      {
        gfl2::str::StrBuf& joinFestaName = m_pJoinFestaProfileWork->GetJoinFestaName();
        
        pJoinFestaDataSave->GetJoinFestaNameString( &joinFestaName );

        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &joinFestaName );
        m_pFestaNameTextPane = m_pMyPaneListData[ pane_index ].pTextBox01;
      }
      break;

      // -------------------------------------------------------
      // 対戦の申し込み
      case EDIT_MENU_ID_BATTLE:
      {
        bool bFlag = pJoinFestaDataSave->IsAlwaysBattleOk();
        u32 messageID = ( bFlag ) ? pEditMenuInfo->selectMenuID0 : pEditMenuInfo->selectMenuID1;
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // 交換の申し込み
      case EDIT_MENU_ID_TRADE:
      {
        bool bFlag = pJoinFestaDataSave->IsAlwaysTradeOk();
        u32 messageID = ( bFlag ) ? pEditMenuInfo->selectMenuID0 : pEditMenuInfo->selectMenuID1;
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // アトラクションへの意気込み
      case EDIT_MENU_ID_ATTRACTION:
      {
        bool bFlag = pJoinFestaDataSave->IsAlwaysAttractionOk();
        u32 messageID = ( bFlag ) ? pEditMenuInfo->selectMenuID0 : pEditMenuInfo->selectMenuID1;
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // ゲストとの通信
      case EDIT_MENU_ID_GUEST:
      {
        bool bFlag = pJoinFestaDataSave->IsRejectGuest();
        u32 messageID = ( bFlag ) ? pEditMenuInfo->selectMenuID1 : pEditMenuInfo->selectMenuID0;
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // VIPとの通信
      case EDIT_MENU_ID_VIP:
      {
        bool bFlag = pJoinFestaDataSave->IsRejectVip();
        u32 messageID = ( bFlag ) ? pEditMenuInfo->selectMenuID1 : pEditMenuInfo->selectMenuID0;
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ) );
      }
      break;

      // -------------------------------------------------------
      // フレンドとの通信
      case EDIT_MENU_ID_FRIEND:
      {
        bool bFlag = pJoinFestaDataSave->IsRejectFriend();
        u32 messageID = ( bFlag ) ? pEditMenuInfo->selectMenuID1 : pEditMenuInfo->selectMenuID0;
        pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ) );
      }
      break;
    }
  }
}


void JoinFestaProfileLowerView::UpdateLikePokemonTextPane()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  if( m_pLikePokemonTextPane )
  {
    pG2DUtil->SetTextBoxPaneString( m_pLikePokemonTextPane, &m_pJoinFestaProfileWork->GetLikePokemonName() );
  }
}


void JoinFestaProfileLowerView::UpdateHonorTextPane()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  if( m_pHonorTextPane )
  {
    pG2DUtil->SetTextBoxPaneString( m_pHonorTextPane, &m_pJoinFestaProfileWork->GetHonorString() );
  }
}


void JoinFestaProfileLowerView::UpdateFestaNameTextPane()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  if( m_pFestaNameTextPane )
  {
    pG2DUtil->SetTextBoxPaneString( m_pFestaNameTextPane, &m_pJoinFestaProfileWork->GetJoinFestaName() );
  }
}


void JoinFestaProfileLowerView::UpdateOKNGTextPane()
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      u32 cursorPos = 0;
      pPaneList->GetCursorData( &cursorPos, NULL );
      u32 paneIndex = pPaneList->GetPosPaneIndex( cursorPos );
      if( paneIndex < LIST_ITEM_PANE_MAX )
      {
        CallBack_ListWrite( NULL, m_pPaneListData, paneIndex, m_SelectListIndex );

        GFL_PRINT( "cursorPos = %d\n", cursorPos );
        GFL_PRINT( "m_SelectListIndex = %d\n", m_SelectListIndex );
      }
    }
  }
}


void JoinFestaProfileLowerView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void JoinFestaProfileLowerView::PaneListItemSelectListener( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( !m_bPaneListInputEnable )
  {
    return;
  }

  backupPaneListCursorInfo();

  NetApp::JoinFestaProfile::System::EditMenuInfo* pEditMenuInfo = getEditMenuInfo( pos );

  if( pEditMenuInfo )
  {
    if( m_pJoinFestaProfileLowerViewListener )
    {
      m_pJoinFestaProfileLowerViewListener->SetSelectEditMenu( pEditMenuInfo );

      m_SelectListIndex = pos;
    }
  }
}


NetApp::JoinFestaProfile::System::EditMenuInfo* JoinFestaProfileLowerView::getEditMenuInfo( u32 index )
{
  gfl2::util::List<NetApp::JoinFestaProfile::System::EditMenuInfo>& editMenuList = m_pJoinFestaProfileWork->GetEditMenuList();

  u32 count = 0;

  for( gfl2::util::List<NetApp::JoinFestaProfile::System::EditMenuInfo>::Iterator it = editMenuList.Begin(); it != editMenuList.End(); ++it )
  {
    if( count == index )
    {
      return &(*it);
    }
    ++count;
  }

  return NULL;
}

// @fix GFMMCat[27]: インフォボタンの座標取得
void JoinFestaProfileLowerView::GetInfoButtonPos( gfl2::math::Vector3 * vec )
{
  gfl2::math::VEC3 pos = GetButtonManager()->GetButtonPos( BUTTON_ID_INFO_BUTTON );
  *vec = gfl2::math::ConvertNwVec3ToGfVec3( pos );
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
