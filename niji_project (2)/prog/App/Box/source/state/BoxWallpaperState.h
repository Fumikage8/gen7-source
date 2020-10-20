#ifndef __BOXWALLPAPERSTATE_H__
#define __BOXWALLPAPERSTATE_H__
//=============================================================================
/**
 * @file	 BoxWallpaperState.h
 * @brief	 壁紙のシーケンス
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

class BoxWallpaperState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxWallpaperState);


  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxWallpaperState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxWallpaperState();

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
	void ModeStart(int tray,int cur);


private:

    //ボックスのタイトルを選択した時のリスナー
  class InnerBoxOptionListener  : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxWallpaperState* pthis ){ mpThis = pthis;}
  private:
    BoxWallpaperState* mpThis;
  };
  //壁紙メニュー
  class   InnerPaperMenuListener   : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxWallpaperState* pthis ){ mpThis = pthis;}
  private:
    BoxWallpaperState* mpThis;
  };
  //壁紙メニュー風景１
  class   InnerPaperLandscape1Listener   : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxWallpaperState* pthis ){ mpThis = pthis;}
  private:
    BoxWallpaperState* mpThis;
  };
  //壁紙メニュー風景２
  class   InnerPaperLandscape2Listener   : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxWallpaperState* pthis ){ mpThis = pthis;}
  private:
    BoxWallpaperState* mpThis;
  };
  //壁紙メニュー風景３
  class   InnerPaperLandscape3Listener   : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxWallpaperState* pthis ){ mpThis = pthis;}
  private:
    BoxWallpaperState* mpThis;
  };
  //壁紙メニュー風景 その他
  class   InnerPaperEtcListener   : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxWallpaperState* pthis ){ mpThis = pthis;}
  private:
    BoxWallpaperState* mpThis;
  };

  InnerBoxOptionListener mInnerBoxOptionListener;
  InnerPaperMenuListener mInnerPaperMenuListener;
  InnerPaperLandscape1Listener mInnerPaperLandscape1Listener;
  InnerPaperLandscape2Listener mInnerPaperLandscape2Listener;
  InnerPaperLandscape3Listener mInnerPaperLandscape3Listener;
  InnerPaperEtcListener mInnerPaperEtcListener;


  void PaperMenuOpen(void);
  void PaperXMenuOpen(int no);
  void PaperChangeExec( int no );
  void BoxMenuOpen(void);



  
  
  pml::pokepara::CoreParam* mpCoreTemp;  //マークつける時 交換検査に使用

	int mSelectCursor;
  int mSelectTray;

};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXWALLPAPERSTATE_H__




