//=============================================================================
/**
 * @file	 BoxTeamNameState.cpp
 * @brief	 チームネーム付けの遷移
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

#include "BoxTeamNameState.h"
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
BoxTeamNameState::BoxTeamNameState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxTeamNameState::~BoxTeamNameState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxTeamNameState::Setup(void)
{
}


void BoxTeamNameState::Release(void)
{
}


enum{
	MM_SEQ_TEAMNAME_INIT,
  MM_SEQ_TEAMNAME_YESNOLOOP,
  MM_SEQ_CLOSE,
  MM_SEQ_APP_JUMP,

};

void BoxTeamNameState::ModeStart(int tray,int cursor)
{
  mSelectTray=tray;
	mSelectCursor=cursor;

  //選んだ場所
  SetUpdateStartSeq(MM_SEQ_TEAMNAME_INIT);
}


u32 BoxTeamNameState::StateFunc(void)
{
  switch(GetSubSeq()){
  case MM_SEQ_TEAMNAME_INIT:
    TeamNameFuncStart();
    break;
  case MM_SEQ_TEAMNAME_YESNOLOOP:
    break;

    
  case MM_SEQ_CLOSE:
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->ResetChatChangeButton();
    mpBoxBgLowDraw->LiftPokeIconClear();
    mpBoxBgLowDraw->ResetFunc();
    return BOXSTATE_RETURN;   //終了

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
 applib::frame::Result BoxTeamNameState::InitFunc(void)
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
 applib::frame::Result BoxTeamNameState::UpdateFunc(void)
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
 void BoxTeamNameState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
 applib::frame::Result BoxTeamNameState::EndFunc(void)
{
  End();
  Release();
  return applib::frame::RES_FINISH;
}




//チームネームつけるかのはいいいえを受け取るインナークラス
void BoxTeamNameState::InnerTeamNameYesNoListener::SelectMenu( u32 id )
{
  if(id==0){  //はい選択
    mpThis->mpAppParam->outJumpMode = App::Box::BOXJUMP_TEAM_NAME;
    mpThis->mpAppParam->outTray = (u8)mpThis->mSelectTray;
    mpThis->mpAppParam->outPos = (u8)mpThis->mSelectCursor;
		mpThis->SetSubSeq(MM_SEQ_APP_JUMP);
    mpThis->mpMenuWindow->EndMenu();

  }
  else{  //いいえは元に戻す
    mpThis->mpMenuWindow->EndMenu();
    mpThis->mpBoxBgLowDraw->MessageLine2Close();
    mpThis->SetSubSeq(MM_SEQ_CLOSE);
  }
}





///------------------------------------------------------------------------------

void BoxTeamNameState::TeamNameFuncStart(void)
{

  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_yes);
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_no, true);
  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);
  Sound::PlaySE(SEQ_SE_DECIDE1);
  mInnerTeamNameYesNoListener.SetThis(this);
  mpMenuWindow->SetListener( &mInnerTeamNameYesNoListener );
  mpAppParam->outTeam = mSelectTray;
  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_05);
  SetSubSeq( MM_SEQ_TEAMNAME_YESNOLOOP );
  
  mpBoxBgLowDraw->SetUIListener( NULL );
  mpBoxBgLowDraw->AllPassiveStart();

}




GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




