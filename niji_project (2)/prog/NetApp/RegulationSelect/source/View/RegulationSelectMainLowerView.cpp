// ============================================================================
/*
 * @file RegulationSelectMainLowerView.cpp
 * @brief レギュレーション選択の下画面を表示するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/regulation/RegulationSelect.h>
#include <niji_conv_header/app/regulation/RegulationSelect_anim_list.h>
#include <niji_conv_header/app/regulation/RegulationSelect_pane.h>
#include <niji_conv_header/message/msg_regulation.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)

static const u32 LIST_ITEM_PANE_MAX                   = 8;  //!< リスト項目ペイン数

static const gfl2::lyt::LytPaneIndex btns_tbl[LIST_ITEM_PANE_MAX] =
{
  PANENAME_REGULATION_SLC_LOW_000_PANE_BUTTONCHECK_00,
  PANENAME_REGULATION_SLC_LOW_000_PANE_BUTTONCHECK_01,
  PANENAME_REGULATION_SLC_LOW_000_PANE_BUTTONCHECK_02,
  PANENAME_REGULATION_SLC_LOW_000_PANE_BUTTONCHECK_03,
  PANENAME_REGULATION_SLC_LOW_000_PANE_BUTTONCHECK_04,
  PANENAME_REGULATION_SLC_LOW_000_PANE_BUTTONCHECK_05,
  PANENAME_REGULATION_SLC_LOW_000_PANE_BUTTONCHECK_06,
  PANENAME_REGULATION_SLC_LOW_000_PANE_BUTTONCHECK_07
};

static const u32 anm[LIST_ITEM_PANE_MAX][4] =
{
  {
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_00_TOUCH,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_00_RELEASE,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_00_CANSEL,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_00_TOUCH_RELEASE
  },
  {
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_01_TOUCH,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_01_RELEASE,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_01_CANSEL,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_01_TOUCH_RELEASE
  },
  {
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_02_TOUCH,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_02_RELEASE,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_02_CANSEL,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_02_TOUCH_RELEASE
  },
  {
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_03_TOUCH,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_03_RELEASE,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_03_CANSEL,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_03_TOUCH_RELEASE
  },
  {
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_04_TOUCH,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_04_RELEASE,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_04_CANSEL,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_04_TOUCH_RELEASE
  },
  {
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_05_TOUCH,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_05_RELEASE,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_05_CANSEL,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_05_TOUCH_RELEASE
  },
  {
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_06_TOUCH,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_06_RELEASE,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_06_CANSEL,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_06_TOUCH_RELEASE
  },
  {
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_07_TOUCH,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_07_RELEASE,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_07_CANSEL,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BUTTONCHECK_07_TOUCH_RELEASE
  }
};

RegulationSelectMainLowerView::RegulationSelectMainLowerView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pRegulationSelectWork,
    NetApp::RegulationSelect::REGULATIONSELECT_RESOURCE_ID_LAYOUT,
    LYTRES_REGULATIONSELECT_REGULATION_SLC_LOW_000_BFLYT,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pRegulationSelectWork->GetMessageUtility(),
    GARC_message_regulation_DAT ),
  m_pRegulationSelectWork( pRegulationSelectWork ),
  m_aPaneListCursorInfos(),
  m_eListType( LIST_TYPE_PRESET_DOWNLOADED ),
  m_pRegulationSelectMainLowerViewListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_pRegulationPaneListData( NULL ),
  m_MenuCursor( pRegulationSelectWork ),
  m_pRegulationList( NULL ),
  m_bPaneListInputEnable( false )
{
  gfl2::std::MemClear( m_aPaneListCursorInfos, sizeof( m_aPaneListCursorInfos ) );

  setupLayout( LAYOUT_WORK_ID_LOWER, LA_REGULATIONSELECT_REGULATION_SLC_LOW_000_PATERN_01 );
  setupButton();

  m_MenuCursor.Create( GetG2DUtil(), LAYOUT_RESOURCE_ID_REGULATIONSELECT, LAYOUT_WORK_ID_LOWER );

  createPaneList();

  SetInputListener( this );

  UIView::GetG2DUtil()->StartAnime( 0, LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BG_ANIM_BG_LOOP_00 );
}


RegulationSelectMainLowerView::~RegulationSelectMainLowerView()
{
  deletePaneList();

  m_MenuCursor.Destroy();
}


bool RegulationSelectMainLowerView::IsDrawing(void) const
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


void RegulationSelectMainLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_TOUCH,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_RELEASE,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_CANSEL,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_ACTIVE,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_REGULATIONSELECT );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_REGULATION_SLC_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
}


void RegulationSelectMainLowerView::createPaneList()
{
  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_REGULATIONSELECT );

  // -----------------------------------------------------------------------
  { // m_pPaneListData 初期化 ここから

    m_pPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];
    m_pRegulationPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) RegulationPaneData[ LIST_ITEM_PANE_MAX ];
   
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      gfl2::lyt::LytParts* pBTNS = pLower->GetPartsPane( btns_tbl[i] );
      m_pPaneListData[i].base       = pBTNS;
      m_pPaneListData[i].text       = pLower->GetTextBoxPane( pBTNS, PANENAME_NET_BTN_LOW_000_PANE_TEXTBOX_00, &resourceID );
      m_pPaneListData[i].bound      = pLower->GetBoundingPane( pBTNS, PANENAME_NET_BTN_LOW_000_PANE_BOUND_00, &resourceID );
      m_pPaneListData[i].cursor_pos = pLower->GetPane( pBTNS, PANENAME_NET_BTN_LOW_000_PANE_NULL_CUR, &resourceID );

      m_pPaneListData[i].holdAnime    = anm[i][0];
      m_pPaneListData[i].releaseAnime = anm[i][1];
      m_pPaneListData[i].cancelAnime  = anm[i][2];
      m_pPaneListData[i].selectAnime  = anm[i][3];

      m_pRegulationPaneListData[i].pTextBox00 = pLower->GetTextBoxPane( pBTNS, PANENAME_NET_BTN_LOW_000_PANE_TEXTBOX_00, &resourceID );
      m_pRegulationPaneListData[i].pTextBox01 = pLower->GetTextBoxPane( pBTNS, PANENAME_NET_BTN_LOW_000_PANE_TEXTBOX_01, &resourceID );
      m_pRegulationPaneListData[i].pCheckMark = pLower->GetPane( pBTNS, PANENAME_NET_BTN_LOW_000_PANE_MARK_CHECK, &resourceID );
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
      PANENAME_REGULATION_SLC_LOW_000_PANE_SCROLL,
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


void RegulationSelectMainLowerView::deletePaneList()
{
  {
    m_pPaneListView->RemoveFromSuperView();
    m_pPaneListView->DeleteScrollBar();
    m_pPaneListView->DeletePaneList();
    GFL_SAFE_DELETE( m_pPaneListView );
  }

  {
    GFL_SAFE_DELETE_ARRAY( m_pRegulationPaneListData );
    GFL_SAFE_DELETE_ARRAY( m_pPaneListData );
  }
}


void RegulationSelectMainLowerView::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll )
{
  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  pParam->valMax = ( m_pRegulationList ) ? m_pRegulationList->Size() : 1;					// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = 4;			// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 92.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                          //   この座標で指定した場所にあるペインが描画開始位置のペインになる
  pParam->defSX = 270.0f;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  pParam->defSY = 50.0f;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  pParam->writeOffsetY = -1;		// 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  pParam->listStartPane = pLower->GetPane( PANENAME_REGULATION_SLC_LOW_000_PANE_ITEM_CHANGE_OFF );   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  pParam->cbFunc = this;	// コールバック関数
  pParam->cbWork = NULL;			// コールバック関数に渡すワーク

  pParam->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  pParam->menuCursor = m_MenuCursor.GetAppToolMenuCursor();		// カーソル
}


void RegulationSelectMainLowerView::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
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

    f32 lx = 4.0f - pParam->defSX / 2.0f;
    f32 rx = lx + pParam->defSX;
    f32 uy = pParam->defPY + pos.y + pParam->defSY / 2.0f;
    f32 dy = uy - pParam->defSY * pos_max;
    pPaneList->MakeTouchArea( lx, rx, uy, dy );
  }

}


void RegulationSelectMainLowerView::setInputEnablePaneList( bool bInputEnable )
{
  m_bPaneListInputEnable = bInputEnable;

  GFL_PRINT( "RegulationSelectMainLowerView::setInputEnablePaneList\n" );
  if( m_pPaneListView )
  {
    GFL_PRINT( "m_pPaneListView\n" );
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      GFL_PRINT( "pPaneList\n" );
      GFL_PRINT( "bInputEnable = %d\n", bInputEnable );
      pPaneList->SetInputEnabled( bInputEnable );
    }
  }
}


void RegulationSelectMainLowerView::CheckBoxEnable()
{
  checkBoxVisible( true );
}


void RegulationSelectMainLowerView::CheckBoxDisable()
{
  checkBoxVisible( false );
}


void RegulationSelectMainLowerView::checkBoxVisible( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_REGULATIONSELECT );
  
  for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
  {
    gfl2::lyt::LytParts* pBTNS = pLower->GetPartsPane( btns_tbl[i] );
    gfl2::lyt::LytPane* pCheckBox = pLower->GetPane( pBTNS, PANENAME_NET_BTN_LOW_000_PANE_BG_CHECK, &resourceID );
    pCheckBox->SetVisible( bVisible );
  } 
}


void RegulationSelectMainLowerView::ResetPaneListCursorInfo( ListType eListType )
{
  m_aPaneListCursorInfos[ eListType ].pos     = 0;
  m_aPaneListCursorInfos[ eListType ].scroll  = 0.0f;
  m_aPaneListCursorInfos[ eListType ].listMax = 0;
}


void RegulationSelectMainLowerView::SetRegulationList( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, bool bCheckMarkEnable, ListType eListType )
{
  GFL_PRINT( "RegulationSelectMainLowerView::SetRegulationList \n" );

  m_pRegulationList = pRegulationList;
  m_eListType = eListType;

  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList = m_pRegulationSelectWork->GetDownloadedList();

  GFL_PRINT( "pDownloadedList->Size() = %d \n", pDownloadedList->Size() );

  for( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = pRegulationList->Begin(); it != pRegulationList->End(); ++it )
  {
    bool bIsDownloaded = ( bCheckMarkEnable ) ? isDownloaded( *it, pDownloadedList ) : false;
    (*it)->SetDownloaded( bIsDownloaded );
  }

  // 同じリストが設定される時、リストサイズに変動があったらカーソル位置をリセットする
  if( m_aPaneListCursorInfos[ m_eListType ].listMax > 0 )
  {
    if( m_aPaneListCursorInfos[ m_eListType ].listMax != m_pRegulationList->Size() )
    {
      m_aPaneListCursorInfos[ m_eListType ].pos = 0;
      m_aPaneListCursorInfos[ m_eListType ].scroll = 0.0f;
    }
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  setPaneListParam( &list_setup, m_aPaneListCursorInfos[ m_eListType ].pos, m_aPaneListCursorInfos[ m_eListType ].scroll );
  m_pPaneListView->ResetPaneList( &list_setup );
  setPaneListConfig( &list_setup );
  m_aPaneListCursorInfos[ m_eListType ].listMax = m_pRegulationList->Size();
}


bool RegulationSelectMainLowerView::isDownloaded( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList )
{
  GFL_PRINT( "RegulationSelectMainLowerView::isDownloaded \n" );
  GFL_PRINT( "pRegulationDrawInfo->GetRegulation().GetCrc() = %d \n", pRegulationDrawInfo->GetRegulation().GetCrc() );
  GFL_PRINT( "pRegulationDrawInfo->GetRegulation().GetSerializeDataSize() = %d \n", pRegulationDrawInfo->GetRegulation().GetSerializeDataSize() );

  for( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = pDownloadedList->Begin(); it != pDownloadedList->End(); ++it )
  {
    GFL_PRINT( "(*it)->GetRegulation().GetCrc() = %d \n", (*it)->GetRegulation().GetCrc() );
    GFL_PRINT( "(*it)->GetRegulation().GetSerializeDataSize() = %d \n", (*it)->GetRegulation().GetSerializeDataSize() );
    
    // @fix NMCat[4266]配信レギュレーションで大会IDが同じデータが、ローカル側で別なデータとして扱われてしまう　の修正
    // レギュレーションの一致チェックは、crcではなく、大会IDとバージョンで比較する。
    if( pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID) == (*it)->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID) &&
        pRegulationDrawInfo->GetRegulation().GetVersion() <= (*it)->GetRegulation().GetVersion() )
    {
      return true;
    }
  }
  return false;
}


NetAppLib::UI::RegulationDrawInfo* RegulationSelectMainLowerView::getRegulationDrawInfo( u32 index )
{
  if( m_pRegulationList && m_pRegulationList->Size() > 0 )
  {
    u32 count = 0;
    for( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = m_pRegulationList->Begin(); it != m_pRegulationList->End(); ++it )
    {
      if( index == count )
      {
        return *it;
      }
      ++count;
    }
  }
  return NULL;
}


//-----------------------------------------------------------------------------
// UIView
//-----------------------------------------------------------------------------
void RegulationSelectMainLowerView::OnAddChild()
{
  GFL_PRINT( "RegulationSelectMainLowerView::OnAddChild\n" );

  m_MenuCursor.SetVisible( false );

  setInputEnablePaneList( false );
}


void RegulationSelectMainLowerView::OnAddedToParent()
{
  m_MenuCursor.SetVisible( true );
}


void RegulationSelectMainLowerView::OnRemoveChild()
{
  GFL_PRINT( "RegulationSelectMainLowerView::OnRemoveChild\n" );

  m_MenuCursor.SetVisible( true );

  setInputEnablePaneList( true );
}


void RegulationSelectMainLowerView::OnRemovedFromParent()
{
  m_MenuCursor.SetVisible( false );
}


void RegulationSelectMainLowerView::Update( void )
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_MenuCursor.Update();
}


void RegulationSelectMainLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pRegulationSelectWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  m_MenuCursor.Draw( displayNo );
}


::app::ui::UIInputListener::ListenerResult RegulationSelectMainLowerView::OnLayoutPaneEvent( const u32 painId )
{
  GFL_PRINT( "RegulationSelectMainLowerView::OnLayoutPaneEvent\n" );

  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pRegulationSelectMainLowerViewListener )
      {
        GFL_PRINT( "m_pRegulationSelectMainLowerViewListener->TouchBackButton();\n" );

        m_pRegulationSelectMainLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult RegulationSelectMainLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  GFL_PRINT( "RegulationSelectMainLowerView::OnKeyAction\n" );

  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    GFL_PRINT( "m_button_manager->StartSelectedAct( BUTTON_ID_BACK_BUTTON );\n" );

    m_button_manager->StartSelectedAct( BUTTON_ID_BACK_BUTTON );
    result = DISABLE_ACCESS;
  }

  return result;
}



void RegulationSelectMainLowerView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  if( m_pRegulationSelectMainLowerViewListener )
  {
    m_pRegulationSelectMainLowerViewListener->SetSelectRegulationDrawInfo( getRegulationDrawInfo( pos ) );
  }
}


void RegulationSelectMainLowerView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void RegulationSelectMainLowerView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pRegulationSelectWork->GetMessageUtility();

  NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = getRegulationDrawInfo( pos );
  if( pRegulationDrawInfo )
  {
    // プリセットのレギュレーション
    if( pRegulationDrawInfo->IsPreset() )
    {
      switch( m_pRegulationSelectWork->GetBootMode() )
      {
        case NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P:
          pG2DUtil->SetTextBoxPaneString( m_pRegulationPaneListData[ pane_index ].pTextBox00, &pMessageUtility->GetString( GARC_message_regulation_DAT, pRegulationDrawInfo->GetBattleSystemMessageID() ) );
          pG2DUtil->SetTextBoxPaneString( m_pRegulationPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_regulation_DAT, pRegulationDrawInfo->GetBattleRuleMessageID() ) );
        break;

        default:
          pG2DUtil->SetTextBoxPaneString( m_pRegulationPaneListData[ pane_index ].pTextBox00, &pMessageUtility->GetString( GARC_message_regulation_DAT, pRegulationDrawInfo->GetBattleNameMessageID() ) );
          pG2DUtil->SetTextBoxPaneString( m_pRegulationPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_regulation_DAT, pRegulationDrawInfo->GetBattleRankMessageID() ) );
        break;
      }
    }
    // 配信レギュレーション
    else
    {
      gfl2::str::StrBuf* pTempStrBuf = pG2DUtil->GetTempStrBuf(0);
      pRegulationDrawInfo->GetRegulation().GetTextParam( Regulation::DELIVERY_NAME, pTempStrBuf );
    
      pG2DUtil->SetTextBoxPaneString( m_pRegulationPaneListData[ pane_index ].pTextBox00, &pMessageUtility->GetString( GARC_message_regulation_DAT, pRegulationDrawInfo->GetBattleSystemMessageID() ) );
      pG2DUtil->SetTextBoxPaneString( m_pRegulationPaneListData[ pane_index ].pTextBox01, pTempStrBuf );
    }

    m_pRegulationPaneListData[ pane_index ].pCheckMark->SetVisible( pRegulationDrawInfo->IsDownloaded() );
  }

}


void RegulationSelectMainLowerView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void RegulationSelectMainLowerView::PaneListItemSelectListener( u32 pos )
{
  GFL_PRINT( "RegulationSelectMainLowerView::PaneListItemSelectListener\n" );

  if( IsButtonAnimation() )
  {
    GFL_PRINT( "IsButtonAnimation() return \n" );

    return;
  }

  if( !m_bPaneListInputEnable )
  {
    GFL_PRINT( "!m_bPaneListInputEnable return \n" );

    return;
  }

  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    
    if( pPaneList )
    {
      pPaneList->GetCursorData( &m_aPaneListCursorInfos[ m_eListType ].pos, &m_aPaneListCursorInfos[ m_eListType ].scroll );
    }
  }

  if( m_pRegulationSelectMainLowerViewListener )
  {
    GFL_PRINT( "m_pRegulationSelectMainLowerViewListener->SelectRegulation \n" );

    m_pRegulationSelectMainLowerViewListener->SelectRegulation( pos );
    m_pRegulationSelectMainLowerViewListener->SetSelectRegulationDrawInfo( getRegulationDrawInfo( pos ) );
  }
}


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
