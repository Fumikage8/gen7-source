//======================================================================
/**
 * @file LangSelectUpperDraw.cpp
 * @date 2015/07/23 15:47:04
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "LangSelectUpperDraw.h"
#include "LangSelectCursor.h"
#include "LangSelectDrawListener.h"
#include "LangSelectDrawerBase.h"
#include "LangSelectButton.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <AppLib/include/Tool/TalkWindow.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>
#include <sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/lang_select/lang_select.h>
#include <niji_conv_header/app/lang_select/lang_select_pane.h>
#include <niji_conv_header/app/lang_select/lang_select_anim_list.h>
#include <niji_conv_header/message/msg_language_select.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

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
    SEQ_ANIM_WAIT,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END,
    SEQ_SETUP_WAIT,
    SEQ_SETUP_MSG,
    SEQ_SETUP_MSG_WAIT,
    SEQ_SETUP_MSG_SKIP_WAIT,
    SEQ_SETUP_CALL_SE_START,
    SEQ_SETUP_CALL_SE_WAIT,
  };

  static const LangData c_LangData[LANG_ID_MAX] = {
    { POKEMON_LANG_ENGLISH, 1.0f },
    { POKEMON_LANG_SPAIN,   2.0f },
    { POKEMON_LANG_FRANCE,  3.0f },
    { POKEMON_LANG_GERMANY, 4.0f },
    { POKEMON_LANG_ITALY,   5.0f },
    { POKEMON_LANG_JAPAN,   0.0f },
    { POKEMON_LANG_KOREA,   6.0f },
    { POKEMON_LANG_CHINA,   8.0f }, // @fix NMCat[1281]
    { POKEMON_LANG_TAIWAN,  7.0f }, // @fix NMCat[1281]
  };

  static const u32 CHARA_ANIM = LA_LANG_SELECT_LANGSELECT_MIN_UPP_CHARA_04;
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
LangSelectUpperDraw::LangSelectUpperDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager, MenuCursor* pCursor )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pGameManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) )
  , m_pFadeManager( NULL )
  , m_Seq( SEQ_INIT )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pDrawerBase( NULL )
  , m_pCursor( pCursor )
  , m_pLangButton( NULL )
  , m_DispMode( DISP_MODE_SAVE_DATA )
  , m_ActCharaAnim( 0xFFFFFFFF )
  , m_pMantaPane(NULL)
  , m_DecideFlg( false )
  , m_PlaySE_Landing(true)
  , m_pTalkWindow( NULL )
  , m_talkStartFlag( false )
  , m_settingWait( 0 )
  , m_settingWaitFlag( false )
{
  for( u8 i=0; i<DISP_LANG_SELECT_NUM; ++i )
  {
    m_pKeyController[i] = NULL;
    m_CursorPos[i] = 0;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam       外部設定パラメータ
 * @param   pAppLytBuff     レイアウトバイナリ
 * @param   pWinLytBuff     TalkWindow用レイアウトバイナリ
 * @param   msgData         メッセージデータ
 * @param   wordSet         ワードセット
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, void* pWinLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pAppParam = pAppParam;

  m_pMsgData = msgData;
  m_pWordSet = wordSet;

  m_pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

  // デフォルトの言語を設定
  SetDefaultLang( m_pAppParam->defaultLang );

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( pAppLytBuff );
  InitializeText();

//  CreateTalkWindow( pWinLytBuff );
  m_talkWindowLytBuff = pWinLytBuff;

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LangSelectUpperDraw::End( void )
{
  DeleteTalkWindow();

  if( m_pCursor )
  {
    m_pCursor->UnsetupCursor();
  }

  for( u32 i=0; i<DISP_LANG_SELECT_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pKeyController[i] );
  }

  GFL_SAFE_DELETE( m_pLangButton );

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
void LangSelectUpperDraw::Update( void )
{
  app::ui::UIView::Update();

  if( m_pCursor ) m_pCursor->Update();

  UpdateSettingWait();
  PlaySE_Landing();     // 着地音の再生

  bool is_repSE = ReplayCallSE();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      LangSelectOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_ANIM_WAIT:
    {
      if( m_pDrawerBase->IsAnimEnd( CHARA_ANIM ) )
      {
        // フェードアウト
/*
        gfl2::math::Vector4 color( 0, 0, 0, 255 );
        if( m_pFadeManager ) m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
*/
        if( m_pFadeManager )
        {
          gfl2::math::Vector4 col_s( 0.0f, 0.0f, 0.0f, 0.0f );
          gfl2::math::Vector4 col_e( 0.0f, 0.0f, 0.0f, 0.0f );
          m_pFadeManager->RequestOut( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &col_s, &col_e );
        }
        m_pDrawerBase->StopAnim( LA_LANG_SELECT_LANGSELECT_MIN_UPP_DEMO_01 );
        m_Seq = SEQ_OUT;
      }
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
//      Sound::StopSE( SEQ_SE_SYS_NJ_016 );   //!< 波の音、潮騒停止(ループ)
//      Sound::EndBGMReq();

      if( m_uiListener ) m_uiListener->EndLangSelect();

      m_Seq = SEQ_END;
    } break;

  case SEQ_SETUP_WAIT:
    if( IsSetLangFrame() != false )
    {
      // 言語のセット
      m_uiListener->SetLangID( m_LangData[m_selectLangIndex].lang, m_isKanji );
      StartSettingWait();
      m_Seq = SEQ_SETUP_MSG;
    }
    break;
  case SEQ_SETUP_MSG:
    if( IsSettingWait() == false )
    {
      m_pDrawerBase->StartAnim( LA_LANG_SELECT_LANGSELECT_MIN_UPP_DEMO_02 );
      PrintMessage( language_select_10 );
      m_Seq = SEQ_SETUP_MSG_WAIT;
    }
    break;
  case SEQ_SETUP_MSG_WAIT:
    if( IsPrintMessage() == false )
    {
      if( m_printMsgID == language_select_10 )
      {
        Sound::PlaySE( SEQ_SE_SD9030_CONFIG );
      }
      StartSettingWait( true );
      m_Seq = SEQ_SETUP_MSG_SKIP_WAIT;
    }
    break;
  case SEQ_SETUP_MSG_SKIP_WAIT:
    break;
  case SEQ_SETUP_CALL_SE_START:
    if( IsSettingWait() == false )
    {
      SetDispMode( DISP_MODE_TELL );
      m_Seq = SEQ_SETUP_CALL_SE_WAIT;
    }
    break;
  case SEQ_SETUP_CALL_SE_WAIT:
    // @note SE終了前にリプレイされるかもしれないので2回目が再生されたら進めるようにしておく
    if( Sound::IsSEPlaying(SEQ_SE_SD9030_CALLING) == false ||
        is_repSE != false )
    {
      PrintMessage( language_select_11 );
      m_Seq = SEQ_SETUP_MSG_WAIT;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );

  if( m_pCursor ) m_pCursor->Draw( no );
}

