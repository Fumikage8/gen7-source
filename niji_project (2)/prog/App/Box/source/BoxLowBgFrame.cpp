//=============================================================================
/**
 * @file	 BoxLowBgFrame.cpp
 * @brief	 下画面BOXの操作や制御を管理
 * @author k.ohno
 * @date	 2015.7.14
 */
//=============================================================================
#pragma once


#include "BoxLowBgFrame.h"
#include "BoxLowBgDraw.h"
#include "BoxLowChtDraw.h"
#include "BoxLowFolderDraw.h"
#include "BoxPokepara.h"
#include "BoxSimpleAccesser.h"

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
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include "AppLib/include/Ui/UIViewManager.h"

#include "state/BoxItemState.h"
#include "state/BoxAllBoxState.h"
#include "state/BoxP2pTradeState.h"
#include "state/BoxBattleTeamState.h"
#include "state/BoxMarkState.h"
#include "state/BoxEscapeState.h"
#include "state/BoxWallPaperState.h"
#include "state/BoxResortState.h"
#include "state/BoxNormalState.h"
#include "state/BoxExitState.h"
#include "state/BoxTeamNameState.h"

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

#if PM_DEBUG
static BoxNormalState* mpStaticBoxNormalState;
static BoxLowBgFrame* mpStaticBoxLowBgFrame;
static APP_PARAM*  mpStaticAppParam;
#endif

enum{
  MM_SEQ_NONE,
  MM_SEQ_ICONLOAD,
  MM_SEQ_LOOP,
  MM_SEQ_LOOP_WAIT,
  MM_SEQ_END,
  MM_SEQ_END_TIMER,
  MM_SEQ_NORMAL_SHORTCUT, //通常
  MM_SEQ_NORMAL_MENU,
  MM_SEQ_NORMAL_RANGE,
  MM_SEQ_NORMAL_END,
  MM_SEQ_RESORT_SHORTCUT,  //  リゾート 10
  MM_SEQ_RESORT_MENU,//
  MM_SEQ_RESORT_RANGE,
  MM_SEQ_RESORT_END,
  MM_SEQ_BATTLE_RANGE,  //バトルチーム選択
  MM_SEQ_BATTLE_MENU,
  MM_SEQ_BATTLE_IN,//
  MM_SEQ_BATTLE_IN_LOOP,
  MM_SEQ_BATTLE_REGIST,
  MM_SEQ_BATTLE_REMOVE,//
  MM_SEQ_TRADE_START,//  通信交換20
  MM_SEQ_TRADE_WAIT,
  MM_SEQ_TRADE_WAIT1,
  MM_SEQ_TRADE_CHAT,
  MM_SEQ_ITEM_INIT,//   アイテム移動
  MM_SEQ_MOVE_ITEM_TOUCH,
  MM_SEQ_MOVE_ITEM,
  MM_SEQ_ITEM_FIRST,
  MM_SEQ_ITEM_LOOP,//
  MM_SEQ_MARK_INIT,   //マーク
  MM_SEQ_MARK_LOOP,                  //30
  MM_SEQ_ESCAPE_INIT,   //逃げる
  MM_SEQ_ESCAPE_FINISH,
  MM_SEQ_FOLDER_INIT_RE,//
  MM_SEQ_FOLDER_INIT,//
  MM_SEQ_FOLDER_LOOP,
  MM_SEQ_FOLDER_RANGE_INIT,//
  MM_SEQ_BOXMENU_START,//
  MM_SEQ_BOXMENU_LOOP,
  MM_SEQ_PAPER_INIT,
  MM_SEQ_PAPER_LOOP,
  MM_SEQ_EXIT_MENU,
  MM_SEQ_EXIT_REQ,
  MM_SEQ_EXIT_END,

  // @fix niji GFNMCat[254]: 手持ちトレイの移動待ちシーケンスを追加
  MM_SEQ_MENU_CANCEL,
  MM_SEQ_MENU_CANCEL_WAIT,
	MM_SEQ_TEAM_PAGECHANGE_LOOP,   // GFNMCat[1551]チーム切り替えシーケンス追加
  MM_SEQ_PAGECHANGE_LOOP,  ///GFNMCat[1621]ページ切り替えのシーケンス追加
  MM_SEQ_FIND_JUMP, // GFNMCat[1614]検索Jump

};




//p2p交換終了時
void BoxLowBgFrame::InnerBoxP2pTradeEndListener::OnEnd( u32 retcode )
{
  mpThis->mBoxP2pTradeStateRet=retcode;
  mpThis->mpBoxP2pTradeState=NULL;
}





#if PM_DEBUG
void  BoxLowBgFrame::DInnerNoneListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerNoneListener\n");
}
#endif

///************************************



//普通のカーソルの時用
#if PM_DEBUG
void  BoxLowBgFrame::DInnerNormalListener::DebugDisp(void)
{
  OHNO_PRINT("DInnerNormalListener\n");
}
#endif
bool BoxLowBgFrame::DInnerNormalListener::OnMousePokemonCurpos(int curpos)
{
  mpThis->mCursorPos = curpos;
  GFL_ASSERT(mpThis->mCursorPos>=0);
  mpThis->CursorAction(-1);
  OHNO_PRINT("OnMousePokemonCurposカーソル位置 %d\n",curpos);
  if(mpThis->mpBoxAppSetup->GetMoveMode() == BOX_CHANGE_SELECTMODE_GROUP){  //複数選択
    return true;
  }
  return false;
}


bool BoxLowBgFrame::DInnerNormalListener::OnMousePokemonFirstTouch(int curpos)
{
  if(mpThis->mpBoxAppSetup->GetMoveMode() == BOX_CHANGE_SELECTMODE_GROUP){  //複数選択
    mpThis->mCursorPos = curpos;
    GFL_ASSERT(mpThis->mCursorPos>=0);
    mpThis->FirstDecide();
    return true;
  }
  return false;
}

bool BoxLowBgFrame::DInnerNormalListener::OnKeyAction( u32 dir )
{
  mpThis->SetSubSeq( MM_SEQ_END );
  return true;
}

void BoxLowBgFrame::DInnerNormalListener::OnButtonAction( int button_no )
{
  OHNO_PRINT("DInnerNormalListener::OnButtonAction\n");

  //  if(!mpThis->m_Slave){
  switch(button_no){
  case BoxLowBgDrawListener::LOWBG_ACTION_CANCEL:
    mpThis->SetSubSeq(  MM_SEQ_EXIT_MENU);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_DECIDE:
    mpThis->FirstDecide();
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_UP:   //上
  case BoxLowBgDrawListener::LOWBG_ACTION_DOWN:   //下
  case BoxLowBgDrawListener::LOWBG_ACTION_LEFT:    //左
  case BoxLowBgDrawListener::LOWBG_ACTION_RIGHT:    //右
    mpThis->CursorAction(button_no - BoxLowBgDrawListener::LOWBG_ACTION_UP);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE:    //LBUTTON
    mpThis->PageChange(-1);
    mpThis->SetSubSeq(MM_SEQ_PAGECHANGE_LOOP);   ///GFNMCat[1621]ページ切り替えのシーケンス追加
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE:    //RBUTTON
    mpThis->PageChange(1);
    mpThis->SetSubSeq(MM_SEQ_PAGECHANGE_LOOP);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_L_TEAM:    //LBUTTON
    mpThis->BattleInPageChange(-1);
    mpThis->SetSubSeq(MM_SEQ_TEAM_PAGECHANGE_LOOP); // GFNMCat[1551]チーム切り替えシーケンス追加
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_R_TEAM:    //RBUTTON
    mpThis->BattleInPageChange(1);
    mpThis->SetSubSeq(MM_SEQ_TEAM_PAGECHANGE_LOOP);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_SELECT_CHANGE:
    mpThis->MoveModeChange();
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE:
    mpThis->EditTypeChange();
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE0:
    Sound::PlaySE(SEQ_SE_DECIDE1);
    mpThis->SetEditType(0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE1:
    Sound::PlaySE(SEQ_SE_DECIDE1);
    mpThis->SetEditType(1);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE2:
    Sound::PlaySE(SEQ_SE_DECIDE1);
    mpThis->SetEditType(2);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_BOX:
  case BoxLowBgDrawListener::LOWBG_ACTION_BOXBAR:
    mpThis->SetSubSeq(MM_SEQ_PAPER_INIT);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_TEAM_NAME:
    mpThis->TeamNameChangeOpen();
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_FOLDER:
		mpThis->mpAppParam->folderCursor=0;
    mpThis->SetSubSeq(  MM_SEQ_FOLDER_INIT );  //ここからいくのは掴んでない
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_JUDGE:
    Sound::PlaySE(SEQ_SE_DECIDE1);
    mpThis->mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_JUGDE,0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_FIND:
    mpThis->SetSubSeq(  MM_SEQ_FIND_JUMP );  
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_INFO:
    mpThis->InfoDispStart();
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_CHAT_CHANGE:
    Sound::PlaySE(SEQ_SE_DECIDE1);  //GFNMCat1507
    mpThis->RequestChangePoke();
    mpThis->mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_CHAT ,0);
    break;
  case BoxLowBgDrawListener::LOWBG_ACTION_CHAT:
    mpThis->SetSubSeq(  MM_SEQ_TRADE_CHAT );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   初期位置キープ開始
 */
//--------------------------------------------------------------------------
void BoxLowBgFrame::DInnerNormalListener::OnMousePokemonStart(int x,int y)
{
}

//--------------------------------------------------------------------------
/**
 * @brief   掴みに変更
 */
//--------------------------------------------------------------------------
void BoxLowBgFrame::DInnerNormalListener::OnMousePokemonHand(int x,int y, int cursorno)
{
//  mpThis->mpBoxBgLowDraw->PassiveCancelButton();

  if(mpThis->mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM){
    mpThis->SetSubSeq(MM_SEQ_MOVE_ITEM_TOUCH);
  }
  else if(mpThis->mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM){
    mpThis->SetSubSeq(MM_SEQ_BATTLE_IN);
  }
  else if( mpThis->mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpThis->mpAppParam->type == BOX_CALL_RESORT_GIM){
    mpThis->SetSubSeq(MM_SEQ_RESORT_SHORTCUT);
  }
  else if(mpThis->mpBoxAppSetup->GetMoveMode() != BOX_CHANGE_SELECTMODE_GROUP){  //複数選択でない
    mpThis->SetSubSeq(MM_SEQ_NORMAL_SHORTCUT);
  }

}

//--------------------------------------------------------------------------
/**
 * @brief   メニューをひらく
 */
//--------------------------------------------------------------------------
void BoxLowBgFrame::DInnerNormalListener::OnMouseMenuOpen(int cursorno)
{
  if(cursorno==-1){

  }
  else if(mpThis->mpBoxAppSetup->GetMoveMode() != BOX_CHANGE_SELECTMODE_GROUP){  //複数選択
    mpThis->mCursorPos = cursorno;
    GFL_ASSERT(mpThis->mCursorPos>=0);
    mpThis->FirstDecide();
  }
  else{  //範囲が確定

  }
}





//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
BoxLowBgFrame::BoxLowBgFrame(void)
{
  mReadSeq=0;
  mbInitEnd=false;
  mbEndReq=false;
  m_CurSeq=0;
  mCursorPos=0;

  GetInputExculusiveKeyMode();
  SetInputExculusiveKeyMode( true );
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxLowBgFrame::~BoxLowBgFrame()
{
  SetInputExculusiveKeyMode( m_isUIViewInputExclusiveKey );
}





void BoxLowBgFrame::Setup( APP_PARAM* pAppParam, BoxAppSetup* pBoxAppSetup )
{
  mpAppHeap = pAppParam->mpAppHeap;
  mpAppRenderingManager = pAppParam->mpAppRenderingManager;
  mpAppParam = pAppParam;
  mCursorPos = mpAppParam->outPos;
  mpBoxAppSetup = pBoxAppSetup;
}

enum{
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT,
  LOCAL_POKEICON_CREATE,
};






//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BoxLowBgFrame::InitFunc(void)
{
#if PM_DEBUG
  mDebugSeqBackup=-1;
#endif
#if PM_DEBUG
  mpStaticBoxLowBgFrame = this;
  mpStaticAppParam = mpAppParam;
#endif
  
  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない  128

#if PM_DEBUG
  OHNO_PRINT("BoxLowBgFrame::InitFunc %d\n",mReadSeq);
#endif

  switch(mReadSeq){  ///読み込み開始をここに記述      heapとり方修正  2015.05.20
  case LOCAL_OPEN_START:
#if defined(GF_PLATFORM_CTR)
    OHNO_PRINT("BoxLowBgFrame::InitFunc %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif


    mpCoreTemp = GFL_NEW( mpAppHeap->GetDeviceHeap() ) pml::pokepara::CoreParam( mpAppHeap->GetDeviceHeap() );
    mpPartyTemp = GFL_NEW( mpAppHeap->GetDeviceHeap() ) pml::PokeParty( mpAppHeap->GetDeviceHeap() );
    mpBoxPokeParaFunc = GFL_NEW( mpAppHeap->GetDeviceHeap() ) PokePara();
    mpBoxPokeParaFunc->Initialize(  mpAppParam->pBoxPokemon , mpAppHeap->GetDeviceHeap()  );
    mpBoxPokemonMoveFunction = GFL_NEW( mpAppHeap->GetDeviceHeap() ) BoxPokemonMoveFunction();
    mpBoxPokemonMoveFunction->Setup( mpAppParam,mpBoxAppSetup,mpBoxPokeParaFunc  );

    //   ResetReloadIcon();
    mReadSeq = LOCAL_READ_START;

  case LOCAL_READ_START:

    mReadSeq = LOCAL_READ_WAIT;

  case LOCAL_READ_WAIT:
    //  メニューカーソル作成
    {
      mpMenuCursor = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::tool::MenuCursor( mpAppHeap, &mpMenuCursorLytDataBuffer );
    }
    mReadSeq = LOCAL_CLOSE_WAIT;

  case LOCAL_CLOSE_WAIT:

    if( mpMenuCursor->LoadResource() == false )
    {
      break;
    }

    //  汎用メニューウィンドウ
    {
      mpMenuWindow = GFL_NEW( mpAppHeap->GetDeviceHeap()) app::tool::MenuWindow( mpAppHeap, mpAppRenderingManager, mpMenuCursorLytDataBuffer,
                                                                                 app::util::G2DUtil::SETUP_LOWER, mpBoxAppSetup->GetMenuWindowResource() );
#if PM_DEBUG
      mpMenuWindow->Debug_YesNoPosAssertInvalid();
#endif//PM_DEBUG
    }
    mReadSeq = LOCAL_POKEICON_CREATE;
    break;

  case LOCAL_POKEICON_CREATE://
    if(mpAppParam->pP2PTradeRequestClient){//通信の場合
      if( !mpBoxAppSetup->IsResourceLoaded( GARC_boxLyt_chtlow_applyt_COMP) ){
        break;
      }
    }

    {
      void* pData = mpBoxAppSetup->GetBinLinkReadBuff(BL_IDX_BOX_PACK_FLDLOW2_APPLYT);
      mpBoxLowFolderDraw = GFL_NEW(mpAppHeap->GetSystemHeap()) BoxLowFolderDraw(mpAppHeap, mpAppRenderingManager);
      mpBoxLowFolderDraw->Init( mpAppParam, pData, mpBoxAppSetup);
      mpBoxLowFolderDraw->SetUIDrawListener(this);
    }


    {
      if(mpAppParam->pP2PTradeRequestClient){//通信の場合

        mpBoxChtLowDrawBk = GFL_NEW(mpAppHeap->GetSystemHeap()) BoxLowChtDraw(mpAppHeap, mpAppRenderingManager);
        mpBoxChtLowDrawBk->Init(mpBoxAppSetup->GetArcReadBuff(GARC_boxLyt_chtlow_applyt_COMP), mpAppParam->pMsgData ,  mpMenuCursorLytDataBuffer);
      }


      mpBoxBgLowDraw = GFL_NEW(mpAppHeap->GetSystemHeap()) BoxLowBgDraw(mpAppHeap, mpAppRenderingManager);

      {
        void* pData = mpBoxAppSetup->GetBinLinkReadBuff(BL_IDX_BOX_PACK_MINLOW2_APPLYT);

        switch( mpAppParam->type ){
        case BOX_CALL_PC:        //パソコン
        case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択
        case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
        case BOX_CALL_RESORT_GIM:   //ポケリゾート
          mpBoxBgLowDraw->Init( mpAppParam, pData , mpBoxAppSetup, false);
          break;
        case BOX_CALL_TRADE:    //P2P交換
        case BOX_CALL_FIELD_TRADE:  //フィールド交換
        case BOX_CALL_GTS:      //GTS
        case BOX_CALL_MIRACLE:   //ミラクル交換
          mpBoxBgLowDraw->Init( mpAppParam, pData , mpBoxAppSetup, true);
          break;
        }
      }

      mbInitEnd=true;

      mpBoxBgLowDraw->SetMenuCursor(mpMenuCursor);
      mpBoxBgLowDraw->DrawTeamName(mpAppParam->nowBattleTray);
      mpBoxBgLowDraw->AddSubView( mpMenuWindow );
      mpBoxBgLowDraw->PushButtonMode(-1, true);
      mpBoxBgLowDraw->PushButtonType(-1, true);

      if(mpAppParam->pP2PTradeRequestClient){
        //      RequestChangePoke();
        
        if(!mpAppParam->bAllBoxMode){
          mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_CHAT_FORCE ,0);
        }
      }

      NetworkSendMessage( msg_chat_01_13-msg_chat_01_01 );
      //innerclass列挙していく
      mDInnerNormalListener.SetThis(this);
      mDInnerNoneListener.SetThis(this);

      mInnerBoxP2pTradeEndListener.SetThis(this);
      mInnerBoxItemEndListener.SetThis(this);
      mInnerBoxAllBoxEndListener.SetThis(this);
      mInnerBoxBattleTeamEndListener.SetThis(this);
      mInnerBoxWallPaperEndListener.SetThis(this);
      mInnerBoxResortEndListener.SetThis(this);
      mInnerBoxNormalEndListener.SetThis(this);
      mInnerBoxExitEndListener.SetThis(this);
      mInnerBoxTeamNameEndListener.SetThis(this);


#if defined(GF_PLATFORM_CTR)
      OHNO_PRINT("BoxLowBgFrame::InitFunc End %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif
      return applib::frame::RES_FINISH;
    }
    //  break;
  }
  return applib::frame::RES_CONTINUE;


}


void BoxLowBgFrame::SetListener(void)
{
  mpBoxBgLowDraw->SetUIDrawListener(this);
//  PageChange( 0 );
  mpBoxAppSetup->GetBoxSimpleAccesser()->SetBoxDrawPartsListener(mpBoxBgLowDraw);

}

bool BoxLowBgFrame::IsPokeIconInit(void)
{
//アイコン初期化完了
  if(mpBoxBgLowDraw){
    return mpBoxBgLowDraw->IsPokeIconInit();
  }
  return false;
}



//------------------------------------------------------------------
/**
 * @brief	  バトルトレイのアイコン描画
 */
//------------------------------------------------------------------
void BoxLowBgFrame::drawPokeIconBattle(void)
{
  // mpBoxAppSetup->GetBoxSimpleAccesser()->TumeruBattleBoxSimpleData();

  mpBoxBgLowDraw->DrawPokeIconAll(mpAppParam->nowDispTray, mpAppParam->nowBattleTray);
}


//------------------------------------------------------------------
/**
 * @brief	  ポケモンアイコンALL
 */
//------------------------------------------------------------------
void BoxLowBgFrame::drawPokeIconAll(void)
{
  mpBoxBgLowDraw->DrawPokeIconAll(mpAppParam->nowDispTray, mpAppParam->nowBattleTray);
}


//------------------------------------------------------------------
/**
 * @brief	  ポケモンアイコンBox再描画
 *
 * @note
 *  スクロール終了時にしか呼ばれてなかったので、
 *  専用処理としました 2016/02/24 by nakahiro
 */
//------------------------------------------------------------------
void BoxLowBgFrame::drawPokeIconBox(void)
{
  int i;
  for(i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconChangeAll(mpAppParam->nowDispTray,i);
  }
}


//------------------------------------------------------------------
/**
 * @brief	  jumpする前にやるべき処理
 */
//------------------------------------------------------------------
void BoxLowBgFrame::celljumpBeforeFunc(void)
{
  mpBoxBgLowDraw->SetUIListener( &mDInnerNoneListener );//何も動かさない
  mSelectPagePos = GetNowTrayNo();  //位置を記憶

  if(mCursorPos >= Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS){ // 全体ＢＯＸをさしていたらページをさすように
    mSelectPagePos = mpAppParam->nowDispTray;
  }

  mSelectCursorPos = GetNowCursorNo();
  mbSelecting = true;//選択している状態である
}

//------------------------------------------------------------------
/**
 * @brief	  jump戻った時にやるべき処理
 */
//------------------------------------------------------------------
void BoxLowBgFrame::celljumpAfterFunc(void)
{
	mpBoxBgLowDraw->MouseModeEnd();
  mpBoxBgLowDraw->ResetFunc();
  mpBoxBgLowDraw->SetVisibleBoxTrayChangeCursor( true );
  mpBoxBgLowDraw->ResetMainCursor();

  mbSelecting = false;//選択していない
  mpBoxBgLowDraw->SetUIListener(&mDInnerNormalListener);
  
  RequestChangePoke();  //現カーソルポケモン表示更新
}


//------------------------------------------------------------------
/**
 * @brief	  boxノーマルステートの作成共通処理
 * @return	BoxNormalState
 */
//------------------------------------------------------------------

BoxNormalState* BoxLowBgFrame::CreateBoxNormalState(void)
{
  BoxNormalState* pBoxState;

  pBoxState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxNormalState >( GetManager() );
  pBoxState->SetParam( &mInnerBoxNormalEndListener,mpAppParam, mpBoxAppSetup,
                       mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow,mpBoxBgLowDraw );
  pBoxState->LocalSetup( mpBoxLowFolderDraw );
  pBoxState->SetBoxUppStatusListener(mpBoxUppStatusListener);

#if PM_DEBUG
  mpStaticBoxNormalState = pBoxState;
#endif
  
  return pBoxState;
}


//------------------------------------------------------------------
/**
 * @brief	  boxチームの名前ステートの作成共通処理
 * @return	BoxTeamNameState
 */
//------------------------------------------------------------------

BoxTeamNameState* BoxLowBgFrame::CreateBoxTeamNameState(void)
{
  BoxTeamNameState* pBoxTeamNameState;
  pBoxTeamNameState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxTeamNameState >( GetManager() );
  pBoxTeamNameState->SetParam( &mInnerBoxTeamNameEndListener,mpAppParam, mpBoxAppSetup,
                               mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow, mpBoxBgLowDraw );
	return pBoxTeamNameState;
}


//------------------------------------------------------------------
/**
 * @brief	  boxリゾートステートの作成共通処理
 * @return	BoxResortState
 */
//------------------------------------------------------------------

BoxResortState* BoxLowBgFrame::CreateBoxResortState(void)
{
  BoxResortState* pBoxState;

  pBoxState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxResortState >( GetManager() );
  pBoxState->SetParam( &mInnerBoxResortEndListener,mpAppParam, mpBoxAppSetup,
                       mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow,mpBoxBgLowDraw );
  pBoxState->LocalSetup( mpBoxLowFolderDraw );
  pBoxState->SetBoxUppStatusListener(mpBoxUppStatusListener);
  return pBoxState;
}


//------------------------------------------------------------------
/**
 * @brief	  boxリゾートステートの作成共通処理
 * @return	BoxResortState
 */
//------------------------------------------------------------------

BoxAllBoxState* BoxLowBgFrame::CreateBoxAllBoxState(void)
{

  BoxAllBoxState*  pBoxAllBoxState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxAllBoxState >( GetManager() );
  pBoxAllBoxState->SetParam( &mInnerBoxAllBoxEndListener,mpAppParam, mpBoxAppSetup,
                             mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow,mpBoxBgLowDraw );
  pBoxAllBoxState->AllBoxLocalSetup( mpBoxLowFolderDraw );
  pBoxAllBoxState->SetBoxUppStatusListener(mpBoxUppStatusListener);
  return pBoxAllBoxState;
  
}


//------------------------------------------------------------------
/**
 * @brief	  boxバトルチームステートの作成共通処理
 * @return	BoxBattleTeamState
 */
//------------------------------------------------------------------

BoxBattleTeamState* BoxLowBgFrame::CreateBoxBattleTeamState(void)
{
  BoxBattleTeamState* pBoxBattleTeamState;
  pBoxBattleTeamState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxBattleTeamState >( GetManager() );
  pBoxBattleTeamState->SetParam( &mInnerBoxBattleTeamEndListener,mpAppParam, mpBoxAppSetup,
                                 mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow,mpBoxBgLowDraw );
  return pBoxBattleTeamState;
}



//------------------------------------------------------------------
/**
 * @brief	  box終了ステートの作成共通処理
 * @return	BoxExitState
 */
//------------------------------------------------------------------

BoxExitState* BoxLowBgFrame::CreateBoxExitState(void)
{
  BoxExitState* pBoxExitState;
  pBoxExitState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxExitState >( GetManager() );
  pBoxExitState->SetParam( &mInnerBoxExitEndListener,mpAppParam, mpBoxAppSetup,
                           mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow, mpBoxBgLowDraw );
  return pBoxExitState;
}


//------------------------------------------------------------------
/**
 * @brief	  boxマーク付けステートの作成共通処理
 * @return	BoxExitState
 */
//------------------------------------------------------------------

BoxMarkState* BoxLowBgFrame::CreateBoxMarkState(void)
{
  BoxMarkState* pBoxMarkState;
  pBoxMarkState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxMarkState >( GetManager() );
  pBoxMarkState->SetParam( NULL,mpAppParam, mpBoxAppSetup,
                           mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow,mpBoxBgLowDraw );
	return pBoxMarkState;
}


//------------------------------------------------------------------
/**
 * @brief	  box escape付けステートの作成共通処理
 * @return	BoxExitState
 */
//------------------------------------------------------------------

BoxEscapeState* BoxLowBgFrame::CreateBoxEscapeState(void)
{
  BoxEscapeState* pEscapeState;
  pEscapeState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxEscapeState >( GetManager() );
  pEscapeState->SetParam( NULL,mpAppParam, mpBoxAppSetup, mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow,mpBoxBgLowDraw );
	return pEscapeState;
}

//------------------------------------------------------------------
/**
 * @brief	  box アイテム選択ステートの作成共通処理
 * @return	BoxExitState
 */
//------------------------------------------------------------------

BoxItemState* BoxLowBgFrame::CreateBoxItemState(void)
{
  BoxItemState* pBoxItemState;
  pBoxItemState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxItemState >( GetManager() );
  pBoxItemState->SetParam( &mInnerBoxItemEndListener,mpAppParam, mpBoxAppSetup,
                           mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow,mpBoxBgLowDraw );
  return pBoxItemState;
}


//------------------------------------------------------------------
/**
 * @brief	    box 通信交換ステートの作成共通処理
 * @attention	通信リスナーから返事をもらうのでメンバ変数
 */
//------------------------------------------------------------------

void BoxLowBgFrame::CreateBoxP2pTradeState(void)
{
  mpBoxP2pTradeState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxP2pTradeState >( GetManager() );
  mpBoxP2pTradeState->SetParam( &mInnerBoxP2pTradeEndListener,mpAppParam, mpBoxAppSetup,
                                mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow,mpBoxBgLowDraw );
  mpBoxP2pTradeState->SetBoxUppStatusListener(mpBoxUppStatusListener);
  mpBoxP2pTradeState->BoxP2pTradeLocalSetup( mpBoxChtLowDrawBk );
}


//------------------------------------------------------------------
/**
 * @brief	    box 壁紙ステートの作成共通処理
 * @attention	通信リスナーから返事をもらうのでメンバ変数
 */
//------------------------------------------------------------------

BoxWallpaperState* BoxLowBgFrame::CreateBoxWallpaperState(void)
{
  BoxWallpaperState* pWallState = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxWallpaperState >( GetManager() );
  pWallState->SetParam( &mInnerBoxWallPaperEndListener,mpAppParam, mpBoxAppSetup,
                        mpBoxPokeParaFunc, mpBoxPokemonMoveFunction, this, mpMenuWindow, mpBoxBgLowDraw );
  return pWallState;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BoxLowBgFrame::UpdateFunc(void)
{
#if PM_DEBUG
  mpAppParam->cursorPos = mCursorPos;
  if(mDebugSeqBackup !=  GetSubSeq()){
    OHNO_PRINT("mBOXSEQ %d\n",GetSubSeq());
    mDebugSeqBackup=GetSubSeq();
  }
#endif

  if(GetSubSeq()==MM_SEQ_NONE){  //検索から戻ったときは検索に行く
    if(mpAppParam->bAllBoxMode){
//      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA );
      mpBoxBgLowDraw->SetCursorPos(mCursorPos,-1);  //1614
      SetSubSeq(MM_SEQ_FOLDER_INIT_RE);
    }
  }

  {
    switch(GetSubSeq()){
    case MM_SEQ_NONE:
      if(mpAppParam->mbNetEndRequest){
        SetSubSeq(MM_SEQ_EXIT_REQ);
        break;
      }
      SetSubSeq( MM_SEQ_ICONLOAD );
      //      break;
    case MM_SEQ_ICONLOAD:
      if(!mpBoxBgLowDraw->IsPokeIconInit()){
        break;
      }
  //    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA );
      mpBoxBgLowDraw->SetCursorPos(mCursorPos,-1);
      SetSubSeq( MM_SEQ_LOOP );
      break;
    case MM_SEQ_LOOP:
      if(!mbFadeEnd){
        OHNO_PRINT("fadewait\n");
        break;
      }
      mpAppParam->bAllBoxMode=false;
//      if( mpAppParam->type==BOX_CALL_PC  ){
//        if(!mpAppParam->pBoxData->IsTutorialEnd()){  //GF調整要望[367]
//          InfoDispStart();
//          mpAppParam->pBoxData->SetTutorialEnd();
//          break;
//        }
//      }
      celljumpAfterFunc();
      SetSubSeq(MM_SEQ_LOOP_WAIT);
      //      break;
    case MM_SEQ_LOOP_WAIT:     //メインループ
      if(mpAppParam->mbNetEndRequest){
        SetSubSeq(MM_SEQ_EXIT_REQ);
        break;
      }

      if(!mpBoxBgLowDraw->IsHoldTouchPanel()){  //タッチ操作できて、タッチでないからカーソルを出す
        mpBoxBgLowDraw->ResetMainCursorVisible();
      }


      break;
    case MM_SEQ_END_TIMER:
      mpAppParam->outJumpMode = App::Box::BOXJUMP_CANCEL;
      mbEndReq=true;
      OnTradeExitSuccess();
      SetSubSeq(MM_SEQ_END);
      break;
    case MM_SEQ_END:
      return applib::frame::RES_FINISH;
      ////ノーマル移動
    case MM_SEQ_NORMAL_SHORTCUT: //ショートカット
      celljumpBeforeFunc();
      {
        BoxNormalState* pBoxState = CreateBoxNormalState();
        pBoxState->ShortcutStart(GetNowTrayNo(), GetNowCursorNo());
        SetSubSeq(MM_SEQ_NORMAL_END);
      }
      break;
    case MM_SEQ_NORMAL_MENU://メニューから
      celljumpBeforeFunc();
      {
        BoxNormalState* pBoxState = CreateBoxNormalState();
        pBoxState->ModeStart(GetNowTrayNo(), GetNowCursorNo());
        SetSubSeq(MM_SEQ_NORMAL_END);
      }
      break;
    case MM_SEQ_NORMAL_RANGE://範囲
      celljumpBeforeFunc();
      {
        BoxNormalState* pBoxState = CreateBoxNormalState();
        pBoxState->RangeStart(GetNowTrayNo(), GetNowCursorNo());
        SetSubSeq(MM_SEQ_NORMAL_END);
      }
      break;
    case MM_SEQ_NORMAL_END:   //ノーマル移動からの戻りはここ
      mpBoxBgLowDraw->TemotiPokeIconAnimEnd();
      mpBoxAppSetup->GetBoxSimpleAccesser()->AnimEnd();
      mpBoxBgLowDraw->LiftPokeIconClear();

      switch(mBoxNormalStateRet){
      case BoxStateBasis::BOXSTATE_NONE:   //シーケンスはそのまま
        break;
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
        SetSubSeq(MM_SEQ_MENU_CANCEL);  // @fix niji GFNMCat[254]: 手持ちフレーム移動待ちへ
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //アプリJUMP
        mbEndReq=true;
        break;
      case BoxStateBasis::BOXSTATE_ITEM:  //アイテムへいく
        SetSubSeq(MM_SEQ_ITEM_INIT);
        break;
      case BoxStateBasis::BOXSTATE_MARK:
        SetSubSeq(MM_SEQ_MARK_INIT);
        break;
      case BoxStateBasis::BOXSTATE_STATUS:
        SetStatusJump();
        break;
      case BoxStateBasis::BOXSTATE_ESCAPE:
        SetSubSeq(MM_SEQ_ESCAPE_INIT);
        break;
      default:
        GFL_ASSERT(0);
        break;
      }
      break;
      ///  チームの名前付け
    case MM_SEQ_BOXMENU_START:
      celljumpBeforeFunc();
      {
        BoxTeamNameState* pBoxTeamNameState = CreateBoxTeamNameState();
        pBoxTeamNameState->ModeStart(mpAppParam->nowBattleTray, mCursorPos);
        SetSubSeq(MM_SEQ_BOXMENU_LOOP);
      }
      break;
    case MM_SEQ_BOXMENU_LOOP:
      switch(mBoxTeamNameStateRet){
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
        SetSubSeq(MM_SEQ_LOOP);
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
        mbEndReq=true;
        break;
      }
      break;
    case MM_SEQ_RESORT_SHORTCUT://リゾートショートカット
      celljumpBeforeFunc();
      {
        BoxResortState* pBoxState = CreateBoxResortState();
        pBoxState->ShortcutStart(GetNowTrayNo(), GetNowCursorNo());
        SetSubSeq(MM_SEQ_RESORT_END);
      }
      break;
    case MM_SEQ_RESORT_MENU://リゾートメニュー
      celljumpBeforeFunc();
      {
        BoxResortState* pBoxState = CreateBoxResortState();
        pBoxState->ModeStart(GetNowTrayNo(), GetNowCursorNo());
        SetSubSeq(MM_SEQ_RESORT_END);
      }
      break;
    case MM_SEQ_RESORT_RANGE://リゾート範囲
      celljumpBeforeFunc();
      {
        BoxResortState* pBoxState = CreateBoxResortState();
        pBoxState->RangeStart(GetNowTrayNo(), GetNowCursorNo());
        SetSubSeq(MM_SEQ_RESORT_END);
      }
      break;
    case MM_SEQ_RESORT_END:
      mpBoxBgLowDraw->TemotiPokeIconAnimEnd();
      mpBoxAppSetup->GetBoxSimpleAccesser()->AnimEnd();
      mpBoxBgLowDraw->LiftPokeIconClear();
      switch(mBoxResortStateRet){
      case BoxStateBasis::BOXSTATE_NONE:   //シーケンスはそのまま
        break;
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
        SetSubSeq(MM_SEQ_LOOP);
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
        mbEndReq=true;
        break;
      case BoxStateBasis::BOXSTATE_ITEM:
        SetSubSeq(MM_SEQ_ITEM_INIT);
        break;
      case BoxStateBasis::BOXSTATE_MARK:
        SetSubSeq(MM_SEQ_MARK_INIT);
        break;
      case BoxStateBasis::BOXSTATE_STATUS:
        SetStatusJump();
        break;
      }
      break;
    case MM_SEQ_BATTLE_RANGE:  ///バトルチーム選択
      celljumpBeforeFunc();
      {
        BoxBattleTeamState* pBoxBattleTeamState = CreateBoxBattleTeamState();
        pBoxBattleTeamState->RangeModeStart(GetNowTrayNo(),GetNowCursorNo() );
        SetSubSeq(MM_SEQ_BATTLE_IN_LOOP);
      }
      break;
    case MM_SEQ_BATTLE_MENU:
      celljumpBeforeFunc();
      {
        BoxBattleTeamState* pBoxBattleTeamState = CreateBoxBattleTeamState();
        pBoxBattleTeamState->ModeStart(GetNowTrayNo(),GetNowCursorNo() );
        SetSubSeq(MM_SEQ_BATTLE_IN_LOOP);
      }
      break;

    case MM_SEQ_BATTLE_IN:
      celljumpBeforeFunc();
      {
        BoxBattleTeamState* pBoxBattleTeamState = CreateBoxBattleTeamState();
        pBoxBattleTeamState->CarryStart(GetNowTrayNo(),GetNowCursorNo() );
        SetSubSeq(MM_SEQ_BATTLE_IN_LOOP);
      }
      break;
    case MM_SEQ_BATTLE_IN_LOOP:
      mpBoxAppSetup->GetBoxSimpleAccesser()->AnimClear();
      
      switch(mBoxBattleTeamStateRet){
      case BoxStateBasis::BOXSTATE_NONE:   //シーケンスはそのまま
        break;
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
//        SetSubSeq(MM_SEQ_LOOP);
        SetSubSeq(MM_SEQ_MENU_CANCEL);  // @fix niji GFNMCat[963]: 矢印の表示が早くなるので移動を待つ
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
        mbEndReq=true;
        break;
      case BoxStateBasis::BOXSTATE_ITEM:
        SetSubSeq(MM_SEQ_ITEM_INIT);
        break;
      case BoxStateBasis::BOXSTATE_MARK:
        SetSubSeq(MM_SEQ_MARK_INIT);
        break;
      case BoxStateBasis::BOXSTATE_STATUS:
        SetStatusJump();
        break;
      }
      break;
    case MM_SEQ_EXIT_MENU:  //終了
      celljumpBeforeFunc();
      {
        BoxExitState* pBoxExitState = CreateBoxExitState();
        pBoxExitState->ModeStart();
        SetSubSeq(MM_SEQ_EXIT_END);
      }
      break;
    case MM_SEQ_EXIT_REQ:
      celljumpBeforeFunc();
      {
        BoxExitState* pBoxExitState = CreateBoxExitState();
        pBoxExitState->ModeStart();
        pBoxExitState->EndReq();
        SetSubSeq(MM_SEQ_EXIT_END);
      }
      break;
    case MM_SEQ_EXIT_END:
      switch(mBoxExitStateRet){
      case BoxStateBasis::BOXSTATE_NONE:   //シーケンスはそのまま
        break;
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
        SetSubSeq(MM_SEQ_LOOP);
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
        mpAppParam->outJumpMode = App::Box::BOXJUMP_CANCEL;
        mbEndReq=true;
        break;
      case BoxStateBasis::BOXSTATE_EXIT_STATE:
        SetSubSeq(MM_SEQ_END_TIMER);
        break;
      }
      break;
    case MM_SEQ_MARK_INIT:   //マーク付け
      celljumpBeforeFunc();
      {
        BoxMarkState* pBoxMarkState=CreateBoxMarkState();
        pBoxMarkState->ModeStart(GetNowTrayNo(),GetNowCursorNo() );
        SetSubSeq(MM_SEQ_MARK_LOOP);
      }
      break;
    case MM_SEQ_MARK_LOOP:
      //マークの戻るところは待機のみ
      SetSubSeq(MM_SEQ_LOOP);
      break;
    case MM_SEQ_ESCAPE_INIT:
      celljumpBeforeFunc();
      {
        BoxEscapeState* pEscapeState = CreateBoxEscapeState();
        pEscapeState->ModeStart(GetNowTrayNo(), GetNowCursorNo());
        SetSubSeq(MM_SEQ_ESCAPE_FINISH);
      }
      break;
    case MM_SEQ_ESCAPE_FINISH:  //逃がす処理もどり先はループのみ
      SetSubSeq(MM_SEQ_LOOP);
      break;
    case MM_SEQ_FOLDER_RANGE_INIT:
      mpAppParam->bAllBoxMode=true;
      celljumpBeforeFunc();
      {
        BoxAllBoxState*  pBoxAllBoxState = CreateBoxAllBoxState();
        pBoxAllBoxState->FolderRangeMoveStart(GetNowTrayNo(),GetNowCursorNo(),&mRangeSelect);
        mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_FOLDER ,0);
      }
      SetSubSeq( MM_SEQ_FOLDER_LOOP );
      break;
    case MM_SEQ_FOLDER_INIT_RE:
      mpAppParam->bAllBoxMode=true;
      celljumpBeforeFunc();
      {
        BoxAllBoxState*  pBoxAllBoxState = CreateBoxAllBoxState();
        pBoxAllBoxState->FolderStartRe(mpAppParam->folderCursor, GetNowTrayNo(), GetNowCursorNo());
        mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_FOLDER ,0);
      }
      SetSubSeq( MM_SEQ_FOLDER_LOOP );
      break;
    case MM_SEQ_FOLDER_INIT:
      mpAppParam->bAllBoxMode=true;
      celljumpBeforeFunc();
      {
        BoxAllBoxState*  pBoxAllBoxState = CreateBoxAllBoxState();
        pBoxAllBoxState->FolderStart(mpAppParam->folderCursor, GetNowTrayNo(), GetNowCursorNo());
        mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_FOLDER ,0);
      }
      SetSubSeq( MM_SEQ_FOLDER_LOOP );
      break;
    case MM_SEQ_FOLDER_LOOP:
      mpAppParam->folderCursor=mFolderCursorPos;
      mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_FOLDER_END ,0);
      switch( mBoxAllBoxStateRet ){
      case BoxStateBasis::BOXSTATE_NONE:   //シーケンスはそのまま
        GFL_ASSERT(0);
        break;
      case BoxStateBasis::BOXSTATE_RETURN_POKEMON_NORMAL:
        GFL_ASSERT(0);
        break;
      case BoxStateBasis::BOXSTATE_RETURN_POKEMON_RANGE:
        GFL_ASSERT(0);
        break;
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
        SetSubSeq(MM_SEQ_LOOP);
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
        mbEndReq=true;
        break;
      default:
        GFL_ASSERT(0);
        break;
      }
      break;
    case MM_SEQ_ITEM_INIT:
      celljumpBeforeFunc();
      {
        BoxItemState* pBoxItemState = CreateBoxItemState();
        pBoxItemState->ModeStart(GetNowTrayNo(), GetNowCursorNo());
        SetSubSeq(MM_SEQ_ITEM_LOOP);
      }
      break;
    case MM_SEQ_MOVE_ITEM_TOUCH:
      celljumpBeforeFunc();
      {
        BoxItemState* pBoxItemState = CreateBoxItemState();
        pBoxItemState->CarryModeStart(GetNowTrayNo(), GetNowCursorNo(), true);
        SetSubSeq(MM_SEQ_ITEM_LOOP);
      }
      break;
    case MM_SEQ_MOVE_ITEM:
      celljumpBeforeFunc();
      {
        BoxItemState* pBoxItemState = CreateBoxItemState();
        pBoxItemState->CarryModeStart(GetNowTrayNo(), GetNowCursorNo(), false);
        SetSubSeq(MM_SEQ_ITEM_LOOP);
      }
      break;
    case MM_SEQ_ITEM_FIRST:
      celljumpBeforeFunc();
      {
        BoxItemState* pBoxItemState = CreateBoxItemState();
        pBoxItemState->FirstDecide(GetNowTrayNo(), mCursorPos);
        SetSubSeq(MM_SEQ_ITEM_LOOP);
      }
      break;
    case MM_SEQ_ITEM_LOOP:
      mpBoxAppSetup->GetBoxSimpleAccesser()->AnimEnd();
      switch(mBoxItemStateRet){
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
        SetSubSeq(MM_SEQ_LOOP);
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
        mbEndReq=true;
        break;
      }
      break;
    case MM_SEQ_TRADE_START:
      celljumpBeforeFunc();
      CreateBoxP2pTradeState();
      mpBoxP2pTradeState->ModeStart(GetNowTrayNo(),GetNowCursorNo() );
      SetSubSeq(MM_SEQ_TRADE_WAIT);
      break;
    case MM_SEQ_TRADE_CHAT:
      celljumpBeforeFunc();
      CreateBoxP2pTradeState();
      mpBoxP2pTradeState->ChatStart();
      SetSubSeq(MM_SEQ_TRADE_WAIT);
      break;
    case MM_SEQ_TRADE_WAIT:
      switch(mBoxP2pTradeStateRet){
      case BoxStateBasis::BOXSTATE_NONE:   //シーケンスはそのまま
        break;
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
        SetSubSeq(MM_SEQ_LOOP);
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
        mbEndReq=true;
        break;
      }
      break;
    case MM_SEQ_PAPER_INIT:
      celljumpBeforeFunc();
      {
        BoxWallpaperState*  pWallState = CreateBoxWallpaperState();
        pWallState->ModeStart( mpAppParam->nowDispTray  , GetNowCursorNo());
        SetSubSeq(MM_SEQ_PAPER_LOOP);
      }
      break;
    case MM_SEQ_PAPER_LOOP:
      switch(mBoxWallPaperStateRet){
      case BoxStateBasis::BOXSTATE_RETURN:  //もどる
        SetSubSeq(MM_SEQ_LOOP);
        break;
      case BoxStateBasis::BOXSTATE_APP_JUMP:  //JUMP
        mbEndReq=true;
        break;
      case BoxStateBasis::BOXSTATE_ALLBOX:  //フォルダーへ
				mpAppParam->folderCursor=mpAppParam->nowDispTray+1;
        SetSubSeq(MM_SEQ_FOLDER_INIT);
        break;
      }
      break;

    // @fix niji GFNMCat[254]: 手持ちトレイの移動待ちシーケンスを追加
    case MM_SEQ_MENU_CANCEL:
      mpBoxBgLowDraw->ResetTemotiTray();
      SetSubSeq(MM_SEQ_MENU_CANCEL_WAIT);
      /* FALL THROUGH */
    case MM_SEQ_MENU_CANCEL_WAIT:
      if( mpBoxBgLowDraw->IsTemochiFrameMove() == false )
      {
        SetSubSeq(MM_SEQ_LOOP);
      }
      break;
    case MM_SEQ_TEAM_PAGECHANGE_LOOP:  // GFNMCat[1551]チーム切り替えシーケンス追加
			mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
      if(!mpBoxBgLowDraw->IsTeamFrameMove()){
        SetSubSeq(MM_SEQ_LOOP);
      }
      break;
    case MM_SEQ_PAGECHANGE_LOOP:
			mpBoxBgLowDraw->SetUIListener(&mDInnerNoneListener);
      if(!mpBoxBgLowDraw->IsPageFrameMove()){
        mpBoxBgLowDraw->SetUIListener(&mDInnerNormalListener);
        SetSubSeq(MM_SEQ_LOOP_WAIT);
      }
			break;
    case MM_SEQ_FIND_JUMP:
      mpBoxBgLowDraw->SetUIListener( &mDInnerNoneListener );//何も動かさない
      NetworkSendMessage( msg_chat_01_12-msg_chat_01_01 );
      mpAppParam->outJumpMode = App::Box::BOXJUMP_FIND;
      mpAppParam->outTray = (u8)mpAppParam->nowDispTray;
      mpAppParam->outPos = (u8)mCursorPos;
      mbEndReq=true;
      break;
    default:
      GFL_ASSERT(0);  //これ以外の遷移はない
      break;
    }
    mpBoxBgLowDraw->Update();
  }

  return applib::frame::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	  複数選択の描画範囲きめるところ
 */
//------------------------------------------------------------------
void BoxLowBgFrame::RangeSelectStretch(RangeData* pRange )
{
  if(mCursorPos < Savedata::BoxPokemon::TRAY_POKE_MAX)
  {
    if(mSelectCursorPos < Savedata::BoxPokemon::TRAY_POKE_MAX)
    {
      pRange->skip = mpBoxBgLowDraw->RangeSelectStretch( mSelectCursorPos % 6,  mSelectCursorPos / 6  , mCursorPos % 6 , mCursorPos / 6);
    }
  }
  else if(mCursorPos < Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS)
  {
    if( (mSelectCursorPos >= Savedata::BoxPokemon::TRAY_POKE_MAX)  && (mSelectCursorPos < Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS))
    {
      pRange->skip = mpBoxBgLowDraw->RangeSelectStretch( mSelectCursorPos % 6,  mSelectCursorPos / 6  , mCursorPos % 6 , mCursorPos / 6);
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 */
//------------------------------------------------------------------
void BoxLowBgFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  mpBoxBgLowDraw->DrawTree(no);
}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result BoxLowBgFrame::EndFunc(void)
{
  if( mpMenuWindow != NULL )
  {
    if( mpMenuWindow->Create() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
  }
  // 交換のときフォルムチェンジが必要なポケモンはここで処理する
  switch( mpAppParam->type ){
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
    if(mpAppParam->outJumpMode == BOXJUMP_OK || mpAppParam->outJumpMode == RETURN_TEMOTI ){
      mpBoxPokemonMoveFunction->GetPokemonPokemonParam( GetNowTrayNo(), mCursorPos, mpAppParam->pSelectPokemonParam);
      if(mpBoxPokeParaFunc->CheckAndChangeFormChangePokemon(mpAppParam->pSelectPokemonParam, mpAppParam->pZukanData  )){
        mpBoxPokemonMoveFunction->SetPokemonPokemonParam(GetNowTrayNo(), mCursorPos, mpAppParam->pSelectPokemonParam);
      }
      OHNO_PRINT("フォルムチェンジルーチン通過 %d %d\n", mpAppParam->pSelectPokemonParam->GetMonsNo(), mpAppParam->pSelectPokemonParam->GetFormNo());
    }
    break;
  }

  mpBoxPokemonMoveFunction->GetPokemonPokemonParam( GetNowTrayNo(), mCursorPos, mpAppParam->pSelectPokemonParam);

  mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_EndFunc(mpAppParam->pTemochi2);




  if(mpBoxLowFolderDraw){
    mpBoxLowFolderDraw->End();
  }
  GFL_SAFE_DELETE(mpBoxLowFolderDraw);

  if(mpBoxChtLowDrawBk){
    mpBoxChtLowDrawBk->End();
  }
  GFL_SAFE_DELETE(mpBoxChtLowDrawBk);

  if(mpBoxBgLowDraw){
    mpBoxBgLowDraw->End();
  }
  GFL_SAFE_DELETE(mpBoxBgLowDraw);
  mpBoxPokemonMoveFunction->Release();
  GFL_SAFE_DELETE(mpBoxPokemonMoveFunction);


  GFL_SAFE_DELETE( mpMenuWindow );

  if(mpMenuCursor){
    mpMenuCursor->DeleteResource();
  }
  GFL_SAFE_DELETE( mpMenuCursor );

  GFL_SAFE_DELETE( mpBoxPokeParaFunc );
  GflHeapFreeMemory( mpArcReadBuff );
  GFL_SAFE_DELETE(mpPartyTemp);
  GFL_SAFE_DELETE(mpCoreTemp);
  return applib::frame::RES_FINISH;
}


void BoxLowBgFrame::CursorAction(int udlr)
{
  if(mpBoxBgLowDraw){
    mCursorPos = mpBoxBgLowDraw->SetCursorPos(mCursorPos, udlr);

    RequestChangePoke();
  }
}


void BoxLowBgFrame::PageChangeDirectSelectTray(void)
{
  //手持ちでない場合通常トレイを元に戻す
  if(mSelectCursorPos < Savedata::BoxPokemon::TRAY_POKE_MAX || (mSelectCursorPos >= Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS)){
    PageChangeDirect(mSelectPagePos);
  }
  else{
    mpAppParam->nowBattleTray=mSelectPagePos;
    BattleInPageChange( 0 );
  }

}


void BoxLowBgFrame::PageChangeDirectNowTray(void)
{
  PageChangeDirect(mpAppParam->nowDispTray);
}



void BoxLowBgFrame::PageChangeDirect( int tray)
{
  GFL_ASSERT(tray>=0);
  GFL_ASSERT(tray<32);
  if((tray < 0) || (tray >= 32)){
    return;
  }
  
  mpAppParam->nowDispTray = tray;
  RequestChangePoke();

  if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM)
  {
    ItemModeShutdown();
    ItemModeSetup();
  }
  mpBoxBgLowDraw->DrawBoxName();
  mpBoxBgLowDraw->BgChange( mpAppParam->pBoxData->GetWallPaper( mpAppParam->nowDispTray ) );


  drawPokeIconAll();

}



//次のページ
int BoxLowBgFrame::nextPage(int changenum)
{
  int now =  mpAppParam->nowDispTray;
  now += changenum;
  if(now < 0){
    now = (int)mpAppParam->pBoxData->GetTrayMax() - 1;
  }
  else if(now >= (int)mpAppParam->pBoxData->GetTrayMax()){
    now = 0;
  }
  return now;
}

//ページスクロール開始
void BoxLowBgFrame::PageChange( int changenum)
{
  int now = nextPage(changenum);



  mpBoxBgLowDraw->DrawPageChangeAnim(changenum);
  mpBoxBgLowDraw->DrawBoxNameBefore(now);
  mpBoxBgLowDraw->DrawPokeIconBefore(now);
  mpBoxBgLowDraw->BeforeBgChange( mpAppParam->pBoxData->GetWallPaper( now ) );

}

//ページスクロール終わり
void BoxLowBgFrame::OnPageScrollEnd(int changenum)
{
  int now = 0;
  int next = 0;

  switch(changenum){
  case BoxLowBgDrawListener::SCROLL_TEAM_RESET:
    next = getBattleChange(0);
    mpAppParam->nowBattleTray = next;
    drawPokeIconBattle();
    mpBoxBgLowDraw->DrawTeamName(next);  //名前表示
    break;
  case BoxLowBgDrawListener::SCROLL_TEAM_LEFT:
    next = getBattleChange(-1);
    mpAppParam->nowBattleTray = next;
    drawPokeIconBattle();
    mpBoxBgLowDraw->DrawTeamName(next);  //名前表示
    break;
  case BoxLowBgDrawListener::SCROLL_TEAM_RIGHT:
    next = getBattleChange(1);
    mpAppParam->nowBattleTray = next;
    drawPokeIconBattle();
    mpBoxBgLowDraw->DrawTeamName(next);  //名前表示
    break;
  case BoxLowBgDrawListener::SCROLL_PAGE_RESET:
    now = nextPage(0);
    mpAppParam->nowDispTray = now;
    RequestChangePoke();
    if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM)
    {
      ItemModeShutdown();
      ItemModeSetup();
    }
    mpBoxBgLowDraw->DrawBoxName();
    mpBoxBgLowDraw->BgChange( mpAppParam->pBoxData->GetWallPaper( mpAppParam->nowDispTray ) );
    drawPokeIconAll();
    //    SetSubSeq( MM_SEQ_ICONLOAD );
    break;
  case BoxLowBgDrawListener::SCROLL_PAGE_LEFT:
    now = nextPage(-1);
    mpAppParam->nowDispTray = now;
    RequestChangePoke();
    if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM)
    {
      ItemModeShutdown();
      ItemModeSetup();
    }
    mpBoxBgLowDraw->DrawBoxName();
    mpBoxBgLowDraw->BgChange( mpAppParam->pBoxData->GetWallPaper( mpAppParam->nowDispTray ) );
    drawPokeIconBox();
    //    SetSubSeq( MM_SEQ_ICONLOAD );
    break;
  case BoxLowBgDrawListener::SCROLL_PAGE_RIGHT:
    now = nextPage(1);
    mpAppParam->nowDispTray = now;
    RequestChangePoke();
    if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM)
    {
      ItemModeShutdown();
      ItemModeSetup();
    }
    mpBoxBgLowDraw->DrawBoxName();
    mpBoxBgLowDraw->BgChange( mpAppParam->pBoxData->GetWallPaper( mpAppParam->nowDispTray ) );
    drawPokeIconBox();
    //    SetSubSeq( MM_SEQ_ICONLOAD );
    break;
  }

}


void BoxLowBgFrame::FirstDecideNormal(void)
{
  int tray = GetNowTrayNo();
  int pos = mCursorPos;

  mpBoxPokemonMoveFunction->GetPokemon(tray,pos, mpCoreTemp);
  if(mpCoreTemp->IsNull()){ //ポケモンがいない場合
    mDInnerNormalListener.SetThis(this);
    mpBoxBgLowDraw->SetUIListener( &mDInnerNormalListener );
    return;
  }

  if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM){
    SetSubSeq(MM_SEQ_BATTLE_MENU);
  }
  else{
    switch( mpAppParam->type ){
    case BOX_CALL_PC:        //パソコン
    case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択
      SetSubSeq(MM_SEQ_NORMAL_MENU);
      break;
    case BOX_CALL_TRADE:    //P2P交換
    case BOX_CALL_FIELD_TRADE:  //フィールド交換
    case BOX_CALL_GTS:      //GTS
    case BOX_CALL_MIRACLE:   //ミラクル交換
      SetSubSeq(MM_SEQ_TRADE_START);
      break;
    case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
    case BOX_CALL_RESORT_GIM:   //ポケリゾート
      SetSubSeq(MM_SEQ_RESORT_MENU);
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
}


void BoxLowBgFrame::EditTypeChange(void)
{
  if(mpBoxBgLowDraw->IsEditTypeVisible()){

    int x = mpBoxAppSetup->GetSelectMode();

    x++;
    if(x >= BOX_CHANGE_EDITTYPE_MAX){
      x = 0;
    }
    SetEditType(x);

    Sound::PlaySE(SEQ_SE_DECIDE1);

  }

//  RequestChangePoke();  //現カーソルポケモン表示更新

}


void BoxLowBgFrame::SetEditType(int i)
{
  mpBoxBgLowDraw->PushButtonMode(i);

  if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM)
  {
    ItemModeSetup();
  }
  else{
    ItemModeShutdown();
  }
  if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM)
  {
    BattleTeamModeSetup();
  }
  else{
    BattleTeamShutdown();
  }
  RequestChangePoke();  //現カーソルポケモン表示更新

}




//インフォメーションウインドウが閉じました
void BoxLowBgFrame::BoxInfoWindowBookTypeListener::OnClose(void)
{
  mpThis->mpBoxBgLowDraw->InfoDispClose();
}



void BoxLowBgFrame::InfoDispStart(void)
{
  mBoxInfoWindowBookTypeListener.SetThis(this);
  mpBoxBgLowDraw->InfoDispStart(&mBoxInfoWindowBookTypeListener);
}


//procからのみ呼ばれる
void BoxLowBgFrame::CloseWindow(void)
{
  SetSubSeq( MM_SEQ_END );
}



void BoxLowBgFrame::RequestChangePoke(u32 param)
{
  mpBoxUppStatusListener->PokeChangeRequest(BoxUppStatusListener::UPPDISP_UPDATE_STATE,GetChangePoke(),param);
}


void BoxLowBgFrame::RequestNetChangePoke(u32 param)
{
  mpBoxUppStatusListener->PokeChangeRequest(BoxUppStatusListener::UPPDISP_NET_STATE,GetChangePoke(),param);
}


//変更があったポケモンを帰す
u16 BoxLowBgFrame::GetChangePoke(void)
{
  if(mbSelecting){
    if(mSelectCursorPos < Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS){
      OHNO_PRINT("GetChangePoke 4\n");
      return (mSelectPagePos<<8) + mSelectCursorPos;
    }
  }
  else{
    if(mCursorPos < Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS){
      if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
        if(mCursorPos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
          OHNO_PRINT("GetChangePoke 1 %d %d\n",mpAppParam->nowBattleTray,mCursorPos);
          return (mpAppParam->nowBattleTray<<8) + mCursorPos;
        }
      }
      if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM){
        if(mCursorPos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
          OHNO_PRINT("GetChangePoke 2\n");
          return (mpAppParam->nowBattleTray<<8) + mCursorPos;
        }
      }
      if(mCursorPos<Savedata::BoxPokemon::TRAY_POKE_MAX){//GFNMCat[1563]
        OHNO_PRINT("GetChangePoke 3 %d %d\n",mpAppParam->nowDispTray,mCursorPos);
        return (mpAppParam->nowDispTray<<8) + mCursorPos;
      }
      else{
        OHNO_PRINT("GetChangePoke 5 %d %d\n",mpAppParam->nowBattleTray,mCursorPos);
        return (mpAppParam->nowBattleTray<<8) + mCursorPos;
      }
    }
  }
  return EMPTY_BOXNO;
}

void BoxLowBgFrame::ResetChangePoke(void)
{
  //  mbChagePokemon=false;
  //mNetPokemonStatusType=0;
}



/*
void BoxLowBgFrame::ChangePokemonBoxLinkData(int selectpage,int selectpos,int oldpage,int oldpos)
{
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はさわらない
    return;
  }

  if((oldpos < Savedata::BoxPokemon::TRAY_POKE_MAX) && (selectpos < Savedata::BoxPokemon::TRAY_POKE_MAX)){  //どちらもBOXトレイ
    mpAppParam->pBoxData->ChangePokemon(selectpage, selectpos, oldpage, oldpos);
  }
  else if(oldpos < Savedata::BoxPokemon::TRAY_POKE_MAX){  //元がBOXトレイ
    mpAppParam->pBoxData->DeleteTeam(oldpage,oldpos);
  }
  else if(selectpos < Savedata::BoxPokemon::TRAY_POKE_MAX){  //先がBOXトレイ
    mpAppParam->pBoxData->DeleteTeam(selectpage,selectpos);
  }
  else{  //パーティー内はロックが無いので必要ない
  }



}
*/



void BoxLowBgFrame::SetPartyPokemon(int partypos, pml::pokepara::PokemonParam * pp)
{
  int x = partypos - Savedata::BoxPokemon::TRAY_POKE_MAX;

  mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_ReplaceMember(x, pp);  //置換

}






int BoxLowBgFrame::getBattleChange(int changenum)
{
  int next = mpAppParam->nowBattleTray;
  int max = Savedata::BOX::TEAM_MAX;

  switch( mpAppParam->type ){
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    max = 3;
    break;
  }

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
void BoxLowBgFrame::BattleInPageChange(int changenum)
{
  int next = getBattleChange(changenum);

  mpBoxBgLowDraw->DrawTeamAnim(changenum);
  mpBoxBgLowDraw->DrawTeamNameBefore(next);
  mpBoxBgLowDraw->DrawTeamIconBefore(next);
}


////通信リスナー

void BoxLowBgFrame::OnFirstTradeSuccess(const pml::pokepara::PokemonParam& pokemonParam)
{
  //相手の交換予定ポケモン
  mpAppParam->pTargetPokemonParam->CopyFrom(pokemonParam);

  GFL_ASSERT(mpBoxP2pTradeState);
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->OnFirstTradeSuccess(pokemonParam);
  }
}


void BoxLowBgFrame::OnTradeSuccess(void)
{
  GFL_ASSERT(mpBoxP2pTradeState);
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->OnTradeSuccess();
  }
}


void BoxLowBgFrame::OnTradeExitReceived(void)
{
  OHNO_PRINT("ここをとおった OnTradeExitReceivedx \n");
  mpAppParam->mbNetEndRequest=true;
}

void BoxLowBgFrame::OnTradeExitSuccess(void)
{
  OHNO_PRINT("ここをとおった OnTradeExitSuccess \n");
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->OnTradeExitSuccess();
  }
  else{
//    mpAppParam->outJumpMode = App::Box::BOXJUMP_CANCEL;
  ///  mbEndReq=true;
  }
  mpAppParam->mbNetEndSuccess=true;
}

void BoxLowBgFrame::OnTradeComplete(void)
{
  GFL_ASSERT(mpBoxP2pTradeState);
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->OnTradeComplete();
  }
}


void BoxLowBgFrame::OnTradeCancelReceived(void)
{
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->OnTradeCancelReceived();
  }
  //確認を行う時に、片方が先に交換ステータスを抜けていることはありえる。
  //その場合何もする必要は無い。
}


void BoxLowBgFrame::OnFinalTradeCancel(void)
{
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->OnFinalTradeCancel();
  }
  //最終確認を行う時に、片方が先に交換ステータスを抜けていることはありえる。
  //その場合何もする必要は無い。
}

//交換２回目の決定時の処理
void BoxLowBgFrame::TradeFinalDecide(void)
{
  GFL_ASSERT(mpBoxP2pTradeState);
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->TradeFinalDecide();
  }
}


void BoxLowBgFrame::OnFirstTradePokemonInvalid(bool bMyValidationError)
{
  GFL_ASSERT(mpBoxP2pTradeState);
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->OnFirstTradePokemonInvalid(bMyValidationError);
  }
}


void BoxLowBgFrame::OnFirstTradeValidationError(bool bMyValidationError)
{
  GFL_ASSERT(mpBoxP2pTradeState);
  if(mpBoxP2pTradeState){
    mpBoxP2pTradeState->OnFirstTradeValidationError(bMyValidationError);
  }
}

////------------------------------------------------------------------------------

//
void BoxLowBgFrame::TeamNameChangeOpen(void)
{
  if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM){  //バトルモードの時は
    SetSubSeq( MM_SEQ_BOXMENU_START );
  }

}

//最初のポケモン選択の時、分岐多し
void BoxLowBgFrame::FirstDecide(void)
{
  // @fix niji GFNMCat[254]: 手持ちトレイ動作中は入力を無効にする
  if( mpBoxBgLowDraw->IsTemochiFrameMove() != false )
  {
    return;
  }

  mDInnerNoneListener.SetThis(this);
  mpBoxBgLowDraw->SetUIListener( &mDInnerNoneListener );

  mpBoxPokemonMoveFunction->GetPokemon(GetNowTrayNo(), mCursorPos, mpCoreTemp);  //アイテムを持っているか確認

  if(mpBoxAppSetup->GetMoveMode() == BOX_CHANGE_SELECTMODE_GROUP){  //複数選択

    if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM)
    {  //バトルモードの時は
      SetSubSeq(MM_SEQ_BATTLE_RANGE);
    }
    else if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM)
    {
      SetSubSeq(MM_SEQ_RESORT_RANGE);
    }
    else{  //ノーマル
      SetSubSeq(MM_SEQ_NORMAL_RANGE);
    }

    return;
  }
  else{  //グループ以外ではポケモンがいない場合操作不可
    if(mpCoreTemp->IsNull()){
      mDInnerNormalListener.SetThis(this);
      mpBoxBgLowDraw->SetUIListener( &mDInnerNormalListener );
      return;
    }
  }
  if(mpBoxAppSetup->GetMoveMode() == BOX_CHANGE_SELECTMODE_SHORTCUT){  //ショートカットモード

    if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM){  //アイテムモードの時は

      SetSubSeq(MM_SEQ_MOVE_ITEM);

    }
    else if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM){  //バトルモードの時は
      mpBoxBgLowDraw->LiftPokeIconReady(GetNowTrayNo(),mCursorPos);
      SetSubSeq(MM_SEQ_BATTLE_IN);
    }
    else if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM)
    {
      SetSubSeq(MM_SEQ_RESORT_SHORTCUT);//リゾートショートカット
    }
    else{  //ノーマル
      SetSubSeq(MM_SEQ_NORMAL_SHORTCUT);

    }
    return;
  }
  else if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM){  //アイテムモードの時は
    SetSubSeq(MM_SEQ_ITEM_FIRST);
    return;
  }

  FirstDecideNormal();  //通常の決定

}



void BoxLowBgFrame::ItemModeSetup(void)
{
  mpBoxBgLowDraw->DrawPokeIconStateAll(mpAppParam->nowDispTray, mpAppParam->nowBattleTray);
  mpBoxBgLowDraw->ActiveRangeButton(false);

  if(mpBoxAppSetup->GetMoveMode() == BOX_CHANGE_SELECTMODE_GROUP){
    mpBoxBgLowDraw->PushButtonType( 0 );
  }


}


void BoxLowBgFrame::ItemModeShutdown(void)
{
  mpBoxBgLowDraw->ActiveRangeButton(true);
  mpBoxBgLowDraw->DrawPokeIconStateAll(mpAppParam->nowDispTray, mpAppParam->nowBattleTray);


}


void BoxLowBgFrame::SetFolderCursorPos(int cursor)
{
  mFolderCursorPos=cursor;
}


//０は表示しない 1-32
int BoxLowBgFrame::GetChangeFolderPoke(void)
{
  if(mFolderCursorPos < Savedata::Box::TRAY_MAX+1)
  {
    return mFolderCursorPos-1;
  }
  return EMPTY_BOXNO;
}


void BoxLowBgFrame::MoveModeChange(void)
{
  int x = mpBoxAppSetup->GetMoveMode();
  x++;
  if(x >= BOX_CHANGE_SELECTMODE_MAX){
    x = 0;
  }
  if(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM && x==2){
    x=0;
  }
  mpBoxBgLowDraw->PushButtonType(x);

  Sound::PlaySE(SEQ_SE_DECIDE1);
}



void BoxLowBgFrame::BattleTeamModeSetup(void)
{
  mpBoxBgLowDraw->DrawPokeIconStateAll(mpAppParam->nowDispTray, mpAppParam->nowBattleTray);
  mpBoxBgLowDraw->DrawPokeIconBattle(mpAppParam->nowBattleTray);

  mpBoxBgLowDraw->DrawTeamName(mpAppParam->nowBattleTray);
  mpBoxBgLowDraw->ResetL2R2Key();
}


void BoxLowBgFrame::BattleTeamShutdown(void)
{
  mpBoxBgLowDraw->DrawPokeIconStateAll(mpAppParam->nowDispTray, mpAppParam->nowBattleTray);
  mpBoxBgLowDraw->DrawPokeIconBattle(0);
  mpBoxBgLowDraw->DrawTeamName(mpAppParam->nowBattleTray);
  mpBoxBgLowDraw->ResetL2R2Key();
}


void BoxLowBgFrame::NetworkSendMessage(int btnno)
{
  if(mpAppParam->pP2PTradeRequestClient){
    gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
    pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);
    if(btnno==1){  //相手のポケモンを指す特殊処理
      pp->CopyFrom(*mpAppParam->pTargetPokemonParam);
    }
    else{
      mpBoxPokemonMoveFunction->GetPokemon( GetNowTrayNo(), mCursorPos, pp);
    }

    int no = pp->GetMonsNo();
    int message = btnno;
    if(message==1){
      message = msg_chat_01_09 - msg_chat_01_01;
    }
    if(pp->IsNull()){
      if(message == 8 || message == 10){
        no=0;
        message = msg_chat_01_13 - msg_chat_01_01;
      }
    }
    mpAppParam->pP2PTradeRequestClient->AddSendMessageIDRequest( static_cast<NetAppLib::P2PTrade::MessageID>( message ), (MonsNo)no, *pp );
    mpAppParam->pMyWantPokemonParam->CopyFrom(*pp);
    
    mpAppParam->chatPokemonSelf = no;
    mpAppParam->chatMsgSelf = message;

    mpBoxUppStatusListener->Request( BoxUppStatusListener::UPPDISP_CHAT_UPDATE,0);

    GFL_DELETE(pp);
  }
}



int* BoxLowBgFrame::GetNowCursorNoPtr(void)
{
  return &mCursorPos;
}


int BoxLowBgFrame::GetNowCursorNo(void)
{
  return mCursorPos;
}

void BoxLowBgFrame::SetNowCursorNo(int no)
{
  mCursorPos=no;
}


int BoxLowBgFrame::GetNowTrayNo(void)
{
  int tray = mpAppParam->nowDispTray;
  if(mCursorPos < Savedata::BoxPokemon::TRAY_POKE_MAX){
    tray = mpAppParam->nowDispTray;
  }
  else{
    if( App::Box::BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode() ){
      tray = (u8)mpAppParam->nowBattleTray;
    }
    else if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM){
      tray = (u8)mpAppParam->nowBattleTray;
    }
    else{
      tray=0;
    }
  }
  return tray;
}



void BoxLowBgFrame::SetSelectCursor(void)  //今のカーソル位置をセレクトに設定
{
  mSelectCursorPos=mCursorPos;
  mSelectPagePos=GetNowTrayNo();
}



//ステータスアプリにいく時の値の設定

void BoxLowBgFrame::SetStatusJump( void )
{
  mpAppParam->outJumpMode = App::Box::BOXJUMP_STATUS;
  mpAppParam->outPos = (u8)mCursorPos;
  mpAppParam->outTray = (u8)mpAppParam->nowDispTray;

  if(mCursorPos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
    if( App::Box::BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode() ){
      mpAppParam->outTray = (u8)mpAppParam->nowBattleTray;
    }
    else if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM){
      mpAppParam->outTray = (u8)mpAppParam->nowBattleTray;
    }
  }
  mbEndReq = true;
}






//--------------------------------------------------------------------------
/**
 * @brief  マウスモードが終わったとき呼ばれる
 */
//--------------------------------------------------------------------------
void BoxLowBgFrame::OnMouseModeEnd(void)
{
  //mpBoxLowFolderDrawBk->MouseModeEnd();
  mpBoxBgLowDraw->MouseModeEnd();
}


void BoxLowBgFrame::BoxBgLowDrawLiftPokeIconEnd(void)
{
  mpBoxBgLowDraw->LiftPokeIconEnd(GetNowTrayNo(),mCursorPos);
}


//--------------------------------------------------------------------------
/**
 * @brief UIViewマネージャのボタン監視モードを取得
 */
//--------------------------------------------------------------------------
void BoxLowBgFrame::GetInputExculusiveKeyMode(void)
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  m_isUIViewInputExclusiveKey = man->GetInputExclusiveKeyByButtonAnimation();
}

//--------------------------------------------------------------------------
/**
 * @brief UIViewマネージャのボタン監視モードをセット
 * @param flg   true = ON, false = OFF
 */
//--------------------------------------------------------------------------
void BoxLowBgFrame::SetInputExculusiveKeyMode( bool flg )
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  man->SetInputExclusiveKeyByButtonAnimation( flg );
}


GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




