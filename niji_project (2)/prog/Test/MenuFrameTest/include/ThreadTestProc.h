//=============================================================================================
/**
 * @file   MenuTestProc.h
 * @brief  呼び出しのProcから
 * @author k.ohno
 * @date   2015.2.16
 */
//=============================================================================================
#if PM_DEBUG

#ifndef __MENU_TEST_PROC_H__
#define __MENU_TEST_PROC_H__
#pragma once

#include "GameSys/include/GameProc.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Frame/AppFrameListener.h"
#include "AppLib/include/Util/app_util_heap.h"

#include "AppLib/include/Util/AppRenderingManager.h"


#include "MenuSampleEvent.h"
#include "thread/include/gfl2_Scopedlock.h"
#include "thread/include/gfl2_Thread.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ThreadTest )

class MenuSampleFrame;

class Proc :  public GameSys::GameProc {
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

  static const int SIZE_MY_APP = 0x180000;       ///2Mバイト  アプリメモリも容量管理のため分けて管理
  static const int SIZE_MY_APPDEV = 0x200000;    ///4Mバイト アプリメモリも容量管理のため分けて管理
  
  app::util::Heap* mpAppHeap;
  
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


  //------------------------------------------------------------------
  /**
   * @brief	  EndFuncが完了した直後に呼び出される関数
   */
  //------------------------------------------------------------------
//  virtual void EndFuncFinish(void);

	void scopeTest(void);
 

  app::util::Heap* GetAppHeap(void){ return mpAppHeap; }

private:
  ThreadFunc* mpFunction;

  gfl2::thread::Thread* mpThread;
  u32   mSeq;
  int   mLockTest;
private:
  gfl2::thread::CriticalSection m_criticalSection;

};

GFL_NAMESPACE_END( ThreadTest )
GFL_NAMESPACE_END( Test )

#endif // __MENU_TEST_PROC_H__

#endif // PM_DEBUG
