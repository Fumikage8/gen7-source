//=============================================================================
/**
 * @file	 BtlvUIPokeEntryConfirmFrame.h
 * @brief	 バトルUIポケモン参加確認画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.04.23
 */
//=============================================================================
#pragma once

#include "BtlvUIPokeEntryConfirmFrame.h"

#include "Battle/source/btl_ClientPublicInformation.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

//  nijiのインクルード
#include <system/include/HeapDefine.h>
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUIPokeEntryConfirmFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUIPokeEntryConfirmFrame::BattleUIPokeEntryConfirmFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUIPokeEntryConfirmFrame::~BattleUIPokeEntryConfirmFrame()
  {
  }

  void BattleUIPokeEntryConfirmFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIPokeEntryConfirmFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIPokeEntryConfirmFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPめもりーから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUIPokeEntryConfirmFrame" );

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
  applib::frame::Result BattleUIPokeEntryConfirmFrame::InitFunc(void)
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

      SEQ_END
    };

    int seq = applib::frame::CellSuper::GetSubSeq();

    //  読み込み開始をここに記述
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

    else
    if( seq == SEQ_LOAD_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

      req.arcId             = s_arcId;
      req.datId             = GARC_battleLowwerMenu_opplow_APPLYT;
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

    else
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
      //  画面クラスを作成
      if( m_bEnd == false )
      {
        m_pDrawButtonCtrl = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUIPokeEntryConfirmtLowerButtonCtrl( this );

        app::BattleUIPokeEntryConfirmLowerDraw::INIT_PARAM  drawInitParam;

        app::BattleUIDataManager*  pUIDataManager = m_pAppParam->in_param.pBtlParamAccessror;

        //  画面クラスに必要な初期化パラメータ設定
        {
          drawInitParam.pLytBuff          = m_pArcReadBuff;
          drawInitParam.pRenderingManager = m_pAppParam->in_param.pRenderingManager;
          drawInitParam.pUtilHeap         = m_pAppHeap;
          drawInitParam.pButtonCtrl       = m_pDrawButtonCtrl;
          drawInitParam.pBtlAppMsgData    = pUIDataManager->GetAppMesssageData();
          drawInitParam.pBtlWordSet       = pUIDataManager->GetWordSet();
          drawInitParam.bUseInputSE       = pUIDataManager->IsPlayInputSE();
          drawInitParam.pAppCommonGrpIconData = &pUIDataManager->GetAppCommonGrpIconData();
        }

        m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUIPokeEntryConfirmLowerDraw( drawInitParam );
        GFL_ASSERT( m_pDraw );

        m_pDraw->Initialize();

        //  描画パラメータを作成
        {
          app::BattleUIPokeEntryConfirmLowerDraw::PARTY_DATA* pActivePartyData  = NULL;
          app::BattleUIPokeEntryConfirmLowerDraw::POKE_DATA*  pActivePokeData   = NULL;

          //  通信対戦トレーナー情報を取得して描画パラメータに設定
          app::BattleUIDataManager::ENEMY_TRAINER_PARAM enemyTrainerParam;
          MonsNo  monsNo  = MONSNO_NULL;
#if 0

//  test data
          m_drawParamPartyListIdxMax  = 3;
          m_pDrawParamPartyList = GFL_NEW_ARRAY( m_pAppHeap->GetDeviceHeap() ) app::BattleUIPokeEntryConfirmLowerDraw::PARTY_DATA[ m_drawParamPartyListIdxMax ];

          for( u32 partyListIdx = 0; partyListIdx < m_drawParamPartyListIdxMax; ++partyListIdx )
          {
            //  描画パラメータ設定
            pActivePartyData = &m_pDrawParamPartyList[ partyListIdx ];
            pActivePartyData->b_enemy_party  = true;

            for( u32 pokeIdx = 0; pokeIdx < BTL_PARTY_MEMBER_MAX; ++pokeIdx )
            {
              pActivePokeData = &pActivePartyData->poke_data[ pokeIdx ];

              monsNo  = MONSNO_RIZAADO;

              //  ポケモンがいればパラメータ構造体を作成
              pml::pokepara::InitialSpec  initParam;
              if( partyListIdx == 0 )
              {
                monsNo  = MONSNO_RIZAADO;
                initParam.level     = 10;
              }
              else
              {
                monsNo  = MONSNO_KAMEKKUSU;
                initParam.level     = 1;
              }
              
              initParam.monsno    = monsNo;

              pml::pokepara::PokemonParam*  pNewPokeParam = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) pml::pokepara::PokemonParam( m_pAppHeap->GetDeviceHeap(), initParam );

              //  アイテムidは適当、用はアイテムを持っているかどうかが判別できればいい
              if( partyListIdx == 0 )
              {
                if( pokeIdx == 0 )
                {
                  pNewPokeParam->SetItem( ITEM_MASUTAABOORU );
                  pNewPokeParam->SetSick( pml::pokepara::SICK_DOKU );

                  pActivePokeData->b_on_battle_field  = true;
                }
                else
                {
                  pActivePokeData->b_on_battle_field  = false;
                }
              }
              else
              {
                if( pokeIdx == 1 )
                {
                  pNewPokeParam->SetItem( ITEM_MASUTAABOORU );
                  pNewPokeParam->SetSick( pml::pokepara::SICK_DOKU );

                  pActivePokeData->b_on_battle_field  = true;
                }
                else
                {
                  pActivePokeData->b_on_battle_field  = false;
                }
              }
              
              pActivePokeData->p_poke_param = pNewPokeParam;
            }
          }
#else
          m_drawParamPartyListIdxMax  = pUIDataManager->GetEnemyTrainerNum();
          m_pDrawParamPartyList = GFL_NEW_ARRAY( m_pAppHeap->GetDeviceHeap() ) app::BattleUIPokeEntryConfirmLowerDraw::PARTY_DATA[ m_drawParamPartyListIdxMax ];

          for( u32 partyListIdx = 0; partyListIdx < m_drawParamPartyListIdxMax; ++partyListIdx )
          {
            //  パラメータ取得
            pUIDataManager->OutputEnemyTrainerParam( &enemyTrainerParam, partyListIdx );

            //  描画パラメータ設定
            pActivePartyData = &m_pDrawParamPartyList[ partyListIdx ];

            pActivePartyData->rate = enemyTrainerParam.ratingNum;
            pActivePartyData->b_enemy_party   = true;
            pActivePartyData->b_rating        = enemyTrainerParam.bRating;
            pActivePartyData->p_player_name   = enemyTrainerParam.pName;
            pActivePartyData->player_grammer  = enemyTrainerParam.grammer;

            u32 pokeIdxLoopNum = GFL_NELEMS( pActivePartyData->poke_data );
            
            GFL_ASSERT( pokeIdxLoopNum == BTL_PARTY_MEMBER_MAX );
            for( u32 pokeIdx = 0; pokeIdx < pokeIdxLoopNum; ++pokeIdx )
            {
              pActivePokeData = &pActivePartyData->poke_data[ pokeIdx ];
              pActivePokeData->b_on_battle_field  = enemyTrainerParam.pInfoParam->IsPokemonAppearedOnBattleField( pokeIdx );

              monsNo  = enemyTrainerParam.pInfoParam->GetPokemonMonsterNo( pokeIdx );

              //  ポケモンがいればパラメータ構造体を作成
              if( monsNo != MONSNO_NULL )
              {
                pml::pokepara::InitialSpec  initParam;
                // @fix NMCat[2102] フォルムNO指定を忘れいた。
                initParam.formno    = enemyTrainerParam.pInfoParam->GetPokemonFormNo( pokeIdx );

                initParam.level     = enemyTrainerParam.pInfoParam->GetPokemonLevel( pokeIdx );
                initParam.monsno    = monsNo;
                initParam.sex       = enemyTrainerParam.pInfoParam->GetPokemonSex( pokeIdx );

                pml::pokepara::PokemonParam*  pNewPokeParam = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) pml::pokepara::PokemonParam( m_pAppHeap->GetDeviceHeap(), initParam );

                bool  bHaveItem     = enemyTrainerParam.pInfoParam->HavePokemonItem( pokeIdx );
                if( bHaveItem == true )
                {
                  //  アイテムidは適当、ようはアイテムを持っているかどうかが判別できればいい
                  pNewPokeParam->SetItem( ITEM_MASUTAABOORU );
                }

                pNewPokeParam->SetSick( enemyTrainerParam.pInfoParam->GetPokemonSick( pokeIdx ) );

                pActivePokeData->p_poke_param = pNewPokeParam;
              }
              else
              {
                pActivePartyData->poke_data[ pokeIdx ].p_poke_param = NULL;
              }
            }
          }
#endif

          //  パラメータ設定
          m_pDraw->SetParam( m_pDrawParamPartyList[ 0 ], false, ( 1 < m_drawParamPartyListIdxMax ) );
        }
      }

      ++seq;
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
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIPokeEntryConfirmFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw->GetState() == app::BattleUIPokeEntryConfirmLowerDraw::STATE_TYPE_INPUT_IDLE )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
    }

    //  画面終了入力では入力アニメが終わるまで待つ
    if( m_state == STATE_TYPE_END_ENTER )
    {
      //  ボタンアニメ終了を待つ
      if( m_pDrawButtonCtrl->IsAnimEnd() )
      {
        //  画面終了
        End();
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      if( ( m_pDraw == NULL ) || ( m_pDraw->GetState() == app::BattleUIPokeEntryConfirmLowerDraw::STATE_TYPE_END ) )
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
  void BattleUIPokeEntryConfirmFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
  applib::frame::Result BattleUIPokeEntryConfirmFrame::EndFunc(void)
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
      m_pAppParam->out_param.bEnd                 = true;

      {
        app::BattleUIPokeEntryConfirmLowerDraw::PARTY_DATA* pActivePartyData = NULL;

        u32 partyListloopNum  = m_drawParamPartyListIdxMax;
        for( u32 partyListIdx = 0; partyListIdx < partyListloopNum; ++partyListIdx )
        {
          pActivePartyData = &m_pDrawParamPartyList[ partyListIdx ];

          u32 pokeIdxLoopNum = GFL_NELEMS( pActivePartyData->poke_data );
          for( u32 pokeIdx = 0; pokeIdx < pokeIdxLoopNum; ++pokeIdx )
          {
            GFL_SAFE_DELETE( pActivePartyData->poke_data[ pokeIdx ].p_poke_param );
          }
        }

        GFL_SAFE_DELETE_ARRAY( m_pDrawParamPartyList );
      }

      GFL_SAFE_DELETE( m_pDrawButtonCtrl );
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
  void BattleUIPokeEntryConfirmFrame::End()
  {
    if( m_bEnd == false )
    {
      //  画面を終了させる
      if( m_pDraw == NULL )
      {
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
      UCHIDA_PRINT( "warning: continuing BattleUIPokeEntryConfirmFrame::End\n" );
    }
  }

  //  決定入力用リスナ
  bool BattleUIPokeEntryConfirmFrame::OnEnterInput( app::BattleUIPokeEntryConfirmLowerDraw* pDrawInst, app::BattleUIPokeEntryConfirmtLowerButtonCtrl* pButtonCtrl, s32 buttonId )
  {
    bool  bDrawLeftArrowButton    = ( 1 < m_drawParamPartyListIdxMax );
    bool  bDrawRightArrowButton   = ( 1 < m_drawParamPartyListIdxMax );

    if( ( buttonId == app::BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_CANCEL )
    ||  ( buttonId == app::BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_CANCEL_02 ) )
    {
      _SetState( STATE_TYPE_END_ENTER );
      return  true;
    }
    else
    if( buttonId == app::BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_LEFT_ARROW )
    {
      --m_drawParamPartyListNumIdx;
      GFL_ASSERT( m_drawParamPartyListNumIdx < m_drawParamPartyListIdxMax );

      //  もう左方向に配列のidxが移動できない時は左矢印は出さない
      bDrawLeftArrowButton  = ( 0 < m_drawParamPartyListNumIdx );

      m_pDraw->ChangeParamEvent( m_pDrawParamPartyList[ m_drawParamPartyListNumIdx ], bDrawLeftArrowButton, bDrawRightArrowButton, app::BattleUIPokeEntryConfirmLowerDraw::PARAM_CHANGE_EVENT_TYPE_LEFT_MOVE );

      return  true;
    }
    else
    if( buttonId == app::BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_RIGHT_ARROW )
    {
      ++m_drawParamPartyListNumIdx;
      GFL_ASSERT( m_drawParamPartyListNumIdx < m_drawParamPartyListIdxMax );

      //  もう右方向に配列のidxが移動できない時は右の矢印は出さない
      bDrawRightArrowButton  = ( m_drawParamPartyListNumIdx < ( m_drawParamPartyListIdxMax - 1 ) );

      m_pDraw->ChangeParamEvent( m_pDrawParamPartyList[ m_drawParamPartyListNumIdx ], bDrawLeftArrowButton, bDrawRightArrowButton, app::BattleUIPokeEntryConfirmLowerDraw::PARAM_CHANGE_EVENT_TYPE_RIGHT_MOVE );

      return  true;
    }

    return  false;
  }

  /// キー入力通知用リスナ
  bool BattleUIPokeEntryConfirmFrame::OnKeyAction( app::BattleUIPokeEntryConfirmLowerDraw* pDrawInst, app::BattleUIPokeEntryConfirmtLowerButtonCtrl* pButtonCtrl, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) 
    ||  pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
    {
      pButtonCtrl->Push( app::BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_CANCEL );
      return  true;
    }
    //  左右のボタンはパーティが複数ある時のみ
    else
    if( 1 < m_drawParamPartyListIdxMax )
    {
      if( 0 < m_drawParamPartyListNumIdx )
      {
        if( ( pButton->IsHold( gfl2::ui::BUTTON_LEFT ) || ( pButton->IsHold( gfl2::ui::BUTTON_L ) ) ) )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          pButtonCtrl->Push( app::BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_LEFT_ARROW );
          return  true;
        }
      }

      if( m_drawParamPartyListNumIdx < m_drawParamPartyListIdxMax )
      {
        if( ( pButton->IsHold( gfl2::ui::BUTTON_RIGHT ) || ( pButton->IsHold( gfl2::ui::BUTTON_R ) ) ) )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          pButtonCtrl->Push( app::BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_RIGHT_ARROW );
          return  true;
        }
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUIPokeEntryConfirmFrame::_SetState( const BattleUIPokeEntryConfirmFrame::StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
      //  開始
      if( m_pDraw != NULL )
      {
        m_pDraw->Start();
      }
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
    }
    else
    if( m_state == STATE_TYPE_END_ENTER )
    {
      // @fix NMCat[1881] 終了入力なので入力は無効に
      m_pDraw->SetInputEnabled( false ); 
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      //  終了
      if( m_pDraw != NULL )
      {
        m_pDraw->End();
      }
    }
    else
    if( m_state == STATE_TYPE_END )
    {
    }
  }

GFL_NAMESPACE_END( btl )
