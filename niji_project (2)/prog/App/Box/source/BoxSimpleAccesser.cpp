//===================================================================
/**
 *	@file	    BoxSimpleAccesser.cpp
 *	@brief    ポケモンアイコン表示の統合クラス
 *	@author	  k.ohno
 *	@date		  2015.11.14
 */
//===================================================================

#include "BoxSimpleAccesser.h"

#include "System/include/HeapDefine.h"
#include <GameSys/include/GameProcManager.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"


#include "niji_conv_header/poke_lib/monsno_def.h"
#include "niji_conv_header/poke_lib/wazano_def.h"
#include "niji_conv_header/poke_lib/item/itemsym.h"
#include "System/include/PokemonVersion.h"

#include "PokeTool/include/PokeTool.h"
#include "App/Box/include/BoxAppSetup.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------

#define ICONANIM_FRAME (4.0f)

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
BoxSimpleAccesser::BoxSimpleAccesser(void)
{
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
BoxSimpleAccesser::~BoxSimpleAccesser()
{
}

void BoxSimpleAccesser::Init(App::Box::APP_PARAM* pParam,App::Box::BoxAppSetup* pBoxAppSetup,App::BoxSearch::APP_PARAM* pBoxSearch)
{
  int i;

  mpAppParam = pParam;
  gfl2::heap::HeapBase * heap = mpAppParam->mpAppHeap->GetDeviceHeap();
  mpTempPokemonParam = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);  //受け渡し用
  mpTempPokemonParam2 = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);  //受け渡し用
  mpPartyTemp = GFL_NEW( heap ) pml::PokeParty( heap );

  for( i=0;i<pml::PokeParty::MAX_MEMBERS;i++){
    mpPartyBuff[i] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);
  }

  for( i=0;i<pml::PokeParty::MAX_MEMBERS;i++){
    mpPartyBuffTemp[i] = GFL_NEW(heap) pml::pokepara::CoreParam(heap);
  }

  GFL_ASSERT(mpAppParam->pTemochi2);
  Temochi_PartyToThis(mpAppParam->pTemochi2);

  mpBoxAppSetup = pBoxAppSetup;
  mpBoxSearch = pBoxSearch;

  mBackupChnagePokeNum=0;
}






void BoxSimpleAccesser::End(void)
{
  int i;
  
  for( i=0;i<pml::PokeParty::MAX_MEMBERS;i++){
    GFL_SAFE_DELETE(mpPartyBuff[i]);
  }
  for( i=0;i<pml::PokeParty::MAX_MEMBERS;i++){
    GFL_SAFE_DELETE(mpPartyBuffTemp[i]);
  }

  GFL_SAFE_DELETE(mpPartyTemp);
  GFL_SAFE_DELETE(mpTempPokemonParam2);
  GFL_SAFE_DELETE(mpTempPokemonParam);
}


void BoxSimpleAccesser::SetupTemochi(void)
{
  for(int j=0;j<pml::PokeParty::MAX_MEMBERS;j++){
    makeTemotiSimpleData(j);
  }
}

void BoxSimpleAccesser::SetupBattleTeam(void)
{
  switch(mpAppParam->type){
  case BOX_CALL_PC:        //パソコン ※デフォルト
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
  case BOX_CALL_FIELD_TRADE: //フィールドの交換
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択
    makeBattleBoxSimpleData();
    break;
  case BOX_CALL_RESORT_GIM:   //ポケリゾートうんどう
    makeResortGimSimpleData();
    break;
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾートおんせん
    makeResortHotspaSimpleData();
    break;
  }
}

bool BoxSimpleAccesser::IsWazaHave(const pml::pokepara::CoreParam* cp,int wazano)
{
	for( u8 i=0; i<cp->GetWazaCount(); i++ ) 
	{
		WazaNo own_wazano = cp->GetWazaNo(i);
		if( own_wazano == wazano ) {
			return true;
		}

	} 
	return false;
}

bool BoxSimpleAccesser::IsTypeHave(const pml::pokepara::CoreParam* cp,int type)
{

	if(cp->GetType1() == type){
			return true;
  }
	if(cp->GetType2() == type){
		return true;
	}

	return false;


}


/*
  @fix NMCat[3189]: 検索結果の設定をdefSetupPokemon()に集約、チーム検索用に"tray"と"pos"を追加
    tray = cpのボックストレイ番号　※手持ち、リゾート関連は-1
    pos  = cpのトレイ内の位置　　　※手持ち、リゾート関連は-1
*/
void BoxSimpleAccesser::defSetupPokemon(PokemonIconDraw* pPkd,const pml::pokepara::CoreParam* cp,int tray,int pos)
{
  bool begg = cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG);
  int itemno = cp->GetItem();

  if(cp->IsNull()){
    pPkd->bHide = 1;
  }
  else{
    pPkd->bHide = 0;
  }
  if(mpBoxSearch->endMode  != App::BoxSearch::END_MODE_SEARCH)
  {
    if(itemno != ITEM_DUMMY_DATA)
    {
      pPkd->bItem = true;
    }
    else
    {
      pPkd->bItem = false;
    }
  }
  else{
    pPkd->bItem = true;
  }

  pPkd->carryup = false;
  pPkd->bblack=false;

  if(mpAppParam->trade.bOn){
    pPkd->bblack=false;

    if(mpAppParam->trade.MonsNo != MONSNO_NULL){ //!< ポケモン番号
      if(mpAppParam->trade.MonsNo != cp->GetMonsNo() || begg){
				pPkd->bblack = true;
			}
		}

		if( mpAppParam->trade.Gender != pml::SEX_NUM ){
			if( begg == true || mpAppParam->trade.Gender != cp->GetSex() ){
        pPkd->bblack = true;
			}
		}
		if( mpAppParam->trade.Level != 0 ){
			if( begg == true || (mpAppParam->trade.Level > cp->GetLevel()) || ((mpAppParam->trade.Level+10) <= cp->GetLevel())){
        pPkd->bblack = true;
			}
		}
  }

  if(mpBoxSearch->endMode  == App::BoxSearch::END_MODE_SEARCH)
	{
		
		// ボックスマーク
		for( int i=0; i<static_cast<int>(pml::pokepara::BOX_MARK_NUM); i++ ){
			if(mpBoxSearch->mark.markColor[i] != pml::pokepara::BOX_MARK_COLOR_NONE){
				if( cp->GetBoxMark( (pml::pokepara::BoxMark)i ) != mpBoxSearch->mark.markColor[i] ){
					pPkd->bblack=true;
				}
			}
		}
    if(mpBoxSearch->monsNo != MONSNO_NULL){ //!< ポケモン番号
      if(mpBoxSearch->monsNo != cp->GetMonsNo() || begg){
				pPkd->bblack=true;
			}
		}
		if (mpBoxSearch->wazaNo != WAZANO_NULL)//!< わざ番号
		{
			if (!IsWazaHave(cp,mpBoxSearch->wazaNo) || begg)
			{
				pPkd->bblack=true;
			}
		}

		if(mpBoxSearch->item == App::BoxSearch::CONDITIONAL_TRUE)
		{
      if(itemno == ITEM_DUMMY_DATA){
				pPkd->bblack=true;
      }
    }
		if(mpBoxSearch->item == App::BoxSearch::CONDITIONAL_FALSE)
		{
      if(itemno != ITEM_DUMMY_DATA){
        pPkd->bblack=true;
      }
    }
		if (mpBoxSearch->type1!=POKETYPE_NULL)
		{
			if(!IsTypeHave(cp,mpBoxSearch->type1) || begg){
				pPkd->bblack=true;
			}
		}
		if (mpBoxSearch->type2!=POKETYPE_NULL)
		{
			if(!IsTypeHave(cp,mpBoxSearch->type2) || begg){
				pPkd->bblack=true;
			}
		}
    /*
		if(mpBoxSearch->item != ITEM_DUMMY_DATA){  //まちがい
			if(cp->GetItem()!=mpBoxSearch->item || begg){
				pPkd->bblack=true;
			}
		}*/
		if( mpBoxSearch->wazaMachine != App::BoxSearch::CONDITIONAL_NULL ){
			if( begg == true || cp->CheckWazaMachine(mpBoxSearch->wazaMachine-1) == false ){
				pPkd->bblack=true;
			}
		}
		if( mpBoxSearch->sex != pml::SEX_NUM ){
			if( begg == true || mpBoxSearch->sex != cp->GetSex() ){
				pPkd->bblack=true;
			}
		}
		// 性格
		if( mpBoxSearch->seikaku != pml::pokepara::SEIKAKU_NUM ){
			if( begg == true || mpBoxSearch->seikaku != cp->GetSeikaku() ){
				pPkd->bblack=true;
			}
		}
		// 特性
		if( mpBoxSearch->tokusei != TOKUSEI_NULL ){
			if( begg == true || mpBoxSearch->tokusei != cp->GetTokuseiNo() ){
				pPkd->bblack=true;
			}
		}

    // チーム
    if( mpBoxSearch->teamNo != App::BoxSearch::CONDITIONAL_NULL )
    {
      if( tray != -1 && pos != -1 )
      {
        if( mpAppParam->pBoxData->IsTeamInMe( tray, pos, mpBoxSearch->teamNo-1 ) == -1 )
        {
          pPkd->bblack=true;
        }
      }
      else
      {
        pPkd->bblack=true;
      }
    }
  }
  
}



