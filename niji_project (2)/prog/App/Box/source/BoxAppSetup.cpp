//===================================================================
/**
 *	@file	    BoxAppSetup.cpp
 *	@brief    必要なパラメータをそろえる
 *	@author	  k.ohno
 *	@date		  2015.9.23
 */
//===================================================================

#include "App/Box/include/BoxAppSetup.h"

#include "System/include/HeapDefine.h"
#include <GameSys/include/GameProcManager.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

//#include "AppLib/include/Util/app_util_AppLytAccessor.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "niji_conv_header/poke_lib/monsno_def.h"
#include "niji_conv_header/poke_lib/wazano_def.h"
#include "niji_conv_header/poke_lib/item/itemsym.h"
#include "System/include/PokemonVersion.h"

#include "PokeTool/include/PokeTool.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>
#include "arc_index/MenuWindow.gaix"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------
enum
{
  EVSEQ_INIT,
  EVSEQ_INIT_TEST,
  EVSEQ_WAIT_TEST,
  EVSEQ_JUMP_TITLE,
  EVSEQ_CALL_SEARCH,
  EVSEQ_WAIT_SEARCH,
  EVSEQ_CALL_ITEM,
  EVSEQ_WAIT_ITEM,
  EVSEQ_CALL_STATUS,
  EVSEQ_WAIT_STATUS,
  EVSEQ_END,
};

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
 */
//-----------------------------------------------------------------------------


//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
BoxAppSetup::BoxAppSetup(void)
{
  mOtherMessage=-1;
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
BoxAppSetup::~BoxAppSetup()
{
}






void BoxAppSetup::Setup(gfl2::heap::HeapBase * heap,App::Box::APP_PARAM* pParam,GameSys::GameManager* game_manager)
{
  mpParam = pParam;

  makeManagerData(heap, game_manager);


  for(int i = 0;i < GARC_boxLyt_DATA_NUM; i++){
    mpResourceBuff[i]=NULL;
    mbResourceLoaded[i]=false;
  }

}


void BoxAppSetup::Shutdown(void)
{

  mpParam->pBoxData->SetCurrentTray(mpParam->nowDispTray);//GFNMCat[3634]

  
}


void BoxAppSetup::makeManagerData(gfl2::heap::HeapBase * heap,GameSys::GameManager* game_manager)
{
  mpParam->pZukanData = game_manager->GetGameData()->GetZukanData();

  mpParam->pBoxData = game_manager->GetGameData()->GetBOX();
  mpParam->pBoxPokemon = game_manager->GetGameData()->GetBoxPokemon();
  mpParam->pResortSave = game_manager->GetGameData()->GetResortSave();
  mpParam->outJumpMode = App::Box::BOXJUMP_OK;
  mpParam->pMyItem = game_manager->GetGameData()->GetMyItem();

  mpParam->pBoxPokemon->UpdateTrayMax(heap);  //BOX数開放の確認 NMCat[1201]
  
  mpParam->nowDispTray = mpParam->pBoxData->GetCurrentTray();

  mpParam->bBoxJudge = game_manager->GetGameData()->GetEventWork()->CheckEventFlag(SYS_FLAG_JUDGE_OPEN);

#ifdef DEBUG_ONLY_FOR_ohno_katsumi
  mpParam->bBoxJudge = true;
#endif

  if(mpParam->type == BOX_CALL_TRADE){  //Ｐ２Ｐ交換の場合常にOFF
    mpParam->bBoxJudge = false;
  }
  
  
 // mpParam->bBoxJudge = //game_manager->GetGameData()->GetMisc()->GetBoxJudgeFlag();
//  mpParam->pTemochi = game_manager->GetGameData()->GetPlayerParty();
  mpParam->pTemochi2 = game_manager->GetGameData()->GetPlayerParty();

  mpParam->mpAsyncFileManager = game_manager->GetAsyncFileManager();
  mMoveButton[0]=1;
  mMoveButton[1]=0;
  mMoveButton[2]=0;

  if(mpParam->type == BOX_CALL_BATTLE_TEAM){
    mSelectButton[0]=0;
    mSelectButton[1]=0;
    mSelectButton[2]=1;
  }
  else{
    mSelectButton[0]=1;
    mSelectButton[1]=0;
    mSelectButton[2]=0;
  }
  

  for(int i=0;i<elementof(mPokeIconActive);i++){
   // mPokeIconActive[i] = 1;
  }

  
}





void BoxAppSetup::ProcResourceFree(void)
{
  for(int i = 0;i < GARC_boxLyt_DATA_NUM; i++){
    if(mpResourceBuff[i]){
      GflHeapFreeMemory(mpResourceBuff[i]);
      mpResourceBuff[i]=NULL;
      mbResourceLoaded[i]=false;
    }
  }
}


void BoxAppSetup::ProcResourceRead(int gaixnum, app::util::Heap* pAppHeap, gfl2::fs::AsyncFileManager* pFM)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

  req.arcId             = ARCID_BOX;
  req.datId             = gaixnum;
  req.ppBuf             = &mpResourceBuff[gaixnum];
  req.heapForBuf        = pAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
  req.heapForReq        = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  req.heapForCompressed = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
  req.align             = 128;
  pFM->AddArcFileLoadDataReq(req);

}

//この関数は使わないで下さい。停止の不具合になります
bool BoxAppSetup::IsResourceLoaded(gfl2::fs::AsyncFileManager* pFM)
{
  for(int i = 0;i < GARC_boxLyt_DATA_NUM; i++){
    if( mpResourceBuff[i]){
      if(!pFM->IsArcFileLoadDataFinished(&mpResourceBuff[i]) ){
        return false;
      }
    }
  }
  return true;
}

bool BoxAppSetup::IsResourceLoaded(int no)
{
 //if(!pFM->IsArcFileLoadDataFinished(&mpResourceBuff[no]) ){
  if(!mbResourceLoaded[no]){
    return false;
  }
  return true;
}


void BoxAppSetup::Update(gfl2::fs::AsyncFileManager* pFM)
{
  for(int i = 0;i < GARC_boxLyt_DATA_NUM; i++){
    if(!mbResourceLoaded[i]){
      if( mpResourceBuff[i]){
        if(pFM->IsArcFileLoadDataFinished(&mpResourceBuff[i]) ){
          mbResourceLoaded[i]=true;
        }
      }
    }
  }
}



void* BoxAppSetup::GetArcReadBuff(int arcno)
{
  return mpResourceBuff[arcno];
}


int BoxAppSetup::GetSelectMode(void)
{
  for(int i = 0;i < BOX_CHANGE_EDITTYPE_MAX; i++){
    if( mSelectButton[i]==1){
      return i;
    }
  }
  GFL_ASSERT(0);
  return 0;
}


int BoxAppSetup::GetMoveMode(void)
{
  for(int i = 0;i < BOX_CHANGE_SELECTMODE_MAX; i++){
    if( mMoveButton[i] == 1){
      return i;
    }
  }
  GFL_ASSERT(0);
  return 0;
}


void BoxAppSetup::CreateMenuWindowResource(gfl2::fs::AsyncFileManager* pFM, app::util::Heap* pAppHeap)
{

  gfl2::fs::AsyncFileManager::ArcFileOpenReq reqo;
  reqo.arcId             = ARCID_MENU_WINDOW;
  reqo.heapForFile       = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  reqo.heapForReq        = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  pFM->AddArcFileOpenReq(reqo);
  
  mSequence=0;

}




