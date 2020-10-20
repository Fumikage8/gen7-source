//=============================================================================
/**
 * @file    WazaOmoidasiDraw.cpp
 * @brief   技思い出し画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.12.08
 */
//=============================================================================

// module
#include "WazaOmoidasiDraw.h"
// niji
#include "GameSys/include/GameData.h"
#include "PokeTool/include/PokeToolRemaindSkill.h"
#include "AppLib/include/Util/app_util_Common.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "App/RaderChart/include/app_tool_StatusRaderChart.h"
#include "Sound/include/Sound.h"
// resource
#include "niji_conv_header/app/wazaomoidashi/WazaOmoidasi.h"
#include "niji_conv_header/app/wazaomoidashi/WazaOmoidasi_pane.h"
#include "niji_conv_header/app/wazaomoidashi/WazaOmoidasi_anim_list.h"
#include "niji_conv_header/message/msg_waza_omoidashi.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(WazaOmoidasi)

const f32 WazaOmoidasiDraw::LIST_ITEM_PX = 2.0f;    //!< リスト項目X座標
const f32 WazaOmoidasiDraw::LIST_ITEM_PY = 15.0f;   //!< リスト項目Y座標
const f32 WazaOmoidasiDraw::LIST_ITEM_SX = 310.0f;  //!< リスト項目Xサイズ
const f32 WazaOmoidasiDraw::LIST_ITEM_SY = 30.0f;   //!< リスト項目Xサイズ

//! 技リストペインテーブル
const gfl2::lyt::LytPaneIndex WazaOmoidasiDraw::ListPaneTable[WazaOmoidasiDraw::LIST_PANE_MAX] =
{
  PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_WAZABUTTON_00,
  PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_WAZABUTTON_01,
  PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_WAZABUTTON_02,
  PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_WAZABUTTON_03,
  PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_WAZABUTTON_04,
  PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_WAZABUTTON_05,
  PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_WAZABUTTON_06,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
   * @param   pp          対象ポケモン
   */
  //-----------------------------------------------------------------------------
WazaOmoidasiDraw::WazaOmoidasiDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, pml::pokepara::PokemonParam * pp )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_pp( pp )
  , m_pMsgData( NULL )
  , m_pTokuseiMsgData( NULL )
  , m_pWazaMsgData( NULL )
  , m_pWordSet( NULL )
  , m_msgFinishType( gfl2::str::StrWin::FINISH_NONE )
  , m_msgCallBackWrok( 0 )
  , m_pPokeIcon( NULL )
  , m_isLoadPokeIcon( false )
  , m_pCommonIcon( NULL )
  , m_menuCursor( NULL )
  , m_pPaneListData( NULL )
  , m_pRemaindSkill( NULL )
  , m_pRaderChart( NULL )
  , m_isRaderChartUnset( false )
  , m_listener( NULL )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
  , m_statusMode( 0 )
{
  SetInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
WazaOmoidasiDraw::~WazaOmoidasiDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.10.13
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::Update(void)
{
  app::ui::UIView::Update();
  UpdatePokeIcon();
  if( m_menuCursor != NULL )
  {
    m_menuCursor->Update();
  }
  if( m_pRaderChart != NULL )
  {
    m_pRaderChart->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.10.13
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_UPPER );
    GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_LOWER );
    if( m_menuCursor != NULL )
    {
      m_menuCursor->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListener
 * @brief   リスナーをセット
 * @date    2015.12.10
 *
 * @param   listener  リスナー
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetListener( InputListener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2015.10.13
 *
 * @param   res_buf           レイアウトリソースバッファ
 * @param   msg_buf           メッセージリソースバッファ
 * @param   msg_tokusei_buf   特性説明メッセージリソースバッファ
 * @param   msg_waza_buf      技説明メッセージリソースバッファ
 * @param   wset_loader       WordSetLoader
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::Init( void * res_buf, void * msg_buf, void * msg_tokusei_buf, void * msg_waza_buf, WordSetLoader * wset_loader )
{
  switch( m_mainSeq )
  {
  case 0:
    if( CreateWazaList() == false )
    {
      break;
    }
    Initialize2D( res_buf, msg_buf, msg_tokusei_buf, msg_waza_buf, wset_loader );
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateCommonIcon() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( CreatePokeIcon() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 3:
    if( CreateRaderChart() == false )
    {
      break;
    }
    PutPokeInfo();
    CreateButton();
    SetInputListener( this );
    m_mainSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2015.10.13
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }

  if( DeleteRaderChart() == false )
  {
    return false;
  }
  if( DeletePokeIcon() == false )
  {
    return false;
  }
  DeleteCommonIcon();
  Terminate2D();
  DeleteWazaList();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2015.10.13
 *
 * @param   res_buf           レイアウトリソースバッファ
 * @param   msg_buf           メッセージリソースバッファ
 * @param   msg_tokusei_buf   特性説明メッセージリソースバッファ
 * @param   msg_waza_buf      技説明メッセージリソースバッファ
 * @param   wset_loader       WordSetLoader
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::Initialize2D( void * res_buf, void * msg_buf, void * msg_tokusei_buf, void * msg_waza_buf, WordSetLoader * wset_loader )
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
      LYTRES_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000_BFLYT,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_BFLYT,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_buf, m_heap->GetDeviceHeap() );
  m_pTokuseiMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_tokusei_buf, m_heap->GetDeviceHeap() );
  m_pWazaMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_waza_buf, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  SetWordSetLoader( wset_loader );

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

  CreateMessageWindow();

  WriteDefaultString();

  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    g2d->StartAnime( LYTID_UPPER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__BG_01_BG_LOOP_00 );
    g2d->StartAnime( LYTID_LOWER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__BG_00_BG_LOOP_00 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2015.10.13
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::Terminate2D(void)
{
  DeleteMessageWindow();
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pWazaMsgData );
  GFL_SAFE_DELETE( m_pTokuseiMsgData );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursorLayout
 * @brief   メニューカーソル用レイアウト作成
 * @date    2015.12.12
 *
 * @param   cursor  メニューカーソル
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::CreateMenuCursorLayout( app::tool::MenuCursor * cursor )
{
  m_menuCursor = cursor;

  app::util::G2DUtil * g2d = GetG2DUtil();

  m_menuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  m_menuCursor->SetVisible( true );
  m_menuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursorLayout
 * @brief   メニューカーソル用レイアウト削除
 * @date    2015.12.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::DeleteMenuCursorLayout(void)
{
  if( m_menuCursor != NULL )
  {
    if( m_menuCursor->DeleteLayoutWork() == false )
    {
      return false;
    }
    m_menuCursor = NULL;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始処理
 * @date    2015.10.13
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::Start(void)
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsStart
 * @brief   開始処理が終了したかをチェック
 * @date    2015.10.14
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::IsStart(void)
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteDefaultSrring
 * @brief   デフォルト文字列描画
 * @date    2015.10.22
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::WriteDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  // いりょく
  g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_TEXTBOX_01, msg_waza_omoidashi_11_01 );
  // めいちゅう
  g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_TEXTBOX_03, msg_waza_omoidashi_12_01 );

  // 技リスト関連
  for( u32 i=0; i<LIST_PANE_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ListPaneTable[i] );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_TEXTBOX_01,&res_id), msg_waza_omoidashi_03_02 );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_TEXTBOX_04,&res_id), msg_waza_omoidashi_03_04 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::CreateButton(void)
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_BACKBUTTON,
  };
  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
  };
  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    {
      BTNID_RETURN, NULL, NULL, NULL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__BACKBUTTON_TOUCH,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__BACKBUTTON_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__BACKBUTTON_CANSEL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__BACKBUTTON_ACTIVE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoLytWk * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoLytWk[BTNID_MAX];

  for( u32 i=0; i<BTNID_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound_tbl[i], &res_id );
    info[i].lyt_wk       = lytwk;
  }

  app::ui::UIResponder::CreateButtonManager( m_heap, info, BTNID_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  man->SetButtonBindKey( BTNID_RETURN, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonSelectSE( BTNID_RETURN, SEQ_SE_CANCEL1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPokeInfo
 * @brief   ポケモン情報表示
 * @date    2015.12.11
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::PutPokeInfo(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_UPPER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  // ポケモンアイコン
  m_pPokeIcon->ReadRequest( 0, m_pp );

  { // 性別
    pml::Sex sex = PokeTool::GetDrawSex( m_pp );
    if( sex == pml::SEX_MALE )
    {
      g2d->StartAnime( LYTID_UPPER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__GENDERICON_ICON_MALE );
    }
    else if( sex == pml::SEX_FEMALE )
    {
      g2d->StartAnime( LYTID_UPPER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__GENDERICON_ICON_FEMALE );
    }
    else
    {
      g2d->SetPaneVisible( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_GENDERICON, false );
    }
  }

  { // タイプ
    pml::PokeType type1 = m_pp->GetType1();
    pml::PokeType type2 = m_pp->GetType2();

    m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon(
      static_cast<PokeType_tag>(type1),
      lytwk->GetPicturePane(
        lytwk->GetPartsPane(PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TYPEICON_00),
        PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE,
        &res_id )
      );

    if( type1 != type2 )
    {
      m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon(
        static_cast<PokeType_tag>(type2),
        lytwk->GetPicturePane(
          lytwk->GetPartsPane(PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TYPEICON_01),
          PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE,
          &res_id )
        );
    }
    else
    {
      g2d->SetPaneVisible( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TYPEICON_01, false );
    }
  }

  // ニックネーム
  g2d->SetRegisterPokeNickName( 0, m_pp );
  g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXT_PNAME, static_cast<u32>(msg_waza_omoidashi_01_01) );

  // レベル
  g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXT_LV, msg_waza_omoidashi_01_02 );
  g2d->SetRegisterNumber( 0, m_pp->GetLevel(), 3 );
  g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXT_LV100, static_cast<u32>(msg_waza_omoidashi_01_03) );

  { // レーダーチャート
    gfl2::math::VEC3 p3 = g2d->GetPanePos( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PIVOT_CENTER );
    gfl2::math::Vector2 p2( p3.x, p3.y );
    m_pRaderChart->SetPos( p2 );
    { // 半径
      gfl2::math::VEC3 tp = g2d->GetPanePos( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PIVOT_00 );
      m_pRaderChart->SetStatusRadius( tp.y - p3.y + 1.0f );  // @fix NMCat[2019] ステータス画面の「種族値+努力値」のグラフが、隣接する能力によって見え方が異なってしまう  →横に-0.5ずらすというのは行わないが、半径は1.0増やす。
    }
    PutRaderChart();
  }

  { // パラメータ
    struct PUT_DATA
    {
      pml::pokepara::PowerID power;     // パワーID
      gfl2::lyt::LytPaneIndex pane;     // 数値を書き込むペイン
      u32 anm;                          // 色変更アニメ
      gfl2::lyt::LytPaneIndex gra_font; // グラフィックフォントのペイン
    };

    static const PUT_DATA data_tbl[] =
    {
      { // HP
        pml::pokepara::POWER_HP,
        0,
        LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000_PARAM_NAME_00,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PARAMNAME_00,
      },
      { // こうげき
        pml::pokepara::POWER_ATK,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXTVALUE_03,
        LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000_PARAM_NAME_01,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PARAMNAME_01,
      },
      { // ぼうぎょ
        pml::pokepara::POWER_DEF,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXTVALUE_04,
        LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000_PARAM_NAME_02,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PARAMNAME_02,
      },
      { // とくこう
        pml::pokepara::POWER_SPATK,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXTVALUE_05,
        LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000_PARAM_NAME_03,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PARAMNAME_03,
      },
      { // とくぼう
        pml::pokepara::POWER_SPDEF,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXTVALUE_06,
        LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000_PARAM_NAME_04,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PARAMNAME_04,
      },
      { // すばやさ
        pml::pokepara::POWER_AGI,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXTVALUE_07,
        LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000_PARAM_NAME_05,
        PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PARAMNAME_05,
      },
    };

    pml::pokepara::PowerID good;
    pml::pokepara::PowerID weak;
    app::util::OutputSpecialPointPokemSeikaku( m_pp->GetSeikaku(), &good, &weak );

    for( u32 i=0; i<GFL_NELEMS(data_tbl); i++ )
    {
      if( data_tbl[i].power == pml::pokepara::POWER_HP )
      {
        g2d->SetRegisterNumber( 0, m_pp->GetHp(), 3 );
        g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXTVALUE_01, static_cast<u32>(msg_waza_omoidashi_02_02) );
        g2d->SetRegisterNumber( 0, m_pp->GetMaxHp(), 3 );
        g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXTVALUE_02, static_cast<u32>(msg_waza_omoidashi_02_02) );
      }
      else
      {
        g2d->SetRegisterNumber( 0, m_pp->GetPower(data_tbl[i].power), 3 );
        g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, data_tbl[i].pane, static_cast<u32>(msg_waza_omoidashi_02_02) );
      }

      if( good == data_tbl[i].power )
      {
        g2d->StartAnimeFrame( LYTID_UPPER, data_tbl[i].anm, 1, false );
      }
      else if( weak == data_tbl[i].power )
      {
        g2d->StartAnimeFrame( LYTID_UPPER, data_tbl[i].anm, 2, false );
      }
      else
      {
        g2d->StartAnimeFrame( LYTID_UPPER, data_tbl[i].anm, 0, false );
      }

      { // グラフィックフォント差し替え
        gfl2::lyt::LytPicture * picture = g2d->GetLayoutWork(LYTID_UPPER)->GetPicturePane( data_tbl[i].gra_font );
        m_pCommonIcon->ReplacePaneTextureByPokeParamIcon( data_tbl[i].power, picture );
      }
    }
    // HPの"/"
    g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXTVALUE_00, msg_waza_omoidashi_03_04 );
  }

  // 技
  for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ )
  {
    PutPokeWaza( i );
  }

  { // 特性
    TokuseiNo tokusei = m_pp->GetTokuseiNo();
    g2d->SetRegisterTokuseiName( 0, tokusei );
    g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXT_TOKUSEINAME, msg_waza_omoidashi_10 );

    gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
    m_pTokuseiMsgData->GetString( tokusei, *str );
    g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_TEXT_TOKUSEIDESC, str );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutRaderChart
 * @brief   レーダーチャート表示
 * @date    2016.03.17
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::PutRaderChart(void)
{
  // パラメータ
  struct PUT_DATA
  {
    pml::pokepara::PowerID power;     // パワーID
    pml::personal::ParamID param;     // パラメータID
    gfl2::lyt::LytPaneIndex eff_pane; // エフェクト用ペイン
    u16 eff_anm;                      // エフェクト用アニメ
    u16 rader;                        // レーダーチャートの位置
  };
  static const PUT_DATA data_tbl[] =
  {
    { // HP
      pml::pokepara::POWER_HP,
      pml::personal::PARAM_ID_basic_hp,
      PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_STAR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__STAR_00_STAR_00,
      0,
    },
    { // こうげき
      pml::pokepara::POWER_ATK,
      pml::personal::PARAM_ID_basic_atk,
      PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_STAR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__STAR_01_STAR_00,
      1,
    },
    { // ぼうぎょ
      pml::pokepara::POWER_DEF,
      pml::personal::PARAM_ID_basic_def,
      PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_STAR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__STAR_02_STAR_00,
      2,
    },
    { // とくこう
      pml::pokepara::POWER_SPATK,
      pml::personal::PARAM_ID_basic_spatk,
      PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_STAR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__STAR_03_STAR_00,
      5,
    },
    { // とくぼう
      pml::pokepara::POWER_SPDEF,
      pml::personal::PARAM_ID_basic_spdef,
      PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_STAR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__STAR_04_STAR_00,
      4,
    },
    { // すばやさ
      pml::pokepara::POWER_AGI,
      pml::personal::PARAM_ID_basic_agi,
      PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_STAR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__STAR_05_STAR_00,
      3,
    },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  pml::personal::LoadPersonalData( m_pp->GetMonsNo(), m_pp->GetFormNo() );

  // 能力値
  if( m_statusMode == 0 )
  {
    m_pRaderChart->SetStatusMode( app::tool::StatusRaderChart::STATUS_MODE_AMA );

    for( u32 i=0; i<GFL_NELEMS(data_tbl); i++ )
    {
      u32 val = 0;
      if( data_tbl[i].power == pml::pokepara::POWER_HP )
      {
        val = m_pp->GetMaxHp();
      }
      else
      {
        val = m_pp->GetPower( data_tbl[i].power );
      }
      m_pRaderChart->SetStatusVal( app::tool::StatusRaderChart::STATUS_FIGURE_AMA, data_tbl[i].rader, val );

      // エフェクト非表示
      g2d->StopAnime( LYTID_UPPER, data_tbl[i].eff_anm );
      g2d->SetPaneVisible( LYTID_UPPER, data_tbl[i].eff_pane, false );
    }
    // 色
    m_pRaderChart->SetStatusColor(
      app::tool::StatusRaderChart::RADAR_CHART_INSIDE_COLOR,
      app::tool::StatusRaderChart::RADAR_CHART_OUTSIDE_COLOR,
      app::tool::StatusRaderChart::RADAR_CHART_NORMAL_PARAM_COLOR );

    g2d->SetPaneVisible( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PARAMNAME_NUM, true );
  }
  // 種族値/努力値
  else
  {
    m_pRaderChart->SetStatusMode( app::tool::StatusRaderChart::STATUS_MODE_PRO );

    u32 total_effort = 0;

    for( u32 i=0; i<GFL_NELEMS(data_tbl); i++ )
    {
      // 種族値
      m_pRaderChart->SetStatusVal( app::tool::StatusRaderChart::STATUS_FIGURE_PRO_INSIDE,  data_tbl[i].rader, pml::personal::GetPersonalParam(data_tbl[i].param) );
      // 努力値
      u32 effort = m_pp->GetEffortPower( data_tbl[i].power );
      m_pRaderChart->SetStatusVal( app::tool::StatusRaderChart::STATUS_FIGURE_PRO_OUTSIDE, data_tbl[i].rader, effort );
      if( effort == pml::MAX_EFFORT_POWER )
      {
        g2d->StartAnime( LYTID_UPPER, data_tbl[i].eff_anm );
        g2d->SetPaneVisible( LYTID_UPPER, data_tbl[i].eff_pane, true );
      }
      else
      {
        g2d->StopAnime( LYTID_UPPER, data_tbl[i].eff_anm );
        g2d->SetPaneVisible( LYTID_UPPER, data_tbl[i].eff_pane, false );
      }
      total_effort += effort;
    }
    // 色
    if( total_effort >= pml::MAX_TOTAL_EFFORT_POWER )
    {
      m_pRaderChart->SetStatusColor(
        app::tool::StatusRaderChart::RADAR_CHART_INSIDE_COLOR,
        app::tool::StatusRaderChart::RADAR_CHART_OUTSIDE_COLOR_DORYOKU_MAX,
        app::tool::StatusRaderChart::RADAR_CHART_NORMAL_PARAM_COLOR );
    }
    else
    {
      m_pRaderChart->SetStatusColor(
        app::tool::StatusRaderChart::RADAR_CHART_INSIDE_COLOR,
        app::tool::StatusRaderChart::RADAR_CHART_OUTSIDE_COLOR,
        app::tool::StatusRaderChart::RADAR_CHART_NORMAL_PARAM_COLOR );
    }

    g2d->SetPaneVisible( LYTID_UPPER, PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_PARAMNAME_NUM, false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetUpperWazaColor
 * @brief   上画面の技の背景色を変更
 * @date    2015.12.11
 *
 * @param   id    項目ID
 * @param   waza  技番号
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetUpperWazaColor( u32 id, WazaNo waza )
{
  static const u32 tbl[][POKETYPE_MAX] =
  {
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_00_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_01_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_02_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_UPP_000__WAZAPLATE_03_COLOR_17,
    },
  };

  GetG2DUtil()->StartAnime( LYTID_UPPER, tbl[id][pml::wazadata::GetType(waza)] );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetLowerWazaColor
 * @brief   下画面の技の背景色を変更
 * @date    2015.12.12
 *
 * @param   id    項目ID
 * @param   waza  技番号
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetLowerWazaColor( u32 id, WazaNo waza )
{
  static const u32 tbl[][POKETYPE_MAX] =
  {
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_COLOR_17,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_COLOR_17,
    },
  };

  GetG2DUtil()->StartAnime( LYTID_LOWER, tbl[id][pml::wazadata::GetType(waza)] );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPokeWaza
 * @brief   技表示
 * @date    2015.12.26
 *
 * @param   pos   位置
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::PutPokeWaza( u32 pos )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_WAZAPLATE_00,
    PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_WAZAPLATE_01,
    PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_WAZAPLATE_02,
    PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_WAZAPLATE_03,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_UPPER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[pos] );

  WazaNo waza = m_pp->GetWazaNo( pos );

  if( waza == WAZANO_NULL )
  {
    parts->SetVisible( false );
  }
  else
  {
    parts->SetVisible( true );
    // 技名
    g2d->SetRegisterWazaName( 0, waza );
    g2d->GetStringExpand( str, msg_waza_omoidashi_03_01 );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_PLT_UPP_000_PANE_TEXTBOX_00,&res_id), str );
    // PP
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_PLT_UPP_000_PANE_TEXTBOX_01,&res_id), msg_waza_omoidashi_03_02 );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_PLT_UPP_000_PANE_TEXTBOX_03,&res_id), msg_waza_omoidashi_03_04 );
    g2d->SetRegisterNumber( 0, m_pp->GetWazaPP(pos), 2 );
    g2d->GetStringExpand( str, msg_waza_omoidashi_02_01 );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_PLT_UPP_000_PANE_TEXTBOX_02,&res_id), str );
    // Max PP
    g2d->SetRegisterNumber( 0, m_pp->GetWazaMaxPP(pos), 2 );
    g2d->GetStringExpand( str, msg_waza_omoidashi_02_01 );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_PLT_UPP_000_PANE_TEXTBOX_04,&res_id), str );
    // タイプ
    m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon(
      static_cast<PokeType_tag>(pml::wazadata::GetType(waza)),
      lytwk->GetPicturePane(
        lytwk->GetPartsPane(parts,PANENAME_WAZAOMOIDASHI_PLT_UPP_000_PANE_ICON_TYPE,&res_id),
        PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE,
        &res_id )
      );

    // 項目カラー
    SetUpperWazaColor( pos, waza );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateCommonIcon
 * @brief   汎用アイコン生成
 * @date    2015.12.11
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::CreateCommonIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pCommonIcon = GFL_NEW(m_heap->GetSystemHeap()) app::tool::AppCommonGrpIconData();
    m_pCommonIcon->Initialize( m_heap );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pCommonIcon->IsReady() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteCommonIcon
 * @brief   汎用アイコン削除
 * @date    2015.12.11
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::DeleteCommonIcon(void)
{
  if( m_pCommonIcon != NULL )
  {
    m_pCommonIcon->Terminate();
    GFL_SAFE_DELETE( m_pCommonIcon );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePokeIcon
 * @brief   ポケモンアイコン生成
 * @date    2015.12.11
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::CreatePokeIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pPokeIcon = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::PokeIcon( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), pml::PokeParty::MAX_MEMBERS );
    m_pPokeIcon->AllocTextureBuffer( 0 );
    m_pPokeIcon->FileOpen( m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPokeIcon->IsFileOpen() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePokeIcon
 * @brief   ポケモンアイコン削除
 * @date    2015.12.11
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::DeletePokeIcon(void)
{
  if( m_pPokeIcon == NULL )
  {
    return true;
  }

  switch( m_subSeq )
  {
  case 0:
    if( m_pPokeIcon->IsModuleFree() == false )
    {
      break;
    }
    m_pPokeIcon->FileClose(m_heap->GetSystemHeap());
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPokeIcon->IsFileClose() == false )
    {
      break;
    }
    GFL_SAFE_DELETE( m_pPokeIcon );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdatePokeIcon
 * @brief   ポケモンアイコン更新
 * @date    2015.12.11
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::UpdatePokeIcon(void)
{
  if( m_isLoadPokeIcon != false )
  {
    return;
  }

  if( m_pPokeIcon == NULL )
  {
    return;
  }

  if( m_pPokeIcon->IsLoadFinished(0) != false )
  {
    gfl2::lyt::LytWk * lytwk = GetG2DUtil()->GetLayoutWork( LYTID_UPPER );
    m_pPokeIcon->ReplacePaneTexture( 0, m_pp, lytwk->GetPicturePane(PANENAME_WAZAOMOIDASHI_MIN_UPP_000_PANE_POKEMONICON) );
    m_isLoadPokeIcon = true;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateWazaList
 * @brief   技リスト生成
 * @date    2015.12.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::CreateWazaList(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pRemaindSkill = GFL_NEW(m_heap->GetSystemHeap()) PokeTool::RemaindSkill( m_heap->GetDeviceHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pRemaindSkill->LoadRemaindWaza(m_pp,m_heap->GetSystemHeap()) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteWazaList
 * @brief   技リスト削除
 * @date    2015.12.12
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::DeleteWazaList(void)
{
  GFL_SAFE_DELETE( m_pRemaindSkill );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetWazaNo
 * @brief   リストから技を取得
 * @date    2015.12.14
 *
 * @return  技番号
 */
//-----------------------------------------------------------------------------
WazaNo WazaOmoidasiDraw::GetWazaNo( u32 pos )
{
  return m_pRemaindSkill->GetWazaNo( pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMessageWindow
 * @brief   メッセージウィンドウ生成
 * @date    2015.12.14
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::CreateMessageWindow(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane(
                                  lytwk->GetPartsPane(PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_MENU),
                                  PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01,
                                  &res_id );

  gfl2::lyt::LytTextBox * text1 = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A, &res_id );
  gfl2::lyt::LytTextBox * text2 = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B, &res_id );

  g2d->CreateMessageWindow(
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
    0,
    text1,
    text2,
    m_heap );
  g2d->SetMessageCallback( this );

  g2d->CreateMsgCursor( m_heap, lytwk->GetPane(parts,PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00,&res_id) );
  g2d->StartAnime( LYTID_LOWER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__MENU__PARTSMSG_01__CURSOR_00_KEEP );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMessageWindow
 * @brief   メッセージウィンドウ削除
 * @date    2015.12.14
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::DeleteMessageWindow(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->DeleteMsgCursor();
  g2d->DeleteMessageWindow();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleMessage
 * @brief   メッセージ表示部分の表示切り替え
 * @date    2015.12.14
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetVisibleMessage( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane(
                                  lytwk->GetPartsPane(PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_MENU),
                                  PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01,
                                  &res_id );

  g2d->SetPaneVisible( lytwk->GetPane(parts,PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1,&res_id), flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartMessage
 * @brief   メッセージ処理開始
 * @date    2015.12.14
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::StartMessage(void)
{
  GetG2DUtil()->StartAnime( LYTID_LOWER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__MENU_IN );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndMessage
 * @brief   メッセージ処理終了
 * @date    2015.12.14
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::EndMessage(void)
{
  GetG2DUtil()->StartAnime( LYTID_LOWER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__MENU_OUT );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsMessageWindowAnimeEnd
 * @brief   メッセージウィンドウのアニメ終了チェック
 * @date    2015.12.14
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::IsMessageWindowAnimeEnd(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( g2d->IsAnimeEnd( LYTID_LOWER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__MENU_IN ) == false ||
      g2d->IsAnimeEnd( LYTID_LOWER, LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__MENU_OUT ) == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMessage
 * @brief   メッセージセット
 * @date    2015.12.14
 *
 * @param   id        メッセージID
 * @param   set_waza  思い出す技
 * @param   del_waza  忘れる技
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetMessage( MsgID id, WazaNo set_waza, WazaNo del_waza )
{
  static const u32 str_tbl[] =
  {
    msg_waza_omoidashi_50_04,
    msg_waza_omoidashi_50_01,
    msg_waza_omoidashi_50_05,
    msg_waza_omoidashi_50_06,
    msg_waza_omoidashi_50_08,
  };
  static const gfl2::str::StrWin::FinishType type_tbl[] =
  {
    gfl2::str::StrWin::FINISH_NONE,
    gfl2::str::StrWin::FINISH_NONE,
    gfl2::str::StrWin::FINISH_USER,
    gfl2::str::StrWin::FINISH_USER,
    gfl2::str::StrWin::FINISH_USER,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSGID_CANCEL:        // あきらめますか？
    g2d->SetRegisterPokeNickName( 0, m_pp );
    break;
  case MSGID_SELECT:        // おもいだしますか？
    g2d->SetRegisterWazaName( 0, set_waza );
    break;
  case MSGID_DECIDE:        // おもいだした
    g2d->SetRegisterPokeNickName( 0, m_pp );
    g2d->SetRegisterWazaName( 1, set_waza );
    break;
  case MSGID_CHANGE_START:  // わすれるわざを　えらんでください
    g2d->SetRegisterWazaName( 0, set_waza );
    break;
  case MSDID_CHANGE_DECIDE: // わすれて　おもいだした
    g2d->SetRegisterPokeNickName( 0, m_pp );
    g2d->SetRegisterWazaName( 1, set_waza );
    g2d->SetRegisterWazaName( 2, del_waza );
    break;
  }

  g2d->GetStringExpand( str, str_tbl[id] );
  g2d->SetMessage( str );
  g2d->SetMessageFinishMode( type_tbl[id], 0 );
  m_msgFinishType = type_tbl[id];
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintMessage
 * @brief   メッセージプリント
 * @date    2015.12.14
 *
 * @retval  true  = 処理中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::PrintMessage(void)
{
  gfl2::str::StrWin::Result result = GetG2DUtil()->PrintMessage();
  if( m_msgFinishType == gfl2::str::StrWin::FINISH_NONE && result == gfl2::str::StrWin::RES_DONE )
  {
    return false;
  }
  if( m_msgFinishType == gfl2::str::StrWin::FINISH_USER && result == gfl2::str::StrWin::RES_FINISH )
  {
    Sound::PlaySE( SEQ_SE_MESSAGE );
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetString
 * @brief   文字列取得
 * @date    2015.12.14
 *
 * @param   id  メッセージID
 *
 * @return  文字列
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * WazaOmoidasiDraw::GetString( u32 id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );
  g2d->GetString( str, id );
  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMsgCallback
 * @brief   メッセージ表示コールバック関数
 * @date    2015.12.26
 *
 * @param   arg   コールバックID
 *
 * @retval  true  = 継続
 * @retval  false = 終了
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::UpdateMsgCallback( u16 arg )
{
  switch( arg )
  {
  case 3:   // 技マシン忘れ（ポカン）
    SetMsgCallBackSE( SEQ_SE_WAZAWASURE );
    return false;

  case 4:   // 技を覚えた
    SetMsgCallBackME( STRM_ME_LVUP );
    return false;

  case 5:   // SEウェイト
    if( Sound::IsSEFinished( m_msgCallBackWrok ) != 0 )
    {
      if( m_msgCallBackWrok == SEQ_SE_WAZAWASURE )
      {
        m_listener->SetWazaNo();
      }
      return false;
    }
    return true;

  case 6:   // MEウェイト
    if( Sound::IsMEFinished( m_msgCallBackWrok ) != 0 )
    {
      Sound::StartBaseBGMFromME();
      return false;
    }
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMsgCallBackSE
 * @brief   メッセージ表示コールバック中のSE再生
 * @date    2015.12.26
 *
 * @param   se  SE番号
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetMsgCallBackSE( u32 se )
{
  m_msgCallBackWrok = se;
  Sound::PlaySE( se );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMsgCallBackME
 * @brief   メッセージ表示コールバック中のME再生
 * @date    2015.12.26
 *
 * @param   me  ME番号
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetMsgCallBackME( u32 me )
{
  m_msgCallBackWrok = me;
  Sound::StartME( me );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateRaderChart
 * @brief   レーダーチャート生成
 * @date    2015.12.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::CreateRaderChart(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pRaderChart = GFL_NEW(m_heap->GetSystemHeap()) app::tool::StatusRaderChart();
    m_pRaderChart->StartAsyncCreate( m_heap, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    m_pRaderChart->Update();
    if( m_pRaderChart->IsCreated() == false )
    {
      break;
    }
    m_renderingManager->SetRaderChart( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pRaderChart );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRaderChart
 * @brief   レーダーチャート削除
 * @date    2015.12.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiDraw::DeleteRaderChart(void)
{
  if( m_pRaderChart != NULL )
  {
    if( m_isRaderChartUnset == false )
    {
      m_renderingManager->UnsetRaderChart( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
      m_isRaderChartUnset = true;
      return false;
    }
    if( m_pRaderChart->CanDestroy() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pRaderChart );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutListWazaInfo
 * @brief   カーソル位置の技情報を表示
 * @date    2016.01.28
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::PutListWazaInfo( u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  WazaNo waza = GetWazaNo( pos );

  { // いりょく
    u32 power = pml::wazadata::GetPower( waza );
    if( power <= 1 )
    {
      g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_TEXTBOX_02, msg_waza_omoidashi_12_03 );
    }
    else
    {
      g2d->SetRegisterNumber( 0, power, 3 );
      g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_TEXTBOX_02, static_cast<u32>(msg_waza_omoidashi_02_02) );
    }
  }

  // めいちゅう
  if( pml::wazadata::IsAlwaysHit(waza) != false )
  {
    g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_TEXTBOX_04, msg_waza_omoidashi_12_03 );
  }
  else
  {
    g2d->SetRegisterNumber( 0, pml::wazadata::GetParam(waza,pml::wazadata::PARAM_ID_HITPER), 3 );
    g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_TEXTBOX_04, static_cast<u32>(msg_waza_omoidashi_02_02) );
  }

  { // 分類アイコン
    static const u32 bunrui[] =
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__CATEGORYICON_TEXTURE_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__CATEGORYICON_TEXTURE_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__CATEGORYICON_TEXTURE_02,
    };
    g2d->StartAnime( LYTID_LOWER, bunrui[pml::wazadata::GetDamageType(waza)] );
  }

  { // 背景カラー
    static const u32 color[] =
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_00,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_01,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_02,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_03,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_04,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_05,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_06,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_07,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_08,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_09,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_10,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_11,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_12,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_13,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_14,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_15,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_16,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000_COLOR_17,
    };
    g2d->StartAnime( LYTID_LOWER, color[pml::wazadata::GetType(waza)] );
  }

  // 説明
  m_pWazaMsgData->GetString( waza, *str );
  g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_TEXTBOX_00, str );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPaneList
 * @brief   ペインリスト初期化
 * @date    2015.12.12
 *
 * @param   view        リストビュークラス
 * @param   list_pos    カーソル位置
 * @parma   list_scroll スクロール値
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::InitPaneList( app::tool::PaneListView * view, u16 list_pos, u16 list_scroll )
{
  static const u32 anm[LIST_PANE_MAX][4] =
  {
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_TOUCH,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_CANSEL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_00_TOUCH_RELEASE,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_TOUCH,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_CANSEL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_01_TOUCH_RELEASE,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_TOUCH,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_CANSEL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_02_TOUCH_RELEASE,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_TOUCH,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_CANSEL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_03_TOUCH_RELEASE,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_TOUCH,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_CANSEL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_04_TOUCH_RELEASE,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_TOUCH,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_CANSEL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_05_TOUCH_RELEASE,
    },
    {
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_TOUCH,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_RELEASE,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_CANSEL,
      LA_WAZAOMOIDASI_WAZAOMOIDASHI_MIN_LOW_000__WAZABUTTON_06_TOUCH_RELEASE,
    },
  };

  m_pPaneListData = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::tool::PaneList::PANE_DATA[LIST_PANE_MAX];

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<LIST_PANE_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ListPaneTable[i] );
    m_pPaneListData[i].base       = parts;
    m_pPaneListData[i].text       = lytwk->GetTextBoxPane( parts, PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_TEXTBOX_00, &res_id );
    m_pPaneListData[i].bound      = lytwk->GetBoundingPane( parts, PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_BOUND_00, &res_id );
    m_pPaneListData[i].cursor_pos = lytwk->GetPane( parts, PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_NULL_CUR, &res_id );

    m_pPaneListData[i].holdAnime    = anm[i][0];
    m_pPaneListData[i].releaseAnime = anm[i][1];
    m_pPaneListData[i].cancelAnime  = anm[i][2];
    m_pPaneListData[i].selectAnime  = anm[i][3];
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  SetPaneListParam( &list_setup, list_pos, list_scroll );

  u32 scroll_bar_max;
  if( list_setup.valMax <= list_setup.listPosMax )
  {
    scroll_bar_max = 0;
  }
  else
  {
    scroll_bar_max = list_setup.valMax - list_setup.listPosMax;
  }

  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
  {
    m_heap->GetSystemHeap(),
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
    scroll_bar_max,

    lytwk,
    res_id,
    PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_SCROLL,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL,
    NULL,
    NULL
  };
  
  view->CreatePaneList( &list_setup );
  view->CreateScrollBar( &scroll_bar_setup );
  SetPaneListConfig( view->GetPaneList(), &list_setup );

  AddSubView( view );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndPaneList
 * @brief   ペインリスト終了
 * @date    2015.12.12
 *
 * @param   view    リストビュークラス
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::EndPaneList( app::tool::PaneListView * view )
{
  if( view != NULL )
  {
    view->DeleteScrollBar();
    view->DeletePaneList();
    view->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE_ARRAY( m_pPaneListData );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListParam
 * @brief   ペインリスト初期化パラメータ設定
 * @date    2015.12.12
 *
 * @param   param   パラメータ生成場所
 * @param   pos     カーソル位置
 * @param   scroll  スクロール値
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetPaneListParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, u32 scroll )
{
  gfl2::lyt::LytWk * lytwk = GetG2DUtil()->GetLayoutWork( LYTID_LOWER );

  param->heap = m_heap->GetSystemHeap();  // ヒープ
  param->pLytWk = lytwk;                  // レイアウトワーク
  param->paneList = m_pPaneListData;      // ペインデータ
  param->paneListMax = LIST_PANE_MAX;     // ペインデータ数

  param->valMax = m_pRemaindSkill->GetListMax();  // 実際に使用するリストの項目数（ペインの数ではない）
  param->listPosMax = LIST_CURSOR_MOVE_MAX;       // カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  param->listPos = pos;                           // 初期カーソル位置 ( 0 ? listPosMax-1 の範囲 )
  param->listScroll = scroll;                     // 初期スクロール位置 ( 0 ? valMax-listPosMax-1 の範囲 )

  param->defPY = LIST_ITEM_PY;  // ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
  param->defSX = LIST_ITEM_SX;  // ペインのXサイズ(レイアウトデディターで厳密に確認）
  param->defSY = LIST_ITEM_SY;  // ペインのYサイズ(レイアウトデディターで厳密に確認）

  param->writeOffsetY = -1;     // 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  param->listStartPane = lytwk->GetPane( PANENAME_WAZAOMOIDASHI_MIN_LOW_000_PANE_PROGRAMNULL_00 );  // 項目基準位置のペイン

  param->cbFunc = this;   // コールバック関数
  param->cbWork = NULL;   // コールバック関数に渡すワーク

  param->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  param->menuCursor = m_menuCursor;   // カーソル
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListConfig
 * @brief   ペインリストの設定変更
 * @date    2015.12.12
 *
 * @param   list    ペインリスト
 * @param   setup   ペインリストのセットアップパラメータ
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::SetPaneListConfig( app::tool::PaneList * list, app::tool::PaneList::SETUP_PARAM * setup )
{
  // SE
  list->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );

  { // タッチ範囲
    gfl2::math::VEC3 pos = m_pPaneListData[0].bound->GetTranslate();
    u32 pos_max = setup->listPosMax;
    if( setup->valMax < pos_max )
    {
      pos_max = setup->valMax;
    }
    f32 lx = LIST_ITEM_PX - LIST_ITEM_SX / 2.0f;
    f32 rx = lx + LIST_ITEM_SX;
    f32 uy = LIST_ITEM_PY + pos.y + LIST_ITEM_SY / 2.0f;
    f32 dy = uy - LIST_ITEM_SY * pos_max;
    list->MakeTouchArea( lx, rx, uy, dy );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_CursorMove
 * @brief   カーソル移動時のコールバック関数
 * @date    2015.12.12
 *
 * @param   work        外部から指定されたワーク
 * @param   cur_pane    ペインデータ
 * @param   pane_index  カーソル位置のペインデータインデックス
 * @param   pos         カーソル位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  PutListWazaInfo( pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_ListWrite
 * @brief		リストの項目を描画するコールバック関数
 * @date    2015.12.12
 *
 * @param		work				外部から指定されたワーク
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ListPaneTable[pane_index] );
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  WazaNo waza = GetWazaNo( pos );

  // 技名
  g2d->SetRegisterWazaName( 0, waza );
  g2d->GetStringExpand( str, msg_waza_omoidashi_03_01 );
  g2d->SetTextBoxPaneString( pane[pane_index].text, str );
  { // PP
    u32 val = pml::wazadata::GetMaxPP( waza, 0 );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_TEXTBOX_04,&res_id), msg_waza_omoidashi_03_04 );
    g2d->SetRegisterNumber( 0, val, 2 );
    g2d->GetStringExpand( str, msg_waza_omoidashi_02_01 );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_TEXTBOX_02,&res_id), str );
    g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_TEXTBOX_03,&res_id), str );
  }
  // タイプ
  m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon(
    static_cast<PokeType_tag>(pml::wazadata::GetType(waza)),
    lytwk->GetPicturePane(
      lytwk->GetPartsPane(parts,PANENAME_WAZAOMOIDASHI_BTN_LOW_000_PANE_PARTS_TYPE,&res_id),
      PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE,
      &res_id )
    );
  // 項目カラー
  SetLowerWazaColor( pane_index, waza );
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_CursorOff
 * @brief		カーソルを非表示にするコールバック関数
 * @date    2015.12.12
 *
 * @param		work				外部から指定されたワーク
 * @param		pane				ペインデータ
 * @param		pane_index	カーソル位置のペインデータインデックス
 * @param		pos					カーソル位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_ItemChange
 * @brief		入れ替え用コールバック関数
 * @date    2015.12.12
 *
 * @param		work	外部から指定されたワーク
 * @param		pos1	入れ替え位置１
 * @param		pos2	入れ替え位置２
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiDraw::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.10.14
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
app::ui::UIInputListener::ListenerResult WazaOmoidasiDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( button_id == BTNID_RETURN )
  {
    m_listener->OnAction( InputListener::ACTION_ID_RETURN );
    return DISABLE_ACCESS;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.10.13
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
app::ui::UIInputListener::ListenerResult WazaOmoidasiDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
  {
    m_statusMode ^= 1;
    PutRaderChart();
    return DISABLE_ACCESS;
  }
  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(WazaOmoidasi)
GFL_NAMESPACE_END(App)
