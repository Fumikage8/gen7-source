//======================================================================
/**
 * @file    StatusRibbon.cpp
 * @date    2015/12/04 14:39:41
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：リボン(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusRibbon.h"

#include <App/Status/source/StatusDrawListener.h>
#include <App/Status/source/StatusDrawerBase.h>
#include <App/Status/source/StatusLowerDraw.h>
#include <AppLib/include/Ui/UIViewManager.h>
#include <AppLib/include/Util/app_util_ScrollBar.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/field_status/field_status.h>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  // @brief   リボンID
  const u32 c_RibbonIDList[PokeTool::RIBBON_NO_MAX] = {
    PokeTool::RIBBON_NO_NIJI_CROWN,
    PokeTool::RIBBON_NO_NIJI_ROYAL,
    PokeTool::RIBBON_NO_NIJI_BTLTOWER,
    PokeTool::RIBBON_NO_NIJI_MASTER,
    PokeTool::RIBBON_NO_SANGO_CHAMP,
    PokeTool::RIBBON_NO_CAROS_CHAMP,
    PokeTool::RIBBON_NO_HOUEN_CHAMP,
    PokeTool::RIBBON_NO_SINOU_CHAMP,
    PokeTool::RIBBON_NO_CONTEST_STAR,
    PokeTool::RIBBON_NO_STYLE_MASTER,
    PokeTool::RIBBON_NO_BEAUTIFUL_MASTER,
    PokeTool::RIBBON_NO_CUTE_MASTER,
    PokeTool::RIBBON_NO_CLEVER_MASTER,
    PokeTool::RIBBON_NO_STRONG_MASTER,
    PokeTool::RIBBON_NO_KAWAIGARI,
    PokeTool::RIBBON_NO_TRAINING,
    PokeTool::RIBBON_NO_SUPER_BATTLE,
    PokeTool::RIBBON_NO_MASTER_BATTLE,
    PokeTool::RIBBON_NO_GANBA,
    PokeTool::RIBBON_NO_SYAKKIRI,
    PokeTool::RIBBON_NO_DOKKIRI,
    PokeTool::RIBBON_NO_SYONBORI,
    PokeTool::RIBBON_NO_UKKARI,
    PokeTool::RIBBON_NO_SUKKIRI,
    PokeTool::RIBBON_NO_GUSSURI,
    PokeTool::RIBBON_NO_NIKKORI,
    PokeTool::RIBBON_NO_GORGEOUS,
    PokeTool::RIBBON_NO_ROYAL,
    PokeTool::RIBBON_NO_GORGEOUS_ROYAL,
    PokeTool::RIBBON_NO_BROMIDE,
    PokeTool::RIBBON_NO_ASHIATO,
    PokeTool::RIBBON_NO_RECORD,
    PokeTool::RIBBON_NO_LEGEND,
    PokeTool::RIBBON_NO_COUNTRY,
    PokeTool::RIBBON_NO_NATIONAL,
    PokeTool::RIBBON_NO_EARTH,
    PokeTool::RIBBON_NO_WORLD,
    PokeTool::RIBBON_NO_CLASSIC,
    PokeTool::RIBBON_NO_PREMIERE,
    PokeTool::RIBBON_NO_EVENT,
    PokeTool::RIBBON_NO_BIRTHDAY,
    PokeTool::RIBBON_NO_SPECIAL,
    PokeTool::RIBBON_NO_MEMORIAL,
    PokeTool::RIBBON_NO_WISH,
    PokeTool::RIBBON_NO_BATTLE_CHAMP,
    PokeTool::RIBBON_NO_AREA_CHAMP,
    PokeTool::RIBBON_NO_NATIONAL_CHAMP,
    PokeTool::RIBBON_NO_WORLD_CHAMP,
    PokeTool::RIBBON_NO_LUMPING_CONTEST,
    PokeTool::RIBBON_NO_LUMPING_TOWER,
  };

  //! @brief  リボンのテクスチャ
  const u32 c_RibbonTexture[] = {
    LYTRES_FIELD_STATUS_RIBBON_05_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_03_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_04_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_06_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_07_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_08_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_09_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_11_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_12_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_13_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_14_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_15_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_16_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_17_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_18_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_19_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_20_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_21_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_10_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_22_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_23_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_24_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_25_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_26_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_27_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_28_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_29_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_30_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_31_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_32_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_33_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_34_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_35_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_36_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_37_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_38_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_39_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_01_A_BFLIM,
    //LYTRES_FIELD_STATUS_RIBBON_01_B_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_02_A_BFLIM,
    //LYTRES_FIELD_STATUS_RIBBON_02_B_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_40_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_41_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_42_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_43_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_44_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_45_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_46_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_47_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_48_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_49_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_50_BFLIM,
  };

  static const u32 c_LumpingContestRibbonTexture[] = {
    LYTRES_FIELD_STATUS_RIBBON_01_A_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_01_B_BFLIM,
  };

  static const u32 c_LumpingTowerRibbonTexture[] = {
    LYTRES_FIELD_STATUS_RIBBON_02_A_BFLIM,
    LYTRES_FIELD_STATUS_RIBBON_02_B_BFLIM,
  };

  //! @brief  リボンのパーツインデックス
  const u32 c_RibbonBtnParts[] = {
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_00,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_01,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_02,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_03,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_04,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_05,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_06,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_07,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_08,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_09,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_10,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_11,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_12,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_13,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_14,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_15,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_16,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_17,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_18,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_19,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_20,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_21,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_22,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_23,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_24,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_25,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_26,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSRIBBON_27,
  };

  //! @brief  ボタンID
  static const u32 c_ButtonID[] = {
    BTN_ID_WAZA_00,
    BTN_ID_WAZA_01,
    BTN_ID_WAZA_02,
    BTN_ID_WAZA_03,
    BTN_ID_INFO_TOKUSEI,
    BTN_ID_INFO_ITEM,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
Ribbon::Ribbon( DrawerBase* base, MenuCursor* cursor, gfl2::ui::DeviceManager* devMan, RibbonListener* listener )
  : m_pBase(base)
  , m_pCursor(cursor)
  , m_pDevMan(devMan)
  , m_pScrollPaneManager(NULL)
  , m_pListener(NULL)
  , m_pPokeParam(NULL)
  , m_pRibbonListener(listener)
  , m_CursorPos(0)
  , m_LastRibbonIndex(0)
  , m_isOpen(false)
  , m_InputExclusive(false)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
Ribbon::~Ribbon( void )
{
  if( m_isOpen )
  {
    _SetInputExclusive( m_InputExclusive );
  }

  // スクロールペインマネージャの破棄
  DeleteScrollPaneManager();
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   listener    リスナー
 */
//------------------------------------------------------------------------------
void Ribbon::Initialize( StatusDrawListener* listener )
{
  // リスナーのセット
  m_pListener = listener;

  // スクロールペインのセット
  SetScrollPane();

  // カーソルのセットアップ
  if( m_pCursor )
  {
    m_pCursor->SetupCursor( m_pBase->GetG2D(), NULL, 0, gfl2::lyt::DISPLAY_LOWER );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   スクロールペインのセット
 */
//------------------------------------------------------------------------------
void Ribbon::SetScrollPane( void )
{
  const app::tool::ScrollPaneManager::PaneInfo c_DefInfo = {
    NULL, NULL, NULL, NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
  };

  // スクロールペインのセット
  for( u32 i=0; i<RIBBON_BTN_NUM; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pBase->GetParts( c_RibbonBtnParts[i] );

    // リボンピクチャペイン
    m_pRibbonPane[i] = m_pBase->GetPicture( PANENAME_STATUS_BTN_LOW_000_PANE_RIBBON_01_A, parts );

    m_InfoArray[i] = c_DefInfo;

    m_InfoArray[i].root_pane     = parts;
    m_InfoArray[i].picture_pane  = m_pRibbonPane[i];
    m_InfoArray[i].bounding_pane = m_pBase->GetPane( PANENAME_STATUS_BTN_LOW_000_PANE_BOUND_00, parts );
    m_InfoArray[i].cursor_pane   = m_pBase->GetPane( PANENAME_STATUS_BTN_LOW_000_PANE_NULL_CUR, parts );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void Ribbon::Update( void )
{
  if( m_isOpen )
  {
    app::tool::MenuCursor* menuCursor = m_pCursor->GetMenuCursor();
    gfl2::math::VEC3 cursorPos = m_pBase->GetPanePos( m_InfoArray[ m_CursorPos ].root_pane );

    cursorPos.x += 80.0f;   //!< なんか左にずれていたので右にずらす

    menuCursor->SetPos( &cursorPos );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void Ribbon::Draw( void )
{
  if( m_pScrollPaneManager )
  {
    if( m_pScrollBar )
    {
      m_pScrollBar->Draw( m_pScrollPaneManager->GetScrollValue() );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Ribbon::Setup( const pml::pokepara::PokemonParam* pp )
{
  //if( m_pPokeParam != pp )
  {
    m_pPokeParam = pp;

    // 所持リボンのセットアップ
    m_HaveRibbonNum = SetupHaveRibbon( m_pPokeParam );

    // 開いている時はスクロールペインマネージャを生成しなおす
    if( m_isOpen )
    {
      // 破棄
      DeleteScrollPaneManager();
      // 生成
      CreateScrollPaneManager( m_HaveRibbonNum );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   所持リボンのセットアップ
 *
 * @param   pp    ポケモンパラメータ
 *
 * @return  所持リボンの数
 */
//------------------------------------------------------------------------------
u32 Ribbon::SetupHaveRibbon( const pml::pokepara::PokemonParam* pp )
{
  u32 haveRibbon = 0;

  bool fastmode_flag = pp->StartFastMode();
  {
    // リボン数を調べる
    for( u32 i=0; i<PokeTool::RIBBON_NO_MAX; ++i )
    {
      const u32 ribbonID = c_RibbonIDList[i];

      if( ribbonID == PokeTool::RIBBON_NO_LUMPING_CONTEST || ribbonID == PokeTool::RIBBON_NO_LUMPING_TOWER )
      {
        u32 num = pp->GetLumpingRibbon( ribbonID );

        // リボンを１以上所持していればカウント
        if( num > 0 )
        {
          // 所持しているリボンをセット
          m_RibbonKind[haveRibbon] = ribbonID;

          haveRibbon++;
        }
      }
      else {
        if( pp->HaveRibbon( ribbonID ) )
        {
          // 所持しているリボンをセット
          m_RibbonKind[haveRibbon] = ribbonID;

          // リボンを所持していればカウント
          haveRibbon++;
        }
      }
    }
  }
  pp->EndFastMode( fastmode_flag );

  return haveRibbon;
}


//------------------------------------------------------------------------------
/**
 * @brief   スクロールペインマネージャの生成
 *
 * @param   ribbonNum   所持リボン数
 */
//------------------------------------------------------------------------------
void Ribbon::CreateScrollPaneManager( u32 ribbonNum )
{
  // 生成パラメータのセットアップ
  app::tool::ScrollPaneManager::Description param;
  SetupDescription( &param, ribbonNum );

  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  // スクロールペインマネージャの生成
  m_pScrollPaneManager = GFL_NEW(devHeap) app::tool::ScrollPaneManager( param );
  {
    GFL_ASSERT( m_pScrollPaneManager );

    // アナログキーの入力をOFF
    SetInputKeyEnabled( false );

    // サブビューに追加
    app::ui::UIView* view = m_pBase->GetUIView();
    view->AddSubView( m_pScrollPaneManager );
  }

  // スクロールバーの生成
  CreateScrollBar();

  m_LastRibbonIndex = 0;
}

//------------------------------------------------------------------------------
/**
 * @brief   スクロールペインマネージャの破棄
 */
//------------------------------------------------------------------------------
void Ribbon::DeleteScrollPaneManager( void )
{
  // スクロールバーの破棄
  DeleteScrollBar();

  if( m_pScrollPaneManager )
  {
    m_pScrollPaneManager->RemoveFromSuperView();

    GFL_DELETE m_pScrollPaneManager;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   スクロールペインマネージャ生成パラメータのセットアップ
 *
 * @param   desc        生成パラメータ
 * @param   ribbonNum   所持リボン数
 */
//------------------------------------------------------------------------------
void Ribbon::SetupDescription( app::tool::ScrollPaneManager::Description* desc, u32 ribbonNum )
{
  desc->heap                  = m_pBase->GetHeap();     //!< ヒープ
  desc->layout_work           = m_pBase->GetLytWk();    //!< レイアウトワーク
  desc->ui_device_manager     = m_pDevMan;              //!< デバイスマネージャ

  desc->pane_table            = m_InfoArray;            //!< ペインデータテーブル
  desc->pane_table_num        = RIBBON_BTN_NUM;         //!< ペインデータ数

  desc->list_data_num         = ribbonNum;              //!< リストに表示する数
  desc->col                   = 4;                      //!< 列→方向個数（X) 最低1以上

  //desc->pane_start_x          = -60;                    //!< ペインのデフォルトX座標 ( listPos = 0 の位置のペイン座標、この位置からcol個並ぶ )
  //desc->pane_start_y          = 100;                    //!< ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標、この位置からならぶ )
  desc->pane_start_x          = -56;                    //!< ペインのデフォルトX座標 ( listPos = 0 の位置のペイン座標、この位置からcol個並ぶ )
  desc->pane_start_y          = 104;                    //!< ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標、この位置からならぶ )
  desc->pane_width            = 40;                     //!< ペインのXサイズ
  desc->pane_height           = 40;                     //!< ペインのYサイズ

  // 下記はペイン表示エリア  エリア外へいくと
  desc->pane_draw_area_top    = 120;                    //!< ペインを表示する領域  上（レイアウト座標）
  desc->pane_draw_area_left   = -56;                    //!< ペインを表示する領域  左（レイアウト座標）
  desc->pane_draw_area_down   = -120;                   //!< ペインを表示する領域  下（レイアウト座標）
  desc->pane_draw_area_right  = 104;                    //!< ペインを表示する領域  右（レイアウト座標）

  // 下記は入力エリアの開始座標(左上の座標値を指定)
  desc->pane_input_start_x    = 0;                      //!< ペイン入力エリアの開始X座標(レイアウト座標)
  desc->pane_input_start_y    = 100;                    //!< ペイン入力エリアの開始Y座標(レイアウト座標)

  // 下記はペインへの入力エリア
  desc->pane_input_limit_col  = 4;                      //!< ペインへ入力できる領域  行
  desc->pane_input_limit_row  = 6;                      //!< ペインへ入力できる領域  列

  desc->framerate             = GameSys::FRAMEMODE_30;  //!< ゲームフレームレート
  desc->menu_cursor           = m_pCursor->GetMenuCursor();   //!< メニューカーソル
  desc->put_cursor_visible    = false;                  //!< カーソルの位置設定時にカーソルを表示するかどうか(true = Put, false = PutNonVisible)
  desc->listener              = this;                   //!< リスナー

  desc->default_cursor_pos    = 0;                      //!< デフォルト位置

  desc->mode  = app::tool::ScrollPaneManager::MODE_NORMAL;
}

//------------------------------------------------------------------------------
/**
 * @brief   スクロールバーの生成
 */
//------------------------------------------------------------------------------
void Ribbon::CreateScrollBar( void )
{
  GFL_ASSERT( m_pScrollPaneManager );

  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();

  // セットアップパラメータ
  app::util::ScrollBar::SETUP_PARAM setup = {
    heap,
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
    m_pScrollPaneManager->GetScrollMax(),

    m_pBase->GetLytWk(),
    m_pBase->GetLytMultiResID(),

    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_SCROLL,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL,
    NULL,
    NULL,
    NULL,
  };

  // スクロールバーの生成
  m_pScrollBar = GFL_NEW(heap) app::util::ScrollBar( &setup );
  {
    GFL_ASSERT( m_pScrollBar );

    m_pScrollBar->SetRequest( app::util::ScrollBar::REQ_BAR_ON );
    m_pScrollBar->ChangeParam( setup.max );
    m_pScrollBar->SetDrawEnable( setup.max > 0 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   スクロールバーの破棄
 */
//------------------------------------------------------------------------------
void Ribbon::DeleteScrollBar( void )
{
  GFL_SAFE_DELETE( m_pScrollBar );
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void Ribbon::Open( void )
{
  if( !m_isOpen )
  {
    // スクロールペインマネージャの破棄
    DeleteScrollPaneManager();

    // スクロールペインマネージャの生成
    CreateScrollPaneManager( m_HaveRibbonNum );

    // スクロールペインマネージャの入力を変更
    _SetInputEnable( (m_HaveRibbonNum != 0) );
    //!< @note 内部でカーソルの表示フラグを変更しているので注意
    SetVisibleCursor( false );

    // インアニメを再生
    m_pBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000_RIBBON_IN );

    m_InputExclusive = _GetInputExclusive();
    _SetInputExclusive( true );

    m_isOpen = true;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void Ribbon::Close( void )
{
  if( m_isOpen )
  {
    _SetInputExclusive( m_InputExclusive );

    // スクロールペインマネージャの入力を無効
    _SetInputEnable( false );

    // 一部のボタンの入力を元に戻す
    //SetEnableInput();

    // アウトアニメを再生
    m_pBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000_RIBBON_OUT );

    m_isOpen = false;
    m_CursorPos = 0;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   テクスチャの差し替え
 *
 * @param   paneIndex   ペインインデックス
 * @param   dataID      データID
 */
//------------------------------------------------------------------------------
void Ribbon::ReplaceTexture( u32 paneIndex, u32 dataID )
{
  GFL_ASSERT( paneIndex < RIBBON_BTN_NUM );
  GFL_ASSERT( dataID < GFL_NELEMS(c_RibbonTexture) );

  gfl2::lyt::LytRes*     lytres = m_pBase->GetLytRes();
  gfl2::lyt::LytPicture* pic    = m_pRibbonPane[paneIndex];
  
  if( pic )
  {
    // マテリアルの取得
    gfl2::lyt::LytMaterial* material = pic->GetMaterial( 0 );

    // テクスチャ情報の取得
    u32 ribbonID    = m_RibbonKind[dataID];
    u32 ribbonTexID = c_RibbonTexture[ribbonID];

    if( ribbonID == PokeTool::RIBBON_NO_LUMPING_CONTEST )
    {
      if( m_HaveRibbonNum >= PokeTool::RIBBON_COMPLETE_LUMPING_CONTEST )
      {
        ribbonTexID = c_LumpingContestRibbonTexture[1];
      }
      else {
        ribbonTexID = c_LumpingContestRibbonTexture[0];
      }
    }
    else if( ribbonID == PokeTool::RIBBON_NO_LUMPING_TOWER )
    {
      if( m_HaveRibbonNum >= PokeTool::RIBBON_COMPLETE_LUMPING_TOWER )
      {
        ribbonTexID = c_LumpingTowerRibbonTexture[1];
      }
      else {
        ribbonTexID = c_LumpingTowerRibbonTexture[0];
      }
    }

    const gfl2::lyt::LytResTextureInfo* texInfo = lytres->GetTexture( ribbonTexID, 0 );

    // テクスチャの差し替え
    material->SetTexMap( 0, texInfo );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   入力切替
 *
 * @param   enable    入力フラグ
 */
//------------------------------------------------------------------------------
void Ribbon::SetInputEnable( bool enable )
{
  if( m_isOpen )
  {
    _SetInputEnable( enable );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力切替
 *
 * @param   enable    入力フラグ
 */
//------------------------------------------------------------------------------
void Ribbon::_SetInputEnable( bool enable )
{
  if( m_pScrollPaneManager )
  {
    m_pScrollPaneManager->SetInputEnabled( enable );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アナログキーの入力切替
 * 
 * @param   enable    入力フラグ
 */
//------------------------------------------------------------------------------
void Ribbon::SetInputKeyEnabled( bool enable )
{
  if( m_pScrollPaneManager )
  {
    m_pScrollPaneManager->SetInputKeyEnabled( enable );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンの入力を元に戻す
 */
//------------------------------------------------------------------------------
void Ribbon::SetEnableInput( void )
{
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();

  for( u32 i=0; i<GFL_NELEMS(c_ButtonID); ++i )
  {
    u32 btnIdx = c_ButtonID[i];

    // 入力を元に戻す
    btnMan->SetButtonInputEnable( btnIdx, m_BtnEnable[btnIdx] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの入力を無効にする
 */
//------------------------------------------------------------------------------
void Ribbon::SetDisableInput( void )
{
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();

  for( u32 i=0; i<GFL_NELEMS(c_ButtonID); ++i )
  {
    u32 btnIdx = c_ButtonID[i];

    // 現在の入力フラグを取得
    m_BtnEnable[btnIdx] = btnMan->IsButtonInputEnable( btnIdx );
    // 入力無効に
    btnMan->SetButtonInputEnable( btnIdx, false );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   カーソルの表示切替
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void Ribbon::SetVisibleCursor( bool visible )
{
  if( m_pScrollPaneManager )
  {
    m_pScrollPaneManager->SetVisibleMenuCursor( visible );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   最後に選択したリボンIDの取得
 *
 * @return  リボンID
 */
//------------------------------------------------------------------------------
u32 Ribbon::GetLastRibbonID( void ) const
{
  if( m_HaveRibbonNum == 0 )
  {
    FUKUSHIMA_PRINT( "GetLastRibbonID : m_HaveRibbonNum == 0\n" );
    return 0;
  }
  if( m_HaveRibbonNum < m_LastRibbonIndex )
  {
    FUKUSHIMA_PRINT(
      "GetLastRibbonID : m_HaveRibbonNum[%d] < m_LastRibbonIndex[%d]\n"
      , m_HaveRibbonNum, m_LastRibbonIndex );
    return 0;
  }

  return m_RibbonKind[m_LastRibbonIndex];
}


//------------------------------------------------------------------------------
/**
 * @brief   リボン説明関連
 */
//------------------------------------------------------------------------------
void Ribbon::UpdateRibbonExplain( u32 dataID )
{
  u32 ribbonID = m_RibbonKind[dataID];

  // @fix NMCat[1517]
  if( !m_pCursor->IsVisible() )
  {
    // リボン説明の表示
    if( !_OpenRibbonExplain( ribbonID ) )
    {
      // リボン説明の切り替え
      _SetRibbonExplain( ribbonID );
    }
  }
  else {
    // リボン説明の切り替え
    _SetRibbonExplain( ribbonID );
  }
}

bool Ribbon::_OpenRibbonExplain( u32 ribbonID )
{
  if( m_pListener )
  {
    return m_pListener->OpenRibbonExplain( ribbonID );
  }

  return false;
}

void Ribbon::_SetRibbonExplain( u32 ribbonID )
{
  if( m_pListener )
  {
    m_pListener->SetRibbonExplain( ribbonID );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   画面モードの設定・取得
 */
//------------------------------------------------------------------------------
void Ribbon::_SetDispMode( u8 mode )
{
  if( m_pListener )
  {
    m_pListener->SetDispMode( mode );
  }
}

u8 Ribbon::_GetDispMode( void ) const
{
  if( m_pListener )
  {
    return m_pListener->GetDispMode();
  }

  return DISP_MODE_STATUS;
}


//------------------------------------------------------------------------------
/**
 * @brief   入力モードの設定・取得
 */
//------------------------------------------------------------------------------
void Ribbon::_SetInputExclusive( bool exclusive )
{
  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetInputExclusiveKeyByButtonAnimation( exclusive );
}

bool Ribbon::_GetInputExclusive( void )
{
  return GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->GetInputExclusiveKeyByButtonAnimation();
}




app::tool::ScrollPaneManager::State Ribbon::GetUpdateState( void )
{
  app::tool::ScrollPaneManager::State state = app::tool::ScrollPaneManager::STATE_INPUT_WAIT;

  if( m_pScrollPaneManager )
  {
    state = m_pScrollPaneManager->GetState();
  }

  return state;
}





//==============================================================================
/**
 *      ↓　以下がリスナー　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   リストの項目を描画通知
 *
 * @param   pane_index    選択されたボタンのインデックス
 * @param   dataID        選択されたデータのインデックス
 * @param   is_end_async  非同期の読み込み完了フラグ
 */
//------------------------------------------------------------------------------
void Ribbon::WriteTexture( u32 pane_index, u32 dataID, bool is_end_async )
{
  if( is_end_async )
  {
    // テクスチャの差し替え
    ReplaceTexture( pane_index, dataID );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   決定ボタンの押下通知
 *
 * @param   pane_index  選択されたボタンのインデックス
 * @param   dataID      選択されたデータのインデックス
 */
//------------------------------------------------------------------------------
void Ribbon::Decide( u32 pane_index, u32 dataID )
{
  // 
  if( _GetDispMode() == DISP_MODE_STATUS )
  {
    // タブのパッシブ処理
    m_pRibbonListener->TabPassive();

    // 画面モードをメモへ
    _SetDispMode( DISP_MODE_MEMO );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルの選択通知
 *
 * @param   pane_index  選択されたボタンのインデックス
 * @param   dataID      選択されたデータのインデックス
 */
//------------------------------------------------------------------------------
void Ribbon::OnCursor( u32 pane_index, u32 dataID )
{
  m_CursorPos       = pane_index;
  m_LastRibbonIndex = dataID;

  if( _GetDispMode() == DISP_MODE_MEMO )
  {
    // リボン説明の更新
    UpdateRibbonExplain( dataID );

    // キャンセルボタンの入力を有効
    m_pRibbonListener->SetCancelBtnEnable( true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   スクロールの通知
 *
 * @param   move    移動量
 */
//------------------------------------------------------------------------------
void Ribbon::OnScroll( f32 move )
{
  // キャンセルボタンの入力を無効
  m_pRibbonListener->SetCancelBtnEnable( false );

  // リボンのスクロール時はタブのパッシブ処理を行う
  if( _GetDispMode() == DISP_MODE_STATUS )
  {
    // タブのパッシブ処理
    m_pRibbonListener->TabPassive();

    // 画面モードをメモへ
    _SetDispMode( DISP_MODE_MEMO );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   リボンリストを開いているかどうか
 *
 * @return  "true  = 開いている"
 * @return  "false = 閉じている"
 */
//------------------------------------------------------------------------------
bool Ribbon::IsRibbonListOpen( void ) const
{
  return m_isOpen;
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
