//=============================================================================
/**
 * @file	 BoxExitState.cpp
 * @brief	 終了の状態移動
 * @author k.ohno
 * @date	 2016.2.28
 */
//=============================================================================
#pragma once


#include "../BoxPokepara.h"
#include "../BoxSimpleAccesser.h"
#include "../BoxLowBgFrame.h"
#include "../BoxLowBgDraw.h"

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

#include "niji_conv_header/message/msg_box.h"

#include "BoxExitState.h"
#include "BoxWarrningMessageState.h"

#include "../function/BoxPokemonMoveFunction.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"



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
BoxExitState::BoxExitState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxExitState::~BoxExitState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxExitState::Setup(void)
{
}


void BoxExitState::Release(void)
{
}


enum{
	MM_SEQ_EXIT_INIT,
  MM_SEQ_EXIT_YESNOLOOP,
  MM_SEQ_WARRNING_MSG,
  MM_SEQ_WARRNING_MSG_LOOP,
  MM_SEQ_CLOSE,
	MM_SEQ_NET_FINAL_WAIT,
  MM_SEQ_APP_JUMP,
  MM_SEQ_CHANGE_EXIT_STATE,

};

void BoxExitState::ModeStart(void)
{
  //選んだ場所
  SetUpdateStartSeq(MM_SEQ_EXIT_INIT);
}


void BoxExitState::EndReq(void)
{
  SetUpdateStartSeq(MM_SEQ_NET_FINAL_WAIT);
}



u32 BoxExitState::StateFunc(void)
{

  if(mpAppParam->mbNetEndSuccess){
    SetSubSeq(MM_SEQ_APP_JUMP);
  }

  
  switch(GetSubSeq()){
  case MM_SEQ_EXIT_INIT:
    ExitFuncStart();
    break;

  case MM_SEQ_EXIT_YESNOLOOP:
    break;

  case MM_SEQ_NET_FINAL_WAIT:

    if(mpAppParam->mbNetEndRequest){// あいてからも終了リクエストがきていた
      mpAppParam->mbNetEndRequest=false;
      mpBoxBgLowDraw->SetTimerIcon(false);
      simpleWarrningMessage(msg_box_p2ptrade_05, 2, false);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
    }
    break;

  case MM_SEQ_WARRNING_MSG:
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->WarrningMessageBefore(&mDInnerNoneListener); //GFNMCat[1746]
		{
	    BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
	    pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
	    pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
		}
    mpBoxBgLowDraw->WarrningMessageAfter(mpAppParam->nowDispTray,  mpAppParam->nowDispTray, mpAppParam->nowBattleTray); //GFNMCat[1609]
    SetSubSeq(MM_SEQ_WARRNING_MSG_LOOP);
    break;
  case MM_SEQ_WARRNING_MSG_LOOP:
    return BOXSTATE_EXIT_STATE; ///戻ってOnTradeExitSuccessを呼ぶ
    
  case MM_SEQ_CLOSE:
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->ResetChatChangeButton();
    mpBoxBgLowDraw->LiftPokeIconClear();
    mpBoxBgLowDraw->ResetFunc();
    return BOXSTATE_RETURN;   //終了しない

  case MM_SEQ_APP_JUMP:
    return BOXSTATE_APP_JUMP;
  default:
    break;
  }
  mpBoxBgLowDraw->Update();

  return BOXSTATE_NONE;
}


//------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
 applib::frame::Result BoxExitState::InitFunc(void)
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
 applib::frame::Result BoxExitState::UpdateFunc(void)
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
 void BoxExitState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  {
    mpBoxBgLowDraw->DrawTree(no);
  }

}
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
 applib::frame::Result BoxExitState::EndFunc(void)
{
  End();
  Release();
  return applib::frame::RES_FINISH;
}




//終了時のはいいいえを受け取るインナークラス
void BoxExitState::InnerExitYesNoListener::SelectMenu( u32 id )
{
  if(id==0){  //はい選択
    if(mpThis->mpAppParam->pP2PTradeRequestClient){ // 通信ならば
      mpThis->mpAppParam->pP2PTradeRequestClient->AddTradeExitRequest();
      mpThis->mpBoxBgLowDraw->DrawMessageLine2(msg_box_p2ptrade_06);  //つうしんを　しょうりょう　していますしばらく　おまちくださいSetMessage
      mpThis->mpBoxBgLowDraw->SetTimerIcon(true);
      mpThis->SetSubSeq(MM_SEQ_NET_FINAL_WAIT);
    }
    else{
      mpThis->mpAppParam->outJumpMode = App::Box::BOXJUMP_CANCEL;
      mpThis->SetSubSeq(MM_SEQ_APP_JUMP);  //終了する
    }
  }
  else{  //いいえは元に戻す
    mpThis->mpBoxBgLowDraw->MessageLine2Close();
    mpThis->mpBoxBgLowDraw->AllPassiveEnd();
    mpThis->SetSubSeq(MM_SEQ_CLOSE);
  }
  mpThis->mpMenuWindow->EndMenu();
}







///------------------------------------------------------------------------------

void BoxExitState::ExitFuncStart(void)
{

  //      msg_box_02_01  //そうさをやめますか
  //      msg_box_02_23//ぽけりぞーとにもどりますか
  //msg_box_p2ptrade_04  ポケモンこうかんをしゅうりょうしますか？
  int pos=MENU_2LINE_POS;

  switch( mpAppParam->type ){
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
  case BOX_CALL_TRADE:    //P2P交換
    mpBoxBgLowDraw->DrawMessageLine2(msg_box_p2ptrade_04);  //はい・いいえに移動
    pos = MENU_2LINE_POS;
    break;
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
    mpAppParam->outJumpMode = App::Box::BOXJUMP_CANCEL;
    mpMenuWindow->EndMenu();
    SetSubSeq(MM_SEQ_APP_JUMP);
    return;

  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択
    mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_01);  //はい・いいえに移動
    pos = MENU_1LINE_POS;
    break;
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_23);  //はい・いいえに移動
    pos = MENU_1LINE_POS;
    break;
  }

  SetSubSeq( MM_SEQ_EXIT_YESNOLOOP );
  mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_yes);
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_no, true);
  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, pos);

  mpBoxBgLowDraw->AllPassiveStart();

  mInnerExitYesNoListener.SetThis(this);
  mpMenuWindow->SetListener( &mInnerExitYesNoListener );


}

#if PM_DEBUG
void  BoxExitState::DInnerNoneListener::DebugDisp(void)
{
  OHNO_PRINT("BoxExitState::DInnerNoneListener\n");
}

#endif



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




