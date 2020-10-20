//=============================================================================
/**
 * @file	 BoxBattleTeamState.cpp
 * @brief	 チーム選択の移動状態
 * @author k.ohno
 * @date	 2016.3.01
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

#include "BoxBattleTeamState.h"
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
BoxBattleTeamState::BoxBattleTeamState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxBattleTeamState::~BoxBattleTeamState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxBattleTeamState::Setup(void)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpCoreTemp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  mDecideCursor = -1;

  mDInnerNoneListener.SetThis(this);
  
}


void BoxBattleTeamState::Release(void)
{
  GFL_SAFE_DELETE(mpCoreTemp);
  if(mDecideCursor>=0){
    mpBoxBgLowDraw->SetCursorPos(mDecideCursor, -1);
    mpBoxLowBgFrame->SetNowCursorNo( mDecideCursor );
  }
  else{
    mpBoxBgLowDraw->SetCursorPos(mSelectCursor, -1);
    mpBoxLowBgFrame->SetNowCursorNo( mSelectCursor );
  }
}


enum{
  MM_SEQ_BATTLE_MODE_START,
  MM_SEQ_MOVE_SINGLE_READY,
  MM_SEQ_BATTLE_IN_BEFORE,
  MM_SEQ_BATTLE_IN,
  MM_SEQ_BATTLE_IN_LOOP_ANIM,
  MM_SEQ_BATTLE_IN_LOOP,
  MM_SEQ_BATTLE_REGIST,
  MM_SEQ_BATTLE_REMOVE,
  MM_SEQ_RANGE_START,
  MM_SEQ_RANGE_SELECT,
  MM_SEQ_MOVE_RANGE_CARRY,
  MM_SEQ_RANGE_SELECT_2ND,
  MM_SEQ_RANGE_SET,
  MM_SEQ_RANGE_SET_TOUCH,
  MM_SEQ_MOVE_SINGLE_MOVEEXEC,
  MM_SEQ_WARRNING_MSG,
  MM_SEQ_WARRNING_MSG_LOOP,
  MM_SEQ_CLOSE_BEFORE,
  MM_SEQ_CLOSE,
  MM_SEQ_CLOSE_RETURN_SELECT_POS,
  MM_SEQ_APP_JUMP,
  MM_SEQ_CHANGE_ITEM_STATE,
	MM_SEQ_CHANGE_MARK_STATE,
  MM_SEQ_CHANGE_STATUS_STATE,

  MM_SEQ_PAGECHANGE_POKEMOVE,
	MM_SEQ_PAGECHANGE_CARRYRANGE,

  
};

//-----------------------------公開クラス

///Aボタンで
void BoxBattleTeamState::ModeStart(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq(MM_SEQ_BATTLE_MODE_START);

}

//もちはこび
void BoxBattleTeamState::CarryStart(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq(MM_SEQ_MOVE_SINGLE_READY);
}



//範囲選択
void BoxBattleTeamState::RangeModeStart(int tray,int cursor)
{
  mSelectTray = tray;
  mSelectCursor = cursor;

  SetUpdateStartSeq( MM_SEQ_RANGE_START );
}

///---------------------------------------------

u32 BoxBattleTeamState::StateFunc(void)
{
  switch(GetSubSeq()){
  case MM_SEQ_BATTLE_MODE_START:
    FirstMenuStart();
    break;

  case MM_SEQ_BATTLE_IN_BEFORE:
    if( mpBoxBgLowDraw->IsTemochiFrameMove() == false )
    {
      
      SetSubSeq(MM_SEQ_BATTLE_IN);
    }
    break;
  case MM_SEQ_MOVE_SINGLE_READY:
    mpBoxBgLowDraw->LiftPokeIconShortcut(mSelectTray,mSelectCursor);
    mpBoxBgLowDraw->PassiveCancelButton();
    mpBoxBgLowDraw->PassiveUnderButton();
    SetSubSeq(MM_SEQ_BATTLE_IN);
    //break;
  case MM_SEQ_BATTLE_IN:
    ///ポケモンをもちあげる
    CarryupPokemon();
    break;
  case MM_SEQ_BATTLE_IN_LOOP_ANIM:
    if( mpBoxBgLowDraw->IsTemochiFrameMove() == false )
    {
      mpBoxBgLowDraw->SetVisibleBoxTrayChangeCursor(true);
      mpBoxBgLowDraw->ResetBoxTrayChangeCursor();
      SetSubSeq(MM_SEQ_BATTLE_IN_LOOP);
    }
    break;
  case MM_SEQ_BATTLE_IN_LOOP:
    break;
  case MM_SEQ_BATTLE_REGIST:
    switch(Regist()){
    case BoxPokemonMoveFunction::MOVEERR_NONE:    // 置けた
      SetSubSeq( MM_SEQ_CLOSE );
      break;
    default:  // 配置できない条件、BOXにもどりメッセージ表示
      SetSubSeq(MM_SEQ_WARRNING_MSG);
    }
    break;
  case MM_SEQ_BATTLE_REMOVE:
    Remove();
    break;
  case MM_SEQ_RANGE_START:
    RangeStart();
    break;
  case MM_SEQ_RANGE_SELECT:
    mpBoxLowBgFrame->RangeSelectStretch(&mRangeSelect);    //枠設定中
    break;
  case MM_SEQ_MOVE_RANGE_CARRY:
    CarryRange();   //複数持ち上げる
    break;

  case MM_SEQ_RANGE_SELECT_2ND:
    break;

  case MM_SEQ_RANGE_SET:
    SetRangePokemon(false);
    break;
  case MM_SEQ_RANGE_SET_TOUCH:
    SetRangePokemon(true);
    break;

  case MM_SEQ_MOVE_SINGLE_MOVEEXEC:
    PokeMoveExec();
    break;
  case MM_SEQ_WARRNING_MSG:
		mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->WarrningMessageBefore(&mDInnerNoneListener);
		{
	    BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
	    pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
	    pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
		}
    mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpBoxBgLowDraw->WarrningMessageAfter(mpAppParam->nowDispTray,  mSelectTray, mpAppParam->nowBattleTray);
    SetSubSeq(MM_SEQ_WARRNING_MSG_LOOP);
    break;
  case MM_SEQ_WARRNING_MSG_LOOP:
    return BOXSTATE_RETURN;  //アイテムの仕事完了

  case MM_SEQ_CLOSE_BEFORE:

    if( mpBoxBgLowDraw->IsTemochiFrameMove() == false )
    {
      MenuCancel();
      return BOXSTATE_RETURN;
    }
    break;
    
  case MM_SEQ_CLOSE:
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
    //    break;
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->MessageLine2Close();
    mpBoxBgLowDraw->ResetChatChangeButton();
    mpBoxBgLowDraw->LiftPokeIconClear();
//    mpBoxBgLowDraw->ResetFunc();
    return BOXSTATE_RETURN;  //仕事完了

  case  MM_SEQ_CLOSE_RETURN_SELECT_POS:
    mpBoxBgLowDraw->LiftPokeIconClear();
    mpBoxBgLowDraw->MessageLine2Close();
    mpBoxLowBgFrame->PageChangeDirectSelectTray();
    SetSubSeq( MM_SEQ_CLOSE );
    break;
//    return BOXSTATE_RETURN;  //仕事完了

  case MM_SEQ_APP_JUMP:
    return BOXSTATE_APP_JUMP;
  case MM_SEQ_CHANGE_ITEM_STATE:
    return BOXSTATE_ITEM;
	case MM_SEQ_CHANGE_MARK_STATE:
		return BOXSTATE_MARK;
  case MM_SEQ_CHANGE_STATUS_STATE:
		return BOXSTATE_STATUS;

  case MM_SEQ_PAGECHANGE_POKEMOVE:
    mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    if(!mpBoxBgLowDraw->IsTeamFrameMove()){
      if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
        OHNO_PRINT("MM_SEQ_MOVE_SINGLE_M MM_SEQ_CLOSE\n");
        mpBoxBgLowDraw->CarryPokeIconRangeEnd(  mpAppParam->nowDispTray );
        SetSubSeq(MM_SEQ_CLOSE_RETURN_SELECT_POS);
      }
      else{
        SetSubSeq( MM_SEQ_BATTLE_IN_LOOP );  //スクロールが終わると先ほどのシーケンスへ復帰
        mpBoxBgLowDraw->SetUIListener(&mDInnerPokeMoveListener);
      }
    }
    break;
  case MM_SEQ_PAGECHANGE_CARRYRANGE:
    mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    if(!mpBoxBgLowDraw->IsTeamFrameMove()){
      if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
        OHNO_PRINT("MM_SEQ_MOVE_SINGLE_M MM_SEQ_CLOSE\n");
        SetSubSeq(MM_SEQ_CLOSE);
      }
      else{
        SetSubSeq( MM_SEQ_RANGE_SELECT_2ND );  //スクロールが終わると先ほどのシーケンスへ復帰
        mpBoxBgLowDraw->SetUIListener(&mDInnerCarryRangePokeMoveListener);
      }
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
applib::frame::Result BoxBattleTeamState::InitFunc(void)
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
applib::frame::Result BoxBattleTeamState::UpdateFunc(void)
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
void BoxBattleTeamState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
applib::frame::Result BoxBattleTeamState::EndFunc(void)
{
  End();
  Release();
  return applib::frame::RES_FINISH;
}


//リスナー

#if PM_DEBUG
void  BoxBattleTeamState::DInnerNoneListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerNoneListener\n");
}
#endif


//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー バトルBOX選択
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxBattleTeamState::InnerBattleListener::SelectMenu( u32 id )
{
  switch(id){
  case 0:    //とうろくする  -->>どこに登録しますかへ
    mpThis->SetSubSeq(MM_SEQ_BATTLE_IN_BEFORE);
    mpThis->mpBoxBgLowDraw->ResetTemotiTray();

    mpThis->mpMenuWindow->EndMenu();
 //   mpThis->m_Slave = false;
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
//    mpThis->m_Slave = false;
//    mpThis->mpBoxBgLowDraw->LiftPokeIconReadyReturn(mpThis->mpBoxLowBgFrame->GetNowTrayNo(), mpThis->mpBoxLowBgFrame->GetNowCursorNo());
    mpThis->mpBoxBgLowDraw->ResetTemotiTray();
    mpThis->SetSubSeq( MM_SEQ_CLOSE_BEFORE );
//    mpThis->mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
    break;
  }

  mpThis->mpMenuWindow->EndMenu();

}


//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー バトルBOX選択 バトルボックス部分
 * @date    2015.03.10
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxBattleTeamState::InnerBattleTeamListener::SelectMenu( u32 id )
{
  switch(id){
  case 0:    //いどうする  -->>どこにいどうしますか
/*    if( mpThis->mpAppParam->pBoxData->IsTeamLock(mpThis->mpAppParam->nowBattleTray) ){
      mpThis->simpleWarrningMessage(msg_box_05_09, 2,  true);
      mpThis->SetSubSeq(MM_SEQ_WARRNING_MSG);
      
    }*/
