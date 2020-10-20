//=============================================================================
/**
 * @file	 BoxMarkState.cpp
 * @brief	 マーク付けのシーケンス
 * @author k.ohno
 * @date	 2016.3.02
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

#include "BoxMarkState.h"
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
BoxMarkState::BoxMarkState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxMarkState::~BoxMarkState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxMarkState::Setup(void)
{

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpPPTemp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap );

 
}


void BoxMarkState::Release(void)
{
  GFL_SAFE_DELETE(mpPPTemp);
}


enum{
  MM_SEQ_MARK_INIT,
  MM_SEQ_MARK_LOOP_BEFOER,
  MM_SEQ_MARK_LOOP,
  MM_SEQ_WARRNING_MSG,
  MM_SEQ_WARRNING_MSG_LOOP,
  MM_SEQ_CLOSE,
  MM_SEQ_MARK_END,
  MM_SEQ_MARK_END_LOOP,
  MM_SEQ_APP_JUMP,

};

//他のメニューから切り替えられた時
void BoxMarkState::ModeStart(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq(MM_SEQ_MARK_INIT);
}



u32 BoxMarkState::StateFunc(void)
{
  switch(GetSubSeq()){

  case MM_SEQ_MARK_INIT:
    MarkInitialize();
    break;
  case MM_SEQ_MARK_LOOP_BEFOER:
    if(mpBoxBgLowDraw->MarkSrideEndFunc()){
      SetSubSeq(MM_SEQ_MARK_LOOP);
    }
    break;
  case MM_SEQ_MARK_LOOP:
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
//    mpMenuWindow->EndMenu();
//    mpBoxBgLowDraw->ResetChatChangeButton();
//    mpBoxBgLowDraw->LiftPokeIconClear();
//    mpBoxBgLowDraw->ResetFunc();
    mpBoxBgLowDraw->MessageLine2Close();
    return BOXSTATE_RETURN;

  case MM_SEQ_MARK_END:
    SetSubSeq(MM_SEQ_MARK_END_LOOP);
    break;
  case MM_SEQ_MARK_END_LOOP:
		if(!mpBoxBgLowDraw->IsPageFrameMove()){
	    mpBoxBgLowDraw->MessageLine2Close();
			return BOXSTATE_RETURN;
		}
		break;

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
applib::frame::Result BoxMarkState::InitFunc(void)
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
applib::frame::Result BoxMarkState::UpdateFunc(void)
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
void BoxMarkState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
applib::frame::Result BoxMarkState::EndFunc(void)
{
  End();
  Release();
  return applib::frame::RES_FINISH;
}

///リスナー
#if PM_DEBUG
void  BoxMarkState::DInnerNoneListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerNoneListener\n");
}
#endif

//マークいつけてる時のカーソルの時用
#if PM_DEBUG
void  BoxMarkState::DInnerMarkListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerMarkListener\n");
}
#endif
bool BoxMarkState::DInnerMarkListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_MARK_END );
  return true;
}


void BoxMarkState::DInnerMarkListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK0:
		mpThis->MarkCursorMove(0);
    break;
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK1:
		mpThis->MarkCursorMove(1);
    break;
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK2:
		mpThis->MarkCursorMove(2);
    break;
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK3:
		mpThis->MarkCursorMove(3);
    break;
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK4:
		mpThis->MarkCursorMove(4);
    break;
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK5:
		mpThis->MarkCursorMove(5);
    break;
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_CANCEL:
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
		mpThis->mpBoxBgLowDraw->MarkWinEnd();
		mpThis->mpBoxBgLowDraw->PassiveMainCursor();
		mpThis->SetSubSeq( MM_SEQ_MARK_END );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_DECIDE:
    //マーク
    mpThis->SetMarkTableAction();
    //ひきつづき
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_UP:   //上
  case BoxLowBgDrawListener::LOWBG_ACTION_DOWN:   //下
  case BoxLowBgDrawListener::LOWBG_ACTION_LEFT:    //左
  case BoxLowBgDrawListener::LOWBG_ACTION_RIGHT:    //右
    mpThis->CursorActionMark(button_no - BoxLowBgDrawListener::LOWBG_ACTION_UP);
    break;
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_OK:
    mpThis->mMarkCursorPos=6;
    mpThis->SetMarkTableAction();
    break;
  }
}



void BoxMarkState::CursorActionMark( int udlr)
{
  if(mpBoxBgLowDraw){
    mMarkCursorPos = mpBoxBgLowDraw->SetCursorPosMk(mMarkCursorPos, udlr);
  }
}



void BoxMarkState::MarkCursorMove(int no)
{
  mMarkCursorPos=no;
  mpBoxBgLowDraw->DirectCursorPosMk(no);
  mpBoxBgLowDraw->ReloadCursorPos();
  SetMarkTableAction();
}


void BoxMarkState::MarkInitialize(void)
{

  if(mSelectCursor < Savedata::BoxPokemon::TRAY_POKE_MAX)
  {  //box
    if( mpAppParam->pBoxData->IsMeLock(mSelectTray, mSelectCursor) )// NMCat746
    {
      simpleWarrningMessage(msg_box_05_11, 2, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      return;
    }
  }

  mpBoxBgLowDraw->SetUIListener(&mDInnerMarkListener);
  mDInnerMarkListener.SetThis(this);

  mMarkCursorPos=0;
  mpBoxBgLowDraw->DirectCursorPosMk(0);//NMCat2511
  CursorActionMark(0);
  if(mSelectCursor < Savedata::BoxPokemon::TRAY_POKE_MAX)
  {
    mpBoxBgLowDraw->MarkWinStart(false);
  }
  else
  {
    mpBoxBgLowDraw->MarkWinStart(true);
  }
  SetSubSeq( MM_SEQ_MARK_LOOP_BEFOER );
  mpBoxPokemonMoveFunction->GetPokemonPokemonParam(mSelectTray, mSelectCursor, mpPPTemp);  //マーク操作するポケモンを抽出
  mpBoxBgLowDraw->ResetMainCursor();

  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_13);


  for(int i=0;i<_MARK_MAX;i++)
  {
    pml::pokepara::BoxMarkColor color = mpPPTemp->GetBoxMark((pml::pokepara::BoxMark)i);
    mpBoxBgLowDraw->StartMarkAnim(i, color );  //色アニメ変更
  }
}


//マークをつけるか、決定かキャンセル
void BoxMarkState::SetMarkTableAction(void)
{
  //  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap();


  if(mMarkCursorPos < _MARK_MAX){

    pml::pokepara::BoxMarkColor color = mpPPTemp->GetBoxMark((pml::pokepara::BoxMark)mMarkCursorPos);

    OHNO_PRINT("アニメ開始1 %d %d\n",mMarkCursorPos, color);

    Sound::PlaySE(SEQ_SE_DECIDE1);

    switch(color){
    case pml::pokepara::BOX_MARK_COLOR_NONE:
      color = pml::pokepara::BOX_MARK_COLOR_BLUE;
      mpBoxBgLowDraw->StartMarkAnim(mMarkCursorPos, color );  //色アニメ変更
      mpPPTemp->SetBoxMark((pml::pokepara::BoxMark)mMarkCursorPos, color);  //値変更
      break;
    case pml::pokepara::BOX_MARK_COLOR_BLUE:
      color = pml::pokepara::BOX_MARK_COLOR_RED;
      mpBoxBgLowDraw->StartMarkAnim(mMarkCursorPos, color );  //色アニメ変更
      mpPPTemp->SetBoxMark((pml::pokepara::BoxMark)mMarkCursorPos, color);  //値変更
      break;
    case pml::pokepara::BOX_MARK_COLOR_RED:
      color = pml::pokepara::BOX_MARK_COLOR_NONE;
      mpBoxBgLowDraw->StartMarkAnim(mMarkCursorPos, color );  //色アニメ変更
      mpPPTemp->RemoveBoxMark((pml::pokepara::BoxMark)mMarkCursorPos);
      break;
    default:
      GFL_ASSERT(0);
    }
    OHNO_PRINT("アニメ開始2 %d %d\n",mMarkCursorPos, color);

  }
  else if(mMarkCursorPos==6){  //決定
    Sound::PlaySE(SEQ_SE_DECIDE1);
    mpBoxPokemonMoveFunction->SetPokemonPokemonParam(mSelectTray, mSelectCursor, mpPPTemp);
    mpBoxBgLowDraw->MarkWinEnd();
    mpBoxBgLowDraw->PassiveMainCursor();
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon( mSelectTray, mSelectCursor );   // @fix NMCat[2929]: アイコン表示更新
    SetSubSeq( MM_SEQ_MARK_END );
  }
  else{ //キャンセル
    mpBoxBgLowDraw->PassiveMainCursor();
    mpBoxBgLowDraw->MarkWinEnd();
    SetSubSeq( MM_SEQ_MARK_END );
  }


}




GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




