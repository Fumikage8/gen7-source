//======================================================================
/**
 * @file    FinderStudioCaptureCameraView.cpp
 * @date    2016/11/28 16:09:47
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：カメラView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureCameraView.h"
#include "FinderStudioCaptureViewListener.h"
#include "../FinderStudioCaptureScrollBar.h"
#include "FinderStudioCaptureShootCommonView.h"

#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>

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
CameraView::CameraView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : CaptureViewBase( heap, renderMan )
  , m_pCommonView( NULL )
  , m_pCameraAuthenticBasePane( NULL )
  , m_pCameraEasyBasePane( NULL )
  , m_pForcusBasePane( NULL )
  , m_LastTouchCameraRotateIndex( CAMERA_ROTATE_NULL )
  , m_bTouchEnable( true )
{
  for( u32 i = 0; i < SCROLLBAR_ID_NUM; ++i )
  {
    m_pScrollBar[i] = NULL;
  }
  for( u32 i = 0; i < CAMERA_ROTATE_NUM; ++i )
  {
    m_pCameraRotatePane[i] = NULL;
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
void CameraView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // アニメの初期化
  InitAnim();
  // ペインの初期化
  InitPane();
  // ボタンの初期化
  InitButton();
}

/**
 * @brief   2Dの初期化
 */
void CameraView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1, resTbl, 1, setupData, 1, msgData, wordSet );

  // UIInputListenerのセット
  this->SetInputListener( this );
  // 十字キースティックを別々にする
  this->SetStickEmulationMode( false );
  // Viewの初期設定
  this->_InitView();
}

/**
 * @brief   ペインの初期化
 */
void CameraView::InitPane( void )
{
  // カメラ本格ベースペイン
  m_pCameraAuthenticBasePane = m_pUtil->GetPane( PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA );
  // カメラ簡単ベースペイン
  m_pCameraEasyBasePane = m_pUtil->GetPane( PANENAME_STUDIO_CAMERA_LOW_00_PANE_STUDIO_CSTM_BASE_01 );
  // 注視点バーのNULLペイン
  m_pForcusBasePane = m_pUtil->GetPane( PANENAME_STUDIO_CAMERA_LOW_00_PANE_MOVE );

  // カメラ回転ペイン
  const u32 c_CameraRotatePane[] = {
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA_UP,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA_DOWN,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA_LEFT,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA_RIGHT,
  };
  for( u32 i = 0; i < CAMERA_ROTATE_NUM; ++i )
  {
    gfl2::lyt::LytParts* btnParts = m_pUtil->GetParts( c_CameraRotatePane[i] );
    m_pCameraRotatePane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00, btnParts );
  }

  // スクロールバー
  _InitScrollBar();
}

/**
 * @brief   アニメの初期化
 */
void CameraView::InitAnim( void )
{
  m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__BG01_BG_LOOP_01 );
}

/**
 * @brief   2Dの初期化
 */
void CameraView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__INFOBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__CAMERA_UP_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__CAMERA_DOWN_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__CAMERA_LEFT_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__CAMERA_RIGHT_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__ZOOM_UP_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__ZOOM_DOWN_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__HIGH_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__LOW_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__PLAYER_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CAMERA_LOW_00__POKE_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_INFOBUTTON,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA_UP,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA_DOWN,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA_LEFT,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_CAMERA_RIGHT,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_ZOOM_UP,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_ZOOM_DOWN,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_HIGH,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_LOW,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_PLAYER,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_POKE,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_13_PANE_BOUND_00,
  };

  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    Static::ButtonInfo::SetupBtnInfo( &info[i], i, c_ButtonAnim[i] );

    switch( i )
    {
      case BTN_ID_INFO:
      case BTN_ID_ROTATE_X_UP:
      case BTN_ID_ROTATE_X_DOWN:
      case BTN_ID_ROTATE_Y_LEFT:
      case BTN_ID_ROTATE_Y_RIGHT:
      case BTN_ID_ZOOM_PLUS:
      case BTN_ID_ZOOM_MINUS:
      case BTN_ID_HEIGHT_PLUS:
      case BTN_ID_HEIGHT_MINUS:
      case BTN_ID_FORCUS_TRAINER:
      case BTN_ID_FORCUS_POKEMON:
      {
        gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ButtonParts[i] );
        gfl2::lyt::LytPane*  bound = m_pUtil->GetPane( c_ButtonBounding[i], parts );
        info[i].picture_pane = parts;
        info[i].bound_pane = bound;
      }
    }
  }

  // ボタンマネージャの生成
  CreateButtonManager( m_pHeap, m_pUtil->GetLytWk(), info, BTN_ID_NUM );

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   初期化：スクロールバー
 */
