#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossImmediateTaskThread.cpp
 * @brief  BOSS即時実行タスクスレッド
 * @author Shin Kosaka
 * @date   2012.8.09
 */
//=============================================================================
#include "gflnet2_BossImmediateTaskThread.h"

#include <nn/boss.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)

//タスクのターゲットURLフォーマット(nsa/[BOSSコード]/[タスク名]/[ファイル名])
//現在の種別はNSAアーカイブのみ
static const char* TASK_TARGET_URL_FORMAT = "https://npdl.cdn.nintendowifi.net/p01/nsa/%s/%s/%s/%s";
static const char* TASK_TARGET_URL_FORMAT_NO_REGION = "https://npdl.cdn.nintendowifi.net/p01/nsa/%s/%s/%s";




//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 */
//--------------------------------------------------------------------------------
BossImmediateTaskThread::BossImmediateTaskThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , const char* pFileName , const char* pTaskName , const char* pBossCode , const char* pRegionURL )
: m_pURLBuffer(NULL)
, m_isSuccess(false)
, m_result( nn::ResultSuccess() )
, m_immediatePolicy()			//!<即時実行タスク用ポリシー
, m_immediateAction()			//!<即時実行タスク用アクション
, m_immediateFgOnlyTask()		//!<即時実行タスク
, m_task()
, m_resultCode(nn::boss::TASK_SUCCESS)//!<エラーコード
, m_pTaskName(pTaskName)
, m_disconnectCheckCount(0)
, m_startTime(0)
, m_pThread(NULL)
{
  m_pURLBuffer = GFL_NEW_ARRAY( pHeap ) char[nn::boss::MAX_URL_LENGTH];



  ::std::memset( m_pURLBuffer , 0 , sizeof(nn::boss::MAX_URL_LENGTH) );
  if( m_pTaskName == NULL )
  {
    if( pRegionURL )
    {
      ::std::sprintf( m_pURLBuffer , TASK_TARGET_URL_FORMAT , pBossCode , nn::boss::FG_ONLY_TASK_ID ,pRegionURL, pFileName );
    }
    else
    {
      ::std::sprintf( m_pURLBuffer , TASK_TARGET_URL_FORMAT_NO_REGION , pBossCode , nn::boss::FG_ONLY_TASK_ID , pFileName );
    }
  }
  else
  {
    if( pRegionURL )
    {
      ::std::sprintf( m_pURLBuffer , TASK_TARGET_URL_FORMAT , pBossCode , pTaskName ,pRegionURL, pFileName );
    }
    else
    {
      ::std::sprintf( m_pURLBuffer , TASK_TARGET_URL_FORMAT_NO_REGION , pBossCode , pTaskName , pFileName );
    }
  }

  m_cancelEvent.Initialize(true);

  m_pThread = GFL_NEW(pHeap) gfl2::thread::Thread(this, pHeap, true, stackSize);
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
BossImmediateTaskThread::~BossImmediateTaskThread()
{
  if( m_pTaskName == NULL )
  {
    nn::Result result = nn::boss::UnregisterTask( &m_immediateFgOnlyTask );
    if( !result.IsSuccess() )
    {
      GFL_PRINT("BossImmediateTaskThread:: warning Finalize:UnregisterTask failed\n");
    }
  }

  m_cancelEvent.Finalize();
  GFL_SAFE_DELETE_ARRAY( m_pURLBuffer );

  GFL_SAFE_DELETE( m_pThread );
}

//--------------------------------------------------------------------------------
/**
* @brief スレッド開始
*/
//--------------------------------------------------------------------------------
void BossImmediateTaskThread::Start()
{
  m_pThread->Start();
}

//--------------------------------------------------------------------------------
/**
* @brief 成否取得
*/
//--------------------------------------------------------------------------------
bool BossImmediateTaskThread::IsSuccess() const
{
  return m_isSuccess;
}

//--------------------------------------------------------------------------------
/**
* @brief リザルトコード取得
*/
//--------------------------------------------------------------------------------
nn::Result BossImmediateTaskThread::GetResult() const
{
  return m_result;
}



//--------------------------------------------------------------------------------
/**
* @brief タスクのキャンセル
*/
//--------------------------------------------------------------------------------
nn::Result BossImmediateTaskThread::CancelTask()
{
  GFL_PRINT("BossImmediateTaskThread::Initialize : CancelTask\n");
  m_cancelEvent.Signal();

  return nn::ResultSuccess();
}

//--------------------------------------------------------------------------------
/**
* @brief タスクエラー取得
*/
//--------------------------------------------------------------------------------
u32 BossImmediateTaskThread::GetErrorCode()
{
  //return m_immediateFgOnlyTask.GetResult();
  return m_resultCode;
}

//--------------------------------------------------------------------------------
/**
* @brief 初期化
*/
//--------------------------------------------------------------------------------
bool BossImmediateTaskThread::InitializeInternal()
{
  //Initializeから移植↓
  GFL_PRINT("BossImmediateTaskThread::Initialize:m_immediateAction.Initialize([%s])\n",m_pURLBuffer);

  nn::Result result = m_immediateAction.Initialize( m_pURLBuffer );
  if( !result.IsSuccess() )
  {//失敗
    GFL_PRINT("BossImmediateTaskThread::Initialize : error m_immediateAction.Initialize(m_pURLBuffer) failed[%s]\n",m_pURLBuffer);
    m_result = result;

    return false;
  }

  result = m_immediateAction.SetApInfo( nn::boss::APINFOTYPE_AP );//「HTTPクエリーへのAP情報付与」用設定を行う。
  if( !result.IsSuccess() )
  {
    GFL_PRINT("BossImmediateTaskThread::Initialize : error m_immediateAction.SetApInfo( nn::boss::APINFOTYPE_AP ) failed\n");
    m_result = result;

    return false;
  }
  //result = dAction.AddHeaderField(pLabel, pValue);//必要に応じて、独自のHTTPリクエストヘッダの設定を行う。

  // 即時実行専用タスク
  // 即時実行専用タスクは専用のタスク ID "FGONLYT" を使用するので、
  // Initialize でタスク ID を指定する必要はありません。
  //nn::boss::FgOnlyTask dFgOnlyTask;

  nn::boss::Task* pTask = NULL;
  if( m_pTaskName == NULL )
  {
    pTask = &m_immediateFgOnlyTask;
    m_immediatePolicy.Initialize( 1 , 1 );
    // タスクを即時実行専用タスクとして登録。
    result = nn::boss::RegisterImmediateTask(pTask, &m_immediateAction, &m_immediatePolicy);
    if( !result.IsSuccess() )
    {//失敗
      GFL_PRINT("BossImmediateTaskThread::Initialize : error nn::boss::RegisterImmediateTask(&dFgOnlyTask, &dAction, &dPolicy) failed\n");
      m_result = result;

      return false;
    }
  }
  else
  {
    GFL_PRINT("BossImmediateTaskThread::Initialize:m_task.Initialize([%s])\n",m_pTaskName);
    m_task.Initialize( m_pTaskName );
    pTask = &m_task;
  }

  // タスクの即時実行
  // FgOnlyTask は FG 即時実行しかできません。
  GFL_PRINT("BossImmediateTaskThread::Initialize : startImmediate\n");
  result = pTask->StartImmediate();
  if( !result.IsSuccess() )
  {//失敗
    GFL_PRINT("BossImmediateTaskThread::Initialize : error dFgOnlyTask.StartImmediate() failed\n");
    m_result = result;

    return false;
  }

  m_startTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();

  return true;
}

//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
void BossImmediateTaskThread::Function( void )
{
  if( m_startTime == 0 )
  {//開始前
    if( InitializeInternal() == false )
    {
      m_pThread->Kill();
      return;
    }
  }

  nn::boss::Task* pTask = NULL;
  if( m_pTaskName == NULL )
  {
    pTask = &m_immediateFgOnlyTask;
  }
  else
  {
    pTask = &m_task;
  }

  if( m_disconnectCheckCount++ > DISCONNECT_CHECK_INTERVAL || m_cancelEvent.TryWait() )
  {
    u64 currentTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
    bool isTimeout = ( currentTime - m_startTime ) > BOSS_IMMEDIATE_TIME_OUT;
    if( m_cancelEvent.TryWait() || nn::ac::IsConnected() == false || isTimeout )
    {//切断orキャンセル
      GFL_PRINT("BossImmediateTaskThread::Main : Cancel\n");
      pTask->Cancel();
      if( !m_cancelEvent.TryWait() )
      {//タイムアウトかAC切断
        m_result = nn::boss::ResultWaitFinishTimeout();
      }
    }
    m_disconnectCheckCount = 0;
  }

  nn::boss::TaskStateCode immediateState = pTask->GetState();
  //GFL_PRINT("BossManager::Update : m_immediateFgOnlyTask.GetState() == %d\n",immediateState);
  switch( immediateState )
  {
  case nn::boss::TASK_STOPPED:// 0 タスク停止中
    {
      GFL_PRINT("BossImmediateTaskThread::Main( void ) :  m_immediateFgOnlyTask.GetState() == TASK_STOPPED\n");
      m_pThread->Kill();
    }
  break;
  case nn::boss::TASK_WAITING:// 2 タスクの実行待ち状態
  case nn::boss::TASK_RUNNING:// 3 タスク実行中
  case nn::boss::TASK_PAUSED:// 4 タスクポーズ中
    {

    }
    break;
  case nn::boss::TASK_RETRY:// 8 タスク実行中の通信エラーによるリトライ状態
    {
      GFL_PRINT("BossImmediateTaskThread::Main( void ) :  m_immediateFgOnlyTask.GetState() == %d\n",immediateState);
    }
  break;
  case nn::boss::TASK_DONE:// 6 タスク実行完了状態
    {
      GFL_PRINT("BossImmediateTaskThread::Main( void ) : success\n");
      m_isSuccess = true;
      m_pThread->Kill();
    }
  break;
  case nn::boss::TASK_ERROR:// 7 タスク実行の異常終了
    {
      GFL_PRINT("BossImmediateTaskThread::Main( void ) : nn::boss::TASK_ERROR[%d]\n" , pTask->GetResult() );
      m_resultCode = pTask->GetResult();

      m_pThread->Kill();
    }
  break;
  case nn::boss::TASK_WAITING_TIMER:// 1 タスクの開始時刻待ち状態
  case nn::boss::TASK_REGISTERED:// 5 タスクを登録した状態
  case nn::boss::GET_TASK_STATE_ERROR:// 9 タスク状態取得エラー
  case nn::boss::STATUS_MAX_VALUE:// （内部処理用の列挙子です。アプリケーションからは使用できません。）
  default:
    {//ここには入らないはず
      GFL_PRINT("BossImmediateTaskThread::Main( void ) : warning m_immediateFgOnlyTask.GetState() == %d\n",immediateState);
      //_NotifyOnBossTaskError( FG_ONLY_TASK_NAME , m_immediateFgOnlyTask.GetResult() );
      m_pThread->Kill();
    }
  break;
  }

  gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_TIME );
}




GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif
