#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossCheckNewArrivalDataThread.h
 * @brief  新着データ取得スレッド
 * @author Shin Kosaka
 * @date   2012.8.08
 */
//=============================================================================

#ifndef __GFLNET_BOSS_CHECK_NEW_ARRIVAL_DATA_THREAD_H__
#define __GFLNET_BOSS_CHECK_NEW_ARRIVAL_DATA_THREAD_H__

#include <nn.h>
// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)


class BossCheckNewArrivalDataThread : public gfl2::thread::ThreadInterface
{
GFL_FORBID_COPY_AND_ASSIGN(BossCheckNewArrivalDataThread); //コピーコンストラクタ＋代入禁止
	
private:
	
	enum
	{
		SLEEP_TIME = 1000 / 60,				//スリープ時間(ms)
	};
	


public:
	//--------------------------------------------------------------------------------
	/**
	* @brief コンストラクタ
	* @param pHeap           スレッドのスタック領域に使用するヒープ
	* @param stackSize     スレッドのスタック領域のサイズ
	*/
	//--------------------------------------------------------------------------------
	BossCheckNewArrivalDataThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , u32* pSerialIDList , u32 idListSize );

	//--------------------------------------------------------------------------------
	/**
	* @brief デストラクタ
	*/
	//--------------------------------------------------------------------------------
	virtual ~BossCheckNewArrivalDataThread();

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
	* @brief ID数取得
	*/
	//--------------------------------------------------------------------------------
	u32 GetIdNum();
	
	//--------------------------------------------------------------------------------
	/**
	* @brief ID取得
	*/
	//--------------------------------------------------------------------------------
	u32 GetNsDataId( u32 index );

public:
  

  bool IsAlive( void ){ return m_pThread->IsAlive(); };
  void Kill( void ){ m_pThread->Kill(); };
  
private:


	virtual void Function( void );
	
private:
	gfl2::heap::HeapBase* m_pHeap;
	bool m_isSuccess;
	//u32* m_pSerialIDListBuffer;
	//u32 m_idListBufferSize;
	nn::Result m_result;
	nn::boss::NsDataIdList m_serialIdList;

  gfl2::thread::Thread* m_pThread;
};

GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif //__GFLNET_BOSS_CHECK_NEW_ARRIVAL_DATA_THREAD_H__

#endif