void CameraView::_InitScrollBar( void )
{
  const u32 c_ScrollBarParts[] = {
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_SCROLL_ZOOM,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_SCROLL_HIGHLOW,
    PANENAME_STUDIO_CAMERA_LOW_00_PANE_SCROLL_MOVE,
  };

  SlideScrollBar::ScrollDirection ScrollDirTable[] = {
    SlideScrollBar::SCROLL_DIR_VERTICAL,
    SlideScrollBar::SCROLL_DIR_VERTICAL,
    SlideScrollBar::SCROLL_DIR_HORIZONTAL,
  };

  for( u32 i = 0; i < SCROLLBAR_ID_NUM; ++i )
  {
    // ベースペイン
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ScrollBarParts[i] );
    gfl2::lyt::LytPane* tabPane = m_pUtil->GetPane( PANENAME_STUDIOPARTS_SCROLL_PANE_STUDIO_SCROLL_BTN, parts );
    gfl2::lyt::LytPane* minPane = m_pUtil->GetPane( PANENAME_STUDIOPARTS_SCROLL_PANE_NULL_TOP, parts );
    gfl2::lyt::LytPane* maxPane = m_pUtil->GetPane( PANENAME_STUDIOPARTS_SCROLL_PANE_NULL_BOTTOM, parts );

    m_pScrollBar[i] = GFL_NEW( m_pHeap->GetDeviceHeap() ) SlideScrollBar( m_pUtil->GetLytWk(), parts, tabPane, minPane, maxPane, ScrollDirTable[i] );
  }
}

/**
 * @brief   終了
 */
bool CameraView::Terminate( void )
{
  for( u32 i = 0; i < SCROLLBAR_ID_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pScrollBar[i] );
  }

  // View設定の解除
  this->_TermView();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void CameraView::Show( void )
{
  // スクロールバーの更新
  _UpdateScrollBar_Zoom();
  _UpdateScrollBar_Height();
  _UpdateScrollBar_Forcus();

  // 撮影モードごとの表示設定
  _SetCaptureMode();
  // 難易度モードごとの表示設定
  _SetDiffMode();

  // 表示
  this->_Show();

  this->SetModal( true );
}

/**
 * @brief   非表示
 */
void CameraView::Hide( void )
{
  this->SetModal( false );
  this->SetInputEnabled( false );
  this->SetVisible( false );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示設定
///
//////////////////////////////////////////////////////////////////////////
/** 
 * @brief   撮影モードごとの表示設定
 */
void CameraView::_SetCaptureMode( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  // ツーショット
  if( m_pWork->prepare.capture_mode == Viewer::CAPTURE_MODE_BOTH )
  {
    btnMan->SetButtonVisible( BTN_ID_FORCUS_POKEMON );
    btnMan->SetButtonVisible( BTN_ID_FORCUS_TRAINER );

    m_pUtil->SetVisiblePane( m_pForcusBasePane, true );
  }
  // ポケモンのみ
  else {
    btnMan->SetButtonInvisible( BTN_ID_FORCUS_POKEMON );
    btnMan->SetButtonInvisible( BTN_ID_FORCUS_TRAINER );

    m_pUtil->SetVisiblePane( m_pForcusBasePane, false );
  }
}

/**
 * @brief   難易度モードごとの表示設定
 */
void CameraView::_SetDiffMode( void )
{
  Static::DifficultyMode mode = m_pWork->app.diff_mode;
  GFL_ASSERT( mode < Static::DIFF_MODE_NUM );

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  // 本格撮影かどうか
  bool bAuthentic = (mode == Static::DIFF_MODE_AUTHENTIC);

  // ボタン入力設定
  btnMan->SetButtonInputEnable( BTN_ID_ROTATE_X_UP, bAuthentic );
  btnMan->SetButtonInputEnable( BTN_ID_ROTATE_X_DOWN, bAuthentic );
  btnMan->SetButtonInputEnable( BTN_ID_ROTATE_Y_LEFT, bAuthentic );
  btnMan->SetButtonInputEnable( BTN_ID_ROTATE_Y_RIGHT, bAuthentic );

  // 表示設定
  m_pUtil->SetVisiblePane( m_pCameraAuthenticBasePane, bAuthentic );
  m_pUtil->SetVisiblePane( m_pCameraEasyBasePane, !bAuthentic );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    インフォ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   インフォウィンドウの初期化
 */
void CameraView::_InitInfo( void )
{
  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  // インフォ用パラメータの生成
  this->_CreateInfoParam( INFO_PAGE_MAX );

  if( m_pWork->app.diff_mode == Static::DIFF_MODE_AUTHENTIC )
  {
    // 文字列
    g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_15 );
    // テクスチャ
    m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_11_BFLIM );
  }
  else {
    // 文字列
    g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_13 );
    // テクスチャ
    m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_09_BFLIM );
  }
}

/**
 * @brief   インフォウィンドウの表示
 */
void CameraView::_ShowInfo( void )
{
  // 入力を無効に
  this->SetInputEnabled( false );

  // インフォの初期化
  _InitInfo();

  // インフォウィンドウのセットアップ
  Static::InformationWindow::SetupInfoWindow( m_pHeap, m_pAppRenderingManager );
  // ウィンドウコンテンツのセットアップ
  Static::InformationWindow::SetupContents( INFO_PAGE_MAX, m_pInfoStrArray, m_pInfoTexArray );
  // インフォウィンドウの表示
  gfl2::lyt::LytPane* infoBtn = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_INFOBUTTON );
  Static::InformationWindow::Show( this, m_pUtil->GetPanePos( infoBtn ) );

  this->SetResult( RES_INFO_SHOW );
}

/**
 * @brief   チュートリアルの初期化：本格撮影
 */
void CameraView::_InitTutorial_Authentic( void )
{
  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  // インフォ用パラメータの生成
  this->_CreateInfoParam( INFO_PAGE_MAX );

  // 文字列
  g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_14 );
  // テクスチャ
  m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_10_BFLIM );
}

/**
 * @brief   チュートリアルの初期化：お手軽撮影
 */
void CameraView::_InitTutorial_Easy( void )
{
  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  // インフォ用パラメータの生成
  this->_CreateInfoParam( INFO_PAGE_MAX );

  // 文字列
  g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_13 );
  // テクスチャ
  m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_09_BFLIM );
}

/**
 * @brief   チュートリアルの表示
 */
void CameraView::_ShowTutorial( void )
{
  bool bTutorial = false;

  if( m_pWork->app.diff_mode == Static::DIFF_MODE_AUTHENTIC )
  {
    bTutorial = _ShowTutorial_Authentic();
  }
  else {
    bTutorial = _ShowTutorial_Easy();
  }

  if( bTutorial )
  {
    // 入力を無効に
    this->SetInputEnabled( false );

    // インフォウィンドウのセットアップ
    Static::InformationWindow::SetupInfoWindow( m_pHeap, m_pAppRenderingManager );
    // ウィンドウコンテンツのセットアップ
    Static::InformationWindow::SetupContents( INFO_PAGE_MAX, m_pInfoStrArray, m_pInfoTexArray );
    // インフォウィンドウの表示
    gfl2::lyt::LytPane* infoBtn = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_INFOBUTTON );
    Static::InformationWindow::Show( this, m_pUtil->GetPanePos( infoBtn ) );

    this->SetResult( RES_INFO_SHOW );
  }
}

