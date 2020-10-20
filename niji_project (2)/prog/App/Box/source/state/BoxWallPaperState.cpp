//=============================================================================
/**
 * @file	 BoxWallpaperState.cpp
 * @brief	 壁紙の移動状態
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

#include "BoxWallpaperState.h"
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
BoxWallpaperState::BoxWallpaperState(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxWallpaperState::~BoxWallpaperState()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxWallpaperState::Setup(void)
{

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpCoreTemp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
}


void BoxWallpaperState::Release(void)
{
  GFL_SAFE_DELETE(mpCoreTemp);
}


enum{
  MM_SEQ_PAPER_INIT,
  MM_SEQ_PAPER_LOOP,
	MM_SEQ_WALL_CHANGE_ST,
	MM_SEQ_PAPER_END,
//  MM_SEQ_CLOSE,
  MM_SEQ_APP_JUMP,
  MM_SEQ_ALLBOX_JUMP,

};

//BOXボタン押した時
void BoxWallpaperState::ModeStart(int tray,int cur)
{
  //選んだ場所
  mSelectTray = tray;
	mSelectCursor = cur;
  SetUpdateStartSeq(MM_SEQ_PAPER_INIT);
}



u32 BoxWallpaperState::StateFunc(void)
{
  switch(GetSubSeq()){

  case MM_SEQ_PAPER_INIT:
    BoxMenuOpen();
    break;
  case MM_SEQ_PAPER_LOOP:
    break;
  case MM_SEQ_PAPER_END:
    mpBoxBgLowDraw->ResetFunc();
    return BOXSTATE_RETURN;
  case MM_SEQ_WALL_CHANGE_ST:
    if(mpBoxBgLowDraw->WallPaperChange(mpAppParam->pBoxData->GetWallPaper(mpAppParam->nowDispTray))){
      SetSubSeq( MM_SEQ_PAPER_END );
    }
    break;
  case MM_SEQ_APP_JUMP:
    return BOXSTATE_APP_JUMP;
  case MM_SEQ_ALLBOX_JUMP:
    return BOXSTATE_ALLBOX;
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
applib::frame::Result BoxWallpaperState::InitFunc(void)
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
applib::frame::Result BoxWallpaperState::UpdateFunc(void)
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
void BoxWallpaperState::DrawFunc(gfl2::gfx::CtrDisplayNo no)
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
applib::frame::Result BoxWallpaperState::EndFunc(void)
{
  mpBoxBgLowDraw->MessageLine2Close();
  mpMenuWindow->EndMenu();
  End();
  Release();
  return applib::frame::RES_FINISH;
}

///リスナー
//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   ボックスのタイトルを選択した時のリスナー
 * @date    2015.11.28
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxWallpaperState::InnerBoxOptionListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //ジャンプする
    mpThis->SetSubSeq(MM_SEQ_ALLBOX_JUMP);
    break;
  case 1:    //かべがみ
    mpThis->PaperMenuOpen();
    return;
  case 2:  //なまえ
    mpThis->mpAppParam->outJumpMode = App::Box::BOXJUMP_NAME;
    mpThis->mpAppParam->outTray = (u8)mpThis->mSelectTray;
    mpThis->mpAppParam->outPos = (u8)mpThis->mSelectCursor;
    mpThis->SetSubSeq(MM_SEQ_APP_JUMP);
    break;
  case 3:  //やめる
    mpThis->SetSubSeq( MM_SEQ_PAPER_END );
    break;
  }
  mpThis->mpBoxBgLowDraw->MessageLine2Close();
  mpThis->mpMenuWindow->EndMenu();
}




//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   壁紙メニュー
 * @date    2015.11.28
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxWallpaperState::InnerPaperMenuListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //ふうけい１
    mpThis->PaperXMenuOpen(0);
    break;
  case 1:    //ふうけい２
    mpThis->PaperXMenuOpen(1);
    break;
  case 2:   //ふうけい３
    mpThis->PaperXMenuOpen(2);
    break;
  case 3:   //えとせとら
    mpThis->PaperXMenuOpen(3);
    break;
  case 4:  //やめる
    mpThis->SetSubSeq( MM_SEQ_PAPER_END );
    break;
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   壁紙メニュー風景１
 * @date    2015.11.28
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxWallpaperState::InnerPaperLandscape1Listener::SelectMenu( u32 id )
{
  switch(id){
  case 0:    //もり
    mpThis->PaperChangeExec( 0 );
    break;
  case 1:    //シティー
    mpThis->PaperChangeExec( 1 );
    break;
  case 2:   //さばく
    mpThis->PaperChangeExec( 2 );
    break;
  case 3:   //さばんな
    mpThis->PaperChangeExec( 3 );
    break;
  case 4:  //もどる
    mpThis->PaperMenuOpen();
    break;
  }


}

//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   壁紙メニュー風景２
 * @date    2015.11.28
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxWallpaperState::InnerPaperLandscape2Listener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //
    mpThis->PaperChangeExec( 4 );
    break;
  case 1:    //シティー
    mpThis->PaperChangeExec( 5 );
    break;
  case 2:   //さばく
    mpThis->PaperChangeExec( 6 );
    break;
  case 3:   //さばんな
    mpThis->PaperChangeExec( 7 );
    break;
  case 4:  //もどる
    mpThis->PaperMenuOpen();
    break;
  }


}


//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   壁紙メニュー風景３
 * @date    2015.11.28
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxWallpaperState::InnerPaperLandscape3Listener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //もり
    mpThis->PaperChangeExec( 8 );
    break;
  case 1:    //シティー
    mpThis->PaperChangeExec( 9 );
    break;
  case 2:   //さばく
    mpThis->PaperChangeExec( 10 );
    break;
  case 3:   //さばんな
    mpThis->PaperChangeExec( 11 );
    break;
  case 4:  //もどる
    mpThis->PaperMenuOpen();
    break;
  }


}



//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   壁紙メニュー風景３
 * @date    2015.11.28
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxWallpaperState::InnerPaperEtcListener::SelectMenu( u32 id )
{

  switch(id){
  case 0:    //
    mpThis->PaperChangeExec( 12 );
    break;
  case 1:    //
    mpThis->PaperChangeExec( 13 );
    break;
  case 2:   //
    mpThis->PaperChangeExec( 14 );
    break;
  case 3:   //
    mpThis->PaperChangeExec( 15 );
    break;
  case 4:  //もどる
    mpThis->PaperMenuOpen();
    break;
  }

}



void BoxWallpaperState::PaperMenuOpen(void)
{
  mpMenuWindow->EndMenu();

  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_20);
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_21);
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_22);
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_23);
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_end, true, true);
  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);
  mInnerPaperMenuListener.SetThis( this );
  mpMenuWindow->SetListener( &mInnerPaperMenuListener );

  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_07);
  SetSubSeq( MM_SEQ_PAPER_LOOP );

  
}



void BoxWallpaperState::PaperXMenuOpen(int no)
{
  mpMenuWindow->EndMenu();
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_24 + no*4);//もり
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_25 + no*4);//シティー
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_26 + no*4);//サバク
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_27 + no*4);//サバンナ
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_40, true, true);  //もどる
  switch(no){
  case 0:
    mInnerPaperLandscape1Listener.SetThis( this );
    mpMenuWindow->SetListener( &mInnerPaperLandscape1Listener );
    break;
  case 1:
    mInnerPaperLandscape2Listener.SetThis( this );
    mpMenuWindow->SetListener( &mInnerPaperLandscape2Listener );
    break;
  case 2:
    mInnerPaperLandscape3Listener.SetThis( this );
    mpMenuWindow->SetListener( &mInnerPaperLandscape3Listener );
    break;
  case 3:
    mInnerPaperEtcListener.SetThis( this );
    mpMenuWindow->SetListener( &mInnerPaperEtcListener );
    break;
  }
  mpMenuWindow->StartMenu(0, app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);

  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_08);

  SetSubSeq( MM_SEQ_PAPER_LOOP );

}

void BoxWallpaperState::PaperChangeExec( int no )
{
  mpAppParam->pBoxData->SetWallPaper( mpAppParam->nowDispTray ,no  );
  mpMenuWindow->EndMenu();
  mpBoxBgLowDraw->MessageLine2Close();
  SetSubSeq( MM_SEQ_WALL_CHANGE_ST );
}



//ボックスのタイトルが選択された時の処理
void BoxWallpaperState::BoxMenuOpen(void)
{
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_07);  //じゃんぷ
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_08);  //かべがみ
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_01_09);  //なまえ
  mpMenuWindow->AddItem(mpAppParam->pMsgData, msg_box_end, true, true);   //bぼたんあり。やめる

  mpMenuWindow->StartMenu(0,app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_FX, MENU_1LINE_POS);  //fix

  mpBoxBgLowDraw->DrawMessageLine1(msg_box_02_04);

  SetSubSeq( MM_SEQ_PAPER_LOOP );

  mInnerBoxOptionListener.SetThis(this);
  mpMenuWindow->SetListener( &mInnerBoxOptionListener );

  mpBoxBgLowDraw->SetUIListener( NULL );

  mpBoxBgLowDraw->PassiveUpperButton();
  mpBoxBgLowDraw->PassiveUnderButton();
  mpBoxBgLowDraw->PassiveBoxChangeCursor();
  mpBoxBgLowDraw->PassiveFolderButton();
  mpBoxBgLowDraw->PassiveCancelButton();
  mpBoxBgLowDraw->PassiveBoxTrayChangeCursor();
  mpBoxBgLowDraw->PassiveMainCursor();

}



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




