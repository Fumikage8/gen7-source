//=============================================================================
/**
 * @file	 BattleVideoPlayerDataDelFlow.h
 * @brief	 データ削除の共通フロー
 * @author yuto_uchida
 * @date	 2015.10.02
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoPlayer/source/frame/BattleVideoPlayerDataDelFlow.h"

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
  DataDelFlow::DataDelFlow(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  DataDelFlow::~DataDelFlow()
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief  初期化を行う関数
   */
  //------------------------------------------------------------------
  bool DataDelFlow::InitFunc( const PARAM& rParam )
  {
    m_param = rParam;

    return  true;
  }

  //------------------------------------------------------------------
  /**
   * @brief  開始(削除フラグを設定している場合に呼ぶ 主に選択用)
   */
  //------------------------------------------------------------------
  bool DataDelFlow::Start()
  {
    UIViewPool*         pUIViewPool       = m_param.pUIViewPool;
    pUIViewPool->HideLowerSystemMessage();

    if( m_param.pVideoDataManager->GetDataDelFlgCount() <= 0 )
    {
      //  削除フラグがない
      GFL_ASSERT( 0 );
      m_result  = RESULT_CANCEL;
      return  false;
    }
    else
    {
      m_result  = RESULT_RUN;
    }

    m_updateSeq = SEQ_DEL_CONFIRM_START;
    return  true;
  }

  //------------------------------------------------------------------
  /**
   * @brief  開始(指定したリストデータを削除)
   */
  //------------------------------------------------------------------
  bool DataDelFlow::Start( const NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData )
  {
    NetAppLib::BattleVideoPlayer::VideoDataManager*   pVideoDataManager = m_param.pVideoDataManager;
    //  念のために削除フラグを全クリア
    pVideoDataManager->ClearDataDelFlg();

    //  指定したデータがリストデータのものか取得
    u32 delSelectIdx  = pVideoDataManager->GetVideoDataListIdx( pVideoData );
    if( delSelectIdx == NetAppLib::BattleVideoPlayer::VideoDataManager::LIST_IDX_ERROR )
    {
      return  false;
    }

    //  指定したリストidxに削除フラグをつける
    pVideoDataManager->SetDataDelFlg( delSelectIdx, true );

    return  Start();
  }

  //------------------------------------------------------------------
  /**
   * @brief  更新を行う関数
   */
  //------------------------------------------------------------------
  DataDelFlow::Result DataDelFlow::Run()
  {
    if( m_result != RESULT_RUN )
    {
      return  m_result;
    }

    UIViewPool*         pUIViewPool                                     = m_param.pUIViewPool;
    NetAppLib::BattleVideoPlayer::VideoDataManager*   pVideoDataManager = m_param.pVideoDataManager;

    LowRootView*      pLowRootView      = pUIViewPool->GetLowRootView();

    //  削除確認
    if( m_updateSeq == SEQ_DEL_CONFIRM_START )
    {
      const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();

      gfl2::str::StrBuf*  pMessageStrBuf  = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
      // @fix NMCat[1062] 削除個数によってメッセージを変える
      if( 1 < pVideoDataManager->GetDataDelFlgCount() )
      {
        gfl2::str::StrBuf*  pTmpStrBuf  = pLowRootView->GetG2DUtil()->GetTempStrBuf( 1 );

        rInitParam.pWordSet->RegisterNumber( 0, pVideoDataManager->GetDataDelFlgCount(), 3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
        rInitParam.pAppMsgData->GetString( msg_bvp_win_07, *pTmpStrBuf );

        rInitParam.pWordSet->Expand( pMessageStrBuf, pTmpStrBuf );
      }
      else
      {
        rInitParam.pAppMsgData->GetString( msg_bvp_win_18, *pMessageStrBuf );
      }

      pUIViewPool->ShowMessageWithListMenu(
        /*  ウインドウ設定  */
        pMessageStrBuf,
        false,
        true,

        /*  リスト設定  */
        *rInitParam.pAppMsgData,
        msg_bvp_sys_low_13,
        msg_bvp_sys_low_14
      );

      m_updateSeq = SEQ_DEL_CONFIRM_WAIT;
    }
    else
    if( m_updateSeq == SEQ_DEL_CONFIRM_WAIT )
    {
      if( pUIViewPool->IsEndProcLowerSystemMessage() == true )
      {
        if( ( pUIViewPool->GetLowerSystemMessageCode() == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
        ||  ( pUIViewPool->GetLowerSystemMessageCode() == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo ) )
        {
          if( pUIViewPool->GetLowerSystemMessageCode() == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
          {
            //  削除開始！
            m_updateSeq = SEQ_DEL_DATA_START;
          }
          else
          if( pUIViewPool->GetLowerSystemMessageCode() == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
          {
            //  削除キャンセル
            m_result  = RESULT_CANCEL;
          }
          else
          {
            GFL_ASSERT( 0 );
          }

          pUIViewPool->HideLowerSystemMessage();
        }
      }
    }

    //  ビデオデータ削除
    if( m_updateSeq == SEQ_DEL_DATA_START )
    {
      m_deleteResult  = NetAppLib::BattleVideoPlayer::VideoDataManager::DELETE_RESULT_SUCCESS;

      //  実際の削除処理
      //  ここにきたら削除データがなくて失敗はありえない
      if( pVideoDataManager->StartDeleteVideoData() == false )
      {
        GFL_ASSERT( 0 );
        m_updateSeq = SEQ_DEL_DATA_NOTICE_START;
      }
      else
      {
        //  メッセージ表示
        {
          const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
          pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_08, true, true, true );
        }

        m_updateSeq = SEQ_DEL_DATA_WAIT;
      }
    }
    else
    if( m_updateSeq == SEQ_DEL_DATA_WAIT )
    {
      //  削除終了するまで待つ
      m_deleteResult  = pVideoDataManager->UpdateDeleteVideoData();
      if( m_deleteResult == NetAppLib::BattleVideoPlayer::VideoDataManager::DELETE_RESULT_RUN )
      {
      }
      else
      {
        //  メッセージ表示をクリア
        pUIViewPool->HideLowerSystemMessage();

        m_updateSeq = SEQ_DEL_DATA_NOTICE_START;
      }
    }

    if( m_updateSeq == SEQ_DEL_DATA_NOTICE_START )
    {
      //  削除結果
      if( m_deleteResult == NetAppLib::BattleVideoPlayer::VideoDataManager::DELETE_RESULT_SUCCESS )
      {
        //  メッセージ表示
        {
          const UIViewPool::INIT_PARAM& rInitParam  = pUIViewPool->GetInitParam();
          pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_09, false, false, true );
        }

        m_updateSeq = SEQ_DEL_DATA_NOTICE_WAIT;
      }
      else
      if( m_deleteResult == NetAppLib::BattleVideoPlayer::VideoDataManager::DELETE_RESULT_ERROR )
      {
        //  アプリ内のでのエラー
        m_result  = RESULT_DEL_ERROR;
      }
      else
      if( m_deleteResult == NetAppLib::BattleVideoPlayer::VideoDataManager::DELETE_RESULT_FATAL )
      {
        //  システムのエラー
        m_result  = RESULT_DEL_FATAL;
      }
      else
      {
        GFL_ASSERT( 0 );
      }

    }
    else
    if( m_updateSeq == SEQ_DEL_DATA_NOTICE_WAIT )
    {
      //  即時表示設定だとすぐに EVENT_OnFinish がキックする
      if( pUIViewPool->IsEndProcLowerSystemMessage() )
      {
        //  メッセージを消す
        {
          pUIViewPool->HideLowerSystemMessage();
        }

        m_result  = RESULT_DEL_SUCCESS;
      }
    }

    return m_result;
  }

  //------------------------------------------------------------------
  /**
   * @brief   終了を行う関数
   */
  //------------------------------------------------------------------
  bool DataDelFlow::EndFunc(void)
  {
    UIViewPool* pUIViewPool = m_param.pUIViewPool;
    pUIViewPool->HideLowerSystemMessage();

    return true;
  }

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )
