#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossTaskRegisterThread.h
 * @brief  BOSSバックグラウンドタスク登録スレッド
 * @author Shin Kosaka
 * @date   2012.8.09
 */
//=============================================================================

#ifndef __GFLNET_BOSS_TASK_REGISTER_THREAD_H__
#define __GFLNET_BOSS_TASK_REGISTER_THREAD_H__

#include <nn.h>


#include <gflnet2/include/boss/gflnet2_BossManager.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)


class BossTaskRegisterThread : public gfl2::thread::ThreadInterface
{
GFL_FORBID_COPY_AND_ASSIGN(BossTaskRegisterThread); //コピーコンストラクタ＋代入禁止
	
private:

	enum
	{
		WORK_BUFFER_SIZE = 1024,
		FILE_PATH_SIZE = 128,
	};


public:
	//--------------------------------------------------------------------------------
	/**
	* @brief コンストラクタ
	* @param pHeap           スレッドのスタック領域に使用するヒープ
	* @param stackSize     スレッドのスタック領域のサイズ
	*/
	//--------------------------------------------------------------------------------
	BossTaskRegisterThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , const gflnet2::boss::BossManager::TASK_INIT_PARAM& taskInitParam );

	//--------------------------------------------------------------------------------
	/**
	* @brief デストラクタ
	*/
	//--------------------------------------------------------------------------------
	virtual ~BossTaskRegisterThread();

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
	* @brief 実行タスク名取得
	*/
	//--------------------------------------------------------------------------------
	const char* GetTaskName() const;


public:
  

  bool IsAlive( void ){ return m_pThread->IsAlive(); };
  void Kill( void ){ m_pThread->Kill(); };

private:

	virtual void Function( void );

private:
	

	
private:
	
	char* m_pURLBuffer;
	bool m_isSuccess;
	u32 m_taskInterval;
	u32 m_taskCount;
	nn::Result m_result;
	nn::boss::TaskPolicy m_taskPolicy;			//!<タスク用ポリシー
	nn::boss::NsaDownloadAction m_taskAction;	//!<タスク用アクション
	nn::boss::Task m_task;		//!<タスク
	char m_taskName[nn::boss::TASK_ID_LENGTH];

  gfl2::thread::Thread* m_pThread;
};

GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif //__GFLNET_BOSS_TASK_REGISTER_THREAD_H__

#endif
