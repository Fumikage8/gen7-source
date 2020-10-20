//=============================================================================
/**
 * @file	 BattleVideoPlayerDataCtrlBaseFrame.h
 * @brief	 データ操作フレーム(共通機能)
 * @author yuto_uchida
 * @date	 2015.10.01
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataCtrlBaseFrame.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"

//  gfl2のインクルード

//  nijiのインクルード

//  リソースのインクルード

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  DataCtrlBaseFrame::DataCtrlBaseFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  DataCtrlBaseFrame::~DataCtrlBaseFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void DataCtrlBaseFrame::Initialize( FRAME_PARAM* pFrameParam )
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
  applib::frame::Result DataCtrlBaseFrame::InitFunc(void)
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    if( m_initResult == applib::frame::RES_FINISH )
    {
      return  applib::frame::RES_FINISH;
    }

    u32 seq = CellSuper::GetSubSeq();

    //  UIViewの状態を初期化
    if( seq == INIT_ROOT_SEQ_SETTING_INIT )
    {
      //  下
      {
        LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();
        pLowRootView->RemoveAllSubView();

        //  Bボタンを出す
        pLowRootView->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, true );

        //  データ操作Viewを追加
        {
          LowDataCtrlView*  pLowDataCtrlView  = pUIViewPool->GetLowDataCtrlView();
          pLowDataCtrlView->SetState( LowDataCtrlView::STATE_IDLE );

          //  すべてのボタンを非表示に
          pLowDataCtrlView->SetContentButtonNonVisible();

          pLowRootView->AddSubView( pLowDataCtrlView );
        }

        pLowRootView->SetState( LowRootView::STATE_INPUT_STOP );
      }

      //  上
      {
        UppRootView*  pUppRootView  = pUIViewPool->GetUppRootView();
        pUppRootView->SetState( UppRootView::STATE_IDLE );

        pUppRootView->HideMessageCenter();
      }

      ++seq;
    }
    else
    if( seq == INIT_ROOT_SEQ_SETTING_WAIT )
    {
      bool  bRet  = true;
      DataDelFlow::PARAM  param;
      param.pUIViewPool = m_pFrameParam->in.pUIViewPool;
      param.pVideoDataManager = m_pFrameParam->in.pVideoDataManager;

      if( m_dataDelFlow.InitFunc( param ) == false )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++seq;
      }
    }
    else
    {
      UppRootView*  pUppRootView  = pUIViewPool->GetUppRootView();

      //  ゲーム情報viewを追加
      {
        pUppRootView->RemoveAllSubView();

        //  情報更新
        // @fix GFNMCat[1589] サーバーデータの場合は上画面表示反映を強制する
        //                    内部でデータのアドレスが異なる時だけ表示するようにしているが、サーバーデータの場合は一つのデータしかないので。
        pUIViewPool->SetVideoDataUppInfoView( m_pFrameParam->in.pVideoData, ( m_pFrameParam->in.pVideoData->bServerDataFlg == 1 ) );
        pUIViewPool->AddUppRootInfoView();
      }

      m_initResult  = applib::frame::RES_FINISH;
    }

    CellSuper::SetSubSeq( seq );

    return  m_initResult;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result DataCtrlBaseFrame::UpdateFunc(void)
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;
    LowRootView*      pLowRootView      = pUIViewPool->GetLowRootView();

    applib::frame::Result result  = applib::frame::RES_CONTINUE;

    u32 seq = CellSuper::GetSubSeq();

    if( seq == 0 )
    {
      //  初回起動
      seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_CTRL_START );
    }
    else
    //  ビデオ操作画面制御中
    if( seq == UPDATE_ROOT_SEQ_DATA_CTRL_WAIT )
    {
      LowDataCtrlView*  pLowDataCtrlView  = pUIViewPool->GetLowDataCtrlView();

      if( pLowRootView->IsEndInput() == true )
      {
        //  Bキャンセルを押したので終了
        if( pLowRootView->GetPushBtn() == LowRootView::BUTTON_ID_BACK )
        {
          m_updateDataCtrlResult.bCancel = true;
          seq = UPDATE_ROOT_SEQ_DATA_CTRL_END;
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
      else
      if( pLowDataCtrlView->IsEndInput() == true )
      {
        m_updateDataCtrlResult.bCancel = false;

        LowDataCtrlView::ButtonId pushButtonId  = pLowDataCtrlView->GetPushButtonId();
        if( pushButtonId == LowDataCtrlView::BUTTON_ID_CHNAGE_INFO )
        {
          //  情報切り替え
          pUIViewPool->ChangeUppInfoViewState();

          //  フロー復帰
          pLowRootView->SetState( LowRootView::STATE_IDLE );
          pLowDataCtrlView->SetState( LowDataCtrlView::STATE_IDLE );
        }
        else
        {
          seq = UPDATE_ROOT_SEQ_DATA_CTRL_END;
        }

        m_updateDataCtrlResult.pushBtn  = pushButtonId;
      }
    }
    else
    //  データ削除
    if( seq == UPDATE_ROOT_SEQ_DATA_DEL_WAIT )
    {
      DataDelFlow::Result delFlowResult  = m_dataDelFlow.Run();
      if( delFlowResult != DataDelFlow::RESULT_RUN )
      {
        if( delFlowResult == DataDelFlow::RESULT_CANCEL ) 
        {
          //  元の操作に戻る
          seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_CTRL_START );
        }
        else
        if( delFlowResult == DataDelFlow::RESULT_DEL_FATAL )
        {
          //  フェータル失敗はフェータルイベント呼び出すように結果を返す
          //  元の操作に戻る
          m_updateDataCtrlResult.bFatal = true;
          seq = UPDATE_ROOT_SEQ_DATA_CTRL_END;
        }
        else
        {
          seq = UPDATE_ROOT_SEQ_DATA_DEL_END;
        }
      }
    }
    //  メッセージ
    else
    if( seq == UPDATE_ROOT_SEQ_MESSAGE )
    {
      if( pUIViewPool->IsEndProcLowerSystemMessage() == true )
      {
        pUIViewPool->HideLowerSystemMessage();
        seq = _StartSeq( m_messageRetSeq );
      }
    }
    else
    if( seq == UPDATE_ROOT_SEQ_YES_NO_MENU_MESSAGE )
    {
      if( pUIViewPool->IsEndProcLowerSystemMessage() == true )
      {
        if( ( pUIViewPool->GetLowerSystemMessageCode() == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
        ||  ( pUIViewPool->GetLowerSystemMessageCode() == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo ) )
        {
          pUIViewPool->HideLowerSystemMessage();

          if( pUIViewPool->GetLowerSystemMessageCode() == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
          {
            seq = _StartSeq( m_yesNoMenuMessageYesSeq );
          }
          else
          if( pUIViewPool->GetLowerSystemMessageCode() == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
          {
            seq = _StartSeq( m_yesNoMenuMessageNoSeq );
          }
          else
          {
            GFL_ASSERT( 0 );
          }
        }
      }
    }
    else
    if( seq == UPDATE_ROOT_SEQ_DATA_BTL_TEAM_SELECT_MESSAGE_END )
    {
      //  模擬戦のためのチーム選択へ
      m_pFrameParam->out.result = DataCtrlBaseFrame::RESULT_BATTLE_AI;
      result  = applib::frame::RES_FINISH;      
    }

    CellSuper::SetSubSeq( seq );

    return result;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void DataCtrlBaseFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result DataCtrlBaseFrame::EndFunc(void)
  {
    u32 seq = CellSuper::GetSubSeq();

    if( m_endResult == applib::frame::RES_FINISH )
    {
      return  m_endResult;
    }

    if( seq == END_ROOT_SEQ_START )
    {
      ++seq;
    }
    else
    if( seq == END_ROOT_SEQ_WAIT )
    {
      if( m_dataDelFlow.EndFunc() == true )
      {
        ++seq;
      }
    }
    else
    if( seq == END_ROOT_SEQ_END )
    {
      UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;
      pUIViewPool->HideLowerSystemMessage();

      m_endResult = applib::frame::RES_FINISH;
    }

    CellSuper::SetSubSeq( seq );

    return m_endResult;
  }

  //  シーケンス設定
  u32 DataCtrlBaseFrame::_StartSeq( const u32 seq )
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    u32 newSeq  = seq;

    if( newSeq == UPDATE_ROOT_SEQ_DATA_CTRL_START )
    {
      LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();
      LowDataCtrlView*  pLowDataCtrlView  = pUIViewPool->GetLowDataCtrlView();

      pLowDataCtrlView->SetVisible( true );

      pLowRootView->SetState( LowRootView::STATE_IDLE );
      pLowDataCtrlView->SetState( LowDataCtrlView::STATE_IDLE );

      m_updateDataCtrlResult.Reset();
      newSeq  = UPDATE_ROOT_SEQ_DATA_CTRL_WAIT;
    }
    else
    if( newSeq == UPDATE_ROOT_SEQ_DATA_DEL_START )
    {
      m_dataDelFlow.Start( m_pFrameParam->in.pVideoData );

      newSeq = UPDATE_ROOT_SEQ_DATA_DEL_WAIT;
    }
    else
    if( newSeq == UPDATE_ROOT_SEQ_DATA_BTL_TEAM_SELECT_MESSAGE )
    {
      newSeq  = _ShowMessageSeq( msg_bvp_win_10, UPDATE_ROOT_SEQ_DATA_BTL_TEAM_SELECT_MESSAGE_END, true );
    }

    return  newSeq;
  }

  //  メッセージ表示して終ったら指定したシーケンスに遷移
  u32 DataCtrlBaseFrame::_ShowMessageSeq( const u32 strId, const u32 retSeq, const bool bShowBG )
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    //  メッセージ表示
    const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
    pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, strId, false, false, bShowBG );

    m_messageRetSeq = retSeq;

    return  _StartSeq( UPDATE_ROOT_SEQ_MESSAGE );
  }

  //  メニューウィンドウYES/NOを表示して終ったら指定したシーケンスに遷移
  u32 DataCtrlBaseFrame::_ShowYesNoMenuMessageSeq( const u32 strId, const u32 yesRetSeq, const u32 noRetSeq, const bool bShowBG )
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;
    const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();

    m_yesNoMenuMessageYesSeq  = yesRetSeq;
    m_yesNoMenuMessageNoSeq   = noRetSeq;

    gfl2::str::StrBuf*  pTitleStrBuf  = pUIViewPool->GetLowRootView()->GetG2DUtil()->GetTempStrBuf( 0 );

    rInitParam.pAppMsgData->GetString( strId, *pTitleStrBuf );
    pUIViewPool->ShowMessageWithListMenu(
      /*  ウインドウ設定  */
      pTitleStrBuf,
      false,
      true,

      /*  リスト設定  */
      *rInitParam.pAppMsgData,
      msg_bvp_sys_low_13,
      msg_bvp_sys_low_14
    );

    return  _StartSeq( UPDATE_ROOT_SEQ_YES_NO_MENU_MESSAGE );
  }


GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )
