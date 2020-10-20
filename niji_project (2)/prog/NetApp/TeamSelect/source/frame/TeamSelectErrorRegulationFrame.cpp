//======================================================================
/**
 * @file TeamSelectErrorRegulationFrame.cpp
 * @date 2015/07/03 15:20:03
 * @author uchida_yuto
 * @brief バトルチーム選択レギュレーション違反フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/Frame/TeamSelectErrorRegulationFrame.h"
#include "NetApp/TeamSelect/source/TeamSelectUIViewPool.h"

#include <macro/include/gfl2_Macros.h>

#include "niji_conv_header/message/msg_battleteam_select.h"

//  nijiのインクルード
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"
#include "AppLib/include/Util/app_util_Heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //  違反メッセージテキスト最大数
  static const u32 s_cErrorMessageTextMax = 18 * 10 * 3;

  //  画面のAボタンのテキスト最大数
  static const u32 s_cEnterButtonTextMax  = 10 * 3;

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ず最初に呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TeamSelectErrorRegulationFrame::Initialize( struct APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    GFL_ASSERT( pAppParam->in.pUIViewPool );

    const struct TeamSelectUIViewPool::INIT_PARAM&  rUIViewPoolInitParam  = pAppParam->in.pUIViewPool->GetInitParam();

    //  必要なView設定
    {
      TeamSelectUppRootView*  pUppRootView  = pAppParam->in.pUIViewPool->GetUppRootView();
      GFL_ASSERT( pUppRootView );
      pUppRootView->RemoveAllSubView();

      TeamSelectUppEntryPokePartyView*  pUppEntryPokePartyView  = pAppParam->in.pUIViewPool->GetUppEntryPokePartyView();
      GFL_ASSERT( pUppEntryPokePartyView );
      pUppRootView->AddSubView( pUppEntryPokePartyView );

      //  上画面のタイトル表示
      pUppRootView->ShowMessageWindow( msg_bts_sys_01 );
    }

    //  下画面の生成
    {
      TeamSelectLowRootView*  pLowRootView  = pAppParam->in.pUIViewPool->GetLowRootView();
      GFL_ASSERT( pLowRootView );
      pLowRootView->RemoveAllSubView();

      //  インフォウィンドウのロード
      {
        app::tool::InfoWindowBookType*  pInfoWindowBookType = pAppParam->in.pUIViewPool->GetLowInfoWindowBookType();
        GFL_ASSERT( pInfoWindowBookType );

        // @fix GFNMCat[1287] 使う側でレギュレーション違反ボタンを押せないように選択させる
        pInfoWindowBookType->SetCommandButtonActive( pAppParam->in.bOpenRegulationButton );
        pLowRootView->AddSubView( pInfoWindowBookType );
        
        pLowRootView->SetState( TeamSelectLowRootView::STATE_IDLE );
      }

      //  違反メッセージ生成
      {
        struct ErrorMessageData
        {
          REGULATION_NG_TYPE  type;
          s32 msgId;
        };

        const struct ErrorMessageData aErrorMessageTable[]  =
        {
          { REGULATION_NG_TYPE_EGG        , msg_bts_violation_01 }, ///< タマゴチェック
          { REGULATION_NG_TYPE_NUM_LOW    , msg_bts_violation_02 }, ///< ポケモン数下限チェック
          { REGULATION_NG_TYPE_NUM_HI     , msg_bts_violation_03 }, ///< ポケモン数上限チェック
          { REGULATION_NG_TYPE_MAST_POKE  , msg_bts_violation_04 }, ///< 必須ポケモン（全国図鑑＃）チェック
          { REGULATION_NG_TYPE_VETO_WAZA  , msg_bts_violation_05 }, ///< 禁止わざチェック
          { REGULATION_NG_TYPE_VETO_POKE  , msg_bts_violation_06 }, ///< 禁止ポケモンチェック
          { REGULATION_NG_TYPE_VETO_ITEM  , msg_bts_violation_07 }, ///< 禁止どうぐチェック
          { REGULATION_NG_TYPE_LEVEL      , msg_bts_violation_08 }, ///< 参加レベル指定チェック
          { REGULATION_NG_TYPE_BOTHPOKE   , msg_bts_violation_09 }, ///< 同種ポケモンチェック
          { REGULATION_NG_TYPE_BOTHITEM   , msg_bts_violation_10 }, ///< 同じどうぐチェック
          { REGULATION_NG_TYPE_NUM_LEGEND , msg_bts_violation_11 }, ///< 伝説ポケモン数チェック
          { REGULATION_NG_TYPE_QR         , msg_bts_violation_12 }, ///< QRバトルチーム使用チェック
          { REGULATION_NG_TYPE_SPECIAL    , msg_bts_violation_13 }, ///< 特別な条件を満たすポケモンチェック
          { REGULATION_NG_TYPE_COMPATIBLE , msg_bts_violation_14 } ///< niji通信互換チェック
        };

        u32 loopNum               = pAppParam->in.NGTypeNum;
        u32 errorMessagaTableNum  = GFL_NELEMS( aErrorMessageTable );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pErrorMessageStrBufArray[ i ] = GFL_NEW( rUIViewPoolInitParam.pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( s_cErrorMessageTextMax, rUIViewPoolInitParam.pUtilHeap->GetDeviceHeap() );

          for( u32 table_i = 0; table_i < errorMessagaTableNum; ++table_i )
          {
            if( pAppParam->in.aResultList[ i ].NGType == aErrorMessageTable[ table_i ].type )
            {
              rUIViewPoolInitParam.pAppMsgData->GetString( aErrorMessageTable[ table_i ].msgId, *m_pErrorMessageStrBufArray[ i ] );
              break;
            }
          }
        }
      }

      //  違反画面の決定ボタンテキスト生成
      {
        m_pEnterButtonTextStrBuf  = GFL_NEW( rUIViewPoolInitParam.pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( s_cEnterButtonTextMax, rUIViewPoolInitParam.pUtilHeap->GetDeviceHeap() );
        rUIViewPoolInitParam.pAppMsgData->GetString( msg_bts_sel_08, *m_pEnterButtonTextStrBuf );
      }

      //  ルートViewのキャンセルボタンを非表示
      pLowRootView->SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, false );

      //  ウィンドウは非表示
      pLowRootView->HideMesssageWindow();
    }

    m_pAppParam = pAppParam;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの初期化を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectErrorRegulationFrame::InitFunc()
  {
    app::tool::InfoWindowBookType*  pInfoWindowBookType = m_pAppParam->in.pUIViewPool->GetLowInfoWindowBookType();
    if( pInfoWindowBookType->IsCreated() == true )
    {
      //  各ページ設定
      u32 loopNum               = m_pAppParam->in.NGTypeNum;

      //  ページを１ページ目に
      pInfoWindowBookType->ResetContents( loopNum, true );

      //  各ページのテキストを設定
      for( u32 i = 0; i < loopNum; ++i )
      {
        pInfoWindowBookType->SetTextContent( i, m_pErrorMessageStrBufArray[ i ], m_pEnterButtonTextStrBuf );
      }

      //  上画面の表示更新
      _UpdateTeamViewMaskDraw( 0 );

      gfl2::math::Vector3 infoPos( 0, 0, 0 );
      pInfoWindowBookType->StartOpen( this, infoPos );

      return applib::frame::RES_FINISH;
    }

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectErrorRegulationFrame::UpdateFunc()
  {
    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン描画を行う関数
    *          純粋仮想ではありません。
    */
  //------------------------------------------------------------------
  void TeamSelectErrorRegulationFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの終了を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectErrorRegulationFrame::EndFunc()
  {
    enum
    {
      SEQ_START = 0,
      SEQ_WAIT,
      SEQ_END
    };

    s32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == SEQ_START )
    {
      app::tool::InfoWindowBookType*  pInfoWindowBookType = m_pAppParam->in.pUIViewPool->GetLowInfoWindowBookType();
      pInfoWindowBookType->RemoveFromSuperView();

      ++seq;
    }

    if( seq == SEQ_WAIT )
    {
      ++seq;
    }

    if( seq == SEQ_END )
    {
      u32 loopNum = m_pAppParam->in.NGTypeNum;
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_pErrorMessageStrBufArray[ i ] );
      }

      GFL_SAFE_DELETE( m_pEnterButtonTextStrBuf );

      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return  applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief     チームビューのマスク描画更新
    * @param[in] 違反したレギュレーションデータidx
    */
  //------------------------------------------------------------------
  void TeamSelectErrorRegulationFrame::_UpdateTeamViewMaskDraw( const u32 errorRegulationDataIdx )
  {
    GFL_ASSERT( errorRegulationDataIdx <  m_pAppParam->in.NGTypeNum );
    TeamSelectUppEntryPokePartyView*  pUppEntryPokePartyView  = m_pAppParam->in.pUIViewPool->GetUppEntryPokePartyView();
    GFL_ASSERT( pUppEntryPokePartyView );

    REGULATION_CHECK_RESULT*  pCheckResultData  = &m_pAppParam->in.aResultList[ errorRegulationDataIdx ];

    u32 loopNum = GFL_NELEMS( pCheckResultData->isPokeNG );
    for( u32 i = 0; i < loopNum; ++i )
    {
      pUppEntryPokePartyView->SetEnableNGFrame( i, pCheckResultData->isPokeNG[ i ] );
    }
  }

//  app::tool::InfoWindowBookTypeListener の仮想関数
  //! @brief  クローズし切ったときに呼ばれるイベントハンドラ
  void TeamSelectErrorRegulationFrame::OnClose()
  {
    m_pAppParam->out.bEnd = true;
    if( m_pAppParam->out.push == PUSH_NONE )
    {
      m_pAppParam->out.push = PUSH_CANCEL;
    }
  }

  //! @brief  ページが変わり切ったときに呼ばれるイベントハンドラ
  //! @param[in]  pageNo  これから表示するページ番号(0から開始。表示上の1ページ目が0になる。)
  //! @note  オープンし切ったときにも呼ばれる
  void TeamSelectErrorRegulationFrame::OnChangePage( u32 pageNo )
  {
    //  上画面の表示更新
    _UpdateTeamViewMaskDraw( pageNo );
  }

  //! @brief  コマンドのボタン(Aボタン)を押し切ったときに呼ばれるイベントハンドラ
  //! @param[in]  pageNo  表示されているページ番号(0から開始。表示上の1ページ目が0になる。)
  void TeamSelectErrorRegulationFrame::OnCommand( u32 pageNo )
  {
    app::tool::InfoWindowBookType*  pInfoWindowBookType = m_pAppParam->in.pUIViewPool->GetLowInfoWindowBookType();

    pInfoWindowBookType->StartClose();

    m_pAppParam->out.push = PUSH_CHK_REGULATION;
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
