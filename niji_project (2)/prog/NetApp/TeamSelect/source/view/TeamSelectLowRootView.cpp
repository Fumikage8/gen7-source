//======================================================================
/**
 * @file TeamSelectLowRootView.cpp
 * @date 2015/06/30 19:29:11
 * @author uchida_yuto
 * @brief チーム選択の下画面ルートView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/View/TeamSelectLowRootView.h"

#include "niji_conv_header/app/teamselect/res2d/teamSelect.h"
#include "niji_conv_header/app/teamselect/res2d/teamSelect_anim_list.h"
#include "niji_conv_header/app/teamselect/res2d/teamSelect_pane.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  TeamSelectLowRootView::TeamSelectLowRootView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager ) :
  UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( pRenderManager );
    m_pHeap           = pHeap;
    m_pRenderManager  = pRenderManager;
  }

  TeamSelectLowRootView::~TeamSelectLowRootView()
  {
    m_pTitleMessageWindow->Clear();
    GFL_SAFE_DELETE( m_pTitleMessageWindow );
    GFL_SAFE_DELETE( m_pCommonMessageWindow );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void TeamSelectLowRootView::Initialize( void* pLytBinData, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    GFL_ASSERT( pLytBinData );
    GFL_ASSERT( pAppMsgData );
    GFL_ASSERT( pWordSet );

    _Initialize2D( pLytBinData, pAppMsgData, pWordSet );

    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  BGのループアニメ再生
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__BG_00_BG_LOOP_00 );
    }

    //  ボタン関連の設定
    {
      gfl2::lyt::LytParts*  pBackBtnPartsPane  = pLayoutWork->GetPartsPane( PANENAME_TEAMSELECT_MIN_LOW_000_PANE_BACKBUTTON );
      GFL_ASSERT( pBackBtnPartsPane );

      const ::app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
      {
      /*
        app::tool::ButtonManager::ButtonId button_id;
        gfl2::lyt::LytPane* picture_pane;
        gfl2::lyt::LytPane* bound_pane;
        u32                 touch_anime_index;        //!< タッチ時のアニメ
        u32                 release_anime_index;      //!< タッチ決定時のアニメ
        u32                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
        u32                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
        u32                 active_anime_index;       //!< アクティブアニメ
        u32                 passive_anime_index;      //!< パッシブアニメ
      */
        //  もどるボタン
        {
          BUTTON_ID_BACK,
          pBackBtnPartsPane,
          pLayoutWork->GetBoundingPane( pBackBtnPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rMultiResId ),
          LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__BACKBUTTON_TOUCH,
          LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__BACKBUTTON_RELEASE,
          LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__BACKBUTTON_CANSEL,
          LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
          LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__BACKBUTTON_ACTIVE,
          LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__BACKBUTTON_PASSIVE
        },
      };

      UIResponder::CreateButtonManager( m_pHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
      //  入力コールバック設定
      UIView::SetInputListener( this );

      //  ボタンSEの設定
      {
        app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
        pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK, SEQ_SE_CANCEL1 );
      }
    }

    //  2行メッセージイベント作成
    //  共通クラスである、NetAppMessageWindowクラスをAppLibに移行したタイミングで差し替える
    {
      m_pCommonMessageWindow  = GFL_NEW( m_pHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowManipulator();
      App::Tool::CommonMessageWindowManipulator::SSetupInfo  setupInfo;
      //  作成に必要なパラメータを設定
      {
        setupInfo.pHeap               = m_pHeap;
        setupInfo.pTargetView         = this;
        setupInfo.layoutWorkID        = LYT_WORK_NORMAL;
        setupInfo.pHandler            = this;
        setupInfo.animID_TimerIcon    = LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__MESSAGE_L2__TIMERICON_KEEP;
        setupInfo.animID_TimerIcon_In = LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__MESSAGE_L2__TIMERICON_IN;
        setupInfo.animID_TimerIcon_Out= LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__MESSAGE_L2__TIMERICON_OUT;
        setupInfo.animID_MsgCursor    = LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000__MESSAGE_L2__CURSOR_00_KEEP;
        setupInfo.paneID_WindowParts  = PANENAME_TEAMSELECT_MIN_LOW_000_PANE_MESSAGE_L2;
        setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
        setupInfo.paneID_MsgCursor    = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;
        setupInfo.paneID_TimerIcon    = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;

        /**<  ストリーム表示TextBoxPaneがまとめられているPaneのID、即時表示との切り替えに使う  */
        setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;

        /**<  ストリーム表示TextBoxPane1/2                                      */
        setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;

        /**<  ストリーム表示TextBoxPane2/2                                      */
        setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;
      }

      m_pCommonMessageWindow->Setup( setupInfo );
    }

    //  タイトルメッセージのペイン作成
    //  共通クラスである、NetAppMessageWindowクラスをAppLibに移行したタイミングで差し替える
    {
      m_pTitleMessageWindowPartsPane = pLayoutWork->GetPartsPane( PANENAME_TEAMSELECT_MIN_LOW_000_PANE_MESSAGE_L1 );
      GFL_ASSERT( m_pTitleMessageWindowPartsPane );

      m_pTitleMessageTextBoxPane = pLayoutWork->GetTextBoxPane( m_pTitleMessageWindowPartsPane, PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, &rMultiResId );
      GFL_ASSERT( m_pTitleMessageTextBoxPane );

      pG2DUtil->SetPaneVisible( m_pTitleMessageTextBoxPane, true );

      m_pTitleMessageWindow = GFL_NEW( m_pHeap->GetDeviceHeap() ) print::MessageWindow( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(), pLayoutWork, 1, 1, m_pHeap->GetDeviceHeap() );
      m_pTitleMessageWindow->AddLinePaneIdxPtr( m_pTitleMessageTextBoxPane );
    }

    //  バックボタンは非表示に( ※ 使う側が表示設定をする )
    SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void TeamSelectLowRootView::Terminate()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   破棄可能か
    */
  //------------------------------------------------------------------
  bool TeamSelectLowRootView::IsDelete()
  {
    return  ( UIView::IsDrawing() == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectLowRootView::Update()
  {
    if( m_state == STATE_IDLE )
    {
    }

    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        //  ボタンアニメ終了
        //  入力復帰は外側で行う
        if( m_pushButtonId == BUTTON_ID_BACK )
        {
          m_bPushBackBtn  = true;
        }

        SetState( STATE_INPUT_STOP );
      }
    }

    if( m_state == STATE_INPUT_STOP )
    {
    }

    UIView::Update();

    if( m_pTitleMessageWindow != NULL )
    {
      m_pTitleMessageWindow->Proc();
    }

    m_pCommonMessageWindow->UpdateWindow();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectLowRootView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WORK_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderManager, displayNo, i );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult TeamSelectLowRootView::OnLayoutPaneEvent( const u32 id )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    if( id == BUTTON_ID_BACK )
    {
      //  入力演出待ちに（このときはすべての入力はOFF）
      m_pushButtonId  = (ButtonId)id;

      SetState( STATE_INPUT_EVENT_WAIT );

      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力通知用リスナ
    * @param[in] pButton  ボタン入力の情報
    * @param[in] pKey     キー入力の情報
    * @param[in] pStick   スティック入力の情報
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult TeamSelectLowRootView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_BACK );
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタン表示設定
    * @param[in] id   ボタンid
    * @param[in] bFlg 表示設定
    */
  //------------------------------------------------------------------
  void TeamSelectLowRootView::SetBtnVisible( const ButtonId id, const bool bFlg )
  {
    app::tool::ButtonManager* pBtnManager = UIResponder::GetButtonManager();
    if( bFlg == true )
    {
      pBtnManager->SetButtonVisible( id );
    }
    else
    {
      pBtnManager->SetButtonInvisible( id );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void TeamSelectLowRootView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
      m_pCommonMessageWindow->HideMessage();

      m_bPushBackBtn  = false;
      UIView::SetInputEnabledAllView( true );
    }
    else
    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      UIView::SetInputEnabledAllView( false );
    }
    else
    if( m_state == STATE_INPUT_STOP )
    {
      
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  メッセージ表示
  * @param[in]  pStrBuf             文字列
  * @param[in]  bUserInput ユーザー入力でメッセージ送りをするか
  * @param[in]  bImmediateMessage  true:即時表示 / false:ストリーム（タイピング）表示
  * @param[in]  bShowTimerIcon     タイマーアイコンを表示する（SetupでpaneID_TimerIconを指定していること）
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectLowRootView::ShowMessageWindow( gfl2::str::StrBuf* pStrBuf, const bool bUserInput, const bool bImmediateMessage, const bool bShowTimerIcon )
  {
    GFL_ASSERT( pStrBuf );

    m_bMessageFinishUserInput = false;
    if( bUserInput == true )
    {
      m_pCommonMessageWindow->SetFinishMode_UserInput();
      m_bMessageFinishUserInput = true;
    }
    else
    {
      m_pCommonMessageWindow->SetFinishMode_None();
    }

    m_pCommonMessageWindow->ShowMessage( pStrBuf, bImmediateMessage, bShowTimerIcon );

    m_bMessageEvent  = true;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  タイトルメッセージ表示
  * @param[in]  pStrBuf             文字列
  * @param[in]  bImmediateMessage   true:即時表示 / false:ストリーム（タイピング）表示
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectLowRootView::ShowTitleMessageWindow( gfl2::str::StrBuf* pStrBuf, const bool bImmediateMessage )
  {
    GFL_ASSERT( pStrBuf );

    //  共通用のは非表示に
    m_pCommonMessageWindow->HideMessage();

    ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pTitleMessageWindowPartsPane, true );

    if( bImmediateMessage == true )
    {
      pG2DUtil->SetTextBoxPaneString( m_pTitleMessageTextBoxPane, pStrBuf );
    }
    else
    {
      m_pTitleMessageWindow->SetString( *pStrBuf );
      m_pTitleMessageWindow->SetFinishMode( print::MessageWindow::FINISH_NONE, 15 );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  タイトルメッセージ非表示
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectLowRootView::HideTitleMessageWindow()
  {
    ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    if( pG2DUtil->GetPaneVisible( m_pTitleMessageWindowPartsPane ) == true )
    {
      pG2DUtil->SetPaneVisible( m_pTitleMessageWindowPartsPane, false );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  メッセージ非表示
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectLowRootView::HideMesssageWindow()
  {
    HideTitleMessageWindow();

    m_pCommonMessageWindow->HideMessage();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  メッセージイベント通知
  */
  //--------------------------------------------------------------------------------------------
  void  TeamSelectLowRootView::CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event )
  {
    if( event == App::Tool::CommonMessageWindowManipulator::EVENT_OnFinish )
    {
      m_bMessageEvent  = false;
    }
    else
    if( event == App::Tool::CommonMessageWindowManipulator::EVENT_OnEOM )
    {
      if( m_bMessageFinishUserInput == false )
      {
        m_bMessageEvent = false;
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void TeamSelectLowRootView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_TEAMSELECT_TEAMSELECT_MIN_LOW_000_BFLYT,
        LA_TEAMSELECT_TEAMSELECT_MIN_LOW_000___NUM,
        pAppLytBuff,
        true,
        ::app::util::G2DUtil::SETUP_LOWER,
        true
      }
    };

    UIView::Create2D( m_pHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
