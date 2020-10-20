#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_NdmSuspendThread.h
 * @brief  Wifi接続・切断用用スレッド
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NDM_SUSPEND_THREAD_H__
#define __GFLNET_NDM_SUSPEND_THREAD_H__

#pragma once


#include <nn.h>
#include <nn/ndm.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ndm)



class NdmEventListener;
class NdmManager;

class NdmSuspendThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(NdmSuspendThread); //コピーコンストラクタ＋代入禁止


public:
  //--------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param pManager            NdmManagerへのポインタ
   * @param pHeap           スレッドのスタック領域に使用するヒープ
   * @param stackSize     スレッドのスタック領域のサイズ
   * @param pListener       完了通知用リスナ
   */
  //--------------------------------------------------------------------------------
  NdmSuspendThread( NdmManager* pManager,  gfl2::heap::HeapBase* pHeap, u32 stackSize, NdmEventListener* pListener );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~NdmSuspendThread();

public:

  void Start( void ){ m_pThread->Start(); };
  bool IsAlive( void ) { return m_pThread->IsAlive(); };
  void Kill( void ) { m_pThread->Kill(); };

private:

  virtual void Function( void );
	
	
private:
  NdmEventListener*  	m_pListener;     // イベントリスナ
  NdmManager*           m_pManager;      // WifiSystem
  gfl2::thread::Thread* m_pThread;
};




GFL_NAMESPACE_END(ndm)
GFL_NAMESPACE_END(gflnet2)



#endif  // __GFLNET_NDM_SUSPEND_THREAD_H__

#endif
