//======================================================================
/**
 * @file TrainerPhotoLowRootView.h
 * @date 2015/09/02 16:23:10
 * @author uchida_yuto
 * @brief 証明写真アプリの下画面ルートview
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "App/TrainerPhoto/source/View/TrainerPhotoLowRootView.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt.h"
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt_anim_list.h"
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt_pane.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(photo)

  TrainerPhotoLowRootView::TrainerPhotoLowRootView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  TrainerPhotoLowRootView::~TrainerPhotoLowRootView()
  {
    GFL_SAFE_DELETE( m_pCommonMessageDialog );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool TrainerPhotoLowRootView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBinData, m_initParam.pAppMsgData, m_initParam.pWordSet );

      gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
      ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
      gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      //  ボタン登録
      {
        app::util::GetPaneHelper  backBtnPaneHelper( pLayoutWork, &rMultiResId );
        backBtnPaneHelper.Push( PANENAME_ID_PHOTO_MIN_LOW_000_PANE_BUTTONBACK );

        app::util::GetPaneHelper  enterBtnPaneHelper( pLayoutWork, &rMultiResId );
        enterBtnPaneHelper.Push( PANENAME_ID_PHOTO_MIN_LOW_000_PANE_BUTTONA );

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
            backBtnPaneHelper.Peek(),
            backBtnPaneHelper.GetBoundingPane( PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 ),
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONBACK_TOUCH,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONBACK_RELEASE,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONBACK_CANSEL,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONBACK_TOUCH_RELEASE,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONBACK_ACTIVE,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONBACK_PASSIVE,
          },

          //  決定ボタン
          {
            BUTTON_ID_ENTER,
            enterBtnPaneHelper.Peek(),
            enterBtnPaneHelper.GetBoundingPane( PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 ),
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONA_TOUCH,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONA_RELEASE,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONA_CANSEL,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONA_TOUCH_RELEASE,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONA_ACTIVE,
            LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__BUTTONA_PASSIVE,
          },
        };

        UIResponder::CreateButtonManager( m_initParam.pHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
        //  ボタンSEの設定
        {
          app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
          pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK, SEQ_SE_CANCEL1 );
          pButtonManager->SetButtonSelectSE( BUTTON_ID_ENTER, SEQ_SE_DECIDE1 );
        }
      }

      //  汎用ウィンドウダイアログ作成
      {
        //  汎用ウィンドウ作成
        m_pCommonMessageDialog  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowManipulator();

        //  パラメータ設定
        App::Tool::CommonMessageWindowManipulator::SSetupInfo setupInfo;

        app::util::GetPaneHelper  msgPartsPaneHelper( pLayoutWork, &rMultiResId );
        msgPartsPaneHelper.Push( PANENAME_ID_PHOTO_MIN_LOW_000_PANE_MASSAGE_01 );
        {
          setupInfo.layoutWorkID        = LYT_WORK_NORMAL;

          setupInfo.animID_MsgCursor    = LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__MASSAGE_01__PARTSMSG_01__CURSOR_00_KEEP;
          setupInfo.animID_TimerIcon    = LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__MASSAGE_01__PARTSMSG_01__TIMERICON_KEEP;
          setupInfo.animID_TimerIcon_In = LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__MASSAGE_01__PARTSMSG_01__TIMERICON_IN;
          setupInfo.animID_TimerIcon_Out= LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__MASSAGE_01__PARTSMSG_01__TIMERICON_OUT;
          setupInfo.paneID_MsgCursor    = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
          setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
          setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1A_OF_TEXTBOX_1A;
          setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
          setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1B_OF_TEXTBOX_1B;
          setupInfo.paneID_TimerIcon    = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
          setupInfo.pHandler            = this;
          setupInfo.pHeap               = m_initParam.pHeap;
          setupInfo.pTargetView         = this;
        }
        m_pCommonMessageDialog->Setup( setupInfo, msgPartsPaneHelper.GetPartsPane( PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01 ) );

        m_pMessageDialogPartPane  = msgPartsPaneHelper.Peek();
      }

      {
        app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
        paneHelper.Push( PANENAME_ID_PHOTO_MIN_LOW_000_PANE_MASSAGE_00 );

        m_pMessageWindowTextPane  = paneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_LOW_007_PANE_TEXTBOX_00 );
      }

      {
        app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
        paneHelper.Push( PANENAME_ID_PHOTO_MIN_LOW_000_PANE_BUTTONA );

        m_pEnterButtonTextPane    = paneHelper.GetTextBoxPane( PANENAME_COMMON_BTN_LOW_019_PANE_TEXTBOX_00 );
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
  bool TrainerPhotoLowRootView::EndFunc()
  {
    return  ( UIView::IsDrawing() == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void TrainerPhotoLowRootView::Update()
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

    m_pCommonMessageDialog->UpdateWindow();

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void TrainerPhotoLowRootView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
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
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult TrainerPhotoLowRootView::OnLayoutPaneEvent( const u32 id )
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
  app::ui::UIInputListener::ListenerResult TrainerPhotoLowRootView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_ENTER );
      return  DISABLE_ACCESS;
    }

    if( m_pInputDelegate != NULL )
    {
      m_pInputDelegate->OnTrainerPhotoLowRootViewKeyAction( pButton, pKey, pStick );
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void TrainerPhotoLowRootView::SetState( const State newStaet )
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
      
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   決定ボタンのテキスト設定
    */
  //------------------------------------------------------------------
  void TrainerPhotoLowRootView::SetEnterButtonText( gfl2::str::StrBuf* pStrBuf )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pStrBuf );

    pG2DUtil->SetTextBoxPaneString( m_pEnterButtonTextPane, pStrBuf );
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージウィンドウ表示
    */
  //------------------------------------------------------------------
  void TrainerPhotoLowRootView::ShowMessageWindow( gfl2::str::StrBuf* pStrBuf )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pStrBuf );

    pG2DUtil->SetTextBoxPaneString( m_pMessageWindowTextPane, pStrBuf );
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージダイアログ表示
    */
  //------------------------------------------------------------------
  void TrainerPhotoLowRootView::ShowMessageDialog( gfl2::str::StrBuf* pStrBuf, const u32 showTime )
  {
    GFL_ASSERT( pStrBuf );

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pMessageDialogPartPane, true );

    if( m_bShowMessageDialog == false )
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__MASSAGE_01_IN );
    }

    m_bShowMessageDialog  = true;
    m_bEndUpdateMessageDialog = false;

    m_pCommonMessageDialog->SetFinishMode_UserInputOrTimer( showTime );
    UIView::SetInputEnabledAllView( false );

    m_pCommonMessageDialog->ShowMessage( pStrBuf );
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージダイアログ非表示
    */
  //------------------------------------------------------------------
  void TrainerPhotoLowRootView::HideMessageDialog()
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    if( m_bShowMessageDialog == true )
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000__MASSAGE_01_OUT );
    }

    if( m_state == STATE_IDLE )
    {
      UIView::SetInputEnabledAllView( true );
    }

    m_bShowMessageDialog  = false;
    m_bEndUpdateMessageDialog = false;
    m_pCommonMessageDialog->HideMessage();
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージダイアログのコールバック
    */
  //------------------------------------------------------------------
  void  TrainerPhotoLowRootView::CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event )
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
  void TrainerPhotoLowRootView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
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
        LYTRES_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000_BFLYT,
        LA_IDPHOTOLYT_ID_PHOTO_MIN_LOW_000___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );

    UIView::SetInputListener( this );
  }

GFL_NAMESPACE_END(photo)
GFL_NAMESPACE_END(app)
