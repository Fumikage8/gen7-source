//=============================================================================
/**
 * @file	 BtlvUIItemConfirmFrame.h
 * @brief	 バトルUIアイテム使用使用確認の制御フレーム
 * @author yuto_uchida
 * @date	 2015.04.07
 */
//=============================================================================
#pragma once

#include "BtlvUIItemConfirmFrame.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

//  nijiのインクルード
#include <system/include/HeapDefine.h>
#include <Sound/include/Sound.h>

#include "niji_conv_header/message/msg_btl_app.h"
#include "Battle/source/btl_tables.h"

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUIItemConfirmFrame::ms_arcId = ARCID_BATTLE_LOWER_MENU;
  const u32 BattleUIItemConfirmFrame::ms_tmpStrBufferLength = 36 * 3;

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUIItemConfirmFrame::BattleUIItemConfirmFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUIItemConfirmFrame::~BattleUIItemConfirmFrame()
  {
  }

  void BattleUIItemConfirmFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    GFL_ASSERT( pAppParam->in_param.pAsyncFileManager );
    GFL_ASSERT( pAppParam->in_param.pBattleParamAcceesor );
    GFL_ASSERT( pAppParam->in_param.pParentAppHeap );
    GFL_ASSERT( pAppParam->in_param.pParentDevHeap );
    GFL_ASSERT( pAppParam->in_param.pPocketDataList );
    GFL_ASSERT( pAppParam->in_param.pRenderingManager );

    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIItemConfirmFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIItemConfirmFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPめもりーから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BtlvUIItemConfirmFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

    // ローカルヒープを作る
	  m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.pParentAppHeap, m_pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );

    //  アイコンデータを作成(ファイルの開閉のみに使うのでテクスチャー登録数は０でよい)
    m_pItemIconData = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) ::app::tool::ItemIcon( m_pAppHeap->GetSystemHeap(), m_pAppHeap->GetDeviceHeap(), 0 );

    //  道具使用時のエラーメッセージテキストバッファ
    m_pUseErrorMessageStrBuf  = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( ms_tmpStrBufferLength, m_pAppHeap->GetDeviceHeap() );

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIItemConfirmFrame::InitFunc(void)
  {
    GFL_ASSERT( m_pAppHeap );
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in_param.pRenderingManager );
    GFL_ASSERT( m_pAppParam->in_param.pAsyncFileManager );
    GFL_ASSERT( m_pAppParam->in_param.pBattleParamAcceesor );

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

    //  読み込み開始をここに記述
    if( seq == SEQ_OPEN_FILE )
    {
      //  レイアウトファイルを開く
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
        oreq.arcId       = ms_arcId;
        oreq.heapForFile = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
        oreq.heapForReq  = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
        pAsyncFileMan->AddArcFileOpenReq(oreq);
      }

      //  アイテムアイコンファイルを開く
      {
        m_pItemIconData->FileOpen( m_pAppHeap->GetDeviceHeap() );
      }

      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_OPEN )
    {
      if(  pAsyncFileMan->IsArcFileOpenFinished( ms_arcId ))
      {
        if( m_pItemIconData->IsFileOpen() )
        {
          ++seq;
        }
      }
    }

    if( seq == SEQ_LOAD_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

      req.arcId             = ms_arcId;
      req.datId             = GARC_battleLowwerMenu_itmlow_APPLYT;
      req.ppBuf             = &m_pArcReadBuff;
      req.heapForBuf        = m_pAppHeap->GetDeviceHeap();
      req.heapForReq        = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      req.heapForCompressed = NULL;
      req.align             = 128;

      pAsyncFileMan->AddArcFileLoadDataReq( req );

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
      creq.arcId       = ms_arcId;
      creq.heapForReq  = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileMan->AddArcFileCloseReq(creq);

      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_CLOSE )
    {
      if(pAsyncFileMan->IsArcFileCloseFinished( ms_arcId ) )
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
  applib::frame::Result BattleUIItemConfirmFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw->GetState() == app::BattleUIItemConfirmLowerDraw::STATE_TYPE_UPDATE )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
      if( m_updateSubState == UPDATE_SUB_STATE_INFO )
      {
      }
      else
      if( m_updateSubState == UPDATE_SUB_STATE_ERROR_MESSAGE )
      {
        if( m_pDraw->IsEventMessage() == false )
        {
          _SetUpdateSubState( UPDATE_SUB_STATE_INFO );
        }
      }
    }

    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      //  入力したボタンアニメ終了を待つ
      if( m_pDraw->IsButtonAnimation() == false )
      {
        //  ボタン終了

        bool  bUseItemError = false;
        const pml::pokepara::PokemonParam*  pSasiosaePokeParam  = NULL;

        if( m_pAppParam->out_param.bUse )
        {
          //  アイテムが使用可能かチェック

          if( ( m_pAppParam->in_param.itemPocketType == item::BTLPOCKET_BALL ) && ( m_pAppParam->in_param.useBallErrorMessageId != -1 ) )
          {
            //  ポケットがボールで使用エラーメッセージidがあれば使えない
            m_pAppParam->in_param.pBattleParamAcceesor->GetAppMesssageData()->GetString( m_pAppParam->in_param.useBallErrorMessageId, *m_pUseErrorMessageStrBuf );

            bUseItemError = true;
          }
          else
          //  にげられないバトルの時ににげるアイテムを使用した場合
          if( _IsUseEscItem( m_pAppParam->out_param.useItemId ) == false )
          {
            gfl2::str::StrBuf* pTempbuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( ms_tmpStrBufferLength, m_pAppHeap->GetDeviceHeap() );

            {
              print::WordSet* pWordset = m_pAppParam->in_param.pBattleParamAcceesor->GetWordSet();
              m_pAppParam->in_param.pBattleParamAcceesor->GetAppMesssageData()->GetString( str_btl_bag_16, *pTempbuf );
              pWordset->RegisterPlayerName( 0, m_pAppParam->in_param.pBattleParamAcceesor->GetMyStatus() );
              pWordset->Expand( m_pUseErrorMessageStrBuf, pTempbuf );
            }

            GFL_SAFE_DELETE( pTempbuf );

            bUseItemError = true;
          }
          else
          if( m_pAppParam->in_param.pTargetPokeParam != NULL )
          {
            //  ポケモンリストで対象選択した時にチェック
            if( m_pAppParam->in_param.pTargetPokeParam->CheckSick( pml::wazadata::WAZASICK_SASIOSAE ) == true )
            {
              pSasiosaePokeParam = m_pAppParam->in_param.pTargetPokeParam->GetSrcData();
            }
          }
          else
          if( m_pAppParam->in_param.pSingleEntryPokeParam != NULL )
          {
            //  シングルor乱入バトルで場に出ているポケモンがさしおさえでかつせんとうようのアイテムを選んだ場合
            //  @fix MMCat[225]: ロトポンポケットのアイテムも使用できないようにした
            if( item::CheckBtlPocketInclude( item::BTLPOCKET_BATTLE, m_pAppParam->out_param.useItemId ) == true ||
                item::CheckBtlPocketInclude( item::BTLPOCKET_ROTOPON, m_pAppParam->out_param.useItemId ) == true )
            {
              if( m_pAppParam->in_param.pSingleEntryPokeParam->CheckSick( pml::wazadata::WAZASICK_SASIOSAE ) == true )
              {
                pSasiosaePokeParam = m_pAppParam->in_param.pSingleEntryPokeParam->GetSrcData();
              }
            }
          }
          else
          {
            // @fix MMCat[225]: ダブル用対処
            if( m_pAppParam->in_param.targetPokeIndex >= 2 )
            {
              GFL_ASSERT( 0 );
            }
            else
            {
              const btl::BTL_POKEPARAM * bpp = m_pAppParam->in_param.pDoubleEntryPokeParam[m_pAppParam->in_param.targetPokeIndex];
              if( bpp != NULL )
              {
                if( item::CheckBtlPocketInclude( item::BTLPOCKET_ROTOPON, m_pAppParam->out_param.useItemId ) == true )
                {
                  if( bpp->CheckSick( pml::wazadata::WAZASICK_SASIOSAE ) == true )
                  {
                    pSasiosaePokeParam = bpp->GetSrcData();
                  }
                }
              }
            }
          }

          if( pSasiosaePokeParam != NULL )
          {
            //  さしおさえ状態なら使えない
            // ●●（技名）の効果で ●●（ニックネーム）には使えません！
            gfl2::str::StrBuf* pTempbuf = GFL_NEW_LOW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( ms_tmpStrBufferLength, m_pAppHeap->GetDeviceHeap() );

            {
              m_pAppParam->in_param.pBattleParamAcceesor->GetAppMesssageData()->GetString( str_btl_bag_12, *pTempbuf );
              print::WordSet* pWordset = m_pAppParam->in_param.pBattleParamAcceesor->GetWordSet();
              pWordset->RegisterPokeNickName( 0, pSasiosaePokeParam );
              pWordset->RegisterWazaName( 1, WAZANO_SASIOSAE );
              pWordset->Expand( m_pUseErrorMessageStrBuf, pTempbuf );
            }

            GFL_SAFE_DELETE( pTempbuf );

            bUseItemError = true;
          }
        }

        if( bUseItemError )
        {
          _SetState( STATE_TYPE_UPDATE );
          _SetUpdateSubState( UPDATE_SUB_STATE_ERROR_MESSAGE );
        }
        else
        {
          End();
        }
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      if( m_pDraw->GetState() == app::BattleUIItemConfirmLowerDraw::STATE_TYPE_END )
      {
        _SetState( STATE_TYPE_END );
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      return applib::frame::RES_FINISH;
    }

    if( m_pDraw )
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
  void BattleUIItemConfirmFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
  applib::frame::Result BattleUIItemConfirmFrame::EndFunc(void)
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
      m_pItemIconData->FileClose( m_pAppHeap->GetDeviceHeap() );

      if( m_pDraw != NULL )
      {
        m_pDraw->Terminate();
      }

      ++seq;
    }

    if( seq == SEQ_WAIT )
    {
      bool  bRet  = true;
      if( m_pItemIconData->IsFileClose() == false )
      {
        bRet  = false;
      }

      if( ( m_pDraw != NULL ) && ( m_pDraw->IsDelete() == false ) )
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
      //  入力結果を出力
      m_pAppParam->out_param.bEnd  = true;

      GFL_SAFE_DELETE( m_pUseErrorMessageStrBuf );
      GFL_SAFE_DELETE( m_pItemIconData );
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
  void BattleUIItemConfirmFrame::End()
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
      UCHIDA_PRINT( "warning: continuing BattleUIItemConfirmFrame::End\n" );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief コマンド通知
    */
  //------------------------------------------------------------------
  bool BattleUIItemConfirmFrame::PushCmd( const CmdType type )
  {
    if( type == CMD_TYPE_PUSH_BUTTON_ITEM_USE )
    {
      m_pDraw->PushBtn( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_ENTER, true );
    }

    return  true;
}

  //------------------------------------------------------------------
  /**
    * @brief 決定入力用リスナ
  */
  //------------------------------------------------------------------
  bool BattleUIItemConfirmFrame::OnEnterInput( app::BattleUIItemConfirmLowerDraw* pDrawInst, s32 buttonId )
  {
    //  更新以外では受け取らない
    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    Savedata::MyItem::ITEM_ST*  pItemData = &m_pAppParam->in_param.pPocketDataList->table[ m_currentPocketListIdx ];

    //  現在選択しているアイテムid
    m_pAppParam->out_param.useItemId  = pItemData->id;

    //  入力したボタンの処理待ちをする（ボタンアニメなどの終了を待つ）
    if( buttonId == app::BattleUIItemConfirmLowerDraw::BUTTON_ID_CANCEL )
    {
      m_pAppParam->out_param.bUse = false;

      _SetState( STATE_TYPE_ENTER_EVENT );
    }
    else
    if( buttonId == app::BattleUIItemConfirmLowerDraw::BUTTON_ID_ENTER )
    {
      m_pAppParam->out_param.bUse = true;

      _SetState( STATE_TYPE_ENTER_EVENT );
    }
    else
    if( buttonId == app::BattleUIItemConfirmLowerDraw::BUTTON_ID_MOVE_LEFT_LIST )
    {
      //  リストのカレントはループ移動する
      s32 newListIdx  = (s32)( m_currentPocketListIdx - 1 );
      if( newListIdx < 0 )
      {
        newListIdx  = m_pAppParam->in_param.pPocketDataList->max - 1;
      }

      _ChangeItemInfoData( newListIdx );

      m_currentPocketListIdx  = (u16)newListIdx;
    }
    else
    if( buttonId == app::BattleUIItemConfirmLowerDraw::BUTTON_ID_MOVE_RIGHT_LIST )
    {
      //  リストのカレントはループ移動する
      ++m_currentPocketListIdx;
      if( m_pAppParam->in_param.pPocketDataList->max <= m_currentPocketListIdx )
      {
        m_currentPocketListIdx  = 0;
      }

      _ChangeItemInfoData( m_currentPocketListIdx );
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief キーとスティックの入力結果
  */
  //------------------------------------------------------------------
  void BattleUIItemConfirmFrame::OnKeyAction( app::BattleUIItemConfirmLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        pDrawInst->PushBtn( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_CANCEL );
        return;
      }
    }

    //  決定ボタンを押した(A/Y)
    {
      if( ( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      ||  ( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) ) )
      {
        pDrawInst->PushBtn( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_ENTER );
        return;
      }
    }

    //  LRと左右ボタンでアイテムリスト移動
    // @fix 大田哲司さん要望　リスト移動をトリガーでなく、ホールドにして押しっぱでも切り替えるようにした。
    {
      if( pButton->IsHold( gfl2::ui::BUTTON_L ) || pButton->IsHold( gfl2::ui::BUTTON_LEFT ) )
      {
        pDrawInst->PushBtn( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_MOVE_LEFT_LIST );
        return;
      }
      else
      if( pButton->IsHold( gfl2::ui::BUTTON_R ) || pButton->IsHold( gfl2::ui::BUTTON_RIGHT ) )
      {
        pDrawInst->PushBtn( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_MOVE_RIGHT_LIST );
        return;
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUIItemConfirmFrame::_SetState( const BattleUIItemConfirmFrame::StateType newStateType )
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
        app::BattleUIItemConfirmLowerDraw::INIT_PARAM initParam;
        {
          initParam.pDelegate             = this;
          initParam.pLytBuff              = m_pArcReadBuff;
          initParam.pLytMenuCursorBuff    = m_pAppParam->in_param.pBattleParamAcceesor->GetMenuCusrorLytResourceBuffer();
          initParam.pRenderingManager     = m_pAppParam->in_param.pRenderingManager;
          initParam.pUtilHeap             = m_pAppHeap;
          initParam.pItemMessage          = m_pAppParam->in_param.pBattleParamAcceesor->GetInstanceItemMessage();

          //  フレームを使う側でメッセージ関連の設定をしてもらう
          initParam.pBtlAppMsgData   = m_pAppParam->in_param.pBattleParamAcceesor->GetAppMesssageData();
          initParam.pBtlWordSet      = m_pAppParam->in_param.pBattleParamAcceesor->GetWordSet();
        }

        m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUIItemConfirmLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );

        m_pDraw->Initialize();

        //  ポケットリスト数がひとつでないのであれば矢印を出す
        {
          if( 2 <= m_pAppParam->in_param.pPocketDataList->max )
          {
            m_pDraw->SetVisibleButton( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_MOVE_LEFT_LIST, true );
            m_pDraw->SetVisibleButton( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_MOVE_RIGHT_LIST, true );
          }
          else
          {
            m_pDraw->SetVisibleButton( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_MOVE_LEFT_LIST, false );
            m_pDraw->SetVisibleButton( app::BattleUIItemConfirmLowerDraw::BUTTON_ID_MOVE_RIGHT_LIST, false );
          }
        }

        //  開始
        m_pDraw->Start();

        //  初回表示
        {
          m_currentPocketListIdx  = 0;

          //  アイテムidを指定しているなら、idに合わせたリストのカレントidxにする
          if( ( m_pAppParam->in_param.currentItemId != ITEM_DUMMY_DATA ) && ( m_pAppParam->in_param.currentItemId <= ITEM_DATA_MAX ) )
          {
            u16 loopNum = m_pAppParam->in_param.pPocketDataList->max;
            for( u16 i = 0; i < loopNum; ++i )
            {
              u32 itemId  = m_pAppParam->in_param.pPocketDataList->table[ i ].id;
              if( itemId == m_pAppParam->in_param.currentItemId )
              {
                m_currentPocketListIdx  = i;
                break;
              }
            }
          }
        }

        _ChangeItemInfoData( m_currentPocketListIdx );

        m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
        m_pDraw->SetInputEnabled( false );

        //  背景BGを消すかきめる
        if( m_pAppParam->in_param.bBGMaskDraw == true )
        {
          m_pDraw->SetViaibleBG( true );
        }
        else
        {
          m_pDraw->SetViaibleBG( false );
        }
      }
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      _SetUpdateSubState( UPDATE_SUB_STATE_INFO );
    }
    else
    if( m_state == STATE_TYPE_ENTER_EVENT )
    {
      // @fix NMCat[1582] 入力イベント中はボタンを押せないようにする
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
    * @brief 更新内のサブステート設定
  */
  //------------------------------------------------------------------
  void BattleUIItemConfirmFrame::_SetUpdateSubState( const UpdateSubState newState )
  {
    GFL_ASSERT( m_state == STATE_TYPE_UPDATE );

    m_updateSubState  = newState;

    if( newState == UPDATE_SUB_STATE_INFO )
    {
      m_pDraw->SetInputEnabled( m_pAppParam->in_param.pBattleParamAcceesor->GetCore()->IsControllable() );
    }
    else
    if( newState == UPDATE_SUB_STATE_ERROR_MESSAGE )
    {
      m_pDraw->SetInputEnabled( false );

      m_pDraw->EventMessage( m_pUseErrorMessageStrBuf );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief アイテム表示内容を変更
  */
  //------------------------------------------------------------------
  void BattleUIItemConfirmFrame::_ChangeItemInfoData( const s32 pocketListIdx )
  {
    GFL_ASSERT( pocketListIdx < m_pAppParam->in_param.pPocketDataList->max );
    GFL_ASSERT( 0 <= pocketListIdx);
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in_param.pPocketDataList );

    Savedata::MyItem::ITEM_ST*  pItemData = &m_pAppParam->in_param.pPocketDataList->table[ pocketListIdx ];
    GFL_ASSERT( pItemData );

    m_pDraw->SetInfoData( pItemData->id, pItemData->num, m_pAppParam->in_param.itemPocketType );
  }

  //  にげるアイテムがつかえるか
  bool BattleUIItemConfirmFrame::_IsUseEscItem( const u16 itemId )
  {
    //  まずにげるアイテムなのか判定
    if( btl::tables::IsNoTargetItem( itemId ) )
    {
      if( m_pAppParam->in_param.pBattleParamAcceesor->IsEsc() == false )
      {
        return  false;
      }
    }

    return  true;
  }

GFL_NAMESPACE_END( btl )
