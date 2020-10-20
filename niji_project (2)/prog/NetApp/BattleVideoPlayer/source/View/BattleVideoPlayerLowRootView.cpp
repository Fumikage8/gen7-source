//======================================================================
/**
 * @file BattleVideoPlayerLowRootView.h
 * @date 2015/09/15 16:23:10
 * @author uchida_yuto
 * @brief アプリの下画面ルートview
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerLowRootView.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_anim_list.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_pane.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  LowRootView::LowRootView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  LowRootView::~LowRootView()
  {
    GFL_SAFE_DELETE( m_pCommonMessageDialogTwoLine );
    GFL_SAFE_DELETE( m_pCommonMessageDialogOneLine );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool LowRootView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBinData, m_initParam.pAppMsgData, m_initParam.pWordSet );

      gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
      ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
      gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__BG_WAVE_BLUE_LOW_00_BG_LOOP_00 );

#if 0
      //  汎用ウィンドウダイアログ作成(1行用)
      //  ※仕様を見ると使うケースがない！
      {
        //  汎用ウィンドウ作成
        app::util::GetPaneHelper  msgPartsPaneHelper( pLayoutWork, &rMultiResId );
        msgPartsPaneHelper.Push( PANENAME_BVP_MIN_LOW_000_PANE_PARTS_00 );

        m_pMessageDialogOneLinePartPane  = msgPartsPaneHelper.Peek();
        pG2DUtil->SetPaneVisible( m_pMessageDialogOneLinePartPane, false );
      }

      //  汎用ウィンドウダイアログ作成(2行用)
      {
        //  汎用ウィンドウ作成
        m_pCommonMessageDialogTwoLine  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowManipulator();

        //  パラメータ設定
        App::Tool::CommonMessageWindowManipulator::SSetupInfo setupInfo;

        app::util::GetPaneHelper  msgPartsPaneHelper( pLayoutWork, &rMultiResId );
        msgPartsPaneHelper.Push( PANENAME_BVP_MIN_LOW_000_PANE_PARTS_01 );
        {
          setupInfo.layoutWorkID        = LYT_WORK_NORMAL;

          setupInfo.paneID_WindowParts  = PANENAME_BVP_MIN_LOW_000_PANE_PARTS_01;
          setupInfo.animID_MsgCursor    = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__PARTS_01__CURSOR_00_KEEP;
          setupInfo.animID_TimerIcon    = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__PARTS_01__TIMERICON_KEEP;
          setupInfo.animID_TimerIcon_In = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__PARTS_01__TIMERICON_IN;
          setupInfo.animID_TimerIcon_Out= LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__PARTS_01__TIMERICON_OUT;
          setupInfo.paneID_MsgCursor    = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;
          setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
          setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1A_OF_TEXTBOX_1A;
          setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
          setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1B_OF_TEXTBOX_1B;
          setupInfo.paneID_TimerIcon    = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
          setupInfo.pHandler            = this;
          setupInfo.pHeap               = m_initParam.pHeap;
          setupInfo.pTargetView         = this;
        }
        m_pCommonMessageDialogTwoLine->Setup( setupInfo, msgPartsPaneHelper.Peek() );

        m_pMessageDialogTwoLinePartPane = msgPartsPaneHelper.Peek();
      }
#else
      //  じゃまなので１行と２行のは非表示に
      {
        app::util::GetPaneHelper  msgPartsPaneHelper( pLayoutWork, &rMultiResId );
        msgPartsPaneHelper.Push( PANENAME_BVP_MIN_LOW_000_PANE_PARTS_00 );
        m_pMessageDialogOneLinePartPane  = msgPartsPaneHelper.Peek();

        pG2DUtil->SetPaneVisible( m_pMessageDialogOneLinePartPane, false );
      }

      {
        app::util::GetPaneHelper  msgPartsPaneHelper( pLayoutWork, &rMultiResId );
        msgPartsPaneHelper.Push( PANENAME_BVP_MIN_LOW_000_PANE_PARTS_01 );

        pG2DUtil->SetPaneVisible( msgPartsPaneHelper.Peek(), false );
      }
#endif

      //  ボタン関連の設定
      {
        gfl2::lyt::LytParts*  pBackBtnPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_000_PANE_BTN_BACK );
        GFL_ASSERT( pBackBtnPartsPane );

        const app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
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
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__BTN_BACK_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__BTN_BACK_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__BTN_BACK_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__BTN_BACK_TOUCH_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__BTN_BACK_ACTIVE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__BTN_BACK_PASSIVE
          },
        };

        UIResponder::CreateButtonManager( m_initParam.pHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
        //  入力コールバック設定
        UIView::SetInputListener( this );

        //  ボタンSEの設定
        {
          app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
          pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK, SEQ_SE_CANCEL1 );
        }
      }

      ++m_initSeq;
    }
    
    if( m_initSeq == 1 )
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  bool LowRootView::EndFunc()
  {
    return  ( UIView::IsDrawing() == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowRootView::Update()
  {
    if( m_state == STATE_IDLE )
    {
    }

    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        SetState( STATE_INPUT_STOP );
      }
    }

    if( m_state == STATE_INPUT_STOP )
    {
    }

    if( m_pCommonMessageDialogOneLine != NULL )
    {
      m_pCommonMessageDialogOneLine->UpdateWindow();
    }

    if( m_pCommonMessageDialogTwoLine != NULL )
    {
      m_pCommonMessageDialogTwoLine->UpdateWindow();
    }

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowRootView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderManager, displayNo, i );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   指定したボタンを表示設定
    */
  //------------------------------------------------------------------
  void LowRootView::SetVisiblePushBtn( const ButtonId id, const bool bVisible )
  {
    if( bVisible == true )
    {
      UIView::GetButtonManager()->SetButtonVisible( id );
    }
    else
    {
      UIView::GetButtonManager()->SetButtonInvisible( id );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowRootView::OnLayoutPaneEvent( const u32 id )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    m_pushButtonId  = (ButtonId)id;

    //  入力演出待ちに（このときはすべての入力はOFF）
    SetState( STATE_INPUT_EVENT_WAIT );

    return  DISABLE_ACCESS;
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
  app::ui::UIInputListener::ListenerResult LowRootView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    if( m_pInputDelegate != NULL )
    {
      m_pInputDelegate->OnTrainerPhotoLowRootViewKeyAction( pButton, pKey, pStick );
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
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void LowRootView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
      m_pushButtonId  = BUTTON_ID_NONE;
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
      UIView::SetInputEnabledAllView( false );
    }
  }

    //  下画面のシステムウィンドウは App::Tool::MessageMenuView クラスを使っているのでもう使わない
#if 0
  //------------------------------------------------------------------
  /**
    * @brief   メッセージダイアログ表示
    */
  //------------------------------------------------------------------
  void LowRootView::ShowMessageDialog( const ShowDialogMessageLine messageLine, gfl2::str::StrBuf* pStrBuf, const bool bUseInput, const bool bShowTimerIcon )
  {
    GFL_ASSERT( pStrBuf );

    //  いったん非表示
    HideMessageDialog();

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    m_bShowMessageDialog      = true;
    m_bEndUpdateMessageDialog = false;

    App::Tool::CommonMessageWindowManipulator*  pAcitveDialog = m_pCommonMessageDialogOneLine;
    if( messageLine == SHOW_DIALOG_MESSAGE_LINE_ONE )
    {
      pAcitveDialog = m_pCommonMessageDialogOneLine;
      pG2DUtil->SetPaneVisible( m_pMessageDialogOneLinePartPane, true );

      if( m_bShowMessageDialog == false )
      {
    //    pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BattleVideoPlayerLYTDATA_BVR_MIN_LOW_000__PARTS_01_IN );
      }
    }
    else
    if( messageLine == SHOW_DIALOG_MESSAGE_LINE_TWO )
    {
      pAcitveDialog = m_pCommonMessageDialogTwoLine;
      pG2DUtil->SetPaneVisible( m_pMessageDialogTwoLinePartPane, true );

      if( m_bShowMessageDialog == false )
      {
        pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000__PARTS_01_IN );
      }
    }

    if( bUseInput == true )
    {
      pAcitveDialog->SetFinishMode_UserInputOrTimer( 15 );
      UIView::SetInputEnabledAllView( false );
    }
    else
    {
      pAcitveDialog->SetFinishMode_None();
    }

    pAcitveDialog->ShowMessage( pStrBuf, false, bShowTimerIcon );
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージダイアログ非表示
    */
  //------------------------------------------------------------------
  void LowRootView::HideMessageDialog()
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    if( m_state == STATE_IDLE )
    {
      UIView::SetInputEnabledAllView( true );
    }

    m_bShowMessageDialog  = false;
    m_bEndUpdateMessageDialog = false;

    if( m_pCommonMessageDialogOneLine != NULL )
    {
      m_pCommonMessageDialogOneLine->HideMessage();
    }

    if( m_pCommonMessageDialogTwoLine != NULL )
    {
      m_pCommonMessageDialogTwoLine->HideMessage();
    }
  }
#endif

  //------------------------------------------------------------------
  /**
    * @brief   メッセージダイアログのコールバック
    */
  //------------------------------------------------------------------
  void  LowRootView::CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event )
  {
    if( event == App::Tool::CommonMessageWindowManipulator::EVENT_OnFinish )
    {
      m_bEndUpdateMessageDialog = true;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void LowRootView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  pAppLytBuff, 16, app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000_BFLYT,
        LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_000___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );

    UIView::SetInputListener( this );
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