void BoxSimpleAccesser::SetupBoxPokemon(int page,int i)
{
  pml::pokepara::CoreParam* cp = mpTempPokemonParam;

  mpAppParam->pBoxPokemon->GetPokemon( cp,  page, i);
  PokeTool::GetSimpleParam( &mBoxPokeIcon[page][i].simp , cp );

  defSetupPokemon(&mBoxPokeIcon[page][i], cp, page, i);

  mBoxPokeIcon[page][i].bLock = false;

  //チームマークの登録
  for(int teamno = 0; teamno < Savedata::BOX::TEAM_MAX; teamno++){
    int teamno2 = mpAppParam->pBoxData->IsTeamInMe(page, i, teamno);
    mBoxPokeIcon[page][i].bTeam[teamno] = false;
		if(teamno2==-1){
		}
		else{//チームに登録されている
      mBoxPokeIcon[page][i].bTeam[teamno] = true;
      if(mpAppParam->pBoxData->IsTeamLock(teamno)){
        mBoxPokeIcon[page][i].bLock = true;
      }
    }
  }

  // @fix GFNMCat[5377]: リゾートの場合はチームに対して検索中処理をしない
  if( mpAppParam->type == BOX_CALL_RESORT_HOTSPA || mpAppParam->type == BOX_CALL_RESORT_GIM )
  {
    return;
  }
  // 以下、検索中の追加処理
  if( mpBoxSearch->endMode != App::BoxSearch::END_MODE_SEARCH )
  {
    return;
  }
  // @fix NMCat[2929]: チームに登録されている場合はチーム内のデータも更新する
  for( int k=0; k<Savedata::BOX::TEAM_MAX; k++ )
  {
    for( int j=0; j<pml::PokeParty::MAX_MEMBERS; j++ )
    {
      u16 boxno = mpAppParam->pBoxData->GetTeamPokePos( k, j );
      if( (boxno>>8) == page && (boxno&0xff) == i )
      {
        PokeTool::GetSimpleParam( &mTeamPokeIcon[k][j].simp, cp );
        defSetupPokemon( &mTeamPokeIcon[k][j], cp, page, i );
        if( mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM && k == mpAppParam->nowBattleTray )
        {
          // @fix NMCat[3140]: 表示更新中は何もしない
          if( mTeamPokeIcon[k][j].nowRedraw == 0 )
          {
            SetPokeIconDrawAll( k, j+Savedata::BoxPokemon::TRAY_POKE_MAX );   // @fix NMCat[3653]: アイコンも書き換える
          }
        }
        break;
      }
    }
  }
}




void BoxSimpleAccesser::SetupBox(void)
{
  pml::pokepara::CoreParam* cp = mpTempPokemonParam;
  
  for(int j=0;j<Savedata::Box::TRAY_MAX;j++){
    for(int i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){
      SetupBoxPokemon(j,i);
    }
  }
}


void BoxSimpleAccesser::makeTemotiSimpleData(int j)
{
//  const pml::pokepara::PokemonParam* pp = mpAppParam->pTemochi->GetMemberPointerConst( j );
  const pml::pokepara::PokemonParam* pp = Temochi_GetMemberPointerConst( j );
  PokeTool::GetSimpleParam( &mTemotiPokeIcon[j].simp , pp );
  defSetupPokemon(&mTemotiPokeIcon[j], pp, -1, -1);
}




//バトルボックス単体セットアップ
void BoxSimpleAccesser::_makeBattleSimpleData(const u16 boxno, int x, App::Box::PokemonIconDraw* ptemp,pml::pokepara::CoreParam* cp)
{
  int tray = -1;
  int pos  = -1;

  if(boxno!=(u16)Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE){
    tray = boxno >> 8;
    pos  = boxno & 0xff;
    mpAppParam->pBoxPokemon->GetPokemon( cp, tray, pos );
  }
  else{
    cp->Clear();
  }
  PokeTool::GetSimpleParam( &ptemp->simp , cp );
  defSetupPokemon(ptemp, cp, tray, pos);

  // 以下、検索中の追加処理
  if( mpBoxSearch->endMode != App::BoxSearch::END_MODE_SEARCH )
  {
    return;
  }
  if( mpBoxSearch->teamNo == App::BoxSearch::CONDITIONAL_NULL )
  {
    return;
  }
  // @fix NMCat[2933][3133]: 登録元の表示を更新
  if( boxno != static_cast<u16>(Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE) )
  {
    PokeTool::GetSimpleParam( &mBoxPokeIcon[tray][pos].simp, cp );
    defSetupPokemon( &mBoxPokeIcon[tray][pos], cp, tray, pos );
    if( tray == mpAppParam->nowDispTray )
    {
      // @fix NMCat[3140]: 表示更新中は何もしない
      if( mBoxPokeIcon[tray][pos].nowRedraw == 0 )
      {
        SetPokeIconDrawState( tray, pos );
      }
    }
  }
}




//バトルボックス単体セットアップ
void BoxSimpleAccesser::makeBattleSimpleData(int x, int j)
{
  const u16 boxno = mpAppParam->pBoxData->GetTeamPokePos(x,j);
    pml::pokepara::CoreParam* cp =mpTempPokemonParam;

  _makeBattleSimpleData(boxno,x,&mTeamPokeIcon[x][j],cp);
}


//バトルボックスデータからsimpleデータ作成

void BoxSimpleAccesser::makeBattleBoxSimpleData(void)
{
  for(int x=0;x<Savedata::BOX::TEAM_MAX;x++){
    for(int j=0;j<Savedata::BOX::TEAM_NUM_MAX;j++){
      makeBattleSimpleData(x,j);
    }
  }
}



//ジムのsimpleデータ作成
void BoxSimpleAccesser::makeResortGimSimpleData(int page,int pos)
{
  pml::pokepara::CoreParam* cp =mpTempPokemonParam;

  mpAppParam->pResortSave->GetGimPokemon(page*pml::PokeParty::MAX_MEMBERS + pos, cp);
  
  PokeTool::GetSimpleParam( &mTeamPokeIcon[page][pos].simp , cp );
  defSetupPokemon(&mTeamPokeIcon[page][pos], cp, -1, -1);
}



//ジム用
void BoxSimpleAccesser::makeResortGimSimpleData(void)
{
  for(int x=0;x<Savedata::ResortSave::STOCKPOKEMON_MAX;x++){
    makeResortGimSimpleData(x / pml::PokeParty::MAX_MEMBERS, x % pml::PokeParty::MAX_MEMBERS);
  }
}


//温泉のsimpleデータ作成
void BoxSimpleAccesser::makeResortHotspaSimpleData(int page,int pos)
{
  pml::pokepara::CoreParam* cp =mpTempPokemonParam;

  mpAppParam->pResortSave->GetHotSpaPokemon(page*pml::PokeParty::MAX_MEMBERS + pos, cp);
  
  PokeTool::GetSimpleParam( &mTeamPokeIcon[page][pos].simp , cp );
  defSetupPokemon(&mTeamPokeIcon[page][pos], cp, -1, -1);
}


//温泉用
void BoxSimpleAccesser::makeResortHotspaSimpleData(void)
{
  for(int x=0;x<Savedata::ResortSave::STOCKPOKEMON_MAX;x++){
    makeResortHotspaSimpleData(x / pml::PokeParty::MAX_MEMBERS, x % pml::PokeParty::MAX_MEMBERS);
  }
}


void BoxSimpleAccesser::RefreashPokemon(int selectpage, int selectpos)
{

  if(selectpos < Savedata::BoxPokemon::TRAY_POKE_MAX){
    SetupBoxPokemon(selectpage,selectpos);
  }
  else if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
    makeBattleSimpleData(selectpage, selectpos - Savedata::BoxPokemon::TRAY_POKE_MAX );
  }
  else{
    switch( mpAppParam->type ){
    case BOX_CALL_PC:        //パソコン
    case BOX_CALL_TRADE:    //P2P交換
    case BOX_CALL_FIELD_TRADE:  //フィールド交換
    case BOX_CALL_GTS:      //GTS
    case BOX_CALL_MIRACLE:   //ミラクル交換
    case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択だがてもち そうでないときはてもち
      makeTemotiSimpleData( selectpos - Savedata::BoxPokemon::TRAY_POKE_MAX   );
      break;
    case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
      makeResortHotspaSimpleData(selectpage, selectpos - Savedata::BoxPokemon::TRAY_POKE_MAX);
      break;
    case BOX_CALL_RESORT_GIM:   //ポケリゾート
      makeResortGimSimpleData(selectpage, selectpos - Savedata::BoxPokemon::TRAY_POKE_MAX);
      break;
    }
  }
}


//移動するポケモンの状態を即時反映できないので予約
void BoxSimpleAccesser::ChangePokemonReserve(int selectpage, int selectpos, int nowpage, int nowpos, int rangepos)
{
  mBackupChnagePoke[mBackupChnagePokeNum].selectpage = selectpage;
  mBackupChnagePoke[mBackupChnagePokeNum].selectpos = selectpos;
  mBackupChnagePoke[mBackupChnagePokeNum].nowpage = nowpage;
  mBackupChnagePoke[mBackupChnagePokeNum].nowpos = nowpos;
  mBackupChnagePoke[mBackupChnagePokeNum].rangepos = rangepos;
  mBackupChnagePokeNum++;
  GFL_ASSERT(mBackupChnagePokeNum<=Savedata::BoxPokemon::TRAY_POKE_MAX);
}

//実際の交換を行う
void BoxSimpleAccesser::ChangePokemonUseStock(void)
{
  ResetPokemonIconDrawData();

  for(int i=0;i<mBackupChnagePokeNum;i++){
    ChangePokemon(mBackupChnagePoke[i].selectpage, mBackupChnagePoke[i].selectpos,
                  mBackupChnagePoke[i].nowpage,  mBackupChnagePoke[i].nowpos);
  }
  mBackupChnagePokeNum=0;
}


//範囲移動アイコンをアニメする為に情報を取り出す
BoxSimpleAccesser::StockChangePos* BoxSimpleAccesser::GetChangePokemonReserveData(int index)
{
  if(mBackupChnagePokeNum <= index){
    return NULL;
  }
  return &mBackupChnagePoke[index];
}





void BoxSimpleAccesser::ChangePokemon(int selectpage, int selectpos, int nowpage, int nowpos)
{

  RefreashPokemon(selectpage, selectpos);
  RefreashPokemon(nowpage, nowpos);
  SetPokeIconDrawAll(selectpage,selectpos);
  SetPokeIconDrawAll(nowpage,nowpos);

}


App::Box::PokemonIconDraw* BoxSimpleAccesser::GetPokemonIconDrawData(int disp,int battledisp ,int pos)
{
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode() && pos>=30){  //バトルチームモードの時はバトルチーム
    return GetPokemonIconDrawData(battledisp,pos);
  }
  switch( mpAppParam->type ){
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択 そうでないときはてもち
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    if(pos>=30){
      return GetPokemonIconDrawData(battledisp,pos);
    }
  }
  return GetPokemonIconDrawData(disp,pos);  //
}



App::Box::PokemonIconDraw* BoxSimpleAccesser::GetPokemonIconDrawData(int disp ,int pos)
{
  if(pos<0 || disp<0){
    return NULL;
  }
  if(pos < Savedata::BoxPokemon::TRAY_POKE_MAX){
    return &mBoxPokeIcon[disp][pos];
  }
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
    return &mTeamPokeIcon[disp][pos - Savedata::BoxPokemon::TRAY_POKE_MAX];
  }
  switch( mpAppParam->type ){
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択 そうでないときはてもち
    return &mTemotiPokeIcon[pos - Savedata::BoxPokemon::TRAY_POKE_MAX];
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
  return &mTeamPokeIcon[disp][pos - Savedata::BoxPokemon::TRAY_POKE_MAX];  //リゾートは手持ちない

}


PokeTool::SimpleParam* BoxSimpleAccesser::GetSimpleData(int disp ,int pos)
{
  PokemonIconDraw* pselect = GetPokemonIconDrawData(disp,pos);
  return &pselect->simp;
}



void BoxSimpleAccesser::SetVisible(int page,int cursor, bool visible)
{
  u8 bhide = 0;
  if(!visible){
    bhide = 1;
  }
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);
  pPokeID->bHide = bhide;
  SetPokeIconDrawState(page,cursor);
  
}


void BoxSimpleAccesser::SetCarryFlg(int page,int cursor, bool carryup)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);
  pPokeID->carryup = carryup;
  //持ち上げフラグはフォルダー上画面で操作する
}



void BoxSimpleAccesser::SetTeamIcon(int page,int cursor, int team, bool visible)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);

  GFL_ASSERT(team < Savedata::BOX::TEAM_MAX);
  pPokeID->bTeam[team] = visible;
  SetPokeIconDrawState(page,cursor);
}

//ポケモンデータをボックス位置、カーソル位置から得る
void BoxSimpleAccesser::GetPokemon(int boxno,int cursorno, pml::pokepara::CoreParam* cp)
{
  int tno = cursorno-Savedata::BoxPokemon::TRAY_POKE_MAX;

  cp->Clear();

  if((cursorno >= Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS)){  //BOXトレイ
    return;  //範囲外
  }
  if((cursorno < Savedata::BoxPokemon::TRAY_POKE_MAX)){  //BOXトレイ
    mpAppParam->pBoxPokemon->GetPokemon(cp, boxno, cursorno);  //BOXからぬく
    return;
  }
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
    const u16 boxno2 = mpAppParam->pBoxData->GetTeamPokePos(boxno, tno);
    if(boxno2!=(u16)Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE){
      mpAppParam->pBoxPokemon->GetPokemon( cp,  boxno2 >> 8 , boxno2&0xff);
    }
    return;
  }
  switch( mpAppParam->type ){
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択 そうでないときはてもち
    {
//      const pml::pokepara::PokemonParam* pp = mpAppParam->pTemochi->GetMemberPointerConst( tno );
      const pml::pokepara::PokemonParam* pp = Temochi_GetMemberPointerConst( tno );
      cp->CopyFrom(*pp);
    }
    break;
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
    mpAppParam->pResortSave->GetHotSpaPokemon( boxno*6+tno , cp);
    break;
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    mpAppParam->pResortSave->GetGimPokemon( boxno*6+tno , cp);
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}


//ポケモンデータをボックス位置、カーソル位置から得る
void BoxSimpleAccesser::GetPokemonPokemonParam(int boxno,int cursorno, pml::pokepara::PokemonParam* cp)
{
  int tno = cursorno-Savedata::BoxPokemon::TRAY_POKE_MAX;

  cp->Clear();

  if((cursorno >= Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS)){  //BOXトレイ
    return;  //範囲外
  }
  if((cursorno < Savedata::BoxPokemon::TRAY_POKE_MAX)){  //BOXトレイ
    mpAppParam->pBoxPokemon->GetPokemon(cp, boxno, cursorno);  //BOXからぬく
    return;
  }
  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
    const u16 boxno2 = mpAppParam->pBoxData->GetTeamPokePos(boxno, tno);
    if(boxno2!=(u16)Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE){
      mpAppParam->pBoxPokemon->GetPokemon( cp,  boxno2 >> 8 , boxno2&0xff);
    }
    return;
  }
  switch( mpAppParam->type ){
  case BOX_CALL_PC:        //パソコン
  case BOX_CALL_TRADE:    //P2P交換
  case BOX_CALL_FIELD_TRADE:  //フィールド交換
  case BOX_CALL_GTS:      //GTS
  case BOX_CALL_MIRACLE:   //ミラクル交換
  case BOX_CALL_BATTLE_TEAM:   //バトルチーム選択 そうでないときはてもち
    {
//      const pml::pokepara::PokemonParam* pp = mpAppParam->pTemochi->GetMemberPointerConst( tno );
      const pml::pokepara::PokemonParam* pp = Temochi_GetMemberPointerConst( tno );
      cp->CopyFrom(*pp);
    }
    break;
  case BOX_CALL_RESORT_HOTSPA:   //ポケリゾート
    mpAppParam->pResortSave->GetHotSpaPokemon( boxno*6+tno , cp);
    break;
  case BOX_CALL_RESORT_GIM:   //ポケリゾート
    mpAppParam->pResortSave->GetGimPokemon( boxno*6+tno , cp);
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}




int BoxSimpleAccesser::GetPokeIconActive(int page,int cursor)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);
  return !pPokeID->bPassive;
}


void BoxSimpleAccesser::SetPokeIconActive(int page,int cursor,bool bact)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);
  pPokeID->bPassive = !bact;


  SetPokeIconDrawState(page,cursor);
  
}


bool BoxSimpleAccesser::IsTeamIcon(App::Box::PokemonIconDraw* pPkd)
{
  int i;
  for(i=0;i<Savedata::BOX::TEAM_MAX;i++)
  {
    if(pPkd->bTeam[i])
    {
      return true;
    }
  }
  return false;
}


int BoxSimpleAccesser::RangePokemonNum(int startx, int starty, int sizex, int sizey,int page, bool skipSelect)
{
  int num=0;
  
  for(int ypos = 0 ; ypos < sizey; ypos++){
    for(int xpos = 0; xpos < sizex; xpos++){
      int cur = (xpos+startx) + (ypos+starty)*6;
      int regpos = xpos + ypos * 6;
      if(skipSelect && (xpos % 2==1)){
        continue;
      }
      PokeTool::SimpleParam* pPara = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSimpleData(page, cur);
      if(MONSNO_NULL != pPara->monsNo){
        num++;
      }
    }
  }
  return num;

}



///描画をさせる stateのみ
void BoxSimpleAccesser::SetPokeIconDrawState(int page,int cursor)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);
  pPokeID->nowRedraw = 1;
}


///描画をさせる 全部
void BoxSimpleAccesser::SetPokeIconDrawAll(int page,int cursor)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);
  pPokeID->nowRedraw = 2;
}

///スクロールさせたものに置き換える 全部
void BoxSimpleAccesser::SetPokeIconChangeAll(int page,int cursor)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);
  pPokeID->nowRedraw = 4;
}



//アイコン情報をバックアップ
void BoxSimpleAccesser::PushPokemonIconDrawData(int page,int cursor,int x,int y)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);

	if(mPkdBackupNum<(Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS)){
  gfl2::std::MemCopy( pPokeID, &mBackup[mPkdBackupNum].PokeIconDraw, sizeof(PokemonIconDraw) );
  mBackup[mPkdBackupNum].page = page;
  mBackup[mPkdBackupNum].pos = cursor;
  mBackup[mPkdBackupNum].x = x;
  mBackup[mPkdBackupNum].y = y;
  mPkdBackupNum++;
	}
  GFL_ASSERT(mPkdBackupNum<(Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS));
}


//アイコン情報をリフレッシュ=>再描画命令
void BoxSimpleAccesser::RefreshPokemonIconDrawData(void)
{
  for(int i = mPkdBackupNum - 1 ; i >= 0 ; i--){
    PokemonIconDraw* pPokeID = GetPokemonIconDrawData(mBackup[i].page ,mBackup[i].pos);
    if(pPokeID){
      gfl2::std::MemCopy( &mBackup[i].PokeIconDraw, pPokeID, sizeof(PokemonIconDraw) );
      SetPokeIconDrawAll(mBackup[i].page ,mBackup[i].pos);
    }
  }
  mPkdBackupNum=0;
}


//アイコン情報をリセット
void BoxSimpleAccesser::ResetPokemonIconDrawData(void)
{
  mPkdBackupNum=0;
}



//ポケモンデータをアイコン情報から得る
void BoxSimpleAccesser::GetCarryPokemon(int index, pml::pokepara::CoreParam* cp)
{
  if(index < mPkdBackupNum){
    GetPokemon(mBackup[index].page,mBackup[index].pos,cp);
  }
  else{
    cp->Clear();
  }
}




pml::PokeParty* BoxSimpleAccesser::GetCarryParty(void)
{
  gfl2::heap::HeapBase * heap = mpAppParam->mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::PokemonParam* pp = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);

  mpPartyTemp->Clear();
  for(int i=0;i<mPkdBackupNum;i++){
    GetCarryPokemon(i, pp);
    if(!mpTempPokemonParam->IsNull()){
      mpPartyTemp->AddMember(*pp);
    }
  }
  GFL_SAFE_DELETE(pp);
  return mpPartyTemp;
}


//検索したポケモンがいるかどうか
int BoxSimpleAccesser::GetSearchPokemonNum(int page)
{
  int num=0;
  for(int i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){
    if(mBoxPokeIcon[page][i].simp.monsNo!=MONSNO_NULL){
      if(mBoxPokeIcon[page][i].bblack!=true){
        num++;
      }
    }
  }
  return num;
}


//ボックスのみ かずを数える
int BoxSimpleAccesser::GetPokemonCount(int page)
{
  int num=0;
  for(int i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){
    if(mBoxPokeIcon[page][i].simp.monsNo!=MONSNO_NULL){
      num++;
    }
  }
  return num;
}


bool BoxSimpleAccesser::IsSearchMode(void)
{
  if(mpBoxSearch->endMode  == App::BoxSearch::END_MODE_SEARCH){
    return true;
  }
  return false;
}


void BoxSimpleAccesser::SetPokeIconWhite(int page,int cursor, bool bWhite)
{
  PokemonIconDraw* pPokeID = GetPokemonIconDrawData(page ,cursor);
  pPokeID->bwhite = bWhite;
  SetPokeIconDrawState(page,cursor);
}


void BoxSimpleAccesser::ChangeTray(int box1no, int box2no)
{
  App::Box::PokemonIconDraw temp;

  for(int i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){
    gfl2::std::MemCopy( &mBoxPokeIcon[box1no][i], &temp, sizeof(PokemonIconDraw) );
    gfl2::std::MemCopy( &mBoxPokeIcon[box2no][i], &mBoxPokeIcon[box1no][i], sizeof(PokemonIconDraw) );
    gfl2::std::MemCopy( &temp, &mBoxPokeIcon[box2no][i],  sizeof(PokemonIconDraw) );
  }

}




void BoxSimpleAccesser::TumeruBattleBoxSimpleData(int box)
{
  pml::pokepara::CoreParam* cp =mpTempPokemonParam;

  for(int j=0;j<pml::PokeParty::MAX_MEMBERS;j++){
    App::Box::PokemonIconDraw temp;
    gfl2::std::MemClear(&temp,sizeof(App::Box::PokemonIconDraw));
    
    const u16 boxno = mpAppParam->pBoxData->GetTeamPokePos(box,j);
 //   pml::pokepara::CoreParam* cp =mpTempPokemonParam;

    _makeBattleSimpleData(boxno, box,  &temp, cp );

    _teamPokeIconTumeru(cp, box, j, &temp);

  }
}



void BoxSimpleAccesser::TumeruResortBoxSimpleData(int type, int trayno)
{
  pml::pokepara::CoreParam* cp =mpTempPokemonParam;

  for(int j=0;j<pml::PokeParty::MAX_MEMBERS;j++){
    App::Box::PokemonIconDraw temp;
    gfl2::std::MemClear(&temp,sizeof(App::Box::PokemonIconDraw));
    GetResortPokemon(type,  trayno * pml::PokeParty::MAX_MEMBERS + j, cp);

    PokeTool::GetSimpleParam( &temp.simp , cp );
    defSetupPokemon(&temp, cp, -1, -1);
    _teamPokeIconTumeru(cp, trayno, j, &temp);

  }
}



void BoxSimpleAccesser::_teamPokeIconTumeru(pml::pokepara::CoreParam* cp,int box,int j, App::Box::PokemonIconDraw* ptemp)
{
  
  int icomp = gfl2::std::MemComp(ptemp,&mTeamPokeIcon[box][j],sizeof(App::Box::PokemonIconDraw));
  if( icomp==0 ){
  }
  else if(mTeamPokeIcon[box][j].nowRedraw == 2){ //すでに再描画予約
    gfl2::std::MemCopy( ptemp, &mTeamPokeIcon[box][j], sizeof(PokemonIconDraw) );
    mTeamPokeIcon[box][j].nowRedraw = 2;
  }
  else if(ptemp->simp.monsNo != mTeamPokeIcon[box][j].simp.monsNo){
    gfl2::std::MemCopy( ptemp, &mTeamPokeIcon[box][j], sizeof(PokemonIconDraw) );
    mTeamPokeIcon[box][j].nowRedraw = 2;
  }
  else if(ptemp->simp.formNo !=  mTeamPokeIcon[box][j].simp.formNo){
    gfl2::std::MemCopy( ptemp, &mTeamPokeIcon[box][j], sizeof(PokemonIconDraw) );
    mTeamPokeIcon[box][j].nowRedraw = 2;
  }
  else{
    gfl2::std::MemCopy( ptemp, &mTeamPokeIcon[box][j], sizeof(PokemonIconDraw) );
    mTeamPokeIcon[box][j].nowRedraw = 1;
  }
}




///以下パーティーのラッパークラス
int BoxSimpleAccesser::Temochi_GetMemberCount(void)
{
  int i,x=0;
  
  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    if(!mpPartyBuff[i]->IsNull()){
      x++;
    }
  }
  return x;
}


void BoxSimpleAccesser::Temochi_RemoveMember(int x)
{
  mpPartyBuff[x]->Clear();
}


void BoxSimpleAccesser::Temochi_ReplaceMember(int x, const pml::pokepara::PokemonParam* pp)
{
  mpPartyBuff[x]->CopyFrom(*pp);
}

void BoxSimpleAccesser::Temochi_AddMember(const pml::pokepara::PokemonParam* pp)
{
  int i;

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    if(!mpPartyBuff[i]->IsNull()){
      mpPartyBuff[i]->CopyFrom(*pp);
      break;
    }
  }
}

pml::pokepara::PokemonParam * BoxSimpleAccesser::Temochi_GetMemberPointer(int x)
{
  return mpPartyBuff[x];
}

const pml::pokepara::PokemonParam * BoxSimpleAccesser::Temochi_GetMemberPointerConst(int x)
{
  return mpPartyBuff[x];
}


void BoxSimpleAccesser::_alignMember(u8* pBuff,int* rangebuffer)
{
  int i;

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS-1 ; i++){
    if(mpPartyBuff[i]->IsNull()){
      pBuff[i]=pBuff[i+1];
      pBuff[i+1]=0xff;
      mpPartyBuff[i]->CopyFrom(*mpPartyBuff[i+1]);
      mpPartyBuff[i+1]->Clear();
      if(rangebuffer)
      {
        rangebuffer[i]=rangebuffer[i+1];
        rangebuffer[i+1]=-1;
      }
    }
  }
}


bool BoxSimpleAccesser::Temochi_AlignMemberRange(int* rangebuffer)
{
  int i;
	bool retflg = false;
  u8 partyNo[pml::PokeParty::MAX_MEMBERS]={0xff,0xff,0xff,0xff,0xff,0xff};
  int copyrangebuffer[pml::PokeParty::MAX_MEMBERS];

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    if(!mpPartyBuff[i]->IsNull()){
      partyNo[i]=i;
    }
    copyrangebuffer[i]=rangebuffer[i];
  }

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS-1 ; i++){
    _alignMember(partyNo, copyrangebuffer);
  }

  {
    for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
      OHNO_PRINT("Temochi_AlignMemberRange- %d  %d %d\n", partyNo[i], i,copyrangebuffer[i]);
      
      if(i!=partyNo[i] && partyNo[i]!=0xff){
        if( copyrangebuffer[i]!=-1 ){ //いまもってきたアイコン移動
          ChangePokemonLandingAnimStart(0,partyNo[i]+30, 0,i+30,copyrangebuffer[i]);
          OHNO_PRINT("移動L %d %d\n", i, copyrangebuffer[i]);
          retflg = true;
        }
        else{
          OHNO_PRINT("移動2 %d %d\n", i);
          SetMoveTypePainTemotiTumeru( 0,partyNo[i]+30, 0,i+30);
        }
      }
    }
  }


  
	return retflg;
}



















bool BoxSimpleAccesser::Temochi_AlignMember(int maincursorno , int* ppos ,bool bmoveanim)
{
  int i;
	bool retflg = false;
  u8 partyNo[pml::PokeParty::MAX_MEMBERS]={0xff,0xff,0xff,0xff,0xff,0xff};

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    if(!mpPartyBuff[i]->IsNull()){
      partyNo[i]=i;
    }
  }

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS-1 ; i++){
    _alignMember(partyNo);
  }

  if(bmoveanim){
    for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
      OHNO_PRINT(" %d  %d \n", partyNo[i], i);
      
      if(i!=partyNo[i] && partyNo[i]!=0xff){
        retflg=true;
        if( maincursorno == (Savedata::BoxPokemon::TRAY_POKE_MAX + partyNo[i])){ //いまもってきたアイコン移動
          if(ppos){
            *ppos = i;  //移動先格納
          }
        }
        else{
          SetMoveTypePainTemotiTumeru( 0,partyNo[i]+30, 0,i+30);
//          SetMoveTypePain2( 0,partyNo[i]+30, 0,i+30);
        }
      }
    }
  }
	return retflg;
}

void BoxSimpleAccesser::Temochi_ExchangePosition(int x,int y)
{
  pml::pokepara::PokemonParam *px = mpPartyBuff[x];
  mpPartyBuff[x] = mpPartyBuff[y];
  mpPartyBuff[y] = px;
}


