//=============================================================================
/**
 * @file	 BpxP2pTradeState.cpp
 * @brief	 アイテムの移動状態
 * @author k.ohno
 * @date	 2016.2.28
 */
//=============================================================================
#pragma once


#include "../BoxPokepara.h"
#include "../BoxSimpleAccesser.h"
#include "../BoxLowBgFrame.h"
#include "../BoxLowBgDraw.h"
#include "../BoxLowChtDraw.h"

#include <arc_index/debug_message.gaix>

#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>

#include <arc_index/script_message.gaix>

#include "AppLib/include/Frame/AppFrameManager.h"

#include <Fade/include/gfl2_FadeManager.h>
#include "PokeTool/include/PokeToolPresentPokemon.h"

#include "App/Box/include/BoxAppParam.h"
#include "App/Box/include/BoxAppSetup.h"

#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/BoxPokemonSave.h"
#include "Savedata/include/ResortSave.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"

#include "niji_conv_header/message/msg_box.h"

#include "BoxP2pTradeState.h"
#include "BoxWarrningMessageState.h"

#include "../function/BoxPokemonMoveFunction.h"


#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )


//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
BoxP2pTradeState::BoxP2pTradeState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxP2pTradeState::~BoxP2pTradeState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxP2pTradeState::Setup(void)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpCoreTemp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  mpPartyTemp = GFL_NEW( mpAppHeap->GetDeviceHeap() ) pml::PokeParty( mpAppHeap->GetDeviceHeap() );
  mInnerPeaceListener.SetThis(this);

}


void BoxP2pTradeState::Release(void)
{
  GFL_SAFE_DELETE(mpPartyTemp);
  GFL_SAFE_DELETE(mpCoreTemp);
}

void BoxP2pTradeState::BoxP2pTradeLocalSetup(BoxLowChtDraw* pBoxChtLowDraw)
{
  mpBoxChtLowDrawBk = pBoxChtLowDraw;

}


enum{
  POKECHK_NG_,
  POKECHK_PEACE_,
  POKECHK_OK_,
};


enum{
  MM_SEQ_TRADE_ST,
  MM_SEQ_TRADE_LOOP,
  MM_SEQ_TRADE_WAIT,
  MM_SEQ_TRADE_WAIT1,
  MM_SEQ_TRADE_WAIT2,
  MM_SEQ_TRADE_WAIT3,
  MM_SEQ_TRADE_WAIT4,
  MM_SEQ_TRADE_WAIT5,
  MM_SEQ_TRADE_WAIT6,
  MM_SEQ_TRADE_WAIT7,
  MM_SEQ_CLOSE_MESSAGE_LOOP,
  MM_SEQ_CLOSE_MESSAGE,
  MM_SEQ_GTSTRADECHECK,
  MM_SEQ_MSG_PEACE,
  MM_SEQ_MSG_PEACE_LOOP,

  MM_SEQ_WARRNING_MSG_AND_CHAT_RESET,
  MM_SEQ_WARRNING_MSG,
  MM_SEQ_WARRNING_MSG_LOOP,
  MM_SEQ_CLOSE,

  MM_ERROR_MSG,
  MM_ERROR_MSG_LOOP,
  MM_ERROR_MSG_CLOSE,

  MM_SEQ_YESNO,
  MM_SEQ_YESNOLOOP,


  MM_SEQ_END_LOOP,
  MM_SEQ_END_TIMER,
  MM_SEQ_APP_JUMP,

  MM_SEQ_CHAT_START,
  MM_SEQ_CHAT_LOOP,
  MM_SEQ_CHAT_END,

};



//通信Aボタン選択開始
void BoxP2pTradeState::ModeStart(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;

  SetUpdateStartSeq(MM_SEQ_TRADE_ST);
}

//チャット開始
void BoxP2pTradeState::ChatStart(void)
{
  SetUpdateStartSeq( MM_SEQ_CHAT_START );
}




u32 BoxP2pTradeState::StateFunc(void)
{
#if PM_DEBUG
  if(mDebugSeqBackup !=  GetSubSeq()){
    OHNO_PRINT("BoxP2pTradeState %d\n",GetSubSeq());
    mDebugSeqBackup = GetSubSeq();
  }
#endif
  


  switch(GetSubSeq()){
  case MM_SEQ_TRADE_ST:
    pokeTradeStartMenu();
    break;
  case MM_SEQ_TRADE_LOOP:
    break;
  case MM_SEQ_TRADE_WAIT:
    switch(pokeTradeCheck())
    {
    case POKECHK_OK_:
      {
        mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_11 - msg_chat_01_01 );
        SetSubSeq(MM_SEQ_TRADE_WAIT1);
      }
      break;
    case POKECHK_PEACE_:
      SetSubSeq(MM_SEQ_MSG_PEACE);
      break;
    case POKECHK_NG_:
      mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
      break;
    }
    break;
  case MM_SEQ_TRADE_WAIT1:

    if( mpAppParam->type != BOX_CALL_FIELD_TRADE)
    {  //フィールド交換でない
      mpBoxBgLowDraw->DrawMessageLine2Upper(msg_box_time_01);  //通信待機中しばらくお待ち下さい。 ->相手の　返事を　待っています
      mpBoxBgLowDraw->SetTimerIcon(true);
    }
    {
      pml::pokepara::PokemonParam * pp = GFL_NEW(mpAppHeap->GetDeviceHeap()->GetLowerHandle())
        pml::pokepara::PokemonParam(mpAppHeap->GetDeviceHeap()->GetLowerHandle());
      mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, pp);
      if(mpAppParam->pP2PTradeRequestClient){
        if(mSelectCursor < Savedata::BoxPokemon::TRAY_POKE_MAX){
          mpAppParam->pP2PTradeRequestClient->AddFirstTradeRequest(*pp, mSelectTray, mSelectCursor);
        }
        else{
          mpAppParam->pP2PTradeRequestClient->AddFirstTradeRequest(*pp, App::Box::RETURN_TEMOTI, mSelectCursor-Savedata::BoxPokemon::TRAY_POKE_MAX);
        }
      }
      GFL_SAFE_DELETE(pp);
    }
    SetSubSeq(MM_SEQ_TRADE_WAIT2);

    mpBoxBgLowDraw->SetUIListener(&mDInnerMessageTradeFirstListener);
    mDInnerMessageTradeFirstListener.SetThis(this);

    mpBoxBgLowDraw->PassiveChatChangeButton();
    mpBoxBgLowDraw->ResetCancelButton();

    break;
  case MM_SEQ_TRADE_WAIT2:
    //Bぼたんでキャンセル

    if( mpAppParam->type == BOX_CALL_FIELD_TRADE){  //フィールド交換
      // ゲーム内交換
      OnTradeComplete();
    }
    else if(mpAppParam->mbNetEndRequest){
      OHNO_PRINT("ここをとおった x \n");
      mpAppParam->mbNetEndRequest=false;
      mpBoxBgLowDraw->SetTimerIcon(false);
      simpleWarrningMessage(msg_box_p2ptrade_05, 2, false);
      SetSubSeq(MM_ERROR_MSG);
    }

    break;
  case MM_SEQ_TRADE_WAIT3:

    mpBoxBgLowDraw->DrawMessageLine2(msg_box_04_01);  //こうかんをおこないますか？。
    mpBoxBgLowDraw->SetTimerIcon(false);
    OHNO_PRINT("MM_SEQ_TRADE_WAIT3\n");

    //最終確認用メニュー
    mInnerP2PTradeListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerP2PTradeListener );
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_03_01);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_chat_list_01);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_chat_list_02);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_chat_list_03);
    mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_2LINE_POS);
    
    mpBoxBgLowDraw->PassiveCancelButton();
    
    SetSubSeq(MM_SEQ_TRADE_WAIT4);
    break;
  case MM_SEQ_TRADE_WAIT4:

    break;
  case MM_SEQ_TRADE_WAIT5:
    //Bぼたんでキャンセル
    break;
  case MM_SEQ_TRADE_WAIT6:
    ///キー待ち
    break;
  case MM_SEQ_GTSTRADECHECK:
    {
      switch(pokeTradeCheck()){
      case POKECHK_OK_:
      case POKECHK_PEACE_:
        mpBoxBgLowDraw->MessageLine2Close();
        mpBoxBgLowDraw->DrawMessageLine2(msg_box_04_01);  //はい・いいえに移動
        SetSubSeq( MM_SEQ_YESNO );
        break;
      }
    }
    break;
  case MM_SEQ_CLOSE_MESSAGE:
  case MM_SEQ_TRADE_WAIT7:
    mpBoxBgLowDraw->MessageLine2Close();
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
    return BOXSTATE_RETURN;  //仕事完了

  case MM_SEQ_MSG_PEACE:
    mpBoxBgLowDraw->MessageLine2Close();
    mpMenuWindow->EndMenu();//GFNMCat[2116]
    ItemPeaceCheckMenu();
    break;
  case MM_SEQ_MSG_PEACE_LOOP:
    break;

  case MM_SEQ_WARRNING_MSG_AND_CHAT_RESET:
    mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_13-msg_chat_01_01 );  //chatメッセージをもどす
    //break throught
  case MM_SEQ_WARRNING_MSG:
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->WarrningMessageBefore(&mDInnerNoneListener); //GFNMCat[1746]
		{
	    BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
	    pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
	    pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
		}
    mpBoxBgLowDraw->WarrningMessageAfter(mpAppParam->nowDispTray,  mSelectTray, mpAppParam->nowBattleTray); //GFNMCat[1609]
    SetSubSeq(MM_SEQ_WARRNING_MSG_LOOP);
    break;
  case MM_SEQ_WARRNING_MSG_LOOP:
    //上をチャットにもどす
    if(BOX_CALL_TRADE == mpAppParam->type){
      mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_CHAT_FORCE ,0);
    }
    return BOXSTATE_RETURN;  //仕事完了
  case MM_ERROR_MSG://エラーのときはBOX自体終了する
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->WarrningMessageBefore(&mDInnerNoneListener); //GFNMCat[1746]
		{
	    BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
	    pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
	    pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
		}
    mpBoxBgLowDraw->WarrningMessageAfter(mpAppParam->nowDispTray,  mSelectTray, mpAppParam->nowBattleTray); //GFNMCat[1609]
    SetSubSeq(MM_ERROR_MSG_LOOP);
    break;
  case MM_ERROR_MSG_LOOP:
    mpAppParam->outJumpMode = App::Box::BOXJUMP_CANCEL;
    return BOXSTATE_APP_JUMP;
    
  case MM_SEQ_CLOSE:
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
    mpBoxBgLowDraw->ResetChatChangeButton();
    return BOXSTATE_RETURN;  //仕事完了

  case MM_SEQ_APP_JUMP:
    return BOXSTATE_APP_JUMP;
  case MM_SEQ_END_TIMER:
    OnTradeExitSuccess();
    break;
  case MM_SEQ_YESNO:
    SetSubSeq( MM_SEQ_YESNOLOOP );
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_yes);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_no,true);
    mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_2LINE_POS);
    mInnerYesNoListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerYesNoListener );
    break;
  case MM_SEQ_YESNOLOOP:
    break;

  case MM_SEQ_CHAT_START:
    mpBoxChtLowDraw = mpBoxChtLowDrawBk;
   // mpBoxLowBgFrame->mpBoxChtLowDraw = mpBoxChtLowDrawBk;
    mpBoxChtLowDraw->SetUIListener( &mBoxChtLowDrawListener);
    mBoxChtLowDrawListener.SetThis(this);
    mpBoxBgLowDraw->SetUIListener(NULL);
    Sound::PlaySE(SEQ_SE_DECIDE1);  //GFNMCat1507
    SetSubSeq( MM_SEQ_CHAT_LOOP );
    break;
  case MM_SEQ_CHAT_LOOP:
    break;
  case MM_SEQ_CHAT_END:
    return BOXSTATE_RETURN;  //仕事完了

   
  default:
    break;
  }

  if(mpBoxChtLowDraw)
  {
    mpBoxChtLowDraw->Update();
  }
  else
  {
    mpBoxBgLowDraw->Update();
  }


  return BOXSTATE_NONE;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BoxP2pTradeState::InitFunc(void)
{
  Setup();
  return applib::frame::RES_FINISH;
}
//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BoxP2pTradeState::UpdateFunc(void)
{
  int seq = UpdateStart();
  if(MENU_SEQ_INIT!=seq){
    SetSubSeq(seq);
  }
  mRetCode = StateFunc();

  switch(mRetCode){
  case BoxStateBasis::BOXSTATE_NONE:   //シーケンスはそのまま
    break;
  default:
    return applib::frame::RES_FINISH;
  }
  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void BoxP2pTradeState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  if(mpBoxChtLowDraw)
  {
    mpBoxChtLowDraw->DrawTree(no);
  }
  else{
    mpBoxBgLowDraw->DrawTree(no);
  }

}
//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result BoxP2pTradeState::EndFunc(void)
{
  End();
  Release();
  LiftEndFunc();
  return applib::frame::RES_FINISH;
}



void BoxP2pTradeState::LiftEndFunc(void)
{
  mpMenuWindow->EndMenu();
  mpBoxBgLowDraw->LiftPokeIconReadyReturn(mSelectTray, mSelectCursor);
  mpBoxBgLowDraw->ResetChatChangeButton();
  mpBoxBgLowDraw->LiftPokeIconClear();
  mpBoxBgLowDraw->MessageLine2Close();
}




#if PM_DEBUG
void  BoxP2pTradeState::DInnerNoneListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerMessageTradeFirstListener\n");
}

#endif



//チャット結果を受け取るインナークラス
//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void BoxP2pTradeState::InnerBoxLowChtDrawListener::OnButtonAction(int button_no)
{
  switch(button_no){
  case LOWCHT_ACTION_CANCEL:
    Sound::PlaySE(SEQ_SE_CANCEL1);//GFNMCat1507  -->4964
    break;
  default:
    mpThis->mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_CHAT_FORCE ,0);
    mpThis->mpBoxLowBgFrame->NetworkSendMessage(button_no);
    break;
  }
  mpThis->ChatEndReq();


}

//--------------------------------------------------------------------------
/**
 * @brief   キー操作通知
 * @param		dir  キー方向
 * @retval  true = 操作実行
 * @retval  false = 操作失敗
 */
//--------------------------------------------------------------------------
bool BoxP2pTradeState::InnerBoxLowChtDrawListener::OnKeyAction( u32 dir )
{
  return false;
}


//はいいいえを受け取るインナークラス
void BoxP2pTradeState::InnerYesNoListener::SelectMenu( u32 id )
{
  if(id==0){  //はい選択
    if( mpThis->mpAppParam->type == BOX_CALL_TRADE){
      GFL_ASSERT(0);
    }
    
    if( item::ITEM_CheckPiece(mpThis->mItemno) )
    {
      mpThis->SetSubSeq(MM_SEQ_MSG_PEACE);
    }
    else
    {
      mpThis->SetSubSeq(MM_SEQ_APP_JUMP);
      mpThis->mpAppParam->outJumpMode = App::Box::BOXJUMP_OK;
      mpThis->mpAppParam->outTray = (u8)mpThis->mSelectTray;
      mpThis->mpAppParam->outPos = (u8)mpThis->mSelectCursor;
    }
  }
  else{  //いいえは元に戻す
    mpThis->mpMenuWindow->EndMenu();
    mpThis->mpBoxBgLowDraw->MessageLine2Close();
    mpThis->SetSubSeq(MM_SEQ_CLOSE);
    mpThis->mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰

  }

}




//ピースについての確認を受け取るインナークラス
void BoxP2pTradeState::InnerPeaceListener::SelectMenu( u32 id )
{
  if(id==0){
    if( mpThis->mpAppParam->type == BOX_CALL_TRADE){  //通信交換
      mpThis->mpMenuWindow->EndMenu();
      mpThis->mpBoxBgLowDraw->MessageLine2Close();
      mpThis->mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_11 - msg_chat_01_01 );
      mpThis->SetSubSeq(MM_SEQ_TRADE_WAIT1);
    }
    else{   //その他の交換
//      mpThis->mpBoxBgLowDraw->DrawMessageLine2(msg_box_04_01);  //はい・いいえに移動
  ///    mpThis->SetSubSeq( MM_SEQ_YESNO );

      {
        mpThis->SetSubSeq(MM_SEQ_APP_JUMP);
        mpThis->mpAppParam->outJumpMode = App::Box::BOXJUMP_OK;
        mpThis->mpAppParam->outTray = (u8)mpThis->mSelectTray;
        mpThis->mpAppParam->outPos = (u8)mpThis->mSelectCursor;
      }

    }
  }
  else{  //いいえは元に戻す
    mpThis->mpMenuWindow->EndMenu();
    mpThis->mpBoxBgLowDraw->MessageLine2Close();
    mpThis->SetSubSeq(MM_SEQ_CLOSE);
    mpThis->mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
  }
}