//    else{
      mpThis->SetSubSeq(MM_SEQ_BATTLE_IN_BEFORE);
      mpThis->mpBoxBgLowDraw->ResetTemotiTray();
//      mpThis->m_Slave = false;
  //  }
    break;
  case 1:    //はずす
    if( mpThis->mpAppParam->pBoxData->IsTeamLock(mpThis->mpAppParam->nowBattleTray) ){
      mpThis->simpleWarrningMessage(msg_box_05_08, 2,  true);
      mpThis->SetSubSeq(MM_SEQ_WARRNING_MSG);
    }
    else{
      mpThis->mSelectTray = mpThis->mpBoxLowBgFrame->GetNowTrayNo();
      mpThis->mSelectCursor = mpThis->mpBoxLowBgFrame->GetNowCursorNo();
      mpThis->Remove();
    }
    break;
  case 2:    //つよさをみる
    mpThis->SetSubSeq(MM_SEQ_CHANGE_STATUS_STATE);
    break;
  default:
    GFL_ASSERT(0);
  case 3:  //やめる
//    mpThis->mpBoxBgLowDraw->LiftPokeIconReadyReturn(mpThis->mpBoxLowBgFrame->GetNowTrayNo(), mpThis->mpBoxLowBgFrame->GetNowCursorNo());
    mpThis->mpBoxBgLowDraw->ResetTemotiTray();
    mpThis->SetSubSeq( MM_SEQ_CLOSE_BEFORE );
    break;
  }
  mpThis->mpMenuWindow->EndMenu();

}



//ポケモンを抱えて動いている時用単数

#if PM_DEBUG
void  BoxBattleTeamState::DInnerPokeMoveListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerPokeMoveListener\n");
}
#endif
bool BoxBattleTeamState::DInnerPokeMoveListener::OnMousePokemonCurpos(int no)
{
  return true;//タッチパネルキャッチに変更
}

bool BoxBattleTeamState::DInnerPokeMoveListener::OnMousePokemonFirstTouch(int curpos)
{
	mpThis->mpBoxBgLowDraw->SetTukamiMode();
  return false;
}


//--------------------------------------------------------------------------
/**
 * @brief   掴みに変更
 */
//--------------------------------------------------------------------------
void BoxBattleTeamState::DInnerPokeMoveListener::OnMousePokemonHand(int x,int y, int cursorno)
{
  mpThis->mpBoxBgLowDraw->PassiveCancelButton();
}


//--------------------------------------------------------------------------
/**
 * @brief   配置   //ペイン同士で判定
 */
//--------------------------------------------------------------------------
bool BoxBattleTeamState::DInnerPokeMoveListener::OnMousePokemonPut(int cursorno,int targetno)
{
  OHNO_PRINT("DInnerPokeMoveListener %d\n",targetno);
  if(targetno < 0){//キャンセル
    mpThis->mpBoxBgLowDraw->LiftPokeIconEnd(  mpThis->mpAppParam->nowDispTray,mpThis->mpAppParam->nowBattleTray );
    Sound::PlaySE(SEQ_SE_RELEASE);
    mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpThis->SetSubSeq( MM_SEQ_CLOSE );
  }
  else{ //実行
    GFL_ASSERT(mpThis->mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM);

    if(targetno >= Savedata::BoxPokemon::TRAY_POKE_MAX){  //バトルトレイへの操作
      Sound::PlaySE(SEQ_SE_RELEASE);    // @fix GFNMCat[4491]: タッチで登録したとき
      mpThis->mpBoxLowBgFrame->SetNowCursorNo( targetno );
      mpThis->mpBoxBgLowDraw->SetCursorPos(targetno,-1);

      //今選んでる場所がバトルトレイの場合代入
      mpThis->SetSubSeq(MM_SEQ_BATTLE_REGIST);  //登録
    }
    else{
      if( targetno < Savedata::BoxPokemon::TRAY_POKE_MAX  && mpThis->mSelectCursor>=Savedata::BoxPokemon::TRAY_POKE_MAX ){//はずす
        mpThis->mpBoxLowBgFrame->SetNowCursorNo( targetno );
        mpThis->SetSubSeq(MM_SEQ_BATTLE_REMOVE);  //はずす
      }
      else if( targetno < Savedata::BoxPokemon::TRAY_POKE_MAX){//キャンセル扱いにする
        Sound::PlaySE(SEQ_SE_RELEASE);
        mpThis->mpBoxBgLowDraw->LiftPokeIconEnd(  mpThis->mpAppParam->nowDispTray,mpThis->mpAppParam->nowBattleTray );
        mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
        mpThis->SetSubSeq( MM_SEQ_CLOSE );
      }
      else{
        mpThis->SetSubSeq(MM_SEQ_BATTLE_REMOVE);  //はずす
      }
    }
  }
  return true;
}

bool BoxBattleTeamState::DInnerPokeMoveListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

void BoxBattleTeamState::DInnerPokeMoveListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
    mpThis->mpBoxBgLowDraw->LiftPokeIconEnd(  mpThis->mpAppParam->nowDispTray,mpThis->mpAppParam->nowBattleTray );
    Sound::PlaySE(SEQ_SE_RELEASE);
    mpThis->SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_DECIDE:
    mpThis->SetSubSeq( MM_SEQ_MOVE_SINGLE_MOVEEXEC );

    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_UP:   //上
  case BoxLowBgDrawListener::LOWBG_ACTION_DOWN:   //下
  case BoxLowBgDrawListener::LOWBG_ACTION_LEFT:    //左
  case BoxLowBgDrawListener::LOWBG_ACTION_RIGHT:    //右
    OHNO_PRINT("カーソル移動\n");
    mpThis->mpBoxLowBgFrame->CursorAction(button_no - BoxLowBgDrawListener::LOWBG_ACTION_UP);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE:    //LBUTTON
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE:    //RBUTTON
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_TEAM:    //LBUTTON
    mpThis->BattleInPageChange(-1);
    mpThis->SetSubSeq(MM_SEQ_PAGECHANGE_POKEMOVE);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_TEAM:    //RBUTTON
    mpThis->BattleInPageChange(1);
    mpThis->SetSubSeq(MM_SEQ_PAGECHANGE_POKEMOVE);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_SELECT_CHANGE:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_FOLDER:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_INFO:
    GFL_ASSERT(0);
    break;

  }
}


