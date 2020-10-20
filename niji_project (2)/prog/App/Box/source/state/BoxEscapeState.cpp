//=============================================================================
/**
 * @file	 BoxEscapeState.cpp
 * @brief	 にがすしょり
 * @author k.ohno
 * @date	 2016.3.03
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

#include "BoxEscapeState.h"
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
BoxEscapeState::BoxEscapeState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxEscapeState::~BoxEscapeState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxEscapeState::Setup(void)
{

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpCoreTemp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  mpPartyTemp = GFL_NEW( mpAppHeap->GetDeviceHeap() ) pml::PokeParty( mpAppHeap->GetDeviceHeap() );
  mDInnerNoneListener.SetThis(this);
}


void BoxEscapeState::Release(void)
{
  GFL_SAFE_DELETE(mpPartyTemp);
  GFL_SAFE_DELETE(mpCoreTemp);
}


enum{
  MM_SEQ_ESCAPE_INIT,
	MM_SEQ_ESCAPE_LOOP,
  MM_SEQ_ESCAPE_START,
  MM_SEQ_ECCAPE_ANIME,
	MM_SEQ_ESCAPE_FIRSTMSG,
  MM_SEQ_ESCAPE_END,
	MM_SEQ_ESCAPE_LASTMSG,
  MM_SEQ_ESCAPE_FINISH,
  MM_SEQ_WARRNING_MSG,
  MM_SEQ_WARRNING_MSG_LOOP,
  MM_SEQ_ICON_ANIM,
  MM_SEQ_CLOSE_WAIT,
  MM_SEQ_CLOSE,
};

//他のメニューから切り替えられた時
void BoxEscapeState::ModeStart(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq(MM_SEQ_ESCAPE_INIT);
}



u32 BoxEscapeState::StateFunc(void)
{
  switch(GetSubSeq()){
  case MM_SEQ_ESCAPE_INIT:
    EscapeFuncStart();
    break;
  case MM_SEQ_ESCAPE_LOOP:
    break;
  case MM_SEQ_ESCAPE_START:
    mpBoxBgLowDraw->MessageLine2Close();
    EscapeExecStart();
    SetSubSeq(MM_SEQ_ECCAPE_ANIME);
    break;
  case MM_SEQ_ECCAPE_ANIME:
    if( mpBoxBgLowDraw->PokeFreeAnime(mSelectCursor) == false )
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->SetVisible( mSelectTray, mSelectCursor, false );
      SetSubSeq(MM_SEQ_ESCAPE_FIRSTMSG);
    }
    break;
  case MM_SEQ_ESCAPE_FIRSTMSG:
    {
      BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
      pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
      pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
    }
    SetSubSeq(MM_SEQ_ESCAPE_END);
    break;
  case MM_SEQ_ESCAPE_END:
    mpBoxBgLowDraw->MessageLine2Close();
    EscapeExecEnd();
    SetSubSeq(MM_SEQ_ESCAPE_LASTMSG);
    break;
  case MM_SEQ_ESCAPE_LASTMSG:
    {
      BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
      pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
      pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
    }
    SetSubSeq(MM_SEQ_ESCAPE_FINISH);
    break;
  case MM_SEQ_ESCAPE_FINISH:
    EscapeExecFinish();
    break;
  case MM_SEQ_WARRNING_MSG:
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->WarrningMessageBefore(&mDInnerNoneListener); //GFNMCat[1777]
    {
      BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
      pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
      pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
    }
    mpBoxBgLowDraw->WarrningMessageAfter(mpAppParam->nowDispTray,  mSelectTray, mpAppParam->nowBattleTray); //GFNMCat[1777]
    SetSubSeq(MM_SEQ_WARRNING_MSG_LOOP);

    break;
  case MM_SEQ_WARRNING_MSG_LOOP:
    mpMenuWindow->EndMenu();
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();

    mpBoxBgLowDraw->ResetTemotiTray();
    SetSubSeq( MM_SEQ_CLOSE_WAIT );

/*
    mpBoxBgLowDraw->ResetChatChangeButton();
    mpBoxBgLowDraw->LiftPokeIconClear();
    mpBoxBgLowDraw->ResetFunc();
    return BOXSTATE_RETURN;*/
    break;

  case MM_SEQ_ICON_ANIM:
    mpBoxAppSetup->GetBoxSimpleAccesser()->AnimMove();
    if( mpBoxAppSetup->GetBoxSimpleAccesser()->IsAnimMoveEnd() ){
      mpBoxBgLowDraw->ResetTemotiTray();
      SetSubSeq( MM_SEQ_CLOSE_WAIT );
      mpBoxAppSetup->GetBoxSimpleAccesser()->AnimEnd(); //GFNMCat[3232]
    }
    break;

  case MM_SEQ_CLOSE_WAIT:
    if( mpBoxBgLowDraw->IsTemochiFrameMove() == false )
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
      mpBoxBgLowDraw->LiftPokeIconClear();
      SetSubSeq( MM_SEQ_CLOSE );
    }
    break;
  case MM_SEQ_CLOSE:
    return BOXSTATE_RETURN;
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
applib::frame::Result BoxEscapeState::InitFunc(void)
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
applib::frame::Result BoxEscapeState::UpdateFunc(void)
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
void BoxEscapeState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
applib::frame::Result BoxEscapeState::EndFunc(void)
{
  End();
  Release();
  return applib::frame::RES_FINISH;
}

#if PM_DEBUG
void  BoxEscapeState::DInnerNoneListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerNoneListener\n");
}
#endif



//逃がす時のはいいいえを受け取るインナークラス
void BoxEscapeState::InnerEscapeYesNoListener::SelectMenu( u32 id )
{
  if(id==0){  //はい選択
    mpThis->SetSubSeq(MM_SEQ_ESCAPE_START);
    mpThis->mpMenuWindow->EndMenu();
  }
  else{  //いいえは元に戻す
    mpThis->mpMenuWindow->EndMenu();
    mpThis->mpBoxBgLowDraw->MessageLine2Close();

    mpThis->mpBoxBgLowDraw->ResetTemotiTray();
    mpThis->SetSubSeq( MM_SEQ_CLOSE_WAIT );

//    mpThis->SetSubSeq(MM_SEQ_CLOSE);
    mpThis->mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
  }
}



void BoxEscapeState::EscapeFuncStart(void)
{
  if(!pokeEscapeCheck()){
    return;   //にがせなかった
  }
  mpBoxBgLowDraw->MessageLine2Close();

  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_14);//1行に
  mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_yes);
  mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_no, true);

  mpMenuWindow->StartMenu(1,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_2LINE_POS);

  mInnerEscapeYesNoListener.SetThis(this);
  mpMenuWindow->SetListener( &mInnerEscapeYesNoListener );

  SetSubSeq(MM_SEQ_ESCAPE_LOOP);

}


void BoxEscapeState::EscapeExecStart(void)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);
  mpBoxPokemonMoveFunction->GetPokemon( mSelectTray, mSelectCursor, pp);

  simpleWarrningMessage(msg_box_02_15, 1, false);

  GFL_SAFE_DELETE(pp);

}


void BoxEscapeState::EscapeExecEnd(void)
{
  simpleWarrningMessage(msg_box_02_16, 1, false);
}


void BoxEscapeState::EscapeExecFinish(void)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);
	mpBoxPokemonMoveFunction->GetPokemonPokemonParam( mSelectTray, mSelectCursor, pp);
  pp->Clear();
	mpBoxPokemonMoveFunction->SetPokemonPokemonParam( mSelectTray, mSelectCursor, pp);
  GFL_SAFE_DELETE(pp);

  mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconWhite(mSelectTray, mSelectCursor,false);
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(mSelectTray, mSelectCursor);
  mpBoxAppSetup->GetBoxSimpleAccesser()->ResetPokemonIconDrawData();  //

  mpBoxPokemonMoveFunction->TeamPackAll();

  bool banim = mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_AlignMember(0,NULL);

  if(mSelectCursor >= Savedata::BoxPokemon::TRAY_POKE_MAX && banim)
  {
//    mpBoxAppSetup->GetBoxSimpleAccesser()->ResetPokemonIconDrawData();
    mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    SetSubSeq( MM_SEQ_ICON_ANIM );
  }
  else
  {
//    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(mSelectTray, mSelectCursor);
//    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawState(mSelectTray, mSelectCursor);
    mpBoxBgLowDraw->ResetTemotiTray();
    SetSubSeq(MM_SEQ_CLOSE_WAIT);
  }

//  mpBoxLowBgFrame->RequestChangePoke();
}


//にがしていいかどうか
bool BoxEscapeState::pokeEscapeCheck(void)
{
	mpBoxPokemonMoveFunction->GetPokemon( mSelectTray, mSelectCursor, mpCoreTemp);

  if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){   //たまご
    simpleWarrningMessage(msg_box_02_19, 1, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
    return false;
  }
  if(mpBoxPokeParaFunc->CheckSpecialPokemon(mpCoreTemp)){  //合体
    simpleWarrningMessage(msg_box_02_20, 1, true);  //1行
     SetSubSeq(MM_SEQ_WARRNING_MSG);
    return false;
  }

  if(mSelectCursor < Savedata::BoxPokemon::TRAY_POKE_MAX)
  {  //box
    int teamno = mpAppParam->pBoxData->IsTeamInMe(mSelectTray,mSelectCursor);
    if(teamno!=-1)
    {
      {
        simpleWarrningMessage(msg_box_05_15, 2, true);
        SetSubSeq(MM_SEQ_WARRNING_MSG);
        return false;
      }
    }
  }
  else{  //手持ちの時
    mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_ThisToParty(mpPartyTemp);
    //    mpPartyTemp->CopyFrom( *mpAppParam->pTemochi );
    mpPartyTemp->RemoveMember(mSelectCursor - Savedata::BoxPokemon::TRAY_POKE_MAX);  //入れる前に消す
    if(!mpBoxPokeParaFunc->CheckBattlePokemon(mpPartyTemp) ){  //戦うポケモン残るかどうか
      simpleWarrningMessage(msg_box_02_03, 1, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      return false;
    }
  }
  return true;


}




GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




