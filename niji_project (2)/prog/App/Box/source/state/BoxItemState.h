#ifndef __BOXITEMSTATE_H__
#define __BOXITEMSTATE_H__
//=============================================================================
/**
 * @file	 BoxItemState.h
 * @brief	 アイテム移動のシーケンス
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

class BoxItemState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxItemState);


public:


  enum{
    ITEMEXEC_OK, //無事に処理が完了
    ITEMEXEC_TAMAGO,   //たまご
    ITEMEXEC_NOPOKEMON,  //ポケモンがいない
    ITEMEXEC_TEAMLOCK,   //ロックされている
  };


  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxItemState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxItemState();

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

  //ショートカットモードから Aorタッチ
  void CarryModeStart(int tray,int cursor, bool bTouch);

  //アイテムモードの時に最初の決定処理 メニューを出す
  void FirstDecide(int tray,int cursor);

private:

  ///アイテムを運ぶメニューの表示
  void StartItemCarryMenu(int itemno);
  //アイテムの入れ替え
  int ChangeItem(void);
  //アイテムの入れ替え
  void ChangeItemData(int selectpage,int selectpos,int oldpage,int oldpos);
  // アイテムを掴む
  void ItemHandStart(void);

  void MenuCancel(void);
  //もちあげる
  void CarryUpItem(bool bTouch);
  //バッグに入れる
  void RemoveItem(void);


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
    void SetThis( BoxItemState* pthis ){ mpThis = pthis;}
  private:
    BoxItemState* mpThis;
  };

  
  //アイテムを抱えて動いている時用
  class DInnerItemMoveListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnMousePokemonCurpos(int cursorno);
    virtual void OnNoTouchRelease(int pos);
    virtual void OnMousePokemonHand(int x,int y, int cursorno);
    virtual void OnMouseMenuOpen(int pos);
    virtual bool OnMousePokemonPut(int cursorno,int targetno);
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxItemState* pthis ){ mpThis = pthis;}
  private:
    BoxItemState* mpThis;
  };

  // 簡単にアイテムを戻すだけの時のインナークラス
  class InnerItemSimpleRemoveYesNoListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxItemState* pthis ){ mpThis = pthis;}
  private:
    BoxItemState* mpThis;
  };
  // アイテムを戻す時のインナークラス
  class InnerItemRemoveYesNoListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxItemState* pthis ){ mpThis = pthis;}
  private:
    BoxItemState* mpThis;
  };
  // アイテムを持たせる時のインナークラス
  class InnerItemHandYesNoListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxItemState* pthis ){ mpThis = pthis;}
  private:
    BoxItemState* mpThis;
  };


  struct ItemIconAnimControl{
    int page;
    int pos;
		int nextpage;
		int nextpos;
    int itemno;
    bool bfchange;
  };
  static const int ITEM_ICON_ANIM_CONTROL = 2;

  void SetItemIconAnim(int no, bool bfchange, int page,int pos, int nextpage,int nextpos,int itemno);
  void ItemIconAnimStart(int no);
  bool ItemIconAnimChk(int no);

  
  InnerItemSimpleRemoveYesNoListener mInnerItemSimpleRemoveYesNoListener;
  InnerItemRemoveYesNoListener mInnerItemRemoveYesNoListener;
  InnerItemHandYesNoListener mInnerItemHandYesNoListener;
  DInnerItemMoveListener mDInnerItemMoveListener;
  DInnerNoneListener mDInnerNoneListener;

  ItemIconAnimControl mItemIconAnimControl[ITEM_ICON_ANIM_CONTROL];
  pml::pokepara::CoreParam* mpCoreTemp;  //マークつける時 交換検査に使用

  u32 mSelectTray;
  u32 mSelectCursor;
  int mDecideCursor;
  bool mbTouchStart;


#if PM_DEBUG
  int mDebugSeqBackup;
#endif

};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXITEMSTATE_H__




