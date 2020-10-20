//=============================================================================
/**
 * @file	 BtlvUISkillStatusFrame.h
 * @brief	 バトルUI技ステータス画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.19
 */
//=============================================================================
#pragma once

#include "BtlvUISkillStatusFrame.h"
#include "Battle/source/btlv/btlv_core.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

//  nijiのインクルード
#include <system/include/HeapDefine.h>
#include <Sound/include/Sound.h>

#include "AppLib/include/Ui/UIViewManager.h"
#include <base/include/gfl2_Singleton.h>

GFL_NAMESPACE_BEGIN( btl )

  const s32 BattleUISkillStatusFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUISkillStatusFrame::BattleUISkillStatusFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUISkillStatusFrame::~BattleUISkillStatusFrame()
  {
  }

  void BattleUISkillStatusFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;

    GFL_ASSERT( m_pAppParam->in_param.pBtlViewSystem != NULL );

    m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUISkillStatusFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUISkillStatusFrame(Devcie) Initialize\n" );
#endif

    //HEAPID_APP  本体に関するものはAPPメモリから確保します

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUISkillStatusFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

    // ローカルヒープを作る
    m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.pParentAppHeap, m_pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );

    //  技の名前と説明の文字列クラス作成
    {
      m_pWazaSetumeiStr = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 60 * 3, m_pAppHeap->GetDeviceHeap() );
      GFL_ASSERT( m_pWazaSetumeiStr );
    }

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUISkillStatusFrame::InitFunc(void)
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

    // 読み込み開始をここに記述
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
      req.datId             = GARC_battleLowwerMenu_dsclow_APPLYT;
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
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUISkillStatusFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      if( m_pDraw->GetState() == app::BattleUISkillStatusLowerDraw::STATE_TYPE_INPUT_IDLE )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      if( m_pDraw->GetState() == app::BattleUISkillStatusLowerDraw::STATE_TYPE_END )
      {
        _SetState( STATE_TYPE_END );
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      return applib::frame::RES_FINISH;
    }

    if(m_pDraw != NULL )
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
  void BattleUISkillStatusFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
  applib::frame::Result BattleUISkillStatusFrame::EndFunc(void)
  {
    m_pAppParam->out_param.bEnd = true;

    // @fix GFNMCat[3254] この段階で m_pDraw が NULLになることがあるのでチェックを追加
    if( ( m_pDraw == NULL ) || ( m_pDraw->IsDrawing() == false ) )
    {
      GFL_SAFE_DELETE( m_pWazaSetumeiStr );

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
    * @brief 終了通知
    */
  //------------------------------------------------------------------
  void BattleUISkillStatusFrame::End()
  {
    if( m_bEnd == false )
    {
      //  ロード中はいきなり殺す
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
      //GFL_ASSERT( 0 );
      UCHIDA_PRINT( "warning: continuing BattleUISkillStatusFrame::End\n" );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 決定入力用リスナ
  */
  //------------------------------------------------------------------
  //  overwrite
  bool BattleUISkillStatusFrame::OnEnterInput( app::BattleUISkillStatusLowerDraw* pDrawInst, s32 buttonId )
  {
    if( m_state != STATE_TYPE_UPDATE )
    {
      return  false;
    }

    if( buttonId == app::BattleUISkillStatusLowerDraw::BUTTON_ID_CLOSE )
    {
      End();
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUISkillStatusFrame::_SetState( const BattleUISkillStatusFrame::StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else if( m_state == STATE_TYPE_OPEN )
    {
        //  画面クラスを作成
      {
        app::BattleUISkillStatusLowerDraw::INIT_PARAM initParam;
        {
          initParam.pDelegate               = this;
          initParam.pLytBuff                = m_pArcReadBuff;
          initParam.pRenderingManager       = m_pAppParam->in_param.pRenderingManager;
          initParam.pUtilHeap               = m_pAppHeap;
          initParam.bUseInputSE             = m_pAppParam->in_param.bUseInputSE;
          initParam.bZenryoku               = m_pAppParam->in_param.bZenryoku;
          initParam.pAppCommonGrpIconData   = m_pAppParam->in_param.pAppCommonGrpIconData;
          {
            /*
            //  タイプ
            PokeType_tag  waza_type;

            //  分類タイプ

            //  技名(使う側でメモリ取得して開放は中で行う)
            gfl2::str::StrBuf*  p_waza_name;

            //  PP(now/max)
            s32 pp_now_num, pp_max_num;

            //  威力
            s32 power_num;

            //  命中
            s32 hit_num;

            //  技説明
            gfl2::str::StrBuf*  p_waza_setumei_string;
            */

            //  メッセージデータ設定
            {
              initParam.pBtlAppMsgData  = m_pAppParam->in_param.pBtlAppMsgData;
              initParam.pBtlWordSet     = m_pAppParam->in_param.pBtlWordSet;
            }

            //  文字列のポインタ設定
            {
              initParam.pWazaSetumeiString = m_pWazaSetumeiStr;
            }

            {
              WazaID  wazaId  = m_pAppParam->in_param.wazaId;

              //  わざidを指定
              {
                initParam.wazaId  = wazaId;

                if( m_pAppParam->in_param.bZenryoku )
                {
                  initParam.zenryokuWazaId    = m_pAppParam->in_param.zenryokuWazaId;
                  initParam.pZenryokuWazaName = m_pAppParam->in_param.pZenryokuWazaName;
                }

                //  関数内で通常とＺ技とで分岐して正しい威力を取得
                // @fix NMCat[1902]/[1904] 汎用Ｚ技と専用Ｚ技では威力計算が異なるのでラッパ関数内でまとめた。
                //                         zenryokuWazaId == WAZANO_NULL であれば通常技のデータ取得になる
                initParam.powerNum = m_pAppParam->in_param.pBtlViewSystem->GetUiDisplay_WazaPower( initParam.wazaId, initParam.zenryokuWazaId );
              }

              //  タイプ
              // @fix NMCat[1902]/[1904] 汎用Ｚ技と専用Ｚ技ではタイプ計算が異なるのでラッパ関数内でまとめた。
              //                         zenryokuWazaId == WAZANO_NULL であれば通常技のデータ取得になる
              initParam.wazaType = static_cast<PokeType_tag>( m_pAppParam->in_param.pBtlViewSystem->GetUiDisplay_WazaType( initParam.wazaId, initParam.zenryokuWazaId ) );

              //  分類
              {
              /*
                enum WazaDamageType {
                  DMG_NONE = 0, ///< へんか
                  DMG_PHYSIC,   ///< 物理ダメージ
                  DMG_SPECIAL,  ///< 特殊ダメージ
                  DMG_MAX,
                };
              */
                // @fix NMCat[1902]/[1904] 汎用Ｚ技と専用Ｚ技ではダメージ分類計算が異なるのでラッパ関数内でまとめた。
                //                         zenryokuWazaId == WAZANO_NULL であれば通常技のデータ取得になる
                initParam.wazaBunruiType  = m_pAppParam->in_param.pBtlViewSystem->GetUiDisplay_WazaDamageType( initParam.wazaId, initParam.zenryokuWazaId );
              }

              //  命中
              // @fix NMCat[1902]/[1904] 汎用Ｚ技と専用Ｚ技では命中計算が異なるのでラッパ関数内でまとめた。
              //                         zenryokuWazaId == WAZANO_NULL であれば通常技のデータ取得になる
              initParam.hitNum   = m_pAppParam->in_param.pBtlViewSystem->GetUiDisplay_WazaHitPer( initParam.wazaId, initParam.zenryokuWazaId );

              //  PP値設定
              initParam.maxPPNum  = m_pAppParam->in_param.ppMaxNum;
              initParam.nowPPNum  = m_pAppParam->in_param.ppNowNum;

              //  技説明
              {
                gfl2::str::MsgData* pWazaInfoMsgData  = m_pAppParam->in_param.pWazaInfoMsgData;
                GFL_ASSERT( pWazaInfoMsgData );

                //  全力技であれば
                if( m_pAppParam->in_param.bZenryoku )
                {
                  pWazaInfoMsgData->GetString( m_pAppParam->in_param.zenryokuWazaId, *initParam.pWazaSetumeiString );
                }
                else
                {
                  pWazaInfoMsgData->GetString( wazaId, *initParam.pWazaSetumeiString );
                }
              }
            }
          }
        }

        m_pDraw = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::BattleUISkillStatusLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );

        //  開始
        {
          app::BattleUISkillStatusLowerDraw::WindowType windowType  = app::BattleUISkillStatusLowerDraw::WINDOW_TYPE_NORMAL;
          if( m_pAppParam->in_param.windowType == WINDOW_TYPE_NORMAL )
          {
          }
          else
          if( m_pAppParam->in_param.windowType == WINDOW_TYPE_INFO )
          {
            windowType  = app::BattleUISkillStatusLowerDraw::WINDOW_TYPE_INFO;
          }
          else
          {
            GFL_ASSERT( 0 );
          }

          m_pDraw->Start( windowType, m_pAppParam->in_param.infoWindowPivotPos );
        }

        m_pDraw->SetInputEnabled( false );
        m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
      }
    }
    else if( m_state == STATE_TYPE_UPDATE )
    {
      m_pDraw->SetInputEnabled( m_pAppParam->in_param.bUserInput );
    }
    else if( m_state == STATE_TYPE_CLOSE )
    {
      m_pDraw->SetInputEnabled( false );

      //  終了
      m_pDraw->End();
    }
  }

GFL_NAMESPACE_END( btl )
