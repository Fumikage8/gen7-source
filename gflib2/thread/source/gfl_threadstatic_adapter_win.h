#ifndef GFL_PLATFORM_3DS

#ifndef __GFL_THREADSTATIC_ADAPTER_WIN_H__
#define __GFL_THREADSTATIC_ADAPTER_WIN_H__
//=============================================================================
/**
 * @file	 gfl_threadstatic_adapter_win.h
 * @brief	 windowsでのスレッド実装部
 * @author k.ohno
 * @date	 2014.12.24
 */
//=============================================================================
#pragma once


#include "thread/include/gfl_threadstatic.h"
#include "thread/source/gfl_threadstatic_adapter.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)




class ThreadStaticAdapterWin : public ThreadStaticAdapter {
  GFL_FORBID_COPY_AND_ASSIGN(ThreadStaticAdapterWin);

public:

  //-----------------------------------------------------------------------
  /**
   * @brief スレッドコンストラクタ
   * @param pInterFace   上に書いてあるクラス Function 実行関数を書いた物を登録
   * @param pHeap        スレッドのメモリ確保のために使用します
   * @param bMainLoop    Functionを繰り返し呼ぶ場合 true
   */
  //-----------------------------------------------------------------------
  ThreadStaticAdapterWin(void);
  virtual ~ThreadStaticAdapterWin();
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



};



GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif __GFL_THREADSTATIC_ADAPTER_WIN_H__

#endif  // GFL_PLATFORM_3DS
