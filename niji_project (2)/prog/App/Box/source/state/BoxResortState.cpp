﻿//=============================================================================
/**
 * @file	 BoxResortState.cpp
 * @brief	 リゾートの移動状態
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

#include "BoxResortState.h"
#include "BoxAllBoxState.h"
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
BoxResortState::BoxResortState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxResortState::~BoxResortState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxResortState::Setup(void)
{

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpCoreTemp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  mInnerBoxAllBoxEndListener.SetThis(this);
	mDInnerPokeMoveListener.SetThis(this);
  mDInnerRangeMoveListener.SetThis(this);
  mDInnerCarryRangePokeMoveListener.SetThis(this);
  mDInnerNoneListener.SetThis(this);
  mDecideCursor = -1;

}


void BoxResortState::Release(void)
{
  GFL_SAFE_DELETE(mpCoreTemp);

  if(mDecideCursor>=0){
//    mpBoxBgLowDraw->SetCursorPos(mDecideCursor, -1);
//    mpBoxLowBgFrame->SetNowCursorNo( mDecideCursor );
  }
  else{
    mpBoxBgLowDraw->SetCursorPos(mSelectCursor, -1);
    mpBoxLowBgFrame->SetNowCursorNo( mSelectCursor );
  }
}


enum{
  MM_SEQ_MENU_START,
	MM_SEQ_MENU_LOOP,
  MM_SEQ_MENU_CANCEL_BEFORE,
  MM_SEQ_MENU_CANCEL,
  MM_SEQ_MOVE_SINGLE_READY,
  MM_SEQ_MOVE_SINGLE_BEFORE,
  MM_SEQ_MOVE_SINGLE_BEFORE2,
	MM_SEQ_MOVE_SINGLE,
	MM_SEQ_MOVE_SINGLE_M,
  MM_SEQ_MOVE_SINGLE_LOOP,
	MM_SEQ_MOVE_SINGLE_MOVEEXEC,
	MM_SEQ_MOVE_SINGLE_M_AN,
	MM_SEQ_CLOSE_RETURN_SELECT_POS,
  MM_SEQ_CLOSE_RETURN_SELECT_POS_WAIT,
  MM_SEQ_PAGEWAIT_CLOSE,
	MM_SEQ_MOVE_SINGLE_C,
	MM_SEQ_MOVE_SINGLE_M2_C,
	MM_SEQ_MOVE_SINGLE_M2_CANCEL,
	MM_SEQ_MOVE_SINGLE_M2,
	MM_SEQ_MOVE_SINGLE_M3,
	MM_SEQ_RANGE_START,
  MM_SEQ_RANGE_SELECT_2ND,
	MM_SEQ_RANGE_SELECT,
	MM_SEQ_RANGE_SET,
	MM_SEQ_RANGE_SET_TOUCH,
  MM_SEQ_MOVE_RANGE_CARRY,
  MM_SEQ_RR_WARRNING_MSG,  //持ち上げる前のエラー
  MM_SEQ_WARRNING_MSG,
  MM_SEQ_WARRNING_MSG_LOOP,
  MM_SEQ_CLOSE,
  MM_SEQ_APP_JUMP,
  MM_SEQ_CHANGE_ITEM_STATE,
  MM_SEQ_CHANGE_MARK_STATE,
  MM_SEQ_CHANGE_STATUS_STATE,
  
  MM_SEQ_CHANGE_FOLDER_SINGLE_STATE,
  MM_SEQ_CHANGE_FOLDER_RANGE_STATE,
	MM_SEQ_CHANGE_FOLDER_END,
  MM_SEQ_PAGECHANGE_POKEMOVE,//GFNMCat[1621]
  MM_SEQ_PAGECHANGE_RANGEMOVE,//GFNMCat[1621]
	MM_SEQ_PAGECHANGE_CARRYRANGE,//GFNMCat[1621]
};

//他のメニューから切り替えられた時
void BoxResortState::ModeStart(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq(MM_SEQ_MENU_START);
}



//ショートカット
void BoxResortState::ShortcutStart(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq(MM_SEQ_MOVE_SINGLE_READY);
}


//範囲選択
void BoxResortState::RangeStart(int tray,int cursor)
{
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq( MM_SEQ_RANGE_START );
}

void BoxResortState::LocalSetup( BoxLowFolderDraw* pBoxLowFolderDraw )
{
  mpBoxLowFolderDraw=pBoxLowFolderDraw;
}




u32 BoxResortState::StateFunc(void)
{
  switch(GetSubSeq()){
  case MM_SEQ_MENU_START:
    FirstMenuInit();
    break;
  case MM_SEQ_MENU_LOOP:
    break;


  case MM_SEQ_MENU_CANCEL_BEFORE:
    mpBoxBgLowDraw->ResetTemotiTray();
    SetSubSeq( MM_SEQ_MENU_CANCEL );
    break;

  case MM_SEQ_MENU_CANCEL:
    if( !mpBoxBgLowDraw->IsTemochiFrameMove() ){
      MenuCancel();
      return BOXSTATE_RETURN;
    }
    break;

  case MM_SEQ_MOVE_SINGLE_BEFORE:
    mpBoxBgLowDraw->ResetTemotiTray();
		SetSubSeq( MM_SEQ_MOVE_SINGLE_BEFORE2 );
    break;
  case MM_SEQ_MOVE_SINGLE_BEFORE2:
    if( !mpBoxBgLowDraw->IsTemochiFrameMove() ){
      SetSubSeq( MM_SEQ_MOVE_SINGLE );
    }
    break;
    
  case MM_SEQ_MOVE_SINGLE_READY:
    mpBoxBgLowDraw->LiftPokeIconShortcut(mSelectTray,mSelectCursor);
    mpBoxBgLowDraw->PassiveRightUnderButton();
    mpBoxBgLowDraw->PassiveCancelButton();
    SetSubSeq(MM_SEQ_MOVE_SINGLE);
    //break;
  case MM_SEQ_MOVE_SINGLE:
    CarryupPokemon();
    break;
  case MM_SEQ_MOVE_SINGLE_M:
    break;

  case MM_SEQ_MOVE_SINGLE_MOVEEXEC:
    if(PokeMoveExec()!=BoxPokemonMoveFunction::MOVEERR_NONE)
		{
			SetSubSeq(MM_SEQ_WARRNING_MSG);
		}
    break;
  case MM_SEQ_MOVE_SINGLE_M_AN:
    mpBoxAppSetup->GetBoxSimpleAccesser()->AnimMove();
    if( mpBoxAppSetup->GetBoxSimpleAccesser()->IsAnimMoveEnd() ){
      mpBoxAppSetup->GetBoxSimpleAccesser()->PacksResortPokemon();
      
			mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonUseStock();
      mpBoxBgLowDraw->CarryPokeIconRangeEnd( mpAppParam->nowDispTray );

//      mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
 //     mpBoxBgLowDraw->LiftPokeIconClear();
      mpBoxBgLowDraw->MessageLine2Close();
      return BOXSTATE_RETURN;
    }
    break;

  case  MM_SEQ_CLOSE_RETURN_SELECT_POS:
    mpBoxLowBgFrame->PageChangeDirectSelectTray();
    SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS_WAIT );
    break;
    
  case  MM_SEQ_CLOSE_RETURN_SELECT_POS_WAIT:
    if(mpBoxBgLowDraw->IsPageFrameMove()){
      break;
    }
    mpBoxBgLowDraw->CarryPokeIconRangeEnd(  mpAppParam->nowDispTray );
    mpBoxBgLowDraw->LiftPokeIconClear();
    mpBoxBgLowDraw->MessageLine2Close();
    return BOXSTATE_RETURN;
    
	case MM_SEQ_PAGEWAIT_CLOSE:
    if(mpBoxBgLowDraw->IsPageFrameMove()){
      break;
    }
    Sound::PlaySE(SEQ_SE_RELEASE);
    mpBoxBgLowDraw->LiftPokeIconEnd(  mpAppParam->nowDispTray,mpAppParam->nowBattleTray );
    return BOXSTATE_RETURN;

  case  MM_SEQ_MOVE_SINGLE_M2_CANCEL:
    Sound::PlaySE(SEQ_SE_CANCEL1);
  case MM_SEQ_MOVE_SINGLE_M2:
    mpBoxBgLowDraw->LiftPokeIconClear();
    SetSubSeq( MM_SEQ_MOVE_SINGLE_M3 );
    //      break;
  case MM_SEQ_MOVE_SINGLE_M3:
    mpBoxBgLowDraw->MessageLine2Close();
 //   mpBoxBgLowDraw->ResetFunc();
		return BOXSTATE_RETURN;
  case MM_SEQ_RANGE_START:
    RangeStart();
    break;
  case MM_SEQ_RANGE_SELECT_2ND:
    break;
  case MM_SEQ_RANGE_SELECT:
    mpBoxLowBgFrame->RangeSelectStretch(&mRangeSelect);    //枠設定中
    break;
  case MM_SEQ_MOVE_RANGE_CARRY:
    CarryRange();   //複数持ち上げる
    break;
  case MM_SEQ_RANGE_SET:
    SetRangePokemon(false);
    break;
  case MM_SEQ_RANGE_SET_TOUCH:
    SetRangePokemon(true);
    break;
  case MM_SEQ_RR_WARRNING_MSG:
    mpBoxBgLowDraw->LiftPokeIconReadyReturn2(mSelectTray, mSelectCursor);
//    break;
  case MM_SEQ_WARRNING_MSG:
		mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->WarrningMessageBefore(&mDInnerNoneListener);

    {
      mpBoxBgLowDraw->MessageLine2Close();
      
      BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
      pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
      pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
    }
    mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpBoxBgLowDraw->WarrningMessageAfter(mpAppParam->nowDispTray,  mSelectTray, mpAppParam->nowBattleTray);
    SetSubSeq(MM_SEQ_WARRNING_MSG_LOOP);

    break;
  case MM_SEQ_WARRNING_MSG_LOOP:
    return BOXSTATE_RETURN;

  case MM_SEQ_CLOSE:
		return BOXSTATE_RETURN;

  case MM_SEQ_CHANGE_ITEM_STATE:
    return BOXSTATE_ITEM;
  case MM_SEQ_CHANGE_MARK_STATE:
    return BOXSTATE_MARK;
  case MM_SEQ_CHANGE_STATUS_STATE:
    return BOXSTATE_STATUS;


  case MM_SEQ_CHANGE_FOLDER_SINGLE_STATE:
    {
      BoxAllBoxState*  pBoxAllBoxState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxAllBoxState >( GetManager() );
      pBoxAllBoxState->SetParam( &mInnerBoxAllBoxEndListener,mpAppParam, mpBoxAppSetup,
                                 mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, mpBoxLowBgFrame, mpMenuWindow,mpBoxBgLowDraw );
      pBoxAllBoxState->AllBoxLocalSetup( mpBoxLowFolderDraw );

      pBoxAllBoxState->SetBoxUppStatusListener(mpBoxUppStatusListener);
      mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_FOLDER ,0);
      
      pBoxAllBoxState->FolderPokemonMoveStart(mSelectTray,mSelectCursor);
    }
		SetSubSeq(MM_SEQ_CHANGE_FOLDER_END);
    break;
  case MM_SEQ_CHANGE_FOLDER_RANGE_STATE:
    {
      BoxAllBoxState*  pBoxAllBoxState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxAllBoxState >( GetManager() );
      pBoxAllBoxState->SetParam( &mInnerBoxAllBoxEndListener,mpAppParam, mpBoxAppSetup,
                                 mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, mpBoxLowBgFrame, mpMenuWindow,mpBoxBgLowDraw );
      pBoxAllBoxState->AllBoxLocalSetup( mpBoxLowFolderDraw );

      pBoxAllBoxState->SetBoxUppStatusListener(mpBoxUppStatusListener);
      mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_FOLDER ,0);
      
      pBoxAllBoxState->FolderRangeMoveStart(mSelectTray,mSelectCursor,&mRangeSelect);
    }
		SetSubSeq(MM_SEQ_CHANGE_FOLDER_END);
    break;
  case MM_SEQ_CHANGE_FOLDER_END:
    mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_FOLDER_END ,0);
    switch( mBoxAllBoxStateRet ){
    case BoxStateBasis::BOXSTATE_NONE:   //シーケンスはそのまま
      GFL_ASSERT(0);
      break;
    case BoxStateBasis::BOXSTATE_RETURN_POKEMON_NORMAL:
      if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、離された場合ＮＧ IsHoldTouchPanel
        SetSubSeq( MM_SEQ_MENU_CANCEL_BEFORE );
      }
      else{
        mpBoxBgLowDraw->SetUIListener(&mDInnerPokeMoveListener);
        SetSubSeq(MM_SEQ_MOVE_SINGLE_M);
      }
      break;
    case BoxStateBasis::BOXSTATE_RETURN_POKEMON_RANGE:
      if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、離された場合ＮＧ IsHoldTouchPanel
        SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS ); //キャンセル
      }
      else{
        mpBoxBgLowDraw->SetUIListener(&mDInnerCarryRangePokeMoveListener);
        SetSubSeq(MM_SEQ_RANGE_SELECT_2ND);
      }
      break;
    case BOXSTATE_WARRNING_BATTLE:
      simpleWarrningMessage(msg_box_02_03, 1, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      break;
    case BOXSTATE_WARRNING_TEAM:
      simpleWarrningMessage(msg_box_05_09, 2, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      break;
    case BoxStateBasis::BOXSTATE_RETURN:  //もどる
      return BOXSTATE_RETURN;  //リゾートモードも抜けてOK
    case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
      return BOXSTATE_APP_JUMP;  //リゾートモードも抜けてOK
    }
    break;
  case MM_SEQ_PAGECHANGE_POKEMOVE://GFNMCat[1621]
    mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    if(!mpBoxBgLowDraw->IsPageFrameMove()){

      if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
        OHNO_PRINT("MM_SEQ_MOVE_SINGLE_M MM_SEQ_CLOSE\n");
        SetSubSeq(MM_SEQ_CLOSE_RETURN_SELECT_POS);
      }
      else{
        SetSubSeq( MM_SEQ_MOVE_SINGLE_M );  //スクロールが終わると先ほどのシーケンスへ復帰
        mpBoxBgLowDraw->SetUIListener(&mDInnerPokeMoveListener);
      }
    }
    break;
  case MM_SEQ_PAGECHANGE_RANGEMOVE:
    mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    if(!mpBoxBgLowDraw->IsPageFrameMove()){

      if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
        OHNO_PRINT("MM_SEQ_MOVE_SINGLE_M MM_SEQ_CLOSE\n");
        SetSubSeq(MM_SEQ_CLOSE_RETURN_SELECT_POS);
      }
      else{
        SetSubSeq( MM_SEQ_RANGE_SELECT );  //スクロールが終わると先ほどのシーケンスへ復帰
        mpBoxBgLowDraw->SetUIListener(&mDInnerRangeMoveListener);
      }
    }
    break;

  case MM_SEQ_PAGECHANGE_CARRYRANGE:
    mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    if(!mpBoxBgLowDraw->IsPageFrameMove()){
      if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
        OHNO_PRINT("MM_SEQ_MOVE_SINGLE_M MM_SEQ_CLOSE\n");
        SetSubSeq(MM_SEQ_CLOSE_RETURN_SELECT_POS);
      }
      else{
        SetSubSeq( MM_SEQ_RANGE_SELECT_2ND );  //スクロールが終わると先ほどのシーケンスへ復帰
        mpBoxBgLowDraw->SetUIListener(&mDInnerCarryRangePokeMoveListener);
      }
    }
    break;
  default:
    GFL_ASSERT(0);  //定義を必ず書く
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
applib::frame::Result BoxResortState::InitFunc(void)
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
applib::frame::Result BoxResortState::UpdateFunc(void)
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
void BoxResortState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
applib::frame::Result BoxResortState::EndFunc(void)
{
  End();
  Release();
  return applib::frame::RES_FINISH;
}

///---リスナー



#if PM_DEBUG
void  BoxResortState::DInnerNoneListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerNoneListener\n");
}
#endif




//ポケモンを抱えて動いている時用単数

#if PM_DEBUG
void  BoxResortState::DInnerPokeMoveListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerPokeMoveListener\n");
}
#endif
bool BoxResortState::DInnerPokeMoveListener::OnMousePokemonCurpos(int no)
{
  return true;//タッチパネルキャッチに変更
}

bool BoxResortState::DInnerPokeMoveListener::OnMousePokemonFirstTouch(int curpos)
{
  mpThis->mpBoxBgLowDraw->SetTukamiMode();
  return false;
}


//--------------------------------------------------------------------------
/**
 * @brief   掴みに変更
 */
