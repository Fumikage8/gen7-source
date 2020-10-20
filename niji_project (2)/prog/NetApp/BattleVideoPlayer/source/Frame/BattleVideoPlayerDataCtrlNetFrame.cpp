//=============================================================================
/**
 * @file	 BattleVideoPlayerDataCtrlNetFrame.h
 * @brief	 データ操作フレーム(通信データ用)
 * @author yuto_uchida
 * @date	 2015.11.18
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataCtrlNetFrame.h"

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
  DataCtrlNetFrame::DataCtrlNetFrame(void)
  : DataCtrlBaseFrame()
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  DataCtrlNetFrame::~DataCtrlNetFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの初期化を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result DataCtrlNetFrame::InitFunc()
  {
    if( DataCtrlBaseFrame::InitFunc() == applib::frame::RES_CONTINUE )
    {
      return  applib::frame::RES_CONTINUE;
    }

    _RefrashView();

    return  applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result DataCtrlNetFrame::UpdateFunc()
  {
    applib::frame::Result result  = DataCtrlBaseFrame::UpdateFunc();

    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    u32 seq = DataCtrlBaseFrame::GetSubSeq();

    //  データ操作シーケンスが実行して終った
    if( seq == UPDATE_ROOT_SEQ_DATA_CTRL_END )
    {
      //  操作終了
      const UpdataDataCtrlResult& rResult = DataCtrlBaseFrame::GetUpdateDataCtrlResult();
      if( rResult.bFatal == true )
      {
        m_pFrameParam->out.result = DataCtrlBaseFrame::RESULT_FATAL;

        result  = applib::frame::RES_FINISH;
      }
      else
      if( rResult.bCancel == true )
      {
        //  終了
        m_pFrameParam->out.result = DataCtrlBaseFrame::RESULT_CANCEL;
        result  = applib::frame::RES_FINISH;
      }
      else
      {
        //  押したボタンで対処を変える
        if( rResult.pushBtn == LowDataCtrlView::BUTTON_ID_PLAYER )
        {
          //  再生
          m_pFrameParam->out.result = DataCtrlBaseFrame::RESULT_VIDEO_PLAY;
          result  = applib::frame::RES_FINISH;
        }
        else
        if( ( rResult.pushBtn == LowDataCtrlView::BUTTON_ID_SELECT_01 )
        ||  ( rResult.pushBtn == LowDataCtrlView::BUTTON_ID_SELECT_02 )
        ||  ( rResult.pushBtn == LowDataCtrlView::BUTTON_ID_SELECT_03 ) )
        {
          LowDataCtrlView*  pLowDataCtrlView  = pUIViewPool->GetLowDataCtrlView();
          u32 attachId  = pLowDataCtrlView->GetButtonAttachId( rResult.pushBtn );
          if( attachId == BUTTON_ID_VS )
          {
            //  模擬戦
            if( m_pFrameParam->in.pVideoData->bServerDataFlg == 1 )
            {
              //  他人のデータであれば相手を選択
              seq = _StartSeq( UPDATE_SEQ_DUEL_SELECT );
            }
            else
            {
              // @fix GFNMCat[995] 模擬戦相手は敵側にしかならない
              m_pFrameParam->out.dualTargetIdx  = BTL_CLIENT_ENEMY1;
              seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_BTL_TEAM_SELECT_MESSAGE );
            }
          }
          else
          if( attachId == BUTTON_ID_UPLOAD )
          {
            if( m_pFrameParam->in.pVideoData->sdCardData.upload_flag == true )
            {
              //  アップロードキャンセル
              seq = _ShowYesNoMenuMessageSeq( msg_bvp_win_15, UPDATE_SEQ_UPLOAD_CANCEL_START, UPDATE_ROOT_SEQ_DATA_CTRL_START, true );
            }
            else
            {
              //  アップロード
              seq = _ShowYesNoMenuMessageSeq( msg_bvp_win_11, UPDATE_SEQ_UPLOAD_START, UPDATE_ROOT_SEQ_DATA_CTRL_START, true );
            }
          }
          else
          if( attachId == BUTTON_ID_DEL )
          {
            //  削除
            seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_DEL_START );
          }
          else
          {
            GFL_ASSERT( 0 );
          }
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }
    else
    //  データ削除シーケンスが実行して終った
    if( seq == UPDATE_ROOT_SEQ_DATA_DEL_END )
    {
      m_pFrameParam->out.result = DataCtrlBaseFrame::RESULT_VIDEO_DEL;
      result  = applib::frame::RES_FINISH;
    }

    //  アップロード
    if( seq == UPDATE_SEQ_UPLOAD_START )
    {
      m_pFrameParam->in.pVideoDataManager->StartUpload( m_pFrameParam->in.pVideoData );

      seq = UPDATE_SEQ_UPLOAD_WAIT;
    }
    else
    if( seq == UPDATE_SEQ_UPLOAD_WAIT )
    {
      m_uploadResult = m_pFrameParam->in.pVideoDataManager->WaitUpload();
      if( m_uploadResult != NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_RESULT_RUN )
      {
        seq = UPDATE_SEQ_UPLOAD_END;
      }
    }
    else
    if( seq == UPDATE_SEQ_UPLOAD_END )
    {
      NetAppLib::BattleVideoPlayer::VideoDataManager::UploadResult localUploadReault  = m_uploadResult;
      if( localUploadReault == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_RESULT_SUCCESS )
      {
        //  公開成功
        {
          // @fix GFNMCat[1103] 画面表示をアップロードの部分のみ更新
          pUIViewPool->UpdateInfoUploadData();
        }
        seq = _ShowMessageSeq( msg_bvp_win_14, UPDATE_ROOT_SEQ_DATA_CTRL_START, true );
      }
      else
      if( localUploadReault == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_RESULT_NG_DATA )
      {
        //  ビデオデータ公開できない
        seq = _ShowMessageSeq( msg_bvp_win_12, UPDATE_ROOT_SEQ_DATA_CTRL_START, true );
      }
      else
      if( localUploadReault == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_RESULT_OVERLIMIT )
      {
        //  ビデオデータのアップロード公開数を超えた
        seq = _ShowMessageSeq( msg_bvp_win_28, UPDATE_ROOT_SEQ_DATA_CTRL_START, true );
      }
      else
      if( localUploadReault == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_RESULT_ERROR )
      {
        //  その他のエラー
        seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_CTRL_START );
      }
      else
      if( localUploadReault == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_RESULT_FATAL )
      {
        //  アプリ終了
        seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_CTRL_START );
      }
    }

    //  アップロードキャンセル
    if( seq == UPDATE_SEQ_UPLOAD_CANCEL_START )
    {
      m_pFrameParam->in.pVideoDataManager->StartUploadCancel( m_pFrameParam->in.pVideoData );

      seq = UPDATE_SEQ_UPLOAD_CANCEL_WAIT;
    }
    else
    if( seq == UPDATE_SEQ_UPLOAD_CANCEL_WAIT )
    {
      m_uploadCanelResult = m_pFrameParam->in.pVideoDataManager->WaitUploadCancel();
      if( m_uploadCanelResult != NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_CANCEL_RESULT_RUN )
      {
        seq = UPDATE_SEQ_UPLOAD_CANCEL_END;
      }
    }
    else
    if( seq == UPDATE_SEQ_UPLOAD_CANCEL_END )
    {
      NetAppLib::BattleVideoPlayer::VideoDataManager::UploadCancelResult  localUploacCancelResult = m_uploadCanelResult;
      if( localUploacCancelResult == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_CANCEL_RESULT_SUCCESS )
      {
        //  キャンセル成功
        pUIViewPool->UpdateInfoUploadData();

        seq = _ShowMessageSeq( msg_bvp_win_17, UPDATE_ROOT_SEQ_DATA_CTRL_START, true );
      }
      else
      if( localUploacCancelResult == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_CANCEL_RESULT_NOW_UPLOAD_DATA )
      {
        //  今日アップロードしたデータなのでキャンセルできない
        seq = _ShowMessageSeq( msg_bvp_win_16, UPDATE_ROOT_SEQ_DATA_CTRL_START, true );
      }
      else
      if( localUploacCancelResult == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_CANCEL_RESULT_ERROR )
      {
        //  その他エラー
        seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_CTRL_START );
      }
      else
      if( localUploacCancelResult == NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_CANCEL_RESULT_FATAL )
      {
        //  アプリ終了
        seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_CTRL_START );
      }
    }

    //  模擬戦の相手選択
    if( seq == UPDATE_SEQ_DUEL_SELECT )
    {
      if( pUIViewPool->IsEndProcLowerSystemMessage() == true )
      {
        App::Tool::MessageMenuView::IEventHandler::EventCode  messageResult  = pUIViewPool->GetLowerSystemMessageCode();
        if( ( messageResult == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
        ||  ( messageResult == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo ) )
        {
          pUIViewPool->HideLowerSystemMessage();

          //  押したボタンを選択
          u32 selectIdx = pUIViewPool->GetLowerSystemMessageMenuSelectIdx();
          if( ( selectIdx == 0 )
          ||  ( selectIdx == 1 ) )
          {
            m_pFrameParam->out.dualTargetIdx  = selectIdx;
            seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_BTL_TEAM_SELECT_MESSAGE );
          }
          else
          {
            //  戻る
            seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_CTRL_START );
          }
        }
      }
    }

    DataCtrlBaseFrame::SetSubSeq( seq );

    if( result == applib::frame::RES_FINISH )
    {
      //  終了時には結果を設定しないとアウト
      GFL_ASSERT( m_pFrameParam->out.result != DataCtrlBaseFrame::RESULT_NONE );
    }

    return  result;
  }

  //  シーケンス設定
  u32 DataCtrlNetFrame::_StartSeq( const u32 seq )
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    u32 newSeq  = DataCtrlBaseFrame::_StartSeq( seq );

    if( seq == UPDATE_ROOT_SEQ_DATA_CTRL_START )
    {
      _RefrashView();
    }
    else
    if( newSeq == UPDATE_SEQ_UPLOAD_START )
    {
      //  通信メッセージを出す
      const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
      m_pFrameParam->in.pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_13, true, true, true );
    }
    else
    if( newSeq == UPDATE_SEQ_UPLOAD_CANCEL_START )
    {
      //  通信メッセージを出す
      const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
      m_pFrameParam->in.pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_25, true, true, true );
    }
    else
    if( newSeq == UPDATE_SEQ_DUEL_SELECT )
    {
      //  模擬戦相手の選択
      m_pFrameParam->in.pUIViewPool->ShowMessageWithListMenuByDual( m_pFrameParam->in.pVideoData );
    }

    return  newSeq;
  }

  //  画面更新
  void DataCtrlNetFrame::_RefrashView()
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    //  データ操作View
    {
      LowDataCtrlView*  pLowDataCtrlView  = pUIViewPool->GetLowDataCtrlView();

      bool  bVsBtl  = m_pFrameParam->in.pVideoData->IsVsBtl();

      if( m_pFrameParam->in.pVideoData->bServerDataFlg == 1 )
      {
        //  他人のデータ
        //  再生と対戦相手のボタンのみ
        if( bVsBtl == true )
        {
          //  模擬戦できる
          pLowDataCtrlView->SetContentButton( LowDataCtrlView::BUTTON_ID_SELECT_01, LowDataCtrlView::BUTTON_CONTENT_VS, BUTTON_ID_VS );
        }
      }
      else
      {
        //  アップロード対象のデータ
        //  再生と対戦相手とアップロードと削除ボタン

        LowDataCtrlView::ButtonContent  uploadContent = LowDataCtrlView::BUTTON_CONTENT_CANCEL_UPLOAD;
        if( m_pFrameParam->in.pVideoData->sdCardData.upload_flag == false )
        {
          uploadContent = LowDataCtrlView::BUTTON_CONTENT_GO_UPLOAD;
        }

        if( bVsBtl == true )
        {
          //  模擬戦できる
          pLowDataCtrlView->SetContentButton( LowDataCtrlView::BUTTON_ID_SELECT_01, LowDataCtrlView::BUTTON_CONTENT_VS, BUTTON_ID_VS );

          pLowDataCtrlView->SetContentButton( LowDataCtrlView::BUTTON_ID_SELECT_02, uploadContent, BUTTON_ID_UPLOAD );
        }
        else
        {
          pLowDataCtrlView->SetContentButton( LowDataCtrlView::BUTTON_ID_SELECT_01, uploadContent, BUTTON_ID_UPLOAD );
        }

        pLowDataCtrlView->SetContentButton( LowDataCtrlView::BUTTON_ID_SELECT_03, LowDataCtrlView::BUTTON_CONTENT_DEL, BUTTON_ID_DEL );
      }
    }
  }

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )
