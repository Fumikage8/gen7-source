//======================================================================
/**
 * @file    FinderStudioCapturePrepareCommonView.cpp
 * @date    2017/01/18 18:35:46
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：準備共通View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCapturePrepareCommonView.h"

#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewListener.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>

#include <Savedata/include/FinderStudioSave.h>

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
PrepareCommonView::PrepareCommonView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : CaptureViewBase( heap, renderMan )
  , m_ViewType( VIEW_TYPE_NULL )
  , m_pMessageText( NULL )
  , m_MainSeq( SEQ_BEGIN )
  , m_InfoPageNum( 0 )
  , m_bTutorial( false )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void PrepareCommonView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // テキストの初期化
  InitText();
  // ボタンの初期化
  InitButton();
}

/**
 * @brief   2dの初期化
 */
void PrepareCommonView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_01_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_01___NUM,
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
void PrepareCommonView::InitPane( void )
{
  gfl2::lyt::LytParts* parts = m_pUtil->GetParts( PANENAME_STUDIO_COMMONPARTS_LOW_01_PANE_MESSAGE_L1 );
  m_pMessageText = m_pUtil->GetTextBox( PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, parts );
}

/**
 * @brief   テキストの初期化
 */
void PrepareCommonView::InitText( void )
{
  gfl2::lyt::LytParts* cameraBtnParts = m_pUtil->GetParts( PANENAME_STUDIO_COMMONPARTS_LOW_01_PANE_CAMERABUTTON );
  gfl2::lyt::LytTextBox* cameraBtnText = m_pUtil->GetTextBox( PANENAME_COMMON_BTN_LOW_016_PANE_TEXTBOX_00, cameraBtnParts );

  this->SetTextboxPaneMessage( cameraBtnText, fs_photo_cap_12 );
}

/**
 * @brief   ボタンの初期化
 */
void PrepareCommonView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_01__BACKBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_01__CAMERABUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_01__BUTTON_KISEKAE_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_01__INFOBUTTON_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_COMMONPARTS_LOW_01_PANE_BACKBUTTON,
    PANENAME_STUDIO_COMMONPARTS_LOW_01_PANE_CAMERABUTTON,
    PANENAME_STUDIO_COMMONPARTS_LOW_01_PANE_BUTTON_KISEKAE,
    PANENAME_STUDIO_COMMONPARTS_LOW_01_PANE_INFOBUTTON,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_016_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
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
    // Bind
    btnMan->SetButtonBindKey( BTN_ID_BACK, gfl2::ui::BUTTON_B );
    btnMan->SetButtonBindKey( BTN_ID_NEXT, gfl2::ui::BUTTON_R );
    btnMan->SetButtonBindKey( BTN_ID_KISEKAE, gfl2::ui::BUTTON_Y );

    // SE
    btnMan->SetButtonSelectSE( BTN_ID_BACK, SEQ_SE_CANCEL1 );
    btnMan->SetButtonSelectSE( BTN_ID_NEXT, SEQ_SE_DECIDE1 );
    btnMan->SetButtonSelectSE( BTN_ID_KISEKAE, SEQ_SE_DECIDE1 );
  }

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   終了
 */
bool PrepareCommonView::Terminate( void )
{
  // View設定の解除
  this->_TermView();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void PrepareCommonView::Show( void )
{
  // 表示
  this->_Show();

  m_MainSeq = SEQ_MAIN;
}

/**
 * @brief   表示：チュートリアル
 */
void PrepareCommonView::ShowTutorial( void )
{
  if( m_bTutorial )
  {
    m_MainSeq = SEQ_SHOW_INFO;

    m_bTutorial = false;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新・描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
void PrepareCommonView::Update( void )
{
  this->_Update();

  _UpdateSequence();
}

void PrepareCommonView::_UpdateSequence( void )
{
  switch( m_MainSeq )
  {
    // インフォ表示
    case SEQ_SHOW_INFO:
    {
      _ShowInfo();
      m_MainSeq = SEQ_INFO;
    } break;
  }
}

/**
 * @brief   描画
 */
void PrepareCommonView::Draw( gfl2::gfx::CtrDisplayNo no )
{
  this->_Draw( no );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示設定
 *
 * @param   type    表示タイプ
 */
void PrepareCommonView::SetViewType( ViewType type )
{
  // テキストの表示
  _SetViewType_Text( type );
  // ボタンの表示
  _SetViewType_Button( type );
  // チュートリアルフラグの確認
  m_bTutorial = _IsFirstTutorial( type );

  m_ViewType = type;
}

/**
 * @brief   テキストの表示
 */
void PrepareCommonView::_SetViewType_Text( ViewType type )
{
  const u32 c_MessageLabel[] = {
    fs_photo_win_58,  //!< VIEW_TYPE_DIFFICULTY
    fs_photo_win_59,  //!< VIEW_TYPE_PREPARE
    fs_photo_win_18,  //!< VIEW_TYPE_CAPTURE_MODE
    fs_photo_win_19,  //!< VIEW_TYPE_POKEMON
    fs_photo_win_20,  //!< VIEW_TYPE_BACKGROUND
    fs_photo_win_21,  //!< VIEW_TYPE_DRESSUP
    fs_photo_win_36,  //!< VIEW_TYPE_COMPOSITION
    fs_photo_win_62,  //!< VIEW_TYPE_COMPOSITION_DISABLE
    fs_photo_win_22,  //!< VIEW_TYPE_FINISH_DIFFICULTY
    fs_photo_win_22,  //!< VIEW_TYPE_FINISH_PREPARE
    fs_photo_win_61,  //!< VIEW_TYPE_FINISH_MAIN
  };

  GFL_ASSERT( type < VIEW_TYPE_NUM );
  SetTextboxPaneMessage( m_pMessageText, c_MessageLabel[type] );
  m_pUtil->SetVisiblePane( m_pMessageText, true );
}

/**
 * @brief   ボタンの表示
 */
void PrepareCommonView::_SetViewType_Button( ViewType type )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  switch( type )
  {
    case VIEW_TYPE_PREPARE:
    {
      btnMan->SetButtonVisible( BTN_ID_BACK );
      btnMan->SetButtonVisible( BTN_ID_NEXT );
      btnMan->SetButtonInvisible( BTN_ID_KISEKAE );
      btnMan->SetButtonVisible( BTN_ID_INFO );
    } break;

    case VIEW_TYPE_FINISH_DIFFICULTY:
    case VIEW_TYPE_FINISH_PREPARE:
    case VIEW_TYPE_FINISH_MAIN:
    {
      btnMan->SetButtonInvisible( BTN_ID_BACK );
      btnMan->SetButtonInvisible( BTN_ID_NEXT );
      btnMan->SetButtonInvisible( BTN_ID_KISEKAE );
      btnMan->SetButtonInvisible( BTN_ID_INFO );
    } break;

    case VIEW_TYPE_DIFFICULTY:
    case VIEW_TYPE_BACKGROUND:
    case VIEW_TYPE_DRESSUP:
    {
      btnMan->SetButtonVisible( BTN_ID_BACK );
      btnMan->SetButtonInvisible( BTN_ID_NEXT );
      btnMan->SetButtonInvisible( BTN_ID_KISEKAE );
      btnMan->SetButtonVisible( BTN_ID_INFO );
    } break;

    default:
    {
      btnMan->SetButtonVisible( BTN_ID_BACK );
      btnMan->SetButtonInvisible( BTN_ID_NEXT );
      btnMan->SetButtonInvisible( BTN_ID_KISEKAE );
      btnMan->SetButtonInvisible( BTN_ID_INFO );
    }
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    インフォ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   インフォの初期化
 */
void PrepareCommonView::_ShowInfo( void )
{
  FUKUSHIMA_PRINT("ShowInfo\n");

  this->SetInputEnabled( false );

  // インフォの初期化
  _InitInfo();

  // インフォウィンドウの表示
  gfl2::lyt::LytPane* infoBtn = m_pUtil->GetPane( PANENAME_STUDIO_COMMONPARTS_LOW_01_PANE_INFOBUTTON );
  Static::InformationWindow::Show( this, m_pUtil->GetPanePos( infoBtn ) );
}

/**
 * @brief   インフォの初期化
 */
void PrepareCommonView::_InitInfo( void )
{
  // インフォ用パラメータの生成
  this->_CreateInfoParam( INFO_PAGE_MAX );

  // インフォウィンドウのセットアップ
  Static::InformationWindow::SetupInfoWindow( m_pHeap, m_pAppRenderingManager );

  if( m_ViewType == VIEW_TYPE_DIFFICULTY )
  {
    _InitInfo_SelectDifficulty();

    // ウィンドウコンテンツのセットアップ
    Static::InformationWindow::SetupContents( m_InfoPageNum, m_pInfoStrArray );
  }
  else if( m_ViewType == VIEW_TYPE_PREPARE )
  {
    _InitInfo_SelectPrepare();

    // ウィンドウコンテンツのセットアップ
    Static::InformationWindow::SetupContents( m_InfoPageNum, m_pInfoStrArray );
  }
  else if( m_ViewType == VIEW_TYPE_BACKGROUND )
  {
    _InitInfo_SelectBackGround();
    
    // ウィンドウコンテンツのセットアップ
    Static::InformationWindow::SetupContents( m_InfoPageNum, m_pInfoStrArray, m_pInfoTexArray );
  }
  else {
    _InitInfo_SelectDressup();

    // ウィンドウコンテンツのセットアップ
    Static::InformationWindow::SetupContents( m_InfoPageNum, m_pInfoStrArray, m_pInfoTexArray );
  }

  // サブビューへ追加
  Static::InformationWindow::AddSubView( this );
}

/**
 * @brief   インフォの初期化：モード選択(難易度)
 */
void PrepareCommonView::_InitInfo_SelectDifficulty( void )
{
  FUKUSHIMA_PRINT( "_InitInfo_SelectDifficulty\n" );

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  m_InfoPageNum = 1;

  // 文字列
  g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_01 );
}

/**
 * @brief   インフォの初期化：準備選択
 */
void PrepareCommonView::_InitInfo_SelectPrepare( void )
{
  FUKUSHIMA_PRINT( "_InitInfo_SelectPrepare\n" );

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  m_InfoPageNum = 1;

  // 文字列
  if( m_pWork->app.diff_mode == Static::DIFF_MODE_EASY )
  {
    g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_02 );
  }
  else {
    g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_03 );
  }
}

/**
 * @brief   インフォの初期化：背景選択
 */
void PrepareCommonView::_InitInfo_SelectBackGround( void )
{
  FUKUSHIMA_PRINT( "_InitInfo_SelectBackGround\n" );

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  m_InfoPageNum = 1;

  // 文字列
  g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_04 );
  // テクスチャ
  m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_00_BFLIM );
}

/**
 * @brief   インフォの初期化
 */
void PrepareCommonView::_InitInfo_SelectDressup( void )
{
  FUKUSHIMA_PRINT( "_InitInfo_SelectDressup\n" );

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  m_InfoPageNum = 1;

  // 文字列
  g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_05 );
  // テクスチャ
  m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_01_BFLIM );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初回チュートリアルを表示するかどうか
 *
 * @param[in]   type    表示タイプ
 *
 * @retval  "true"なら表示する
 * @retval  "false"なら表示しない
 */
bool PrepareCommonView::_IsFirstTutorial( ViewType type )
{
  const Savedata::FinderStudioSave::TutorialState c_TutorialState[] = {
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_DIFFICULTY,
    Savedata::FinderStudioSave::TUTORIAL_STATE_NONE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_NONE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_NONE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_BACKGROUND,
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_DRESSUP,
    Savedata::FinderStudioSave::TUTORIAL_STATE_NONE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_NONE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_NONE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_NONE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_NONE,
  };

  GFL_ASSERT( type < GFL_NELEMS( c_TutorialState ) );

  Savedata::FinderStudioSave::TutorialState state = c_TutorialState[type];

  if( type == VIEW_TYPE_PREPARE )
  {
    state = ( _IsDiffModeEasy() ) ? Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_PREPARE_EASY : Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_PREPARE_AUTHENTIC;
  }

  if( state != Savedata::FinderStudioSave::TUTORIAL_STATE_NONE )
  {
    bool bTutorial = !m_pFinderStudioSave->IsTutorialState( state );
    m_pFinderStudioSave->SetTutorialState( state );

    return bTutorial;
  }

  return false;
}

/**
 * @brief   撮影難易度がお手軽かどうか
 *
 * @retval  "true"ならお手軽
 * @retval  "false"ならそれ以外
 */
bool PrepareCommonView::_IsDiffModeEasy( void )
{
  return (m_pWork->app.diff_mode == Static::DIFF_MODE_EASY);
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   UIInputListener：ボタンアクション開始イベント
 */
void PrepareCommonView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  if( m_pListener )
  {
    // 画面遷移の開始を通知
    m_pListener->StartScreenTransition();
  }
}

/**
 * @brief   UIInputListener：ボタンアクションイベント
 */
app::ui::UIInputListener::ListenerResult PrepareCommonView::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_MainSeq != SEQ_MAIN ) return ENABLE_ACCESS;

  this->SetInputEnabled( false );

  if( paneId == BTN_ID_BACK )
  {
    this->SetResult( RES_BACK );
  }
  else if( paneId == BTN_ID_NEXT )
  {
    this->SetResult( RES_NEXT );
  }
  else if( paneId == BTN_ID_KISEKAE )
  {
    this->SetResult( RES_APP_KISEKAE );
  }
  else if( paneId == BTN_ID_INFO )
  {
    m_MainSeq = SEQ_SHOW_INFO;
  }

  return ENABLE_ACCESS;
}


/**
 * @brief   PrepareCommonViewListener：着せ替えボタンのアクティブ
 */
void PrepareCommonView::ActiveKisekaeButton( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  if( !btnMan->IsButtonActive( BTN_ID_KISEKAE ) )
  {
    btnMan->SetButtonActive( BTN_ID_KISEKAE );
    btnMan->SetButtonInputEnable( BTN_ID_KISEKAE, true );
  }
}

/**
 * @brief   PrepareCommonViewListener：着せ替えボタンのパッシブ
 */
void PrepareCommonView::PassiveKisekaeButton( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  if( btnMan->IsButtonActive( BTN_ID_KISEKAE ) )
  {
    btnMan->SetButtonPassive( BTN_ID_KISEKAE );
    btnMan->SetButtonInputEnable( BTN_ID_KISEKAE, false );
  }
}

/**
 * @brief   PrepareCommonViewListener：着せ替えボタンの表示
 */
void PrepareCommonView::VisibleKisekaeButton( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
  btnMan->SetButtonVisible( BTN_ID_KISEKAE );
}

/**
 * @brief   PrepareCommonViewListener：着せ替えボタンの非表示
 */
void PrepareCommonView::InvisibleKisekaeButton( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
  btnMan->SetButtonInvisible( BTN_ID_KISEKAE );
}

/**
 * @brief   ボタンをロック
 */
void PrepareCommonView::LockCommonButton( void )
{
  m_pUtil->LockButton();
}

/**
 * @brief   ボタンをアンロック
 */
void PrepareCommonView::UnlockCommonButton( void )
{
  m_pUtil->UnlockButton();
}

/**
 * @brief   PrepareCommonViewListener：ボタン有効設定
 */
void PrepareCommonView::SetInputEnabledCommonButton( bool enable )
{
  this->SetInputEnabled( enable );
}


/**
 * @brief   app::tool::InfoWindowBookTypeListener：インフォウィンドウを閉じきったイベント
 */
void PrepareCommonView::OnClose( void )
{
  FUKUSHIMA_PRINT("OnClose\n");

  Static::InformationWindow::Hide();

  // インフォ用パラメータの破棄
  this->_DeleteInfoParam();
  this->SetInputEnabled( true );
  this->SetResult( RES_INFO_CLOSE );

  m_MainSeq = SEQ_MAIN;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
