//======================================================================
/**
 * @file TeamSelectConfirmSelectPokePartyFrame.cpp
 * @date 2015/07/03 15:20:03
 * @author uchida_yuto
 * @brief バトルチーム選択確認フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/Frame/TeamSelectConfirmSelectPokePartyFrame.h"
#include "NetApp/TeamSelect/source/TeamSelectUIViewPool.h"
#include "NetApp/TeamSelect/source/TeamSelectShareParam.h"

#include "PokeTool/include/PokeToolBattleParty.h"

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
  void TeamSelectConfirmSelectPokePartyFrame::Initialize( struct APP_PARAM* pAppParam )
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

      //  選択チーム表示を指定位置にして内容を反映
      {
        pUppEntryPokePartyView->SetYPosPokePartyFrame( pUppEntryPokePartyView->GetDefaultPos().y );
        //  チーム選択のQRを選択してもここのフレームにはこない。
        pUppEntryPokePartyView->SetViewParam( pAppParam->in.pSelectTeamViewParam, true );
      }

      //  上画面のタイトル表示
      pUppRootView->ShowMessageWindow( msg_bts_sys_01 );
    }

    {
      TeamSelectLowRootView*  pLowRootView  = pAppParam->in.pUIViewPool->GetLowRootView();
      GFL_ASSERT( pLowRootView );
      pLowRootView->RemoveAllSubView();

      //  2択のViewを登録
      TeamSelectLowTwoItemSelectView*  pLowTwoItemSelectView = pAppParam->in.pUIViewPool->GetLowTwoItemSelectView();
      {
        //  選択項目の設定
        pLowTwoItemSelectView->SetTextBtn( TeamSelectLowTwoItemSelectView::BUTTON_ID_ONE_SELECT, msg_bts_sel_04 );
        pLowTwoItemSelectView->SetTextBtn( TeamSelectLowTwoItemSelectView::BUTTON_ID_TWO_SELECT, msg_bts_sel_05 );
        pLowTwoItemSelectView->SetCursorPos( TeamSelectLowTwoItemSelectView::BUTTON_ID_ONE_SELECT );
      }

      pLowRootView->SetState( TeamSelectLowRootView::STATE_IDLE );
      pLowTwoItemSelectView->SetState( TeamSelectLowTwoItemSelectView::STATE_IDLE );

      //  ルートViewのキャンセルボタンを非表示
      pLowRootView->SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, false );

      //  通知メッセージ
      {
        //  チーム名に置換が必要
        app::util::G2DUtil*  pG2DUtl = pLowRootView->GetG2DUtil();

        //  ワードセットで文字列をチーム名に置換
        pG2DUtl->SetRegisterWord( 0, *( pAppParam->in.pSelectTeamViewParam->pTeamData->pPokeParty->GetTeamName() ) );

        u32 textId  = msg_bts_win_06;
        if( pAppParam->in.tournamentType == TOURNAMENT_TYPE_LIVE )
        {
          //  ライブ大会
          // @fix NMCat[2830] ライブ大会でも警告分を出すように変更
          textId  = msg_bts_win_03;
        }
        else
        if( pAppParam->in.tournamentType == TOURNAMENT_TYPE_NET )
        {
          //  ネット大会
          textId  = msg_bts_win_03;
        }
        else
        {
          textId  = msg_bts_win_06;
        }

        pG2DUtl->GetStringExpand( pG2DUtl->GetTempStrBuf( 1 ), textId );

        pLowRootView->ShowMessageWindow( pG2DUtl->GetTempStrBuf( 1 ), false, false, false );
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
  applib::frame::Result TeamSelectConfirmSelectPokePartyFrame::InitFunc()
  {
    return applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectConfirmSelectPokePartyFrame::UpdateFunc()
  {
    enum
    {
      SEQ_WAIT_START_MESSAGE_EVENT  = 0,
      SEQ_INPUT
    };

    s32 seq = applib::frame::CellSuper::GetSubSeq();

    TeamSelectLowRootView*  pLowRootView  = m_pAppParam->in.pUIViewPool->GetLowRootView();
    TeamSelectLowTwoItemSelectView*  pLowTwoItemSelectView = m_pAppParam->in.pUIViewPool->GetLowTwoItemSelectView();

    if( seq == SEQ_WAIT_START_MESSAGE_EVENT )
    {
      if( pLowRootView->IsMessageEvent() == false )
      {
        pLowRootView->AddSubView( pLowTwoItemSelectView );
        ++seq;
      }
    }
    else
    if( seq == SEQ_INPUT )
    {
      if( pLowTwoItemSelectView->GetState() == TeamSelectLowTwoItemSelectView::STATE_INPUT_STOP )
      {
        m_pAppParam->out.bEnd = true;

        TeamSelectLowTwoItemSelectView::ButtonId  pushBtnId = pLowTwoItemSelectView->GetPushBtnId();
        if( pushBtnId == TeamSelectLowTwoItemSelectView::BUTTON_ID_ONE_SELECT )
        {
          m_pAppParam->out.push = PUSH_YES;
        }
        else
        if( pushBtnId == TeamSelectLowTwoItemSelectView::BUTTON_ID_TWO_SELECT )
        {
          m_pAppParam->out.push = PUSH_NO;
        }
        else
        {
          //  論理バグ
          GFL_ASSERT( 0 );
        }
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
  void TeamSelectConfirmSelectPokePartyFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの終了を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectConfirmSelectPokePartyFrame::EndFunc()
  {    
    return applib::frame::RES_FINISH;
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
