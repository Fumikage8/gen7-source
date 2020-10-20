#ifndef __GFL_MUTEX_ADAPTER_H__
#define __GFL_MUTEX_ADAPTER_H__
//=============================================================================
/**
 * @file	 gfl_mutex_adapter.h
 * @brief	 ミューテックス アダプタ このクラスを継承して機種依存実装を行うこと
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(GF_PLATFORM_WIN32)
#include <mutex>
#elif defined(GF_PLATFORM_CTR)
#include "nn/os.h"
#endif


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


class MutexAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(MutexAdapter);

public:
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ ここではなにもしません
   */
  //-----------------------------------------------------------------------
  MutexAdapter(void){};
  //-----------------------------------------------------------------------
  /**
   * @brief ロック ここ以降他のスレッドからは割り込まれなくなる
   */
  //-----------------------------------------------------------------------
  virtual void lock(void) = 0;
  //-----------------------------------------------------------------------
  /**
   * @brief  ロックできるのであればロックする
   * @return falseロックできなかった  true ロックできた
   */
  //-----------------------------------------------------------------------
  virtual bool tryLock(void) = 0;
  //-----------------------------------------------------------------------
  /**
   * @brief アンロック  ここ以降他のスレッドの割り込みを許可する
   */
  //-----------------------------------------------------------------------
  virtual void unlock(void) = 0;

};



GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif//__GFL_MUTEX_ADAPTER_H__
