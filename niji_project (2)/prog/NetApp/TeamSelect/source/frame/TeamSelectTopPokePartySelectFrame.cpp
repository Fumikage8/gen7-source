//======================================================================
/**
 * @file TeamSelectTopPokePartySelectFrame.cpp
 * @date 2015/07/02 15:20:03
 * @author uchida_yuto
 * @brief 先頭画面のポケモンパーティ選択フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/Frame/TeamSelectTopPokePartySelectFrame.h"
#include "NetApp/TeamSelect/source/TeamSelectUIViewPool.h"
#include "NetApp/TeamSelect/source/TeamSelectShareParam.h"

//  gflib2のインクルード
#include <math/include/gfl2_Easing.h>
#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
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
  void TeamSelectTopPokePartySelectFrame::Initialize( struct APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;

    //  必要なView設定
    {
      TeamSelectUppRootView*  pUppRootView  = pAppParam->in.pUIViewPool->GetUppRootView();
      pUppRootView->RemoveAllSubView();

      TeamSelectUppEntryPokePartyView*  pUppEntryPokePartyView  = pAppParam->in.pUIViewPool->GetUppEntryPokePartyView();
      pUppRootView->AddSubView( pUppEntryPokePartyView );

      //  初回は上画面のポケモンパーティーは非表示
      pUppEntryPokePartyView->SetVisible( false );

      //  上画面のタイトル表示
      pUppRootView->ShowMessageWindow( msg_bts_sys_01 );
    }

    {
      TeamSelectLowRootView*  pLowRootView  = pAppParam->in.pUIViewPool->GetLowRootView();
      pLowRootView->RemoveAllSubView();

      //  先頭画面のViewを登録
      TeamSelectLowTopPokePartySelectView*  pLowTopView = pAppParam->in.pUIViewPool->GetLowTopPokePartySelectView();
      
      pLowRootView->AddSubView( pLowTopView );

      //  開始
      pLowTopView->Start();

      pLowRootView->SetState( TeamSelectLowRootView::STATE_IDLE );
      pLowTopView->SetState( TeamSelectLowTopPokePartySelectView::STATE_IDLE );

      //  キャンセルボタンを表示
      pLowRootView->SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, true );

      //  メッセージ表示
      _ShowHelpMessage();
      {
        app::util::G2DUtil*  pG2DUtl = pLowRootView->GetG2DUtil();
        pG2DUtl->GetString( pG2DUtl->GetTempStrBuf( 0 ), msg_bts_win_04 );

        pLowRootView->ShowTitleMessageWindow( pG2DUtl->GetTempStrBuf( 0 ), true );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief    PROCの初期化を行う関数
    * @return   Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectTopPokePartySelectFrame::InitFunc()
  {
    return applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
    * @brief    PROCのメイン更新を行う関数
    * @return   Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectTopPokePartySelectFrame::UpdateFunc()
  {
    GFL_ASSERT( m_pAppParam );

    if( m_pAppParam->out.bEnd == true )
    {
      return applib::frame::RES_CONTINUE;
    }

    TeamSelectLowTopPokePartySelectView*  pLowTopView = m_pAppParam->in.pUIViewPool->GetLowTopPokePartySelectView();
    TeamSelectLowRootView*  pLowRootView  = m_pAppParam->in.pUIViewPool->GetLowRootView();

    s32 seq = applib::frame::CellSuper::GetSubSeq();
    if( seq == SEQ_USER_INPUT )
    {
      //  Viewの処理が終了
      if( pLowTopView->GetState() == TeamSelectLowTopPokePartySelectView::STATE_INPUT_STOP )
      {
        //  何を入力して終了したか
        TeamSelectLowTopPokePartySelectView::ButtonId pushButtonId  = pLowTopView->GetPushBtnId();

        //  選択リストidxを取得
        m_pAppParam->out.selectTeamIdx  = pLowTopView->GetCurrentListIdx();

        //  いれかえボタンを入力
        if( pushButtonId == TeamSelectLowTopPokePartySelectView::BUTTON_ID_TEAM_SETTING )
        {
          //  終了チーム編成へ
          m_pAppParam->out.push = PUSH_TEAM_SETTING;
          m_pAppParam->out.bEnd = true;
        }
        else
        //  決定ボタンを入力
        if( pushButtonId == TeamSelectLowTopPokePartySelectView::BUTTON_ID_TEAM_ENTER )
        {
          //  ロックされているかチェック
          const TEAM_VIEW_PARAM&  rTeamViewData = pLowTopView->GetTeamViewListItemParam( m_pAppParam->out.selectTeamIdx );

          TeamStatus  status  = rTeamViewData.pTeamData->status;
          if( status == TEAM_STATUS_PARTY_SELECT )
          {
            if( rTeamViewData.pTeamData->lockStatus != LOCK_STATUS_NONE )
            {
              //  ロック中のメッセージを出す
              TeamLockMessageEventType  messageEventType  = TEAM_LOCK_MESSAGE_EVENT_TYPE_LIVE;

              //  ライブ大会用
              if( rTeamViewData.pTeamData->lockStatus == LOCK_STATUS_LIVE )
              {
                messageEventType  = TEAM_LOCK_MESSAGE_EVENT_TYPE_LIVE;
              }
              else
              //  通信大会用
              if( rTeamViewData.pTeamData->lockStatus == LOCK_STATUS_NET )
              {
                messageEventType  = TEAM_LOCK_MESSAGE_EVENT_TYPE_NET;
              }
              //  仲間大会用
              else
              if( rTeamViewData.pTeamData->lockStatus == LOCK_STATUS_FRIEND )
              {
                messageEventType  = TEAM_LOCK_MESSAGE_EVENT_TYPE_FRIEND;
              }
              else
              {
                GFL_ASSERT( 0 );
              }

              //  ロックメッセージイベント開始
              if( _StartTeamLockMessageEvent( messageEventType ) == true )
              {
                seq = SEQ_WAIT_LOCK_MESSAGE;
                applib::frame::CellSuper::SetSubSeq( seq );
              }
              else
              {
                GFL_ASSERT( 0 );
              }
            }
            else
            //  上画面のポケモンパーティーのViewを表示
            {
              TeamSelectUppEntryPokePartyView*  pUppEntryPokePartyView  = m_pAppParam->in.pUIViewPool->GetUppEntryPokePartyView();

              pUppEntryPokePartyView->SetViewParam( &rTeamViewData, false );
              pUppEntryPokePartyView->SetVisible( true );

              seq = SEQ_SELECT_EVENT;
              applib::frame::CellSuper::SetSubSeq( seq );
            }
          }
          else
          if( status == TEAM_STATUS_QR_READ_REQUEST )
          {
            //  QR読み込みのアプリへ遷移してすぐにフレーム終了フラグを立てる
            m_pAppParam->out.push = PUSH_SELECT_QR_PARTY;
            m_pAppParam->out.bEnd = true;
          }
          else
          if( status == TEAM_STATUS_NONE )
          {
            //  もう一度入力処理に戻る
            _ReturnSeqUserInput();
          }
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }

    if( seq == SEQ_WAIT_LOCK_MESSAGE )
    {
      if( pLowRootView->IsMessageEvent() == false )
      {
        //  メッセージイベントが終了したので入力処理に戻る
        _ReturnSeqUserInput();
      }
    }

    if( seq == SEQ_SELECT_EVENT )
    {
      TeamSelectUppEntryPokePartyView*  pUppEntryPokePartyView  = m_pAppParam->in.pUIViewPool->GetUppEntryPokePartyView();
      
      //  下画面の選択したチーム項目が下から上に移動
      bool  bEndMove  = false;

      //  補完のスピード値はてきとう
      m_moveTime += 0.1f;
      if( 1.0f <= m_moveTime )
      {
        m_moveTime  = 1.0f;
        bEndMove  = true;
      }

      //  下画面の選択したチーム項目のY座標を移動
      {
        f32 centerYPos = ( DISPLAY_LOWER_HEIGHT / 2 ) + ( DISPLAY_UPPER_HEIGHT / 2 + pUppEntryPokePartyView->GetDefaultPos().y );
        f32 nextYPos  = gfl2::math::Easing::GetValue( gfl2::math::Easing::IN_SINE, pLowTopView->GetListYPos(), centerYPos, m_moveTime );

        pLowTopView->SetPosYPokePartyFrame( pLowTopView->GetCurrentLayoutListIdx(), nextYPos );
      }

      //  上画面のチーム項目のオブジェクトのY座標を指定
      {
        f32 beginYPos = ( -DISPLAY_LOWER_HEIGHT / 2 + pLowTopView->GetListYPos() ) + ( -DISPLAY_UPPER_HEIGHT / 2 );
        f32 nextYPos  = gfl2::math::Easing::GetValue( gfl2::math::Easing::IN_SINE, beginYPos, pUppEntryPokePartyView->GetDefaultPos().y, m_moveTime );

        pUppEntryPokePartyView->SetYPosPokePartyFrame( nextYPos );
      }

      if( bEndMove == true )
      {
        //  フレーム終了フラグを立てて、ポケモンパーティー選択
        m_pAppParam->out.push = PUSH_SELECT_POKE_PARTY;
        m_pAppParam->out.bEnd = true;
      }
    }

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン描画を行う関数
    *          純粋仮想ではありません。
    */
  //------------------------------------------------------------------
  void TeamSelectTopPokePartySelectFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの終了を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectTopPokePartySelectFrame::EndFunc()
  {
    TeamSelectLowRootView*  pLowRootView  = m_pAppParam->in.pUIViewPool->GetLowRootView();

    //  下画面のメッセージ非表示
    {
      pLowRootView->HideMesssageWindow();
    }

    return applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
    * @brief チームロックメッセージイベント開始
    */
  //------------------------------------------------------------------
  bool TeamSelectTopPokePartySelectFrame::_StartTeamLockMessageEvent( const TeamLockMessageEventType type )
  {
    TeamSelectLowTopPokePartySelectView*  pLowTopView = m_pAppParam->in.pUIViewPool->GetLowTopPokePartySelectView();
    TeamSelectLowRootView*  pLowRootView  = m_pAppParam->in.pUIViewPool->GetLowRootView();

    app::util::G2DUtil*  pG2DUtl = pLowRootView->GetG2DUtil();

    u32 textId  = msg_bts_win_01;
    if( type == TEAM_LOCK_MESSAGE_EVENT_TYPE_LIVE )
    {
      textId  = msg_bts_win_01;
    }
    else
    if( type == TEAM_LOCK_MESSAGE_EVENT_TYPE_NET )
    {
      textId  = msg_bts_win_02;
    }
    // @fix GFNMCat[2674] 「仲間大会」用のロックメッセージがなかったので追加
    else
    if( type == TEAM_LOCK_MESSAGE_EVENT_TYPE_FRIEND )
    {
      textId  = msg_bts_win_09;
    }
    else
    {
      //  それ以外の場合はデフォルト
      textId  = msg_bts_win_02;
    }

    pG2DUtl->GetString( pG2DUtl->GetTempStrBuf( 0 ), textId );
    pLowRootView->ShowMessageWindow( pG2DUtl->GetTempStrBuf( 0 ), false, false, false );

    //  下画面にはウィンドウ以外は非表示に
    pLowTopView->RemoveFromSuperView();
    pLowRootView->SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, false );

    // @fix GFNMCat[5087] ヘルプメッセージ非表示
    pLowRootView->HideTitleMessageWindow();

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   処理を再起動
    * @note
    *          リストViewの入力処理処理が終わった後に再度処理を動かす時に実行
    */
  //------------------------------------------------------------------
  void TeamSelectTopPokePartySelectFrame::_ReturnSeqUserInput()
  {
    TeamSelectLowTopPokePartySelectView*  pLowTopView = m_pAppParam->in.pUIViewPool->GetLowTopPokePartySelectView();
    TeamSelectLowRootView*  pLowRootView  = m_pAppParam->in.pUIViewPool->GetLowRootView();

    pLowRootView->SetState( TeamSelectLowRootView::STATE_IDLE );
    pLowTopView->SetState( TeamSelectLowTopPokePartySelectView::STATE_IDLE );

    //  下画面の表示を復帰
    if( pLowTopView->GetSuperView() == NULL )
    {
      pLowRootView->AddSubView( pLowTopView );
    }

    pLowRootView->SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, true );

    applib::frame::CellSuper::SetSubSeq( SEQ_USER_INPUT );

    // @fix GFNMCat[5087] ヘルプメッセージ表示
    _ShowHelpMessage();
  }

  //  ヘルプメッセージを表示
  void TeamSelectTopPokePartySelectFrame::_ShowHelpMessage()
  {
    //  先頭画面のViewを登録
    TeamSelectLowRootView*  pLowRootView  = m_pAppParam->in.pUIViewPool->GetLowRootView();

    app::util::G2DUtil*  pG2DUtl = pLowRootView->GetG2DUtil();
    pG2DUtl->GetString( pG2DUtl->GetTempStrBuf( 0 ), msg_bts_win_04 );

    pLowRootView->ShowTitleMessageWindow( pG2DUtl->GetTempStrBuf( 0 ), true );
  }



GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
