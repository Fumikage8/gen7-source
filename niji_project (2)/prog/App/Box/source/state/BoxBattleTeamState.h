#ifndef __BOXBATTLETEAMSTATE_H__
#define __BOXBATTLETEAMSTATE_H__
//=============================================================================
/**
 * @file	 BoxBattleTeamState.h
 * @brief	 P2p交換のシーケンス
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


#include "../BoxStateBasis.h"



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class PokePara;
class BoxLowBgDraw;
class BoxLowChtDraw;
class BoxAppSetup;
class BoxLowFolderDraw;

class BoxBattleTeamState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxBattleTeamState);


public:




  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxBattleTeamState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxBattleTeamState();

public:


  void Setup(void);

  void Release(void);

  virtual u32 StateFunc(void);

	void ModeStart(int tray,int cursor);  //メニューを開く

  void CarryStart(int tray,int cursor);  //ポケモンを持ち上げる
  
	void RangeModeStart(int tray,int cursor);


  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

private:

	void FirstMenuStart(void);
  void SetRangePokemon(bool bTouch);
  void MenuCancel(void);

  void CarryupPokemon(void);
  int Regist(void);
  void Remove(void);
  void RemoveTeam(int page,int pos);
    int getBattleChange(int changenum);
  void BattleInPageChange(int changenum);
  void PokeMoveExec(void);

  void CarryRange(void);
  void RangeStart(void);

  
  //BattleTeamSelect用
  class InnerBattleListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxBattleTeamState* pthis ){ mpThis = pthis;}
  private:
    BoxBattleTeamState* mpThis;
  };
  //BattleTeamListener用
  class InnerBattleTeamListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxBattleTeamState* pthis ){ mpThis = pthis;}
  private:
    BoxBattleTeamState* mpThis;
  };

  //なにもしない時用
  class DInnerNoneListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnTouchFirstHook( bool isTouch ){return true;};
    virtual bool OnKeyAction( u32 dir ){return false;};
    virtual void OnButtonAction( int button_no ){};
    virtual bool GetInputEnableFlg(void){ return false; }
    void SetThis( BoxBattleTeamState* pthis ){ mpThis = pthis;}
  private:
    BoxBattleTeamState* mpThis;
  };
  //ポケモンを抱えて動いている時用
  class DInnerPokeMoveListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnMousePokemonCurpos(int cursorno);
    virtual bool OnMousePokemonFirstTouch(int curpos);
    virtual void OnMousePokemonHand(int x,int y, int cursorno);
    virtual bool OnMousePokemonPut(int cursorno,int targetno);
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxBattleTeamState* pthis ){ mpThis = pthis;}
  private:
    BoxBattleTeamState* mpThis;
  };
  //複数選択時用
  class DInnerRangeMoveListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnMousePokemonCurpos(int cursorno){ return true; };
    virtual bool IsReleaseControl(void){ return false; }
    virtual void OnMouseDragCurpos(int cursorno);
    virtual bool OnMousePokemonFirstTouch(int cursorno);
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxBattleTeamState* pthis ){ mpThis = pthis;}
  private:
    BoxBattleTeamState* mpThis;
  };
  //ポケモンを複数抱えて動いている時用
  class DInnerCarryRangePokeMoveListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnMousePokemonCurpos(int cursorno){ return true; };
    virtual bool OnMousePokemonFirstTouch(int cursorno);
    virtual bool OnMousePokemonPut(int cursorno,int targetno);
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxBattleTeamState* pthis ){ mpThis = pthis;}
  private:
    BoxBattleTeamState* mpThis;
  };

  
  DInnerNoneListener mDInnerNoneListener;
  DInnerPokeMoveListener mDInnerPokeMoveListener;
  DInnerRangeMoveListener mDInnerRangeMoveListener;
  DInnerCarryRangePokeMoveListener mDInnerCarryRangePokeMoveListener;

  InnerBattleListener mInnerBattleListener;
  InnerBattleTeamListener mInnerBattleTeamListener;
  
  
  pml::pokepara::CoreParam* mpCoreTemp;  //マークつける時 交換検査に使用

  RangeData mRangeSelect;
  int mSelectTray;
  int mSelectCursor;
  int mDecideCursor;


};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXBATTLETEAMSTATE_H__




