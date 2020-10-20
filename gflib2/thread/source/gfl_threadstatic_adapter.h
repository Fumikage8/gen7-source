#ifndef __GFL_THREADSTATIC_ADAPTER_H__
#define __GFL_THREADSTATIC_ADAPTER_H__
//=============================================================================
/**
 * @file	 gfl_threadstatic_adapter.h
 * @brief	 このクラスを継承して機種依存実装を行うこと
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)




class ThreadStaticAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(ThreadStaticAdapter);

public:

  ThreadStaticAdapter(void){};
  virtual ~ThreadStaticAdapter(){};

  //-----------------------------------------------------------------------
  /**
   * @brief イールド 処理系に再スケジュールの機会を与える。
   */
  //-----------------------------------------------------------------------
  virtual void CurrentYield( void ) = 0;
  //-------------------------------------------------------------------------
  /**
   * @brief カレントスレッドを指定時間停止させる
   * @param milliSeconds スレッドを停止させる時間（ミリ秒）
   */
  //-------------------------------------------------------------------------
  virtual void CurrentSleep( s32 milliSeconds ) = 0;
  //-------------------------------------------------------------------------
  /**
   * @brief カレントスレッドを指定時間停止させる
   *
   * @param microSeconds スレッドを停止させる時間（マイクロ秒）
   */
  //-------------------------------------------------------------------------
  virtual void CurrentSleepMicro( s32 microSeconds ) = 0;

};





GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif __GFL_THREADSTATIC_ADAPTER_H__
