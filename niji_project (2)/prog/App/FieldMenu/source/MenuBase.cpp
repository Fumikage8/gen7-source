//=============================================================================
/**
 * @file    MenuBase.cpp
 * @brief   フィールドメニュー基本処理
 * @author  Hiroyuki Nakamura
 * @date    2015.07.13
 */
//=============================================================================

// module
#include "App/FieldMenu/include/FieldMenuListener.h"
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
#include "MenuDefine.h"
#include "MenuBase.h"
#include "RotomFriendship.h"
// momiji
#include "System/include/GflUse.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/EventWork.h"
#include "Savedata/include/Record.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/AppToolRotomFriendship.h"
#include "AppLib/include/Tool/app_tool_Gauge.h"
#include "Sound/include/Sound.h"
#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"
#include "Field/FieldStatic/include/RotomPower/RotomPowerTime.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/FieldMenu.gaix"
#include "niji_conv_header/app/field_menu/MainUpper.h"
#include "niji_conv_header/app/field_menu/MainUpper_pane.h"
#include "niji_conv_header/app/field_menu/MainUpper_anim_list.h"
#include "niji_conv_header/field/script/inc/scene_work_const.inc"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//! ロトムアニメデータ
#include "RotomAnime.cdat"
//! ロトム分岐データ
#include "niji_conv_header/app/data/field_menu/RotomQuestionData.cdat"
//! ロトム分岐データ
#include "niji_conv_header/app/data/field_menu/RotomQuestionFuncData.cdat"

//! ボタン用当り判定のペインテーブル
static const gfl2::lyt::LytPaneIndex ButtonBoundTable[] =
{
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_08,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_00,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_07,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_01,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_02,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_03,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_04,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_05,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_BOUND_06,
};

//! ロトム選択肢メニュー部品ペインテーブル
static const gfl2::lyt::LytPaneIndex RotomSelectMenuPartsTable[] =
{
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_ANSWER_00,
  PANENAME_FIELDMENU_RTM_LOW_000_PANE_ANSWER_01,
};

//! ロトム状況報告発生確率計算値
const f32 MenuBase::ROTOM_FS_TALK_START_LOT = 0.0008;

//!< ロトポンアイテム
const MenuBase::ROTOPON_ITEM MenuBase::RotoponItem[MenuBase::ROTOPON_ITEM_MAX] =
{
  { ITEM_HOZYOPON,      2, msg_field_rotom_rotopon_13 },  // @note チュートリアルで必ず設定するため０固定
  { ITEM_TAMAGOHUKAPON, 3, msg_field_rotom_rotopon_04 },
  { ITEM_YASUURIPON,    0, msg_field_rotom_rotopon_05 },
  { ITEM_OKODUKAIPON,   0, msg_field_rotom_rotopon_06 },
  { ITEM_KEIKENTIPON,   0, msg_field_rotom_rotopon_07 },
  { ITEM_NATUKIPON,     5, msg_field_rotom_rotopon_08 },
  { ITEM_SOUGUUPON,     1, msg_field_rotom_rotopon_09 },
  { ITEM_ONMITUPON,     1, msg_field_rotom_rotopon_10 },
  { ITEM_KAIHUKUPON,    4, msg_field_rotom_rotopon_11 },
  { ITEM_PIIPIIPON,     4, msg_field_rotom_rotopon_12 },
  { ITEM_TUKAMAEPON,    1, msg_field_rotom_rotopon_14 },
};

//!< ロトムメッセージ用動作アニメテーブル
const u32 MenuBase::RotomMessageAnimeTable[MenuBase::ROTOM_MSG_ANM_MAX] =
{
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN,
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_OUT,
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN_LEFT,
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_OUT_LEFT,
};

//! ロトム時間帯カラーアニメテーブル
const u32 MenuBase::RotomTimeColorAnimeTable[MenuBase::TIME_COLOR_ANIME_MAX] =
{
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_TIME_00,
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_TIME_01,
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_TIME_02,
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_TIME_03,
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_TIME_04,
  LA_MAINUPPER_FIELDMENU_RTM_LOW_000_TIME_05,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 * @param   mode        表示モード
 * @param   is_passive  下画面入力禁止フラグ
 */
//-----------------------------------------------------------------------------
MenuBase::MenuBase( app::util::Heap * heap, app::util::AppRenderingManager * render_man, const s32 * mode, const bool * is_passive )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_dispMode( mode )
  , m_frameListener( NULL )
  , m_gameData( NULL )
  , m_saveFieldMenu( NULL )
  , m_saveMisc( NULL )
  , m_myStatus( NULL )
  , m_playData( NULL )
  , m_touchPanel( NULL )
  , m_eventWork( NULL )
  , m_pLytResBuff( NULL )
  , m_pWordSet( NULL )
  , m_pStrBuf( NULL )
  , m_pExpBuf( NULL )
  , m_isInitialized( false )
  , m_rotomMsgData( NULL )
  , m_rotomMsgSeq( 0 )
  , m_isRotomUpdateStop( false )
  , m_rotomMsgIndex( PlayData::ROTOM_MSG_MAX )
  , m_rotomMsgStartAnime( LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN )
  , m_rotomMotionSeq( 0 )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
  , m_isCreateLayout( 0 )
  , m_pRotomFriendship( NULL )
  , m_rotomAnime( ROTOM_ANIME_NULL )
  , m_rotomFace( ROTOM_FACE_NONE )
  , m_isRotomFriendshipUp( false )
  , m_rotomQuestion( NULL )
  , m_rotomMenuSelect( ROTOM_SELECT_NONE )
  , m_rotoponSeq( ROTOPON_SEQID_NONE )
  , m_isRotoponInputTrg( false )
  , m_isRotoponScript( false )
  , m_rotoponItem( NULL )
  , m_pokeParty( NULL )
  , m_configSave( NULL )
  , m_myItem( NULL )
  , m_rotomPower( NULL )
  , m_isRotomPowerGaugeOn( false )
  , m_isEventUpdate( false )
  , m_rotomTimeColorAnime( 0xffffffff )
  , m_rotomTimeColorFrame( 0xffffffff )
  , m_isPassive( is_passive )
#if PM_DEBUG
  , m_debugRotomTouchPos( 0 )
  , m_debugRotomTouchCount( 0 )
  , m_debugIsTimeZoneCheck( false )
#endif  // PM_DEBUG
{
  SetInputEnabled( false );
  SetStickEmulationMode( false );
  GetGameData();
  InitRotomMessage();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
MenuBase::~MenuBase()
{
  m_playData->InitRotomTouch();                         // ロトムのタッチ情報をクリア
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ関連取得
 * @date    2015.09.09
 */
//-----------------------------------------------------------------------------
void MenuBase::GetGameData(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager); 

  m_gameData = gm->GetGameData();

  m_saveFieldMenu = m_gameData->GetFieldMenu();
  m_saveMisc      = m_gameData->GetMisc();
  m_myStatus      = m_gameData->GetPlayerStatus();
  m_playData      = m_gameData->GetFieldMenuPlayData();
  m_touchPanel    = gm->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  m_pokeParty     = m_gameData->GetPlayerParty();
  m_configSave    = m_gameData->GetConfig();
  m_myItem        = m_gameData->GetMyItem();
  m_eventWork     = m_gameData->GetEventWork();
  m_rotomPower    = gm->GetRotomPowerManager();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFrameListener
 * @brief   リスナーをセット
 * @date    2015.11.07
 *
 * @param   listener リスナー
 */
//-----------------------------------------------------------------------------
void MenuBase::SetFrameListener( FrameListener * listener )
{
  m_frameListener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    OpenLayoutFile
 * @brief   レイアウトリソースファイルを開く
 * @date    2016.04.19
 */
//-----------------------------------------------------------------------------
void MenuBase::OpenLayoutFile(void)
{
  app::util::FileAccessor::FileOpen( ARCID_FIELD_MENU, m_heap->GetSystemHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsOpenLayoutFile
 * @brief   レイアウトリソースファイルが開いたかをチェック
 * @date    2016.04.20
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsOpenLayoutFile(void)
{
  return app::util::FileAccessor::IsFileOpen( ARCID_FIELD_MENU );
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize
 * @brief   初期化
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::Initialize(void)
{
  switch( m_subSeq )
  {
  case 0:
    CreateRotomFriendship();
    OpenLayoutFile();
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( IsOpenLayoutFile() == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_FIELD_MENU,
      GARC_FieldMenu_MainUpper_APPLYT,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      false,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    CreateView2D();
    CreateButton();
    StartCoverAnime( LA_MAINUPPER_FIELDMENU_RTM_LOW_000_BG_LOOP, 0.0f );
    SetMessageAnime( m_saveFieldMenu->IsVisibleRotom(), true );
/*
    if( *m_dispMode == DISP_MODE_NORMAL )
    {
      GetG2DUtil()->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_EYE_LOOP );
    }
*/
    SetRotomFace();
    StartRotomPowerGauge( true );
    SetInputListener( this );
    m_isInitialized = true;

      {
        GFL_PRINT(
          "■Base Sys: Free = 0x%08x, Alloc = 0x%08x\n",
          m_heap->GetSystemHeap()->GetTotalFreeSize(),
          m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
        GFL_PRINT(
          "■Base Dev: Free = 0x%08x, Alloc = 0x%08x\n",
          m_heap->GetDeviceHeap()->GetTotalFreeSize(),
          m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
      }

    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitializeSync
 * @brief   初期化（同期版）
 * @date    2016.04.20
 *
 * @return  true
 */
//-----------------------------------------------------------------------------
bool MenuBase::InitializeSync(void)
{
  app::util::FileAccessor::FileLoadSync(
    ARCID_FIELD_MENU,
    GARC_FieldMenu_MainUpper_APPLYT,
    &m_pLytResBuff,
    m_heap->GetDeviceHeap(),
    false,
    nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

  CreateView2D();
  CreateButton();
  CreateRotomFriendship();
  StartCoverAnime( LA_MAINUPPER_FIELDMENU_RTM_LOW_000_BG_LOOP, 0.0f );
  SetMessageAnime( m_saveFieldMenu->IsVisibleRotom(), true );
/*
  if( *m_dispMode == DISP_MODE_NORMAL )
  {
    GetG2DUtil()->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_EYE_LOOP );
  }
*/
  SetRotomFace();
  StartRotomPowerGauge( true );
  SetInputListener( this );
  m_isInitialized = true;

      {
        GFL_PRINT(
          "■Base Sys: Free = 0x%08x, Alloc = 0x%08x\n",
          m_heap->GetSystemHeap()->GetTotalFreeSize(),
          m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
        GFL_PRINT(
          "■Base Dev: Free = 0x%08x, Alloc = 0x%08x\n",
          m_heap->GetDeviceHeap()->GetTotalFreeSize(),
          m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
      }

  m_subSeq = 0;

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate
 * @brief   終了処理
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::Terminate(void)
{
  m_isDrawEnable = false;

  switch( m_subSeq )
  {
  case 0:
    if( IsDrawing() != false )
    {
      break;
    }
    DeleteRotomFriendship();
    DeleteView2D();
    GflHeapFreeMemory( m_pLytResBuff );
    app::util::FileAccessor::FileClose( ARCID_FIELD_MENU, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_FIELD_MENU ) == false )
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
 * @func    Update
 * @brief   更新
 * @date    2015.05.22
 */
//-----------------------------------------------------------------------------
void MenuBase::Update(void)
{
  app::ui::UIView::Update();

  if( m_isInitialized != false && m_isDrawEnable != false )
  {
    if( UpdateRotopon() == false )
    {
      UpdateRotomMessage();
      UpdateRotomMotion();
    }
    SetRotomFace();
    UpdateRotomPowerGauge();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.05.22
 *
 * @param   displayNo   描画ディスプレイ番号
 */
//-----------------------------------------------------------------------------
void MenuBase::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    if( ( m_isCreateLayout & (1<<LYTID_UPPER) ) != 0 )
    {
      g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_UPPER, DRAW_PRI_UPPER_BAR );
    }
    if( ( m_isCreateLayout & (1<<LYTID_LOWER_BASE) ) != 0 )
    {
      g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_LOWER_BASE, DRAW_PRI_LOWER_BG );
    }
    if( ( m_isCreateLayout & (1<< LYTID_LOWER_ROTOM) ) != 0 )
    {
      g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_LOWER_ROTOM, DRAW_PRI_LOWER_COVER );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateView2D
 * @brief   2D関連生成
 * @date    2016.01.08
 */
//-----------------------------------------------------------------------------
void MenuBase::CreateView2D(void)
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { m_pLytResBuff, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
  const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_MAINUPPER_FIELDMENU_MIN_UPP_000_BFLYT,
      LA_MAINUPPER_FIELDMENU_MIN_UPP_000___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      false
    },
    {
      0,
      LYTRES_MAINUPPER_FIELDMENU_DEF_LOW_000_BFLYT,
      LA_MAINUPPER_FIELDMENU_DEF_LOW_000___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
    {
      0,
      LYTRES_MAINUPPER_FIELDMENU_RTM_LOW_000_BFLYT,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false
    },
  };

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    app::ui::UIView::UNUSE_MESSAGE_DATA );

  m_isCreateLayout = ( 1 << LYTID_UPPER ) | ( 1 << LYTID_LOWER_BASE ) | ( 1 << LYTID_LOWER_ROTOM );

  m_pWordSet = GFL_NEW(m_heap->GetSystemHeap()) print::WordSet( m_heap->GetSystemHeap() );

  m_pStrBuf = GFL_NEW(m_heap->GetSystemHeap()) gfl2::str::StrBuf( ROTOM_TEXT_LEN, m_heap->GetSystemHeap() );
  m_pExpBuf = GFL_NEW(m_heap->GetSystemHeap()) gfl2::str::StrBuf( ROTOM_TEXT_LEN, m_heap->GetSystemHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteView2D
 * @brief   2D関連削除
 * @date    2016.01.08
 */
//-----------------------------------------------------------------------------
void MenuBase::DeleteView2D(void)
{
  GFL_SAFE_DELETE( m_pExpBuf );
  GFL_SAFE_DELETE( m_pStrBuf );

  GFL_SAFE_DELETE( m_pWordSet );

  Delete2D();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.05.22
 */
//-----------------------------------------------------------------------------
void MenuBase::CreateButton(void)
{
  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    { // ロトムメッセージ
      BTNID_MESSAGE, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000_SWITCHING,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // タウンマップ
      BTNID_TOWNMAP, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000_SWITCHING,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // 図鑑
      BTNID_ZUKAN, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000_SWITCHING,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // モーション１（左目）
      BTNID_MOTION_01, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // モーション２（右目）
      BTNID_MOTION_02, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // モーション３（左上）
      BTNID_MOTION_03, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // モーション４（右上）
      BTNID_MOTION_04, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // モーション５（左下）
      BTNID_MOTION_05, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // モーション６（右下）
      BTNID_MOTION_06, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { //!< ロトム選択肢１
      BTNID_ROTOM_SELECT_01, NULL, NULL,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_00_TOUCH,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_00_RELEASE,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_00_CANSEL,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_00_TOUCH_RELEASE,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_00_ACTIVE,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_00_PASSIVE,
    },
    { //!< ロトム選択肢２
      BTNID_ROTOM_SELECT_02, NULL, NULL,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_01_TOUCH,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_01_RELEASE,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_01_CANSEL,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_01_TOUCH_RELEASE,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_01_ACTIVE,
      LA_MAINUPPER_FIELDMENU_RTM_LOW_000__ANSWER_01_PASSIVE,
    },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_ROTOM );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[BTNID_MAX];

  for( u32 i=0; i<=BTNID_MOTION_06; i++ )
  {
    info[i] = info_tbl[i];
    info[i].picture_pane = lytwk->GetPane( ButtonBoundTable[i] );
    info[i].bound_pane   = info[i].picture_pane;
  }
  for( u32 i=BTNID_ROTOM_SELECT_01; i<=BTNID_ROTOM_SELECT_02; i++ )
  {
    info[i] = info_tbl[i];
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( RotomSelectMenuPartsTable[i-BTNID_ROTOM_SELECT_01] );
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, PANENAME_FIELDMENU_BTN_LOW_018_PANE_BOUND_00, &res_id );
  }

  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, BTNID_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
  // SE
//  man->SetButtonSelectSE( BTNID_TOWNMAP, SEQ_SE_DECIDE1 );
//  man->SetButtonSelectSE( BTNID_ZUKAN, SEQ_SE_DECIDE1 );
  for( u32 i=0; i<BTNID_MAX; i++ )
  {
    man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }

  // モード別設定
  if( *m_dispMode != DISP_MODE_NORMAL )
  {
    for( u32 i=0; i<BTNID_MAX; i++ )
    {
      man->SetButtonInvisible( i );
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleLayout
 * @brief   上画面レイアウト表示切り替え
 * @date    2015.06.04
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuBase::SetVisibleLayout( bool flg )
{
  GetG2DUtil()->SetLayoutDrawEnable( LYTID_UPPER, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleCover
 * @brief   ロトムの枠レイアウト表示切り替え
 * @date    2016.01.08
 *
 * @param   flg       true = 表示, false = 非表示
 * @param   is_event  true = イベント起動時
 */
//-----------------------------------------------------------------------------
void MenuBase::SetVisibleCover( bool flg, bool is_event )
{
  if( *m_dispMode != DISP_MODE_FRAME_OFF || is_event != false )
  {
    GetG2DUtil()->SetLayoutDrawEnable( LYTID_LOWER_ROTOM, flg );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    VanishRotomEye
 * @brief   ロトムの目を非表示にする
 * @date    2016.04.25
 */
//-----------------------------------------------------------------------------
void MenuBase::VanishRotomEye(void)
{
  GetG2DUtil()->SetPaneAlpha( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_ROTOM_EYE, 0 );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartCoverAnime
 * @brief   ロトムの枠関連のアニメを再生
 * @date    2016.01.08
 *
 * @param   anm     アニメ番号
 * @param   frame   開始フレーム
 */
//-----------------------------------------------------------------------------
void MenuBase::StartCoverAnime( u32 anm, f32 frame )
{
  GetG2DUtil()->StartAnimeFrame( LYTID_LOWER_ROTOM, anm, frame );
}

//-----------------------------------------------------------------------------
/**
 * @func    StopCoverAnime
 * @brief   ロトムの枠関連のアニメを停止
 * @date    2016.01.08
 *
 * @param   anm     アニメ番号
 */
//-----------------------------------------------------------------------------
void MenuBase::StopCoverAnime( u32 anm )
{
  GetG2DUtil()->StopAnime( LYTID_LOWER_ROTOM, anm );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCoverAnimeEnd
 * @brief   ロトムの枠関連のアニメ終了チェック
 * @date    2016.01.08
 *
 * @param   anm     アニメ番号
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsCoverAnimeEnd( u32 anm )
{
  return GetG2DUtil()->IsAnimeEnd( LYTID_LOWER_ROTOM, anm );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCoverAnimeMaxFrame
 * @brief   ロトムの枠関連のアニメの最大フレーム数を取得
 * @date    2016.01.08
 *
 * @param   anm     アニメ番号
 *
 * @return  最大フレーム数
 */
//-----------------------------------------------------------------------------
f32 MenuBase::GetCoverAnimeMaxFrame( u32 anm )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->BindAnime( LYTID_LOWER_ROTOM, anm );
  return g2d->GetAnimeMaxFrame( LYTID_LOWER_ROTOM, anm );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomMessageMoveAnime
 * @brief   ロトムメッセージ開始/終了アニメ取得
 * @date    2016.12.02
 *
 * @param   in    開始アニメ
 * @param   out   終了アニメ
 */
//-----------------------------------------------------------------------------
/*
void MenuBase::GetRotomMessageMoveAnime( u32 * in, u32 * out )
{
  *in  = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN;
  *out = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_OUT;

  PlayData::RotomMsgIndex msg_index = m_playData->GetEnableRottomMsgIndex();

  if( msg_index != PlayData::ROTOM_MSG_MAX )
  {
    u32 label = m_playData->GetRotomMessageText( msg_index );
    if( CheckSelectRotomMessage(label) != false )
    {
      *in  = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN_LEFT;
      *out = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_OUT_LEFT;
    }
  }
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomMessageStartAnime
 * @brief   ロトムメッセージ開始アニメ取得
 * @date    2016.12.19
 *
 * @return  開始アニメ
 */
//-----------------------------------------------------------------------------
u32 MenuBase::GetRotomMessageStartAnime(void)
{
  m_rotomMsgStartAnime = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN;

  PlayData::RotomMsgIndex msg_index = m_playData->GetEnableRottomMsgIndex();

  if( msg_index != PlayData::ROTOM_MSG_MAX )
  {
    u32 label = m_playData->GetRotomMessageText( msg_index );
    // @fix MMCat[333]: 答えも左に動作させる
    if( CheckSelectRotomMessage(label) != false ||
        CheckAnswerRotomMessage(label) != false )
    {
      m_rotomMsgStartAnime = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN_LEFT;
    }
  }

  return m_rotomMsgStartAnime;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomMessageEndAnime
 * @brief   ロトムメッセージ終了アニメ取得
 * @date    2016.12.19
 *
 * @return  終了アニメ
 */
//-----------------------------------------------------------------------------
u32 MenuBase::GetRotomMessageEndAnime(void)
{
  if( m_rotomMsgStartAnime == LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN )
  {
    return LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_OUT;
  }
  return LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_OUT_LEFT;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMessageAnime
 * @brief   ロトムメッセージ開始/終了アニメセット
 * @date    2016.01.08
 *
 * @param   is_enable   true = 開始, false = 終了
 * @param   is_end      true = 最終フレームから開始
 */
//-----------------------------------------------------------------------------
void MenuBase::SetMessageAnime( bool is_enable, bool is_end )
{
  // 全体を停止
  for( u32 i=0; i<ROTOM_MSG_ANM_MAX; i++ )
  {
    StopCoverAnime( RotomMessageAnimeTable[i] );
  }

  u32 play;
  if( is_enable != false )
  {
    play = GetRotomMessageStartAnime();
  }
  else
  {
    play = GetRotomMessageEndAnime();
  }
  StartRotomAnime( play, is_end );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsMessageAnime
 * @brief   ロトムメッセージ開始/終了アニメが再生中か
 * @date    2016.01.08
 *
 * @retval  true  = 再生中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsMessageAnime(void)
{
  for( u32 i=0; i<ROTOM_MSG_ANM_MAX; i++ )
  {
    if( IsCoverAnimeEnd(RotomMessageAnimeTable[i]) == false )
    {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomPowerOnAnime
 * @brief   ロトムの電源ONアニメ再生
 * @date    2016.01.08
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomPowerOnAnime(void)
{
  StartCoverAnime( LA_MAINUPPER_FIELDMENU_RTM_LOW_000_POWER_ON, 0.0f );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomPowerOnAnimeEnd
 * @brief   ロトムの電源ONアニメ終了チェック
 * @date    2016.01.08
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsRotomPowerOnAnimeEnd(void)
{
  return IsCoverAnimeEnd( LA_MAINUPPER_FIELDMENU_RTM_LOW_000_POWER_ON );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomSleepAnime
 * @brief   ロトムのスリープアニメ再生
 * @date    2016.01.16
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomSleepAnime(void)
{
  GetG2DUtil()->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_SLEEP_00 );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomSleepAnimeEnd
 * @brief   ロトムのスリープアニメ終了チェック
 * @date    2016.01.16
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsRotomSleepAnimeEnd(void)
{
  return GetG2DUtil()->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_SLEEP_00 );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePassivePane
 * @brief   パッシブ表示用ペインの表示切り替え
 * @date    2015.06.24
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuBase::SetVisiblePassivePane( bool flg )
{
  GetG2DUtil()->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_BG_BLACK, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartRotomAnime
 * @brief   ロトム関連のアニメを開始
 * @date    2016.01.08
 *
 * @param   anm     アニメ番号
 * @param   is_end  true = 終了フレームから開始
 */
//-----------------------------------------------------------------------------
void MenuBase::StartRotomAnime( u32 anm, bool is_end )
{
  f32 frame = 0.0f;
  if( is_end != false )
  {
    frame = GetCoverAnimeMaxFrame( anm );
  }
  StartCoverAnime( anm, frame );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomAnime
 * @brief   ロトムメッセージ開始アニメ再生
 * @date    2015.09.04
 *
 * @param   is_enable   true = 開始, false = 終了
 * @param   is_end      true = 最終フレームから開始
 *
 * @note  口パク停止、メッセージ処理のシーケンスクリア
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomAnime( bool is_enable, bool is_end )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  SetMessageAnime( is_enable, is_end );

  if( is_enable != false )
  {
    g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_00, false );
    g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_01, false );
    ClearRotomMessageWork();
  }
/*
  if( is_enable == false )
  {
    u32 play, stop;
    GetRotomMessageMoveAnime( &stop, &play );
    g2d->StopAnime( LYTID_LOWER_ROTOM, stop );
    StartRotomAnime( play, is_end );
  }
  else
  {
    u32 play, stop;
    GetRotomMessageMoveAnime( &play, &stop );
    g2d->StopAnime( LYTID_LOWER_ROTOM, stop );
    StartRotomAnime( play, is_end );
    g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_00, false );
    g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_01, false );
    ClearRotomMessageWork();
  }
*/
  g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_MOUTH_LOOP );   // 口パクを止める
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomAnimeEnd
 * @brief   ロトムメッセージ開始/終了アニメが再生中か
 * @date    2016.01.08
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsRotomAnimeEnd(void)
{
  return IsMessageAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomMsgData
 * @brief   ロトムメッセージデータセット
 * @date    2015.09.04
 *
 * @parma   msg   メッセージデータ
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomMsgData( gfl2::str::MsgData * msg )
{
  m_rotomMsgData = msg;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitRotomMessage
 * @brief   ロトムメッセージ初期化
 * @date    2015.09.04
 */
//-----------------------------------------------------------------------------
void MenuBase::InitRotomMessage(void)
{
  m_isRotomUpdateStop = true;
  ClearRotomMessageWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomMessageUpdateEnable
 * @brief   ロトムメッセージ更新制御フラグをセット
 * @date    2016.01.15
 *
 * @param   flg   true = 更新許可, false = 更新停止
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomMessageUpdateEnable( bool flg )
{
  m_isRotomUpdateStop = !flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequestRotomMessage
 * @brief   ロトムメッセージリクエストセット
 * @date    2015.09.04
 *
 * @param   index       ロトムメッセージインデックス（種類）
 * @param   text        テキスト
 * @param   work        ワーク
 * @param   is_rank_on  true = 親密度ランク補正をかける
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRequestRotomMessage( u32 index, u32 text, u32 work, bool is_rank_on )
{
  if( is_rank_on != false &&
      ( index == PlayData::ROTOM_MSG_EVENT ||
        index == PlayData::ROTOM_MSG_POS ||
        index == PlayData::ROTOM_MSG_ETC ) )
  {
    text += m_pRotomFriendship->GetRank();
  }

  if( m_rotomMsgIndex == index )
  {
    m_playData->SetRotomMessage( static_cast<PlayData::RotomMsgIndex>(index), text, work, true );
  }
  else
  {
    m_playData->SetRotomMessage( static_cast<PlayData::RotomMsgIndex>(index), text, work, false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequestRotomMessageFreeTalk
 * @brief   ロトムの会話メッセージリクエストセット
 * @date    2016.12.07
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRequestRotomMessageFreeTalk(void)
{
  static const u32 tbl[] =
  {
    msg_field_rotom_rand_que_00_00,
    msg_field_rotom_rand_que_01_00,
    msg_field_rotom_rand_que_02_00,
    msg_field_rotom_rand_que_03_00,
    msg_field_rotom_rand_que_04_00,
    msg_field_rotom_rand_que_05_00,
    msg_field_rotom_rand_que_06_00,
    msg_field_rotom_rand_que_07_00,
    msg_field_rotom_rand_que_08_00,
    msg_field_rotom_rand_que_09_00,
    msg_field_rotom_rand_que_10_00,
    msg_field_rotom_rand_que_11_00,
    msg_field_rotom_rand_que_12_00,
    msg_field_rotom_rand_que_13_00,
    msg_field_rotom_rand_que_14_00,
    msg_field_rotom_rand_que_15_00,
    msg_field_rotom_rand_que_19_00,
    msg_field_rotom_rand_que_20_00,
    msg_field_rotom_rand_que_21_00,
    msg_field_rotom_rand_que_22_00,
    msg_field_rotom_rand_que_23_00,
    msg_field_rotom_rand_que_24_00,
    msg_field_rotom_rand_que_25_00,
    msg_field_rotom_rand_que_26_00,
    msg_field_rotom_rand_que_27_00,
    msg_field_rotom_rand_que_28_00,
  };

  u32 size = GFL_NELEMS( tbl );
  u32 rand = 0;

  // ニックネームが設定されている
  Savedata::FieldMenu::NicknameType nickname = m_saveFieldMenu->GetNicknameType();
  if( nickname != Savedata::FieldMenu::NICKNAME_TYPE_NONE )
  {
    rand = System::GflUse::GetPublicRand( size+1 );
#if PM_DEBUG
    u32 debug_id = m_playData->DebugGetRotomFreeTalkID();
    if( debug_id != 0 )
    {
      rand = debug_id - 1;
    }
#endif  // PM_DEBUG

    if( rand == size )
    {
      if( nickname == Savedata::FieldMenu::NICKNAME_TYPE_NAME )
      {
        SetRequestRotomMessage(
          PlayData::ROTOM_MSG_FREE,
          msg_field_rotom_rand_que_16_00 + m_pRotomFriendship->GetRank(),
          PlayData::ROTOM_WORK_NONE );
      }
      else if( nickname == Savedata::FieldMenu::NICKNAME_TYPE_DEFAULT )
      {
        SetRequestRotomMessage(
          PlayData::ROTOM_MSG_FREE,
          msg_field_rotom_rand_que_17_00 + m_pRotomFriendship->GetRank(),
          PlayData::ROTOM_WORK_NONE );
      }
      else if( nickname == Savedata::FieldMenu::NICKNAME_TYPE_MASTER )
      {
        SetRequestRotomMessage(
          PlayData::ROTOM_MSG_FREE,
          msg_field_rotom_rand_que_18_00 + m_pRotomFriendship->GetRank(),
          PlayData::ROTOM_WORK_NONE );
      }
      return;
    }
  }
  else
  {
    rand = System::GflUse::GetPublicRand( size );
#if PM_DEBUG
    u32 debug_id = m_playData->DebugGetRotomFreeTalkID();
    if( debug_id != 0 && (debug_id-1) < size )
    {
      rand = debug_id - 1;
    }
#endif  // PM_DEBUG
  }

  SetRequestRotomMessage(
    PlayData::ROTOM_MSG_FREE,
    tbl[rand] + m_pRotomFriendship->GetRank(),
    PlayData::ROTOM_WORK_NONE );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequestRotomMessageRequestTalk
 * @brief   ロトムのお願いメッセージリクエスト
 * @date    2017.02.08
 *
 * @param   req_id  お願いID
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRequestRotomMessageRequestTalk( Savedata::FieldMenu::RotomRequest req_id )
{
  static const u32 tbl[] =
  {
    msg_field_rotom_onegai_01_01,   // ファインダースタジオ
    msg_field_rotom_onegai_02_01,   // マンタインサーフ
    msg_field_rotom_onegai_03_01,   // ルナソルダイブ
    msg_field_rotom_onegai_04_01,   // バトルツリー
    msg_field_rotom_onegai_05_01,   // フェスサークル
    msg_field_rotom_onegai_01_01,   // チュートリアル（ダミー）
    msg_field_rotom_onegai_01_03,   // 進化
  };

  if( req_id == Savedata::FieldMenu::ROTOM_REQ_NONE ||
      req_id == Savedata::FieldMenu::ROTOM_REQ_TUTORIAL ||
      req_id == Savedata::FieldMenu::ROTOM_REQ_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }

  SetRequestRotomMessage( PlayData::ROTOM_MSG_REQUEST, tbl[req_id-1], req_id );
  m_playData->SetRotomRequestEvolution( false );  // 上書きされた場合もOFFでok
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequestRotomMessageEvolution
 * @brief   進化後のロトムメッセージリクエスト
 * @date    2017.04.19
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRequestRotomMessageEvolution(void)
{
  // @note 特殊なお願いなので、状況報告と同じにしておく
  SetRequestRotomMessage( PlayData::ROTOM_MSG_REPORT, msg_field_rotom_onegai_01_03, PlayData::ROTOM_WORK_NONE );
  m_playData->SetRotomRequestEvolution( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequestRotomMessageReport
 * @brief   ロトムの状況報告メッセージリクエストセット
 * @date    2016.12.07
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRequestRotomMessageReport(void)
{
  Savedata::FieldMenu::RotomRank rank = m_pRotomFriendship->GetRank();

  f32 p = static_cast<f32>(m_saveFieldMenu->GetRotomFriendship()) * ROTOM_FS_TALK_START_LOT;
#if PM_DEBUG
  if( m_playData->DebugGetRotomReportFlag() != false )
  {
    p = 1.0f;
  }
#endif

  if( System::GflUse::GetPublicRand(100) < static_cast<u32>(p*100.0f) )
  {
    u32 hp_index = IsRotomReportPokemonHP( rank );
    RotomTimeTalk time_talk = CheckRotomReportAlolaTime( rank );
    bool is_play_time = IsRotomReportPlayTime( rank );
    Savedata::FieldMenu::RotomCharacter chara = m_pRotomFriendship->GetCharacter();

    // 報告なし
    if( hp_index == pml::PokeParty::MAX_MEMBERS && time_talk == ROTOM_TIME_TALK_NOME && is_play_time == false )
    {
      if( chara != Savedata::FieldMenu::ROTOM_CHAR_SHY )
      {
        SetRotomReportCharacter( chara );
      }
    }
    else
    {
      u32 rand_max = 3;
      if( chara != Savedata::FieldMenu::ROTOM_CHAR_SHY )
      {
        rand_max += 1;
      }

      for ever
      {
        u32 random = System::GflUse::GetPublicRand( rand_max );
        if( random == 0 )
        {
          if( hp_index != pml::PokeParty::MAX_MEMBERS )
          {
            SetRotomReportPokemonHP( static_cast<u32>(rank), hp_index );
            break;
          }
        }
        else if( random == 1 )
        {
          if( time_talk != ROTOM_TIME_TALK_NOME )
          {
            SetRotomReportAlolaTime( static_cast<u32>(rank)-1, time_talk );
            break;
          }
        }
        else if( random == 2 )
        {
          if( is_play_time != false )
          {
            SetRotomReportPlayTime( static_cast<u32>(rank)-2 );
            break;
          }
        }
        else
        {
          SetRotomReportCharacter( chara );
          break;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomReportPokemonHP
 * @brief   ロトム状況報告チェック：ポケモンのHP
 * @date    2016.12.12
 *
 * @param   rank  親密度ランク
 *
 * @return  ポケモンの位置
 */
//-----------------------------------------------------------------------------
u32 MenuBase::IsRotomReportPokemonHP( Savedata::FieldMenu::RotomRank rank )
{
  for( u32 i=0; i<m_pokeParty->GetMemberCount(); i++ )
  {
    const pml::pokepara::PokemonParam * pp = m_pokeParty->GetMemberPointerConst( i );
    bool fast = pp->StartFastMode();
    bool is_egg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
    u32 hp  = pp->GetHp();
    u32 mhp = pp->GetMaxHp();
    pp->EndFastMode( fast );
    if( is_egg == false && hp != 0 && hp != mhp )
    {
      // @fix MMCat[297]: DebugとReleaseでf32=>u32のキャストで差が出たので計算を変更
      u32 val = hp * 100 / mhp;
      if( val <= ROTOM_REPORT_HP_RATE )
      {
        return i;
      }
    }
  }

  return pml::PokeParty::MAX_MEMBERS;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckRotomReportAlolaTime
 * @brief   ロトム状況報告チェック：時間帯変化の会話が発生するか
 * @date    2016.12.12
 *
 * @param   rank  親密度ランク
 *
 * @return  会話の時間帯
 */
//-----------------------------------------------------------------------------
MenuBase::RotomTimeTalk MenuBase::CheckRotomReportAlolaTime( Savedata::FieldMenu::RotomRank rank )
{
  if( rank < Savedata::FieldMenu::ROTOM_RANK_2 )
  {
    return ROTOM_TIME_TALK_NOME;
  }

  // @5分で時が変わるか
  if( Field::EvTime::GetAlolaMinute( m_gameData ) < 55 )
  {
    return ROTOM_TIME_TALK_NOME;
  }

  { // 現在の時間帯
    GameSys::TimeZone::Zone now = Field::EvTime::GetAlolaTimeZone( m_gameData );
    // 夜
    if( now == GameSys::TimeZone::NIGHT || now == GameSys::TimeZone::MIDNIGHT )
    {
      s32 next_hour = ( Field::EvTime::GetAlolaHour(m_gameData) + 1 ) % 24;
      GameSys::TimeZone::Zone next_zone = GameSys::TimeZone::ConvertHourToTimeZone( next_hour );
      // 次が昼か @note MMCat[296]関連メモ: EVENINGは昼扱いだけど、夜=>夕はありえないので含めなくても大丈夫。
      if( next_zone == GameSys::TimeZone::MORNING || next_zone == GameSys::TimeZone::NOON )
      {
        return ROTOM_TIME_TALK_NOON;
      }
    }
    // 昼
    else
    {
      s32 next_hour = ( Field::EvTime::GetAlolaHour(m_gameData) + 1 ) % 24;
      GameSys::TimeZone::Zone next_zone = GameSys::TimeZone::ConvertHourToTimeZone( next_hour );
      // 次が夜か
      if( next_zone == GameSys::TimeZone::NIGHT || next_zone == GameSys::TimeZone::MIDNIGHT )
      {
        return ROTOM_TIME_TALK_NIGHT;
      }
    }
  }

  return ROTOM_TIME_TALK_NOME;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomReportPlayTime
 * @brief   ロトム状況報告チェック：時間経過
 * @date    2016.12.12
 *
 * @param   rank  親密度ランク
 *
 * @retval  true  = 一定時間経過
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsRotomReportPlayTime( Savedata::FieldMenu::RotomRank rank )
{
  if( rank < Savedata::FieldMenu::ROTOM_RANK_3 )
  {
    return false;
  }
  return m_playData->IsRotomTalkDiffTime();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomReportPokemonHP
 * @brief   ロトムの状況報告：ポケモンのHP
 * @date    2016.12.07
 *
 * @param   level       親密度の状態
 * @param   poke_index  ポケモンのインデックス
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomReportPokemonHP( u32 level, u32 poke_index )
{
  static const u32 tbl[] =
  {
    msg_field_rotom_jyokyo01_01,
    msg_field_rotom_jyokyo01_02,
    msg_field_rotom_jyokyo01_03,
    msg_field_rotom_jyokyo01_04,
  };

  if( level >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  SetRequestRotomMessage( PlayData::ROTOM_MSG_REPORT, tbl[level], PlayData::ROTOM_WORK_NONE );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomReportAlolaTime
 * @brief   ロトムの状況報告：昼夜
 * @date    2016.12.07
 *
 * @param   level   親密度の状態
 * @param   time    時間帯
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomReportAlolaTime( u32 level, RotomTimeTalk time )
{
  static const u32 tbl[3][2] =
  { // 昼, 夜
    { msg_field_rotom_jyokyo03_02, msg_field_rotom_jyokyo02_02 },
    { msg_field_rotom_jyokyo03_03, msg_field_rotom_jyokyo02_03 },
    { msg_field_rotom_jyokyo03_04, msg_field_rotom_jyokyo02_04 },
  };

  if( time == ROTOM_TIME_TALK_NOME )
  {
    GFL_ASSERT( 0 );
    return;
  }
  if( level >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  SetRequestRotomMessage( PlayData::ROTOM_MSG_REPORT, tbl[level][time], PlayData::ROTOM_WORK_NONE );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomReportPlayTime
 * @brief   ロトムの状況報告：連続プレイ時間
 * @date    2016.12.07
 *
 * @param   level   親密度の状態
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomReportPlayTime( u32 level )
{
  static const u32 tbl[] =
  {
    msg_field_rotom_jyokyo04_03,
    msg_field_rotom_jyokyo04_04,
  };

  if( level >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  SetRequestRotomMessage( PlayData::ROTOM_MSG_REPORT, tbl[level], PlayData::ROTOM_WORK_REPORT );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomReportCharacter
 * @brief   ロトムの状況報告：性格別
 * @date    2016.12.07
 *
 * @param   chara   性格
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomReportCharacter( Savedata::FieldMenu::RotomCharacter chara )
{
  switch( chara )
  {
  case Savedata::FieldMenu::ROTOM_CHAR_SHY:
    GFL_ASSERT( 0 );
    break;

  case Savedata::FieldMenu::ROTOM_CHAR_BELLIGERENT:
    {
      static const u32 tbl[] =
      {
        msg_field_rotom_jyokyo05,
        msg_field_rotom_jyokyo06,
        msg_field_rotom_jyokyo07,
        msg_field_rotom_jyokyo08,
        msg_field_rotom_jyokyo09,
        msg_field_rotom_jyokyo10,
        msg_field_rotom_jyokyo11,
        msg_field_rotom_jyokyo12,
        msg_field_rotom_jyokyo13,
        msg_field_rotom_jyokyo14,
      };
      SetRequestRotomMessage(
        PlayData::ROTOM_MSG_REPORT, tbl[ System::GflUse::GetPublicRand(GFL_NELEMS(tbl)) ], PlayData::ROTOM_WORK_NONE );
    }
    break;

  case Savedata::FieldMenu::ROTOM_CHAR_FRIENDLY:
    {
      static const u32 tbl[] =
      {
        msg_field_rotom_jyokyo25,
        msg_field_rotom_jyokyo26,
        msg_field_rotom_jyokyo27,
        msg_field_rotom_jyokyo28,
        msg_field_rotom_jyokyo29,
        msg_field_rotom_jyokyo30,
        msg_field_rotom_jyokyo31,
        msg_field_rotom_jyokyo32,
        msg_field_rotom_jyokyo33,
        msg_field_rotom_jyokyo34,
      };
      SetRequestRotomMessage(
        PlayData::ROTOM_MSG_REPORT, tbl[ System::GflUse::GetPublicRand(GFL_NELEMS(tbl)) ], PlayData::ROTOM_WORK_NONE );
    }
    break;

  case Savedata::FieldMenu::ROTOM_CHAR_OPTIMISTIC:
    {
      static const u32 tbl[] =
      {
        msg_field_rotom_jyokyo15,
        msg_field_rotom_jyokyo16,
        msg_field_rotom_jyokyo17,
        msg_field_rotom_jyokyo18,
        msg_field_rotom_jyokyo19,
        msg_field_rotom_jyokyo20,
        msg_field_rotom_jyokyo21,
        msg_field_rotom_jyokyo22,
        msg_field_rotom_jyokyo23,
        msg_field_rotom_jyokyo24,
      };
      SetRequestRotomMessage(
        PlayData::ROTOM_MSG_REPORT, tbl[ System::GflUse::GetPublicRand(GFL_NELEMS(tbl)) ], PlayData::ROTOM_WORK_NONE );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ClearRotomMessageWork
 * @brief   ロトムメッセージワーククリア
 * @date    2015.09.04
 */
//-----------------------------------------------------------------------------
void MenuBase::ClearRotomMessageWork(void)
{
  m_rotomMsgSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotomMessage
 * @brief   ロトムメッセージ更新
 * @date    2015.09.04
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotomMessage(void)
{
  if( m_isRotomUpdateStop != false )
  {
    return;
  }

  switch( m_rotomMsgSeq )
  {
  case 0:
    if( PrintRotomMessage() != false )
    {
      m_rotomMsgSeq++;
    }
    break;

  case 1:
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      gfl2::str::StrWin::Result result = g2d->PrintMessage();
      if( result != gfl2::str::StrWin::RES_FINISH && m_touchPanel->IsTouch() != false )
      {
        result = g2d->PrintMessage();
      }
      if( result == gfl2::str::StrWin::RES_FINISH )
      {
        // @fix GFMMCat[237]関連対処: 下画面の入力が禁止されている場合、イベントメッセージを終了させないようにする
        // @fix MMCat[787]: イベント以外のメッセージもセーブデータを変更しているため全て終了しないようにする
        if( *m_isPassive != false )
        {
          // @fix GFMMCat[1078]: 上画面がメッセージ終了を待っているため、スクリプトは抜けられるようにする
          if( m_rotomMsgIndex != PlayData::ROTOM_MSG_SCRIPT )
          {
            break;
          }
        }

        // 分岐があるか
        if( m_rotomQuestion != NULL )
        {
          g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_MOUTH_LOOP );   // 口パクを止める
          SetRotomSelectMenu();
          m_rotomMenuSelect = ROTOM_SELECT_NONE;
          m_rotomMsgSeq = 2;
          break;
        }

        VanishRotomSelectButton();

        AfterPrintRotomMessage();
        m_rotomMsgIndex = PlayData::ROTOM_MSG_MAX;

        m_isEventUpdate = false;
      }
    }
    break;

  case 2:
    if( m_rotomMenuSelect != ROTOM_SELECT_NONE )
    {
      u16 select = m_rotomQuestion->result1;
      s8 param = m_rotomQuestion->result1_prm;
      if( m_rotomMenuSelect != 0 )
      {
        select = m_rotomQuestion->result2;
        param = m_rotomQuestion->result2_prm;
      }

      m_isEventUpdate = UpdateMainEventWork( m_rotomQuestion->start_wk_value, m_rotomQuestion->next_wk_value );

      if( param > 0 )
      {
        m_pRotomFriendship->Add( param, false );
      }

      if( SetRotomSelectMessageFunc( m_rotomQuestion->label, m_rotomMenuSelect ) == false )
      {
        // 終了
        if( select == ROTOM_QUESTION_EX_LABEL_MESSAGE_END )
        {
          VanishRotomSelectButton();

          AfterPrintRotomMessage();
          m_rotomMsgIndex = PlayData::ROTOM_MSG_MAX;

          m_isEventUpdate = false;
        }
        // メッセージ表示
        else
        {
          if( m_rotomMenuSelect == 0 )
          {
            SetVisibleRotomSelectButton( BTNID_ROTOM_SELECT_01, ROTOM_SELECT_VIEW_ENABLE );
            SetVisibleRotomSelectButton( BTNID_ROTOM_SELECT_02, ROTOM_SELECT_BUTTON_OFF );
          }
          else
          {
            SetVisibleRotomSelectButton( BTNID_ROTOM_SELECT_02, ROTOM_SELECT_VIEW_ENABLE );
            SetVisibleRotomSelectButton( BTNID_ROTOM_SELECT_01, ROTOM_SELECT_BUTTON_OFF );
          }

          // @note タイプは引き継ぐ
          if( m_rotomMsgIndex == PlayData::ROTOM_MSG_MAX )
          {
            GFL_ASSERT( 0 );
            m_rotomMsgIndex = PlayData::ROTOM_MSG_ETC;
          }
          SetRequestRotomMessage( m_rotomMsgIndex, select, m_playData->GetRotomMessageWork(m_rotomMsgIndex), false );
          CheckSelectRotomMessage( select );

          AfterPrintRotomMessage();

          if( param > 0 )
          {
            GetG2DUtil()->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_HEART_01 );
          }
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    AfterPrintRotomMessage
 * @brief   ロトムメッセージ表示終了後の処理
 * @date    2016.12.06
 */
//-----------------------------------------------------------------------------
void MenuBase::AfterPrintRotomMessage(void)
{
  if( m_rotomMsgIndex == PlayData::ROTOM_MSG_EVENT )
  {
    m_saveMisc->SetEventFirstCheck( m_playData->GetRotomMessageWork(m_rotomMsgIndex) );   // イベントのメッセージを既読にする
    m_playData->SetEndContinueRotomMessage();   // 初回起動フラグをセット
  }
  else if( m_rotomMsgIndex == PlayData::ROTOM_MSG_REQUEST )
  {
    m_saveFieldMenu->SetRotomRequest( static_cast<Savedata::FieldMenu::RotomRequest>(m_playData->GetRotomMessageWork(m_rotomMsgIndex)) );
    m_playData->SetEndContinueRotomMessage();   // 初回起動フラグをセット
  }
  else if( m_rotomMsgIndex == PlayData::ROTOM_MSG_REPORT )
  {
    if( m_playData->GetRotomMessageWork(m_rotomMsgIndex) == PlayData::ROTOM_WORK_REPORT )
    {
      m_playData->SetRotomReportTalkEnable( false );
      m_playData->ResetRotomTalkDiffTime();
    }
    m_playData->SetEndContinueRotomMessage();   // 初回起動フラグをセット
  }
  else if( m_rotomMsgIndex == PlayData::ROTOM_MSG_ETC || m_rotomMsgIndex == PlayData::ROTOM_MSG_FREE )
  {
    m_playData->SetEndContinueRotomMessage();   // 初回起動フラグをセット
  }

  // 表示中に同タイプのメッセージが更新されたか
  if( m_playData->GetRotomMessageIsUpdate( m_rotomMsgIndex ) == false )
  {
    m_playData->InitRotomMessage( m_rotomMsgIndex );
  }
  else
  {
    m_playData->ResetRotomMessageIsUpdate( m_rotomMsgIndex );
  }

  if( m_playData->GetEnableRottomMsgIndex() == PlayData::ROTOM_MSG_MAX || m_rotomMsgIndex == PlayData::ROTOM_MSG_SCRIPT )
  {
    m_frameListener->OnAction( FrameListener::ACTION_ROTOM_CLOSE );
  }
  GetG2DUtil()->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_MOUTH_LOOP );   // 口パクを止める
//  m_rotomMsgIndex = PlayData::ROTOM_MSG_MAX;  // @note 続きがある場合があるため外で個別で入れる
  m_rotomMsgSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintRotomMessage
 * @brief   ロトムメッセージ表示
 * @date    2015.09.04
 *
 * @retval  true  = 表示開始
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::PrintRotomMessage(void)
{
  if( m_saveFieldMenu->IsVisibleRotom() == false )
  {
    return false;
  }

  if( m_rotomMsgData == NULL )
  {
    return false;
  }

  // @note 続きがあるため、設定されている場合は同じタイプを再表示
  if( m_rotomMsgIndex == PlayData::ROTOM_MSG_MAX )
  {
    m_rotomMsgIndex = m_playData->GetEnableRottomMsgIndex();
  }

  if( m_rotomMsgIndex == PlayData::ROTOM_MSG_MAX )
  {
    m_frameListener->OnAction( FrameListener::ACTION_ROTOM_CLOSE );   // 開いてるけどメッセージなし
    return false;
  }

  u32 label = m_playData->GetRotomMessageText(m_rotomMsgIndex);
  StartRotomMessage( label );

  app::util::G2DUtil * g2d = GetG2DUtil();
  print::MessageWindow * win = g2d->GetMsgWin();
  win->SetMessageSpeed( Savedata::ConfigSave::MSG_SLOW );
  win->SetMessageAutoFlg( true );
  if( CheckSelectRotomMessage( label ) == false )
  {
    win->SetFinishMode( gfl2::str::StrWin::FINISH_TIMER, ROTOM_MSG_FINISH_FRAME );
  }
  else
  {
    win->SetFinishMode( gfl2::str::StrWin::FINISH_TIMER, 0 );
  }

  // ▼を表示させないようにする
  g2d->SetMsgCursorUserVisible( true, false );

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartRotomMessage
 * @brief   ロトムメッセージ開始
 * @date    2016.12.12
 */
//-----------------------------------------------------------------------------
void MenuBase::StartRotomMessage( u32 id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  m_rotomMsgData->GetString( id, *m_pStrBuf );
  m_pWordSet->RegisterPlayerName( 0, m_myStatus );
  m_pWordSet->RegisterPlayerNickName( 1, m_myStatus, m_saveFieldMenu );
  m_pWordSet->RegisterPlayerNickNamePrefix( 2, m_myStatus );
  { // 単語：マスター
    gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );
    u32 text = msg_field_rotom_nickname_02;
    print::WordSet::Grammer grammer = print::WordSet::GRAMMER_MALE;
    if( m_myStatus->GetSex() == PM_FEMALE )
    {
      text = msg_field_rotom_nickname_03;
      grammer = print::WordSet::GRAMMER_FEMALE;
    }
    m_rotomMsgData->GetString( text, *str );
    m_pWordSet->RegisterWord( 3, *str, grammer );
  }
  m_pWordSet->Expand( m_pExpBuf, m_pStrBuf );

  g2d->GetMsgWin()->ProcAtOnce();   // @fix MMCat[710]: メッセージ処理を最後まで進める
  g2d->SetMessage( m_pExpBuf );

  g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_00, true );
  g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_01, true );

  g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_MOUTH_LOOP );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateRotomMsgWindow
 * @brief   ロトムメッセージウィンドウ生成
 * @date    2015.11.11
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void MenuBase::CreateRotomMsgWindow( app::util::Heap * heap )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_ROTOM );

  g2d->CreateMessageWindow(
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
    LYTID_LOWER_ROTOM,
    lytwk->GetTextBoxPane( PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_00 ),
    lytwk->GetTextBoxPane( PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_01 ),
    heap );
//  g2d->SetMessageCallback( this );

/*
  print::MessageWindow * win = g2d->GetMsgWin();
  win->SetMessageSpeed( Savedata::ConfigSave::MSG_SLOW );
  win->SetMessageAutoFlg( true );
	win->SetFinishMode( gfl2::str::StrWin::FINISH_TIMER, ROTOM_MSG_FINISH_FRAME );
*/

  g2d->CreateMsgCursor( heap, LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_CURSOR_00 );

//  g2d->GetMsgWin()->SetEnableCtrl( false, true );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRotomMsgWindow
 * @brief   ロトムメッセージウィンドウ削除
 * @date    2015.11.11
 */
//-----------------------------------------------------------------------------
void MenuBase::DeleteRotomMsgWindow(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  if( g2d != NULL )
  {
    g2d->DeleteMessageWindow();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    EndPrintRotomMessage
 * @brief   ロトムメッセージ表示終了
 * @date    2015.11.14
 */
//-----------------------------------------------------------------------------
void MenuBase::EndPrintRotomMessage(void)
{
  if( m_rotomMsgSeq != 0 )
  {
    for ever
    {
      if( GetG2DUtil()->PrintMessage() == gfl2::str::StrWin::RES_FINISH )
      {
        break;
      }
    }
    m_rotomMsgSeq = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRotomMessage
 * @brief   特定のロトムメッセージを破棄
 * @date    2017.04.28
 *
 * @note  @fix MMCat[310]: 画面遷移時に特定のメッセージを消す
 */
//-----------------------------------------------------------------------------
void MenuBase::DeleteRotomMessage(void)
{
  static const PlayData::RotomMsgIndex tbl[] =
  {
    PlayData::ROTOM_MSG_ETC,      // ゲームクリア後のイベント用
    PlayData::ROTOM_MSG_REPORT,   // ロトム状況報告＋進化お願い
    PlayData::ROTOM_MSG_FREE,     // ロトム会話
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    m_playData->InitRotomMessage( tbl[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckSelectRotomMessage
 * @brief   ロトムメッセージが選択肢ありかをチェック
 * @date    2016.12.02
 *
 * @param   msg_index   メッセージインデックス
 *
 * @retval  true  = 選択肢あり
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::CheckSelectRotomMessage( u32 msg_index )
{
  for( u32 i=0; i<GFL_NELEMS(RotomQuestionData); i++ )
  {
    if( RotomQuestionData[i].label == msg_index )
    {
      m_rotomQuestion = &RotomQuestionData[i];
      return true;
    }
  }
  m_rotomQuestion = NULL;
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckAnswerRotomMessage
 * @brief   ロトムメッセージが質問の答えかをチェック
 * @date    2017.05.10
 *
 * @param   msg_index   メッセージインデックス
 *
 * @retval  true  = 質問の答え
 * @retval  false = それ以外
 *
 * @fix MMCat[333]
 */
//-----------------------------------------------------------------------------
bool MenuBase::CheckAnswerRotomMessage( u32 msg_index )
{
  for( u32 i=0; i<GFL_NELEMS(RotomQuestionData); i++ )
  {
    if( RotomQuestionData[i].result1 == msg_index ||
        RotomQuestionData[i].result2 == msg_index )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleRotomSelectButton
 * @brief   ロトム選択肢表示設定
 * @date    2016.12.05
 *
 * @param   id    項目ID
 * @param   flg   設定フラグ
 */
//-----------------------------------------------------------------------------
void MenuBase::SetVisibleRotomSelectButton( u32 id, RotomSelectVisible flg )
{
  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( flg & ROTOM_SELECT_VIEW_ENABLE )
  {
    man->SetButtonVisible( id );
  }
  else
  {
    man->SetButtonInvisible( id );
  }

  if( flg & ROTOM_SELECT_BUTTON_ENABLE )
  {
    if( man->IsButtonVisible( id ) == false )
    {
      GFL_ASSERT( 0 );
      man->SetButtonPassive( id, false );
      return;
    }
    man->SetButtonActive( id, false );
  }
  else
  {
    man->SetButtonPassive( id, false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    VanishRotomSelectButton
 * @brief   ロトム選択肢非表示
 * @date    2017.04.18
 */
//-----------------------------------------------------------------------------
void MenuBase::VanishRotomSelectButton(void)
{
  SetVisibleRotomSelectButton( BTNID_ROTOM_SELECT_01, ROTOM_SELECT_BUTTON_OFF );
  SetVisibleRotomSelectButton( BTNID_ROTOM_SELECT_02, ROTOM_SELECT_BUTTON_OFF );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomMenuString
 * @brief   ロトム選択肢文字列書き込み
 * @date    2016.12.05
 *
 * @param   id    項目ID
 * @param   str   文字列ID
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomMenuString( u32 id, u32 str )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_ROTOM );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  m_rotomMsgData->GetString( str, *m_pStrBuf );

  g2d->SetTextBoxPaneString(
    lytwk->GetTextBoxPane( lytwk->GetPartsPane(RotomSelectMenuPartsTable[id]), PANENAME_FIELDMENU_BTN_LOW_018_PANE_TEXTBOX_00, &res_id ),
    m_pStrBuf );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomSelectMenu
 * @brief   ロトム選択肢セット
 * @date    2016.12.05
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomSelectMenu(void)
{
  SetRotomMenuString( 0, m_rotomQuestion->select1 );
  SetRotomMenuString( 1, m_rotomQuestion->select2 );

  SetVisibleRotomSelectButton( BTNID_ROTOM_SELECT_01, ROTOM_SELECT_BUTTON_ON );
  SetVisibleRotomSelectButton( BTNID_ROTOM_SELECT_02, ROTOM_SELECT_BUTTON_ON );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMainEventWork
 * @brief   メインイベントを進める
 * @date    2017.01.31
 *
 * @param   now_value   現在のイベントワーク値
 * @param   next_value  新たに設定するイベントワーク値
 *
 * @retval  true  = 進行した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::UpdateMainEventWork( u16 now_value, u16 next_value )
{
  // ワーク値が設定されていない
  if( now_value == ROTOM_QUESTION_WORK_VALUE_NULL || next_value == ROTOM_QUESTION_WORK_VALUE_NULL )
  {
    return false;
  }

  // 現在のワーク値が変更されている
  if( m_eventWork->GetEventWork( WK_EVLIST_MAIN_001 ) != now_value )
  {
    return false;
  }

  m_eventWork->SetEventWork( WK_EVLIST_MAIN_001, next_value );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomMainEventUpdate
 * @brief   ロトムメッセージがメインイベントを更新したか
 * @date    2017.02.14
 *
 * @retval  true  = 更新した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsRotomMainEventUpdate(void)
{
  return m_isEventUpdate;
}

//-----------------------------------------------------------------------------
/**
 * @func    USetRotomSelectMessageFunc
 * @brief   ロトム分岐メッセージ特殊処理セット
 * @date    2017.02.10
 *
 * @param   msg     元メッセージ
 * @param   select  選択された項目番号
 *
 * @retval  true  = 別画面を呼び出す
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::SetRotomSelectMessageFunc( u32 msg, u32 select )
{
  for( u32 i=0; i<GFL_NELEMS(RotomQuestionFuncData); i++ )
  {
    if( RotomQuestionFuncData[i].label != msg )
    {
      continue;
    }

    u8 func = RotomQuestionFuncData[i].func1;
    if( select != 0 )
    {
      func = RotomQuestionFuncData[i].func2;
    }

    switch( func )
    {
    case ROTOM_QUESTION_FUNC_LABEL_CALL_BAG:  // バッグ画面へ
      VanishRotomSelectButton();
      AfterPrintRotomMessage();
      m_rotomMsgIndex = PlayData::ROTOM_MSG_MAX;
      m_frameListener->OnAction( FrameListener::ACTION_BAG );
      return true;

    case ROTOM_QUESTION_FUNC_LABEL_CALL_REPORT:  // レポート画面へ
      VanishRotomSelectButton();
      AfterPrintRotomMessage();
      m_rotomMsgIndex = PlayData::ROTOM_MSG_MAX;
      m_frameListener->OnAction( FrameListener::ACTION_REPORT );
      return true;

    case ROTOM_QUESTION_FUNC_LABEL_CALL_FINDER_STUDIO:  // ファインダースタジオ画面へ
      VanishRotomSelectButton();
      AfterPrintRotomMessage();
      m_rotomMsgIndex = PlayData::ROTOM_MSG_MAX;
      m_frameListener->OnAction( FrameListener::ACTION_CALL_FINDER_STUDIO );
      return true;

    case ROTOM_QUESTION_FUNC_LABEL_NICKNAME_RESET:    // ニックネームを名前へ変更
      {
        gfl2::str::StrBuf * str = GetG2DUtil()->GetTempStrBuf( 0 );
        m_myStatus->GetNameString( str );
        m_saveFieldMenu->SetNickNameString( str );
        m_saveFieldMenu->SetNicknameType( Savedata::FieldMenu::NICKNAME_TYPE_NAME );
      }
      return false;

    case ROTOM_QUESTION_FUNC_LABEL_NICKNAME_SET:      // ニックネームを○○ぽんへ変更
      {
        app::util::G2DUtil * g2d = GetG2DUtil();
        gfl2::str::StrBuf * tmp = g2d->GetTempStrBuf( 0 );
        gfl2::str::StrBuf * exp = g2d->GetTempStrBuf( 1 );
        m_rotomMsgData->GetString( msg_field_rotom_nickname_01, *tmp );
        m_pWordSet->RegisterPlayerNickNamePrefix( 2, m_myStatus );
        m_pWordSet->Expand( exp, tmp );
        m_saveFieldMenu->SetNickNameString( exp );
        m_saveFieldMenu->SetNicknameType( Savedata::FieldMenu::NICKNAME_TYPE_DEFAULT );
      }
      return false;

    case ROTOM_QUESTION_FUNC_LABEL_NICKNAME_MASTER:   // ニックネームをマスターへ変更
      {
        gfl2::str::StrBuf * str = GetG2DUtil()->GetTempStrBuf( 0 );
        if( m_myStatus->GetSex() == PM_MALE )
        {
          m_rotomMsgData->GetString( msg_field_rotom_nickname_02, *str );
        }
        else
        {
          m_rotomMsgData->GetString( msg_field_rotom_nickname_03, *str );
        }
        m_saveFieldMenu->SetNickNameString( str );
        m_saveFieldMenu->SetNicknameType( Savedata::FieldMenu::NICKNAME_TYPE_MASTER );
      }
      return false;

    default:
      return false;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEnableButton
 * @brief   ボタン入力制御切り替え
 * @date    2016.01.16
 *
 * @param   flg   true = 入力可, false = 入力不可
 */
//-----------------------------------------------------------------------------
void MenuBase::SetEnableButton( bool flg )
{
  // ロトムアニメ再生中は無効
  if( IsRotomMotionAnime() != false )
  {
    return;
  }

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  // @note ロトム選択肢は操作しない
//  for( u32 i=0; i<BTNID_MAX; i++ )
  for( u32 i=0; i<=BTNID_MOTION_06; i++ )
  {
    // 非表示中は無効
    if( man->IsButtonVisible( i ) == false )
    {
      continue;
    }

    if( flg == false )
    {
      if( man->IsButtonActive( i ) != false )
      {
        man->SetButtonPassive( i, false );
      }
    }
    else
    {
      if( IsRotomAnimeEnd() != false )
      {
        if( man->IsButtonActive( i ) == false )
        {
          man->SetButtonActive( i, false );
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleButton
 * @brief   ボタン表示切り替え
 * @date    2016.01.16
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuBase::SetVisibleButton( bool flg )
{
  // ロトムアニメ再生中は無効
  if( IsRotomMotionAnime() != false )
  {
    return;
  }

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( flg == false )
  {
    // @note ロトム選択肢を操作すると、ロトムアウト時に選択肢が表示されてしまう
//    for( u32 i=0; i<BTNID_MAX; i++ )
    for( u32 i=0; i<=BTNID_MOTION_06; i++ )
    {
      man->SetButtonInvisible( i );
    }
  }
  else
  {
    if( *m_dispMode == DISP_MODE_NORMAL )
    {
      // @note ロトム選択肢を操作すると、ロトムアウト時に選択肢が表示されてしまう
//      for( u32 i=0; i<BTNID_MAX; i++ )
      for( u32 i=0; i<=BTNID_MOTION_06; i++ )
      {
        man->SetButtonVisible( i );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMapCenterPane
 * @brief   マップ中央のペインを取得
 * @date    2016.01.12
 *
 * @return  マップ中央のペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPane * MenuBase::GetMapCenterPane(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_ROTOM );
  return lytwk->GetPane( PANENAME_FIELDMENU_RTM_LOW_000_PANE_MAPCENTER_00 );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartRotomMotion
 * @brief   ロトムモーション開始
 * @date    2016.01.16
 *
 * @param   id  モーションID
 */
//-----------------------------------------------------------------------------
void MenuBase::StartRotomMotion( u32 id )
{
#if PM_DEBUG
  if( m_playData->DebugIsRotomAnimeLoop() == 0 && m_playData->RotomTouch( id ) == false )
#else
  if( m_playData->RotomTouch( id ) == false )
#endif  // PM_DEBUG
  {
    m_rotomAnime = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ANGER;
  }
  else
  {
    const MenuBase::ROTOM_ANIME_PARTS * parts;
    const MenuBase::ROTOM_ANIME_RANK * rank_anime;

    Savedata::FieldMenu::RotomRank rank = m_pRotomFriendship->GetRank();
    if( rank != Savedata::FieldMenu::ROTOM_RANK_4 )
    {
      parts = RotomAnime_RankTable[rank]->parts[id];
    }
    else
    {
      Savedata::FieldMenu::RotomCharacter character = m_pRotomFriendship->GetCharacter();
      parts = RotomAnime_RankTable[rank+character]->parts[id];
    }

#if PM_DEBUG
    if( m_playData->DebugIsRotomAnimeLoop() != 0 )
    {
      if( m_debugRotomTouchPos == (id+1) )
      {
        m_debugRotomTouchCount++;
        if( m_debugRotomTouchCount >= parts->max )
        {
          m_debugRotomTouchCount = 0;
        }
      }
      else
      {
        m_debugRotomTouchPos = id + 1;
        m_debugRotomTouchCount = 0;
      }
      m_rotomAnime = parts->anime[ m_debugRotomTouchCount ];
    }
    else
    {
      m_rotomAnime = parts->anime[ System::GflUse::GetPublicRand(parts->max) ];
    }
#else
    m_rotomAnime = parts->anime[ System::GflUse::GetPublicRand(parts->max) ];
#endif  // PM_DEBUG
  }
  m_rotomMotionSeq = 1;

  { // @fix GFNMCat[3678]: アニメの再生前に押せてしまうのでペインを非表示にする。アニメで自動復帰。
    app::util::G2DUtil * g2d = GetG2DUtil();
    for( u32 i=0; i<=BTNID_MOTION_06; i++ )
    {
      g2d->SetPaneVisible( LYTID_LOWER_ROTOM, ButtonBoundTable[i], false );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotomMotion
 * @brief   ロトムモーション更新
 * @date    2016.01.20
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotomMotion(void)
{
  switch( m_rotomMotionSeq )
  {
  case 0:
    break;

  case 1:
    GetG2DUtil()->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_REACTION_START );
    m_rotomMotionSeq++;
    /* FALL THROUGH */
  case 2:
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      if( g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_REACTION_START ) == false )
      {
        break;
      }
      Savedata::IncRecord( Savedata::Record::RECID_ROTOM );   // レコード @note 連打対策でタッチが無効になってからにしておく
      g2d->StartAnime( LYTID_LOWER_ROTOM, m_rotomAnime );
    }
    m_rotomMotionSeq++;
    /* FALL THROUGH */
  case 3:
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      if( g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, m_rotomAnime ) == false )
      {
        break;
      }
      if( m_isRotomFriendshipUp != false )
      {
        g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_HEART_00 );
        m_isRotomFriendshipUp = false;
        m_rotomMotionSeq = 4;
      }
      else
      {
        g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_REACTION_END );
        m_rotomMotionSeq = 5;
      }
    }
    break;

  case 4:
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      if( g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_HEART_00 ) == false )
      {
        break;
      }
      g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_REACTION_END );
    }
    m_rotomMotionSeq++;
    /* FALL THROUGH */
  case 5:
    if( GetG2DUtil()->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_REACTION_END ) == false )
    {
      break;
    }
    m_rotomMotionSeq = 0;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomMotionAnime
 * @brief   ロトムモーション中かをチェック
 * @date    2016.01.16
 *
 * @retval  true  = モーション中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsRotomMotionAnime(void)
{
  if( m_rotomMotionSeq != 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetUpperLayout
 * @brief   上画面のレイアウトを取得
 * @date    2016.03.28
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytWk * MenuBase::GetUpperLayout(void)
{
  return GetG2DUtil()->GetLayoutWork( LYTID_UPPER );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMultiResID
 * @brief   マルチリソースIDを取得
 * @date    2016.03.28
 *
 * @return  マルチリソースID
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytMultiResID MenuBase::GetMultiResID(void)
{
  gfl2::lyt::LytMultiResID id = GetG2DUtil()->GetLayoutResourceID( 0 );
  return id;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateRotomFriendship
 * @brief   ロトム親密度操作クラス生成
 * @date    2016.11.28
 */
//-----------------------------------------------------------------------------
void MenuBase::CreateRotomFriendship(void)
{
  m_pRotomFriendship = GFL_NEW(m_heap->GetSystemHeap()) RotomFriendship();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRotomFriendship
 * @brief   ロトム親密度操作クラス削除
 * @date    2016.11.28
 */
//-----------------------------------------------------------------------------
void MenuBase::DeleteRotomFriendship(void)
{
  GFL_SAFE_DELETE( m_pRotomFriendship );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartRotopon
 * @brief   ロトポン開始
 * @date    2016.12.08
 */
//-----------------------------------------------------------------------------
void MenuBase::StartRotopon(void)
{
  SetEnableButton( false );   // ボタン無効化
  m_rotoponSeq = ROTOPON_SEQID_INIT;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsUpdateRotopon
 * @brief   ロトポン中か
 * @date    2016.12.09
 *
 * @retval  true  = ロトポン中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::IsUpdateRotopon(void)
{
  if( m_rotoponSeq != ROTOPON_SEQID_NONE )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon
 * @brief   ロトポン用アップデート処理
 * @date    2016.12.09
 *
 * @retval  true  = ロトポン中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuBase::UpdateRotopon(void)
{
  switch( m_rotoponSeq )
  {
  case ROTOPON_SEQID_NONE:
    UpdateRotopon_None();
    m_isRotoponInputTrg = false;
    return false;

  case ROTOPON_SEQID_INIT:
    UpdateRotopon_Init();
    break;

  case ROTOPON_SEQID_INIT_WAIT:
    UpdateRotopon_InitWait();
    break;

  case ROTOPON_SEQID_START_MESSAGE:
    UpdateRotopon_StartMessage();
    break;

  case ROTOPON_SEQID_START_REEL:
    UpdateRotopon_StartReel();
    break;

  case ROTOPON_SEQID_MAIN:
    UpdateRotopon_Main();
    break;

  case ROTOPON_SEQID_STOP:
    UpdateRotopon_Stop();
    break;

  case ROTOPON_SEQID_PRESENT_MESSAGE:
    UpdateRotopon_PresentMessage();
    break;

  case ROTOPON_SEQID_ITEM_SCRIPT_WAIT:
    UpdateRotopon_ItemScriptWait();
    break;

  case ROTOPON_SEQID_END_MESSAGE:
    UpdateRotopon_EndMessage();
    break;

  case ROTOPON_SEQID_END:
    UpdateRotopon_End();
    break;
  }

  m_isRotoponInputTrg = false;

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_None
 * @brief   ロトポン用アップデート：待機中
 * @date    2016.12.12
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_None(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  // モーション中
  if( IsRotomMotionAnime() != false )
  {
    if( g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_00 ) == false )
    {
      g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_00 );
      g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_05 );
    }
  }
  else
  {
    if( m_pRotomFriendship->IsStartRotopon() != false &&
        g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_00 ) != false )
    {
      g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_00 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_Init
 * @brief   ロトポン用アップデート：初期化
 * @date    2016.12.09
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_Init(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  SetVisibleLayout( true );
  g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_00, false );
  g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_01, false );
  g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_00 );
  g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_05 );
  g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN );
  g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000__CURSOR_00_KEEP );
  m_rotoponSeq = ROTOPON_SEQID_INIT_WAIT;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_InitWait
 * @brief   ロトポン用アップデート：初期化待ち
 * @date    2016.12.09
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_InitWait(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_IN ) != false )
  {
    static const u32 tbl[] =
    {
      msg_field_rotom_rotopon_03,       // なし
      msg_field_rotom_onegai_01_02,     // ファインダースタジオ
      msg_field_rotom_onegai_02_02,     // マンタインサーフ
      msg_field_rotom_onegai_03_02,     // ルナソルダイブ
      msg_field_rotom_onegai_04_02,     // バトルツリー
      msg_field_rotom_onegai_05_02,     // フェスサークル
      msg_field_rotom_rotopon_01,       // チュートリアル
      msg_field_rotom_onegai_01_03_RP,  // 進化
    };

    Savedata::FieldMenu::RotomRequest req = m_saveFieldMenu->GetRotomRequest();
    if( req >= Savedata::FieldMenu::ROTOM_REQ_MAX )
    {
      GFL_ASSERT( 0 );
      req = Savedata::FieldMenu::ROTOM_REQ_NONE;
    }
    // お題をクリアしていない場合
    if( req != Savedata::FieldMenu::ROTOM_REQ_NONE && m_saveFieldMenu->IsRotomRequestClearFlag() == false )
    {
      req = Savedata::FieldMenu::ROTOM_REQ_NONE;
    }
    StartRotomMessage( tbl[req] );

    print::MessageWindow * win = g2d->GetMsgWin();
    win->SetMessageSpeed( m_configSave->GetTalkingSpeed() );
    win->SetMessageAutoFlg( false );
    win->SetFinishMode( gfl2::str::StrWin::FINISH_USER, 0 );

    // ▼を表示できるようにする
    g2d->SetMsgCursorUserVisible( false, false );

    m_rotoponSeq = ROTOPON_SEQID_START_MESSAGE;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_StartMessage
 * @brief   ロトポン用アップデート：開始メッセージ処理
 * @date    2016.12.12
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_StartMessage(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrWin::Result result = g2d->PrintMessage();
  if( result != gfl2::str::StrWin::RES_FINISH && m_touchPanel->IsTouch() != false )
  {
    result = g2d->PrintMessage();
  }

  if( result == gfl2::str::StrWin::RES_FINISH )
  {
    g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_01 );
    Sound::PlaySE( SEQ_SE_FLD_0432 );
    m_rotoponSeq = ROTOPON_SEQID_START_REEL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_StartReel
 * @brief   ロトポン用アップデート：回転開始
 * @date    2016.12.12
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_StartReel(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  f32 anm01_frame = g2d->GetAnimeFrame( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_01 );

  if( anm01_frame == 42.0f )
  {
    Sound::PlaySE( SEQ_SE_FLD_0433 );
  }
  else if( anm01_frame == 56.0f )
  {
    Sound::PlaySE( SEQ_SE_FLD_0434 );
  }

  if( g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_01 ) != false )
  {
    g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_02 );
    m_rotoponSeq = ROTOPON_SEQID_MAIN;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_Main
 * @brief   ロトポン用アップデート：メイン
 * @date    2016.12.09
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_Main(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( m_isRotoponInputTrg != false )
  {
//    SetRotoponItem();   // 道具決定
    g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_02 );
    g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_03 );
    g2d->StartAnimeFrame( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_04, static_cast<f32>(m_rotoponItem->type), false, false );
    g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_00, false );
    g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_01, false );
    Sound::StopSE( SEQ_SE_FLD_0434 );
    Sound::PlaySE( SEQ_SE_FLD_0435 );
    m_rotoponSeq = ROTOPON_SEQID_STOP;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_Stop
 * @brief   ロトポン用アップデート：停止
 * @date    2016.12.12
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_Stop(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  f32 anm03_frame = g2d->GetAnimeFrame( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_03 );

  if( anm03_frame == 72.0f )
  {
    Sound::PlaySE( SEQ_SE_FLD_0436 );
  }

  if( g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOPON_03 ) != false )
  {
    StartRotomMessage( msg_field_rotom_rotopon_02 );
    m_rotoponSeq = ROTOPON_SEQID_PRESENT_MESSAGE;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_PresentMessage
 * @brief   ロトポン用アップデート：プレゼント用メッセージ処理
 * @date    2016.12.12
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_PresentMessage(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrWin::Result result = g2d->PrintMessage();
  if( result != gfl2::str::StrWin::RES_FINISH && m_touchPanel->IsTouch() != false )
  {
    result = g2d->PrintMessage();
  }

  if( result == gfl2::str::StrWin::RES_FINISH )
  {
    g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_MOUTH_LOOP );   // 口パクを止める
    m_frameListener->OnAction( FrameListener::ACTION_ROTOPON_ITEMEVENT );
    SetVisibleLayout( false );
    SetVisiblePassivePane( true );
    SetRotoponItemScript( true );
    m_rotoponSeq = ROTOPON_SEQID_ITEM_SCRIPT_WAIT;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_ItemScriptWait
 * @brief   ロトポン用アップデート：スクリプト待ち
 * @date    2016.12.16
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_ItemScriptWait(void)
{
  if( m_isRotoponScript == false )
  {
    SetVisibleLayout( true );
    SetVisiblePassivePane( false );
    StartRotomMessage( m_rotoponItem->msg );
    m_rotoponSeq = ROTOPON_SEQID_END_MESSAGE;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_EndMessage
 * @brief   ロトポン用アップデート：終了メッセージ処理
 * @date    2016.12.12
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_EndMessage(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrWin::Result result = g2d->PrintMessage();
  if( result != gfl2::str::StrWin::RES_FINISH && m_touchPanel->IsTouch() != false )
  {
    result = g2d->PrintMessage();
  }

  if( result == gfl2::str::StrWin::RES_FINISH )
  {
    g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_MOUTH_LOOP );   // 口パクを止める
    g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_OUT );
    m_rotoponSeq = ROTOPON_SEQID_END;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotopon_End
 * @brief   ロトポン用アップデート：終了処理
 * @date    2016.12.09
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotopon_End(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000_ROTOM_OUT ) != false )
  {
    m_pRotomFriendship->ResetRotopon();
    m_frameListener->OnAction( FrameListener::ACTION_ROTOPON_END );
    SetEnableButton( true );  // ボタン有効
    SetVisibleLayout( false );
    g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_00, false );
    g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_TEXTBOX_01, false );
    g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000__CURSOR_00_KEEP );

    Savedata::IncRecord( Savedata::Record::RECID_ROTOPON_CNT );   // レコード

    m_rotoponSeq = ROTOPON_SEQID_NONE;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotoponItem
 * @brief   ロトポンで入手するアイテムを設定
 * @date    2016.12.13
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotoponItem(void)
{
  if( m_saveFieldMenu->GetRotomRequest() == Savedata::FieldMenu::ROTOM_REQ_TUTORIAL )
  {
    m_rotoponItem = &RotoponItem[0];
  }
  else
  {
    for ever
    {
      m_rotoponItem = &RotoponItem[ System::GflUse::GetPublicRand(ROTOPON_ITEM_MAX) ];
      // 特定のイベントまで"タマゴふかポン"を入手できないようにする
      if( m_eventWork->CheckEventFlag(FE_SUB059_END) != false )
      {
        break;
      }
      if( m_rotoponItem->id != ITEM_TAMAGOHUKAPON )
      {
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotoponItem
 * @brief   ロトポンで獲得した道具を取得
 * @date    2016.12.12
 *
 * @return  アイテム番号
 */
//-----------------------------------------------------------------------------
u16 MenuBase::GetRotoponItem(void)
{
  if( m_rotoponItem == NULL )
  {
    GFL_ASSERT( 0 );
    return RotoponItem[0].id;
  }
  return m_rotoponItem->id;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotoponItemScript
 * @brief   ロトポンの道具スクリプトが起動中かを設定
 * @date    2016.12.16
 *
 * @param   flg   true = 起動中
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotoponItemScript( bool flg )
{
  m_isRotoponScript = flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomFace
 * @brief   ロトムの表情設定
 * @date    2016.12.13
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomFace(void)
{
  if( *m_dispMode != DISP_MODE_NORMAL )
  {
    return;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  u32 face = m_rotomFace;

/*
  // モーション中 or ロトポン中
  if( IsRotomMotionAnime() != false || IsUpdateRotopon() != false )
  {
    m_rotomFace = ROTOM_FACE_NONE;
  }
  // 親密度を上げられる = 暇
  else if( m_playData->IsRotomTouchFriendshipUp() != false )
  {
    face = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_FACE_03;
  }
*/
  // 親密度を上げられる = 暇 and 会話中じゃない and ロトポン中じゃない
  if( m_playData->IsRotomTouchFriendshipUp() != false &&
      m_saveFieldMenu->IsVisibleRotom() == false &&
      IsUpdateRotopon() == false )
  {
    face = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_FACE_03;
  }
  else
  {
    face = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_EYE_LOOP;
  }

  if( face != m_rotomFace )
  {
/*
    if( face == ROTOM_FACE_NONE )
    {
      g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_FACE_SET, false );
    }
    else
    {
      g2d->SetPaneVisible( LYTID_LOWER_ROTOM, PANENAME_FIELDMENU_RTM_LOW_000_PANE_FACE_SET, true );
      g2d->StartAnime( LYTID_LOWER_ROTOM, face );
    }
*/
    if( m_rotomFace != ROTOM_FACE_NONE )
    {
      g2d->StopAnime( LYTID_LOWER_ROTOM, m_rotomFace );
    }
    if( face != ROTOM_FACE_NONE )
    {
      g2d->StartAnime( LYTID_LOWER_ROTOM, face );
    }
    m_rotomFace = face;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartRotomPowerGauge
 * @brief   ロトムパワーゲージ表示
 * @date    2017.02.03
 *
 * @param   is_init   画面初期化時 = true
 */
//-----------------------------------------------------------------------------
void MenuBase::StartRotomPowerGauge( bool is_init )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  f32 frame = 0.0f;

  if( is_init != false )
  {
    if( m_rotomPower->IsRotomPower() == false )
    {
      return;
    }
    frame = g2d->GetAnimeMaxFrame( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000__RP_GAUGE_IN );
  }
  g2d->StartAnimeFrame( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000__RP_GAUGE_IN, frame );
  g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000__RP_GAUGE_LOOP );
  SetRotomPowerGauge();
  m_isRotomPowerGaugeOn = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateRotomPowerGauge
 * @brief   ロトムパワーゲージ更新
 * @date    2017.02.03
 */
//-----------------------------------------------------------------------------
void MenuBase::UpdateRotomPowerGauge(void)
{
  if( m_isRotomPowerGaugeOn == false )
  {
    if( m_rotomPower->IsRotomPower() != false )
    {
      StartRotomPowerGauge( false );
    }
  }
  else
  {
    if( m_rotomPower->IsRotomPower() == false )
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      g2d->StopAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000__RP_GAUGE_LOOP );
      g2d->StartAnime( LYTID_LOWER_ROTOM, LA_MAINUPPER_FIELDMENU_RTM_LOW_000__RP_GAUGE_OUT );
      SetRotomPowerGauge();
      m_isRotomPowerGaugeOn = false;
    }
    else
    {
      SetRotomPowerGauge();
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomPowerGauge
 * @brief   ロトムパワーゲージ設定
 * @date    2017.02.03
 */
//-----------------------------------------------------------------------------
void MenuBase::SetRotomPowerGauge(void)
{
  /*
    @note
      ゲージは0フレーム目が最大。
      Gaugeクラスで0が返ってきた場合が最大なので、now=maxでは最小となる。
  */
  u32 now = ROTOM_POWER_GAUGE_FRAME;
  u32 max = ROTOM_POWER_GAUGE_FRAME;

  if( m_rotomPower->IsRotomPower() != false )
  {
    const Field::RotomPower::RotomPowerTime * time = m_rotomPower->GetRotomPowerTime();
    max = time->GetMaxLife();
    now = max - time->GetLife();
  }
  GetG2DUtil()->StartAnimeFrame(
    LYTID_LOWER_ROTOM,
    LA_MAINUPPER_FIELDMENU_RTM_LOW_000__RP_GAUGE_ROTO_GAUGE,
    app::tool::Gauge::GetDottoNumF32(now,max,ROTOM_POWER_GAUGE_FRAME),
    false, false );
}


//-----------------------------------------------------------------------------
/**
 * @func    SetTimeRotomColor
 * @brief   ロトム時間帯アニメセット
 * @date    2017.03.21
 *
 * @param   is_room         true = 室内
 * @param   is_ex_weather   true = 特殊天候（暗黒天候）
 */
//-----------------------------------------------------------------------------
void MenuBase::SetTimeRotomColor( bool is_room, bool is_ex_weather )
{
  if( m_isInitialized == false || m_isDrawEnable == false )
  {
    return;
  }

  u32 anime = 0;
  u32 frame = 0;

  // 室内
  if( is_room != false )
  {
    anime = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_TIME_04;
    if( anime != m_rotomTimeColorAnime )
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      for( u32 i=0; i<TIME_COLOR_ANIME_MAX; i++ )
      {
        g2d->StopAnime( LYTID_LOWER_ROTOM, RotomTimeColorAnimeTable[i] );
      }
      g2d->StartAnime( LYTID_LOWER_ROTOM, anime );
    }
  }
  // 特殊天候
  else if( is_ex_weather != false )
  {
    anime = LA_MAINUPPER_FIELDMENU_RTM_LOW_000_TIME_05;
    if( anime != m_rotomTimeColorAnime )
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      for( u32 i=0; i<TIME_COLOR_ANIME_MAX; i++ )
      {
        g2d->StopAnime( LYTID_LOWER_ROTOM, RotomTimeColorAnimeTable[i] );
      }
      g2d->StartAnime( LYTID_LOWER_ROTOM, anime );
    }
  }
  // それ以外
  else
  {
    GetTimeRotomColor( &anime, &frame );

    if( anime != m_rotomTimeColorAnime )
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      for( u32 i=0; i<TIME_COLOR_ANIME_MAX; i++ )
      {
        g2d->StopAnime( LYTID_LOWER_ROTOM, RotomTimeColorAnimeTable[i] );
      }
      g2d->StartAnimeFrame( LYTID_LOWER_ROTOM, anime, static_cast<f32>(frame), false );
    }
    else if( frame != m_rotomTimeColorFrame )
    {
      GetG2DUtil()->SetAnimeFrame( LYTID_LOWER_ROTOM, anime, static_cast<f32>(frame) );
    }
  }

  m_rotomTimeColorAnime = anime;
  m_rotomTimeColorFrame = frame;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetTimeRotomColor
 * @brief   ロトム時間帯アニメデータ取得
 * @date    2017.03.21
 *
 * @param   anime   アニメ番号取得場所
 * @param   frame   フレーム取得場所
 */
//-----------------------------------------------------------------------------
void MenuBase::GetTimeRotomColor( u32 * anime, u32 * frame )
{
  static const u32 tbl[][2] =
  { // 開始時間, 終了時間
    {  6,  9 },   // 朝
    { 10, 16 },   // 昼
    { 17, 17 },   // 夕
    { 18, 29 },   // 夜
  };

#if PM_DEBUG
  if( m_debugIsTimeZoneCheck == false )
  {
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      for( u32 j=0; j<2; j++ )
      {
        s32 h = tbl[i][j];
        if( h >= 24 )
        {
          h -= 24;
        }
        GameSys::TimeZone::Zone d_zone = GameSys::TimeZone::ConvertHourToTimeZone( h );
        if( d_zone == GameSys::TimeZone::MIDNIGHT )
        {
          d_zone = GameSys::TimeZone::NIGHT;
        }
        GFL_ASSERT( static_cast<u32>(d_zone) == i );
      }
    }
    m_debugIsTimeZoneCheck = true;
  }
#endif

  GameSys::TimeZone::Zone zone = GameSys::TimeZone::GetNowTimeZone( GameSys::TimeZone::TYPE_ALOLA );  // @fix MMCat[544]: 現在のタイムゾーンを取得
  s32 hour = Field::EvTime::GetAlolaHour( m_gameData );

  // 深夜の場合は夜に続くように変換
  if( zone == GameSys::TimeZone::MIDNIGHT )
  {
    zone = GameSys::TimeZone::NIGHT;
    hour += 24;
  }

  if( static_cast<u32>(zone) >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    zone = GameSys::TimeZone::MORNING;
  }

  const u32 * data = tbl[zone];

  f32 min = static_cast<f32>( (hour-data[0]) * 60 + Field::EvTime::GetAlolaMinute(m_gameData) );
  f32 f_min = static_cast<f32>( (data[1]-data[0]+1) * 60 );

  *anime = RotomTimeColorAnimeTable[zone];
  *frame = static_cast<u32>( static_cast<f32>(TIME_COLOR_ANIME_FRAME) / f_min * min );
  if( *frame >= TIME_COLOR_ANIME_FRAME )
  {
    *frame = TIME_COLOR_ANIME_FRAME - 1;
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.11.07
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
app::ui::UIInputListener::ListenerResult MenuBase::OnLayoutPaneEvent( const u32 button_id )
{
  // ロトポンモード
  if( IsUpdateRotopon() != false )
  {
    return DISABLE_ACCESS;
  }

  if( button_id == BTNID_MESSAGE )
  {
    m_frameListener->OnAction( FrameListener::ACTION_ROTOM_OPEN );
    return DISABLE_ACCESS;
  }

  if( button_id == BTNID_TOWNMAP )
  {
    m_frameListener->OnAction( FrameListener::ACTION_CALL_TOWNMAP );
    return DISABLE_ACCESS;
  }

  if( button_id == BTNID_ZUKAN )
  {
    m_frameListener->OnAction( FrameListener::ACTION_ZUKAN );
    return DISABLE_ACCESS;
  }

  if( button_id >= BTNID_MOTION_01 && button_id <= BTNID_MOTION_06 )
  {
    // ロトポン
    if( button_id == BTNID_MOTION_01 || button_id == BTNID_MOTION_02 )
    {
      if( m_pRotomFriendship->IsStartRotopon() != false )
      {
        SetRotoponItem();   // 道具決定
        m_frameListener->OnAction( FrameListener::ACTION_ROTOPON_START );
        return DISABLE_ACCESS;
      }
    }

    bool is_play_motion = true;
    Savedata::FieldMenu::RotomRequest req = m_pRotomFriendship->LotRotomRequest();

    m_isRotomFriendshipUp = false;

    // ニックネーム設定イベント
    if( m_pRotomFriendship->GetRank() == Savedata::FieldMenu::ROTOM_RANK_4 &&
        m_saveFieldMenu->GetNicknameType() == Savedata::FieldMenu::NICKNAME_TYPE_NONE )
    {
      SetRequestRotomMessage(
        PlayData::ROTOM_MSG_NICKNAME, msg_field_rotom_nickname_04, PlayData::ROTOM_WORK_NONE );
      is_play_motion = false;
    }
    // お願いが発生するか
    else if( req != Savedata::FieldMenu::ROTOM_REQ_NONE )
    {
      SetRequestRotomMessageRequestTalk( req );
      is_play_motion = false;
    }
    // 親密度を上げられるか
    else if( m_playData->IsRotomTouchFriendshipUp() != false )
    {
      // 会話が発生するか
      if( m_pRotomFriendship->CheckRotomTalkStart() != false )
      {
        SetRequestRotomMessageFreeTalk();
        is_play_motion = false;
      }
      else
      {
        m_pRotomFriendship->Add( 20, true );
        m_isRotomFriendshipUp = true;
      }
      m_playData->SetRotomTouchFriendshipUpFlag( false );
    }

    if( is_play_motion != false )
    {
      StartRotomMotion( button_id - BTNID_MOTION_01 );
    }
    return DISABLE_ACCESS;
  }

  if( button_id == BTNID_ROTOM_SELECT_01 )
  {
    m_rotomMenuSelect = 0;
    return DISABLE_ACCESS;
  }

  if( button_id == BTNID_ROTOM_SELECT_02 )
  {
    m_rotomMenuSelect = 1;
    return DISABLE_ACCESS;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.11.24
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
app::ui::UIInputListener::ListenerResult MenuBase::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  // ロトポンモード
  if( IsUpdateRotopon() != false )
  {
    if( button->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      m_isRotoponInputTrg = true;
      return DISABLE_ACCESS;
    }
    return ENABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_X) )
  {
    m_frameListener->OnAction( FrameListener::ACTION_CALL_MENU );
    return DISABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
  {
    m_frameListener->OnAction( FrameListener::ACTION_CALL_RIDE );
    return DISABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_R) )
  {
    m_frameListener->OnAction( FrameListener::ACTION_CALL_FINDER );
    return DISABLE_ACCESS;
  }

  if( key->IsTrigger(gfl2::ui::DIR_UP) )
  {
    m_frameListener->OnAction( FrameListener::ACTION_RIDE_SC_UP );
    return DISABLE_ACCESS;
  }
  else if( key->IsTrigger(gfl2::ui::DIR_DOWN) )
  {
    m_frameListener->OnAction( FrameListener::ACTION_RIDE_SC_DOWN );
    return DISABLE_ACCESS;
  }
  else if( key->IsTrigger(gfl2::ui::DIR_LEFT) )
  {
    m_frameListener->OnAction( FrameListener::ACTION_RIDE_SC_LEFT );
    return DISABLE_ACCESS;
  }
  else if( key->IsTrigger(gfl2::ui::DIR_RIGHT) )
  {
    m_frameListener->OnAction( FrameListener::ACTION_RIDE_SC_RIGHT );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent
 * @brief   タッチパネルイベントの検知
 * @date    2016.12.12
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
app::ui::UIInputListener::ListenerResult MenuBase::OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch )
{
  if( IsUpdateRotopon() != false )
  {
    if( touch_panel->IsTouchTrigger() != false )
    {
      m_isRotoponInputTrg = true;
      return DISABLE_ACCESS;
    }
  }
  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)