void BoxSimpleAccesser::Temochi_PartyToThis(const pml::PokeParty* pParty)
{
  int i;
  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    const pml::pokepara::PokemonParam * pp = pParty->GetMemberPointerConst(i);
    if(!pp->IsNull()){
      Temochi_ReplaceMember(i,pp);
#if PM_DEBUG
      mPokemonNoBackup[i] = pp->GetMonsNo();
#endif
    }
  }
}


void BoxSimpleAccesser::Temochi_ThisToParty(pml::PokeParty* pParty)
{
  int i;
  pParty->Clear();
  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    if(!mpPartyBuff[i]->IsNull()){
      pParty->AddMember(*mpPartyBuff[i]);
    }
  }
}


void BoxSimpleAccesser::Temochi_EndFunc(pml::PokeParty* pParty)
{
  int i;

  switch( mpAppParam->type ){
  case BOX_CALL_TRADE:    //P2P交換はＢＯＸの裏でセーブデータを書き換えているので書き戻さない
    break;
  default:
#if PM_DEBUG   //pokepartyをさわった人がいないかどうかの検査
    for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
      const pml::pokepara::PokemonParam * pp = pParty->GetMemberPointerConst(i);
      if(!pp->IsNull()){
        GFL_ASSERT( mPokemonNoBackup[i] == pp->GetMonsNo());
      }
    }
#endif
    Temochi_ThisToParty(pParty);
    break;
  }
}




//--------------------------------------------------------------------------------------------
/**
 * @brief		戦えるポケモンがいるか その数を帰す
 *
 * @param		検査しないポケモンは１
 * @return  数
 */
//--------------------------------------------------------------------------------------------
int BoxSimpleAccesser::Temochi_CheckBattlePokemon( u8* emptybuff )
{
  int i;
  int retnum=0;

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    const pml::pokepara::PokemonParam* pp = Temochi_GetMemberPointerConst(i);
		if(!emptybuff[i]){
	    if(!pp->IsNull()){
		    if( pp->GetHp() != 0 && pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false ){
          retnum++;

        }
      }
    }
  }
	return retnum;
}

//チームからはずす
void BoxSimpleAccesser::RemoveTeam(u32 battletray, u32 battepos)
{
  //置かれているものが無いかどうか検査し
  u16 boxpos = mpAppParam->pBoxData->GetTeamPokePos(battletray, battepos);

  if(boxpos!=Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE){
    mpAppParam->pBoxData->SetTeamPokePos( battletray, battepos,
                                          Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE );
 //   mpBoxAppSetup->GetBoxSimpleAccesser()->SetTeamIcon(boxpos>>8, boxpos&0xff ,battletray , false);

    SetupBoxPokemon(boxpos>>8, boxpos&0xff);  //SystemNMCat[367]
    SetPokeIconDrawState(boxpos>>8, boxpos&0xff);

  }
}


int BoxSimpleAccesser::SetTeamPokePos( u32 battletray, u32 battepos, u32 selecttray, u32 selectpos )
{
  int i=0;

  RemoveTeam(battletray, battepos);//置いてあるものがあれば登録解除
  
  if(selectpos < Savedata::BoxPokemon::TRAY_POKE_MAX){   //BOX直接
    i = mpAppParam->pBoxData->SetTeamPokePos(  battletray,
                                           battepos,  (selecttray<<8) + selectpos  );
  }
  else{  //バトルトレイ内の移動

    u16 pos =  mpAppParam->pBoxData->GetTeamPokePos(selecttray, selectpos-Savedata::BoxPokemon::TRAY_POKE_MAX);
    i = mpAppParam->pBoxData->SetTeamPokePos(  battletray, battepos,  pos );
  }
  return i;
}



//位置の入れ替えを行う バトルトレイ、バトルトレイ
void BoxSimpleAccesser::ChangeTeamPokePos( u32 battletray, u32 battepos, u32 selecttray, u32 selectpos )
{
  u16 boxtrayandpos = mpAppParam->pBoxData->GetTeamPokePos(  battletray, battepos - Savedata::BoxPokemon::TRAY_POKE_MAX);

  GFL_ASSERT(selectpos >= Savedata::BoxPokemon::TRAY_POKE_MAX);

  u16 pos2 = mpAppParam->pBoxData->GetTeamPokePos(  selecttray,  selectpos - Savedata::BoxPokemon::TRAY_POKE_MAX  );


  if(battletray==selecttray){//同じトレイは入れ替えてＯＫ
    mpAppParam->pBoxData->SetTeamPokePos( selecttray, selectpos - Savedata::BoxPokemon::TRAY_POKE_MAX,  boxtrayandpos );
    mpAppParam->pBoxData->SetTeamPokePos( battletray, battepos - Savedata::BoxPokemon::TRAY_POKE_MAX,  pos2 );
  }
  else{//別トレイはコピーがありえる
    if(boxtrayandpos!=Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE){
      mpAppParam->pBoxData->SetTeamPokePos( selecttray, selectpos - Savedata::BoxPokemon::TRAY_POKE_MAX,  boxtrayandpos );
    }
    if(pos2!=Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE  ){
      mpAppParam->pBoxData->SetTeamPokePos( battletray, battepos - Savedata::BoxPokemon::TRAY_POKE_MAX,  pos2 );
    }
  }
}



void BoxSimpleAccesser::ChangePokemonRangeAnimStart(void)
{
  int i;
  
  for(i = 0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){
    BoxSimpleAccesser::StockChangePos* pStock = mpBoxAppSetup->GetBoxSimpleAccesser()->GetChangePokemonReserveData(i);
    if(pStock==NULL){
      break;
    }
    ChangePokemonLandingAnimStart(pStock->selectpage, pStock->selectpos, pStock->nowpage, pStock->nowpos, pStock->rangepos );
  }
}


void BoxSimpleAccesser::ChangePokemonLandingAnimStart(int selectpage,int selectpos, int nowpage,int nowpos, int rangepos)
{
  SetLandingTypePain( mpBoxDrawPartsListener->GetCarryIconParts( rangepos ),
                      mpBoxDrawPartsListener->GetIconParts(nowpos),nowpage, nowpos,  selectpage, selectpos );
}




/// パターン１　　　入れ替え移動
/// トレイの移動
/// キャンセルの移動


void BoxSimpleAccesser::SetMoveTypePain2(int selectpage,int selectpos, int nowpage,int nowpos)
{
  SetMoveTypePain(mpBoxDrawPartsListener->GetIconParts(selectpos),mpBoxDrawPartsListener->GetIconParts(nowpos),
                  selectpage, selectpos, nowpage, nowpos);
}

//手持ちの詰める時に移動するアニメ
void BoxSimpleAccesser::SetMoveTypePainTemotiTumeru(int selectpage,int selectpos, int nowpage,int nowpos)
{
  SetMoveTypePain(mpBoxDrawPartsListener->GetTemotiIconParts(selectpos),mpBoxDrawPartsListener->GetTemotiIconParts(nowpos),
                  selectpage, selectpos, nowpage, nowpos);

  mpBoxDrawPartsListener->TemotiIconPartsReady(selectpage, selectpos);

}



void BoxSimpleAccesser::SetMoveTypePain(gfl2::lyt::LytParts* pMovePain, gfl2::lyt::LytPane* pGoalPain,int selectpage,int selectpos, int nowpage,int nowpos,gfl2::lyt::LytParts* pChangePain)
{

  for(int i = 0;i < ICONANIM_BUFF_NUM; i++){

    if(mIconAnimBuff[ i ].pMovePain != NULL){
      continue;
    }
    OHNO_PRINT("移動アニメ %d %d => %d %d\n",selectpage,selectpos,nowpage,nowpos);
    
    mIconAnimBuff[ i ].bLanding=false;
    mIconAnimBuff[ i ].bMoveStart=true;
    mIconAnimBuff[ i ].selectpage = selectpage;
    mIconAnimBuff[ i ].selectpos = selectpos;
    mIconAnimBuff[ i ].nowpage = nowpage;
    mIconAnimBuff[ i ].nowpos = nowpos;
		mIconAnimBuff[ i ].pMovePain = pMovePain;
		mIconAnimBuff[ i ].pGoalPain = pGoalPain;
		mIconAnimBuff[ i ].pChangePain = pChangePain;
    mIconAnimBuff[ i ].nowframe = ICONANIM_FRAME;
    mIconAnimBuff[ i ].bMoveReady=false;
    break;
  }

}

void BoxSimpleAccesser::SetLandingTypePain(gfl2::lyt::LytParts* pLandingPain,gfl2::lyt::LytPane* pGoalPain,int selectpage,int selectpos, int nowpage,int nowpos)
{

  for(int i = 0;i < ICONANIM_BUFF_NUM; i++){

    if(mIconAnimBuff[ i ].pMovePain != NULL){
      continue;
    }
    OHNO_PRINT("着地アニメ %d %d => %d %d\n",selectpage,selectpos,nowpage,nowpos);

    mIconAnimBuff[ i ].bLanding=true;
    mIconAnimBuff[ i ].bMoveStart=true;
    mIconAnimBuff[ i ].bMoveReady=false;
    mIconAnimBuff[ i ].selectpage = selectpage;
    mIconAnimBuff[ i ].selectpos = selectpos;
    mIconAnimBuff[ i ].nowpage = nowpage;
    mIconAnimBuff[ i ].nowpos = nowpos;
    mIconAnimBuff[ i ].pMovePain = pLandingPain;
		mIconAnimBuff[ i ].pGoalPain = pGoalPain;
    mIconAnimBuff[ i ].nowframe = ICONANIM_FRAME;
    break;
  }


}



void BoxSimpleAccesser::AnimMove(void)
{

  for(int i = 0;i < ICONANIM_BUFF_NUM; i++){

    if(mIconAnimBuff[ i ].pMovePain == NULL){
     continue;
    }
    if(!mIconAnimBuff[ i ].bMoveStart){
      continue;
     }
    if(!mIconAnimBuff[ i ].bMoveReady){
      mIconAnimBuff[ i ].vecMove =  mIconAnimBuff[ i ].pMovePain->GetTranslate();
      mIconAnimBuff[ i ].vecGoal =  mIconAnimBuff[ i ].pGoalPain->GetTranslate();
      mIconAnimBuff[ i ].globalMtx = mIconAnimBuff[ i ].pMovePain->GetGlobalMtx() - mIconAnimBuff[ i ].pGoalPain->GetGlobalMtx();
      mIconAnimBuff[ i ].bMoveReady = true;
      continue;
    }

    
    gfl2::math::VEC3 vec3(0.0f,0.0f,0.0f);

    vec3.x = mIconAnimBuff[ i ].globalMtx._03;
    vec3.y = mIconAnimBuff[ i ].globalMtx._13;

//    OHNO_PRINT("7 %f %f %f\n",	vec3.x,	vec3.y, vec3.z);
		mIconAnimBuff[ i ].nowframe -= 1.0f;

    float frame = mIconAnimBuff[ i ].nowframe;
    if(frame < 0.0f){
      frame = 0.0f;
    }
    
		vec3 = (vec3 / ICONANIM_FRAME) * (ICONANIM_FRAME - frame);

//    OHNO_PRINT("4 %f %f %f\n",	vec3.x,	vec3.y, vec3.z);

    
    vec3 = mIconAnimBuff[ i ].vecMove - vec3;

//    OHNO_PRINT("5 %f %f %f\n",	vec3.x,	vec3.y, vec3.z);


    mIconAnimBuff[ i ].pMovePain->SetTranslate(vec3);

		if(mIconAnimBuff[ i ].nowframe < 0.0f){  //終了
			//終了処理
     // if(!mIconAnimBuff[ i ].bLanding){
//        RefreshPokemonIconDrawData();
        ChangePokemon(mIconAnimBuff[ i ].selectpage, mIconAnimBuff[ i ].selectpos, mIconAnimBuff[ i ].nowpage, mIconAnimBuff[ i ].nowpos);
      //}
//      mIconAnimBuff[ i ].pMovePain->SetTranslate(mIconAnimBuff[ i ].vecMove); //元の位置に
//			mIconAnimBuff[ i ].pMovePain=NULL; //終了
      mIconAnimBuff[ i ].bMoveStart=false;  //移動の終わり
		}

  }


}


bool BoxSimpleAccesser::IsAnimMoveEnd(void)
{

  for(int i = 0;i < ICONANIM_BUFF_NUM; i++){
    if(mIconAnimBuff[ i ].bMoveStart){
      return false;
    }
  }
  return true;
}


void BoxSimpleAccesser::AnimEnd(void)
{

  for(int i = 0;i < ICONANIM_BUFF_NUM; i++){
    if(mIconAnimBuff[ i ].pChangePain){
      mIconAnimBuff[ i ].pChangePain->SetVisible(true);
      mIconAnimBuff[ i ].pMovePain->SetVisible(false);
      mIconAnimBuff[ i ].pChangePain=NULL;
    }
    if(mIconAnimBuff[ i ].pMovePain){
      mIconAnimBuff[ i ].pMovePain->SetTranslate(mIconAnimBuff[ i ].vecMove); //元の位置に
      mIconAnimBuff[ i ].pMovePain=NULL;
    }
  }
}


void BoxSimpleAccesser::AnimClear(void)
{

  for(int i = 0;i < ICONANIM_BUFF_NUM; i++){
    if(mIconAnimBuff[ i ].pChangePain){
      mIconAnimBuff[ i ].pChangePain=NULL;
    }
    if(mIconAnimBuff[ i ].pMovePain){
      mIconAnimBuff[ i ].pMovePain=NULL;
    }
    mIconAnimBuff[ i ].bMoveStart=false;  //移動のリセット
  }
}




void BoxSimpleAccesser::GetResortPokemon(int type, int index, pml::pokepara::CoreParam * cp)
{
  if(type == BOX_CALL_RESORT_HOTSPA){
    mpAppParam->pResortSave->GetHotSpaPokemon( index , cp);
  }
  else{
    mpAppParam->pResortSave->GetGimPokemon( index , cp);
  }
}



void BoxSimpleAccesser::PackResortPokemon(int type, int pos)
{

  gfl2::heap::HeapBase * heap = mpAppParam->mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::CoreParam* cp1 = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  pml::pokepara::CoreParam* cp2 = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );


  for(int x=0;x<5;x++){
    for(int i = pos ;i < pos + pml::PokeParty::MAX_MEMBERS - 1 ; i++){
      GetResortPokemon(type,  i , cp1);
      if(cp1->IsNull()){
        GetResortPokemon(type, i + 1 , cp2);
        if(!cp2->IsNull()){
          _setResortPokemon(type,  i , cp2);
          _setResortPokemon(type,  i + 1 , cp1);
        }
      }
    }
  }

  GFL_SAFE_DELETE( cp1);
  GFL_SAFE_DELETE( cp2);

  
}


void BoxSimpleAccesser::PacksResortPokemon(void)
{
  PackResortPokemon(BOX_CALL_RESORT_HOTSPA, 0);
  PackResortPokemon(BOX_CALL_RESORT_HOTSPA, 6);
  PackResortPokemon(BOX_CALL_RESORT_HOTSPA,12);
  PackResortPokemon(BOX_CALL_RESORT_GIM, 0);
  PackResortPokemon(BOX_CALL_RESORT_GIM, 6);
  PackResortPokemon(BOX_CALL_RESORT_GIM,12);

  if(mpAppParam->type == BOX_CALL_RESORT_HOTSPA){
    TumeruResortBoxSimpleData(BOX_CALL_RESORT_HOTSPA,0);
    TumeruResortBoxSimpleData(BOX_CALL_RESORT_HOTSPA,1);
    TumeruResortBoxSimpleData(BOX_CALL_RESORT_HOTSPA,2);
  }
  if(mpAppParam->type == BOX_CALL_RESORT_GIM){
    TumeruResortBoxSimpleData(BOX_CALL_RESORT_GIM,0);
    TumeruResortBoxSimpleData(BOX_CALL_RESORT_GIM,1);
    TumeruResortBoxSimpleData(BOX_CALL_RESORT_GIM,2);
  }
  SetupTemochi();
}


void BoxSimpleAccesser::SetResortPokemon(int type, int index, pml::pokepara::CoreParam * cp)
{
  _setResortPokemon(type, index, cp);
}

void BoxSimpleAccesser::_setResortPokemon(int type, int index, pml::pokepara::CoreParam * cp)
{
  if(type == BOX_CALL_RESORT_HOTSPA){
    mpAppParam->pResortSave->SetHotSpaPokemon( index , cp);
  }
  else{
    mpAppParam->pResortSave->SetGimPokemon( index , cp);
  }
}



void BoxSimpleAccesser::_alignMember_Temp(u8* pBuff,int* rangebuffer)
{
  int i;

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS-1 ; i++){
    if(mpPartyBuffTemp[i]->IsNull()){
      pBuff[i]=pBuff[i+1];
      pBuff[i+1]=0xff;
      mpPartyBuffTemp[i]->CopyFrom(*mpPartyBuffTemp[i+1]);
      mpPartyBuffTemp[i+1]->Clear();
      if(rangebuffer){
        rangebuffer[i]=rangebuffer[i+1];
        rangebuffer[i+1]=-1;
      }
    }
  }
}



// リゾートの詰める移動を行う
bool BoxSimpleAccesser::Resort_AlignMemberRange(int tray,int* rangebuffer)
{
  int i;
	bool retflg = false;
  gfl2::heap::HeapBase * heap = mpAppParam->mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::CoreParam* cp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  u8 partyNo[ pml::PokeParty::MAX_MEMBERS ]={ 0xff,0xff,0xff,0xff,0xff,0xff };
  int copyrangebuffer[pml::PokeParty::MAX_MEMBERS];

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    GetResortPokemon( mpAppParam->type, tray * 6 + i, cp);
    if(!cp->IsNull()){
      partyNo[i]=i;
    }
    mpPartyBuffTemp[i]->CopyFrom(*cp);
    copyrangebuffer[i]=rangebuffer[i];
  }

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS-1 ; i++){
    _alignMember_Temp(  partyNo,copyrangebuffer );
  }

  {
    for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
      OHNO_PRINT("Resort_AlignMemberRange- %d  %d %d\n", partyNo[i], i,copyrangebuffer[i]);
      
      if(i!=partyNo[i] && partyNo[i]!=0xff){
        if( copyrangebuffer[i]!=-1 ){ //いまもってきたアイコン移動
          ChangePokemonLandingAnimStart(0,partyNo[i]+30, 0,i+30,copyrangebuffer[i]);
          OHNO_PRINT("移動L %d %d\n", i, copyrangebuffer[i]);
          retflg = true;
        }
        else{
          OHNO_PRINT("移動2 %d %d\n", i);
          SetMoveTypePainTemotiTumeru( tray,partyNo[i]+30, tray,i+30);
        }
      }
    }
	}

  GFL_DELETE( cp );

  return retflg;
}





// リゾートの詰める移動を行う
bool BoxSimpleAccesser::Resort_AlignMember(int tray,int maincursorno,int* ppos,bool bmoveanim)
{
  int i;
	bool retflg=false;
  gfl2::heap::HeapBase * heap = mpAppParam->mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::CoreParam* cp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  u8 partyNo[ pml::PokeParty::MAX_MEMBERS ]={ 0xff,0xff,0xff,0xff,0xff,0xff };

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    GetResortPokemon( mpAppParam->type, tray * 6 + i, cp);
    if(!cp->IsNull()){
      partyNo[i]=i;
    }
    mpPartyBuffTemp[i]->CopyFrom(*cp);
  }

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS-1 ; i++){
    _alignMember_Temp(  partyNo );
  }

  if(bmoveanim){
    for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
      OHNO_PRINT(" %d  %d \n", partyNo[i], i);
      
      if(i!=partyNo[i] && partyNo[i]!=0xff){
        retflg=true;
        if( maincursorno == (Savedata::BoxPokemon::TRAY_POKE_MAX + partyNo[i])){ //いまもってきたアイコン移動
          if(ppos){
            *ppos = i;  //移動先格納
          }
        }
        else{
          SetMoveTypePainTemotiTumeru( tray,partyNo[i]+30, tray,i+30);
//          SetMoveTypePain2( tray,partyNo[i]+30, tray,i+30);
        }
      }
    }
  }


  
  GFL_DELETE( cp );
	return retflg;
}








// バトルボックスの詰める移動を行う
void BoxSimpleAccesser::Battlebox_AlignMember(int tray)
{
  int i;
  gfl2::heap::HeapBase * heap = mpAppParam->mpAppHeap->GetDeviceHeap()->GetLowerHandle();
  pml::pokepara::CoreParam* cp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
  u8 partyNo[ pml::PokeParty::MAX_MEMBERS ]={ 0xff,0xff,0xff,0xff,0xff,0xff };

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
    GetPokemon(tray , i+30, cp);
    if(!cp->IsNull()){
      partyNo[i]=i;
    }
    mpPartyBuffTemp[i]->CopyFrom(*cp);
  }

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS - 1 ; i++){
    _alignMember_Temp( partyNo );
  }

  for(i = 0;i < pml::PokeParty::MAX_MEMBERS ; i++){
//    OHNO_PRINT(" %d  %d \n", partyNo[i], i);

    if(i!=partyNo[i] && partyNo[i]!=0xff){
      SetMoveTypePain2( 0,partyNo[i]+30, 0,i+30);
	  }
	}

  GFL_DELETE( cp );
}



bool BoxSimpleAccesser::IsBoxSearchMode(void)
{
  if(mpBoxSearch->endMode  == App::BoxSearch::END_MODE_SEARCH){
    return true;
  }
  return false;
}


bool BoxSimpleAccesser::IsPokemonLock(int tray ,int pos)
{
  bool block = false;
  
  if(pos<Savedata::BoxPokemon::TRAY_POKE_MAX){
    if(mpAppParam->pBoxData->IsMeLock(tray  ,pos)){
      block = true;
    }
  }
  else{
    if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時は
      const u16 boxno2 = mpAppParam->pBoxData->GetTeamPokePos(tray, pos-Savedata::BoxPokemon::TRAY_POKE_MAX);
      if(boxno2 != (u16)Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE){
        if(mpAppParam->pBoxData->IsMeLock(  boxno2 >> 8 , boxno2&0xff)){
          block = true;
        }
      }
    }
  }
  return block;
}


bool BoxSimpleAccesser::IsPokemonTeam(int tray ,int pos)
{
  bool bteam = false;
  
  if(pos<Savedata::BoxPokemon::TRAY_POKE_MAX){
    if(mpAppParam->pBoxData->IsTeamInMe(tray  ,pos)!=-1){
      bteam = true;
    }
  }
  else{
    if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時は
      const u16 boxno2 = mpAppParam->pBoxData->GetTeamPokePos(tray, pos-Savedata::BoxPokemon::TRAY_POKE_MAX);
      if(boxno2 != (u16)Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE){
        if(mpAppParam->pBoxData->IsTeamInMe(  boxno2 >> 8 , boxno2&0xff)!=-1){
          bteam = true;
        }
      }
    }
  }
  return bteam;
}



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )
