//=============================================================================
/**
 * @file	 BoxStateBasis.cpp
 * @brief	 BOXの状態の基底
 * @author k.ohno
 * @date	 2016.2.28
 */
//=============================================================================
#pragma once


//#include "../BoxPokepara.h"
//#include "../BoxSimpleAccesser.h"

#include <arc_index/debug_message.gaix>

#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>

#include <arc_index/script_message.gaix>

#include "AppLib/include/Frame/AppFrameManager.h"

#include <Fade/include/gfl2_FadeManager.h>

#include "App/Box/include/BoxAppParam.h"
#include "App/Box/include/BoxAppSetup.h"

#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/BoxPokemonSave.h"
#include "Savedata/include/ResortSave.h"
#include "PokeTool/include/PokeToolPresentPokemon.h"

#include "niji_conv_header/message/msg_box.h"


#include "BoxLowBgDraw.h"
#include "BoxStateBasis.h"


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
BoxStateBasis::BoxStateBasis(void)
{
  mUpdateStartSeq=MENU_SEQ_INIT;

}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxStateBasis::~BoxStateBasis()
{
  GFL_ASSERT(mpListener==NULL);
}



//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------

void BoxStateBasis::SetParam(BoxStateEndListener* pListener,
                             APP_PARAM* pAppParam, BoxAppSetup* pBoxAppSetup,
                             PokePara* pBoxPokeParaFunc,
                             BoxPokemonMoveFunction* pBoxPokemonMoveFunction,
                             BoxLowBgFrame* pBoxLowBgFrame ,
                             app::tool::MenuWindow* pMenuWindow,
                             BoxLowBgDraw* pBoxLowBgDraw)
{
  mpListener = pListener;


  mpAppParam=pAppParam;
  mpBoxAppSetup= pBoxAppSetup;
  mpBoxPokeParaFunc = pBoxPokeParaFunc;
  mpBoxPokemonMoveFunction=pBoxPokemonMoveFunction;
  mpBoxLowBgFrame = pBoxLowBgFrame;
  mpMenuWindow = pMenuWindow;
  mpBoxBgLowDraw = pBoxLowBgDraw;

  mpAppHeap = mpAppParam->mpAppHeap;
}


void BoxStateBasis::End(void)
{
  //抜ける前に処理をはずしておく
  if(mpBoxBgLowDraw){
    mpBoxBgLowDraw->SetUIListener(NULL);
  }
	if(mpListener){  //リスナーが無いタイプも作ったから
	  mpListener->OnEnd(mRetCode);
	}
  mpListener=NULL;
}


//シーケンススタート位置をframeupdateに伝える関数
int BoxStateBasis::UpdateStart(void)
{
  int no = mUpdateStartSeq;
  mUpdateStartSeq = MENU_SEQ_INIT;
  return no;
}




void BoxStateBasis::simpleWarrningMessage(u32 no, u32 line, u32 bse )
{
  mMessageParam.MsgNo = no;
  mMessageParam.LineOrPos = line;
  mMessageParam.bWarrningSe = bse;
}



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




