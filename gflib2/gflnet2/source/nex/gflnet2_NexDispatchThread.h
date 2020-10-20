#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_NexDispatchThread.h
 * @author suzuki_hisanao
 */
//=============================================================================

#ifndef __GFLNET_NEX_DISPATCH_THREAD_H__
#define __GFLNET_NEX_DISPATCH_THREAD_H__
#pragma once

#include <nn.h>
#include <nn/ngc.h>

#include <gflnet2/include/nex/gflnet2_NexManager.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

namespace gflnet2 {
namespace nex {



/**
 * @brief  NEXのディスパッチ用スレッドクラス。<p>
 *         NEXのディスパッチ処理を非同期に行う為に準備されたクラスです。
 *
 * @note   本スレッドをアプリケーションスレッドと並列で実行させる場合は、排他管理を厳密に行ってください。
 */
class NexDispatchThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(NexDispatchThread); //コピーコンストラクタ＋代入禁止

private:
  NexManager*             m_nexMgr;          //!< NEXマネージャ
  bool                    m_isLoop;          //!< ループ
  nn::os::CriticalSection m_criticalSection; //!< クリティカルセクション
  gfl2::thread::Thread* m_pThread;

public:
  //--------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param[in] nexMgr         NexManagerへのポインタ
   * @param[in] heap           スレッドのスタック領域に使用するヒープ
   * @param[in] stack_size     スレッドのスタック領域のサイズ
   */
  //--------------------------------------------------------------------------------
  NexDispatchThread( NexManager* nexMgr,  gfl2::heap::HeapBase* heap, u32 stack_size );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~NexDispatchThread();

public:
  //--------------------------------------------------------------------------------
  /**
   * @brief Dispatch処理の停止
   */
  //--------------------------------------------------------------------------------
  void StopDispatch( void );

public:

  void Start(void){ m_pThread->Start(); };
  void Kill(void){ m_pThread->Kill(); };
  bool IsAlive(void){ return m_pThread->IsAlive(); };

private:
  bool IsLoop( void );

  virtual void Function( void );
};




} // nex
} // gflnet


#endif  // __GFLNET_NEX_DISPATCH_THREAD_H__

#endif
