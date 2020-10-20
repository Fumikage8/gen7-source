//==============================================================================
/**
 * @file    PokeListWindow.h
 * @author  fukushima_yuya
 * @date    2015.04.24
 * @brief   ポケモンリストのウィンドウ
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListDrawListener.h"
#include "App/PokeList/source/parts/PokeListCursorDraw.h"
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListItemExchange.h"
#include "App/PokeList/source/parts/PokeListMenuWindowDraw.h"
#include "App/PokeList/source/parts/PokeListPlateDraw.h"
#include "App/PokeList/source/parts/PokeListWindow.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_TimeIcon.h>
#include <Savedata/include/MyItemSave.h>


// arc
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//==============================================================================
// @brief   メッセージウィンドウクラス
//==============================================================================

  //----------------------------------------------------------------------------
  // @brief   定数
  //----------------------------------------------------------------------------
  //const u32 c_MSG_WND_IN_ANIM = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSMSG_00_IN;
  //const u32 c_MSG_WND_OUT_ANIM = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSMSG_00_OUT;
  const u32 c_MSG_WND_IN_ANIM = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000_IN_MESSAGE;
  const u32 c_MSG_WND_OUT_ANIM = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000_OUT_MESSSAGE;


  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  MessageWindow::MessageWindow( DrawerBase* pBase )
    : m_pBase( pBase )
    , m_isOpen( false )
    , m_FieldBtnVis( false )
    , m_JoinBtnVis( false )
    , m_pMsgParts( NULL )
    , m_pFieldBtnPane( NULL )
    , m_pJoinBtnPane( NULL )
    , m_pMsgText( NULL )
  {
    Init();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //----------------------------------------------------------------------------
  void MessageWindow::Init( void )
  {
    // ペインのセット
    {
      m_pMsgParts = m_pBase->GetParts( PANENAME_POKELIST_MIN_LOW_000_PANE_PARTSMSG_00 );
      m_pMsgText = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00, m_pMsgParts );

      m_pFieldBtnPane = m_pBase->GetPane( PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_00 );
      m_pJoinBtnPane = m_pBase->GetPane( PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_01 );

      m_pBase->SetVisiblePane( m_pMsgText, true );
      m_pBase->SetVisiblePane( m_pMsgParts, false );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウを開く
   */
  //----------------------------------------------------------------------------
  void MessageWindow::Open( void )
  {
    if( m_isOpen ) return;

    m_FieldBtnVis = m_pBase->IsVisiblePane( m_pFieldBtnPane );
    m_JoinBtnVis = m_pBase->IsVisiblePane( m_pJoinBtnPane );

    m_pBase->SetVisiblePane( m_pFieldBtnPane, false );
    m_pBase->SetVisiblePane( m_pJoinBtnPane, false );

    m_pBase->ChangeAnim( c_MSG_WND_OUT_ANIM, c_MSG_WND_IN_ANIM );
    m_pBase->SetVisiblePane( m_pMsgParts, true );

    m_isOpen = true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウを閉じる
   */
  //----------------------------------------------------------------------------
  void MessageWindow::Close( void )
  {
    if( !m_isOpen ) return;

    m_pBase->SetVisiblePane( m_pFieldBtnPane, m_FieldBtnVis );
    m_pBase->SetVisiblePane( m_pJoinBtnPane, m_JoinBtnVis );

    m_pBase->ChangeAnim( c_MSG_WND_IN_ANIM, c_MSG_WND_OUT_ANIM );

    m_isOpen = false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウのメッセージを設定する
   * @param   msgID   メッセージID
   */
  //----------------------------------------------------------------------------
  void MessageWindow::SetMessage( s32 msgID )
  {
    app::ui::UIView* view = m_pBase->GetUIView();

    view->SetTextboxPaneMessage( m_pMsgText, msgID );
  }




//==============================================================================
// @brief   メッセージウィンドウクラス
//==============================================================================

  //----------------------------------------------------------------------------
  // @brief   定数
  //----------------------------------------------------------------------------
  static const u32 c_WND_IN_ANIM = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_COMMANDSELECT_IN;
  static const u32 c_WND_OUT_ANIM = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_COMMANDSELECT_OUT;

  //! @brief  シーケンス
  enum WindowSequence
  {
    SEQ_WAIT,
    SEQ_ALL_OPEN,
    SEQ_OTR_OPEN,
    SEQ_MAIN,
    SEQ_ALL_CLOSE,
    SEQ_OTR_CLOSE,
    SEQ_MSG_WAIT,
    SEQ_NUM,
  };

  //! @brief  
  const s32 c_ItemExplainMsgID[] = {
    mes_pokelist_03_01,
    mes_pokelist_03_02,
    mes_pokelist_03_03,
    -1,
  };

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  PokeListWindow::PokeListWindow(
    DrawerBase* pBase,
    PlateDraw** ppPokePlate,
    CursorDraw* pCursor,
    app::util::AppRenderingManager* pRenderManager )
    : m_pRenderingManager(pRenderManager)
    , m_pBase( pBase )
    , m_pMenuWindow( NULL )
    , m_ppPokePlate( ppPokePlate )
    , m_pCursor( pCursor )
    , m_pItemExchange( NULL )
    , m_pLowMsgWnd( NULL )
    , m_uiListener( NULL )
    , m_Seq( SEQ_WAIT )
    , m_MenuCPos( 0 )
    , m_isOpen( false )
    , m_InputWait( false )
    , m_isFormChange( false )
    , m_BtnDisableFlag(false)
    , m_DisableContinue(false)
    , m_pCmdSelectParts( NULL )
    , m_pItemNamePane( NULL )
    , m_pItemNameText( NULL )
    , m_pMsg00TextBox2( NULL )
    , m_pItemIconPicture( NULL )
    , m_pMsgWindowParts( NULL )
    , m_PrevItemNo(ITEM_DUMMY_DATA)
  {
    ;
  }

  
  //----------------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::Initialize( void )
  {
    m_pCmdSelectParts = m_pBase->GetParts( PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_COMMANDSELECT );

    // メニューウィンドウの生成
    CreateMenuWindow();
    // メッセージウィンドウの生成
    CreateMessageWindow();
    // アイテム欄の初期化
    InitializePane_Item();
  }
    
  //----------------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの生成
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::CreateMenuWindow( void )
  {
    gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
    {
      MenuWindowDraw::CreateMenuParam param = {
        m_pRenderingManager,
        this,
        m_pBase,
        m_pCursor->GetMenuCursorBuffer()
      };

      // メニューウィンドウの生成
      m_pMenuWindow = GFL_NEW(heap) MenuWindowDraw( param );
      {
        GFL_ASSERT(m_pMenuWindow);
      }
    }
  }
    
  //----------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウの生成
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::CreateMessageWindow( void )
  {
    gfl2::lyt::LytParts* msgParts = m_pBase->GetParts( PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01, m_pCmdSelectParts );
    {
      m_pPaneNullText1 = m_pBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1, msgParts );
      m_pMsg00TextBox2 = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, msgParts );

      gfl2::lyt::LytPane* timeIcon = m_pBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON, msgParts );
      {
        m_pBase->SetVisiblePane( timeIcon, true );    // @fix GFNMCat[4571]
      }

      // メッセージウィンドウの生成
      app::util::G2DUtil* g2d = m_pBase->GetG2D();
      {
        gfl2::lyt::LytTextBox* textBox_msgLine1 = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A, msgParts );
        gfl2::lyt::LytTextBox* textBox_msgLine2 = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B, msgParts );
        {
          g2d->CreateMessageWindow(
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
            0, textBox_msgLine1, textBox_msgLine2, m_pBase->GetHeap() );
        }

        gfl2::lyt::LytPane* cursorPane = m_pBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00, msgParts );
        {
          g2d->CreateMsgCursor( m_pBase->GetHeap(), cursorPane );
        }
      }
    }

    // メッセージウィンドウ部品ペイン
    m_pMsgWindowParts = msgParts;   //!< @fix momiji:GFNMCat[4]
  }
    
  //----------------------------------------------------------------------------
  /**
   * @brief   アイテム欄の初期化
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::InitializePane_Item( void )
  {
    // CommandSelect内にあるItemName
    m_pItemNamePane = m_pBase->GetPane( PANENAME_COMMON_WIN_LOW_002_PANE_NULL_ITEMNAME, m_pCmdSelectParts );
    // CommnadSelect内にあるItemNameの子のTextBox00
    m_pItemNameText = m_pBase->GetTextBox( PANENAME_COMMON_WIN_LOW_002_PANE_TEXTBOX_00, m_pCmdSelectParts );

    // ItemIcon
    gfl2::lyt::LytParts* pItemIcon = m_pBase->GetParts( PANENAME_COMMON_WIN_LOW_002_PANE_ITEM_00, m_pCmdSelectParts );
    {
      m_pItemIconPicture = m_pBase->GetPicture( PANENAME_COMMON_ICN_006_PANE_ICON_ITEM, pItemIcon );
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //----------------------------------------------------------------------------
  bool PokeListWindow::Terminate( void )
  {
    // メニューウィンドウの破棄
    if( !DeleteMenuWindow() )
    {
      return false;
    }

    // メッセージウィンドウの破棄
    DeleteMessageWindow();

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの破棄
   */
  //----------------------------------------------------------------------------
  bool PokeListWindow::DeleteMenuWindow( void )
  {
    if( m_pMenuWindow )
    {
      if( !m_pMenuWindow->DeleteMenu() )
      {
        return false;
      }

      GFL_SAFE_DELETE( m_pMenuWindow );
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウの破棄
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::DeleteMessageWindow( void )
  {
    app::util::G2DUtil* g2d = m_pBase->GetG2D();
    {
      g2d->DeleteMessageWindow();
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   *
   * @return  "true  == 完了"
   * @return  "false == それ以外"
   */
  //----------------------------------------------------------------------------
  bool PokeListWindow::IsReady( void )
  {
    if( m_pMenuWindow )
    {
      return m_pMenuWindow->IsReady();
    }

    return true;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンのセット
   *
   * @param   itemNo    アイテムNo
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetItemIcon( u16 itemNo )
  {
    if( itemNo == ITEM_DUMMY_DATA )
    {
      FUKUSHIMA_PRINT( "itemNo == ITEM_DUMMY_DATA\n" );
      return;
    }
    if( itemNo == m_PrevItemNo )
    {
      FUKUSHIMA_PRINT( "itemNo == m_PrevItemNo\n" );
      return;
    }

    app::tool::ItemIconToolEx* itemIcon = m_pBase->GetItemIcon();
    {
      if( itemIcon )
      {
        itemIcon->SetItemIcon( 1, itemNo, m_pItemIconPicture );
      }
    }

    m_PrevItemNo = itemNo;
  }





  //----------------------------------------------------------------------------
  /**
   * @brief   セットアップ(フィールド用)
   * @param   pItemExchange   道具入れ替えクラス
   * @param   pMsgWnd         下部メッセージウィンドウクラス
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetupModeField( ItemExchange* pItemExchange, MessageWindow* pMsgWnd )
  {
    m_pItemExchange = pItemExchange;
    m_pLowMsgWnd = pMsgWnd;
  }



  //----------------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::Update( void )
  {
    if( m_pMenuWindow ) m_pMenuWindow->Update();

    switch( m_Seq )
    {
    case SEQ_ALL_OPEN:
      {
        if( !IsAnimEnd( WND_ANIM_OTHER_IN ) ) break;
        if( !IsAnimEnd( WND_ANIM_ALL_IN ) ) break;

        FUKUSHIMA_PRINT( "DisableContinue=[%s]\n", (m_DisableContinue) ? "true" : "false" );
        m_Seq = SEQ_MAIN;
      } break;

    case SEQ_ALL_CLOSE:
      {
        if( !IsAnimEnd( WND_ANIM_OTHER_OUT ) ) break;
        if( !IsAnimEnd( WND_ANIM_ALL_OUT ) ) break;

        FUKUSHIMA_PRINT( "DisableContinue=[%s]\n", (m_DisableContinue) ? "true" : "false" );
        // あずける時のみフォルムチェンジを開始する
        if( m_isFormChange )
        {
          //m_pItemExchange->StartFormChange( m_pCursor->GetCursorPos() );
          m_pItemExchange->StartFormChange( m_pMenuWindow->GetCursorPos() );  // @fix NMCat[1249]
          m_isFormChange = false;
        }
        m_Seq = SEQ_WAIT;
      } break;

    case SEQ_MAIN:
      {
        if( m_isFormChange ) break;

        MenuID menuId = m_pMenuWindow->GetActMenuID();
        CursorPos cpos = m_pMenuWindow->GetCursorPos();

        if( menuId != MENU_ITEM ) break;
        if( cpos == CPOS_NULL ) break;

        app::tool::MenuWindow* pWnd = m_pMenuWindow->GetMenuWindow( cpos, menuId );

        if( pWnd )
        {
          u32 pos = pWnd->GetCursorPos();

          if( m_MenuCPos != pos )
          {
            SetMessage( MSG_TYPE_MSG_ONLY, c_ItemExplainMsgID[pos], cpos );
            m_MenuCPos = pos;
          }
        }
      } break;

    case SEQ_OTR_OPEN:
      {
        if( !IsAnimEnd( WND_ANIM_OTHER_IN ) ) break;

        FUKUSHIMA_PRINT( "DisableContinue=[%s]\n", (m_DisableContinue) ? "true" : "false" );
        m_Seq = SEQ_MAIN;
      } break;

    case SEQ_OTR_CLOSE:
      {
        if( !IsAnimEnd( WND_ANIM_OTHER_OUT ) ) break;

        FUKUSHIMA_PRINT( "DisableContinue=[%s]\n", (m_DisableContinue) ? "true" : "false" );
        m_Seq = SEQ_WAIT;
      } break;

    case SEQ_MSG_WAIT:
      {
        app::util::G2DUtil* g2d = m_pBase->GetG2D();
        {
          gfl2::str::StrWin::Result result = g2d->PrintMessage();

          if( result == gfl2::str::StrWin::RES_FINISH )
          {
            Close();
          }
        }
      } break;
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   上部に表示するメッセージの設定
   * @param   msgType   メッセージのタイプ
   * @param   msgID     表示するメッセージのID
   * @param   cpos      プレートの位置
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetMessage( PokeListWindow::MessageType msgType, s32 msgID, CursorPos cpos )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();

    // 汎用メッセージのテキストを変更
    if( msgID != -1 )
    {
      if( msgType != MSG_TYPE_MSG_ONLY )
      {
        const pml::pokepara::PokemonParam* pp = staticWork->GetPokeConst( cpos );

        // ポケモンニックネームの設定
        SetPokeNickName( pp );
        // アイテム名の設定
        SetItemName( msgType, pp );
      }

      // テキストボックスに文字列をセット
      app::ui::UIView* view = m_pBase->GetUIView();
      {
        view->SetTextboxPaneMessage( m_pMsg00TextBox2, msgID );
      }
    }

    m_pBase->SetVisiblePane( m_pMsgWindowParts, ( msgID != -1 ) );   //!< @fix momiji:GFNMCat[4]
    m_pBase->SetVisiblePane( m_pMsg00TextBox2, true );
    m_pBase->SetVisiblePane( m_pPaneNullText1, false );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンニックネームの設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetPokeNickName( const pml::pokepara::PokemonParam* pp )
  {
    print::WordSet* wordSet = m_pBase->GetWordSet();
    {
      wordSet->RegisterPokeNickName( 0, pp );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アイテム名の設定
   *
   * @param   msgType   メッセージの種類
   * @param   pp        ポケモンパラメータ
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetItemName( PokeListWindow::MessageType msgType, const pml::pokepara::PokemonParam* pp )
  {
    // アイテム名を表示するメッセージタイプでなければ抜ける
    if( msgType != MSG_TYPE_MSG_NAME_ITEM )
    {
      FUKUSHIMA_PRINT( "msgType != MSG_TYPE_MSG_NAME_ITEM\n" );
      return;
    }

    // アイテムの取得
    u16 itemNo = ITEM_DUMMY_DATA;
    {
      bool fastmode = pp->StartFastMode();
      {
        itemNo = pp->GetItem();        //!< 所持アイテムの取得
      }
      pp->EndFastMode( fastmode );
    }

    // アイテムIDがダミーなら抜ける
    if( itemNo == ITEM_DUMMY_DATA )
    {
      FUKUSHIMA_PRINT( "itemNo == ITEM_DUMMY_DATA\n" );
      return;
    }

    print::WordSet* wordSet = m_pBase->GetWordSet();
    {
      wordSet->RegisterItemName( 1, itemNo, 1 );
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウを開く(全て)
   * @param   menuID    メニューウィンドウID
   * @param   item_out  アイテム欄の強制非表示フラグ(デフォルトfalse)
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::Open( MenuID menuID, bool item_out )
  {
    CursorPos cpos = m_pCursor->GetCursorPos();

    m_isOpen = true;

    // メッセージの設定
    {
      switch( menuID )
      {
      case MENU_POKE:
        SetMessage( MSG_TYPE_MSG_NAME, mes_pokelist_02_10, cpos );  //!< [ニックネーム]をどうする？
        break;
      case MENU_JOIN:
      case MENU_JOIN_ALREADY:
      case MENU_JOIN_DISABLE:
      case MENU_SODATEYA:
        SetMessage( MSG_TYPE_MSG_NAME, mes_pokelist_02_10, cpos );  //!< [ニックネーム]をどうする？
        break;
      }
    }

    // メッセージ＋半透明板の表示
    m_pBase->ChangeAnim( c_WND_OUT_ANIM, c_WND_IN_ANIM );
    m_pBase->SetVisiblePane( m_pCmdSelectParts, true );

    // ボタンの入力を無効に
    SetPokeListButtonDisable();

    OpenOther( menuID, item_out );

    m_Seq = SEQ_ALL_OPEN;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウを開く(メニューウィンドウ＋アイテム欄)
   * @param   menuID    メニューウィンドウID
   * @param   item_out  アイテム欄の強制非表示フラグ(デフォルトfalse)
   * @note    背景なしでOtherが呼ばれることはない
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::OpenOther( MenuID menuID, bool item_out )
  {
    if( !m_isOpen ) return;   //!< 背景が開かれていないのでリターン

    // メニューウィンドウを開く
    if( menuID != MENU_NULL )
    {
      if( m_pMenuWindow )
      {
        m_pMenuWindow->OpenWindow( menuID );
      }
    }

    // アイテム欄の表示
    if( !item_out )
    {
      StaticWork* staticWork = m_pBase->GetStaticWork();

      CursorPos cpos = staticWork->GetCursorPos( CID_POKE );
      u16 item_no = staticWork->GetPokeItem( cpos );

      if( item_no != ITEM_DUMMY_DATA )
      {
        app::util::G2DUtil* g2d = m_pBase->GetG2D();
        g2d->SetRegisterItemName( 0, item_no );
        g2d->SetTextBoxPaneStringExpand( m_pItemNameText, mes_pokelist_item_name );

        // アイテムアイコン
        SetItemIcon( item_no );
      }

      item_out = (item_no == ITEM_DUMMY_DATA);
    }

    m_pBase->SetVisiblePane( m_pItemNamePane, !item_out );

    SetPokeListEnable( false );   //!< ポケモンリスト側を無効に

    m_Seq = SEQ_OTR_OPEN;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メッセージのみのウィンドウをを開く
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::OpenMsgOnly( void )
  {
    // メッセージ＋半透明板の表示
    m_pBase->ChangeAnim( c_WND_OUT_ANIM, c_WND_IN_ANIM );
    m_pBase->SetVisiblePane( m_pCmdSelectParts, true );

    // カーソル非表示
    m_pCursor->SetVisible( false );

    m_InputWait = true;
    m_Seq = SEQ_ALL_OPEN;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウを閉じる(全て)
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::Close( void )
  {
    CloseOther();

    if( !m_DisableContinue )
    {
      // ボタンの入力を有効に
      SetPokeListButtonEnable();
    }

    // メッセージ＋半透明板の表示
    m_pBase->ChangeAnim( c_WND_IN_ANIM, c_WND_OUT_ANIM );

    m_InputWait = false;
    m_Seq = SEQ_ALL_CLOSE;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウを閉じる(メニューウィンドウ＋アイテム欄)
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::CloseOther( void )
  {
    if( m_pMenuWindow )   // @fix cov_ctr[10137]
    {
      m_pMenuWindow->CloseWindow();
    }

    m_pBase->SetVisiblePane( m_pItemNamePane, false );

    if( !m_DisableContinue )
    {
      SetPokeListEnable( true );   //!< ポケモンリスト側を有効に
    }

    m_Seq = SEQ_OTR_CLOSE;
  }

  void PokeListWindow::CloseWindow_MenuAndItem( void )
  {
    m_pMenuWindow->CloseWindow();

    m_pBase->SetVisiblePane( m_pItemNamePane, false );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メッセージのみのウィンドウを閉じる
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::CloseMsgOnly( void )
  {
    // メッセージ＋半透明板の表示
    m_pBase->ChangeAnim( c_WND_IN_ANIM, c_WND_OUT_ANIM );

    SetPokeListEnable( true );   //!< ポケモンリスト側を有効に

    m_InputWait = false;
    m_Seq = SEQ_ALL_CLOSE;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   開く・閉じるのアニメーションの終了確認
   * @param   anim_type   アニメーションタイプ
   */
  //----------------------------------------------------------------------------
  bool PokeListWindow::IsAnimEnd( PokeListWindow::WndAnimType anim_type )
  {
    switch( anim_type )
    {
    case WND_ANIM_ALL_IN:
      {
        return m_pBase->IsAnimEnd( c_WND_IN_ANIM );
      } //break;

    case WND_ANIM_ALL_OUT:
      {
        return m_pBase->IsAnimEnd( c_WND_OUT_ANIM );
      } //break;

    case WND_ANIM_OTHER_IN:
      {

      } break;

    case WND_ANIM_OTHER_OUT:
      {

      } break;
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   UIイベントリスナーのセット
   * @param   pListener   UIイベントリスナー
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetUIListener( PokeListDrawListener* pListener )
  {
    m_uiListener = pListener;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   入力待ちフラグの取得
   * @return  
   */
  //----------------------------------------------------------------------------
  bool PokeListWindow::GetInputWait( void )
  {
    return m_InputWait;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの設定
   * @param   enable    入力設定やカーソルの表示などのフラグ
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetPokeListEnable( bool enable )
  {
    // リスト側の入力の設定
    app::ui::UIView* view = m_pBase->GetUIView();
    view->SetInputEnabled( enable );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ボタンの入力を有効に
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetPokeListButtonEnable( void )
  {
    if( m_BtnDisableFlag )
    {
      app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
      {
        for( u8 i=0; i<BTN_ID_NUM; ++i )
        {
          // 元の入力設定状態に戻す
          btnMan->SetButtonInputEnable( i, m_BtnEnable[i] );
        }
      }
    }

    // カーソルの設定
    if( m_pCursor )
    {
      m_pCursor->SetVisible( true );
    }

    // 画面モードの変更
    LocalWork* localWork = m_pBase->GetLocalWork();
    {
      if( localWork->GetDispMode() == DISP_MODE_WINDOW )
      {
        localWork->SetDispMode( DISP_MODE_NORMAL );
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ボタンの入力を無効に
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SetPokeListButtonDisable( void )
  {
    app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
    {
      for( u8 i=0; i<BTN_ID_NUM; ++i )
      {
        // 現状の状態を保存
        m_BtnEnable[i] = btnMan->IsButtonInputEnable( i );
        // 全てfalseに
        btnMan->SetButtonInputEnable( i, false );
      }

      m_BtnDisableFlag = true;
    }

    // カーソルの設定
    if( m_pCursor )
    {
      m_pCursor->SetVisible( false );
    }

    // 画面モードの変更
    LocalWork* localWork = m_pBase->GetLocalWork();
    {
      localWork->SetDispMode( DISP_MODE_WINDOW );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アイテムを預けられるかの結果を取得
   *
   * @param   cpos    対象のプレート位置
   *
   * @return  RESULT_STORE_OK   = 道具を預けた
   *          RESULT_HAVE_NONE  = 道具を所持していない
   *          RESULT_BAG_FULL   = バッグがいっぱい
   */
  //----------------------------------------------------------------------------
  PokeListWindow::ITEM_STORE_RESULT PokeListWindow::GetItemStoreResult( CursorPos cpos )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();
    Savedata::MyItem * myItem = staticWork->GetMyItem();
    
    // 所持道具Noを取得
    u16 itemNo = ITEM_DUMMY_DATA;
    {
      const pml::pokepara::PokemonParam* pp = staticWork->GetPoke( cpos );
      
      bool fastmode_flag = pp->StartFastMode();
      {
        itemNo = pp->GetItem();
      }
      pp->EndFastMode( fastmode_flag );
    }
    
    // 所持アイテムなし
    if( itemNo == ITEM_DUMMY_DATA )
    {
      return RESULT_HAVE_NONE;
    }
    // ゼンリョクピース
    else if( item::ITEM_CheckPiece( itemNo ) )  // @fix NMCat[1774]
    {
      return RESULT_STORE_OK_ZITEM;
    }
    // バッグがいっぱい
    else if( myItem->AddCheck( itemNo, 1 ) == false )
    {
      return RESULT_BAG_FULL;
    }

    // 預けられた
    return RESULT_STORE_OK;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アイテムを預ける
   *
   * @param   cpos    対象のプレート位置
   * @param   res     アイテムを預けた結果
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::ItemStore( CursorPos cpos, ITEM_STORE_RESULT res )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();
    Savedata::MyItem * myItem = staticWork->GetMyItem();

    // 対象のポケモンを取得
    pml::pokepara::PokemonParam* pp = staticWork->GetPoke( cpos );

    bool fastmode_flag = pp->StartFastMode();
    {
      // 所持アイテムの取得
      u16 itemNo = pp->GetItem();

      // 預けられる
      if( res == RESULT_STORE_OK )
      {
        // バッグに収納
        myItem->Add( itemNo, 1, myItem->IsNew( itemNo ) );
        // 所持アイテムを空に
        pp->SetItem( ITEM_DUMMY_DATA );
      }
      // ゼンリョクピース用
      else if( res == RESULT_STORE_OK_ZITEM )
      {
        // 所持アイテムを空に
        pp->SetItem( ITEM_DUMMY_DATA );
      }
    }
    pp->EndFastMode( fastmode_flag );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   対象選択が必要なフィールド技か？
   * @param   wazaNo    技番号
   * @return  true = 対象選択が必要なフィールド技、false = それ以外
   */
  //----------------------------------------------------------------------------
  bool PokeListWindow::IsPokeSelectFieldWaza( WazaNo wazaNo )
  {
    if( wazaNo != WAZANO_TAMAGOUMI && wazaNo != WAZANO_MIRUKUNOMI ) return false;

    StaticWork* staticWork = m_pBase->GetStaticWork();
    CursorPos cpos = m_pMenuWindow->GetCursorPos();

    u32 nowHP = 0;
    u32 maxHP = 0;

    const pml::pokepara::PokemonParam* pp = staticWork->GetPokeConst( cpos );
    bool fastmode_flag = pp->StartFastMode();
    {
      nowHP = pp->GetHp();
      maxHP = pp->GetMaxHp();
    }
    pp->EndFastMode( fastmode_flag );

    // HPが1/5以下では使用できない
    u32 num = maxHP / 5;
    if( nowHP <= (num) ) return false;

    return true;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   メッセージのみのウィンドウをを開く
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::OpenMsgOnly( MessageType msgType, s32 msgID, CursorPos cpos, bool disableContinue )
  {
    app::util::G2DUtil* g2d = m_pBase->GetG2D();
    {
      g2d->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER, 0 );

      if( msgType != MSG_TYPE_MSG_ONLY )
      {
        StaticWork* staticWork = m_pBase->GetStaticWork();
        u16 itemNo = ITEM_DUMMY_DATA;

        const pml::pokepara::PokemonParam* pp = staticWork->GetPokeConst( cpos );
        {
          bool fastmode = pp->StartFastMode();
          {
            itemNo = pp->GetItem();
          }
          pp->EndFastMode( fastmode );
        }

        // ポケモン名
        g2d->SetRegisterPokeNickName( 0, pp );

        // アイテム名
        if( msgType == MSG_TYPE_MSG_NAME_ITEM )
        {
          if( itemNo != ITEM_DUMMY_DATA )
          {
            g2d->SetRegisterItemName( 1, itemNo );
          }
        }
      }

      gfl2::str::StrBuf* str = g2d->GetTempStrBuf( 1 );
      {
        // メッセージのセット
        g2d->GetStringExpand( str, msgID );
        // メッセージのセット
        g2d->SetMessage( str );
      }
    }

    m_pBase->SetVisiblePane( m_pPaneNullText1, true );    //!< @fix momiji:GFNMCat[4]
    m_pBase->SetVisiblePane( m_pMsg00TextBox2, false );   //!< @fix momiji:GFNMCat[4]
    m_pBase->SetVisiblePane( m_pMsgWindowParts, true );

    if( m_Seq == SEQ_WAIT )
    {
      OpenMsgOnly();
      m_Seq = SEQ_MSG_WAIT;
    }

    // 入力無効フラグのセット
    m_DisableContinue = disableContinue;
    FUKUSHIMA_PRINT( "DisableContinue=[%s]\n", (m_DisableContinue) ? "true" : "false" );
  }

  // ボタンの無効化リクエスト
  void PokeListWindow::RequestButtonDisable( void )
  {
    app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
    {
      for( u8 i=0; i<BTN_ID_NUM; ++i )
      {
        // 現状の状態を保存
        m_BtnEnable[i] = btnMan->IsButtonInputEnable( i );
        // 全てfalseに
        btnMan->SetButtonInputEnable( i, false );
      }

      m_BtnDisableFlag = true;
    }

    // カーソルの設定
    if( m_pCursor )
    {
      m_pCursor->SetVisible( false );
    }
  }

  // 画面状態の変更リクエスト
  void PokeListWindow::RequestSetWindowMode( void )
  {
    // 画面モードの変更
    LocalWork* localWork = m_pBase->GetLocalWork();
    {
      localWork->SetDispMode( DISP_MODE_WINDOW );
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   タイマーアイコンの再生
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::StartTimeIcon( void )
  {
    m_pBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_COMMANDSELECT__PARTSMSG_01__TIMERICON_IN );

    gfl2::lyt::LytWk* lytwk = m_pBase->GetLytWk();
    {
      // タイマーアイコンのループアニメ再生
      App::Tool::TimeIcon::StartAnime( lytwk, LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_COMMANDSELECT__PARTSMSG_01__TIMERICON_KEEP );
    }
  }



  //----------------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの選択
   * @param   選択された項目ID
   */
  //----------------------------------------------------------------------------
  void PokeListWindow::SelectMenu( u32 id )
  {
    FUKUSHIMA_PRINT( "id : %d\n", id );

    LocalWork* pLocalWork = m_pBase->GetLocalWork();
    StaticWork* staticWork = m_pBase->GetStaticWork();

    MenuAltID altID = m_pMenuWindow->GetSelectAltID( id );
    CursorPos cpos = m_pMenuWindow->GetCursorPos();
    FUKUSHIMA_PRINT( "altID : %d\n", altID );
    FUKUSHIMA_PRINT( "cpos : %d\n", cpos );

    //--------------------------------------------------
    // 他画面をコール
    //--------------------------------------------------
    switch( altID )
    {
    case ALT_POKE_STATUS:
    case ALT_JOIN_STATUS:
    case ALT_SODATEYA_STATUS:
    case ALT_PROMOTION_STATUS:
      {
        m_pBase->GetUIView()->SetInputEnabled( false );
        m_uiListener->CallPokeStatus( cpos );
        return;
      } //break;
    case ALT_POKE_REPAIR:
      {
        m_pBase->GetUIView()->SetInputEnabled( false );
        m_uiListener->CallBagRepair( cpos );
        return;
      } //break;
    case ALT_ITEM_USE:
      {
        m_pBase->GetUIView()->SetInputEnabled( false );
        m_uiListener->CallBag( cpos );
        return;
      } //break;
    }

    //--------------------------------------------------
    // メニューウィンドウの変更
    //--------------------------------------------------
    switch( altID )
    {
    case ALT_POKE_ITEM:
      {
        CloseOther();   //!< メニューウィンドウを閉じる
        SetMessage( MSG_TYPE_MSG_ONLY, mes_pokelist_03_01, cpos );
        OpenOther( MENU_ITEM );
        return;
      } //break;
    case ALT_ITEM_RETURN:
      {
        CloseOther();   //!< メニューウィンドウを閉じる
        SetMessage( MSG_TYPE_MSG_NAME, mes_pokelist_02_10, cpos );
        OpenOther( MENU_POKE );
        return;
      } //break;
    }

    //--------------------------------------------------
    // メッセージの変更
    //--------------------------------------------------
    switch( altID )
    {
    case ALT_ITEM_STORE:
      {
        // アイテムを預けられるかの結果を取得
        ITEM_STORE_RESULT result = GetItemStoreResult( cpos );

        if( result == RESULT_STORE_OK || result == RESULT_STORE_OK_ZITEM )
        {
          //SetMessage( MSG_TYPE_MSG_NAME_ITEM, mes_pokelist_04_05, cpos );
          OpenMsgOnly( MSG_TYPE_MSG_NAME_ITEM, mes_pokelist_04_05, cpos );

          // @fix GFNMcat[1566]
          //if( cpos < staticWork->GetMemberCount() )
          //{
          //  const pml::pokepara::PokemonParam* pPoke = staticWork->GetPokeConst( cpos );
          //  m_ppPokePlate[cpos]->SetupPlatePokeDisplay( pPoke );
          //}
        }
        else if( result == RESULT_HAVE_NONE )
        {
          //SetMessage( MSG_TYPE_MSG_NAME, mes_pokelist_04_03, cpos );
          OpenMsgOnly( MSG_TYPE_MSG_NAME, mes_pokelist_04_03, cpos );
        }
        else {
          //SetMessage( MSG_TYPE_MSG_ONLY, mes_pokelist_04_06, cpos );
          OpenMsgOnly( MSG_TYPE_MSG_ONLY, mes_pokelist_04_06, cpos );
        }

        // アイテムを預ける
        ItemStore( cpos, result );

        CloseWindow_MenuAndItem();   //!< メニューウィンドウを閉じる
        {
          m_Seq = SEQ_MSG_WAIT;
        }

        m_isFormChange = true;    //!< フォルムチェンジフラグを有効に
        return;
      } //break;
    case ALT_ITEM_HAND:
      {
        u16 itemNo = staticWork->GetPokeItem( cpos );

        if( itemNo == ITEM_DUMMY_DATA )
        {
          //SetMessage( MSG_TYPE_MSG_NAME_ITEM, mes_pokelist_04_03, cpos );
          OpenMsgOnly( MSG_TYPE_MSG_NAME_ITEM, mes_pokelist_04_03, cpos );

          CloseWindow_MenuAndItem();   //!< メニューウィンドウを閉じる
          {
            m_Seq = SEQ_MSG_WAIT;
          }
          return;
        }
        else {
          if( m_pItemExchange )
          {
            m_pItemExchange->StartExchange( false );
            m_pItemExchange->SetMovePos( cpos );

            // もちもの入れ替えをする場合は入れ替えボタンの入力を無効にしておく @fix NMCat[283]
            m_BtnEnable[BTN_ID_X] = false;
            m_BtnEnable[BTN_ID_Y] = false;
          }
          pLocalWork->SetDispMode( DISP_MODE_ITEM_EXCHANGE );
        }
      } break;
    }

    //--------------------------------------------------
    // 参加状況の変更
    //--------------------------------------------------
    u8 joinMax = staticWork->GetModeContext().join.join_poke_count_max;
    u8 joinCnt = staticWork->GetJoinPokeCount();

    switch( altID )
    {
      //! さんかする
    case ALT_JOIN_JOIN:
      {
        if( joinCnt < joinMax )
        {
          u8 poke_index = staticWork->GetPokeIndex( cpos );
          staticWork->AddJoinPoke( poke_index );

          // プレートメッセージウィンドウの更新
          //m_ppPokePlate[cpos]->SetModePlate( PLATE_MODE_JOIN );
          PlateMessageWindow* pMsgWindow = m_ppPokePlate[cpos]->GetPlateMessageWindow();
          pMsgWindow->SetupMessageWindow( PLATE_MODE_JOIN );
        }
        else {
          // NGメッセージの表示
          s32 msgID = mes_pokelist_04_40 + joinMax - 1;
          //SetMessage( MSG_TYPE_MSG_ONLY, msgID, CPOS_NULL );
          OpenMsgOnly( MSG_TYPE_MSG_ONLY, msgID, CPOS_NULL );

          CloseWindow_MenuAndItem();   //!< メニューウィンドウを閉じる
          {
            m_Seq = SEQ_MSG_WAIT;
          }
          return;
        }
      } break;

      //! さんかしない
    case ALT_JOIN_CANCEL:
      {
        u8 pokeIndex = staticWork->GetPokeIndex( cpos );
        // 参加をやめる
        staticWork->RemoveJoinPoke( pokeIndex );

        // メッセージウィンドウを閉じる
        PlateMessageWindow* pMsgWindow = m_ppPokePlate[cpos]->GetPlateMessageWindow();
        pMsgWindow->CloseWindow();

        for( u8 i=0; i<PLATE_NUM; ++i )
        {
          CursorPos cur_pos = static_cast<CursorPos>(i);
          pokeIndex = staticWork->GetPokeIndex( cur_pos );
          
          // 参加済みのポケモンのメッセージを更新
          if( staticWork->IsJoinPoke( pokeIndex ) )
          {
            pMsgWindow = m_ppPokePlate[cur_pos]->GetPlateMessageWindow();
            pMsgWindow->SetupMessageWindow( PLATE_MODE_JOIN );
          }
        }
      } break;
    }

    //--------------------------------------------------
    // 育て屋
    //--------------------------------------------------
    switch( altID )
    {
    case ALT_SODATEYA_DECIDE:
      {
        staticWork->SetEndMode( END_MODE_SELECT );
        m_uiListener->SetupOutData( END_MODE_SELECT );
      } break;
    }

    // ウィンドウを閉じる
    Close();
  }



GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
