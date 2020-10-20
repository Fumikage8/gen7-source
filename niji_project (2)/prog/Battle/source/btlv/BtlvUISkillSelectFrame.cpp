//=============================================================================
/**
 * @file	 BtlvUISkillSelectFrame.h
 * @brief	 バトルUI技選択画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.16
 */
//=============================================================================
#pragma once

#include "BtlvUISkillSelectFrame.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/app_util_KeyRepeatController.h"
#include <system/include/HeapDefine.h>
#include <Sound/include/Sound.h>

//  battleのインクルード
#include "Battle/source/btl_ZenryokuWazaStatus.h"
#include "Battle/source/btl_ZenryokuWaza.h"

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUISkillSelectFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUISkillSelectFrame::BattleUISkillSelectFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUISkillSelectFrame::~BattleUISkillSelectFrame()
  {
  }

  void BattleUISkillSelectFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;
    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUISkillSelectFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUISkillSelectFrame(Devcie) Initialize\n" );
#endif

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUISkillSelectFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

    // ローカルヒープを作る
    m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.pParentAppHeap, m_pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );

    //  キーリピート作成
    m_pSelectListYKeyRepertCtrl = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) ::app::util::KeyRepeatController( 0, 0 );

    m_resultData.Clear();

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUISkillSelectFrame::InitFunc(void)
  {
    GFL_ASSERT( m_pAppHeap );
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in_param.pRenderingManager );
    GFL_ASSERT( m_pAppParam->in_param.pAsyncFileManager );

