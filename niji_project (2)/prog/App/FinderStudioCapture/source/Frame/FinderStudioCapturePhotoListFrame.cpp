//======================================================================
/**
 * @file    FinderStudioCapturePhotoListFrame.cpp
 * @date    2016/11/09 20:40:10
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：写真リストフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCapturePhotoListFrame.h"

#include <App/FinderStudioStatic/include/FinderStudioViewList.h>

#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureUpperView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureBGView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCapturePhotoListView.h>

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
PhotoListFrame::PhotoListFrame( void )
  : m_pUpperView( NULL )
  , m_pBGView( NULL )
  , m_pPhotoListView( NULL )
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
applib::frame::Result PhotoListFrame::InitFunc( void )
{
  GFL_PRINT("PhotoListFrame Start\n");

  Static::ViewList* viewList = m_pMainWork->viewList;

  /// 写真リストView
  m_pPhotoListView = reinterpret_cast<PhotoListView*>( viewList->GetView( VIEW_ID_PHOTO_LIST ) );
  /// 上画面View
  m_pUpperView = reinterpret_cast<UpperView*>( viewList->GetView( VIEW_ID_UPPER ) );
  /// BGView
  m_pBGView = reinterpret_cast<BGView*>( viewList->GetView( VIEW_ID_BG ) );
  /// メッセージウィンドウ
  m_pMsgMenuView = viewList->GetMessageMenuView();
  m_pMsgMenuView->SetListener( m_pPhotoListView );

  // サムネイルの初期化
  m_pPhotoListView->InitThumbnail();
  // 保存可能枚数のセット
  m_pPhotoListView->SetSavePhotoCount( m_pMainWork->work->app.save_photo_count );

  // Viewerの描画を停止
  m_pListener->HideViewer();

  return applib::frame::RES_FINISH;
}

/**
 * @brief   終了
 */
