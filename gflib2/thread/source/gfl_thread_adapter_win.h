#ifndef __GFL_THREAD_ADAPTER_WIN_H__
#define __GFL_THREAD_ADAPTER_WIN_H__
//=============================================================================
/**
 * @file	 gfl_thread_adapter_win.h
 * @brief	 windowsでのスレッド実装部
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once

#if defined(GF_PLATFORM_WIN32)

#include "thread/include/gfl2_Thread.h"
#include "thread/source/gfl_thread_adapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)




class ThreadAdapterWin : public ThreadAdapter {
  GFL_FORBID_COPY_AND_ASSIGN(ThreadAdapterWin);

public:

  //-----------------------------------------------------------------------
  /**
   * @brief スレッドコンストラクタ
   * @param pInterFace   上に書いてあるクラス Function 実行関数を書いた物を登録
   * @param pHeap        スレッドのメモリ確保のために使用します
   * @param bMainLoop    Functionを繰り返し呼ぶ場合 true
   */
  //-----------------------------------------------------------------------
  ThreadAdapterWin(ThreadInterface* pInterFace, gfl2::heap::HeapBase* pHeap, bool bMainLoop = true);
  virtual ~ThreadAdapterWin();
  //-----------------------------------------------------------------------
  /**
   * @brief プライオリティーを取得する
   */
  //-----------------------------------------------------------------------
  virtual int GetPriority(void);
  //-----------------------------------------------------------------------
  /**
   * @brief プライオリティーを設定する 
   */
  //-----------------------------------------------------------------------
  virtual void SetPriority(int no);
  //-----------------------------------------------------------------------
  /**
   * @brief スレッドスタート
   * @param priority 
   */
  //-----------------------------------------------------------------------
  virtual void Start(void);
  virtual void Start(int priority);
  //-----------------------------------------------------------------------
  /**
   * @brief スレッドが終わるまでブロックします
   */
  //-----------------------------------------------------------------------
  virtual void Wait(void);
  //-----------------------------------------------------------------------
  /**
   * @brief カレントスレッドのイールド処理  再スケジュールの機会を与える。
   */
  //-----------------------------------------------------------------------
  virtual void CurrentYield( void );
  //-------------------------------------------------------------------------
  /**
   * @brief カレントスレッドを指定時間停止させる
   *
   * @param milliSeconds スレッドを停止させる時間（ミリ秒）
   */
  //-------------------------------------------------------------------------
  virtual void CurrentSleep( s32 milliSeconds );
  //-------------------------------------------------------------------------
  /**
   * @brief カレントスレッドを指定時間停止させる
   *
   * @param microSeconds スレッドを停止させる時間（マイクロ秒）
   */
  //-------------------------------------------------------------------------
  virtual void CurrentSleepMicro( s32 microSeconds );
  //-------------------------------------------------------------------------
  /**
   * @brief   スレッドが終了したかどうか確認する
   * @return  終了していたらtrue
   */
  //-------------------------------------------------------------------------
  virtual bool IsAlive(void);
  //-------------------------------------------------------------------------
  /**
   * @brief スレッドを終了処理へいかせる。強制終了ではないのでIsAliveで確認すること。
   */
  //-------------------------------------------------------------------------
  virtual void Kill(void);

public:
  //-----------------------------------------------------------------------
  /**
   * @brief スレッドを呼ぶ 実行部分
   * @param pInterFace   上に書いてあるクラス Function 実行関数を書いた物を登録
   * @param pHeap        スレッドのメモリ確保のために使用します
   */
  //-----------------------------------------------------------------------
	void operator()();

private:
  ThreadInterface* mpInterFace;
  gfl2::heap::HeapBase* mpHeap;
  bool mbMainLoop;
  ::std::thread* mpThread;

  Event* mpMyDead;
  Event* mpKillReq;

};



GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif //GF_PLATFORM_WIN32

#endif __GFL_THREAD_ADAPTER_WIN_H__


