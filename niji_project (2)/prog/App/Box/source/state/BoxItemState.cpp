//=============================================================================
/**
 * @file	 BoxItemState.cpp
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

#include "BoxItemState.h"
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
BoxItemState::BoxItemState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxItemState::~BoxItemState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxItemState::Setup(void)
{

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpCoreTemp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  mDecideCursor = -1;
}


void BoxItemState::Release(void)
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
	MM_SEQ_ITEM_FIRST_DECIDE,
  MM_SEQ_ITEM_INIT,
  MM_SEQ_ITEM_LOOP,
  MM_SEQ_ITEM_LOOP_AFTER,
  MM_SEQ_ITEM_LOOP_AFTER_END,
  MM_SEQ_MOVE_ITEM_SHORTCUT_START,
  MM_SEQ_MOVE_ITEM,      
	MM_SEQ_MOVE_ITEM_TRAY_REV,
  MM_SEQ_MOVE_ITEM_M,
	MM_SEQ_MOVE_ITEM_EXEC,
  MM_SEQ_ANIM_LOOP,
  MM_SEQ_ANIM_LOOP1,
  MM_SEQ_ANIM_PAGE,
  MM_SEQ_ANIM_LOOP_RET,
  MM_SEQ_ANIM_WARRNING,
  MM_SEQ_ANIM_WARRNING_LOOP1,
  MM_SEQ_WARRNING_MSG,
  MM_SEQ_WARRNING_MSG_LOOP,
  MM_SEQ_CLOSE_RETURN_SELECT_POS,
  MM_SEQ_CLOSE_BEFORE,
  MM_SEQ_CLOSE_WAIT,
  MM_SEQ_CLOSE,
  MM_SEQ_NORMAL_MSG,
  MM_SEQ_NORMAL_MSG_LOOP,
  MM_SEQ_APP_JUMP,
  MM_SEQ_PAGECHANGE_ITEMMOVE,

};

//他のメニューから切り替えられた時
void BoxItemState::ModeStart(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq(MM_SEQ_ITEM_INIT);
}

//ショートカット持ち上げる
void BoxItemState::CarryModeStart(int tray,int cursor, bool bTouch)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  mbTouchStart = bTouch;
  SetUpdateStartSeq(MM_SEQ_MOVE_ITEM_SHORTCUT_START);
}

//最初のAボタンメニュー
void BoxItemState::FirstDecide(int tray,int cursor)
{
  //選んだ場所
  mSelectTray = tray;
  mSelectCursor = cursor;
  SetUpdateStartSeq( MM_SEQ_ITEM_FIRST_DECIDE );
}
  


u32 BoxItemState::StateFunc(void)
{
//	OHNO_PRINT("BoxItemState %d\n",GetSubSeq());

#if PM_DEBUG
  if(mDebugSeqBackup !=  GetSubSeq()){
    OHNO_PRINT("BoxItemState %d\n",GetSubSeq());
    mDebugSeqBackup=GetSubSeq();
  }
#endif


  
  switch(GetSubSeq()){

  case MM_SEQ_ITEM_FIRST_DECIDE:
    //選んだ場所
  //  mSelectTray = tray;
 //   mSelectCursor = cursor;
    mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, mpCoreTemp);  //アイテムを持っているか確認

    if(mpCoreTemp->GetItem() != ITEM_DUMMY_DATA)
    {  //からでない場合
      StartItemCarryMenu(mpCoreTemp->GetItem());
      SetSubSeq( MM_SEQ_ITEM_LOOP );
    }
    else if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){   //たまご
      simpleWarrningMessage(msg_box_02_12, 1, true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
    }
    else{ //空の場合
      mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_18);
      mpBoxBgLowDraw->AllPassiveStart();
      mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_11);
      mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_end, true, true);
      mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);  //fix
      Sound::PlaySE(SEQ_SE_DECIDE1);  //GFNMCat[1495]
      mpBoxBgLowDraw->LiftItemIconReady( mSelectTray, mSelectCursor,ITEM_DUMMY_DATA,false );
      mInnerItemHandYesNoListener.SetThis(this);
      mpMenuWindow->SetListener( &mInnerItemHandYesNoListener );
      SetSubSeq( MM_SEQ_ITEM_LOOP );
    }
    
    break;
    
    
  case MM_SEQ_ITEM_INIT:
    ItemHandStart();
    break;
  case MM_SEQ_ITEM_LOOP:
    break;

  case MM_SEQ_ITEM_LOOP_AFTER:
    mpBoxBgLowDraw->ResetTemotiTray();
    SetSubSeq(MM_SEQ_ITEM_LOOP_AFTER_END);
    break;
  case MM_SEQ_ITEM_LOOP_AFTER_END:
    if( !mpBoxBgLowDraw->IsTemochiFrameMove() ){
      SetSubSeq(MM_SEQ_MOVE_ITEM);
      mbTouchStart=false;
    }
    break;
  case MM_SEQ_MOVE_ITEM_SHORTCUT_START:
    mpBoxBgLowDraw->PassiveCancelButton();
    mpBoxBgLowDraw->PassiveUpperButton();
    mpBoxBgLowDraw->PassiveUnderButton();
    SetSubSeq(MM_SEQ_MOVE_ITEM);
    break;
    
  case MM_SEQ_MOVE_ITEM:   ///アイテムをもちあげる
    CarryUpItem(mbTouchStart);
    break;
  case MM_SEQ_MOVE_ITEM_TRAY_REV:
    if( !mpBoxBgLowDraw->IsTemochiFrameMove() ){
//      mpBoxBgLowDraw->LiftItemIconAfter();
//      mpBoxBgLowDraw->LiftItemIconStart(mSelectTray, mSelectCursor, itemno, false);
      Sound::PlaySE(SEQ_SE_CATCH);
      mpBoxBgLowDraw->SetUIListener(&mDInnerItemMoveListener);
      mDInnerItemMoveListener.SetThis(this);
      mpBoxBgLowDraw->SetBoxBarInputEnable(false);
      mpBoxBgLowDraw->PassiveUnderButton();
      SetSubSeq( MM_SEQ_MOVE_ITEM_M );
    }
    break;
  case MM_SEQ_MOVE_ITEM_M:
    break;

  case MM_SEQ_MOVE_ITEM_EXEC:
    switch(ChangeItem()){
    case ITEMEXEC_NOPOKEMON:
      SetSubSeq(MM_SEQ_ANIM_PAGE);
      break;
    case ITEMEXEC_TAMAGO:
      SetSubSeq(MM_SEQ_ANIM_WARRNING);
      break;
    case ITEMEXEC_TEAMLOCK:
      SetSubSeq(MM_SEQ_ANIM_WARRNING);
      break;
    case ITEMEXEC_OK:
      SetSubSeq(MM_SEQ_ANIM_LOOP);
      break;
    default:
      GFL_ASSERT(0);
      SetSubSeq(MM_SEQ_ANIM_LOOP);
      break;
    }
    break;
  case MM_SEQ_ANIM_LOOP:
    if(ItemIconAnimChk(0)){
      ItemIconAnimStart(1);
      SetSubSeq(MM_SEQ_ANIM_LOOP1);
      mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    }
    break;
  case MM_SEQ_ANIM_LOOP1:
    if(ItemIconAnimChk(1)){  ///インデックスの間違い  //NMCat1219
			//mpThis->mpBoxBgLowDraw->SetCursorPos(targetno, -1);

      mpBoxBgLowDraw->SetCursorPos(mpBoxLowBgFrame->GetNowCursorNo(), -1);
      
			mpBoxBgLowDraw->ResetFunc();
      mpBoxBgLowDraw->LiftItemIconEnd(  mpBoxLowBgFrame->GetNowTrayNo() );
      SetSubSeq(MM_SEQ_CLOSE);
    }
    break;
  case MM_SEQ_ANIM_PAGE:
    mpBoxLowBgFrame->PageChangeDirectSelectTray(); //GFNMCat1137
    SetSubSeq(MM_SEQ_ANIM_LOOP_RET);
    break;


  case MM_SEQ_ANIM_LOOP_RET:
    if(ItemIconAnimChk(0)){
      mpBoxBgLowDraw->SetCursorPos(mSelectCursor, -1);
//			mpBoxBgLowDraw->ResetFunc();
      mpBoxBgLowDraw->LiftItemIconEnd(  mpBoxLowBgFrame->GetNowTrayNo() );
      SetSubSeq(MM_SEQ_CLOSE);
    }
    break;

  case MM_SEQ_ANIM_WARRNING:
    mpBoxLowBgFrame->PageChangeDirectSelectTray(); //GFNMCat1137
    SetSubSeq(MM_SEQ_ANIM_WARRNING_LOOP1);
    break;
  case MM_SEQ_ANIM_WARRNING_LOOP1:
    if(ItemIconAnimChk(0)){
			mpBoxBgLowDraw->ResetFunc();
      mpBoxBgLowDraw->LiftItemIconEnd(  mpBoxLowBgFrame->GetNowTrayNo() );
      SetSubSeq(MM_SEQ_WARRNING_MSG);
    }
    break;
  case MM_SEQ_WARRNING_MSG:
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->WarrningMessageBefore(&mDInnerNoneListener); //GFNMCat[1609]
    {
	    BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
	    pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
	    pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
		}
    mpBoxLowBgFrame->PageChangeDirectSelectTray();
    mpBoxBgLowDraw->WarrningMessageAfter(mpAppParam->nowDispTray,  mSelectTray, mpAppParam->nowBattleTray); //GFNMCat[1609]
    
    SetSubSeq(MM_SEQ_WARRNING_MSG_LOOP);

    break;
  case MM_SEQ_WARRNING_MSG_LOOP:
    mpBoxBgLowDraw->ResetTemotiTray();
    SetSubSeq( MM_SEQ_CLOSE_WAIT );
    break;
  case MM_SEQ_CLOSE_WAIT:
    if( mpBoxBgLowDraw->IsTemochiFrameMove() == false )
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
      mpBoxBgLowDraw->LiftPokeIconClear();
      SetSubSeq( MM_SEQ_CLOSE );
    }
    break;

  case  MM_SEQ_CLOSE_RETURN_SELECT_POS:
    OHNO_PRINT("場所を元にもどします \n");
    mpBoxBgLowDraw->LiftPokeIconClear();
    mpBoxBgLowDraw->MessageLine2Close();
    mpBoxLowBgFrame->PageChangeDirectSelectTray();
    SetSubSeq( MM_SEQ_CLOSE );
    break;

  case MM_SEQ_CLOSE_BEFORE:
    if( !mpBoxBgLowDraw->IsTemochiFrameMove() ){
      MenuCancel();
      return BOXSTATE_RETURN;
    }
    break;

    
  case MM_SEQ_CLOSE:
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
    //    break;
    mpMenuWindow->EndMenu();
//    mpBoxBgLowDraw->MessageLine2Close();
    mpBoxBgLowDraw->ResetChatChangeButton();
    mpBoxBgLowDraw->LiftPokeIconClear();
  //  mpBoxBgLowDraw->ResetFunc();
    return BOXSTATE_RETURN;  //アイテムの仕事完了

  case MM_SEQ_NORMAL_MSG:
		{
	    BoxWarrningMessageState* pBoxWarrningMessageState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWarrningMessageState >( GetManager() );
	    pBoxWarrningMessageState->SetParam( mpBoxBgLowDraw );
	    pBoxWarrningMessageState->SimpleWarrningMessage(&mMessageParam);
		}
    mpBoxBgLowDraw->PassiveMainCursor(); //GFNMCat[1941]
    mpBoxBgLowDraw->PassiveCancelButton();

    
    SetSubSeq(MM_SEQ_NORMAL_MSG_LOOP);
    break;
  case MM_SEQ_NORMAL_MSG_LOOP:
//    mpBoxBgLowDraw->ResetChatChangeButton();
//    mpBoxBgLowDraw->LiftPokeIconClear();

    mpBoxBgLowDraw->ResetTemotiTray();
    SetSubSeq(MM_SEQ_CLOSE_BEFORE);
    break;
//    return BOXSTATE_RETURN;  //アイテムの仕事完了
    
  case MM_SEQ_APP_JUMP:
    return BOXSTATE_APP_JUMP;
  case MM_SEQ_PAGECHANGE_ITEMMOVE://GFNMCat[1621]
    mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
    if(!mpBoxBgLowDraw->IsPageFrameMove()){
      if(mpBoxBgLowDraw->GetTukamiMode() && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからキャンセル
        mpBoxBgLowDraw->LiftItemIconEnd(  mpAppParam->nowDispTray );
        SetSubSeq( MM_SEQ_CLOSE_RETURN_SELECT_POS );
			}
      else{
        SetSubSeq( MM_SEQ_MOVE_ITEM_M );  //スクロールが終わると先ほどのシーケンスへ復帰
        mpBoxBgLowDraw->SetUIListener(&mDInnerItemMoveListener);
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
 applib::frame::Result BoxItemState::InitFunc(void)
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
 applib::frame::Result BoxItemState::UpdateFunc(void)
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
 void BoxItemState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
 applib::frame::Result BoxItemState::EndFunc(void)
{
  End();
  Release();
  return applib::frame::RES_FINISH;
}




// アイテムを掴む
void BoxItemState::ItemHandStart(void)
{


  if(mpBoxPokemonMoveFunction->ThisPokeLockCheck(mSelectTray, mSelectCursor)){
    simpleWarrningMessage(msg_box_05_10, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;

  }

  mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, mpCoreTemp);
  int itemno = mpCoreTemp->GetItem();

  if(itemno != ITEM_DUMMY_DATA)
  {  //からでない場合


    //あずかるだけ
    mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_yes);  //はい
    mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_no, true);  //いいえ
    mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);
    SetSubSeq( MM_SEQ_ITEM_LOOP );

    mInnerItemSimpleRemoveYesNoListener.SetThis(this);
    mpMenuWindow->SetListener( &mInnerItemSimpleRemoveYesNoListener );
    mpBoxBgLowDraw->ExpandItem(0, itemno);

  // ピース
    if( !item::ITEM_CheckPiece(itemno) )
    {
      mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_09);
    }
    else{
      mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_24);
    }

  }
  else
  { //空の時
    if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){   //たまご
      simpleWarrningMessage(msg_box_02_12, 1,  true);
      SetSubSeq(MM_SEQ_WARRNING_MSG);
      return;
    }
    {
      //バッグへ
      mpAppParam->outJumpMode = App::Box::BOXJUMP_ITEM;
      mpAppParam->outTray = (u8)mpAppParam->nowDispTray;
      mpAppParam->outPos = (u8)mSelectCursor;
      SetSubSeq(MM_SEQ_APP_JUMP);
    }
  }
}

void BoxItemState::MenuCancel(void)
{

  mpBoxBgLowDraw->LiftPokeIconClear();

  mpBoxBgLowDraw->LiftPokeIconReadyReturn2(mSelectTray, mSelectCursor);
  mpBoxBgLowDraw->ResetLiftPokeIconClear(); // @fix niji GFNMCat[254]: LiftPokeIconClear()内のカーソル以外の処理を実行
  mpBoxBgLowDraw->MessageLine2Close();
  mpBoxBgLowDraw->LiftItemIconEnd(mSelectTray);
}



//もちあげる
void BoxItemState::CarryUpItem(bool bTouch)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();

  pml::pokepara::CoreParam* cp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  mpBoxPokemonMoveFunction->GetPokemon(mSelectTray, mSelectCursor, cp);
  bool isnull = cp->IsNull();
  int itemno = cp->GetItem();
  bool begg = cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG);

  GFL_SAFE_DELETE(cp);

  if(isnull){
    SetSubSeq( MM_SEQ_CLOSE );  //GFNMCat5358
    return;
  }

	if(begg){   //たまご
		simpleWarrningMessage(msg_box_02_12, 1, true);
		SetSubSeq(MM_SEQ_WARRNING_MSG);
		return;
	}


  if(mpBoxPokemonMoveFunction->ThisPokeLockCheck(mSelectTray, mSelectCursor)){
    simpleWarrningMessage(msg_box_05_10, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  }
  
  if(bTouch){
    if(itemno != ITEM_DUMMY_DATA){
      mpBoxBgLowDraw->LiftItemIconReady( mSelectTray, mSelectCursor,itemno,true );
      mpBoxBgLowDraw->PassiveCancelButton();
      mpBoxBgLowDraw->PassiveUnderButton();
    }
    else{
      Sound::PlaySE(SEQ_SE_DECIDE1);
      mpAppParam->outJumpMode = App::Box::BOXJUMP_ITEM;
      mpAppParam->outTray = (u8)mSelectTray;
      mpAppParam->outPos = (u8)mSelectCursor;
      SetSubSeq(MM_SEQ_APP_JUMP);
      return ;
    }
  }
  if(mpBoxAppSetup->GetMoveMode() == BOX_CHANGE_SELECTMODE_SHORTCUT){
    if(itemno == ITEM_DUMMY_DATA){
      Sound::PlaySE(SEQ_SE_DECIDE1);
      mpAppParam->outJumpMode = App::Box::BOXJUMP_ITEM;
      mpAppParam->outTray = (u8)mSelectTray;
      mpAppParam->outPos = (u8)mSelectCursor;
      SetSubSeq(MM_SEQ_APP_JUMP);
      return;
    }
  }

  if(itemno == ITEM_DUMMY_DATA){
    SetSubSeq( MM_SEQ_CLOSE );
    return;
  }

  if(bTouch && !mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチで進んできたが、タッチ状態で無い場合キャンセル
    OHNO_PRINT("確認b BoxItemState::Touch \n");
    SetSubSeq( MM_SEQ_CLOSE );
    return;
  }


  
  mpBoxBgLowDraw->LiftItemIconStart(mSelectTray, mSelectCursor, itemno, bTouch);
  Sound::PlaySE(SEQ_SE_CATCH);

  mpBoxBgLowDraw->SetUIListener(&mDInnerItemMoveListener);
  mDInnerItemMoveListener.SetThis(this);
  SetSubSeq( MM_SEQ_MOVE_ITEM_M );

  mpBoxBgLowDraw->LiftItemIconAfter(bTouch);
  
  mpBoxBgLowDraw->SetBoxBarInputEnable(false);

  mpBoxBgLowDraw->SetCursorPos(mpBoxLowBgFrame->GetNowCursorNo(),-1);

  
}



#if PM_DEBUG
void  BoxItemState::DInnerNoneListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerNoneListener\n");
}
#endif

//アイテムを抱えて動いている時用
#if PM_DEBUG
void  BoxItemState::DInnerItemMoveListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerItemMoveListener\n");
}
#endif


bool BoxItemState::DInnerItemMoveListener::OnMousePokemonCurpos(int no)
{
  return true;//タッチパネルキャッチに変更
}



void BoxItemState::DInnerItemMoveListener::OnNoTouchRelease(int pos)
{

  if(mpThis->GetSubSeq()==MM_SEQ_MOVE_ITEM_M ){
    mpThis->mpBoxLowBgFrame->SetNowCursorNo(-1);
    mpThis->SetSubSeq( MM_SEQ_MOVE_ITEM_EXEC );
  }
  else{
    mpThis->mpBoxBgLowDraw->LiftItemIconEnd(  mpThis->mSelectTray );
    mpThis->SetSubSeq( MM_SEQ_CLOSE );
  }

//  mpThis->SetSubSeq( MM_SEQ_CLOSE );
}

//--------------------------------------------------------------------------
/**
 * @brief   掴みに変更
 */
//--------------------------------------------------------------------------
void BoxItemState::DInnerItemMoveListener::OnMousePokemonHand(int x,int y, int cursorno)
{
  mpThis->mpBoxBgLowDraw->PassiveCancelButton();
}


void BoxItemState::DInnerItemMoveListener::OnMouseMenuOpen(int pos)
{
  mpThis->mpBoxLowBgFrame->SetNowCursorNo( pos );
  mpThis->SetSubSeq(MM_SEQ_MOVE_ITEM_EXEC);
}


//--------------------------------------------------------------------------
/**
 * @brief   配置   //ペイン同士で判定
 */
//--------------------------------------------------------------------------
bool BoxItemState::DInnerItemMoveListener::OnMousePokemonPut(int cursorno,int targetno)
{
  OHNO_PRINT("DInnerItemMoveListener\n");
  mpThis->mpBoxLowBgFrame->SetNowCursorNo( targetno );
  mpThis->SetSubSeq(MM_SEQ_MOVE_ITEM_EXEC);
  return true;
}
bool BoxItemState::DInnerItemMoveListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

void BoxItemState::DInnerItemMoveListener::OnButtonAction( int button_no )
{
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
    mpThis->mpBoxBgLowDraw->LiftItemIconEnd(  mpThis->mpAppParam->nowDispTray );

    mpThis->SetSubSeq( MM_SEQ_CLOSE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_DECIDE:
    mpThis->SetSubSeq( MM_SEQ_MOVE_ITEM_EXEC );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_UP:   //上
  case BoxLowBgDrawListener::LOWBG_ACTION_DOWN:   //下
  case BoxLowBgDrawListener::LOWBG_ACTION_LEFT:    //左
  case BoxLowBgDrawListener::LOWBG_ACTION_RIGHT:    //右
    mpThis->mpBoxLowBgFrame->CursorAction(button_no - BoxLowBgDrawListener::LOWBG_ACTION_UP);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE:    //LBUTTON
    mpThis->mpBoxLowBgFrame->PageChange(-1);
    mpThis->SetSubSeq( MM_SEQ_PAGECHANGE_ITEMMOVE );
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE:    //RBUTTON
    mpThis->mpBoxLowBgFrame->PageChange(1);
    mpThis->SetSubSeq( MM_SEQ_PAGECHANGE_ITEMMOVE );
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
    GFL_ASSERT(0);//ぱっしぶだからこない
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_INFO:
    break;


  }
}



//アイテムを戻すだけの時のはいいいえを受け取るインナークラス
void BoxItemState::InnerItemSimpleRemoveYesNoListener::SelectMenu( u32 id )
{
  if(id==0){  //バッグへ
    mpThis->RemoveItem();
  }
  else{  //いいえは元に戻す
    mpThis->mpMenuWindow->EndMenu();
    mpThis->mpBoxBgLowDraw->MessageLine2Close();
    mpThis->mpBoxBgLowDraw->ResetTemotiTray();
    mpThis->SetSubSeq(MM_SEQ_CLOSE_BEFORE);
//    mpThis->SetSubSeq(MM_SEQ_CLOSE);
  }
}
//アイテムを戻す時のはいいいえを受け取るインナークラス
void BoxItemState::InnerItemRemoveYesNoListener::SelectMenu( u32 id )
{
  mpThis->mpMenuWindow->EndMenu();
  mpThis->mpBoxBgLowDraw->MessageLine2Close();

  if(id==0){  // item移動>>アイテム移動モード
    mpThis->SetSubSeq(MM_SEQ_ITEM_LOOP_AFTER);//MM_SEQ_MOVE_ITEM);
  }
  else if (id==1){  //バッグへ
    mpThis->RemoveItem();
  }
  else{  //いいえは元に戻す
 //   mpThis->mpBoxBgLowDraw->LiftItemIconReadyReturn();
    mpThis->mpBoxBgLowDraw->ResetTemotiTray();
    mpThis->SetSubSeq(MM_SEQ_CLOSE_BEFORE);
  }
}

//アイテムを持たせる時のはいいいえを受け取るインナークラス
void BoxItemState::InnerItemHandYesNoListener::SelectMenu( u32 id )
{
  mpThis->mpMenuWindow->EndMenu();
  mpThis->mpBoxBgLowDraw->MessageLine2Close();

  if(id==0){  //はい選択
    if(!mpThis->mpBoxPokemonMoveFunction->ThisPokeLockCheck(mpThis->mSelectTray, mpThis->mSelectCursor)){
      mpThis->mpAppParam->outJumpMode = App::Box::BOXJUMP_ITEM;
      mpThis->mpAppParam->outTray = (u8)mpThis->mSelectTray;
      mpThis->mpAppParam->outPos = (u8)mpThis->mSelectCursor;
      mpThis->SetSubSeq(MM_SEQ_APP_JUMP);
    }
    else{
      mpThis->simpleWarrningMessage(msg_box_05_10, 2, true);
      mpThis->SetSubSeq(MM_SEQ_WARRNING_MSG);
    }
  }
  else{  //いいえは元に戻す
    mpThis->mpBoxBgLowDraw->ResetTemotiTray();
    mpThis->SetSubSeq(MM_SEQ_CLOSE_BEFORE);
  }
}




void BoxItemState::RemoveItem(void)
{
  int page = mSelectTray;
  int pos = mSelectCursor;

  if(mpBoxPokemonMoveFunction->ThisPokeLockCheck(page, pos)){
    simpleWarrningMessage(msg_box_05_10, 2, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    return;
  }

	gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);

  mpMenuWindow->EndMenu();
  mpBoxPokemonMoveFunction->GetPokemonPokemonParam(page, pos, pp);

  mpBoxBgLowDraw->ExpandItem(0, pp->GetItem());

  GFL_ASSERT(mpAppParam->pMyItem);

  int itemno = pp->GetItem();


  if( !item::ITEM_CheckPiece(itemno) && !mpAppParam->pMyItem->Add(itemno,1) ){   //Ｚ系以外でもどす時に失敗したら バッグが一杯
    simpleWarrningMessage(msg_box_02_11, 1, true);
    SetSubSeq(MM_SEQ_WARRNING_MSG);
    GFL_SAFE_DELETE(pp);
    return;
  }

  bool bfchange = mpBoxPokeParaFunc->CheckAndChangeFormChangePokemonItem(pp,  ITEM_DUMMY_DATA, mpAppParam->pZukanData);  //入れ替えるアイテムを渡してフォルム検査

  pp->SetItem(ITEM_DUMMY_DATA);  //けす

  mpBoxPokemonMoveFunction->SetPokemonPokemonParam(page, pos, pp); //いれ直す


  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(page, pos);

  mpBoxPokemonMoveFunction->TeamPackAll();

  if(!bfchange)
  {
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawState(page, pos);
  }
  else
  {
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawAll(page, pos);
  }
  mpBoxLowBgFrame->RequestChangePoke();

  OHNO_PRINT("ポケモンのHP %d\n",pp->GetHp());
  
  GFL_SAFE_DELETE(pp);

  // ピース
  if( !item::ITEM_CheckPiece(itemno) ){
    simpleWarrningMessage(msg_box_02_10, 1, false); //ばっぐにいれた
  }
  else{
    simpleWarrningMessage(msg_box_02_25, 1, false); //はずしました
  }



  SetSubSeq(MM_SEQ_NORMAL_MSG);
}



int BoxItemState::ChangeItem(void)
{
  mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);

  int selectpage=mSelectTray;
  int selectpos=mSelectCursor;
  int oldpage=mpBoxLowBgFrame->GetNowTrayNo();
  int oldpos=mpBoxLowBgFrame->GetNowCursorNo();

  mpBoxPokemonMoveFunction->GetPokemon(selectpage, selectpos, mpCoreTemp);

  if(mpCoreTemp->IsNull() || oldpos==-1){   //いない
    Sound::PlaySE( SEQ_SE_RELEASE );
		mpBoxLowBgFrame->SetNowCursorNo(mSelectCursor);//カーソル位置を戻しておく
		mpBoxBgLowDraw->ChangeItemAnimStart(selectpage, selectpos,selectpage, selectpos);
    return ITEMEXEC_NOPOKEMON;  //いないときはいないと帰す
  }
  if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){   //たまご
		mpBoxLowBgFrame->SetNowCursorNo(mSelectCursor);//カーソル位置を戻しておく
		mpBoxBgLowDraw->ChangeItemAnimStart(selectpage, selectpos,selectpage, selectpos);
    simpleWarrningMessage(msg_box_02_12, 1, true);
    return ITEMEXEC_TAMAGO;
  }
  if( mpAppParam->pBoxData->IsMeLock(selectpage,selectpos) ){  //ロック
		mpBoxBgLowDraw->ChangeItemAnimStart(selectpage, selectpos,selectpage, selectpos);
    simpleWarrningMessage(msg_box_05_10, 2,true);
    return ITEMEXEC_TEAMLOCK;
  }
  if( mpAppParam->pBoxData->IsMeLock(oldpage,oldpos) ){ //ロック
		mpBoxBgLowDraw->ChangeItemAnimStart(selectpage, selectpos,selectpage, selectpos);
    simpleWarrningMessage(msg_box_05_10, 2, true);
    return ITEMEXEC_TEAMLOCK;
  }

  int itemno = mpCoreTemp->GetItem();

  mpBoxPokemonMoveFunction->GetPokemon(oldpage, oldpos, mpCoreTemp);

  if(mpCoreTemp->IsNull() ){   //いない
    Sound::PlaySE( SEQ_SE_RELEASE );
		mpBoxLowBgFrame->SetNowCursorNo(mSelectCursor);//カーソル位置を戻しておく
    mpBoxBgLowDraw->ChangeItemAnimStart(selectpage, selectpos,selectpage, selectpos);
    return ITEMEXEC_NOPOKEMON;  //いないときはいないと帰す
  }
  if(mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){   //たまご
		mpBoxLowBgFrame->SetNowCursorNo(mSelectCursor);//カーソル位置を戻しておく
		mpBoxBgLowDraw->ChangeItemAnimStart(selectpage, selectpos,selectpage, selectpos);
    simpleWarrningMessage(msg_box_02_12, 1, true);
    return ITEMEXEC_TAMAGO;
  }
  Sound::PlaySE( SEQ_SE_RELEASE );

  int itemno2 = mpCoreTemp->GetItem();
  {
    gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
    pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);

    mpBoxPokemonMoveFunction->GetPokemonPokemonParam(oldpage, oldpos, pp);
    
    bool bfchange = mpBoxPokeParaFunc->CheckAndChangeFormChangePokemonItem(pp,  itemno, mpAppParam->pZukanData);  //入れ替えるアイテムを渡してフォルム検査
    pp->SetItem(itemno);
    mpBoxPokemonMoveFunction->SetPokemonPokemonParam(oldpage, oldpos, pp);

    SetItemIconAnim(0, bfchange, oldpage, oldpos,selectpage, selectpos,itemno);


    mpBoxPokemonMoveFunction->GetPokemonPokemonParam(selectpage, selectpos, pp);

    bfchange = mpBoxPokeParaFunc->CheckAndChangeFormChangePokemonItem(pp,  itemno2, mpAppParam->pZukanData);  //入れ替えるアイテムを渡してフォルム検査

    pp->SetItem(itemno2);
    mpBoxPokemonMoveFunction->SetPokemonPokemonParam(selectpage, selectpos, pp);

 //   OHNO_PRINT("ポケモンのHP %d\n",pp->GetHp());

    SetItemIconAnim(1, bfchange, selectpage, selectpos,oldpage, oldpos, itemno2);

    ItemIconAnimStart(0);

    GFL_SAFE_DELETE(pp);
  }
//  mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
//  SetSubSeq(MM_SEQ_ANIM_LOOP);

  mDecideCursor=mpBoxLowBgFrame->GetNowCursorNo();

  return ITEMEXEC_OK;
}







///アイテムを運ぶメニューの表示
void BoxItemState::StartItemCarryMenu(int itemno)
{
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_01);  //いどうする

  // ピース
  if( !item::ITEM_CheckPiece(itemno) )
  {
    mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_10);  //ばっくへ
  }
  else
  {
    mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_03_03);  //はずす
  }
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_end, true, true);   //bぼたんあり。、キャンセルマークあり
  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);  //fix

  mpBoxBgLowDraw->ExpandItem(0, itemno);
  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_17);

  Sound::PlaySE(SEQ_SE_DECIDE1);

  mpBoxBgLowDraw->LiftItemIconReady( mSelectTray, mSelectCursor,itemno,false );
  mpBoxBgLowDraw->PassiveBoxChangeCursor();

  mInnerItemRemoveYesNoListener.SetThis(this);
  mpMenuWindow->SetListener( &mInnerItemRemoveYesNoListener );
}



void BoxItemState::SetItemIconAnim(int no, bool bfchange, int page,int pos, int nextpage,int nextpos,int itemno)
{
  GFL_ASSERT(no<ITEM_ICON_ANIM_CONTROL);

  mItemIconAnimControl[no].page = page;
  mItemIconAnimControl[no].pos = pos;
  mItemIconAnimControl[no].nextpage = nextpage;
  mItemIconAnimControl[no].nextpos = nextpos;
  mItemIconAnimControl[no].bfchange = bfchange;
  mItemIconAnimControl[no].itemno = itemno;
}



void BoxItemState::ItemIconAnimStart(int no)
{
  int page = mItemIconAnimControl[no].page;
  int pos = mItemIconAnimControl[no].pos;
  int nextpage = mItemIconAnimControl[no].nextpage;
  int nextpos = mItemIconAnimControl[no].nextpos;
  int bfchange = mItemIconAnimControl[no].bfchange;
  int itemno = mItemIconAnimControl[no].itemno;

  if(itemno!=ITEM_DUMMY_DATA){
    if(no==0){
      mpBoxBgLowDraw->ChangeItemAnimStart(page, pos, page, pos);
    }
    else{
      mpBoxBgLowDraw->ChangeItemIcon(0,itemno);
      mpBoxBgLowDraw->ChangeItemAnimStart(
        nextpage, nextpos,
        page, pos,
        mpBoxBgLowDraw->GetIconPutArrow(mpAppParam->nowDispTray,page,pos) );
    }
  }
}



bool BoxItemState::ItemIconAnimChk(int no)
{
  int page = mItemIconAnimControl[no].page;
  int pos = mItemIconAnimControl[no].pos;
  int nextpage = mItemIconAnimControl[no].nextpage;
  int nextpos = mItemIconAnimControl[no].nextpos;
  int bfchange = mItemIconAnimControl[no].bfchange;

  mpBoxAppSetup->GetBoxSimpleAccesser()->AnimMove();
  if( mpBoxAppSetup->GetBoxSimpleAccesser()->IsAnimMoveEnd() ){

    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(page, pos);//変更
    mpBoxAppSetup->GetBoxSimpleAccesser()->RefreashPokemon(nextpage, nextpos);
      mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawState(nextpage, nextpos);

    if(!bfchange)
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawState(page, pos);
    }
    else
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawAll(page, pos);
      mpBoxPokemonMoveFunction->TeamPackAll();
    }
    return true;
  }
  return false;
}

  
GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