//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー 普通のBOX選択
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxP2pTradeState::InnerP2PListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //こうかん
    mpThis->SetSubSeq(MM_SEQ_TRADE_WAIT);
    break;
  case 1:    //強さをみる
    mpThis->SetStatusJump();
    mpThis->mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_10-msg_chat_01_01 );
    break;
  default:
    GFL_ASSERT(0);
  case 2:  //やめる
    mpThis->mpBoxBgLowDraw->LiftPokeIconReadyReturn(mpThis->mSelectTray, mpThis->mSelectCursor);
    mpThis->SetSubSeq( MM_SEQ_CLOSE );
    break;
  }
  mpThis->mpBoxBgLowDraw->MessageLine2Close();
  mpThis->mpMenuWindow->EndMenu();
}


//-----------------------------------------------------------------------------
/**
 * @func    InnerP2PTradeListener
 * @brief   交換の最終確認
 * @date    2015.10.17
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxP2pTradeState::InnerP2PTradeListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //交換する
    mpThis->TradeFinalDecide();
    break;
  case 1:    //強さをみる
    mpThis->SetNetworkPokemonStatus(1);
    break;
  case 2:
    mpThis->SetNetworkPokemonStatus(2);
    break;
  default:
    GFL_ASSERT(0);
  case 3:  //やめる
    mpThis->mpBoxBgLowDraw->LiftPokeIconReadyReturn(mpThis->mSelectTray, mpThis->mSelectCursor);
    mpThis->mpMenuWindow->EndMenu();
    mpThis->mpAppParam->pP2PTradeRequestClient->AddSendTradeCancelRequest();

    if(BOX_CALL_TRADE == mpThis->mpAppParam->type){
      mpThis->mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_CHAT_FORCE ,0);
    }
    mpThis->mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_13-msg_chat_01_01 );  //chatメッセージをもどす

    
    mpThis->mpBoxBgLowDraw->ResetChatChangeButton();
    mpThis->SetSubSeq( MM_SEQ_TRADE_WAIT7 );

    break;
  }
}



//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー 普通のBOX選択
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxP2pTradeState::InnerGTSListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //選択
    mpThis->SetSubSeq( MM_SEQ_GTSTRADECHECK );
    break;
  case 1:    //強さをみる
    mpThis->SetStatusJump();
    break;
  default:
    GFL_ASSERT(0);
  case 2:  //やめる
//    mpThis->mpBoxBgLowDraw->LiftPokeIconReadyReturn(mpThis->mSelectTray, mpThis->mSelectCursor);
    mpThis->SetSubSeq( MM_SEQ_CLOSE );
    mpThis->mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
    break;
  }

  mpThis->mpMenuWindow->EndMenu();
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー 普通のBOX選択
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxP2pTradeState::InnerMiracleListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //選択
    mpThis->SetSubSeq( MM_SEQ_GTSTRADECHECK );
    break;
  case 1:    //強さをみる
    mpThis->SetStatusJump();
    break;
  default:
    GFL_ASSERT(0);
  case 2:  //やめる
    OHNO_PRINT("BoxP2pTradeState::InnerMiracleListener::SelectMenu\n");
    mpThis->mpBoxBgLowDraw->LiftPokeIconReadyReturn(mpThis->mSelectTray, mpThis->mSelectCursor);
    mpThis->SetSubSeq( MM_SEQ_CLOSE );
    break;
  }

  mpThis->mpMenuWindow->EndMenu();
}


//しばらくお待ち下さいメッセージ表示中 1回目の方
#if PM_DEBUG
void  BoxP2pTradeState::DInnerMessageTradeFirstListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerMessageTradeFirstListener\n");
}
#endif

bool BoxP2pTradeState::DInnerMessageTradeFirstListener::OnTouchFirstHook(bool isTouch)
{
  return true;
}

bool BoxP2pTradeState::DInnerMessageTradeFirstListener::OnKeyAction( u32 dir )
{
  return true;
}

void BoxP2pTradeState::DInnerMessageTradeFirstListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
    mpThis->mpAppParam->pP2PTradeRequestClient->CancelFirstTradeRequest();
    //    OHNO_PRINT("送信のみ\n");
//    mpThis->mpBoxBgLowDraw->LiftPokeIconReadyReturn(mpThis->mSelectTray, mpThis->mSelectCursor);  //GFNMCat[1672]

//    mpThis->mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_13-msg_chat_01_01 );  //chatメッセージをもどす//1672派生
    break;
  }
}
//しばらくお待ち下さいメッセージ表示中 ２回目の方
#if PM_DEBUG
void  BoxP2pTradeState::DInnerMessageTradeFinalListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerMessageTradeFinalListener\n");
}
#endif
bool BoxP2pTradeState::DInnerMessageTradeFinalListener::OnTouchFirstHook(bool isTouch)
{
  return true;
}
bool BoxP2pTradeState::DInnerMessageTradeFinalListener::OnKeyAction( u32 dir )
{
  return true;
}

void BoxP2pTradeState::DInnerMessageTradeFinalListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
    mpThis->mpAppParam->pP2PTradeRequestClient->CancelFinalTradeRequest();
    //OHNO_PRINT("送信のみ\n");
    mpThis->mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_13-msg_chat_01_01 );  //chatメッセージをもどす

    break;
  }
}


void BoxP2pTradeState::pokeTradeStartMenu(void)
{
  int retchk = mpBoxPokemonMoveFunction->FirstTradeCheck(mSelectTray,mSelectCursor);
  switch(retchk){
	case BoxPokemonMoveFunction::MOVEERR_EGG_PULL:
    simpleWarrningMessage(msg_box_05_03, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
	case BoxPokemonMoveFunction::MOVEERR_NO_CONDITION:
    simpleWarrningMessage(msg_box_05_01, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  case BoxPokemonMoveFunction::MOVEERR_COMPATIBLE_ERROR:
    simpleWarrningMessage(msg_box_05_17, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  }

  switch( mpAppParam->type ){
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択
    break;
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_03_01);  //こうかんする
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_02);  //つよさをみる
    mInnerP2PListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerP2PListener );
    mpBoxBgLowDraw->PassiveChatChangeButton();
    break;
  case BOX_CALL_GTS:      //GTS
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_03_02);  //あずける
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_02);
    mInnerGTSListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerGTSListener );
    break;
  case BOX_CALL_MIRACLE:   //ミラクル交換
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_03_01);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_02);
    mInnerMiracleListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerMiracleListener );
    break;
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    break;
  }
  mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_end,true,true);
  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);  //fix
  Sound::PlaySE(SEQ_SE_DECIDE1);//GFNMCat[1489]
  mpBoxBgLowDraw->LiftPokeIconReady(mSelectTray, mSelectCursor);
  
  mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, mpCoreTemp);
  mpBoxBgLowDraw->ExpandPokemon( 0,mpCoreTemp );
  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_02);
	SetSubSeq(MM_SEQ_TRADE_LOOP);


}



//交換に出していいかどうか
int BoxP2pTradeState::pokeTradeCheck(void)
{
  int tray = mSelectTray;
  int pos =  mSelectCursor;
  u16 itemno;
  
  mpBoxPokemonMoveFunction->GetPokemon(tray,pos, mpCoreTemp);
  itemno = mpCoreTemp->GetItem();

  switch( mpAppParam->type ){
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
    if(mpBoxPokeParaFunc->CheckEventHaifuPokemon(mpCoreTemp)){  //リボン
      simpleWarrningMessage(msg_box_05_05, 2, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      return POKECHK_NG_;
    }
    if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){   //たまご
      simpleWarrningMessage(msg_box_05_03, 2, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      return POKECHK_NG_;
    }
    if( mpBoxPokeParaFunc->CheckItem(mpCoreTemp->GetItem()) ){  //メガストーン
      simpleWarrningMessage(msg_box_05_06, 2, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      return POKECHK_NG_;
    }
  case BOX_CALL_TRADE:    //P2P交換
    if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_ONLY_ILLEGAL_EGG)){   //不正たまご
      simpleWarrningMessage(msg_box_05_03, 2, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      return POKECHK_NG_;
    }
    if(mpBoxPokeParaFunc->CheckSpecialPokemon(mpCoreTemp)){  //合体
      simpleWarrningMessage(msg_box_05_05, 2, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      return POKECHK_NG_;
    }
    if(pos < Savedata::BoxPokemon::TRAY_POKE_MAX)
    {  //box
      int teamno = mpAppParam->pBoxData->IsTeamInMe(tray,pos);
      if(teamno!=-1)
      {
        // if( mpAppParam->pBoxData->IsTeamLock(teamno) )//ロック中のチーム
        {
          simpleWarrningMessage(msg_box_05_04, 2, true);
          SetSubSeq(MM_SEQ_WARRNING_MSG);
          return POKECHK_NG_;
        }
      }
    }
    else{  //手持ちの時
      mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_ThisToParty(mpPartyTemp);
      //      mpPartyTemp->CopyFrom( *mpAppParam->pTemochi );
      mpPartyTemp->RemoveMember(pos - Savedata::BoxPokemon::TRAY_POKE_MAX);  //入れる前に消す
      if(!mpBoxPokeParaFunc->CheckBattlePokemon(mpPartyTemp) ){  //戦うポケモン残るかどうか
        simpleWarrningMessage(msg_box_05_02, 2, true);
        SetSubSeq(MM_SEQ_WARRNING_MSG);
        return POKECHK_NG_;
      }
    }
    break;
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    break;
  }
  
  if( item::ITEM_CheckPiece(itemno) )
  {
    mItemno = itemno;
    return POKECHK_PEACE_;
  }
  return POKECHK_OK_;
}


void BoxP2pTradeState::OnFirstTradeSuccess(const pml::pokepara::PokemonParam& pokemonParam)
{
  GFL_ASSERT(GetSubSeq()==MM_SEQ_TRADE_WAIT2);
  SetSubSeq(MM_SEQ_TRADE_WAIT3);
  OHNO_PRINT("OnFirstTradeSuccess MM_SEQ_TRADE_WAIT3 \n" );
}


void BoxP2pTradeState::OnTradeSuccess(void)
{
  mpAppParam->outJumpMode = App::Box::BOXJUMP_OK;
  mpAppParam->outTray = (u8)mSelectTray;
  mpAppParam->outPos = (u8)mSelectCursor;
  SetSubSeq(MM_SEQ_APP_JUMP);

}


void BoxP2pTradeState::OnTradeExitSuccess(void)
{
  OHNO_PRINT("ここをとおった OnTradeExitSuccess \n");
  mpAppParam->outJumpMode = App::Box::BOXJUMP_CANCEL;
  SetSubSeq(MM_SEQ_APP_JUMP);
}

void BoxP2pTradeState::OnTradeComplete(void)
{

  mpAppParam->outJumpMode = App::Box::BOXJUMP_OK;
  mpAppParam->outTray = (u8)mSelectTray;
  mpAppParam->outPos = (u8)mSelectCursor;
  SetSubSeq(MM_SEQ_APP_JUMP);

}


void BoxP2pTradeState::OnTradeCancelReceived(void)
{
  mpMenuWindow->EndMenu();
  //mpBoxBgLowDraw->SetUIListener(&mDInnerMessageDispListener);
  //mDInnerMessageDispListener.SetThis(this);
  //  mpBoxBgLowDraw->DrawMessageLine2(msg_box_p2ptrade_01);  //あいてが交換を中止しました
  //あいてが交換を中止しました
  mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_13-msg_chat_01_01 );  //chatメッセージをもどす//1672派生

  simpleWarrningMessage(msg_box_p2ptrade_01,2, true);
  SetSubSeq(MM_SEQ_WARRNING_MSG);
  mpBoxBgLowDraw->PassiveCancelButton();

  mpBoxBgLowDraw->SetTimerIcon(false);
//  mpBoxBgLowDraw->ResetChatChangeButton();

  // SetSubSeq(MM_SEQ_TRADE_WAIT6);
}


void BoxP2pTradeState::OnFinalTradeCancel(void)
{

  mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_13-msg_chat_01_01 );  //chatメッセージをもどす//1672派生

  mpMenuWindow->EndMenu();
  //交換を中止しました
  simpleWarrningMessage(msg_box_p2ptrade_02,1, true);//1行に
  mpBoxBgLowDraw->PassiveCancelButton();
      SetSubSeq(MM_SEQ_WARRNING_MSG);

  mpBoxBgLowDraw->SetTimerIcon(false);
//  mpBoxBgLowDraw->ResetChatChangeButton();

}

//交換２回目の決定時の処理
void BoxP2pTradeState::TradeFinalDecide(void)
{
  mpBoxLowBgFrame->RequestChangePoke();
  if(BOX_CALL_TRADE == mpAppParam->type){
    mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_CHAT_FORCE ,0);
  }

  mpBoxBgLowDraw->DrawMessageLine2Upper(msg_box_time_01);  //あいての返事を待っています
  mpBoxBgLowDraw->SetTimerIcon(true);

  SetSubSeq( MM_SEQ_TRADE_WAIT5 );
  mpMenuWindow->EndMenu();
  mpAppParam->pP2PTradeRequestClient->AddFinalTradeRequest();
  mpBoxBgLowDraw->SetUIListener(&mDInnerMessageTradeFinalListener);
  mDInnerMessageTradeFinalListener.SetThis(this);
  mpBoxBgLowDraw->ResetCancelButton();

}


void BoxP2pTradeState::OnFirstTradePokemonInvalid(bool bMyPokemonInvalid)
{
  if( bMyPokemonInvalid ){
    simpleWarrningMessage(msg_box_p2ptrade_08, 2, true);
  }
  else{
    simpleWarrningMessage(msg_box_p2ptrade_03, 2, true);
  }

  SetSubSeq( MM_SEQ_WARRNING_MSG_AND_CHAT_RESET );
  mpBoxBgLowDraw->SetTimerIcon(false);
}


void BoxP2pTradeState::OnFirstTradeValidationError(bool bMyValidationError)
{

  if( bMyValidationError ){
    simpleWarrningMessage(msg_box_p2ptrade_09, 2, true);

  }
  else{
    simpleWarrningMessage(msg_box_p2ptrade_09, 2, true);
  }
  SetSubSeq( MM_SEQ_WARRNING_MSG_AND_CHAT_RESET );
  mpBoxBgLowDraw->SetTimerIcon(false);

  
}



void BoxP2pTradeState::ChatEndReq(void)
{
  mpBoxChtLowDraw=NULL;
//  mpBoxLowBgFrame->mpBoxChtLowDraw =NULL;
  SetSubSeq( MM_SEQ_CHAT_END );
}

void BoxP2pTradeState::SetStatusJump( void )
{
  mpAppParam->outJumpMode = App::Box::BOXJUMP_STATUS;
  mpAppParam->outPos = (u8)mSelectCursor;
  mpAppParam->outTray = (u8)mSelectTray;

  /*
	if(mSelectCursor >= Savedata::BoxPokemon::TRAY_POKE_MAX){
		if( App::Box::BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode() ){
			mpAppParam->outTray = (u8)mpAppParam->nowBattleTray;
		}
		else if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM){
			mpAppParam->outTray = (u8)mpAppParam->nowBattleTray;
		}
	}*/
  SetSubSeq(MM_SEQ_APP_JUMP);
}

void BoxP2pTradeState::SetNetworkPokemonStatus(int statusType)
{
  mpBoxLowBgFrame->RequestNetChangePoke( statusType );
}



void BoxP2pTradeState::ItemPeaceCheckMenu(void)
{
  mpBoxBgLowDraw->ExpandItem( 0, mItemno );
  mpBoxBgLowDraw->DrawMessageLine2(msg_box_p2ptrade_07);
  mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_yes);
  mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_no,true);
  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_2LINE_POS);
  mpMenuWindow->SetListener( &mInnerPeaceListener );
  SetSubSeq(MM_SEQ_MSG_PEACE_LOOP);
}



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




