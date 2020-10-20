//======================================================================
/**
 * @file    FinderStudioMessageMenuView.cpp
 * @date    2016/11/30 15:23:05
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：メッセージメニューView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>

#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_Heap.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

#include <niji_conv_header/message/msg_fs_photo.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/MenuWindow.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )


//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
MessageMenuView::MessageMenuView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, gfl2::str::MsgData* msgData, void* cursorBuf )
  : m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pMsgMenuView( NULL )
  , m_pMsgData( msgData )
  , m_pStrBuf( NULL )
  , m_pListener( NULL )
  , m_pMsgManager( NULL )
  , m_pLytDatBuff( NULL )
  , m_pCursorBuff( cursorBuf )
  , m_MainSeq( 0 )
  , m_LoadSeq( 0 )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  m_pStrBuf = GFL_NEW( devHeap ) gfl2::str::StrBuf( 1024, devHeap );
}

/**
 * @brief   デストラクタ
 */
MessageMenuView::~MessageMenuView( void )
{
  GFL_SAFE_DELETE( m_pStrBuf );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
bool MessageMenuView::InitFunc( void )
{
  enum {
    INIT_SEQ_LOAD_GARC,
    INIT_SEQ_CREATE_VIEW,
    INIT_SEQ_SETUP,
    INIT_SEQ_END,
  };

  if( m_MainSeq == INIT_SEQ_LOAD_GARC )
  {
    if( _LoadLayout() )
    {
      m_MainSeq++;
    }
  }
  if( m_MainSeq == INIT_SEQ_CREATE_VIEW )
  {
    _CreateMsgManager();
    _CreateMessageMenuView();
    m_MainSeq++;
  }
  if( m_MainSeq == INIT_SEQ_SETUP )
  {
    if( _IsSetup() )
    {
      m_MainSeq++;
    }
  }
  if( m_MainSeq == INIT_SEQ_END )
  {
    m_MainSeq = 0;
    return true;
  }

  return false;
}

/**
 * @brief   終了処理
 */
bool MessageMenuView::EndFunc( void )
{
  enum {
    END_SEQ_DELETE_VIEW,
    END_SEQ_DELETE_LAYOUT,
    END_SEQ_END,
  };

  if( m_MainSeq == END_SEQ_DELETE_VIEW )
  {
    _DeleteMessageMenuView();
    _DeleteMsgManager();
    m_MainSeq++;
  }
  if( m_MainSeq == END_SEQ_DELETE_LAYOUT )
  {
    _ReleaseLayout();
    m_MainSeq++;
  }
  if( m_MainSeq == END_SEQ_END )
  {
    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    生成・破棄：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   App::Tool::MessageMenuView 生成
 */
void MessageMenuView::_CreateMessageMenuView( void )
{
  App::Tool::MessageMenuView::PARAM param( m_pHeap, m_pAppRenderingManager );
  m_pMsgMenuView = GFL_NEW(m_pHeap->GetSystemHeap()) App::Tool::MessageMenuView( param, this );
  m_pMsgMenuView->Setup( m_pLytDatBuff, m_pCursorBuff );
}

/**
 * @brief   App::Tool::MessageMenuView 破棄
 */
void MessageMenuView::_DeleteMessageMenuView( void )
{
  GFL_SAFE_DELETE( m_pMsgMenuView );
}

/**
 * @brief   MessageMenuMsgManager 生成
 */
void MessageMenuView::_CreateMsgManager( void )
{
  m_pMsgManager = GFL_NEW( m_pHeap->GetSystemHeap() ) MessageMenuMsgManager( m_pHeap );
}

/**
 * @brief   MessageMenuMsgManager 破棄
 */
void MessageMenuView::_DeleteMsgManager( void )
{
  GFL_SAFE_DELETE( m_pMsgManager );
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   セットアップの完了チェック
 *
 * @retval  "true"なら完了
 * @retval  "false"ならそれ以外
 */
bool MessageMenuView::_IsSetup( void )
{
  if( !_IsCreateMessageMenuView() ) { return true; }

  return m_pMsgMenuView->IsSetup();
}

/**
 * @brief   App::Tool::MessageMenuView生成チェック
 *
 * @retval  "true"なら生成済み
 * @retval  "false"ならそれ以外
 */
bool MessageMenuView::_IsCreateMessageMenuView( void )
{
  if( m_pMsgMenuView == NULL )
  {
    GFL_PRINT( "Not Create MessageMenuView!!\n" );
    GFL_ASSERT( 0 );
    return false;
  }

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    レイアウト 読込み・開放：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   レイアウトデータ 読込み
 */
bool MessageMenuView::_LoadLayout( void )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetSystemHeap();
  u32 arcID = ARCID_MENU_WINDOW;

  if( m_LoadSeq == 0 )
  {
    app::util::FileAccessor::FileOpen( arcID, heap->GetLowerHandle() );
    m_LoadSeq++;
  }
  if( m_LoadSeq == 1 )
  {
    if( !app::util::FileAccessor::IsFileOpen( arcID ) ) return false;

    app::util::FileAccessor::FileLoad(
      arcID,
      GARC_MenuWindow_MenuWindow_applyt_COMP,
      &m_pLytDatBuff,
      heap->GetLowerHandle(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_LoadSeq++;
  }
  if( m_LoadSeq == 2 )
  {
    if( !app::util::FileAccessor::IsFileLoad( &m_pLytDatBuff ) ) return false;

    app::util::FileAccessor::FileClose( arcID, heap->GetLowerHandle() );
    m_LoadSeq++;
  }
  if( m_LoadSeq == 3 )
  {
    if( !app::util::FileAccessor::IsFileClose( arcID ) ) return false;

    m_LoadSeq++;
  }
  if( m_LoadSeq == 4 )
  {
    return true;
  }

  return false;
}

/**
 * @brief   レイアウトデータ 解放
 */
void MessageMenuView::_ReleaseLayout( void )
{
  GflHeapSafeFreeMemory( m_pLytDatBuff );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージ 表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージのみの表示：メッセージID指定版
 */
void MessageMenuView::ShowMessage_Id( u32 msgID, bool isImmediateMessage/* = false*/, bool isShowTimerIcon/* = false*/, bool isShowBG/* = true*/ )
{
  if( !_IsCreateMessageMenuView() ) { return; }

  m_pMsgMenuView->ShowMessage( *m_pMsgData, msgID, isImmediateMessage, isShowTimerIcon, isShowBG );
  m_pMsgMenuView->SetModal( true );
}

/**
 * @brief   メッセージのみの表示：文字列指定版
 */
void MessageMenuView::ShowMessage_Str( const gfl2::str::StrBuf* strBuf, bool isImmediateMessage/* = false*/, bool isShowTimerIcon/* = false*/, bool isShowBG/* = true*/ )
{
  if( !_IsCreateMessageMenuView() ) { return; }

  m_pMsgMenuView->ShowMessage( strBuf, isImmediateMessage, isShowTimerIcon, isShowBG );
  m_pMsgMenuView->SetModal( true );
}

/**
 * @brief   メッセージ＋メニューウィンドウの表示
 */
void MessageMenuView::ShowMessageWithListMenu( MenuType type, u32 msgID, bool isImmediateMessage/* = false*/ )
{
  if( !_IsCreateMessageMenuView() ) { return; }

  m_pMsgMenuView->ClearMenu();

  void( MessageMenuView::*SHOW_MSGWITHLISTMENU_FUNC[MENU_TYPE_NUM] )( u32, bool ) = {
    &MessageMenuView::_ShowMessageWithListMenu_YesNoConfirm,
  };

  ( this->*SHOW_MSGWITHLISTMENU_FUNC[type] )( msgID, isImmediateMessage );
}

/**
 * @brief   メッセージ＋メニューウィンドウの表示：YesNo確認メッセージ
 */
void MessageMenuView::_ShowMessageWithListMenu_YesNoConfirm( u32 msgID, bool isImmediateMessage )
{
  m_pMsgMenuView->SetYNListMode( true );
  m_pMsgMenuView->ShowMessageWithListMenu(
    *m_pMsgData,
    msgID,
    isImmediateMessage,
    *m_pMsgData,
    fs_photo_cap_15,
    fs_photo_cap_16 );
}

/**
 * @brief   ウィンドウの非表示
 */
void MessageMenuView::HideMessage( void )
{
  m_pMsgMenuView->SetYNListMode( false );
  m_pMsgMenuView->SetModal( false );
  m_pMsgMenuView->HideMessage();
}

/**
 * @brief   メッセージ＋メニューウィンドウの表示（項目指定版）
 */
void MessageMenuView::ShowMessageWithListMenu( u32 msgID, const u32 * item, u32 item_max, u32 cancel, bool cancel_mark, bool isImmediateMessage/* = false*/ )
{
  if( !_IsCreateMessageMenuView() ) { return; }

  m_pMsgMenuView->ClearMenu();

  for( u32 i=0; i<item_max; i++ )
  {
    // @fix GFMMCat[537]: 戻るマークを表示する場合、キャンセル位置のみ表示
    bool is_cancel = false;
    bool is_mark   = false;
    if( i == cancel )
    {
      is_cancel = true;
      is_mark   = cancel_mark;
    }
    m_pMsgMenuView->AddItem( m_pMsgData, item[i], is_cancel, is_mark, SMID_NULL );
  }

  gfl2::str::StrBuf * str = GFL_NEW_LOW(m_pHeap->GetSystemHeap()) gfl2::str::StrBuf( 256, m_pHeap->GetSystemHeap()->GetLowerHandle() ); // 文字サイズはMessageMenuViewと同じ
  m_pMsgData->GetString( msgID, *str );
  m_pMsgMenuView->ShowMessageWithListMenu( str, isImmediateMessage, NULL );
  GFL_DELETE str;
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージデータ セット
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージデータとワードセットの設定
 *
 * @param   msgData   メッセージデータ
 * @param   wordSet   ワードセット
 */
void MessageMenuView::SetMessageManagerMsgDataAndWordSet( gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pMsgManager->SetMessageData( msgData, wordSet );
}

/**
 * @brief   メッセージデータとワードセットの解除
 */
void MessageMenuView::RemoveMessageManagerMsgDataAndWordSet( void )
{
  m_pMsgManager->SetMessageData( NULL, NULL );
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    イベント
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   App::Tool::MessageMenuView::IEventHandler
 *
 * @note    各イベントごとにMessageMenuViewListenerの関数をコールする。
 *          アプリ側はMessageMenuViewListenerをセットする事でイベントを取得できる。
 */
void MessageMenuView::MessageMenuView_OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  FUKUSHIMA_PRINT("MessageMenuView_OnEvent\n");

  if( m_pListener )
  {
    m_pListener->OnEvent( event );

    switch( event )
    {
      case App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedItem:
      case App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes:
      case App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo:
      {
        m_pListener->OnSelectMenu( m_pMsgMenuView->GetSelectedItemIndex() );
      } break;
    }
  }
}

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
