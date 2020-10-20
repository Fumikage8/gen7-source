//======================================================================
/**
 * @file    ResortLowerAdventureResultDraw.cpp
 * @date    2015/10/02 11:56:35
 * @author  fukushima_yuya
 * @brief   ポケリゾート：探検リゾート結果画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Resort/source/lower/ResortLowerAdventureResultDraw.h>
#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortConst.h>
#include <App/Resort/source/parts/ResortPartsInformationWindow.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/MyItemSave.h>
#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/resort/resort.h>
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/app/resort/resort_anim_list.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum {
    BTN_ID_CANCEL,
    BTN_ID_INFO_00,
    BTN_ID_INFO_01,
    BTN_ID_INFO_02,
    BTN_ID_INFO_03,
    BTN_ID_INFO_04,
    BTN_ID_INFO_05,
    BTN_ID_INFO_06,
    BTN_ID_NUM,
  };

  static const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_RESORT_RESORT_TANKEN_LOW_000__BACKBUTTON_TOUCH,
      LA_RESORT_RESORT_TANKEN_LOW_000__BACKBUTTON_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__BACKBUTTON_CANSEL,
      LA_RESORT_RESORT_TANKEN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__BACKBUTTON_ACTIVE,
      LA_RESORT_RESORT_TANKEN_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO_00, NULL, NULL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_00_TOUCH,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_00_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_00_CANSEL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_00_TOUCH_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_00_ACTIVE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_00_PASSIVE,
    },
    {
      BTN_ID_INFO_01, NULL, NULL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_01_TOUCH,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_01_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_01_CANSEL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_01_ACTIVE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_01_PASSIVE,
    },
    {
      BTN_ID_INFO_02, NULL, NULL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_02_TOUCH,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_02_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_02_CANSEL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_02_TOUCH_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_02_ACTIVE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_02_PASSIVE,
    },
    {
      BTN_ID_INFO_03, NULL, NULL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_03_TOUCH,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_03_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_03_CANSEL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_03_TOUCH_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_03_ACTIVE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_03_PASSIVE,
    },
    {
      BTN_ID_INFO_04, NULL, NULL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_04_TOUCH,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_04_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_04_CANSEL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_04_TOUCH_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_04_ACTIVE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_04_PASSIVE,
    },
    {
      BTN_ID_INFO_05, NULL, NULL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_05_TOUCH,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_05_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_05_CANSEL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_05_TOUCH_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_05_ACTIVE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_05_PASSIVE,
    },
    {
      BTN_ID_INFO_06, NULL, NULL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_06_TOUCH,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_06_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_06_CANSEL,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_06_TOUCH_RELEASE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_06_ACTIVE,
      LA_RESORT_RESORT_TANKEN_LOW_000__INFOBUTTON_06_PASSIVE,
    },
  };

  static const u32 c_ButtonBaseIndex[BTN_ID_NUM] = {
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BACKBUTTON,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_00,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_01,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_02,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_03,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_04,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_05,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_06,
  };
  static const u32 c_ButtonBoundIndex[BTN_ID_NUM] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
  };

  static const f32 PANELIST_OFFSET_Y = 1.0f;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
LowerAdventureResultDraw::LowerAdventureResultDraw( CreateParam& param )
  : LowerBaseDraw( param )
  , m_pInfoWindow( NULL )
{
#if PM_DEBUG
#if 0
#if RESORT_DEBUG_SAVE_DATA
  static const u32 c_ItemList[] = {
    ITEM_AKAIKAKERA, ITEM_AOIKAKERA, ITEM_KIIROIKAKERA, ITEM_MIDORINOKAKERA, ITEM_GENKINOKAKERA, ITEM_HOSINOKAKERA,
    ITEM_KAWARAZUNOISI, ITEM_KATAIISI, ITEM_KARUISI, ITEM_HONOONOISI, ITEM_KAMINARINOISI, ITEM_MIZUNOISI, ITEM_RIIHUNOISI, ITEM_HIKARINOISI, ITEM_YAMINOISI, ITEM_MEZAMEISI, ITEM_TAIYOUNOISI, ITEM_TUKINOISI, ITEM_MANMARUISI,
    ITEM_KINNOTAMA, ITEM_DEKAIKINNOTAMA, ITEM_KITYOUNAHONE, ITEM_SINZYU, ITEM_OOKINASINZYU, ITEM_ODANGOSINZYU, ITEM_SUISEINOKAKERA
  };

  for( u32 i=0; i<ITEM_RESULT_MAX; ++i )
  {
    u16 itemNo = ITEM_DUMMY_DATA;
    bool getItem = GFL_BOOL_CAST( GET_RAND(2) );

    if( getItem )
    {
      u32 rand = static_cast<u16>( GET_RAND( GFL_NELEMS( c_ItemList ) ) );
      itemNo = c_ItemList[rand];
    }

    m_pResortSave->SetAdventureItem( static_cast<int>(i), itemNo );
  }
#endif
#endif
#endif
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_TANKEN_LOW_000_BFLYT,
      LA_RESORT_RESORT_TANKEN_LOW_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pLytHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    param.msgSys,
    param.wordSet );

  this->SetVisible( false );
  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   クラスの初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetSystemHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // ボタンマネージャの生成
    CreateButtonManager( c_DefInfo, c_ButtonBaseIndex, c_ButtonBoundIndex, BTN_ID_NUM );

    // アイテムアイコン
    app::tool::ItemIconToolEx::ItemIconToolParam itemIconParam;
    {
      itemIconParam.pUIView = this;
      itemIconParam.pHeap   = m_pIconHeap;
      itemIconParam.iconMax = LIST_BTN_MAX;
      itemIconParam.sync    = true;
      itemIconParam.allocBuffer = true;
    }
    m_pDrawerBase->CreateItemIcon( itemIconParam );

    // ペインリストの初期化
    InitPaneList();

    // インフォメーションウィンドウの生成
    m_pInfoWindow = GFL_NEW(heap) ResortInfoWindow( m_pDrawerBase, m_pAppRenderingManager, m_pPaneList, param.msgItem );
    {
      GFL_ASSERT( m_pInfoWindow );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::InitializeText( void )
{
  gfl2::lyt::LytTextBox* textBox = m_pDrawerBase->GetTextBox( PANENAME_RESORT_TANKEN_LOW_000_PANE_HEADDER_MAIN );
  {
    this->SetTextboxPaneMessage( textBox, msg_prsys_up_03_05 );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LowerAdventureResultDraw::Terminate( void )
{
  if( m_pInfoWindow )
  {
    if( !m_pInfoWindow->DeleteSequence() ) return false;

    GFL_SAFE_DELETE( m_pInfoWindow );
  }

  GFL_SAFE_DELETE( m_pPaneList );

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::Update( void )
{
  app::ui::UIView::Update();

  if( m_pInfoWindow )
  {
    m_pInfoWindow->Update();
  }

  // リスト背景を更新
  UpdateListBG();

  // インフォボタンの表示を更新
  UpdateInfoBtn();  // @fix GFNMCat[3178]
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = UIView::GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }

  if( m_pInfoWindow )
  {
    m_pInfoWindow->Draw( no, gfl2::lyt::ORDER_NORMAL, 0 );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの初期化
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::InitPaneList( void )
{
  static const u32 c_ListNull[] = {
    PANENAME_RESORT_TANKEN_LOW_000_PANE_LIST_00,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_LIST_01,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_LIST_02,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_LIST_03,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_LIST_04,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_LIST_05,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_LIST_06,
  };

  static const u32 c_ListItemName[] = {
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNAME_00,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNAME_01,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNAME_02,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNAME_03,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNAME_04,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNAME_05,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNAME_06,
  };

  static const u32 c_ListBounding[] = {
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BOUND_00,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BOUND_01,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BOUND_02,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BOUND_03,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BOUND_04,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BOUND_05,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BOUND_06,
  };

  static const u32 c_ListItemIcon[] = {
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMICON_01,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMICON_02,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMICON_03,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMICON_04,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMICON_05,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMICON_06,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMICON_00,
  };

  static const u32 c_ListItemNumb[] = {
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNUMB_00,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNUMB_01,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNUMB_02,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNUMB_03,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNUMB_04,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNUMB_05,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_ITEMNUMB_06,
  };

  static const u32 c_ListBG[] = {
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BG_LIST_00,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BG_LIST_01,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BG_LIST_02,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BG_LIST_03,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BG_LIST_04,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BG_LIST_05,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_BG_LIST_06,
  };

  for( u32 i=0; i<LIST_BTN_MAX; ++i )
  {
    app::tool::PaneList::PANE_DATA* data = &m_paneData[i];
    gfl2::lyt::LytPane* pane = m_pDrawerBase->GetPane( c_ListNull[i] );

    data->base  = pane;
    data->text  = m_pDrawerBase->GetTextBox( c_ListItemName[i] );
    data->bound = m_pDrawerBase->GetBounding( c_ListBounding[i] );
    data->cursor_pos = NULL;
    
    data->holdAnime    = app::tool::ButtonManager::ANIMATION_NULL;
    data->releaseAnime = app::tool::ButtonManager::ANIMATION_NULL;
    data->cancelAnime  = app::tool::ButtonManager::ANIMATION_NULL;
    data->selectAnime  = app::tool::ButtonManager::ANIMATION_NULL;

    m_pItemIcon[i]  = m_pDrawerBase->GetPicture( c_ListItemIcon[i] );
    m_pItemCount[i] = m_pDrawerBase->GetTextBox( c_ListItemNumb[i] );
    m_pItemBG[i]    = m_pDrawerBase->GetPane( c_ListBG[i] );
  }

  m_pPaneList = GFL_NEW(m_pLytHeap->GetSystemHeap()) ResortPaneList( m_pDrawerBase, this, this );
  GFL_ASSERT( m_pPaneList );
}

//------------------------------------------------------------------------------
/**
 * @brief    ペインリストの生成
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::CreatePaneList( void )
{
  ResortPaneList::SETUP_PARAM param;
  {
    param.listMax       = m_ItemMax;
    param.paneMax       = LIST_BTN_MAX;
    param.viewCount     = 5;
    param.listStartPane = PANENAME_RESORT_TANKEN_LOW_000_PANE_LIST_ALL;
    param.defPY         = 74.0f + PANELIST_OFFSET_Y;
    param.paneWidth     = 194.0f;
    param.paneHeight    = 36.0f;
    param.scrollPane    = PANENAME_RESORT_TANKEN_LOW_000_PANE_SCROLL_00;
    param.moveSE        = SEQ_SE_SELECT1;
  }

  // ペインリストの生成
  if( m_pPaneList )
  {
    m_pPaneList->Create( param, m_paneData );
    // 入力を有効に
    m_pPaneList->SetInputEnable( true );  // @fix GFNMCat[2884]

    // @fix GFNMCat[5519]
    {
      u32 listMax = param.viewCount;
      if( param.listMax < listMax )
      {
        listMax = param.listMax;
      }

      f32 lx = -110.0f;
      f32 rx = 80.0f;
      f32 uy = param.defPY + (param.paneHeight / 2);
      f32 dy = param.defPY - (param.paneHeight * listMax);
      m_pPaneList->GetPaneListView()->GetPaneList()->MakeTouchArea( lx, rx, uy, dy );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief    ペインリストの更新
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::UpdatePaneList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  print::WordSet* wordSet = m_pDrawerBase->GetWordSet();

  u16 itemNo = m_ItemNo[pos];
  {
    m_InfoItemNo[pane_index] = itemNo;
  }

  // アイテム名
  {
    gfl2::lyt::LytTextBox* textBox = pane[pane_index].text;

    wordSet->RegisterItemName( 0, itemNo, 1 );
    this->SetTextboxPaneMessage( textBox, msg_prsys_common_05 );
  }

  // アイテムアイコン
  {
    app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();
    itemIcon->SetItemIcon( pane_index, itemNo, m_pItemIcon[pane_index] );
  }

  // アイテム数
  {
    u32 num = m_ItemNum[pos];

    wordSet->RegisterNumber( 0, num, 3, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT );
    this->SetTextboxPaneMessage( m_pItemCount[pane_index], msg_prsys_common_04 );
  }

  // インフォボタン
  {
    app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
    {
      btnMan->SetButtonInputEnable( (BTN_ID_INFO_00 + pane_index), true );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リスト背景の更新
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::UpdateListBG( void )
{
  for( u32 i=0; i<LIST_BTN_MAX; ++i )
  {
    gfl2::lyt::LytPane* pane = m_paneData[i].base;
    
    if( pane )
    {
      gfl2::lyt::LytPane* bgPane = m_pItemBG[i];
      gfl2::math::VEC3 pos = pane->GetTranslate();
      
      if( bgPane )
      {
        pos.y += PANELIST_OFFSET_Y;

        m_pItemBG[i]->SetTranslate( pos );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   インフォメーションボタンの有効設定更新
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::UpdateInfoBtn( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  for( u32 i=0; i<LIST_BTN_MAX; ++i )
  {
    // リストボタンの表示フラグを取得
    bool visible = m_paneData[i].base->IsVisible();
    {
      u32 btnID = BTN_ID_INFO_00 + i;

      if( !visible )
      {
        m_InfoItemNo[i] = ITEM_DUMMY_DATA;

        // ボタンが有効な場合は無効にする
        if( btnMan->IsButtonInputEnable( btnID ) )
        {
          btnMan->SetButtonInputEnable( btnID, false );
        }
      }
      else {
        // ボタンが無効な場合は有効にする
        if( !btnMan->IsButtonInputEnable( btnID ) )
        {
          btnMan->SetButtonInputEnable( btnID, true );
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   取得したアイテムの初期化
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::InitResultItem( void )
{
  for( u32 i=0; i<ITEM_RESULT_MAX; ++i )
  {
    m_ItemNo[i]  = ITEM_DUMMY_DATA;
    m_ItemNum[i] = 0;
  }

  u32 addIndex = 0;

  for( u32 i=0; i<ITEM_RESULT_MAX; ++i )
  {
    u16 itemNo = m_pResortSave->GetAdventureItem( i );

    if( itemNo == ITEM_DUMMY_DATA ) continue;

    u32  entryIdx = 0;
    bool entry = false;

    for( u32 j=0; j<ITEM_RESULT_MAX; ++j )
    {
      // 追加済みかどうか
      if( m_ItemNo[j] == itemNo )
      {
        entryIdx = j;
        entry = true;
        break;
      }
    }

    if( entry )
    {
      m_ItemNum[entryIdx]++;

      // momiji対応：入手アイテムが欠片系の場合は入手数を２倍に
      if( IsItemKindKakera( itemNo ) )
      {
        m_ItemNum[entryIdx]++;
      }
    }
    else {
      m_ItemNo[addIndex] = itemNo;
      m_ItemNum[addIndex]++;

      // momiji対応：入手アイテムが欠片系の場合は入手数を２倍に
      if( IsItemKindKakera( itemNo ) )
      {
        m_ItemNum[addIndex]++;
      }

      addIndex++;
    }
  }

  m_ItemMax = addIndex;

  for( u32 i=0; i<ITEM_RESULT_MAX; ++i )
  {
    FUKUSHIMA_PRINT( "ItemNo[%d] : [%d]\n", m_ItemNo[i], m_ItemNum[i] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   取得したアイテムをセーブデータに反映
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::SetResultItemData( void )
{
  for( u32 i=0; i<m_ItemMax; ++i )
  {
    u16 itemNo  = m_ItemNo[i];
    u32 itemNum = m_ItemNum[i];

    if( m_pItemSave->AddCheck( itemNo, itemNum ) )
    {
      m_pItemSave->Add( itemNo, itemNum );
    }
  }

  for( u32 i=0; i<ITEM_RESULT_MAX; ++i )
  {
    m_pResortSave->SetAdventureItem( static_cast<int>(i), ITEM_DUMMY_DATA );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   取得したアイテムが欠片かどうか
 */
//------------------------------------------------------------------------------
bool LowerAdventureResultDraw::IsItemKindKakera( u16 itemNo )
{
  const u16 c_ItemKakeraList[] = {
    ITEM_AKAIKAKERA,
    ITEM_AOIKAKERA,
    ITEM_KIIROIKAKERA,
    ITEM_MIDORINOKAKERA,
    ITEM_GENKINOKAKERA,
    ITEM_HOSINOKAKERA,
    ITEM_SUISEINOKAKERA,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_ItemKakeraList ); ++i )
  {
    if( c_ItemKakeraList[i] == itemNo )
    {
      return true;
    }
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief   インフォメーションウィンドウの表示
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::OpenInformationWindow( u32 pos )
{
  static const u32 c_InfoBtnIndex[] = {
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_00,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_01,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_02,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_03,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_04,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_05,
    PANENAME_RESORT_TANKEN_LOW_000_PANE_INFOBUTTON_06,
  };

  GFL_ASSERT( pos < LIST_BTN_MAX );

  if( m_pInfoWindow )
  {
    u16 itemNo = m_InfoItemNo[pos];

    // ボタンの座標位置
    gfl2::math::Vector3 btnPos( 0, 0, 0 );
    {
      gfl2::lyt::LytPane* parentPane = m_paneData[pos].base;
      {
        gfl2::math::VEC3 basePos = parentPane->GetTranslate();
        gfl2::math::VEC3 childPos(0, 0, 0);

        gfl2::lyt::LytPane* childPane = m_pDrawerBase->GetPane( c_InfoBtnIndex[pos] );
        {
          childPos = childPane->GetTranslate();
          btnPos = gfl2::math::ConvertNwVec3ToGfVec3( basePos + childPos );
        }
      }
    }

    m_pInfoWindow->Open( itemNo, btnPos );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::Open( void )
{
  // インフォボタンの有効設定
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    for( u32 i=BTN_ID_INFO_00; i<=BTN_ID_INFO_06; ++i )
    {
      btnMan->SetButtonInputEnable( i, false );
    }
  }

  // アイテムの初期化
  InitResultItem();

  // ペインリストの生成
  CreatePaneList();

  // リザルトの初期化
  this->InitResult();

  this->SetVisible( true );
  this->SetInputEnabled( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::Close( void )
{
  if( m_pPaneList )
  {
    m_pPaneList->Delete();
  }

  this->SetInputEnabled( false );
  this->SetVisible( false );
}





//==============================================================================
/**
 *      UIInputListener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 *
 * @param   paneID
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerAdventureResultDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  switch( paneID )
  {
  case BTN_ID_CANCEL:
    {
      // セーブデータへの反映
      SetResultItemData();
      // レコード：回数をカウント
      Savedata::IncRecord( Savedata::Record::RECID_RESORT_ITEM );

      m_Result = RESULT_CANCEL;
    } break;

  case BTN_ID_INFO_00:
  case BTN_ID_INFO_01:
  case BTN_ID_INFO_02:
  case BTN_ID_INFO_03:
  case BTN_ID_INFO_04:
  case BTN_ID_INFO_05:
  case BTN_ID_INFO_06:
    {
      u32 pos = paneID - BTN_ID_INFO_00;
      OpenInformationWindow( pos );
    } break;
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペイン選択アニメ開始通知用リスナー
 *
 * @param   paneID
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::OnLayoutPaneStartingAnimeEvent( const u32 paneID )
{
  switch( paneID )
  {
  case BTN_ID_CANCEL:   // @fix GFNMCat[2884]
  case BTN_ID_INFO_00:  // @fix GFNMCat[3186]
  case BTN_ID_INFO_01:  // @fix GFNMCat[3186]
  case BTN_ID_INFO_02:  // @fix GFNMCat[3186]
  case BTN_ID_INFO_03:  // @fix GFNMCat[3186]
  case BTN_ID_INFO_04:  // @fix GFNMCat[3186]
  case BTN_ID_INFO_05:  // @fix GFNMCat[3186]
  case BTN_ID_INFO_06:  // @fix GFNMCat[3186]
    {
      if( m_pPaneList )
      {
        // ボタンが選択されたらペインリストの入力を無効
        m_pPaneList->SetInputEnable( false );
      }
    } break;
  }
}






//==============================================================================
/**
 *      PaneListView::Listener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   リストの項目を描画するコールバック関数
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  UpdatePaneList( pane, pane_index, pos );
}






//==============================================================================
/**
 *      ButtonManager
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンの個別セットアップ
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerAdventureResultDraw::SetupButton( app::tool::ButtonManager* btnMan )
{
  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    if( i == BTN_ID_CANCEL )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
    else {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)
