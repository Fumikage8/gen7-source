//======================================================================
/**
 * @file BattleVideoRecordingLowRootView.h
 * @date 2015/09/15 16:23:10
 * @author uchida_yuto
 * @brief アプリの下画面ルートview
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/BattleVideoRecording/source/View/BattleVideoRecordingLowRootView.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/battle_video_rec/BattleVideoRecordingLytData.h"
#include "niji_conv_header/app/battle_video_rec/BattleVideoRecordingLytData_anim_list.h"
#include "niji_conv_header/app/battle_video_rec/BattleVideoRecordingLytData_pane.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

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
    GFL_SAFE_DELETE( m_pCommonMessageDialog );

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

      //  BGのループアニメ
      {
        pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEORECORDINGLYTDATA_BVR_MIN_LOW_000__BG_WAVE_BLUE_LOW_00_BG_LOOP_00 );
      }

      //  汎用ウィンドウダイアログ作成
      {
        //  汎用ウィンドウ作成
        m_pCommonMessageDialog  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowManipulator();

        //  パラメータ設定
        App::Tool::CommonMessageWindowManipulator::SSetupInfo setupInfo;

        app::util::GetPaneHelper  msgPartsPaneHelper( pLayoutWork, &rMultiResId );
        msgPartsPaneHelper.Push( PANENAME_BVR_MIN_LOW_000_PANE_PARTS_01 );
        {
          setupInfo.layoutWorkID        = LYT_WORK_NORMAL;

          setupInfo.paneID_WindowParts  = PANENAME_BVR_MIN_LOW_000_PANE_PARTS_01;
          setupInfo.animID_MsgCursor    = LA_BATTLEVIDEORECORDINGLYTDATA_BVR_MIN_LOW_000__PARTS_01__CURSOR_00_KEEP;
          setupInfo.animID_TimerIcon    = LA_BATTLEVIDEORECORDINGLYTDATA_BVR_MIN_LOW_000__PARTS_01__TIMERICON_KEEP;
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
        m_pCommonMessageDialog->Setup( setupInfo, msgPartsPaneHelper.Peek() );

        m_pMessageDialogPartPane  = msgPartsPaneHelper.Peek();
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

    m_pCommonMessageDialog->UpdateWindow();

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
      m_pInputDelegate->OnVideoRecordingLowRootViewKeyAction( pButton, pKey, pStick );
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
      
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージダイアログ表示
    */
  //------------------------------------------------------------------
  void LowRootView::ShowMessageDialog( gfl2::str::StrBuf* pStrBuf, const bool bUseInput, const bool bShowTimerIcon )
  {
    GFL_ASSERT( pStrBuf );

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pMessageDialogPartPane, true );

    if( m_bShowMessageDialog == false )
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEORECORDINGLYTDATA_BVR_MIN_LOW_000__PARTS_01_IN );
    }

    m_bShowMessageDialog  = true;
    m_bEndUpdateMessageDialog = false;

    if( bUseInput == true )
    {
      m_pCommonMessageDialog->SetFinishMode_UserInputOrTimer( 15 );
      UIView::SetInputEnabledAllView( false );
    }
    else
    {
      m_pCommonMessageDialog->SetFinishMode_None();
    }

    m_pCommonMessageDialog->ShowMessage( pStrBuf, false, bShowTimerIcon );
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージダイアログ非表示
    */
  //------------------------------------------------------------------
  void LowRootView::HideMessageDialog()
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    if( m_bShowMessageDialog == true )
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEORECORDINGLYTDATA_BVR_MIN_LOW_000__PARTS_01_OUT );
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
        LYTRES_BATTLEVIDEORECORDINGLYTDATA_BVR_MIN_LOW_000_BFLYT,
        LA_BATTLEVIDEORECORDINGLYTDATA_BVR_MIN_LOW_000___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );

    UIView::SetInputListener( this );
  }

GFL_NAMESPACE_END(BattleVideoRecording)
GFL_NAMESPACE_END(NetApp)
