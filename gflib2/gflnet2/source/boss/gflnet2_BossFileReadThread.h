#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossFileReadThread.h
 * @brief  BOSSファイル読み込みスレッド
 * @author Shin Kosaka
 * @date   2012.8.06
 */
//=============================================================================

#ifndef __GFLNET_BOSS_FILE_READ_THREAD_H__
#define __GFLNET_BOSS_FILE_READ_THREAD_H__

#include <nn.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)


class BossFileReadThread : public gfl2::thread::ThreadInterface
{
GFL_FORBID_COPY_AND_ASSIGN(BossFileReadThread); //コピーコンストラクタ＋代入禁止

private:

  /*
  enum
  {
    WORK_BUFFER_SIZE = 1024,
  };
  */


public:
  //--------------------------------------------------------------------------------
  /**
  * @brief コンストラクタ
  * @param pHeap           スレッドのスタック領域に使用するヒープ
  * @param stackSize     スレッドのスタック領域のサイズ
  */
  //--------------------------------------------------------------------------------
  BossFileReadThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , const u32 serialId );

  //--------------------------------------------------------------------------------
  /**
  * @brief デストラクタ
  */
  //--------------------------------------------------------------------------------
  virtual ~BossFileReadThread();

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
  * @brief データヘッダ取得
  */
  //--------------------------------------------------------------------------------
  const nn::boss::NsDataHeaderInfo* GetDataHeader() const;

  //--------------------------------------------------------------------------------
  /**
  * @brief データ取得
  */
  //--------------------------------------------------------------------------------
  const void* GetDataBody() const;

  //--------------------------------------------------------------------------------
  /**
  * @brief データサイズ取得
  */
  //--------------------------------------------------------------------------------
  size_t GetReadSize() const;

  //--------------------------------------------------------------------------------
  /**
  * @brief コンテンツデータ削除
  */
  //--------------------------------------------------------------------------------
  void DeleteContentsData();

public:
  

  bool IsAlive( void ){ return m_pThread->IsAlive(); };
  void Kill( void ){ m_pThread->Kill(); };

private:


  virtual void Function( void );

private:



private:
  gfl2::heap::HeapBase* m_pHeap;
  bool m_isSuccess;
  const u32 m_serialId;
  void* m_pDataBody;
  size_t m_readSize;
  nn::Result m_result;
  nn::boss::NsData m_contentsData;
  nn::boss::NsDataHeaderInfo m_dataHeader;
  //u8 m_workBuffer[WORK_BUFFER_SIZE];

  gfl2::thread::Thread* m_pThread;
};

GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif //__GFLNET_BOSS_FILE_READ_THREAD_H__

#endif
