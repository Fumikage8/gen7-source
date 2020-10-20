//======================================================================
/**
 * @file    FinderMenuWindow.cpp
 * @date    2015/11/06 11:27:10
 * @author  fukushima_yuya
 * @brief   ウィンドウ関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuWindow.h"

#include <App/FinderMenu/source/FinderMenuDrawerBase.h>
#include <AppLib/include/Tool/app_tool_MenuWindow.h>

// arc
#include <niji_conv_header/app/finder_menu/finder_menu_pane.h>
#include <niji_conv_header/app/finder_menu/finder_menu_anim_list.h>
#include <niji_conv_header/message/msg_pokefinder.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//==============================================================================
// @brief   メニューウィンドウ
//==============================================================================

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  MenuWindow::MenuWindow( DrawerBase* base, app::util::AppRenderingManager* renderMan, app::tool::MenuWindowListener* listener, void* cursorBuf )
    : m_pBase( base )
    , m_pMenuWindow(NULL)
    , m_isOpen( false )
  {
    gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();
    gfl2::str::MsgData*   msgData = m_pBase->GetMsgData();
    app::ui::UIView*      view    = m_pBase->GetUIView();

    // メニューウィンドウの生成
    m_pMenuWindow = GFL_NEW(devHeap) app::tool::MenuWindow( m_pBase->GetHeap(), renderMan, cursorBuf, app::util::G2DUtil::SETUP_LOWER );
    GFL_ASSERT( m_pMenuWindow );

    // リスナーの登録
    m_pMenuWindow->SetListener( listener );

    // アイテム登録
    m_pMenuWindow->AddItem( msgData, msg_pokefinder_yes );
    m_pMenuWindow->AddItem( msgData, msg_pokefinder_no, true );

    view->AddSubView( m_pMenuWindow );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  MenuWindow::~MenuWindow( void )
  {
    if( m_pMenuWindow )
    {
      m_pMenuWindow->RemoveFromSuperView();

      GFL_SAFE_DELETE( m_pMenuWindow );
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   準備完了か
   */
  //----------------------------------------------------------------------------
  bool MenuWindow::IsReady( void ) const
  {
    if( m_pMenuWindow )
    {
      return m_pMenuWindow->IsCreate();
    }

    GFL_ASSERT(0);
    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   破棄完了か
   */
  //----------------------------------------------------------------------------
  bool MenuWindow::IsDelete( void ) const
  {
    if( m_pMenuWindow )
    {
      return m_pMenuWindow->IsModuleFree();
    }

    GFL_ASSERT(0);
    return true;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //----------------------------------------------------------------------------
  void MenuWindow::Open( void )
  {
    GFL_ASSERT( IsReady() );    //!< 準備が完了済み
    GFL_ASSERT( !m_isOpen );

    // メニューウィンドウを開く
    m_pMenuWindow->StartYesNoMenu( 1 );

    m_isOpen = true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //----------------------------------------------------------------------------
  void MenuWindow::Close( void )
  {
    GFL_ASSERT( m_isOpen );

    // メニューウィンドウを閉じる
    m_pMenuWindow->EndMenu( false );

    m_isOpen = false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   開いているか
   */
  //----------------------------------------------------------------------------
  bool MenuWindow::IsOpened( void ) const
  {
    return m_isOpen;
  }



//==============================================================================
// @brief   メッセージウィンドウ
//==============================================================================

  //----------------------------------------------------------------------------
  // @brief   定数
  //----------------------------------------------------------------------------
  namespace
  {
    enum {
      SEQ_INIT,
      SEQ_IDLE,
      SEQ_OPEN,
      SEQ_MSG,
      SEQ_MAIN,
      SEQ_CLOSE,
      SEQ_TERM,
      SEQ_END,
    };

    enum WindowType
    {
      WND_TYPE_MSG_ONLY,
      WND_TYPE_CONFIRM,
      WND_TYPE_NUM,
    };

    enum AnimType
    {
      ANIM_TYPE_PHOTO_SELECT,
      ANIM_TYPE_CMD_SELECT,
      ANIM_TYPE_SCORE,
      ANIM_TYPE_NUM,
    };

    struct WindowData
    {
      u8    wndType;
      u32   msgID;
      u8    animType;
    } c_WindowData[] = {
      { WND_TYPE_CONFIRM,  msg_pokefinder_10_02, ANIM_TYPE_PHOTO_SELECT },            //!< 写真選択確認
      { WND_TYPE_CONFIRM,  msg_pokefinder_10_04, ANIM_TYPE_PHOTO_SELECT },            //!< やめる確認
      { WND_TYPE_MSG_ONLY, msg_pokefinder_10_03, ANIM_TYPE_PHOTO_SELECT },            //!< 写真決定
      
      { WND_TYPE_CONFIRM,  msg_pokefinder_10_10, ANIM_TYPE_CMD_SELECT },              //!< 保存確認
      { WND_TYPE_MSG_ONLY, msg_pokefinder_10_11, ANIM_TYPE_CMD_SELECT },              //!< 保存成功
      { WND_TYPE_MSG_ONLY, msg_pokefinder_15_01, ANIM_TYPE_CMD_SELECT },              //!< 保存失敗：容量不足
      { WND_TYPE_MSG_ONLY, msg_pokefinder_15_02, ANIM_TYPE_CMD_SELECT },              //!< 保存失敗：空き不足
      { WND_TYPE_MSG_ONLY, msg_pokefinder_15_03, ANIM_TYPE_CMD_SELECT },              //!< 保存失敗：それ以外のエラー

      { WND_TYPE_MSG_ONLY, msg_pokefinder_tutorial_02_01, ANIM_TYPE_PHOTO_SELECT },   //!< チュートリアル：写真選択
    };
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  MessageWindow::MessageWindow( DrawerBase* base, MessageWindowListener* listener, const u32 partsIndex, bool maskVisible )
    : m_pBase( base )
    , m_pMenuWindow( NULL )
    , m_pListener( listener )
    , m_InitSeq( 0 )
    , m_EndSeq( 0 )
    , m_MainSeq( SEQ_INIT )
    , m_WndType( WND_TYPE_NUM )
    , m_WndKind( WND_KIND_NUM )
    , m_ActiveAnim( 0 )
    , m_OpenReqKind( WND_KIND_NUM )
    , m_isOpened(false)
    , m_Result(gfl2::str::StrWin::RES_DONE)
    , m_pMsgLine_1( NULL )
    , m_pMsgLine_2( NULL )
  {
    gfl2::lyt::LytParts* wndParts = m_pBase->GetParts( partsIndex );
    {
      m_pBase->SetVisiblePane( wndParts, true );

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

      gfl2::lyt::LytPane* maskPane = m_pBase->GetPane( PANENAME_COMMON_WIN_LOW_002_PANE_BG_01, wndParts );
      {
        m_pBase->SetVisiblePane( maskPane, maskVisible );
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  MessageWindow::~MessageWindow( void )
  {
    ;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //----------------------------------------------------------------------------
  bool MessageWindow::InitFunc( app::util::AppRenderingManager* renderMan, app::tool::MenuWindowListener* listener, void* cursorBuf )
  {
    enum {
      INIT_SEQ_MSG_WND,
      INIT_SEQ_MENU_WND,
      INIT_SEQ_WAIT,
      INIT_SEQ_END,
    };

    switch( m_InitSeq )
    {
    case INIT_SEQ_MSG_WND:
      {
        app::util::G2DUtil* g2d = m_pBase->GetG2D();
        {
          g2d->CreateMessageWindow(
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
            0, m_pMsgLine_1, m_pMsgLine_2, m_pBase->GetHeap() );

          g2d->CreateMsgCursor( m_pBase->GetHeap(), m_pMsgCursor );
        }

        m_InitSeq++;
      } break;

    case INIT_SEQ_MENU_WND:
      {
        gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

        if( listener && cursorBuf )
        {
          m_pMenuWindow = GFL_NEW(devHeap) MenuWindow( m_pBase, renderMan, listener, cursorBuf );
          GFL_ASSERT( m_pMenuWindow );

          m_InitSeq++;
        }
        else {
          m_MainSeq = SEQ_IDLE;
          m_InitSeq = INIT_SEQ_END;
        }
      } break;

    case INIT_SEQ_WAIT:
      {
        if( m_pMenuWindow->IsReady() )
        {
          m_InitSeq++;
          m_MainSeq = SEQ_IDLE;
        }
      } break;

    case INIT_SEQ_END:
      {
        return true;
      }
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //----------------------------------------------------------------------------
  bool MessageWindow::EndFunc( void )
  {
    enum {
      END_SEQ_INIT,
      END_SEQ_WAIT,
      END_SEQ_DELETE,
      END_SEQ_END,
    };

    switch( m_EndSeq )
    {
    case END_SEQ_INIT:
      {
        m_MainSeq = SEQ_TERM;
        m_EndSeq++;
      } break;

    case END_SEQ_WAIT:
      {
        if( m_pMenuWindow )
        {
          if( !m_pMenuWindow->IsDelete() ) break;
        }

        m_EndSeq++;
      } break;

    case END_SEQ_DELETE:
      {
        GFL_SAFE_DELETE( m_pMenuWindow );

        app::util::G2DUtil* g2d = m_pBase->GetG2D();
        {
          g2d->DeleteMessageWindow();
        }

        m_MainSeq = SEQ_END;
        m_EndSeq++;
      } break;

    case END_SEQ_END:
      {
        return true;
      }
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief    更新
   */
  //----------------------------------------------------------------------------
  void MessageWindow::UpdateFunc( void )
  {
    app::util::G2DUtil* g2d = m_pBase->GetG2D();
    gfl2::str::StrWin::Result result = g2d->PrintMessage();

    switch( m_MainSeq )
    {
    case SEQ_OPEN:
      {
        if( m_pBase->IsAnimEnd( m_ActiveAnim ) )
        {
          m_MainSeq = SEQ_MSG;
        }
      } break;

    case SEQ_MSG:
      {
        if( result == m_Result )
        {
          if( m_WndType == WND_TYPE_MSG_ONLY )
          {
            Close();
          }
          else {
            // メニューウィンドウのオープン
            if( m_pMenuWindow ) m_pMenuWindow->Open();
            m_MainSeq = SEQ_MAIN;
          }
        }
      } break;

    case SEQ_CLOSE:
      {
        if( m_pBase->IsAnimEnd( m_ActiveAnim ) )
        {
          m_MainSeq = SEQ_IDLE;

          if( m_OpenReqKind == WND_KIND_NUM )
          {
            // 入力を有効
            if( m_pListener )
            {
              m_pListener->OnClosed();
              m_pListener->SetButtonEnable( true );
            }

            m_isOpened = false;
          }
          else {
            Open( m_OpenReqKind );
            m_OpenReqKind = WND_KIND_NUM;
          }
        }
      } break;
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief    開く
   */
  //----------------------------------------------------------------------------
  void MessageWindow::Open( WindowKind kind )
  {
    app::ui::UIView* view = m_pBase->GetUIView();
    
    GFL_ASSERT( kind < WND_KIND_TUTORIAL_SCORE );
    
    if( m_MainSeq == SEQ_IDLE )
    {
      m_WndType = c_WindowData[kind].wndType;
      m_WndKind = kind;
      u8 animType = c_WindowData[kind].animType;

      // メッセージの設定
      app::util::G2DUtil* g2d = m_pBase->GetG2D();
      {
        gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
        {
          g2d->GetString( str, c_WindowData[kind].msgID );

          gfl2::str::StrWin::FinishType finishType;
          if( m_WndType == WND_TYPE_MSG_ONLY )
          {
            m_Result   = gfl2::str::StrWin::RES_FINISH;
            finishType = gfl2::str::StrWin::FINISH_USER;
          }
          else {
            m_Result   = gfl2::str::StrWin::RES_DONE;
            finishType = gfl2::str::StrWin::FINISH_NONE;
          }
          g2d->SetMessageFinishMode( finishType, 0 );

          g2d->SetMessage( str );
        }
      }

      //m_pBase->SetVisiblePane( m_pMsgLine_1, true );
      //m_pBase->SetVisiblePane( m_pMsgLine_2, true );

      // カーソルアニメの再生
      static const u32 c_CursorKeepAnim[] = {
        LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__MENUWINDOW__PARTSMSG_01__CURSOR_00_KEEP,
        LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__MENUWINDOW__PARTSMSG_01__CURSOR_00_KEEP,
      };
      m_pBase->StartAnim( c_CursorKeepAnim[animType] );

      // インアニメの再生
      static const u32 c_OpenAnim[] = {
        LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__MENUWINDOW_IN,
        LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__MENUWINDOW_IN,
      };
      m_ActiveAnim = c_OpenAnim[animType];
      m_pBase->StartAnim( m_ActiveAnim );

      // 入力を無効
      if( m_pListener )
      {
        m_pListener->SetButtonEnable( false );
      }

      m_isOpened = true;
      m_MainSeq  = SEQ_OPEN;
    }
    else {
      m_OpenReqKind = kind;
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief    閉じる
   */
  //----------------------------------------------------------------------------
  void MessageWindow::Close( void )
  {
    if( m_pMenuWindow )
    {
      if( m_pMenuWindow->IsOpened() )
      {
        m_pMenuWindow->Close();
      }
    }

    static const u32 c_CloseAnim[] = {
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__MENUWINDOW_OUT,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__MENUWINDOW_OUT,
    };
    u8 animType = c_WindowData[m_WndKind].animType;
    m_ActiveAnim = c_CloseAnim[animType];
    m_pBase->StartAnim( m_ActiveAnim );

    m_MainSeq = SEQ_CLOSE;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief    開いているか
   */
  //----------------------------------------------------------------------------
  bool MessageWindow::IsOpened( void ) const
  {
    return m_isOpened;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief    ウィンドウの種類を取得
   */
  //----------------------------------------------------------------------------
  WindowKind MessageWindow::GetWindowKind( void ) const
  {
    return m_WndKind;
  }

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)