/**
 * @brief   チュートリアルの表示：本格撮影
 *
 * @retval  "true"ならチュートリアル表示
 * @retval  "false"ならそれ以外
 */
bool CameraView::_ShowTutorial_Authentic( void )
{
  Savedata::FinderStudioSave::TutorialState state =
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_CAMERA_AUTHENTIC;

  if( !m_pFinderStudioSave->IsTutorialState( state ) )
  {
    // 初期化
    _InitTutorial_Authentic();

    // チュートリアルフラグを更新
    m_pFinderStudioSave->SetTutorialState( state );

    return true;
  }

  return false;
}

/**
 * @brief   チュートリアルの表示：お手軽撮影
 *
 * @retval  "true"ならチュートリアル表示
 * @retval  "false"ならそれ以外
 */
bool CameraView::_ShowTutorial_Easy( void )
{
  Savedata::FinderStudioSave::TutorialState state =
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_CAMERA_EASY;

  if( !m_pFinderStudioSave->IsTutorialState( state ) )
  {
    // 初期化
    _InitTutorial_Easy();

    // チュートリアルフラグを更新
    m_pFinderStudioSave->SetTutorialState( state );

    return true;
  }

  return false;
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    カメラ操作
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カメラ操作：ズームイン
 */
void CameraView::_SetCamera_ZoomIn( void )
{
  // ズームイン
  m_pListener->CameraZoomIn();

  // スクロールバーを更新
  _UpdateScrollBar_Zoom();
}

/**
 * @brief   カメラ操作：ズームアウト
 */
void CameraView::_SetCamera_ZoomOut( void )
{
  // ズームアウト
  m_pListener->CameraZoomOut();

  // スクロールバーを更新
  _UpdateScrollBar_Zoom();
}

/**
 * @brief   カメラ操作：高さ 上
 */
void CameraView::_SetCamera_HeightUp( void )
{
  // カメラの高さ：上
  m_pListener->CameraMoveYUp();

  // スクロールバーを更新
  _UpdateScrollBar_Height();
}

/**
 * @brief   カメラ操作：高さ 下
 */
void CameraView::_SetCamera_HeightDown( void )
{
  // カメラの高さ：下
  m_pListener->CameraMoveYDown();

  // スクロールバーを更新
  _UpdateScrollBar_Height();
}

/**
 * @brief   カメラ操作：フォーカス ポケモン
 */
void CameraView::_SetCamera_ForcusPoke( void )
{
  // フォーカス：ポケモン
  m_pListener->CameraLookAtPokemon();

  // スクロールバーを更新
  _UpdateScrollBar_Forcus();
}

/**
 * @brief   カメラ操作：フォーカス プレイヤー
 */
void CameraView::_SetCamera_ForcusPlayer( void )
{
  // フォーカス：プレイヤー
  m_pListener->CameraLookAtPlayer();

  // スクロールバーを更新
  _UpdateScrollBar_Forcus();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    スクロールバー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   スクロールバー：ズーム
 */
void CameraView::_UpdateScrollBar_Zoom( void )
{
  // スクロールバーを更新
  m_pScrollBar[SCROLLBAR_ID_ZOOM]->Update( m_pListener->GetCameraZoomRate() );
}

/**
 * @brief   スクロールバー：高さ
 */
void CameraView::_UpdateScrollBar_Height( void )
{
  // スクロールバーを更新
  m_pScrollBar[SCROLLBAR_ID_HEIGHT]->Update( 1.0f - m_pListener->GetCameraHeightRate() );
}

/**
 * @brief   スクロールバー：フォーカス
 */
void CameraView::_UpdateScrollBar_Forcus( void )
{
  // スクロールバーを更新
  m_pScrollBar[SCROLLBAR_ID_FORCUS]->Update( 1.0f - m_pListener->GetCameraLookAtRate() );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    タッチイベント
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   タッチイベント
 */
void CameraView::_TouchEvent( gfl2::ui::TouchPanel* touchPanel, bool isTouch )
{
  FUKUSHIMA_PRINT( "_TouchEvent : isTouch=[%d] diffmode=[%d]\n", isTouch, m_pWork->app.diff_mode );

  if( m_pWork->app.diff_mode == Static::DIFF_MODE_AUTHENTIC )
  {
    _TouchEventFull( touchPanel, isTouch );
  }
  else {
    _TouchEventPart( touchPanel, isTouch );
  }

  _TouchEventScrollBar( touchPanel, isTouch );
}

/**
 * @brief   タッチイベント：フルモード
 */
void CameraView::_TouchEventFull( gfl2::ui::TouchPanel* touchPanel, bool isTouch )
{
  // 簡易モード
  _TouchEventPart( touchPanel, isTouch );

  // ボタン入力
  _TouchEventFullButton( touchPanel, isTouch );
}

/**
 * @brief   タッチイベント：簡易モード
 */
void CameraView::_TouchEventPart( gfl2::ui::TouchPanel* touchPanel, bool isTouch )
{
  // ボタン入力
  _TouchEventPartButton( touchPanel, isTouch );
}

/**
 * @brief   タッチイベント：ボタン
 */
void CameraView::_TouchEventFullButton( gfl2::ui::TouchPanel* touchPanel, bool isTouch )
{
  if( touchPanel->IsTouchRelease() || !isTouch )
  {
    m_LastTouchCameraRotateIndex = CAMERA_ROTATE_NULL;
  }

  /// カメラ回転ボタンのトリガー判定
  _CameraRotateButtonTrigger( touchPanel, isTouch );

  /// 最初に選択したボタンがタッチされ続けているか
  if( !_IsTouchCameraRotateButton( touchPanel, isTouch ) )
  {
    return;   //!< タッチされていない
  }

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  switch( btnMan->GetHoldingButtonId() )
  {
    case BTN_ID_ROTATE_X_UP:
      m_pListener->CameraRotateXUp();
      break;
    case BTN_ID_ROTATE_X_DOWN:
      m_pListener->CameraRotateXDown();
      break;
    case BTN_ID_ROTATE_Y_LEFT:
      m_pListener->CameraRotateYRight();
      break;
    case BTN_ID_ROTATE_Y_RIGHT:
      m_pListener->CameraRotateYLeft();
      break;
    default:
    return;
  }

  if( touchPanel->IsTouchTrigger() )
  {
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

/**
 * @brief   タッチイベント：ボタン
 */
void CameraView::_TouchEventPartButton( gfl2::ui::TouchPanel* touchPanel, bool isTouch )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
  u32 btnID = btnMan->GetHoldingButtonId();

  switch( btnID )
  {
    case BTN_ID_ZOOM_PLUS:
      _SetCamera_ZoomIn();
      break;
    case BTN_ID_ZOOM_MINUS:
      _SetCamera_ZoomOut();
      break;
    case BTN_ID_HEIGHT_PLUS:
      _SetCamera_HeightUp();
      break;
    case BTN_ID_HEIGHT_MINUS:
      _SetCamera_HeightDown();
      break;
    case BTN_ID_FORCUS_POKEMON:
      _SetCamera_ForcusPoke();
      break;
    case BTN_ID_FORCUS_TRAINER:
      _SetCamera_ForcusPlayer();
      break;
    default:
      return;
  }

  if( touchPanel->IsTouchTrigger() )
  {
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

/**
 * @brief   タッチイベント：スクロールバー
 */
void CameraView::_TouchEventScrollBar( gfl2::ui::TouchPanel* touchPanel, bool isTouch )
{
  // ズームバー
  if( m_pScrollBar[SCROLLBAR_ID_ZOOM]->Update( touchPanel, isTouch ) )
  {
    f32 value = m_pScrollBar[SCROLLBAR_ID_ZOOM]->GetValue();
    FUKUSHIMA_PRINT( "ScrollBar Zoom Value=[%.1f]\n", value );
    m_pListener->SetCameraZoomRate( value );
  }

  // 高さ
  if( m_pScrollBar[SCROLLBAR_ID_HEIGHT]->Update( touchPanel, isTouch ) )
  {
    f32 value = m_pScrollBar[SCROLLBAR_ID_HEIGHT]->GetValue();
    FUKUSHIMA_PRINT( "ScrollBar Height Value=[%.1f]\n", value );
    m_pListener->SetCameraHeightRate( 1.0f - value );
  }

  // フォーカス
  if( m_pWork->viewer.capture_mode == Viewer::CAPTURE_MODE_BOTH )
  {
    if( m_pScrollBar[SCROLLBAR_ID_FORCUS]->Update( touchPanel, isTouch ) )
    {
      f32 value = m_pScrollBar[SCROLLBAR_ID_FORCUS]->GetValue();
      FUKUSHIMA_PRINT( "ScrollBar Forcus Value=[%.1f]\n", value );
      m_pListener->SetCameraLookAtRate( 1.0f - value );
    }
  }
}

/**
 * @brief   カメラ回転ボタン：トリガー判定
 *
 * @param[in]   touchPanel    タッチパネル
 * @param[in]   pane          対象ペイン
 */
void CameraView::_CameraRotateButtonTrigger( gfl2::ui::TouchPanel* touchPanel, bool isTouch )
{
  if( !isTouch )
  {
    return;
  }
  /// 既にタッチ中のボタンがある場合は判定をしない
  if( m_LastTouchCameraRotateIndex != CAMERA_ROTATE_NULL )
  {
    return;
  }

  for( u32 i = 0; i < CAMERA_ROTATE_NUM; ++i )
  {
    gfl2::lyt::LytPane* btnPane = m_pCameraRotatePane[i];

    /// ボタンがタッチされているかをチェック
    if( m_pUtil->IsTouchTriggerPane(btnPane) )
    {
      m_LastTouchCameraRotateIndex = i;
      break;
    }
  }
}

/**
 * @brief   カメラ回転ボタン：最初にタッチされたボタンがタッチされているか
 *
 * @param[in]   touchPanel    タッチパネル
 * @param[in]   pane          対象ペイン
 *
 * @retval  "true"ならタッチされている
 * @retval  "false"ならそれ以外
 */
bool CameraView::_IsTouchCameraRotateButton( gfl2::ui::TouchPanel* touchPanel, bool isTouch )
{
  if( !isTouch )
  {
    return false;
  }
  /// タッチされているボタンがない場合は何もしない
  if( m_LastTouchCameraRotateIndex == CAMERA_ROTATE_NULL )
  {
    return false;
  }

  GFL_ASSERT( m_LastTouchCameraRotateIndex < CAMERA_ROTATE_NUM );

  /// 最初にタッチしたボタンペインを取得
  gfl2::lyt::LytPane* btnPane = m_pCameraRotatePane[m_LastTouchCameraRotateIndex];

  /// 最初にタッチしたボタンがタッチされているか？
  return m_pUtil->IsTouchHoldPane( btnPane );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    キーアクション
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   キーアクション
 */
void CameraView::_KeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick )
{
  FUKUSHIMA_PRINT( "_KeyAction : diffmode=[%d]\n", m_pWork->app.diff_mode );

  if( m_pWork->app.diff_mode == Static::DIFF_MODE_AUTHENTIC )
  {
    _KeyActionFull( button, stick );
  }
  else {
    _KeyActionPart( button, stick );
  }
}

/**
 * @brief   キーアクション：フルモード
 */
void CameraView::_KeyActionFull( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick )
{
  // 簡易モード
  _KeyActionPart( button, stick );

  // カメラ回転
  _KeyActionRotate( stick );
}

/**
 * @brief   キーアクション：簡易モード
 */
void CameraView::_KeyActionPart( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick )
{
  // ズーム
  _KeyActionZoom( button );
  // 高さ
  _KeyActionHeight( button );
  // フォーカス
  _KeyActionForcus( button );
}

/**
 * @brief   キーアクション：カメラ回転
 */
void CameraView::_KeyActionRotate( gfl2::ui::VectorDevice* stick )
{
  f32 stickX = stick->GetX();
  f32 stickY = stick->GetY();

  // X軸回転
  if( stickY > 0.5f )
  {
    m_pListener->CameraRotateXUp();
  }
  else if( stickY < -0.5f )
  {
    m_pListener->CameraRotateXDown();
  }

  // Y軸回転
  if( stickX > 0.5f )
  {
    m_pListener->CameraRotateYLeft();
  }
  else if( stickX < -0.5f )
  {
    m_pListener->CameraRotateYRight();
  }
}

/**
 * @brief   キーアクション：ズーム
 */
void CameraView::_KeyActionZoom( gfl2::ui::Button* button )
{
  if( button->IsHold( gfl2::ui::BUTTON_UP ) )
  {
    // ズームイン
    _SetCamera_ZoomIn();
  }
  else if( button->IsHold( gfl2::ui::BUTTON_DOWN ) )
  {
    // ズームアウト
    _SetCamera_ZoomOut();
  }
}

/**
 * @brief   キーアクション：高さ
 */
void CameraView::_KeyActionHeight( gfl2::ui::Button* button )
{
  if( button->IsHold( gfl2::ui::BUTTON_X ) )
  {
    // カメラの高さ：上
    _SetCamera_HeightUp();
  }
  else if( button->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    // カメラの高さ：下
    _SetCamera_HeightDown();
  }
}

/**
 * @brief   キーアクション：フォーカス
 */
void CameraView::_KeyActionForcus( gfl2::ui::Button* button )
{
  // ツーショットの時のみフォーカスを操作できる
  if( m_pWork->prepare.capture_mode == Viewer::CAPTURE_MODE_BOTH )
  {
    if( button->IsHold( gfl2::ui::BUTTON_LEFT ) )
    {
      // フォーカス：ポケモン
      _SetCamera_ForcusPoke();
    }
    else if( button->IsHold( gfl2::ui::BUTTON_RIGHT ) )
    {
      // フォーカス：プレイヤー
      _SetCamera_ForcusPlayer();
    }
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   UIInputListener：ペインアクション開始イベント
 */
void CameraView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  if( paneId == BTN_ID_INFO )
  {
    m_bTouchEnable = false;

    // 共通ボタンをロック    // @fix MMCat[634]
    m_pCommonView->SetButtonLock();
  }
}

/**
 * @brief   UIInputListener：ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult CameraView::OnLayoutPaneEvent( const u32 paneId )
{
  if( paneId == BTN_ID_INFO )
  {
    // インフォの表示
    _ShowInfo();
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult CameraView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_pListener == NULL ) return ENABLE_ACCESS;

  if( m_bTouchEnable )
  {
    // タッチイベント
    _TouchEvent( pTouchPanel, isTouch );
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：キーアクション
 */
app::ui::UIInputListener::ListenerResult CameraView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  if( m_pListener == NULL ) return ENABLE_ACCESS;

  // キーアクション
  _KeyAction( button, stick );

  return ENABLE_ACCESS;
}

/**
 * @brief   InfoWindowBookTypeListener：ウィンドウを閉じきったイベント
 */
void CameraView::OnClose( void )
{
  Static::InformationWindow::Hide();

  // インフォ用パラメータの破棄
  this->_DeleteInfoParam();

  m_bTouchEnable = true;

  // 共通ボタンをアンロック    // @fix MMCat[634]
  m_pCommonView->SetButtonUnlock();

  this->SetInputEnabled( true );
  this->SetResult( RES_INFO_HIDE );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
