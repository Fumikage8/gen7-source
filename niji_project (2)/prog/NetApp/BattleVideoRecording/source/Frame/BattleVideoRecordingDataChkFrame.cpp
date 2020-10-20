//=============================================================================
/**
 * @file	 BattleVideoRecordingDataChkFrame.h
 * @brief	 データチェックフレーム
 * @author yuto_uchida
 * @date	 2015.09.15
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoRecording/source/frame/BattleVideoRecordingDataChkFrame.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoRecording/source/BattleVideoRecordingUIViewPool.h"

//  gfl2のインクルード

//  nijiのインクルード
#include "ExtSavedata/include/BattleRecorderSaveData.h"

//  リソースのインクルード

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_rec.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoRecording )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  DataChkFrame::DataChkFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  DataChkFrame::~DataChkFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void DataChkFrame::Initialize( FRAME_PARAM* pFrameParam )
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
  applib::frame::Result DataChkFrame::InitFunc(void)
  {
    //  UIViewの状態を初期化
    {
      UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

      //  下
      {
        LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();
        pLowRootView->SetState( LowRootView::STATE_IDLE );

        pLowRootView->RemoveAllSubView();
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
  applib::frame::Result DataChkFrame::UpdateFunc(void)
  {
    enum
    {
      SEQ_REQ_ENABLE_CHK_START,
      SEQ_REQ_ENABLE_CHK_ERROR,
      SEQ_SERVER_VERSION_CHK_START,
      SEQ_SERVER_VERSION_CHK_ERROR,
      SEQ_END
    };

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    //  録画可能か
    {
      if( seq == SEQ_REQ_ENABLE_CHK_START )
      {
        if( m_pFrameParam->in.bRecNG == true )
        {
          //  録画不可能
          pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_10, pUIViewPool->GetLowRootView(), false, false, false, false );
          seq = SEQ_REQ_ENABLE_CHK_ERROR;
        }
        else
        {
          //  録画可能
          seq = SEQ_SERVER_VERSION_CHK_START;
        }
      }

      if( seq == SEQ_REQ_ENABLE_CHK_ERROR )
      {
        //  録画不可能
        if( pUIViewPool->IsMessageShow() == false )
        {
          seq = SEQ_END;
        }
      }
    }

    //  バージョンチェック
    {
      if( seq == SEQ_SERVER_VERSION_CHK_START )
      {
        if( m_pFrameParam->in.serverVersion != BTL_NET_SERVER_VERSION )
        {
          //  バージョンが違うのでエラーにする
          pUIViewPool->ShowSystemMessageLower( pUIViewPool->GetInitParam().pAppMsgData, msg_bv_rec_win_01, pUIViewPool->GetLowRootView(), false, false, false, false );

          m_pFrameParam->out.result = RESULT_APP_END;

          seq = SEQ_SERVER_VERSION_CHK_ERROR;
        }
        else
        {
          m_pFrameParam->out.result = RESULT_GO_SAVE;
          seq = SEQ_END;
        }
      }

      if( seq == SEQ_SERVER_VERSION_CHK_ERROR )
      {
        if( pUIViewPool->IsMessageShow() == false )
        {
          seq = SEQ_END;
        }
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
  void DataChkFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result DataChkFrame::EndFunc(void)
  {
    return applib::frame::RES_FINISH;
  }

GFL_NAMESPACE_END( BattleVideoRecording )
GFL_NAMESPACE_END( NetApp )
