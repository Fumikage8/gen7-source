//======================================================================
/**
 * @file    FinderStudioCaptureSelectButtonView.cpp
 * @date    2016/11/24 20:07:36
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：汎用選択ボタンビュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureSelectButtonView.h"
#include "FinderStudioCaptureViewDefine.h"
#include "FinderStudioCaptureViewListener.h"
#include "FinderStudioCapturePrepareCommonView.h"

#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>

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
SelectButtonView::SelectButtonView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf )
  : CaptureViewBase( heap, renderMan, cursorBuf )
  , m_pPreCmnViewListener( NULL )
  , m_ButtonType( BTN_TYPE_NUM )
  , m_ButtonNum( BTN_ID_NUM )
  , m_pTwoBtnPane( NULL )
  , m_pThreeBtnPane( NULL )
{
  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    m_pButtonText[i] = NULL;
  }
  for( u32 i = 0; i < TWO_BTN_NUM; ++i )
  {
    m_pTwoBtnCursorPane[i] = NULL;
  }
  for( u32 i = 0; i < THREE_BTN_NUM; ++i )
  {
    m_pThreeBtnCursorPane[i] = NULL;
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
void SelectButtonView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // ボタンの初期化
  InitButton();
  // メニューカーソルの生成
  this->CreateMenuCursor();
}

/**
 * @brief   2dの初期化
 */
void SelectButtonView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_00___NUM,
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
void SelectButtonView::InitPane( void )
{
  // ボタンテキスト
  const u32 c_SelectButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_04,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_SelectButtonParts ); ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_SelectButtonParts[i] );
    m_pButtonText[i] = m_pUtil->GetTextBox( PANENAME_STUDIOPARTS_BUTTON_02_PANE_TEXTBOX_00, parts );
  }

  // カーソルペイン
  const u32 c_ThreeButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_02,
  };
  const u32 c_TwoButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_04,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_ThreeButtonParts ); ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ThreeButtonParts[i] );
    m_pThreeBtnCursorPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_02_PANE_NULL_CUR, parts );
  }
  for( u32 i = 0; i < GFL_NELEMS( c_TwoButtonParts ); ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_TwoButtonParts[i] );
    m_pTwoBtnCursorPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_02_PANE_NULL_CUR, parts );
  }

  // ヌルペイン
  m_pTwoBtnPane = m_pUtil->GetPane( PANENAME_STUDIO_LIST_LOW_00_PANE_NULL_BTN_2 );
  m_pThreeBtnPane = m_pUtil->GetPane( PANENAME_STUDIO_LIST_LOW_00_PANE_NULL_BTN_3 );
}

/**
 * @brief   ボタンの初期化
 */
void SelectButtonView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_00__BUTTON_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_00__BUTTON_01_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_00__BUTTON_02_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_00__BUTTON_03_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_00__BUTTON_04_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_00_PANE_BUTTON_04,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
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

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   終了
 */
bool SelectButtonView::Terminate( void )
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
void SelectButtonView::Update( void )
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
void SelectButtonView::_UpdateCommonButton( void )
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
void SelectButtonView::Show( void )
{
  // キーリピートの生成
  this->CreateKeyRepeatController( m_ButtonNum, 0 );

  this->_Show();

  m_CursorPos = 0;

  gfl2::lyt::LytPane* cursorPane = _GetCursorPane( m_CursorPos );
  this->PutCursor( cursorPane );
}

/**
 * @brief   非表示
 */
void SelectButtonView::Hide( void )
{
  this->_Hide();

  // キーリピートの生成
  this->DeleteKeyRepeatController();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ボタン設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンの設定
 *
 * @param   type    ボタンタイプ
 */
void SelectButtonView::SetSelectButtonType( BtnType type )
{
  FUKUSHIMA_PRINT( "SetSelectButtonType : nowType[%d] nextType[%d]\n", m_ButtonType, type );

  if( m_ButtonType != type )
  {
    m_ButtonType = type;

    // ボタンの数を設定
    _SetSelectButtonNum();
    // テキストの設定
    _SetSelectButtonText();
    // SEの設定
    _SetSelectButtonSE();
    // ボタンの有効設定
    _SetSelectButtonEnable();
  }
}

/**
 * @brief   ボタンの数を設定
 */
void SelectButtonView::_SetSelectButtonNum( void )
{
  bool bThreeBtn = _IsThreeButton();

  if( bThreeBtn )
  {
    m_ButtonNum = THREE_BTN_NUM;  //!< ３個
  }
  else {
    m_ButtonNum = TWO_BTN_NUM;    //!< ２個
  }

  m_pUtil->SetVisiblePane( m_pTwoBtnPane, !bThreeBtn );
  m_pUtil->SetVisiblePane( m_pThreeBtnPane, bThreeBtn );

  FUKUSHIMA_PRINT( "_SetSelectButtonNum : ButtonNum[%d]\n", m_ButtonNum );
}

/**
 * @brief   選択ボタンのテキスト設定
 */
void SelectButtonView::_SetSelectButtonText( void )
{
  // 難易度選択
  if( m_ButtonType == BTN_TYPE_DIFFICULTY )
  {
    _SetButtonText( BTN_ID_TWO_BTN_00, fs_photo_cap_106 );
    _SetButtonText( BTN_ID_TWO_BTN_01, fs_photo_cap_107 );
  }
  // 撮影モード選択
  else if( m_ButtonType == BTN_TYPE_CAPTURE_MODE )
  {
    _SetButtonText( BTN_ID_TWO_BTN_00, fs_photo_cap_02 );
    _SetButtonText( BTN_ID_TWO_BTN_01, fs_photo_cap_01 );
  }
  // 難易度選択からの終了確認
  else if( m_ButtonType == BTN_TYPE_FINISH_YESNO )
  {
    _SetButtonText( BTN_ID_TWO_BTN_00, fs_photo_cap_15 );
    _SetButtonText( BTN_ID_TWO_BTN_01, fs_photo_cap_16 );
  }
  // 準備画面からの終了確認
  else if( m_ButtonType == BTN_TYPE_FINISH_PREPARE )
  {
    _SetButtonText( BTN_ID_THREE_BTN_00, fs_photo_cap_20 );
    _SetButtonText( BTN_ID_THREE_BTN_01, fs_photo_cap_104 );
    _SetButtonText( BTN_ID_THREE_BTN_02, fs_photo_cap_22 );
  }
  // 準備画面からの終了確認
  else if( m_ButtonType == BTN_TYPE_FINISH_PREPARE_LOCK )
  {
    _SetButtonText( BTN_ID_TWO_BTN_00, fs_photo_cap_20 );
    _SetButtonText( BTN_ID_TWO_BTN_01, fs_photo_cap_22 );
  }
  // 撮影メイン画面からの終了確認
  else if( m_ButtonType == BTN_TYPE_FINISH_MAIN )
  {
    _SetButtonText( BTN_ID_THREE_BTN_00, fs_photo_cap_20 );
    _SetButtonText( BTN_ID_THREE_BTN_01, fs_photo_cap_21 );
    _SetButtonText( BTN_ID_THREE_BTN_02, fs_photo_cap_22 );
  }
}

/**
 * @brief   選択ボタンのSE設定
 */
void SelectButtonView::_SetSelectButtonSE( void )
{
  // 難易度選択
  if( m_ButtonType == BTN_TYPE_DIFFICULTY )
  {
    _SetButtonSE( BTN_ID_TWO_BTN_00, SEQ_SE_DECIDE1 );
    _SetButtonSE( BTN_ID_TWO_BTN_01, SEQ_SE_DECIDE1 );
  }
  // 撮影モード選択
  else if( m_ButtonType == BTN_TYPE_CAPTURE_MODE )
  {
    _SetButtonSE( BTN_ID_TWO_BTN_00, app::tool::ButtonManager::SE_NULL );
    _SetButtonSE( BTN_ID_TWO_BTN_01, app::tool::ButtonManager::SE_NULL );
  }
  // 難易度選択からの終了確認
  else if( m_ButtonType == BTN_TYPE_FINISH_YESNO )
  {
    _SetButtonSE( BTN_ID_TWO_BTN_00, SEQ_SE_DECIDE1 );
    _SetButtonSE( BTN_ID_TWO_BTN_01, SEQ_SE_CANCEL1 );
  }
  // 準備画面からの終了確認
  else if( m_ButtonType == BTN_TYPE_FINISH_PREPARE )
  {
    _SetButtonSE( BTN_ID_THREE_BTN_00, SEQ_SE_DECIDE1 );
    _SetButtonSE( BTN_ID_THREE_BTN_01, SEQ_SE_DECIDE1 );
    _SetButtonSE( BTN_ID_THREE_BTN_02, SEQ_SE_DECIDE1 );
  }
  // 準備画面からの終了確認
  else if( m_ButtonType == BTN_TYPE_FINISH_PREPARE_LOCK )
  {
    _SetButtonSE( BTN_ID_TWO_BTN_00, SEQ_SE_DECIDE1 );
    _SetButtonSE( BTN_ID_TWO_BTN_01, SEQ_SE_DECIDE1 );
  }
  // 撮影メイン画面からの終了確認
  else if( m_ButtonType == BTN_TYPE_FINISH_MAIN )
  {
    _SetButtonSE( BTN_ID_THREE_BTN_00, SEQ_SE_DECIDE1 );
    _SetButtonSE( BTN_ID_THREE_BTN_01, SEQ_SE_DECIDE1 );
    _SetButtonSE( BTN_ID_THREE_BTN_02, SEQ_SE_DECIDE1 );
  }
}

/**
* @brief   ボタンの有効設定
*
* @param   type    ボタンタイプ
*/
void SelectButtonView::_SetSelectButtonEnable( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonInputEnable( BTN_ID_THREE_BTN_00 + i, false );
  }

  if( _IsThreeButton() )
  {
    // 3択
    btnMan->SetButtonInputEnable( BTN_ID_THREE_BTN_00, true );
    btnMan->SetButtonInputEnable( BTN_ID_THREE_BTN_01, true );
    btnMan->SetButtonInputEnable( BTN_ID_THREE_BTN_02, true );
  }
  else {
    // 2択
    btnMan->SetButtonInputEnable( BTN_ID_TWO_BTN_00, true );
    btnMan->SetButtonInputEnable( BTN_ID_TWO_BTN_01, true );
  }
}

/**
 * @brief   ボタンテキストの設定
 *
 * @param   btnID   ボタンID
 * @param   msgID   メッセージID
 */
void SelectButtonView::_SetButtonText( u32 btnID, u32 msgID )
{
  GFL_ASSERT( btnID < BTN_ID_NUM );

  // テキストの設定
  SetTextboxPaneMessage( m_pButtonText[btnID], msgID );
}

/**
 * @brief   ボタンSEの設定
 *
 * @param   btnID   ボタンID
 * @param   seID    SEのID
 */
void SelectButtonView::_SetButtonSE( u32 btnID, u32 seID )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  // SEの設定
  btnMan->SetButtonSelectSE( btnID, seID );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーソル
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カーソル移動
 */
void SelectButtonView::_MoveCursor( u32 nextPos )
{
  m_CursorPos = nextPos % m_ButtonNum;

  // カーソルの配置
  gfl2::lyt::LytPane* cursorPane = _GetCursorPane( m_CursorPos );
  this->PutCursor( cursorPane );

  Sound::PlaySE( SEQ_SE_SELECT1 );
}

/**
 * @brief   カーソル配置ペインの取得
 *
 * @param   pos   カーソル位置：2ボタン(0～1), 3ボタン(0～2)
 *
 * @return  カーソル配置ペイン
 */
gfl2::lyt::LytPane* SelectButtonView::_GetCursorPane( u32 pos )
{
  if( _IsThreeButton() )
  {
    return m_pThreeBtnCursorPane[pos];
  }

  return m_pTwoBtnCursorPane[pos];
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ３つボタンかどうか
 *
 * @retval  "true"なら３つボタン
 * @retval  "false"なら２つボタン
 */
bool SelectButtonView::_IsThreeButton( void )
{
  if( m_ButtonType == BTN_TYPE_FINISH_PREPARE ) return true;
  if( m_ButtonType == BTN_TYPE_FINISH_MAIN ) return true;

  return false;
}


/**
 * @brief   ボタンがアクションか
 *
 * @retval  "true"なら何かしらアクション中
 * @retval  "false"ならそれ以外
 */
bool SelectButtonView::_IsButtonAction( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  return ( btnMan->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ボタンアクション
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンアクション：難易度選択
 */
void SelectButtonView::ButtonAction_Difficulty( const u32 paneID )
{
  FUKUSHIMA_PRINT( "ButtonAction_Difficulty : paneID=[%d]\n", paneID );

  if( paneID == BTN_ID_TWO_BTN_00 )
  {
    // お手軽撮影
    this->SetResult( RES_SELECT_BTN_00 );
  }
  else if( paneID == BTN_ID_TWO_BTN_01 )
  {
    // 本格撮影
    this->SetResult( RES_SELECT_BTN_01 );
  }
}

/**
 * @brief   ボタンアクション：撮影モード選択
 */
void SelectButtonView::ButtonAction_CaptureMode( const u32 paneID )
{
  FUKUSHIMA_PRINT( "ButtonAction_CaptureMode : paneID=[%d]\n", paneID );

  if( paneID == BTN_ID_TWO_BTN_00 )
  {
    // ツーショット撮影
    m_pListener->SetCaptureMode( Viewer::CAPTURE_MODE_BOTH );
  }
  else if( paneID == BTN_ID_TWO_BTN_01 )
  {
    // ポケモンだけ撮影
    m_pListener->SetCaptureMode( Viewer::CAPTURE_MODE_POKE );
  }
}

/**
 * @brief   ボタンアクション：難易度選択からの終了確認
 */
void SelectButtonView::ButtonAction_FinishYesNo( const u32 paneID )
{
  FUKUSHIMA_PRINT( "ButtonAction_PreFinConfirm : paneID=[%d]\n", paneID );

  if( paneID == BTN_ID_TWO_BTN_00 )
  {
    // はい
    this->SetResult( RES_SELECT_BTN_00 );
  }
  else if( paneID == BTN_ID_TWO_BTN_01 )
  {
    // いいえ
    this->SetResult( RES_SELECT_BTN_01 );
  }
}

/**
 * @brief   ボタンアクション：準備選択からの終了確認
 */
void SelectButtonView::ButtonAction_FinishPrepare( const u32 paneID )
{
  FUKUSHIMA_PRINT( "ButtonAction_FinishPrepare : paneID=[%d]\n", paneID );

  if( paneID == BTN_ID_THREE_BTN_00 )
  {
    // つづける
    this->SetResult( RES_SELECT_BTN_00 );
  }
  else if( paneID == BTN_ID_THREE_BTN_01 )
  {
    // 難易度選択へ
    this->SetResult( RES_SELECT_BTN_01 );
  }
  else if( paneID == BTN_ID_THREE_BTN_02 )
  {
    // 終わる
    this->SetResult( RES_SELECT_BTN_02 );
  }
}

/**
 * @brief   ボタンアクション：準備選択からの終了確認
 */
void SelectButtonView::ButtonAction_FinishPrepareLock( const u32 paneID )
{
  FUKUSHIMA_PRINT( "ButtonAction_FinishPrepareLock : paneID=[%d]\n", paneID );

  if( paneID == BTN_ID_TWO_BTN_00 )
  {
    // つづける
    this->SetResult( RES_SELECT_BTN_00 );
  }
  else if( paneID == BTN_ID_TWO_BTN_01 )
  {
    // 終わる
    this->SetResult( RES_SELECT_BTN_02 );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンアクション開始イベント
 */
void SelectButtonView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  // カーソル位置の設定
  if( _IsThreeButton() )
  {
    m_CursorPos = paneId;
  }
  else {
    m_CursorPos = paneId - BTN_ID_TWO_BTN_00;
  }

  gfl2::lyt::LytPane* cursorPane = _GetCursorPane( m_CursorPos );
  this->PutCursor( cursorPane );

  // SE再生
  if( m_ButtonType == BTN_TYPE_CAPTURE_MODE )
  {
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }

  /// ボタンアクション開始なので無効に
  m_pPreCmnViewListener->SetInputEnabledCommonButton( false );
}

/**
 * @brief   ボタンアクションイベント
 */
app::ui::UIInputListener::ListenerResult SelectButtonView::OnLayoutPaneEvent( const u32 paneId )
{
  void ( SelectButtonView::*BUTTON_ACTION_FUNC[BTN_TYPE_NUM] )( const u32 ) = {
    &SelectButtonView::ButtonAction_Difficulty,
    &SelectButtonView::ButtonAction_CaptureMode,
    &SelectButtonView::ButtonAction_FinishYesNo,
    &SelectButtonView::ButtonAction_FinishPrepare,
    &SelectButtonView::ButtonAction_FinishPrepareLock,
    &SelectButtonView::ButtonAction_FinishPrepare,
  };

  if( m_pListener )
  {
    // ボタンタイプごとのボタンアクション
    ( this->*BUTTON_ACTION_FUNC[m_ButtonType] )( paneId );
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   キーアクションイベント
 */
app::ui::UIInputListener::ListenerResult SelectButtonView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
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

    if( _IsThreeButton() )
    {
      btnMan->StartSelectedAct( m_CursorPos );
    }
    else {
      btnMan->StartSelectedAct( m_CursorPos + BTN_ID_TWO_BTN_00 );
    }
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
