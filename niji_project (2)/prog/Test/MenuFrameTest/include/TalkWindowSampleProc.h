//=============================================================================================
/**
 * @file   TalkWindowSampleProc.h
 * @brief  呼び出しのProcから
 * @author k.ohno
 * @date   2015.2.16
 */
//=============================================================================================
#if PM_DEBUG

#ifndef __TALK_WINDOW_SAMPLE_PROC_H__
#define __TALK_WINDOW_SAMPLE_PROC_H__
#pragma once

#include "GameSys/include/GameProc.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/app_util_heap.h"

#include "AppLib/include/Util/AppRenderingManager.h"


#include "FieldMessageSampleEvent.h"
#include "thread/include/gfl2_Scopedlock.h"
#include "thread/include/gfl2_Thread.h"


//下画面
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProc.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( TalkWindowTest )

class TalkWindowSampleFrame;

class Proc : public GameSys::GameProc {
//class Proc : public Field::SubScreen::FieldSubWindowProc {
  GFL_FORBID_COPY_AND_ASSIGN(Proc); //コピーコンストラクタ＋代入禁止



public:

  class ThreadFunc : public gfl2::thread::ThreadInterface{

  public:
		ThreadFunc(Proc* p);
    virtual ~ThreadFunc(){};
    
    void Function(void);
    

  private:
    Proc* mpThreadThis;
  };
  
private:
  static const int SIZE_PROC_MANAGER = 0x1000;   /// 4Kバイト  マネージャーのヒープは荒れるので別途管理

//  static const int SIZE_MY_APP = 0x2000;       ///2Mバイト  アプリメモリも容量管理のため分けて管理
//  static const int SIZE_MY_APPDEV = 0xb0000;    ///4Mバイト アプリメモリも容量管理のため分けて管理
  

  applib::frame::Manager *mpFrameManager;

  app::util::AppRenderingManager* mpAppRenderingManager;

  app::util::Heap* mpAppHeap;
  
  applib::frame::CellSuper* mpMenuCellFrame;

  gfl2::heap::HeapBase *mpManagerHeap;
  
public:
  Proc(void);  //コンストラクタ
  virtual ~Proc(void);  //デストラクタ

  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo no);

  //-----------------------------------------------------------------------------
  /**
   * @func    PreDrawFunc
   * @brief   Proc描画下準備
   * @date    2015.07.09
   *
   * @param   pManager    プロセス管理マネージャー
   */
  //-----------------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager * pManager );
  
  //------------------------------------------------------------------
  /**
   * @brief   PROCのメインを行う関数
   *  1/30フレームの②フレーム目の処理です。
   *  1/60フレーム動作時には、使われません。
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief   PROCのイベント起動検知処理
   *
   *  以下のタイミングで行うべき、イベント起動処理
   *  　GameProc->EventCheckFunc
   *  　EventManager->Main
   *  　GameProc->Main
   */
  //------------------------------------------------------------------
 // virtual void EventCheckFunc(GameManager* pManager);

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールド復帰　再稼働
   */
  //-----------------------------------------------------------------------------
 // virtual void Resume( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールド削除　一時停止
   */
  //-----------------------------------------------------------------------------
 // virtual void Suspend( void );

  applib::frame::Manager* GetFrameManager(void){ return mpFrameManager; };

  void SetAsyncFileManager(gfl2::fs::AsyncFileManager* pFM){mpAsyncFileManager = pFM;};

  app::util::AppRenderingManager* GetAppRenderingManager(void){  return mpAppRenderingManager;  }
  app::util::Heap* GetAppHeap(void){ return mpAppHeap; }

private:
	ThreadFunc* mpFunction;
  gfl2::fs::AsyncFileManager* mpAsyncFileManager;   //ファイルマネージャー

  gfl2::thread::Thread* mpThread;
  u32   mSeq;
  int   mLockTest;

};

GFL_NAMESPACE_END( TalkWindowTest )
GFL_NAMESPACE_END( Test )

#endif // __TALK_WINDOW_SAMPLE_PROC_H__

#endif // PM_DEBUG
