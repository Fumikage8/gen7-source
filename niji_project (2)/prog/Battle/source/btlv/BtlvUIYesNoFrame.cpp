//=============================================================================
/**
 * @file	 BtlvUIYesNoFrame.h
 * @brief	 バトルUI2選択画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.05
 */
//=============================================================================
#pragma once

#include "BtlvUIYesNoFrame.h"

//  nijiのインクルード
#include "AppLib/include/Util/app_util_KeyRepeatController.h"
#include "system/include/HeapDefine.h"

//  リソースのインクルード
#include <Sound/include/Sound.h>
#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUIYesNoFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUIYesNoFrame::BattleUIYesNoFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUIYesNoFrame::~BattleUIYesNoFrame()
  {
  }

  void BattleUIYesNoFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.p_parent_app_heap->PrintHeapInfo( "BattleUIYesNoFrame(System) Initialize\n" );
      m_pAppParam->in_param.p_parent_dev_heap->PrintHeapInfo( "BattleUIYesNoFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPめもりーから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.p_parent_app_heap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUIYesNoFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

    // ローカルヒープを作る
    m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.p_parent_app_heap, m_pAppParam->in_param.p_parent_dev_heap, SIZE_MY_APP, SIZE_MY_APPDEV );

    m_pSelectListYKeyRepertCtrl = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) ::app::util::KeyRepeatController( SELECT_TYPE_MAX - 1, 0 );

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIYesNoFrame::InitFunc(void)
  {
    GFL_ASSERT( m_pAppHeap );
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in_param.p_rendering_manager );
    GFL_ASSERT( m_pAppParam->in_param.p_async_file_manager );

    gfl2::fs::AsyncFileManager* pAsyncFileMan = m_pAppParam->in_param.p_async_file_manager;

    enum
    {
      SEQ_OPEN_FILE       = 0,
      SEQ_WAIT_FILE_OPEN,

      SEQ_LOAD_FILE,
      SEQ_WAIT_FILE_LOAD,

      SEQ_CLOSE_FILE,
      SEQ_WAIT_FILE_CLOSE,

      SEQ_CREATE_VIEW,
      SEQ_CREATE_WAIT_VIEW,

      SEQ_END
    };

    int seq = applib::frame::CellSuper::GetSubSeq();

    ///読み込み開始をここに記述
    if( seq == SEQ_OPEN_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
      oreq.arcId       = s_arcId;
      oreq.heapForFile = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      oreq.heapForReq  = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileMan->AddArcFileOpenReq(oreq);

      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_OPEN )
    {
      if(  pAsyncFileMan->IsArcFileOpenFinished( s_arcId ))
      {
        ++seq;
      }
    }

    if( seq == SEQ_LOAD_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId             = s_arcId;
      req.datId             = GARC_battleLowwerMenu_ysnlow_APPLYT;
      req.ppBuf             = &m_pArcReadBuff;
      req.heapForBuf        = m_pAppHeap->GetDeviceHeap();
      req.heapForReq        = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      req.heapForCompressed = NULL;
      req.align             = 128;

      pAsyncFileMan->AddArcFileLoadDataReq(req);

      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_LOAD )
    {
      if(pAsyncFileMan->IsArcFileLoadDataFinished( &m_pArcReadBuff ) )
      {
        ++seq;
      }
    }

    if( seq == SEQ_CLOSE_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
      creq.arcId       = s_arcId;
      creq.heapForReq  = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileMan->AddArcFileCloseReq(creq);

      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_CLOSE )
    {
      if( pAsyncFileMan->IsArcFileCloseFinished( s_arcId ) )
      {
        ++seq;
      }
    }

    if( seq == SEQ_CREATE_VIEW )
    {
      if( m_bEnd == false )
      //  画面クラスを作成
      {
        app::BattleUIYesNoLowerDraw::INIT_PARAM initParam;
        {
          initParam.pDelegate             = this;
          initParam.pLytBuff              = m_pArcReadBuff;
          initParam.pRenderingManager     = m_pAppParam->in_param.p_rendering_manager;
          initParam.pUtilHeap             = m_pAppHeap;
          initParam.pMenuCursorLytBuff    = m_pAppParam->in_param.p_menu_cursor_lyt_buffer;

          //  フレームを使う側でメッセージ関連の設定をしてもらう
          initParam.pBtlAppMsgData        = m_pAppParam->in_param.p_btl_app_msg_data;
          initParam.pBtlWordSet           = m_pAppParam->in_param.p_btl_word_set;
          initParam.aItemTextIdList[ 0 ]  = m_pAppParam->in_param.a_select_item_text_id_list[ 0 ];
          initParam.aItemTextIdList[ 1 ]  = m_pAppParam->in_param.a_select_item_text_id_list[ 1 ];
          initParam.bUseInputSE           = m_pAppParam->in_param.b_use_input_se;
          initParam.selectItemIdx         = (s32)m_pAppParam->in_param.first_select_type;
        }

        m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUIYesNoLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );

#ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        m_pAppParam->in_param.p_parent_app_heap->PrintHeapInfo( "BattleUIYesNoFrame(System) Create View\n" );
        m_pAppParam->in_param.p_parent_dev_heap->PrintHeapInfo( "BattleUIYesNoFrame(Devcie) Create View\n" );
#endif
      }

      ++seq;
    }

    if( seq == SEQ_CREATE_WAIT_VIEW )
    {
      if( ( m_pDraw == NULL ) || ( m_pDraw->InitFunc() == true ) )
      {
        ++seq;
      }
    }

    if( seq == SEQ_END )
    {
      //  ロードを終了して更新処理へ
      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIYesNoFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw->GetState() == app::BattleUIYesNoLowerDraw::STATE_TYPE_INPUT_IDLE )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      //  入力したボタンアニメ終了を待つ
      if( m_pDraw->IsButtonAnimation() == false )
      {
        // 閉じる
        _SetState( STATE_TYPE_CLOSE );
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      if( m_pDraw->GetState() == app::BattleUIYesNoLowerDraw::STATE_TYPE_END )
      {
        _SetState( STATE_TYPE_END );
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      return applib::frame::RES_FINISH;
    }

    GFL_ASSERT( m_pDraw );
    m_pDraw->UpdateTree();

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void BattleUIYesNoFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {
    if( no == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      if( m_pDraw )
      {
        m_pDraw->DrawTree( no );
      }
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIYesNoFrame::EndFunc(void)
  {
    enum
    {
      SEQ_WAIT  = 0,
      SEQ_END
    };

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == SEQ_WAIT )
    {
      if( ( m_pDraw == NULL ) || ( m_pDraw->EndFunc() == true ) )
      {
        ++seq;
      }
    }

    if( seq == SEQ_END )
    {
      //  入力結果を出力
      m_pAppParam->out_param.resultSelectItemIdx = (SelectType)m_selectItemIdx;
      m_pAppParam->out_param.bEnd                 = true;

      GFL_SAFE_DELETE( m_pSelectListYKeyRepertCtrl );
      GFL_SAFE_DELETE( m_pDraw );

      GflHeapSafeFreeMemory( m_pArcReadBuff );
      GFL_SAFE_DELETE( m_pAppHeap );

      gfl2::heap::Manager::DeleteHeap( m_pManagerHeapBase );

#if GFL_DEBUG_PRINT
      m_pAppParam->in_param.p_async_file_manager->PrintArcFileList();
#endif

      return  applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief 終了通知
    */
  //------------------------------------------------------------------
  void BattleUIYesNoFrame::End()
  {
    if( m_bEnd == false )
    {
      if( m_state <= STATE_TYPE_OPEN )
      {
        //  まだロード中なのでいきなり殺す
        _SetState( STATE_TYPE_END );
      }
      else
      {
        _SetState( STATE_TYPE_CLOSE );
      }

      m_bEnd  = true;
    }
    else
    {
      // @fix NMCat[3418] フレーム破棄開始関数を連続で呼ぶ場合にアサートを出しているが、制限時間ではタイミングによっては破棄関数が連続で呼ばれるケースを失念していた。
      //                  また破棄関数を連続で呼んでも問題ない作りになっているのでアサートから警告に格下げする。

      //      GFL_ASSERT( 0 );
      UCHIDA_PRINT( "warning: continuing BattleUIYesNoFrame::End\n" );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 選択した選択項目
  */
  //------------------------------------------------------------------
  bool BattleUIYesNoFrame::OnPushSelectItem( const u32 in_itemIdx )
  {
    //  更新以外では受け取らない
    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    //  入力した項目をとっておく
    m_selectItemIdx = in_itemIdx;

    //  入力したボタンの処理待ちをする（ボタンアニメなどの終了を待つ）
    _SetState( STATE_TYPE_ENTER_EVENT );

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief キーとスティックの入力結果
  */
  //------------------------------------------------------------------
  void BattleUIYesNoFrame::OnKeyAction( app::BattleUIYesNoLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    GFL_ASSERT( pDrawInst );
    GFL_ASSERT( pKey );
    GFL_ASSERT( pStick );

    //  更新以外では受け取らない
    if( m_state != STATE_TYPE_UPDATE )
    {
      return;
    }

    //  Bボタンを入力で終了
    {
      if( m_pAppParam->in_param.b_use_b_button == true )
      {
        if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
        {
          //  入力していないと表示
          pDrawInst->PushActiveSelectItem( 1 );
          return;
        }
      }
    }

    //  決定ボタンを押した
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        //  現在activeになっている項目を押していることに
        pDrawInst->PushActiveSelectItem( pDrawInst->GetActiveSelectItemIdx() );
        return;
      }
    }

    //  キーでカーソル移動
    {
      m_pSelectListYKeyRepertCtrl->UpdateInfo( pKey, pDrawInst->GetActiveSelectItemIdx(), 0 );

      bool  bChangeCursor = false;
      if( m_pSelectListYKeyRepertCtrl->IsRepeat( gfl2::ui::DIR_UP ) )
      {
        if( pDrawInst->MoveUpCursor() == true )
        {
          bChangeCursor = true;
        }
      }
      else
      if( m_pSelectListYKeyRepertCtrl->IsRepeat( gfl2::ui::DIR_DOWN ) )
      {
        if( pDrawInst->MoveDownCursor() == true )
        {
          bChangeCursor = true;
        }
      }

      if( bChangeCursor == true )
      {
        if( m_pAppParam->in_param.b_use_input_se == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }

        return;
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUIYesNoFrame::_SetState( const BattleUIYesNoFrame::StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw != NULL )
      {
        //  開始
        m_pDraw->Start();

        m_pDraw->SetInputEnabled( false );
        m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
      }
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pDraw->SetInputEnabled( m_pAppParam->in_param.b_user_input );
    }
    else
    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      m_pDraw->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      if( m_pDraw != NULL )
      {
        m_pDraw->SetInputEnabled( false );

        //  終了
        m_pDraw->End();
      }
    }
  }

GFL_NAMESPACE_END( btl )
