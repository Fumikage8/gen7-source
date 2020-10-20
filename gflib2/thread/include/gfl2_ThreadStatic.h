#ifndef __GFL2_THREADSTATIC_H__
#define __GFL2_THREADSTATIC_H__
//=============================================================================
/**
 * @file	 gfl2_threadstatic.h
 * @brief	 スレッドのスタティックな関数へ実装を隠蔽したままアクセスするためのクラス
 * @author k.ohno
 * @date	 2014.12.24
 */
//=============================================================================
#pragma once


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include "thread/include/gfl2_Event.h"



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)

class ThreadStatic{
  GFL_FORBID_COPY_AND_ASSIGN(ThreadStatic);


public:
  //-----------------------------------------------------------------------
  /**
   * @brief イールド 処理系に再スケジュールの機会を与える。
   */
  //-----------------------------------------------------------------------
  static void CurrentYield( void );
  //-------------------------------------------------------------------------
  /**
   * @brief カレントスレッドを指定時間停止させる
   *
   * @param milliSeconds スレッドを停止させる時間（ミリ秒）
   */
  //-------------------------------------------------------------------------
  static void CurrentSleep( s32 milliSeconds );
  //-------------------------------------------------------------------------
  /**
   * @brief カレントスレッドを指定時間停止させる
   *
   * @param microSeconds スレッドを停止させる時間（マイクロ秒）
   */
  //-------------------------------------------------------------------------
  static void CurrentSleepMicro( s32 microSeconds );
  //-------------------------------------------------------------------------
  /**
   * @brief スレッドIDがメインスレッドかチェック
   * @attention nijiではCTR版のみ実装、それ以外ではアサートになります
   *
   * @param thread_id スレッドID
   *
   * @return 結果
   */
  //-------------------------------------------------------------------------
  static b32 IsMainThread( bit32 thread_id );
  //-------------------------------------------------------------------------
  /**
  * @brief 現在のスレッドIDを取得
  * @attention nijiではCTR版のみ実装、それ以外ではアサートになります
  *
  * @return スレッドID
  */
  //-------------------------------------------------------------------------
  static bit32 GetCurrentId( void );

};



GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif __GFL2_THREADSTATIC_H__