//-----------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 */
//-----------------------------------------------------------------------------
bool LangSelectUpperDraw::IsReady( void )
{
  return m_Seq == SEQ_MAIN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool LangSelectUpperDraw::IsExit( void )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//-----------------------------------------------------------------------------
void LangSelectUpperDraw::LangSelectOpen( void )
{
  // フェードイン
  if( m_pFadeManager ) m_pFadeManager->RequestIn( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );

  // 入力の開始
  this->SetInputEnabled( true );

  m_Seq = SEQ_IN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//-----------------------------------------------------------------------------
void LangSelectUpperDraw::LangSelectClose( void )
{
  this->SetInputEnabled( false );

  if( m_pCursor )
  {
    m_pCursor->SetVisible( false );
  }

  m_Seq = SEQ_ANIM_WAIT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool LangSelectUpperDraw::IsEndInOutAnim( bool in )
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
 * @brief   言語選択の開始
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::StartLangSelect( void )
{
  // @fix GFNMCat[3978]
  {
    //      Sound::PlaySE( SEQ_SE_SYS_NJ_016 );   //!< 波の音、潮騒(ループ)
    Sound::StartBGMReq( STRM_BGM_ISTARTING );
  }

  // 言語選択の表示
  m_pDrawerBase->SetVisiblePane( m_pPlatePane, true );
  m_pDrawerBase->SetVisiblePane( m_pCharaPane, true );
  m_pDrawerBase->SetVisiblePane( m_pMantaPane, true );
  m_pDrawerBase->SetVisiblePane( m_pMsgPlate[1], true );
  m_pDrawerBase->SetVisiblePane( m_pDispPane[DISP_MODE_LANG_SELECT], true );

  // カーソルの表示
  if( m_pCursor )
  {
    m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), BTN_ID_LANG_00 );
    m_pCursor->SetVisible( true );
  }

  SetDispMode( DISP_MODE_LANG_SELECT );
}

//------------------------------------------------------------------------------
/**
 * @brief	  このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::SetTitleViewUIListener( LangSelectDrawListener* pListener )
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
void LangSelectUpperDraw::Initialize2D( void* pAppLytBuff )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_LANG_SELECT_LANGSELECT_MIN_UPP_BFLYT,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet );

  this->SetInputEnabled( false );

  {
    // 描画共通処理の生成
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
    {
      GFL_ASSERT( m_pDrawerBase );

      // アニメの再生
      m_pDrawerBase->StartAnim( LA_LANG_SELECT_LANGSELECT_MIN_UPP_BG_LOOP_00 );
      m_pDrawerBase->StartAnim( LA_LANG_SELECT_LANGSELECT_MIN_UPP_CHARA_00 );

      m_pDrawerBase->StartAnim( LA_LANG_SELECT_LANGSELECT_MIN_UPP__MSGPLATE_01_GRPFONT, false );
    }

    // ボタンマネージャの生成
    CreateButtonManager();
    SetDispModeEnable( DISP_MODE_SAVE_DATA );

    // カーソル
    {
      static const u32 c_CursorPosIndex[BTN_ID_NUM][2] = {
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_00, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_01, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_02, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_03, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_04, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_05, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_06, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_07, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_08, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_00, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_NO_00, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_01, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_02, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
        { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_NO_01, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
      };

      gfl2::lyt::LytPane* curPosPane[BTN_ID_NUM];

      for( u32 i=0; i<BTN_ID_NUM; ++i )
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

    // キー入力
    {
      m_pKeyController[DISP_MODE_LANG_SELECT] = GFL_NEW(devHeap) app::util::KeyRepeatController( 9-1, 0 );
      GFL_ASSERT( m_pKeyController[DISP_MODE_LANG_SELECT] );

      m_pKeyController[DISP_MODE_CONFIRM] = GFL_NEW(devHeap) app::util::KeyRepeatController( 2-1, 0 );
      GFL_ASSERT( m_pKeyController[DISP_MODE_CONFIRM] );
      
      m_pKeyController[DISP_MODE_CONFIRM_JPN] = GFL_NEW(devHeap) app::util::KeyRepeatController( 3-1, 0 );
      GFL_ASSERT( m_pKeyController[DISP_MODE_CONFIRM_JPN] );
    }

    // 言語ボタン
    m_pLangButton = GFL_NEW(devHeap) LangSelectButton( m_pDrawerBase, m_LangData );
    GFL_ASSERT( m_pLangButton );

    static const u32 c_DispPaneIndex[DISP_LANG_SELECT_NUM] = {
      PANENAME_LANGSELECT_MIN_UPP_PANE_LANGSELECT,
      PANENAME_LANGSELECT_MIN_UPP_PANE_LANGCONFIRM_00,
      PANENAME_LANGSELECT_MIN_UPP_PANE_LANGCONFIRM_01,
    };

    for( u32 i=0; i<DISP_LANG_SELECT_NUM; ++i )
    {
      m_pDispPane[i] = m_pDrawerBase->GetPane( c_DispPaneIndex[i] );
      m_pDrawerBase->SetVisiblePane( m_pDispPane[i], false );
    }

    // プレート
    m_pPlatePane = m_pDrawerBase->GetPane( PANENAME_LANGSELECT_MIN_UPP_PANE_PLATE_00 );
    m_pDrawerBase->SetVisiblePane( m_pPlatePane, false );
    // キャラ(ヌルペイン)
    m_pCharaPane = m_pDrawerBase->GetPane( PANENAME_LANGSELECT_MIN_UPP_PANE_CHARA );
    m_pDrawerBase->SetVisiblePane( m_pCharaPane, false );
    // マンタ(ヌルペイン)
    m_pMantaPane = m_pDrawerBase->GetPane( PANENAME_LANGSELECT_MIN_UPP_PANE_MANTA );
    m_pDrawerBase->SetVisiblePane( m_pMantaPane, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::InitializeText( void )
{
  {
    // 言語選択画面
    m_pMsgPlate[0] = m_pDrawerBase->GetParts( PANENAME_LANGSELECT_MIN_UPP_PANE_MSGPLATE_00 );
    gfl2::lyt::LytTextBox* textBox = m_pDrawerBase->GetTextBox( PANENAME_LANGSELECT_MSG_UPP_000_PANE_TEXTBOX_00, m_pMsgPlate[0] );
    this->SetTextboxPaneMessage( textBox, language_select_01 );

    // 言語決定確認画面
    m_pMsgPlate[1] = m_pDrawerBase->GetParts( PANENAME_LANGSELECT_MIN_UPP_PANE_MSGPLATE_01 );
    textBox = m_pDrawerBase->GetTextBox( PANENAME_LANGSELECT_MSG_UPP_001_PANE_TEXTBOX_00, m_pMsgPlate[1] );
    m_pDrawerBase->SetVisiblePane( textBox, true );
    m_pDrawerBase->SetVisiblePane( m_pMsgPlate[1], false );
    this->SetTextboxPaneMessage( textBox, language_select_02 );
  }

  {
    static const u32 c_ConfirmBtnIndex[] = {
      PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_00,   //!< 「ではじめる」
      PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_NO_00,    //!< 「言語を選びなおす」
      PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_01,   //!< 「かなではじめる」
      PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_02,   //!< 「漢字ではじめる」
      PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_NO_01,    //!< 「言語を選びなおす」
    };
    static const u32 c_ConfirmMsgID[] = {
      language_select_upper_menu_01,
      language_select_upper_menu_02,
      language_select_upper_menu_03,
      language_select_upper_menu_04,
      language_select_upper_menu_02,
    };

    for( u32 i=0; i<GFL_NELEMS(c_ConfirmBtnIndex); ++i )
    {
      gfl2::lyt::LytParts* confBtnParts = m_pDrawerBase->GetParts( c_ConfirmBtnIndex[i] );
      gfl2::lyt::LytTextBox* btnTextBox =
        m_pDrawerBase->GetTextBox( PANENAME_LANGSELECT_BTN_UPP_001_PANE_TEXTBOX_00, confBtnParts );

      this->SetTextboxPaneMessage( btnTextBox, c_ConfirmMsgID[i] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デフォルト言語のセット
 * @param   langID    言語IG
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::SetDefaultLang( u8 langID )
{
  u32 arrayIdx = 0;
  u32 defaultIdx = 0;

  // デフォルト言語のセット
  for( u32 i=0; i<LANG_ID_MAX; ++i )
  {
    if( langID == c_LangData[i].lang )
    {
      m_LangData[arrayIdx] = c_LangData[i];
      arrayIdx++;
      defaultIdx = i;
    }
  }

  // デフォルト以外の言語を順番にセット
  for( u32 i=0; i<LANG_ID_MAX; ++i )
  {
    if( i != defaultIdx )
    {
      m_LangData[arrayIdx] = c_LangData[i];
      arrayIdx++;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   画面モードの設定
 * @param   dispMode    画面モード
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::SetDispMode( DispMode dispMode )
{
  // 表示切替
  for( u32 i=0; i<DISP_LANG_SELECT_NUM; ++i )
  {
    DispMode mode = static_cast<DispMode>( i );
    m_pDrawerBase->SetVisiblePane( m_pDispPane[i], (mode == dispMode) );
  }

  m_pDrawerBase->SetVisiblePane( m_pMsgPlate[0], (dispMode == DISP_MODE_LANG_SELECT) );
  m_pDrawerBase->SetVisiblePane( m_pMsgPlate[1], (dispMode != DISP_MODE_LANG_SELECT) );

  if( dispMode == DISP_MODE_LANG_SELECT )
  {
    m_CursorPos[dispMode] = m_pLangButton->GetSelectBtn();
    if( m_pCursor ) m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), m_CursorPos[dispMode] );   // @fix cov_ctr[11043]
  }
  else if( dispMode == DISP_MODE_CONFIRM || dispMode == DISP_MODE_CONFIRM_JPN )
  {
    u8 btnIdx = (dispMode == DISP_MODE_CONFIRM) ? 9 : 11;
    m_CursorPos[dispMode] = 0;
    if( m_pCursor ) m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), btnIdx );   // @fix cov_ctr[11043]
  }
  else if( dispMode == DISP_MODE_END_SETUP )
  {
    // カーソル非表示
    if( m_pCursor ) m_pCursor->SetVisible( false );   // @fix cov_ctr[11043]
    // アニメ再生
    m_pDrawerBase->StartAnim( LA_LANG_SELECT_LANGSELECT_MIN_UPP_DEMO_00 );
    // 下画面フェードアウト
    gfl2::math::Vector4 color( 0, 0, 0, 255 );
    m_pFadeManager->ForceOut( gfl2::Fade::DISP_LOWER, &color );
  }
  else if( dispMode == DISP_MODE_TELL )
  {
    // カーソル非表示
    if( m_pCursor ) m_pCursor->SetVisible( false );   // @fix cov_ctr[11043]
    // アニメ再生
    m_pDrawerBase->StartAnim( LA_LANG_SELECT_LANGSELECT_MIN_UPP_DEMO_01 );
    // SE再生
    Sound::PlaySE( SEQ_SE_SD9030_CALLING );
  }

  // 入力モードの変更
  SetDispModeEnable( dispMode );

  // 下画面に通知
  m_uiListener->SetDispMode( dispMode );

  m_DispMode = dispMode;
}

//------------------------------------------------------------------------------
/**
 * @brief   選択言語のセット
 * @param   pos   選択位置
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::SelectLangID( u32 pos )
{
  const u32 animID = LA_LANG_SELECT_LANGSELECT_MIN_UPP__MSGPLATE_01_GRPFONT;
  {
    m_pDrawerBase->SetAnimFrame( animID, m_LangData[pos].frame );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   着地SEの再生
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::PlaySE_Landing( void )
{
  if( !m_PlaySE_Landing )
  {
    if( m_pDrawerBase->GetAnimFrame( CHARA_ANIM ) >= 25.0f )
    {
      Sound::PlaySE( SEQ_SE_SYS_NJ_019 ); //!< ピカチュウが浮き輪に着地

      m_PlaySE_Landing = true;

      GFL_PRINT( "PlaySE_Landing\n" );
    }
  }
}






//==============================================================================
/**
 *      ↓　以下がリスナー関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LangSelectUpperDraw::OnLayoutPaneEvent( const u32 paneId )
{
  app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( m_uiListener == NULL ) return result;

  if( m_DispMode == DISP_MODE_SAVE_DATA ||
      m_DispMode == DISP_MODE_END_SETUP ||
      m_DispMode == DISP_MODE_TELL )
  {
    return result;
  }

  switch( paneId )
  {
  case BTN_ID_LANG_00:
  case BTN_ID_LANG_01:
  case BTN_ID_LANG_02:
  case BTN_ID_LANG_03:
  case BTN_ID_LANG_04:
  case BTN_ID_LANG_05:
  case BTN_ID_LANG_06:
  case BTN_ID_LANG_07:
  case BTN_ID_LANG_08:
    {
      u32 langPos = paneId - BTN_ID_LANG_00;

      // アニメの再生
      m_pDrawerBase->StartAnim( CHARA_ANIM );
      // 選択した言語をセット
      SelectLangID( langPos );
      // 言語決定確認画面へ
      DispMode mode = (m_LangData[langPos].lang == POKEMON_LANG_JAPAN) ? DISP_MODE_CONFIRM_JPN : DISP_MODE_CONFIRM;
      SetDispMode( mode );

      Sound::PlaySE( SEQ_SE_SYS_NJ_017 ); //!< ピカチュウがはねる

      m_DecideFlg = false;
      m_PlaySE_Landing = false;
    } break;

  case BTN_ID_CONFIRM_YES:
  case BTN_ID_CONFIRM_JPN_KANA:
  case BTN_ID_CONFIRM_JPN_KANJI:
    {
      gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
      m_isKanji = (paneId == BTN_ID_CONFIRM_JPN_KANJI);
      m_selectLangIndex = m_pLangButton->GetSelectBtn();

      // 言語のセット
//      m_uiListener->SetLangID( m_LangData[index].lang, bKanji );

      if( m_pAppParam->debugLangSelect != false )
      {
        // 言語のセット
        m_uiListener->SetLangID( m_LangData[m_selectLangIndex].lang, m_isKanji );
        LangSelectClose();
      }
      else
      {
        SetDispMode( DISP_MODE_END_SETUP );
        m_Seq = SEQ_SETUP_WAIT;
      }

      m_DecideFlg = false;
    } break;

  case BTN_ID_CONFIRM_NO:
  case BTN_ID_CONFIRM_JPN_NO:
    {
      // 言語決定確認画面へ
      SetDispMode( DISP_MODE_LANG_SELECT );

      m_DecideFlg = false;
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
app::ui::UIInputListener::ListenerResult LangSelectUpperDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;
  if( m_DecideFlg ) return ENABLE_ACCESS;

  app::tool::ButtonManager* btnMan = this->GetButtonManager();
  gfl2::lyt::LytWk* lytwk = m_pDrawerBase->GetLytWk();

  if( m_DispMode == DISP_MODE_LANG_SELECT )
  {
    app::util::KeyRepeatController* keyRepeat = m_pKeyController[m_DispMode];
    u8 curPos = m_CursorPos[m_DispMode];
    bool is_key_input = false;

    keyRepeat->UpdateInfo( pKey, curPos, 0 );

#if PM_DEBUG
    if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
    {
      btnMan->StartSelectedAct( BTN_ID_LANG_00 + curPos );
      return DISABLE_ACCESS;
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
#else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
#endif
    {
      u32 selectBtn = m_pLangButton->GetSelectBtn();
      btnMan->StartSelectedAct( selectBtn );
      m_DecideFlg = true;
      return DISABLE_ACCESS;
    }
    else if( keyRepeat->IsRepeat( gfl2::ui::DIR_UP ) )
    {
      m_pLangButton->Prev();
      curPos = m_pLangButton->GetSelectBtn();
      is_key_input = true;
    }
    else if( keyRepeat->IsRepeat( gfl2::ui::DIR_DOWN ) )
    {
      m_pLangButton->Next();
      curPos = m_pLangButton->GetSelectBtn();
      is_key_input = true;
    }

    if( m_CursorPos[m_DispMode] != curPos )
    {
      m_pCursor->SetCursorPos( lytwk, curPos );
      m_CursorPos[m_DispMode] = curPos;
      Sound::PlaySE( SEQ_SE_SELECT1 );
    }
    if( is_key_input != false )
    {
      return DISABLE_ACCESS;
    }
  }
  else if( m_DispMode == DISP_MODE_CONFIRM || m_DispMode == DISP_MODE_CONFIRM_JPN )
  {
    app::util::KeyRepeatController* keyRepeat = m_pKeyController[m_DispMode];
    u8 curPos = m_CursorPos[m_DispMode];
    u8 btnNum = (m_DispMode == DISP_MODE_CONFIRM) ? 2 : 3;
    u32 btnID = (m_DispMode == DISP_MODE_CONFIRM) ? BTN_ID_CONFIRM_YES + curPos : BTN_ID_CONFIRM_JPN_KANA + curPos;
    bool is_key_input = false;

    keyRepeat->UpdateInfo( pKey, curPos, 0 );

#if PM_DEBUG
    if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
    {
      if( curPos < (btnNum - 1) )
      {
        m_pAppParam->debugLangSelect = true;    //!< デバッグモードの起動：ランチャーへ移動
        btnMan->StartSelectedAct( btnID );
      }
      return DISABLE_ACCESS;
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
#else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
#endif
    {
      btnMan->StartSelectedAct( btnID );
      m_DecideFlg = true;
      return DISABLE_ACCESS;
    }
/*
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      btnMan->StartSelectedAct( BTN_ID_CONFIRM_NO );
      return DISABLE_ACCESS;
    }
*/
    else if( keyRepeat->IsRepeat( gfl2::ui::DIR_UP ) )
    {
      curPos = (curPos + btnNum - 1) % btnNum;
      is_key_input = true;
    }
    else if( keyRepeat->IsRepeat( gfl2::ui::DIR_DOWN ) )
    {
      curPos = (curPos + 1) % btnNum;
      is_key_input = true;
    }

    if( m_CursorPos[m_DispMode] != curPos )
    {
      u8 btnIdx = (m_DispMode == DISP_MODE_CONFIRM) ? curPos + 9 : curPos + 11;
      m_pCursor->SetCursorPos( lytwk, btnIdx );
      m_CursorPos[m_DispMode] = curPos;
      Sound::PlaySE( SEQ_SE_SELECT1 );
    }
    if( is_key_input != false )
    {
      return DISABLE_ACCESS;
    }
  }
  else if( m_DispMode == DISP_MODE_SAVE_DATA )
  {
    if( pButton->IsHold( gfl2::ui::BUTTON_B ) &&
      pButton->IsHold( gfl2::ui::BUTTON_X ) &&
      pButton->IsHold( gfl2::ui::BUTTON_UP ))
    {
      m_uiListener->InputSavedataDeleteCommand();
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      m_uiListener->InputDecide( true );
    }
    else if( pButton->IsRelease( gfl2::ui::BUTTON_A ) )
    {
      m_uiListener->InputDecide( false );
    }
  }
  else if( m_DispMode == DISP_MODE_END_SETUP )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      if( IsEndSettingSeq() != false )
      {
        Sound::PlaySE( SEQ_SE_MESSAGE );
        EndMessage();
        StartSettingWait();
        m_Seq = SEQ_SETUP_CALL_SE_START;
      }
      return DISABLE_ACCESS;
    }
  }
  else if( m_DispMode == DISP_MODE_TELL )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      if( IsEndCallMessageSeq() != false )
      {
        Sound::PlaySE( SEQ_SE_MESSAGE );
        EndMessage();
        LangSelectClose();
      }
      return DISABLE_ACCESS;
    }
  }

  return ENABLE_ACCESS;
}





//==============================================================================
/**
 *      ↓　以下がボタンマネージャ関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_LANG_00, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_00_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_00_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_00_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_00_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_00_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_00_PASSIVE,
    },
    {
      BTN_ID_LANG_01, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_01_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_01_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_01_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_01_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_01_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_01_PASSIVE,
    },
    {
      BTN_ID_LANG_02, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_02_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_02_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_02_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_02_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_02_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_02_PASSIVE,
    },
    {
      BTN_ID_LANG_03, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_03_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_03_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_03_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_03_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_03_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_03_PASSIVE,
    },
    {
      BTN_ID_LANG_04, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_04_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_04_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_04_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_04_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_04_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_04_PASSIVE,
    },
    {
      BTN_ID_LANG_05, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_05_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_05_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_05_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_05_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_05_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_05_PASSIVE,
    },
    {
      BTN_ID_LANG_06, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_06_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_06_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_06_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_06_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_06_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_06_PASSIVE,
    },
    {
      BTN_ID_LANG_07, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_07_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_07_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_07_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_07_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_07_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_07_PASSIVE,
    },
    {
      BTN_ID_LANG_08, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_08_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_08_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_08_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_08_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_08_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_08_PASSIVE,
    },
    {
      BTN_ID_CONFIRM_YES, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_00_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_00_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_00_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_00_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_00_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_00_PASSIVE,
    },
    {
      BTN_ID_CONFIRM_NO, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_00_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_00_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_00_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_00_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_00_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_00_PASSIVE,
    },
    {
      BTN_ID_CONFIRM_JPN_KANA, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_01_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_01_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_01_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_01_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_01_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_01_PASSIVE,
    },
    {
      BTN_ID_CONFIRM_JPN_KANJI, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_02_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_02_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_02_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_02_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_02_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_YES_02_PASSIVE,
    },
    {
      BTN_ID_CONFIRM_JPN_NO, NULL, NULL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_01_TOUCH,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_01_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_01_CANSEL,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_01_TOUCH_RELEASE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_01_ACTIVE,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_NO_01_PASSIVE,
    },
  };

  const gfl2::lyt::LytPaneIndex c_ButtonPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_00, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_01, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_02, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_03, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_04, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_05, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_06, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_07, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_08, PANENAME_LANGSELECT_BTN_UPP_000_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_00, PANENAME_LANGSELECT_BTN_UPP_001_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_NO_00, PANENAME_LANGSELECT_BTN_UPP_001_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_01, PANENAME_LANGSELECT_BTN_UPP_001_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_02, PANENAME_LANGSELECT_BTN_UPP_001_PANE_BOUND_00 },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_NO_01, PANENAME_LANGSELECT_BTN_UPP_001_PANE_BOUND_00 },
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

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  btnMan->SetInputMode( app::tool::ButtonManager::INPUT_MODE_KEY_ONLY );   //!< キーのみ入力可
  btnMan->SetButtonBindKey( BTN_ID_CONFIRM_NO, gfl2::ui::BUTTON_B );
  btnMan->SetButtonBindKey( BTN_ID_CONFIRM_JPN_NO, gfl2::ui::BUTTON_B );

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }

  GFL_DELETE_ARRAY info;
}

//------------------------------------------------------------------------------
/**
 * @brief   画面モードごとのボタン設定
 * @param   dispMode    画面モード
 */
//------------------------------------------------------------------------------
void LangSelectUpperDraw::SetDispModeEnable( DispMode dispMode )
{
  u32 start = 0;
  u32 end = 0;

  if( dispMode == DISP_MODE_LANG_SELECT )
  {
    start = BTN_ID_LANG_00;
    end = BTN_ID_LANG_08;
  }
  else if( dispMode == DISP_MODE_CONFIRM )
  {
    start = BTN_ID_CONFIRM_YES;
    end = BTN_ID_CONFIRM_NO;
  }
  else if( dispMode == DISP_MODE_CONFIRM_JPN )
  {
    start = BTN_ID_CONFIRM_JPN_KANA;
    end = BTN_ID_CONFIRM_JPN_NO;
  }

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    bool enable = (i >= start) && (i <= end);
    btnMan->SetButtonInputEnable( i, enable );
  }
}


//------------------------------------------------------------------
/**
 * @brief   TalkWindow生成
 * @param   res_buf   リソースバッファ
 */
//------------------------------------------------------------------
void LangSelectUpperDraw::CreateTalkWindow( void * res_buf )
{
  m_pTalkWindow = GFL_NEW(m_pHeap->GetDeviceHeap()) App::Tool::TalkWindow( m_pHeap, m_pAppRenderingManager );
  m_pTalkWindow->SetWindowType( App::Tool::WIN_TYPE_SYS_DOWN );
  m_pTalkWindow->Setup( res_buf );
  m_pTalkWindow->InitMsgData( m_pMsgData );
  m_pTalkWindow->SetEnableMessageBoost( false );  //!< Boost OFF
  m_pTalkWindow->SetMessageSpeed( Savedata::ConfigSave::MSG_NORMAL );   //!< 速度：ふつう
  this->AddSubView( m_pTalkWindow );
}

//------------------------------------------------------------------
/**
 * @brief   TalkWindow削除
 */
//------------------------------------------------------------------
void LangSelectUpperDraw::DeleteTalkWindow(void)
{
  if( m_pTalkWindow != NULL )
  {
    m_pTalkWindow->RemoveFromSuperView();
    m_pTalkWindow->End();
    GFL_SAFE_DELETE( m_pTalkWindow );
  }
}

//------------------------------------------------------------------
/**
 * @brief   メッセージ表示開始
 * @param   id  メッセージID
 */
//------------------------------------------------------------------
void LangSelectUpperDraw::PrintMessage( u32 id )
{
  m_printMsgID = id;
  m_pTalkWindow->SetMessage( id );
  m_pTalkWindow->CallMsgWin();
  m_talkStartFlag = true;   // 呼び出し時に終了を返してしまうため
}

//------------------------------------------------------------------
/**
 * @brief   メッセージ表示終了チェック
 * @retval  true  = 表示中
 * @retval  false = 終了
 */
//------------------------------------------------------------------
bool LangSelectUpperDraw::IsPrintMessage(void)
{
  gfl2::str::StrWin::Result res = m_pTalkWindow->GetStrWinResult();

  if( res == gfl2::str::StrWin::RES_DONE )
  {
    if( m_talkStartFlag != false )
    {
      m_talkStartFlag = false;
    }
    else
    {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   メッセージ終了処理
 */
//------------------------------------------------------------------
void LangSelectUpperDraw::EndMessage(void)
{
  m_pTalkWindow->SetMsgCursorUserVisible( true, false );
  m_pTalkWindow->CloseMsgWin();
}


//------------------------------------------------------------------
/**
 * @brief   設定終了の次へ進めるか
 * @retval  true  = ok
 * @retval  false = ng
 */
//------------------------------------------------------------------
bool LangSelectUpperDraw::IsEndSettingSeq(void)
{
  if( m_Seq == SEQ_SETUP_MSG_SKIP_WAIT &&
      IsSettingWait() == false &&
      m_pDrawerBase->IsAnimEnd(LA_LANG_SELECT_LANGSELECT_MIN_UPP_DEMO_02) != false )
  {
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   博士からの連絡メッセージの次へ進めるか
 * @retval  true  = ok
 * @retval  false = ng
 */
//------------------------------------------------------------------
bool LangSelectUpperDraw::IsEndCallMessageSeq(void)
{
  if( m_Seq == SEQ_SETUP_MSG_SKIP_WAIT && IsSettingWait() == false )
  {
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   言語選択を変更できるタイミングか
 * @retval  true  = ok
 * @retval  false = ng
 */
//------------------------------------------------------------------
bool LangSelectUpperDraw::IsSetLangFrame(void)
{
  return m_pDrawerBase->IsAnimEnd( LA_LANG_SELECT_LANGSELECT_MIN_UPP_DEMO_00 );
}

//------------------------------------------------------------------
/**
 * @brief   メッセージ関連のデータを再設定
 * @param   msg_data  メッセージデータ
 * @param   word_set  単語セット
 */
//------------------------------------------------------------------
void LangSelectUpperDraw::ResetMessage( gfl2::str::MsgData * msg_data, print::WordSet * word_set )
{
  m_pMsgData = msg_data;
  m_pWordSet = word_set;
  GetG2DUtil()->SetMessageData( m_pMsgData, m_pWordSet );
  CreateTalkWindow( m_talkWindowLytBuff );
}

//------------------------------------------------------------------
/**
 * @brief   設定後のウェイト開始
 */
//------------------------------------------------------------------
void LangSelectUpperDraw::StartSettingWait( bool flg )
{
  m_isPutMsgArrow = flg;
  m_settingWait = 0;
  m_settingWaitFlag = true;
}

//------------------------------------------------------------------
/**
 * @brief   設定後のウェイト更新処理
 */
//------------------------------------------------------------------
void LangSelectUpperDraw::UpdateSettingWait(void)
{
  if( m_settingWaitFlag != false )
  {
    if( m_settingWait >= SETTING_WAIT_COUNT )
    {
      if( m_isPutMsgArrow != false )
      {
        m_pTalkWindow->SetMsgCursorUserVisible( true, true );
      }
      m_settingWaitFlag = false;
    }
    else
    {
      m_settingWait++;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   設定後のウェイト中か
 * @retval  true  = ウェイト中
 * @retval  false = それ以外
 */
//------------------------------------------------------------------
bool LangSelectUpperDraw::IsSettingWait(void)
{
  return m_settingWaitFlag;
}


//------------------------------------------------------------------
/**
 * @brief   博士からの呼び出しSEをリプレイ
 * @retval  true  = 再生した
 * @retval  false = それ以外
 */
//------------------------------------------------------------------
bool LangSelectUpperDraw::ReplayCallSE(void)
{
  if( m_pDrawerBase->IsAnimEnd(LA_LANG_SELECT_LANGSELECT_MIN_UPP_DEMO_01) == false )
  {
    if( m_pDrawerBase->GetAnimFrame(LA_LANG_SELECT_LANGSELECT_MIN_UPP_DEMO_01) == 0 )
    {
      Sound::PlaySE( SEQ_SE_SD9030_CALLING );
      return true;
    }
  }
  return false;
}


GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )
