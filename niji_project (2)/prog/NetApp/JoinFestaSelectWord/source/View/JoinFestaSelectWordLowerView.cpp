// ============================================================================
/*
 * @file JoinFestaSelectWordLowerView.cpp
 * @brief 挨拶選択アプリの下画面を表示するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordLowerView.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word.h>
#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word_anim_list.h>
#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word_pane.h>
#include <niji_conv_header/message/msg_setphrase.h>
#include <niji_conv_header/message/msg_jf_phrase.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)


static const gfl2::lyt::LytPaneIndex btns_tbl[JoinFestaSelectWordLowerView::LIST_ITEM_PANE_MAX] =
{
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_05,
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_06,
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_07,
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_08,
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_09,
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_10,
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_11,
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_12,
  PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_13,
};

static const u32 anm[JoinFestaSelectWordLowerView::LIST_ITEM_PANE_MAX][4] =
{
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_05_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_05_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_05_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_05_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_06_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_06_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_06_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_06_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_07_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_07_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_07_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_07_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_08_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_08_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_08_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_08_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_09_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_09_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_09_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_09_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_10_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_10_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_10_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_10_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_11_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_11_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_11_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_11_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_12_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_12_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_12_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_12_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_13_TOUCH,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_13_RELEASE,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_13_CANSEL,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_13_TOUCH_RELEASE,
  },
};


JoinFestaSelectWordLowerView::JoinFestaSelectWordLowerView( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaSelectWordWork,
    NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000_BFLYT,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaSelectWordWork->GetMessageUtility(),
    GARC_message_setphrase_DAT ),
  m_pJoinFestaSelectWordWork( pJoinFestaSelectWordWork ),
  m_aPaneListCursorInfos(),
  m_eListType( LIST_TYPE_CATEGORY ),
  m_pJoinFestaSelectWordLowerViewListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_MenuCursor( pJoinFestaSelectWordWork ),
  m_aMessageTable(),
  m_MessageTableSize( 0 ),
  m_bPaneListInputEnable( false )
{
  gfl2::std::MemClear( m_aPaneListCursorInfos, sizeof( m_aPaneListCursorInfos ) );

  m_MenuCursor.Create( GetG2DUtil(), LAYOUT_RESOURCE_ID_JOINFESTASELECTWORD, LAYOUT_WORK_ID_LOWER, JOINFESTASELECTWORD_RESOURCE_ID_MENU_CURSOR );

  setupButton();

  createPaneList();

  SetInputListener( this );

  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSW_MIN_LOW_000_PANE_INFOBUTTON );
    pButton->SetVisible( true );
  }
}


JoinFestaSelectWordLowerView::~JoinFestaSelectWordLowerView()
{
  deletePaneList();

  m_MenuCursor.Destroy();
}


void JoinFestaSelectWordLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_00_TOUCH,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_00_RELEASE,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_00_CANSEL,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_00_TOUCH_RELEASE,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_00_ACTIVE,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__PARTS_00_PASSIVE,
    },
    {
      BUTTON_ID_INFO_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__INFOBUTTON_TOUCH,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__INFOBUTTON_RELEASE,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__INFOBUTTON_CANSEL,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__INFOBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__INFOBUTTON_ACTIVE,
      LA_JOIN_FESTA_SELECT_WORD_JFSW_MIN_LOW_000__INFOBUTTON_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASELECTWORD );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_JFSW_MIN_LOW_000_PANE_PARTS_00 );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pInfoButton = pLower->GetPartsPane( PANENAME_JFSW_MIN_LOW_000_PANE_INFOBUTTON );
    buttonInfoTable[ BUTTON_ID_INFO_BUTTON ].picture_pane = pInfoButton;
    buttonInfoTable[ BUTTON_ID_INFO_BUTTON ].bound_pane = pLower->GetBoundingPane( pInfoButton, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pJoinFestaSelectWordWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_INFO_BUTTON, SEQ_SE_DECIDE1 );
}


void JoinFestaSelectWordLowerView::createPaneList()
{
  app::util::Heap* pAppHeap = m_pJoinFestaSelectWordWork->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASELECTWORD );

  // -----------------------------------------------------------------------
  { // m_pPaneListData 初期化 ここから

    m_pPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];
   
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      gfl2::lyt::LytParts* pBTNS = pLower->GetPartsPane( btns_tbl[i] );
      m_pPaneListData[i].base       = pBTNS;
      m_pPaneListData[i].text       = pLower->GetTextBoxPane( pBTNS, PANENAME_FESTA_BTN_LOW_004_PANE_TEXTBOX_00, &resourceID );
      m_pPaneListData[i].bound      = pLower->GetBoundingPane( pBTNS, PANENAME_FESTA_BTN_LOW_004_PANE_BOUND_00, &resourceID );
      m_pPaneListData[i].cursor_pos = pLower->GetPane( pBTNS, PANENAME_FESTA_BTN_LOW_004_PANE_NULL_CUR, &resourceID );

      m_pPaneListData[i].holdAnime    = anm[i][0];
      m_pPaneListData[i].releaseAnime = anm[i][1];
      m_pPaneListData[i].cancelAnime  = anm[i][2];
      m_pPaneListData[i].selectAnime  = anm[i][3];
    }
  } // m_pPaneListData 初期化 ここまで
  // -----------------------------------------------------------------------

  // -----------------------------------------------------------------------
  { // m_pPaneListView 初期化 ここから

    app::tool::PaneList::SETUP_PARAM list_setup;
    setPaneListParam( &list_setup, 0, 0.0f );

    app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
    {
      pAppHeap->GetDeviceHeap(),
      app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
      list_setup.valMax-list_setup.listPosMax,

      pLower,
      resourceID,
      PANENAME_JFSW_MIN_LOW_000_PANE_SCROLL_BAR,
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


void JoinFestaSelectWordLowerView::deletePaneList()
{
  {
    m_pPaneListView->RemoveFromSuperView();
    m_pPaneListView->DeleteScrollBar();
    m_pPaneListView->DeletePaneList();
    GFL_SAFE_DELETE( m_pPaneListView );
  }

  {
    GFL_SAFE_DELETE_ARRAY( m_pPaneListData );
  }
}


void JoinFestaSelectWordLowerView::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll )
{
  app::util::Heap* pAppHeap = m_pJoinFestaSelectWordWork->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  pParam->valMax = m_MessageTableSize;					// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = 7;			// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 103.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                          //   この座標で指定した場所にあるペインが描画開始位置のペインになる
  pParam->defSX = 306.0f;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  pParam->defSY = 28.0f;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  pParam->writeOffsetY = -1;		// 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  pParam->listStartPane = NULL;   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  pParam->cbFunc = this;	// コールバック関数
  pParam->cbWork = NULL;			// コールバック関数に渡すワーク

  pParam->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  pParam->menuCursor = m_MenuCursor.GetAppToolMenuCursor();		// カーソル
}


void JoinFestaSelectWordLowerView::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
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


void JoinFestaSelectWordLowerView::setInputEnablePaneList( bool bInputEnable )
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


void JoinFestaSelectWordLowerView::backupPaneListCursorInfo()
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    
    if( pPaneList )
    {
      pPaneList->GetCursorData( &m_aPaneListCursorInfos[ m_eListType ].pos, &m_aPaneListCursorInfos[ m_eListType ].scroll );
    }
  }
}


void JoinFestaSelectWordLowerView::SetCategoryTable( NetAppLib::JoinFesta::JoinFestaSelectWordTable::Message* pMessageTable, u32 messageTableSize )
{
  m_eListType = LIST_TYPE_CATEGORY;
  setList( pMessageTable, messageTableSize, SEQ_SE_DECIDE1 );
}


void JoinFestaSelectWordLowerView::SetMessageTable( NetAppLib::JoinFesta::JoinFestaSelectWordTable::Message* pMessageTable, u32 messageTableSize )
{
  m_eListType = LIST_TYPE_MESSAGE;
  setList( pMessageTable, messageTableSize, SEQ_SE_COMPLETE1 );
}


void JoinFestaSelectWordLowerView::setList( NetAppLib::JoinFesta::JoinFestaSelectWordTable::Message* pMessageTable, u32 messageTableSize, u32 se )
{
  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  u32 size = 0;

  const u32 FEMALE_MESSAGE_OFFSET = msg_jf_phrase_max / 2;

  for( u32 i = 0; i < messageTableSize; ++i )
  {
    if( pMessageTable[i].messageID != NetAppLib::JoinFesta::JoinFestaSelectWordTable::INVALID_MESSAGE_ID )
    {
      u32 messageID = static_cast<u32>( pMessageTable[i].messageID );

      if( pJoinFestaDataSave->IsUseSelectWord( messageID ) )
      {
        if( m_eListType == LIST_TYPE_MESSAGE )
        {
          if( pMyStatus->GetSex() == PM_FEMALE )
          {
            messageID += FEMALE_MESSAGE_OFFSET;
          }
        }

        m_aMessageTable[size] = messageID;

        ++size;
      }
    }
  }

  m_MessageTableSize = size;

  // 同じリストが設定される時、リストサイズに変動があったらカーソル位置をリセットする
  if( m_aPaneListCursorInfos[ m_eListType ].listMax > 0 )
  {
    if( m_aPaneListCursorInfos[ m_eListType ].listMax != m_MessageTableSize )
    {
      m_aPaneListCursorInfos[ m_eListType ].pos = 0;
      m_aPaneListCursorInfos[ m_eListType ].scroll = 0.0f;
    }
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  setPaneListParam( &list_setup, m_aPaneListCursorInfos[ m_eListType ].pos, m_aPaneListCursorInfos[ m_eListType ].scroll );
  m_pPaneListView->ResetPaneList( &list_setup );
  setPaneListConfig( &list_setup );
  m_aPaneListCursorInfos[ m_eListType ].listMax = m_MessageTableSize;

  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  if( pPaneList )
  {
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      pPaneList->SetButtonSEPaneIndex( i, se );
    }
  }

}


void JoinFestaSelectWordLowerView::Update()
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_MenuCursor.Update();
}


void JoinFestaSelectWordLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaSelectWordWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  m_MenuCursor.Draw( displayNo );
}


bool JoinFestaSelectWordLowerView::IsDrawing(void) const
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


void JoinFestaSelectWordLowerView::OnAddChild( void )
{
  setInputEnablePaneList( false );
}


void JoinFestaSelectWordLowerView::OnRemoveChild( void )
{
  setInputEnablePaneList( true );
}


::app::ui::UIInputListener::ListenerResult JoinFestaSelectWordLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaSelectWordLowerViewListener )
      {
        m_pJoinFestaSelectWordLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_INFO_BUTTON:
    {
      if( m_pJoinFestaSelectWordLowerViewListener )
      {
        m_pJoinFestaSelectWordLowerViewListener->TouchInfoButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaSelectWordLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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


void JoinFestaSelectWordLowerView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
}


void JoinFestaSelectWordLowerView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void JoinFestaSelectWordLowerView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  if( pos < m_MessageTableSize )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
      
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaSelectWordWork->GetMessageUtility();

    gfl2::fs::ArcFile::ARCDATID arcDataID = ( m_eListType == LIST_TYPE_CATEGORY ) ? GARC_message_setphrase_DAT : GARC_message_jf_phrase_DAT;

    pG2DUtil->SetTextBoxPaneString( m_pPaneListData[ pane_index ].text, &pMessageUtility->GetString( arcDataID, m_aMessageTable[ pos ] ) );
  }
}


void JoinFestaSelectWordLowerView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void JoinFestaSelectWordLowerView::PaneListItemSelectListener( u32 pos )
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

  if( pos < m_MessageTableSize )
  {
    if( m_pJoinFestaSelectWordLowerViewListener )
    {
      m_pJoinFestaSelectWordLowerViewListener->SelectMessage( pos, m_aMessageTable[ pos ] );
    }
  }
}


// @fix GFMMCat[27]: インフォボタンの座標取得
void JoinFestaSelectWordLowerView::GetInfoButtonPos( gfl2::math::Vector3 * vec )
{
  gfl2::math::VEC3 pos = GetButtonManager()->GetButtonPos( BUTTON_ID_INFO_BUTTON );
  *vec = gfl2::math::ConvertNwVec3ToGfVec3( pos );
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
