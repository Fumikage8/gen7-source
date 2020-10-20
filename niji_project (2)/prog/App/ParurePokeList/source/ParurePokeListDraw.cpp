//=============================================================================
/**
 * @file    ParurePokeListDraw.cpp
 * @brief   ポケパルレポケモン選択画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.11.12
 */
//=============================================================================

// module
#include "App/ParurePokeList/include/ParurePokeListAppParam.h"
#include "ParurePokeListDraw.h"
// gflib2
#include "model/include/gfl2_BaseCamera.h"
// niji
#include "GameSys/include/GameData.h"
#include "PokeTool/include/NakayoshiManager.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "Sound/include/Sound.h"
#include "App/AppEvent/include/PokeParure/PokeParureEvent.h"
// resource
#include "niji_conv_header/app/kawaigari_menu/KawaigariMenu.h"
#include "niji_conv_header/app/kawaigari_menu/KawaigariMenu_pane.h"
#include "niji_conv_header/app/kawaigari_menu/KawaigariMenu_anim_list.h"
#include "niji_conv_header/message/msg_pokeparure.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(PokeParure)

//! ポケモンアイコンペインテーブル
const gfl2::lyt::LytPaneIndex ParurePokeListDraw::PokeIconPaneTable[pml::PokeParty::MAX_MEMBERS] =
{
  PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PICONBUTTON_00,
  PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PICONBUTTON_01,
  PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PICONBUTTON_02,
  PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PICONBUTTON_03,
  PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PICONBUTTON_04,
  PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PICONBUTTON_05,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   app_param   外部設定パラメータ
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 */
//-----------------------------------------------------------------------------
ParurePokeListDraw::ParurePokeListDraw( const APP_PARAM * app_param, app::util::Heap * heap, app::util::AppRenderingManager * render_man )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_appParam( app_param )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_listener( NULL )
  , m_myStatus( NULL )
  , m_pokeParty( NULL )
  , m_pEffectUtil( NULL )
  , m_effectDrawEnvNodeOfCamera( NULL )
  , m_effectCamera( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pPokeIcon( NULL )
  , m_iconLoadCount( 0 )
  , m_iconLoadSeq( 0xff )
  , m_pos( pml::PokeParty::MAX_MEMBERS )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
{
  SetInputEnabled( false );
  GetGameData();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ParurePokeListDraw::~ParurePokeListDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_myStatus  = gd->GetPlayerStatus();
  m_pokeParty = gd->GetPlayerParty();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListener
 * @brief   リスナーをセット
 * @date    2015.11.12
 *
 * @param   listener リスナー
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::SetListener( Listener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::Update(void)
{
  app::ui::UIView::Update();
  LoadPokeIcon();
  m_pEffectUtil->UpdateForMode1Res1EmitterSet1Group();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.11.12
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_UPPER );
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_LOWER );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2015.11.12
 *
 * @param   res_buf   レイアウトリソースバッファ
 * @param   msg_buf   メッセージリソースバッファ
 * @param   eff_buf   パーティクルリソースバッファ
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::Init( void * res_buf, void * msg_buf, void * eff_buf )
{
  Initialize2D( res_buf, msg_buf );
  ChangePokemon( m_appParam->select_pos );
  PutSelectIcon();
  CreatePokeIcon();
  CreateEffect( eff_buf );
  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2015.11.12
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  if( DeleteEffect() == false )
  {
    return false;
  }
  if( DeletePokeIcon() == false )
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
 * @date    2015.11.12
 *
 * @param   res_buf       レイアウトリソースバッファ
 * @param   msg_buf       メッセージリソースバッファ
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::Initialize2D( void * res_buf, void * msg_buf )
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
      LYTRES_KAWAIGARIMENU_KAWAIGARIMENU_MAIN_UP_000_BFLYT,
      LA_KAWAIGARIMENU_KAWAIGARIMENU_MAIN_UP_000___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000_BFLYT,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000___NUM,
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

  CreateButton();

  WriteDefaultString();

  GetG2DUtil()->StartAnime( LYTID_UPPER, LA_KAWAIGARIMENU_KAWAIGARIMENU_MAIN_UP_000_KEEP );
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::Terminate2D(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.11.18
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::CreateButton(void)
{
  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BTNID_POKEMON_00, NULL, NULL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_00_TOUCH,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_00_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_00_CANSEL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_00_TOUCH_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_00_ACTIVE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_00_PASSIVE,
    },
    {
      BTNID_POKEMON_01, NULL, NULL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_01_TOUCH,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_01_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_01_CANSEL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_01_TOUCH_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_01_ACTIVE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_01_PASSIVE,
    },
    {
      BTNID_POKEMON_02, NULL, NULL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_02_TOUCH,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_02_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_02_CANSEL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_02_TOUCH_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_02_ACTIVE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_02_PASSIVE,
    },
    {
      BTNID_POKEMON_03, NULL, NULL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_03_TOUCH,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_03_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_03_CANSEL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_03_TOUCH_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_03_ACTIVE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_03_PASSIVE,
    },
    {
      BTNID_POKEMON_04, NULL, NULL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_04_TOUCH,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_04_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_04_CANSEL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_04_TOUCH_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_04_ACTIVE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_04_PASSIVE,
    },
    {
      BTNID_POKEMON_05, NULL, NULL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_05_TOUCH,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_05_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_05_CANSEL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_05_TOUCH_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_05_ACTIVE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_05_PASSIVE,
    },
    {
      BTNID_CHANGE, NULL, NULL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__SELECTBUTTON_TOUCH,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__SELECTBUTTON_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__SELECTBUTTON_CANSEL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__SELECTBUTTON_TOUCH_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__SELECTBUTTON_ACTIVE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__SELECTBUTTON_PASSIVE,
    },
    {
      BTNID_RETURN, NULL, NULL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__BACKBUTTON_TOUCH,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__BACKBUTTON_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__BACKBUTTON_CANSEL,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__BACKBUTTON_ACTIVE,
      LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_POKEBGBUTTON_00,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_POKEBGBUTTON_01,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_POKEBGBUTTON_02,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_POKEBGBUTTON_03,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_POKEBGBUTTON_04,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_POKEBGBUTTON_05,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_SELECTBUTTON,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_BACKBUTTON,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_KAWAIGARI_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_KAWAIGARI_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_KAWAIGARI_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_KAWAIGARI_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_KAWAIGARI_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_KAWAIGARI_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[BTNID_MAX];

  for( u32 i=0; i<BTNID_MAX; i++ )
  {
    info[i] = info_tbl[i];

    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetPane( parts, bound_tbl[i], &res_id );
  }

  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, BTNID_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
  for( u32 i=0; i<=BTNID_CHANGE; i++ )
  {
    man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }
  man->SetButtonSelectSE( BTNID_RETURN, SEQ_SE_CANCEL1 );

  { // 個別設定
    static const u32 anm[pml::PokeParty::MAX_MEMBERS][2] =
    {
      { LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_00_PATERN_NORMAL, LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_00_PATERN_STAIN },
      { LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_01_PATERN_NORMAL, LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_01_PATERN_STAIN },
      { LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_02_PATERN_NORMAL, LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_02_PATERN_STAIN },
      { LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_03_PATERN_NORMAL, LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_03_PATERN_STAIN },
      { LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_04_PATERN_NORMAL, LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_04_PATERN_STAIN },
      { LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_05_PATERN_NORMAL, LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__POKEBGBUTTON_05_PATERN_STAIN },
    };
    u32 max = m_pokeParty->GetMemberCount();
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      if( i < max )
      {
        const pml::pokepara::PokemonParam * pp = m_pokeParty->GetMemberPointer( i );
        // よごれ or 状態異常
        if( pp->CheckDirt() != false || pp->HaveSick() != false )
        {
          g2d->StartAnime( LYTID_LOWER, anm[i][1] );
        }
        else
        {
          g2d->StartAnime( LYTID_LOWER, anm[i][0] );
        }
      }
      else
      {
        man->SetButtonPassive( BTNID_POKEMON_00+i );
        g2d->SetPaneVisible( LYTID_LOWER, PokeIconPaneTable[i], false );
        g2d->StartAnime( LYTID_LOWER, anm[i][0] );
      }
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始処理
 * @date    2015.11.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListDraw::Start(void)
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsStart
 * @brief   開始処理が終了したかをチェック
 * @date    2015.11.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListDraw::IsStart(void)
{
  if( InitPokeIcon() == false )
  {
    return false;
  }
  SetInputEnabled( true );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteDefaultSrring
 * @brief   デフォルト文字列描画
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::WriteDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  // なかよし
  g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PARAM_00, msg_pkparure_03_01 );

  // まんぷく
  g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PARAM_01, msg_pkparure_03_02 );

  // なでなで
  g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PARAM_02, msg_pkparure_03_03 );

  // ポケモンをいれかえる
  g2d->SetTextBoxPaneString(
    lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_KAWAIGARI_SLC_LOW_000_PANE_SELECTBUTTON), PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &res_id ),
    msg_pkparure_04_01 );

  // いれかえるポケモンをえらぼう
  g2d->SetTextBoxPaneString(
    lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_KAWAIGARI_SLC_LOW_000_PANE_MESSAGE), PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00, &res_id ),
    msg_pkparure_05_01 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutSelectIcon
 * @brief   選択中アイコン表示
 * @date    2015.11.18
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::PutSelectIcon(void)
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_SELECTICON_00,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_SELECTICON_01,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_SELECTICON_02,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_SELECTICON_03,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_SELECTICON_04,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_SELECTICON_05,
  };
  GetG2DUtil()->SetPaneVisible( LYTID_LOWER, tbl[m_pos], true );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePokemon
 * @brief   ポケモン切り替え
 * @date    2015.11.18
 *
 * @param   pos   選択位置
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::ChangePokemon( u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( pos == m_pos )
  {
    return;
  }

  m_pos = pos;

  pml::pokepara::PokemonParam * pp = m_pokeParty->GetMemberPointer( m_pos );
  bool is_egg  = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  bool is_sick = pp->HaveSick();

  if( m_pos == m_appParam->select_pos || is_egg != false )
  {
    app::ui::UIResponder::GetButtonManager()->SetButtonInvisible( BTNID_CHANGE );
  }
  else
  {
    app::ui::UIResponder::GetButtonManager()->SetButtonVisible( BTNID_CHANGE );
  }

  { // 名前
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_POKENAME, msg_pkparure_02_01 );
  }

  PutPokeHeart( pp, is_egg );     // 仲良し度
  PutPokeHungry( pp, is_egg );    // 満腹度
  PutPokeNadenade( pp, is_egg );  // なでなで

  // お手入れマーク
  if( is_egg != false )
  {
    g2d->SetPaneVisible( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_STAINICON, false );
  }
  else
  {
    // よごれ or 状態異常
    if( pp->CheckDirt() != false || is_sick != false )
    {
      g2d->SetPaneVisible( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_STAINICON, true );
    }
    else
    {
      g2d->SetPaneVisible( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_STAINICON, false );
    }
  }

  // アニメ
  static const u32 anm_tab[] =
  {
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000_SELECT_00,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000_SELECT_01,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000_SELECT_02,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000_SELECT_03,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000_SELECT_04,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000_SELECT_05,
  };
  static const u32 anm_poke_on[] =
  {
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_00_JAMP_HIGH,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_01_JAMP_HIGH,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_02_JAMP_HIGH,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_03_JAMP_HIGH,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_04_JAMP_HIGH,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_05_JAMP_HIGH,
  };
  static const u32 anm_poke_off[] =
  {
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_00_STOP,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_01_STOP,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_02_STOP,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_03_STOP,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_04_STOP,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_05_STOP,
  };
  static const u32 anm_poke_sick[] =
  {
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_00_SICK,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_01_SICK,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_02_SICK,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_03_SICK,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_04_SICK,
    LA_KAWAIGARIMENU_KAWAIGARI_SLC_LOW_000__PICONBUTTON_05_SICK,
  };
  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    if( i == pos )
    {
      g2d->StartAnime( LYTID_LOWER, anm_tab[i] );
      // タマゴ or 瀕死
      if( is_egg != false || pp->GetHp() == 0 )
      {
        g2d->StopAnime( LYTID_LOWER, anm_poke_on[i] );
        g2d->StopAnime( LYTID_LOWER, anm_poke_sick[i] );
        g2d->StartAnime( LYTID_LOWER, anm_poke_off[i] );
      }
      // 状態異常
      else if( is_sick != false )
      {
        g2d->StopAnime( LYTID_LOWER, anm_poke_on[i] );
        g2d->StopAnime( LYTID_LOWER, anm_poke_off[i] );
        g2d->StartAnime( LYTID_LOWER, anm_poke_sick[i] );
      }
      else
      {
        g2d->StopAnime( LYTID_LOWER, anm_poke_off[i] );
        g2d->StopAnime( LYTID_LOWER, anm_poke_sick[i] );
        g2d->StartAnime( LYTID_LOWER, anm_poke_on[i] );
      }
    }
    else
    {
      g2d->StopAnime( LYTID_LOWER, anm_tab[i] );
      g2d->StopAnime( LYTID_LOWER, anm_poke_on[i] );
      g2d->StopAnime( LYTID_LOWER, anm_poke_sick[i] );
      g2d->StartAnime( LYTID_LOWER, anm_poke_off[i] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPutParamMarkMax
 * @brief   マーク表示数取得
 * @date    2015.11.18
 *
 * @param   prm   表示パラメータ
 *
 * @return  マークの数
 */
//-----------------------------------------------------------------------------
u32 ParurePokeListDraw::GetPutParamMarkMax( u32 prm )
{
  if( prm == 0 )
  {
    return 0;
  }
  else if( prm >= 1 && prm <= 49 )
  {
    return 1;
  }
  else if( prm >= 50 && prm <= 99 )
  {
    return 2;
  }
  else if( prm >= 100 && prm <= 149 )
  {
    return 3;
  }
  else if( prm >= 150 && prm <= 254 )
  {
    return 4;
  }
  return 5;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPokeHeart
 * @brief   なかよし度表示
 * @date    2015.11.18
 *
 * @param   pp      ポケモンパラメータ
 * @param   is_egg  true = タマゴ
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::PutPokeHeart( const pml::pokepara::PokemonParam * pp, bool is_egg )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NAKAYOSHIICON_00,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NAKAYOSHIICON_01,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NAKAYOSHIICON_02,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NAKAYOSHIICON_03,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NAKAYOSHIICON_04,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  bool is_blank = false;
  u32 mark = 0;
  u8 alpha = 128;

  // 表示可能か
  if( is_egg == false )
  {
    is_blank = true;
    mark  = GetPutParamMarkMax( PokeTool::nakayoshi::GetFriendship(pp,m_myStatus) );
    alpha = 255;
  }

  // ラベル
  g2d->SetPaneAlpha( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PARAM_00, alpha );
  // 背景
  g2d->SetPaneVisible( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_BGPARAM_00, is_blank );
  // マーク
  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( i < mark )
    {
      g2d->SetPaneVisible( LYTID_LOWER, tbl[i], true );
    }
    else
    {
      g2d->SetPaneVisible( LYTID_LOWER, tbl[i], false );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPokeHungry
 * @brief   満腹度表示
 * @date    2015.11.18
 *
 * @param   pp  ポケモンパラメータ
 * @param   is_egg  true = タマゴ
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::PutPokeHungry( const pml::pokepara::PokemonParam * pp, bool is_egg )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_MANPUKUICON_00,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_MANPUKUICON_01,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_MANPUKUICON_02,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_MANPUKUICON_03,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_MANPUKUICON_04,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  bool is_blank = false;
  u32 mark = 0;
  u8 alpha = 128;

  // 表示可能か
  if( is_egg == false && App::Event::PokeParureEvent::IsNoHungryPokemon(pp->GetMonsNo()) == false )
  {
    is_blank = true;
    mark  = GetPutParamMarkMax( pp->GetFeed() );
    alpha = 255;
  }

  // ラベル
  g2d->SetPaneAlpha( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PARAM_01, alpha );
  // 背景
  g2d->SetPaneVisible( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_BGPARAM_01, is_blank );
  // マーク
  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( i < mark )
    {
      g2d->SetPaneVisible( LYTID_LOWER, tbl[i], true );
    }
    else
    {
      g2d->SetPaneVisible( LYTID_LOWER, tbl[i], false );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPokeNadenade
 * @brief   なでなで表示
 * @date    2015.11.18
 *
 * @param   pp  ポケモンパラメータ
 * @param   is_egg  true = タマゴ
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::PutPokeNadenade( const pml::pokepara::PokemonParam * pp, bool is_egg )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NADENADEICON_00,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NADENADEICON_01,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NADENADEICON_02,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NADENADEICON_03,
    PANENAME_KAWAIGARI_SLC_LOW_000_PANE_NADENADEICON_04,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  bool is_blank = false;
  u32 mark = 0;
  u8 alpha = 128;

  // 表示可能か
  if( is_egg == false )
  {
    is_blank = true;
    mark  = GetPutParamMarkMax( pp->GetNadenadeValue() );
    alpha = 255;
  }

  // ラベル
  g2d->SetPaneAlpha( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_PARAM_02, alpha );
  // 背景
  g2d->SetPaneVisible( LYTID_LOWER, PANENAME_KAWAIGARI_SLC_LOW_000_PANE_BGPARAM_02, is_blank );
  // マーク
  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( i < mark )
    {
      g2d->SetPaneVisible( LYTID_LOWER, tbl[i], true );
    }
    else
    {
      g2d->SetPaneVisible( LYTID_LOWER, tbl[i], false );
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    CreatePokeIcon
 * @brief   ポケモンアイコン生成
 * @date    2015.11.24
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::CreatePokeIcon(void)
{
  m_pPokeIcon = GFL_NEW(m_heap->GetSystemHeap()) app::tool::PokeIcon( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), pml::PokeParty::MAX_MEMBERS, true );
  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS+1; i++ )
  {
    m_pPokeIcon->AllocTextureBuffer( i );
  }
  m_pPokeIcon->FileOpen( m_heap->GetSystemHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePokeIcon
 * @brief   ポケモンアイコン削除
 * @date    2015.11.24
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListDraw::DeletePokeIcon(void)
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
 * @func    LoadPokeIcon
 * @brief   ポケモンアイコン読み込み
 * @date    2015.11.24
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListDraw::LoadPokeIcon(void)
{
  switch( m_iconLoadSeq )
  {
  case 0:
    if( m_iconLoadCount >= m_pokeParty->GetMemberCount() )
    {
      return true;
    }
    m_pPokeIcon->ReadRequest( m_iconLoadCount, m_pokeParty->GetMemberPointer(m_iconLoadCount) );
    m_iconLoadSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPokeIcon->IsLoadFinished(m_iconLoadCount) == false )
    {
      break;
    }
    m_pPokeIcon->ReplaceReadTexture( m_iconLoadCount, GetPokeIconPicturePane(m_iconLoadCount) );
    m_iconLoadCount++;
    m_iconLoadSeq = 0;
    break;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPokeIcon
 * @brief   ポケモンアイコン初期化
 * @date    2015.11.24
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListDraw::InitPokeIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_pPokeIcon->IsFileOpen() == false )
    {
      break;
    }
    m_pPokeIcon->LoadDummyTextureRequest();
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPokeIcon->IsLoadDummyTextureFinished() == false )
    {
      break;
    }
    for( u32 i=0; i<m_pokeParty->GetMemberCount(); i++ )
    {
      m_pPokeIcon->ReplaceReadDummyTexture( GetPokeIconPicturePane(i) );
    }
    m_iconLoadSeq = 0;  // 個別呼び出し開始
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPokeIconPicturePane
 * @brief   ポケモンアイコンのピクチャペインを取得
 * @date    2015.11.24
 *
 * @param   index   ポケモンの並び位置
 *
 * @return  ピクチャペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPicture * ParurePokeListDraw::GetPokeIconPicturePane( u32 index )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  return lytwk->GetPicturePane( lytwk->GetPartsPane(PokeIconPaneTable[index]), PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, &res_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateEffect
 * @brief   パーティクル関連生成
 * @date    2016.01.15
 *
 * @param   buf   リソースバッファ
 */
//-----------------------------------------------------------------------------
void ParurePokeListDraw::CreateEffect( void * buf )
{
  m_pEffectUtil = GFL_NEW(m_heap->GetSystemHeap()) app::util::EffectUtil();

  {
    app::util::EffectUtil::Description decc;
    u32 group_array[1] = { 1 };

    decc.layerNum = 1;
    decc.effectResNumMax = 1;
    decc.mode = app::util::EffectUtil::MODE_1RES_1EMITTER_SET_1GROUP;
    decc.effectGroupIDNumMaxArray = group_array;

    m_pEffectUtil->Create( m_heap->GetDeviceHeap(), decc );
  }

  m_pEffectUtil->EffectSystemEntryResource( buf, 0 );
  m_pEffectUtil->GetData(0)->effectGroupIDList->PushFront( 0 );

  {
    app::util::AppRenderingManager::CreateDefaultCamera( m_heap, &m_effectDrawEnvNodeOfCamera, &m_effectCamera );
    m_effectCamera->SetupCameraLookAt( gfl2::math::Vector3(0,5,10), gfl2::math::Vector3(0,0,0), gfl2::math::Vector3(0,1,0) );
  }
  {
    m_renderingManager->SetEffectRenderPathConfig( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pEffectUtil->GetData(0)->effectRenderPathConfig );
    m_renderingManager->SetEffectCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_effectCamera );
  }

  m_pEffectUtil->CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteEffect
 * @brief   パーティクル関連削除
 * @date    2016.01.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListDraw::DeleteEffect(void)
{
  if( m_pEffectUtil == NULL )
  {
    return true;
  }

  switch( m_subSeq )
  {
  case 0:
    m_renderingManager->UnsetEffectRenderPathConfig( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
    m_renderingManager->UnsetEffectCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
    m_pEffectUtil->KillAllEmitterSetForMode1Res1EmitterSet1Group();
    m_subSeq++;
    break;

  case 1:
    m_pEffectUtil->EffectSystemClearResource( 0 );
    GFL_SAFE_DELETE( m_effectCamera );
    GFL_SAFE_DELETE( m_effectDrawEnvNodeOfCamera );
    GFL_SAFE_DELETE( m_pEffectUtil );
    m_subSeq = 0;
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.11.18
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
app::ui::UIInputListener::ListenerResult ParurePokeListDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( button_id >= BTNID_POKEMON_00 && button_id <= BTNID_POKEMON_05 )
  {
    ChangePokemon( button_id-BTNID_POKEMON_00 );
    return DISABLE_ACCESS;
  }

  if( button_id == BTNID_CHANGE )
  {
    m_listener->OnAction( m_pos );
    return DISABLE_ACCESS;
  }
  if( button_id == BTNID_RETURN )
  {
    m_listener->OnAction( pml::PokeParty::MAX_MEMBERS );
    return DISABLE_ACCESS;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.11.12
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
app::ui::UIInputListener::ListenerResult ParurePokeListDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(PokeParure)
GFL_NAMESPACE_END(App)
