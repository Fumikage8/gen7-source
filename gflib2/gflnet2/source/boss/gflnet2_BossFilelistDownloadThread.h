#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossFilelistDownloadThread.h
 * @brief  BOSSサーバファイルリストダウンロードスレッド
 * @author Shin Kosaka
 * @date   2012.8.03
 */
//=============================================================================

#ifndef __GFLNET_WIRELESS_boss_RECV_THREAD_H__
#define __GFLNET_WIRELESS_boss_RECV_THREAD_H__

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


class BossFilelistDownloadThread : public gfl2::thread::ThreadInterface
{
GFL_FORBID_COPY_AND_ASSIGN(BossFilelistDownloadThread); //コピーコンストラクタ＋代入禁止

private:

  enum
  {
    WORK_BUFFER_SIZE = 1024,
    FILE_PATH_SIZE = 128,
    ATTRIBUTE_SIZE = 10,//アトリビュート文字数(9文字)
  };


public:
  //--------------------------------------------------------------------------------
  /**
  * @brief コンストラクタ
  * @param pHeap           スレッドのスタック領域に使用するヒープ
  * @param stackSize     スレッドのスタック領域のサイズ
  */
  //--------------------------------------------------------------------------------
  BossFilelistDownloadThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , nn::boss::NsaList::NsaInformation* pNsaListInfo , u32 nsaListLength , const char* pTaskName , const BossManager::BOSS_SEARCH_ATTRIBUTES* pAttributes = NULL );

  //--------------------------------------------------------------------------------
  /**
  * @brief デストラクタ
  */
  //--------------------------------------------------------------------------------
  virtual ~BossFilelistDownloadThread();

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

  //--------------------------------------------------------------------------------
  /**
  * @brief ファイル数取得
  */
  //--------------------------------------------------------------------------------
  u32 GetFileNum() const;

  //--------------------------------------------------------------------------------
  /**
  * @brief ダウンロードのキャンセル
  */
  //--------------------------------------------------------------------------------
  nn::Result CancelDownload();

public:
  
  u32 GetErrorCode( void ){ return m_errorCode; };

public:

  bool IsAlive( void ){ return m_pThread->IsAlive(); };
  void Kill( void ){ m_pThread->Kill(); };

private:

  virtual void Function( void );

private:



private:

  nn::boss::NsaList::NsaInformation* m_pNsaListInfo;
  u32 m_nsaListLength;
  bool m_isSuccess;
  u32 m_fileNum;
  nn::boss::NsaList* m_pNsaList;
  nn::Result m_result;
  char m_taskName[nn::boss::TASK_ID_LENGTH];
  u8 m_workBuffer[WORK_BUFFER_SIZE];
  char m_filePath[FILE_PATH_SIZE];

  char m_attribute1[ATTRIBUTE_SIZE];
  char m_attribute2[ATTRIBUTE_SIZE];
  char m_attribute3[ATTRIBUTE_SIZE];

  gfl2::thread::Thread* m_pThread;

  u32 m_errorCode;
};

GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif //__GFLNET_WIRELESS_boss_RECV_THREAD_H__

#endif
