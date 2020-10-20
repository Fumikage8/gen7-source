//======================================================================
/**
 * @file    ResortLowerFarmListDraw.h
 * @date    2015/10/01 20:16:51
 * @author  fukushima_yuya
 * @brief   ポケリゾート：植えるきのみのリスト画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerFarmListDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortConst.h>
#include <App/Resort/source/parts/ResortPartsInformationWindow.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <sound/include/Sound.h>
#include <Savedata/include/ResortSave.h>
#include <Savedata/include/MyItemSave.h>

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
    BTN_ID_DECIDE,
    BTN_ID_INFO_00,
    BTN_ID_INFO_01,
    BTN_ID_INFO_02,
    BTN_ID_INFO_03,
    BTN_ID_INFO_04,
    BTN_ID_INFO_05,
    BTN_ID_INFO_06,
    BTN_ID_REMOVE_00,
    BTN_ID_REMOVE_01,
    BTN_ID_REMOVE_02,
    BTN_ID_REMOVE_03,
    BTN_ID_REMOVE_04,
    BTN_ID_REMOVE_05,
    BTN_ID_NUM,
  };

  enum {
    BTN_ANIM_TOUCH,
    BTN_ANIM_RELEASE,
    BTN_ANIM_CANCEL,
    BTN_ANIM_PASSIVE,
    BTN_ANIM_ACTIVE,
    BTN_ANIM_TOUCH_RELEASE,
    BTN_ANUM_NUM,
  };

  static const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__BACKBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__BACKBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__BACKBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__BACKBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_DECIDE, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__SELECTBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__SELECTBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__SELECTBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__SELECTBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__SELECTBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__SELECTBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO_00, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_00__INFOBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_00__INFOBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_00__INFOBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_00__INFOBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_00__INFOBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_00__INFOBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO_01, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_01__INFOBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_01__INFOBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_01__INFOBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_01__INFOBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_01__INFOBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_01__INFOBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO_02, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_02__INFOBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_02__INFOBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_02__INFOBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_02__INFOBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_02__INFOBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_02__INFOBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO_03, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_03__INFOBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_03__INFOBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_03__INFOBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_03__INFOBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_03__INFOBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_03__INFOBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO_04, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_04__INFOBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_04__INFOBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_04__INFOBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_04__INFOBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_04__INFOBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_04__INFOBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO_05, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_05__INFOBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_05__INFOBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_05__INFOBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_05__INFOBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_05__INFOBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_05__INFOBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO_06, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_06__INFOBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_06__INFOBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_06__INFOBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_06__INFOBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_06__INFOBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_06__INFOBUTTON_PASSIVE,
    },
    {
      BTN_ID_REMOVE_00, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_00_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_00_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_00_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_00_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_00_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_00_PASSIVE,
    },
    {
      BTN_ID_REMOVE_01, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_01_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_01_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_01_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_01_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_01_PASSIVE,
    },
    {
      BTN_ID_REMOVE_02, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_02_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_02_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_02_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_02_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_02_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_02_PASSIVE,
    },
    {
      BTN_ID_REMOVE_03, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_03_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_03_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_03_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_03_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_03_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_03_PASSIVE,
    },
    {
      BTN_ID_REMOVE_04, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_04_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_04_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_04_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_04_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_04_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_04_PASSIVE,
    },
    {
      BTN_ID_REMOVE_05, NULL, NULL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_05_TOUCH,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_05_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_05_CANSEL,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_05_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_05_ACTIVE,
      LA_RESORT_RESORT_KINOMILIST_LOW_000__OFFBUTTON_05_PASSIVE,
    },
  };
  
  static const u32 c_ButtonBaseIndex[BTN_ID_NUM] = {
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_BACKBUTTON,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_SELECTBUTTON,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_00,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_01,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_02,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_03,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_04,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_05,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_06,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_00,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_01,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_02,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_03,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_04,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_05,
  };
  
  static const u32 c_ButtonPartsIndex[BTN_ID_NUM] = {
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
    PANENAME_RESORT_BTN_LOW_001_PANE_INFOBUTTON,
    PANENAME_RESORT_BTN_LOW_001_PANE_INFOBUTTON,
    PANENAME_RESORT_BTN_LOW_001_PANE_INFOBUTTON,
    PANENAME_RESORT_BTN_LOW_001_PANE_INFOBUTTON,
    PANENAME_RESORT_BTN_LOW_001_PANE_INFOBUTTON,
    PANENAME_RESORT_BTN_LOW_001_PANE_INFOBUTTON,
    PANENAME_RESORT_BTN_LOW_001_PANE_INFOBUTTON,
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
  };
  
  static const u32 c_ButtonBoundIndex[BTN_ID_NUM] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_021_PANE_BOUND_00,
    PANENAME_RESORT_BTN_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_BTN_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_BTN_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_BTN_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_BTN_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_BTN_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_BTN_LOW_001_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
  };

  #include "niji_conv_header/poke_lib/item/itemtype.cdat"   // 各分類テーブル
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
LowerFarmListDraw::LowerFarmListDraw( CreateParam& param )
  : LowerBaseDraw( param )
  , m_pInfoWindow(NULL)
  , m_FieldIndex(0)
  , m_ItemMax(0)
  , m_DiaryFlag(false)
  , m_pHeaderText(NULL)
  , m_pPaneList(NULL)
  , m_pAlertText(NULL)
  , m_FieldAddCount(0)
{
  for( u32 i=0; i<LIST_BTN_MAX; ++i )
  {
    m_pItemIcon[i]  = NULL;
    m_pItemCount[i] = NULL;
    m_ItemNo[i]     = ITEM_DUMMY_DATA;
  }
  for( u32 i=0; i<FIELD_PUT_MAX; ++i )
  {
    m_FieldItemNo[i]     = ITEM_DUMMY_DATA;
    m_FieldItemEnable[i] = true;
    m_pFieldItemIcon[i]  = NULL;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_KINOMILIST_LOW_000_BFLYT,
      LA_RESORT_RESORT_KINOMILIST_LOW_000___NUM,
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
void LowerFarmListDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetSystemHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // ボタンマネージャの生成
    CreateButtonManager( c_DefInfo, c_ButtonBaseIndex, c_ButtonPartsIndex, c_ButtonBoundIndex, BTN_ID_NUM );

    // アイテムアイコン
    CreateItemIcon();

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
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::InitializePane( void )
{
  // テキストボックスのセット
  m_pHeaderText = m_pDrawerBase->GetTextBox( PANENAME_RESORT_KINOMILIST_LOW_000_PANE_HEADDER_MAIN );

  // 
  static const u32 c_FielRemoveBtnID[] = {
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_00,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_01,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_02,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_03,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_04,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_OFFBUTTON_05,
  };

  for( u32 i=0; i<FIELD_PUT_MAX; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( c_FielRemoveBtnID[i] );
    m_pRemoveMarkPane[i] = m_pDrawerBase->GetPane( PANENAME_RESORT_BTN_LOW_002_PANE_VISIBLE, parts );
    m_pFieldItemIcon[i]  = m_pDrawerBase->GetPicture( PANENAME_RESORT_BTN_LOW_002_PANE_ITEMICON_00, parts );

    m_pDrawerBase->SetVisiblePane( m_pFieldItemIcon[i], false );
  }

  // アラートメッセージ
  m_pAlertText = m_pDrawerBase->GetTextBox( PANENAME_RESORT_KINOMILIST_LOW_000_PANE_TEXT_ERROR );

  // はたけの更新
  UpdateField();
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::InitializeText( void )
{
  gfl2::lyt::LytParts* btnParts =
    m_pDrawerBase->GetParts( PANENAME_RESORT_KINOMILIST_LOW_000_PANE_SELECTBUTTON );
  {
    gfl2::lyt::LytTextBox* btnText =
      m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_021_PANE_TEXTBOX_00, btnParts );
    {
      this->SetTextboxPaneMessage( btnText, msg_prsys_button_01 );
    }
  }
  
  this->SetTextboxPaneMessage( m_pAlertText, msg_prsys_low_02_10 );
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LowerFarmListDraw::Terminate( void )
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
void LowerFarmListDraw::Update( void )
{
  app::ui::UIView::Update();

  if( m_pInfoWindow )
  {
    m_pInfoWindow->Update();
  }

  // インフォボタンの表示を更新
  UpdateInfoBtn();  // @fix GFNMCat[3178]
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::Draw( gfl2::gfx::CtrDisplayNo no )
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
 * @brief   開く
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::Open( void )
{
  // きのみポケット内のアイテム数を取得
  m_ItemMax = m_pItemSave->GetItemCount( ITEM_POKET_KINOMI, false );

  // ペインリストの生成
  CreatePaneList();

  // メッセージの設定
  static const u32 c_HeaderMsg[] = {
    msg_prsys_low_02_06,
    msg_prsys_low_02_07,
    msg_prsys_low_02_08,
  };
  this->SetTextboxPaneMessage( m_pHeaderText, c_HeaderMsg[m_FieldIndex] );

  m_FieldAddCount = 0;

  for( u32 i=0; i<FIELD_PUT_MAX; ++i )
  {
    u32 index = m_FieldIndex * FIELD_PUT_MAX + i;

    u16 itemNo = m_pResortSave->GetFarmItemKind( index );

    m_FieldItemNo[i] = itemNo;  //!< ItemNoを格納
    m_FieldItemEnable[i] = (itemNo == ITEM_DUMMY_DATA);

    FUKUSHIMA_PRINT( "%d : ItemNo[%d], Enable[%d]\n", i, itemNo, m_FieldItemEnable[i] );
  }

  // アラートメッセージの表示切替
  SetAlertMessage();

  // 除外ボタンの有効設定更新
  UpdateRemoveBtn();

  // はたけの状態を更新
  UpdateField();

  // 決定ボタンの更新
  UpdateDecideBtn();

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
void LowerFarmListDraw::Close( void )
{
  if( m_pPaneList )
  {
    m_pPaneList->Delete();
  }

  this->SetInputEnabled( false );
  this->SetVisible( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   きのみリスト画面を開けるか？
 *
 * @return  "true  == 開ける"
 * @return  "false == 開けない"
 */
//------------------------------------------------------------------------------
bool LowerFarmListDraw::IsCanOpen( void )
{
  // アイテムアイコンの読み込みが完了したかどうか
  return IsLoadedItemIcon();
}


//------------------------------------------------------------------------------
/**
 * @brief   アイテムアイコンの生成
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::CreateItemIcon( void )
{
  // アイテムアイコン
  app::tool::ItemIconToolEx::ItemIconToolParam itemIconParam;
  {
    itemIconParam.pUIView = this;
    itemIconParam.pHeap   = m_pIconHeap;
    itemIconParam.iconMax = GFL_NELEMS(ItemTable_NUTS);//LIST_BTN_MAX + FIELD_PUT_MAX;
    itemIconParam.sync    = true;
    itemIconParam.allocBuffer = true;
  }
  m_pDrawerBase->CreateItemIcon( itemIconParam );

  app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();
  {
    for( u32 i=0; i<GFL_NELEMS(ItemTable_NUTS); ++i )
    {
      itemIcon->LoadItemIcon( i, ItemTable_NUTS[i] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムアイコンのロードが完了したか
 *
 * @return  "true  == 完了"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool LowerFarmListDraw::IsLoadedItemIcon( void )
{
  app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();
  {
    for( u32 i=0; i<GFL_NELEMS(ItemTable_NUTS); ++i )
    {
      // アイコンの読み込み完了チェック
      if( !itemIcon->IsLoadItemIcon( i ) )
      {
        // まだ読み込み中
        return false;
      }
    }
  }

  // 読み込みが完了
  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの初期化
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::InitPaneList( void )
{
  static const u32 c_ListPartsIndex[] = {
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_00,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_01,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_02,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_03,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_04,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_05,
    PANENAME_RESORT_KINOMILIST_LOW_000_PANE_KINOMISELECTBTN_06,
  };

  static const u32 c_ListBtnAnim[] = {
    LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_00_TOUCH,
    LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_01_TOUCH,
    LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_02_TOUCH,
    LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_03_TOUCH,
    LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_04_TOUCH,
    LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_05_TOUCH,
    LA_RESORT_RESORT_KINOMILIST_LOW_000__KINOMISELECTBTN_06_TOUCH,
  };

  for( u32 i=0; i<LIST_BTN_MAX; ++i )
  {
    gfl2::lyt::LytParts* parts =
      m_pDrawerBase->GetParts( c_ListPartsIndex[i] );

    app::tool::PaneList::PANE_DATA* data = &m_paneData[i];
    {
      data->base        = parts;
      data->text        = m_pDrawerBase->GetTextBox( PANENAME_RESORT_BTN_LOW_001_PANE_TEXTBOX_00, parts );
      data->bound       = m_pDrawerBase->GetBounding( PANENAME_RESORT_BTN_LOW_001_PANE_BOUND_00, parts );
      data->cursor_pos  = NULL;
      
      const u32 animID = c_ListBtnAnim[i];
      {
        data->holdAnime    = animID + BTN_ANIM_TOUCH;
        data->releaseAnime = animID + BTN_ANIM_RELEASE;
        data->cancelAnime  = animID + BTN_ANIM_CANCEL;
        data->selectAnime  = animID + BTN_ANIM_TOUCH_RELEASE;
      }
    }

    m_pItemIcon[i]      = m_pDrawerBase->GetPicture( PANENAME_RESORT_BTN_LOW_001_PANE_KINOMIICON, parts );
    m_pItemCount[i]     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_BTN_LOW_001_PANE_ITEMNUMB, parts );
    m_pListBtnParts[i]  = parts;
  }
  
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetSystemHeap();
  {
    // ペインリストの生成
    m_pPaneList = GFL_NEW(heap) ResortPaneList( m_pDrawerBase, this, this );
    {
      GFL_ASSERT( m_pPaneList );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの生成
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::CreatePaneList( void )
{
  ResortPaneList::SETUP_PARAM param;
  {
    param.listMax       = m_ItemMax;
    param.paneMax       = LIST_BTN_MAX;
    param.viewCount     = 5;
    param.listStartPane = PANENAME_RESORT_KINOMILIST_LOW_000_PANE_LISTALL;
    param.defPY         = 60.0f;
    param.paneWidth     = 224.0f;
    param.paneHeight    = 30.0f;//27.0f;
    param.scrollPane    = PANENAME_RESORT_KINOMILIST_LOW_000_PANE_SCROLL_00;
    param.moveSE        = SEQ_SE_SELECT1;
    param.decideSE      = SEQ_SE_DECIDE1;
  }

  if( m_pPaneList )
  {
    // ペインリストの生成
    m_pPaneList->Create( param, m_paneData );
    // 入力を有効に
    m_pPaneList->SetInputEnable( true );  // @fix GFNMCat[2884]
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの更新
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::UpdatePaneList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  u16 itemNo  = ITEM_DUMMY_DATA;
  u32 itemNum = 0;
  
  // アイテム情報の取得
  SetItemInfo( pos, &itemNo, &itemNum );

  // インフォメーションウィンドウに渡すデータ
  m_ItemNo[pane_index] = itemNo;
  
  // アイテム名の表示
  SetItemName( pane[pane_index].text, itemNo, itemNum );
  // アイテム数の表示
  SetItemNum( pane_index, itemNum );
  // アイテムアイコンの差し替え
  SetItemIcon( pane_index, itemNo, itemNum );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの更新
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::RequestUpdatePaneList( void )
{
  // インフォウィンドウに渡すデータの初期化  // @fix GFNMCat[2641]
  for( u32 i=0; i<LIST_BTN_MAX; ++i )
  {
    m_ItemNo[i] = ITEM_DUMMY_DATA;
  }

  // きのみポケット内のアイテム数を取得
  m_ItemMax = m_pItemSave->GetItemCount( ITEM_POKET_KINOMI, false );

  ResortPaneList::SETUP_PARAM param;
  {
    param.listMax       = m_ItemMax;
    param.paneMax       = LIST_BTN_MAX;
    param.viewCount     = 5;
    param.listStartPane = PANENAME_RESORT_KINOMILIST_LOW_000_PANE_LISTALL;
    param.defPY         = 60.0f;
    param.paneWidth     = 224.0f;
    param.paneHeight    = 30.0f;//27.0f;
    param.scrollPane    = PANENAME_RESORT_KINOMILIST_LOW_000_PANE_SCROLL_00;
    param.moveSE        = SEQ_SE_SELECT1;
    param.decideSE      = SEQ_SE_DECIDE1;
  }

  if( m_pPaneList )
  {
    m_pPaneList->ResetPaneList( param );

    // 入力を有効に
    m_pPaneList->SetInputEnable( true );  // @fix NMCat[1635]
  }

  // きのみ未所持のアラートメッセージの表示
  SetAlertMessage();
}

//------------------------------------------------------------------------------
/**
 * @brief   インフォメーションボタンの有効設定更新
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::UpdateInfoBtn( void )
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
        m_ItemNo[i] = ITEM_DUMMY_DATA;

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
 * @brief   アイテム名の表示
 *
 * @param   textBox   テキストボックス
 * @param   itemNo    アイテムNo
 * @param   itemNum   アイテムの数
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::SetItemName( gfl2::lyt::LytTextBox* textBox, u16 itemNo, u32 itemNum )
{
  // テキストカラーの変更
  nw::ut::Color8 color;
  {
    if( itemNum > 0 )
    {
      color = nw::ut::Color8::BLACK;
    }
    else {
      color = nw::ut::Color8::GRAY;
    }
    
    textBox->SetTextColor( color, color );
  }

  // アイテム名のセット
  print::WordSet* wordSet = m_pDrawerBase->GetWordSet();
  {
    wordSet->RegisterItemName( 0, itemNo, 1 );
  }
  // メッセージのセット
  this->SetTextboxPaneMessage( textBox, msg_prsys_common_05 );
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテム数の表示
 *
 * @param   pane_index    ペインのインデックス
 * @param   itemNum       アイテムの数
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::SetItemNum( u32 pane_index, u32 itemNum )
{
  if( itemNum > 0 )
  {
    gfl2::lyt::LytTextBox* textBox = m_pItemCount[pane_index];
    {
      // アイテム数のセット
      print::WordSet* wordSet = m_pDrawerBase->GetWordSet();
      {
        wordSet->RegisterNumber( 0, itemNum, 3, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT );
      }
      // メッセージのセット
      this->SetTextboxPaneMessage( textBox, msg_prsys_common_04 );
      // テキストボックスの表示
      m_pDrawerBase->SetVisiblePane( textBox, true );
    }
  }
  else {
    gfl2::lyt::LytTextBox* textBox = m_pItemCount[pane_index];
    {
      // テキストボックスの表示
      m_pDrawerBase->SetVisiblePane( textBox, false );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムアイコンの差し替え
 *
 * @param   pane_index    ペインのインデックス
 * @param   itemNo        アイテムNo
 * @param   itemNum       アイテム数
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::SetItemIcon( u32 pane_index, u16 itemNo, u32 itemNum )
{
  if( itemNum > 0 )
  {
    gfl2::lyt::LytPicture* pic = m_pItemIcon[pane_index];
    {
      // アイコンの差し替え
      //itemIcon->SetItemIcon( pane_index, itemNo, pic );
      _SetItemIcon( itemNo, pic );

      // ペインの表示
      m_pDrawerBase->SetVisiblePane( pic, true );
    }
  }
  else {
    gfl2::lyt::LytPicture* pic = m_pItemIcon[pane_index];
    {
      // ペインの表示
      m_pDrawerBase->SetVisiblePane( pic, false );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムアイコンの差し替え
 *
 * @param   itemNo    アイテムNo
 * @param   picture   ピクチャペイン
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::_SetItemIcon( u16 itemNo, gfl2::lyt::LytPicture* picture )
{
  app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();
  {
    if( !itemIcon ) return;

    for( u32 i=0; i<GFL_NELEMS(ItemTable_NUTS); ++i )
    {
      if( itemNo == ItemTable_NUTS[i] )
      {
        itemIcon->ReplaceIcon( i, picture );

        break;    //!< 差し替えたので終了させる
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   きのみ未所持のアラートメッセージの表示
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::SetAlertMessage( void )
{
  bool visible = (m_ItemMax == 0);
  {
    m_pDrawerBase->SetVisiblePane( m_pAlertText, visible );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   植えるきのみの追加
 * @param   pos   データリストのインデックス
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::AddNuts( u32 pos )
{
  const Savedata::MyItem::ITEM_ST* itemST = m_pItemSave->GetItem( ITEM_POKET_KINOMI, pos );

  // 
  if( itemST == NULL )
  {
    GFL_ASSERT(0);
    return;
  }
  if( itemST->num == 0 )
  {
    return;
  }
  
  // アイテムNo
  u16 itemNo = itemST->id;

  for( u32 i=0; i<FIELD_PUT_MAX; ++i )
  {
    
    if( !m_FieldItemEnable[i] ) continue;
    if( m_FieldItemNo[i] != ITEM_DUMMY_DATA ) continue;

    FUKUSHIMA_PRINT( "AddNuts : Pos[%d], ItemNo[%d]", i, itemNo );
    
    // はたけに植えるきのみをセット
    m_FieldItemNo[i] = itemNo;
    // アイテムを減らす
    m_pItemSave->Sub( itemNo, 1 );
    
    m_FieldAddCount++;
    
    break;
  }

  // ボタンの有効設定の変更
  UpdateRemoveBtn();
  // はたけの状態を更新
  UpdateField();
  // 決定ボタンの更新
  UpdateDecideBtn();
  // ペインリストの更新リクエスト
  RequestUpdatePaneList();
}

//------------------------------------------------------------------------------
/**
 * @brief   植えたきのみを除外
 *
 * @param   pos   データリストのインデックス
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::RemoveNuts( u32 pos )
{
  GFL_ASSERT( pos < FIELD_PUT_MAX );

  // 植えたきのみの戻す
  u16 removeItemNo = m_FieldItemNo[pos];
  {
    m_pItemSave->Add( removeItemNo, 1 );

    m_FieldItemNo[pos] = ITEM_DUMMY_DATA;
    m_FieldAddCount--;
    
    FUKUSHIMA_PRINT( "RemoveItemNo=[%d]\n", removeItemNo );
  }

  // はたけ内のきのみを詰める(現在植えようとしているきのみのみ)
  u32 offset = pos;
  {
    for( u32 i=pos+1; i<FIELD_PUT_MAX; ++i )
    {
      if( !m_FieldItemEnable[i] ) continue;
      if( m_FieldItemNo[i] == ITEM_DUMMY_DATA ) continue;

      // 詰める
      m_FieldItemNo[offset] = m_FieldItemNo[i];
      // 元のは空にしておく
      m_FieldItemNo[i] = ITEM_DUMMY_DATA;

      offset = i;
    }
  }

  // ボタンの有効設定の変更
  UpdateRemoveBtn();
  // はたけの状態を更新
  UpdateField();
  // 決定ボタンの更新
  UpdateDecideBtn();
  // ペインリストの更新リクエスト
  RequestUpdatePaneList();
}

//------------------------------------------------------------------------------
/**
 * @brief   除外ボタンの有効設定の更新
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::UpdateRemoveBtn( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    for( u32 i=0; i<FIELD_PUT_MAX; ++i )
    {
      if( m_FieldItemEnable[i] && m_FieldItemNo[i] != ITEM_DUMMY_DATA )
      {
        btnMan->SetButtonActive( BTN_ID_REMOVE_00 + i );
        m_pDrawerBase->SetVisiblePane( m_pRemoveMarkPane[i], true );
      }
      else {
        btnMan->SetButtonPassive( BTN_ID_REMOVE_00 + i );
        m_pDrawerBase->SetVisiblePane( m_pRemoveMarkPane[i], false );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   はたけの状態を更新
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::UpdateField( void )
{
  for( u32 i=0; i<FIELD_PUT_MAX; ++i )
  {
    u16 itemNo = m_FieldItemNo[i];
    bool vis = (itemNo != ITEM_DUMMY_DATA);

    if( vis )
    {
      _SetItemIcon( itemNo, m_pFieldItemIcon[i] );
    }

    m_pDrawerBase->SetVisiblePane( m_pFieldItemIcon[i], vis );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   インフォメーションウィンドウの表示
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::OpenInformationWindow( u32 pos )
{
  GFL_ASSERT( pos < LIST_BTN_MAX );

  if( m_pInfoWindow )
  {
    // アイテムNo
    u16 itemNo = m_ItemNo[pos];

    // ボタンの座標位置
    gfl2::math::Vector3 btnPos( 0, 0, 0 );
    {
      gfl2::lyt::LytParts* parts = m_pListBtnParts[pos];
      {
        gfl2::math::VEC3 basePos = parts->GetTranslate();
        gfl2::math::VEC3 childPos(0, 0, 0);

        gfl2::lyt::LytPane* childPane =
          m_pDrawerBase->GetPane( PANENAME_RESORT_BTN_LOW_001_PANE_INFOBUTTON, parts );
        {
          childPos = childPane->GetTranslate();
          btnPos   = gfl2::math::ConvertNwVec3ToGfVec3( basePos + childPos );
        }
      }
    }

    m_pInfoWindow->Open( itemNo, btnPos );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   決定ボタンの更新
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::UpdateDecideBtn( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  if( m_FieldAddCount == 0 )
  {
    if( btnMan->IsButtonActive( BTN_ID_DECIDE ) )
    {
      btnMan->SetButtonPassive( BTN_ID_DECIDE );
    }
  }
  else {
    if( !btnMan->IsButtonActive( BTN_ID_DECIDE ) )
    {
      btnMan->SetButtonActive( BTN_ID_DECIDE );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   決定処理
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::DecideFunc( void )
{
  for( u32 i=0; i<FIELD_PUT_MAX; ++i )
  {
    if( m_FieldItemEnable[i] && m_FieldItemNo[i] != ITEM_DUMMY_DATA )
    {
      u32 index = m_FieldIndex * FIELD_PUT_MAX + i;

      m_uiListener->PlantKinomi( index, m_FieldItemNo[i] );

      if( !m_DiaryFlag )
      {
        // ダイアリーのセット
        PokeDiary::Set( PDID_COUNT_PLANT_SEED, m_FieldItemNo[i] );
        m_DiaryFlag = true;
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キャンセル処理
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::CancelFunc( void )
{
  // 植える予定のきのみを元に戻す
  for( u32 i=0; i<FIELD_PUT_MAX; ++i )
  {
    if( !m_FieldItemEnable[i] ) continue;
    
    u16 itemNo = m_FieldItemNo[i];
    
    if( itemNo != ITEM_DUMMY_DATA )
    {
      m_pItemSave->Add( itemNo, 1 );
      
      m_FieldItemNo[i] = ITEM_DUMMY_DATA;
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   アイテム情報の取得
 *
 * @param   index     [IN]データインデックス
 * @param   itemNo    [OUT]道具No
 * @param   itemNum   [OUT]道具の数
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::SetItemInfo( u32 index, u16* itemNo, u32* itemNum )
{
  const Savedata::MyItem::ITEM_ST* itemST = m_pItemSave->GetItem( ITEM_POKET_KINOMI, index );

  // 
  if( itemST == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  if( itemNo )
  {
    *itemNo  = static_cast<u16>(itemST->id);
  }
  if( itemNum )
  {
    *itemNum = itemST->num;
  }
  
  FUKUSHIMA_PRINT( "SetItemInfo : ItemNo[%d], ItemNum[%d]\n", *itemNo, *itemNum );
}






//==============================================================================
/**
 *      app::ui::UIInputListener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 *
 * @param   paneID
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerFarmListDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  switch( paneID )
  {
  case BTN_ID_CANCEL:
    {
      CancelFunc();
      
      m_Result = RESULT_CANCEL;
    } break;

  case BTN_ID_DECIDE:
    {
      DecideFunc();

      m_Result = RESULT_DECIDE;
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

  case BTN_ID_REMOVE_00:
  case BTN_ID_REMOVE_01:
  case BTN_ID_REMOVE_02:
  case BTN_ID_REMOVE_03:
  case BTN_ID_REMOVE_04:
  case BTN_ID_REMOVE_05:
    {
      u32 pos = paneID - BTN_ID_REMOVE_00;
      RemoveNuts( pos );
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
void LowerFarmListDraw::OnLayoutPaneStartingAnimeEvent( const u32 paneID )
{
  switch( paneID )
  {
  case BTN_ID_CANCEL:     // @fix GFNMCat[2884]
  case BTN_ID_DECIDE:     // @fix NMCat[1636]
  case BTN_ID_INFO_00:    // @fix GFNMCat[3186]
  case BTN_ID_INFO_01:    // @fix GFNMCat[3186]
  case BTN_ID_INFO_02:    // @fix GFNMCat[3186]
  case BTN_ID_INFO_03:    // @fix GFNMCat[3186]
  case BTN_ID_INFO_04:    // @fix GFNMCat[3186]
  case BTN_ID_INFO_05:    // @fix GFNMCat[3186]
  case BTN_ID_INFO_06:    // @fix GFNMCat[3186]
  case BTN_ID_REMOVE_00:  // @fix NMCat[1635]
  case BTN_ID_REMOVE_01:  // @fix NMCat[1635]
  case BTN_ID_REMOVE_02:  // @fix NMCat[1635]
  case BTN_ID_REMOVE_03:  // @fix NMCat[1635]
  case BTN_ID_REMOVE_04:  // @fix NMCat[1635]
  case BTN_ID_REMOVE_05:  // @fix NMCat[1635]
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
 *      app::tool::PaneListView::Listener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   リストの項目を描画するコールバック関数
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  UpdatePaneList( pane, pane_index, pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリスト決定時に呼ばれるリスナー
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::PaneListItemSelectListener( u32 pos )
{
  AddNuts( pos );
}





//==============================================================================
/**
 *      app::tool::ButtonManager
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンの個別セットアップ
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerFarmListDraw::SetupButton( app::tool::ButtonManager* btnMan )
{
  // 除外ボタンをパッシブ
  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    if( i >= BTN_ID_REMOVE_00 && i <= BTN_ID_REMOVE_05 )
    {
      btnMan->SetButtonPassive( i );
    }
  }
  
  // SE設定
  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    if( i == BTN_ID_CANCEL )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
    else if( i < BTN_ID_INFO_00 || i > BTN_ID_INFO_06 )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)
