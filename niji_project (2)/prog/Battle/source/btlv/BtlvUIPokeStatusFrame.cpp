//=============================================================================
/**
 * @file	 BtlvUIPokeStatusFrame.cpp
 * @brief	 バトルUIポケモンステータス画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.19
 */
//=============================================================================
#pragma once

#include "BtlvUIPokeStatusFrame.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

#include "niji_conv_header/message/msg_btl_app.h"

//  nijiのインクルード
#include <system/include/HeapDefine.h>
#include <Sound/include/Sound.h>
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h"
#include "AppLib/include/Util/app_util_Common.h"

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUIPokeStatusFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUIPokeStatusFrame::BattleUIPokeStatusFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUIPokeStatusFrame::~BattleUIPokeStatusFrame()
  {
  }

  void BattleUIPokeStatusFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    GFL_ASSERT( pAppParam->in_param.pPokeParam );

    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIPokeStatusFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIPokeStatusFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPメモリーから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUIPokeStatusFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

    // ローカルヒープを作る
    m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.pParentAppHeap, m_pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIPokeStatusFrame::InitFunc(void)
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
      if( pAsyncFileMan->IsArcFileOpenFinished( s_arcId ) )
      {
        ++seq;
      }
    }

    if( seq == SEQ_LOAD_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

      req.arcId             = s_arcId;
      req.datId             = GARC_battleLowwerMenu_stalow_APPLYT;
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

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIPokeStatusFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw->GetState() == app::BattleUIPokeStatusLowerDrawBase::STATE_TYPE_INPUT_IDLE )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
    }

    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      //  入力したボタンアニメ終了を待つ
      if( m_pDraw->GetButtonManager()->GetUpdateState() == ::app::tool::ButtonManager::UPDATE_STATE_IDLE )
      {
        //  キャンセル入力であれば終了させる
        if( m_enterData.eventId == EVENT_ID_CANCEL )
        {
          //  ボタン終了
          End();
        }
        //  特性インフォ表示
        else
        if( m_enterData.eventId == EVENT_ID_TOKUSEI_INFO_DRAW )
        {
          _SetTokuseiInfoState( m_enterData.idStyle.id, m_enterData.idStyle.infoPos );
        }
        //  アイテムインフォ表示
        else
        if( m_enterData.eventId == EVENT_ID_ITEM_INFO_DRAW )
        {
          _SetItemInfoState( m_enterData.idStyle.id, m_enterData.idStyle.infoPos );
        }
        //  ステータスインフォ表示
        else
        if( m_enterData.eventId == EVENT_ID_STATUS_INFO_DRAW )
        {
          _SetStatusInfoState( m_enterData.infoWindowMessageStyle.titleMsgId, m_enterData.infoWindowMessageStyle.infoMsgId, m_enterData.infoWindowMessageStyle.infoPos );
        }
        //  状態リストを戻す
        else
        if( m_enterData.eventId == EVENT_ID_STATUS_LIST_PREV )
        {
          m_pDraw->GetStatusList().MovePrevPage();
           _SetState( STATE_TYPE_UPDATE );
        }
        //  状態リストを進める
        else
        if( m_enterData.eventId == EVENT_ID_STATUS_LIST_NEXT )
        {
          m_pDraw->GetStatusList().MoveNextPage();
           _SetState( STATE_TYPE_UPDATE );
        }
        else
        {
          //  論理バグ
          GFL_ASSERT( 0 );
        }
      }
    }

    if( m_state == STATE_TYPE_INFO_WINDOW )
    {
      app::BattleUIDataManager* pBtlDataManager = m_pAppParam->in_param.pBtlParamAccessror;
      ::app::tool::InfoWindowSheetType* pInfoWindowView = pBtlDataManager->GetInfoWindowView();

      if( pInfoWindowView->IsOpen() == false )
      {
        //  ルートViewからはずす
        pInfoWindowView->RemoveFromSuperView();

        //  通常更新に戻る
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      if( m_pDraw->GetState() == app::BattleUIPokeStatusLowerDrawBase::STATE_TYPE_END )
      {
        _SetState( STATE_TYPE_END );
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      return applib::frame::RES_FINISH;
    }

    if( m_pDraw != NULL )
    {
      m_pDraw->UpdateTree();
    }

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void BattleUIPokeStatusFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIPokeStatusFrame::EndFunc(void)
  {
    enum
    {
      SEQ_INIT  = 0,
      SEQ_WAIT,
      SEQ_END
    };

    int seq = applib::frame::CellSuper::GetSubSeq();
    if( seq == SEQ_INIT )
    {
      if( m_pDraw != NULL )
      {
        m_pDraw->Terminate();
      }

      ++seq;
    }

    if( seq == SEQ_WAIT )
    {
      if( m_pDraw != NULL )
      {
        m_pDraw->Update();
        if( m_pDraw->IsDelete() )
        {
          ++seq;
        }
      }
      else
      {
        ++seq;
      }
    }

    if( seq == SEQ_END )
    {
      m_pAppParam->out_param.bEnd  = true;

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

    return  applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief 終了通知
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusFrame::End()
  {
    if( m_bEnd == false )
    {
      if( m_state <= STATE_TYPE_OPEN )
      {
        _SetState( STATE_TYPE_END );
      }
      else
      {
        //  画面を終了させる
        _SetState( STATE_TYPE_CLOSE );
      }

      m_bEnd  = true;
    }
    else
    {
      // @fix NMCat[3418] フレーム破棄開始関数を連続で呼ぶ場合にアサートを出しているが、制限時間ではタイミングによっては破棄関数が連続で呼ばれるケースを失念していた。
      //                  また破棄関数を連続で呼んでも問題ない作りになっているのでアサートから警告に格下げする。

//      GFL_ASSERT( 0 );
      UCHIDA_PRINT( "warning: continuing BattleUIPokeStatusFrame::End\n" );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 選択した選択項目
  */
  //------------------------------------------------------------------
  // overwride
  bool BattleUIPokeStatusFrame::OnEnterInput( app::BattleUIPokeStatusLowerDrawBase* pDrawInst, const ENTER_DATA& rEnterData )
  {
    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    m_enterData.Clear();

    if( rEnterData.buttonId == app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_CANCEL )
    {
      m_enterData.eventId = EVENT_ID_CANCEL;

      _SetState( STATE_TYPE_ENTER_EVENT );
      return  true;
    }
    
    //  状態インフォボタンを押しているか
    {
      const s32 aInfoButtonId[] =
      {
        app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_STATIS_LIST_ITEM_INFO_0,
        app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_STATIS_LIST_ITEM_INFO_1,
        app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_STATIS_LIST_ITEM_INFO_2,
        app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_STATIS_LIST_ITEM_INFO_3,
      };

      u32 loopNum = GFL_NELEMS( aInfoButtonId );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( rEnterData.buttonId == aInfoButtonId[ i ] )
        {
          //  インフォボタンが押された
          m_enterData.eventId   = EVENT_ID_STATUS_INFO_DRAW;

          m_enterData.infoWindowMessageStyle.titleMsgId   = rEnterData.pushStatusData.titleNameId;
          m_enterData.infoWindowMessageStyle.infoMsgId    = rEnterData.pushStatusData.infoNameId;
          m_enterData.infoWindowMessageStyle.infoPos      = rEnterData.pushInfoIconPos;

          _SetState( STATE_TYPE_ENTER_EVENT );
          return  true;
        }
      }
    }

    //  状態リストの左右矢印が押された
    {
      if( rEnterData.buttonId == app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_STATUS_LIST_LEFT_ARROW )
      {
        m_enterData.eventId = EVENT_ID_STATUS_LIST_PREV;
        _SetState( STATE_TYPE_ENTER_EVENT );

        return  true;
      }
      else
      if( rEnterData.buttonId == app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_STATUS_LIST_RIGHT_ARROW )
      {
        m_enterData.eventId = EVENT_ID_STATUS_LIST_NEXT;
        _SetState( STATE_TYPE_ENTER_EVENT );

        return  true;
      }
    }

    if( rEnterData.buttonId == app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_INFO_TOKUSEI )
    {
      m_enterData.eventId         = EVENT_ID_TOKUSEI_INFO_DRAW;
      m_enterData.idStyle.id      = m_pDraw->GetInitParam().pPokeParam->GetValue( BTL_POKEPARAM::BPP_TOKUSEI );
      m_enterData.idStyle.infoPos = rEnterData.pushInfoIconPos;

      _SetState( STATE_TYPE_ENTER_EVENT );
      return  true;
    }

    if( rEnterData.buttonId == app::BattleUIPokeStatusLowerDrawBase::BUTTON_ID_INFO_ITEM )
    {
      m_enterData.eventId         = EVENT_ID_ITEM_INFO_DRAW;
      m_enterData.idStyle.id      = m_pDraw->GetInitParam().pPokeParam->GetItem();
      m_enterData.idStyle.infoPos = rEnterData.pushInfoIconPos;

      _SetState( STATE_TYPE_ENTER_EVENT );
      return  true;
    }
    
    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief  キー入力通知用リスナ
  */
  //------------------------------------------------------------------
  // overwride
  bool BattleUIPokeStatusFrame::OnKeyAction( app::BattleUIPokeStatusLowerDrawBase* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      pDrawInst->PushBtn( app::BattleUIEnemyPokeStatusLowerDraw::BUTTON_ID_CANCEL );
      return true;
    }

    app::BattleUIPokeStatusLowerDrawBase::StatusList& rStatusList = pDrawInst->GetStatusList();

    u32 nowStatusListPageNum    = rStatusList.GetNowPageNum();
    u32 bottomStatusListPageNum = rStatusList.GetBottomPageNum();
    u32 topStatusListPageNum    = rStatusList.GetTopPageNum();

    if( pKey->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      if( nowStatusListPageNum < bottomStatusListPageNum )
      {
        pDrawInst->PushBtn( app::BattleUIEnemyPokeStatusLowerDraw::BUTTON_ID_STATUS_LIST_RIGHT_ARROW );
        return true;
      }
    }

    if( pKey->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      if( topStatusListPageNum < nowStatusListPageNum )
      {
        pDrawInst->PushBtn( app::BattleUIEnemyPokeStatusLowerDraw::BUTTON_ID_STATUS_LIST_LEFT_ARROW );
        return true;
      }
    }

    return false;
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUIPokeStatusFrame::_SetState( const BattleUIPokeStatusFrame::StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
      //  画面クラスを作成
      {
        app::BattleUIPokeStatusLowerDrawBase::INIT_PARAM initParam;
        APP_PARAM::_tag_in_param* pAppInParam = &m_pAppParam->in_param;

        const BattleViewSystem* pCoreSystem = m_pAppParam->in_param.pBtlParamAccessror->GetCore();
        GFL_ASSERT( pCoreSystem );

        {
          initParam.pDelegate             = this;
          initParam.p_lyt_buff            = m_pArcReadBuff;
          initParam.pUtilHeap             = m_pAppHeap;

          initParam.pRenderingManager     = pAppInParam->pRenderingManager;
          initParam.pPokeParam            = pAppInParam->pPokeParam;
          initParam.pBtlAppMsgData        = pAppInParam->pBtlParamAccessror->GetAppMesssageData();
          initParam.pBtlStatusMsgData     = pAppInParam->pBtlParamAccessror->GetStatusMessageData();
          initParam.pBtlWordSet           = pAppInParam->pBtlParamAccessror->GetWordSet();
          initParam.bUseInputSE           = pAppInParam->pBtlParamAccessror->IsPlayInputSE();
          initParam.pStatusDataList       = pAppInParam->pStatusDataList;
          initParam.pAppCommonGrpIconData = &pAppInParam->pBtlParamAccessror->GetAppCommonGrpIconData();
          initParam.bShowTypeIcon         = true;

          //  図鑑に登録しているかどうか
          initParam.bRegsterZukan = pCoreSystem->IsZukanRegistered( m_pAppParam->in_param.pPokeParam );
        }

        //  タイプ取得(タイプ取得するには特殊なので使う側で設定)
        {
          pml::PokeType aOrgPokeType[ 2 ] = { POKETYPE_NULL, POKETYPE_NULL };
          // @fix NMCat[2729] イリュージョンと技によって変化したタイプを考えた取得関数を使ってポケモンタイプ表示
          // @fix GFNMCat[3869] タイプ出力をポケモンの元パラメータではなく、バトル中に変化した最新のタイプを表示に変更（仕様変更）
          pCoreSystem->GetUiDisplay_PokeType( &aOrgPokeType[ 0 ], &aOrgPokeType[ 1 ], *initParam.pPokeParam );

          //  タイプ出力
          ::app::util::OutputPokemonTypeManual( &initParam.aPokeType[ 0 ], &initParam.aPokeType[ 1 ], aOrgPokeType[ 0 ], aOrgPokeType[ 1 ] );
        }

        if( m_pAppParam->in_param.statusType == STATUS_TYPE_SELF )
        {
          m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUISelfPokeStatusLowerDraw( initParam );
        }
        else
        if( m_pAppParam->in_param.statusType == STATUS_TYPE_ENEMY )
        {
          //  図鑑登録していない敵のポケモンはタイプアイコン表示はしない
          //  わざ相性表示と連動するのでわざ表示フラグ関数を使う
          initParam.bShowTypeIcon = pCoreSystem->IsWazaAffinityDisplayEnable( initParam.pPokeParam->GetID() );

          m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUIEnemyPokeStatusLowerDraw( initParam );
        }

        GFL_ASSERT( m_pDraw );

        //  初期化
        m_pDraw->Initialize();

        //  開始
        m_pDraw->Start();

        m_pDraw->SetInputEnabled( false );
        m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
      }
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pDraw->SetInputEnabled( m_pAppParam->in_param.pBtlParamAccessror->GetCore()->IsControllable() );
    }
    else
    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      //  ボタン入力をすべて無効に
      m_pDraw->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      m_pDraw->SetInputEnabled( false );

      //  終了
      m_pDraw->End();
    }
    else
    if( m_state == STATE_TYPE_END )
    {
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態インフォ表示ステートに遷移
  */
  //------------------------------------------------------------------
  void BattleUIPokeStatusFrame::_SetStatusInfoState( const u32 titleMsgId, const u32 infoMsgId, const gfl2::math::Vector3 pos )
  {
    app::BattleUIDataManager* pBtlDataManager = m_pAppParam->in_param.pBtlParamAccessror;

    //  文字列バッファは一時用のを使用
    gfl2::str::StrBuf*  pTitleStrBuf    = m_pDraw->GetG2DUtil()->GetTempStrBuf( 0 );
    gfl2::str::StrBuf*  pInfoStrBuf     = m_pDraw->GetG2DUtil()->GetTempStrBuf( 1 );

    //  状態の名前と詳細テキストを取得
    pBtlDataManager->OutputStatusStrBuf( pTitleStrBuf, pInfoStrBuf, titleMsgId, infoMsgId, m_pAppParam->in_param.pPokeParam );

    //  インフォウィンドウを開いて描画に追加
    {
      //  ウィンドウを出す
      ::app::tool::InfoWindowSheetType* pInfoWindowView = pBtlDataManager->GetInfoWindowView();

      pInfoWindowView->StartOpen( NULL, pos, pTitleStrBuf, pInfoStrBuf );

      m_pDraw->AddSubView( pInfoWindowView );
    }

    _SetState( STATE_TYPE_INFO_WINDOW );
  }

  //------------------------------------------------------------------
  /**
    * @brief 特性インフォ表示ステートに遷移
  */
  //------------------------------------------------------------------
  void BattleUIPokeStatusFrame::_SetTokuseiInfoState( const u32 tokuseiId, const gfl2::math::Vector3 pos )
  {
    app::BattleUIDataManager* pBtlDataManager = m_pAppParam->in_param.pBtlParamAccessror;

    //  文字列バッファは一時用のを使用
    gfl2::str::StrBuf*  pTitleStrBuf    = pBtlDataManager->GetTempStrBuf( 0 );
    gfl2::str::StrBuf*  pInfoStrBuf     = pBtlDataManager->GetTempStrBuf( 1 );

    //  特性の名前と詳細のテキストを取得
    pBtlDataManager->OutputTokuseiStrBuf( pTitleStrBuf, pInfoStrBuf, tokuseiId );

    //  インフォウィンドウを開いて描画に追加
    {
      //  ウィンドウを出す
      ::app::tool::InfoWindowSheetType* pInfoWindowView = pBtlDataManager->GetInfoWindowView();

      pInfoWindowView->StartOpen( NULL, pos, pTitleStrBuf, pInfoStrBuf );

      m_pDraw->AddSubView( pInfoWindowView );
    }

    _SetState( STATE_TYPE_INFO_WINDOW );
  }

  //------------------------------------------------------------------
  /**
    * @brief アイテムインフォ表示ステートに遷移
  */
  //------------------------------------------------------------------
  void BattleUIPokeStatusFrame::_SetItemInfoState( const u32 itemId, const gfl2::math::Vector3 pos )
  {
    //  ウィンドウを出す
    app::BattleUIDataManager* pBtlDataManager = m_pAppParam->in_param.pBtlParamAccessror;

    //  文字列バッファは一時用のを使用
    gfl2::str::StrBuf*  pTitleStrBuf    = pBtlDataManager->GetTempStrBuf( 0 );
    gfl2::str::StrBuf*  pInfoStrBuf     = pBtlDataManager->GetTempStrBuf( 1 );

    //  アイテムの名前と詳細のテキスト取得
    pBtlDataManager->OutputItemStrBuf( pTitleStrBuf, pInfoStrBuf, itemId );

    //  インフォウィンドウを開いて描画に追加
    {
      //  ウィンドウを出す
      ::app::tool::InfoWindowSheetType* pInfoWindowView = pBtlDataManager->GetInfoWindowView();

      pInfoWindowView->StartOpen( NULL, pos, pTitleStrBuf, pInfoStrBuf );

      m_pDraw->AddSubView( pInfoWindowView );
    }

    _SetState( STATE_TYPE_INFO_WINDOW );
  }

GFL_NAMESPACE_END( btl )
