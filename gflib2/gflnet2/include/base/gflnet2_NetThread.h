#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETTHREAD_H__
#define __GFLNET_NETTHREAD_H__
//=============================================================================
/**
 * @file	 gflnet_NetThread.h
 * @brief	 通信のスレッド 継承する
 * @author k.ohno
 * @date	 2012.8.2
 */
//=============================================================================
#pragma once

#include <gflnet2/include/gflnet2_NetworkDefine.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)


class NetThread : public gfl2::thread::ThreadInterface  {
  GFL_FORBID_COPY_AND_ASSIGN( NetThread ); //コピーコンストラクタ＋代入禁止

private:
  bool mbKillThread;  //スレッドを終了する時につかう
protected:
  gfl2::thread::Thread* m_pThread;

public:
  NetThread(gfl2::heap::HeapBase* pHeap, size_t size=0) 
    : mbKillThread(false),m_pThread(NULL)
  {
    m_pThread = GFL_NEW(pHeap) gfl2::thread::Thread(this, pHeap, true , GFLNET_NETBASE_STACK_SIZE );
  };
  virtual ~NetThread()
  {
    GFL_SAFE_DELETE( m_pThread );
  };

  //------------------------------------------------------------------
  /**
   * @brief	  このスレッドを終了する
   */
  //------------------------------------------------------------------
  void KillNetThread(void){mbKillThread=true;};


  //------------------------------------------------------------------
  /**
   * @brief	  このスレッドを終了する
   */
  //------------------------------------------------------------------
  bool GetKillNetThread(void){ return mbKillThread; };



  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  virtual void Update(void){};


  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  virtual void UpdateBefore(void){};

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  virtual void UpdateAfter(void){};

  bool IsAlive( void ){ return m_pThread->IsAlive(); };
  void Start( int priority ){ m_pThread->Start(priority); };

protected:
  void Kill( void ){ m_pThread->Kill(); };

};

GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)


#endif //__GFLNET_NETTHREAD_H__

#endif
