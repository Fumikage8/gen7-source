//======================================================================
/**
 * @file    TitleMenuUpperDraw.cpp
 * @date    2015/07/21 15:13:48
 * @author  fukushima_yuya
 * @brief   タイトルメニュー上画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "TitleMenuUpperDraw.h"
#include "TitleMenuDrawListener.h"
#include "TitleMenuDrawerBase.h"
#include "TitleMenuYesNoWindow.h"

#include <AppLib/include/Tool/AppToolTrainerIconRendering.h>
#include <AppLib/include/Tool/TalkWindow.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <Field/FieldStatic/include/Zone/ZoneDataAccessor.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>
#include <sound/include/Sound.h>
#include <Savedata/include/MyItemSave.h>
#include <Savedata/include/Situation.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/title_menu/title_menu.h>
#include <niji_conv_header/app/title_menu/title_menu_pane.h>
#include <niji_conv_header/app/title_menu/title_menu_anim_list.h>
#include <niji_conv_header/message/msg_title_menu.h>
#include <niji_conv_header/trainer/trtype_def.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };

  enum
  {
    SAVE_SEQ_NONE,
    SAVE_SEQ_FIRST_MSG,         //!< 初回確認メッセージ
    SAVE_SEQ_FIRST_CONFIRM,     //!< 初回確認
    SAVE_SEQ_FINAL_MSG,         //!< 最終確認メッセージ
    SAVE_SEQ_FINAL_CONFIRM,     //!< 最終確認
    SAVE_SEQ_DATA_DELETE,       //!< セーブデータの削除
    SAVE_SEQ_END,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//------------------------------------------------------------------------------
TitleMenuUpperDraw::TitleMenuUpperDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager, MenuCursor* pCursor )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pGameManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) )
  , m_pFadeManager( NULL )
  , m_Seq( SEQ_INIT )
  , m_SubSeq( 0 )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pDrawerBase( NULL )
  , m_pCursor( pCursor )
  , m_DispMode( DISP_MODE_TITLE_MENU )
  , m_InitSaveSeq( SAVE_SEQ_NONE )
  , m_yesNoRes( YESNO_RES_NONE )
  , m_pTalkWindow( NULL )
  , m_pWindow( NULL )
  , m_pGameData( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData() )
  , m_isInitSaveData( false )
  , m_pKeyController( NULL )
  , m_CursorPos( 0 )
  , m_CreateSeq(0)
  , m_DeleteSeq(0)
  , m_pTrainerIconRendering(NULL)
{
  m_pMyStatus = m_pGameData->GetPlayerStatus();
  m_pMyItem   = m_pGameData->GetMyItem();
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam       外部設定パラメータ
 * @param   pAppLytBuff     レイアウトバイナリ
 * @param   pMsgBuf
 * @param   pWordSetLoader  ワードセットローダー
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, void* pAppWndLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet, WordSetLoader* wordSetLoader )
{
  m_pMsgData        = msgData;
  m_pWordSet        = wordSet;
  m_pWordSetLoader  = wordSetLoader;

  m_pAppParam = pAppParam;
  m_isInitSaveData = (m_pAppParam->initMode != INIT_MODE_NONE);

  // セーブデータ初期化モードか？
  if( m_isInitSaveData )
  {
    m_DispMode = DISP_MODE_INIT_SAVEDATA;   //!< セーブデータ初期化モードへ
    m_InitSaveSeq = SAVE_SEQ_FIRST_MSG;
  }

  m_pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( pAppLytBuff, pAppWndLytBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool TitleMenuUpperDraw::End( void )
{
  if( !DeleteFaceIcon() )
  {
    return false;
  }

  if( m_pWindow )
  {
    if( !m_pWindow->IsDelete() ) return false;

    GFL_SAFE_DELETE( m_pWindow );
  }

  if( m_pTalkWindow )
  {
    m_pTalkWindow->RemoveFromSuperView();
    m_pTalkWindow->End();
    GFL_SAFE_DELETE( m_pTalkWindow );
  }

  if( m_pCursor )
  {
    m_pCursor->UnsetupCursor();
  }

  GFL_SAFE_DELETE( m_pKeyController );

  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::Update( void )
{
  app::ui::UIView::Update();

  if( m_pTrainerIconRendering ) m_pTrainerIconRendering->Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( !MainSequenceReady() ) break;

      // ステータス画面を開く
      TitleMenuOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      if( m_pCursor ) m_pCursor->Update();
      if( m_pWindow ) m_pWindow->Update();

      if( m_SubSeq == 0 )      { SubSequenceInit(); }
      else if( m_SubSeq == 1 ) { SubSequenceMain(); }
      else                     { SubSequenceTerm(); }
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      if( m_uiListener ) m_uiListener->EndTitleMenu();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );

  if( m_pCursor ) m_pCursor->Draw( no );
  if( m_pTrainerIconRendering ) m_pTrainerIconRendering->Draw( no );
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool TitleMenuUpperDraw::IsExit( void )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//-----------------------------------------------------------------------------
void TitleMenuUpperDraw::TitleMenuOpen( void )
{
  // フェードイン
  if( m_pFadeManager ) m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );

  if( m_pCursor && !m_isInitSaveData )
  {
    m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), 0 );
    m_pCursor->SetVisible( true );
  }

  this->SetInputEnabled( true );
  this->SetTouchEnabled( false );

  m_Seq = SEQ_IN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//-----------------------------------------------------------------------------
void TitleMenuUpperDraw::TitleMenuClose( void )
{
  this->SetInputEnabled( false );

  // フェードアウト
  gfl2::math::Vector4 color( 0, 0, 0, 255 );
  if( m_pFadeManager ) m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );

  m_Seq = SEQ_OUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool TitleMenuUpperDraw::IsEndInOutAnim( bool in )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
      && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief    このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::SetTitleViewUIListener( TitleMenuDrawListener* pListener )
{
  m_uiListener = pListener;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::Initialize2D( void* pAppLytBuff, void* pAppWndLytBuff )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_TITLE_MENU_TITLEMENU_MIN_UPP_000_BFLYT,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  SetWordSetLoader( m_pWordSetLoader );

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    2,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet );

  this->SetInputEnabled( false );

  // 描画共通処理の生成
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  {
    GFL_ASSERT( m_pDrawerBase );

    // アニメの再生
    m_pDrawerBase->StartAnim( LA_TITLE_MENU_TITLEMENU_MIN_UPP_000_BG_LOOP_00 );
    //m_pDrawerBase->StartAnim( LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BG_01_BG_LOOP_00 );
  }

  // ボタンマネージャの生成
  CreateButtonManager();

  // カーソル
  {
    static const u32 c_CursorPosIndex[BTN_ID_NUM][2] = {
      { PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_REPORT, PANENAME_TITLEMENU_BTN_LOW_000_PANE_NULL_CUR },
      { PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_00, PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR },
      { PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_01, PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR },
    };

    gfl2::lyt::LytPane* curPosPane[BTN_ID_NUM];

    for( u8 i=0; i<BTN_ID_NUM; ++i )
    {
      u32 partsIndex = c_CursorPosIndex[i][0];
      u32 paneIndex  = c_CursorPosIndex[i][1];

      DrawerBase* base = m_pDrawerBase;

      gfl2::lyt::LytParts* parts = base->GetParts( partsIndex );
      curPosPane[i] = base->GetPane( paneIndex, parts );
    }
    // セットアップ
    m_pCursor->SetupCursor( m_pDrawerBase->GetG2D(), curPosPane, BTN_ID_NUM, gfl2::lyt::DISPLAY_UPPER );
  }

  // 
  m_pKeyController = GFL_NEW(devHeap) app::util::KeyRepeatController( BTN_ID_NUM-1, 0 );
  GFL_ASSERT( m_pKeyController );

  // トークウィンドウ
  {
    m_pTalkWindow = GFL_NEW(devHeap) App::Tool::TalkWindow( m_pDrawerBase->GetHeap(), m_pAppRenderingManager );
    //m_pTalkWindow->Setup( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
    m_pTalkWindow->SetWindowType( App::Tool::WIN_TYPE_SYS_DOWN );
    m_pTalkWindow->Setup( pAppWndLytBuff );
    m_pTalkWindow->InitMsgData( m_pMsgData );
    m_pTalkWindow->SetEnableMessageBoost( false );  //!< Boost OFF
    this->AddSubView( m_pTalkWindow );
  }

  // ビーズアイコンの初期化
  InitBeadsIcon();

  // パーツの表示切替
  static const u32 c_TitleMenuParts[] = {
    PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_00,
    PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_01,
    PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_REPORT,
  };

  for( u32 i=0; i<GFL_NELEMS(c_TitleMenuParts); ++i )
  {
    gfl2::lyt::LytPane* pane = m_pDrawerBase->GetPane( c_TitleMenuParts[i] );
    m_pDrawerBase->SetVisiblePane( pane, !m_isInitSaveData );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::InitializeText( void )
{
  //gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  app::util::G2DUtil* pG2D = this->GetG2DUtil();

  if( m_DispMode != DISP_MODE_INIT_SAVEDATA )
  {
    gfl2::lyt::LytParts* reportParts = m_pDrawerBase->GetParts( PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_REPORT );
    {
      GameSys::GameData* gd = m_pGameManager->GetGameData();
      Savedata::MyStatus* ms = gd->GetPlayerStatus();
      Savedata::ZukanData* zd = gd->GetZukanData();
      //Savedata::Misc* misc = gd->GetMisc();

      // つづきから
      gfl2::lyt::LytTextBox* pContinue =
        m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_SELECTCONTINUE, reportParts );
      this->SetTextboxPaneMessage( pContinue, title_menu_01_01 );

      // プレイヤー名
      {
        gfl2::lyt::LytTextBox* pPlayerName =
          m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_PLAYERNAME, reportParts );

        pG2D->SetRegisterPlayerName( 0, ms );
        this->SetTextboxPaneMessage( pPlayerName, title_menu_01_02 );
      }

      // 場所
      {
        Savedata::Situation* situation = gd->GetSituation();

        Field::Location::SaveData * loc = NULL;
        u16 zoneID = 0;
        // ゲームクリア時は遷移先を見る
        if( situation->HasWarpContinueRequest() )
        {
#if 0 //!< sangoのプログラムを移植
          loc = situation->GetSpecialLocation();
          u16* evwk_c107 = gd->GetEventWork()->GetEventWorkAdrs( WK_SCENE_C107 );

          // 初回ゲームクリア時はトクサネから始まるがメニューには「ミシロタウン」と出したい！
          if( *evwk_c107 == SEQ_C107_EV_AFTER_CLEAR_01_START )
          {
            zoneID = ZONE_ID_T101;
          }
          else {
            zoneID = loc->zone_id;
          }
#else
          loc = situation->GetSpecialLocation();
          zoneID = loc->zone_id;
#endif
        }
        else {
          loc = situation->GetStartLocation();
          zoneID = loc->zone_id;
        }

        Field::ZoneDataLoader* loader = gd->GetFieldZoneDataLoader();
        {
          Field::ZoneDataAccessor accessor( loader->GetZoneData( zoneID ) );

          m_pWordSet->RegisterPlaceName( 0, accessor.GetPlaceLabelID() );
          gfl2::lyt::LytTextBox* pPlaceName =
            m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_PLACENAME, reportParts );
          this->SetTextboxPaneMessage( pPlaceName, title_menu_01_03 );
        }
      }

      // 図鑑匹数
      {
        bool zukanGet = gd->GetEventWork()->CheckEventFlag( SYS_FLAG_ZUKAN_GET );

        gfl2::lyt::LytTextBox* pZukanTitle =
          m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_ZUKAN, reportParts );
        gfl2::lyt::LytTextBox* pZukanValue =
          m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_ZUKANNUMB, reportParts );
        gfl2::lyt::LytTextBox* pZukanHiki =
          m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_HIKI, reportParts );

        // メッセージのセット
        if( zukanGet )
        {
          u32 count = 0;

          if( zd->GetZenkokuZukanFlag() )
          {
            //count = zd->GetPokeSeeCount();
            count = zd->GetPokeGetCount();
          }
          else {
            //PokeTool::ExtendData* ex = GFL_NEW_LOW(devHeap) PokeTool::ExtendData(devHeap);

            ////zd->GetLocalPokeSeeCount( ex, PokeTool::ExtendData::LOCAL_AREA_A );
            //count = zd->GetPokeSeeCount();

            //GFL_DELETE ex;

            // 捕まえたポケモン数  仕様変更：見た数->捕まえた数
            count = zd->GetLocalPokeGetCountStatic( PokeTool::ExtendData::LOCAL_AREA_A );
          }

          this->SetTextboxPaneMessage( pZukanTitle, title_menu_01_04 );
          this->SetTextboxPaneMessage( pZukanHiki, title_menu_01_06 );
          this->SetTextboxPaneNumber( pZukanValue, title_menu_01_05, count, 3, 0, print::NUM_DISP_SPACE );
        }

        // 表示
        m_pDrawerBase->SetVisiblePane( pZukanTitle, zukanGet );
        m_pDrawerBase->SetVisiblePane( pZukanValue, zukanGet );
        m_pDrawerBase->SetVisiblePane( pZukanHiki, zukanGet );
      }

      // プレイ時間
      {
        const Savedata::PlayTime* playTime = gd->GetPlayTime();
        
        u32 hour = playTime->GetPlayHour();
        u32 minute = playTime->GetPlayMinute();

        // プレイ時間
        gfl2::lyt::LytTextBox* pTimeTitle =
          m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_PLAYTIME, reportParts );
        this->SetTextboxPaneMessage( pTimeTitle, title_menu_01_07 );

        // 時間(h)
        gfl2::lyt::LytTextBox* pTimeHour =
          m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_TIMEHOUR, reportParts );

        this->SetTextboxPaneNumber( pTimeHour, title_menu_01_08, hour, 3, 0, print::NUM_DISP_SPACE );

        // 時間(m)
        gfl2::lyt::LytTextBox* pTimeMinute =
          m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_TIMEMIN, reportParts );

        this->SetTextboxPaneNumber( pTimeMinute, title_menu_01_10, minute, 2, 0, print::NUM_DISP_ZERO );

        // コロン
        gfl2::lyt::LytTextBox* pTimeColon =
          m_pDrawerBase->GetTextBox( PANENAME_TITLEMENU_BTN_LOW_000_PANE_TEXT_TIMECOLON, reportParts );
        this->SetTextboxPaneMessage( pTimeColon, title_menu_01_09 );
      }
    }

    static const u32 c_ButtonPartsIndex[] = {
      PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_00,
      PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_01,
    };
    static const u32 c_ButtonMsgID[] = {
      title_menu_02_01,
      title_menu_03_01,
    };

    for( u32 i=0; i<GFL_NELEMS(c_ButtonPartsIndex); ++i )
    {
      gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( c_ButtonPartsIndex[i] );
      gfl2::lyt::LytTextBox* btnMsg = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, btnParts );

      this->SetTextboxPaneMessage( btnMsg, c_ButtonMsgID[i] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ビーズアイコンの初期化
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::InitBeadsIcon( void )
{
  struct BEADS_ICON
  {
    u32 item;
    gfl2::lyt::LytPaneIndex pane;
  };

  static const BEADS_ICON tbl[] =
  {
    { ITEM_NOOMARUBIIZU, PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_00 },
    { ITEM_HONOOBIIZU,   PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_01 },
    { ITEM_MIZUBIIZU,    PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_02 },
    { ITEM_DENKIBIIZU,   PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_03 },
    { ITEM_KUSABIIZU,    PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_04 },
    { ITEM_KOORIBIIZU,   PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_05 },
    { ITEM_KAKUTOUBIIZU, PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_06 },
    { ITEM_DOKUBIIZU,    PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_07 },
    { ITEM_ZIMENBIIZU,   PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_08 },
    { ITEM_HIKOUBIIZU,   PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_09 },
    { ITEM_ESUPAABIIZU,  PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_10 },
    { ITEM_MUSIBIIZU,    PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_11 },
    { ITEM_IWABIIZU,     PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_12 },
    { ITEM_GOOSUTOBIIZU, PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_13 },
    { ITEM_DORAGONBIIZU, PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_14 },
    { ITEM_AKUBIIZU,     PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_15 },
    { ITEM_HAGANEBIIZU,  PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_16 },
    { ITEM_FEARIIBIIZU,  PANENAME_TITLEMENU_BTN_LOW_000_PANE_BEADS_17 },
  };

  static const f32 c_BeadsPosY[] = {
    -58.0f, -66.0f,
  };

  gfl2::lyt::LytParts* parts       = m_pDrawerBase->GetParts( PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_REPORT );
  gfl2::lyt::LytPane*  firstBeads  = m_pDrawerBase->GetPane( tbl[0].pane, parts );
  gfl2::lyt::LytPane*  secondBeads = m_pDrawerBase->GetPane( tbl[1].pane, parts );

  const gfl2::math::VEC3 initPos = m_pDrawerBase->GetPanePos( firstBeads );
  const gfl2::math::VEC3 nextPos = m_pDrawerBase->GetPanePos( secondBeads );
  const f32 range_x = nextPos.x - initPos.x;
  f32 px = initPos.x;
  u8 index = 0;

  for( u32 i=0; i<GFL_NELEMS(tbl); ++i )
  {
    if( !m_pMyItem->Check(tbl[i].item, 1) )
    {
      continue;   //!< 持っていなければ表示しない
    }

    f32 py = c_BeadsPosY[index % 2];
    ++index;

    gfl2::math::VEC3 pos( px, py, 0.0f );
    gfl2::lyt::LytPane* pane = m_pDrawerBase->GetPane( tbl[i].pane, parts );

    m_pDrawerBase->SetPanePos( pane, pos );
    m_pDrawerBase->SetVisiblePane( pane, true );

    px += range_x;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：準備
 */
