#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossTaskRegisterThread.cpp
 * @brief  BOSSバックグラウンドタスク登録スレッド
 * @author Shin Kosaka
 * @date   2012.8.09
 */
//=============================================================================
#include "gflnet2_BossTaskRegisterThread.h"

#include <nn/boss.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 */
//--------------------------------------------------------------------------------
BossTaskRegisterThread::BossTaskRegisterThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , const gflnet2::boss::BossManager::TASK_INIT_PARAM& taskInitParam )
: m_pURLBuffer(NULL)//!<URLバッファ
, m_isSuccess(false)
, m_taskInterval(taskInitParam.interval)
, m_taskCount(taskInitParam.count)
, m_result()
, m_taskPolicy()	//!<タスク用ポリシー
, m_taskAction()	//!<タスク用アクション
, m_task()			//!<タスク
, m_pThread(NULL)
{
  
 //タスクのターゲットURLフォーマット(nsa/[BOSSコード]/[タスク名]/[ファイル名])
 //現在の種別はNSAアーカイブのみ
  const char* pBossCode = GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->GetBossCode();
  const char* TASK_TARGET_URL_FORMAT = "https://npdl.cdn.nintendowifi.net/p01/nsa/%s/%s/%s";

  m_pURLBuffer = GFL_NEW_ARRAY( pHeap ) char[nn::boss::MAX_URL_LENGTH];

  memset( m_pURLBuffer , 0 , sizeof(nn::boss::MAX_URL_LENGTH) );
  sprintf( m_pURLBuffer , TASK_TARGET_URL_FORMAT , pBossCode , taskInitParam.pTaskName , taskInitParam.pFileName );

  strncpy( m_taskName , taskInitParam.pTaskName , nn::boss::TASK_ID_LENGTH - 1 );
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
BossTaskRegisterThread::~BossTaskRegisterThread()
{
  GFL_SAFE_DELETE_ARRAY( m_pURLBuffer );
}


//--------------------------------------------------------------------------------
/**
* @brief スレッド開始
*/
//--------------------------------------------------------------------------------
void BossTaskRegisterThread::Start()
{
  m_pThread->Start();
}


//--------------------------------------------------------------------------------
/**
* @brief 成否取得
*/
//--------------------------------------------------------------------------------
bool BossTaskRegisterThread::IsSuccess() const
{
  return m_isSuccess;
}

//--------------------------------------------------------------------------------
/**
* @brief リザルトコード取得
*/
//--------------------------------------------------------------------------------
nn::Result BossTaskRegisterThread::GetResult() const
{
  return m_result;
}

//--------------------------------------------------------------------------------
/**
* @brief 実行タスク名取得
*/
//--------------------------------------------------------------------------------
const char* BossTaskRegisterThread::GetTaskName() const
{
  return m_taskName;
}




//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
void BossTaskRegisterThread::Function( void )
{

  m_isSuccess = false;

  //同じIDのタスクが残っている状況を考慮して、タスクを削除。
  {
    nn::boss::Task deleteTargetTask;
    nn::Result result = deleteTargetTask.Initialize(m_taskName);
    if( !result.IsSuccess() )
    {
      GFL_PRINT("BossTaskRegisterThread::Main : error deleteTargetTask.Initialize failed[%s]\n",m_taskName);
    }
    else
    {
      result = deleteTargetTask.Cancel();//タスクが動作中ですと、UnregisterTaskは失敗します。確実に削除したい場合は、Cancelの実施後にUnregisterTaskを実行してください。
      if( !result.IsSuccess() )
      {
        GFL_PRINT("BossTaskRegisterThread::Main : deleteTargetTask.Cancel failed[%s]\n",m_taskName);
      }

      result = nn::boss::UnregisterTask(&deleteTargetTask);
      if( !result.IsSuccess() )
      {
        GFL_PRINT("BossTaskRegisterThread::Main : UnregisterTask is failed[%s]\n",m_taskName);
      }
    }
  }


  //NADLタスクの登録と実行
  {
    //NADLタスクの登録
    m_result = m_taskPolicy.Initialize( m_taskInterval , m_taskCount );
    //nn::Result result = dPolicy.InitializeWithSecInterval(NADL_TASK_EXECUTE_TIME, NADL_TASK_EXECUTE_COUNT);//テスト用に実行インターバルを秒単位に指定することも可能。
    if( !m_result.IsSuccess() )
    {
      GFL_PRINT("BossTaskRegisterThread::Main : error m_taskPolicy.Initialize is failed[%d][%d]\n",m_taskInterval , m_taskCount);
      m_pThread->Kill();
      return;
    }

    m_result = m_taskAction.Initialize(m_pURLBuffer);
    if( !m_result.IsSuccess() )
    {
      GFL_PRINT("BossTaskRegisterThread::Main : error m_taskAction.Initialize is failed[%s]\n",m_pURLBuffer );
      m_pThread->Kill();
      return;
    }

    //result = dAction.SetApInfo( nn::boss::APINFOTYPE_APGROUP|nn::boss::APINFOTYPE_APAREA|nn::boss::APINFOTYPE_AP );//必要に応じて、「HTTPクエリーへのAP情報付与」用設定を行う。
    //result = dAction.AddHeaderField(pLabel, pValue);//必要に応じて、独自のHTTPリクエストヘッダの設定を行う。

    m_result = m_task.Initialize(m_taskName);
    if( !m_result.IsSuccess() )
    {
      GFL_PRINT("BossTaskRegisterThread::Main : error m_task.Initialize is failed[%s]\n",m_taskName );
      m_pThread->Kill();
      return;
    }

    //タスクの登録。
    m_result = nn::boss::RegisterTask( &m_task , &m_taskPolicy , &m_taskAction );
    if( m_result == nn::boss::ResultTaskIdAlreadyExist() )
    {
        GFL_PRINT("BossTaskRegisterThread::Main : RegisterTask failed. Same name task(%s) already registered.If regist again, need to UnregisterTask before.\n", m_taskName);
    }
    else if( !m_result.IsSuccess() )
    {
      GFL_PRINT("BossTaskRegisterThread::Main : error RegisterTask is failed[%s]\n",m_taskName );
      m_pThread->Kill();
      return;
    }


    //タスク実行
    GFL_PRINT("BossTaskRegisterThread::Main : startBgImmediate[%d][%d][%s][%s]\n" , m_taskInterval , m_taskCount , m_pURLBuffer,m_taskName);
    m_result = m_task.StartBgImmediate();//初回は即実行される
    if( !m_result.IsSuccess() )
    {
      GFL_PRINT("BossTaskRegisterThread::Main : error StartBgImmediate is failed[%s]\n",m_taskName);
      m_pThread->Kill();
      return;
    }
  }

  //登録＆実行成功
  m_isSuccess = true;

  m_pThread->Kill();
}



GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif
