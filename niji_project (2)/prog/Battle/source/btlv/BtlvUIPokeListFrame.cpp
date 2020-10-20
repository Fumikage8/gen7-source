//=============================================================================
/**
 * @file	 BtlvUITargetSelectFrame.h
 * @brief	 バトルUIポケモンリスト画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.04.09
 */
//=============================================================================
#pragma once

#include "BtlvUIPokeListFrame.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerPokeListMenu.gaix"

#include "niji_conv_header/message/msg_btl_app.h"

//  gflib2のインクルード
#include <system/include/HeapDefine.h>
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h"
#include "AppLib/include/Ui/UIViewManager.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUIPokeListFrame::s_arcId = ARCID_BATTLE_LOWER_POKE_LIST_MENU;

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUIPokeListFrame::BattleUIPokeListFrame()
{
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUIPokeListFrame::~BattleUIPokeListFrame()
  {
  }

  void BattleUIPokeListFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    GFL_ASSERT( pAppParam->in_param.pAsyncFileManager );
    GFL_ASSERT( pAppParam->in_param.pBtlParamAccessror );
    GFL_ASSERT( pAppParam->in_param.pParentAppHeap );
    GFL_ASSERT( pAppParam->in_param.pParentDevHeap );
    GFL_ASSERT( pAppParam->in_param.pRenderingManager );
    GFL_ASSERT( pAppParam->in_param.pPokeSelectResult );

    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIPokeListFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIPokeListFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPめもりーから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUIPokeListFrame" );

    // このアプリ全体に必要なメモリを作成
    {
      m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

      // ローカルヒープを作る
      m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.pParentAppHeap, m_pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );
    }

    //  イベントメッセージ用の文字列バッファ作成
    {
      m_pEventMessageStrBuf = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 18 * 2 * 3, m_pAppHeap->GetDeviceHeap() );
    }

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIPokeListFrame::InitFunc(void)
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
      SEQ_WAIT_CREATE_VIEW,

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

#if GFL_DEBUG_PRINT
      GFL_PRINT( "\n%s: file open arcId %d\n\n", __FILE__, oreq.arcId );