//------------------------------------------------------------------------------
bool TitleMenuUpperDraw::MainSequenceReady( void )
{
  // YesNoウィンドウ
  if( m_pWindow )
  {
    m_pWindow->Update();

    if( !m_pWindow->IsReady() ) return false;
  }
  else {
    m_pWindow = GFL_NEW(m_pHeap->GetDeviceHeap())
      YesNoWindow( m_pDrawerBase, m_pCursor, m_pAppRenderingManager, this );

    return false;
  }

  // 顔アイコン
  if( !CreateFaceIcon() )
  {
    return false;
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   つづきからはじめるを選択
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::SelectContinue( void )
{
  if( m_uiListener ) m_uiListener->SetEndMode( END_MODE_SELECT_CONTINUE );

  TitleMenuClose();
}

//------------------------------------------------------------------------------
/**
 * @brief   ふしぎなおくりものを選択
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::SelectPresent( void )
{
  if( m_uiListener ) m_uiListener->SetEndMode( END_MODE_SELECT_PRESENT );

  TitleMenuClose();
}

//------------------------------------------------------------------------------
/**
 * @brief   ライブ大会を選択
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::SelectLiveCup( void )
{
  if( m_uiListener ) m_uiListener->SetEndMode( END_MODE_SELECT_LIVE_CUP );

  TitleMenuClose();
}

//------------------------------------------------------------------------------
/**
 * @brief   セーブデータの初期化シーケンス：初期化
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::SubSequenceInit( void )
{
  switch( m_InitSaveSeq )
  {
  case SAVE_SEQ_FIRST_MSG:
    {
      m_pTalkWindow->SetMessage( static_cast<u32>(title_menu_12_01) );
      m_pTalkWindow->CallMsgWin();
    } break;

  case SAVE_SEQ_FIRST_CONFIRM:
  case SAVE_SEQ_FINAL_CONFIRM:
    {
      m_pWindow->OpenWindow();
      m_yesNoRes = YESNO_RES_NONE;
    } break;

  case SAVE_SEQ_FINAL_MSG:
    {
      m_pTalkWindow->SetMessage( static_cast<u32>(title_menu_12_02) );
      m_pTalkWindow->CallMsgWin();
    } break;

  case SAVE_SEQ_DATA_DELETE:
    {
      // セーブーデータフォーマット通知
      m_uiListener->OnSavedataFormat();
      // セーブデータの消去
      m_pGameData->FormatStart( m_pHeap->GetDeviceHeap() );
    } break;
  }

  m_SubSeq++;
}

//------------------------------------------------------------------------------
/**
 * @brief   セーブデータの初期化シーケンス：処理
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::SubSequenceMain( void )
{
  switch( m_InitSaveSeq )
  {
  case SAVE_SEQ_FIRST_MSG:
  case SAVE_SEQ_FINAL_MSG:
    {
      gfl2::str::StrWin::Result res = m_pTalkWindow->GetStrWinResult();

      if( res == gfl2::str::StrWin::RES_DONE )
      { 
        m_SubSeq++;
      }
    } break;

  case SAVE_SEQ_FIRST_CONFIRM:
  case SAVE_SEQ_FINAL_CONFIRM:
    {
      if( m_yesNoRes != YESNO_RES_NONE )
      {
        if( m_pWindow ) m_pWindow->CloseWindow();   // @fix cov_ctr[11050]
        m_pTalkWindow->CloseMsgWin();
        m_SubSeq++;
      }
    } break;

  case SAVE_SEQ_DATA_DELETE:
    {
      gfl2::fs::FsCtrBackupSystem::Result res;

      if( !m_pGameData->FormatMain( &res ) ) break;

      if( res == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
      {
        m_SubSeq++;
        break;
      }

      //セーブデータのフォーマット失敗は致命的
      GFL_PRINT("Format Result = %d\n", res );
      GFL_ASSERT_STOP( 0 );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   セーブデータの初期化シーケンス：解放
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::SubSequenceTerm( void )
{
  switch( m_InitSaveSeq )
  {
  case SAVE_SEQ_FIRST_MSG:
    {
      m_InitSaveSeq = SAVE_SEQ_FIRST_CONFIRM;
    } break;

  case SAVE_SEQ_FIRST_CONFIRM:
    {
      if( m_yesNoRes == YESNO_RES_YES )
      {
        m_InitSaveSeq = SAVE_SEQ_FINAL_MSG;
      }
      else if( m_yesNoRes == YESNO_RES_NO )
      {
        m_uiListener->SetEndMode( END_MODE_INIT_SAVE_CANCEL );

        TitleMenuClose();
      }
    } break;

  case SAVE_SEQ_FINAL_MSG:
    {
      m_InitSaveSeq = SAVE_SEQ_FINAL_CONFIRM;
    } break;

  case SAVE_SEQ_FINAL_CONFIRM:
    {
      if( m_yesNoRes == YESNO_RES_YES )
      {
        m_InitSaveSeq = SAVE_SEQ_DATA_DELETE;
      }
      else if( m_yesNoRes == YESNO_RES_NO )
      {
        m_uiListener->SetEndMode( END_MODE_INIT_SAVE_CANCEL );

        TitleMenuClose();
      }
    } break;

  case SAVE_SEQ_DATA_DELETE:
    {
      m_uiListener->SetEndMode( END_MODE_INIT_SAVE_DECIDE );

      TitleMenuClose();
    } break;
  }

  m_SubSeq = 0;
}


//------------------------------------------------------------------------------
/**
 * @brief   顔アイコンの生成
 */
//------------------------------------------------------------------------------
bool TitleMenuUpperDraw::CreateFaceIcon( void )
{
  enum {
    SEQ_CREATE_INIT,
    SEQ_CREATE_READY,
    SEQ_CREATE_SETUP,
    SEQ_CREATE_END,
  };

  // セーブデータの初期化時は何もしない
  if( m_isInitSaveData )
  {
    return true;
  }

  switch( m_CreateSeq )
  {
  case SEQ_CREATE_INIT:
    {
      gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();

      // 顔アイコンの生成
      m_pTrainerIconRendering = GFL_NEW(devHeap) app::tool::AppToolTrainerIconRendering();
      GFL_ASSERT( m_pTrainerIconRendering );
      // 初期化
      m_pTrainerIconRendering->Initialize(
        m_pAppRenderingManager,
        m_pDrawerBase->GetHeap(),
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),
        1,
        app::util::G2DUtil::SETUP_UPPER );

      m_CreateSeq++;
    } break;

  case SEQ_CREATE_READY:
    {
      if( m_pTrainerIconRendering->IsReady() )
      {
        m_CreateSeq++;
      }
    } break;

  case SEQ_CREATE_SETUP:
    {
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA objData;
      {
        gfl2::lyt::LytParts* parts        = m_pDrawerBase->GetParts( PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_REPORT );
        objData.pReplacePane              = m_pDrawerBase->GetPicture( PANENAME_TITLEMENU_BTN_LOW_000_PANE_PHOTO_DUMMY, parts );
        objData.pInLoadVisibleChangePane  = m_pDrawerBase->GetPane( PANENAME_TITLEMENU_BTN_LOW_000_PANE_WINDOW_PLAYER_FRAME, parts );

        objData.ConvFromMyStatus( *m_pMyStatus );
      }

      m_pTrainerIconRendering->SetIconObjectData( 0, objData );

      m_CreateSeq++;
    } break;

  case SEQ_CREATE_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   顔アイコンの破棄
 */
//------------------------------------------------------------------------------
bool TitleMenuUpperDraw::DeleteFaceIcon( void )
{
  enum {
    SEQ_DELETE_TERM,
    SEQ_DELETE_WAIT,
    SEQ_DELETE_END,
  };

  // セーブデータの初期化時は何もしない
  if( m_isInitSaveData )
  {
    return true;
  }

  switch( m_DeleteSeq )
  {
  case SEQ_DELETE_TERM:
    {
      m_pTrainerIconRendering->Terminate();

      m_DeleteSeq++;
    } break;

  case SEQ_DELETE_WAIT:
    {
      if( m_pTrainerIconRendering->IsDelete() )
      {
        GFL_SAFE_DELETE( m_pTrainerIconRendering );

        m_DeleteSeq++;
      }
    } break;

  case SEQ_DELETE_END:
    {
      return true;
    }
  }

  return false;
}





//==============================================================================
/**
 *      ↓  以下がリスナー関連  ↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult TitleMenuUpperDraw::OnLayoutPaneEvent( const u32 paneId )
{
  app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( m_uiListener == NULL ) return result;
  if( m_DispMode == DISP_MODE_INIT_SAVEDATA ) return result;

  switch( paneId )
  {
  case BTN_ID_CONTINUE:   //!< つづきから
    {
      SelectContinue();

      result = DISABLE_ACCESS;
    } break;

  case BTN_ID_PRESENT:   //!< ふしぎなおくりもの
    {
      SelectPresent();

      result = DISABLE_ACCESS;
    } break;

  case BTN_ID_LIVE_CUP:   //!< ライブ大会
    {
      SelectLiveCup();

      result = DISABLE_ACCESS;
    } break;
  }

  return result;
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult TitleMenuUpperDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  if( m_DispMode == DISP_MODE_TITLE_MENU )
  {
    u32 curPos = m_CursorPos;

    m_pKeyController->UpdateInfo( pKey, curPos, 0 );

    if( m_pKeyController->IsRepeat( gfl2::ui::DIR_UP ) )
    {
      curPos = (curPos + BTN_ID_NUM - 1) % BTN_ID_NUM;
    }
    else if( m_pKeyController->IsRepeat( gfl2::ui::DIR_DOWN ) )
    {
      curPos = (curPos + 1) % BTN_ID_NUM;
    }

    if( m_CursorPos != curPos )
    {
      m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), curPos );

      m_CursorPos = curPos;

      Sound::PlaySE( SEQ_SE_SELECT1 );
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      app::tool::ButtonManager* bm = m_pDrawerBase->GetButtonManager();

      bm->StartSelectedAct( BTN_ID_CONTINUE + m_CursorPos );
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      m_uiListener->SetEndMode( END_MODE_CANCEL );

      TitleMenuClose();

      Sound::PlaySE( SEQ_SE_CANCEL1 );  //!< キャンセル音
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューウィンドウの選択
 * @param   選択された項目ID
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::SelectMenu( u32 id )
{
  if( id == 0 )
  {
    m_yesNoRes = YESNO_RES_YES;
  }
  else {
    m_yesNoRes = YESNO_RES_NO;
  }
}





//==============================================================================
/**
 *      ↓  以下がボタンマネージャ関連  ↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void TitleMenuUpperDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CONTINUE, NULL, NULL,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_REPORT_TOUCH,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_REPORT_RELEASE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_REPORT_CANSEL,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_REPORT_TOUCH_RELEASE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_REPORT_ACTIVE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_REPORT_PASSIVE,
    },
    {
      BTN_ID_PRESENT, NULL, NULL,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_00_TOUCH,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_00_RELEASE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_00_CANSEL,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_00_TOUCH_RELEASE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_00_ACTIVE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_00_PASSIVE,
    },
    {
      BTN_ID_LIVE_CUP, NULL, NULL,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_01_TOUCH,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_01_RELEASE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_01_CANSEL,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_01_TOUCH_RELEASE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_01_ACTIVE,
      LA_TITLE_MENU_TITLEMENU_MIN_UPP_000__BUTTON_01_PASSIVE,
    },
  };

  const gfl2::lyt::LytPaneIndex c_ButtonPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_REPORT, PANENAME_TITLEMENU_BTN_LOW_000_PANE_REPORT_SHADOW },
    { PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_00, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00 },
    { PANENAME_TITLEMENU_MIN_UPP_000_PANE_BUTTON_01, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00 },
  };

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(devHeap) app::ui::ButtonInfoEx[BTN_ID_NUM];
  gfl2::lyt::LytWk* pLytWk = m_pDrawerBase->GetLytWk();

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    u32 paneIndex = c_ButtonPaneIndex[i][0];
    DrawerBase* pBase = m_pDrawerBase;

    gfl2::lyt::LytParts* pParts = pBase->GetParts( paneIndex );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = pParts;
    info[i].bound_pane = m_pDrawerBase->GetPane( c_ButtonPaneIndex[i][1], pParts );
  }

  app::ui::UIView* pUIView = m_pDrawerBase->GetUIView();
  pUIView->CreateButtonManager( m_pHeap, pLytWk, info, BTN_ID_NUM );

  app::tool::ButtonManager* pBtnMan = m_pDrawerBase->GetButtonManager();
  pBtnMan->SetInputMode( app::tool::ButtonManager::INPUT_MODE_KEY_ONLY );   //!< キーのみ入力可

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    pBtnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }

  GFL_DELETE_ARRAY info;
}


GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )
