//======================================================================
/**
 * @file    FinderStudioCaptureCompositionView.cpp
 * @date    2017/01/20 13:24:00
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：構図選択View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureCompositionView.h"
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
/// @brief    コンストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
CompositionView::CompositionView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf )
  : CaptureViewBase( heap, renderMan, cursorBuf )
  , m_pPreCmnViewListener( NULL )
  , m_Seq( 0 )
{
  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    m_pCursorPane[i] = NULL;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void CompositionView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ボタンの初期化
  InitButton();
  // カーソルの初期化
  this->CreateMenuCursor();
  // キーリピートの生成
  this->CreateKeyRepeatController( 0, BTN_ID_NUM );
}

/**
 * @brief   2Dの初期化
 */
void CompositionView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_05_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_05___NUM,
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
 * @brief   ボタンの初期化
 */
void CompositionView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_05__POSE_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_05__POSE_01_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_05_PANE_POSE_00,
    PANENAME_STUDIO_LIST_LOW_05_PANE_POSE_01,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
  };

  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    Static::ButtonInfo::SetupBtnInfo( &info[i], i, c_ButtonAnim[i] );

    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ButtonParts[i] );
    gfl2::lyt::LytPane*  bound = m_pUtil->GetPane( c_ButtonBounding[i], parts );

    m_pCursorPane[i] = m_pUtil->GetPane( PANENAME_STUDIO_LIST_LOW_05_PANE_NULL_CUR_00 + i );

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
bool CompositionView::Terminate( void )
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
void CompositionView::Update( void )
{
  this->_Update();

  if( m_Seq == 1 )
  {
    /// 描画を有効
    m_bDrawEnable = true;
    /// 入力を有効
    this->SetInputEnabled( true );

    m_Seq = 0;
  }

  /// ボタンアクション中か？
  if( !_IsButtonAction() )
  {
    /// 共通ボタン
    _UpdateCommonButton();
  }
}

/**
 * @brief   更新：共通ボタン
 */
void CompositionView::_UpdateCommonButton( void )
{
  if( !m_bDrawEnable )
  {
    return;
  }
  if( !m_pPreCmnViewListener )
  {
    return;
  }

  /// ボタンアクション終了なので有効に
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
void CompositionView::Show( void )
{
  // 構図設定
  _SetComposition();
  // 表示
  this->SetVisible( true );

  m_Seq = 1;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   設定：構図画面
 */
void CompositionView::_SetComposition( void )
{
  _SetComposition_Button();
  _SetComposition_Cursor();
}

/**
 * @brief   設定：構図ボタン
 */
void CompositionView::_SetComposition_Button( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  // 構図１
  if( m_pListener->IsCanUseComposition( Viewer::COMPOSITION_1 ) )
  {
    btnMan->SetButtonActiveAnimeFrameLast( BTN_ID_POSE_01 );
  }
  else {
    btnMan->SetButtonPassiveAnimeFrameLast( BTN_ID_POSE_01 );
  }

  // 構図２
  if( m_pListener->IsCanUseComposition( Viewer::COMPOSITION_2 ) )
  {
    btnMan->SetButtonActiveAnimeFrameLast( BTN_ID_POSE_02 );
  }
  else {
    btnMan->SetButtonPassiveAnimeFrameLast( BTN_ID_POSE_02 );
  }
}

/**
 * @brief   設定：カーソル
 */
void CompositionView::_SetComposition_Cursor( void )
{
  u32 nextPos = 1;

  if( m_pListener->IsCanUseComposition( Viewer::COMPOSITION_1 ) )
  {
    nextPos = 0;
  }

  /// カーソル位置の設定
  _SetCursorPosWithMessage( nextPos );
}

/**
 * @brief   設定：構図リザルト
 */
void CompositionView::_SetCompositionEnableResult( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  // ボタンがアクティブかどうか
  if( btnMan->IsButtonActive( m_CursorPos ) )
  {
    this->SetResult( RES_ENABLE );
  }
  else {
    this->SetResult( RES_DISABLE );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーソル：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カーソル移動
 *
 * @param   nextPos   次の位置
 */
void CompositionView::_CursorMove( u32 nextPos )
{
  u32 pos = nextPos % BTN_ID_NUM;

  /// カーソル位置の設定
  _SetCursorPosWithMessage( pos );

  Sound::PlaySE( SEQ_SE_SELECT1 );
}

/**
 * @brief   カーソル位置の設定
 *
 * @param[in]   pos   カーソル位置
 */
void CompositionView::_SetCursorPos( u32 pos )
{
  /// カーソル位置の更新
  m_CursorPos = pos;

  /// カーソル位置の反映
  gfl2::lyt::LytPane* cursorPane = m_pCursorPane[m_CursorPos];
  this->PutCursor( cursorPane );
}

/**
 * @brief   カーソル位置の設定とメッセージの反映
 *
 * @param[in]   pos   カーソル位置
 */
void CompositionView::_SetCursorPosWithMessage( u32 pos )
{
  /// カーソル位置の設定
  _SetCursorPos( pos );

  // 選択中の構図のリザルトを設定
  _SetCompositionEnableResult();
}






//////////////////////////////////////////////////////////////////////////
///
/// @brief    ボタン：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンがアクションか
 *
 * @retval  "true"なら何かしらアクション中
 * @retval  "false"ならそれ以外
 */
bool CompositionView::_IsButtonAction( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  return ( btnMan->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関連
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   ボタンアクション開始イベント
*/
void CompositionView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  /// カーソル位置の設定
  _SetCursorPosWithMessage( paneId );

  // SE再生
  Sound::PlaySE( SEQ_SE_DECIDE1 );

  /// ボタンアクション開始なので無効に
  m_pPreCmnViewListener->SetInputEnabledCommonButton( false );
}

/**
* @brief   レイアウトアクションイベント
*/
app::ui::UIInputListener::ListenerResult CompositionView::OnLayoutPaneEvent( const u32 paneId )
{
  if( paneId == BTN_ID_POSE_01 )
  {
    m_pListener->SetComposition( Viewer::COMPOSITION_1 );
  }
  else if( paneId == BTN_ID_POSE_02 )
  {
    m_pListener->SetComposition( Viewer::COMPOSITION_2 );
  }

  return ENABLE_ACCESS;
}

/**
* @brief   キーアクションイベント
*/
app::ui::UIInputListener::ListenerResult CompositionView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  m_pKeyController->UpdateInfo( key, 0, m_CursorPos );

  if( m_pKeyController->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    u32 nextPos = ( m_CursorPos + BTN_ID_NUM - 1 );

    _CursorMove( nextPos );
  }
  else if( m_pKeyController->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    u32 nextPos = ( m_CursorPos + 1 );

    _CursorMove( nextPos );
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
    btnMan->StartSelectedAct( BTN_ID_POSE_01 + m_CursorPos );
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
