//======================================================================
/**
 * @file    FinderStudioCaptureShootMainFrame.cpp
 * @date    2016/11/09 20:36:29
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影メインフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureShootMainFrame.h"

#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioInformationWindow.h>

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioCapture/source/FinderStudioCaptureViewerManager.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureUpperView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureBGView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureShootView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureShootCommonView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureZMotionView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureCameraView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureSelectButtonView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCapturePrepareCommonView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureShootLayoutView.h>

#include <App/FinderStudioViewer/include/FinderStudioViewer.h>

#include <Fade/include/gfl2_FadeManager.h>

#include <niji_conv_header/message/msg_fs_photo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 定数宣言 begin ]---
/// フェードイン用のカラー
static const gfl2::math::Vector4 FADE_IN_COLOR( 0, 0, 0, 0 );
/// シャッター音
static const u32 SE_ID_SHUTTER = SEQ_SE_MJ_SYS_030;
/// 表示する写真のインデックス
static const u8  PHOTO_IMAGE_INDEX = 0;
/// ---[ 定数宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
ShootMainFrame::ShootMainFrame( void )
  : m_pUpperView( NULL )
  , m_pBGView( NULL )
  , m_pShootView( NULL )
  , m_pShootCommonView( NULL )
  , m_pZMotionView( NULL )
  , m_pCameraView( NULL )
  , m_pSelectButtonView( NULL )
  , m_pPrepareCommonView( NULL )
  , m_pLayoutView( NULL )
  , m_ShutterCloseSeq( 0 )
  , m_ShutterOpenSeq( 0 )
  , m_SeSeq( 0 )
  , m_SeCount( 0 )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
applib::frame::Result ShootMainFrame::InitFunc( void )
{
  GFL_PRINT( "ShootMainFrame Start\n" );

  Static::ViewList* viewList = m_pMainWork->viewList;

  /// 上画面View
  m_pUpperView = reinterpret_cast<UpperView*>( viewList->GetView(VIEW_ID_UPPER) );
  // 背景View
  m_pBGView = reinterpret_cast<BGView*>( viewList->GetView(VIEW_ID_BG) );
  /// 撮影メインView
  m_pShootView = reinterpret_cast<ShootView*>( viewList->GetView( VIEW_ID_SHOOT ) );
  /// 撮影共通View
  m_pShootCommonView = reinterpret_cast<ShootCommonView*>( viewList->GetView(VIEW_ID_SHOOT_COMMON) );
  /// Zモーション選択View
  m_pZMotionView = reinterpret_cast<ZMotionView*>( viewList->GetView( VIEW_ID_ZMOTION ) );
  /// 位置変更View
  m_pCameraView = reinterpret_cast<CameraView*>( viewList->GetView( VIEW_ID_CAMERA ) );
  /// 汎用ボタン選択View
  m_pSelectButtonView = reinterpret_cast<SelectButtonView*>( viewList->GetView( VIEW_ID_SELECT_BUTTON ) );
  // 準備共通View
  m_pPrepareCommonView = reinterpret_cast<PrepareCommonView*>( viewList->GetView( VIEW_ID_PREPARE_COMMON ) );
  // 位置調整View
  m_pLayoutView = reinterpret_cast<LayoutView*>( viewList->GetView(VIEW_ID_LAYOUT) );
  /// 汎用メッセージウィンドウView
  m_pMsgMenuView = viewList->GetMessageMenuView();

  return applib::frame::RES_FINISH;
}

/**
 * @brief   終了
 */
