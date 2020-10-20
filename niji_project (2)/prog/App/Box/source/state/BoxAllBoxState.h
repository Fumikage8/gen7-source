#ifndef __BOXALLBOXSTATE_H__
#define __BOXALLBOXSTATE_H__
//=============================================================================
/**
 * @file	 BoxAllBoxState.h
 * @brief	 全体ボックスの操作のシーケンス
 * @author k.ohno
 * @date	 2016.2.29
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
#include "../BoxLowFolderDrawListener.h"


#include "../BoxStateBasis.h"

#include "../../include/BoxUppStatusListener.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class PokePara;
class BoxLowBgDraw;
class BoxLowChtDraw;
class BoxAppSetup;
class BoxLowFolderDraw;

class BoxAllBoxState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxAllBoxState);


public:




  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxAllBoxState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxAllBoxState();

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

  

  //全体ボックスボタンから呼び出し
  void FolderStart(int foldercursor,int tray,int cursor);
  //検索からのもどり
  void FolderStartRe(int foldercursor,int tray,int cursor);
  
  //ポケモンを持った状態
  void FolderPokemonMoveStart(int tray,int cursor);

  //ポケモンを範囲で持った状態
  void FolderRangeMoveStart(int tray,int cursor,const RangeData* pRange);

  void AllBoxLocalSetup(  BoxLowFolderDraw* pBoxLowFolderDraw );

  u32 GetFolderCursorPos(void);

  
private:

  //フォルダー画面初期化 共通処理
  void FolderAnimInitCommon(bool bfindreturn=false);

  //フォルダー画面初期化 掴んで無い状態
  void FolderAnimInitNoPoke(bool bfindreturn=false);

  //フォルダー画面初期化 掴んだ状態
  void FolderAnimInitCatch(void);
  void FolderAnimInitCatch2(void);  // kawa_box_modify  使用中のペインのテクスチャを貼り替えるので処理を2フレームに分ける
  
  //フォルダー画面初期化 範囲状態
  void FolderAnimInitRange(void);

	//描画部分を終了させる主にキャンセル
	void DrawEnd(void);

  //フォルダー画面初期化
  void FolderAnimInit(bool brange);
  //カーソルの位置
  void FolderCursorAction(int udlr);
  //フォルダーモードの終了の時 共通処理
  void FolderModeEnd(void);

  //フォルダーからボックスにポケモンを置く
  int PutBox(int selectpage,int selectpos, int nowpage);
  //フォルダーの見た目の再計算
  void MathBoxPokeNum(void);
  //フォルダーからボックスにポケモンを置く 範囲のポケモン
  int PutBoxRangeFunc(int selectpage,int selectpos, int nowpage);
  //検索ボタンが押された
  void ActionFindButton(void);
  //メイン画面へ戻る
  void FolderModeChangeToMain(void);

  //   検索ボタンをＯＮにする
  void SetSearchButtonActive(void);


   class DInnerFolderNoneListener : public BoxLowFolderDrawListener
   {
   public:
     virtual void OnButtonAction( int button_no ){};
     virtual bool OnTouchFirstHook( bool isTouch ){return true;};  //処理しない
     virtual bool GetInputEnableFlg(void){ return false; }  //ButtonManager止める
     void SetThis( BoxAllBoxState* pthis ){ mpThis = pthis;}
  private:
    BoxAllBoxState* mpThis;
   };

  
  //フォルダー画面用
  class DInnerFolderViewListener : public BoxLowFolderDrawListener
  {
  public:

    //--------------------------------------------------------------------------
    /**
     * @brief   タッチ開始
     */
    //--------------------------------------------------------------------------
    virtual void TouchFolder( int index );
    //--------------------------------------------------------------------------
    /**
     * @brief   つかみ開始
     */
    //--------------------------------------------------------------------------
    virtual void OnFolderHand( int x, int y, int index );
    //--------------------------------------------------------------------------
    /**
     * @brief   フォルダー移動
     */
    //--------------------------------------------------------------------------
    virtual void MoveFolder( int index ,int selectindex);
    //--------------------------------------------------------------------------
    /**
     * @brief   フォルダーにポケモンを入れる
     */
    //--------------------------------------------------------------------------
    virtual void DropFolder( int index );
    //--------------------------------------------------------------------------
    /**
     * @brief   drag移動中の処理
     */
    //--------------------------------------------------------------------------
    virtual bool DragDrop( int index );
    //--------------------------------------------------------------------------
    /**
     * @brief   カーソル移動もしくはフォルダーに入る
     */
    //--------------------------------------------------------------------------
    virtual void ChangeFolder( int index,int index2 );

    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxAllBoxState* pthis ){ mpThis = pthis;}
  private:
    BoxAllBoxState* mpThis;
  };
  //フォルダー画面複数選択用
  class DInnerFolderViewRangeListener : public BoxLowFolderDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    //--------------------------------------------------------------------------
    /**
     * @brief   タッチ開始
     */
    //--------------------------------------------------------------------------
    virtual void TouchFolder( int index );
    //--------------------------------------------------------------------------
    /**
     * @brief   つかみ開始
     */
    //--------------------------------------------------------------------------
    virtual void OnFolderHand( int x, int y, int index );
    //--------------------------------------------------------------------------
    /**
     * @brief   drag移動中の処理
     */
    //--------------------------------------------------------------------------
    virtual bool DragDrop( int index );
    //--------------------------------------------------------------------------
    /**
     * @brief   フォルダーにポケモンを入れる
     */
    //--------------------------------------------------------------------------
    virtual void DropFolder( int index );
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxAllBoxState* pthis ){ mpThis = pthis;}
  private:
    BoxAllBoxState* mpThis;
  };

	pml::pokepara::CoreParam* mpCoreTemp;

  
  DInnerFolderViewListener mDInnerFolderViewListener;
  DInnerFolderViewRangeListener mDInnerFolderViewRangeListener;
  DInnerFolderNoneListener mDInnerFolderNoneListener;


  BoxLowFolderDraw* mpBoxLowFolderDraw;
  RangeData mRangeSelect;
  
 // u32 mSelectTray;
//  u32 mSelectCursor;

  int mFolderCursorPosBack;
  int mFolderCursorPos;

  int mTempCursorNo;


  u32 mSelectPagePos;
  u32 mSelectCursorPos;
  
  bool mbFolderPokeCatch;
  bool mbFadeEnd;

public:
  void SetBoxUppStatusListener(BoxUppStatusListener* pBoxUppStatusListener){ mpBoxUppStatusListener = pBoxUppStatusListener; };

private:
  BoxUppStatusListener* mpBoxUppStatusListener;  //上画面との掛け渡し
  
#if PM_DEBUG
  int mDebugSeqBackup;
#endif

};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXALLBOXSTATE_H__




