//=============================================================================
/**
 * @file	 BtlvUITargetSelectFrame.h
 * @brief	 バトルUI対象選択画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.23
 */
//=============================================================================
#pragma once

#include "BtlvUITargetSelectFrame.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

//  nijiのインクルード
#include <system/include/HeapDefine.h>
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUITargetSelectFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

  u32  BattleUITargetSelectFrame::m_saConvPosIdTable[ app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX ][3] =
  {
     { BTL_VPOS_ROYAL_NEAR,   BTL_VPOS_NEAR_1,  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN, },
     { BTL_VPOS_ROYAL_RIGHT,  BTL_VPOS_NEAR_2,  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_DOWN,},
     { BTL_VPOS_ROYAL_FAR,    BTL_VPOS_FAR_1,   app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_UP,},
     { BTL_VPOS_ROYAL_LEFT,   BTL_VPOS_FAR_2,   app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP,},
  };

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUITargetSelectFrame::BattleUITargetSelectFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUITargetSelectFrame::~BattleUITargetSelectFrame()
  {
  }

  void BattleUITargetSelectFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    GFL_ASSERT( pAppParam->in_param.pAsyncFileManager );
    GFL_ASSERT( pAppParam->in_param.pBtlParamAccessror );
    GFL_ASSERT( pAppParam->in_param.pParentAppHeap );
    GFL_ASSERT( pAppParam->in_param.pParentDevHeap );
    GFL_ASSERT( pAppParam->in_param.pRenderingManager );
    GFL_ASSERT( pAppParam->in_param.pPokeParam );

    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUITargetSelectFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUITargetSelectFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPめもりーから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BtlvUITargetSelectFrame" );

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
  applib::frame::Result BattleUITargetSelectFrame::InitFunc(void)
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

    /// 読み込み開始をここに記述
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
      req.datId             = GARC_battleLowwerMenu_trglow_APPLYT;
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
      if( pAsyncFileMan->IsArcFileLoadDataFinished( &m_pArcReadBuff ) )
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
  applib::frame::Result BattleUITargetSelectFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_CREATE );
    }

    if( m_state == STATE_TYPE_CREATE )
    {
      if( m_pDraw->IsReady() )
      {
        _SetState( STATE_TYPE_OPEN );
      }
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw->GetState() == app::BattleUITargetSelectLowerDraw::STATE_TYPE_UPDATE )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      //  入力したボタンアニメ終了を待つ
      if( m_pDraw->IsButtonAnimation() == false )
      {
        //  ボタン終了
        End();
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      if( m_pDraw->GetState() == app::BattleUITargetSelectLowerDraw::STATE_TYPE_END )
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
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void BattleUITargetSelectFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
  applib::frame::Result BattleUITargetSelectFrame::EndFunc(void)
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
      //  入力結果を出力
      m_pAppParam->out_param.bEnd                 = true;

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
    * @brief 終了通知
    */
  //------------------------------------------------------------------
  void BattleUITargetSelectFrame::End()
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
      UCHIDA_PRINT( "warning: continuing BattleUITargetSelectFrame::End\n" );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 決定入力用リスナ
    */
  //------------------------------------------------------------------
  bool BattleUITargetSelectFrame::OnEnterInput( app::BattleUITargetSelectLowerDraw* pDrawInst, s32 buttonId )
  {
    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    if( buttonId == app::BattleUITargetSelectLowerDraw::BUTTON_ID_BACK )
    {
      m_pAppParam->out_param.selectItemPos  = pDrawInst->GetCurrentCursorPos();
    }
    else
    //  押したボタンが対象選択項目かチェック
    {
      const s32 aButtonIdToSelectItemPosMap[ app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX ][ 2 ] =
      {
        { app::BattleUITargetSelectLowerDraw::BUTTON_ID_LEFT_UP,    app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP     },
        { app::BattleUITargetSelectLowerDraw::BUTTON_ID_RIGHT_UP,   app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_UP    },
        { app::BattleUITargetSelectLowerDraw::BUTTON_ID_LEFT_DOWN,  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN   },
        { app::BattleUITargetSelectLowerDraw::BUTTON_ID_RIGHT_DOWN, app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_DOWN  },
      };

      for( s32 i = 0; i < app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX; ++i )
      {
        if( aButtonIdToSelectItemPosMap[ i ][ 0 ] == buttonId )
        {
          //  押したボタン項目からターゲット結果値を取得して設定
          const app::BattleUITargetSelectLowerDraw::INIT_PARAM& rDrawInitParam  = pDrawInst->GetInitParam();
          m_pAppParam->out_param.btlvPos        = (BtlvPos)rDrawInitParam.aSelectItem[ aButtonIdToSelectItemPosMap[ i ][ 1 ] ].btlvTargetPos;
          m_pAppParam->out_param.selectItemPos  = (app::BattleUITargetSelectLowerDraw::SelectItemPos)aButtonIdToSelectItemPosMap[ i ][ 1 ];

          break;
        }
      }
    }

    _SetState( STATE_TYPE_ENTER_EVENT );

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief キー入力通知用リスナ
    */
  //------------------------------------------------------------------
  bool BattleUITargetSelectFrame::OnKeyAction( app::BattleUITargetSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      pDrawInst->PushBtnBack();
      return  true;
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      pDrawInst->PushBtnCurrentCursor();
      return  true;
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ) )
    {
      if( pDrawInst->MoveCursorUp() == true )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }
      }

      return  true;
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) )
    {
      if( pDrawInst->MoveCursorDown() == true )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }
      }

      return  true;
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
    {
      if( pDrawInst->MoveCursorLeft() == true )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }
      }

      return  true;
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
    {
      if( pDrawInst->MoveCursorRight() == true )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }
      }

      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUITargetSelectFrame::_SetState( const StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else if( m_state == STATE_TYPE_CREATE )
    {
      //  画面クラスを作成
      {
        //  画面クラスに必要な初期化パラメータ設定
        app::BattleUITargetSelectLowerDraw::INIT_PARAM  initParam;
        {
          initParam.pLytBuff              = m_pArcReadBuff;
          initParam.pRenderingManager     = m_pAppParam->in_param.pRenderingManager;
          initParam.pUtilHeap             = m_pAppHeap;
          initParam.pBtlAppMsgData        = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();
          initParam.pBtlWordSet           = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
          initParam.pMenuCursorLytBuff    = m_pAppParam->in_param.pBtlParamAccessror->GetMenuCusrorLytResourceBuffer();
          initParam.pDelegate             = this;
          initParam.bUseInputSE           = m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE();
        }

        //  各選択項目のパラメータを設定
        {
          //  わざ範囲取得
          const WazaRange range   = _GetTargetWazaRange( m_pAppParam->in_param.wazaNo, m_pAppParam->in_param.pokeIndex );

          app::BattleUIDataManager*  pBattleParamAccessor  = m_pAppParam->in_param.pBtlParamAccessror;

          app::BattleUITargetSelectLowerDraw::SELECT_ITEM_PARAM* pSelectItemParam = NULL;

          const BtlRule rule  = pBattleParamAccessor->GetBattleRule();

          //  ルールごとに設定する項目ステータス
          {
            // @fix MCT要望 デバッグバトルのAIコントロールのポケモンの場合には対象となる選択データを切り替える
            if( pBattleParamAccessor->IsAICtrlTurn() == false )
            {
              const app::BattleUITargetSelectLowerDraw::SelectItemStatus aSelectItemStatusType[ BTL_RULE_INTRUDE + 1 ][ app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX ] =
              {
                //  BTL_RULE_SINGLE,    ///< シングル
                {
                  /*
                  SELECT_ITEM_POS_LEFT_DOWN = 0,
                  SELECT_ITEM_POS_RIGHT_DOWN,
                  SELECT_ITEM_POS_LEFT_UP,
                  SELECT_ITEM_POS_RIGHT_UP,
                  */
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                },

                //  BTL_RULE_DOUBLE,    ///< ダブル
                {
                  /*
                  SELECT_ITEM_POS_LEFT_DOWN = 0,
                  SELECT_ITEM_POS_RIGHT_DOWN,
                  SELECT_ITEM_POS_LEFT_UP,
                  SELECT_ITEM_POS_RIGHT_UP,
                  */
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                },

                //  BTL_RULE_ROYAL,    ///< ロイヤル
                {
                  /*
                  SELECT_ITEM_POS_LEFT_DOWN = 0,
                  SELECT_ITEM_POS_RIGHT_DOWN,
                  SELECT_ITEM_POS_LEFT_UP,
                  SELECT_ITEM_POS_RIGHT_UP,
                  */
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                },

                //  BTL_RULE_INTRUDE, // 乱入
                {
                  /*
                  SELECT_ITEM_POS_LEFT_DOWN = 0,
                  SELECT_ITEM_POS_RIGHT_DOWN,
                  SELECT_ITEM_POS_LEFT_UP,
                  SELECT_ITEM_POS_RIGHT_UP,
                  */
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                }
              };

              for( s32 i = 0; i < app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX; ++i )
              {
                initParam.aSelectItem[ i ].status = aSelectItemStatusType[ rule ][ i ];
              }

            }
            else
            {
              const app::BattleUITargetSelectLowerDraw::SelectItemStatus aSelectItemStatusType[ BTL_RULE_INTRUDE + 1 ][ app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX ] =
              {
                //  BTL_RULE_SINGLE,    ///< シングル
                {
                  /*
                  SELECT_ITEM_POS_LEFT_DOWN = 0,
                  SELECT_ITEM_POS_RIGHT_DOWN,
                  SELECT_ITEM_POS_LEFT_UP,
                  SELECT_ITEM_POS_RIGHT_UP,
                  */
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                },

                //  BTL_RULE_DOUBLE,    ///< ダブル
                {
                  /*
                  SELECT_ITEM_POS_LEFT_DOWN = 0,
                  SELECT_ITEM_POS_RIGHT_DOWN,
                  SELECT_ITEM_POS_LEFT_UP,
                  SELECT_ITEM_POS_RIGHT_UP,
                  */
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF,
                },

                //  BTL_RULE_ROYAL,    ///< ロイヤル
                {
                  /*
                  SELECT_ITEM_POS_LEFT_DOWN = 0,
                  SELECT_ITEM_POS_RIGHT_DOWN,
                  SELECT_ITEM_POS_LEFT_UP,
                  SELECT_ITEM_POS_RIGHT_UP,
                  */
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                },

                //  BTL_RULE_INTRUDE, // 乱入
                {
                  /*
                  SELECT_ITEM_POS_LEFT_DOWN = 0,
                  SELECT_ITEM_POS_RIGHT_DOWN,
                  SELECT_ITEM_POS_LEFT_UP,
                  SELECT_ITEM_POS_RIGHT_UP,
                  */
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA,
                  app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF,
                }
              };

              for( s32 i = 0; i < app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX; ++i )
              {
                initParam.aSelectItem[ i ].status = aSelectItemStatusType[ rule ][ i ];
              }
            }
          }

          //  使用わざポケモンの位置から項目のステータスを設定
          //  わざを放つ位置を取得
          s32 userPokeItemIdx = _GetTargetSelectSetting_UserPos( m_pAppParam->in_param.pokeIndex );
          initParam.aSelectItem[ userPokeItemIdx ].status = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA;

          //  各項目のデータ設定
          {
            typedef struct
            {
              s32           itemIdx;
              btl::BtlvPos  posId;

            } LOCAL_LIST_ITEM_CREATE_DATA;

            LOCAL_LIST_ITEM_CREATE_DATA aCreateDataTable[ app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX ]  =
            {
              //  左下
              {
                app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN,
                _ConvDrawSelectItemPosToBtlvPosId( app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN ),
              },

              //  右下
              {
                app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_DOWN,
                _ConvDrawSelectItemPosToBtlvPosId( app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_DOWN ),
              },

              //  左上
              {
                app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP,
                _ConvDrawSelectItemPosToBtlvPosId( app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP ),
              },

              //  右上
              {
                app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_UP,
                _ConvDrawSelectItemPosToBtlvPosId( app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_UP ),
              }
            };

            //  ロイヤルだとposidが異なる
            if( rule == BTL_RULE_ROYAL )
            {
              initParam.drawMode  = app::BattleUITargetSelectLowerDraw::DRAW_MODE_ROYAL;
            }
            else
            {
              initParam.drawMode  = app::BattleUITargetSelectLowerDraw::DRAW_MODE_NORMAL;
            }

            const LOCAL_LIST_ITEM_CREATE_DATA*  pCreateData = NULL;

            //  関数内を使うのにconstがじゃまになるのがあるのではずす
            BattleViewSystem*   pBtlViewSystem  = (BattleViewSystem*)pBattleParamAccessor->GetCore();

            const Savedata::MyStatus* pPlayerStatus   = NULL;

            const gfl2::str::StrBuf*  pNPCTrainerNameStrBuf = NULL;

            u32 loopNum = GFL_NELEMS( aCreateDataTable );
            for( u32 i = 0; i < loopNum; ++i )
            {
              pCreateData = &aCreateDataTable[ i ];
              pSelectItemParam  = &initParam.aSelectItem[ pCreateData->itemIdx ];

              BTL_POKEPARAM*  pBtlParam = pBattleParamAccessor->GetBattlePokeParam( pCreateData->posId );
              if( pBtlParam != NULL )
              {
                //  ロイヤル固有設定をする
                if( initParam.drawMode == app::BattleUITargetSelectLowerDraw::DRAW_MODE_ROYAL )
                {
                  u8  clientId  = pBtlViewSystem->GetBtlvPosToClientId( pCreateData->posId );
                  //  各位置からクライアントidを取得して名前を取る
                  {
                    pPlayerStatus = pBtlViewSystem->GetClientPlayerData( clientId );
                    if( pPlayerStatus != NULL )
                    {
                      pPlayerStatus->GetNameString( pSelectItemParam->aPlayerName );
                      pSelectItemParam->bNPCName  = false;
                    }
                    else
                    {
                      //  NPCのトレーナー名取得
                      pNPCTrainerNameStrBuf = pBtlViewSystem->GetClientTrainerName( &pSelectItemParam->playerNameAtribute, clientId );
                      if( pNPCTrainerNameStrBuf != NULL )
                      {
                        pNPCTrainerNameStrBuf->PutStr( pSelectItemParam->aPlayerName, GFL_NELEMS( pSelectItemParam->aPlayerName ) );
                        pSelectItemParam->bNPCName  = true;
                      }
                      else
                      {
                        //  名がないのはありえない
                        GFL_ASSERT( 0 );
                      }
                    }
                  }

                  //  コーナー色を取得
                  {
                    pSelectItemParam->cornerColor = GetCornerColorFromBtlClientId( (BTL_CLIENT_ID)clientId );
                  }
                }

                //  死亡している場合はアイコンなどは非表示なので関連するデータはなにも設定しない
                if( pBtlParam->IsDead() == false )
                {
                  pSelectItemParam->pBtlPokeParam = pBtlParam;
                }
              }

              //  ボタンを押した時の結果取得に必要なので、どんな状態でも位置は設定する
              pSelectItemParam->btlvTargetPos = pCreateData->posId;
            }
          }

          //  技選択設定
          {
            if( rule == BTL_RULE_SINGLE )
            {
              //  そもそもシングル対戦では対象選択には来ない
              GFL_ASSERT_STOP( 0 );
            }
            else
            if( ( rule == BTL_RULE_DOUBLE )
            ||  ( rule == BTL_RULE_INTRUDE ) )
            {
              //  ターゲット選択データ出力
              _OutputDoubleTargetSelectData( &initParam, range, m_pAppParam->in_param.firstCursorPos );
            }
            else
            if( rule == BTL_RULE_ROYAL )
            {
              //  ターゲット選択データ出力
              _OutputRoyaleTargetSelectData( &initParam, range, m_pAppParam->in_param.firstCursorPos );
            }
            else
            {
              //  ルール指定以外はNG
              GFL_ASSERT( 0 );
            }
          }

          //  選択対象のみ効果タイプを設定
          {
            u32 loopNum = GFL_NELEMS( initParam.aSelectItem );
            for( u32 i = 0; i < loopNum; ++i )
            {
              pSelectItemParam  = &initParam.aSelectItem[ i ];

              //  選択できないあるいは死亡している場合は pBtlPokeParam == NULL になっている
              //  ・死亡している場合はアイコンなどは非表示なので関連するデータは何もないので効果は出さない
              //  ・選択非対称のポケモンは効果を出さない
              if( ( pSelectItemParam->pBtlPokeParam != NULL )
              &&  ( pSelectItemParam->bSelectPermission == true ) )
              {
                if( pSelectItemParam->status != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA )
                {
                  BtlTypeAffAbout type  = m_pAppParam->in_param.pBtlParamAccessror->GetPokePosListKoukaType( 
                  &pSelectItemParam->bKoukaTypePutFlg, 
                  m_pAppParam->in_param.wazaNo, 
                  m_pAppParam->in_param.pPokeParam->GetID(), 
                  pSelectItemParam->pBtlPokeParam->GetID() );

                  pSelectItemParam->koukaType = type;
                }
              }
            }
          }
        }

        m_pDraw = GFL_NEW( m_pAppHeap->GetSystemHeap() ) app::BattleUITargetSelectLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );

        m_pDraw->Initialize();

        m_pDraw->SetInputEnabled( false );
        m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
      }
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
      //  開始
      m_pDraw->Start();
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pDraw->SetInputEnabled( m_pAppParam->in_param.pBtlParamAccessror->GetCore()->IsControllable() );
    }
    else
    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      // @fix NMCat[3430] 入力イベントでは入力は無効にしないと排他にならない
      m_pDraw->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      m_pDraw->SetInputEnabled( false );

      //  終了
      m_pDraw->End();
    }
    else if( m_state == STATE_TYPE_END )
    {
    }
  }

  /**
   * @brief 対象選択画面の動作設定( わざを使用するポケモンの立ち位置 )
   * @param[in]  poke_index  ポケモンのパーティ内インデックス
   * @return わざを使用するポケモンの立ち位置
   */
  u32 BattleUITargetSelectFrame::_GetTargetSelectSetting_UserPos( u8 poke_index ) const
  {
    app::BattleUIDataManager* pBattleParamAccessor  = m_pAppParam->in_param.pBtlParamAccessror;

    const u8 clientID     = pBattleParamAccessor->GetClientID();
    const BtlvPos btlvPos = pBattleParamAccessor->GetCore()->GetClientIdToBtlvPos( clientID, poke_index );

    return  _ConvBtlvPosIdToDrawSelectItemPos( btlvPos );
  } 

  //  PosIdから描画項目idへ変換
  app::BattleUITargetSelectLowerDraw::SelectItemPos BattleUITargetSelectFrame::_ConvBtlvPosIdToDrawSelectItemPos( const BtlvPos btlvPos ) const
  {
    app::BattleUIDataManager* pBattleParamAccessor  = m_pAppParam->in_param.pBtlParamAccessror;
    const BtlRule rule  = pBattleParamAccessor->GetBattleRule();

    if( rule == BTL_RULE_ROYAL )
    {
      for( u32 i = 0; i < app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX; ++i )
      {
        if( m_saConvPosIdTable[ i ][ 0 ] == (u32)btlvPos )
        {
          return  (app::BattleUITargetSelectLowerDraw::SelectItemPos)m_saConvPosIdTable[ i ][ 2 ];
        }
      }
    }
    else
    {
      for( u32 i = 0; i < app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX; ++i )
      {
        if( m_saConvPosIdTable[ i ][ 1 ] == (u32)btlvPos )
        {
          return  (app::BattleUITargetSelectLowerDraw::SelectItemPos)m_saConvPosIdTable[ i ][ 2 ];
        }
      }
    }

    return app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN;
  }

  //  描画項目idからPosIdへ変換
  BtlvPos BattleUITargetSelectFrame::_ConvDrawSelectItemPosToBtlvPosId( const app::BattleUITargetSelectLowerDraw::SelectItemPos drawPos ) const
  {
    app::BattleUIDataManager* pBattleParamAccessor  = m_pAppParam->in_param.pBtlParamAccessror;
    const BtlRule rule  = pBattleParamAccessor->GetBattleRule();

    for( u32 i = 0; i < app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX; ++i )
    {
      if( m_saConvPosIdTable[ i ][ 2 ] == (u32)drawPos )
      {
        if( rule == BTL_RULE_ROYAL )
        {
          return  (BtlvPos)m_saConvPosIdTable[ i ][ 0 ];
        }
        else
        {
          return  (BtlvPos)m_saConvPosIdTable[ i ][ 1 ];
        }
      }

    }

    return BTL_VPOS_NEAR_1;
  }

  //------------------------------------------------------------------
  /**
    * @brief 対象選択画面の動作設定( わざの効果範囲 )
    * @param wazano  使用する技
    * @return 技の効果範囲
  */
  //------------------------------------------------------------------
  BattleUITargetSelectFrame::WazaRange BattleUITargetSelectFrame::_GetTargetWazaRange( const WazaNo wazano, const u8 poke_index ) const
  {
    pml::wazadata::WazaTarget waza_target = static_cast<pml::wazadata::WazaTarget>( pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_TARGET ) );
    
    // のろい用特殊処理
    if( wazano == WAZANO_NOROI )
    {
      const BTL_PARTY* party = m_pAppParam->in_param.pBtlParamAccessror->GetBattleParty_Self();
      const BTL_POKEPARAM* param = party->GetMemberDataConst( poke_index );
      waza_target = btl::calc::GetNoroiTargetType(param);
    }

    switch( waza_target ) {
    default:  break;

    case pml::wazadata::TARGET_OTHER_SELECT:       ///< 通常ポケ（１体選択）
      if( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_TripleFar ) )
      {
        return WAZA_RANGE_OTHER_ONE_LONG;
      }
      else
      {
        return WAZA_RANGE_OTHER_ONE;
      }
      ///< 自分を含む味方ポケ（１体選択）
    case pml::wazadata::TARGET_FRIEND_USER_SELECT:
      return WAZA_RANGE_FRIEND_ONE;

      ///< 自分以外の味方ポケ（１体選択）
    case pml::wazadata::TARGET_FRIEND_SELECT:
      return WAZA_RANGE_FRIEND_OTHER_ONE;

      ///< 相手側ポケ（１体選択）
    case pml::wazadata::TARGET_ENEMY_SELECT:
      return WAZA_RANGE_ENEMY_ONE;

      ///< 自分以外の全ポケ
    case pml::wazadata::TARGET_OTHER_ALL:
      return WAZA_RANGE_OTHER_ALL;

      ///< 相手側全ポケ
    case pml::wazadata::TARGET_ENEMY_ALL:
      return WAZA_RANGE_ENEMY_ALL;

      ///< 味方側全ポケ
    case pml::wazadata::TARGET_FRIEND_ALL:
      return WAZA_RANGE_FRIEND_ALL;

      ///< 自分のみ
    case pml::wazadata::TARGET_USER:
      return WAZA_RANGE_USER;

      ///< 場に出ている全ポケ
    case pml::wazadata::TARGET_ALL:
      return WAZA_RANGE_ALL;

      ///< 相手ポケ１体ランダム
    case pml::wazadata::TARGET_ENEMY_RANDOM:
      return WAZA_RANGE_ENEMY_ONE_RANDOM;

      ///< 場全体（天候など）
    case pml::wazadata::TARGET_FIELD:
      return WAZA_RANGE_FIELD_ALL;

      ///< 敵側陣営
    case pml::wazadata::TARGET_SIDE_ENEMY:
      return WAZA_RANGE_FIELD_ENEMY;

      ///< 自分側陣営
    case pml::wazadata::TARGET_SIDE_FRIEND:
      return WAZA_RANGE_FIELD_FRIEND;

      ///< ゆびをふるなど特殊型
    case pml::wazadata::TARGET_UNKNOWN:
      return WAZA_RANGE_UNKNOWN;
    }

    return WAZA_RANGE_UNKNOWN;
  }

  //------------------------------------------------------------------
  /**
    * @brief 対象選択画面の選択データ出力(各ルールごとに用意)
  */
  //------------------------------------------------------------------
  void BattleUITargetSelectFrame::_OutputDoubleTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const WazaRange range, const s32 firstCursorPos )
  {
    GFL_ASSERT( pOutputData );

    //  パターン１
    {
      // 自分以外の1ポケ
      // 自分以外の1ポケ( 遠距離技 )
      if( ( range == WAZA_RANGE_OTHER_ONE ) ||
          ( range == WAZA_RANGE_OTHER_ONE_LONG ) )
      {
        _OutputUseSelfOtherTargetSelectData( pOutputData, firstCursorPos );
      }
    }

    //  パターン2(複数対象)
    {
      // 相手側の全ポケ
      // 相手場
      if( ( range == WAZA_RANGE_ENEMY_ALL ) ||
          ( range == WAZA_RANGE_FIELD_ENEMY ) )
      {
        //  敵のみまとめて選択対象に
        for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
        {
          if( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY )
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = true;
            pOutputData->aSelectItem[ i ].btlvTargetPos     = 0;
          }
          else
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = false;
            pOutputData->aSelectItem[ i ].btlvTargetPos     = BTL_VPOS_ERROR;
          }
        }

        //  初期カーソルを取得
        app::BattleUITargetSelectLowerDraw::SelectItemPos selectItemPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP;
        if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
        {
          selectItemPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
        }
        pOutputData->cursorItemPos  = _GetSelectItemCursorPos( selectItemPos, *pOutputData );

        //  連結設定
        {
          pOutputData->aLinkItemPos[ 0 ] = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_UP;
          pOutputData->aLinkItemPos[ 1 ] = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP;
        }
      }
    }

    //  パターン3(複数対象)
    {
      // 自分以外の全ポケ
      if( range == WAZA_RANGE_OTHER_ALL )
      {
        _OutputOtherAllTargetSelectData( pOutputData, firstCursorPos );
      }
    }

    //  パターン4(複数対象)
    {
      // 場全体
      // 自分を含む全ポケ
      if( ( range == WAZA_RANGE_FIELD_ALL ) ||
          ( range == WAZA_RANGE_ALL ) )
      {
        _OutputAllTaragetSelectData( pOutputData, firstCursorPos );
      }
    }

    //  パターン5
    {
      // 自分のみ
      // 特殊型
      // 相手側の1ポケランダム
      if( ( range == WAZA_RANGE_USER ) ||
          ( range == WAZA_RANGE_UNKNOWN ) || 
          ( range == WAZA_RANGE_ENEMY_ONE_RANDOM ))
      {
        _OutputUseSelfOnlyTargetSelectData( pOutputData, firstCursorPos );
      }
    }

    //  パターン6(複数対象)
    {
      // 味方場
      // 自分を含む全味方ポケ
      if( ( range == WAZA_RANGE_FIELD_FRIEND ) ||
          ( range == WAZA_RANGE_FRIEND_ALL ) )
      {
        //  自分と味方ポケをまとめて選択
        for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
        {
          if( ( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF ) ||
              ( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA ) )
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = true;
            pOutputData->aSelectItem[ i ].btlvTargetPos     = 0;
          }
          else
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = false;
            pOutputData->aSelectItem[ i ].btlvTargetPos     = BTL_VPOS_ERROR;
          }
        }

        //  初期カーソルを取得
        app::BattleUITargetSelectLowerDraw::SelectItemPos selectItemPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN;
        if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
        {
          selectItemPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
        }
        pOutputData->cursorItemPos  = _GetSelectItemCursorPos( selectItemPos, *pOutputData );

        //  連結設定
        {
          pOutputData->aLinkItemPos[ 0 ] = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_DOWN;
          pOutputData->aLinkItemPos[ 1 ] = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN;
        }
      }
    }

    //  パターン7
    {
      // 自分以外の1味方ポケ
      if( range == WAZA_RANGE_FRIEND_OTHER_ONE )
      {
        //  味方ポケのみ選択
        for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
        {
          if( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF )
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = true;
          }
          else
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = false;
            pOutputData->aSelectItem[ i ].btlvTargetPos     = BTL_VPOS_ERROR;
          }
        }

        //  初期カーソルを取得
        app::BattleUITargetSelectLowerDraw::SelectItemPos selectItemPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_RIGHT_DOWN;
        if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
        {
          selectItemPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
        }
        pOutputData->cursorItemPos  = _GetSelectItemCursorPos( selectItemPos, *pOutputData );
      }
    }

    //  パターン8
    {
      // 味方側の1ポケ
      if( range == WAZA_RANGE_FRIEND_ONE )
      {
        //  自分と味方のどれか１つを選択
        for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
        {
          if( ( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF ) ||
            ( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA ) )
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = true;
          }
          else
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = false;
            pOutputData->aSelectItem[ i ].btlvTargetPos     = BTL_VPOS_ERROR;
          }
        }

        //  初期カーソルを取得
        app::BattleUITargetSelectLowerDraw::SelectItemPos selectItemPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN;
        if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
        {
          selectItemPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
        }
        pOutputData->cursorItemPos  = _GetSelectItemCursorPos( selectItemPos, *pOutputData );
      }
    }

    //  パターン9
    {
      // 相手側の1ポケ
      if( range == WAZA_RANGE_ENEMY_ONE )
      {
        //  敵ポケをどれか１つを選択
        for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
        {
          if( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_ENEMY )
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = true;
          }
          else
          {
            pOutputData->aSelectItem[ i ].bSelectPermission = false;
            pOutputData->aSelectItem[ i ].btlvTargetPos     = BTL_VPOS_ERROR;
          }
        }

        //  初期カーソルを取得
        app::BattleUITargetSelectLowerDraw::SelectItemPos selectItemPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP;
        if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
        {
          selectItemPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
        }
        pOutputData->cursorItemPos  = _GetSelectItemCursorPos( selectItemPos, *pOutputData );
      }
    }
  }

  void BattleUITargetSelectFrame::_OutputRoyaleTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const WazaRange range, const s32 firstCursorPos )
  {
    GFL_ASSERT( pOutputData );

    //  バターン1
    {
      // 自分以外の1ポケ
      // 自分以外の1ポケ( 遠距離技 )
      // 相手側の1ポケ
      // 自分以外の1味方ポケ
      if( ( range == WAZA_RANGE_OTHER_ONE )         ||
          ( range == WAZA_RANGE_OTHER_ONE_LONG )    ||
          ( range == WAZA_RANGE_ENEMY_ONE )         ||
          ( range == WAZA_RANGE_FRIEND_OTHER_ONE ) )
      {
        _OutputUseSelfOtherTargetSelectData( pOutputData, firstCursorPos );
      }
    }

    //  バターン2(複数対象)
    {
      // 相手側の全ポケ
      // 相手場
      // 自分以外の全ポケ
      if( ( range == WAZA_RANGE_ENEMY_ALL )   ||
          ( range == WAZA_RANGE_FIELD_ENEMY ) ||
          ( range == WAZA_RANGE_OTHER_ALL ) )
      {
        _OutputOtherAllTargetSelectData( pOutputData, firstCursorPos );
      }
    }

    //  バターン3(全対象)
    {
      // 場全体
      // 自分を含む全ポケ
      if( ( range == WAZA_RANGE_FIELD_ALL ) ||
          ( range == WAZA_RANGE_ALL ) )
      {
        _OutputAllTaragetSelectData( pOutputData, firstCursorPos );
      }
    }

    //  バターン4
    {
      // 自分のみ
      // 特殊型
      // 相手側の1ポケランダム
      // 味方場
      // 味方側の1ポケ
      // 自分を含む全味方ポケ
      if( ( range == WAZA_RANGE_USER )              ||
          ( range == WAZA_RANGE_UNKNOWN )           ||
          ( range == WAZA_RANGE_ENEMY_ONE_RANDOM )  ||
          ( range == WAZA_RANGE_FIELD_FRIEND )      ||
          ( range == WAZA_RANGE_FRIEND_ONE )        ||
          ( range == WAZA_RANGE_FRIEND_ALL ) )
      {
        _OutputUseSelfOnlyTargetSelectData( pOutputData, firstCursorPos );
      }
    }
  }

  void BattleUITargetSelectFrame::_OutputIntrusionTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const WazaRange range, const s32 firstCursorPos )
  {
    GFL_ASSERT( pOutputData );

    //  ダブルルール処理でも兼用可能
    _OutputDoubleTargetSelectData( pOutputData, range, firstCursorPos );
  }

  //  自分以外のどれか１つを選択時のデータ出力
  void BattleUITargetSelectFrame::_OutputUseSelfOtherTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const s32 firstCursorPos )
  {
    //  自分以外のポケをどれか１つ選択
    for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
    {
      //  自身を非対象に
      if( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA )
      {
        pOutputData->aSelectItem[ i ].bSelectPermission = false;
        pOutputData->aSelectItem[ i ].btlvTargetPos     = BTL_VPOS_ERROR;
      }
      //  自身も選択対象に
      else
      {
        pOutputData->aSelectItem[ i ].bSelectPermission = true;
      }
    }

    //  初期カーソルを取得
    {
      app::BattleUITargetSelectLowerDraw::SelectItemPos cursorPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP;
      if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
      {
        cursorPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
      }
      pOutputData->cursorItemPos  = _GetSelectItemCursorPos( cursorPos, *pOutputData );
    }
  }

  //  自分以外のすべてをまとめて選択時のデータ出力
  void BattleUITargetSelectFrame::_OutputOtherAllTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const s32 firstCursorPos )
  {
    s32 linkItemIdx = 0;

    //  自分以外はすべてまとめて選択対象に
    for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
    {
      if( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA )
      {
        pOutputData->aSelectItem[ i ].bSelectPermission = false;
        pOutputData->aSelectItem[ i ].btlvTargetPos     = BTL_VPOS_ERROR;
      }
      else
      {
        pOutputData->aSelectItem[ i ].bSelectPermission = true;
        pOutputData->aSelectItem[ i ].btlvTargetPos     = 0;

        //  連結設定
        pOutputData->aLinkItemPos[ linkItemIdx ]  = (app::BattleUITargetSelectLowerDraw::SelectItemPos)i;
        ++linkItemIdx;
      }
    }

    //  初期カーソルを取得
    {
      app::BattleUITargetSelectLowerDraw::SelectItemPos cursorPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP;
      if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
      {
        cursorPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
      }
      pOutputData->cursorItemPos  = _GetSelectItemCursorPos( cursorPos, *pOutputData );
    }
  }

  //  すべてをまとめて選択時のデータ出力
  void BattleUITargetSelectFrame::_OutputAllTaragetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const s32 firstCursorPos )
  {
    s32 linkItemIdx = 0;

    //  すべてのポケがまとめて対象
    for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
    {
      pOutputData->aSelectItem[ i ].bSelectPermission = true;
      //  全体位置
      pOutputData->aSelectItem[ i ].btlvTargetPos     = 0;

      //  連結設定
      pOutputData->aLinkItemPos[ linkItemIdx ]  = (app::BattleUITargetSelectLowerDraw::SelectItemPos)i;
      ++linkItemIdx;
    }

    //  初期カーソルを取得
    {
      app::BattleUITargetSelectLowerDraw::SelectItemPos cursorPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP;
      if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
      {
        cursorPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
      }
      pOutputData->cursorItemPos  = _GetSelectItemCursorPos( cursorPos, *pOutputData );
    }
  }

  //  自分自身のみ選択時のデータ出力
  void BattleUITargetSelectFrame::_OutputUseSelfOnlyTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const s32 firstCursorPos )
  {
    //  自分のみ選択
    for( s32 i = 0; i < GFL_NELEMS( pOutputData->aSelectItem ); ++i )
    {
      if( pOutputData->aSelectItem[ i ].status == app::BattleUITargetSelectLowerDraw::SELECT_ITEM_STATUS_SELF_USE_WAZA )
      {
        pOutputData->aSelectItem[ i ].bSelectPermission = true;
      }
      else
      {
        pOutputData->aSelectItem[ i ].bSelectPermission = false;
        pOutputData->aSelectItem[ i ].btlvTargetPos     = BTL_VPOS_ERROR;
      }
    }

    //  初期カーソルを取得
    {
      app::BattleUITargetSelectLowerDraw::SelectItemPos cursorPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN;
      if( firstCursorPos != app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE )
      {
        cursorPos = (app::BattleUITargetSelectLowerDraw::SelectItemPos)firstCursorPos;
      }
      pOutputData->cursorItemPos  = _GetSelectItemCursorPos( cursorPos, *pOutputData );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief カーソル位置を取得
    * @param beginCursorPos : 初回カーソル位置を指定する、もし初回カーソル位置を設定できない場合はチェックする
    */
  //------------------------------------------------------------------
  app::BattleUITargetSelectLowerDraw::SelectItemPos BattleUITargetSelectFrame::_GetSelectItemCursorPos( app::BattleUITargetSelectLowerDraw::SelectItemPos beginCursorPos, app::BattleUITargetSelectLowerDraw::INIT_PARAM& rDrawInitParam )
  {
    s32 selectItemMax = GFL_NELEMS( rDrawInitParam.aSelectItem );
    s32 chkCursorPos  = (s32)beginCursorPos;

    if( selectItemMax <= chkCursorPos )
    {
      GFL_ASSERT( 0 );
      return  beginCursorPos;
    }

    app::BattleUITargetSelectLowerDraw::SELECT_ITEM_PARAM*  pSelectItemParam  = NULL;

    //  初回位置を選択できるか
    pSelectItemParam  = &rDrawInitParam.aSelectItem[ chkCursorPos ];
    //  pBtlPokeParam == NULL の時は選択対象ポケモンが死亡しているが、それでも選択は空枠として可能
    if( pSelectItemParam->bSelectPermission )
    {
      return  (app::BattleUITargetSelectLowerDraw::SelectItemPos)chkCursorPos;
    }

    // @fix NMCat[1267] 左上の項目からチェックするようにする
    /*
        検索順として
          左上　－＞　右上　－＞　左下　－＞　右下
    */                  
    u32 chkItemPos  = (u32)app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_UP;
    for( s32 i = 0; i < selectItemMax; ++i )
    {
      pSelectItemParam  = &rDrawInitParam.aSelectItem[ chkItemPos ];
      //  pBtlPokeParam == NULL の時は選択対象ポケモンが死亡しているが、それでも選択は空枠として可能
      if( pSelectItemParam->bSelectPermission )
      {
        return  (app::BattleUITargetSelectLowerDraw::SelectItemPos)chkItemPos;
      }

      ++chkItemPos;
      if( selectItemMax <= chkItemPos )
      {
        chkItemPos  = (u32)app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_LEFT_DOWN;
      }
    }

    //  どれも選択できないのでカーソル選択できないのでは
    GFL_ASSERT( 0 );
    return  beginCursorPos;
  }

GFL_NAMESPACE_END( btl )
