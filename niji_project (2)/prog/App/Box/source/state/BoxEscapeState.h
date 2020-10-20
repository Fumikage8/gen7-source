#ifndef __BOXESCAPESTATE_H__
#define __BOXESCAPESTATE_H__
//=============================================================================
/**
 * @file	 BoxEscapeState.h
 * @brief	 逃がすシーケンス
 * @author k.ohno
 * @date	 2016.3.3
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

class BoxEscapeState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxEscapeState);


  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxEscapeState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxEscapeState();

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


private:

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
    void SetThis( BoxEscapeState* pthis ){ mpThis = pthis;}
  private:
    BoxEscapeState* mpThis;
  };

  
  // にがすかどうかをきめるときの受け取るインナークラス
  class InnerEscapeYesNoListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxEscapeState* pthis ){ mpThis = pthis;}
  private:
    BoxEscapeState* mpThis;
  };


  void EscapeFuncStart(void);
  void EscapeExecStart(void);
  void EscapeExecEnd(void);
  void EscapeExecFinish(void);
  bool pokeEscapeCheck(void);



  
  
  InnerEscapeYesNoListener mInnerEscapeYesNoListener;
  DInnerNoneListener mDInnerNoneListener;
  
  pml::pokepara::CoreParam* mpCoreTemp;  //マークつける時 交換検査に使用
	pml::PokeParty* mpPartyTemp;  // 交換検査に使用

  
  u32 mSelectTray;
  u32 mSelectCursor;


};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXESCAPESTATE_H__