//--------------------------------------------------------------------------
void BoxResortState::DInnerPokeMoveListener::OnMousePokemonHand(int x,int y, int cursorno)
{
  mpThis->mpBoxBgLowDraw->PassiveCancelButton();
}


//--------------------------------------------------------------------------
/**
 * @brief   配置   //ペイン同士で判定
 */
//--------------------------------------------------------------------------
bool BoxResortState::DInnerPokeMoveListener::OnMousePokemonPut(int cursorno,int targetno)
{
  OHNO_PRINT("DInnerPokeMoveListener %d\n",targetno);
  if(targetno < 0){//キャンセル
//    mpThis->mpBoxBgLowDraw->LiftPokeIconEnd(  mpThis->mpAppParam->nowDispTray,mpThis->mpAppParam->nowBattleTray );
    mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpThis->SetSubSeq( MM_SEQ_PAGEWAIT_CLOSE );
  }
  else{
    mpThis->mpBoxLowBgFrame->SetNowCursorNo( targetno );
    mpThis->mpBoxBgLowDraw->SetCursorPos(targetno, -1);
    mpThis->SetSubSeq(MM_SEQ_MOVE_SINGLE_MOVEEXEC);

  }
  return true;
}

bool BoxResortState::DInnerPokeMoveListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

void BoxResortState::DInnerPokeMoveListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
  //  mpThis->mpBoxBgLowDraw->LiftPokeIconEnd(  mpThis->mpAppParam->nowDispTray,mpThis->mpAppParam->nowBattleTray );
    mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpThis->SetSubSeq( MM_SEQ_PAGEWAIT_CLOSE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_DECIDE:
    mpThis->SetSubSeq( MM_SEQ_MOVE_SINGLE_MOVEEXEC );

    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_UP:   //上
  case BoxLowBgDrawListener::LOWBG_ACTION_DOWN:   //下
  case BoxLowBgDrawListener::LOWBG_ACTION_LEFT:    //左
  case BoxLowBgDrawListener::LOWBG_ACTION_RIGHT:    //右
    mpThis->mpBoxLowBgFrame->CursorAction(button_no - BoxLowBgDrawListener::LOWBG_ACTION_UP);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE:    //LBUTTON
    mpThis->mpBoxLowBgFrame->PageChange(-1);
    mpThis->SetSubSeq( MM_SEQ_PAGECHANGE_POKEMOVE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE:    //RBUTTON
    mpThis->mpBoxLowBgFrame->PageChange(1);
    mpThis->SetSubSeq( MM_SEQ_PAGECHANGE_POKEMOVE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_TEAM:    //LBUTTON
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_TEAM:    //RBUTTON
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_SELECT_CHANGE:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_FOLDER:
    mpThis->SetSubSeq(MM_SEQ_CHANGE_FOLDER_SINGLE_STATE);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_INFO:
    GFL_ASSERT(0);
    break;

  }
}
//複数選択時用

#if PM_DEBUG
void  BoxResortState::DInnerRangeMoveListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerRangeMoveListener\n");
}
#endif


