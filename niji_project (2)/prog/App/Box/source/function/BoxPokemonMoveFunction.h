#ifndef __BOXPOKEMONMOVEFUNCTION_H__
#define __BOXPOKEMONMOVEFUNCTION_H__
//=============================================================================
/**
 * @file	 BoxPokemonMoveFunction.h
 * @brief	 ポケモンの移動に関する機能をあつめたクラス
 * @author k.ohno
 * @date	 2016.2.27
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
//#include "BoxLowDrawListener.h"
//#include "BoxLowBgDrawListener.h"
#include "App/Box/include/BoxUppStatusListener.h"

#include "App/Box/include/BoxAppParam.h"

#include "AppLib/include/Util/AppRenderingManager.h"


#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>

//#include "BoxLowChtDrawListener.h"
//#include "BoxLowFolderDrawListener.h"





GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class PokePara;
class BoxLowBgDraw;
class BoxLowChtDraw;
class BoxAppSetup;
class BoxLowFolderDraw;

class BoxPokemonMoveFunction
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxPokemonMoveFunction);


public:

  enum {
    MOVEERR_NONE,    //問題ない
    MOVEERR_NO_PUT_PLACE,   //置く場所が無い
    MOVEERR_TEAM_LOCK,  // チームロック
    MOVEERR_TEAM_ENTRY, // チームに所属している
    MOVEERR_NO_BATTLEPOKE, //たたかうポケモンがいなくなる
    MOVEERR_NULL_POKEMON,  //移動するものが存在していなかった
    MOVEERR_EGG_PUT,   //ここにタマゴは配置できない
    MOVEERR_EGG_PULL,   //タマゴは操作してはいけない
    MOVEERR_NO_CONDITION,  //（交換）条件に該当しない
    MOVEERR_TEAM_LOCK_MEMBER,  //ロックされているメンバー]
    MOVEERR_TEAM_LOCK_MEMBER_REMOVE,//ロックされているメンバーをはずす
    MOVEERR_COMPATIBLE_ERROR,   // niji通信互換エラー
  };


  enum{
    RANGETEST_MOVE,  //移動できるかテスト
    RANGETEST_BATTLE,   //バトルに出れるかどうか
    RANGETEST_TEAM,  //チームに含まれてるかどうか
    RANGETEST_TEAMLOCK, //ロックしたチームかどうか
    RANGETEST_EXEC,  //実際のデータを移動
  };


  

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxPokemonMoveFunction(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxPokemonMoveFunction();

public:

  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録 削除
   */
  //------------------------------------------------------------------
  void Setup(APP_PARAM* pAppParam, BoxAppSetup* pBoxAppSetup,PokePara* pBoxPokeParaFunc);
  void Release(void);

	//チームからはずす
  void RemoveTeam(int page,int pos);
  
	//バトルボックス登録解除
	void BattleRegistOrRemove(int selectpage,int selectpos,int oldpage,int oldpos);
  //範囲指定でBOX領域にポケモンをおく
  int PutBoxRange(int selectpage,int selectpos, int nowpage, RangeData* pRange);

  //移動してよいかどうか 移動する時に検査
  int pokeMoveCheck(int tray,int pos, int nowtray, int nowpos);
  //全体ＢＯＸの移動の場合の戦うポケモンが残るかどうか検査
  int pokeMoveCheckFolderOnly(int tray,int pos, int nowtray, int nowpos);

  //置いていいかどうか検査
  int pokeMoveCheckRange(int selectpos,RangeData* pRange);

  //ポケパーティーにセット
  void SetPartyPokemon(int partypos, pml::pokepara::PokemonParam * pp);

  //ボックスと運動リゾートのデータ入れ替え
  void ReplaceBoxToGim(int boxpage,int boxpos,int partypage,int partypos);

  //運動リゾート内部データ入れ替え
  void ReplaceGimToGim(int boxpage,int boxpos,int partypage,int partypos);
  //範囲選択でパーティー内に戦うポケモンが残るかどうか
  bool BattlePokemonCheck(RangeData* pRange);
  //データの入れ替えラップクラス
  void ChangePokemonData(int selectpage,int selectpos,int oldpage,int oldpos);
  //ポケモンを入れ替える場合にいれかえ
  //範囲外は入れ替えない
  bool PokemonSet(int selectpage,int selectpos, int nowpage, int nowpos, int rangepos);
  //リンク張替え
  void ChangePokemonBoxLinkData(int selectpage,int selectpos,int oldpage,int oldpos);
  //ポケモンデータをボックス位置、カーソル位置から得る
  void GetPokemon(int boxno,int cursorno, pml::pokepara::CoreParam* cp);
  //ポケモンデータをボックス位置、カーソル位置から得る
  void GetPokemonPokemonParam(int boxno,int cursorno, pml::pokepara::PokemonParam* cp);

  //ボックスとパーティーのデータ入れ替え
  void ReplaceBoxToParty(int boxpage,int boxpos,int partypage,int partypos);

  //このポケモンがロックしているかどうか
  bool ThisPokeLockCheck(int tray,int cursor);

  //チーム全体を詰めなおす
  void TeamPackAll(void);

  //ポケモンデータをボックス位置、カーソル位置から得て、設定
