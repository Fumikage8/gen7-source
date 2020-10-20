#ifndef __BOXSTATEBASIS_H__
#define __BOXSTATEBASIS_H__
//=============================================================================
/**
 * @file	 BoxStateBasis.h
 * @brief	 BOXの状態の基底
 * @author k.ohno
 * @date	 2016.2.28
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
//#include "BoxLowDrawListener.h"
#include "BoxLowBgDrawListener.h"
#include "App/Box/include/BoxUppStatusListener.h"

#include "App/Box/include/BoxAppParam.h"

#include "AppLib/include/Util/AppRenderingManager.h"
#include "BoxStateEndListener.h"


#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>






GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class PokePara;
class BoxLowBgDraw;
class BoxLowChtDraw;
class BoxAppSetup;
class BoxLowFolderDraw;
class BoxPokemonMoveFunction;
class BoxLowBgFrame;

class BoxStateBasis
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxStateBasis);


public:

  enum {
    BOXSTATE_NONE,   //シーケンスはそのまま
    BOXSTATE_RETURN,  //もどる
    BOXSTATE_RETURN_POKEMON_NORMAL,  //ポケモンをもったまま（単体）
    BOXSTATE_RETURN_POKEMON_RANGE,  //ポケモンをもったまま（範囲）
    BOXSTATE_APP_JUMP, //アプリに進む,終了する
    BOXSTATE_FIRST_WAIT,  //待機状態
    BOXSTATE_NORMAL_MOVE,   // 普通のポケモン移動
    BOXSTATE_ITEM,     // アイテム
    BOXSTATE_MARK,    //マーク
    BOXSTATE_TEAM,     // チーム
    BOXSTATE_ALLBOX,   //全体ボックス
    BOXSTATE_NETWORK,  //P2P通信
    BOXSTATE_RESORT,   //リゾート
    BOXSTATE_SIMPLESELECT,  //選ぶだけ GTS,MIRACLE,FIELD
    BOXSTATE_STATUS,   //ステータス表示アプリに進む
    BOXSTATE_EXIT_STATE,  //終了（通信）待ちに進む
    BOXSTATE_ESCAPE,  //にげるに進む
    BOXSTATE_WARRNING_TEAM,  //チームポケモン移動不可
    BOXSTATE_WARRNING_BATTLE,  //たたかうぽけもんがいなくなる
  };


  static const int MENU_1LINE_POS = -80;
  static const int MENU_2LINE_POS = -60;

  static const int MENU_SEQ_INIT = -1;

  struct MessageParam{
    u32 MsgNo;
    u32 LineOrPos;
    u32 bWarrningSe;
  };


  
  
public:

	BoxStateBasis(void);
	virtual ~BoxStateBasis();

  virtual u32 StateFunc(void)=0;

public:

  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録  今は沢山書いておかないとどもならない
   */
  //------------------------------------------------------------------
  void SetParam(BoxStateEndListener* pListener, APP_PARAM* pAppParam, BoxAppSetup* pBoxAppSetup,
             PokePara* pBoxPokeParaFunc,BoxPokemonMoveFunction* pBoxPokemonMoveFunction,
             BoxLowBgFrame* pBoxLowBgFrame , app::tool::MenuWindow* pMenuWindow,
             BoxLowBgDraw* pBoxLowBgDraw);


  void End(void);

	void simpleWarrningMessage(u32 no, u32 line, u32 bse );

protected:

  int UpdateStart(void);
  void SetUpdateStartSeq(int seq){ mUpdateStartSeq=seq; };
  int mUpdateStartSeq;
  
  BoxStateEndListener* mpListener;
  u32 mRetCode;

  BoxLowBgDraw* mpBoxBgLowDraw;
  APP_PARAM* mpAppParam;
  BoxAppSetup* mpBoxAppSetup;
  app::tool::MenuWindow* mpMenuWindow;
  app::util::Heap* mpAppHeap;
  PokePara* mpBoxPokeParaFunc;
  BoxPokemonMoveFunction* mpBoxPokemonMoveFunction;
  BoxLowBgFrame* mpBoxLowBgFrame;

  MessageParam mMessageParam;

  
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXSTATEBASIS_H__