//--------------------------------------------------------------------------
/**
 * @brief   ドラッグ中カーソル位置変更
 */
//--------------------------------------------------------------------------
void BoxResortState::DInnerRangeMoveListener::OnMouseDragCurpos(int cursorno)
{
  mpThis->mpBoxLowBgFrame->SetNowCursorNo( cursorno );
  GFL_ASSERT(cursorno>=0);
}


//--------------------------------------------------------------------------
/**
 * @brief   タッチの直後
 */
//--------------------------------------------------------------------------
bool BoxResortState::DInnerRangeMoveListener::OnMousePokemonFirstTouch(int cursorno)
{
  mpThis->mpBoxBgLowDraw->SetTukamiMode();
  mpThis->SetSubSeq( MM_SEQ_MOVE_RANGE_CARRY );
  return false;
}


bool BoxResortState::DInnerRangeMoveListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

void BoxResortState::DInnerRangeMoveListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
    mpThis->mpBoxBgLowDraw->RangeSelectEnd();
    mpThis->SetSubSeq( MM_SEQ_CLOSE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_DECIDE:
    //      複数もちあげる
    mpThis->mpBoxBgLowDraw->ResetMainCursor();
    mpThis->SetSubSeq( MM_SEQ_MOVE_RANGE_CARRY );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_UP:   //上
  case BoxLowBgDrawListener::LOWBG_ACTION_DOWN:   //下
  case BoxLowBgDrawListener::LOWBG_ACTION_LEFT:    //左
  case BoxLowBgDrawListener::LOWBG_ACTION_RIGHT:    //右
    mpThis->mpBoxLowBgFrame->CursorAction(button_no - BoxLowBgDrawListener::LOWBG_ACTION_UP);
    mpThis->mpBoxBgLowDraw->ResetMainCursor();
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE:    //LBUTTON
    mpThis->mpBoxLowBgFrame->PageChange(-1);
    mpThis->SetSubSeq( MM_SEQ_PAGECHANGE_RANGEMOVE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE:    //RBUTTON
    mpThis->mpBoxLowBgFrame->PageChange(1);
    mpThis->SetSubSeq( MM_SEQ_PAGECHANGE_RANGEMOVE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_TEAM:    //LBUTTON
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_TEAM:    //RBUTTON
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_SELECT_CHANGE:
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE:
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_FOLDER:
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_INFO:
    break;

  }
}


//ポケモンを抱えて動いている時用（複数）
#if PM_DEBUG
void  BoxResortState::DInnerCarryRangePokeMoveListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerCarryRangePokeMoveListener\n");
}
#endif
bool BoxResortState::DInnerCarryRangePokeMoveListener::OnMousePokemonPut(int cursor,int terger)
{
  OHNO_PRINT("DInnerCarryRangePokeMoveListener %d\n",terger);
  if(terger!=-1){
    mpThis->mpBoxLowBgFrame->SetNowCursorNo(terger);
  }
  GFL_ASSERT(mpThis->mpBoxLowBgFrame->GetNowCursorNo() >= 0);

  if(terger < 0){//キャンセル
    mpThis->mpBoxBgLowDraw->CarryPokeIconRangeEnd(  mpThis->mpAppParam->nowDispTray );
//    mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpThis->SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS );
  }
  else{
    mpThis->SetSubSeq( MM_SEQ_RANGE_SET_TOUCH );
  }
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   タッチの直後
 */
//--------------------------------------------------------------------------
bool BoxResortState::DInnerCarryRangePokeMoveListener::OnMousePokemonFirstTouch(int cursorno)
{
  mpThis->mpBoxBgLowDraw->SetTukamiMode();
  return false;
}


bool BoxResortState::DInnerCarryRangePokeMoveListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

void BoxResortState::DInnerCarryRangePokeMoveListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
    mpThis->mpBoxBgLowDraw->CarryPokeIconRangeEnd(  mpThis->mpAppParam->nowDispTray );
//    mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpThis->SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_DECIDE:
    mpThis->SetSubSeq( MM_SEQ_RANGE_SET );
    
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_UP:   //上
  case BoxLowBgDrawListener::LOWBG_ACTION_DOWN:   //下
  case BoxLowBgDrawListener::LOWBG_ACTION_LEFT:    //左
  case BoxLowBgDrawListener::LOWBG_ACTION_RIGHT:    //右
    mpThis->mpBoxLowBgFrame->CursorAction(button_no - BoxLowBgDrawListener::LOWBG_ACTION_UP);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE:    //LBUTTON
    mpThis->mpBoxLowBgFrame->PageChange(-1);
    mpThis->SetSubSeq( MM_SEQ_PAGECHANGE_CARRYRANGE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE:    //RBUTTON
    mpThis->mpBoxLowBgFrame->PageChange(1);
    mpThis->SetSubSeq( MM_SEQ_PAGECHANGE_CARRYRANGE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_TEAM:    //LBUTTON
    mpThis->mpBoxLowBgFrame->BattleInPageChange(-1);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_TEAM:    //RBUTTON
    mpThis->mpBoxLowBgFrame->BattleInPageChange(1);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_SELECT_CHANGE:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_FOLDER:
    mpThis->SetSubSeq(  MM_SEQ_CHANGE_FOLDER_RANGE_STATE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_INFO:
    GFL_ASSERT(0);
    break;

  }
}



//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー リゾート グループ部分
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxResortState::InnerResortGroupListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //いどうする  -->>どこにいどうしますか
    mpThis->SetSubSeq(MM_SEQ_MOVE_SINGLE_BEFORE);
    break;
  case 1:    //つよさをみる
    mpThis->SetSubSeq(MM_SEQ_CHANGE_STATUS_STATE);
    break;
  case 2:  //もちもの
    mpThis->SetSubSeq(MM_SEQ_CHANGE_ITEM_STATE);
    break;
  case 3:  //マーキング
    mpThis->SetSubSeq(MM_SEQ_CHANGE_MARK_STATE);
    break;



    
  default:
    GFL_ASSERT(0);
  case 4:  //やめる
    mpThis->SetSubSeq( MM_SEQ_MENU_CANCEL_BEFORE );
    break;
  }
  mpThis->mpMenuWindow->EndMenu();

}





