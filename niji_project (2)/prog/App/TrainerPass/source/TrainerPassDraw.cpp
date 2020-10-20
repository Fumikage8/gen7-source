//=============================================================================
/**
 * @file    TrainerPassDraw.cpp
 * @brief   トレーナーパス画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.11.06
 */
//=============================================================================

// module
#include "TrainerPassDraw.h"
// niji
#include "GameSys/include/GameData.h"
#include "Sound/include/Sound.h"
// resource
#include "niji_conv_header/app/trainer_pass/TrainerPass.h"
#include "niji_conv_header/app/trainer_pass/TrainerPass_pane.h"
#include "niji_conv_header/app/trainer_pass/TrainerPass_anim_list.h"
#include "niji_conv_header/message/msg_trainer_pass.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TrainerPass)

//! スタンプを押したときのSEを再生するアニメフレーム
const f32 TrainerPassDraw::STAMP_PUT_SE_FRAME = 6.0f;
//! スタンプを離したときのSEを再生するアニメフレーム
const f32 TrainerPassDraw::STAMP_RELEASE_SE_FRAME = 20.0f;


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 * @param   is_demo     true = スタンプデモ
 */
//-----------------------------------------------------------------------------
TrainerPassDraw::TrainerPassDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, bool is_demo )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_isStampDemo( is_demo )
  , m_listener( NULL )
  , m_myStatus( NULL )
  , m_zukanData( NULL )
  , m_gameTime( NULL )
  , m_playTime( NULL )
  , m_misc( NULL )
  , m_eventWork( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pTrainerIcon( NULL )
  , m_page( 0 )
  , m_mainSeq( 0 )
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
TrainerPassDraw::~TrainerPassDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.11.07
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::GetGameData(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  GameSys::GameData * gd    = gm->GetGameData();

  m_myStatus  = gd->GetPlayerStatus();
  m_zukanData = gd->GetZukanData();
  m_gameTime  = gd->GetGameTime();
  m_playTime  = gd->GetPlayTime();
  m_misc      = gd->GetMisc();
  m_eventWork = gd->GetEventWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::Update(void)
{
  app::ui::UIView::Update();
  UpdatePlayTime();
  m_pTrainerIcon->Update();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.11.06
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_UPPER );
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_LOWER );
    m_pTrainerIcon->Draw( displayNo );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDrawListener
 * @brief   リスナーをセット
 * @date    2015.11.06
 *
 * @param   listener リスナー
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::SetDrawListener( DrawListener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2015.11.06
 *
 * @param   res_buf   レイアウトリソースバッファ
 * @param   msg_buf   メッセージリソースバッファ
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassDraw::Init( void * res_buf, void * msg_buf )
{
  switch( m_mainSeq )
  {
  case 0:
    Initialize2D( res_buf, msg_buf );
    CreateButton();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateTrainerIcon() != false )
    {
      SetInputListener( this );
      m_mainSeq = 0;
      return true;
    }
    break;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2015.11.06
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  if( DeleteTrainerIcon() == false )
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
 * @date    2015.11.06
 *
 * @param   res_buf   レイアウトリソースバッファ
 * @param   msg_buf   メッセージリソースバッファ
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::Initialize2D( void * res_buf, void * msg_buf )
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
      LYTRES_TRAINERPASS_TRAINERPASS_MIN_UPP_BFLYT,
      LA_TRAINERPASS_TRAINERPASS_MIN_UPP___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_TRAINERPASS_TRAINERPASS_MIN_LOW_BFLYT,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_buf, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

//  SetWordSetLoader( wset_loader );

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

  WriteDefaultString();
  UpdatePlayTime();

/*  @note ウザイのでいらないとのこと。
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->StartAnime( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_L_KEEP );
  g2d->StartAnime( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_R_KEEP );
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::Terminate2D(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始処理
 * @date    2015.11.06
 *
 * @param   page  表示ページ
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassDraw::Start( s32 page )
{
  PutPage( PUT_DEFAULT, page );
  ChangeStamp( PUT_DEFAULT, page );
  SetPage( page );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsStart
 * @brief   開始処理が終了したかをチェック
 * @date    2015.11.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassDraw::IsStart(void)
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteDefaultSrring
 * @brief   デフォルト文字列描画
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::WriteDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  // なまえ
  g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_NAME_00, static_cast<u32>(tr_pass_01_01) );
  g2d->SetRegisterPlayerName( 0, m_myStatus );
  g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_NAME_01, tr_pass_01_02 );

  // IDNo.
  g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_ID_00, static_cast<u32>(tr_pass_01_03) );
  g2d->SetRegisterNumber( 0, m_myStatus->GetDrawID(), 6, print::NUM_DISP_ZERO );
  g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_ID_01, tr_pass_01_04 );

  // おこづかい
  g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_MONEY_00, static_cast<u32>(tr_pass_01_08) );
  g2d->SetRegisterNumber( 0, m_misc->GetGold(), Savedata::Misc::MY_GOLD_KETA );
  g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_MONEY_01, tr_pass_01_09 );

  // バトルポイント
  g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_BP_00, static_cast<u32>(tr_pass_01_12) );
  g2d->SetRegisterNumber( 0, m_misc->GetBP(), Savedata::Misc::BP_KETA );
  g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_BP_01, tr_pass_01_13 );

  // ぼうけんをはじめたとき
  g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_PLAYDAY_00, static_cast<u32>(tr_pass_01_18) );
  {
    gfl2::system::Date * date = GFL_NEW_LOW(m_heap->GetSystemHeap()) gfl2::system::Date();
    date->SetDateTimeToSecond( m_gameTime->GetStartSec() );
    g2d->SetRegisterNumber( 0, date->GetYear(), 4 );
    g2d->SetRegisterNumber( 1, date->GetMonth(), 2 );
    g2d->SetRegisterNumber( 2, date->GetDay(), 2 );
    GFL_SAFE_DELETE( date );
    g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_PLAYDAY_01, tr_pass_01_19 );
  }

  // ／（ページ）
  g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_TRAINERPASS_MIN_LOW_PANE_PAGE_NO_01, static_cast<u32>(tr_pass_02_02) );
  g2d->SetTextBoxPaneString( LYTID_LOWER, PANENAME_TRAINERPASS_MIN_LOW_PANE_PAGE_NO_04, static_cast<u32>(tr_pass_02_02) );
  g2d->SetRegisterNumber( 0, STAMP_PAGE_MAX, 2 );
  g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, PANENAME_TRAINERPASS_MIN_LOW_PANE_PAGE_NO_02, tr_pass_02_01 );
  g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, PANENAME_TRAINERPASS_MIN_LOW_PANE_PAGE_NO_05, tr_pass_02_01 );

  // すがた/しゃしん
  if( m_isStampDemo == false )
  {
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_UPPER );
    gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
    g2d->SetTextBoxPaneString(
      lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_TRAINERPASS_MIN_UPP_PANE_A_BTN_TXT), PANENAME_COMMON_PLT_002_PANE_TEXTBOX_00, &res_id),
      tr_pass_01_50 );
  }
  else
  {
    g2d->SetPaneVisible( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_A_BTN_TXT, false );
  }

  // 図鑑
  if( m_eventWork->CheckEventFlag(SYS_FLAG_ZUKAN_GET) == false )
  {
    g2d->SetPaneVisible( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_POKEDEX_00, false );
    g2d->SetPaneVisible( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_POKEDEX_01, false );
    g2d->SetPaneVisible( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_POKEDEX_02, false );
  }
  else
  {
    // ずかん
    g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_POKEDEX_00, static_cast<u32>(tr_pass_01_05) );
    // ひき（ずかん）
    g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_POKEDEX_02, static_cast<u32>(tr_pass_01_07) );

    // 地方図鑑の数を取得
    g2d->SetRegisterNumber( 0, m_zukanData->GetLocalPokeGetCountStatic( PokeTool::ExtendData::LOCAL_AREA_A ), 3, print::NUM_DISP_LEFT );
    g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_POKEDEX_01, tr_pass_01_06 );
  }

  // プレイ時間
  g2d->SetTextBoxPaneString( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_PLAYTIME_00, static_cast<u32>(tr_pass_01_14) );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdatePlayTime
 * @brief   プレイ時間表示更新
 * @date    2015.11.07
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::UpdatePlayTime(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->SetRegisterNumber( 0, m_playTime->GetPlayHour(), 3, print::NUM_DISP_LEFT );
  g2d->SetRegisterNumber( 1, m_playTime->GetPlayMinute(), 2, print::NUM_DISP_ZERO );
  g2d->SetTextBoxPaneStringExpand( LYTID_UPPER, PANENAME_TRAINERPASS_MIN_UPP_PANE_PLAYTIME_01, tr_pass_01_15 );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.11.07
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::CreateButton(void)
{
  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    { // 戻る
      BTNID_RETURN, NULL, NULL,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__BTN_BACK_TOUCH,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__BTN_BACK_RELEASE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__BTN_BACK_CANSEL,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__BTN_BACK_TOUCH_RELEASE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__BTN_BACK_ACTIVE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__BTN_BACK_PASSIVE,
    },
    { // スタンプ切り替え（左）
      BTNID_STAMP_L, NULL, NULL,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_L_TOUCH,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_L_RELEASE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_L_CANSEL,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_L_TOUCH_RELEASE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_L_ACTIVE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_L_PASSIVE,
    },
    { // スタンプ切り替え（右）
      BTNID_STAMP_R, NULL, NULL,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_R_TOUCH,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_R_RELEASE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_R_CANSEL,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_R_TOUCH_RELEASE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_R_ACTIVE,
      LA_TRAINERPASS_TRAINERPASS_MIN_LOW__CURSOR_R_PASSIVE,
    },
  };
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_TRAINERPASS_MIN_LOW_PANE_BTN_BACK,
    PANENAME_TRAINERPASS_MIN_LOW_PANE_CURSOR_L,
    PANENAME_TRAINERPASS_MIN_LOW_PANE_CURSOR_R,
  };
  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[BTNID_MAX];

  for( u32 i=0; i<BTNID_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane    = lytwk->GetPane( parts, bound_tbl[i], &res_id );
  }

  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, BTNID_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
  man->SetButtonSelectSE( BTNID_RETURN, SEQ_SE_CANCEL1 );
  man->SetButtonSelectSE( BTNID_STAMP_L, SEQ_SE_TPASS_PAGETURN );
  man->SetButtonSelectSE( BTNID_STAMP_R, SEQ_SE_TPASS_PAGETURN );
  man->SetButtonBindKey( BTNID_RETURN, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BTNID_STAMP_L, gfl2::ui::BUTTON_L|gfl2::ui::DIR_LEFT, app::tool::ButtonManager::BIND_KEY_MODE_REPEAT );
  man->SetButtonBindKey( BTNID_STAMP_R, gfl2::ui::BUTTON_R|gfl2::ui::DIR_RIGHT, app::tool::ButtonManager::BIND_KEY_MODE_REPEAT );

  if( m_isStampDemo != false )
  {
    man->SetButtonInvisible( BTNID_RETURN );
    man->SetButtonInvisible( BTNID_STAMP_L );
    man->SetButtonInvisible( BTNID_STAMP_R );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartPageChangeAnime
 * @brief   ページ切り替えアニメ開始
 * @date    2015.11.09
 *
 * @param   mv  切り替え方向
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::StartPageChangeAnime( s32 mv )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( mv > 0 )
  {
    g2d->StartAnime( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_PAGE_00 );
  }
  else if( mv < 0 )
  {
    g2d->StartAnime( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_PAGE_01 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEndPageChangeAnime
 * @brief   ページ切り替えアニメ終了チェック
 * @date    2015.11.09
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassDraw::IsEndPageChangeAnime(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( g2d->IsAnimeEnd( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_PAGE_00 ) != false &&
      g2d->IsAnimeEnd( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_PAGE_01 ) != false )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPage
 * @brief   ページ設定
 * @date    2015.11.09
 *
 * @param   page  ページ番号
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::SetPage( s32 page )
{
  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( m_isStampDemo == false )
  {
    if( page == 0 )
    {
      man->SetButtonInvisible( BTNID_STAMP_L );
    }
    else if( page == (STAMP_PAGE_MAX-1) )
    {
      man->SetButtonInvisible( BTNID_STAMP_R );
    }
    else
    {
      man->SetButtonVisible( BTNID_STAMP_L );
      man->SetButtonVisible( BTNID_STAMP_R );
    }
  }

  // めくったページを元の位置に戻す
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->BindAnime( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_PAGE_01 );
  g2d->StartAnimeFrame( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_PAGE_01, g2d->GetAnimeMaxFrame(LYTID_LOWER,LA_TRAINERPASS_TRAINERPASS_MIN_LOW_PAGE_01) );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPage
 * @brief   ページ表示
 * @date    2015.11.09
 *
 * @param   id        表示ID
 * @param   page      ページ番号
 * @param   new_page  追加するページ（スタンプ演出時の新規追加ページ）
 *
 * @param   通常時は new_page = NEW_STAMP_NONE
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::PutPage( StampPutID id, u32 page, u32 new_page )
{
  struct TEXT_DATA
  {
    u32 name;
    u32 info;
  };

  struct LYT_DATA
  {
    gfl2::lyt::LytPaneIndex name;
    gfl2::lyt::LytPaneIndex info;
    gfl2::lyt::LytPaneIndex page;
  };

  static const TEXT_DATA text_tbl[] =
  {
    { tr_pass_10_01, tr_pass_10_02 },
    { tr_pass_11_01, tr_pass_11_02 },
    { tr_pass_12_01, tr_pass_12_02 },
    { tr_pass_13_01, tr_pass_13_02 },
    { tr_pass_14_01, tr_pass_14_02 },
    { tr_pass_15_01, tr_pass_15_02 },
    { tr_pass_16_01, tr_pass_16_02 },
    { tr_pass_17_01, tr_pass_17_02 },
    { tr_pass_18_01, tr_pass_18_02 },
    { tr_pass_19_01, tr_pass_19_02 },
    { tr_pass_20_01, tr_pass_20_02 },
    { tr_pass_21_01, tr_pass_21_02 },
    { tr_pass_22_01, tr_pass_22_02 },
    { tr_pass_24_01, tr_pass_24_02 },
    { tr_pass_23_01, tr_pass_23_02 },
  };

  static const LYT_DATA lyt_tbl[] =
  {
    { PANENAME_TRAINERPASS_MIN_LOW_PANE_STAMP_NAME_00, PANENAME_TRAINERPASS_MIN_LOW_PANE_STAMP_INFO_00, PANENAME_TRAINERPASS_MIN_LOW_PANE_PAGE_NO_00 },
    { PANENAME_TRAINERPASS_MIN_LOW_PANE_STAMP_NAME_01, PANENAME_TRAINERPASS_MIN_LOW_PANE_STAMP_INFO_01, PANENAME_TRAINERPASS_MIN_LOW_PANE_PAGE_NO_03 },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  if( m_misc->IsTrainerPassStamp(static_cast<Savedata::TrainerPassStampID>(page)) == false || page == new_page )
  {
    g2d->SetPaneVisible( LYTID_LOWER, lyt_tbl[id].name, false );
    g2d->SetPaneVisible( LYTID_LOWER, lyt_tbl[id].info, false );
  }
  else
  {
    g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, lyt_tbl[id].name, static_cast<u32>(text_tbl[page].name) );  // @fix ContactNMCat[764]: EFIGS文法タグ使用のため展開
    g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, lyt_tbl[id].info, static_cast<u32>(text_tbl[page].info) );  // @fix ContactNMCat[764]: EFIGS文法タグ使用のため展開
    g2d->SetPaneVisible( LYTID_LOWER, lyt_tbl[id].name, true );
    g2d->SetPaneVisible( LYTID_LOWER, lyt_tbl[id].info, true );
  }

  g2d->SetRegisterNumber( 0, page+1, 2 );
  g2d->SetTextBoxPaneStringExpand( LYTID_LOWER, lyt_tbl[id].page, tr_pass_02_01 );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeStamp
 * @brief   スタンプ切り替え
 * @date    2015.11.09
 *
 * @param   id        表示ID
 * @param   page      ページ番号
 * @param   new_page  追加するページ（スタンプ演出時の新規追加ページ）
 *
 * @param   通常時は new_page = NEW_STAMP_NONE
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::ChangeStamp( StampPutID id, u32 page, u32 new_page )
{
  static const u32 anm_tbl[] =
  {
    LA_TRAINERPASS_TRAINERPASS_MIN_LOW__MARK_00_STAMP_TEX,
    LA_TRAINERPASS_TRAINERPASS_MIN_LOW__MARK_01_STAMP_TEX,
  };

  if( m_misc->IsTrainerPassStamp(static_cast<Savedata::TrainerPassStampID>(page)) == false || page == new_page )
  {
    GetG2DUtil()->StartAnimeFrame( LYTID_LOWER, anm_tbl[id], 0, false );
  }
  else
  {
#if PM_VERSION == VERSION_SUN2
    static const u8 frm_tbl[STAMP_PAGE_MAX] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16 };
    GetG2DUtil()->StartAnimeFrame( LYTID_LOWER, anm_tbl[id], frm_tbl[page], false );
#elif PM_VERSION == VERSION_MOON2
    static const u8 frm_tbl[STAMP_PAGE_MAX] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16 };
    GetG2DUtil()->StartAnimeFrame( LYTID_LOWER, anm_tbl[id], frm_tbl[page], false );
#endif
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    SetStampAnime
 * @brief   スタンプアニメ再生
 * @date    2015.11.09
 *
 * @param   flg   true = 押す, false = 離れる
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::StartStampAnime( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( flg != false )
  {
    g2d->StartAnime( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_STAMP_IN );
  }
  else
  {
    g2d->StartAnime( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_STAMP_OUT );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEndStampAnime
 * @brief   スタンプアニメ終了チェック
 * @date    2015.11.09
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassDraw::IsEndStampAnime(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( g2d->IsAnimeEnd( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_STAMP_IN ) != false &&
      g2d->IsAnimeEnd( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_STAMP_OUT ) != false )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    PlaySE_StampPut
 * @brief   スタンプを押したときのSEを再生
 * @date    2015.11.13
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::PlaySE_StampPut(void)
{
  if( GetG2DUtil()->GetAnimeFrame( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_STAMP_IN ) == STAMP_PUT_SE_FRAME )
  {
    Sound::PlaySE( SEQ_SE_TPASS_HANKO );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PlaySE_StampRelease
 * @brief   スタンプを離したときのSEを再生
 * @date    2015.11.13
 */
//-----------------------------------------------------------------------------
void TrainerPassDraw::PlaySE_StampRelease(void)
{
  if( GetG2DUtil()->GetAnimeFrame( LYTID_LOWER, LA_TRAINERPASS_TRAINERPASS_MIN_LOW_STAMP_OUT ) == STAMP_RELEASE_SE_FRAME )
  {
    Sound::PlaySE( SEQ_SE_TPASS_HANKORELEASE );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateTrainerIcon
 * @brief   トレーナーアイコン生成
 * @date    2015.11.11
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassDraw::CreateTrainerIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pTrainerIcon = GFL_NEW(m_heap->GetSystemHeap()) app::tool::AppToolTrainerIconRendering();
    m_pTrainerIcon->Initialize(
      m_renderingManager,
      m_heap,
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),
      1,
      app::util::G2DUtil::SETUP_UPPER );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pTrainerIcon->IsReady() == false )
    {
      break;
    }
    {
      gfl2::lyt::LytWk * lytwk = GetG2DUtil()->GetLayoutWork( LYTID_UPPER );
      m_iconData.pReplacePane     = lytwk->GetPicturePane( PANENAME_TRAINERPASS_MIN_UPP_PANE_PLAYERPHOTO );
      m_iconData.pSyncVisiblePane = lytwk->GetPane( PANENAME_TRAINERPASS_MIN_UPP_PANE_TIMER_POS );
      m_iconData.pInLoadVisibleChangePane = lytwk->GetPane( PANENAME_TRAINERPASS_MIN_UPP_PANE_WINDOW_PLAYER_FRAME );
    }
    m_iconData.ConvFromMyStatus( *m_myStatus );
    m_pTrainerIcon->SetIconObjectData(
      0,
      m_iconData,
      gfl2::math::ConvertNwVec3ToGfVec3(m_iconData.pSyncVisiblePane->GetTranslate()) );
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteTrainerIcon
 * @brief   トレーナーアイコン削除
 * @date    2015.11.11
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassDraw::DeleteTrainerIcon(void)
{
  if( m_pTrainerIcon != NULL )
  {
    switch( m_subSeq )
    {
    case 0:
      m_pTrainerIcon->Terminate();
      m_subSeq++;
      /* FALL THROUGH */
    case 1:
      if( m_pTrainerIcon->IsDelete() == false )
      {
        return false;
      }
      GFL_SAFE_DELETE( m_pTrainerIcon );
      break;
    }
  }
  return true;
}



//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.11.06
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
app::ui::UIInputListener::ListenerResult TrainerPassDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( button_id == BTNID_RETURN )
  {
    m_listener->OnAction( DrawListener::ACTION_ID_B_BUTTON );
    return DISABLE_ACCESS;
  }
  if( button_id == BTNID_STAMP_L )
  {
    m_listener->OnAction( DrawListener::ACTION_ID_STAMP_L );
    return DISABLE_ACCESS;
  }
  if( button_id == BTNID_STAMP_R )
  {
    m_listener->OnAction( DrawListener::ACTION_ID_STAMP_R );
    return DISABLE_ACCESS;
  }
  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.11.06
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
app::ui::UIInputListener::ListenerResult TrainerPassDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    Sound::PlaySE( SEQ_SE_DECIDE1 );
    m_listener->OnAction( DrawListener::ACTION_ID_A_BUTTON );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(TrainerPass)
GFL_NAMESPACE_END(App)
