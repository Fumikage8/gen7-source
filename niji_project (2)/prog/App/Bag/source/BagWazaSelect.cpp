//=============================================================================
/**
 * @file    BagWazaSelect.cpp
 * @brief   バッグ画面技選択
 * @author  Hiroyuki Nakamura
 * @date    2015.04.21
 */
//=============================================================================

// module
#include "BagWazaSelect.h"
#include "App/Bag/include/BagAppParam.h"
// pmlib
#include <pml/include/pmlib.h>
#include <pml/include/poketype_def.h>
// niji
#include "AppLib/include/Util/app_util_Common.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "Sound/include/Sound.h"
// resource
#include "niji_conv_header/message/msg_bag.h"
#include "niji_conv_header/app/common_bag/CommonBag.h"
#include "niji_conv_header/app/common_bag/CommonBag_pane.h"
#include "niji_conv_header/app/common_bag/CommonBag_anim_list.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param         外部設定パラメータ
 * @param   heap          ヒープ
 * @param   render_man    描画マネージャ
 * @param   is_demo_mode  デモモードフラグ
 */
//-----------------------------------------------------------------------------
BagWazaSelect::BagWazaSelect(
      const APP_PARAM * param,
      app::util::Heap * heap,
      app::util::AppRenderingManager * render_man,
      app::tool::AppCommonGrpIconData * common_icon,
      const bool * is_demo_mode )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_appParam( param )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_commonIcon( common_icon )
  , m_pMenuCursor( NULL )
  , m_result( RESULT_NONE )
  , m_cursorPos( 0 )
  , m_seq( SEQ_NONE )
  , m_isDemoMode( is_demo_mode )
  , m_isDrawEnable( true )
{
  SetInputEnabled( false );   // @note Bボタンをバインドするとメイン画面のBボタンが反応しなくなるため
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
BagWazaSelect::~BagWazaSelect()
{
  DeleteMenuCursor();
  Delete2D();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputEnabled
 * @brief   入力判定の切り替え
 * @date    2015.06.25
 *
 * @param   flg   true = 有効, false = 無効
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::SetInputEnabled( bool flg )
{
  if( *m_isDemoMode != false )
  {
    flg = false;
  }
  app::ui::UIResponder::SetInputEnabled( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   データ設定
 * @date    2015.04.21
 *
 * @param   lyt_buf       レイアウトリソースバッファ
 * @param   msg_data      メッセージデータ
 * @param   wset          単語セット
 * @param   cursor_buf    メニューカーソルのリソースバッファ
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::Setup( void * lyt_buf, gfl2::str::MsgData * msg_data, print::WordSet * wset, void ** cursor_buf )
{
  CreateLayout( lyt_buf, msg_data, wset );
  SetDefaultString();
  CreateButton();
  CreateMenuCursor( cursor_buf );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsModuleEnd
 * @brief   終了できるかチェック
 * @date    2015.08.21
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool BagWazaSelect::IsModuleEnd(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  if( m_pMenuCursor->GetLayoutWork()->IsDrawing() != false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateLayout
 * @brief   レイアウト生成
 * @date    2015.04.21
 *
 * @param   buf       レイアウトリソースバッファ
 * @param   msg_data  メッセージデータ
 * @param   wset      単語セット
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::CreateLayout( void * buf, gfl2::str::MsgData * msg_data, print::WordSet * wset )
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_COMMONBAG_BAG_PPX_LOW_000_BFLYT,
      LA_COMMONBAG_BAG_PPX_LOW_000___NUM,
      buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false
    },
  };

  Create2D(
    m_heap,
    NULL,
    GFL_NELEMS(data_tbl),
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    msg_data,
    wset );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetParts
 * @brief   項目の部品ペインを取得
 * @date    2015.04.21
 *
 * @param   id  項目ID
 *
 * @return  部品ペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytParts * BagWazaSelect::GetParts( u32 id )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_BAG_PPX_LOW_000_PANE_SKILLBUTTON_00,
    PANENAME_BAG_PPX_LOW_000_PANE_SKILLBUTTON_01,
    PANENAME_BAG_PPX_LOW_000_PANE_SKILLBUTTON_02,
    PANENAME_BAG_PPX_LOW_000_PANE_SKILLBUTTON_03,
  };
  return GetG2DUtil()->GetLayoutWork(0)->GetPartsPane( tbl[id] );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPane
 * @brief   項目の部品からペインを取得
 * @date    2015.04.21
 *
 * @param   id      項目ID
 * @param   index   ペインのインデックス
 *
 * @return  ペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPane * BagWazaSelect::GetPane( u32 id, gfl2::lyt::LytPaneIndex index )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  return g2d->GetLayoutWork(0)->GetPane( GetParts(id), index, &res_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetTextBoxPane
 * @brief   項目の部品からテキストボックスペインを取得
 * @date    2015.04.21
 *
 * @param   id      項目ID
 * @param   index   ペインのインデックス
 *
 * @return  テキストボックスペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytTextBox * BagWazaSelect::GetTextBoxPane( u32 id, gfl2::lyt::LytPaneIndex index )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  return g2d->GetLayoutWork(0)->GetTextBoxPane( GetParts(id), index, &res_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDefaultString
 * @brief   デフォルト文字列を描画
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::SetDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ )
  {
    //「PP」
    g2d->SetTextBoxPaneString( GetTextBoxPane(i,PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_01), msg_bag_01_10 );
    //「/」
    g2d->SetTextBoxPaneString( GetTextBoxPane(i,PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_04), msg_bag_01_11 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::CreateButton(void)
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_BAG_PPX_LOW_000_PANE_SKILLBUTTON_00,
    PANENAME_BAG_PPX_LOW_000_PANE_SKILLBUTTON_01,
    PANENAME_BAG_PPX_LOW_000_PANE_SKILLBUTTON_02,
    PANENAME_BAG_PPX_LOW_000_PANE_SKILLBUTTON_03,
    PANENAME_BAG_PPX_LOW_000_PANE_BACKBUTTON,
  };

  static const app::ui::ButtonInfoEx def_info[] =
  {
    {
      0, NULL, NULL,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_TOUCH,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_CANSEL,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_ACTIVE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_PASSIVE,
    },
    {
      1, NULL, NULL,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_TOUCH,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_CANSEL,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_ACTIVE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_PASSIVE,
    },
    {
      2, NULL, NULL,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_TOUCH,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_CANSEL,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_ACTIVE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_PASSIVE,
    },
    {
      3, NULL, NULL,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_TOUCH,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_CANSEL,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_ACTIVE,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_PASSIVE,
    },

    { // 戻るボタン
      pml::MAX_WAZA_NUM, NULL, NULL,
      LA_COMMONBAG_BAG_PPX_LOW_000__BACKBUTTON_TOUCH,
      LA_COMMONBAG_BAG_PPX_LOW_000__BACKBUTTON_RELEASE,
      LA_COMMONBAG_BAG_PPX_LOW_000__BACKBUTTON_CANSEL,
      LA_COMMONBAG_BAG_PPX_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[GFL_NELEMS(parts_tbl)];

  for( u32 i=0; i<GFL_NELEMS(parts_tbl); i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = def_info[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, PANENAME_BAG_BTN_LOW_001_PANE_BOUND_00, &res_id );
  }
  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, GFL_NELEMS(parts_tbl) );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  // 技
  for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ )
  {
    man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }
  // 戻るボタン
  man->SetButtonBindKey( pml::MAX_WAZA_NUM, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonSelectSE( pml::MAX_WAZA_NUM, SEQ_SE_CANCEL1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新処理
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::Update(void)
{
  app::ui::UIView::Update();
  m_pMenuCursor->Update();

  switch( m_seq )
  {
  case SEQ_NONE:
    break;

  case SEQ_IN_WAIT:
//    if( GetG2DUtil()->IsAnimeEnd( 0, LA_COMMONBAG_BAG_PPX_LOW_000_IN ) != false )
    {
      m_seq = SEQ_MAIN;
    }
    break;

  case SEQ_MAIN:
    break;

  case SEQ_OUT_WAIT:
//    if( GetG2DUtil()->IsAnimeEnd( 0, LA_COMMONBAG_BAG_PPX_LOW_000_OUT ) != false )
    {
      SetInputEnabled( false );
      SetInputListener( NULL );
      m_seq = SEQ_LAYOUT_OFF;
    }
    break;

  // @fix niji NMCat[236]: 非表示のタイミングを１フレ遅らせる
  case SEQ_LAYOUT_OFF:
    // @fix GFNMCat[5268]: バトルで技が選択された場合は表示したままにする
    if( !( m_appParam->call_mode == CALL_BATTLE_MENU && m_result < pml::MAX_WAZA_NUM ) )
    {
      GetG2DUtil()->SetLayoutDrawEnable( 0, false );
    }
    m_seq = SEQ_NONE;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画処理
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, 0 );
    m_pMenuCursor->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   処理開始
 * @date    2015.04.21
 *
 * @param   pp        ポケモンパラメータ
 * @param   message   表示するメッセージ
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::Start( const pml::pokepara::PokemonParam * pp, u32 message )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ )
  {
    WazaNo waza = pp->GetWazaNo( i );
    if( waza == WAZANO_NULL )
    {
      // 技名
      g2d->SetTextBoxPaneString( GetTextBoxPane(i,PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_00), msg_bag_01_19 );
      // PP
      GetTextBoxPane(i,PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_01)->SetVisible( false );
      // タイプ
      GetPane(i,PANENAME_BAG_BTN_LOW_003_PANE_PARTS_TYPE)->SetVisible( false );

      man->SetButtonPassive( i );
    }
    else
    {
      // 技名
      g2d->SetRegisterWazaName( 0, waza );
      g2d->GetStringExpand( str, msg_bag_01_02 );
      g2d->SetTextBoxPaneString( GetTextBoxPane(i,PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_00), str );
      // @fix GFMMCat[29]: 文字色を変更するようにした
      {
        u32 waza_pp     = pp->GetWazaPP(i);
        u32 waza_pp_max = pp->GetWazaMaxPP(i);
        gfl2::lyt::ColorU8 color = app::util::GetWazaTextColor( waza_pp, waza_pp_max );
        // PP
        g2d->SetRegisterNumber( 0, waza_pp, 2 );
        g2d->GetStringExpand( str, msg_bag_01_18 );
        {
          gfl2::lyt::LytTextBox * pane = GetTextBoxPane( i, PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_02 );
          g2d->SetTextBoxPaneString( pane, str );
          g2d->SetTextBoxPaneColor( pane, color, color );
        }
        // Max PP
        g2d->SetRegisterNumber( 0, waza_pp_max, 2 );
        g2d->GetStringExpand( str, msg_bag_01_18 );
        {
          gfl2::lyt::LytTextBox * pane = GetTextBoxPane( i, PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_03 );
          g2d->SetTextBoxPaneString( pane, str );
          g2d->SetTextBoxPaneColor( pane, color, color );
        }
        //「PP」
        g2d->SetTextBoxPaneColor( GetTextBoxPane(i,PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_01), color, color );
        //「/」
        g2d->SetTextBoxPaneColor( GetTextBoxPane(i,PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_04), color, color );
      }
      GetTextBoxPane(i,PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_01)->SetVisible( true );
      // タイプ
      {
        gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
        gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
        gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( GetParts(i), PANENAME_BAG_BTN_LOW_003_PANE_PARTS_TYPE, &res_id );

        m_commonIcon->ReplacePaneTextureByWazaTypeIcon(
          static_cast<PokeType_tag>(pml::wazadata::GetType(waza)),
          lytwk->GetPicturePane( parts, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &res_id ) );
      }
      GetPane(i,PANENAME_BAG_BTN_LOW_003_PANE_PARTS_TYPE)->SetVisible( true );

      man->SetButtonActive( i );
    }
    // 項目カラー
    SetItemColosr( i, waza );
  }

  { // メッセージウィンドウ
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
    gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
    gfl2::lyt::LytTextBox * pane = lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_BAG_PPX_LOW_000_PANE_MESSAGE), PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, &res_id );
    g2d->SetTextBoxPaneString( pane, message );
    pane->SetVisible( true );
  }

  // カーソル
  m_pMenuCursor->SetVisible( true );
  PutCursor( 0 );

  m_result = RESULT_NONE;

//  g2d->StartAnime( 0, LA_COMMONBAG_BAG_PPX_LOW_000_IN );

  g2d->SetLayoutDrawEnable( 0, true );

  SetInputEnabled( true );
  SetInputListener( this );

  m_seq = SEQ_IN_WAIT;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetItemColosr
 * @brief   項目の色を変更
 * @date    2015.04.21
 *
 * @param   id    項目ID
 * @param   waza  技番号
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::SetItemColosr( u32 id, WazaNo waza )
{
  static const u32 tbl[][POKETYPE_MAX] =
  {
    {
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_00,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_01,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_02,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_03,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_04,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_05,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_06,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_07,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_08,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_09,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_10,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_11,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_12,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_13,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_14,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_15,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_16,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_00_COLOR_17,
    },
    {
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_00,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_01,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_02,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_03,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_04,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_05,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_06,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_07,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_08,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_09,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_10,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_11,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_12,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_13,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_14,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_15,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_16,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_01_COLOR_17,
    },
    {
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_00,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_01,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_02,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_03,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_04,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_05,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_06,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_07,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_08,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_09,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_10,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_11,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_12,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_13,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_14,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_15,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_16,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_02_COLOR_17,
    },
    {
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_00,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_01,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_02,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_03,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_04,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_05,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_06,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_07,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_08,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_09,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_10,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_11,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_12,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_13,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_14,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_15,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_16,
      LA_COMMONBAG_BAG_PPX_LOW_000__SKILLBUTTON_03_COLOR_17,
    },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  if( waza == WAZANO_NULL )
  {
    g2d->StartAnime( 0, tbl[id][0] );
  }
  else
  {
    g2d->StartAnime( 0, tbl[id][pml::wazadata::GetType(waza)] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEnd
 * @brief   終了チェック
 * @date    2015.04.21
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagWazaSelect::IsEnd(void)
{
  if( m_seq != SEQ_NONE && m_seq != SEQ_OUT_WAIT && m_seq != SEQ_LAYOUT_OFF )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetResult
 * @brief   終了チェック
 * @date    2015.04.21
 *
 * @return  選択結果
 */
//-----------------------------------------------------------------------------
u32 BagWazaSelect::GetResult(void)
{
  return m_result;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursor
 * @brief   メニューカーソル生成
 * @date    2015.04.21
 *
 * @param   buf   メニューカーソルのリソースバッファ
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::CreateMenuCursor( void ** buf )
{
  m_pMenuCursor = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuCursor( m_heap, buf );

  app::util::G2DUtil * g2d = GetG2DUtil();

  m_pMenuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup(gfl2::lyt::DISPLAY_LOWER),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  m_pMenuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursor
 * @brief   メニューカーソル削除
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::DeleteMenuCursor(void)
{
  m_pMenuCursor->DeleteLayoutWork();
  GFL_SAFE_DELETE( m_pMenuCursor );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCursor
 * @brief   カーソル配置
 * @date    2015.04.21
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::PutCursor( u32 pos )
{
  m_cursorPos = pos;

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = GetParts( pos );
  gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_BAG_BTN_LOW_003_PANE_NULL_CUR, &res_id );
  gfl2::math::VEC3 trans = parts->GetTranslate() + pane->GetTranslate();
  nw::lyt::Size	size = pane->GetSize();
  m_pMenuCursor->SetPos( &trans );
  m_pMenuCursor->SetSize( pane );
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneTouchTriggerEvent
 * @brief   ペインタッチ時を検知
 * @date    2015.04.21
 *
 * @param   button_id   選択されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void BagWazaSelect::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
{
  if( m_seq == SEQ_MAIN )
  {
    if( button_id != pml::MAX_WAZA_NUM )
    {
      PutCursor( button_id );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.04.21
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagWazaSelect::OnLayoutPaneEvent( const u32 button_id )
{
  if( m_seq == SEQ_MAIN )
  {
    m_result = button_id;
//    GetG2DUtil()->StartAnime( 0, LA_COMMONBAG_BAG_PPX_LOW_000_OUT );
    m_pMenuCursor->SetVisible( false );
    m_seq = SEQ_OUT_WAIT;
  }
  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.04.21
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagWazaSelect::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_seq != SEQ_MAIN )
  {
    return ENABLE_ACCESS;
  }

  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();

  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    man->StartSelectedAct( m_cursorPos );
    return DISABLE_ACCESS;
  }

  // ボタンマネージャが動作中かどうか
  if( man->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE )
  {
    return DISABLE_ACCESS;
  }

  if( key->IsRepeat(gfl2::ui::DIR_UP) )
  {
    s16 pos = m_cursorPos - 1;
    if( pos < 0 )
    {
      pos = pml::MAX_WAZA_NUM - 1;
    }
    PutCursor( pos );
    Sound::PlaySE( SEQ_SE_SELECT1 );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
  {
    s32 pos = m_cursorPos + 1;
    if( pos >= pml::MAX_WAZA_NUM )
    {
      pos = 0;
    }
    PutCursor( pos );
    Sound::PlaySE( SEQ_SE_SELECT1 );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)
