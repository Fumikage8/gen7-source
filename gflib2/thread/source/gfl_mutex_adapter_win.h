#ifndef __GFL_MUTEX_ADAPTER_WIN_H__
#define __GFL_MUTEX_ADAPTER_WIN_H__
//=============================================================================
/**
 * @file	 gfl_mutex_adapter_win.h
 * @brief	 ミューテックス std実装
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once

#if defined(GF_PLATFORM_WIN32)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mutex>

#include <thread/source/gfl_mutex_adapter.h>


/*
    セマフォ 	ある処理があって、同時にその処理ができるスレッド数が限られている場合に使用。プロセスをまたいで排他制御可能
★    ミューテックス 	ある処理があって、同時にその処理ができるスレッドが一つだけに限られている場合に使用。プロセスをまたいで排他制御可能
    クリティカルセクション 	ある処理があって、同時にその処理ができるスレッドが一つだけに限られている場合に使用。プロセスをまたいで排他制御不可能
*/


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


class MutexAdapterWin : public MutexAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(MutexAdapterWin);

public:
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ ここではなにもしません
   */
  //-----------------------------------------------------------------------
  MutexAdapterWin(void);
  //-----------------------------------------------------------------------
  /**
   * @brief ロック ここ以降他のスレッドからは割り込まれなくなる
   */
  //-----------------------------------------------------------------------
  virtual void lock(void);
  //-----------------------------------------------------------------------
  /**
   * @brief  ロックできるのであればロックする
   * @return falseロックできなかった  true ロックできた
   */
  //-----------------------------------------------------------------------
  virtual bool tryLock(void);
  //-----------------------------------------------------------------------
  /**
   * @brief アンロック  ここ以降他のスレッドの割り込みを許可する
   */
  //-----------------------------------------------------------------------
  virtual void unlock(void);

private:
  ::std::mutex mMutex;

};



GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif  //GF_PLATFORM_WIN32

#endif //__GFL_MUTEX_ADAPTER_WIN_H__
