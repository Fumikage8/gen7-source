//=============================================================================
/**
 * @file	 BattleVideoPlayerNetTopSelectFrame.h
 * @brief	 通信時の選択フレーム
 * @author yuto_uchida
 * @date	 2015.11.16
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoPlayer/source/frame/BattleVideoPlayerNetTopSelectFrame.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  NetTopSelectFrame::NetTopSelectFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  NetTopSelectFrame::~NetTopSelectFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void NetTopSelectFrame::Initialize( FRAME_PARAM* pFrameParam )
  {
    GFL_ASSERT( pFrameParam != NULL );

    m_pFrameParam = pFrameParam;

    //  出力パラメータをリセット
    m_pFrameParam->out.Reset();
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result NetTopSelectFrame::InitFunc(void)
  {
    //  UIViewの状態を初期化
    {
      UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

      //  下
      {
        LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();
        pLowRootView->SetState( LowRootView::STATE_IDLE );

        const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();

        pLowRootView->RemoveAllSubView();

        //  2択のViewを追加
        app::tool::GeneralLowerView*  pSelectButtonsView = pUIViewPool->GetLowGeneralButtonsView();
        {
          const UIViewPool::INIT_PARAM& rUiViewPoolInitParam = pUIViewPool->GetInitParam();
          gfl2::str::MsgData* pAppMsgData = rUiViewPoolInitParam.pAppMsgData;

          //  ボタンを二つ出す
          pSelectButtonsView->SetLayoutMode( app::tool::GeneralLowerView::LAYOUTMODE_2Buttons, false, false );

          //  1のボタン(バトルビデオを探す)
          {
            gfl2::str::StrBuf*  pTempStrBuf = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );

            pAppMsgData->GetString( msg_bvp_sys_low_11, *pTempStrBuf );
            pSelectButtonsView->SetTextBoxString( app::tool::GeneralLowerView::TEXTBOX_Button0, pTempStrBuf );
          }

          //  2のボタン(バトルビデオを公開)
          {
            gfl2::str::StrBuf*  pTempStrBuf = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );

            pAppMsgData->GetString( msg_bvp_sys_low_12, *pTempStrBuf );
            pSelectButtonsView->SetTextBoxString( app::tool::GeneralLowerView::TEXTBOX_Button1, pTempStrBuf );
          }

          pSelectButtonsView->SetEventHandler( this );
        }

        //  メッセージ表示
        {
          pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_02, false, false, false );
        }

        //  最初はＢボタンを出さない
        pLowRootView->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, false );
      }

      //  上
      {
        UppRootView*  pUppRootView  = pUIViewPool->GetUppRootView();
        pUppRootView->SetState( UppRootView::STATE_IDLE );

        pUppRootView->RemoveAllSubView();
        pUIViewPool->RemoveUppRootInfoView();

        const UIViewPool::INIT_PARAM& rUiViewPoolInitParam = pUIViewPool->GetInitParam();
        gfl2::str::MsgData* pAppMsgData = rUiViewPoolInitParam.pAppMsgData;

        gfl2::str::StrBuf*  pTempStrBuf = pUppRootView->GetG2DUtil()->GetTempStrBuf( 0 );

        //  タイトル表記
        {
          pAppMsgData->GetString( msg_bvp_sys_up_01, *pTempStrBuf );
          pUppRootView->ShowMessageUpperOneLine( pTempStrBuf );
        }

        //  説明表記
        {
          pAppMsgData->GetString( msg_bvp_sys_up_03, *pTempStrBuf );
          pUppRootView->ShowMessageCenter( pTempStrBuf );
        }
      }
    }

    return  applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result NetTopSelectFrame::UpdateFunc(void)
  {
    enum
    {
      SEQ_SELECT_START  = 0,
      SEQ_SELECT_WAIT,
      SEQ_END
    };

    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == SEQ_SELECT_START )
    {
      if( pUIViewPool->IsEndProcLowerSystemMessage() == true )
      {
        _StartSelectButtonsSelectView();

        pLowRootView->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, true );
        ++seq;
      }
    }

    if( seq == SEQ_SELECT_WAIT )
    {
      if( pLowRootView->IsEndInput() == true )
      {
        if( pLowRootView->GetPushBtn() == LowRootView::BUTTON_ID_BACK )
        {
          m_pFrameParam->out.result = RESULT_CANCEL;
          seq = SEQ_END;
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
      else
      if( m_selectButtonsEvent != app::tool::GeneralLowerView::IEventHandler::EVENT_ )
      {
        if( m_selectButtonsEvent == app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton0 )
        {
          m_pFrameParam->out.result = RESULT_SELECT_VIDEO_SEARCH;
        }
        else
        if( m_selectButtonsEvent == app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton1 )
        {
          m_pFrameParam->out.result = RESULT_SELECT_VIDEO_PUBLIC;
        }
        else
        {
          GFL_ASSERT( 0 );
        }

        seq = SEQ_END;
      }
    }

    if( seq == SEQ_END )
    {
      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void NetTopSelectFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result NetTopSelectFrame::EndFunc(void)
  {
    _EndSelectButtonsSelectView();

    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;
    UppRootView*  pUppRootView  = pUIViewPool->GetUppRootView();

    app::tool::GeneralLowerView*  pSelectButtonsView = pUIViewPool->GetLowGeneralButtonsView();
    pSelectButtonsView->SetEventHandler( NULL );

    //  中央のダイアログを非表示に
    pUppRootView->HideMessageCenter();

    return applib::frame::RES_FINISH;
  }

  //  選択のコールバック
  void  NetTopSelectFrame::GeneralLowerView_OnEvent( app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode )
  {
    m_selectButtonsEvent = eventCode;
  }

  //  選択ボタン表示開始/終了!
  void  NetTopSelectFrame::_StartSelectButtonsSelectView()
  {
    UIViewPool* pUIViewPool     = m_pFrameParam->in.pUIViewPool;
    LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();

    app::tool::GeneralLowerView*  pSelectButtonsView = pUIViewPool->GetLowGeneralButtonsView();
    GFL_ASSERT( pSelectButtonsView );
    pSelectButtonsView->SetInputEnabledAllView( true );

    pLowRootView->AddSubView( pSelectButtonsView );

    m_selectButtonsEvent = app::tool::GeneralLowerView::IEventHandler::EVENT_;
  }

  void  NetTopSelectFrame::_EndSelectButtonsSelectView()
  {
    UIViewPool* pUIViewPool     = m_pFrameParam->in.pUIViewPool;
    pUIViewPool->GetLowGeneralButtonsView()->RemoveFromSuperView();
  }

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )
