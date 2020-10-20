//=============================================================================
/**
 * @file	 BattleVideoPlayerSDCardChkFrame.h
 * @brief	 データ保存フレーム
 * @author yuto_uchida
 * @date	 2015.09.15
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoPlayer/source/frame/BattleVideoPlayerSDCardChkFrame.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

//  gfl2のインクルード

//  nijiのインクルード

//  リソースのインクルード

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

//  サウンドのインクルード
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  SDCardChkFrame::SDCardChkFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  SDCardChkFrame::~SDCardChkFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void SDCardChkFrame::Initialize( FRAME_PARAM* pFrameParam )
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
  applib::frame::Result SDCardChkFrame::InitFunc(void)
  {
    GFL_ASSERT( m_pFrameParam->in.pUIViewPool );
    GFL_ASSERT( m_pFrameParam->in.pVideoDataManager );

    //  UIViewの状態を初期化
    {
      UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

      const UIViewPool::INIT_PARAM& rUiViewPoolInitParam = pUIViewPool->GetInitParam();

      gfl2::str::MsgData* pAppMsgData = rUiViewPoolInitParam.pAppMsgData;

      //  下
      {
        LowRootView*  pLowRootView  = pUIViewPool->GetLowRootView();
        pLowRootView->SetState( LowRootView::STATE_IDLE );

        pLowRootView->RemoveAllSubView();

        //  SDカード確認メッセージ
        {
          const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
          pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_03, true, true, false );
        }

        //  Bボタンを非表示
        pLowRootView->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, false );
      }

      //  上
      {
        UppRootView*  pUppRootView  = pUIViewPool->GetUppRootView();
        pUppRootView->SetState( UppRootView::STATE_IDLE );

        pUppRootView->RemoveAllSubView();
        pUIViewPool->RemoveUppRootInfoView();

        gfl2::str::StrBuf*  pTempStrBuf = pUppRootView->GetG2DUtil()->GetTempStrBuf( 0 );

        //  タイトル表記
        {
          pAppMsgData->GetString( msg_bvp_sys_up_01, *pTempStrBuf );
          pUppRootView->ShowMessageUpperOneLine( pTempStrBuf );
        }

        pUppRootView->HideMessageCenter();
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
  applib::frame::Result SDCardChkFrame::UpdateFunc(void)
  {
    enum
    {
      SEQ_INIT_VIDEO  = 0,
      SEQ_LOAD_VIDEO,
      SEQ_SHOW_MESSAGE,
      SEQ_INIT_ONLINE,
      SEQ_LOAD_ONLINE,
      SEQ_END
    };

    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    //  ローカル・オンラインでの処理
    if( seq == SEQ_INIT_VIDEO )
    {
      //  ビデオデータを初期化
      m_pFrameParam->in.pVideoDataManager->InitVideoData();

      ++seq;
    }

    if( seq == SEQ_LOAD_VIDEO )
    {
      NetAppLib::BattleVideoPlayer::VideoDataManager::LoadResult  result  = m_pFrameParam->in.pVideoDataManager->LoadVideoData();
      if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::LOAD_RESULT_RUN )
      {
        //  まだロード中
      }
      else
      if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::LOAD_RESULT_SUCCESS )
      {
        //  ロード終了
        if( m_pFrameParam->in.pVideoDataManager->IsDeleteBrokeData() == true )
        {
          //  破損ファイルを削除
          const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
          pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_04, false, false, false );

          m_sysMessageEndNextSeq  = SEQ_INIT_ONLINE;
          seq = SEQ_SHOW_MESSAGE;
        }
        else
        {
          seq = SEQ_INIT_ONLINE;
        }

        m_pFrameParam->out.result  = RESULT_SUCCESS;
      }
      else
      if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::LOAD_RESULT_FATAL )
      {
        //  失敗したのでアプリ終了通知
        m_pFrameParam->out.result  = RESULT_FATAL;
        seq = SEQ_END;
      }
      else
      {
        GFL_ASSERT( 0 );
      }
    }

    if( seq == SEQ_SHOW_MESSAGE )
    {
      if( pUIViewPool->IsEndProcLowerSystemMessage() == true )
      {
        seq = m_sysMessageEndNextSeq;
      }
    }

    //  オンラインでの処理
    if( seq == SEQ_INIT_ONLINE )
    {
      if( m_pFrameParam->in.pVideoDataManager->InitOnline() == false )
      {
        //  オンラインでないので終了
        seq = SEQ_END;
      }
      else
      {
        //  メッセージ表示(通信中)
        {
          const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
          pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_27, true, true, false );
        }

        seq = SEQ_LOAD_ONLINE;
      }
    }

    if( seq == SEQ_LOAD_ONLINE )
    {
      NetAppLib::BattleVideoPlayer::VideoDataManager::LoadResult  result  = m_pFrameParam->in.pVideoDataManager->LoadOnline();
      if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::LOAD_RESULT_SUCCESS )
      {
        //  オンライン処理成功
        const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
        pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_06, false, false, false );

        //  完了SE
        Sound::PlaySE( SEQ_SE_COMPLETE1 );

        m_pFrameParam->out.result  = RESULT_SUCCESS;

        m_sysMessageEndNextSeq  = SEQ_END;
        seq = SEQ_SHOW_MESSAGE;
      }
      else
      if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::LOAD_RESULT_FATAL )
      {
        pUIViewPool->HideLowerSystemMessage();

        //  失敗したのでアプリ終了通知(フェイタル終了)
        m_pFrameParam->out.result  = RESULT_FATAL;
        seq = SEQ_END;
      }
      else
      if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::LOAD_RESULT_ERROR )
      {
        pUIViewPool->HideLowerSystemMessage();

        //  失敗したのでアプリ終了通知(通常終了)
        m_pFrameParam->out.result  = RESULT_ERROR;
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
  void SDCardChkFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result SDCardChkFrame::EndFunc(void)
  {
    UIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;
    pUIViewPool->HideLowerSystemMessage();

    return applib::frame::RES_FINISH;
  }

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )
