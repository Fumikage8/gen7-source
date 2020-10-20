#ifndef __BOX_WARRNINGMESSAGE_H__
#define __BOX_WARRNINGMESSAGE_H__
//=============================================================================
/**
 * @file	 BoxWarrningMessageState.h
 * @brief	 ワーニングメッセージのシーケンス
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

class BoxWarrningMessageState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxWarrningMessageState);


public:




  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxWarrningMessageState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxWarrningMessageState();


  void SetParam(BoxLowBgDraw* pBoxLowBgDraw);

public:

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

  void SimpleWarrningMessage(const MessageParam* pMsgPara);


  virtual void SetSubSeq(int seq);
  
private:

 
  //汎用メッセージ終了用
  class DInnerMessageDefMessageListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnTouchFirstHook( bool isTouch );
    virtual bool OnKeyAction( u32 dir );
    virtual bool PreButtonAction( int button_no );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxWarrningMessageState* pthis);
  private:
    BoxWarrningMessageState* mpThis;
  public:
    u32 mHookStat;
  };

  DInnerMessageDefMessageListener mDInnerMessageDefMessageListener;
  


};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOX_WARRNINGMESSAGE_H__