applib::frame::Result PhotoListFrame::EndFunc( void )
{
  GFL_PRINT( "PhotoListFrame Finish\n" );

  m_pUpperView->InvisiblePhotoImage();
  m_pMsgMenuView->RemoveListener();

  // サムネイルの破棄
  m_pPhotoListView->TermThumbnail();  // <=これ重要

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
applib::frame::Result PhotoListFrame::UpdateFunc( void )
{
#if PM_DEBUG
  if( DebugClass::IsAnyDebugMode() )
  {
    return applib::frame::RES_FINISH;
  }
#endif

  Result( PhotoListFrame::*UPDATE_FUNC_TABLE[] )( void ) = {
    &PhotoListFrame::_UpdatePhotoListInit,
    &PhotoListFrame::_UpdatePhotoListMain,
    &PhotoListFrame::_UpdateCurtainClose,
    &PhotoListFrame::_UpdateErrorMessage,
  };

  // 各準備画面の更新
  if( ( this->*UPDATE_FUNC_TABLE[m_MainSeq] )( ) == RESULT_FINISH )
  {
    // 背景を表示
    m_pBGView->Show();
    // 写真リストを非表示
    m_pPhotoListView->Hide();
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：写真リスト初期化
 */
PhotoListFrame::Result PhotoListFrame::_UpdatePhotoListInit( void )
{
  enum {
    PHOTOLIST_INIT_SEQ_READY,
    PHOTOLIST_INIT_SEQ_SHOW,
    PHOTOLIST_INIT_SEQ_END,
    PHOTOLIST_INIT_SEQ_HIDE_BG,
  };

  if( m_SubSeq == PHOTOLIST_INIT_SEQ_READY )
  {
    if( m_pPhotoListView->IsReadyThumbnail() )
    {
      this->SetSubSeq( PHOTOLIST_INIT_SEQ_SHOW );
    }
  }
  if( m_SubSeq == PHOTOLIST_INIT_SEQ_SHOW )
  {
    // 写真リストを表示
    m_pPhotoListView->Show();

    this->SetSubSeq( PHOTOLIST_INIT_SEQ_END );
  }
  else if( m_SubSeq == PHOTOLIST_INIT_SEQ_END )
  {
    this->SetSubSeq( PHOTOLIST_INIT_SEQ_HIDE_BG );
  }
  else if( m_SubSeq == PHOTOLIST_INIT_SEQ_HIDE_BG )
  {
    // 背景を非表示
    m_pBGView->Hide();
    this->SetMainSeq( SEQ_PHOTOLIST_MAIN );
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：写真リストメイン
 */
PhotoListFrame::Result PhotoListFrame::_UpdatePhotoListMain( void )
{
  u32 result = m_pPhotoListView->GetResult();
  m_pPhotoListView->SetResult( PhotoListView::RES_CONTINUE );

#if PM_DEBUG
  if( result != PhotoListView::RES_CONTINUE )
  {
    FUKUSHIMA_PRINT( "_UpdatePhotoList PhotoListView : result=[%d]\n", result );
  }
#endif

  if( result == PhotoListView::RES_CONFIRM )
  {
    m_pPhotoListView->SetMsgWndType( PhotoListView::MSGWND_TYPE_CONFIRM );
    m_pMsgMenuView->ShowMessageWithListMenu( Static::MessageMenuView::MENU_TYPE_YESNO_CONFIRM, fs_photo_win_30 );
  }
  else if( result == PhotoListView::RES_DECIDE )
  {
    m_pMsgMenuView->HideMessage();
    this->SetMainSeq( SEQ_CURTAIN_CLOSE );
    return RESULT_CHANGE_SEQ;
  }
  else if( result == PhotoListView::RES_CANCEL )
  {
    m_pMsgMenuView->HideMessage();
    m_pPhotoListView->SetInputEnabled( true );
  }
  else if( result == PhotoListView::RES_OVER_NUM )
  {
    this->SetMainSeq( SEQ_ERROR_MESSAGE );
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：カーテン演出
 */
PhotoListFrame::Result PhotoListFrame::_UpdateCurtainClose( void )
{
  enum {
    CURTAIN_SEQ_CLOSE,
    CURTAIN_SEQ_CLOSE_WAIT,
    CURTAIN_SEQ_END,
  };

  /// カーテンを閉じる
  if( m_SubSeq == CURTAIN_SEQ_CLOSE )
  {
    m_pUpperView->CurtainClose( false );
    this->SetSubSeq( CURTAIN_SEQ_CLOSE_WAIT );
  }
  /// カーテンを閉じきるまで待機
  if( m_SubSeq == CURTAIN_SEQ_CLOSE_WAIT )
  {
    if( m_pUpperView->IsCurtainClose() )
    {
      this->SetSubSeq( CURTAIN_SEQ_END );
    }
  }
  /// 写真保存シーケンスへ
  if( m_SubSeq == CURTAIN_SEQ_END )
  {
    this->GetFrameManager()->SetResult( Frame::RES_NEXT );
    return RESULT_FINISH;
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：エラーメッセージ
 */
PhotoListFrame::Result PhotoListFrame::_UpdateErrorMessage( void )
{
  enum {
    MSG_SEQ_HIDE,
    MSG_SEQ_SHOW,
    MSG_SEQ_MAIN,
  };

  if( m_SubSeq == MSG_SEQ_HIDE )
  {
    m_pMsgMenuView->HideMessage();
    this->SetSubSeq( MSG_SEQ_SHOW );
  }
  else if( m_SubSeq == MSG_SEQ_SHOW )
  {
    Static::MessageMenuMsgManager* msgMan = m_pMsgMenuView->GetMsgManager();
    gfl2::str::StrBuf* strBuf = m_pMsgMenuView->GetStrBuf();

    msgMan->SetRegisterNumber( 0, m_pMainWork->work->app.save_photo_count, 1 );
    msgMan->GetStringExpand( strBuf, fs_photo_win_35 );

    m_pMsgMenuView->ShowMessage_Str( strBuf );
    m_pPhotoListView->SetMsgWndType( PhotoListView::MSGWND_TYPE_OVER_NUM );

    this->SetSubSeq( MSG_SEQ_MAIN );
  }
  else if( m_SubSeq == MSG_SEQ_MAIN )
  {
    u32 result = m_pPhotoListView->GetResult();
    m_pPhotoListView->SetResult( PhotoListView::RES_CONTINUE );

#if PM_DEBUG
    if( result != PhotoListView::RES_CONTINUE )
    {
      FUKUSHIMA_PRINT( "_UpdateErrorMessage PhotoListView : result=[%d]\n", result );
    }
#endif

    if( result == PhotoListView::RES_CANCEL )
    {
      m_pMsgMenuView->HideMessage();

      /// 入力を戻す
      m_pPhotoListView->SetInputEnabled( true );

      this->SetMainSeq( SEQ_PHOTOLIST_MAIN );
    }
  }

  return RESULT_CONTINUE;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
