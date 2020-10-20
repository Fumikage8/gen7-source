//======================================================================
/**
 * @file TeamSelectServerRequestRegulationFrame.cpp
 * @date 2015/07/03 15:20:03
 * @author uchida_yuto
 * @brief バトルチーム選択サーバー通知レギュレーション
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/Frame/TeamSelectServerRequestRegulationFrame.h"
#include "NetApp/TeamSelect/source/TeamSelectUIViewPool.h"

#include <macro/include/gfl2_Macros.h>

#include "niji_conv_header/message/msg_battleteam_select.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ず最初に呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TeamSelectServerRequestRegulationFrame::Initialize( struct APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );

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

    {
      TeamSelectLowRootView*  pLowRootView  = pAppParam->in.pUIViewPool->GetLowRootView();
      GFL_ASSERT( pLowRootView );
      pLowRootView->RemoveAllSubView();

      pLowRootView->SetState( TeamSelectLowRootView::STATE_IDLE );

      //  ルートViewのキャンセルボタンを非表示
      pLowRootView->SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, false );

      {
        app::util::G2DUtil* pG2DUtil  = pLowRootView->GetG2DUtil();

        pG2DUtil->GetString( pG2DUtil->GetTempStrBuf( 0 ), msg_bts_win_08 );
        pLowRootView->ShowMessageWindow( pG2DUtil->GetTempStrBuf( 0 ), false, true, true );
      }
    }

    m_pAppParam = pAppParam;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの初期化を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectServerRequestRegulationFrame::InitFunc()
  {
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in.pServerRequestClient );

    m_pAppParam->out.resultServerData.Clear();

#if defined(GF_PLATFORM_CTR)
    m_pAppParam->in.pServerRequestClient->CheckRegulationAndValidationRequest( this, m_pAppParam->in.serverRequestClientParam );
#else
     
     // レギュレーション違反のテスト設定
#if 0
      //  test
      {
        m_pAppParam->out.resultServerData.data.regulationNG.NGTypeNum = 3;

        struct REGULATION_CHECK_RESULT* pCheckResultDatam = NULL;

        {
          pCheckResultDatam = &m_pAppParam->out.resultServerData.data.regulationNG.aResultList[ 0 ];
          pCheckResultDatam->NGType = REGULATION_NG_TYPE_EGG;
          pCheckResultDatam->isPokeNG[ 0 ]  = true;
        }

        {
          pCheckResultDatam = &m_pAppParam->out.resultServerData.data.regulationNG.aResultList[ 1 ];
          pCheckResultDatam->NGType = REGULATION_NG_TYPE_BOTHITEM;
          pCheckResultDatam->isPokeNG[ 1 ]  = true;
        }

        {
          pCheckResultDatam = &m_pAppParam->out.resultServerData.data.regulationNG.aResultList[ 2 ];
          pCheckResultDatam->NGType = REGULATION_NG_TYPE_SPECIAL;
          pCheckResultDatam->isPokeNG[ 3 ]  = true;
        }

        m_pAppParam->out.resultServerData.type = SERVER_RESULT_TYPE_REGULATION_NG;
      }
#else
      m_pAppParam->out.resultServerData.type  = SERVER_RESULT_TYPE_SUCCESS;
#endif

#endif

    return applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectServerRequestRegulationFrame::UpdateFunc()
  {
    enum
    {
      SEQ_SERVER_REQUEST_WAIT = 0,
      SEQ_SERVER_ERROR_MESSAGE
    };

    int seq = applib::frame::CellSuper::GetSubSeq();

    if( seq ==  SEQ_SERVER_REQUEST_WAIT )
    {
      if( m_pAppParam->out.resultServerData.type == SERVER_RESULT_TYPE_NONE )
      {
        //  リクエスト待ち
      }
      else
      if( m_pAppParam->out.resultServerData.type == SERVER_RESULT_TYPE_SUCCESS )
      {
        //  成功
        m_pAppParam->out.bEnd = true;
      }
      else
      if( m_pAppParam->out.resultServerData.type == SERVER_RESULT_TYPE_REGULATION_NG )
      {
        //  レギュレーション違反
        m_pAppParam->out.bEnd = true;
      }
      else
      if( m_pAppParam->out.resultServerData.type == SERVER_RESULT_TYPE_POKE_ERROR )
      {
        TeamSelectLowRootView*  pLowRootView  = m_pAppParam->in.pUIViewPool->GetLowRootView();
        GFL_ASSERT( pLowRootView );

        //  ポケモン不正
        // @fix NMCat[1874] ポケモン不正についての対処忘れがあったので対応
        {
          app::util::G2DUtil* pG2DUtil  = pLowRootView->GetG2DUtil();

          pG2DUtil->GetString( pG2DUtil->GetTempStrBuf( 1 ), msg_bts_win_07 );
          pLowRootView->ShowMessageWindow( pG2DUtil->GetTempStrBuf( 1 ), false, false, false );
        }

        seq = SEQ_SERVER_ERROR_MESSAGE;
      }
      else
      if( m_pAppParam->out.resultServerData.type == SERVER_RESULT_TYPE_ERROR )
      {
        //  エラー
        m_pAppParam->out.bEnd = true;
      }
    }
    else
    if( seq == SEQ_SERVER_ERROR_MESSAGE )
    {
      TeamSelectLowRootView*  pLowRootView  = m_pAppParam->in.pUIViewPool->GetLowRootView();
      GFL_ASSERT( pLowRootView );

      if( pLowRootView->IsMessageEvent() == false )
      {
        m_pAppParam->out.bEnd = true;        
      }      
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン描画を行う関数
    *          純粋仮想ではありません。
    */
  //------------------------------------------------------------------
  void TeamSelectServerRequestRegulationFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの終了を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectServerRequestRegulationFrame::EndFunc()
  {
    
    return applib::frame::RES_FINISH;
  }

//  ここからは TeamSelectResponseListenrの関数
  //--------------------------------------------------------------------------------------------
  /**
    * @brief   チェック成功
    */
  //--------------------------------------------------------------------------------------------
  void TeamSelectServerRequestRegulationFrame::OnCheckSuccess()
  {
    GFL_PRINT( "TeamSelectRequestClient::OnCheckSuccess\n" );

    m_pAppParam->out.resultServerData.type = SERVER_RESULT_TYPE_SUCCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   レギュレーション違反
    * @param   NGTypeNum            NGになった検査種別の数（＝違反表示画面のページ数）
    * @param   checkPokeResultArray 検査結果の配列(NGTypeNum - 1までアクセス可能)
    */
  //--------------------------------------------------------------------------------------------
  void TeamSelectServerRequestRegulationFrame::OnRegulationNG( u32 NGTypeNum , REGULATION_CHECK_RESULT* checkResultArray )
  {
    GFL_PRINT( "TeamSelectRequestClient::OnRegulationNG\n" );

    m_pAppParam->out.resultServerData.type = SERVER_RESULT_TYPE_REGULATION_NG;
    m_pAppParam->out.resultServerData.data.regulationNG.NGTypeNum    = NGTypeNum;

    //  元データがいつ消えているかわからないのでコピーする
    gfl2::std::MemCopy( checkResultArray, m_pAppParam->out.resultServerData.data.regulationNG.aResultList, sizeof( m_pAppParam->out.resultServerData.data.regulationNG.aResultList[ 0 ] ) * NGTypeNum );
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   ポケモンが不正
    */
  //--------------------------------------------------------------------------------------------
  void TeamSelectServerRequestRegulationFrame::OnValidationNG()
  {
    GFL_PRINT( "TeamSelectRequestClient::OnValidationNG\n" );

    m_pAppParam->out.resultServerData.type = SERVER_RESULT_TYPE_POKE_ERROR;
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   通信エラー
    */
  //--------------------------------------------------------------------------------------------
  void TeamSelectServerRequestRegulationFrame::OnError( void )
  {
    GFL_PRINT( "TeamSelectRequestClient::OnError\n" );

    m_pAppParam->out.resultServerData.type = SERVER_RESULT_TYPE_ERROR;
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