#endif
      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_OPEN )
    {
      if( pAsyncFileMan->IsArcFileOpenFinished( s_arcId ))
      {
        ++seq;
      }
    }

    if( seq == SEQ_LOAD_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

      req.arcId             = s_arcId;
      req.datId             = GARC_battleLowwerPokeListMenu_poke_list_low_APPLYT;
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
      //  ファイルを閉じる
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
      //  画面クラスを作成
      if( m_bEnd == false )
      {
        //  画面クラスに必要な初期化パラメータ設定
        app::BattleUIPokeListLowerDraw::INIT_PARAM  initParam;
        {
          initParam.pLytBuff              = m_pArcReadBuff;
          initParam.pRenderingManager     = m_pAppParam->in_param.pRenderingManager;
          initParam.pUtilHeap             = m_pAppHeap;
          initParam.pBtlAppMsgData        = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();
          initParam.pBtlWordSet           = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
          initParam.pMenuCursorLytBuff    = m_pAppParam->in_param.pBtlParamAccessror->GetMenuCusrorLytResourceBuffer();
          initParam.pDelegate             = this;
          initParam.bUseInputSE           = m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE();
          initParam.pAppCommonGrpIconData = &m_pAppParam->in_param.pBtlParamAccessror->GetAppCommonGrpIconData();

          const BTL_PARTY*  pSelfParty  = m_pAppParam->in_param.pBtlParamAccessror->GetBattleParty_Self();
          GFL_ASSERT( pSelfParty );

          //  バトルの種類によってポケモンの並びを変える
          m_pokeListMax = _OutputPokeSelectItemListData( initParam.aPokeDataList );
        }

        m_pDraw = GFL_NEW( m_pAppHeap->GetSystemHeap() ) app::BattleUIPokeListLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );
      }

      ++seq;
    }

    if( seq == SEQ_WAIT_CREATE_VIEW )
    {
      if( m_pDraw == NULL )
      {
        return applib::frame::RES_FINISH;
      }
      else
      if(  m_pDraw->Initialize() == true )
      {
        //  初回ビュー設定
        m_pDraw->SetViewMode( app::BattleUIPokeListLowerDraw::VIEW_MODE_CMD_SELECT );

        //  初回ポケモンリストのカレントidx
        if( m_pAppParam->in_param.pFirstCurrentPokeParam == NULL )
        {
          _SetActivePokeObjList( 0 );
        }
        else
        {
          bool  bHit  = false;
          ;
          //  パラメータのアドレスから初期カーソル位置を決める
          app::BattleUIPokeListLowerDraw::INIT_PARAM&  rInitParam = m_pDraw->GetInitParam();
          s32 loopNum = GFL_NELEMS( rInitParam.aPokeDataList );
          for( s32 i = 0; i < loopNum; ++i )
          {
            if( rInitParam.aPokeDataList[ i ].pParam->GetSrcData() == m_pAppParam->in_param.pFirstCurrentPokeParam )
            {
              _SetActivePokeObjList( i );
              bHit  = true;
              break;
            }
          }

          if( bHit == false )
          {
            //  見つからない場合はカーソルは最初に
            _SetActivePokeObjList( 0 );
          }
        }

        //  初回ボタンカーソル位置
        if( m_pAppParam->in_param.firstButtonCursorPos == FIRST_BUTTON_CURSOR_POS_CURE )
        {
          m_pDraw->GetButtonCtrl()->SetCurrentCursor( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CURE, false );
        }

        // @fix NMCat[1595] 
        {
          //  ボタンを同時に検知できないようにするために必要
          ::app::ui::UIViewManager* pUIViewMan = GFL_SINGLETON_INSTANCE( ::app::ui::UIViewManager );
          m_bOldInputKeyByButtonAnimation = pUIViewMan->GetInputExclusiveKeyByButtonAnimation();

          pUIViewMan->SetInputExclusiveKeyByButtonAnimation( true );
        }

        //  ロードを終了して更新処理へ
        return applib::frame::RES_FINISH;
      }
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
  applib::frame::Result BattleUIPokeListFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER ) )
      {
        if( m_pDraw->GetState() == app::BattleUIPokeListLowerDraw::STATE_TYPE_UPDATE )
        {
          // @fix GFNMCat[1054] システムメッセージ遷移フラグがあるのであれば、メッセージ遷移へ
          if( m_bNextMessageStep == false )
          {
            _SetState( STATE_TYPE_UPDATE );
          }
          else
          {
            _SetState( STATE_TYPE_MESSAGE );
          }
        }
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
    }

    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      //  入力したボタンアニメ終了を待つ
      if( m_pDraw->IsButtonEndAnime() )
      {
        //  ボタン終了

        //  表示がつよさを見るならコマンド選択に戻す
        if( m_changeState == CHANGE_STATE_CANCEL )
        {
          app::BattleUIPokeListLowerDraw::ViewMode  nowViewMode = m_pDraw->GetViewMode();
          if( nowViewMode == app::BattleUIPokeListLowerDraw::VIEW_MODE_STATUS )
          {
            m_changeState = CHANGE_STATE_CMD;
          }
          else if( nowViewMode == app::BattleUIPokeListLowerDraw::VIEW_MODE_POKE_POS_SELECT )
          {
            m_changeState = CHANGE_STATE_CMD;
          }
        }

        //  回復使用結果通知
        if( m_changeState == CHANGE_STATE_CURE )
        {
          //  リザルトデータを作成
          const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pSelectPokeData = m_pDraw->GetSelectPokeObjDataPtr();
          GFL_ASSERT( pSelectPokeData );
          GFL_ASSERT( pSelectPokeData->pParam );

          _CreateResultDataPokeChange( RESULT_TYPE_USE_ITEM, pSelectPokeData );

          End();
        }
        //  ポケモン入れ替え通知
        else
        if( m_changeState == CHANGE_STATE_POKE_SET )
        {
          const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pSelectPokeData = m_pDraw->GetSelectPokeObjDataPtr();
          GFL_ASSERT( pSelectPokeData );
          GFL_ASSERT( pSelectPokeData->pParam );

          //  ここでポケモン入れ替え可能かチェック
          //  ポケモン入れ替え
          if( _ChkPokeChange( m_pEventMessageStrBuf, pSelectPokeData ) )
          {
            if( m_pAppParam->m_ExchangeOccurredCnt <= 0 )
            {
              //  ポケモン配置指定画面へ遷移するか決める
              if( _IsPokePostionSettingFlow() )
              {
                //  ポケモン配置選択
                m_pDraw->SetViewMode( app::BattleUIPokeListLowerDraw::VIEW_MODE_POKE_POS_SELECT );
                // @fix GFNMCat[1753] 画面モードを切り替えるたびにパラメータ更新しないとゲージ類の長さが初期化される
                m_pDraw->UpdateCurrentPokeStatusView( m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx );

                _SetState( STATE_TYPE_UPDATE );
              }
              else
              {
                //  リザルトデータを作成
                _CreateResultDataPokeChange( RESULT_TYPE_CHANGE_POKE, pSelectPokeData );

                End();
              }
            }
            //  複数入れ替えによる選択
            else
            {
              //  空いている箇所に設定
              for( u32 i = 0; i < GFL_NELEMS( m_pAppParam->out_param.resutlData.aExchangeOccurred ); ++i )
              {
                //  配置していないので配置する
                if( m_pAppParam->out_param.resutlData.aExchangeOccurred[ i ].btlPos == BTL_VPOS_ERROR )
                {
                  m_pAppParam->out_param.resutlData.aExchangeOccurred[ i ].btlPos = ConvExChangeIdxFromBtlvPos( (PokeExChangeIdx)i );
                  m_pAppParam->out_param.resutlData.aExchangeOccurred[ i ].selectPokeIdx  = pSelectPokeData->pokeIdx;
                  break;
                }
              }

              //  リザルトデータを作成
              _CreateResultDataPokeChange( RESULT_TYPE_EXCHANGE_POKE, pSelectPokeData );

              //  入れ替え終了
              End();

              ++m_pAppParam->m_ExchangeOccurredCnt;
            }
          }
          else
          {
            //  エラーメッセージへ
            _SetStateByMessage( true, m_pEventMessageStrBuf );
          }
        }
        //  つよさを見る
        else
        if( m_changeState == CHANGE_STATE_STATUS )
        {
          m_pDraw->SetViewMode( app::BattleUIPokeListLowerDraw::VIEW_MODE_STATUS );
          _SetActivePokeObjList( m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx );

          _SetState( STATE_TYPE_UPDATE );
        }
        //  コマンド選択
        else
        if( m_changeState == CHANGE_STATE_CMD )
        {
          m_pDraw->SetViewMode( app::BattleUIPokeListLowerDraw::VIEW_MODE_CMD_SELECT );
          _SetActivePokeObjList( m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx );

          _SetState( STATE_TYPE_UPDATE );
        }
        //  わざ詳細画面
        else
        if( m_changeState == CHANGE_STATE_WAZA_STATUS )
        {
          _SetState( STATE_TYPE_WAZA_STATUS );
        }
        //  特性のインフォ画面
        else
        if( m_changeState == CHANGE_STATE_TOKUSE_INFO )
        {
          _SetTokuseiInfoState( (u32)m_inputResult.select_tokusei_id, m_inputResult.select_info_icon_pos );
        }
        //  アイテムのインフォ画面
        else
        if( m_changeState == CHANGE_STATE_ITEM_INFO )
        {
          _SetItemInfoState( (u32)m_inputResult.select_item_id, m_inputResult.select_info_icon_pos );
        }
        //  ポケモン配置選択結果
        else
        if( m_changeState == CHANGE_STATE_POKE_POS_SELECT )
        {
          const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pSelectPokeData = m_pDraw->GetSelectPokeObjDataPtr();
          GFL_ASSERT( pSelectPokeData );
          GFL_ASSERT( pSelectPokeData->pParam );

          //  ポケモン配置可能か
          PokeExChangeIdx chnageIdx = LISTPOS_UL;
          BtlvPos chnageBtlvPos = BTL_VPOS_ERROR;
          if( m_inputResult.buttonId == app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE_POS_LEFT )
          {
            chnageIdx     = LISTPOS_UL;
          }
          else
          if( m_inputResult.buttonId == app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE_POS_RIGHT )
          {
            chnageIdx     = LISTPOS_UR;
          }
          else
          {
            GFL_ASSERT( 0 );
          }

          chnageBtlvPos = ConvExChangeIdxFromBtlvPos( chnageIdx );
          if( _ChkPokeReplacePos( m_pEventMessageStrBuf, chnageIdx ) == true )
          {
            //  選択した回数を記録
            ++m_pAppParam->m_ExchangeOccurredCnt;

            //  配置していないので配置する
            m_pAppParam->out_param.resutlData.aExchangeOccurred[ chnageIdx ].btlPos = chnageBtlvPos;
            m_pAppParam->out_param.resutlData.aExchangeOccurred[ chnageIdx ].selectPokeIdx  = pSelectPokeData->pokeIdx;
            m_pAppParam->out_param.resutlData.aExchangeOccurred[ chnageIdx ].outPokeIdx     = 0;

            //  リザルトデータを作成
            _CreateResultDataPokeChange( RESULT_TYPE_EXCHANGE_POKE, pSelectPokeData );

            //  ポケモン配置結果へ
            End();
          }
          else
          {
            //  エラーメッセージへ
            _SetStateByMessage( true, m_pEventMessageStrBuf );
          }
        }
        //  キャンセル
        else
        if( m_changeState == CHANGE_STATE_CANCEL )
        {
          m_pAppParam->out_param.resutlData.type  = RESULT_TYPE_CANCEL;

          End();
        }
      }
    }

    if( m_state == STATE_TYPE_WAZA_STATUS )
    {
      if( m_skillStatusFrameAppParam.out_param.bEnd )
      {
        m_pSkillStatusFrame  = NULL;

        //  終了して前の画面に戻る
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_INFO_WINDOW )
    {
      app::BattleUIDataManager* pBtlDataManager         = m_pAppParam->in_param.pBtlParamAccessror;
      ::app::tool::InfoWindowSheetType* pInfoWindowView = pBtlDataManager->GetInfoWindowView();

      if( pInfoWindowView->IsOpen() == false )
      {
        pInfoWindowView->RemoveFromSuperView();
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_MESSAGE )
    {
      if( m_pDraw->IsProcEventMesssage() == false )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      enum
      {
        SEQ_CLOSE_FADE_OUT  = 0,
        SEQ_CLOSE_SCREEN_END,
      };

      if( m_subState == SEQ_CLOSE_FADE_OUT )
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        if( pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER ) )
        {
            //  終了
          m_pDraw->End();

          ++m_subState;
        }
      }
      
      if( m_subState == SEQ_CLOSE_SCREEN_END )
      {
        if( m_pDraw->GetState() == app::BattleUIPokeListLowerDraw::STATE_TYPE_END )
        {
          _SetState( STATE_TYPE_END );
        }
      }
    }

    // @fix GFNMCat[1054]
    //  メッセージフラグがあれば処理割り込みをする。
    //  この処理は開始ステート以降でないとだめ
    if( ( STATE_TYPE_OPEN < m_state ) && ( m_bNextMessageStep == true ) )
    {
      _SetState( STATE_TYPE_MESSAGE );
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
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void BattleUIPokeListFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
  applib::frame::Result BattleUIPokeListFrame::EndFunc(void)
  {
    int seq = applib::frame::CellSuper::GetSubSeq();

    enum
    {
      SEQ_INIT  = 0,
      SEQ_WAIT,
      SEQ_END
    };

    if( seq == SEQ_INIT )
    {   
      if( m_pDraw != NULL )
      {
        m_pDraw->Terminate();
      }

      //  わざ詳細フレームが開いているのであれば終了
      {
        if( ( m_skillStatusFrameAppParam.out_param.bEnd == false ) && ( m_pSkillStatusFrame != NULL ) )
        {
          m_pSkillStatusFrame->End();
          m_pSkillStatusFrame = NULL;
        }
      }

      ++seq;
    }

    if( seq == SEQ_WAIT )
    {
      if( m_pDraw != NULL )
      {
        m_pDraw->UpdateTree();
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
      //  入力結果を出力

      m_skillStatusFrameAppParam.out_param.bEnd   = true;
      m_pAppParam->out_param.bEnd                 = true;

      GFL_SAFE_DELETE( m_pDraw );

      GflHeapSafeFreeMemory( m_pArcReadBuff );

      GFL_SAFE_DELETE( m_pEventMessageStrBuf );
      GFL_SAFE_DELETE( m_pAppHeap );

      gfl2::heap::Manager::DeleteHeap( m_pManagerHeapBase );

  #if GFL_DEBUG_PRINT
      m_pAppParam->in_param.pAsyncFileManager->PrintArcFileList();
  #endif

      // @fix NMCat[1595] 
      {
        //  ボタンを同時に検知を元に戻す
        ::app::ui::UIViewManager* pUIViewMan = GFL_SINGLETON_INSTANCE( ::app::ui::UIViewManager );
        pUIViewMan->SetInputExclusiveKeyByButtonAnimation( m_bOldInputKeyByButtonAnimation );
      }

      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief 終了通知
    */
  //------------------------------------------------------------------
  void BattleUIPokeListFrame::End()
  {
    if( m_bEnd == false )
    {
      //  わざ詳細フレームが開いているのであれば終了
      {
        if( ( m_skillStatusFrameAppParam.out_param.bEnd == false ) && ( m_pSkillStatusFrame != NULL ) )
        {
          m_pSkillStatusFrame->End();
          m_pSkillStatusFrame = NULL;
        }
      }

      if( m_pDraw == NULL )
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
      // @fix NMCat[2195] フレーム破棄開始関数を連続で呼ぶ場合にアサートを出しているが、制限時間ではタイミングによっては破棄関数が連続で呼ばれるケースを失念していた。
      //                  また破棄関数を連続で呼んでも問題ない作りになっているのでアサートから警告に格下げする。
      //GFL_ASSERT( 0 );
      UCHIDA_PRINT( "warning: continuing BattleUIPokeListFrame::End" );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 決定入力用リスナ
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListFrame::OnEnterInput( app::BattleUIPokeListLowerDraw* pDrawInst, const BattleUIPokeListLowerDrawDelegate::RESULT_DATA& rResultData )
  {
    if( _IsInputCatch() == false )
    {
      return  false;
    }

    //  結果を保存
    m_inputResult = rResultData;

    //  ポケモンオブジェクトを入力
    {
      //  ポケモンオブジェクトのボタンidリスト
      const app::BattleUIPokeListLowerButtonCtrl::ButtonId aPokeObjButtonId[]  =
      {
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_0,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_1,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_2,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_3,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_4,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_5,
      };

      for( s32 i = 0; i < GFL_NELEMS( aPokeObjButtonId ); ++i )
      {
        if( aPokeObjButtonId[ i ] == rResultData.buttonId )
        {
          //  ポケモン参照を切り替え
          _SetActivePokeObjList( i );

          return  true;
        }
      }
    }

    //  わざボタンを入力
    {
      const app::BattleUIPokeListLowerButtonCtrl::ButtonId aSkillButtonId[]  =
      {
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_SKILL_ITEM_00,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_SKILL_ITEM_01,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_SKILL_ITEM_02,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_SKILL_ITEM_03,
      };

      for( s32 i = 0; i < GFL_NELEMS( aSkillButtonId ); ++i )
      {
        if( aSkillButtonId[ i ] == rResultData.buttonId )
        {
          //  わざ詳細説明表示
          m_changeState = CHANGE_STATE_WAZA_STATUS;
          _SetState( STATE_TYPE_ENTER_EVENT );

          return  true;
        }
      }
    }

    //  ポケモン入れ替え位置を入力
    {
      const app::BattleUIPokeListLowerButtonCtrl::ButtonId aChangePosButtonid[]  =
      {
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE_POS_LEFT,
        app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE_POS_RIGHT,
      };

      for( s32 i = 0; i < GFL_NELEMS( aChangePosButtonid ); ++i )
      {
        if( aChangePosButtonid[ i ] == rResultData.buttonId )
        {
          m_changeState = CHANGE_STATE_POKE_POS_SELECT;

          _SetState( STATE_TYPE_ENTER_EVENT );

          return  true;
        }
      }
    }

    //  Back入力
    if( rResultData.buttonId == app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_BACK )
    {
      m_changeState = CHANGE_STATE_CANCEL;
      _SetState( STATE_TYPE_ENTER_EVENT );

      return  true;
    }
    //  いれかえ入力
    else
    if( rResultData.buttonId == app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE )
    {
      m_changeState = CHANGE_STATE_POKE_SET;
      _SetState( STATE_TYPE_ENTER_EVENT );

      return  true;
    }
    //  かいふくするを入力
    else
    if( rResultData.buttonId == app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CURE )
    {
      m_changeState = CHANGE_STATE_CURE;
      _SetState( STATE_TYPE_ENTER_EVENT );

      return  true;
    }
    //  つよさを見るを入力
    else
    if( rResultData.buttonId == app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_STATUS )
    {
      m_changeState = CHANGE_STATE_STATUS;
      _SetState( STATE_TYPE_ENTER_EVENT );

      return  true;
    }
    //  特性のインフォボタン入力
    else
    if( rResultData.buttonId == app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_TOKUSEI_INFO_ICON )
    {
      m_changeState = CHANGE_STATE_TOKUSE_INFO;
      _SetState( STATE_TYPE_ENTER_EVENT );

      return  true;
    }
    //  アイテムのインフォボタン入力
    else
    if( rResultData.buttonId == app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_ITEM_INFO_ICON )
    {
      m_changeState = CHANGE_STATE_ITEM_INFO;
      _SetState( STATE_TYPE_ENTER_EVENT );

      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief キー入力通知用リスナ
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListFrame::OnKeyAction( app::BattleUIPokeListLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( _IsInputCatch() == false )
    {
      return  false;
    }

    app::BattleUIPokeListLowerButtonCtrl*  pButtonCtrl  = pDrawInst->GetButtonCtrl();
    GFL_ASSERT( pButtonCtrl );

    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      if( pButtonCtrl->PushButtonId( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_BACK ) )
      {
        return  true;
      }
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      if( pButtonCtrl->PushBtnCurrentCursor() )
      {
        return  true;
      }
    }

    //  カーソル移動がリスト形式なら上下移動の入力に少し制御を加える
    if( pButtonCtrl->GetCursorMoveTyoe() == app::BattleUIPokeListLowerButtonCtrl::CURSOR_MOVE_TYPE_LIST )
    {
      s32 nowCursorListIdx  = pButtonCtrl->GetCursorMoveIdx();
      s32 maxCursorListIdx  = pButtonCtrl->GetCursorMoveIdxMax();

      if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ) && ( nowCursorListIdx <= 0 ) )
      {
        if( pButtonCtrl->MoveCursorUp( true ) )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          return true;
        }
      }
      else
      if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) && ( 0 < nowCursorListIdx ) )
      {
        if( pButtonCtrl->MoveCursorUp( false ) )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          return true;
        }
      }
      else
      if( ( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) ) && ( maxCursorListIdx <= nowCursorListIdx ) )
      {
        if( pButtonCtrl->MoveCursorDown( true ) )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          return true;
        }
      }
      else
      if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) && ( nowCursorListIdx < maxCursorListIdx ) )
      {
        if( pButtonCtrl->MoveCursorDown( false ) )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          return true;
        }
      }
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ) )
    {
      if( pButtonCtrl->MoveCursorUp( false ) )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }
        }

        return  true;
      }
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) )
    {
      if( pButtonCtrl->MoveCursorDown( false ) )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }

        return  true;
      }
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
    {
      if( pButtonCtrl->MoveCursorLeft( false ) )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }

        return  false;
      }
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
    {
      if( pButtonCtrl->MoveCursorRight( false ) )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }

        return  true;
      }
    }

    //  ポケリストのカレント位置更新
    {
      if( pDrawInst->IsEnablePokeObjList() )
      {
        s32 oldPokeListIdx  = m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx;

        if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) && ( m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx <= 0 ) )
        {
          m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx = m_pokeListMax - 1;
        }
        else
        if( pButton->IsRepeat( gfl2::ui::BUTTON_L ) && ( 0 < m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx ) )
        {
          --m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx;
        }
        else
        if( ( pButton->IsTrigger( gfl2::ui::BUTTON_R ) ) && ( ( m_pokeListMax - 1 ) <= m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx ) )
        {
          m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx = 0;
        }
        else
        if( pButton->IsRepeat( gfl2::ui::BUTTON_R ) && ( m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx < ( m_pokeListMax - 1 ) ) )
        {
          ++m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx;
        }

        //  リストカレントidxが変わっているので変更処理
        if( oldPokeListIdx != m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          _SetActivePokeObjList( m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx );

          return  true;
        }
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief	   描画できる状態になった事を返すための関数
    * @return	 描画できる状態になった場合trueを返す
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListFrame::IsDrawEnable(void)
  {
    if( m_pDraw == NULL )
    {
      return  false;
    }

    return  ( m_pDraw->IsVisible() );
  }

  //------------------------------------------------------------------
  /**
    * @brief わざタイプから現在配置している敵を参照して効果タイプを取得
    * @param[out]   pPutFlg       : 表示可能
    * @param[in]    wazaNo        : 対象のわざNo
    * @param[in]    pBtlPokeParam : wazaNoも持っているバトルポケモンのパラメータ
    */
  //------------------------------------------------------------------
  BtlTypeAffAbout BattleUIPokeListFrame::OnGetKoukaType( bool* pPutFlg, const WazaNo wazaNo, const BTL_POKEPARAM* pBtlPokeParam )
  {
    if( pPutFlg == NULL )
    {
      GFL_ASSERT( 0 );
      return  pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
    }

    GFL_ASSERT( pBtlPokeParam );
    return  m_pAppParam->in_param.pBtlParamAccessror->GetPokePosListKoukaType( pPutFlg, wazaNo, pBtlPokeParam->GetID(), m_pAppParam->in_param.pBtlParamAccessror->GetEnemyPokePosPlaceList() );
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUIPokeListFrame::_SetState( const StateType newStateType )
  {
    m_state = newStateType;
    m_subState  = 0;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
      //  フェードインする
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->RequestIn( gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA );

      //  開始
      m_pDraw->Start();

      m_pDraw->SetInputEnabled( false );
      m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pDraw->SetInputEnabledAllView( m_pAppParam->in_param.pBtlParamAccessror->GetCore()->IsControllable() );
    }
    else
    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      m_pDraw->SetInputEnabledAllView( false );
    }
    else
    if( m_state == STATE_TYPE_WAZA_STATUS )
    {
      //  わざ詳細フレーム作成
      //  フレームの上にさらにわざ説明のフレームを重ねがけ
      m_pSkillStatusFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, btl::BattleUISkillStatusFrame>( CellSuper::GetManager() );
      GFL_ASSERT( m_pSkillStatusFrame );

      BattleUISkillStatusFrame::APP_PARAM::_tag_in_param*  pInParam = &m_skillStatusFrameAppParam.in_param;
      pInParam->pAsyncFileManager     = m_pAppParam->in_param.pAsyncFileManager;
      pInParam->pRenderingManager     = m_pAppParam->in_param.pRenderingManager;
      pInParam->pParentAppHeap        = m_pAppParam->in_param.pParentAppHeap;
      pInParam->pParentDevHeap        = m_pAppParam->in_param.pParentDevHeap;
      pInParam->bUseInputSE           = m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE();
      pInParam->pTmpDevHeap           = m_pAppParam->in_param.pTmpDevHeap;
      pInParam->pAppCommonGrpIconData = &m_pAppParam->in_param.pBtlParamAccessror->GetAppCommonGrpIconData();
      pInParam->pBtlViewSystem        = m_pAppParam->in_param.pBtlParamAccessror->GetCore();
      //  わざ詳細関連のメッセージデータ設定
      {
        pInParam->pBtlAppMsgData    = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();
        pInParam->pBtlWordSet       = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
        pInParam->pWazaInfoMsgData  = m_pAppParam->in_param.pBtlParamAccessror->GetWazaInfoMesssageData();
      }

      //  わざデータ関連の設定
      {
        pInParam->wazaId    = m_inputResult.skill_data.id;
        pInParam->ppNowNum  = m_inputResult.skill_data.nowPPNum;
        pInParam->ppMaxNum  = m_inputResult.skill_data.maxPPNum;
      }

      m_pSkillStatusFrame->Initialize( &m_skillStatusFrameAppParam );
    }
    else
    if( m_state == STATE_TYPE_MESSAGE )
    {
      m_bNextMessageStep  = false;
      if( m_bEventWait )
      {
        m_pDraw->SetInputEnabledAllView( false );
      }

      m_pDraw->EventMessage( m_pEventMessageStrBuf );
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      //  下記の結果時のみフェードアウトを呼ぶ
      //  回復するを押した時に設定
      //  前の画面に戻るを押した時に設定
      if( ( m_pAppParam->out_param.resutlData.type == RESULT_TYPE_USE_ITEM ) ||
          ( m_pAppParam->out_param.resutlData.type == RESULT_TYPE_CANCEL ) )
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestDefaultOut( gfl2::Fade::DISP_LOWER );
      }

      if( m_pDraw != NULL )
      {
        m_pDraw->SetInputEnabledAllView( false );
      }
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      if( m_pDraw != NULL )
      {
        m_pDraw->SetInputEnabledAllView( false );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief メッセージステート設定
  */
  //------------------------------------------------------------------
  void BattleUIPokeListFrame::_SetStateByMessage( const bool bEventWait, gfl2::str::StrBuf* pMessageStrBuf )
  {
    GFL_ASSERT( pMessageStrBuf );
    if( pMessageStrBuf != m_pEventMessageStrBuf )
    {
      //  指定した文字列がこちらで用意した文字列変数でないのであれば、コピーする
      m_pEventMessageStrBuf->SetStr( *pMessageStrBuf );
    }

    m_bEventWait  = bEventWait;

    // @fix GFNMCat[1054]
    //  すぐに遷移しない
    //  遷移はUpdateFunc()内でする
    //  UpdateFunc()即時にメッセージが呼ばれる場合に開始処理をしないでスキップするのを防ぐため
    m_bNextMessageStep  = true;
  }

  //------------------------------------------------------------------
  /**
    * @brief ポケリストidxをアクティブに
  */
  //------------------------------------------------------------------
  void BattleUIPokeListFrame::_SetActivePokeObjList( const s32 pokeListIdx )
  {
    m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx = pokeListIdx;

    //  ここの位置で処理しないと卵選択時で設定したステートが切り替わり、意図しない挙動になる
    m_pDraw->SetActivePokeObjList( pokeListIdx );

    //  現在参照しているポケモンパラメータを結果データに保管
    m_pAppParam->out_param.resutlData.pCurrentPokeParam = m_pDraw->GetSelectPokeObjDataPtr()->pParam;

    //  コマンド選択
    if( m_pDraw->GetViewMode() == app::BattleUIPokeListLowerDraw::VIEW_MODE_CMD_SELECT )
    {
      //  ポケモン状態でボタン設定をする
      app::BattleUIPokeListLowerButtonCtrl* pButtonCtrl = m_pDraw->GetButtonCtrl();

      app::BattleUIPokeListLowerDraw::INIT_PARAM& rInitParam  = m_pDraw->GetInitParam();
      app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pPokeSelectItemData = &rInitParam.aPokeDataList[ pokeListIdx ];

      //  ポケモンが卵なら専用設定
      if( pPokeSelectItemData->pParam->GetSrcData()->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        //  メッセージ設定
        {
          print::WordSet*     pWordset       = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
          gfl2::str::MsgData* pMessageData  = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();

          gfl2::str::StrBuf* pSourceBuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( m_pEventMessageStrBuf->GetSize(), m_pAppHeap->GetDeviceHeap() );

          //  卵の場合
          pMessageData->GetString( STR_CMD_EGG, *pSourceBuf );
          pWordset->Expand( m_pEventMessageStrBuf, pSourceBuf );

          GFL_SAFE_DELETE( pSourceBuf );
        }

        //  エラーメッセージへ
        _SetStateByMessage( false, m_pEventMessageStrBuf );
      }
      else
      {
        //  ボタンを有効に
        pButtonCtrl->SetVisibleButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE, true );
        pButtonCtrl->SetVisibleButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CURE, true );
        pButtonCtrl->SetVisibleButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_STATUS, true );

        // @fix NMCat[2158] プランナーさんから指定された判定順に変更してボタンテキスト表示内容優先度を変える

        //  ポケモンがパートナーのものであれば入れ替えボタン名をパートナーに変更
        if( pPokeSelectItemData->type == app::BattleUIPokeListLowerDraw::POKE_TYPE_PARTNER )
        {
          pButtonCtrl->SetNameButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE, STR_CMD_CHG_BTN_PARTNER );
        }
        else
        if( pPokeSelectItemData->bSelected )
        {
          //  入れ替え選択しているポケモンを選んだ場合
          pButtonCtrl->SetNameButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE, STR_CMD_SELECT_1 );
        }
        else
        //  死亡している
        if( pPokeSelectItemData->pParam->IsDead() )
        {
          pButtonCtrl->SetNameButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE, STR_CMD_CHG_BTN_DEAD );
        }
        else
        //  配置済み
        if( _IsInBattlePoke( pPokeSelectItemData ) )
        {
          //  すでに配置ポケモンなら入れ替えー＞戦闘中に名前を変える
          pButtonCtrl->SetNameButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE, STR_CMD_CHG_BTN_ALREADY );
        }
        else
        {
          //  入れ替え可能であれば入れ替えに名を
          pButtonCtrl->SetNameButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE, STR_CMD_CHG_BTN_CHANGE_00 );
        }

        pButtonCtrl->SetNameButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CURE, STR_CMD_REPAIR );
        pButtonCtrl->SetNameButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_STATUS, STR_VIEW_STATUS );

        pButtonCtrl->SetVisibleButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CHANGE, true );

        pButtonCtrl->SetVisibleButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_STATUS, true );

        //  バッグが使用できないまたは気絶時の入れ替えなら回復ボタンをOFF
        // @fix GFNMCat[833] バトルハウスのチェックを抜けていたのでバッグが使えるかどうかの関数にまとめるようにした。
        if( ( m_pAppParam->in_param.pBtlParamAccessror->IsUseBag() == false ) ||
          m_pAppParam->in_param.frameType == FRAME_TYPE_REPLACE )
        {
          pButtonCtrl->SetVisibleButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CURE, false );
        }
        else
        {
          pButtonCtrl->SetVisibleButton( app::BattleUIPokeListLowerButtonCtrl::BUTTON_ID_CURE, true );
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルポケモンリスト選択のデータ出力
    * @note
    *  バトルのルールや通信対戦などで他人のポケモンもリストに含まれる
    */
  //------------------------------------------------------------------
  s32 BattleUIPokeListFrame::_OutputPokeSelectItemListData( app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM outputData[ BTL_PARTY_MEMBER_MAX ] )
  {
    if( outputData == NULL )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    const BTL_PARTY*  pSelfParty  = m_pAppParam->in_param.pBtlParamAccessror->GetBattleParty_Self();
    GFL_ASSERT( pSelfParty );

    //  初期化
    {
      for( u8 i = 0; i < BTL_PARTY_MEMBER_MAX; ++i )
      {
        outputData[ i ].Clear();
      }
    }

    app::BattleUIDataManager::PokeListType  listType  = m_pAppParam->in_param.pBtlParamAccessror->GetPokeListType();

    s32 dataListIdx = 0;

    if( listType == app::BattleUIDataManager::MEMBER_SINGLE_PARTY )
    {
      //  パートナーがいない
      u8  loopNum = pSelfParty->GetMemberCount();
      for( u8 i = 0; i < loopNum; ++i )
      {
        outputData[ dataListIdx ].type    = app::BattleUIPokeListLowerDraw::POKE_TYPE_PLAYER;
        outputData[ dataListIdx ].pParam  = pSelfParty->GetMemberDataConst( i );
        outputData[ dataListIdx ].pokeIdx = i;

        GFL_ASSERT( outputData[ dataListIdx ].pParam );

        ++dataListIdx;
      }
    }
    else
    if( listType == app::BattleUIDataManager::MEMBER_MULTI_PARTY_L )
    {
      const BTL_PARTY*  pFriendParty  = m_pAppParam->in_param.pBtlParamAccessror->GetBattleParty_Friend();
      GFL_ASSERT( pFriendParty );

      //  プレイヤーがリスト上側
      for( u8 i = 0; i < 3; ++i )
      {
        outputData[ dataListIdx ].type    = app::BattleUIPokeListLowerDraw::POKE_TYPE_PLAYER;
        outputData[ dataListIdx ].pParam  = pSelfParty->GetMemberDataConst( i );
        outputData[ dataListIdx ].pokeIdx = i;

        if( ( outputData[ dataListIdx ].pParam == NULL ) )
        {
          continue;
        }

        ++dataListIdx;
      }

      //  パートナーがリスト下側
      for( u8 i = 0; i < 3; ++i )
      {
        outputData[ dataListIdx ].type    = app::BattleUIPokeListLowerDraw::POKE_TYPE_PARTNER;
        outputData[ dataListIdx ].pParam  = pFriendParty->GetMemberDataConst( i );
        outputData[ dataListIdx ].pokeIdx = i;

        if( ( outputData[ dataListIdx ].pParam == NULL ) )
        {
          continue;
        }

        ++dataListIdx;
      }
    }
    else
    if( listType == app::BattleUIDataManager::MEMBER_MULTI_PARTY_R )
    {
      const BTL_PARTY*  pFriendParty  = m_pAppParam->in_param.pBtlParamAccessror->GetBattleParty_Friend();
      GFL_ASSERT( pFriendParty );

      //  パートナーがリスト上側
      for( u8 i = 0; i < 3; ++i )
      {
        outputData[ dataListIdx ].type    = app::BattleUIPokeListLowerDraw::POKE_TYPE_PARTNER;
        outputData[ dataListIdx ].pParam  = pFriendParty->GetMemberDataConst( i );
        outputData[ dataListIdx ].pokeIdx = i;

        if( ( outputData[ dataListIdx ].pParam == NULL ) )
        {
          continue;
        }

        ++dataListIdx;
      }

      //  プレイヤーがリスト下側
      for( u8 i = 0; i < 3; ++i )
      {
        outputData[ dataListIdx ].type    = app::BattleUIPokeListLowerDraw::POKE_TYPE_PLAYER;
        outputData[ dataListIdx ].pParam  = pSelfParty->GetMemberDataConst( i );
        outputData[ dataListIdx ].pokeIdx = i;

        if( ( outputData[ dataListIdx ].pParam == NULL ) )
        {
          continue;
        }

        ++dataListIdx;
      }
    }

    //  すでに選択しているかチェックして選択済みならフラグを立てる
    {
      //  複数入れ替え用(死亡時入れ替え限定)
      if( m_pAppParam->in_param.frameType == FRAME_TYPE_REPLACE )
      {
        u8  num = GFL_NELEMS( m_pAppParam->out_param.resutlData.aExchangeOccurred );
        for( s32 i = 0; i < dataListIdx; ++i )
        {
          for( u8 resultDataIdx = 0; resultDataIdx < num; ++resultDataIdx )
          {
            if( outputData[ i ].type == app::BattleUIPokeListLowerDraw::POKE_TYPE_PLAYER )
            {
              if( m_pAppParam->out_param.resutlData.aExchangeOccurred[ resultDataIdx ].btlPos != BTL_VPOS_ERROR )
              {
                if( m_pAppParam->out_param.resutlData.aExchangeOccurred[ resultDataIdx ].selectPokeIdx == outputData[ i ].pokeIdx )
                {
                  //  すでに選択している
                  outputData[ i ].bSelected = true;
                  break;
                }
              }
            }
          }
        }
      }
      //  単体入れ替え用(ポケモン選択時)
      else
      {
        u8  num = m_pAppParam->in_param.pPokeSelectResult->GetCount();
        for( s32 i = 0; i < dataListIdx; ++i )
        {
          for( u8 resultDataIdx = 0; resultDataIdx < num; ++resultDataIdx )
          {
            if( outputData[ i ].type == app::BattleUIPokeListLowerDraw::POKE_TYPE_PLAYER )
            {
              if( m_pAppParam->in_param.pPokeSelectResult->Get( resultDataIdx ) == outputData[ i ].pokeIdx )
              {
                //  すでに選択している
                outputData[ i ].bSelected = true;
                break;
              }
            }
          }
        }
      }
    }

    return  dataListIdx;
  }

  //  戦闘にでているポケモンなのか
  bool BattleUIPokeListFrame::_IsInBattlePoke( const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pPokeSelectItemParam )
  {
    GFL_ASSERT( pPokeSelectItemParam );

    u8 bench_pos_index = 0;
    if( pPokeSelectItemParam->type == app::BattleUIPokeListLowerDraw::POKE_TYPE_PLAYER )
    {
      bench_pos_index = m_pAppParam->in_param.pBtlParamAccessror->GetCore()->GetBenchPosIndex_Self();
    }
    else
    {
      bench_pos_index = m_pAppParam->in_param.pBtlParamAccessror->GetCore()->GetBenchPosIndex_Friend();
    }

    //  すでに配置しているポケモンのリストidxか
    return ( pPokeSelectItemParam->pokeIdx < bench_pos_index );
  }

  //  ポケモン入れ替えが可能かチェック(単体ポケチェック用)
  bool BattleUIPokeListFrame::_ChkPokeChange( gfl2::str::StrBuf* pOutErrorMessage, const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pSelectPokeData )
  {
    GFL_ASSERT( pSelectPokeData );
    GFL_ASSERT( pSelectPokeData->pParam );
    GFL_ASSERT( pOutErrorMessage );

    print::WordSet* wordset           = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
    gfl2::str::MsgData* message_data  = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();

    const pml::pokepara::PokemonParam* pPokeParam = pSelectPokeData->pParam->GetSrcData();

    // @fix NMCat[2158] プランナーさんから指定された判定順に変更してエラー時のメッセージ表示内容優先度を変える
    if( pSelectPokeData->type == app::BattleUIPokeListLowerDraw::POKE_TYPE_PARTNER )
    {
      gfl2::str::StrBuf* pSourceBuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( pOutErrorMessage->GetSize(), m_pAppHeap->GetDeviceHeap() );

      //  パートナーの場合
      message_data->GetString( STR_CMD_PARTNER, *pSourceBuf );
      wordset->Expand( pOutErrorMessage, pSourceBuf );

      GFL_SAFE_DELETE( pSourceBuf );

      return  false;
    }
    //  すでに選択済みか
    else
    if( pSelectPokeData->bSelected )
    {
      gfl2::str::StrBuf* pSourceBuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( pOutErrorMessage->GetSize(), m_pAppHeap->GetDeviceHeap() );

      message_data->GetString( STR_CMD_SELECT_2, *pSourceBuf );
      wordset->RegisterPokeNickName( 1, pPokeParam );
      wordset->Expand( pOutErrorMessage, pSourceBuf );

      GFL_SAFE_DELETE( pSourceBuf );

      return  false;
    }
    else
    if( pSelectPokeData->pParam->IsDead() )
    {
      gfl2::str::StrBuf* pSourceBuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( pOutErrorMessage->GetSize(), m_pAppHeap->GetDeviceHeap() );

      //  死亡している
      message_data->GetString( STR_CMD_DEAD, *pSourceBuf );
      wordset->RegisterPokeNickName( 1, pPokeParam );
      wordset->Expand( pOutErrorMessage, pSourceBuf );

      GFL_SAFE_DELETE( pSourceBuf );

      return  false;
    }
    else
    if( _IsInBattlePoke( pSelectPokeData ) )
    {
      gfl2::str::StrBuf* pSourceBuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( pOutErrorMessage->GetSize(), m_pAppHeap->GetDeviceHeap() );

      //  すでに配置している
      message_data->GetString( STR_CMD_ALREADY, *pSourceBuf );
      wordset->RegisterPokeNickName( 1, pPokeParam );
      wordset->Expand( pOutErrorMessage, pSourceBuf );

      GFL_SAFE_DELETE( pSourceBuf );

      return  false;
    }
    else
    if( m_pAppParam->in_param.bCanEsc == false )
    {
      /*
        @memo   きぜつやとんぼ返りなどの強制入れ替えでは必ず入れ替えることができるので
                ここの処理に来るのはトップ画面からのポケモン選択のみ
      */
      GFL_ASSERT( m_pAppParam->in_param.pFirstCurrentPokeParam );

      const pml::pokepara::PokemonParam* pFirstExChangeSourcePokeParam = m_pAppParam->in_param.m_pFirstExChangeSourcePokeParam->GetSrcData();

      gfl2::str::StrBuf* pSourceBuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( pOutErrorMessage->GetSize(), m_pAppHeap->GetDeviceHeap() );

      //  わざの影響で出れない(入れ替え元のポケモン名を出す)
      message_data->GetString( STR_CMD_WAZA_ERR, *pSourceBuf );
      wordset->RegisterPokeNickName( 1, pFirstExChangeSourcePokeParam );
      wordset->Expand( pOutErrorMessage, pSourceBuf );

      GFL_SAFE_DELETE( pSourceBuf );

      return  false;
    }

    return  true;
  }

  //  ポケモンが気絶した場合の入れ替えが可能かチェック(ポケモン位置指定)
  bool BattleUIPokeListFrame::_ChkPokeReplacePos( gfl2::str::StrBuf* pOutErrorMessage, const PokeExChangeIdx exChangeIdx )
  {
    GFL_ASSERT( pOutErrorMessage );

    print::WordSet* wordset           = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
    gfl2::str::MsgData* message_data  = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();

    const BTL_POKEPARAM*  pSettingPokeParam = _GetSettingPokeParam( exChangeIdx );
    if( pSettingPokeParam != NULL )
    {
      //  すでに配置されている
      gfl2::str::StrBuf* pSourceBuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( pOutErrorMessage->GetSize(), m_pAppHeap->GetDeviceHeap() );

      message_data->GetString( STR_CMD_SELECT_2, *pSourceBuf );
      wordset->RegisterPokeNickName( 1, pSettingPokeParam->GetSrcData() );
      wordset->Expand( pOutErrorMessage, pSourceBuf );

      GFL_SAFE_DELETE( pSourceBuf );

      return  false;
    }

    return  true;
  }

  //  ポケモン入れ替え位置指定フローに遷移するかどうか
  bool BattleUIPokeListFrame::_IsPokePostionSettingFlow()
  {
    if( m_pAppParam->in_param.frameType == FRAME_TYPE_REPLACE )
    {
      //  ダブルバトルで複数選択限定
      if( m_pAppParam->in_param.pPokeSelectResult->GetSelectMax() == 2 )
      {
        return  true;
      }
    }

    return  false;
  }

  //  指定配置位置にポケモンがいるか
  const BTL_POKEPARAM*  BattleUIPokeListFrame::_GetSettingPokeParam( const PokeExChangeIdx exChangeIdx )
  {
    app::BattleUIDataManager* pBattleParam  = m_pAppParam->in_param.pBtlParamAccessror;

    //  操作プレイヤーのポケモンが複数配置するケースはダブルしかない
    if( pBattleParam->GetBattleRule() == BTL_RULE_DOUBLE )
    {
      if( m_pAppParam->out_param.resutlData.aExchangeOccurred[ exChangeIdx ].btlPos != BTL_VPOS_ERROR )
      {
        //  配置している
        return pBattleParam->GetBattlePokeParam( m_pAppParam->out_param.resutlData.aExchangeOccurred[ exChangeIdx ].btlPos );
      }
    }

    return  NULL;
  }

  //  選択したポケモンデータから結果データを作成
  void BattleUIPokeListFrame::_CreateResultDataPokeChange( const ResultType resultType, const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pSelectPokeData )
  {
    GFL_ASSERT( pSelectPokeData );

    m_pAppParam->out_param.resutlData.type  = resultType;
    m_pAppParam->out_param.resutlData.data.changePoke.pParam  = pSelectPokeData->pParam;
    m_pAppParam->out_param.resutlData.data.changePoke.pokeIdx = pSelectPokeData->pokeIdx;
  }

  //  入力データを取得状態か
  bool BattleUIPokeListFrame::_IsInputCatch()
  {
    if( m_state == STATE_TYPE_UPDATE )
    {
      return  true;
    }
    //  メッセージ中でありながら、待ちのフラグがない場合は入力を受け付ける
    else
    if( m_state == STATE_TYPE_MESSAGE && ( m_bEventWait == false ) )
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief 特性インフォ表示ステートに遷移
  */
  //------------------------------------------------------------------
  void BattleUIPokeListFrame::_SetTokuseiInfoState( const u32 tokuseiId, const gfl2::math::Vector3& pos )
  {
    app::BattleUIDataManager* pBtlDataManager = m_pAppParam->in_param.pBtlParamAccessror;

    //  文字列バッファは一時用のを使用
    gfl2::str::StrBuf*  pTitleStrBuf    = pBtlDataManager->GetTempStrBuf( 0 );
    gfl2::str::StrBuf*  pInfoStrBuf     = pBtlDataManager->GetTempStrBuf( 1 );

    //  特性名と詳細のテキストを取得
    pBtlDataManager->OutputTokuseiStrBuf( pTitleStrBuf, pInfoStrBuf, tokuseiId );

    //  インフォウィンドウを開いて描画に追加
    {
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
  void BattleUIPokeListFrame::_SetItemInfoState( const u32 itemId, const gfl2::math::Vector3& pos )
  {
    app::BattleUIDataManager* pBtlDataManager = m_pAppParam->in_param.pBtlParamAccessror;

    //  文字列バッファは一時用のを使用
    gfl2::str::StrBuf*  pTitleStrBuf    = pBtlDataManager->GetTempStrBuf( 0 );
    gfl2::str::StrBuf*  pInfoStrBuf     = pBtlDataManager->GetTempStrBuf( 1 );

    //  アイテム名と説明のテキスト取得
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

  //  複数入れ替えidxからバトル配置idxを取得
  BtlvPos BattleUIPokeListFrame::ConvExChangeIdxFromBtlvPos( const PokeExChangeIdx pos )
  {
    const BtlvPos aBtlvPos[ LISTPOS_MAX ]  = 
    {
      BTL_VPOS_NEAR_1,
      BTL_VPOS_NEAR_2
    };

    return  aBtlvPos[ pos ];
  }

GFL_NAMESPACE_END( btl )
