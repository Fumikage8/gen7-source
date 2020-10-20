#if PM_DEBUG
//=============================================================================
/**
 * @file    PokeIconViewDraw.cpp
 * @brief   ポケモンアイコン確認画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2016.04.15
 */
//=============================================================================

// module
#include "PokeIconViewDraw.h"
// niji
#include "AppLib/include/Tool/PokeIcon.h"
#include "PokeTool/include/PokeTool.h"
// resource
#include "niji_conv_header/app/debug/debug_pokeicon_view/DebugPokeIconView.h"
#include "niji_conv_header/app/debug/debug_pokeicon_view/DebugPokeIconView_pane.h"
#include "niji_conv_header/app/debug/debug_pokeicon_view/DebugPokeIconView_anim_list.h"
#include "niji_conv_header/message/debug/msg_d_nakahiro.h"


GFL_NAMESPACE_BEGIN(Debug)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 */
//-----------------------------------------------------------------------------
PokeIconViewDraw::PokeIconViewDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_sex( 0 )
  , m_formNo( 0 )
  , m_flip( 0 )
  , m_bgColor( 0 )
  , m_topMonsNo( 0 )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
{
  SetInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
PokeIconViewDraw::~PokeIconViewDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListener
 * @brief   リスナーをセット
 * @date    2016.04.15
 *
 * @param   listener  リスナー
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::SetListener( Listener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::Update(void)
{
  app::ui::UIView::Update();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2016.04.15
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    for( s32 i=0; i<LYTID_MAX; i++ )
    {
      g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, i );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2016.04.15
 *
 * @param   res_buf   レイアウトリソースバッファ
 * @param   msg_buf   メッセージリソースバッファ
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::Init( void * res_buf, void * msg_buf )
{
  Initialize2D( res_buf, msg_buf );
  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2016.04.15
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PokeIconViewDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  Terminate2D();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2016.04.15
 *
 * @param   res_buf   レイアウトリソースバッファ
 * @param   msg_buf   メッセージリソースバッファ
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::Initialize2D( void * res_buf, void * msg_buf )
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { res_buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_DEBUGPOKEICONVIEW_POKEICON_VIEW_UPPER_BFLYT,
      LA_DEBUGPOKEICONVIEW_POKEICON_VIEW_UPPER___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_DEBUGPOKEICONVIEW_POKEICON_VIEW_LOWER_BFLYT,
      LA_DEBUGPOKEICONVIEW_POKEICON_VIEW_LOWER___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_buf, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

  CreatePokeIcon();
  ChangePokemon( true );
  PutStrBgColor();
  PutStrSex();
  PutStrFlip();
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::Terminate2D(void)
{
  DeletePokeIcon();
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePokeIcon
 * @brief   ポケモンアイコン生成
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::CreatePokeIcon(void)
{
  m_pPokeIcon = GFL_NEW(m_heap->GetSystemHeap()) app::tool::PokeIcon( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), ICON_MAX, false );
  for( u32 i=0; i<ICON_MAX; i++ )
  {
    m_pPokeIcon->AllocTextureBuffer( i );
    m_pokeArcID[i] = 0xffffffff;
  }
  m_pPokeIcon->FileOpenSync( m_heap->GetSystemHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePokeIcon
 * @brief   ポケモンアイコン削除
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::DeletePokeIcon(void)
{
  m_pPokeIcon->FileCloseSync();
  GFL_SAFE_DELETE( m_pPokeIcon );
}


//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始処理
 * @date    2016.04.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PokeIconViewDraw::Start(void)
{
  SetInputEnabled( true );
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    PutStrBgColor
 * @brief   背景色表示
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::PutStrBgColor(void)
{
  GetG2DUtil()->SetTextBoxPaneString( LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_TEXTBOX_03, debug_msg_pi_view_05 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutStrForm
 * @brief   フォルム表示
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::PutStrForm(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->SetRegisterNumber( 0, m_formNo, 2, print::NUM_DISP_ZERO );
  g2d->SetRegisterNumber( 1, MAX_FORM_NUM-1, 2, print::NUM_DISP_ZERO );
  g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_TEXTBOX_01, debug_msg_pi_view_02 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutStrSex
 * @brief   性別表示
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::PutStrSex(void)
{
  static const u32 tbl[] =
  {
    debug_msg_pi_view_00,
    debug_msg_pi_view_01,
  };
  GetG2DUtil()->SetTextBoxPaneString( LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_TEXTBOX_00, tbl[m_sex] );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutStrFlip
 * @brief   反転表示
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::PutStrFlip(void)
{
  static const u32 tbl[] =
  {
    debug_msg_pi_view_03,
    debug_msg_pi_view_04,
  };
  GetG2DUtil()->SetTextBoxPaneString( LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_TEXTBOX_02, tbl[m_flip] );
}



//-----------------------------------------------------------------------------
/**
 * @func    ChangePokemon
 * @brief   ポケモン切り替え
 * @date    2016.04.15
 *
 * @param   is_str  true = 文字列描画
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::ChangePokemon( bool is_str )
{
  static const u32 tbl[][2] =
  {
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_00 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_01 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_02 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_03 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_04 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_05 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_06 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_07 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_08 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_09 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_10 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_11 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_12 },
    { LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_PARTS_13 },

    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_00 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_01 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_02 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_03 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_04 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_05 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_06 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_07 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_08 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_09 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_10 },
    { LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_PARTS_11 },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::math::VEC2 scale( 1.0f, 1.0f );
  if( m_flip != 0 )
  {
    scale.x = -1.0f;
  }

  for( u32 i=0; i<ICON_MAX; i++ )
  {
    PokeTool::SimpleParam param;
    param.monsNo  = static_cast<MonsNo>(m_topMonsNo+i);
    param.formNo  = static_cast<pml::FormNo>(m_formNo);
    param.sex     = static_cast<pml::Sex>(m_sex);
    param.isRare  = false;
    param.isEgg   = false;
    param.perRand = 0;

    if( param.monsNo > MONSNO_MAX )
    {
      g2d->SetPaneVisible( tbl[i][0], tbl[i][1], false );
      continue;
    }

    if( param.monsNo == MONSNO_TAMAGO || param.monsNo == MONSNO_DAMETAMAGO )
    {
      param.isEgg = true;
    }

    g2d->SetPaneVisible( tbl[i][0], tbl[i][1], true );

    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( tbl[i][0] );
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( tbl[i][1] );
    gfl2::lyt::LytPicture * pane = lytwk->GetPicturePane( parts, PANENAME_PI_VIEW_FRAME_PANE_ICON_00, &res_id );

    u32 tmp = app::tool::PokeIcon::GetResourceIndex( &param, m_flip );
    if( tmp != m_pokeArcID[i] )
    {
      m_pPokeIcon->ReplacePaneTexture( i, &param, pane, 0, 0, m_flip );
      m_pokeArcID[i] = tmp;

      if( is_str != false )
      {
        g2d->SetRegisterNumber( 0, m_topMonsNo+i, 3, print::NUM_DISP_ZERO );
        g2d->SetTextBoxPaneStringExpand( lytwk->GetTextBoxPane(parts,PANENAME_PI_VIEW_FRAME_PANE_TEXT_NO_00,&res_id), debug_msg_pi_view_06 );
        g2d->SetRegisterMonsNameNo( 0, param.monsNo );
        g2d->SetTextBoxPaneStringExpand( lytwk->GetTextBoxPane(parts,PANENAME_PI_VIEW_FRAME_PANE_TEXT_NAME_00,&res_id), debug_msg_pi_view_07 );
      }
    }

    // 反転
    g2d->SetScale( pane, scale );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePage
 * @brief   ページ切り替え
 * @date    2016.04.15
 *
 * @param   val   切り替え数
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::ChangePage( s32 val )
{
  s32 page_max = MONSNO_MAX / ICON_MAX;

  s32 tmp = m_topMonsNo + val * ICON_MAX;
  if( tmp > (page_max*ICON_MAX) )
  {
    tmp = page_max * ICON_MAX;
  }
  else if( tmp < 0 )
  {
    tmp = 0;
  }
  m_topMonsNo = tmp;

  ChangePokemon( true );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeSex
 * @brief   性別切り替え
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::ChangeSex(void)
{
  m_sex ^= 1;
  ChangePokemon( false );
  PutStrSex();
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeForm
 * @brief   フォルム切り替え
 * @date    2016.04.15
 *
 * @param   val   切り替え数
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::ChangeForm( s32 val )
{
  s8 tmp = m_formNo + val;
  if( tmp < 0 )
  {
    tmp = MAX_FORM_NUM - 1;
  }
  else if( tmp >= MAX_FORM_NUM )
  {
    tmp = 0;
  }
  m_formNo = tmp;
  ChangePokemon( false );
  PutStrForm();
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeFlip
 * @brief   反転切り替え
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::ChangeFlip(void)
{
  m_flip ^= 1;
  ChangePokemon( false );
  PutStrFlip();
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeColor
 * @brief   背景色切り替え
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewDraw::ChangeBgColor(void)
{
  static const u8 tbl[5][4] =
  {
    { 255, 255, 255, 255 },   // 白
    { 255,   0,   0, 255 },   // 赤
    {   0, 255,   0, 255 },   // 緑
    {   0,   0, 255, 255 },   // 青
    {   0,   0,   0, 255 },   // 黒
  };

  m_bgColor++;
  if( m_bgColor >= 5 )
  {
    m_bgColor = 0;
  }

  nw::ut::Color8 color;
  color.r = tbl[m_bgColor][0];
  color.g = tbl[m_bgColor][1];
  color.b = tbl[m_bgColor][2];
  color.a = tbl[m_bgColor][3];

  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->SetMaterialColor( LYTID_UPPER, PANENAME_POKEICON_VIEW_UPPER_PANE_BG_00, app::util::G2DUtil::COLOR_INDEX_WHITE, color );
  g2d->SetMaterialColor( LYTID_LOWER, PANENAME_POKEICON_VIEW_LOWER_PANE_BG_00, app::util::G2DUtil::COLOR_INDEX_WHITE, color );
}


//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2016.04.15
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
app::ui::UIInputListener::ListenerResult PokeIconViewDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  // 終了
  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    m_listener->OnAction( Listener::ACTION_EXIT );
    return DISABLE_ACCESS;
  }
  // 背景色
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_BG_COLOR );
    return DISABLE_ACCESS;
  }
  // 性別変更
  if( button->IsTrigger(gfl2::ui::BUTTON_X) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_SEX );
    return DISABLE_ACCESS;
  }
  // 反転
  if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_FLIP );
    return DISABLE_ACCESS;
  }
  // フォルム+
  if( button->IsTrigger(gfl2::ui::BUTTON_R) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_FORM_PLUS );
    return DISABLE_ACCESS;
  }
  // フォルム-
  if( button->IsTrigger(gfl2::ui::BUTTON_L) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_FORM_MINUS );
    return DISABLE_ACCESS;
  }

  // ポケモン切り替え
  if( key->IsRepeat(gfl2::ui::DIR_UP) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_POKE_MINUS );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_POKE_PLUS );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_PAGE_MINUS );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
  {
    m_listener->OnAction( Listener::ACTION_CHANGE_PAGE_PLUS );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(Debug)

#endif  // PM_DEBUG
