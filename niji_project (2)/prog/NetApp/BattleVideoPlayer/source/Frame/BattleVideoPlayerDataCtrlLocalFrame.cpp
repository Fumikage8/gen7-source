//=============================================================================
/**
 * @file	 BattleVideoPlayerDataCtrlLocalFrame.h
 * @brief	 データ操作フレーム(ローカルデータ機能)
 * @author yuto_uchida
 * @date	 2015.10.01
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataCtrlLocalFrame.h"

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
  DataCtrlLocalFrame::DataCtrlLocalFrame(void)
  : DataCtrlBaseFrame()
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  DataCtrlLocalFrame::~DataCtrlLocalFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの初期化を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result DataCtrlLocalFrame::InitFunc()
  {
    if( DataCtrlBaseFrame::InitFunc() == applib::frame::RES_CONTINUE )
    {
      return  applib::frame::RES_CONTINUE;
    }

    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    //  データ操作View
    {
      LowDataCtrlView*  pLowDataCtrlView  = pUIViewPool->GetLowDataCtrlView();

      if( m_pFrameParam->in.pVideoData->IsVsBtl() == true )
      {
        //  模擬戦できる
        pLowDataCtrlView->SetContentButton( LowDataCtrlView::BUTTON_ID_SELECT_01, LowDataCtrlView::BUTTON_CONTENT_VS, BUTTON_ID_VS );

        //  削除ボタン
        pLowDataCtrlView->SetContentButton( LowDataCtrlView::BUTTON_ID_SELECT_03, LowDataCtrlView::BUTTON_CONTENT_DEL, BUTTON_ID_DEL );
      }
      else
      {
        //  削除ボタン
        pLowDataCtrlView->SetContentButton( LowDataCtrlView::BUTTON_ID_SELECT_01, LowDataCtrlView::BUTTON_CONTENT_DEL, BUTTON_ID_DEL );
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
  applib::frame::Result DataCtrlLocalFrame::UpdateFunc()
  {
    applib::frame::Result result  = DataCtrlBaseFrame::UpdateFunc();

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
          UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

          LowDataCtrlView*  pLowDataCtrlView  = pUIViewPool->GetLowDataCtrlView();
          u32 attachId  = pLowDataCtrlView->GetButtonAttachId( rResult.pushBtn );

          if( attachId == BUTTON_ID_VS )
          {
            //  模擬戦
            seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_BTL_TEAM_SELECT_MESSAGE );
          }
          else
          if( attachId == BUTTON_ID_DEL )
          {
            //  削除
            seq = _StartSeq( UPDATE_ROOT_SEQ_DATA_DEL_START );
          }
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }

    //  データ削除シーケンスが実行して終った
    if( seq == UPDATE_ROOT_SEQ_DATA_DEL_END )
    {      
      m_pFrameParam->out.result = DataCtrlBaseFrame::RESULT_VIDEO_DEL;
      result  = applib::frame::RES_FINISH;
    }

    DataCtrlBaseFrame::SetSubSeq( seq );

    if( result == applib::frame::RES_FINISH )
    {
      //  終了時には結果を設定しないとアウト
      GFL_ASSERT( m_pFrameParam->out.result != DataCtrlBaseFrame::RESULT_NONE );
    }

    return  result;
  }

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )
