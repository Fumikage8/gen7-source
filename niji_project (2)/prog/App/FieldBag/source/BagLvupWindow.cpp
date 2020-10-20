//=============================================================================
/**
 * @file    BagLvupWindow.cpp
 * @brief   バッグ画面レベルアップウィンドウ
 * @author  Hiroyuki Nakamura
 * @date    2015.04.21
 */
//=============================================================================

// module
#include "App/FieldBag/include/BagLvupWindow.h"
// pmlib
#include <pml/include/pmlib.h>
// niji
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
 * @param   heap          ヒープ
 * @param   g2d           G2DUtil
 * @param   is_demo_mode  デモモードフラグ
 */
//-----------------------------------------------------------------------------
BagLvupWindow::BagLvupWindow( app::util::Heap * heap, app::util::G2DUtil * g2d, const bool * is_demo_mode )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_g2dUtil( g2d )
  , m_pp( NULL )
  , m_input( false )
  , m_seq( SEQ_NONE )
  , m_isDemoMode( is_demo_mode )
{
  Init();
  SetInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
BagLvupWindow::~BagLvupWindow()
{
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
void BagLvupWindow::SetInputEnabled( bool flg )
{
  if( *m_isDemoMode != false )
  {
    flg = false;
  }
  app::ui::UIResponder::SetInputEnabled( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   画面初期化
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagLvupWindow::Init(void)
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_BAG_MIN_LOW_000_PANE_PARTS_LVUP );

  static const u32 pane_tbl[] =
  {
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMNAME_00,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMNAME_01,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMNAME_02,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMNAME_03,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMNAME_04,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMNAME_05,
  };
  static const u32 str_tbl[] =
  {
    msg_bag_10_01,
    msg_bag_10_02,
    msg_bag_10_03,
    msg_bag_10_04,
    msg_bag_10_05,
    msg_bag_10_06,
  };

  for( u32 i=0; i<PARAM_MAX; i++ )
  {
    m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,pane_tbl[i],&res_id), str_tbl[i] );
  }

  // 初期状態で表示になっているので消しておく
  m_g2dUtil->BindAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_OUT );
  m_g2dUtil->StartAnimeFrame( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_OUT, m_g2dUtil->GetAnimeMaxFrame(0,LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_OUT) );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新処理
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagLvupWindow::Update(void)
{
  app::ui::UIView::Update();

  switch( m_seq )
  {
  case SEQ_NONE:
    break;

  case SEQ_IN_WAIT:
    if( m_g2dUtil->IsAnimeEnd( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_IN ) != false )
    {
      m_seq = SEQ_WIN1_WAIT;
    }
    break;

  case SEQ_WIN1_WAIT:
    if( m_input != false )
    {
      m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_ADD );
      Sound::PlaySE( SEQ_SE_SYS_NJ_003 );
      m_input = false;
      m_seq = SEQ_ADD_WAIT;
    }
    break;

  case SEQ_ADD_WAIT:
    if( m_g2dUtil->IsAnimeEnd( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_ADD ) != false )
    {
      PutWindow2nd();
      m_seq = SEQ_WIN2_WAIT;
    }
    break;

  case SEQ_WIN2_WAIT:
    if( m_input != false )
    {
      m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_OUT );
      Sound::PlaySE( SEQ_SE_MESSAGE );
      m_input = false;
      m_seq = SEQ_OUT_WAIT;
    }
    break;

  case SEQ_OUT_WAIT:
    if( m_g2dUtil->IsAnimeEnd( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_OUT ) != false )
    {
      m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_RESET );
      SetInputEnabled( false );
      SetInputListener( NULL );
      m_seq = SEQ_NONE;
    }
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
void BagLvupWindow::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPokemon
 * @brief   ポケモンをセット
 * @date    2015.04.21
 *
 * @param   pp  ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagLvupWindow::SetPokemon( const pml::pokepara::PokemonParam * pp )
{
  m_pp = pp;

  for( u32 i=0; i<PARAM_MAX; i++ )
  {
    m_rep[i] = m_pp->GetPower( static_cast<pml::pokepara::PowerID>(i) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   処理開始
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagLvupWindow::Start(void)
{
  PutWindow1st();
  m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_LVUP_IN );
  m_seq = SEQ_IN_WAIT;

  SetInputEnabled( true );
  SetInputListener( this );

  m_g2dUtil->SetPaneVisible( 0, PANENAME_BAG_MIN_LOW_000_PANE_PARTS_LVUP, true );  // ないと出ない
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
bool BagLvupWindow::IsEnd(void)
{
  if( m_seq != SEQ_NONE )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutWindow1st
 * @brief   最初のパラメータを表示
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagLvupWindow::PutWindow1st(void)
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );
  gfl2::str::StrBuf * str = m_g2dUtil->GetTempStrBuf( 1 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_BAG_MIN_LOW_000_PANE_PARTS_LVUP );

  static const u32 param_tbl[] =
  {
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_00,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_01,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_02,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_03,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_04,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_05,
  };
  static const u32 add_tbl[] =
  {
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMADD_00,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMADD_01,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMADD_02,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMADD_03,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMADD_04,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMADD_05,
  };

  for( u32 i=0; i<PARAM_MAX; i++ )
  {
    // パラメータ
    m_g2dUtil->SetRegisterNumber( 0, m_rep[i], 3 );
    m_g2dUtil->GetStringExpand( str, msg_bag_01_17 );
    m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,param_tbl[i],&res_id), str );
    // +値
    u32 val = m_pp->GetPower( static_cast<pml::pokepara::PowerID>(i) );
    m_g2dUtil->SetRegisterNumber( 0, val-m_rep[i], 3 );
    m_g2dUtil->GetStringExpand( str, msg_bag_01_16 );
    m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,add_tbl[i],&res_id), str );
    // パラメータを更新
    m_rep[i] = val;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutWindow2nd
 * @brief   レベルアップ後のパラメータを表示
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagLvupWindow::PutWindow2nd(void)
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );
  gfl2::str::StrBuf * str = m_g2dUtil->GetTempStrBuf( 1 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_BAG_MIN_LOW_000_PANE_PARTS_LVUP );

  static const u32 param_tbl[] =
  {
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_00,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_01,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_02,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_03,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_04,
    PANENAME_BAG_PLT_LOW_000_PANE_PARAMTOTAL_05,
  };

  for( u32 i=0; i<GFL_NELEMS(param_tbl); i++ )
  {
    m_g2dUtil->SetRegisterNumber( 0, m_rep[i], 3 );
    m_g2dUtil->GetStringExpand( str, msg_bag_01_17 );
    m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,param_tbl[i],&res_id), str );
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent
 * @brief   タッチパネルイベントの検知
 * @date    2015.04.21
 *
 * @param   touch_panel タッチパネル
 * @param   is_touch    true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagLvupWindow::OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch )
{
  m_input = false;

  if( touch_panel->IsTouchTrigger() != false )
  {
    m_input = true;
    return DISABLE_ACCESS;
  }
  return ENABLE_ACCESS;
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
app::ui::UIInputListener::ListenerResult BagLvupWindow::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  m_input = false;

  if( button->IsTrigger(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) )
  {
    m_input = true;
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)