//複数選択時用

#if PM_DEBUG
void  BoxBattleTeamState::DInnerRangeMoveListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerRangeMoveListener\n");
}
#endif


//--------------------------------------------------------------------------
/**
 * @brief   ドラッグ中カーソル位置変更
 */
//--------------------------------------------------------------------------
void BoxBattleTeamState::DInnerRangeMoveListener::OnMouseDragCurpos(int cursorno)
{
  mpThis->mpBoxLowBgFrame->SetNowCursorNo( cursorno );
  GFL_ASSERT(cursorno>=0);
}


//--------------------------------------------------------------------------
/**
 * @brief   タッチの直後
 */
//--------------------------------------------------------------------------
bool BoxBattleTeamState::DInnerRangeMoveListener::OnMousePokemonFirstTouch(int cursorno)
{
	mpThis->mpBoxBgLowDraw->SetTukamiMode();
  mpThis->SetSubSeq( MM_SEQ_MOVE_RANGE_CARRY );
  return false;
}


bool BoxBattleTeamState::DInnerRangeMoveListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

void BoxBattleTeamState::DInnerRangeMoveListener::OnButtonAction( int button_no )
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
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE:    //RBUTTON
    GFL_ASSERT(0);
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
void  BoxBattleTeamState::DInnerCarryRangePokeMoveListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerCarryRangePokeMoveListener\n");
}
#endif
bool BoxBattleTeamState::DInnerCarryRangePokeMoveListener::OnMousePokemonPut(int cursor,int terger)
{
  OHNO_PRINT("DInnerCarryRangePokeMoveListener %d\n",terger);
  if(terger!=-1){
    mpThis->mpBoxLowBgFrame->SetNowCursorNo(terger);
  }
  GFL_ASSERT(mpThis->mpBoxLowBgFrame->GetNowCursorNo() >= 0);
  if(terger < 0){//キャンセル
    mpThis->mpBoxBgLowDraw->CarryPokeIconRangeEnd(  mpThis->mpAppParam->nowDispTray );
    mpThis->SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS );
  }
  else{
    mpThis->SetSubSeq( MM_SEQ_RANGE_SET_TOUCH );
  }
  return true;
}


bool BoxBattleTeamState::DInnerCarryRangePokeMoveListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   タッチの直後
 */
//--------------------------------------------------------------------------
bool BoxBattleTeamState::DInnerCarryRangePokeMoveListener::OnMousePokemonFirstTouch(int cursorno)
{
	mpThis->mpBoxBgLowDraw->SetTukamiMode();
  return false;
}

void BoxBattleTeamState::DInnerCarryRangePokeMoveListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
    mpThis->mpBoxBgLowDraw->CarryPokeIconRangeEnd(  mpThis->mpAppParam->nowDispTray );
    mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpThis->SetSubSeq( MM_SEQ_CLOSE );
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
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE:    //RBUTTON
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_TEAM:    //LBUTTON
    mpThis->BattleInPageChange(-1);
    mpThis->SetSubSeq(MM_SEQ_PAGECHANGE_CARRYRANGE);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_TEAM:    //RBUTTON
    mpThis->BattleInPageChange(1);
    mpThis->SetSubSeq(MM_SEQ_PAGECHANGE_CARRYRANGE);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_SELECT_CHANGE:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_FOLDER:
    GFL_ASSERT(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_INFO:
    GFL_ASSERT(0);
    break;

  }
}



///----------------------------------------------------------------------

void BoxBattleTeamState::FirstMenuStart(void)
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
  }

  if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
    SetSubSeq(MM_SEQ_CLOSE);
    return;
  }
  

  if(mSelectCursor < Savedata::BoxPokemon::TRAY_POKE_MAX){

    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_12);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_02);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_03);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_04);
    mInnerBattleListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerBattleListener );
  }
  else{
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_01);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_06);
    mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_01_02);
    mInnerBattleTeamListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerBattleTeamListener );
  }
  mpMenuWindow->AddItem(mpAppParam->pMsgData,msg_box_end,true,true);

  mpBoxBgLowDraw->PassiveBoxTrayChangeCursor();  //GFNMCat[1649]

  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);  //fix
  mpBoxBgLowDraw->LiftPokeIconReady( mSelectTray, mSelectCursor);
  Sound::PlaySE(SEQ_SE_DECIDE1);

  mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, mpCoreTemp);
  mpBoxBgLowDraw->ExpandPokemon( 0,mpCoreTemp );
  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_02);

  SetSubSeq(MM_SEQ_BATTLE_IN_LOOP);
}



void BoxBattleTeamState::MenuCancel(void)
{
  mpBoxBgLowDraw->LiftPokeIconReadyReturn2(mSelectTray, mSelectCursor);
  mpBoxBgLowDraw->ResetLiftPokeIconClear(); // @fix niji GFNMCat[254]: LiftPokeIconClear()内のカーソル以外の処理を実行
  mpBoxBgLowDraw->MessageLine2Close();
}



void BoxBattleTeamState::CarryupPokemon(void)
{

  if(mSelectCursor>=Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS){
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
    GFL_DELETE(pp);
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

  mpBoxBgLowDraw->LiftPokeIconStart(mSelectTray, mSelectCursor, true );
  Sound::PlaySE(SEQ_SE_CATCH);

  mpBoxLowBgFrame->CursorAction(-1);

  SetSubSeq( MM_SEQ_BATTLE_IN_LOOP_ANIM );

  mpBoxBgLowDraw->MessageLine2Close();

  mpBoxBgLowDraw->SetUIListener(&mDInnerPokeMoveListener);
  mDInnerPokeMoveListener.SetThis(this);

  mpBoxBgLowDraw->SetBoxBarInputEnable(false);
  mpBoxBgLowDraw->PassiveFolderButton();
  mpBoxBgLowDraw->PassiveBoxChangeCursor();
  mpBoxBgLowDraw->ResetBoxTrayChangeCursor();  //GFNMCat[1649]

  
}





//チーム登録
int BoxBattleTeamState::Regist(void)
{
  GFL_ASSERT(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM);

  mpBoxBgLowDraw->LiftPokeIconEnd(mpAppParam->nowDispTray,mpAppParam->nowBattleTray);
  mpBoxBgLowDraw->LiftPokeIconClear(); //浮いてるポケモンが消える

  mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, mpCoreTemp);
  if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){   //たまごは登録できない
    simpleWarrningMessage(msg_box_02_21, 1, true);
    return BoxPokemonMoveFunction::MOVEERR_EGG_PUT;
  }

  //登録したらマークが付くはずなので再度描画
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(mSelectTray, mSelectCursor);
  mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawState(mSelectTray,mSelectCursor);

  if(mpAppParam->pBoxData->IsTeamLock(mpAppParam->nowBattleTray)){  //チームがロックされてたら入れられない
    simpleWarrningMessage(msg_box_05_14, 2, true);
    return BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK;
  }

  int no = mpBoxPokemonMoveFunction->pokeMoveCheck(mSelectTray, mSelectCursor,
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
	}


  if( mpBoxLowBgFrame->GetNowCursorNo() >= Savedata::BoxPokemon::TRAY_POKE_MAX){

    if(mpBoxLowBgFrame->GetNowCursorNo() == mSelectCursor && mpAppParam->nowBattleTray==mSelectTray){
      OHNO_PRINT("一緒はなにもしない\n");
    }
    else{
      if(mSelectCursor >= Savedata::BoxPokemon::TRAY_POKE_MAX ){
        // 移動の場合 いれかえる

        mpBoxAppSetup->GetBoxSimpleAccesser()->ChangeTeamPokePos( mpAppParam->nowBattleTray, mpBoxLowBgFrame->GetNowCursorNo(),
                                                                  mSelectTray, mSelectCursor );
      }
      else{
        int i = mpBoxAppSetup->GetBoxSimpleAccesser()->SetTeamPokePos(  mpAppParam->nowBattleTray,
                                                                        mpBoxLowBgFrame->GetNowCursorNo() - Savedata::BoxPokemon::TRAY_POKE_MAX,  mSelectTray, mSelectCursor  );
        if(i < Savedata::BOX::TEAM_NUM_MAX){

          mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(mpAppParam->nowBattleTray, i + Savedata::BoxPokemon::TRAY_POKE_MAX);
          mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawAll(mpAppParam->nowBattleTray, i + Savedata::BoxPokemon::TRAY_POKE_MAX);
          OHNO_PRINT("重複は消します\n");
        }

        OHNO_PRINT("とうろくしました。 team = %d teamNo = %d /  box %d %d\n",
                   mpAppParam->nowBattleTray, mpBoxLowBgFrame->GetNowCursorNo(), mSelectTray,mSelectCursor );
        mpBoxAppSetup->GetBoxSimpleAccesser()->SetTeamIcon(mSelectTray, mSelectCursor, mpAppParam->nowBattleTray , true);

      }
    }
  }

  //マークが色々変更に
  mpBoxBgLowDraw->DrawPokeIconStateAll(mpAppParam->nowDispTray, mpAppParam->nowBattleTray);

  //ポケモンの絵が変わるのでDrawAll
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(mSelectTray, mSelectCursor);
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(mpAppParam->nowBattleTray, mpBoxLowBgFrame->GetNowCursorNo());
  mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawAll(mSelectTray, mSelectCursor);
  mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawAll(mpAppParam->nowBattleTray, mpBoxLowBgFrame->GetNowCursorNo());

  //詰めて配置
  mpBoxPokemonMoveFunction->TeamPackAll();

  mDecideCursor=mpBoxLowBgFrame->GetNowCursorNo();
  return BoxPokemonMoveFunction::MOVEERR_NONE;
}


//チームから外す
void BoxBattleTeamState::Remove(void)
{
  if(mSelectCursor < Savedata::BoxPokemon::TRAY_POKE_MAX){  //はずす必要が無い
    mpBoxBgLowDraw->LiftPokeIconReadyReturn2( mSelectTray, mSelectCursor);
//    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
    //    m_Slave = false;
    Sound::PlaySE( SEQ_SE_BEEP );
    SetSubSeq( MM_SEQ_CLOSE );
    return;
  }

  mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, mpCoreTemp);
  if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){   //たまごは登録できない
 //   mpBoxBgLowDraw->LiftPokeIconReadyReturn( mSelectTray, mSelectCursor);
 //   mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
    simpleWarrningMessage(msg_box_02_21, 1, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  }
  if( mpAppParam->pBoxData->IsTeamLock(mSelectTray) ){
//    mpBoxBgLowDraw->LiftPokeIconReadyReturn( mSelectTray, mSelectCursor);
//    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
    simpleWarrningMessage(msg_box_05_08, 2, true  );
      SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  }

  Sound::PlaySE(SEQ_SE_RELEASE);  // @fix GFNMCat[4491]: チームから外したとき

  RemoveTeam( mSelectTray, mSelectCursor);
  mpBoxPokemonMoveFunction->TeamPackAll();

//  mpBoxLowBgFrame->RequestChangePoke();

  mpBoxBgLowDraw->ResetTemotiTray();
  SetSubSeq( MM_SEQ_CLOSE_BEFORE );

  //SetSubSeq( MM_SEQ_CLOSE );
  mDecideCursor=mpBoxLowBgFrame->GetNowCursorNo();

}


//チームから外す
void BoxBattleTeamState::RemoveTeam(int page,int pos)
{
  mpBoxAppSetup->GetBoxSimpleAccesser()->ResetPokemonIconDrawData();
  mpBoxAppSetup->GetBoxSimpleAccesser()->RemoveTeam(page, pos - Savedata::BoxPokemon::TRAY_POKE_MAX);
}



//ページ切り替え
int BoxBattleTeamState::getBattleChange(int changenum)
{
  int next = mpAppParam->nowBattleTray;
  int max = Savedata::BOX::TEAM_MAX;

  next += changenum;
  if(next < 0){
    next = max - 1;
  }
  else if(next >= max){
    next = 0;
  }
  return next;
}

//ボックスのページ切り替え
void BoxBattleTeamState::BattleInPageChange(int changenum)
{
  int next = getBattleChange(changenum);

  mpBoxBgLowDraw->DrawTeamAnim(changenum);
  mpBoxBgLowDraw->DrawTeamNameBefore(next);
  mpBoxBgLowDraw->DrawTeamIconBefore(next);
}


//移動決定
void BoxBattleTeamState::PokeMoveExec(void)
{
  mpBoxBgLowDraw->SetUIListener(NULL);

  if(mpBoxLowBgFrame->GetNowCursorNo() >= Savedata::BoxPokemon::TRAY_POKE_MAX){
    Sound::PlaySE(SEQ_SE_RELEASE);    // @fix GFNMCat[4491]: キー操作でチームに登録したとき
    SetSubSeq(MM_SEQ_BATTLE_REGIST);  //登録またはいれかえ
  }
  else{
    if( mpBoxLowBgFrame->GetNowCursorNo() < 0){
      OHNO_PRINT("キャンセル扱い\n");
      Sound::PlaySE(SEQ_SE_RELEASE);
      mpBoxBgLowDraw->LiftPokeIconEnd(  mpAppParam->nowDispTray,mpAppParam->nowBattleTray );
      SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS );
    }
    else{
      //はずす処理
      mpBoxBgLowDraw->SetCursorPos(  mpBoxLowBgFrame->GetNowCursorNo(), -1);
      SetSubSeq(MM_SEQ_BATTLE_REMOVE);  //はずす
    }
  }
}

void BoxBattleTeamState::CarryRange(void)
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
  mDInnerCarryRangePokeMoveListener.SetThis(this);

  mpBoxBgLowDraw->SetBoxBarInputEnable(false);
  mpBoxBgLowDraw->ResetBoxTrayChangeCursor();//GFNMCat[1906]


  if(mpBoxAppSetup->GetSelectMode() != BOX_CHANGE_EDITTYPE_BATTLE_TEAM)
  {  //バトルモード以外の時は
    mpBoxBgLowDraw->ResetFolderButton();
    mpBoxBgLowDraw->ResetBoxChangeCursor();
  }
  mpBoxBgLowDraw->SetCursorPos(mpBoxLowBgFrame->GetNowCursorNo(),-1);

}



void BoxBattleTeamState::SetRangePokemon(bool bTouch)
{
  int ret = mpBoxPokemonMoveFunction->RangePokemonSet( mpBoxLowBgFrame->GetNowCursorNo(), mSelectCursor, &mRangeSelect, mpBoxLowBgFrame->GetNowTrayNo() );

  switch(ret){
  case BoxPokemonMoveFunction::MOVEERR_NONE:    //問題ない

    mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonUseStock();  ///fix

    SetSubSeq( MM_SEQ_CLOSE );
    mpBoxBgLowDraw->CarryPokeIconRangeEnd( mpAppParam->nowDispTray );
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
  case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK:  // チームロック
    simpleWarrningMessage(msg_box_05_14, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  case BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE: //たたかうポケモンがいなくなる>>> たまごがいる
    simpleWarrningMessage(msg_box_02_21, 1, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK_MEMBER://ロックされたメンバー
    simpleWarrningMessage(msg_box_05_09, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK_MEMBER_REMOVE: //ロックされたメンバー
    simpleWarrningMessage(msg_box_05_08, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}


void BoxBattleTeamState::RangeStart(void)
{

  Sound::PlaySE(SEQ_SE_DECIDE1);
  mRangeSelect.skip = mpBoxBgLowDraw->RangeSelectStart(mSelectCursor % 6 , mSelectCursor / 6); // 
  mpBoxBgLowDraw->SetUIListener(&mDInnerRangeMoveListener);
  mDInnerRangeMoveListener.SetThis(this);
  mpBoxLowBgFrame->SetSelectCursor();
  mpBoxBgLowDraw->PassiveBoxTrayChangeCursor();//GFNMCat[1906]
  
  SetSubSeq( MM_SEQ_RANGE_SELECT );
}



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




