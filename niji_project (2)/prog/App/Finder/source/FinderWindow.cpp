//======================================================================
/**
 * @file    FinderWindow.cpp
 * @date    2015/12/07 12:07:53
 * @author  fukushima_yuya
 * @brief   ポケファインダーカメラアプリ：YESNOウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderWindow.h"
#include "FinderDrawerBase.h"
#include "FinderDrawListener.h"

#include <App/Finder/include/FinderAppParam.h>
#include <AppLib/include/Tool/app_tool_MenuWindow.h>

// arc
#include <niji_conv_header/app/finder/finder_pane.h>
#include <niji_conv_header/app/finder/finder_anim_list.h>
#include <niji_conv_header/message/msg_pokefinder_camera.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Finder)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum MainSeq
  {
    SEQ_IDLE,
    SEQ_OPEN,
    SEQ_MSG,
    SEQ_CONF,
    SEQ_CLOSE,
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderConfirmWindow::FinderConfirmWindow( DrawerBase* base, APP_PARAM* appParam, app::util::AppRenderingManager* renderMan, FinerConfirmWindowListenr* listener )
  : m_pBase(base)
  , m_pAppRenderingManager(renderMan)
  , m_pMenuWindow(NULL)
  , m_pListener(listener)
  , m_pAppParam(appParam)
  , m_InitSeq(0)
  , m_EndSeq(0)
  , m_MainSeq(SEQ_IDLE)
  , m_WindowKind(WND_KIND_NUM)
  , m_pMsgLine_1(NULL)
  , m_pMsgLine_2(NULL)
  , m_pMsgCursor(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
bool FinderConfirmWindow::InitFunc( void )
{
  enum {
    INIT_SEQ_TEXT,
    INIT_SEQ_WND,
    INIT_SEQ_WND_READY,
    INIT_SEQ_END,
  };

  switch( m_InitSeq )
  {
  case INIT_SEQ_TEXT:
    {
      // テキストの初期化
      InitializeText();

      m_InitSeq++;
    } // no break;

  case INIT_SEQ_WND:
    {
      // メッセージウィンドウの生成
      CreateMessageWindow();
      // メニューウィンドウの生成
      CreateMenuWindow();

      m_InitSeq++;
    } // no break;

  case INIT_SEQ_WND_READY:
    {
      // メニューウィンドウの生成が完了したか？
      if( m_pMenuWindow->IsCreate() )
      {
        m_InitSeq++;
      }
    } // no break;

  case INIT_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   テキストの初期化
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::InitializeText( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウの生成
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::CreateMessageWindow( void )
{
  // ペインの準備
  gfl2::lyt::LytParts* wndParts = m_pBase->GetParts( PANENAME_FINDER_MIN_LOW_000_PANE_MENU );
  {
    gfl2::lyt::LytParts* msgParts = m_pBase->GetParts( PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01, wndParts );
    {
      m_pMsgLine_1 = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A, msgParts );
      m_pMsgLine_2 = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B, msgParts );

      gfl2::lyt::LytPane* nullPane = m_pBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1, msgParts );
      {
        m_pBase->SetVisiblePane( nullPane, true );
      }

      gfl2::lyt::LytTextBox* textbox = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, msgParts );
      {
        m_pBase->SetVisiblePane( textbox, false );
      }

      m_pMsgCursor = m_pBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00, msgParts );
    }
  }

  app::util::G2DUtil* g2d = m_pBase->GetG2D();
  {
    // メッセージウィンドウの生成
    g2d->CreateMessageWindow(
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
      0, m_pMsgLine_1, m_pMsgLine_2, m_pBase->GetHeap() );

    // カーソルの設定
    g2d->CreateMsgCursor( m_pBase->GetHeap(), m_pMsgCursor );
  }

  // カーソルアニメの再生
  m_pBase->StartAnim( LA_FINDER_FINDER_MIN_LOW_000__MENU__PARTSMSG_01__CURSOR_00_KEEP );
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューウィンドウの生成
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::CreateMenuWindow( void )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();
  gfl2::str::MsgData* msgData   = m_pBase->GetMsgData();

  // メニューウィンドウの生成
  m_pMenuWindow = GFL_NEW(devHeap) app::tool::MenuWindow( m_pBase->GetHeap(), m_pAppRenderingManager, NULL, app::util::G2DUtil::SETUP_LOWER );
  {
    GFL_ASSERT( m_pMenuWindow );

    // リスナーのセット
    m_pMenuWindow->SetListener( this );

    // 項目の追加：「はい」
    m_pMenuWindow->AddItem( msgData, msg_pokefinder_c_yes );
    // 項目の追加：「いいえ」
    m_pMenuWindow->AddItem( msgData, msg_pokefinder_c_no, true );

    // サブビューへの追加
    app::ui::UIView* view = m_pBase->GetUIView();
    view->AddSubView( m_pMenuWindow );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   終了
 */
//------------------------------------------------------------------------------
bool FinderConfirmWindow::EndFunc( void )
{
  enum {
    END_SEQ_CHECK,
    END_SEQ_DELETE,
    END_SEQ_END,
  };

  switch( m_EndSeq )
  {
  case END_SEQ_CHECK:
    {
      if( m_pMenuWindow )
      {
        if( !m_pMenuWindow->IsModuleFree() ) break;
      }

      m_EndSeq++;
    } // no break;

  case END_SEQ_DELETE:
    {
      if( m_pMenuWindow )   // @fix cov_ctr[10127]
      {
        m_pMenuWindow->RemoveFromSuperView();
      }
      GFL_SAFE_DELETE( m_pMenuWindow );

      app::util::G2DUtil* g2d = m_pBase->GetG2D();
      {
        g2d->DeleteMessageWindow();
      }

      m_EndSeq++;
    } // no break;

  case END_SEQ_END:
    {
      return true;
    }
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::UpdateFunc( void )
{
  gfl2::str::StrWin::Result result = m_pBase->GetG2D()->PrintMessage();

  switch( m_MainSeq )
  {
  case SEQ_OPEN:
    {
      if( m_pBase->IsAnimEnd( LA_FINDER_FINDER_MIN_LOW_000__MENU_IN ) )
      {
        m_MainSeq = SEQ_MSG;
      }
    } break;

  case SEQ_MSG:
    {
      if( result == gfl2::str::StrWin::RES_DONE )
      {
        // メニューウィンドウを開く
        OpenMenuWindow();

        m_MainSeq = SEQ_CONF;
      }
    } break;

  case SEQ_CLOSE:
    {
      if( m_pBase->IsAnimEnd( LA_FINDER_FINDER_MIN_LOW_000__MENU_OUT ) )
      {
        m_MainSeq = SEQ_IDLE;
      }
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 *
 * @param   kind    ウィンドウの種類
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::Open( WndKind kind )
{
  if( m_MainSeq == SEQ_IDLE )
  {
    m_WindowKind = kind;

    // メッセージのセット
    SetMessage();

    // 入力切替
    SetEnableInput( false );

    // メニューウィンドウを開く
    OpenMessageWindow();

    m_MainSeq = SEQ_OPEN;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::Close( void )
{
  if( m_MainSeq == SEQ_CONF )
  {
    // メニューウィンドウを閉じる
    CloseMessageWindow();

    // 入力切替
    SetEnableInput( true );

    m_MainSeq = SEQ_CLOSE;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウを開く
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::OpenMessageWindow( void )
{
  // ウィンドウを開く
  m_pBase->StartAnim( LA_FINDER_FINDER_MIN_LOW_000__MENU_IN );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウを開く
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::CloseMessageWindow( void )
{
  GFL_ASSERT( m_pMenuWindow );

  // ウィンドウを閉じる
  m_pBase->StartAnim( LA_FINDER_FINDER_MIN_LOW_000__MENU_OUT );

  // メニューウィンドウを閉じる
  CloseMenuWindow();
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューウィンドウを開く
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::OpenMenuWindow( void )
{
  GFL_ASSERT( m_pMenuWindow );

  // メニューウィンドウを開く
  m_pMenuWindow->StartYesNoMenu( 1 );

  // 入力を無効に
  m_pMenuWindow->SetInputEnabled( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューウィンドウを閉じる
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::CloseMenuWindow( void )
{
  GFL_ASSERT( m_pMenuWindow );

  // メニューウィンドウを閉じる
  m_pMenuWindow->EndMenu( false );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージのセット
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::SetMessage( void )
{
  switch( m_WindowKind )
  {
  case WND_KIND_UPLOAD:
    {
      // 終了確認メッセージ：写真選択画面への遷移
      _SetMessage( msg_pokefinder_05_02 );
    } break;

  case WND_KIND_CLOSE:
    {
      if( m_pAppParam->shutter_count == INIT_SHUTTER_COUNT )
      {
        // 終了確認メッセージ：撮った写真０枚
        _SetMessage( msg_pokefinder_05_04 );
      }
      else {
        // 終了確認メッセージ：撮った写真１枚以上
        _SetMessage( msg_pokefinder_05_05 );
      }
    } break;
  }
}

void FinderConfirmWindow::_SetMessage( u32 msgID )
{
  app::util::G2DUtil* g2d = m_pBase->GetG2D();
  {
    gfl2::str::StrBuf* strBuf = g2d->GetTempStrBuf( 1 );
    {
      g2d->GetString( strBuf, msgID );
      g2d->SetMessageFinishMode( gfl2::str::StrWin::FINISH_NONE, 0 );
      g2d->SetMessage( strBuf );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   親VIEWの入力切り替え
 *
 * @param   enable    入力フラグ(true = 有効、false = 無効)
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::SetEnableInput( bool enable )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  // 入力切替
  view->SetInputEnabled( enable );
}





//------------------------------------------------------------------------------
/**
 * @brief   項目選択時のリスナー
 *
 * @param   id    メニュー項目ID
 */
//------------------------------------------------------------------------------
void FinderConfirmWindow::SelectMenu( u32 id )
{
  if( m_pListener )
  {
    // 入力を無効に
    m_pMenuWindow->SetInputEnabled( false );

    m_pListener->SelectMenu( m_WindowKind, id );
  }
}

GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(App)