//  void SetPokemon(int boxno,int cursorno, pml::pokepara::CoreParam* cp);
  //ポケモンデータをボックス位置、カーソル位置から得て、設定
  void SetPokemonPokemonParam(int boxno,int cursorno, pml::pokepara::PokemonParam* cp);

  //ポケモンを入れ替える場合にいれかえ  範囲用
  //もどりは MOVEERR_ENUM
  int RangePokemonSet(int nowcurpos,int selectcurpos, RangeData* pRange, int nowpage);


  // 持ち上げる時の計算部分
  // 持ち上がらない場合false
  bool SetCarryRange(RangeData* pRange,int* pnowcur,int* pselect, int nowtrayno);


  //交換に出してよいか まずい場合戻り値で帰す
  int FirstTradeCheck(int tray, int pos);
  
  
private:
  bool rangeCheck(int pos1,int pos2);

  bool RangeChangeCheckOrPutBoxToBox(RangeData* pRange , int nowpage, int nowpos,int bput);
  bool RangeChangeCheckOrPutTemotiToBox(RangeData* pRange , int nowpage, int nowpos,int bput);
  bool RangeChangeCheckOrPutBoxToTemoti(RangeData* pRange , int nowpage, int nowpos,int bput);
  bool RangeChangeCheckOrPutTemotiToTemoti( RangeData* pRange , int nowpage, int nowpos,int bput);

  bool RangeExec(int nowcurpos,int selectcurpos, RangeData* pRange, int nowpage, int mode);

	//範囲選択手持ち移動の時のみ、バトルに出れるかどうかの検査が必要になる
	void PokemonBattleCheckOfTemoti(int selectpage,int selectpos, int nowpage, int nowpos,u8* btlbuf);
  bool PokemonBattleCheckOfTemotiEnd( u8* btlbuf );
  void PokemonBattleCheckOfTemotiStart( u8* btlbuf, int nowpage );

  //範囲選択チーム選択、ジムではタマゴ不可
  void PokemonBattleCheckOfTeam(int selectpage,int selectpos, int nowpage, int nowpos,u8* btlbuf, int bput);
  bool PokemonBattleCheckOfTeamEnd( u8* btlbuf );
  void PokemonBattleCheckOfTeamStart( u8* btlbuf );


  
  
  struct PokeMopyStruct{
    u16 page1;
    u16 pos1;
    u16 page2;
    u16 pos2;
		u16 rangepos;
  };

  
  APP_PARAM* mpAppParam;
  BoxAppSetup* mpBoxAppSetup;
  pml::pokepara::PokemonParam* mpCoreTemp;
  app::util::Heap* mpAppHeap;
  PokePara* mpBoxPokeParaFunc;
;

};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXPOKEMONMOVEFUNCTION_H__




