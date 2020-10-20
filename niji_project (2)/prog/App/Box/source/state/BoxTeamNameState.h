#ifndef __BOXTEAMNAMESTATE_H__
#define __BOXTEAMNAMESTATE_H__
//=============================================================================
/**
 * @file	 BoxTeamNameState.h
 * @brief	 チームネーム付けの遷移
 * @author k.ohno
 * @date	 2016.3.04
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

class BoxTeamNameState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxTeamNameState);


public:




  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxTeamNameState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxTeamNameState();

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
	void ModeStart(int tray, int cursor);


private:

  void TeamNameFuncStart(void);

  
  // チームネーム登録のインナークラス
  class InnerTeamNameYesNoListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxTeamNameState* pthis ){ mpThis = pthis;}
  private:
    BoxTeamNameState* mpThis;
  };

  InnerTeamNameYesNoListener mInnerTeamNameYesNoListener;

  int mSelectTray;
	int mSelectCursor;

};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXTEAMNAMESTATE_H__




