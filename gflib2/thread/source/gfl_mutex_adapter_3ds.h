//=============================================================================
/**
 * @file	 gfl_mutex_adapter_3ds.h
 * @brief	 ミューテックス 3ds実装
 * @author k.ohno
 * @date	 2015.1.23
 */
//=============================================================================
#if defined( GF_PLATFORM_CTR )

#ifndef __GFL_MUTEX_ADAPTER_3DS_H__
#define __GFL_MUTEX_ADAPTER_3DS_H__

#pragma once



#include <thread/source/gfl_mutex_adapter.h>


#include <nn/os.h>

/*
    セマフォ 	ある処理があって、同時にその処理ができるスレッド数が限られている場合に使用。プロセスをまたいで排他制御可能
★    ミューテックス 	ある処理があって、同時にその処理ができるスレッドが一つだけに限られている場合に使用。プロセスをまたいで排他制御可能
    クリティカルセクション 	ある処理があって、同時にその処理ができるスレッドが一つだけに限られている場合に使用。プロセスをまたいで排他制御不可能
*/


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


class MutexAdapter3ds : public MutexAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(MutexAdapter3ds);

public:
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ ここではなにもしません
   */
  //-----------------------------------------------------------------------
  MutexAdapter3ds(void);
  ~MutexAdapter3ds();
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

	nn::os::Mutex mMutex;

};

GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif  //__GFL_MUTEX_ADAPTER_3DS_H__

#endif  // GF_PLATFORM_CTR
