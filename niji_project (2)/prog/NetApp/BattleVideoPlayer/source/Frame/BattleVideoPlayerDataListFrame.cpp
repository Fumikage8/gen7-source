//=============================================================================
/**
 * @file	 BattleVideoPlayerDataListFrame.h
 * @brief	 データ選択フレーム
 * @author yuto_uchida
 * @date	 2015.09.24
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoPlayer/source/frame/BattleVideoPlayerDataListFrame.h"

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

//  gfl2のインクルード

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
  DataListFrame::DataListFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  DataListFrame::~DataListFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void DataListFrame::Initialize( FRAME_PARAM* pFrameParam )
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
  applib::frame::Result DataListFrame::InitFunc(void)
  {
    if( m_initResult == applib::frame::RES_FINISH )
    {
      return  applib::frame::RES_FINISH;
    }

    u32 seq = CellSuper::GetSubSeq();
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    //  UIViewの状態を初期化
    if( seq == INIT_ROOT_SEQ_SETTING )
    {
      //  下
      {
        LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();

        pLowRootView->RemoveAllSubView();

        //  Bボタンを出す
        pLowRootView->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, true );

        //  データリストViewを追加
        {
          LowDataListView*  pLowDataListView = pUIViewPool->GetLowDataListView();
          pLowRootView->AddSubView( pLowDataListView );

          pLowDataListView->SetDelegate( this );

          {
            pLowDataListView->SetListData( m_pFrameParam->in.pVideoDataManager, m_pFrameParam->in.listScrollData );
          }
        }

        // @fix GFNMCat[1097] フレーム初期化時にはステートを入力停止してペインリストの選択移動をとめる
        //                    こうしないとがフレームがまだ初期化なのにリスト更新処理が走って動作に不都合が発生する
        pLowRootView->SetState( LowRootView::STATE_INPUT_STOP );
      }

      //  上
      {
        UppRootView*  pUppRootView  = pUIViewPool->GetUppRootView();
        pUppRootView->SetState( UppRootView::STATE_IDLE );

        pUppRootView->RemoveAllSubView();

        //  情報Viewを追加
        pUIViewPool->AddUppRootInfoView();
      }

      ++seq;
    }
    else
    if( seq == INIT_ROOT_SEQ_SETTING_WAIT )
    {
      DataDelFlow::PARAM  param;

      param.pUIViewPool       = pUIViewPool;
      param.pVideoDataManager = m_pFrameParam->in.pVideoDataManager;

      if( m_dataDelFlow.InitFunc( param ) == true )
      {
        ++seq;
      }
    }
    else
    {
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
  applib::frame::Result DataListFrame::UpdateFunc(void)
  {
    applib::frame::Result result  = applib::frame::RES_CONTINUE;

    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    LowRootView*      pLowRootView      = pUIViewPool->GetLowRootView();
    LowDataListView*  pLowDataListView  = pUIViewPool->GetLowDataListView();

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    //  ビデオデータ選択
    if( seq == SEQ_SELECT_START )
    {
      pLowRootView->SetState( LowRootView::STATE_IDLE );
      pLowDataListView->SetState( LowDataListView::STATE_IDLE );

      //  リスト設定
      pLowDataListView->SetListData( m_pFrameParam->in.pVideoDataManager, m_pFrameParam->in.listScrollData );

      //  通常選択のモードへ
      pLowDataListView->SetDelMode( LowDataListView::DEL_MODE_NONE );

      seq = SEQ_SELECT_WAIT;
    }
    else
    if( seq == SEQ_SELECT_WAIT )
    {
      if( pLowRootView->IsEndInput() == true )
      {
        //  Bキャンセルを押したので終了
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
      if( pLowDataListView->IsEndInput() == true )
      {
        //  リスト操作が終ったので一度リストスクロールを出力
        m_pFrameParam->in.listScrollData = pLowDataListView->GetListScrollData();

        LowDataListView::ButtonId pushButtonId  = pLowDataListView->GetPushBtnId();
        if( pushButtonId == LowDataListView::BUTTON_ID_DATA_CHANGE )
        {
          //  情報を切り替え
          pLowDataListView->SetState( LowDataListView::STATE_IDLE );

          _ChangeInfoState();
        }
        else
        if( pushButtonId == LowDataListView::BUTTON_ID_DATA_DEL )
        {
          //  削除シーケンスへ遷移
          seq = SEQ_DEL_SELECT_START;
        }
        else
        //  データ選択
        if( pushButtonId == LowDataListView::BUTTON_ID_DATA_SELECT )
        {
          //  選択したデータが操作可能か
          NetAppLib::BattleVideoPlayer::VideoDataManager* pVideoDataManager = m_pFrameParam->in.pVideoDataManager;
          NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData  = pVideoDataManager->GetListData( m_activeSelectIdx );
          GFL_ASSERT( pVideoData );

          if( pVideoData->bErrorVersionFlg == false )
          {
            m_pFrameParam->out.result = RESULT_DATA_SELECT;
            m_pFrameParam->out.selectDataIdx  = m_activeSelectIdx;

            seq = SEQ_SELECT_ENTRY;
            result  = applib::frame::RES_FINISH;
          }
          else
          {
            //  バージョンエラーで選択できない
            //  もとの選択に戻る
            seq = SEQ_SELECT_START;
          }
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }

    //  複数ビデオデータ選択削除
    if( seq == SEQ_DEL_SELECT_START )
    {
      pLowDataListView->SetState( LowDataListView::STATE_IDLE );

      //  削除選択のモードへ
      pLowDataListView->SetDelMode( LowDataListView::DEL_MODE_SELECT );

      seq = SEQ_DEL_SELECT_WAIT;
    }
    else
    if( seq == SEQ_DEL_SELECT_WAIT )
    {
      //  リスト操作が終ったので一度リストスクロールを出力
      m_pFrameParam->in.listScrollData = pLowDataListView->GetListScrollData();

      if( pLowRootView->IsEndInput() == true )
      {
        //  Bキャンセルを押したので終了
        if( pLowRootView->GetPushBtn() == LowRootView::BUTTON_ID_BACK )
        {
          seq = SEQ_SELECT_START;
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
      else
      if( pLowDataListView->IsEndInput() == true )
      {
        LowDataListView::ButtonId pushButtonId  = pLowDataListView->GetPushBtnId();
        if( pushButtonId == LowDataListView::BUTTON_ID_DATA_CHANGE )
        {
          //  情報を切り替え
          pLowDataListView->SetState( LowDataListView::STATE_IDLE );

          _ChangeInfoState();
        }
        else
        if( pushButtonId == LowDataListView::BUTTON_ID_DATA_DEL )
        {
          //  削除確認へ
          seq = SEQ_DEL_DATA_START;
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }

    // データの削除フロー
    if( seq == SEQ_DEL_DATA_START )
    {
      m_dataDelFlow.Start();

      seq = SEQ_DEL_DATA_WAIT;
    }
    else
    if( seq == SEQ_DEL_DATA_WAIT )
    {
      DataDelFlow::Result delResult  = m_dataDelFlow.Run();
      if( delResult == DataDelFlow::RESULT_RUN )
      {

      }
      else
      if( delResult == DataDelFlow::RESULT_DEL_FATAL )
      {
        //  フェイタル
        m_pFrameParam->out.result = RESULT_DATA_FATAL;
        seq = SEQ_END;
      }
      else
      {
        //  成功・キャンセル・失敗がくる
        if( m_pFrameParam->in.pVideoDataManager->GetListDataNum() <= 0 )
        {
          //  リストが0になったので終了
          m_pFrameParam->out.result = RESULT_NON_DATA;
          seq = SEQ_END;
        }

        else
        {
          //  通常選択に戻る
          seq = SEQ_SELECT_START;
        }
      }
    }

    if( seq == SEQ_END )
    {
      GFL_ASSERT( m_pFrameParam->out.result != RESULT_NONE );
      result  = applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return result;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void DataListFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result DataListFrame::EndFunc(void)
  {
    if( m_endResult == applib::frame::RES_FINISH )
    {
      return  applib::frame::RES_FINISH;
    }
    
    u32 seq = CellSuper::GetSubSeq();

    if( seq == END_ROOT_SEQ_END_FUNC )
    {
      bool  bRet  = true;
      if( m_dataDelFlow.EndFunc() == false )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++seq;
      }
    }
    else
    if( seq == END_ROOT_SEQ_DATA )
    {
      UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;
      pUIViewPool->HideLowerSystemMessage();

      LowDataListView*  pLowDataListView  = pUIViewPool->GetLowDataListView();

      if( m_pFrameParam->out.result == RESULT_CANCEL )
      {
        //  Ｂボタンを押してキャンセルした場合はリスト結果をクリアして最初の位置からはじめる
        m_pFrameParam->out.listScrollData.listPos     = 0;
        m_pFrameParam->out.listScrollData.listScroll  = 0.0f;
      }
      else
      {
      //  リストスクロールを出力
        m_pFrameParam->out.listScrollData = pLowDataListView->GetListScrollData();
      }

      m_endResult = applib::frame::RES_FINISH;
    }

    CellSuper::SetSubSeq( seq );

    return m_endResult;
  }

  //  情報状態設定
  void DataListFrame::_SetInfoState( const UIViewPool::UppInfoViewMode mode )
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    //  プレイヤー情報を表示
    pUIViewPool->SetUppInfoViewState( mode );
  }

  //  情報切り替え
  void DataListFrame::_ChangeInfoState()
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    pUIViewPool->ChangeUppInfoViewState();

    _SetInfoState( pUIViewPool->GetUppInfoViewMode() );
  }

  //  リストのアクティブ位置
  LowDataListViewDelegate::ActiveItemResult DataListFrame::LowDataListViewDelegate_OnActiveCursor( const u32 idx )
  {
    NetAppLib::BattleVideoPlayer::VideoDataManager* pVideoDataManager = m_pFrameParam->in.pVideoDataManager;
    if( pVideoDataManager->GetListDataNum() <= 0 )
    {
      //  データがない場合はスルー
      return  LowDataListViewDelegate::ACTIVE_ITEM_RESULT_INFO_OFF;
    }

    NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData  = pVideoDataManager->GetListData( idx );
    GFL_ASSERT( pVideoData );

    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    //  情報更新
    pUIViewPool->SetVideoDataUppInfoView( pVideoData );
    pUIViewPool->SetVisibleUppInfoView( true );

    m_activeSelectIdx = idx;

    //  エラーデータであればインフォデータはない
    if( pVideoData->bErrorVersionFlg == 1 )
    {
      return  LowDataListViewDelegate::ACTIVE_ITEM_RESULT_INFO_OFF;
    }

    return  LowDataListViewDelegate::ACTIVE_ITEM_RESULT_INFO_ON;
  }

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )
