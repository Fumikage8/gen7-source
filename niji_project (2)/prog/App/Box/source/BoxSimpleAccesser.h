//===================================================================
/**
 *	@file	    BoxSimpleAccesser.h
 *	@brief    ポケモンアイコン表示の統合クラス
 *	@author	  k.ohno
 *	@date		  2015.11.14
 *
 */
//===================================================================
#ifndef __BOX_SIMPLE_ACCESSER_H__
#define __BOX_SIMPLE_ACCESSER_H__
#pragma once

#include "GameSys/include/GameManager.h"

#include "AppLib/include/Util/app_util_heap.h"

#include "App/Box/include/BoxAppParam.h"
#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"
#include "App/BoxSearch/include/BoxSearchAppParam.h"
#include "BoxDrawPartsListener.h"

#include <arc_index/boxLyt.gaix>

#include "pml/include/pml_PokePara.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxEventListener;
class BoxAppSetup;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------

//#define ICONANIM_FRAME (40.0f)


class BoxSimpleAccesser
{
  GFL_FORBID_COPY_AND_ASSIGN( BoxSimpleAccesser ); // コピーを禁止

public:
  static const int POKEICON_PASSIVE_NUM = 128;   //ポケアイコンを半透明にする時の透明度
  static const int ICONANIM_BUFF_NUM = 36;   //
  //    static const float ICONANIM_FRAME = 4.0f;

  struct PokemonIconDrawBackUp{
    int page;   //どの場所のものか
    int pos;
    int x;    //持ち上げた時の位置は
    int y;
    int rangepos;//範囲選択の時の持ち上げた場所
    App::Box::PokemonIconDraw PokeIconDraw;
  };

  struct IconAnim{
    int selectpage;
    int selectpos;
    int nowpage;
    int nowpos;
    gfl2::lyt::LytParts* pMovePain;  //移動するペイン
    gfl2::lyt::LytPane* pGoalPain;  //移動先のペイン
    gfl2::lyt::LytParts* pChangePain;  //最後に交換するペイン
    gfl2::math::VEC3 vecMove;
    gfl2::math::VEC3 vecGoal;
    gfl2::math::MTX34 globalMtx;
    float nowframe;
    bool bMoveStart;
    bool bLanding;
    bool bMoveReady;
    bool dmyx;
  };

  struct StockChangePos{
    int selectpage;
    int selectpos;
    int nowpage;
    int nowpos;
		int rangepos;
  };


public:
  BoxSimpleAccesser(void);
  ~BoxSimpleAccesser();

  PokeTool::SimpleParam* GetSimpleData(int page,int cursor);

  void ChangePokemon(int selectpage, int selectpos, int nowpage, int nowpos);
  void ChangePokemonReserve(int selectpage, int selectpos, int nowpage, int nowpos, int rangepos);
  void ChangePokemonUseStock(void);
  StockChangePos* GetChangePokemonReserveData(int index);

	bool IsBoxSearchMode(void);

  
  void Init(App::Box::APP_PARAM* pAppParam,App::Box::BoxAppSetup* pBoxAppSetup,App::BoxSearch::APP_PARAM* pBoxSearch);
  void End(void);
  void SetupBoxPokemon(int page,int i);

  void AnimMove(void);

  void SetupBox(void);//ボックスのベースデータを更新
  void SetupTemochi(void);
  void SetupBattleTeam(void);

  int GetPokeIconActive(int page,int pos);
  void SetPokeIconActive(int page,int pos,bool bact);

  bool IsTeamIcon(App::Box::PokemonIconDraw* pPkd);

  void SetVisible(int page,int cursor, bool visible);
  void SetTeamIcon(int page,int cursor, int team, bool visible);

  void SetCarryFlg(int page,int cursor, bool carryup);
  
  App::Box::PokemonIconDraw* GetPokemonIconDrawData(int disp ,int pos);
  App::Box::PokemonIconDraw* GetPokemonIconDrawData(int disp,int battledisp ,int pos);

  //ポケモンデータをボックス位置、カーソル位置から得る
  void GetPokemon(int boxno,int cursorno, pml::pokepara::CoreParam* cp);
  void GetPokemonPokemonParam(int boxno,int cursorno, pml::pokepara::PokemonParam* cp);

  //範囲内にポケモンがいるかを検査 数を帰す
  int RangePokemonNum(int startx, int starty, int sizex, int sizey,int page, bool skipSelect);


  ///描画をさせる stateのみ
  void SetPokeIconDrawState(int page,int cursor);
  ///描画をさせる 全部
  void SetPokeIconDrawAll(int page,int cursor);
  ///スクロールさせたものに置き換える 全部
  void SetPokeIconChangeAll(int page,int cursor);
  //アイコン情報をバックアップ
  void PushPokemonIconDrawData(int page,int cursor, int x,int y);
  //アイコン情報をリフレッシュ=>再描画
  void RefreshPokemonIconDrawData(void);
  //アイコン情報をリセット
  void ResetPokemonIconDrawData(void);
  //ポケモンデータをアイコン情報から得る
  void GetCarryPokemon(int index, pml::pokepara::CoreParam* cp);
  //運んでいるポケモンをパーティーに入れる
  pml::PokeParty* GetCarryParty(void);
  //検索したポケモンがいるかどうか
  int GetSearchPokemonNum(int page);
  //ボックスのみ かずを数える
  int GetPokemonCount(int page);
  //サーチモードかどうか
  bool IsSearchMode(void);
  //白くする
  void SetPokeIconWhite(int page,int cursor, bool bWhite);
  //トレイの入れ替え
  void ChangeTray(int box1no, int box2no);
  //バトルボックスの整理
  void TumeruBattleBoxSimpleData(int box);
  //リゾートの整理
  void TumeruResortBoxSimpleData(int type, int trayno);

  //データ再作成
  void RefreashPokemon(int selectpage, int selectpos);
  //バトルトレイへの登録
  int SetTeamPokePos( u32 battletray, u32 battepos, u32 selecttray, u32 selectpos );
  ///バトルチームからはずす
  void RemoveTeam(u32 battletray, u32 battepos);
  //バトルトレイの取得
  void GetTeamPokePos( u32 battletray, u32 battepos, u32 selecttray, u32 selectpos );
  //位置の入れ替えを行う
  void ChangeTeamPokePos( u32 battletray, u32 battepos, u32 selecttray, u32 selectpos );

  void ChangePokemonLandingAnimStart(int selectpage,int selectpos, int nowpage,int nowpos,int rangepos);
	void SetMoveTypePainTemotiTumeru(int selectpage,int selectpos, int nowpage,int nowpos);

  void ChangePokemonRangeAnimStart(void);

  
  //アイコン移動アニメ設定
  void SetMoveTypePain2(int selectpage,int selectpos, int nowpage,int nowpos);
  void SetMoveTypePain(gfl2::lyt::LytParts* pMovePain, gfl2::lyt::LytPane* pGoalPain,int selectpage,int selectpos, int nowpage,int nowpos,gfl2::lyt::LytParts* pChangePain=NULL);
  void SetLandingTypePain(gfl2::lyt::LytParts* pLandingPain, gfl2::lyt::LytPane* pGoalPain,int selectpage,int selectpos, int nowpage,int nowpos);
  bool IsAnimMoveEnd(void);
  void AnimEnd(void);
  void AnimClear(void);

  void SetBoxDrawPartsListener(BoxDrawPartsListener* pBoxDrawPartsListener){ mpBoxDrawPartsListener = pBoxDrawPartsListener;}

private:
  void _teamPokeIconTumeru(pml::pokepara::CoreParam* cp,int box,int j, App::Box::PokemonIconDraw* ptemp);
  bool IsWazaHave(const pml::pokepara::CoreParam* cp,int own_wazano);
  bool IsTypeHave(const pml::pokepara::CoreParam* cp,int type);
  void defSetupPokemon(PokemonIconDraw* pPkd,const pml::pokepara::CoreParam* cp,int tray,int pos);
  void makeManagerData(gfl2::heap::HeapBase * heap,GameSys::GameManager* game_manager);
  void makeBoxSimpleData(gfl2::heap::HeapBase * heap,pml::pokepara::CoreParam* cp);
  void makeTemotiSimpleData(int i);
  void makeBattleBoxSimpleData(void);
  void _makeBattleSimpleData(const u16 boxno, int x, App::Box::PokemonIconDraw* ptemp,pml::pokepara::CoreParam* cp);
  void makeBattleSimpleData(int x, int j);
  void makeResortGimSimpleData(void);
  void makeResortGimSimpleData(int page,int pos);
  void makeResortHotspaSimpleData(void);
  void makeResortHotspaSimpleData(int page,int pos);

  void _setMoveTypePain(gfl2::lyt::LytParts* pMovePain, gfl2::lyt::LytParts* pGoalPain,int selectpage,int selectpos, int nowpage,int nowpos,int type);

public:
  //手持ちポケモンアクセサ関数
  int Temochi_GetMemberCount(void);
  void Temochi_RemoveMember(int x);
  void Temochi_ReplaceMember(int x, const pml::pokepara::PokemonParam* pp);
  void Temochi_AddMember(const pml::pokepara::PokemonParam* pp);
  pml::pokepara::PokemonParam * Temochi_GetMemberPointer(int x);
  const pml::pokepara::PokemonParam* Temochi_GetMemberPointerConst(int x);
  bool Temochi_AlignMemberRange(int* rangebuffer);
  bool Temochi_AlignMember(int maincursorno , int* pos ,bool bmoveanim=true);
  void Temochi_ExchangePosition(int x,int y);

  void Temochi_PartyToThis(const pml::PokeParty* pParty);
  void Temochi_ThisToParty(pml::PokeParty* pParty);
  void Temochi_EndFunc(pml::PokeParty* pParty);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief		戦えるポケモンがいるか その数を帰す
   *
   * @param		検査しないポケモンは１
   * @return  数
   */
  //--------------------------------------------------------------------------------------------
  int Temochi_CheckBattlePokemon( u8* emptybuff );



  bool IsPokemonLock(int disp ,int pos);
  bool IsPokemonTeam(int disp ,int pos);

  

private:

  void PackResortPokemon(int type, int pos);
  void _setResortPokemon(int type, int index, pml::pokepara::CoreParam * cp);

public:
  void GetResortPokemon(int type, int index, pml::pokepara::CoreParam * cp);
  void SetResortPokemon(int type, int index, pml::pokepara::CoreParam * cp);
  void PacksResortPokemon(void);

  bool Resort_AlignMember(int tray,int maincursorno,int* pos,bool bmoveanim=true);
  bool Resort_AlignMemberRange(int tray,int* rangebuffer);
  void Battlebox_AlignMember(int tray);



private:
  void _alignMember(u8* pBuff,int* rangebuffer=NULL);
  void _alignMember_Temp(u8* pBuff,int* rangebuffer=NULL);


private:

  App::Box::APP_PARAM* mpAppParam;
  App::BoxSearch::APP_PARAM* mpBoxSearch;
  pml::pokepara::PokemonParam* mpTempPokemonParam;
  pml::pokepara::PokemonParam* mpTempPokemonParam2;
  App::Box::BoxAppSetup* mpBoxAppSetup;
  pml::PokeParty* mpPartyTemp;  // 交換検査に使用

  BoxDrawPartsListener* mpBoxDrawPartsListener;

  //ポケパーティーを扱う為のバッファ
  pml::pokepara::PokemonParam* mpPartyBuff[pml::PokeParty::MAX_MEMBERS];
  pml::pokepara::CoreParam* mpPartyBuffTemp[pml::PokeParty::MAX_MEMBERS];
#if PM_DEBUG
  u16 mPokemonNoBackup[pml::PokeParty::MAX_MEMBERS];
#endif

  IconAnim mIconAnimBuff[ ICONANIM_BUFF_NUM ];

  App::Box::PokemonIconDraw mBoxPokeIcon[Savedata::Box::TRAY_MAX][Savedata::BoxPokemon::TRAY_POKE_MAX];      //BOX
  App::Box::PokemonIconDraw mTemotiPokeIcon[pml::PokeParty::MAX_MEMBERS];   //手持ち
  App::Box::PokemonIconDraw mTeamPokeIcon[Savedata::BOX::TEAM_MAX][pml::PokeParty::MAX_MEMBERS];     //チームまたはリゾート（チームとリゾートはモード的かぶりが無い）

  //状態が異なるものだけ反映 持ち上げたものを記録
  PokemonIconDrawBackUp mBackup[Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS];
  u32 mPkdBackupNum;

  StockChangePos mBackupChnagePoke[Savedata::BoxPokemon::TRAY_POKE_MAX];
  int mBackupChnagePokeNum;

};


GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif // __BOX_SIMPLE_ACCESSER_H__

