//======================================================================
/**
 * @file    FinderStudioCapturePrepareSelectView.cpp
 * @date    2017/01/18 11:46:50
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：準備選択画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCapturePrepareSelectView.h"
#include "FinderStudioCaptureViewListener.h"
#include "FinderStudioCapturePrepareCommonView.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <niji_conv_header/app/studio/FinderStudioCapture.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_pane.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_anim_list.h>
#include <niji_conv_header/message/msg_fs_photo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
PrepareSelectView::PrepareSelectView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf )
  : CaptureViewBase( heap, renderMan, cursorBuf )
  , m_pPreCmnViewListener( NULL )
  , m_pAuthenticPane( NULL )
  , m_pEasyPane( NULL )
  , m_ButtonNum( PREPARE_BTN_NUM )
  , m_DifficultyMode( Static::DIFF_MODE_NUM )
{
  for( u32 i = 0; i < PREPARE_BTN_NUM; ++i )
  {
    m_pTitleText[i] = NULL;
    m_pContentsText[i] = NULL;
  }

  for( u32 i = 0; i < PREPARE_AUTH_BOTH_BTN_NUM; ++i )
  {
    m_pAuthenticCursorPane[i] = NULL;
  }

  for( u32 i = 0; i < PREPARE_EASY_BTN_NUM; ++i )
  {
    m_pEasyCursorPane[i] = NULL;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void PrepareSelectView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // ボタンの初期化
  InitButton();
  // テキストの初期化
  InitText();
  // カーソルの初期化
  this->CreateMenuCursor();
}

/**
 * @brief   2Dの初期化
 */
void PrepareSelectView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1, resTbl, 1, setupData, 1, msgData, wordSet );

  // UIInputListenerのセット
  this->SetInputListener( this );
  // Viewの初期設定
  this->_InitView();
}

/**
 * @brief   ペインの初期化
 */
void PrepareSelectView::InitPane( void )
{
  // ボタンテキスト
  const u32 c_SelectButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_04,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_05,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_06,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_SelectButtonParts ); ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_SelectButtonParts[i] );

    m_pTitleText[i] = m_pUtil->GetTextBox( PANENAME_STUDIOPARTS_BUTTON_12_PANE_TEXTBOX_00, parts );
    m_pContentsText[i] = m_pUtil->GetTextBox( PANENAME_STUDIOPARTS_BUTTON_12_PANE_TEXTBOX_01, parts );
  }

  //　カーソル
  const u32 c_AuthenticButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_04,
  };
  const u32 c_EasyButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_05,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_06,
  };

  for( u32 i = 0; i < PREPARE_AUTH_BOTH_BTN_NUM; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_AuthenticButtonParts[i] );
    m_pAuthenticCursorPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_12_PANE_NULL_CUR, parts );
  }
  for( u32 i = 0; i < PREPARE_EASY_BTN_NUM; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_EasyButtonParts[i] );
    m_pEasyCursorPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_12_PANE_NULL_CUR, parts );
  }

  // ヌルペイン
  m_pAuthenticPane = m_pUtil->GetPane( PANENAME_STUDIO_LIST_LOW_01_PANE_NULL_AUTHENTIC );
  m_pEasyPane = m_pUtil->GetPane( PANENAME_STUDIO_LIST_LOW_01_PANE_NULL_EASY );
}

/**
 * @brief   テキストの初期化
 */
void PrepareSelectView::InitText( void )
{
  const u32 c_PrepareTitle[] = {
    fs_photo_cap_05,
    fs_photo_cap_06,
    fs_photo_cap_07,
    fs_photo_cap_08,
    fs_photo_cap_09,
    fs_photo_cap_10,
    fs_photo_cap_11,
  };
  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    SetTextboxPaneMessage( m_pTitleText[i], c_PrepareTitle[i] );
  }
}

/**
 * @brief   ボタンの初期化
 */
void PrepareSelectView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01__BUTTON_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01__BUTTON_01_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01__BUTTON_02_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01__BUTTON_03_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01__BUTTON_04_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01__BUTTON_05_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_01__BUTTON_06_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_04,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_05,
    PANENAME_STUDIO_LIST_LOW_01_PANE_BUTTON_06,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_BUTTON_12_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_12_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_12_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_12_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_12_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_12_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_12_PANE_BOUND_00,
  };

  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    Static::ButtonInfo::SetupBtnInfo( &info[i], i, c_ButtonAnim[i] );

    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ButtonParts[i] );
    gfl2::lyt::LytPane*  bound = m_pUtil->GetPane( c_ButtonBounding[i], parts );

    info[i].picture_pane = parts;
    info[i].bound_pane = bound;
  }

  // ボタンマネージャの生成
  CreateButtonManager( m_pHeap, m_pUtil->GetLytWk(), info, BTN_ID_NUM );

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
  {
    // SE
    for( u32 i = 0; i < BTN_ID_NUM; ++i )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   終了
 */
bool PrepareSelectView::Terminate( void )
{
  // キーリピートコントローラーの破棄
  this->DeleteKeyRepeatController();
  // メニューカーソルの破棄
  this->DeleteMenuCursor();
  // View設定の解除
  this->_TermView();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新・描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
void PrepareSelectView::Update( void )
{
  this->_Update();

  if( !_IsButtonAction() )
  {
    /// 共通ボタン
    _UpdateCommonButton();
  }
}

/**
* @brief   更新：共通ボタン
*/
void PrepareSelectView::_UpdateCommonButton( void )
{
  if( !m_bDrawEnable )
  {
    return;
  }
  if( !m_pPreCmnViewListener )
  {
    return;
  }

  /// ボタンアクション開始なので有効に
  m_pPreCmnViewListener->SetInputEnabledCommonButton( true );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void PrepareSelectView::Show( void )
{
  // キーリピートの生成
  this->CreateKeyRepeatController( m_ButtonNum, 0 );
  // ボタンのテキスト設定
  _SetButtonText( m_DifficultyMode );

  this->_Show();

  gfl2::lyt::LytPane* pane = _GetCursorPane( m_CursorPos );
  this->PutCursor( pane );
}

/**
 * @brief   非表示
 */
void PrepareSelectView::Hide( void )
{
  // キーリピートの破棄
  this->DeleteKeyRepeatController();

  this->_Hide();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   難易度の設定
 */
void PrepareSelectView::SetDifficulty( Static::DifficultyMode mode )
{
  if( m_DifficultyMode != mode )
  {
    // 難易度が変更された時のみ位置をリセット
    m_CursorPos = 0;
    //難易度の更新
    m_DifficultyMode = mode;
  }

  // ボタンの数を設定
  _SetButtonNum( mode );
  // ボタンの表示設定
  _SetButtonVisible( mode );
  // ボタンの有効設定
  _SetButtonEnable( mode );
  // ボタンテキストの設定
  _SetButtonText( mode );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ボタン設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンの数を設定
 */
void PrepareSelectView::_SetButtonNum( Static::DifficultyMode mode )
{
  if( mode == Static::DIFF_MODE_EASY )
  {
    m_ButtonNum = PREPARE_EASY_BTN_NUM;
  }
  else if( _IsCaptureModeBoth() )
  {
    m_ButtonNum = PREPARE_AUTH_BOTH_BTN_NUM;
  }
  else {
    m_ButtonNum = PREPARE_AUTH_POKE_BTN_NUM;
  }
}

/**
 * @brief   ボタンの表示設定
 */
void PrepareSelectView::_SetButtonVisible( Static::DifficultyMode mode )
{
  m_pUtil->SetVisiblePane( m_pAuthenticPane, ( mode == Static::DIFF_MODE_AUTHENTIC ) );
  m_pUtil->SetVisiblePane( m_pEasyPane, ( mode == Static::DIFF_MODE_EASY ) );

  // 衣装選択ボタンの表示切替
  if( mode == Static::DIFF_MODE_AUTHENTIC )
  {
    app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

    // ツーショットかどうか
    if( _IsCaptureModeBoth() )
    {
      btnMan->SetButtonVisible( BTN_ID_AUTHENTIC_03 );
      btnMan->SetButtonVisible( BTN_ID_AUTHENTIC_04 );
    }
    else {
      btnMan->SetButtonInvisible( BTN_ID_AUTHENTIC_03 );
      btnMan->SetButtonInvisible( BTN_ID_AUTHENTIC_04 );
    }
  }
}

/**
 * @brief   ボタンの有効設定
 */
void PrepareSelectView::_SetButtonEnable( Static::DifficultyMode mode )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonInputEnable( i, false );
  }

  if( m_DifficultyMode == Static::DIFF_MODE_AUTHENTIC )
  {
    btnMan->SetButtonInputEnable( BTN_ID_AUTHENTIC_00, true );
    btnMan->SetButtonInputEnable( BTN_ID_AUTHENTIC_01, true );
    btnMan->SetButtonInputEnable( BTN_ID_AUTHENTIC_02, true );
    btnMan->SetButtonInputEnable( BTN_ID_AUTHENTIC_03, _IsCaptureModeBoth() );
    btnMan->SetButtonInputEnable( BTN_ID_AUTHENTIC_04, _IsCaptureModeBoth() );
  }
  else {
    btnMan->SetButtonInputEnable( BTN_ID_EASY_00, true );
    btnMan->SetButtonInputEnable( BTN_ID_EASY_01, true );
  }
}

/**
 * @brief   ボタンのテキストを設定
 */
void PrepareSelectView::_SetButtonText( Static::DifficultyMode mode )
{
  void ( PrepareSelectView::*SET_BUTTON_TEXT_AUTHENTIC[] )( u32 ) = {
    &PrepareSelectView::_SetButtonText_CaptureMode,
    &PrepareSelectView::_SetButtonText_Pokemon,
    &PrepareSelectView::_SetButtonText_BackGround,
    &PrepareSelectView::_SetButtonText_Composition,
    &PrepareSelectView::_SetButtonText_Dressup,
  };
  void ( PrepareSelectView::*SET_BUTTON_TEXT_EASY[] )( u32 ) = {
    &PrepareSelectView::_SetButtonText_Pokemon,
    &PrepareSelectView::_SetButtonText_Composition,
  };

  for( u32 i = 0; i < m_ButtonNum; ++i )
  {
    if( mode == Static::DIFF_MODE_AUTHENTIC )
    {
      ( this->*SET_BUTTON_TEXT_AUTHENTIC[i] )( i );
    }
    else {
      ( this->*SET_BUTTON_TEXT_EASY[i] )( i + BTN_ID_EASY_00 );
    }
  }
}

/**
 * @brief   ボタンのテキスト設定：撮影モード
 */
void PrepareSelectView::_SetButtonText_CaptureMode( u32 index )
{
  SetTextboxPaneMessage( m_pTitleText[index], fs_photo_cap_06 );

  const u32 c_CaotureModeText[] = {
    fs_photo_cap_01,
    fs_photo_cap_02,
  };
  Viewer::CaptureMode mode = m_pWork->prepare.capture_mode;
  SetTextboxPaneMessage( m_pContentsText[index], c_CaotureModeText[mode] );
}

/**
 * @brief   ボタンのテキスト設定：ポケモン名
 */
void PrepareSelectView::_SetButtonText_Pokemon( u32 index )
{
  SetTextboxPaneMessage( m_pTitleText[index], fs_photo_cap_07 );

  pml::pokepara::PokemonParam* pp = m_pWork->prepare.pokemon_param;

  m_pUtil->GetG2D()->SetRegisterPokeNickName( 0, pp );
  SetTextboxPaneMessage( m_pContentsText[index], fs_photo_cap_03 );
}

/**
 * @brief   ボタンのテキスト設定：背景名
 */
void PrepareSelectView::_SetButtonText_BackGround( u32 index )
{
  // 背景ID
  u32 background_id = m_pWork->prepare.background_id;
  // 背景UIデータ
  BGDataAccessor::UIData data = BGDataAccessor::GetUIData( background_id );

  // 項目名テキストの表示
  SetTextboxPaneMessage( m_pTitleText[index], fs_photo_cap_08 );
  // 項目内用テキストの表示
  SetTextboxPaneMessage( m_pContentsText[index], data.msgID );
}

/**
 * @brief   ボタンのテキスト設定：衣装名
 */
void PrepareSelectView::_SetButtonText_Dressup( u32 index )
{
  const u32 c_DressUpText[] = {
    fs_photo_cap_05,
    fs_photo_cap_98,
    fs_photo_cap_99,
    fs_photo_cap_100,
    fs_photo_cap_102,
    fs_photo_cap_103,
    fs_photo_cap_101,
  };

  SetTextboxPaneMessage( m_pTitleText[index], fs_photo_cap_09 );

  u32 dressup_id = m_pWork->prepare.dressup_id;
  SetTextboxPaneMessage( m_pContentsText[index], c_DressUpText[dressup_id] );
}

/**
 * @brief   ボタンのテキスト設定：構図名
 */
void PrepareSelectView::_SetButtonText_Composition( u32 index )
{
  const u32 c_CompositionText[] = {
    fs_photo_cap_96,
    fs_photo_cap_97,
  };

  SetTextboxPaneMessage( m_pTitleText[index], fs_photo_cap_10 );

  Viewer::Composition composition = m_pListener->GetComposition();
  GFL_ASSERT( composition < GFL_NELEMS( c_CompositionText ) );

  SetTextboxPaneMessage( m_pContentsText[index], c_CompositionText[composition] );
}






//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーソル
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カーソル移動
 */
void PrepareSelectView::_MoveCursor( u32 nextPos )
{
  m_CursorPos = nextPos % m_ButtonNum;

  gfl2::lyt::LytPane* pane = _GetCursorPane( m_CursorPos );
  this->PutCursor( pane );

  Sound::PlaySE( SEQ_SE_SELECT1 );
}

/**
 * @brief   カーソルペインの取得
 */
gfl2::lyt::LytPane* PrepareSelectView::_GetCursorPane( u32 pos )
{
  if( m_DifficultyMode == Static::DIFF_MODE_AUTHENTIC )
  {
    return m_pAuthenticCursorPane[pos];
  }

  return m_pEasyCursorPane[pos];
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   撮影モードがツーショットかどうか
 *
 * @retval  "true"ならツーショット
 * @retval  "false"ならポケモンのみ
 */
bool PrepareSelectView::_IsCaptureModeBoth( void ) const
{
  return ( m_pWork->prepare.capture_mode == Viewer::CAPTURE_MODE_BOTH );
}


/**
 * @brief   ボタンがアクションか
 *
 * @retval  "true"なら何かしらアクション中
 * @retval  "false"ならそれ以外
 */
bool PrepareSelectView::_IsButtonAction( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  return ( btnMan->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リザルト
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   リザルトの設定
 */
void PrepareSelectView::_SetViewResult( const u32 paneId )
{
  const u32 c_ViewResult[] = {
    RES_CAPTURE_MODE,   //!< 本格：撮影モード
    RES_POKEMON,        //!< 本格：ポケモン選択
    RES_BACKGROUND,     //!< 本格：背景選択
    RES_COMPOSITION,    //!< 本格：構図選択
    RES_DRESSUP,        //!< 本格：衣装選択
    RES_POKEMON,        //!< お手軽：ポケモン選択
    RES_COMPOSITION,    //!< お手軽：構図選択
  };

  this->SetResult( c_ViewResult[paneId] );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンアクション開始イベント
 */
void PrepareSelectView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  m_CursorPos = paneId;

  if( m_DifficultyMode != Static::DIFF_MODE_AUTHENTIC )
  {
    m_CursorPos -= BTN_ID_EASY_00;
  }

  gfl2::lyt::LytPane* pane = _GetCursorPane( m_CursorPos );
  this->PutCursor( pane );

  /// ボタンが選択されたことを通知
  this->SetResult( RES_SELECTED_ITEM );   //!< @fix MMCat[108]

  /// ボタンアクション開始なので無効に
  m_pPreCmnViewListener->SetInputEnabledCommonButton( false );
}

/**
 * @brief   ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult PrepareSelectView::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_pListener == NULL ) return ENABLE_ACCESS;

  // Viewリザルトのセット
  _SetViewResult( paneId );

  return ENABLE_ACCESS;
}

/**
 * @brief   キーアクションイベント
 */
app::ui::UIInputListener::ListenerResult PrepareSelectView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  m_pKeyController->UpdateInfo( key, m_CursorPos, 0 );

  if( m_pKeyController->IsRepeat( gfl2::ui::DIR_UP ) )
  {
    u32 nextPos = m_CursorPos + m_ButtonNum - 1;
    _MoveCursor( nextPos );
  }
  else if( m_pKeyController->IsRepeat( gfl2::ui::DIR_DOWN ) )
  {
    u32 nextPos = m_CursorPos + 1;
    _MoveCursor( nextPos );
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

    u32 cursorPos = m_CursorPos;

    if( m_DifficultyMode != Static::DIFF_MODE_AUTHENTIC )
    {
      cursorPos += BTN_ID_EASY_00;
    }

    btnMan->StartSelectedAct( cursorPos );
  }

  return ENABLE_ACCESS;
}
GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
