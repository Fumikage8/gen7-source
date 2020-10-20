//=============================================================================
/**
 * @file	 BattleVideoRecordingDataSaveFrame.h
 * @brief	 データ保存フレーム
 * @author yuto_uchida
 * @date	 2015.09.15
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoRecording/source/frame/BattleVideoRecordingDataSaveFrame.h"

//  nijiのインクルード
#include "ExtSavedata/include/BattleRecorderSaveData.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_rec.h"

//  サウンドのインクルード
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoRecording )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  DataSaveFrame::DataSaveFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  DataSaveFrame::~DataSaveFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void DataSaveFrame::Initialize( FRAME_PARAM* pFrameParam )
  {
    GFL_ASSERT( pFrameParam != NULL );

    m_pFrameParam = pFrameParam;
    GFL_ASSERT( m_pFrameParam->in.pUIViewPool );
    GFL_ASSERT( m_pFrameParam->in.pExtSaveData );

    //  出力パラメータをリセット
    m_pFrameParam->out.Reset();
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result DataSaveFrame::InitFunc(void)
  {
    //  UIViewの状態を初期化
    {
      UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

      //  下
      {
        LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();
        pLowRootView->SetState( LowRootView::STATE_IDLE );

        pLowRootView->RemoveAllSubView();

        //  2択のViewを追加
        app::tool::GeneralLowerView*  pGeneraluttonsView = pUIViewPool->GetLowGeneralButtonsView();
        {
          const UIViewPool::INIT_PARAM& rUiViewPoolInitParam = pUIViewPool->GetInitParam();
          gfl2::str::MsgData* pAppMsgData = rUiViewPoolInitParam.pAppMsgData;

          gfl2::str::StrBuf*  pTempStrBuf = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );

          //  はい
          {
            pAppMsgData->GetString( msg_bv_rec_sel_01, *pTempStrBuf );
            pGeneraluttonsView->SetTextBoxString( app::tool::GeneralLowerView::TEXTBOX_Button0, pTempStrBuf );
          }

          //  いいえ
          {
            pAppMsgData->GetString( msg_bv_rec_sel_02, *pTempStrBuf );
            pGeneraluttonsView->SetTextBoxString( app::tool::GeneralLowerView::TEXTBOX_Button1, pTempStrBuf );
          }

          pGeneraluttonsView->SetEventHandler( this );
        }
      }

      //  上
      {
        UppRootView*  pUppRootView  = pUIViewPool->GetUppRootView();
        pUppRootView->SetState( UppRootView::STATE_IDLE );

        pUppRootView->RemoveAllSubView();
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
  applib::frame::Result DataSaveFrame::UpdateFunc(void)
  {
    enum
    {
      //  録画するかどうかのメッセージ
      SEQ_EVENT_REC_CONFIRM_MESSAGE = 0,

      //  録画するかどうかの選択
      SEQ_REC_CONFIRM_SELECT_START,
      SEQ_REC_CONFIRM_SELECT_WAIT,

      //  録画キャンセルするかどうか
      SEQ_REC_CANCEL_CONFRIM_SELECT_START,
      SEQ_REC_CANCEL_CONFRIM_SELECT_WAIT,

      //  保存開始
      SEQ_VIDEO_SAVE_START,
      SEQ_VIDEO_SAVE_WAIT,

      //  セーブ成功したが記録数が最大になったとき
      SEQ_VIDEO_SAVE_SUCCESS_DATA_FULL,

      //  メッセージ表示
      SEQ_VIDEO_SHOW_MESSAGE,

      SEQ_END
    };

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    UIViewPool* pUIViewPool     = m_pFrameParam->in.pUIViewPool;

    //  録画するかどうかのメッセージ
    {
      if( seq == SEQ_EVENT_REC_CONFIRM_MESSAGE )
      {
        //  メッセージ表示
        pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_02, pUIViewPool->GetLowRootView(), false, false, false, false );
        m_endShopMessageNextSeq = SEQ_REC_CONFIRM_SELECT_START;

        seq = SEQ_VIDEO_SHOW_MESSAGE;
      }
    }

    //  録画するかどうかのはい・いいえ選択
    {
      if( seq == SEQ_REC_CONFIRM_SELECT_START )
      {
        _StartTwoButtonsSelectView();

        seq = SEQ_REC_CONFIRM_SELECT_WAIT;
      }

      if( seq == SEQ_REC_CONFIRM_SELECT_WAIT )
      {
        bool  bNextFlow = false;

        if( m_twoButtonsEvent == app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton0 )
        {
          bNextFlow = true;

          //  はい
          seq = SEQ_VIDEO_SAVE_START;
        }
        else
        if( m_twoButtonsEvent == app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton1 )
        {
          bNextFlow = true;

          //  いいえ
          //  メッセージ表示
          {
            pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_03, pUIViewPool->GetLowRootView(), false, false, false, false );
            m_endShopMessageNextSeq = SEQ_REC_CANCEL_CONFRIM_SELECT_START;

            seq = SEQ_VIDEO_SHOW_MESSAGE;
          }
        }

        //  遷移が変わったのでこの遷移の後始末をする
        if( bNextFlow == true )
        {
          _EndTwoButtonsSelectView();
        }
      }
    }

    //  録画キャンセル選択
    {
      if( seq == SEQ_REC_CANCEL_CONFRIM_SELECT_START )
      {
        _StartTwoButtonsSelectView();

        seq = SEQ_REC_CANCEL_CONFRIM_SELECT_WAIT;
      }

      if( seq == SEQ_REC_CANCEL_CONFRIM_SELECT_WAIT )
      {
        bool  bNextFlow = false;

        if( m_twoButtonsEvent == app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton0 )
        {
          bNextFlow = true;

          //  はい
          m_pFrameParam->out.result = RESULT_SAVE_CANCEL;
          seq = SEQ_END;
        }
        else
        if( m_twoButtonsEvent == app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton1 )
        {
          bNextFlow = true;

          //  いいえ
          seq = SEQ_EVENT_REC_CONFIRM_MESSAGE;
        }

        //  遷移が変わったのでこの遷移の後始末をする
        if( bNextFlow == true )
        {
          _EndTwoButtonsSelectView();
        }
      }
    }

    //  保存開始
    //  @note 最初にセーブしてからエラーを出す形式みたい
    {
      ExtSavedata::BattleRecorderSaveData::Result result  = ExtSavedata::BattleRecorderSaveData::RESULT_WAIT_ASYNC;

      bool bSaveResultChk = false;

      if( seq == SEQ_VIDEO_SAVE_START )
      {
        //  非同期でセーブ
        result  = m_pFrameParam->in.pExtSaveData->SaveData( true );

        bSaveResultChk  = true;
        m_bFirstWaitAsyncSeq  = true;

        seq = SEQ_VIDEO_SAVE_WAIT;
      }
      else
      if( seq == SEQ_VIDEO_SAVE_WAIT )
      {
        result  = m_pFrameParam->in.pExtSaveData->WaitAsync();

        bSaveResultChk  = true;
      }

      //  セーブ結果をチェック
      if( bSaveResultChk == true )
      {
        ///<非同期終了待ち
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_WAIT_ASYNC )
        {
          if( m_bFirstWaitAsyncSeq == true )
          {
            m_bFirstWaitAsyncSeq  = false;

            // @fix GFNMCat[813] 連続表示の判定方法にミスがあった。専用のフラグを用意して対応
            //  セーブ中でまだメッセージを表示していないのであれば表示する
            pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_06, pUIViewPool->GetLowRootView(), true, false, true, false );
          }
        }
        else
        ///<セーブ成功
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_SUCCESS )
        {
          Sound::PlaySE( SEQ_SE_SAVE );

          {
            pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_08, pUIViewPool->GetLowRootView(), false, false, false, false );
            m_endShopMessageNextSeq = SEQ_END;
          }

          seq = SEQ_VIDEO_SHOW_MESSAGE;

          m_pFrameParam->out.result = RESULT_SAVE_SUCCESS;
        }
        else
        ///<セーブ成功したが、セーブ件数が満杯になった
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_SUCCESS_AND_FULL )
        {
          Sound::PlaySE( SEQ_SE_SAVE );

          m_pFrameParam->out.result = RESULT_SAVE_SUCCESS;

          {
            pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_08, pUIViewPool->GetLowRootView(), false, false, false, false );
            m_endShopMessageNextSeq = SEQ_VIDEO_SAVE_SUCCESS_DATA_FULL;
          }

          seq = SEQ_VIDEO_SHOW_MESSAGE;
        }
        else
        ///<セーブ件数が満杯で失敗
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_FAILED_FULL )
        {
          //  記録数がいっぱいで保存できないメッセージ
          {
            pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_05, pUIViewPool->GetLowRootView(), false, false, false, false );

            // @fix GFNMCat[1640] 保存がもうできないのでメッセージ出して終了
            m_endShopMessageNextSeq = SEQ_END;
            m_pFrameParam->out.result = RESULT_SAVE_ERROR;
          }

          seq = SEQ_VIDEO_SHOW_MESSAGE;
        }
        else
        ///<SDカードが満杯で失敗
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_FAILED_SD_FULL )
        {
          //  SDカードに空きがなくて保存できないメッセージ
          {
            pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_04, pUIViewPool->GetLowRootView(), false, false, false, false );

            // @fix GFNMCat[1640] 保存がもうできないのでメッセージ出して終了
            m_endShopMessageNextSeq = SEQ_END;
            m_pFrameParam->out.result = RESULT_SAVE_ERROR;
          }

          seq = SEQ_VIDEO_SHOW_MESSAGE;
        }
        else
        ///<フェイタルエラー
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_FAILED_FATAL_ERROR )
        {
          m_pFrameParam->out.result = RESULT_SAVE_FATAL;
          seq = SEQ_END;
        }
        else
        ///<その他の失敗
        {
          UCHIDA_PRINT( "Failed rec save result(%d)\n", result );
          //  その他の失敗メッセージ
          {
            pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_09, pUIViewPool->GetLowRootView(), false, false, false, false );
            m_endShopMessageNextSeq = SEQ_END;
            m_pFrameParam->out.result = RESULT_SAVE_ERROR;
          }

          seq = SEQ_VIDEO_SHOW_MESSAGE;
        }
      }
    }

    //  セーブ成功したが記録数が最大になったとき
    if( seq == SEQ_VIDEO_SAVE_SUCCESS_DATA_FULL )
    {
      //  ビデオ記録数が最大になった時のメッセージ
      {
        pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_07, pUIViewPool->GetLowRootView(), false, false, false, false );
        m_endShopMessageNextSeq = SEQ_END;
      }

      seq = SEQ_VIDEO_SHOW_MESSAGE;
    }

    //  メッセージ表示
    if( seq == SEQ_VIDEO_SHOW_MESSAGE )
    {
      if( pUIViewPool->IsMessageShow() == false )
      {
        seq = m_endShopMessageNextSeq;
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
  void DataSaveFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result DataSaveFrame::EndFunc(void)
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    //  ２択設定の解除
    {
      app::tool::GeneralLowerView*  pGeneralButtonsView = pUIViewPool->GetLowGeneralButtonsView();
      pGeneralButtonsView->SetEventHandler( NULL );
    }

    return applib::frame::RES_FINISH;
  }

  //  2選択のコールバック
  void  DataSaveFrame::GeneralLowerView_OnEvent( app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode )
  {
    m_twoButtonsEvent = eventCode;
  }

  //  ２選択ボタン表示開始/終了!
  void  DataSaveFrame::_StartTwoButtonsSelectView()
  {
    UIViewPool* pUIViewPool     = m_pFrameParam->in.pUIViewPool;
    LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();

    app::tool::GeneralLowerView*  pGeneralButtonsView = pUIViewPool->GetLowGeneralButtonsView();

    //  初回カーソル位置設定
    pGeneralButtonsView->CursorMoveTo( 0 );

    pLowRootView->AddSubView( pGeneralButtonsView );

    m_twoButtonsEvent = app::tool::GeneralLowerView::IEventHandler::EVENT_;
  }

  void  DataSaveFrame::_EndTwoButtonsSelectView()
  {
    UIViewPool* pUIViewPool     = m_pFrameParam->in.pUIViewPool;
    pUIViewPool->GetLowGeneralButtonsView()->RemoveFromSuperView();
  }

GFL_NAMESPACE_END( BattleVideoRecording )
GFL_NAMESPACE_END( NetApp )