bool BoxAppSetup::IsMenuWindowResourceLoaded(gfl2::fs::AsyncFileManager* pFM,app::util::Heap* pAppHeap)
{
  switch(mSequence){
  case 0:
    if( !pFM->IsArcFileOpenFinished( ARCID_MENU_WINDOW )){
      break;
    }
    {

      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId             = ARCID_MENU_WINDOW;
      req.datId             = GARC_MenuWindow_MenuWindow_applyt_COMP;
      req.ppBuf             = &mpMenuWindowRes;
      req.heapForBuf        = pAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
      req.heapForReq        = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
      req.heapForCompressed = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
      req.align             = 128;
      pFM->AddArcFileLoadDataReq(req);
    }
    mSequence++;
  case 1:
    if( !pFM->IsArcFileLoadDataFinished( &mpMenuWindowRes)){
      break;
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq reqc;
      reqc.arcId             = ARCID_MENU_WINDOW;
      reqc.heapForReq        = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
      pFM->AddArcFileCloseReq(reqc);
    }
    mSequence++;
  case 2:
    if(!pFM->IsArcFileCloseFinished(ARCID_MENU_WINDOW) ){
      break;
    }
    return true;
  }
  return false;
}


void BoxAppSetup::DeleteMenuWindowResource(void)
{
  GflHeapFreeMemory(mpMenuWindowRes);
}


void BoxAppSetup::SetOtherMessage(int no)
{
  mOtherMessage=no;
}

int BoxAppSetup::GetOtherMessage(void)
{
  return mOtherMessage;
}


void BoxAppSetup::BinLinkLoad(app::util::Heap* pAppHeap)
{
  app::util::FileAccessor::FileOpen( ARCID_BOX, pAppHeap->GetDeviceHeap(), true );
  app::util::FileAccessor::FileLoad( ARCID_BOX, GARC_boxLyt_box_pack_pack_COMP, &m_dataBuf, pAppHeap->GetDeviceHeap(), true, 128 );
}


bool BoxAppSetup::IsBinLinkLoad(void)
{
  return app::util::FileAccessor::IsFileOpen(ARCID_BOX) && app::util::FileAccessor::IsFileLoad(&m_dataBuf);
}


void* BoxAppSetup::GetBinLinkReadBuff(int linkno)
{
  m_binLinker.Initialize(m_dataBuf);
  return m_binLinker.GetData(linkno);
}



void BoxAppSetup::BinLinkClose(app::util::Heap* pAppHeap)
{
  app::util::FileAccessor::FileCloseSync( ARCID_BOX );
//  GFL_SAFE_DELETE(m_dataBuf);
  GflHeapSafeFreeMemory(m_dataBuf);
}



int BoxAppSetup::GetNowTrayNo(void)
{
  int tray = mpParam->nowDispTray;
  if(mpParam->outPos < Savedata::BoxPokemon::TRAY_POKE_MAX){
  }
  else{
    if( App::Box::BOX_CHANGE_EDITTYPE_BATTLE_TEAM == GetSelectMode() ){
      tray = (u8)mpParam->nowBattleTray;
    }
    else if( mpParam->type == BOX_CALL_RESORT_HOTSPA || mpParam->type == BOX_CALL_RESORT_GIM){
      tray = (u8)mpParam->nowBattleTray;
    }
    else{
      tray=0;
    }
  }
  return tray;
}


//ポケモンデータをボックス位置、カーソル位置から得て、設定
void BoxAppSetup::SetPokemon(gfl2::heap::HeapBase * heap)
{
  pml::pokepara::PokemonParam* pp = mpParam->pSelectPokemonParam;
  
  int cursorno=mpParam->outPos;
  int boxno=GetNowTrayNo();
  int tno = cursorno - Savedata::BoxPokemon::TRAY_POKE_MAX;

  if((cursorno < Savedata::BoxPokemon::TRAY_POKE_MAX)){  //BOXトレイ
    mpParam->pBoxPokemon->UpdatePokemonConst(pp, boxno, cursorno);
    return;
  }
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == GetSelectMode()){  //バトルチームモードの時は特になにも行わない
		return;
	}
  switch( mpParam->type ){
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択 そうでないときはてもち
    {
      mpParam->pTemochi2->ReplaceMember(tno, *pp);
    }
    break;
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
    mpParam->pResortSave->SetHotSpaPokemon(boxno * 6+tno , pp);//NMCat2173  getSetがさかさま
    break;
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    mpParam->pResortSave->SetGimPokemon(boxno * 6+tno , pp);//NMCat2173
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}





GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )
