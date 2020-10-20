//=============================================================================
/**
 * @file	 BoxAllBoxState.cpp
 * @brief	 全体ボックスの移動状態
 * @author k.ohno
 * @date	 2016.2.29
 */
//=============================================================================
#pragma once


#include "../BoxPokepara.h"
#include "../BoxSimpleAccesser.h"
#include "../BoxLowBgFrame.h"
#include "../BoxLowBgDraw.h"
#include "../BoxLowFolderDraw.h"

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

#include "BoxAllBoxState.h"
#include "BoxWarrningMessageState.h"

#include "../function/BoxPokemonMoveFunction.h"


#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

#if PM_DEBUG
static BoxAllBoxState* mpStaticBoxAllBoxState;
#endif


//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
BoxAllBoxState::BoxAllBoxState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxAllBoxState::~BoxAllBoxState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxAllBoxState::Setup(void)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpCoreTemp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  mDInnerFolderNoneListener.SetThis(this);
  mDInnerFolderViewListener.SetThis(this);
  mDInnerFolderViewRangeListener.SetThis(this);
  mpBoxLowFolderDraw->SetUIListener(&mDInnerFolderNoneListener);

#if PM_DEBUG
  mpStaticBoxAllBoxState = this;
#endif

}


void BoxAllBoxState::Release(void)
{
  GFL_SAFE_DELETE(mpCoreTemp);
}


enum{
  MM_SEQ_FOLDER_INIT_RE,
  MM_SEQ_FOLDER_INIT,
  MM_SEQ_FOLDER_FADE_WAIT,
	MM_SEQ_FOLDER_CATCH_INIT,
  MM_SEQ_FOLDER_CATCH_INIT2,  // kawa_box_modify  使用中のペインのテクスチャを貼り替えるので処理を2フレームに分ける
  MM_SEQ_FOLDER_LOOP,
  MM_SEQ_FOLDER_RANGE_INIT,
  MM_SEQ_FOLDER_RANGE_LOOP,
  MM_SEQ_FOLDER_DROP,
  MM_SEQ_FOLDER_DROP_TOUCH,
  MM_SEQ_WARRNING_TEAM,
  MM_SEQ_WARRNING_BATTLE,
  MM_SEQ_CLOSE_BOXIN,
  MM_SEQ_CLOSE,
	MM_SEQ_CLOSE_SINGLE,
	MM_SEQ_CLOSE_RANGE,
  MM_SEQ_APP_JUMP,
  MM_SEQ_APP_JUMP_FADEEND,
};

//検索から戻ってきた
void BoxAllBoxState::FolderStartRe(int foldercursor,int tray,int cursor)
{
  mbFolderPokeCatch=false;
  mSelectPagePos=tray;
  mSelectCursorPos=cursor;
  mFolderCursorPos=foldercursor;
  SetUpdateStartSeq(MM_SEQ_FOLDER_INIT_RE);
}


//全体ボックスボタンから呼び出し
void BoxAllBoxState::FolderStart(int foldercursor,int tray,int cursor)
{
  mbFolderPokeCatch=false;
  mSelectPagePos=tray;
  mSelectCursorPos=cursor;
  mFolderCursorPos=foldercursor;
  SetUpdateStartSeq(MM_SEQ_FOLDER_INIT);
}

//ポケモンを持った状態
void BoxAllBoxState::FolderPokemonMoveStart(int tray,int cursor)
{
  mbFolderPokeCatch=true;
  mSelectPagePos=tray;
  mSelectCursorPos=cursor;
  mFolderCursorPos=0;
  SetUpdateStartSeq(MM_SEQ_FOLDER_CATCH_INIT);
}


//ポケモンを範囲で持った状態
void BoxAllBoxState::FolderRangeMoveStart(int tray,int cursor,const RangeData* pRange)
{
  mbFolderPokeCatch=true;
  mSelectPagePos=tray;
  mSelectCursorPos=cursor;
  mFolderCursorPos=0;
  SetUpdateStartSeq(MM_SEQ_FOLDER_RANGE_INIT);
  gfl2::std::MemCopy(pRange ,&mRangeSelect,sizeof(RangeData));
}




u32 BoxAllBoxState::StateFunc(void)
{
#if PM_DEBUG
  if(mDebugSeqBackup !=  GetSubSeq()){
    OHNO_PRINT("BoxAllBoxState %d\n",GetSubSeq());
    mDebugSeqBackup = GetSubSeq();
  }
#endif

  
  switch(GetSubSeq()){
  case MM_SEQ_FOLDER_INIT_RE:
    FolderAnimInitNoPoke(true);
    SetSubSeq( MM_SEQ_FOLDER_FADE_WAIT );
    break;
  case MM_SEQ_FOLDER_INIT:
    FolderAnimInitNoPoke();
    SetSubSeq( MM_SEQ_FOLDER_FADE_WAIT );
    break;
  case MM_SEQ_FOLDER_FADE_WAIT:
    if(!mpBoxLowBgFrame->IsFadeEnd()){
      OHNO_PRINT("fadewait\n");
      break;
    }
    mpBoxLowFolderDraw->SetUIListener(&mDInnerFolderViewListener);
    mpBoxLowFolderDraw->SetCursorVisible(true);
    SetSubSeq( MM_SEQ_FOLDER_LOOP );
    break;
  case MM_SEQ_FOLDER_CATCH_INIT:
    SetSubSeq( MM_SEQ_FOLDER_CATCH_INIT2 );
    FolderAnimInitCatch();
    break;
  case MM_SEQ_FOLDER_CATCH_INIT2:  // kawa_box_modify  使用中のペインのテクスチャを貼り替えるので処理を2フレームに分ける
    SetSubSeq( MM_SEQ_FOLDER_LOOP );
    FolderAnimInitCatch2();
    break;
  case MM_SEQ_FOLDER_LOOP:
    break;
  case MM_SEQ_FOLDER_RANGE_INIT:
    SetSubSeq( MM_SEQ_FOLDER_RANGE_LOOP );
    FolderAnimInitRange();
    break;
  case MM_SEQ_FOLDER_RANGE_LOOP:
    break;
  case MM_SEQ_FOLDER_DROP:
    //ポケモンを持っているとき
    if((mbFolderPokeCatch) && (mFolderCursorPos!=0)){
      SetSubSeq( MM_SEQ_FOLDER_RANGE_LOOP );
      PutBoxRangeFunc(mSelectPagePos, mSelectCursorPos, mFolderCursorPos-1); //置けるかどうか内部で見ている
    }
    else{
      //ポケモン持ってないとき
      SetSubSeq(MM_SEQ_CLOSE);
      if(mFolderCursorPos!=0){
        mpBoxLowBgFrame->PageChangeDirect(mFolderCursorPos-1);
      }
    }
    break;
  case MM_SEQ_FOLDER_DROP_TOUCH:
    //カーソル戻す
    mpBoxLowFolderDraw->SetCursorVisible(true);
    {
      int retno = BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE;
  
      if((mTempCursorNo==-1)||(mTempCursorNo==0)){
      }
      else{
        SetSubSeq(MM_SEQ_CLOSE);
        retno = PutBoxRangeFunc(mSelectPagePos, mSelectCursorPos, mTempCursorNo - 1); //置けるかどうか内部で見ている
      }
      switch(retno){
      case BoxPokemonMoveFunction::MOVEERR_NONE:
        SetSubSeq(MM_SEQ_FOLDER_RANGE_LOOP);
        break;
      case BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE:
        mpBoxBgLowDraw->RangeSelectEnd();
        SetSubSeq(MM_SEQ_CLOSE);
        mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
        mpBoxLowBgFrame->PageChangeDirectSelectTray();
        break;
      case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK:   //ロック
      case BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE:    //戻る処理が必要 バトルポケモンいなくなる
        break;
      }
    }
    break;
  case MM_SEQ_WARRNING_BATTLE:
		mbFolderPokeCatch=false;  //
    mpBoxBgLowDraw->ResetRangeNum();
    mpBoxLowFolderDraw->LiftPokeIconEnd();
		mpBoxLowBgFrame->BoxBgLowDrawLiftPokeIconEnd();
    mpBoxLowBgFrame->PageChangeDirectSelectTray();
    DrawEnd();
    return BOXSTATE_WARRNING_BATTLE;  //エラーメッセージの後はポケモン選択状態でなくなって最後まで戻る
 //   break;
  case MM_SEQ_WARRNING_TEAM:
		mbFolderPokeCatch=false;  //
    mpBoxBgLowDraw->ResetRangeNum();
    mpBoxLowFolderDraw->LiftPokeIconEnd();
		mpBoxLowBgFrame->BoxBgLowDrawLiftPokeIconEnd();
    mpBoxLowBgFrame->PageChangeDirectSelectTray();
    DrawEnd();
    return BOXSTATE_WARRNING_TEAM;  //エラーメッセージの後はポケモン選択状態でなくなって最後まで戻る

  case MM_SEQ_CLOSE_BOXIN:
    if(mFolderCursorPos!=0){
      mpBoxLowBgFrame->PageChangeDirect(mFolderCursorPos-1);
    }
    else{
      mpBoxLowBgFrame->PageChangeDirectSelectTray();
    }
    Sound::PlaySE(SEQ_SE_DECIDE1);  //GFNMCat[1697]
    mpBoxBgLowDraw->CarryPokeIconRangeEnd(false);
    mpBoxBgLowDraw->PassiveMainCursor();
    mpBoxBgLowDraw->ResetRangeNum();
    SetSubSeq( MM_SEQ_CLOSE );
    break;
    
  case MM_SEQ_CLOSE:
		mbFolderPokeCatch=false;  //
    DrawEnd();
    return BOXSTATE_RETURN;  //仕事完了

  case MM_SEQ_CLOSE_SINGLE:

    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->MessageLine2Close();
    return BOXSTATE_RETURN_POKEMON_NORMAL;  //単体で持ったまま完了

  case MM_SEQ_CLOSE_RANGE:
    mpMenuWindow->EndMenu();
    mpBoxBgLowDraw->MessageLine2Close();
    return BOXSTATE_RETURN_POKEMON_RANGE;  //範囲で持ったまま完了

  case MM_SEQ_APP_JUMP:
    mpBoxLowFolderDraw->SetUIListener(&mDInnerFolderNoneListener);//NMCat[3575]
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );
    SetSubSeq( MM_SEQ_APP_JUMP_FADEEND );
    break;
  case MM_SEQ_APP_JUMP_FADEEND:
    if(GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE)){
      return BOXSTATE_APP_JUMP;
    }
    break;
  default:
    break;
  }
  mpBoxLowFolderDraw->Update();

  return BOXSTATE_NONE;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BoxAllBoxState::InitFunc(void)
{
  Setup();
  return applib::frame::RES_NEXT;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BoxAllBoxState::UpdateFunc(void)
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
    return applib::frame::RES_NEXT;
  }
  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void BoxAllBoxState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  mpBoxLowFolderDraw->DrawTree(no);
}
//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result BoxAllBoxState::EndFunc(void)
{
  End();
  Release();
  FolderModeEnd();
  return applib::frame::RES_NEXT;
}




void BoxAllBoxState::AllBoxLocalSetup(  BoxLowFolderDraw* pBoxLowFolderDraw )
{
  GFL_ASSERT(pBoxLowFolderDraw);
  mpBoxLowFolderDraw = pBoxLowFolderDraw;
}




//フォルダー画面用

//--------------------------------------------------------------------------
/**
 * @brief   タッチ開始
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewListener::TouchFolder( int index )
{
  mpThis->mFolderCursorPosBack = mpThis->mFolderCursorPos;
  //上画面がきりかわる
  mpThis->mFolderCursorPos = index;
  OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
  //カーソルが消える
  mpThis->mpBoxLowFolderDraw->SetCursorVisible(false);


}
//--------------------------------------------------------------------------
/**
 * @brief   つかみ開始
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewListener::OnFolderHand( int x, int y, int index )
{
  mpThis->mFolderCursorPos = index;
  OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
  mpThis->FolderCursorAction(-1);

  if(!mpThis->mbFolderPokeCatch){//ポケモン掴んで無い場合
  
    Sound::PlaySE(SEQ_SE_CATCH);
    //フォルダーを消し
    mpThis->mpBoxLowFolderDraw->SetFolderActive(index,false);
    //移動用フォルダーを出す
    mpThis->mpBoxLowFolderDraw->FolderIconMoveActive(index,true);

    mpThis->mpBoxLowFolderDraw->SetSearchButtonPassive();
    mpThis->mpBoxLowFolderDraw->SetBoxListButtonPassive();
  }
  else{
    mpThis->mpBoxLowFolderDraw->SetTukamiMode();
    mpThis->mpBoxBgLowDraw->SetTukamiMode();
    mpThis->mpBoxBgLowDraw->PassiveMainCursor(); //GFNMCat1754
    mpThis->mpBoxBgLowDraw->CarryRangeSelectVisible(true);
    mpThis->mpBoxBgLowDraw->ResetRangeNum();
    OHNO_PRINT("つかみモード\n");
  }
  
}
//--------------------------------------------------------------------------
/**
 * @brief   フォルダー移動
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewListener::MoveFolder( int index,int selectindex )
{
  //フォルダーもどす
  mpThis->mpBoxLowFolderDraw->SetFolderActive(selectindex,true);
  //カーソル戻す
  mpThis->mpBoxLowFolderDraw->SetCursorVisible(true);
  //移動用フォルダーもどす
  mpThis->mpBoxLowFolderDraw->FolderIconMoveActive(index,false);

  if(!mpThis->mpAppParam->trade.bOn)
  {
    mpThis->mpBoxLowFolderDraw->SetSearchButtonActive();
  }
	mpThis->mpBoxLowFolderDraw->SetBoxListButtonActive();

  Sound::PlaySE(SEQ_SE_RELEASE);

  if(index==-1 || index == 0){
    return;
  }
  if(index==selectindex){
    return;
  }
  mpThis->mFolderCursorPos = index;
  OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
  mpThis->FolderCursorAction(-1);

  OHNO_PRINT("交換した場所 %d %d\n",index-1,selectindex-1);
  mpThis->mpAppParam->pBoxData->ChangeTray(index-1,selectindex-1, mpThis->mpAppHeap->GetDeviceHeap());
  mpThis->mpAppParam->pBoxPokemon->ChangeTray(index-1,selectindex-1, mpThis->mpAppHeap->GetDeviceHeap());
  mpThis->mpBoxAppSetup->GetBoxSimpleAccesser()->ChangeTray(index-1,selectindex-1);

  mpThis->MathBoxPokeNum();  //再構成

}

//--------------------------------------------------------------------------
/**
 * @brief   フォルダーにポケモンを入れる
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewListener::DropFolder( int index )
{
  //カーソル戻す
  mpThis->mpBoxLowFolderDraw->SetCursorVisible(true);
  //移動用フォルダーもどす
  mpThis->mpBoxLowFolderDraw->FolderIconMoveActive(index,false);

  if(index==-1 || index==0)
  {
    //キャンセル
    mpThis->mpBoxLowFolderDraw->LiftPokeIconEnd();

    mpThis->SetSubSeq(MM_SEQ_CLOSE);
    Sound::PlaySE(SEQ_SE_RELEASE);

    mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
    //    mpThis->PageChangeDirect(mpThis->mpAppParam->nowDispTray);
    mpThis->mpBoxLowBgFrame->BoxBgLowDrawLiftPokeIconEnd();
    return;
  }
  //
  OHNO_PRINT("はいち %d %d\n",mpThis->mSelectPagePos, mpThis->mSelectCursorPos);


  switch(mpThis->PutBox( mpThis->mSelectPagePos, mpThis->mSelectCursorPos, index - 1 )){
	case BoxPokemonMoveFunction::MOVEERR_NONE:
    // 置けた
    mpThis->mpBoxLowFolderDraw->LiftPokeIconEnd();
    mpThis->mbFolderPokeCatch=false;
    Sound::PlaySE(SEQ_SE_RELEASE);

    mpThis->mFolderCursorPos = index;
    OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
    mpThis->FolderCursorAction(-1);
    mpThis->MathBoxPokeNum();  //再構成
    break;
	case BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE:
    //キャンセル
    mpThis->mpBoxLowFolderDraw->LiftPokeIconEnd();
    mpThis->mpBoxBgLowDraw->LiftPokeIconClear();
    mpThis->mbFolderPokeCatch=false;
    mpThis->SetSubSeq(MM_SEQ_CLOSE);
    mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
    Sound::PlaySE(SEQ_SE_RELEASE);
		mpThis->mpBoxLowBgFrame->BoxBgLowDrawLiftPokeIconEnd();
    break;
	case BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE:
    break;
	case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK:
    break;
  default:
    GFL_ASSERT(0);
    //    mpThis->simpleWarrningMessage(msg_box_05_09, 2, false);
//    mpThis->SetSubSeq(MM_SEQ_WARRNING_TEAM);
  }
}


bool BoxAllBoxState::DInnerFolderViewListener::DragDrop( int index )
{
  //ポケモンを持った状態でドラッグドロップ中
  if(mpThis->mFolderCursorPos != index){

    if(index == 0){
      OHNO_PRINT("BOXにきりかえ %d %d \n",mpThis->mFolderCursorPos,index);
    }
    mpThis->mFolderCursorPos = index;
    OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);

    if(index > 0){
			mpThis->mpBoxLowBgFrame->SetFolderCursorPos(mpThis->mFolderCursorPos);
		}

    return true;
  }
  return false;


}

//--------------------------------------------------------------------------
/**
 * @brief   カーソル移動もしくはフォルダーに入る
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewListener::ChangeFolder( int index,int selectindex )
{
  //カーソル戻す
  mpThis->mpBoxLowFolderDraw->SetCursorVisible(true);
  //移動用フォルダーもどす
  mpThis->mpBoxLowFolderDraw->FolderIconMoveActive(index,false);

  if(index==-1 || index == 0){
    return;
  }

  //カーソルが一致してなかったら移動
  if(index != mpThis->mFolderCursorPosBack){
    //カーソル位置移動
    mpThis->mFolderCursorPos=index;
    OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
    mpThis->mFolderCursorPosBack=index;
    mpThis->FolderCursorAction(-1);
  }
  else{
    //一致してたら中に入る
    mpThis->mFolderCursorPos = index;
    OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
    mpThis->SetSubSeq(MM_SEQ_CLOSE_BOXIN);

  }
}




bool BoxAllBoxState::DInnerFolderViewListener::OnKeyAction( u32 dir )
{
  GFL_ASSERT(0);
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewListener::OnButtonAction( int button_no )
{
    switch(button_no){
    case BoxLowFolderDrawListener::LOWFLD_ACTION_CANCEL:
      mpThis->mpBoxLowFolderDraw->LiftPokeIconEnd();
      mpThis->SetSubSeq(MM_SEQ_CLOSE);
      mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
      //      mpThis->PageChangeDirect(mpThis->mpAppParam->nowDispTray);

      break;
    case BoxLowFolderDrawListener::LOWFLD_ACTION_DECIDE:
      //持っているとき
      if((mpThis->mbFolderPokeCatch) && (mpThis->mFolderCursorPos!=0)){
        //置いてひきつづきこのモード

        switch(mpThis->PutBox( mpThis->mSelectPagePos, mpThis->mSelectCursorPos, mpThis->mFolderCursorPos-1 )){
        case BoxPokemonMoveFunction::MOVEERR_NONE://置いた
          Sound::PlaySE( SEQ_SE_RELEASE );
          mpThis->mpBoxLowFolderDraw->LiftPokeIconEnd();
          mpThis->mpBoxBgLowDraw->PassiveMainCursor();
          mpThis->mbFolderPokeCatch=false;
          break;
        case BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE:
          //置けないときはSEのみ
          Sound::PlaySE( SEQ_SE_BEEP );
          break;
        case BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE:
        case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK:
          break;
        default:
          GFL_ASSERT(0);
          break;
        }
      }
      else{
        //持ってないとき
        mpThis->SetSubSeq(MM_SEQ_CLOSE_BOXIN);
      }
      break;
    case BoxLowFolderDrawListener::LOWFLD_ACTION_UP:   //上
    case BoxLowFolderDrawListener::LOWFLD_ACTION_DOWN:   //下
    case BoxLowFolderDrawListener::LOWFLD_ACTION_LEFT:    //左
    case BoxLowFolderDrawListener::LOWFLD_ACTION_RIGHT:    //右
      mpThis->FolderCursorAction(button_no - BoxLowFolderDrawListener::LOWFLD_ACTION_UP);
      break;
    case BoxLowFolderDrawListener::LOWFLD_ACTION_MODE_CHANGE:
      mpThis->FolderModeChangeToMain();
      break;
    case BoxLowFolderDrawListener::LOWFLD_ACTION_FIND:
      mpThis->ActionFindButton();
      break;
    }
  

}


//フォルダー画面用 複数選択
#if PM_DEBUG
void  BoxAllBoxState::DInnerFolderViewRangeListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerFolderViewRangeListener\n");
}
#endif



//--------------------------------------------------------------------------
/**
 * @brief   タッチ開始
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewRangeListener::TouchFolder( int index )
{
  mpThis->mFolderCursorPosBack = mpThis->mFolderCursorPos;
  //上画面がきりかわる
  mpThis->mFolderCursorPos = index;
  OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
  //カーソルが消える
  mpThis->mpBoxLowFolderDraw->SetCursorVisible(false);


}

//--------------------------------------------------------------------------
/**
 * @brief   つかみ開始
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewRangeListener::OnFolderHand( int x, int y, int index )
{
  mpThis->mFolderCursorPos = index;
  OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
  mpThis->FolderCursorAction(-1);

  if(!mpThis->mbFolderPokeCatch){//ポケモン掴んで無い場合
  }
  else{
    mpThis->mpBoxLowFolderDraw->SetTukamiMode();
    mpThis->mpBoxBgLowDraw->SetTukamiMode();
    mpThis->mpBoxBgLowDraw->PassiveMainCursor(); //GFNMCat1754
    mpThis->mpBoxBgLowDraw->CarryRangeSelectVisible(true);
    mpThis->mpBoxBgLowDraw->ResetRangeNum();

    OHNO_PRINT("つかみモード\n");
  }
}



bool BoxAllBoxState::DInnerFolderViewRangeListener::OnKeyAction( u32 dir )
{
  GFL_ASSERT(0);
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   フォルダーにポケモンを入れる
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewRangeListener::DropFolder( int index )
{
  //カーソル戻す
//  mpThis->mpBoxLowFolderDraw->SetCursorVisible(true);
  //移動用フォルダーもどす
  mpThis->mpBoxLowFolderDraw->FolderIconMoveActive(index,false);

  mpThis->mTempCursorNo = index;

  mpThis->SetSubSeq(MM_SEQ_FOLDER_DROP_TOUCH);

}

bool BoxAllBoxState::DInnerFolderViewRangeListener::DragDrop( int index )
{
  if(mpThis->mFolderCursorPos != index){
    OHNO_PRINT("変数変更 %d LINE %d\n",mpThis->mFolderCursorPos,__LINE__);
    mpThis->mFolderCursorPos = index;

    if(index > 0){
			mpThis->mpBoxLowBgFrame->SetFolderCursorPos(mpThis->mFolderCursorPos);
		}

    return true;
  }
  return false;
}


//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void BoxAllBoxState::DInnerFolderViewRangeListener::OnButtonAction( int button_no )
{
    switch(button_no){
    case BoxLowFolderDrawListener::LOWFLD_ACTION_CANCEL:

      mpThis->mpBoxLowFolderDraw->CarryEnd();

      mpThis->SetSubSeq(MM_SEQ_CLOSE);
      mpThis->mpBoxLowBgFrame->PageChangeDirectSelectTray();
      break;
    case BoxLowFolderDrawListener::LOWFLD_ACTION_DECIDE:
      mpThis->SetSubSeq(MM_SEQ_FOLDER_DROP);
      break;
    case BoxLowFolderDrawListener::LOWFLD_ACTION_UP:   //上
    case BoxLowFolderDrawListener::LOWFLD_ACTION_DOWN:   //下
    case BoxLowFolderDrawListener::LOWFLD_ACTION_LEFT:    //左
    case BoxLowFolderDrawListener::LOWFLD_ACTION_RIGHT:    //右
      mpThis->FolderCursorAction(button_no - BoxLowFolderDrawListener::LOWFLD_ACTION_UP);
      break;
    case BoxLowFolderDrawListener::LOWFLD_ACTION_MODE_CHANGE:
      mpThis->FolderModeChangeToMain();
      break;
    case BoxLowFolderDrawListener::LOWFLD_ACTION_FIND:
      mpThis->ActionFindButton();
      break;
    }
 

}





//フォルダー画面初期化 共通処理
void BoxAllBoxState::FolderAnimInitCommon(bool bfindreturn)
{
  MathBoxPokeNum();
  mpBoxLowFolderDraw->LiftPokeIconEnd();
  mpBoxLowFolderDraw->CarryEnd();

  mpBoxLowFolderDraw->InitBoxAnim(mpBoxAppSetup->GetMoveMode());

  if(!bfindreturn){
    Sound::PlaySE(SEQ_SE_PAGE2);
  }
}




//フォルダー画面初期化 掴んで無い状態
void BoxAllBoxState::FolderAnimInitNoPoke(bool bfindreturn)
{
  FolderAnimInitCommon(bfindreturn);
  if(mpAppParam->trade.bOn)
  {
    mpBoxLowFolderDraw->SetSearchButtonPassive();  //ＯＦＦで開始
  }
  else
  {
    mpBoxLowFolderDraw->SetSearchButtonActive();  //ＯＮで開始
  }
  mpBoxLowFolderDraw->SetButtonPos(mFolderCursorPos);
  FolderCursorAction(-1);
  mpBoxLowFolderDraw->StartMainCursor();  // @fix GFNMCat[4497]: カーソルアニメ開始
}


//フォルダー画面初期化 掴んだ状態
void BoxAllBoxState::FolderAnimInitCatch(void)
{
  FolderAnimInitCommon();
  mpBoxLowFolderDraw->SetSearchButtonPassive();  //ＯＦＦで開始

  {
    //PokeTool::SimpleParam* pPara = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSimpleData(mSelectPagePos, mSelectCursorPos);
    mpBoxLowFolderDraw->LiftPokeIconStart0();
    if(mpBoxBgLowDraw->GetFolderMode() && mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチ操作の場合ＯＫ
      OHNO_PRINT("FolderAnimInitCatch つかんだまま\n"); 
      mpBoxLowFolderDraw->SetTukamiMode();
      mpBoxLowFolderDraw->SetCursorVisible(false);
		}
		else if(mpBoxBgLowDraw->GetFolderMode()){  //タッチ操作できて、タッチ操作で無くなった場合ＮＧ
      OHNO_PRINT("FolderAnimInitCatch きゃんせる\n"); 
 			mpBoxLowFolderDraw->MouseModeEnd();
      SetSubSeq(MM_SEQ_CLOSE);//キャンセル
		}
    else{//タッチ操作で無い場合＝キーの場合はカーソル出して続行ＯＫ
      OHNO_PRINT("FolderAnimInitCatch NoTouch\n"); 
			mpBoxLowFolderDraw->SetCursorVisible(true);
    }
  }
  FolderCursorAction(-1);
  mpBoxLowFolderDraw->StopMainCursor();   // @fix GFNMCat[4497]: カーソルアニメ停止
  mpBoxLowFolderDraw->SetUIListener(&mDInnerFolderViewListener); //@fix GFNMCat[5062] 状態変更は同じ場所でが正しい
}

void BoxAllBoxState::FolderAnimInitCatch2(void)  // kawa_box_modify  使用中のペインのテクスチャを貼り替えるので処理を2フレームに分ける
{
  PokeTool::SimpleParam* pPara = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSimpleData(mSelectPagePos, mSelectCursorPos);
  mpBoxLowFolderDraw->LiftPokeIconStart(pPara);
}

//フォルダー画面初期化 範囲状態
void BoxAllBoxState::FolderAnimInitRange(void)
{
  FolderAnimInitCommon();
  mpBoxLowFolderDraw->SetSearchButtonPassive();  //ＯＦＦで開始

  {
    mpBoxLowFolderDraw->CarryStart( mRangeSelect.page,mRangeSelect.x,mRangeSelect.y,
                                    mRangeSelect.width,mRangeSelect.height,mRangeSelect.cursor,mRangeSelect.num);
    if(mpBoxBgLowDraw->GetFolderMode() && mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチ操作の場合ＯＫ
      mpBoxLowFolderDraw->SetTukamiMode();
      mpBoxLowFolderDraw->SetCursorVisible(false);
    }
		else if(mpBoxBgLowDraw->GetFolderMode()){  //タッチ操作できて、タッチ操作で無くなった場合ＮＧ
      mpBoxLowFolderDraw->MouseModeEnd();
      SetSubSeq(MM_SEQ_CLOSE);//キャンセル
    }
    else{//タッチ操作で無い場合＝キーの場合はカーソル出して続行ＯＫ
      mpBoxLowFolderDraw->SetCursorVisible(true);
    }
    //範囲選択は
  }
  FolderCursorAction(-1);
  mpBoxLowFolderDraw->StopMainCursor();   // @fix GFNMCat[4497]: カーソルアニメ停止
  mpBoxLowFolderDraw->SetUIListener(&mDInnerFolderViewRangeListener);

}






//カーソルの位置
void BoxAllBoxState::FolderCursorAction(int udlr)
{
  if(mpBoxLowFolderDraw){
    mFolderCursorPos = mpBoxLowFolderDraw->SetCursorPos(mFolderCursorPos, udlr, mpAppParam->pBoxData->GetTrayLevel());
    OHNO_PRINT("変数変更 %d LINE %d\n",mFolderCursorPos,__LINE__);
    mpBoxLowBgFrame->SetFolderCursorPos(mFolderCursorPos);
  }
}

//フォルダーモードの終了の時 共通処理
void BoxAllBoxState::FolderModeEnd(void)
{
  mpBoxLowFolderDraw->ButtonReset();
  mpBoxLowFolderDraw->SetUIListener(&mDInnerFolderNoneListener);
  mpBoxLowFolderDraw->MouseModeEnd();
}


//フォルダーからボックスにポケモンを置く
int BoxAllBoxState::PutBox(int selectpage,int selectpos, int nowpage)
{
  int pos=0;
	int no;

  for( pos=0 ;pos<Savedata::BoxPokemon::TRAY_POKE_MAX;pos++){
    if(false == mpAppParam->pBoxPokemon->CheckPokemon( nowpage, pos, mpCoreTemp )){
      break;
    }
  }
  if(pos>=Savedata::BoxPokemon::TRAY_POKE_MAX){
    return BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE; //置くところがない
  }

  if((selectpos<Savedata::BoxPokemon::TRAY_POKE_MAX) && (selectpage == nowpage)){//同じページには置けない仕様 
    return BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE; //置くところがない
  }

  no = mpBoxPokemonMoveFunction->pokeMoveCheck(selectpage, selectpos,
                                               nowpage, pos);  //置いてはいけない条件に引っかかった  代表して空いてる場所posを使用するGFNMCat1584
	switch(no){
	case BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE:
    SetSubSeq(MM_SEQ_WARRNING_BATTLE);
		return no;
	case BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK:
    SetSubSeq(MM_SEQ_WARRNING_TEAM);
		return no;
	}

  no = mpBoxPokemonMoveFunction->pokeMoveCheckFolderOnly(selectpage, selectpos,
                                                             nowpage, pos);  //置いてはいけない条件に引っかかった
	switch(no){
	case BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE:
    SetSubSeq(MM_SEQ_WARRNING_BATTLE);
		return no;
  case BoxPokemonMoveFunction::MOVEERR_NONE:
    break;
  default:
    GFL_ASSERT(0);
	}

  mpBoxPokemonMoveFunction->PokemonSet(selectpage, selectpos, nowpage, pos,-1);

  mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_AlignMember(0,NULL,false);
  mpBoxAppSetup->GetBoxSimpleAccesser()->PacksResortPokemon();
  mpBoxPokemonMoveFunction->TeamPackAll();
  mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonUseStock();

  MathBoxPokeNum();  //再構成

  if(mpBoxUppStatusListener){
    mpBoxUppStatusListener->Request(BoxUppStatusListener::UPPDISP_FOLDER_ICONUPDATE,((nowpage<<8)&0xff00)+pos );
  }
  if(mpAppParam->trade.bOn)
  {
    mpBoxLowFolderDraw->SetSearchButtonPassive();  //ＯＦＦのまま
  }
  else
  {
    mpBoxLowFolderDraw->SetSearchButtonActive();  //ＯＮになる
  }

  mpBoxLowFolderDraw->StartMainCursor();  // @fix GFNMCat[4497]: 通常配置後、アニメ再開

  return BoxPokemonMoveFunction::MOVEERR_NONE;
}

//ポケモンを配置できるかどうか判定して置ければおく
int BoxAllBoxState::PutBoxRangeFunc(int selectpage,int selectpos, int nowpage)
{
  int retno;
  
  if((selectpos<Savedata::BoxPokemon::TRAY_POKE_MAX)&&(selectpage == nowpage)){//同じページには置けない仕様 
    retno = BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE;
  }
  else{
    retno = mpBoxPokemonMoveFunction->PutBoxRange( selectpage, selectpos, nowpage, &mRangeSelect ); //置いた
  }
  if(retno != BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE){//NMCat[365] 置けないときは下記の操作をしない
    MathBoxPokeNum();  //再構成
    mpBoxBgLowDraw->LiftPokeIconEnd(mpAppParam->nowDispTray,mpAppParam->nowBattleTray);
    mpBoxBgLowDraw->LiftPokeIconClear();
    mpBoxLowFolderDraw->StartMainCursor();  // @fix GFNMCat[4497]: 範囲配置後、アニメ再開
    FolderCursorAction(-1);   // @note タッチ操作時にカーソル位置がそのままになっていたので追加。ここでやると一瞬初期位置で表示されてしまう...
  }
  switch(retno){
  case BoxPokemonMoveFunction::MOVEERR_NONE:  //おけたばあい
    Sound::PlaySE(SEQ_SE_RELEASE);
    if(mpBoxUppStatusListener){
      mpBoxUppStatusListener->Request(BoxUppStatusListener::UPPDISP_FOLDER_RELOAD, 0 );
    }
    mpBoxLowFolderDraw->CarryEnd();
    mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_AlignMember(0,NULL,false);
    mpBoxAppSetup->GetBoxSimpleAccesser()->PacksResortPokemon();
    mpBoxPokemonMoveFunction->TeamPackAll();
    mpBoxBgLowDraw->PassiveMainCursor();
    mpBoxBgLowDraw->ResetRangeNum();
    mbFolderPokeCatch=false;
    SetSearchButtonActive();
    mpBoxLowFolderDraw->SetUIListener(&mDInnerFolderViewListener);  //標準に戻す
    break;
  case BoxPokemonMoveFunction::MOVEERR_NO_PUT_PLACE:
    //置けないときはSEのみ
    Sound::PlaySE( SEQ_SE_BEEP );
    break;
  case   BoxPokemonMoveFunction::MOVEERR_TEAM_LOCK:   //ロック
    SetSubSeq(MM_SEQ_WARRNING_TEAM);
    break;
 case  BoxPokemonMoveFunction::MOVEERR_NO_BATTLEPOKE:    //戻る処理が必要 バトルポケモンいなくなる
    SetSubSeq(MM_SEQ_WARRNING_BATTLE);
    break;
  }
  return retno;
}

//フォルダーの見た目の再計算
void BoxAllBoxState::MathBoxPokeNum(void)
{
  int num1,num2;

  if(mpBoxAppSetup->GetBoxSimpleAccesser()->IsSearchMode() || mpAppParam->trade.bOn)  //検索結果
  {
    for(int i=0;i<Savedata::Box::TRAY_MAX;i++){
      bool bactive=false;
      num1 = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSearchPokemonNum(i);
      if(num1!=0){
        bactive=true;
      }
      num2 = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonCount(i);
      mpBoxLowFolderDraw->AnimFolder(i, num2, bactive, mpAppParam->pBoxData->GetTrayLevel());
    }
  }
  else
  {
    for(int i=0;i<Savedata::Box::TRAY_MAX;i++){
      num2 = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonCount(i);
      mpBoxLowFolderDraw->AnimFolder(i, num2, true, mpAppParam->pBoxData->GetTrayLevel());
    }

  }
}

void BoxAllBoxState::ActionFindButton(void)
{
  mpBoxLowBgFrame->NetworkSendMessage( msg_chat_01_12-msg_chat_01_01 );
  mpAppParam->outJumpMode = App::Box::BOXJUMP_FIND;
  mpAppParam->outTray = (u8)mSelectPagePos;
  mpAppParam->outPos = (u8)mSelectCursorPos;
  SetSubSeq(MM_SEQ_APP_JUMP);
}



void BoxAllBoxState::FolderModeChangeToMain(void)
{

  if( mpBoxLowFolderDraw->GetTukamiMode()){
    //掴みモードにもどす
    OHNO_PRINT("掴みモードにもどす\n");
    mpBoxBgLowDraw->SetTukamiMode();
    mpBoxBgLowDraw->InitTouchPos();
	//	mpBoxLowBgFrame->PageChangeDirectSelectTray();

    if(mpBoxAppSetup->GetMoveMode() == BOX_CHANGE_SELECTMODE_GROUP){
      SetSubSeq(  MM_SEQ_CLOSE_RANGE );
    }
    else{
      SetSubSeq(  MM_SEQ_CLOSE_SINGLE );
    }
  }
  else if(mbFolderPokeCatch){
    if(mpBoxAppSetup->GetMoveMode() == BOX_CHANGE_SELECTMODE_GROUP){
      SetSubSeq(  MM_SEQ_CLOSE_RANGE );
    }
    else{
      SetSubSeq(  MM_SEQ_CLOSE_SINGLE );
    }
	//	mpBoxLowBgFrame->PageChangeDirectSelectTray();
  }
  else{//キャンセルあつかい
    SetSubSeq(MM_SEQ_CLOSE);
//		mpBoxLowBgFrame->PageChangeDirectSelectTray();
  }
  Sound::PlaySE(SEQ_SE_PAGE2);

//  if(mFolderCursorPos!=0 && mFolderCursorPos!=-1){
//  }
//  else if(!mbFolderPokeCatch){
//  }
//  else{
		mpBoxLowBgFrame->PageChangeDirectSelectTray();
//  }
  mpBoxLowBgFrame->RequestChangePoke();
  
}


/*
u32 BoxAllBoxState::GetFolderCursorPos(void)
{
  return mFolderCursorPos;
}
*/

void BoxAllBoxState::DrawEnd(void)
{
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
  mpMenuWindow->EndMenu();
  mpBoxBgLowDraw->MessageLine2Close();
  mpBoxBgLowDraw->ResetChatChangeButton();
  mpBoxBgLowDraw->LiftPokeIconClear();
//  mpBoxBgLowDraw->ResetFunc();
}

//--------------------------------------------------------------------------
/**
 * @brief   検索ボタンをＯＮにする
 */
//--------------------------------------------------------------------------

void BoxAllBoxState::SetSearchButtonActive(void)
{
  if(!mpAppParam->trade.bOn)
  {
    mpBoxLowFolderDraw->SetSearchButtonActive();  //ＯＮになる
  }
}


GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




