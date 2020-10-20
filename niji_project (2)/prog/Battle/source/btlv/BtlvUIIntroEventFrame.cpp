//======================================================================
/**
 * @file BtlvUIIntroEventFrame.cpp
 * @date 2015/08/24 17:52:26
 * @author uchida_yuto
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#pragma once

#include "BtlvUIIntroEventFrame.h"
#include "Battle/source/btlv/app/BattleUIDataManager.h"
#include "Battle/source/btlv/app/BattleUIIntroEventLowerDraw.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

#include <system/include/HeapDefine.h>

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUIIntroEventFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUIIntroEventFrame::BattleUIIntroEventFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUIIntroEventFrame::~BattleUIIntroEventFrame()
  {
  }

  void BattleUIIntroEventFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIIntroEventFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIIntroEventFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPめもりーから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUIIntroEventFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

    // ローカルヒープを作る
	  m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.pParentAppHeap, m_pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIIntroEventFrame::InitFunc(void)
  {
    GFL_ASSERT( m_pAppHeap );
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in_param.pRenderingManager );
    GFL_ASSERT( m_pAppParam->in_param.pAsyncFileManager );

    gfl2::fs::AsyncFileManager* pAsyncFileMan = m_pAppParam->in_param.pAsyncFileManager;

    enum
    {
      SEQ_OPEN_FILE       = 0,
      SEQ_WAIT_FILE_OPEN,

      SEQ_LOAD_FILE,
      SEQ_WAIT_FILE_LOAD,

      SEQ_CLOSE_FILE,
      SEQ_WAIT_FILE_CLOSE,

      SEQ_END
    };

    int seq = applib::frame::CellSuper::GetSubSeq();

    //  ロードステートでないとarcファイル処理はしない
    if( m_state == STATE_TYPE_LOAD )
    {
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
        if(  pAsyncFileMan->IsArcFileOpenFinished( s_arcId ) )
        {
          ++seq;
        }
      }

      if( seq == SEQ_LOAD_FILE )
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

        req.arcId             = s_arcId;
        req.datId             = GARC_battleLowwerMenu_intlow_APPLYT;
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
        if(pAsyncFileMan->IsArcFileCloseFinished( s_arcId ) )
        {
          //  ロードを終了して更新処理へ
          return applib::frame::RES_FINISH;
        }
      }
    }
    else
    {
      GFL_ASSERT( 0 );
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
  applib::frame::Result BattleUIIntroEventFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      //  画面クラスを作成
      {
        app::BattleUIIntroEventLowerDraw::INIT_PARAM  initParam;
        initParam.pUtilHeap         = m_pAppHeap;
        initParam.pRenderingManager = m_pAppParam->in_param.pRenderingManager;
        initParam.pLytBuff          = m_pArcReadBuff;

        m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUIIntroEventLowerDraw( initParam );
        m_pDraw->SetVisible( true );
      }

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIIntroEventFrame(System) Create View\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIIntroEventFrame(Devcie) Create View\n" );
#endif
      _SetState( STATE_TYPE_IDLE );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_startDelayFrame <= 0 )
      {
        m_pDraw->Start();
        _SetState( STATE_TYPE_UPDATE );
      }
      else
      {
        --m_startDelayFrame;
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
      if( m_pDraw->GetState() == app::BattleUIIntroEventLowerDraw::STATE_TYPE_END )
      {
        _SetState( STATE_TYPE_CLOSE );
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      _SetState( STATE_TYPE_END );
    }

    if( m_state == STATE_TYPE_END )
    {
      //  入力結果を出力
      m_pAppParam->out_param.bEnd                 = true;

      return applib::frame::RES_FINISH;
    }

    m_pDraw->UpdateTree();

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void BattleUIIntroEventFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {
    if( no == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      if( m_pDraw != NULL )
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
  applib::frame::Result BattleUIIntroEventFrame::EndFunc(void)
  {
    if( m_pDraw->IsDrawing() == false )
    {
      GFL_SAFE_DELETE( m_pDraw );
      GflHeapSafeFreeMemory( m_pArcReadBuff );
      GFL_SAFE_DELETE( m_pAppHeap );

      gfl2::heap::Manager::DeleteHeap( m_pManagerHeapBase );

  #if GFL_DEBUG_PRINT
      m_pAppParam->in_param.pAsyncFileManager->PrintArcFileList();
  #endif

      return applib::frame::RES_FINISH;
    }

    return  applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief 処理の待機から実行開始
  */
  //------------------------------------------------------------------
  void BattleUIIntroEventFrame::Start( const u32 delayFrame )
  {
    if( m_state == STATE_TYPE_IDLE )
    {
      _SetState( STATE_TYPE_OPEN );
      m_startDelayFrame = delayFrame;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUIIntroEventFrame::_SetState( const BattleUIIntroEventFrame::StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
    }
  }

GFL_NAMESPACE_END( btl )
