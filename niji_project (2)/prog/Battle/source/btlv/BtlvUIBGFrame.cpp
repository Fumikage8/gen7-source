//=============================================================================
/**
 * @file	 BtlvUIBGFrame.h
 * @brief	 バトルUIBG画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.23
 */
//=============================================================================
#pragma once

#include "BtlvUIBGFrame.h"
#include "Battle/source/btlv/app/BattleUIDataManager.h"
#include "Battle/source/btlv/app/BattleUIBGFrontLowerDraw.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

#include "niji_conv_header/app/battle/res2d/bgrlow_pane.h"

//  バトルのappテキスト
#include "niji_conv_header/message/msg_btl_app.h"

#include <system/include/HeapDefine.h>

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUIBGFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUIBGFrame::BattleUIBGFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUIBGFrame::~BattleUIBGFrame()
  {
  }

  void BattleUIBGFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
          // ヒープ残量チェック
          m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIBGFrame(System) \n" );
          m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIBGFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPめもりーから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUIBgFrame" );

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
  applib::frame::Result BattleUIBGFrame::InitFunc(void)
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

      SEQ_CREATE_VIEW,
      SEQ_CREATE_WAIT_VIEW,

      SEQ_END
    };

    int seq = applib::frame::CellSuper::GetSubSeq();

    //  ロードステートでないとarcファイル処理はしない
    if( m_state == STATE_TYPE_LOAD )
    {
      ///読み込み開始をここに記述
      if( seq == SEQ_OPEN_FILE )
      {
        //  BG奥のレイアウト読み込み
        {
          gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
          oreq.arcId       = s_arcId;
          oreq.heapForFile = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
          oreq.heapForReq  = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
          pAsyncFileMan->AddArcFileOpenReq(oreq);
        }

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
        //  BG奥のレイアウトファイルロード
        {
          gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

          req.arcId             = s_arcId;
          req.datId             = GARC_battleLowwerMenu_bgrlow_APPLYT;
          req.ppBuf             = &m_pArcReadBuff;
          req.heapForBuf        = m_pAppHeap->GetDeviceHeap();
          req.heapForReq        = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
          req.heapForCompressed = NULL;
          req.align             = 128;

          pAsyncFileMan->AddArcFileLoadDataReq(req);
        }

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
          ++seq;
        }
      }

      if( seq == SEQ_CREATE_VIEW )
      {
        //  BG画面クラスを作成
        {
          //  画面クラスに必要な初期化パラメータ設定
          {
            m_drawInitParam.pLytBuff          = m_pArcReadBuff;
            m_drawInitParam.pRenderingManager = m_pAppParam->in_param.pRenderingManager;
            m_drawInitParam.pUtilHeap         = m_pAppHeap;
            m_drawInitParam.pDelegate         = this;
            m_drawInitParam.pBtlAppMsgData    = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();
            m_drawInitParam.pBtlWordSet       = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
          }

          m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUIBGLowerDraw( m_drawInitParam );
          GFL_ASSERT( m_pDraw );

          m_pDraw->SetInputEnabled( false );
        }

        //  BG手前画面クラスを作成
        {
          app::BattleUIBGFrontLowerDraw::INIT_PARAM initParam;
          initParam.pAsyncFileManager = m_pAppParam->in_param.pAsyncFileManager;
          initParam.pBtlAppMsgData    = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();
          initParam.pBtlWordSet       = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
          initParam.pRenderinManager  = m_pAppParam->in_param.pRenderingManager;
          initParam.pHeap             = m_pAppHeap;
          initParam.pLytBinData       = m_pArcReadBuff;

          m_pFrontDraw  = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUIBGFrontLowerDraw( initParam );
        }

        ++seq;
      }

      if( seq == SEQ_CREATE_WAIT_VIEW )
      {
        if( m_pFrontDraw->InitFunc() == true )
        {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
          // ヒープ残量チェック
          m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIBGFrame(System) Create View\n" );
          m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIBGFrame(Devcie) Create View\n" );
#endif
          //  BG奥のViewに手前Viewをくっつける
          m_pDraw->AddSubView( m_pFrontDraw );

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
  applib::frame::Result BattleUIBGFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_WAITING );
    }

    if( m_state == STATE_TYPE_WAITING )
    {
      if( m_bOpen )
      {
        _SetState( STATE_TYPE_OPEN );
      }
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw->GetState() == app::BattleUIBGLowerDraw::STATE_TYPE_INPUT_IDLE )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      if( m_pDraw->GetState() == app::BattleUIBGLowerDraw::STATE_TYPE_END )
      {
        _SetState( STATE_TYPE_END );
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      //  入力結果を出力
      m_pAppParam->out_param.bEnd                 = true;

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
  void BattleUIBGFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
  applib::frame::Result BattleUIBGFrame::EndFunc(void)
  {
    enum
    {
      SEQ_REMOVE_VIEW  = 0,
      SEQ_END_VIEW,
      SEQ_END
    };

    m_pAppParam->out_param.bEnd                 = true;

    int seq = applib::frame::CellSuper::GetSubSeq();
    if( seq == SEQ_REMOVE_VIEW )
    {
      m_pFrontDraw->RemoveFromSuperView();
      ++seq;
    }

    if( seq == SEQ_END_VIEW )
    {
      bool  bRet  = true;
      if( m_pFrontDraw->EndFunc() == false )
      {
        bRet  = false;
      }

      if( m_pDraw->IsDrawing() == true )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++seq;
      }
    }

    if( seq == SEQ_END )
    {
      GFL_SAFE_DELETE( m_pFrontDraw );
      GFL_SAFE_DELETE( m_pDraw );

      GflHeapSafeFreeMemory( m_pArcReadBuff );
      GFL_SAFE_DELETE( m_pAppHeap );

      gfl2::heap::Manager::DeleteHeap( m_pManagerHeapBase );

  #if GFL_DEBUG_PRINT
      m_pAppParam->in_param.pAsyncFileManager->PrintArcFileList();
  #endif

      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief 開始通知
    * @return true -> 成功 / false -> 失敗
    */
  //------------------------------------------------------------------
  bool BattleUIBGFrame::Start()
  {
    //  待機中のステートでないと処理しない
    if( m_state == STATE_TYPE_LOAD )
    {
      m_bOpen = true;

      return  true;
    }
    else
    if( m_state != STATE_TYPE_WAITING )
    {
      //  すでに開始しているので終了
      return  false;
    }

    _SetState( STATE_TYPE_OPEN );

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief 終了通知
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::End()
  {
    //  画面を終了させる
    _SetState( STATE_TYPE_CLOSE );
  }

  //------------------------------------------------------------------
  /**
    * @brief 強制破棄通知
    * @note
    *  画面が閉じる演出もいきなりインスタンスが破棄
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::Die()
  {
    GFL_ASSERT( m_state != STATE_TYPE_LOAD );

    //  入力結果を出力
    m_pAppParam->out_param.bEnd                 = true;

    _SetState( STATE_TYPE_END );
  }

  //------------------------------------------------------------------
  /**
    * @brief 天候設定
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::SetWeather( const BtlWeather weather )
  {
    m_drawInitParam.weatherType  = weather;
    if( m_pDraw != NULL )
    {
      m_pDraw->SetWeather( weather );
    }

    if( m_pFrontDraw != NULL )
    {
      m_pFrontDraw->SetWeather( m_drawInitParam.weatherType );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ルール設定
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::SetRule( const BtlRule rule )
  {
    m_drawInitParam.modeType = app::BattleUIBGLowerDraw::MODE_TYPE_NORMAL;

    ///< シングル
    if( rule == BTL_RULE_SINGLE )
    {

    }
    ///< ダブル
    else
    if( rule == BTL_RULE_DOUBLE )
    {

    }
    ///< ロイヤル
    else
    if( rule == BTL_RULE_ROYAL )
    {

    }
    else
    //  乱入
    if( rule == BTL_RULE_INTRUDE )
    {
    }

    if( m_pDraw != NULL )
    {
      m_pDraw->SetMode( m_drawInitParam.modeType );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 下画面の暗転フィルターの有効設定
    * @param bEnable : 有効設定
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::SetEnableDimFilterLayout( const bool bEnable )
  {
    GFL_ASSERT( m_pFrontDraw != NULL );
    m_pFrontDraw->SetEnableDimFilterLayout( bEnable );

    // @fix NMCat[2126] 暗転フィルターが表示している時はおていれボタンを押せないようにする
    {
      SetEnableInputEventOteire( ( bEnable == false ) );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   通信待機メッセージウィンドウ表示
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::ShowWaitComMesssageWindow()
  {
    GFL_ASSERT( m_pFrontDraw != NULL );
    m_pFrontDraw->ShowMessageWindow( str_btr_wait, true );
  }

  //------------------------------------------------------------------
  /**
    * @brief   通信待機メッセージウィンドウ表示
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::HideWaitComMesssageWindow()
  {
    GFL_ASSERT( m_pFrontDraw != NULL );
    m_pFrontDraw->HideMessageWindow();
  }

  //------------------------------------------------------------------
  /**
    * @brief   BGカバー表示設定
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::SetVisibleCurverBG( const bool bFlg )
  {
    GFL_ASSERT( m_pDraw );
    m_pDraw->SetVisibleCurverBG( bFlg );
  }

  //------------------------------------------------------------------
  /**
    * @brief   おていれを表示イベント
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::EventOteireDisp()
  {
    GFL_ASSERT( m_pDraw != NULL );

    m_pDraw->StartOteireMode();

    //  おていれボタンの入力可能にする
    SetEnableInputEventOteire( true );

    //  他の表示を整理

    //  天候を非表示
    m_pFrontDraw->SetWeather( BTL_WEATHER_NONE );
  }

  //------------------------------------------------------------------
  /**
    * @brief   おていれ表示中か
    */
  //------------------------------------------------------------------
  bool BattleUIBGFrame::IsOteireDisp()
  {
    if( m_pDraw == NULL )
    {
      GFL_ASSERT( 0 );
    }
   
    return  m_pDraw->IsOteireMode();
  }

  //------------------------------------------------------------------
  /**
    * @brief   おていれイベントのユーザー入力設定
    */
  //------------------------------------------------------------------
  void BattleUIBGFrame::SetEnableInputEventOteire( const bool bFlg )
  {
    if( m_pDraw == NULL )
    {
      GFL_ASSERT( 0 );
    }
    
    //  おていれモードじゃないので設定はスキップ
    if( m_pDraw->IsOteireMode() == false )
    {
      return;
    }

    //  お手入れが押されている状態だと入力設定はスキップ
    if( IsPushOteire() == false )
    {
      m_pDraw->SetInputEnabledAllView( bFlg );
    }    
  }

  //------------------------------------------------------------------
  /**
    * @brief   おていれを押したか
    */
  //------------------------------------------------------------------
  bool BattleUIBGFrame::IsPushOteire()
  {
    return  m_bPushOteire;
  }

  //------------------------------------------------------------------
  /**
    * @brief   決定入力用リスナ
    */
  //------------------------------------------------------------------
  //  override
  bool BattleUIBGFrame::OnEnterInput( app::BattleUIBGLowerDraw* pDrawInst, s32 buttonId )
  {
    if( buttonId == app::BattleUIBGLowerDraw::BUTTON_ID_OTEIRE )
    {
      m_bPushOteire = true;
      pDrawInst->SetInputEnabled( false );
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キーとスティックの入力結果
    */
  //------------------------------------------------------------------
  //  override
  void BattleUIBGFrame::OnKeyAction( app::BattleUIBGLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) == true )
    {
      pDrawInst->PushBtn( app::BattleUIBGLowerDraw::BUTTON_ID_OTEIRE );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUIBGFrame::_SetState( const BattleUIBGFrame::StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
      GFL_ASSERT( m_pDraw );

      //  開始
      m_pDraw->Start();
      m_pFrontDraw->Open();

      m_pDraw->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pDraw->SetInputEnabled( true );
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      m_pDraw->SetInputEnabled( false );

      //  終了
      m_pFrontDraw->Close();
      m_pDraw->End();
    }
  }

  //デバッグ用背景色変え
#if PM_DEBUG
  void BattleUIBGFrame::UpdateDebugBgColorCahnge(void)
  {
    if( m_pDraw )
    {
      nw::ut::Color8 col;
      m_debugColorCnt+=12;
      if( m_debugColorCnt > 255*2 )
      {
        m_debugColorCnt-=255*2;
      }
      col.Set((m_debugColorCnt>255?(255*2)-m_debugColorCnt:m_debugColorCnt),0,0,255);
      m_pDraw->GetLayoutWork(0)->GetPicturePane(PANENAME_BATTLE_BGR_LOW_000_PANE_BATTLE_BG_01)->GetMaterial(0)->SetColor(0,col);
      //m_pDraw->GetLayoutWork(0)->GetPicturePane(PANENAME_BATTLE_BGR_LOW_000_PANE_BATTLE_BG_01)->GetMaterial(0)->SetColor(1,col);
    }
  }
#endif//PM_DEBUG
GFL_NAMESPACE_END( btl )
