//=============================================================================
/**
 * @file	 BoxUppPokeStatusFrame.cpp
 * @brief	 BOX上画面の表示
 * @author k.ohno
 * @date	 2015.8.8
 */
//=============================================================================
#pragma once


#include "BoxUppPokeStatusFrame.h"
#include "BoxUppPokeStatusDraw.h"
#include "BoxUppChatDraw.h"
#include "BoxUppBgDraw.h"
#include "BoxUppFolderDraw.h"
#include "BoxSimpleAccesser.h"

#include "AppLib/include/Frame/AppFrameManager.h"

#include "App/Box/include/BoxProc.h"
#include "App/Box/include/BoxAppSetup.h"

#include <Fade/include/gfl2_FadeManager.h>

#include "Sound/include/Sound.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>
#include <Print/include/SystemFont.h>
#include "App/Box/include/BoxAppParam.h"
#include "SaveData/include/ResortSave.h"

#include "model/include/gfl2_BaseCamera.h"

#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include <arc_index/message.gaix>

#include "niji_conv_header/message/msg_box.h"

#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"

// niji互換対応ポケモンチェック
#include "Battle/Regulation/include/PokeRegulation.h"

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )


enum{
  MM_SEQ_NONE,
  MM_SEQ_YESNO,
  MM_SEQ_END,
};




//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
BoxUppPokeStatusFrame::BoxUppPokeStatusFrame(void)
{
  mReadSeq=0;
  mbJudge=false;
  mbChat=false;
//  mbReDispOtherStatus=true;
	mBackupBoxNo=-1;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxUppPokeStatusFrame::~BoxUppPokeStatusFrame()
{
}


void BoxUppPokeStatusFrame::Setup( APP_PARAM* pAppParam, BoxAppSetup* pBoxAppSetup )
{
  mpAppHeap = pAppParam->mpAppHeapUpper;
  mpAppRenderingManager = pAppParam->mpAppRenderingManager;
  mpAppParam = pAppParam;
  mpBoxAppSetup = pBoxAppSetup;

#if SENDPOKE_SENDQ_
  mSendPokeSt.request = SENDPOKEMON_NONE;
#endif
}

enum{
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT,
  LOCAL_POKEICON_CREATE,
  LOCAL_POKEICON_CREATE2,
};






//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BoxUppPokeStatusFrame::InitFunc(void)
{

  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない


#if PM_DEBUG
  OHNO_PRINT("BoxUppPokeStatusFrame::InitFunc %d\n",mReadSeq);
#endif
  
 // gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;
//  int arcid = ARCID_BOX;
//  int datid = GARC_boxLyt_staupp_applyt_COMP;

  switch(mReadSeq){
  case LOCAL_OPEN_START:
    mBoxUppStatusListener.Init();
    mBoxUppStatusListener.SetThis(this);
    mpTargetParam = mpAppParam->pTargetPokemonParam;
    mpMyParam = mpAppParam->pSelectPokemonParam;
   // mpMyParam->Clear();
    mpAppRenderingManager->CreateDefaultCamera(mpAppHeap,  &m_drawEnvNode, &m_camera ,app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
    mpAppRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY,m_camera,0);
    mpAppRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY,m_camera,1);
    mpAppRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY,m_camera,2);

    mpAppCommonGrpIconData = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::tool::AppCommonGrpIconData();
    mpAppCommonGrpIconData->Initialize( mpAppHeap );

    mReadSeq = LOCAL_READ_START;
    //break;
  case LOCAL_READ_START:
    mReadSeq = LOCAL_READ_WAIT;
  case LOCAL_READ_WAIT:
    mReadSeq = LOCAL_CLOSE_WAIT;
    break;
  case LOCAL_CLOSE_WAIT:
    if( mpAppCommonGrpIconData->IsReady() == false ){
      break;
    }

    {
      App::Box::BoxUppPokeStatusDraw::INIT_PARAM  initParam;
      initParam.pLytBinData             = mpBoxAppSetup->GetBinLinkReadBuff(BL_IDX_BOX_PACK_STAUPP2_APPLYT); //mpBoxAppSetup->GetArcReadBuff(GARC_boxLyt_staupp_applyt_COMP);  //mpArcReadBuff;
      initParam.pRenderinManager        = mpAppRenderingManager;
      initParam.pHeap                   = mpAppHeap;
      initParam.pBoxMsgData             = mpAppParam->pMsgData;

      initParam.pZukanSaveData          = mpAppParam->pZukanData;
      initParam.pAppCommonGrapIconData  = mpAppCommonGrpIconData;
      initParam.pAsyncFileManager       = mpAppParam->mpAsyncFileManager;

      m_pView = GFL_NEW( mpAppHeap->GetDeviceHeap() ) App::Box::BoxUppPokeStatusDraw( initParam );

      {
        void* pData = mpBoxAppSetup->GetBinLinkReadBuff(BL_IDX_BOX_PACK_BGRUPP2_APPLYT);
        mpBoxUppBgDraw = GFL_NEW(mpAppHeap->GetDeviceHeap()) BoxUppBgDraw(  pData, mpAppRenderingManager, mpAppHeap );
        mpBoxUppBgDraw->InitFunc();
      }
      {
        void* pData = mpBoxAppSetup->GetBinLinkReadBuff(BL_IDX_BOX_PACK_FLDUPP2_APPLYT);
        mpBoxUppFolderDrawBk = GFL_NEW( mpAppHeap->GetDeviceHeap() ) App::Box::BoxUppFolderDraw( mpAppHeap,mpAppRenderingManager );
        mpBoxUppFolderDrawBk->Init(mpAppParam, pData, mpBoxAppSetup);
      }

    }

    mReadSeq = LOCAL_POKEICON_CREATE;
    break;
  case LOCAL_POKEICON_CREATE:
    //if( mpBoxUppChatDraw->InitFunc() == true )
    if(mpAppParam->pP2PTradeRequestClient){ //通信の場合
      if(mpAppParam->pBoxUppChatDraw)
      {
        mpAppParam->pBoxUppChatDraw->SetChatWindowMessageID( App::Box::BoxUppChatDraw::PLYAER_TYPE_SELF, msg_chat_01_13 );
        mpAppParam->pBoxUppChatDraw->SetChatWindowMessageID( App::Box::BoxUppChatDraw::PLYAER_TYPE_OTHER, msg_chat_01_13 );
        mpAppParam->pBoxUppChatDraw->SetPokemonStateOnOff( App::Box::BoxUppChatDraw::PLYAER_TYPE_SELF,false);
        mpAppParam->pBoxUppChatDraw->SetPokemonStateOnOff( App::Box::BoxUppChatDraw::PLYAER_TYPE_OTHER,false);

        if(mpBoxAppSetup->GetOtherMessage()!=-1){
          DispOtherMessage2( );
        }
      }
    }
    mReadSeq = LOCAL_POKEICON_CREATE2;
    
    break;
  case LOCAL_POKEICON_CREATE2:
    if( m_pView->InitFunc() == true )
    {
      return applib::frame::RES_FINISH;
    }
    break;
  }
  return applib::frame::RES_CONTINUE;
}

enum{
  SEQ_STATE_LOOP,
  SEQ_STATE_END,
  SEQ_CHAT_INIT,
  SEQ_CHAT_READ,
  SEQ_CHAT_LOOP,
  SEQ_CHAT_END,
  SEQ_STATE_TARGET,
  SEQ_CHAT_END_TARGET,
};



void BoxUppPokeStatusFrame::DelayInitLayout(void)
{
  /*
  if(mpBoxAppSetup->IsResourceLoaded(GARC_boxLyt_fldupp_applyt_COMP) && mpBoxUppFolderDrawBk==NULL){

    mpBoxUppFolderDrawBk = GFL_NEW( mpAppHeap->GetDeviceHeap() ) App::Box::BoxUppFolderDraw( mpAppHeap,mpAppRenderingManager );
    mpBoxUppFolderDrawBk->Init(mpAppParam, mpBoxAppSetup->GetArcReadBuff(GARC_boxLyt_fldupp_applyt_COMP));

  }
     */
}



void BoxUppPokeStatusFrame::ReDispOtherStatus(void)
{
  if(mpAppParam->pP2PTradeRequestClient){
   // if(mbReDispOtherStatus)
    {
      SetOtherPokemon(mpAppParam->pP2PTradeRequestClient->GetLastRecvPokemonParam());

      mpAppParam->pWantPokemonParam->Deserialize_Full( mpAppParam->pP2PTradeRequestClient->GetLastRecvPokemonParamBuffer() );

      mpAppParam->chatMsgOther = mpAppParam->pP2PTradeRequestClient->GetLastRecvMessageID();
      if(mpTargetParam){
        mpAppParam->chatPokemonOther = mpTargetParam->GetMonsNo();
        DispOtherMessage2();
        OHNO_PRINT("--Monsno %d chatno %d  \n",mpAppParam->chatPokemonOther,mpAppParam->chatMsgOther, mpAppParam->pWantPokemonParam->GetMonsNo());
      }
     // mbReDispOtherStatus=false;
    }
    if(mpMyParam && !mpMyParam->IsNull())
    {
      mpAppParam->pBoxUppChatDraw->SetPokeParam( BoxUppChatDraw::PLYAER_TYPE_SELF, mpMyParam );
      mpAppParam->pBoxUppChatDraw->SetPokemonStateOnOff( BoxUppChatDraw::PLYAER_TYPE_SELF, true );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BoxUppPokeStatusFrame::UpdateFunc(void)
{
#if PM_DEBUG
  if(mDebugSeqBackup !=  m_subSeq){
    OHNO_PRINT("BOXUPPSEQ %d\n",m_subSeq);
    mDebugSeqBackup=m_subSeq;
  }
#endif

  mBoxUppStatusListener.Update();
  mBoxUppStatusListener.UpdatePokemon();

  if(GetSubSeq()==MM_SEQ_END){
    return applib::frame::RES_FINISH;
  }
//  DelayInitLayout();
  mpBoxUppBgDraw->Update();

  if(mpBoxUppFolderDraw)
  {
    mpBoxUppFolderDraw->Update();
  }
  else
  {

    switch(m_subSeq){
    case SEQ_STATE_LOOP:
      m_pView->Update();
      break;
    case SEQ_STATE_END:
      if(mpAppParam->pBoxUppChatDraw)
      {
        OHNO_PRINT("pBoxUppChatDraw->Open 1\n");
        mpAppParam->pBoxUppChatDraw->Open();
        m_pView->Close();
        m_subSeq = SEQ_CHAT_LOOP;
      }
      break;
    case SEQ_CHAT_INIT:
      break;
    case SEQ_CHAT_READ:
      break;
    case SEQ_CHAT_LOOP:
      if(mpAppParam->pBoxUppChatDraw)
      {
        mpAppParam->pBoxUppChatDraw->UpdateTree();
      }
      break;
    case SEQ_CHAT_END:
      if(mpAppParam->pBoxUppChatDraw)
      {
        mpAppParam->pBoxUppChatDraw->Close();
      }
      OHNO_PRINT("PokemonStateReloadOpen 1\n");
      PokemonStateReloadOpen(false);
      m_subSeq = SEQ_STATE_LOOP;
      break;
    case SEQ_STATE_TARGET:
      if(mpAppParam->pBoxUppChatDraw)
      {
        mpAppParam->pBoxUppChatDraw->Close();
      }
      OHNO_PRINT("PokemonStateReloadOpen 2\n");
      PokemonStateReloadOpen(true);
      m_subSeq = SEQ_STATE_LOOP;
      break;
    case SEQ_CHAT_END_TARGET:
      if(mpAppParam->pBoxUppChatDraw)
      {
        mpAppParam->pBoxUppChatDraw->Close();
      }
      OHNO_PRINT("PokemonStateReloadOpen x\n");
      m_subSeq = SEQ_STATE_LOOP;
      break;
    }
  }

#if SENDPOKE_SENDQ_
  UpdateSendPoke();
#endif
  
  return applib::frame::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void BoxUppPokeStatusFrame::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
{
  mpBoxUppBgDraw->DrawTree(displayNo);

  if(mpBoxUppFolderDraw)
  {
    mpBoxUppFolderDraw->DrawTree(displayNo);
  }
  else{
    if(mbChat && mpAppParam->pBoxUppChatDraw){
      mpAppParam->pBoxUppChatDraw->DrawTree( displayNo );
    }
    else{
      m_pView->DrawTree( displayNo );
    }
  }
}



bool BoxUppPokeStatusFrame::StateDrawEnd2(void)
{
  if(mpBoxUppFolderDrawBk){
    mpBoxUppFolderDrawBk->End();
    GFL_SAFE_DELETE( mpBoxUppFolderDrawBk );
  }
  return true;
}



bool BoxUppPokeStatusFrame::StateDrawEnd(void)
{
  if( !m_pView->EndFunc() )
  {
    return false;
  }
  mpBoxUppBgDraw->EndFunc();
  GFL_SAFE_DELETE(mpBoxUppBgDraw);
  GFL_SAFE_DELETE( m_pView );

  //  GflHeapFreeMemory( mpArcReadBuff );

  mpAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 0 );
  mpAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 1 );
  mpAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 2 );
  mpAppCommonGrpIconData->Terminate();

  GFL_SAFE_DELETE( mpAppCommonGrpIconData );

  GFL_SAFE_DELETE(m_drawEnvNode);
  GFL_SAFE_DELETE(m_camera);


//  GFL_SAFE_DELETE(mpMyParam);
//  GFL_SAFE_DELETE(mpTargetParam);
  return true;
}




//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result BoxUppPokeStatusFrame::EndFunc(void)
{
  if( !StateDrawEnd2() )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( !StateDrawEnd() )
  {
    return applib::frame::RES_CONTINUE;
  }

  return applib::frame::RES_FINISH;
}


void BoxUppPokeStatusFrame::CloseWindow(void)
{
  SetSubSeq( MM_SEQ_END );
}

bool BoxUppPokeStatusFrame::SetDisp(u16 boxno)
{
  if(m_pView==NULL)
  {
    return false;
  }
  if( !m_pView->InitFunc()  )
  {
    return false;
  }

  

  if(boxno == EMPTY_BOXNO)
  {
    m_pView->Close();
  }
  else
  {      //  テストポケモンデータを設定
    u32 tray = ((boxno>>8)&0xff);
    u32 pos = boxno & 0xff;
 //   int pos2 = pos - Savedata::BoxPokemon::TRAY_POKE_MAX;
    int team=-1;
    bool block=false;
    bool bteam=false;
 //   bool bcreate=false;

    mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonPokemonParam(tray, pos, mpMyParam);

		bool mode =mpMyParam->StartFastMode();

    OHNO_PRINT("----------------tray %d pos %d  mons %d\n", tray,pos,mpMyParam->GetMonsNo()  );

    block = mpBoxAppSetup->GetBoxSimpleAccesser()->IsPokemonLock(tray  ,pos);
    if(!block){
      bteam = mpBoxAppSetup->GetBoxSimpleAccesser()->IsPokemonTeam(tray  ,pos);
    }

    mbLock=block;
    mbTeam=bteam;

    
    if(!mbChat){
      OHNO_PRINT("PokemonStateReloadOpen 3\n");
      PokemonStateReloadOpen(false);
    }
    else{

      OHNO_PRINT("chat \n");
      
      if(!mpMyParam->IsNull())
      {
        OHNO_PRINT("mpMyParamOK \n");

        if( mpAppParam->pBoxUppChatDraw)
        {
          OHNO_PRINT("pBoxUppChatDraw OK \n");
          mpAppParam->pBoxUppChatDraw->SetPokeParam( BoxUppChatDraw::PLYAER_TYPE_SELF, mpMyParam );
          mpAppParam->pBoxUppChatDraw->SetPokemonStateOnOff( BoxUppChatDraw::PLYAER_TYPE_SELF, true );


          
#if 1
          SetSendPokeReq();
#else
          if(mpAppParam->pP2PTradeRequestClient){//通信でポケモンを送る
            mpAppParam->pP2PTradeRequestClient->AddSendPokemonParamRequest(*mpMyParam);
          }
#endif
        }
      }
      else
      {
        if( mpAppParam->pBoxUppChatDraw){
          mpAppParam->pBoxUppChatDraw->SetPokemonStateOnOff( BoxUppChatDraw::PLYAER_TYPE_SELF, false );
        }
#if 1
        SetSendPokeReq();
#else
        if(mpAppParam->pP2PTradeRequestClient){//からでも通信でポケモンを送る
          mpAppParam->pP2PTradeRequestClient->AddSendPokemonParamRequest(*mpMyParam);
        }
#endif
      }
    }
		mpMyParam->EndFastMode(mode);

  }
  return true;
}


void BoxUppPokeStatusFrame::PokemonStateReloadOpen(bool bterget)
{
  if(bterget){
    if(!mpTargetParam->IsNull()){
      m_pView->SetParam( *mpTargetParam, false, false );
      m_pView->Open();
    }
    else{
      m_pView->Close();
    }
  }
  else{
    if(!mpMyParam->IsNull()){
      m_pView->SetParam( *mpMyParam, mbLock, mbTeam );
      m_pView->Open();
    }
    else{
      m_pView->Close();
    }
#if SENDPOKE_SENDQ_
    SetSendPokeReq();
#else
    if(mpAppParam->pP2PTradeRequestClient){//通信でポケモンを送る  ※ここは直接送る
      mpAppParam->pP2PTradeRequestClient->AddSendPokemonParamRequest(*mpMyParam);
    }
#endif
  }
}



//最終画面で自分かあいてかのポケモンステータスを出す時の関数
bool BoxUppPokeStatusFrame::SetDispNetwork(u16 stateno,u16 boxno)
{
  if(m_pView==NULL)
  {
    return false;
  }
  if( !m_pView->InitFunc()  )
  {
    return false;
  }
  if(SEQ_CHAT_LOOP == m_subSeq)
  {
    OHNO_PRINT("SetDispNetwork SEQ_CHAT_END\n");
    if(stateno==2){
      m_subSeq = SEQ_CHAT_END_TARGET;
    }
    else{
      m_subSeq = SEQ_CHAT_END;
    }
    mbChat=false;
    return false;
  }
  if(SEQ_STATE_LOOP != m_subSeq){
    return false;
  }

  if(stateno==1)
  {      //  テストポケモンデータを設定
    u32 tray = ((boxno>>8)&0xff);
    u32 pos = boxno & 0xff;
 //   int pos2 = pos - Savedata::BoxPokemon::TRAY_POKE_MAX;
    int team=-1;
    bool block=false;
    bool bteam=false;

    mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonPokemonParam(tray, pos, mpMyParam);

    block = mpBoxAppSetup->GetBoxSimpleAccesser()->IsPokemonLock(tray  ,pos);
    if(!block){
      bteam = mpBoxAppSetup->GetBoxSimpleAccesser()->IsPokemonTeam(tray  ,pos);
    }
    mbLock=block;
    mbTeam=bteam;
    if(ChangeForceState(false)){
      OHNO_PRINT("PokemonStateReloadOpen 4\n");
      PokemonStateReloadOpen(false);
    }
  }
  else if(stateno==2)
  {
    if(ChangeForceState(true)){
      OHNO_PRINT("PokemonStateReloadOpen 5\n");
      PokemonStateReloadOpen(true);
    }
  }

  return true;
}



//------------------------------------------------------------------
/**
 * @brief	  Judgeモードへの切り替え
 */
//------------------------------------------------------------------
void BoxUppPokeStatusFrame::ChangeJudgeMode(void)
{
  mbJudge = !mbJudge;
  if(mbJudge)
  {
    m_pView->SetDrawMode( BoxUppPokeStatusDraw::DRAW_MODE_JUDDGE );
  }
  else
  {
    m_pView->SetDrawMode( BoxUppPokeStatusDraw::DRAW_MODE_STATUS );
  }
}


//------------------------------------------------------------------
/**
 * @brief	  Chatモードへの切り替え
 */
//------------------------------------------------------------------
void BoxUppPokeStatusFrame::ChangeChatMode(void)
{
  OHNO_PRINT("ChangeChatMode\n");

  mbChat = !mbChat;
  if(mbChat)
  {
    if(SEQ_STATE_LOOP == m_subSeq){
      DispSelfMessage();
      ReDispOtherStatus();
      m_subSeq = SEQ_STATE_END;
    }
  }
  else
  {
    if(SEQ_CHAT_LOOP == m_subSeq){
      OHNO_PRINT("ChangeChatMode SEQ_CHAT_END\n");
      m_subSeq = SEQ_CHAT_END;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief	  Chatモードへの強制切り替え
 */
//------------------------------------------------------------------
void BoxUppPokeStatusFrame::ChangeForceChat(void)
{
  {
    OHNO_PRINT("強制切り替えよばれた %d \n",m_subSeq);
    if(SEQ_STATE_LOOP == m_subSeq){
      m_subSeq = SEQ_STATE_END;
      mbChat = true;
      OHNO_PRINT("強制切り替え実行\n");
      ReDispOtherStatus();
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief	  Statモードへの切り替え
 */
//------------------------------------------------------------------
bool BoxUppPokeStatusFrame::ChangeForceState(bool bterget)
{
  {
    if(SEQ_CHAT_LOOP == m_subSeq){
      if(!bterget){
        OHNO_PRINT("ChangeForceState SEQ_CHAT_END\n");
        m_subSeq = SEQ_CHAT_END;
      }
      else{
        m_subSeq = SEQ_STATE_TARGET;
      }
      mbChat=false;
      return false;
    }
  }
  return true;
}




//------------------------------------------------------------------
/**
 * @brief	  フォルダーモードへの切り替え
 */
//------------------------------------------------------------------
void BoxUppPokeStatusFrame::ChangeFolderMode(bool bFolder)
{
  if(bFolder)
  {
    m_pView->Close();
    mpBoxUppFolderDraw = mpBoxUppFolderDrawBk;

    //  チャット状態でチャットステート終了以外の時は表示を閉じる
    if( ( mbChat == true ) && ( m_subSeq != SEQ_CHAT_END ) )
    {
      if(mpAppParam->pBoxUppChatDraw){
        mpAppParam->pBoxUppChatDraw->Close();
      }
    }
  }
  else
  {
    m_pView->Close();
    mpBoxUppFolderDraw=NULL;

    //  チャット状態でチャットステート終了以外の時は表示を開く
    if( ( mbChat == true ) && ( m_subSeq != SEQ_CHAT_END ) )
    {
      if(mpAppParam->pBoxUppChatDraw){
        OHNO_PRINT("pBoxUppChatDraw->Open 2\n");
        mpAppParam->pBoxUppChatDraw->Open();
      }
    }
  }
}

void BoxUppPokeStatusFrame::SetOtherPokemon(const pml::pokepara::PokemonParam& pokemonParam)
{
  if(mpTargetParam==NULL){
    return;
  }
  OHNO_PRINT("SetOtherPokemon %d chat %d\n",pokemonParam.GetMonsNo(),mbChat);
//  mpTargetParam->CopyFrom(pokemonParam);
  mpAppParam->pTargetPokemonParam->CopyFrom(pokemonParam);

  if(mbChat && mpAppParam->pBoxUppChatDraw){
    if(!pokemonParam.IsNull()){
      mpAppParam->pBoxUppChatDraw->SetPokeParam( BoxUppChatDraw::PLYAER_TYPE_OTHER, &pokemonParam );
      mpAppParam->pBoxUppChatDraw->SetPokemonStateOnOff( BoxUppChatDraw::PLYAER_TYPE_OTHER, true );
    }
    else{
      mpAppParam->pBoxUppChatDraw->SetPokemonStateOnOff( BoxUppChatDraw::PLYAER_TYPE_OTHER, false );
    }
  }
}


void BoxUppPokeStatusFrame::DispOtherMessage( void )
{
  if(mpAppParam->pBoxUppChatDraw){
    ChangeForceChat();
  }

  //■通信交換　チャット更新音
  if(msg_chat_01_13!=DispOtherMessage2()){
    Sound::PlaySE( SEQ_SE_P2P_CHATUPDATE );//  SEQ_SE_P2P_CHATUPDATE
  }
}

int BoxUppPokeStatusFrame::DispOtherMessage2( void )
{
  {
    int msgno[]={
      msg_chat_01_01,
      msg_chat_01_02,
      msg_chat_01_03,
      msg_chat_01_04,
      msg_chat_01_05,
      msg_chat_01_06,
      msg_chat_01_07,
      msg_chat_01_08,
      msg_chat_01_09,
      msg_chat_01_10,
      msg_chat_01_11,
      msg_chat_01_12,
      msg_chat_01_13,
    };
    GFL_ASSERT(mpAppParam->chatMsgOther<elementof(msgno));
    int no = 0;
    if(mpAppParam->chatMsgOther<elementof(msgno)){
      no = mpAppParam->chatMsgOther;
    }
    if(mpAppParam->chatPokemonOther==0 && no == 1){
    }
    else{

      if(msgno[ no ]==msg_chat_01_09){
        ExpandPokemon( mpAppParam->pWantPokemonParam );
      }
      else{
        ExpandPokemon( mpTargetParam);
      }

      mpBoxAppSetup->SetOtherMessage(msgno[ no ]);
      mpAppParam->pBoxUppChatDraw->SetChatWindowMessageID( App::Box::BoxUppChatDraw::PLYAER_TYPE_OTHER, msgno[ no ]);
      return msgno[ no ];
    }
  }
  return -1;
}

void BoxUppPokeStatusFrame::DispSelfMessage( void )
{
  if(mbChat && mpAppParam->pBoxUppChatDraw){

    if((mpAppParam->chatMsgSelf+msg_chat_01_01)==msg_chat_01_09){
      ExpandPokemon( mpAppParam->pMyWantPokemonParam );
    }
    else{
      ExpandPokemon( mpMyParam);
    }
    mpAppParam->pBoxUppChatDraw->SetChatWindowMessageID( App::Box::BoxUppChatDraw::PLYAER_TYPE_SELF, msg_chat_01_01 + mpAppParam->chatMsgSelf );
  }
}


void BoxUppPokeStatusFrame::ExpandPokemon(const pml::pokepara::CoreParam * pp)
{
  if(pp==NULL || pp->IsNull() || pp->IsEgg(pml::pokepara::CHECK_ONLY_ILLEGAL_EGG)){
    mpAppParam->pBoxUppChatDraw->ExpandPokemon(0, MONSNO_TAMAGO);
  }
  else{
    mpAppParam->pBoxUppChatDraw->ExpandPokemon(0, pp);
  }
}

bool BoxUppPokeStatusFrame::SetPokemonFolderDisp(u16 boxno)
{
  if(mpBoxUppFolderDraw==NULL){
    return false;
  }
  if(mBackupBoxNo == boxno){
    return false;
  }
  mBackupBoxNo=boxno;
  if(boxno < Savedata::BoxPokemon::TRAY_MAX){
    mpBoxUppFolderDraw->DrawTitle(boxno);
    for(int i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){
      mpBoxUppFolderDraw->DrawPokeIcon( i , mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(boxno,i));
    }
    mpBoxUppFolderDraw->SetPokemonPanelVisible(true);
  }
  else{
    mpBoxUppFolderDraw->SetPokemonPanelVisible(false);
  }

  return true;
}




bool BoxUppPokeStatusFrame::ChangeIcon(u16 boxpos)
{
  u16 boxno=boxpos>>8;
  u16 pos=boxpos&0xff;

  if(mpBoxUppFolderDraw == NULL){
    return false;
  }
  if(mBackupBoxNo != boxno){
    OHNO_PRINT("%d %d\n",mBackupBoxNo,boxno);
    return false;
  }
  mpBoxUppFolderDraw->DrawPokeIcon( pos , mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(boxno,pos));
  return true;
}


//--------------------------------------------------------------------------
/**
 * @brief   上画面へのリクエスト
 */
//--------------------------------------------------------------------------

void BoxUppPokeStatusFrame::DBoxUppStatusListener::Request(UPPDISP_TYPE no, u32 param)
{
  UppDispChange uppDispChange;

  uppDispChange.requestNo = no;
  uppDispChange.param = param;
  uppDispChange.param2 = 0;
  OHNO_PRINT("DBoxUppStatusListener::Request %d\n",no);
  reqin(&uppDispChange);
  
}

void BoxUppPokeStatusFrame::DBoxUppStatusListener::Request(UPPDISP_TYPE no, u32 param, u32 param2)
{
  UppDispChange uppDispChange;

  uppDispChange.requestNo = no;
  uppDispChange.param = param;
  uppDispChange.param2 = param2;

  OHNO_PRINT("DBoxUppStatusListener::Request %d\n",no);
  reqin(&uppDispChange);
  
}


void BoxUppPokeStatusFrame::DBoxUppStatusListener::reqin(UppDispChange* pUDC)
{
  for(int i=0 ; i < BoxUppStatusListener::UPPDISP_SELECT_PUSHMAX;i++)
  {
    if(mUppDispChange[i].requestNo==UPPDISP_NONE)
    {
      gfl2::std::MemCopy(pUDC,&mUppDispChange[i], sizeof(UppDispChange));
      break;
    }
  }
}


void BoxUppPokeStatusFrame::DBoxUppStatusListener::reqout( UppDispChange* pUDC  )
{
  gfl2::std::MemCopy(&mUppDispChange[0], pUDC, sizeof(UppDispChange));


  for(int i=0;i < (BoxUppStatusListener::UPPDISP_SELECT_PUSHMAX-1);i++){
    gfl2::std::MemCopy(&mUppDispChange[i+1],&mUppDispChange[i], sizeof(UppDispChange));
  }
  mUppDispChange[BoxUppStatusListener::UPPDISP_SELECT_PUSHMAX-1].requestNo=UPPDISP_NONE;
}


void BoxUppPokeStatusFrame::DBoxUppStatusListener::reqinpoke(UppDispChange* pUDC)
{
  for(int i=0 ; i < BoxUppStatusListener::UPPDISP_SELECT_POKE_PUSHMAX;i++)
  {
    if(mUppDispChangePoke[i].requestNo==UPPDISP_NONE)
    {
      gfl2::std::MemCopy(pUDC,&mUppDispChangePoke[i], sizeof(UppDispChange));
      return;
    }
  }
  //新しいもので上書きする GFNMCat1879
  gfl2::std::MemCopy(pUDC,&mUppDispChangePoke[BoxUppStatusListener::UPPDISP_SELECT_POKE_PUSHMAX-1], sizeof(UppDispChange));
}


void BoxUppPokeStatusFrame::DBoxUppStatusListener::reqoutpoke( UppDispChange* pUDC  )
{
  gfl2::std::MemCopy(&mUppDispChangePoke[0], pUDC, sizeof(UppDispChange));


  for(int i=0;i < (BoxUppStatusListener::UPPDISP_SELECT_POKE_PUSHMAX-1);i++){
    gfl2::std::MemCopy(&mUppDispChangePoke[i+1],&mUppDispChangePoke[i], sizeof(UppDispChange));
  }
  mUppDispChangePoke[BoxUppStatusListener::UPPDISP_SELECT_POKE_PUSHMAX-1].requestNo=UPPDISP_NONE;
}

void BoxUppPokeStatusFrame::DBoxUppStatusListener::getpoke( UppDispChange* pUDC  )
{
  gfl2::std::MemCopy(&mUppDispChangePoke[0], pUDC, sizeof(UppDispChange));

}



void BoxUppPokeStatusFrame::DBoxUppStatusListener::PokeChangeRequest(UPPDISP_TYPE no, u32 param, u32 param2)
{
  UppDispChange udcp;
    
  udcp.requestNo = no;
  udcp.param = param;
  udcp.param2 = param2;

  OHNO_PRINT("DBoxUppStatusListener::PokeChangeRequest %d\n",no);
  reqinpoke(&udcp);

}


void BoxUppPokeStatusFrame::DBoxUppStatusListener::Update(void)
{
  UppDispChange uppDispChange;
  
  reqout(&uppDispChange);
  
  switch(uppDispChange.requestNo){
  case UPPDISP_STATE:
    mpThis->ChangeForceState(false);
    break;
  case UPPDISP_UPDATE_STATE:
    mpThis->SetDisp(uppDispChange.param);
    break;
  case UPPDISP_JUGDE:
    mpThis->ChangeJudgeMode();
    break;
  case UPPDISP_CHAT_UPDATE:
    mpThis->DispSelfMessage();
    break;
  case UPPDISP_CHAT:
    mpThis->ChangeChatMode();
    break;
  case UPPDISP_CHAT_FORCE:
    OHNO_PRINT("UPPDISP_CHAT_FORCE \n");
    mpThis->ChangeForceChat();
    break;
  case UPPDISP_FOLDER:
    mpThis->ChangeFolderMode(true);
    break;
  case UPPDISP_FOLDER_END:
    mpThis->ChangeFolderMode(false);
    break;
  case UPPDISP_FOLDER_ICONUPDATE:
    mpThis->ChangeIcon(uppDispChange.param);
    break;
  case UPPDISP_FOLDER_RELOAD:
    mpThis->ResetBackupBoxNo();
    break;
  }

}

void BoxUppPokeStatusFrame::DBoxUppStatusListener::UpdatePokemon(void)
{
  UppDispChange uppDispChange;
  
  getpoke(&uppDispChange);
  switch(uppDispChange.requestNo){
  case UPPDISP_UPDATE_STATE:
    if(mpThis->SetDisp(uppDispChange.param)){
      reqoutpoke(&uppDispChange);
    }
    break;
  case UPPDISP_NET_STATE:
    if(mpThis->SetDispNetwork(uppDispChange.param2, uppDispChange.param )){
      reqoutpoke(&uppDispChange);
    }
    break;
  }
}

void BoxUppPokeStatusFrame::DBoxUppStatusListener::Init(void)
{
  int i;
  for( i=0 ; i < BoxUppStatusListener::UPPDISP_SELECT_PUSHMAX;i++)
  {
    mUppDispChange[i].requestNo = UPPDISP_NONE;
  }
  for( i=0 ; i < BoxUppStatusListener::UPPDISP_SELECT_POKE_PUSHMAX;i++){
    mUppDispChangePoke[i].requestNo = UPPDISP_NONE;
  }
}


#if SENDPOKE_SENDQ_

void BoxUppPokeStatusFrame::SetSendPokeReq(void)
{
  if(mpAppParam->pP2PTradeRequestClient){//通信でポケモンを送る
    if(mSendPokeSt.request == SENDPOKEMON_NONE){
      SendPokemonParam();  //しばらく変更が無くて変更がきた場合は即時反映
    }
    else{
      mSendPokeSt.request = SENDPOKEMON_REQ_CALL;  //要求だけ受付、じかんは変更しない
    }
  }
}



void BoxUppPokeStatusFrame::UpdateSendPoke(void)
{         
  if(mpAppParam->pP2PTradeRequestClient==NULL){//通信でポケモンを送る
    return;
  }
  if( mSendPokeSt.request == SENDPOKEMON_NONE){
    return;
  }
  if(mSendPokeSt.downCount!=0){
    mSendPokeSt.downCount--;
    if(mSendPokeSt.downCount==0){
      if( mSendPokeSt.request == SENDPOKEMON_REQ_CALL){ //追加容共があれば
        SendPokemonParam();  //処理＆延長
      }
      else{
        mSendPokeSt.request = SENDPOKEMON_NONE;  //追加が無かったので元に戻す
      }
    }
  }
  
}
//ポケモンを送る、送ったことをバッファに記録
void BoxUppPokeStatusFrame::SendPokemonParam(void)
{
  // 相手がnijiで、momiji新要素を出そうとした時は送らない
  if( mpAppParam->isNijiCompatibility == true )
  {
    if( PokeRegulation::CheckNijiCompatible( mpMyParam ) == true )
    {
      GFL_RELEASE_PRINT( ">BOX 互換対応ポケモンだったので送信SKIP\n" );
      mSendPokeSt.request = SENDPOKEMON_NONE; // SENDPOKEMON_REQ_CALLで UpdateSendPoke を通る流れだとバグるのでリセット
      return;
    }
  }

  mpAppParam->pP2PTradeRequestClient->AddSendPokemonParamRequest(*mpMyParam);
  mSendPokeSt.request = SENDPOKEMON_REQ;
  mSendPokeSt.downCount = SENDPOKEMON_REQ_TIME;
}



#endif

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