//  フレーム側で持っているファイル制御
//  現在未使用
//  まだ処理を消す段階ではない
//  なぜならファイルロードの先読みしないで済むかもしれないから
    int seq = applib::frame::CellSuper::GetSubSeq();

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

    gfl2::fs::AsyncFileManager* pAsyncFileMan = m_pAppParam->in_param.pAsyncFileManager;

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
      if( pAsyncFileMan->IsArcFileOpenFinished( s_arcId ))
      {
        ++seq;
      }
    }

    if( seq == SEQ_LOAD_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

      req.arcId             = s_arcId;
      req.datId             = GARC_battleLowwerMenu_skllow_APPLYT;
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
  applib::frame::Result BattleUISkillSelectFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw->GetState() != app::BattleUISkillSelectLowerDraw::STATE_TYPE_IN )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    //  画面終了入力では入力アニメが終わるまで待つ
    if( m_state == STATE_TYPE_END_ENTER )
    {
      //  入力したボタンアニメ終了を待つ
      if( m_pDraw->IsButtonAnimation() == false )
      {
        //  入力ボタンアニメ終了
        if( m_resultData.type == RESULT_TYPE_SKILL_SELECT_INFO )
        {
          _SetState( STATE_TYPE_SKILL_STATUS );
        }
        else
        {
          //  入力結果を出力
          m_pAppParam->out_param.bEnterSuspend    = true;
          m_pAppParam->out_param.resultData       = m_resultData;
        }
      }
    }

    //  わざ説明フレーム処理中
    if( m_state == STATE_TYPE_SKILL_STATUS )
    {
      if( m_uiSkillStatusFrameAppParam.out_param.bEnd )
      {
        m_pUISkillStatusFrame = NULL;
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      if( m_pDraw->GetState() == app::BattleUISkillSelectLowerDraw::STATE_TYPE_END )
      {
        _SetState( STATE_TYPE_END );
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      //  入力結果を出力
      m_pAppParam->out_param.bEnd           = true;
      m_pAppParam->out_param.bEnterSuspend  = true;
      m_pAppParam->out_param.resultData     = m_resultData;

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
  void BattleUISkillSelectFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
  applib::frame::Result BattleUISkillSelectFrame::EndFunc(void)
  {
    enum
    {
      BATTLE_UI_SKILL_SELECT_FRAME_END_FUCN_SEQ_INIT  = 0,
      BATTLE_UI_SKILL_SELECT_FRAME_END_FUCN_SEQ_WAIT,
      BATTLE_UI_SKILL_SELECT_FRAME_END_FUCN_SEQ_END
    };

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == BATTLE_UI_SKILL_SELECT_FRAME_END_FUCN_SEQ_INIT )
    {
      // @fix GFNMCat[3252] この段階で m_pDraw が NULLになることがあるのでチェックを追加
      if( m_pDraw != NULL )
      {
        m_pDraw->Terminate();
      }

      ++seq;
    }
    else
    if( seq == BATTLE_UI_SKILL_SELECT_FRAME_END_FUCN_SEQ_WAIT )
    {
      // @fix GFNMCat[3252] この段階で m_pDraw が NULLになることがあるのでチェックを追加
      if( ( m_pDraw == NULL ) || ( m_pDraw->IsDelete() == true ) )
      {
        ++seq;
      }
    }
    else
    if( seq == BATTLE_UI_SKILL_SELECT_FRAME_END_FUCN_SEQ_END )
    {
      GFL_SAFE_DELETE( m_pDraw );

      //  全力技の文字列削除
      {
        app::BattleUISkillSelectLowerDraw::SKILL_SELECT_PARAM*  pSkillSelectParam = NULL;
        u32 loopNum = GFL_NELEMS( m_skillSelectDrawInitParam.aSkillList );

        for( u32 i = 0; i < loopNum; ++i )
        {
          pSkillSelectParam = &m_skillSelectDrawInitParam.aSkillList[ i ];
          GFL_SAFE_DELETE( pSkillSelectParam->zenryoku.pName );
        }
      }

      GFL_SAFE_DELETE( m_pSelectListYKeyRepertCtrl );
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
    * @brief 決定サスペンド状態から復帰
    */
  //------------------------------------------------------------------
  void BattleUISkillSelectFrame::Resume()
  {
    GFL_ASSERT( m_pAppParam->out_param.bEnterSuspend );

    m_pAppParam->out_param.bEnterSuspend  = false;

    _SetState( STATE_TYPE_UPDATE );
  }

  //------------------------------------------------------------------
  /**
    * @brief 終了通知
    */
  //------------------------------------------------------------------
  void BattleUISkillSelectFrame::End()
  {
    if( m_bEnd == false )
    {
      if( m_state <= STATE_TYPE_OPEN )
      {
        //  ロード中であればすぐに終わらせる
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
      UCHIDA_PRINT( "warning: continuing BattleUISkillSelectFrame::End\n" );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   イベント中か
    * @return  イベント中 -> true / イベントしていない -> false
    */
  //------------------------------------------------------------------
  bool BattleUISkillSelectFrame::IsEvent()
  {
    //  更新以外はすべてイベント扱い
    if( m_state == STATE_TYPE_UPDATE )
    {
      return  false;
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief コマンド通知
    */
  //------------------------------------------------------------------
  bool BattleUISkillSelectFrame::PushCmd( const CmdType type )
  {
    if( IsEvent() == true )
    {
      return  false;
    }

    bool  bRet  = false;

    //  わざ項目１番目のボタンを押す
    if( type == CMD_TYPE_PUSH_SKILL_BUTTON_0 )
    {
      m_pDraw->SetCurrentIdxSkillList( 0 );
      m_pDraw->PushBtnCurrentCursorItem();

      bRet  = true;
    }
    else 
    // 全力技項目を選択する。
    if( type == CMD_TYPE_PUSH_BUTTON_ZENRYOKU )
    {
      m_pDraw->MoveCursorSpecialButton();
      m_pDraw->PushBtnCurrentCursorItem();
      bRet  = true;
    }
    else
    // 全力技 技項目の2番目を押す
    if( type == CMD_TYPE_PUSH_BUTTON_ZENRYOKU_SKILL_BUTTON_1 )
    {
      // @fix NMCat[4119] アニメーションさせるボタンを変更
      m_pDraw->SetCurrentIdxSkillList( 0 );
      m_pDraw->PushBtnCurrentCursorItem();
      bRet  = true;
    }

    
    return  bRet;
  }

  //------------------------------------------------------------------
  /**
    * @brief 決定入力用リスナ
  */
  //------------------------------------------------------------------
  bool BattleUISkillSelectFrame::OnEnterInput( app::BattleUISkillSelectLowerDraw* pDrawInst, s32 buttonId )
  {
    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    //  全力・メガ進化のボタンをしても終了入力ステートに繊維しない
    {
      if( buttonId == app::BattleUISkillSelectLowerDraw::BUTTON_ID_MEGA )
      {
        m_resultData.bMega = !m_resultData.bMega;
        pDrawInst->InvokeSpecialBtn( app::BattleUISkillSelectLowerDraw::SPECIAL_BUTTON_TYPE_MEGA, m_resultData.bMega );

        return  true;
      }
      else if( buttonId == app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU )
      {
        //  全力わざ選択開始
        m_resultData.bZenryoku  = true;
        pDrawInst->InvokeSpecialBtn( app::BattleUISkillSelectLowerDraw::SPECIAL_BUTTON_TYPE_ZENRYOKU, m_resultData.bZenryoku );

        return  true;
      }
      else if (buttonId == app::BattleUISkillSelectLowerDraw::BUTTON_ID_ULTRABURST)
      {
        //momiji ウルトラバースト
        m_resultData.bUltraBurst = !m_resultData.bUltraBurst;
        pDrawInst->InvokeSpecialBtn(app::BattleUISkillSelectLowerDraw::SPECIAL_BUTTON_TYPE_ULTRABURST, m_resultData.bUltraBurst);

        return  true;
      }
    }

    //  画面ぜんりょくわざ選択状態でキャンセルすると選択を終了
    {
      if( pDrawInst->GetState() == app::BattleUISkillSelectLowerDraw::STATE_TYPE_INPUT_ZENRYOKU_SKILL_SELECT_LIST )
      {
        if( buttonId == app::BattleUISkillSelectLowerDraw::BUTTON_ID_BACK )
        {
          //  全力わざ選択解除
          m_resultData.bZenryoku  = false;
          pDrawInst->InvokeSpecialBtn( app::BattleUISkillSelectLowerDraw::SPECIAL_BUTTON_TYPE_ZENRYOKU, m_resultData.bZenryoku );

          return  true;
        }
      }
    }

    //  リザルト対応のボタンならリザルトデータ設定
    {
      //  現在選択しているカーソルidxを取得
      s32 nowSkillSelectListIdx = pDrawInst->GetSkillListCursorIdx();

      m_resultData.type = RESULT_TYPE_BACK;

      //  カーソルリストの項目を押した時の検索
      {
        struct _tag_skill_result_data
        {
          app::BattleUISkillSelectLowerDraw::ButtonId chkId;
          ResultType                                  resultType;
          s32                                         listIdx;
          bool                                        bZenryoku;
        };

        const _tag_skill_result_data aResultDataTable[ 25 ]  =
        {
          //  通常わざ項目
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_SKILL_ITEM_00, RESULT_TYPE_SKILL_SELECT, 0, false },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_SKILL_ITEM_01, RESULT_TYPE_SKILL_SELECT, 1, false },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_SKILL_ITEM_02, RESULT_TYPE_SKILL_SELECT, 2, false },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_SKILL_ITEM_03, RESULT_TYPE_SKILL_SELECT, 3, false },

          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_SKILL_ITEM_INFO_00, RESULT_TYPE_SKILL_SELECT_INFO, 0, false },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_SKILL_ITEM_INFO_01, RESULT_TYPE_SKILL_SELECT_INFO, 1, false },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_SKILL_ITEM_INFO_02, RESULT_TYPE_SKILL_SELECT_INFO, 2, false },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_SKILL_ITEM_INFO_03, RESULT_TYPE_SKILL_SELECT_INFO, 3, false },

          //  全力わざ項目
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU_SKILL_ITEM_00, RESULT_TYPE_SKILL_SELECT, 0, true },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU_SKILL_ITEM_01, RESULT_TYPE_SKILL_SELECT, 1, true },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU_SKILL_ITEM_02, RESULT_TYPE_SKILL_SELECT, 2, true },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU_SKILL_ITEM_03, RESULT_TYPE_SKILL_SELECT, 3, true },

          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_00, RESULT_TYPE_SKILL_SELECT_INFO, 0, true },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_01, RESULT_TYPE_SKILL_SELECT_INFO, 1, true },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_02, RESULT_TYPE_SKILL_SELECT_INFO, 2, true },
          { app::BattleUISkillSelectLowerDraw::BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_03, RESULT_TYPE_SKILL_SELECT_INFO, 3, true },
        };

        u32 loopNum = GFL_NELEMS( aResultDataTable );
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( aResultDataTable[ i ].chkId == buttonId )
          {
            m_resultData.type = aResultDataTable[ i ].resultType;

            m_resultData.push_skill_select.skillSelectParam = pDrawInst->GetInitParam().aSkillList[ aResultDataTable[ i ].listIdx ];
            m_resultData.push_skill_select.btnPos           = pDrawInst->GetButtonManager()->GetButtonPosByGfVec3( buttonId );

            if( m_resultData.type == RESULT_TYPE_SKILL_SELECT_INFO )
            {
              //  インフォボタンを押したのなら、わざの情報を設定
              m_resultData.push_skill_select.b_zenryoku = aResultDataTable[ i ].bZenryoku;

            }
            else
            if( m_resultData.type == RESULT_TYPE_SKILL_SELECT )
            {
              //  PP値がないのであれば遷移しない
              if( 0 < m_resultData.push_skill_select.skillSelectParam.normal.ppNowNum )
              {
                m_resultData.push_skill_select.list_idx  = aResultDataTable[ i ].listIdx;

                //  決定選択したわざカーソル位置に
                nowSkillSelectListIdx = aResultDataTable[ i ].listIdx;
              }
              else
              {
                m_resultData.type = RESULT_TYPE_NONE;
              }
            }

            break;
          }
        }
      }

      if( m_resultData.type != RESULT_TYPE_NONE )
      {
        //  カーソル位置を更新
        m_resultData.endCurrentCursorType = pDrawInst->GetCurrentCursorType();
        m_resultData.endSkillCursorIdx    = nowSkillSelectListIdx;

        //  終了入力処理へ
        _SetState( STATE_TYPE_END_ENTER );

        return  true;
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief キー入力通知用リスナ
  */
  //------------------------------------------------------------------
  bool BattleUISkillSelectFrame::OnKeyAction( app::BattleUISkillSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    GFL_ASSERT( pDrawInst );

    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    //  キャンセル
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      pDrawInst->PushBtn( app::BattleUISkillSelectLowerDraw::BUTTON_ID_BACK );

      return  true;
    }

    //  決定
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
      {
        if( pDrawInst->PushBtnCurrentCursorItemInfoIcon() )
        {
          return true;
        }
      }
      else
      {
        pDrawInst->PushBtnCurrentCursorItem();

        return  true;
      }
    }

    //  リスト上下のボタン入力
    {
      if( pDrawInst->IsSkillSelectState() )
      {
        s32 nowSelectListIdx  = pDrawInst->GetSkillListCursorIdx();
        s32 oldSelectListIdx  = nowSelectListIdx;
        s32 listIdxMax        = pDrawInst->GetInitParam().skillSelectItemNum - 1;

        m_pSelectListYKeyRepertCtrl->SetMaxPos( listIdxMax, 0 );
        m_pSelectListYKeyRepertCtrl->UpdateInfo( pKey, nowSelectListIdx, 0 );

        if( m_pSelectListYKeyRepertCtrl->IsRepeat( gfl2::ui::DIR_UP ) )
        {
          bool  bLoop = pDrawInst->IsSkillSelectListCurrentUpperLimit();
          nowSelectListIdx  = _MoveCursorPos( nowSelectListIdx, 0, listIdxMax, -1, pDrawInst, bLoop );
        }
        else
        if( m_pSelectListYKeyRepertCtrl->IsRepeat( gfl2::ui::DIR_DOWN ) )
        {
          bool  bLoop = pDrawInst->IsSkillSelectListCurrentUnderLimit();
          nowSelectListIdx  = _MoveCursorPos( nowSelectListIdx, 0, listIdxMax, 1, pDrawInst, bLoop );
        }

        if( oldSelectListIdx != nowSelectListIdx )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          pDrawInst->SetCurrentIdxSkillList( nowSelectListIdx );
          return  true;
        }
      }
    }

    if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      if( pDrawInst->GetState() == app::BattleUISkillSelectLowerDraw::STATE_TYPE_INPUT_SKILL_SELECT_LIST )
      {
        //  特殊ボタンへ切り替え
        bool  bChange = pDrawInst->MoveCursorSpecialButton();

        // @fix GFNMCat[3919] 切り替えが成功したらSE再生
        if( bChange == true )
        {
          if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
          {
            Sound::PlaySE( SEQ_SE_SELECT1 );
          }

          return  true;
        }
      }
    }

    if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      if( pDrawInst->GetState() == app::BattleUISkillSelectLowerDraw::STATE_TYPE_INPUT_SPECIAL_BUTTON )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }

        //  技リストへ切り替え
        pDrawInst->MoveCursorSkillList();

        return  true;
      }
    }

    return false;
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUISkillSelectFrame::_SetState( const BattleUISkillSelectFrame::StateType newStateType )
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
        {
          //  先読みしたファイルデータを使うか
          m_skillSelectDrawInitParam.pLytBuff            = m_pArcReadBuff;

          const BTL_POKEPARAM*  pBtlParam = _GetSelfBtlPokeParam();
          bool isUltraBurst = false;

          GFL_ASSERT( pBtlParam );
          GFL_ASSERT( m_skillSelectDrawInitParam.pLytBuff );

          m_skillSelectDrawInitParam.pDelegate              = this;
          m_skillSelectDrawInitParam.pRenderingManager      = m_pAppParam->in_param.pRenderingManager;
          m_skillSelectDrawInitParam.pUtilHeap              = m_pAppHeap;
          m_skillSelectDrawInitParam.pMenuCursorLytBuff     = m_pAppParam->in_param.pBtlParamAccessror->GetMenuCusrorLytResourceBuffer();
          m_skillSelectDrawInitParam.pAppCommonGrpIconData  = &m_pAppParam->in_param.pBtlParamAccessror->GetAppCommonGrpIconData();
          // @fix GFNMCat[1552] Z技チュートリアルで入力フラグを見て設定しているのでこれを防ぐために入力フラグを設定
          m_skillSelectDrawInitParam.bControllable          = m_pAppParam->in_param.pBtlParamAccessror->GetCore()->IsControllable();

          //  入力関連のSE再生をするかどうか
          m_skillSelectDrawInitParam.bUseInputSE         = m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE();

          //  初回カーソルタイプ
          {
            m_skillSelectDrawInitParam.firstCursorType    = m_pAppParam->in_param.firstSkillListCursorType;
          }

          //  特別ボタンの設定
          {
            m_skillSelectDrawInitParam.specialSelectParam.type = app::BattleUISkillSelectLowerDraw::SPECIAL_BUTTON_TYPE_NONE;

            //  メガ進化が選択可能か
            if( m_pAppParam->in_param.pBtlParamAccessror->IsMegaPoke( m_pAppParam->in_param.selfPartyPokeIdx ) )
            {
              m_skillSelectDrawInitParam.specialSelectParam.type  = app::BattleUISkillSelectLowerDraw::SPECIAL_BUTTON_TYPE_MEGA;
            }
            //  ヒカリドラゴン進化が選択可能か
            if (m_pAppParam->in_param.pBtlParamAccessror->IsUltraBurstPoke(m_pAppParam->in_param.selfPartyPokeIdx))
            {
              m_skillSelectDrawInitParam.specialSelectParam.type = app::BattleUISkillSelectLowerDraw::SPECIAL_BUTTON_TYPE_ULTRABURST;
              isUltraBurst = true;
            }
          }

          //  メッセージデータ設定
          {
            m_skillSelectDrawInitParam.pBtlAppMsgData = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();
            m_skillSelectDrawInitParam.pBtlWordSet    = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
          }

          //  技関連の設定
          {
            //  選択する技項目設定
            const btl::BattleViewSystem*  pViewSys      = m_pAppParam->in_param.pBtlParamAccessror->GetCore();
            const u8                      selfClientId  = m_pAppParam->in_param.pBtlParamAccessror->GetClientID();

            //  プレイヤー自身のオーラ技の状態を取得
            const btl::ZenryokuWazaStatus*  pAuraWazaStatus = pViewSys->GetZenryokuWazaStatus( selfClientId );

            //  バトルのポケモンデータ（バトル中のデータ反映）
            m_skillSelectDrawInitParam.skillSelectItemNum = pBtlParam->WAZA_GetCount();

            {
              u8 ppNum                 = 0;
              u8 ppMaxNum              = 0;
              WazaID  wazaId           = WAZANO_NULL;
              pml::PokeType  wazaType  = 0;

              app::BattleUISkillSelectLowerDraw::SKILL_SELECT_PARAM*  pSkillSelectParam = NULL;

              s32 loopNum              = m_skillSelectDrawInitParam.skillSelectItemNum;
              for( s32 i = 0; i < loopNum; ++i )
              {
                wazaId    = pBtlParam->WAZA_GetParticular( (u8)i, &ppNum, &ppMaxNum );

                if( wazaId == WAZANO_NULL )
                {
                  //  項目スキップ
                  continue;
                }

                wazaType  = WAZADATA_GetType( wazaId );

                pSkillSelectParam = &m_skillSelectDrawInitParam.aSkillList[ i ];

                //  変身しているのであればインフォアイコンは表示しない
                pSkillSelectParam->bInfoIconVisible = ( pBtlParam->HENSIN_Check() == false );

                //  通常わざの設定
                {
                  pSkillSelectParam->normal.base.wazaId      = wazaId;

                  //  PP値のチェック(オーラ技と連動している)
                  pSkillSelectParam->normal.ppMaxNum    = ppMaxNum;
                  pSkillSelectParam->normal.ppNowNum    = ppNum;
                  pSkillSelectParam->normal.base.koukaType   = m_pAppParam->in_param.pBtlParamAccessror->GetPokePosListKoukaType( 
                                                                          &pSkillSelectParam->normal.base.bKoukaTypePutFlg, 
                                                                          wazaId, 
                                                                          pBtlParam->GetID(), 
                                                                          m_pAppParam->in_param.pBtlParamAccessror->GetEnemyPokePosPlaceList() );
                }

                //  全力わざの設定
                {
                  pSkillSelectParam->zenryoku.base.wazaId    = WAZANO_NULL;

                  // @fix NMCat[891] ここではＺボタンが押せるかどうかの判定まで
                  // ヒカリドラゴン進化はZ技道具でもあるので、進化できる時はZを表示しない
                  if( m_pAppParam->in_param.pBtlParamAccessror->IsZenryoku( m_pAppParam->in_param.selfPartyPokeIdx, pBtlParam, pAuraWazaStatus ) == true &&
                    isUltraBurst == false)
                  {
                    //  オーラ技がなくてもオーラ技発動条件が成立していればオーラ技発動ボタンを押せるようにする
                    //  すでにメガ進化ボタンを押せる設定しても上書きする(というかメガ進化とオーラ技が同時成立はない。あった場合は優先はオーラ技）
                    m_skillSelectDrawInitParam.specialSelectParam.type  = app::BattleUISkillSelectLowerDraw::SPECIAL_BUTTON_TYPE_ZENRYOKU;
                   
                    if( pAuraWazaStatus->CanUseZenryokuWaza( *pBtlParam, i ) == true )
                    {
                      pSkillSelectParam->zenryoku.base.wazaId  = pAuraWazaStatus->GetZenryokuWazaNo( *pBtlParam, (u8)i );
                      if( pSkillSelectParam->zenryoku.base.wazaId != WAZANO_NULL )
                      {
                        //  全力技名を取得して設定
                        {
                          pSkillSelectParam->zenryoku.pName = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( NAME_LENGHT_ZENRYOKE_WAZA * 3, m_pAppHeap->GetDeviceHeap() );
                          ZenryokuWaza::GetZenryokuWazaName( pSkillSelectParam->zenryoku.pName, *pBtlParam, pSkillSelectParam->normal.base.wazaId );
                        }

                        //  技タイプ設定
                        {
                          pSkillSelectParam->zenryoku.type  = static_cast<PokeType_tag>( m_pAppParam->in_param.pBtlParamAccessror->GetCore()->GetUiDisplay_WazaType( pSkillSelectParam->normal.base.wazaId, pSkillSelectParam->zenryoku.base.wazaId ) );
                        }

                        //  全力技の効果相性取得
                        {
                          pSkillSelectParam->zenryoku.base.koukaType   = m_pAppParam->in_param.pBtlParamAccessror->GetPokePosListKoukaType( 
                                                                                  &pSkillSelectParam->zenryoku.base.bKoukaTypePutFlg, 
                                                                                  pSkillSelectParam->zenryoku.base.wazaId, 
                                                                                  pBtlParam->GetID(), 
                                                                                  m_pAppParam->in_param.pBtlParamAccessror->GetEnemyPokePosPlaceList() );
                        }
                      }
                    }
                  }
                }

                //  共通の設定
                {
                  pSkillSelectParam->skillType          = (PokeType_tag)wazaType;
                }
              }

              //  全力技があってその中で一番威力が高いのにフラグをつける
              {
                //  1:全部同じ威力なら全部フラグをつける
                //  2:威力が異なるなら一番大きなのにフラグをつける

                //  一番大きな威力をまず取得
                s32 no1ZenryokuPower  = 0;
                {
                  s32 tmpZenryokuPower  = 0;
                  for( s32 i = 0; i < loopNum; ++i )
                  {
                    pSkillSelectParam = &m_skillSelectDrawInitParam.aSkillList[ i ];
                    if( pSkillSelectParam->zenryoku.base.wazaId != WAZANO_NULL )
                    {
                      //  全力技がある
                      tmpZenryokuPower  = WAZADATA_GetParam( pSkillSelectParam->normal.base.wazaId, pml::wazadata::PARAM_ID_ZENRYOKU_POWER );
                      if( ( 0 < tmpZenryokuPower ) && ( 0 < pSkillSelectParam->normal.ppNowNum ) )
                      {
                        //  威力取得対象はＰＰがある技＋威力がある技（変化技は威力がないので０になっている）
                        if( no1ZenryokuPower <= tmpZenryokuPower )
                        {
                          no1ZenryokuPower  = tmpZenryokuPower;
                        }
                      }
                    }
                  }
                }

                //  一番大きな全力技の威力にヒットしたらフラグを立てる
                {
                  s32 nowZenryokuPower  = 0;
                  for( s32 i = 0; i < loopNum; ++i )
                  {
                    pSkillSelectParam = &m_skillSelectDrawInitParam.aSkillList[ i ];
                    // @fix NMCat[965] 技のＰＰ値がなければエフェクトフラグ設定は非対象
                    if( ( pSkillSelectParam->zenryoku.base.wazaId != WAZANO_NULL ) && ( 0 < pSkillSelectParam->normal.ppNowNum ) )
                    {
                      //  変化技は威力が０なのでエフェクトの非対象にする
                      nowZenryokuPower  = WAZADATA_GetParam( pSkillSelectParam->normal.base.wazaId, pml::wazadata::PARAM_ID_ZENRYOKU_POWER );
                      if( ( 0 < nowZenryokuPower ) && ( no1ZenryokuPower <= nowZenryokuPower ) )
                      {
                        pSkillSelectParam->zenryoku.bNo1Power = true;
                      }
                    }
                  }
                }
              }
            }
          }
        }

        m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUISkillSelectLowerDraw( m_skillSelectDrawInitParam );
        GFL_ASSERT( m_pDraw );

        m_pDraw->Initialize();

        //  開始
        m_pDraw->Start();

        //  開始時は入力をOFF
        m_pDraw->SetInputEnabled( false );
        m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
      }
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      //  更新時には入力をON
      m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
      m_pDraw->SetInputEnabled( m_pAppParam->in_param.pBtlParamAccessror->GetCore()->IsControllable() );
    }
    else
    if( m_state == STATE_TYPE_END_ENTER )
    {
      //  ボタン入力をすべて無効に
      m_pDraw->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_SKILL_STATUS )
    {
      GFL_ASSERT( m_resultData.type == RESULT_TYPE_SKILL_SELECT_INFO );

      //  フレームの上にさらにわざ説明のフレームを重ねがけ
      m_pUISkillStatusFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, btl::BattleUISkillStatusFrame>( CellSuper::GetManager() );
      GFL_ASSERT( m_pUISkillStatusFrame );

      BattleUISkillStatusFrame::APP_PARAM::_tag_in_param* pInParam  = &m_uiSkillStatusFrameAppParam.in_param;

      pInParam->pAsyncFileManager     = m_pAppParam->in_param.pAsyncFileManager;
      pInParam->pRenderingManager     = m_pAppParam->in_param.pRenderingManager;
      pInParam->pParentAppHeap        = m_pAppParam->in_param.pParentAppHeap;
      pInParam->pParentDevHeap        = m_pAppParam->in_param.pParentDevHeap;
      pInParam->bUseInputSE           = m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE();
      pInParam->bUserInput            = m_pAppParam->in_param.pBtlParamAccessror->GetCore()->IsControllable();
      pInParam->pTmpDevHeap           = m_pAppParam->in_param.pTmpDevHeap;
      pInParam->pAppCommonGrpIconData = &m_pAppParam->in_param.pBtlParamAccessror->GetAppCommonGrpIconData();
      pInParam->infoWindowPivotPos    = m_resultData.push_skill_select.btnPos;
      pInParam->windowType            = BattleUISkillStatusFrame::WINDOW_TYPE_INFO;
      pInParam->pBtlViewSystem        = m_pAppParam->in_param.pBtlParamAccessror->GetCore();

      //  わざ詳細関連のメッセージデータ設定
      {
        pInParam->pBtlAppMsgData    = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();
        pInParam->pBtlWordSet       = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
        pInParam->pWazaInfoMsgData  = m_pAppParam->in_param.pBtlParamAccessror->GetWazaInfoMesssageData();
      }

      //  わざのデータ設定
      {
        app::BattleUISkillSelectLowerDraw::SKILL_SELECT_PARAM&  rSkillSelectParam = m_resultData.push_skill_select.skillSelectParam;

        //  通常わざの設定
        {
          pInParam->wazaId          = rSkillSelectParam.normal.base.wazaId;
          pInParam->ppNowNum        = rSkillSelectParam.normal.ppNowNum;
          pInParam->ppMaxNum        = rSkillSelectParam.normal.ppMaxNum;
        }

        //  全力わざであれば、全力わざのidを設定
        pInParam->bZenryoku       = m_resultData.push_skill_select.b_zenryoku;
        if( pInParam->bZenryoku )
        {
          pInParam->zenryokuWazaId    = rSkillSelectParam.zenryoku.base.wazaId;
          //  全力技の場合は名前ポイント指定
          pInParam->pZenryokuWazaName = rSkillSelectParam.zenryoku.pName;
        }
      }

      m_pUISkillStatusFrame->Initialize( &m_uiSkillStatusFrameAppParam );
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      //  わざ詳細画面が開いているのであれば殺す
      if( ( m_uiSkillStatusFrameAppParam.out_param.bEnd == false ) && ( m_pUISkillStatusFrame != NULL ) )
      {
        m_pUISkillStatusFrame->End();
        m_pUISkillStatusFrame = NULL;
      }

      //  画面を閉じる時は入力はOFF
      m_pDraw->SetInputEnabled( false );

      //  終了
      m_pDraw->End();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 現在参照しているポケモンのパラメーターを取得
  */
  //------------------------------------------------------------------
  const BTL_POKEPARAM* BattleUISkillSelectFrame::_GetSelfBtlPokeParam()
  {
    if( m_pAppParam->in_param.pBtlParamAccessror->IsReady() )
    {
      //  自身のパーティデータ
      const BTL_PARTY* pSelfParty = m_pAppParam->in_param.pBtlParamAccessror->GetBattleParty_Self();
      GFL_ASSERT( pSelfParty );

      //  バトルのポケモンデータ（バトル中のデータ反映）
      const BTL_POKEPARAM*  pBtlParam = pSelfParty->GetMemberDataConst( m_pAppParam->in_param.selfPartyPokeIdx );
      GFL_ASSERT( pBtlParam );

      return pBtlParam;
    }

    return NULL;
  }

  //  カーソル移動
  s32 BattleUISkillSelectFrame::_MoveCursorPos( const s32 nowCurosrPos, const s32 cursorMinPos, const s32 cursorMaxPos, const s32 moveNum, app::BattleUISkillSelectLowerDraw* pDrawInst, const bool bPosLoop )
  {
    GFL_ASSERT( moveNum != 0 );
    if( moveNum == 0 )
    {
      return  nowCurosrPos;
    }

    s32 oldSelectListIdx  = nowCurosrPos;
    s32 tmpNowCursorPos   = nowCurosrPos;

    bool  bMoveEnd  = false;

    //  カーソル移動をするが、移動先に項目がないのであれば、項目がある場所まで移動
    for ever
    {
      tmpNowCursorPos += moveNum;

      //  上限、下限設定かループにするか
      if( ( 0 < moveNum ) && ( cursorMaxPos < tmpNowCursorPos ) )
      {
        if( bPosLoop )
        {
          tmpNowCursorPos = 0;
        }
        else
        {
          bMoveEnd        = true;
          tmpNowCursorPos = cursorMaxPos;
        }
      }
      else
      if( ( moveNum < 0 ) && ( tmpNowCursorPos < cursorMinPos ) )
      {
        if( bPosLoop )
        {
          tmpNowCursorPos = cursorMaxPos;
        }
        else
        {
          bMoveEnd        = true;
          tmpNowCursorPos = cursorMinPos;
        }
      }

      if( pDrawInst->SetCurrentIdxSkillList( tmpNowCursorPos ) )
      {
        //  カーソル位置変更が成功したら終了
        break;
      }
      else
      if( oldSelectListIdx == tmpNowCursorPos )
      {
        //  同じ項目到達したらそこで終了
        break;
      }
      else
      if( ( bPosLoop == false ) && bMoveEnd )
      {
        //  ループ移動でなく、リストの上限、下限に到達したら終了
        //  この処理に来たときは一度もカーソル移動しなかったので元のカーソル位置に戻す
        tmpNowCursorPos = oldSelectListIdx;
        break;
      }
    }

    return  tmpNowCursorPos;
  }

GFL_NAMESPACE_END( btl )
