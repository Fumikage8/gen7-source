//===================================================================
/**
 *	@file	    ThreadSampleEvent.h
 *	@brief    スレッドサンプルイベント
 *	@author	  k.ohno
 *	@date		  2015.3.13
 *
 */
//===================================================================
#if PM_DEBUG

#ifndef __THREAD_SAMPLE_EVENT_H__
#define __THREAD_SAMPLE_EVENT_H__
#pragma once

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"


#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

#include "thread/include/gfl2_Scopedlock.h"
#include "thread/include/gfl2_Thread.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ThreadTest )

class Proc;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
 */
//-----------------------------------------------------------------------------


//  gfl2::thread::CriticalSection m_criticalSection;
//  gfl2::thread::ScopedLock lock(m_criticalSection);


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------

class ThreadSampleEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( ThreadSampleEvent ); // コピーを禁止








public:
  debug::launcher::LauncherProc::CallParam m_pLauncherParam;





  // このイベントを起動する関数
  static void StartEvent( GameSys::GameManager* p_gameman );

  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap       workをとるためのメモリクラス
   */
  //--------------------------------------------------------------
  ThreadSampleEvent( gfl2::heap::HeapBase* heap);
  virtual ~ThreadSampleEvent();

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk( GameSys::GameManager* /*gmgr*/ );

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager* /*gmgr*/ );

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager* /*gmgr*/ );



private:
  u32   mSeq;
};


GFL_NAMESPACE_END( ThreadTest )
GFL_NAMESPACE_END( Test )

#endif // __THREAD_SAMPLE_EVENT_H__

#endif //PM_DEBUG
