//======================================================================
/**
 * @file    FinderStudioDecorationMainFrame.cpp
 * @date    2017/01/31 15:43:21
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：編集(デコ)メインフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationMainFrame.h"
#include "FinderStudioDecorationFrameDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioAlbumWork.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>
#include <App/FinderStudioDecoration/source/FinderStudioDecorationDecoItemManager.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationViewDefine.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationUpperView.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationLowerMainView.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationLowerFrameListView.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationLowerStampListView.h>

#include <AppLib/include/Util/AppRenderingManager.h>
#include <Savedata/include/Record.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
DecorationMainFrame::DecorationMainFrame( void )
  : m_pUpperView( NULL )
  , m_pLowerMainView( NULL )
  , m_pFrameListView( NULL )
  , m_pStampListView( NULL )
  , m_pImageDBUtil( NULL )
  , m_MsgWndID( MSG_WND_ID_NULL )
  , m_FrameWait( 0 )
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
applib::frame::Result DecorationMainFrame::InitFunc( void )
{
  GFL_PRINT( "DecorationMainFrame Start\n" );

  bool( DecorationMainFrame::*INIT_FUNC_TABLE[] )( void ) = {
    &DecorationMainFrame::_InitFunc_View,
    &DecorationMainFrame::_InitFunc_Photo,
    &DecorationMainFrame::_InitFunc_Show,
  };

  if( ( this->*INIT_FUNC_TABLE[m_MainSeq] )( ) )
  {
    if( !_IsPhotoMax() )
    {
      this->SetMainSeq( SEQ_MAIN );
    }
    else {
      this->SetMainSeq( SEQ_CAUTION );
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   初期化：View
 */
bool DecorationMainFrame::_InitFunc_View( void )
{
  /// 上画面View
  m_pUpperView = reinterpret_cast<UpperView*>( m_pViewList->GetView( VIEW_ID_UPP_MAIN ) );
  /// 下メイン画面View
  m_pLowerMainView = reinterpret_cast<LowerMainView*>( m_pViewList->GetView( VIEW_ID_LOW_MAIN ) );
  /// フレームリストView
  m_pFrameListView = reinterpret_cast<FrameListView*>( m_pViewList->GetView( VIEW_ID_LOW_FRAME_LIST ) );
  /// スタンプリストView
  m_pStampListView = reinterpret_cast<StampListView*>( m_pViewList->GetView( VIEW_ID_LOW_STAMP_LIST ) );
  /// メッセージメニューView
  m_pMsgMenuView = m_pViewList->GetMessageMenuView();

  this->SetMainSeq( INIT_SEQ_PHOTO );

  return false;
}

/**
 * @brief   初期化：写真データ
 */
bool DecorationMainFrame::_InitFunc_Photo( void )
{
  if( m_SubSeq == 0 )
  {
    void* photoBuffer = m_pLowerMainView->GetPhotoBuffer();
    wchar_t* filepath = reinterpret_cast<wchar_t*>( m_pWork->photo_data.management_data.filepath );

    gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();
    m_pImageDBUtil = GFL_NEW_LOW( heap ) Static::ImageDBUtil();
    m_pImageDBUtil->StartLoadImage( heap->GetLowerHandle(), filepath, photoBuffer, false );

    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    if( m_pImageDBUtil->IsFinishLoadImage() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 2 )
  {
    m_pLowerMainView->ShowPhotoData();
    m_SubSeq++;
  }
  else if( m_SubSeq == 3 )
  {
    m_SubSeq++;
  }
  else if( m_SubSeq == 4 )
  {
    this->SetMainSeq( INIT_SEQ_SHOW );
  }

  return false;
}

/**
 * @brief   初期化：View表示
 */
bool DecorationMainFrame::_InitFunc_Show( void )
{
  /// 表示設定
  m_pUpperView->HideBackground();
  m_pUpperView->Show();

  return true;
}

/**
 * @brief   終了
 */
applib::frame::Result DecorationMainFrame::EndFunc( void )
{
  GFL_PRINT( "DecorationMainFrame Finish\n" );

  /// 非表示
  m_pUpperView->ShowBackground();
  m_pLowerMainView->Hide();
  m_pFrameListView->Hide();
  m_pStampListView->Hide();

  GFL_DELETE( m_pImageDBUtil );

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
applib::frame::Result DecorationMainFrame::UpdateFunc( void )
{
  Result( DecorationMainFrame::*UPDATE_FUNC_TABLE[] )( void ) = {
    &DecorationMainFrame::_UpdateCaution,
    &DecorationMainFrame::_UpdateMain,
    &DecorationMainFrame::_UpdateStampList,
    &DecorationMainFrame::_UpdateStampList,
    &DecorationMainFrame::_UpdateStampList,
    &DecorationMainFrame::_UpdateStampList,
    &DecorationMainFrame::_UpdateFrameList,
    &DecorationMainFrame::_UpdateCapture,
  };

  // 各準備画面の更新
  if( ( this->*UPDATE_FUNC_TABLE[m_MainSeq] )( ) == RESULT_FINISH )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：注意喚起
 */
DecorationMainFrame::Result DecorationMainFrame::_UpdateCaution( void )
{
  if( m_SubSeq == 0 )
  {
    _ShowMsgWnd( MSG_WND_ID_CAUTION_PRE );
    m_SubSeq++;
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：メイン
 */
DecorationMainFrame::Result DecorationMainFrame::_UpdateMain( void )
{
  if( m_SubSeq == 0 )
  {
    m_pLowerMainView->Show();
    m_pLowerMainView->ShowTutorial();
    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    // リザルトの取得
    u32 result = _GetViewResult( m_pLowerMainView );

    switch( result )
    {
      case LowerMainView::RES_STAMP_OVER_OPEN:
      case LowerMainView::RES_STAMP_OVER_CLOSE:
      case LowerMainView::RES_FIN_MENU_OPEN:
      case LowerMainView::RES_FIN_MENU_CLOSE:
      case LowerMainView::RES_FIN_MENU_OVERWRITE:
      case LowerMainView::RES_FIN_MENU_OVERWRITE_YES:
      case LowerMainView::RES_FIN_MENU_OVERWRITE_NO:
      case LowerMainView::RES_FIN_MENU_NEW_WRITE:
      case LowerMainView::RES_FIN_MENU_NEW_WRITE_CAUTION:
      case LowerMainView::RES_FIN_MENU_NOT_WRITE:
      case LowerMainView::RES_FIN_MENU_NOT_WRITE_YES:
      case LowerMainView::RES_FIN_MENU_NOT_WRITE_NO:
      case LowerMainView::RES_FIN_MENU_QUIT:
      {
        return _SetResult_MsgWnd( result );
      } // no break;

      case LowerMainView::RES_LIST_STAMP_S:
      case LowerMainView::RES_LIST_STAMP_M:
      case LowerMainView::RES_LIST_STAMP_L:
      case LowerMainView::RES_LIST_TEXT:
      case LowerMainView::RES_LIST_FRAME:
      {
        return _SetResult_StampList( result );
      } // no break;
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：フレームリスト
 */
DecorationMainFrame::Result DecorationMainFrame::_UpdateFrameList( void )
{
  enum {
    FRAME_LIST_SHOW,
    FRAME_LIST_MAIN,
  };

  if( m_SubSeq == FRAME_LIST_SHOW )
  {
    m_pFrameListView->Show();
    m_SubSeq = FRAME_LIST_MAIN;
  }
  if( m_SubSeq == FRAME_LIST_MAIN )
  {
    // リザルトの取得
    u32 result = _GetViewResult( m_pFrameListView );

    if( result == FrameListView::RES_CLOSE )
    {
      m_pFrameListView->Hide();
      this->SetMainSeq( SEQ_MAIN );
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：スタンプリスト
 */
DecorationMainFrame::Result DecorationMainFrame::_UpdateStampList( void )
{
  enum {
    STAMP_LIST_SHOW,
    STAMP_LIST_MAIN,
  };

  if( m_SubSeq == STAMP_LIST_SHOW )
  {
    m_pStampListView->Show();
    m_SubSeq = STAMP_LIST_MAIN;
  }
  if( m_SubSeq == STAMP_LIST_MAIN )
  {
    // リザルトの取得
    u32 result = _GetViewResult( m_pStampListView );

    if( result == StampListView::RES_CLOSE )
    {
      m_pStampListView->Hide();
      this->SetMainSeq( SEQ_MAIN );
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：キャプチャ
 */
DecorationMainFrame::Result DecorationMainFrame::_UpdateCapture( void )
{
  enum {
    SEQ_CAPTURE_START,
    SEQ_CAPTURE_FINISH,
    SEQ_CAPTURE_END,
  };

  if( m_SubSeq == SEQ_CAPTURE_START )
  {
    app::util::AppRenderingManager* renderMan = m_pDecoWork->render_man;
    void* buffer = m_pDecoWork->photo_data;

    // ディスプレイバッファの取得
    renderMan->RequestTransferColorBuffer_Block( buffer );

    m_FrameWait = 0;
    this->SetSubSeq( SEQ_CAPTURE_FINISH );
  }
  if( m_SubSeq == SEQ_CAPTURE_FINISH )
  {
    m_FrameWait++;

    if( m_FrameWait >= CAPTURE_WAIT_FRAME )
    {
      this->SetSubSeq( SEQ_CAPTURE_END );
    }
  }
  if( m_SubSeq == SEQ_CAPTURE_END )
  {
    Savedata::IncRecord( Savedata::Record::RECID_MYALBUM_DECO_CNT );    //!< マイアルバムでデコった回数

    this->GetFrameManager()->SetResult( Frame::RES_NEXT );
    return RESULT_FINISH;
  }

  return RESULT_CONTINUE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    LowerMainViewリザルト
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   LowerMainViewリザルト：スタンプリスト
 */
DecorationMainFrame::Result DecorationMainFrame::_SetResult_StampList( u32 result )
{
  if( result == LowerMainView::RES_LIST_STAMP_S )
  {
    m_pStampListView->SetDecoType( DECO_TYPE_STAMP_S );
    this->SetMainSeq( SEQ_LIST_STAMP_S );
  }
  else if( result == LowerMainView::RES_LIST_STAMP_M )
  {
    m_pStampListView->SetDecoType( DECO_TYPE_STAMP_M );
    this->SetMainSeq( SEQ_LIST_STAMP_M );
  }
  else if( result == LowerMainView::RES_LIST_STAMP_L )
  {
    m_pStampListView->SetDecoType( DECO_TYPE_STAMP_L );
    this->SetMainSeq( SEQ_LIST_STAMP_L );
  }
  else if( result == LowerMainView::RES_LIST_TEXT )
  {
    m_pStampListView->SetDecoType( DECO_TYPE_TEXT );
    this->SetMainSeq( SEQ_LIST_TEXT );
  }
  else if( result == LowerMainView::RES_LIST_FRAME )
  {
    //m_pStampListView->SetDecoType( DECO_TYPE_FRAME );
    this->SetMainSeq( SEQ_LIST_FRAME );
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   LowerMainViewリザルト：メッセージウィンドウ
 */
DecorationMainFrame::Result DecorationMainFrame::_SetResult_MsgWnd( u32 result )
{
  switch( result )
  {
    /// 編集終了メニューを開く
    case LowerMainView::RES_FIN_MENU_OPEN:
    {
      if( _IsDecoration() )
      {
        _ShowMsgWnd( MSG_WND_ID_FIN_MENU_SAVE );
      }
      else {
        _ShowMsgWnd( MSG_WND_ID_FIN_MENU_QUIT );
      }
    } break;

    /// スタンプ配置オーバー
    case LowerMainView::RES_STAMP_OVER_OPEN:
    {
      _ShowMsgWnd( MSG_WND_ID_STAMP_OVER );
    } break;

    /// スタンプ配置オーバー
    case LowerMainView::RES_STAMP_OVER_CLOSE:
    {
      _HideMsgWnd();
    } break;

    /// 上書きを選択
    case LowerMainView::RES_FIN_MENU_OVERWRITE:
    {
      _HideMsgWnd();
      _ShowMsgWnd( MSG_WND_ID_OVERWRITE_CONF );
    } break;

    /// 上書き確認 YES
    case LowerMainView::RES_FIN_MENU_OVERWRITE_YES:
    {
      _HideMsgWnd();
      m_pDecoWork->save_type = SAVE_TYPE_OVER_WRITE;
      this->SetMainSeq( SEQ_CAPTURE );
    } break;

    /// 上書き確認 NO
    case LowerMainView::RES_FIN_MENU_OVERWRITE_NO:
    /// データ消去確認 NO
    case LowerMainView::RES_FIN_MENU_NOT_WRITE_NO:
    /// 新規保存 枚数オーバー
    case LowerMainView::RES_FIN_MENU_NEW_WRITE_CAUTION:
    {
      _HideMsgWnd();
      _ShowMsgWnd( MSG_WND_ID_FIN_MENU_SAVE );
    } break;

    /// 新規保存を選択
    case LowerMainView::RES_FIN_MENU_NEW_WRITE:
    {
      _HideMsgWnd();

      if( !_IsPhotoMax() )
      {
        m_pDecoWork->save_type = SAVE_TYPE_NEW_WRITE;
        this->SetMainSeq( SEQ_CAPTURE );
      }
      else {
        _ShowMsgWnd( MSG_WND_ID_CAUTION );
      }
    } break;

    /// 保存しない
    case LowerMainView::RES_FIN_MENU_NOT_WRITE:
    {
      _HideMsgWnd();
      _ShowMsgWnd( MSG_WND_ID_DELETE_CONF );
    } break;

    /// データ消去確認 YES
    case LowerMainView::RES_FIN_MENU_NOT_WRITE_YES:
    /// 終了
    case LowerMainView::RES_FIN_MENU_QUIT:
    {
      _HideMsgWnd();
      this->GetFrameManager()->SetResult( Frame::RES_FINISH );
      return RESULT_FINISH;
    } // no break;
  }

  return RESULT_CONTINUE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージウィンドウ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージウィンドウを開く
 */
void DecorationMainFrame::_ShowMsgWnd( MsgWndID id )
{
  void ( DecorationMainFrame::*OPEN_MSGWND_FUNC[] )( MsgWndID ) = {
    &DecorationMainFrame::_ShowMsgWnd_Caution,
    &DecorationMainFrame::_ShowMsgWnd_OverStamp,
    &DecorationMainFrame::_ShowMsgWnd_FinMenu,
    &DecorationMainFrame::_ShowMsgWnd_FinMenu,
    &DecorationMainFrame::_ShowMsgWnd_Confirm,
    &DecorationMainFrame::_ShowMsgWnd_Caution,
    &DecorationMainFrame::_ShowMsgWnd_Confirm,
  };

  ( this->*OPEN_MSGWND_FUNC[id] )( id );
}

/**
 * @brief   メッセージウィンドウを開く：新規保存の出来ない注意喚起
 */
void DecorationMainFrame::_ShowMsgWnd_Caution( MsgWndID id )
{
  _SetMsgWndID( id );
  m_pMsgMenuView->GetView()->SetYNListMode( false );
  m_pMsgMenuView->ShowMessage_Id( fs_deco_win_16 );
}

/**
 * @brief   メッセージウィンドウを開く：スタンプオーバー
 */
void DecorationMainFrame::_ShowMsgWnd_OverStamp( MsgWndID id )
{
  _SetMsgWndID( id );
  m_pMsgMenuView->GetView()->SetYNListMode( false );
  m_pMsgMenuView->ShowMessage_Id( fs_deco_win_10 );
}

/**
 * @brief   メッセージウィンドウを開く：終了メニュー
 */
void DecorationMainFrame::_ShowMsgWnd_FinMenu( MsgWndID id )
{
  _SetMsgWndID( id );

  u32 msgID = 0;

  App::Tool::MessageMenuView* view = m_pMsgMenuView->GetView();
  Static::MessageMenuMsgManager* msgMan = m_pMsgMenuView->GetMsgManager();

  view->ClearMenu();

  if( _IsDecoration() )
  {
    msgID = fs_deco_win_14;

    view->AddItem( msgMan->GetMsgData(), fs_deco_cap_02, false, false, SEQ_SE_DECIDE1 );
    view->AddItem( msgMan->GetMsgData(), fs_deco_cap_01, false, false, SEQ_SE_DECIDE1 );
    view->AddItem( msgMan->GetMsgData(), fs_deco_cap_03, false, false, SEQ_SE_DECIDE1 );
    view->AddItem( msgMan->GetMsgData(), fs_deco_cap_04, true, true, SEQ_SE_CANCEL1 );
  }
  else {
    msgID = fs_deco_win_18;

    view->AddItem( msgMan->GetMsgData(), fs_deco_cap_09, false, false, SEQ_SE_DECIDE1 );
    view->AddItem( msgMan->GetMsgData(), fs_deco_cap_04, true, true, SEQ_SE_CANCEL1 );
  }

  gfl2::str::StrBuf* strBuf = m_pMsgMenuView->GetStrBuf();
  msgMan->GetMsgData()->GetString( msgID, *strBuf );

  view->SetYNListMode( false );
  // メッセージ＋メニューウィンドウの表示
  view->ShowMessageWithListMenu( strBuf, false, NULL );
}

/**
 * @brief   メッセージウィンドウを開く：YesNo確認
 */
void DecorationMainFrame::_ShowMsgWnd_Confirm( MsgWndID id )
{
  _SetMsgWndID( id );

  u32 msgID = ( id == MSG_WND_ID_OVERWRITE_CONF ) ? fs_deco_win_15 : fs_deco_win_17;

  App::Tool::MessageMenuView* view = m_pMsgMenuView->GetView();
  {
    view->ClearMenu();

    Static::MessageMenuMsgManager* msgMan = m_pMsgMenuView->GetMsgManager();

    view->SetYNListMode( true );
    view->ShowMessageWithListMenu(
      *msgMan->GetMsgData(),
      msgID,
      false,
      *msgMan->GetMsgData(),
      fs_deco_cap_05,
      fs_deco_cap_06 );
  }
}

/**
 * @brief   メッセージウィンドウを閉じる
 */
void DecorationMainFrame::_HideMsgWnd( void )
{
  m_pMsgMenuView->HideMessage();

  m_MsgWndID = MSG_WND_ID_NULL;
  m_pLowerMainView->SetMsgWndID( m_MsgWndID );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定・取得関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージウィンドウIDのセット
 */
void DecorationMainFrame::_SetMsgWndID( MsgWndID id )
{
  switch( id )
  {
    case MSG_WND_ID_CAUTION_PRE:
    {
      m_pMsgMenuView->SetListener( this );
      m_MsgWndID = id;
    } break;

    default:
    {
      m_pMsgMenuView->SetListener( m_pLowerMainView );
      m_pLowerMainView->SetMsgWndID( id );
    }
  }
}

/**
 * @brief   Viewリザルト
 */
u32 DecorationMainFrame::_GetViewResult( Static::ViewBase* view )
{
  u32 result = view->GetResult();
  view->SetResult( VIEW_RES_CONTINUE );

  return result;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   写真の保存枚数がいっぱいかどうか
 *
 * @retval  "true"ならいっぱい
 */
bool DecorationMainFrame::_IsPhotoMax( void ) const
{
  return (m_pWork->photo_count == Static::SAVE_PHOTO_MAX);
}

/**
 * @brief   デコレーションがされているかどうか
 *
 * @retval  "true"ならデコレーション済み
 * @retval  "false"ならデコレーションされていない
 */
bool DecorationMainFrame::_IsDecoration( void ) const
{
  return m_pDecoWork->deco_man->IsDecoration();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   Static::MessageMenuViewListener：イベント取得
 */
void DecorationMainFrame::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  if( event == Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
  {
    if( m_MsgWndID == MSG_WND_ID_CAUTION_PRE )
    {
      this->SetMainSeq( SEQ_MAIN );
    }

    _HideMsgWnd();
  }
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