applib::frame::Result ShootMainFrame::EndFunc( void )
{
  GFL_PRINT( "ShootMainFrame Finish\n" );

  m_pMsgMenuView->RemoveListener();

  m_pLayoutView->Hide();
  m_pShootCommonView->Hide();
  m_pSelectButtonView->Hide();
  m_pZMotionView->Hide();
  m_pCameraView->Hide();
  m_pShootView->Hide();

  return applib::frame::RES_FINISH;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
applib::frame::Result ShootMainFrame::UpdateFunc( void )
{
#if PM_DEBUG
  if( DebugClass::IsAnyDebugMode() )
  {
    return applib::frame::RES_FINISH;
  }

  if(!_GetViewerManager()->GetFinderStudioViewer()->IsPrepareCapture())
  {
    GFL_ASSERT_MSG(0,"撮影の準備ができていません");
  }
#endif

  Result( ShootMainFrame::*UPDATE_FUNC_TABLE[] )( void ) = {
    &ShootMainFrame::_UpdateShootMain,
    &ShootMainFrame::_UpdateSelectZMotion,
    &ShootMainFrame::_UpdateCameraControl,
    &ShootMainFrame::_UpdateShootLayout,
    &ShootMainFrame::_UpdateShootLayout,
    &ShootMainFrame::_UpdateShootLayout,
    &ShootMainFrame::_UpdateFinishConfirm,
    &ShootMainFrame::_UpdateCapture,
  };

  // 共通Viewの更新
  _UpdateShootCommonView();

  // 各準備画面の更新
  if( ( this->*UPDATE_FUNC_TABLE[m_MainSeq] )( ) == RESULT_FINISH )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：撮影メイン
 */
ShootMainFrame::Result ShootMainFrame::_UpdateShootMain( void )
{
  enum {
    SHOOT_SEQ_INIT,
    SHOOT_SEQ_SHOW,
    SHOOT_SEQ_MAIN,
  };

  if( m_SubSeq == SHOOT_SEQ_INIT )
  {
    // View表示の初期化
    m_pShootView->InitShowView();

    this->SetSubSeq( SHOOT_SEQ_SHOW );
  }
  else if( m_SubSeq == SHOOT_SEQ_SHOW )
  {
    // リスナーを解除
    m_pMsgMenuView->RemoveListener();

    // 背景Viewの表示
    _ShowBGView();
    // 撮影メインViewの表示
    _ShowShootView();
    // 撮影メイン共通Viewの表示
    _ShowShootCommonView();
    // カメラViewを非表示
    _HideCameraView();
    // 位置調整Viewを非表示
    _HideLayoutView();
    // ZモーションViewを非表示
    _HideZMotionView();
    // 終了確認Viewを非表示
    _HideFinishConfirmView();

    // リスナーを登録
    m_pMsgMenuView->SetListener( m_pShootView );

    this->SetSubSeq( SHOOT_SEQ_MAIN );
  }
  else if( m_SubSeq == SHOOT_SEQ_MAIN )
  {
    // 撮影トップ
    u32 result = m_pShootView->GetResult();
    m_pShootView->SetResult( ShootView::RES_CONTINUE );

    // Zモーション選択Viewのコール
    if( result == ShootView::RES_CALL_SELECT_ZMOTION )
    {
      // Zモーション選択シーケンスへ
      this->SetMainSeq( SEQ_SELECT_ZMOTION );
    }
    // 写真撮影完了メッセージウィンドウの表示
    else if( result == ShootView::RES_CAPTURE )
    {
      // 撮影シーケンスへ
      this->SetMainSeq( SEQ_CAPTURE );
    }
    // ポケモンモーションタイプ変更でのリセット確認
    else if( result == ShootView::RES_MOTION_TYPE_CONFIRM )
    {
      // リセット確認のメッセージウィンドウを表示
      m_pShootView->SetMsgWndType( ShootView::MSGWND_TYPE_MOTION_TYPE_CONFIRM );
      m_pMsgMenuView->ShowMessageWithListMenu( Static::MessageMenuView::MENU_TYPE_YESNO_CONFIRM, fs_photo_cap_109 );
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：Zモーション選択
 */
ShootMainFrame::Result ShootMainFrame::_UpdateSelectZMotion( void )
{
  enum {
    ZMOTION_SEQ_SHOW,
    ZMOTION_SEQ_MAIN,
  };

  if( m_SubSeq == ZMOTION_SEQ_SHOW )
  {
    // リスナーを解除
    m_pMsgMenuView->RemoveListener();

    // Zモーション選択画面の表示
    _ShowZMotionView();
    // 背景Viewを非表示
    _HideBGView();
    // 撮影メインViewを非表示
    _HideShootView();
    // 撮影メイン共通Viewを非表示
    _HideShootCommonView();
    // カメラViewを非表示
    _HideCameraView();
    // 位置調整Viewを非表示
    _HideLayoutView();
    // 終了確認Viewを非表示
    _HideFinishConfirmView();

    // リスナーを登録
    m_pMsgMenuView->SetListener( m_pZMotionView );

    this->SetSubSeq( ZMOTION_SEQ_MAIN );
  }
  else if( m_SubSeq == ZMOTION_SEQ_MAIN )
  {
    // Zモーション選択画面のリザルトを取得
    u32 result = m_pZMotionView->GetResult();
    m_pZMotionView->SetResult( ZMotionView::RES_CONTINUE );

    if( result == ZMotionView::RES_CLOSE )
    {
      // 撮影メインシーケンスに戻す
      this->SetMainSeq( SEQ_SHOOT_MAIN );
    }
    else if( result == ZMotionView::RES_CALL_MSGWND )
    {
      // メッセージウィンドウを表示
      m_pMsgMenuView->ShowMessage_Id( fs_photo_win_29 );
    }
    else if( result == ZMotionView::RES_MSGWND_CLOSE )
    {
      // メッセージウィンドウを閉じる
      m_pMsgMenuView->HideMessage();
      // Zモーション選択画面の非表示を開始
      m_pZMotionView->StartHideView();
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：カメラ操作
 */
ShootMainFrame::Result ShootMainFrame::_UpdateCameraControl( void )
{
  enum {
    CAMERA_SEQ_SHOW,
    CAMERA_SEQ_MAIN,
  };

  if( m_SubSeq == CAMERA_SEQ_SHOW )
  {
    // カメラViewの表示
    _ShowCameraView();
    // 背景Viewを非表示
    _HideBGView();
    // 撮影メインViewを非表示
    _HideShootView();
    // 位置調整Viewを非表示
    _HideLayoutView();
    // ZモーションViewを非表示
    _HideZMotionView();
    // 終了確認Viewを非表示
    _HideFinishConfirmView();

    // カメラViewのチュートリアルの表示
    m_pCameraView->ShowTutorial();
    
    this->SetSubSeq( CAMERA_SEQ_MAIN );
  }
  else if( m_SubSeq == CAMERA_SEQ_MAIN )
  {
    // カメラViewのリザルトを取得
    u32 result = m_pCameraView->GetResult();
    m_pCameraView->SetResult( RESULT_CONTINUE );

    // インフォの表示
    if( result == CameraView::RES_INFO_SHOW )
    {
      // 共通View：入力を無効に
      m_pShootCommonView->SetInputEnabled( false );
    }
    // インフォの非表示
    else if( result == CameraView::RES_INFO_HIDE )
    {
      // 共通View：入力有効に
      m_pShootCommonView->SetInputEnabled( true );
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：位置調整画面(移動・回転)
 */
ShootMainFrame::Result ShootMainFrame::_UpdateShootLayout( void )
{
  enum {
    LAYOUT_SEQ_INIT,
    LAYOUT_SEQ_SHOW,
    LAYOUT_SEQ_MAIN,
  };

  if( m_SubSeq == LAYOUT_SEQ_INIT )
  {
    // Viewタイプの設定
    _SetLayoutViewType();
    this->SetSubSeq( LAYOUT_SEQ_SHOW );
  }
  else if( m_SubSeq == LAYOUT_SEQ_SHOW )
  {
    // 位置調整Viewの表示
    _ShowLayoutView();
    // 背景Viewを非表示
    _HideBGView();
    // 撮影メインViewを非表示
    _HideShootView();
    // カメラViewを非表示
    _HideCameraView();
    // ZモーションViewを非表示
    _HideZMotionView();
    // 終了確認Viewを非表示
    _HideFinishConfirmView();

    // チュートリアルの表示
    m_pLayoutView->ShowTutorial();
    
    this->SetSubSeq( LAYOUT_SEQ_MAIN );
  }
  else if( m_SubSeq == LAYOUT_SEQ_MAIN )
  {
    // 位置調整Viewのリザルト取得
    u32 result = m_pLayoutView->GetResult();
    m_pLayoutView->SetResult( RESULT_CONTINUE );

    // インフォの表示
    if( result == LayoutView::RES_INFO_SHOW )
    {
      // 共通View：入力を無効に
      m_pShootCommonView->SetInputEnabled( false );
    }
    // インフォの非表示
    else if( result == LayoutView::RES_INFO_HIDE )
    {
      // 共通View：入力有効に
      m_pShootCommonView->SetInputEnabled( true );
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：終了確認
 */
ShootMainFrame::Result ShootMainFrame::_UpdateFinishConfirm( void )
{
  enum {
    FIN_SEQ_SHOW,
    FIN_SEQ_MAIN,
  };

  if( m_SubSeq == FIN_SEQ_SHOW )
  {
    // 背景Viewの表示
    _ShowBGView();
    // 終了確認Viewの表示
    _ShowFinishConfirmView();
    // 位置調整Viewを非表示
    _HideLayoutView();
    // 撮影メインViewを非表示
    _HideShootView();
    // 撮影メイン共通Viewを非表示
    _HideShootCommonView();
    // カメラViewを非表示
    _HideCameraView();
    // ZモーションViewを非表示
    _HideZMotionView();

    this->SetSubSeq( FIN_SEQ_MAIN );
  }
  else if( m_SubSeq == FIN_SEQ_MAIN )
  {
    // 終了確認Viewのリザルトを取得
    u32 result = m_pSelectButtonView->GetResult();
    m_pSelectButtonView->SetResult( SelectButtonView::RES_CONTINUE );

    if( result != SelectButtonView::RES_CONTINUE )
    {
      m_pSelectButtonView->SetInputEnabled( false );

      // 「つづける」を選択
      if( result == SelectButtonView::RES_SELECT_BTN_00 )
      {
        this->SetMainSeq( SEQ_SHOOT_MAIN );
      }
      // 「準備をやり直す」を選択
      else if( result == SelectButtonView::RES_SELECT_BTN_01 )
      {
        this->GetFrameManager()->SetResult( Frame::RES_PREV );
        return RESULT_FINISH;
      }
      // 「やめる」を選択
      else if( result == SelectButtonView::RES_SELECT_BTN_02 )
      {
        this->GetFrameManager()->SetResult( Frame::RES_FINISH );
        return RESULT_FINISH;
      }
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：撮影
 */
ShootMainFrame::Result ShootMainFrame::_UpdateCapture( void )
{
  enum {
    CAP_SEQ_SHUTTER_CLOSE,
    CAP_SEQ_CAPTURE,
    CAP_SEQ_IMAGE,
    CAP_SEQ_SHUTTER_OPEN,
    CAP_SEQ_MSGWND,
    CAP_SEQ_MSGWND_WAIT,
    CAP_SEQ_END,
  };

  /// シャッター演出：閉じる
  if( m_SubSeq == CAP_SEQ_SHUTTER_CLOSE )
  {
    bool bEnd = true;

    // シャッター演出：アニメ～フェードアウト
    bEnd &= _UpdateCapture_ShutterClsoe();
    // シャッター演出：シャッター音(６回)
    bEnd &= _UpdateCapture_ShutterSE();

    if( bEnd )
    {
      m_SubSeq = CAP_SEQ_CAPTURE;
    }
  }
  /// 撮影完了チェック
  if( m_SubSeq == CAP_SEQ_CAPTURE )
  {
    if( !_IsFinishCapture() ) { return RESULT_CONTINUE; }
    this->SetSubSeq( CAP_SEQ_IMAGE );
  }
  /// 撮影が完了したら写真を表示する
  if( m_SubSeq == CAP_SEQ_IMAGE )
  {
    void* photoTex = _GetPhotoTexture( PHOTO_IMAGE_INDEX );
    m_pUpperView->RequestPhotoImage( photoTex );
    this->SetSubSeq( CAP_SEQ_SHUTTER_OPEN );
  }
  /// シャッター演出：開く
  if( m_SubSeq == CAP_SEQ_SHUTTER_OPEN )
  {
    if( _UpdateCapture_ShutterOpen() )
    {
      this->SetSubSeq( CAP_SEQ_MSGWND );
    }
  }
  /// 撮影完了メッセージの表示
  if( m_SubSeq == CAP_SEQ_MSGWND )
  {
    m_pShootView->SetMsgWndType( ShootView::MSGWND_TYPE_CAPTURE_FINISH );
    m_pMsgMenuView->ShowMessage_Id( fs_photo_win_28 );

    this->SetSubSeq( CAP_SEQ_MSGWND_WAIT );
  }
  /// 撮影完了メッセージの終了待ち
  if( m_SubSeq == CAP_SEQ_MSGWND_WAIT )
  {
    u32 result = m_pShootView->GetResult();
    m_pShootView->SetResult( ShootView::RES_CONTINUE );

    if( result == ShootView::RES_CALL_PHOTO_LIST )
    {
      this->SetSubSeq( CAP_SEQ_END );
    }
  }
  /// 撮影演出終了。写真保存リストへ
  if( m_SubSeq == CAP_SEQ_END )
  {
    m_pMsgMenuView->HideMessage();
    this->GetFrameManager()->SetResult( Frame::RES_NEXT );
    return RESULT_FINISH;
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：共通View
 */
ShootMainFrame::Result ShootMainFrame::_UpdateShootCommonView( void )
{
  u32 result = m_pShootCommonView->GetResult();
  m_pShootCommonView->SetResult( ShootCommonView::RES_CONTINUE );

  if( result != ShootCommonView::RES_CONTINUE )
  {
    _SetCommonViewResult( result );
  }

  _SetCommonViewInputEnabled();

  return RESULT_CONTINUE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影シーケンス：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   シャッター演出：閉じる
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool ShootMainFrame::_UpdateCapture_ShutterClsoe( void )
{
  enum {
    SHUTTER_CLOSE_SEQ_START,
    SHUTTER_CLOSE_SEQ_WAIT,
    SHUTTER_CLOSE_SEQ_FADEOUT,
    SHUTTER_CLOSE_SEQ_END,
  };

  if( m_ShutterCloseSeq == SHUTTER_CLOSE_SEQ_START )
  {
    // シャッターを閉じる
    m_pUpperView->ShutterClose();
    m_ShutterCloseSeq = SHUTTER_CLOSE_SEQ_WAIT;
  }
  if( m_ShutterCloseSeq == SHUTTER_CLOSE_SEQ_WAIT )
  {
    if( m_pUpperView->IsShutterClose() )
    {
      m_ShutterCloseSeq = SHUTTER_CLOSE_SEQ_FADEOUT;
    }
  }
  if( m_ShutterCloseSeq == SHUTTER_CLOSE_SEQ_FADEOUT )
  {
    // フェードアウト
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut( gfl2::Fade::DISP_UPPER );
    m_ShutterCloseSeq = SHUTTER_CLOSE_SEQ_END;
  }
  if( m_ShutterCloseSeq == SHUTTER_CLOSE_SEQ_END )
  {
    return true;
  }

  return false;
}

/**
 * @brief   シャッター演出：開く
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool ShootMainFrame::_UpdateCapture_ShutterOpen( void )
{
  enum {
    SHUTTER_OPEN_SEQ_FADEIN,
    SHUTTER_OPEN_SEQ_START,
    SHUTTER_OPEN_SEQ_WAIT,
    SHUTTER_OPEN_SEQ_END,
  };

  if( m_ShutterOpenSeq == SHUTTER_OPEN_SEQ_FADEIN )
  {
    // フェードイン
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceOut( gfl2::Fade::DISP_UPPER, &FADE_IN_COLOR );
    m_ShutterOpenSeq = SHUTTER_OPEN_SEQ_START;
  }
  if( m_ShutterOpenSeq == SHUTTER_OPEN_SEQ_START )
  {
    // シャッターを開く
    m_pUpperView->ShutterOpen();
    m_ShutterOpenSeq = SHUTTER_OPEN_SEQ_WAIT;
  }
  if( m_ShutterOpenSeq == SHUTTER_OPEN_SEQ_WAIT )
  {
    if( m_pUpperView->IsShutterOpen() )
    {
      m_ShutterOpenSeq = SHUTTER_OPEN_SEQ_END;
    }
  }
  if( m_ShutterOpenSeq == SHUTTER_OPEN_SEQ_END )
  {
    return true;
  }

  return false;
}

/**
 * @brief   SE演出
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool ShootMainFrame::_UpdateCapture_ShutterSE( void )
{
  enum {
    SHUTTER_SE_SEQ_START,
    SHUTTER_SE_SEQ_WAIT,
    SHUTTER_SE_SEQ_END,
  };

  if( m_SeSeq == SHUTTER_SE_SEQ_START )
  {
    // シャッター音の再生
    Sound::PlaySE( SE_ID_SHUTTER );
    m_SeSeq = SHUTTER_SE_SEQ_WAIT;
  }
  if( m_SeSeq == SHUTTER_SE_SEQ_WAIT )
  {
    // SEが鳴り終わったかチェック
    if( Sound::IsSEFinished( SE_ID_SHUTTER ) )
    {
      m_SeSeq = SHUTTER_SE_SEQ_END;
    }
  }
  if( m_SeSeq == SHUTTER_SE_SEQ_END )
  {
    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    位置調整View関連：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   設定：位置調整Viewタイプ
 */
void ShootMainFrame::_SetLayoutViewType( void )
{
  LayoutView::LayoutType type = LayoutView::TYPE_PLAYER_ROTATE;

  if( m_MainSeq == SEQ_POKEMON_MOVE )
  {
    type = LayoutView::TYPE_POKE_MOVE;
  }
  else if( m_MainSeq == SEQ_POKEMON_ROTATE )
  {
    type = LayoutView::TYPE_POKE_ROTATE;
  }

  m_pLayoutView->SetLayoutType( type );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    共通View関連：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   入力有効設定の更新
 */
void ShootMainFrame::_SetCommonViewInputEnabled( void )
{
  switch( m_MainSeq )
  {
    case SEQ_SHOOT_MAIN:
    {
      // 入力有効状態を設定
      m_pShootCommonView->SetInputEnabled( m_pShootView->IsInputEnabled() );
    } break;

    case SEQ_CAMERA_CONTROL:
    {
      FUKUSHIMA_PRINT( "CameraView : InputEnable=[%d]\n", m_pCameraView->IsInputEnabled() );
      // 入力有効状態を設定
      m_pShootCommonView->SetInputEnabled( m_pCameraView->IsInputEnabled() );
    } break;
    
    case SEQ_POKEMON_MOVE:
    case SEQ_POKEMON_ROTATE:
    case SEQ_TRAINER_ROTATE:
    {
      // 入力有効状態を設定
      m_pShootCommonView->SetInputEnabled( m_pLayoutView->IsInputEnabled() );
    } break;
  }
}

/**
 * @brief   リザルトの更新
 */
void ShootMainFrame::_SetCommonViewResult( u32 result )
{
  switch( m_MainSeq )
  {
    case SEQ_SHOOT_MAIN:
    {
      _SetCommonViewResult_ShootMain( result );
    } break;
    
    case SEQ_CAMERA_CONTROL:
    case SEQ_POKEMON_MOVE:
    case SEQ_POKEMON_ROTATE:
    case SEQ_TRAINER_ROTATE:
    {
      _SetCommonViewResult_ShootOther( result );
    } break;
  }
}

/**
 * @brief   リザルトの更新：撮影メイン
 */
void ShootMainFrame::_SetCommonViewResult_ShootMain( u32 result )
{
  if( result == ShootCommonView::RES_BACK )
  {
    _HideShootCommonView();

    this->SetMainSeq( SEQ_FINISH_CONFIRM );
  }
  else if( result == ShootCommonView::RES_CAMERA )
  {
    this->SetMainSeq( SEQ_CAMERA_CONTROL );
  }
  else if( result == ShootCommonView::RES_POKE_MOVE )
  {
    _SetPokemonMotionAutoPlayEnable( false );
    this->SetMainSeq( SEQ_POKEMON_MOVE );
  }
  else if( result == ShootCommonView::RES_POKE_ROT )
  {
    _SetPokemonMotionAutoPlayEnable( false );
    this->SetMainSeq( SEQ_POKEMON_ROTATE );
  }
  else if( result == ShootCommonView::RES_PLAYER_ROT )
  {
    _SetPokemonMotionAutoPlayEnable( false );
    this->SetMainSeq( SEQ_TRAINER_ROTATE );
  }
}

/**
 * @brief   リザルト更新：撮影その他
 */
void ShootMainFrame::_SetCommonViewResult_ShootOther( u32 result )
{
  if( result == ShootCommonView::RES_BACK || result == ShootCommonView::RES_TOP )
  {
    _SetPokemonMotionAutoPlayEnable( true );
    this->SetMainSeq( SEQ_SHOOT_MAIN );
  }
  else if( result == ShootCommonView::RES_CAMERA )
  {
    _SetPokemonMotionAutoPlayEnable( true );
    this->SetMainSeq( SEQ_CAMERA_CONTROL );
  }
  else if( result == ShootCommonView::RES_POKE_MOVE )
  {
    _SetPokemonMotionAutoPlayEnable( false );
    this->SetMainSeq( SEQ_POKEMON_MOVE );
  }
  else if( result == ShootCommonView::RES_POKE_ROT )
  {
    _SetPokemonMotionAutoPlayEnable( false );
    this->SetMainSeq( SEQ_POKEMON_ROTATE );
  }
  else if( result == ShootCommonView::RES_PLAYER_ROT )
  {
    _SetPokemonMotionAutoPlayEnable( false );
    this->SetMainSeq( SEQ_TRAINER_ROTATE );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    画面 表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   撮影トップ画面の表示
 */
void ShootMainFrame::_ShowShootView( void )
{
  m_pShootView->Show();
}

/**
 * @brief   撮影トップ画面の非表示
 */
void ShootMainFrame::_HideShootView( void )
{
  m_pShootView->Hide();
}

/**
 * @brief   カメラ操作の表示
 */
void ShootMainFrame::_ShowCameraView( void )
{
  m_pCameraView->Show();
}

/**
 * @brief   カメラ操作の非表示
 */
void ShootMainFrame::_HideCameraView( void )
{
  m_pCameraView->Hide();
}

/**
 * @brief   Zモーション選択の表示
 */
void ShootMainFrame::_ShowZMotionView( void )
{
  m_pZMotionView->Show();
}

/**
 * @brief   Zモーション選択の非表示
 */
void ShootMainFrame::_HideZMotionView( void )
{
  m_pZMotionView->Hide();
}

/**
 * @brief   終了確認の表示
 */
void ShootMainFrame::_ShowFinishConfirmView( void )
{
  _HideShootView();
  _HideShootCommonView();

  // １行メッセージの表示
  m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_FINISH_MAIN );
  m_pPrepareCommonView->Show();
  
  // ボタン表示
  m_pSelectButtonView->SetSelectButtonType( SelectButtonView::BTN_TYPE_FINISH_MAIN );
  m_pSelectButtonView->Show();
}

/**
 * @brief   終了確認の非表示
 */
void ShootMainFrame::_HideFinishConfirmView( void )
{
  m_pPrepareCommonView->Hide();
  m_pSelectButtonView->Hide();
}

/**
 * @brief   撮影共通Viewの表示
 */
void ShootMainFrame::_ShowShootCommonView( void )
{
  // タブタイプの設定：デフォルトはフル表示のタイプ
  ShootCommonView::TabType type = ShootCommonView::TAB_TYPE_FULL;

  // お手軽撮影時かどうか
  if( _IsDifficultyEasy() )
  {
    // お手軽撮影
    type = ShootCommonView::TAB_TYPE_CAMERA_ONLY;
  }
  // ポケモンのみの撮影かどうか
  else if( _IsCaptureModePokeOnly() )
  {
    // ポケモンのみの撮影
    type = ShootCommonView::TAB_TYPE_POKE_ONLY;
  }

  // タブのタイプを設定
  m_pShootCommonView->SetTabType( type );
  // 表示
  m_pShootCommonView->Show();
}

/**
 * @brief   撮影共通Viewの非表示
 */
void ShootMainFrame::_HideShootCommonView( void )
{
  m_pShootCommonView->Hide();
}

/**
 * @brief   位置調整Viewの表示
 */
void ShootMainFrame::_ShowLayoutView( void )
{
  m_pLayoutView->Show();
}

/**
 * @brief   位置調整Viewの非表示
 */
void ShootMainFrame::_HideLayoutView( void )
{
  m_pLayoutView->Hide();
}

/**
 * @brief   背景Viewの表示
 */
void ShootMainFrame::_ShowBGView( void )
{
  m_pBGView->Show();
}

/**
 * @brief   背景Viewの非表示
 */
void ShootMainFrame::_HideBGView( void )
{
  m_pBGView->Hide();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief     設定・取得：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ポケモンの自動モーション再生フラグの設定
 *
 * @param[in]   enable    再生有効フラグ
 */
void ShootMainFrame::_SetPokemonMotionAutoPlayEnable( bool enable )
{
  // ViewerManagerの取得
  ViewerManager* viewer = _GetViewerManager();
  // 自動モーション再生フラグの設定
  viewer->RequestSetPokemonMotionAutoPlayEnable( enable );
}

/**
 * @brief   写真テクスチャの取得
 *
 * @param[in]   index   写真テクスチャ配列インデックス
 *
 * @retval  写真テクスチャ
 */
void* ShootMainFrame::_GetPhotoTexture( u32 index )
{
  // 共通データ構造体の取得
  Work* work = _GetWork();
  // 写真テクスチャ
  return work->viewer.photo_tex[index];
}

/**
 * @brief   共通データ構造体ポインタの取得
 *
 * @retval  共通データ構造体ポインタ
 */
Work* ShootMainFrame::_GetWork( void )
{
  GFL_ASSERT( m_pMainWork );
  
  Work* work = m_pMainWork->work;
  GFL_ASSERT( work );
  
  return work;
}

/**
 * @brief   ViewerManagerクラスポインタの取得
 *
 * @retval  ViewerManagerクラスポインタ
 */
ViewerManager* ShootMainFrame::_GetViewerManager( void )
{
  GFL_ASSERT( m_pMainWork );
  
  ViewerManager* viewer = m_pMainWork->viewer;
  GFL_ASSERT( viewer );
  
  return viewer;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   撮影処理が終了したか
 *
 * @retval  "true"なら終了
 * @retval  "false"ならそれ以外
 */
bool ShootMainFrame::_IsFinishCapture( void )
{
  // ViewerManagerの取得
  ViewerManager* viewer = _GetViewerManager();
  // 撮影処理の終了確認
  return viewer->IsFinishCapture();
}

/**
 * @brief   お手軽撮影かどうか
 *
 * @retval  "true"ならお手軽撮影
 * @retval  "false"なら本格撮影
 */
bool ShootMainFrame::_IsDifficultyEasy( void )
{
  // 共通データ構造体の取得
  Work* work = _GetWork();
  // 撮影難易度が「お手軽撮影」かどうか
  return work->app.diff_mode == Static::DIFF_MODE_EASY;
}

/**
 * @brief   撮影モードがポケモンのみかどうか
 *
 * @retval  "true"ならポケモンのみ
 * @retval  "false"ならツーショット
 */
bool ShootMainFrame::_IsCaptureModePokeOnly( void )
{
  // 共通データ構造体の取得
  Work* work = _GetWork();
  // 撮影モードが「ポケモンのみ」かどうか
  return work->prepare.capture_mode == Viewer::CAPTURE_MODE_POKE;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