//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー リゾート選択
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxResortState::InnerResortListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //いどうする
    mpThis->SetSubSeq(MM_SEQ_MOVE_SINGLE_BEFORE);
    break;
  case 1:    //強さをみる
		mpThis->SetSubSeq(MM_SEQ_CHANGE_STATUS_STATE);
    break;

  case 2:  //もちもの
		mpThis->SetSubSeq(MM_SEQ_CHANGE_ITEM_STATE);
    break;
  case 3:  //マーキング
    mpThis->SetSubSeq(MM_SEQ_CHANGE_MARK_STATE);
    break;

  default:
    GFL_ASSERT(0);
  case 4:  //やめる
    mpThis->SetSubSeq( MM_SEQ_MENU_CANCEL_BEFORE );
    break;
  }

  mpThis->mpMenuWindow->EndMenu();
}


void BoxResortState::FirstMenuInit(void)
{
  int retchk = mpBoxPokemonMoveFunction->FirstTradeCheck(mSelectTray,mSelectCursor);
  switch(retchk){
	case BoxPokemonMoveFunction::MOVEERR_EGG_PULL:
    simpleWarrningMessage(msg_box_05_03, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  }
  if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
    SetSubSeq(MM_SEQ_CLOSE);
    return;
  }
  
  if(!(mSelectCursor >= Savedata::BoxPokemon::TRAY_POKE_MAX))
  {
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_01); //移動する
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_02);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_03);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_04);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_end,true,true);
    mInnerResortListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerResortListener );
  }
  else
  {
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_01);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_02);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_03);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_04);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_end,true,true);
    mInnerResortGroupListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerResortGroupListener );
  }

  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);  //fix
  mpBoxBgLowDraw->LiftPokeIconReady(mSelectTray,mSelectCursor);
  Sound::PlaySE(SEQ_SE_DECIDE1);

  mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, mpCoreTemp);
  mpBoxBgLowDraw->ExpandPokemon( 0,mpCoreTemp );  //委譲
  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_02);
  SetSubSeq( MM_SEQ_MENU_LOOP );

}





void BoxResortState::MenuCancel(void)
{
  mpBoxBgLowDraw->LiftPokeIconReadyReturn2(mSelectTray, mSelectCursor);
  mpBoxBgLowDraw->ResetLiftPokeIconClear(); // @fix niji GFNMCat[254]: LiftPokeIconClear()内のカーソル以外の処理を実行
  mpBoxBgLowDraw->MessageLine2Close();
}


void BoxResortState::CarryupPokemon(void)
{

  {
    if(mSelectCursor < Savedata::BoxPokemon::TRAY_POKE_MAX) {
      if( mpAppParam->pBoxData->IsMeLock(mSelectTray, mSelectCursor) ){//ロックしてる自分 NMCat[746]
        simpleWarrningMessage(msg_box_05_09, 2, true);
        SetSubSeq(MM_SEQ_RR_WARRNING_MSG);
        return;
      }
    }
  }

  if(mpBoxLowBgFrame->GetNowCursorNo() >= Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS){
    OHNO_PRINT("止めない処理★★★★★★★★★\n");
    SetSubSeq(MM_SEQ_CLOSE);
    return;
  }
  bool bnull=false;
  {
    pml::pokepara::PokemonParam * pp = GFL_NEW(mpAppHeap->GetDeviceHeap()->GetLowerHandle())
      pml::pokepara::PokemonParam(mpAppHeap->GetDeviceHeap()->GetLowerHandle());
    mpBoxPokemonMoveFunction->GetPokemon( mSelectTray, mSelectCursor, pp);
    bnull=pp->IsNull();

    OHNO_PRINT("---xxx----mons %d %d %d\n", pp->GetMonsNo(), mSelectTray, mSelectCursor);

    GFL_SAFE_DELETE(pp);
  }
  if(bnull){
    OHNO_PRINT("止めない処理２\n");
    SetSubSeq(MM_SEQ_CLOSE);
    return;
  }
  if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
    SetSubSeq(MM_SEQ_CLOSE);
    return;
  }

  mpBoxBgLowDraw->LiftPokeIconStart(mSelectTray, mSelectCursor, false );
  Sound::PlaySE(SEQ_SE_CATCH);

 mpBoxLowBgFrame->CursorAction(-1);

  SetSubSeq( MM_SEQ_MOVE_SINGLE_M );

  mpBoxBgLowDraw->MessageLine2Close();

	mpBoxBgLowDraw->SetUIListener(&mDInnerPokeMoveListener);

  mpBoxBgLowDraw->SetBoxBarInputEnable(false);
  mpBoxBgLowDraw->ResetFolderButton();  //GFNMCat1940
  mpBoxBgLowDraw->ResetBoxChangeCursor();
}






int BoxResortState::PokeMoveExec(void)
{
  mpBoxBgLowDraw->SetUIListener(NULL);

  int tray = mpBoxLowBgFrame->GetNowTrayNo();
  int no;

  no = mpBoxPokemonMoveFunction->pokeMoveCheck(mSelectTray, mSelectCursor,
                                               mpBoxLowBgFrame->GetNowTrayNo(), mpBoxLowBgFrame->GetNowCursorNo());  //置いてはいけない条件に引っかかった
	switch(no){
	case BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE:
		simpleWarrningMessage(msg_box_02_03, 1, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
		return no;
	case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK:
		simpleWarrningMessage(msg_box_05_09, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return no;
  case BoxPokemonMoveFunction::MOVEERR_TEAM_ENTRY:
		simpleWarrningMessage(msg_box_05_16, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return no;
    
	case BoxPokemonMoveFunction::MOVEERR_EGG_PUT:
		simpleWarrningMessage(msg_box_02_26, 1, true);//GFNMCat:2101
    SetSubSeq(MM_SEQ_WARRNING_MSG);
		return no;
	}


  Sound::PlaySE(SEQ_SE_RELEASE);
  mpBoxBgLowDraw->TemotiPokeIconAnimStart();

  mpBoxPokemonMoveFunction->PokemonSet(mSelectTray,mSelectCursor, mpBoxLowBgFrame->GetNowTrayNo(), mpBoxLowBgFrame->GetNowCursorNo(),-1);


  if(mpBoxLowBgFrame->GetNowCursorNo() < Savedata::BoxPokemon::TRAY_POKE_MAX){
    mpBoxBgLowDraw->ChangePokemonAnimStart(
      mSelectTray, mSelectCursor,
      mpBoxLowBgFrame->GetNowTrayNo(), mpBoxLowBgFrame->GetNowCursorNo(),
      mpBoxBgLowDraw->GetIconPutArrow( mpAppParam->nowDispTray, mSelectTray, mSelectCursor ) );
    mpBoxAppSetup->GetBoxSimpleAccesser()->Resort_AlignMember(mpAppParam->nowBattleTray,mpBoxLowBgFrame->GetNowCursorNo(),NULL , true);//GFNMcat[2119]
  }
  else{
    int ans=-1;
    mpBoxAppSetup->GetBoxSimpleAccesser()->Resort_AlignMember(mpAppParam->nowBattleTray,mpBoxLowBgFrame->GetNowCursorNo(),&ans, true);  //１つずつの移動を行う

    if(ans==-1){
      mpBoxBgLowDraw->ChangePokemonAnimStart(
        mSelectTray, mSelectCursor,
        mpBoxLowBgFrame->GetNowTrayNo(), mpBoxLowBgFrame->GetNowCursorNo(),
        mpBoxBgLowDraw->GetIconPutArrow( mpAppParam->nowDispTray, mSelectTray, mSelectCursor ) );
    }
    else{
      //置いた後手持ち移動で移動する場合
       mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonLandingAnimStart(
        mpBoxLowBgFrame->GetNowTrayNo(), mpBoxLowBgFrame->GetNowCursorNo(),
        mpBoxLowBgFrame->GetNowTrayNo(), ans+Savedata::BoxPokemon::TRAY_POKE_MAX,0);
    }
  }

  


  mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
  SetSubSeq( MM_SEQ_MOVE_SINGLE_M_AN );
//  mpBoxLowBgFrame->RequestChangePoke();
  mDecideCursor=mpBoxLowBgFrame->GetNowCursorNo();
	return BoxPokemonMoveFunction::MOVEERR_NONE;
}


void BoxResortState::CarryRange(void)
{

  bool retbool = mpBoxPokemonMoveFunction->SetCarryRange( &mRangeSelect,   mpBoxLowBgFrame->GetNowCursorNoPtr(), &mSelectCursor, mSelectTray);

  if(!retbool){
    mpBoxBgLowDraw->RangeSelectEnd();
    SetSubSeq( MM_SEQ_CLOSE );
    return;
  }
  if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
    SetSubSeq(MM_SEQ_CLOSE);
    return;
  }

  mpBoxLowBgFrame->CursorAction(-1);

  mpBoxBgLowDraw->CarryPokeIconRangeStart(mSelectTray,
                                          mRangeSelect.x ,mRangeSelect.y,
                                          mRangeSelect.width, mRangeSelect.height,
                                          mpBoxLowBgFrame->GetNowCursorNo(), (mRangeSelect.skip!=0) );
  Sound::PlaySE(SEQ_SE_CATCH);

  SetSubSeq( MM_SEQ_RANGE_SELECT_2ND );

  mpBoxBgLowDraw->SetUIListener(&mDInnerCarryRangePokeMoveListener);

  {
    mpBoxBgLowDraw->ResetFolderButton();
    mpBoxBgLowDraw->ResetBoxChangeCursor();
    mpBoxBgLowDraw->SetBoxBarInputEnable(false);
  }
  mpBoxBgLowDraw->SetCursorPos(mpBoxLowBgFrame->GetNowCursorNo(),-1);

}


void BoxResortState::RangeStart(void)
{

  Sound::PlaySE(SEQ_SE_DECIDE1);
  mRangeSelect.skip = mpBoxBgLowDraw->RangeSelectStart(mSelectCursor % 6 , mSelectCursor / 6); // 
  mpBoxBgLowDraw->SetUIListener(&mDInnerRangeMoveListener);
  mDInnerRangeMoveListener.SetThis(this);
  mpBoxLowBgFrame->SetSelectCursor();

  SetSubSeq( MM_SEQ_RANGE_SELECT );
}

void BoxResortState::SetRangePokemon(bool bTouch)
{
  mpBoxBgLowDraw->TemotiPokeIconAnimStart();//
  int ret = mpBoxPokemonMoveFunction->RangePokemonSet( mpBoxLowBgFrame->GetNowCursorNo(), mSelectCursor, &mRangeSelect, mpBoxLowBgFrame->GetNowTrayNo() );

  switch(ret){
  case BoxPokemonMoveFunction::MOVEERR_NONE:    //問題ない

    mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    SetSubSeq( MM_SEQ_MOVE_SINGLE_M_AN );
    mDecideCursor=mpBoxLowBgFrame->GetNowCursorNo();

    mpBoxBgLowDraw->SetCursorPos(mDecideCursor, -1);  //GFNMCat3924
    
    break;
  case BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE:   //置く場所が無い
     Sound::PlaySE( SEQ_SE_BEEP );
    if(bTouch){
      mpBoxBgLowDraw->CarryPokeIconRangeEnd(  mpAppParam->nowDispTray );
      SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS ); //キャンセル
    }
    else{
      SetSubSeq(MM_SEQ_RANGE_SELECT_2ND); //もどるだけ
    }
    return;
  case BoxPokemonMoveFunction::MOVEERR_TEAM_ENTRY:
    simpleWarrningMessage(msg_box_05_16, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK:  // チームロック
    simpleWarrningMessage(msg_box_05_14, 2,true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK_MEMBER://ロックされたメンバー
    simpleWarrningMessage(msg_box_05_09, 2,true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  case BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE: //たたかうポケモンがいなくなる=>たまごはとうろくできません
		simpleWarrningMessage(msg_box_02_26, 1, true);//GFNMCat:2101  1行に
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    mpBoxBgLowDraw->CarryPokeIconRangeEnd( mpAppParam->nowDispTray );
    return;
  default:
    GFL_ASSERT(0);
    break;
  }
}



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




