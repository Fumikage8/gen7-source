//=============================================================================
/**
 * @file	 BoxPokemonMoveFunction.cpp
 * @brief	 ポケモンの移動に関する機能をあつめたクラス
 * @author k.ohno
 * @date	 2016.2.27
 */
//=============================================================================
#pragma once


#include "../BoxPokepara.h"
#include "../BoxSimpleAccesser.h"

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

#include "BoxPokemonMoveFunction.h"

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>

// niji互換対応ポケモンチェック
#include "Battle/Regulation/include/PokeRegulation.h"

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )


//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
BoxPokemonMoveFunction::BoxPokemonMoveFunction(void)
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BoxPokemonMoveFunction::~BoxPokemonMoveFunction()
{
}

//------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------
void BoxPokemonMoveFunction::Setup(APP_PARAM* pAppParam, BoxAppSetup* pBoxAppSetup,PokePara* pBoxPokeParaFunc)
{
  mpAppParam=pAppParam;
  mpBoxAppSetup= pBoxAppSetup;
  mpBoxPokeParaFunc = pBoxPokeParaFunc;
  mpAppHeap = mpAppParam->mpAppHeap;
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  mpCoreTemp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap );
}


void BoxPokemonMoveFunction::Release(void)
{
  GFL_SAFE_DELETE(mpCoreTemp);
}


//移動してよいかどうか 移動する時に検査
int BoxPokemonMoveFunction::pokeMoveCheck(int tray,int pos, int nowtray, int nowpos)
{
  ///box GIM   たまごNG         ok   ok
  ///temoti     box たたかう     ok
  /// tray       ロックしているかどうか  ok
  //> ばとるtray   タマゴNG
  // gim hotspa  チーム登録が不可

  GetPokemon(tray,pos, mpCoreTemp);

  bool bEgg = mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG);

  if((nowpos >= Savedata::BoxPokemon::TRAY_POKE_MAX) && (pos < Savedata::BoxPokemon::TRAY_POKE_MAX) )  //box--> tray
  {

    if( mpAppParam->type == BOX_CALL_RESORT_GIM && bEgg ){
      return MOVEERR_EGG_PUT; //たまごをおけない
    }
  }
	if(nowpos >= Savedata::BoxPokemon::TRAY_POKE_MAX)
	{
		if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
			bool block = mpAppParam->pBoxData->IsTeamLock(nowtray);
			if(block)
			{
				return MOVEERR_TEAM_LOCK;
			}
			if(bEgg){
				return MOVEERR_EGG_PUT; //たまごをおけない
			}
		}
	}

	if((mpAppParam->type == BOX_CALL_RESORT_GIM) || (mpAppParam->type == BOX_CALL_RESORT_HOTSPA)){
    if(pos < Savedata::BoxPokemon::TRAY_POKE_MAX && nowpos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
      int teamno = mpAppParam->pBoxData->IsTeamInMe(tray, pos);
      if(teamno!=-1){
        return MOVEERR_TEAM_ENTRY;
      }
    }
    if(nowpos < Savedata::BoxPokemon::TRAY_POKE_MAX && pos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
      int teamno = mpAppParam->pBoxData->IsTeamInMe(nowtray, nowpos);
      if(teamno!=-1){
				return MOVEERR_TEAM_ENTRY;
      }
    }
  }

  
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM != mpBoxAppSetup->GetSelectMode()){  //バトルチームモードならロックをコピーしてもいいがそのほかはＮＧ
    if(pos < Savedata::BoxPokemon::TRAY_POKE_MAX){
      if(ThisPokeLockCheck(tray,pos)){
        return MOVEERR_TEAM_LOCK;
      }
    }
    if(nowpos < Savedata::BoxPokemon::TRAY_POKE_MAX){
      if(ThisPokeLockCheck(nowtray,nowpos)){
        return MOVEERR_TEAM_LOCK;
      }
    }
  }


  
	{
		int removepos=0;
    bool bremovechk=false;

	//この検査はＢＯＸてもち、てもちＢＯＸの移動と時に必要
		if(nowpos >= Savedata::BoxPokemon::TRAY_POKE_MAX && pos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
		}
		else if(nowpos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
      bremovechk=true;
			removepos = nowpos - Savedata::BoxPokemon::TRAY_POKE_MAX;
		}
		else if(pos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
      bremovechk=true;
			GetPokemon(nowtray,nowpos, mpCoreTemp);
			removepos = pos - Savedata::BoxPokemon::TRAY_POKE_MAX;
		}
		else{
		}
		if(mpAppParam->type==BOX_CALL_PC && bremovechk ){
      pml::PokeParty* pPartyTemp = GFL_NEW( mpAppHeap->GetDeviceHeap() ) pml::PokeParty( mpAppHeap->GetDeviceHeap() );
      mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_ThisToParty(pPartyTemp);
      pPartyTemp->RemoveMember(removepos);  //入れる前に消す
      pPartyTemp->AddMember(*mpCoreTemp);  //末尾に追加
      bool retbool=mpBoxPokeParaFunc->CheckBattlePokemon(pPartyTemp) ;
      GFL_SAFE_DELETE(pPartyTemp);
      if(!retbool ){  //戦うポケモン残るかどうか
        return MOVEERR_NO_BATTLEPOKE;
      }
    }
    
  }
	//いれかえの際の反対側の検査になります
	
	GetPokemon(nowtray,nowpos, mpCoreTemp);
	if(mpCoreTemp->IsNull()){
		return MOVEERR_NONE;
	}

  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードなら  //GFNMCat3646
    //入れ替えポケモンがいて、手前がロックされたトレイの場合NG   //NMCat1274
    if( (pos >= Savedata::BoxPokemon::TRAY_POKE_MAX) && mpAppParam->pBoxData->IsTeamLock(tray) ){
      return MOVEERR_TEAM_LOCK;
    }
  }

  bEgg = mpCoreTemp->IsEgg(pml::pokepara::CHECK_BOTH_EGG);
	if((nowpos < Savedata::BoxPokemon::TRAY_POKE_MAX) && (pos >= Savedata::BoxPokemon::TRAY_POKE_MAX) )  //box<=== tray
	{
		if( mpAppParam->type == BOX_CALL_RESORT_GIM && bEgg ){
			return MOVEERR_EGG_PUT; //たまごをおけない
		}
	}

	return MOVEERR_NONE;
}



//移動してよいかどうか 全体ボックス移動用はポケモンが消える前提で組んである
int BoxPokemonMoveFunction::pokeMoveCheckFolderOnly(int tray,int pos, int nowtray, int nowpos)
{
	{
		int removepos=0;
		if(nowpos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
			removepos = nowpos - Savedata::BoxPokemon::TRAY_POKE_MAX;
		}
		else if(pos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
			removepos = pos - Savedata::BoxPokemon::TRAY_POKE_MAX;
		}
		else{
			return MOVEERR_NONE;
		}
		if(mpAppParam->type==BOX_CALL_PC){
      pml::PokeParty* pPartyTemp = GFL_NEW( mpAppHeap->GetDeviceHeap() ) pml::PokeParty( mpAppHeap->GetDeviceHeap() );
      mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_ThisToParty(pPartyTemp);
      pPartyTemp->RemoveMember(removepos);  //入れる前に消す
      bool retbool=mpBoxPokeParaFunc->CheckBattlePokemon(pPartyTemp) ;
      GFL_SAFE_DELETE(pPartyTemp);
      if(!retbool ){  //戦うポケモン残るかどうか
        return MOVEERR_NO_BATTLEPOKE;
      }
    }
    
    
  }
  return MOVEERR_NONE;
}







//移動してよいかどうか 移動する前のポケモンを検査  範囲版
int BoxPokemonMoveFunction::pokeMoveCheckRange(int selectpos,RangeData* pRange)
{
  if(selectpos >= Savedata::BoxPokemon::TRAY_POKE_MAX){ //手持ちの時
    if( App::Box::BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode() ){
    }
    else if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM){
    }
    else if(BattlePokemonCheck(pRange)){  //戦うポケモン残るかどうか
    }
    else{
      return MOVEERR_NO_BATTLEPOKE;
    }
  }
  return MOVEERR_NONE;
}


//範囲選択でパーティー内に戦うポケモンが残るかどうか
bool BoxPokemonMoveFunction::BattlePokemonCheck(RangeData* pRange)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);

  int y,x,pos;
  u8 emptybuff[6];
  // int xbuff[]={0,1,6,7,12,13};

  for( y = 0; y < 6; y++){
    emptybuff[y] = 0;
  }

  for(  x = 0; x < pRange->width; x++){
    if(pRange->skip && (x % 2 == 1)){
      continue;
    }
    pos = x + pRange->x + 30;
    GetPokemon( 0, pos ,pp);
    if(!pp->IsNull()){
      emptybuff[pos - 30] = 1;  //６体のなかで何匹候補なのかをバッファに入れる
    }
  }
  GFL_SAFE_DELETE(pp);

  int num = mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_CheckBattlePokemon(emptybuff);

  if(num==0){
    return false;
  }
  return true;

}

//ポケモンデータをボックス位置、カーソル位置から得る
void BoxPokemonMoveFunction::GetPokemon(int boxno,int cursorno, pml::pokepara::CoreParam* cp)
{
  mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemon(boxno,cursorno,cp);
}

void BoxPokemonMoveFunction::GetPokemonPokemonParam(int boxno,int cursorno, pml::pokepara::PokemonParam* pp)
{
  mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonPokemonParam(boxno,cursorno,pp);
}


//フォルダーからボックスにポケモンを置く 範囲のポケモン ALLBOXで使用
int BoxPokemonMoveFunction::PutBoxRange(int ,int selectpos, int nowpage,RangeData* pRange)
{
  int num = 0;

  num = mpAppParam->pBoxPokemon->GetPokemonCount(nowpage, mpCoreTemp);

  if(pRange->num + num > Savedata::BoxPokemon::TRAY_POKE_MAX){
    return MOVEERR_NO_PUT_PLACE;  // 数で置くところがない
  }
  int check = pokeMoveCheckRange(selectpos,pRange);

  if(check != MOVEERR_NONE){  //置いてはいけない条件に引っかかった
    return check;
  }


  {
    int x=0,y=0,pos;
    //ロック検査
    for(  y = 0; y < pRange->height; y++){
      for(  x = 0; x < pRange->width; x++){
        if(pRange->skip && (x % 2 == 1)){
          continue;
        }
        pos = x + pRange->x + ( y + pRange->y ) * 6;
        mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemon(pRange->page, pos , mpCoreTemp);
        if(!mpCoreTemp->IsNull()){
          if(ThisPokeLockCheck(pRange->page, pos)){
            return MOVEERR_TEAM_LOCK;
          }
        }
      }
    }

    for(  y = 0; y < pRange->height; y++){
      for(  x = 0; x < pRange->width; x++){
        if(pRange->skip && (x % 2 == 1)){
          continue;
        }
        pos = x + pRange->x + ( y + pRange->y ) * 6;
        mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemon(pRange->page, pos , mpCoreTemp);
        if(!mpCoreTemp->IsNull()){
          int retpos = mpAppParam->pBoxPokemon->CheckPokemonTrayRetPos( nowpage, mpCoreTemp);
          if(-1!=retpos){
            PokemonSet(pRange->page, pos, nowpage, retpos, -1);
          }
        }
      }
    }
  }
  mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonUseStock();  ///fix
  return MOVEERR_NONE;
}


//パーティーにセット
void BoxPokemonMoveFunction::SetPartyPokemon(int partypos, pml::pokepara::PokemonParam * pp)
{
  int x = partypos - Savedata::BoxPokemon::TRAY_POKE_MAX;

  mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_ReplaceMember(x, pp);  //置換

}


//ボックスと運動リゾートのデータ入れ替え
void BoxPokemonMoveFunction::ReplaceBoxToGim(int boxpage,int boxpos,int partypage,int partypos)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap();


  pml::pokepara::PokemonParam * pp = GFL_NEW(mpAppHeap->GetDeviceHeap()->GetLowerHandle()) pml::pokepara::PokemonParam(mpAppHeap->GetDeviceHeap()->GetLowerHandle());
  pml::pokepara::CoreParam* cp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );

  mpAppParam->pBoxPokemon->GetPokemon(pp, boxpage, boxpos);  //BOXからぬく

  mpAppParam->pBoxPokemon->ClearPokemon(boxpage, boxpos, mpAppHeap->GetDeviceHeap()->GetLowerHandle());

  int pos2 = partypage * 6 + partypos - Savedata::BoxPokemon::TRAY_POKE_MAX;

  if(mpAppParam->type == BOX_CALL_RESORT_GIM){
    mpAppParam->pResortSave->GetGimPokemon(pos2, cp);
  }
  else{
    mpAppParam->pResortSave->GetHotSpaPokemon(pos2, cp);
  }

  OHNO_PRINT("リゾートの置く--MonsNo %d %d %d\n",cp->GetMonsNo(), boxpage, boxpos);//

  mpAppParam->pBoxPokemon->PutPokemon(cp, boxpage, boxpos, mpAppHeap->GetDeviceHeap()->GetLowerHandle());

  OHNO_PRINT("リゾートの置く2--MonsNo %d %d\n",pp->GetMonsNo(), pos2);//


  mpBoxAppSetup->GetBoxSimpleAccesser()->SetResortPokemon(mpAppParam->type ,pos2 , pp );


  GFL_DELETE cp;
  GFL_DELETE pp;

}


//チームから外す
void BoxPokemonMoveFunction::RemoveTeam(int page,int pos)
{
  mpBoxAppSetup->GetBoxSimpleAccesser()->ResetPokemonIconDrawData();
  mpBoxAppSetup->GetBoxSimpleAccesser()->RemoveTeam(page, pos - Savedata::BoxPokemon::TRAY_POKE_MAX);
}



//バトルボックス登録解除
void BoxPokemonMoveFunction::BattleRegistOrRemove(int selectpage,int selectpos,int oldpage,int oldpos)
{


  if(selectpos >= Savedata::BoxPokemon::TRAY_POKE_MAX  && oldpos >= Savedata::BoxPokemon::TRAY_POKE_MAX ){ // 移動の場合 いれかえる
    mpBoxAppSetup->GetBoxSimpleAccesser()->ChangeTeamPokePos( selectpage, selectpos,  oldpage, oldpos );
  }
  else if(selectpos >= Savedata::BoxPokemon::TRAY_POKE_MAX  && oldpos < Savedata::BoxPokemon::TRAY_POKE_MAX ){ // はずす
    RemoveTeam(selectpage, selectpos);
  }
  else{//とうろく
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetTeamPokePos(  oldpage, oldpos-Savedata::BoxPokemon::TRAY_POKE_MAX, selectpage, selectpos );

  }
}



//データの入れ替えラップクラス
void BoxPokemonMoveFunction::ChangePokemonData(int selectpage,int selectpos,int oldpage,int oldpos)
{
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
    if((oldpos < Savedata::BoxPokemon::TRAY_POKE_MAX) && (selectpos < Savedata::BoxPokemon::TRAY_POKE_MAX)){  //どちらもBOXトレイ
      return;
    }
    BattleRegistOrRemove(selectpage, selectpos,  oldpage, oldpos );
    return;
  }

  if((oldpos < Savedata::BoxPokemon::TRAY_POKE_MAX) && (selectpos < Savedata::BoxPokemon::TRAY_POKE_MAX)){  //どちらもBOXトレイ
    mpAppParam->pBoxPokemon->ChangePokemon( selectpage,selectpos, oldpage, oldpos,
                                            mpAppHeap->GetDeviceHeap()->GetLowerHandle());
  }
  else if(oldpos < Savedata::BoxPokemon::TRAY_POKE_MAX){  //元がBOXトレイ

    switch(mpAppParam->type){
    case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
    case BOX_CALL_RESORT_GIM:   //ポケリゾート
      ReplaceBoxToGim( oldpage, oldpos, selectpage, selectpos);
      break;
    default:
      ReplaceBoxToParty(oldpage,oldpos,selectpage,selectpos);
      break;
    }

  }
  else if(selectpos < Savedata::BoxPokemon::TRAY_POKE_MAX){  //先がBOXトレイ
    switch(mpAppParam->type){
    case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
    case BOX_CALL_RESORT_GIM:   //ポケリゾート
      ReplaceBoxToGim( selectpage,selectpos,oldpage,oldpos);
      break;
    default:
      ReplaceBoxToParty(selectpage,selectpos,oldpage,oldpos);
      break;
    }
  }
  else{  //パーティー内

    switch(mpAppParam->type){
    case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
    case BOX_CALL_RESORT_GIM:   //ポケリゾート
      ReplaceGimToGim( selectpage,selectpos,oldpage,oldpos);
      break;
    default:
      if(selectpos!=oldpos){
        mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_ExchangePosition(selectpos-Savedata::BoxPokemon::TRAY_POKE_MAX,
                                                                        oldpos-Savedata::BoxPokemon::TRAY_POKE_MAX);
      }
      break;
    }



  }
}



//運動リゾート内部データ入れ替え
void BoxPokemonMoveFunction::ReplaceGimToGim(int boxpage,int boxpos,int partypage,int partypos)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap();

  pml::pokepara::PokemonParam * pp = GFL_NEW(mpAppHeap->GetDeviceHeap()->GetLowerHandle()) pml::pokepara::PokemonParam(mpAppHeap->GetDeviceHeap()->GetLowerHandle());
  pml::pokepara::CoreParam* cp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );

  int pos1 = boxpage * 6 + boxpos - Savedata::BoxPokemon::TRAY_POKE_MAX;
  int pos2 = partypage * 6 + partypos - Savedata::BoxPokemon::TRAY_POKE_MAX;

  if(pos1 != pos2){

    mpBoxAppSetup->GetBoxSimpleAccesser()->GetResortPokemon(mpAppParam->type ,pos1 , cp );
    mpBoxAppSetup->GetBoxSimpleAccesser()->GetResortPokemon(mpAppParam->type ,pos2 , pp );
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetResortPokemon(mpAppParam->type ,pos2 , cp );
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetResortPokemon(mpAppParam->type ,pos1 , pp );

  }

  GFL_DELETE cp;
  GFL_DELETE pp;

}

//ポケモンを入れ替える場合にいれかえ
//範囲外は入れ替えない
///@attention  カーソルや位置の状況を見るのではなく、ポケモンオブジェクトにどのようになって欲しいか書くべき

bool BoxPokemonMoveFunction::PokemonSet(int selectpage,int selectpos, int nowpage, int nowpos, int rangepos)
{
  if((selectpage == nowpage) && (selectpos == nowpos)){  //同じところは入れ替えない
    //アニメだけ実行
    mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonReserve(selectpage, selectpos, nowpage, nowpos, rangepos);
    return true;
  }

  if(nowpos < Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS)
  {
    ChangePokemonData(selectpage,selectpos, nowpage, nowpos );

    ChangePokemonBoxLinkData( selectpage,selectpos, nowpage, nowpos );

    mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonReserve(selectpage, selectpos, nowpage, nowpos, rangepos);
    return true;
  }

  return false;
}



void BoxPokemonMoveFunction::ChangePokemonBoxLinkData(int selectpage,int selectpos,int oldpage,int oldpos)
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


//ボックスとパーティーのデータ入れ替え
void BoxPokemonMoveFunction::ReplaceBoxToParty(int boxpage,int boxpos,int partypage,int partypos)
{

  pml::pokepara::PokemonParam * pp = GFL_NEW(mpAppHeap->GetDeviceHeap()->GetLowerHandle()) pml::pokepara::PokemonParam(mpAppHeap->GetDeviceHeap()->GetLowerHandle());

  mpAppParam->pBoxPokemon->GetPokemon(pp, boxpage, boxpos);  //BOXからぬく


  
  mpAppParam->pBoxPokemon->ClearPokemon(boxpage, boxpos, mpAppHeap->GetDeviceHeap()->GetLowerHandle());

  pml::pokepara::PokemonParam * ppt = mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_GetMemberPointer(partypos - Savedata::BoxPokemon::TRAY_POKE_MAX  );  //てもち

  mpAppParam->pBoxPokemon->PutPokemon(ppt, boxpage, boxpos, mpAppHeap->GetDeviceHeap()->GetLowerHandle());  //てもちをBOXに


  mpBoxPokeParaFunc->CheckAndChangeFormChangePokemon(pp, mpAppParam->pZukanData);  //手持ちに入れる前にフォルムチェンジ
  SetPartyPokemon(partypos, pp);

  GFL_DELETE pp;
}




//このポケモンがロックしているかどうか
bool BoxPokemonMoveFunction::ThisPokeLockCheck(int tray,int cursor)
{
  if(cursor < Savedata::BoxPokemon::TRAY_POKE_MAX)
  {  //box
    if( mpAppParam->pBoxData->IsMeLock(tray, cursor) )//ロック中のチーム   //NMCat746
    {
      return true;
    }
  }
  return false;
}

//チーム全体を詰めなおす
void BoxPokemonMoveFunction::TeamPackAll(void)
{
  if((mpAppParam->type != BOX_CALL_RESORT_GIM) && (mpAppParam->type != BOX_CALL_RESORT_HOTSPA)){
    int teamno;
    for(teamno = 0;teamno < Savedata::BOX::TEAM_MAX ; teamno++){
      mpAppParam->pBoxData->PackTeam( teamno );
      mpBoxAppSetup->GetBoxSimpleAccesser()->TumeruBattleBoxSimpleData(teamno);
    }
  }
}


//ポケモンデータをボックス位置、カーソル位置から得て、設定
/*
void BoxPokemonMoveFunction::SetPokemon(int boxno,int cursorno, pml::pokepara::CoreParam* cp)
{
  int tno = cursorno-Savedata::BoxPokemon::TRAY_POKE_MAX;
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap();

  if((cursorno < Savedata::BoxPokemon::TRAY_POKE_MAX)){  //BOXトレイ
    mpAppParam->pBoxPokemon->UpdatePokemonConst(cp, boxno, cursorno);
    return;
  }
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
    GFL_ASSERT(0);  //このモードの時にこの関数が呼ばれてはいけない
  }
  switch( mpAppParam->type ){
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択 そうでないときはてもち
    {
      pml::pokepara::PokemonParam* pp  = GFL_NEW(heap)  pml::pokepara::PokemonParam( heap, *cp );
      SetPartyPokemon(cursorno, pp);
      GFL_SAFE_DELETE(pp);
    }
    break;
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetResortPokemon(mpAppParam->type ,boxno * 6+tno , cp );
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}
*/

//ポケモンデータをボックス位置、カーソル位置から得て、設定
void BoxPokemonMoveFunction::SetPokemonPokemonParam(int boxno,int cursorno, pml::pokepara::PokemonParam* cp)
{
  int tno = cursorno-Savedata::BoxPokemon::TRAY_POKE_MAX;
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap();

  if((cursorno < Savedata::BoxPokemon::TRAY_POKE_MAX)){  //BOXトレイ
    mpAppParam->pBoxPokemon->UpdatePokemonConst(cp, boxno, cursorno);
    return;
  }
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
    GFL_ASSERT(0);  //このモードの時にこの関数が呼ばれてはいけない
  }
  switch( mpAppParam->type ){
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択 そうでないときはてもち
    {
      pml::pokepara::PokemonParam* pp  = GFL_NEW(heap)  pml::pokepara::PokemonParam( heap, *cp );
      SetPartyPokemon(cursorno, pp);
      GFL_SAFE_DELETE(pp);
    }
    break;
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetResortPokemon(mpAppParam->type ,boxno * 6+tno , cp );
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}


//範囲選択バトルチーム登録では、タマゴ不可
//リゾートＧＩＭでもたまご不可
void BoxPokemonMoveFunction::PokemonBattleCheckOfTeam(int selectpage,int selectpos, int nowpage, int nowpos,u8* btlbuf, int bput)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);
  GetPokemon( selectpage, selectpos ,pp);

  if(bput==RANGETEST_BATTLE){
    if((mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM) || (mpAppParam->type == BOX_CALL_RESORT_GIM)){
      if(pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){
        btlbuf[0]=1;//いっこでもタマゴがあったら不可
      }
    }
  }
  if( bput == RANGETEST_TEAM){// チームに入ってないかどうか
    if((mpAppParam->type == BOX_CALL_RESORT_GIM) || (mpAppParam->type == BOX_CALL_RESORT_HOTSPA)){
      int teamno = mpAppParam->pBoxData->IsTeamInMe(selectpage, selectpos);
      if(teamno!=-1){
        btlbuf[0]=1;//チーム所属は不可
      }
    }
  }
  if( bput == RANGETEST_TEAMLOCK){// ロックされているかどうか
    if(!(mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM)){
      if( ThisPokeLockCheck(selectpage, selectpos) ){ //ロック中のチーム 
        btlbuf[0]=1;//LOCK不可
      }
    }
  }
  GFL_SAFE_DELETE(pp);
}



bool BoxPokemonMoveFunction::PokemonBattleCheckOfTeamEnd( u8* btlbuf )
{
  int retnum=0;

  for(int i=0;i<6;i++){
    if(btlbuf[i]==1){
      retnum=1;
      break;
    }
  }
  return (retnum==0);
}

void BoxPokemonMoveFunction::PokemonBattleCheckOfTeamStart( u8* btlbuf )
{
  for(int i=0;i<6;i++){
    btlbuf[i]=0;
  }
}










//範囲選択手持ち移動の時のみ、バトルに出れるかどうかの検査が必要になる
void BoxPokemonMoveFunction::PokemonBattleCheckOfTemoti(int selectpage,int selectpos, int nowpage, int nowpos,u8* btlbuf)
{
  GFL_ASSERT(selectpos >= 30);
  GFL_ASSERT(selectpos < 36);
  btlbuf[selectpos - Savedata::BoxPokemon::TRAY_POKE_MAX]=0;
	OHNO_PRINT("check %d %d %d %d %d %d\n", btlbuf[0],btlbuf[1],btlbuf[2],btlbuf[3],btlbuf[4],btlbuf[5]);
}



bool BoxPokemonMoveFunction::PokemonBattleCheckOfTemotiEnd( u8* btlbuf )
{
  int retnum=0;

  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はこの検査は必要ない
    return true;// ＯＫ返却
  }
  else if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM){
    return true;// ＯＫ返却
  }
  
  for(int i=0;i<6;i++){
    if(btlbuf[i]==1){
      retnum=1;
      break;
    }
  }
	OHNO_PRINT("end %d %d %d %d %d %d\n", btlbuf[0],btlbuf[1],btlbuf[2],btlbuf[3],btlbuf[4],btlbuf[5]);
  return (retnum!=0);
}

void BoxPokemonMoveFunction::PokemonBattleCheckOfTemotiStart( u8* btlbuf,int nowpage )
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);
  for(int i=0;i<6;i++){
//      mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_GetMemberPointerConst(i);  //てもち
    GetPokemonPokemonParam( nowpage, i+Savedata::BoxPokemon::TRAY_POKE_MAX ,pp);  //GFNMCat5785
    OHNO_PRINT("no %d %d\n",pp->GetMonsNo(),pp->GetHp());
    if(!pp->IsNull() && !pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) && (pp->GetHp()!=0) ){
      btlbuf[i]=1;
    }
    else{
      btlbuf[i]=0;
    }
  }
  GFL_SAFE_DELETE(pp);
}

///範囲移動box to box
bool BoxPokemonMoveFunction::RangeChangeCheckOrPutBoxToBox(RangeData* pRange , int nowpage, int nowpos,int bput)
{
  //置くデータをマップ化 最大３６
  int selectmap[36];
	int rangepos[36];
  int nowmap[36];
  PokeMopyStruct copybuff[30];  //最大３０
  int y;
  int x;
  int ymax=0;
  int xmax=0;
  int pos;

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);

  OHNO_PRINT("num %d x %d skip %d y %d\n",pRange->num,pRange->x,pRange->skip,pRange->y);

  for( y = 0; y < 36; y++){
    selectmap[y] = 0;
    nowmap[y] = 0;
		rangepos[y] = 0;
  }

  for(  y = 0; y < pRange->height; y++){
    for(  x = 0; x < pRange->width; x++){
      pos = x + pRange->x + ( y + pRange->y ) * 6;   /// てもちとBOXで変化
      GetPokemon( pRange->page, pos ,pp);
      if(!pp->IsNull()){
				GFL_ASSERT(pos<36);
        selectmap[pos] = 1;  //いる
				rangepos[pos] = y * 6 + x;
        OHNO_PRINT("置くデータ %d %dいる\n",pos,rangepos[pos]);
        if(xmax<x){
          xmax = x;
        }
        if(ymax<y){
          ymax = y;
        }
      }
    }
  }



  // 置くべき場所のデータを抽出
  for(  y = 0; y < pRange->height; y++){
    for(  x = 0; x < pRange->width; x++){
      pos = nowpos + x + y * 6;
			if(pos>=30){
				continue;//GFNMCat1761
			}
      if( pRange->page == nowpage ){  //同じページの場合で
        if( selectmap[pos] == 1){  //置くデータがあっても
          continue;   //どくので無視してよい
        }
      }
      GetPokemon( nowpage, pos ,pp);
      if(!pp->IsNull()){
        OHNO_PRINT("置くつもりの場所 %d いる\n",pos);
				GFL_ASSERT(pos<36);
        nowmap[pos] = 1;  //いる
      }
    }
  }
  GFL_SAFE_DELETE(pp);

  OHNO_PRINT("xmax %d ymax %d nowpos %d\n",xmax,ymax,nowpos);

  if((6 - (nowpos % 6)) <= xmax){  //横はみだし
    OHNO_PRINT("よこはみだし\n");
    return false;
  }
  if((5 - (nowpos / 6)) <= ymax){  //たてはみだし
    OHNO_PRINT("たてはみだし\n");
    return false;
  }


  // 最終マージ検査
  int movecnt=0;
  for(  y = 0; y < 36; y++){

    x = y - nowpos  + pRange->cursor;
    if(x<0){
      continue;
    }
    if(x>=36){
      continue;
    }
    if((selectmap[x] == 1) && (nowmap[y] == 1)){
      return false;
    }

    if(selectmap[x] == 1){
      if(y>=30){
        return false;
      }
      if(bput==RANGETEST_TEAMLOCK){
        if(ThisPokeLockCheck(pRange->page, x)){
          return false;
        }
      }

      if(bput==RANGETEST_EXEC){
        OHNO_PRINT("おいた%d %d -- %d %d r %d\n", pRange->page, x, nowpage, y, rangepos[x]);
				GFL_ASSERT(movecnt<30);
        copybuff[movecnt].page1=pRange->page;
        copybuff[movecnt].pos1=x;
        copybuff[movecnt].page2=nowpage;
        copybuff[movecnt].pos2=y;
        copybuff[movecnt].rangepos=rangepos[x];
        movecnt++;
      }
    }
  }


  
  if(bput==RANGETEST_EXEC){
    if(nowpos < pRange->cursor){//順方向
      for(x=0;x<movecnt;x++){
        PokemonSet(copybuff[x].page1,  copybuff[x].pos1,  copybuff[x].page2, copybuff[x].pos2, copybuff[x].rangepos);
      }
    }
    else{  //逆方向
      for(x=movecnt-1;x>=0;x--){
        PokemonSet(copybuff[x].page1,  copybuff[x].pos1,  copybuff[x].page2, copybuff[x].pos2, copybuff[x].rangepos);
      }
    }

    mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_AlignMember(0,NULL);
    mpBoxAppSetup->GetBoxSimpleAccesser()->Resort_AlignMember(mpAppParam->nowBattleTray,0,NULL);
    TeamPackAll();
      mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonRangeAnimStart();

  }

  return true;
}


bool BoxPokemonMoveFunction::RangeChangeCheckOrPutTemotiToBox(RangeData* pRange , int nowpage, int nowpos,int bput)
{
  //置くデータをマップ化 最大３０
  int selectmap[36+12];
  int nowmap[36+12];
	int rangepos[36+12];
  u8 battlechk[6];
  int y;
  int x;
  int xmax=0;
  int ymax=0;
  int pos;

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);

  int xbuff[]={0,1,6,7,12,13};

  OHNO_PRINT("num %d x %d skip %d y %d\n",pRange->num,pRange->x,pRange->skip,pRange->y);

  for( y = 0; y < 36; y++){
    selectmap[y] = 0;
    nowmap[y] = 0;
		rangepos[y] = 0;
  }

  for(  x = 0; x < pRange->width; x++){
    if(pRange->skip && (x % 2 == 1)){
      continue;
    }
    pos = x + pRange->x + 30;
    GetPokemon( pRange->page, pos ,pp);
    if(!pp->IsNull()){
			int mpos = pos-30-pRange->x;
			GFL_ASSERT(mpos<(36+12));
      selectmap[mpos] = 1;  //いる
			rangepos[mpos] = xbuff[x];
      OHNO_PRINT("置くデータ確認 %d\n",pos);
    }
  }

  if(selectmap[0]||selectmap[2]||selectmap[4]){
    xmax++;
  }
  if(selectmap[1]||selectmap[3]||selectmap[5]){
    xmax++;
  }
  if(selectmap[0]||selectmap[1]){
    ymax++;
  }
  if(selectmap[2]||selectmap[3]){
    ymax++;
  }
  if(selectmap[4]||selectmap[5]){
    ymax++;
  }

  // 置くべき場所のデータを抽出
  for(  x = 0; x < 6; x++){
    pos = xbuff[x] + nowpos;
    if(pos >= 30){  //boxの範囲の中  GFNMCat[1761]
      continue;
    }
    GetPokemon( nowpage, pos ,pp);
    if(!pp->IsNull()){
      OHNO_PRINT("置くつもりの場所 %d いる\n",pos);
			GFL_ASSERT(x<(36+12));
      nowmap[x] = 1;  //いる
    }
  }
  GFL_SAFE_DELETE(pp);

  OHNO_PRINT("xmax %d ymax %d \n",xmax,ymax);

  if((6 - (nowpos % 6)) < xmax){  //横はみだし
    OHNO_PRINT("よこはみだし\n");
    return false;
  }
  if((5 - (nowpos / 6)) < ymax){  //たてはみだし
    OHNO_PRINT("たてはみだし\n");
    return false;
  }

  PokemonBattleCheckOfTemotiStart( battlechk,pRange->page );

  
  // 最終マージ検査  pRange->x
  for(  y = 0; y < 6; y++){
    x = xbuff[y] + nowpos;

    if((selectmap[y] == 1) && (nowmap[y] == 1)){
      OHNO_PRINT("重なりはＮＧ %d %d\n",y,x);

      if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM != mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時は
        //はずすだけなのでかさなっても大丈夫 それ以外はＮＧ
        return false;
      }
    }

    if(selectmap[y] == 1){
      if(x>=30){
        return false;
      }
      if(bput==RANGETEST_BATTLE){//バトルにでれるかどうか
        PokemonBattleCheckOfTemoti( pRange->page,  y + 30 + pRange->x,  nowpage, x, battlechk);
      }
      if(bput==RANGETEST_EXEC){
        OHNO_PRINT("おいた%d %d -- %d %d r %d\n", pRange->page, y+30, nowpage, x, rangepos[y]);
        PokemonSet( pRange->page,  y + 30 + pRange->x,  nowpage, x, rangepos[y]);
      }
    }
  }

  if(bput==RANGETEST_BATTLE){//バトルにでれるかどうか
    return PokemonBattleCheckOfTemotiEnd( battlechk );
  }

  if(bput==RANGETEST_EXEC){
    mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_AlignMember(0,NULL);
    mpBoxAppSetup->GetBoxSimpleAccesser()->Resort_AlignMember(mpAppParam->nowBattleTray,0,NULL);
    TeamPackAll();
      mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonRangeAnimStart();
  }

  return true;
}


bool BoxPokemonMoveFunction::RangeChangeCheckOrPutBoxToTemoti(RangeData* pRange , int nowpage, int nowpos,int bput)
{
  //置くデータをマップ化 最大３０
  int selectmap[36];
  int nowmap[36];
	int rangepos[36];
	int temochirangepos[6];
  u8 battlechk[6];
  int y;
  int x;
  int pos;
  int xbuff[]={0,1,6,7,12,13};

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);


  for( y = 0; y < 36; y++){
    selectmap[y] = 0;
    nowmap[y] = 0;
		rangepos[y] = 0;
  }
  for(y=0;y<6;y++){
    temochirangepos[y]=-1;
  }

  for(  y = 0; y < pRange->height; y++){
    for(  x = 0; x < pRange->width; x++){
      pos = x + pRange->x + ( y + pRange->y ) * 6;
      GetPokemon( pRange->page, pos ,pp);
      if(!pp->IsNull()){
				GFL_ASSERT(pos<36);
        selectmap[pos] = 1;  //いる
				rangepos[pos] = x + y*6;
        OHNO_PRINT("置くデータ %d いる %d\n",pos,rangepos[pos]);
      }
    }
  }

  // 置くべき場所のデータを抽出 PARTY
  for(  y = 0; y < pRange->height; y++){
    for(  x = 0; x < pRange->width; x++){
      pos = nowpos + x + y * 2;

      GetPokemon( nowpage, pos ,pp);
      if(!pp->IsNull()){
        OHNO_PRINT("置くつもりの場所 %d いる\n",pos);
				GFL_ASSERT(pos<36);
        nowmap[pos] = 1;  //いる
      }
    }
  }
  GFL_SAFE_DELETE(pp);

  x=nowpos-30;
  if((2 - (x % 2)) < pRange->width){  //横はみだし
    OHNO_PRINT("よこはみだし\n");
    return false;
  }
  if((3 - (x / 2)) < pRange->height){  //たてはみだし
    OHNO_PRINT("たてはみだし\n");
    return false;
  }


  // 最終マージ検査
  

  PokemonBattleCheckOfTeamStart(battlechk);

  for(  y = 30; y < 36; y++){
    if((y - nowpos) < 0 || (y - nowpos) >= 6){
      continue;
    }
    x = xbuff[y - nowpos]  + pRange->cursor;
    if(x<0){
      continue;
    }
    if(x>=36){
      continue;
    }
    if((selectmap[x] == 1) && (nowmap[y] == 1)){
      return false;
    }

    if(selectmap[x] == 1){
      if(bput==RANGETEST_BATTLE || bput == RANGETEST_TEAM || bput == RANGETEST_TEAMLOCK){//バトルにでれるかどうかor チームに入ってないかどうかorlockしてるかどうか
        PokemonBattleCheckOfTeam( pRange->page,  x,  nowpage, y, battlechk,bput);
      }
      if(bput==RANGETEST_EXEC){
        OHNO_PRINT("おいた%d %d -- %d %d  r%d\n", pRange->page, x, nowpage, y, rangepos[x]);
        PokemonSet( pRange->page,  x,  nowpage, y, rangepos[x]);
        temochirangepos[y-30]=rangepos[x];
      }
    }
  }

  if(bput==RANGETEST_BATTLE || bput == RANGETEST_TEAM || bput == RANGETEST_TEAMLOCK){//バトルにでれるかどうかor チームに入ってないかどうかorlockしてるかどうか
    return PokemonBattleCheckOfTeamEnd( battlechk );
  }

  if(bput==RANGETEST_EXEC){
    if((mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM) ){
      mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();
    }
    if(!mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_AlignMemberRange(temochirangepos)){
      mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonRangeAnimStart();
    }
    if((mpAppParam->type == BOX_CALL_RESORT_GIM) || (mpAppParam->type == BOX_CALL_RESORT_HOTSPA)){
      if(!mpBoxAppSetup->GetBoxSimpleAccesser()->Resort_AlignMemberRange(mpAppParam->nowBattleTray,temochirangepos)){
        mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonRangeAnimStart();
      }
    }

    TeamPackAll();
  }

  return true;
}


bool BoxPokemonMoveFunction::RangeChangeCheckOrPutTemotiToTemoti( RangeData* pRange , int nowpage, int nowpos,int bput)
{
	int xbuff[]={0,1,6,7,12,13};
  int selectmap[6];
	int rangepos[6];
  int nowmap[6];
	int temochirangepos[6];
  int y;
  int x;
  int z;
  int pos;

  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam * pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);

  for( y = 0; y < 6; y++){
    selectmap[y] = 0;
    nowmap[y] = 0;
		rangepos[y] = 0;
    temochirangepos[y] = -1;
  }

  y=0;
  OHNO_PRINT("num %d x %d skip %d\n",pRange->num,pRange->x,pRange->skip);
  z=-1;
  for(  x = 0; x < 6; x++){
    if(pRange->skip &&  (pRange->x%2 != x%2)  ){
      continue;
    }
    if(pRange->x > x){
      continue;
    }
    if(y>=pRange->num){
      break;
    }
    pos = x + 30;
    GetPokemon( pRange->page, pos ,pp);
    if(!pp->IsNull()){
      y++;
      selectmap[x] = 1;  //いる
      if(z==-1){
        z = x;
      }
      rangepos[x]=xbuff[x-z];
    }
  }
  OHNO_PRINT("置くデータMAP %d %d %d %d %d %d\n",selectmap[0],selectmap[1],selectmap[2],selectmap[3],selectmap[4],selectmap[5]);

  // 置くべき場所のデータを抽出
  for(  x = 0; x < 6; x++){
    GetPokemon( nowpage, x+30 ,pp);
    if(!pp->IsNull()){
      if(nowpage != pRange->page){
        nowmap[x] = 1;  //いる
      }
      else if( selectmap[x] == 1){///もちあげたぽけだからだいじょうぶ
        nowmap[x] = 0;  //いないあつかいにする
      }
      else{
        nowmap[x] = 1;  //いる
      }
    }
  }
  OHNO_PRINT("置かれるデータMAP %d %d %d %d %d %d\n",nowmap[0],nowmap[1],nowmap[2],nowmap[3],nowmap[4],nowmap[5]);
  GFL_SAFE_DELETE(pp);


  // 最終マージ検査  pRange->x

  for(  z = 0; z < 6; z++){
    x = z;
    y = z + (nowpos - 30) - pRange->x;
    if( selectmap[x] == 1 && (y >= 6) ){
      OHNO_PRINT("はみだしてる\n");
      return false;
    }
  }

  for(  z = 6-1; z >= 0; z--){
    x = z;
    y = z + (nowpos - 30) - pRange->x;
    OHNO_PRINT("おきます y = %d x = %d nowpos = %d\n",y , x, nowpos);
    if(y >= 6){
      continue;
    }
    if( selectmap[x] == 1 && nowmap[y]){
      OHNO_PRINT("重なりはＮＧ %d %d\n",x,y);
      return false;
    }
    if(selectmap[x] == 1){
      if(bput==RANGETEST_EXEC){
        OHNO_PRINT("おいた%d %d -- %d %d r%d z%d\n", pRange->page, x+30, nowpage, y+30, rangepos[x],z);
        PokemonSet( pRange->page,  x + 30,  nowpage, y+30, rangepos[x]);
        temochirangepos[y]=rangepos[x];
      }
    }

  }
  if(bput==RANGETEST_EXEC){

    if((mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_POKEMON) ){
      if(!mpBoxAppSetup->GetBoxSimpleAccesser()->Temochi_AlignMemberRange(temochirangepos)){
        mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonRangeAnimStart();
      }
    }
    if((mpAppParam->type == BOX_CALL_RESORT_GIM) || (mpAppParam->type == BOX_CALL_RESORT_HOTSPA)){
      if(!mpBoxAppSetup->GetBoxSimpleAccesser()->Resort_AlignMemberRange(mpAppParam->nowBattleTray,temochirangepos)){
        mpBoxAppSetup->GetBoxSimpleAccesser()->ChangePokemonRangeAnimStart();
      }
    }
    TeamPackAll();
  }

  return true;
}




//ポケモンを入れ替える場合にいれかえ  範囲用 battle normal resortで使用
//もどりは MOVEERR_NONE
int BoxPokemonMoveFunction::RangePokemonSet(int nowcurpos,int selectcurpos, RangeData* pRange, int nowpage)
{
  bool retcode = false;

  
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム用判定
    if((nowcurpos < Savedata::BoxPokemon::TRAY_POKE_MAX) && (selectcurpos>=Savedata::BoxPokemon::TRAY_POKE_MAX)){    //はずす操作をする場合
      if( mpAppParam->pBoxData->IsTeamLock(pRange->page) ){ //チームがロックされているところから動かしてはいけない
        return MOVEERR_TEAM_LOCK_MEMBER_REMOVE;//GFNMCat1772+1932
      }
    }
    if(nowcurpos>= Savedata::BoxPokemon::TRAY_POKE_MAX){
      if(mpAppParam->pBoxData->IsTeamLock(mpAppParam->nowBattleTray)){  //チームがロックされてたら入れられない
        return MOVEERR_TEAM_LOCK;
      }
    }
    if((nowcurpos < Savedata::BoxPokemon::TRAY_POKE_MAX) && (selectcurpos<Savedata::BoxPokemon::TRAY_POKE_MAX)){    //BOXtoBOXの操作は無い
      return MOVEERR_NO_PUT_PLACE;  //入れ替え失敗扱い  GFNMCat[1931]
    }
  }
/*  {
    int chkret = pokeMoveCheckRange(selectcurpos, pRange);

    if(chkret==MOVEERR_TEAM_LOCK){
      return chkret;
    }
  }*/
  {
    if(RangeExec(nowcurpos, selectcurpos, pRange, nowpage, RANGETEST_MOVE) ){
      if(!RangeExec(nowcurpos, selectcurpos, pRange, nowpage, RANGETEST_BATTLE) ){
        return MOVEERR_NO_BATTLEPOKE;
      }
      if(!RangeExec(nowcurpos, selectcurpos, pRange, nowpage, RANGETEST_TEAM) ){
        return MOVEERR_TEAM_ENTRY;
      }
      if(!RangeExec(nowcurpos, selectcurpos, pRange, nowpage, RANGETEST_TEAMLOCK) ){
        return MOVEERR_TEAM_LOCK_MEMBER;
      }
      retcode = true;
    }
  }
  if(retcode == true){
    OHNO_PRINT("いれかえ合格\n");
    RangeExec(nowcurpos, selectcurpos, pRange, nowpage, RANGETEST_EXEC);
    Sound::PlaySE(SEQ_SE_RELEASE);
  }
  else{
    OHNO_PRINT("いれかえ失敗\n");
    return MOVEERR_NO_PUT_PLACE;
  }
  return MOVEERR_NONE;
}


bool BoxPokemonMoveFunction::RangeExec(int nowcurpos,int selectcurpos, RangeData* pRange, int nowpage, int mode)
{
  bool retflg=false;

  if(nowcurpos>=Savedata::BoxPokemon::TRAY_POKE_MAX && selectcurpos>=Savedata::BoxPokemon::TRAY_POKE_MAX){
    retflg= RangeChangeCheckOrPutTemotiToTemoti(pRange, nowpage, nowcurpos, mode);
  }
  else if(selectcurpos>=Savedata::BoxPokemon::TRAY_POKE_MAX){
    retflg = RangeChangeCheckOrPutTemotiToBox(pRange, nowpage, nowcurpos, mode);
  }
  else if(nowcurpos>=Savedata::BoxPokemon::TRAY_POKE_MAX){
    retflg = RangeChangeCheckOrPutBoxToTemoti(pRange, nowpage, nowcurpos, mode);
  }
  else{
    retflg = RangeChangeCheckOrPutBoxToBox(pRange, nowpage, nowcurpos, mode);
  }
  return retflg;
}






//複数移動をやっていいかどうかの検査
bool BoxPokemonMoveFunction::rangeCheck(int pos1,int pos2)
{
  if(pos1 < Savedata::BoxPokemon::TRAY_POKE_MAX){
    if(pos2 < Savedata::BoxPokemon::TRAY_POKE_MAX){
      return true;
    }
  }
  if(pos1 >= Savedata::BoxPokemon::TRAY_POKE_MAX && (pos1 < Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS)){ //NMCat2268
    if(pos2 >= Savedata::BoxPokemon::TRAY_POKE_MAX && (pos2 < Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS)){
      return true;
    }
  }
  return false;
}


bool BoxPokemonMoveFunction::SetCarryRange(RangeData* pRange,int* pnowcur,int* pselect, int nowtrayno)
{
  if(*pnowcur == 0x20 && *pselect==0x1f){
    *pnowcur=33;
    *pselect=30;
  }
  if(*pnowcur==0x22 && *pselect==0x21){
    *pnowcur=35;
    *pselect=32;
  }
  if(*pnowcur==0x22 && *pselect==0x1f){
    *pnowcur=35;
    *pselect=30;
  }
  if(*pnowcur==33 && *pselect==34){
    *pnowcur=35;
    *pselect=32;
  }
  if(*pnowcur==31 && *pselect==34){
    *pnowcur=35;
    *pselect=30;
  }
  if(*pnowcur==31 && *pselect==32){
    *pnowcur=33;
    *pselect=30;
  }

  ///ポケモンをもちあげる
  int inx = *pnowcur % 6;
  int iny = *pnowcur / 6;
  int outx = *pselect % 6;
  int outy = *pselect / 6;

  int startx=inx, starty=iny, sizex=1,sizey=1;

  if(inx > outx){
    startx=outx;
  }
  if(iny > outy){
    starty=outy;
  }
  if(inx!=outx){
    sizex = gfl2::math::Abs(inx-outx)+1;
  }
  if(iny!=outy){
    sizey = gfl2::math::Abs(iny-outy)+1;
  }

  if(!rangeCheck(*pnowcur ,*pselect)){
    return false;   //範囲が異なる場合キャンセル
  }

  int pokenum = mpBoxAppSetup->GetBoxSimpleAccesser()->RangePokemonNum( startx,starty, sizex,sizey,nowtrayno,(pRange->skip!=0) );

  if(pokenum==0){   //ポケモンがいないときはキャンセル
    return false;
  }
  pRange->num=pokenum;
  pRange->x = startx;
  pRange->y = starty;
  pRange->width = sizex;
  pRange->height = sizey;
  *pnowcur  = startx + starty * 6;
  GFL_ASSERT(*pnowcur>=0);
  pRange->page = nowtrayno;
  pRange->cursor = *pnowcur;
//  *pselect = nowtrayno;

	return true;
}




//交換に出してよいか まずい場合戻り値で帰す
int BoxPokemonMoveFunction::FirstTradeCheck(int tray, int pos)
{
  gfl2::heap::HeapBase * heap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::CoreParam* cp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );

  GetPokemon(tray,pos, cp);
	GFL_ASSERT(!cp->IsNull());  //ここに来るフローの段階でいない事が担保されて無いといけない

  bool bEgg = cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG);

  bool bCondition = mpBoxPokeParaFunc->CheckTradeCondition(cp, &mpAppParam->trade);

  bool bNijiCompatibleNg = false;

  // momiji追加:niji互換対応
  {
    if( mpAppParam->isNijiCompatibility == true )
    {
      if( PokeRegulation::CheckNijiCompatible( cp ) == true )
      {
        GFL_RELEASE_PRINT( ">BOX 互換対応ポケモンだったのでAlerMsg\n" );
        bNijiCompatibleNg = true;
      }
    }
  }

  GFL_SAFE_DELETE(cp);

  switch( mpAppParam->type ){
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
    if(bEgg){   //たまごは交換できない
      return MOVEERR_EGG_PULL;  //      simpleWarrningMessage(msg_box_05_03, 2, MM_SEQ_CLOSE_MESSAGE);
    }
    if(!bCondition){
      return MOVEERR_NO_CONDITION;  //      simpleWarrningMessage(msg_box_05_01, 2, MM_SEQ_CLOSE_MESSAGE);
    }
    break;
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    break;
  }
  
  if( bNijiCompatibleNg )
  {
    return MOVEERR_COMPATIBLE_ERROR;
  }

  return MOVEERR_NONE;
}






GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




