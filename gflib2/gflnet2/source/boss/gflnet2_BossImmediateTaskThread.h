#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossImmediateTaskThread.h
 * @brief  BOSS即時実行タスクスレッド
 * @author Shin Kosaka
 * @date   2012.8.09
 */
//=============================================================================

#ifndef __GFLNET_BOSS_IMMEDIATE_TASK_THREAD_H__
#define __GFLNET_BOSS_IMMEDIATE_TASK_THREAD_H__

#include <nn.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)


class BossImmediateTaskThread : public gfl2::thread::ThreadInterface
{
GFL_FORBID_COPY_AND_ASSIGN(BossImmediateTaskThread); //コピーコンストラクタ＋代入禁止

private:

  enum
  {
    WORK_BUFFER_SIZE = 1024,
    FILE_PATH_SIZE = 128,
    SLEEP_TIME = 1000 / 60,				//スリープ時間(ms)
    DISCONNECT_CHECK_INTERVAL = 15,
    BOSS_IMMEDIATE_TIME_OUT = 1000 * 60,
  };


public:
  //--------------------------------------------------------------------------------
  /**
  * @brief コンストラクタ
  * @param pHeap           スレッドのスタック領域に使用するヒープ
  * @param stackSize     スレッドのスタック領域のサイズ
  */
  //--------------------------------------------------------------------------------
  BossImmediateTaskThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , const char* pFileName , const char* pTaskName , const char* pBossCode , const char* pRegionURL );

  //--------------------------------------------------------------------------------
  /**
  * @brief デストラクタ
  */
  //--------------------------------------------------------------------------------
  virtual ~BossImmediateTaskThread();

  //--------------------------------------------------------------------------------
  /**
  * @brief スレッド開始
  */
  //--------------------------------------------------------------------------------
  void Start();

  //--------------------------------------------------------------------------------
  /**
  * @brief 成否取得
  */
  //--------------------------------------------------------------------------------
  bool IsSuccess() const;
  //--------------------------------------------------------------------------------
  /**
  * @brief リザルトコード取得
  */
  //--------------------------------------------------------------------------------
  nn::Result GetResult() const;

  //--------------------------------------------------------------------------------
  /**
  * @brief タスクのキャンセル
  */
  //--------------------------------------------------------------------------------
  nn::Result CancelTask();

  //--------------------------------------------------------------------------------
  /**
  * @brief タスクエラー取得
  */
  //--------------------------------------------------------------------------------
  u32 GetErrorCode();

public:

  bool IsAlive( void ){ return m_pThread->IsAlive(); };
  void Kill( void ){ m_pThread->Kill(); };
  
private:

  virtual void Function( void );

private:

  bool InitializeInternal();

private:

  char*				m_pURLBuffer;//!<URLバッファ
  bool m_isSuccess;
  nn::Result m_result;

  nn::boss::TaskPolicy m_immediatePolicy;			//!<即時実行タスク用ポリシー
  nn::boss::NsaDownloadAction m_immediateAction;	//!<即時実行タスク用アクション
  nn::boss::FgOnlyTask m_immediateFgOnlyTask;		//!<即時実行タスク
  nn::boss::Task m_task;

  nn::boss::TaskResultCode m_resultCode;//<!リザルトコード
  const char* m_pTaskName;
  u32 m_disconnectCheckCount;

  nn::os::LightEvent m_cancelEvent;
  u64 m_startTime;

  gfl2::thread::Thread* m_pThread;
};

GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif //__GFLNET_BOSS_IMMEDIATE_TASK_THREAD_H__

#endif
