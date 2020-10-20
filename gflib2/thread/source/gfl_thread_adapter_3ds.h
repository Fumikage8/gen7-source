//=============================================================================
/**
 * @file	 gfl_thread_adapter_3ds.h
 * @brief	 3dsでのスレッド実装部
 * @author k.ohno
 * @date	 2015.1.23
 */
//=============================================================================
#if defined( GF_PLATFORM_CTR )

#ifndef __GFL_THREAD_ADAPTER_3DS_H__
#define __GFL_THREAD_ADAPTER_3DS_H__
#pragma once

#include <nn/os.h>

#include "thread/include/gfl2_Thread.h"
#include "thread/source/gfl_thread_adapter.h"




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)

class SimpleStackArea;


class ThreadAdapter3ds : public ThreadAdapter {
  GFL_FORBID_COPY_AND_ASSIGN(ThreadAdapter3ds);

private:
  static const s32 DEFAULT_PTIORITY = Thread::THREAD_PRI_NORMAL;  ///< デフォルトプライオリティ

public:

  //-----------------------------------------------------------------------
  /**
   * @brief スレッドコンストラクタ
   * @param pInterFace   上に書いてあるクラス Function 実行関数を書いた物を登録
   * @param pHeap        スレッドのメモリ確保のために使用します
   * @param bMainLoop    Functionを繰り返し呼ぶ場合 true
   * @param stackSize    スタック領域のサイズ
   */
  //-----------------------------------------------------------------------
  ThreadAdapter3ds(ThreadInterface* pInterFace, gfl2::heap::HeapBase* pHeap, bool bMainLoop, u32 stackSize );
  virtual ~ThreadAdapter3ds();
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


private:
  //-------------------------------------------------------------------------
  /**
   * @brief   スレッド実行部分
   * @param   arg_ptr   ThreadAdapterFratクラス
   */
  //-------------------------------------------------------------------------
  static void Function( void * arg_ptr );


private:
  ThreadInterface* mpInterFace;
  gfl2::heap::HeapBase* mpHeap;
 // fratgfl2::heap::HeapBaseAllocator * mpHeapAllocator;
  bool mbMainLoop;
  nn::os::Thread * mpThread;
	SimpleStackArea* mpStack;

//  Event* mpMyDead;
  Event* mpKillReq;

  bool mEndFlag;
};






GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif  // __GFL_THREAD_ADAPTER_3DS_H__

#endif  // GFL_PLATFORM_3DS
