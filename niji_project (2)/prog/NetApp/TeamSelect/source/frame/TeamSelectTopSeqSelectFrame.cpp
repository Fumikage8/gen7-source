//======================================================================
/**
 * @file TeamSelectTopSeqSelectFrame.cpp
 * @date 2015/07/01 15:20:03
 * @author uchida_yuto
 * @brief 先頭画面のシーケンス３択フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/Frame/TeamSelectTopSeqSelectFrame.h"
#include "NetApp/TeamSelect/source/TeamSelectUIViewPool.h"

#include "niji_conv_header/message/msg_battleteam_select.h"

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ず最初に呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TeamSelectTopSeqSelectFrame::Initialize( struct APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );

    //  必要なView設定
    {
      TeamSelectUppRootView*  pUppRootView  = pAppParam->pUIViewPool->GetUppRootView();
      pUppRootView->RemoveAllSubView();

      //  上画面のタイトル表示
      pUppRootView->ShowMessageWindow( msg_bts_sys_01 );
    }

    {
      TeamSelectLowRootView*  pLowRootView  = pAppParam->pUIViewPool->GetLowRootView();
      pLowRootView->RemoveAllSubView();

      TeamSelectLowTopSeqSelectView*  pLowTopView = pAppParam->pUIViewPool->GetLowTopSeqSelectView();

      //  キャンセルボタンをいったん非表示
      pLowRootView->SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, false );

      pLowRootView->SetState( TeamSelectLowRootView::STATE_IDLE );
      pLowTopView->SetState( TeamSelectLowTopSeqSelectView::STATE_IDLE );

      //  メッセージ表示
      {
        app::util::G2DUtil*  pG2DUtl = pLowRootView->GetG2DUtil();
        pG2DUtl->GetString( pG2DUtl->GetTempStrBuf( 0 ), msg_bts_win_05 );

        pLowRootView->ShowMessageWindow( pG2DUtl->GetTempStrBuf( 0 ), false, false, false );
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
  applib::frame::Result TeamSelectTopSeqSelectFrame::InitFunc()
  {
    return applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectTopSeqSelectFrame::UpdateFunc()
  {
    enum
    {
      SEQ_WAIT_EVENT_MESSAGE  = 0,
      SEQ_WAIT_USER_INPUT
    };

    TeamSelectLowTopSeqSelectView*  pLowTopView   = m_pAppParam->pUIViewPool->GetLowTopSeqSelectView();
    TeamSelectLowRootView*          pLowRootView  = m_pAppParam->pUIViewPool->GetLowRootView();

    s32 seq = applib::frame::CellSuper::GetSubSeq();
    if( seq == SEQ_WAIT_EVENT_MESSAGE )
    {
      if( pLowRootView->IsMessageEvent() == false )
      {
        //  先頭画面のViewを登録
        pLowRootView->AddSubView( pLowTopView );

        //  キャンセルボタンを表示
        pLowRootView->SetBtnVisible( TeamSelectLowRootView::BUTTON_ID_BACK, true );

        ++seq;
      }
    }
    else
    if( seq == SEQ_WAIT_USER_INPUT )
    {
      if( pLowTopView->GetState() == TeamSelectLowTopSeqSelectView::STATE_INPUT_STOP )
      {
        m_pAppParam->bEnd = true;

        const Push aPushId[ TeamSelectLowTopSeqSelectView::BUTTON_ID_MAX ] =
        {
          PUSH_TEAM_SETTING,
        };

        GFL_ASSERT( pLowTopView->GetPushBtnId() < GFL_NELEMS( aPushId ) );
        m_pAppParam->push = aPushId[ pLowTopView->GetPushBtnId() ];
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
  void TeamSelectTopSeqSelectFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCの終了を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  applib::frame::Result TeamSelectTopSeqSelectFrame::EndFunc()
  {
    TeamSelectLowRootView*  pLowRootView  = m_pAppParam->pUIViewPool->GetLowRootView();

    //  下画面のメッセージ非表示
    {
      pLowRootView->HideMesssageWindow();
    }

    return applib::frame::RES_FINISH;
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
