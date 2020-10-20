#ifndef __BOXNORMALSTATE_H__
#define __BOXNORMALSTATE_H__
//=============================================================================
/**
 * @file	 BoxNormalState.h
 * @brief	 ノーマルのシーケンス
 * @author k.ohno
 * @date	 2016.3.03
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "App/Box/include/BoxUppStatusListener.h"

#include "App/Box/include/BoxAppParam.h"

#include "AppLib/include/Util/AppRenderingManager.h"

#include "../BoxStateEndListener.h"

#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>



#include "../BoxStateBasis.h"



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class PokePara;
class BoxLowBgDraw;
class BoxLowChtDraw;
class BoxAppSetup;
class BoxLowFolderDraw;

class BoxNormalState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxNormalState);


  //全体ボックス終了時
  class InnerBoxAllBoxEndListener  : public BoxStateEndListener
  {
  public:
    virtual void OnEnd( u32 retcode ){mpThis->mBoxAllBoxStateRet=retcode; };
    void SetThis( BoxNormalState* pthis ){ mpThis = pthis;}
  private:
    BoxNormalState* mpThis;
  };
  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxNormalState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxNormalState();

public:


  void Setup(void);

  void Release(void);

  virtual u32 StateFunc(void);

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

  //--呼び出し方--------------------------------------------
  //他のメニューから選択
	void ModeStart(int tray,int cursor);
    //ショートカット
  void ShortcutStart(int tray,int cursor);
    //範囲選択
  void RangeStart(int tray,int cursor);

  void LocalSetup( BoxLowFolderDraw* pBoxLowFolderDraw );

  
private:

  void FirstMenuInit(void);
  void MenuCancel(void);
  void CarryupPokemon(void);
  int PokeMoveExec(void);
  void RangeStart(void);
  void SetRangePokemon(bool bTouch);
  void CarryRange(void);


  InnerBoxAllBoxEndListener mInnerBoxAllBoxEndListener;//結果のリスナー
  u32 mBoxAllBoxStateRet;  //その返
 
  
  //normal用
  class InnerNormalListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxNormalState* pthis ){ mpThis = pthis;}
  private:
    BoxNormalState* mpThis;
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
    void SetThis( BoxNormalState* pthis ){ mpThis = pthis;}
  private:
    BoxNormalState* mpThis;
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
    void SetThis( BoxNormalState* pthis ){ mpThis = pthis;}
  private:
    BoxNormalState* mpThis;
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
    void SetThis( BoxNormalState* pthis ){ mpThis = pthis;}
  private:
    BoxNormalState* mpThis;
  };
  //ポケモンを複数抱えて動いている時用
  class DInnerCarryRangePokeMoveListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnMousePokemonCurpos(int cursorno);
    virtual bool OnMousePokemonPut(int cursorno,int targetno);
    virtual bool OnMousePokemonFirstTouch(int cursorno);
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxNormalState* pthis ){ mpThis = pthis;}
  private:
    BoxNormalState* mpThis;
  };

#if PM_DEBUG
  int mDebugSeqBackup;
#endif
  

  BoxLowFolderDraw* mpBoxLowFolderDraw;

  
  pml::pokepara::CoreParam* mpCoreTemp;  //マークつける時 交換検査に使用


  DInnerNoneListener mDInnerNoneListener;
  DInnerPokeMoveListener mDInnerPokeMoveListener;
  DInnerRangeMoveListener mDInnerRangeMoveListener;
  DInnerCarryRangePokeMoveListener mDInnerCarryRangePokeMoveListener;
  
  InnerNormalListener mInnerNormalListener;


  RangeData mRangeSelect;

  
  int mSelectTray;
  int mSelectCursor;
  int mDecideCursor;

public:
  void SetBoxUppStatusListener(BoxUppStatusListener* pBoxUppStatusListener){ mpBoxUppStatusListener = pBoxUppStatusListener; };

private:
  BoxUppStatusListener* mpBoxUppStatusListener;  //上画面との掛け渡し

};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXNORMALSTATE_H__




