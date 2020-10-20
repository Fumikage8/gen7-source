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
BoxWarrningMessageState::BoxWarrningMessageState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxWarrningMessageState::~BoxWarrningMessageState()
{
}

enum{
  MM_SEQ_START,
  MM_SEQ_LOOP,
  MM_SEQ_CLOSE,
};


//------------------------------------------------------------------
/**
 * @brief	  プロセスの過程（現在の状態）を更新する
 * @return	ProcSeqENUM
 */
//------------------------------------------------------------------
void BoxWarrningMessageState::SetSubSeq(int seq)
{
  OHNO_PRINT("SetSubSeq %d\n",seq);
  ::applib::frame::CellSuper::SetSubSeq(seq);
}



void BoxWarrningMessageState::SimpleWarrningMessage(const MessageParam* pMsgPara)
{
    gfl2::std::MemCopy(pMsgPara ,&mMessageParam,sizeof(MessageParam));
  
  if(mMessageParam.LineOrPos == 2){
    mpBoxBgLowDraw->DrawMessageLine2( mMessageParam.MsgNo );
  }
  else{
    mpBoxBgLowDraw->DrawMessageLine1( mMessageParam.MsgNo );
  }
  if( mMessageParam.bWarrningSe ){
    Sound::PlaySE( SEQ_SE_BEEP );
  }
  GFL_ASSERT(GetSubSeq()==MM_SEQ_START);
  SetSubSeq(MM_SEQ_START);
}


void BoxWarrningMessageState::SetParam(BoxLowBgDraw* pBoxLowBgDraw)
{
  mpBoxBgLowDraw = pBoxLowBgDraw;
}



u32 BoxWarrningMessageState::StateFunc(void)
{
  switch(GetSubSeq()){
  case MM_SEQ_START:
    mDInnerMessageDefMessageListener.SetThis(this);
    mpBoxBgLowDraw->SetUIListener( &mDInnerMessageDefMessageListener );
    SetSubSeq(MM_SEQ_LOOP);
    break;
  case MM_SEQ_LOOP:
    break;
  case MM_SEQ_CLOSE:
  default:
    return BOXSTATE_RETURN;
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
applib::frame::Result BoxWarrningMessageState::InitFunc(void)
{
  return applib::frame::RES_NEXT;
}
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
 applib::frame::Result BoxWarrningMessageState::UpdateFunc(void)
{
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
 void BoxWarrningMessageState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
 applib::frame::Result BoxWarrningMessageState::EndFunc(void)
{
  End();
  Sound::PlaySE(SEQ_SE_MESSAGE);  //NMCat814
  mpBoxBgLowDraw->MessageLine2Close();
  return applib::frame::RES_NEXT;
}




//汎用メッセージ表示中
#if PM_DEBUG
void  BoxWarrningMessageState::DInnerMessageDefMessageListener::DebugDisp(void)
{
  OHNO_PRINT("BoxStateBasis::DInnerMessageDefMessageListener\n");
}
#endif
//--------------------------------------------------------------------------
/**
 * @brief   タッチの最初の検査
 * @param   touchしたかどうか
 * @return  これ以上処理しない場合true 通常false
 */
//--------------------------------------------------------------------------
bool BoxWarrningMessageState::DInnerMessageDefMessageListener::OnTouchFirstHook(bool isTouch)
{
  switch(mHookStat){
  case 0:
    if(isTouch){
      mHookStat++;
      OHNO_PRINT("mHookStat 1\n");
    }
    else{
      mpThis->SetSubSeq(MM_SEQ_CLOSE);
      mHookStat=2;
      OHNO_PRINT("mHookStat 2\n");
    }
    break;
  case 1:
    if(!isTouch){
      mpThis->SetSubSeq(MM_SEQ_CLOSE);
      mHookStat++;
      OHNO_PRINT("mHookStat 2++\n");
    }
    break;
  }
  return true;
}


bool BoxWarrningMessageState::DInnerMessageDefMessageListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}

bool BoxWarrningMessageState::DInnerMessageDefMessageListener::PreButtonAction( int button_no )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
  return true;
}


void BoxWarrningMessageState::DInnerMessageDefMessageListener::OnButtonAction( int button_no )
{
  mpThis->SetSubSeq( MM_SEQ_CLOSE );
}


void BoxWarrningMessageState::DInnerMessageDefMessageListener::SetThis( BoxWarrningMessageState* pthis )
{
  mpThis = pthis;
}








GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




