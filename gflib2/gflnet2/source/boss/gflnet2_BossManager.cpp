#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_BossManager.cpp
 * @brief  BOSS管理マネージャクラス
 * @author shin kosaka
 * @date   2012.8.2
 */
//=============================================================================

#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include <gflnet2/include/ndm/gflnet2_NdmManager.h>
#include "gflnet2_BossFilelistDownloadThread.h"
#include "gflnet2_BossFileReadThread.h"
#include "gflnet2_BossCheckNewArrivalDataThread.h"
#include "gflnet2_BossImmediateTaskThread.h"
#include "gflnet2_BossTaskRegisterThread.h"
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <base/include/gfl2_Singleton.h>
#include <gflnet2/include/socket/gflnet2_SocketSystem.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(gflnet2::boss::BossManager)
template class gfl2::base::SingletonAccessor<gflnet2::boss::BossManager>;
#endif


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)

//即時実行タスクのタスク名（SDKルールで固定）
static const char* FG_ONLY_TASK_NAME = nn::boss::FG_ONLY_TASK_ID;


//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
BossManager::BossManager()
: m_pHeapBase(NULL)     		//!< ヒープベース
, m_isInit(false)            	//!< 初期済みフラグ
, m_timeout()           		//!< 非同期処理タイムアウト
, m_pListDLThread(NULL)//!< ファイルリストダウンロードスレッド
, m_pFileReadThread(NULL)//!<ファイル読み込みスレッド
, m_pCheckNewArrivalDataThread(NULL)//!<新着ファイル取得スレッド
, m_pImmediateTaskThread(NULL)//!<即時実行タスクスレッド
, m_pTaskRegisterThread(NULL)
, m_storageId(0)				//!<BOSSで使用する拡張セーブデータ領域のID
, m_storageSize(0)				//!<使用するサイズ（データ受信時に累計がこのサイズ以上になったら、古いものから削除される）
, m_entryCount(0)				//!<最大ファイル数(1~2000)（データ受信時にファイル数がこれ以上になったら、古いものから削除される
, m_checkStorageInterval(0)		//!<新着データ確認間隔（ミリ秒）（0の場合は自動的に確認しない）
, m_lastCheckStorageTime( nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() )	//!<最後に新着データ確認を実行した時間
, m_taskList()					//!<タスクIDリスト
, m_isRunningImmediateTask(false)//!<即時実行タスクが実行中かどうか
, m_threadPriority(0)
, m_pNsaListInfo(NULL)//!<ダウンロードしたファイルリスト
, m_pSerialIDList(NULL)//!<新着ファイルシリアルIDリスト
//, m_bossCode[TASK_NAME_LENGTH];//!<BOSSコード（\0で終端されている文字列）
//, m_mountName[MOUNT_NAME_LENGTH];//!<拡張セーブデータマウント名
//, m_apEventListener[MAX_LISTENER_NUM];	//!<イベントリスナー
{
  memset( m_bossCode , 0 , sizeof(m_bossCode) );
  memset( m_mountName , 0 , sizeof(m_mountName) );
  memset( m_apEventListener , 0 , sizeof(m_apEventListener) );
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BossManager::~BossManager()
{
  Finalize();
}

//------------------------------------------------------------------
/**
 * @brief  BossManagerの初期化
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result BossManager::Initialize( const BOSS_INIT_PARAM& initParam )
{
  nn::Result ret = nn::ResultSuccess();

  if( !m_isInit )
  {
    ret = nn::boss::Initialize();
    if( ret == nn::ResultSuccess() )
    {//成功
      ret = GFL_SINGLETON_INSTANCE(gflnet2::ndm::NdmManager)->Initialize();//NDMの管理
      if( ret == nn::ResultSuccess() )
      {
        ret = GFL_SINGLETON_INSTANCE(gflnet2::ndm::NdmManager)->ResumeBoss();
        if( ret == nn::ResultSuccess() )
        {
          //初期化パラメータをチェック
          BOSS_INIT_PARAM defParam;
          if( initParam.storageId == defParam.storageId ||
            initParam.storageSize == defParam.storageSize ||
            initParam.entryCount == defParam.entryCount ||
            initParam.pBossCode == defParam.pBossCode ||
            initParam.pMountName == defParam.pMountName ||
            initParam.checkStorageInterval == defParam.checkStorageInterval
          )
          {
            GFL_PRINT("BossManager::Initialize: error Invalid BOSS_INIT_PARAM\n");
            return nn::Result();
          }

          //初期化パラメータ保存
          m_storageId = initParam.storageId;
          m_storageSize = initParam.storageSize;
          m_entryCount = initParam.entryCount;
          m_checkStorageInterval = initParam.checkStorageInterval;
          memset( m_bossCode , 0 , sizeof(m_bossCode) );
          strncpy( m_bossCode , initParam.pBossCode , MOUNT_NAME_LENGTH - 1 );
          memset( m_mountName , 0 , sizeof( m_mountName ) );
          strncpy( m_mountName , initParam.pMountName , MOUNT_NAME_LENGTH - 1 );



          //ストレージ関連処理
          size_t storageSize = 0;
          u16 storageEntryCount = 0;
          //nn::Result storageResult = nn::boss::UnregisterStorage();//一旦登録解除
          //GFL_PRINT("BossManager::Initialize:UnregisterStorage : [0x%x]\n",storageResult.GetPrintableBits());
          nn::Result storageResult = nn::boss::GetStorageInfo( &storageSize , &storageEntryCount );
          if( storageResult == nn::boss::ResultStorageNotFound() )
          {//ストレージ未登録
            GFL_PRINT("BossManager::Initialize: RegisterStorage\n");
            storageResult = nn::boss::RegisterStorage( m_storageId , m_storageSize , m_entryCount );//ストレージ登録
          }
          else if( storageResult.IsSuccess() )
          {//登録済み
            //GFL_PRINT("BossManager::Initialize: Storage Registed\n");
            if( storageSize != m_storageSize || storageEntryCount != m_entryCount )
            {//ストレージ情報が食い違っている
              GFL_PRINT("BossManager::Initialize: Invalid Storage\n");
              storageResult = nn::boss::UnregisterStorage();
              if( storageResult.IsSuccess() )
              {
                GFL_PRINT("BossManager::Initialize: RegisterStorage Retry\n");
                storageResult = nn::boss::RegisterStorage( m_storageId , m_storageSize , m_entryCount );//ストレージ登録
              }
            }
          }

          if( !storageResult.IsSuccess() )
          {//ストレージ関連処理エラー
            GFL_PRINT("BossManager::Initialize: error Storage Setup Failed\n");
            return storageResult;
          }

          //GFL_PRINT("BossManager::Initialize: Initialize Success\n");

          m_isInit = true;
        }
      }
    }
  }
  return ret;
}




//------------------------------------------------------------------
/**
 * @brief  BossManagerの終了処理
 * @param[in]isUnregistStorage ストレージを解除するか（通常は解除する必要無いはず）
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result BossManager::Finalize( bool isUnregistStorage )
{
  nn::Result ret = nn::ResultSuccess();

  if( m_isInit )
  {
    DeleteAsyncInternal();//スレッド全破棄

    if( isUnregistStorage )
    {
      ret = nn::boss::UnregisterStorage();
      if( ret != nn::ResultSuccess() )
      {
        GFL_PRINT("BossManager::Finalize:error nn::boss::UnregisterStorage failed\n");
      }
    }

    ret = GFL_SINGLETON_INSTANCE(gflnet2::ndm::NdmManager)->Finalize();//NDMの管理
    if( ret == nn::ResultSuccess() )
    {
      ret = nn::boss::Finalize();
      if( ret == nn::ResultSuccess() )
      {
        RemoveAppHeap();

        m_isInit = false;
        memset( m_apEventListener , 0 , sizeof(m_apEventListener) );
      }
    }
  }
  return ret;
}

//------------------------------------------------------------------
/**
* @brief  使用するヒープを設定（ファイル読み込み等でサイズが大きい為、使用するアプリケーションが設定）
*/
//------------------------------------------------------------------
void BossManager::SetAppHeap( gfl2::heap::HeapBase* pHeapBase )
{
  if( pHeapBase && m_pHeapBase == NULL )
  {
    m_pHeapBase = pHeapBase;

    m_pNsaListInfo = GFL_NEW_ARRAY( m_pHeapBase ) nn::boss::NsaList::NsaInformation[NSA_LIST_INFO_NUM];
    m_pSerialIDList = GFL_NEW_ARRAY( m_pHeapBase ) u32[m_entryCount];
  }
  else
  {
    GFL_PRINT("BossManager::SetAppHeap:error !pHeapBase || m_pHeapBase\n");
  }
}

//------------------------------------------------------------------
/**
* @brief  使用するヒープの設定を解除
*/
//------------------------------------------------------------------
void BossManager::RemoveAppHeap()
{
  if( m_pFileReadThread || m_pListDLThread || m_isRunningImmediateTask || m_pImmediateTaskThread || m_pTaskRegisterThread )
  {
    GFL_PRINT("void BossManager::RemoveAppHeap():error task is running[0x%x][0x%x][%d]\n",m_pFileReadThread,m_pListDLThread,m_isRunningImmediateTask);
    GFL_ASSERT(0);//@fix
    return;
  }

  GFL_SAFE_DELETE_ARRAY( m_pNsaListInfo );
  GFL_SAFE_DELETE_ARRAY( m_pSerialIDList );

  m_pHeapBase = NULL;
}

//------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------
void BossManager::Update( void )
{
  //タスクリスト更新
  /*
  nn::Result result = nn::boss::GetTaskIdList( &m_taskList );//毎フレーム更新する必要は無いかも
  if( !result.IsSuccess() )
  {
    GFL_PRINT("BossManager::Update : error nn::boss::GetTaskIdList failed\n");
  }
  */

  //ファイル読み込み完了監視
  if( m_pFileReadThread )
  {
    if( !m_pFileReadThread->IsAlive() )
    {//終了している
      if( m_pFileReadThread->IsSuccess() )
      {//読み込み成功通知
        if( NotifyOnBossReadData( m_pFileReadThread->GetDataHeader() , m_pFileReadThread->GetReadSize() , (u8*)m_pFileReadThread->GetDataBody() ) )
        {//データが処理された
          m_pFileReadThread->DeleteContentsData();
        }
      }
      else
      {//エラー通知
        NotifyOnBossError( m_pFileReadThread->GetResult() );
      }
      GFL_PRINT("BossManager::Update : delete FileReadThread\n");
      GFL_SAFE_DELETE( m_pFileReadThread );
    }
  }

  //ファイルリストのダウンロード終了監視
  if( m_pListDLThread )
  {
    if( !m_pListDLThread->IsAlive() )
    {//終了している
      if( m_pListDLThread->IsSuccess() )
      {//DL成功通知
        NotifyBossRecvFileListEvent( m_pListDLThread->GetTaskName() , m_pNsaListInfo , m_pListDLThread->GetFileNum() );
      }
      else if( m_pListDLThread->GetErrorCode() != 0 )
      {//失敗：タスクエラーコードあり
        NotifyOnBossTaskError( FG_ONLY_TASK_NAME , m_pListDLThread->GetErrorCode() );
      }
      else
      {//DL失敗通知
        NotifyBossRecvFileListEventFailed( m_pListDLThread->GetTaskName() , m_pListDLThread->GetResult() );
      }

      GFL_PRINT("BossManager::Update : delete ListDLThread\n");
      GFL_SAFE_DELETE( m_pListDLThread );
    }
    else
    {//処理中
      if( !GFL_SINGLETON_INSTANCE(gflnet2::ndm::NdmManager)->IsWLANSwitchEnable() )
      {//キャンセルする
        GFL_PRINT("BossManager::Update : cancel ListDLThread\n");
        CancelDownloadFileList();
      }
    }
  }

  //タスク登録スレッド監視
  if( m_pTaskRegisterThread )
  {
    if( !m_pTaskRegisterThread->IsAlive() )
    {//終了している
      if( m_pTaskRegisterThread->IsSuccess() )
      {//成功
        //_NotifyBossRecvFileListEvent( m_pTaskRegisterThread->GetTaskName() , m_pNsaListInfo , m_pTaskRegisterThread->GetFileNum() );
      }
      else
      {//失敗通知
        NotifyOnBossError( m_pTaskRegisterThread->GetResult() );
      }

      GFL_PRINT("BossManager::Update : delete TaskRegisterThread\n");
      GFL_SAFE_DELETE( m_pTaskRegisterThread );
    }
  }

  //即時実行タスク監視
  if( m_isRunningImmediateTask && m_pImmediateTaskThread )
  {
    if( !m_pImmediateTaskThread->IsAlive() )
    {//終了している
      bool isSuccess = false;
      if( m_pImmediateTaskThread->IsSuccess() )
      {//タスク正常完了
        isSuccess = true;
      }
      else
      {//タスク失敗
        if( m_pImmediateTaskThread->GetResult().IsSuccess() )
        {
          NotifyOnBossTaskError( FG_ONLY_TASK_NAME , m_pImmediateTaskThread->GetErrorCode() );
        }
        else
        {
          NotifyOnBossError( m_pImmediateTaskThread->GetResult() );
        }
      }

      GFL_PRINT("BossManager::Update : delete ImmediateTaskThread\n");
      GFL_SAFE_DELETE( m_pImmediateTaskThread );
      m_isRunningImmediateTask = false;

      if( isSuccess )
      {
        CheckNewArrivalDataInternal();//到着データチェック
        m_lastCheckStorageTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
      }
    }
  }

  //新着データの定期確認処理
  s64 currentTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  if( m_checkStorageInterval > 0 && ( currentTime - m_lastCheckStorageTime > m_checkStorageInterval ) )
  {
    if( !IsRunningThread() )//リクエストキューにしたい
    {
      m_lastCheckStorageTime = currentTime;

      CheckNewArrivalDataInternal();//到着データチェック
    }
  }

  //新着データ取得スレッドのチェック
  if( m_pCheckNewArrivalDataThread )
  {
    if( !m_pCheckNewArrivalDataThread->IsAlive() )
    {//終了している
      if( m_pCheckNewArrivalDataThread->IsSuccess() )
      {//取得成功通知
        u32 idNum = m_pCheckNewArrivalDataThread->GetIdNum();
        for( u32 index = 0 ; index < idNum ; ++index )
        {
          bool isFilanData = (index + 1 == idNum);
          NotifyOnBossNewArrivalData( m_pCheckNewArrivalDataThread->GetNsDataId( index ) , isFilanData );
        }
      }
      else if( !m_pCheckNewArrivalDataThread->GetResult().IsSuccess() )
      {//取得失敗通知
        NotifyOnBossError( m_pCheckNewArrivalDataThread->GetResult() );
      }
      else
      {//データが1つもない
        NotifyOnBossNewArrivalDataEmpty();
      }

      GFL_PRINT("BossManager::Update : delete CheckNewArrivalDataThread\n");
      GFL_SAFE_DELETE( m_pCheckNewArrivalDataThread );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  初期化状態を調査
 * @result true： 初期済み
 *         false: 未初期化・または初期化中
 */
//------------------------------------------------------------------
bool BossManager::IsInitialized( void ) const
{
  return m_isInit;
}


//------------------------------------------------------------------
/**
* @brief  イベントリスナーの追加（最大16個）
* @param[in] pListener イベントリスナー
*
* @return 実行結果
*/
//------------------------------------------------------------------
bool BossManager::AddEventListener( BossEventListener* pListener )
{
  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] == pListener )
    {//登録済み
      return false;
    }
  }

  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] == NULL )
    {//登録
      m_apEventListener[index] = pListener;
      return true;
    }
  }

  //リスナー登録数が限界
  return false;
}

//------------------------------------------------------------------
/**
* @brief  リスナーを解除
* @param[in] pListener イベントリスナー
*/
//------------------------------------------------------------------
void BossManager::RemoveEventListener( BossEventListener* pListener )
{
  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] == pListener )
    {//登録解除
      m_apEventListener[index] = NULL;
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  新着ダウンロードデータがBOSSストレージに到着しているかチェック
*
*	新着データがあった場合OnBossNewArrivalDataイベントで通知されます。
*
*/
//------------------------------------------------------------------
void BossManager::CheckNewArrivalData()
{
  CheckNewArrivalDataInternal();
}

  //------------------------------------------------------------------
  /**
  * @brief  BOSSストレージに存在する全新着ダウンロードデータのヘッダを要求する
  *
  *	結果はOnReadHeaderイベントで取得します。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  //bool BossManager::RequestNewArrivalDataHeader(){ 未実装 }

  //------------------------------------------------------------------
  /**
  * @brief  BOSSストレージに存在する全ダウンロードデータのヘッダを要求する
  *
  *	新着以外のヘッダも要求します。
  *	結果はOnReadHeaderイベントで取得します。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  //bool BossManager::RequestAllDataHeader(){ 未実装 }

//------------------------------------------------------------------
/**
* @brief  BOSSストレージ内に存在するダウンロードデータの本体を要求する
* @param[in] serialId	データのシリアルID
*
*	シリアルIDはOnRecvNewDataイベント又は、ヘッダ情報から取得できます。
*	結果はOnReadDataイベントで取得します。
*
*/
//------------------------------------------------------------------
bool BossManager::RequestDataBody( const u32 serialId , int threadPriority )
{
  if( m_pHeapBase == NULL )
  {
    GFL_PRINT("BossManager::RequestDataBody : error m_pHeapBase is NULL\n");
    return false;
  }

  if( m_pFileReadThread )
  {//すでに実行中
    GFL_PRINT("BossManager::RequestDataBody : error already running\n");
    return false;
  }

  m_pFileReadThread = GFL_NEW_LOW( m_pHeapBase ) BossFileReadThread( m_pHeapBase , THREAD_STACK_SIZE , serialId );

  if( !m_pFileReadThread )
  {
    return false;
  }

  GFL_PRINT("BossManager::RequestDataBody: ThreadStart\n");
  m_threadPriority = threadPriority;
  m_pFileReadThread->Start();

  return true;
}


//------------------------------------------------------------------
/**
* @brief  タスクの登録（バックグラウンド実行）
* @param[in] taskInitParam 登録パラメータ
*
*	取得したデータはリスナー経由で受け取ります。
*	すでに同名のタスクが登録されていた場合、上書きします。
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result BossManager::RegisterTask( int threadPriority , const TASK_INIT_PARAM& taskInitParam )
{
  if( m_pHeapBase == NULL )
  {
    GFL_PRINT("BossManager::RegisterTask : error m_pHeapBase is NULL\n");
    return nn::Result();
  }

  nn::Result ret;
  if( m_pTaskRegisterThread == NULL )
  {
    m_pTaskRegisterThread = GFL_NEW_LOW( m_pHeapBase ) BossTaskRegisterThread( 	m_pHeapBase ,
                                              THREAD_STACK_SIZE ,
                                              taskInitParam );

    if( m_pTaskRegisterThread )
    {
      m_threadPriority = threadPriority;
      GFL_PRINT("BossManager::RegisterTask: ThreadStart\n");
      m_pTaskRegisterThread->Start();

      ret = nn::ResultSuccess();
    }
  }

  return ret;
}


//------------------------------------------------------------------
/**
* @brief  タスクの登録解除
* @param[in] pTaskName タスク名(char[TASK_NAME_LENGTH(8)])
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result BossManager::UnregisterTask( const char* pTaskName )
{
    nn::boss::Task deleteTargetTask;
    nn::Result result = deleteTargetTask.Initialize(pTaskName);
  if( !result.IsSuccess() )
  {
    GFL_PRINT("BossTaskRegisterThread::Main : error deleteTargetTask.Initialize failed[%s]\n",pTaskName);
  }
  else
  {
        result = deleteTargetTask.Cancel();//タスクが動作中ですと、UnregisterTaskは失敗します。確実に削除したい場合は、Cancelの実施後にUnregisterTaskを実行してください。
    if( !result.IsSuccess() )
      {
        GFL_PRINT("BossTaskRegisterThread::Main : error deleteTargetTask.Cancel failed[%s]\n",pTaskName);
      }
    else
    {
          result = nn::boss::UnregisterTask(&deleteTargetTask);
      if( !result.IsSuccess() )
        {
          GFL_PRINT("BossTaskRegisterThread::Main : error UnregisterTask is failed[%s]\n",pTaskName);
        }
    }
  }

  return result;
}

//------------------------------------------------------------------
/**
* @brief  サービスが有効かどうかチェック
* @param[in] pTaskName タスク名(char[TASK_NAME_LENGTH(8)])
*
* @return 実行結果
*/
//------------------------------------------------------------------
bool BossManager::IsEnableService( const char* pTaskName )
{
  nn::boss::Task targetTask;
  nn::Result result = targetTask.Initialize(pTaskName);
  if( !result.IsSuccess() )
  {
    GFL_PRINT("BossTaskRegisterThread::Main : error deleteTargetTask.Initialize failed[%s]\n",pTaskName);
  }
  else
  {
    nn::boss::TaskServiceStatus status = targetTask.GetServiceStatus();

    return status != nn::boss::SERVICE_TERMINATED;
  }

  return true;
}

//------------------------------------------------------------------
/**
* @brief  即時実行タスクを開始（フォアグラウンド実行）
* @param[in] pFileName ファイル名
*
*	即時実行タスクは同時に１つしか実行できません。
*	即時実行タスクは1回だけ実行されます。取得したデータはリスナー経由で受け取ります。
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result BossManager::StartImmidiateTask( int threadPriority , const char* pFileName , const char* pTaskName , const char* pRegionURL )
{
  if( m_isRunningImmediateTask )
  {//すでに実行中
    GFL_PRINT("BossManager::StartImmidiateTask: error m_isRunningImmidiateTask == true\n");
    return nn::Result();
  }

  if( m_pHeapBase == NULL )
  {
    GFL_PRINT("BossManager::StartImmidiateTask : error m_pHeapBase is NULL\n");
    return nn::Result();
  }

  nn::Result ret;
  if( m_pImmediateTaskThread == NULL )
  {
    m_pImmediateTaskThread = GFL_NEW_LOW( m_pHeapBase ) BossImmediateTaskThread( 	m_pHeapBase ,
                                              THREAD_STACK_SIZE ,
                                              pFileName , pTaskName , m_bossCode , pRegionURL );

    if( m_pImmediateTaskThread )
    {
      m_threadPriority = threadPriority;
      GFL_PRINT("BossManager::StartImmidiateTask: ThreadStart\n");
      m_pImmediateTaskThread->Start();

      ret = nn::ResultSuccess();
      m_isRunningImmediateTask = true;
    }
  }

  return ret;
}


//------------------------------------------------------------------
/**
* @brief  即時実行タスクをキャンセル
*
*	即時実行タスクをキャンセルし、タスクを破棄します。
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result BossManager::CancelImmidiateTask()
{
  if( !m_isRunningImmediateTask || !m_pImmediateTaskThread )
  {//実行してない
    return nn::ResultSuccess();
  }

  return m_pImmediateTaskThread->CancelTask();
}

//------------------------------------------------------------------
/**
* @brief  ファイルリストのダウンロードを開始（フォアグラウンド実行）
* @param[in] threadPriority スレッド優先度
* @param[in] pTaskName タスク名
* @param[in] pAttributes 検索条件
*
*	ファイルリストダウンロードは1回だけ実行されます。取得したリストはリスナー経由で受け取ります。
*
* @return 実行結果
*/
//------------------------------------------------------------------
bool BossManager::StartDownloadFileList( int threadPriority , const char* pTaskName , const BOSS_SEARCH_ATTRIBUTES* pAttributes )
{
  if( m_pHeapBase == NULL )
  {
    GFL_PRINT("BossManager::StartDownloadFileList : error m_pHeapBase is NULL\n");
    return false;
  }

  bool ret = false;
  if( m_pListDLThread == NULL )
  {
    const char* pTargetTaskName = NULL;
    if( pTaskName == NULL )
    {
      pTargetTaskName = FG_ONLY_TASK_NAME;//NULLだったらFG_ONLY_TASK
    }
    else
    {
      pTargetTaskName = pTaskName;
    }

    m_pListDLThread = GFL_NEW_LOW( m_pHeapBase ) BossFilelistDownloadThread( 	m_pHeapBase ,
                                              THREAD_STACK_SIZE ,
                                              m_pNsaListInfo ,
                                              NSA_LIST_INFO_NUM ,
                                              pTargetTaskName ,
                                              pAttributes );
    if( m_pListDLThread )
    {
      m_threadPriority = threadPriority;
      GFL_PRINT("BossManager::StartDownloadFileList: ThreadStart\n");
      m_pListDLThread->Start();

      ret = true;
    }
  }

  return ret;
}

//------------------------------------------------------------------
/**
* @brief  ファイルリストのダウンロードをキャンセル
*
*	ファイルリストのダウンロードをキャンセルします。
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result BossManager::CancelDownloadFileList()
{
  if( m_pListDLThread )
  {
    return m_pListDLThread->CancelDownload();
  }

  return nn::Result();
}

//------------------------------------------------------------------
/**
* @brief  スレッドが起動中かどうか
*
* @retval true スレッドが起動中 : false スレッドが起動中ではない
*/
//------------------------------------------------------------------
bool BossManager::IsRunningThread() const
{
  return CheckAsyncInternal();
}

//------------------------------------------------------------------
/**
* @brief  タスクの動作設定変更
* @param[in] pTaskName タスク名(char[TASK_NAME_LENGTH(8)])
* @param[in] interval タスク実行間隔（単位はhour）
* @param[in] count タスク実行回数（1~100)
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result BossManager::ChangeTaskConfig( const char* pTaskName , const u32 interval , const u32 count )
{
  nn::boss::TaskPolicy dPolicy;
  dPolicy.Initialize(interval, count);

  nn::boss::Task dTask;
  dTask.Initialize(pTaskName);

  return nn::boss::ReconfigureTask( &dTask , &dPolicy );
}

//------------------------------------------------------------------
/**
* @brief  登録中のタスク数を取得
*
* @retval タスク数
*/
//------------------------------------------------------------------
u16 BossManager::GetRegisterTaskNum( void )
{
  nn::Result result = nn::boss::GetTaskIdList( &m_taskList );
  if( result.IsSuccess() )
  {
    return m_taskList.GetSize();
  }

  return 0;
}

//------------------------------------------------------------------
/**
* @brief  登録中のタスク名を取得
* @param[in] index インデックス
*
* @retval タスク名(引数が無効な場合はNULL)
*/
//------------------------------------------------------------------
const char* BossManager::GetRegisterTaskName( const u16 index )
{
  //nn::Result result = nn::boss::GetTaskIdList( &m_taskList );
  //if( result.IsSuccess() )
  {
    return m_taskList.GetTaskId( index );
  }

  //return NULL;
}

//------------------------------------------------------------------
/**
* @brief  同名のファイル名かどうかチェック
* @param[in] index インデックス
* @param[in] pFileName ファイル名
*
* @retval 同名ならtrueそうでないならfalse
*/
//------------------------------------------------------------------
bool BossManager::IsEqualFileName( const u16 index , const char* pFileName )
{
  nn::boss::Task dTask;
  nn::Result result = dTask.Initialize(GetRegisterTaskName(index));
  if( result.IsSuccess() )
  {
    nn::boss::TaskPolicy dPolicy;
    nn::boss::TaskAction dAction;
    nn::boss::TaskOption dOption;

    result = dTask.GetInfo( &dPolicy , &dAction , &dOption );
    if( result.IsSuccess() )
    {
      char url[nn::boss::MAX_URL_LENGTH];
      std::memset( url , 0 , sizeof(url) );
      dAction.GetProperty( nn::boss::ACTION_URL , url , sizeof(url) );
      GFL_PRINT("BossManager::IsEqualFileName : [%s][%s]\n",pFileName,url);
      if( std::strstr( url , pFileName ) == NULL )
      {//一致なし
        return false;
      }
      else
      {//一致あり
        return true;
      }
    }
  }

  return false;
}



//------------------------------------------------------------------
/**
* @brief  登録されているストレージの最大サイズを取得
*
* @retval ストレージ領域の最大サイズ
*/
//------------------------------------------------------------------
size_t BossManager::GetStorageSizeMax( void ) const
{
  return m_storageSize;
}

//------------------------------------------------------------------
/**
* @brief  登録されているストレージの最大ファイル数を取得
*
* @retval ストレージの最大ファイル数
*/
//------------------------------------------------------------------
u16 BossManager::GetStorageFileNumMax( void ) const
{
  return m_entryCount;
}

//------------------------------------------------------------------
/**
* @brief  OptOutフラグを設定
* @param[in] flag		trueを設定すると、NADL タスクの実行時に、NS アーカイブに含まれているおしらせデータは破棄されます。（新着が通知されない）
*/
//------------------------------------------------------------------
nn::Result BossManager::SetOptoutFlag( const bool flag )
{
  return nn::boss::SetOptoutFlag( flag );
}

//------------------------------------------------------------------
/**
* @brief  設定されているOptOutフラグを取得
*
* @return OptOutフラグ
*/
//------------------------------------------------------------------
nn::Result BossManager::GetOptoutFlag( bool* pOutput ) const
{
  return nn::boss::GetOptoutFlag( pOutput );
}


//------------------------------------------------------------------
/**
* @brief  BOSSストレージのマウント名を取得
* @return マウント名
*/
//------------------------------------------------------------------
const char* BossManager::GetMountName( void ) const
{
  return m_mountName;
}

//------------------------------------------------------------------
/**
* @brief  BOSSコードを取得
* @return BOSSコード
*/
//------------------------------------------------------------------
const char* BossManager::GetBossCode( void ) const
{
  return m_bossCode;
}

//------------------------------------------------------------------
/**
* @brief  BOSSエラー表示
*/
//------------------------------------------------------------------
bool BossManager::PrintError( const nn::Result& result )
{
  GFL_PRINT("BossManaer::PrintError:");

  bool isFSError = false;
  if( result == nn::ResultSuccess() )
  { GFL_PRINT("ResultSuccess\n"); }
  else if( result == nn::boss::ResultInvalidPolicy() )
  { GFL_PRINT("ResultInvalidPolicy\n"); }
  else if( result == nn::boss::ResultInvalidAction() )
  { GFL_PRINT("ResultInvalidAction\n"); }
  else if( result == nn::boss::ResultInvalidOption() )
  { GFL_PRINT("ResultInvalidOption\n"); }
  else if( result == nn::boss::ResultInvalidAppIdList() )
  { GFL_PRINT("ResultInvalidAppIdList\n"); }
  else if( result == nn::boss::ResultInvalidTaskIdList() )
  { GFL_PRINT("ResultInvalidTaskIdList\n"); }
  else if( result == nn::boss::ResultInvalidStepIdList() )
  { GFL_PRINT("ResultInvalidStepIdList\n"); }
  else if( result == nn::boss::ResultInvalidNsDataIdList() )
  { GFL_PRINT("ResultInvalidNsDataIdList\n"); }
  else if( result == nn::boss::ResultInvalidTaskStatus() )
  { GFL_PRINT("ResultInvalidTaskStatus\n"); }
  else if( result == nn::boss::ResultInvalidPropertyValue() )
  { GFL_PRINT("ResultInvalidPropertyValue\n"); }
  else if( result == nn::boss::ResultInvalidNewArrivalEvent() )
  { GFL_PRINT("ResultInvalidNewArrivalEvent\n"); }
  else if( result == nn::boss::ResultInvalidNewArrivalFlag() )
  { GFL_PRINT("ResultInvalidNewArrivalFlag\n"); }
  else if( result == nn::boss::ResultInvalidOptoutFlag() )
  { GFL_PRINT("ResultInvalidOptoutFlag\n"); }
  else if( result == nn::boss::ResultInvalidTaskError() )
  { GFL_PRINT("ResultInvalidTaskError\n"); }
  else if( result == nn::boss::ResultInvalidNsDataValue() )
  { GFL_PRINT("ResultInvalidNsDataValue\n"); }
  else if( result == nn::boss::ResultInvalidNsDataInfo() )
  { GFL_PRINT("ResultInvalidNsDataInfo\n"); }
  else if( result == nn::boss::ResultInvalidNsDataReadFlag() )
  { GFL_PRINT("ResultInvalidNsDataReadFlag\n"); }
  else if( result == nn::boss::ResultInvalidNsDataTime() )
  { GFL_PRINT("ResultInvalidNsDataTime\n"); }
  else if( result == nn::boss::ResultInvalidNextExecuteTime() )
  { GFL_PRINT("ResultInvalidNextExecuteTime\n"); }
  else if( result == nn::boss::ResultHttpRequestHeaderPointerNull() )
  { GFL_PRINT("ResultHttpRequestHeaderPointerNull\n"); }
  else if( result == nn::boss::ResultInvalidPolicyListAvailability() )
  { GFL_PRINT("ResultInvalidPolicyListAvailability\n"); }
  else if( result == nn::boss::ResultInvalidTestModeAvailability() )
  { GFL_PRINT("ResultInvalidTestModeAvailability\n"); }
  else if( result == nn::boss::ResultInvalidTaskId() )
  { GFL_PRINT("ResultInvalidTaskId\n"); }
  else if( result == nn::boss::ResultInvalidTaskStep() )
  { GFL_PRINT("ResultInvalidTaskStep\n"); }
  else if( result == nn::boss::ResultInvalidPropertyType() )
  { GFL_PRINT("ResultInvalidPropertyType\n"); }
  else if( result == nn::boss::ResultInvalidUrl() )
  { GFL_PRINT("ResultInvalidUrl\n"); }
  else if( result == nn::boss::ResultInvalidFilePath() )
  { GFL_PRINT("ResultInvalidFilePath\n"); }
  else if( result == nn::boss::ResultInvalidTaskPriority() )
  { GFL_PRINT("ResultInvalidTaskPriority\n"); }
  else if( result == nn::boss::ResultInvalidTaskTargetDuration() )
  { GFL_PRINT("ResultInvalidTaskTargetDuration\n"); }
  else if( result == nn::boss::ResultActionCodeOutOfRange() )
  { GFL_PRINT("ResultActionCodeOutOfRange\n"); }
  else if( result == nn::boss::ResultInvalidNsDataSeekPosition() )
  { GFL_PRINT("ResultInvalidNsDataSeekPosition\n"); }
  else if( result == nn::boss::ResultInvalidMaxHttpRequestHeader() )
  { GFL_PRINT("ResultInvalidMaxHttpRequestHeader\n"); }
  else if( result == nn::boss::ResultInvalidMaxClientCert() )
  { GFL_PRINT("ResultInvalidMaxClientCert\n"); }
  else if( result == nn::boss::ResultInvalidMaxRootCa() )
  { GFL_PRINT("ResultInvalidMaxRootCa\n"); }
  else if( result == nn::boss::ResultSchedulingPolicyOutOfRange() )
  { GFL_PRINT("ResultSchedulingPolicyOutOfRange\n"); }
  else if( result == nn::boss::ResultApInfoTypeOutOfRange() )
  { GFL_PRINT("ResultApInfoTypeOutOfRange\n"); }
  else if( result == nn::boss::ResultTaskPermissionOutOfRange() )
  { GFL_PRINT("ResultTaskPermissionOutOfRange\n"); }
  else if( result == nn::boss::ResultWaitFinishTimeout() )
  { GFL_PRINT("ResultWaitFinishTimeout\n"); }
  else if( result == nn::boss::ResultWaitFinishTaskNotDone() )
  { GFL_PRINT("ResultWaitFinishTaskNotDone\n"); }
  else if( result == nn::boss::ResultIpcNotSessionInitialized() )
  { GFL_PRINT("ResultIpcNotSessionInitialized\n"); }
  else if( result == nn::boss::ResultIpcPropertySizeError() )
  { GFL_PRINT("ResultIpcPropertySizeError\n"); }
  else if( result == nn::boss::ResultIpcTooManyRequests() )
  { GFL_PRINT("ResultIpcTooManyRequests\n"); }
  else if( result == nn::boss::ResultAlreadyInitialized() )
  { GFL_PRINT("ResultAlreadyInitialized\n"); }
  else if( result == nn::boss::ResultOutOfMemory() )
  { GFL_PRINT("ResultOutOfMemory\n"); }
  else if( result == nn::boss::ResultInvalidNumberOfNsd() )
  { GFL_PRINT("ResultInvalidNumberOfNsd\n"); }
  else if( result == nn::boss::ResultNsDataInvalidFormat() )
  { GFL_PRINT("ResultNsDataInvalidFormat\n"); }
  else if( result == nn::boss::ResultAppNotExist() )
  { GFL_PRINT("ResultAppNotExist\n"); }
  else if( result == nn::boss::ResultTaskNotExist() )
  { GFL_PRINT("ResultTaskNotExist\n"); }
  else if( result == nn::boss::ResultTaskStepNotExist() )
  { GFL_PRINT("ResultTaskStepNotExist\n"); }
  else if( result == nn::boss::ResultApliIdAlreadyExist() )
  { GFL_PRINT("ResultApliIdAlreadyExist\n"); }
  else if( result == nn::boss::ResultTaskIdAlreadyExist() )
  { GFL_PRINT("ResultTaskIdAlreadyExist\n"); }
  else if( result == nn::boss::ResultTaskStepAlreadyExist() )
  { GFL_PRINT("ResultTaskStepAlreadyExist\n"); }
  else if( result == nn::boss::ResultInvalidSequence() )
  { GFL_PRINT("ResultInvalidSequence\n"); }
  else if( result == nn::boss::ResultDatabaseFull() )
  { GFL_PRINT("ResultDatabaseFull\n"); }
  else if( result == nn::boss::ResultCantUnregisterTask() )
  { GFL_PRINT("ResultCantUnregisterTask\n"); }
  else if( result == nn::boss::ResultTaskNotRegistered() )
  { GFL_PRINT("ResultTaskNotRegistered\n"); }
  else if( result == nn::boss::ResultInvalidFileHandle() )
  { GFL_PRINT("ResultInvalidFileHandle\n"); }
  else if( result == nn::boss::ResultInvalidTaskSchedulingPolicy() )
  { GFL_PRINT("ResultInvalidTaskSchedulingPolicy\n"); }
  else if( result == nn::boss::ResultInvalidHttpRequestHeader() )
  { GFL_PRINT("ResultInvalidHttpRequestHeader\n"); }
  else if( result == nn::boss::ResultInvalidHeadType() )
  { GFL_PRINT("ResultInvalidHeadType\n"); }
  else if( result == nn::boss::ResultStorageAccessPermission() )
  { GFL_PRINT("ResultStorageAccessPermission\n"); }
  else if( result == nn::boss::ResultStorageInsufficiency() )
  { GFL_PRINT("ResultStorageInsufficiency\n"); }
  else if( result == nn::boss::ResultStorageNotFound() )
  { GFL_PRINT("ResultStorageNotFound\n"); }
  else if( result == nn::boss::ResultNsDataNotFound() )
  { GFL_PRINT("ResultNsDataNotFound\n"); }
  else if( result == nn::boss::ResultInvalidNsDataGetHeadSize() )
  { GFL_PRINT("ResultInvalidNsDataGetHeadSize\n"); }
  else if( result == nn::boss::ResultNsDataListSizeShortage() )
  { GFL_PRINT("ResultNsDataListSizeShortage\n"); }
  else if( result == nn::boss::ResultNsDataListUpdated() )
  { GFL_PRINT("ResultNsDataListUpdated\n"); }
  else if( result == nn::boss::ResultNotConnectApWithLocation() )
  { GFL_PRINT("ResultNotConnectApWithLocation\n"); }
  else if( result == nn::boss::ResultNotConnectNetwork() )
  { GFL_PRINT("ResultNotConnectNetwork\n"); }
  else if( result == nn::boss::ResultInvalidFriendcode() )
  { GFL_PRINT("ResultInvalidFriendcode\n"); }
  else if( result == nn::boss::ResultFileAccess() )
  { GFL_PRINT("ResultFileAccess\n"); isFSError = true; }
  else if( result == nn::boss::ResultTaskAlreadyPaused() )
  { GFL_PRINT("ResultTaskAlreadyPaused\n"); }
  else if( result == nn::boss::ResultTaskAlreadyResumed() )
  { GFL_PRINT("ResultTaskAlreadyResumed\n"); }
  else if( result == nn::boss::ResultUnexpect() )
  { GFL_PRINT("ResultUnexpect\n"); }
  else if( result == nn::boss::ResultInvalidStorageParameter() )
  { GFL_PRINT("ResultInvalidStorageParameter\n"); }
  else if( result == nn::boss::ResultCfgInfoTypeOutOfRange() )
  { GFL_PRINT("ResultCfgInfoTypeOutOfRange\n"); }
  else if( result == nn::boss::ResultInvalidMaxHttpQuery() )
  { GFL_PRINT("ResultInvalidMaxHttpQuery\n"); }
  else if( result == nn::boss::ResultInvalidMaxDataStoreDst() )
  { GFL_PRINT("ResultInvalidMaxDataStoreDst\n"); }
  else if( result == nn::boss::ResultNSAListInvalidFormat() )
  { GFL_PRINT("ResultNSAListInvalidFormat\n"); }
  else if( result == nn::boss::ResultNSAListDownloadTaskError() )
  { GFL_PRINT("ResultNSAListDownloadTaskError\n"); }
  else if( result == nn::boss::ResultInvalidSize() )
  { GFL_PRINT("ResultInvalidSize\n"); }
  else if( result == nn::boss::ResultInvalidPointer() )
  { GFL_PRINT("ResultInvalidPointer\n"); }
  else if( result == nn::boss::ResultNotFound() )
  { GFL_PRINT("ResultNotFound\n"); }
  else if( result == nn::boss::ResultAlreadyExists() )
  { GFL_PRINT("ResultAlreadyExists\n"); }
  else if( result == nn::boss::ResultOutOfRange() )
  { GFL_PRINT("ResultOutOfRange\n"); }
  else if( result == nn::boss::ResultUnexpect() )
  { GFL_PRINT("ResultUnexpect\n"); }
  else if( result <= nn::fs::ResultMediaNotFound() )//↓FS関連のエラー
  { GFL_PRINT("fs::ResultMediaNotFound\n"); isFSError = true; }
  else if( result <= nn::fs::ResultNotFound() )
  { GFL_PRINT("fs::ResultNotFound\n"); isFSError = true; }
  else if( result <= nn::fs::ResultAlreadyExists() )
  { GFL_PRINT("fs::ResultAlreadyExists\n"); isFSError = true; }
  else if( result <= nn::fs::ResultNotEnoughSpace() )
  { GFL_PRINT("fs::ResultNotEnoughSpace\n"); }
  else if( result <= nn::fs::ResultArchiveInvalidated() )
  { GFL_PRINT("fs::ResultArchiveInvalidated\n"); isFSError = true; }
  else if( result <= nn::fs::ResultOperationDenied() )
  { GFL_PRINT("fs::ResultOperationDenied\n"); isFSError = true; }
  else if( result <= nn::fs::ResultWriteProtected() )
  { GFL_PRINT("fs::ResultWriteProtected\n"); isFSError = true; }
  else if( result <= nn::fs::ResultMediaAccessError() )
  { GFL_PRINT("fs::ResultMediaAccessError\n"); isFSError = true; }
  else if( result <= nn::fs::ResultNotFormatted() )
  { GFL_PRINT("fs::ResultNotFormatted\n"); isFSError = true; }
  else if( result <= nn::fs::ResultBadFormat() )
  { GFL_PRINT("fs::ResultBadFormat\n"); isFSError = true; }
  else if( result <= nn::fs::ResultVerificationFailed() )
  { GFL_PRINT("fs::ResultVerificationFailed\n"); isFSError = true; }
  else if( result <= nn::fs::ResultOutOfResource() )
  { GFL_PRINT("fs::ResultOutOfResource\n"); isFSError = true; }
  else if( result <= nn::fs::ResultAccessDenied() )
  { GFL_PRINT("fs::ResultAccessDenied\n"); isFSError = true; }
  else if( result <= nn::fs::ResultInvalidArgument() )
  { GFL_PRINT("fs::ResultInvalidArgument\n"); isFSError = true; }
  else if( result <= nn::fs::ResultNotInitialized() )
  { GFL_PRINT("fs::ResultNotInitialized\n"); isFSError = true; }
  else if( result <= nn::fs::ResultAlreadyInitialized() )
  { GFL_PRINT("fs::ResultAlreadyInitialized\n"); isFSError = true; }
  else if( result <= nn::fs::ResultUnsupportedOperation() )
  { GFL_PRINT("fs::ResultUnsupportedOperation\n"); isFSError = true; }
  else
  {
    GFL_PRINT("unknown error [0x%x]\n",result.GetPrintableBits() );
  }

  return isFSError;
}

//------------------------------------------------------------------
/**
* @brief  新着データをチェック
*/
//------------------------------------------------------------------
void BossManager::CheckNewArrivalDataInternal()
{
  if( m_pHeapBase == NULL )
  {//使用中のアプリからヒープがセットされていないので、チェックしない
    //GFL_PRINT("BossManager::CheckNewArrivalData() : m_pHeapBase is NULL\n");
    //_NotifyOnBossError( nn::Result() );
    return;
  }

  if( m_pCheckNewArrivalDataThread == NULL )
  {
    m_pCheckNewArrivalDataThread = GFL_NEW_LOW( m_pHeapBase ) BossCheckNewArrivalDataThread( 	m_pHeapBase ,
                                              THREAD_STACK_SIZE ,
                                              m_pSerialIDList ,
                                              m_entryCount );
    if( m_pCheckNewArrivalDataThread )
    {
      GFL_PRINT("BossManager::CheckNewArrivalData: ThreadStart\n");
      m_pCheckNewArrivalDataThread->Start();
    }
    else
    {
      GFL_PRINT("BossManager::CheckNewArrivalData() : error\n");
      NotifyOnBossError( nn::Result() );
    }
  }

  /*
  if( !ret )
  {//すでに起動済み
    GFL_PRINT("BossManager::CheckNewArrivalData() : error\n");
    _NotifyOnBossError( nn::Result() );
  }
  */
}

//------------------------------------------------------------------
/**
* @brief  新着データ発見イベント
*/
//------------------------------------------------------------------
void BossManager::NotifyOnBossNewArrivalData( const u32 serialId , const bool isFinalData )
{
  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] )
    {
      m_apEventListener[index]->OnBossNewArrivalData( serialId , isFinalData );
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  新着確認をしたがデータが空の時に発行されるイベント
*/
//------------------------------------------------------------------
void BossManager::NotifyOnBossNewArrivalDataEmpty()
{
  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] )
    {
      m_apEventListener[index]->OnBossNewArrivalDataEmpty();
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  データ読み込み完了イベント
*/
//------------------------------------------------------------------
bool BossManager::NotifyOnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody )
{
  GFL_PRINT("BossManager::NotifyOnBossReadData( [0x%x] , [%u] , [0x%x])\n" , pHeader , bodySize , pBody );
  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] )
    {
      if( m_apEventListener[index]->OnBossReadData( pHeader , bodySize , pBody ) )
      {
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------
/**
* @brief  ファイルリストダウンロード完了イベント発行
*/
//------------------------------------------------------------------
void BossManager::NotifyBossRecvFileListEvent( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{
  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] )
    {
      m_apEventListener[index]->OnBossRecvFileList( pTaskName , pFileList , fileNum );
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  ファイルリストダウンロード失敗イベント発行
*/
//------------------------------------------------------------------
void BossManager::NotifyBossRecvFileListEventFailed( const char* pTaskName , const nn::Result& result )
{
  GFL_PRINT( "BossManager::NotifyBossRecvFileListEventFailed([%s])\n" , pTaskName );
  bool isFSError = PrintError( result );
  if( isFSError )
  {//FSのエラー
    //復旧のためストレージの再登録を行う
    GFL_PRINT( "BossManager::NotifyOnBossError : Repair Storage\n");

    //ストレージ関連処理
    GFL_PRINT("BossManager::NotifyOnBossError:UnregisterStorage : [0x%x]\n",nn::boss::UnregisterStorage().GetPrintableBits());//一旦登録解除
    nn::boss::UnregisterStorage();
    GFL_PRINT("BossManager::NotifyOnBossError:RegisterStorage : [0x%x]\n",nn::boss::RegisterStorage( m_storageId , m_storageSize , m_entryCount ).GetPrintableBits());
    nn::boss::RegisterStorage( m_storageId , m_storageSize , m_entryCount );//ストレージ登録
  }

  u32 errorCode = 0;
  if( result == nn::boss::ResultFileAccess() )
  {
    nn::boss::GetErrorCode( &errorCode, nn::boss::FS_UNKNOWN_ERROR );
  }
  else
  {
    nn::boss::GetErrorCode( &errorCode, nn::boss::SEVERE_ERROR );
  }

  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] )
    {

      m_apEventListener[index]->OnBossTaskError( pTaskName , errorCode );
      //m_apEventListener[index]->OnBossRecvFileListFailed( pTaskName , result );
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  エラーイベント発行
*/
//------------------------------------------------------------------
void BossManager::NotifyOnBossError( const nn::Result& result )
{
  bool isFSError = PrintError( result );
  if( isFSError )
  {//FSのエラー
    //復旧のためストレージの再登録を行う
    GFL_PRINT( "BossManager::NotifyOnBossError : Repair Storage\n");

    //ストレージ関連処理
    GFL_PRINT("BossManager::NotifyOnBossError:UnregisterStorage : [0x%x]\n",nn::boss::UnregisterStorage().GetPrintableBits());
    nn::boss::UnregisterStorage();//一旦登録解除
    GFL_PRINT("BossManager::NotifyOnBossError:RegisterStorage : [0x%x]\n",nn::boss::RegisterStorage( m_storageId , m_storageSize , m_entryCount ).GetPrintableBits());
    nn::boss::RegisterStorage( m_storageId , m_storageSize , m_entryCount );//ストレージ登録
  }

  u32 errorCode = 0;
  if( result == nn::boss::ResultFileAccess() )
  {
    nn::boss::GetErrorCode( &errorCode, nn::boss::FS_UNKNOWN_ERROR );
  }
  else
  {
    nn::boss::GetErrorCode( &errorCode, nn::boss::SEVERE_ERROR );
  }

  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] )
    {
      m_apEventListener[index]->OnBossTaskError( FG_ONLY_TASK_NAME , errorCode );
      //m_apEventListener[index]->OnBossError( result );
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  タスクエラーイベント発行
*/
//------------------------------------------------------------------
void BossManager::NotifyOnBossTaskError( const char* pTaskName , const u32 errorCode )
{
  GFL_PRINT("BossManager::NotifyOnBossTaskError( [%s] , [%u] )\n",pTaskName,errorCode);

  bool isFSError = ( errorCode == nn::boss::FS_UNKNOWN_ERROR );
  if( isFSError )
  {//FSのエラー
    //復旧のためストレージの再登録を行う
    GFL_PRINT( "BossManager::NotifyOnBossError : Repair Storage\n");

    //ストレージ関連処理
    GFL_PRINT("BossManager::NotifyOnBossError:UnregisterStorage : [0x%x]\n",nn::boss::UnregisterStorage().GetPrintableBits());
    nn::boss::UnregisterStorage();//一旦登録解除
    GFL_PRINT("BossManager::NotifyOnBossError:RegisterStorage : [0x%x]\n",nn::boss::RegisterStorage( m_storageId , m_storageSize , m_entryCount ).GetPrintableBits());
    nn::boss::RegisterStorage( m_storageId , m_storageSize , m_entryCount );//ストレージ登録
  }

  for( u32 index = 0 ; index < MAX_LISTENER_NUM ; ++index )
  {
    if( m_apEventListener[index] )
    {
      m_apEventListener[index]->OnBossTaskError( pTaskName , errorCode );
    }
  }
}

//-------------------------------------------------------------------------------
/**
* @brief 非同期通信が実行中かどうかを調べる
* @retval true   実行中
* @retval false  実行していない
*/
//-------------------------------------------------------------------------------
bool BossManager::CheckAsyncInternal( void ) const
{
  if( m_pFileReadThread || m_pListDLThread || m_isRunningImmediateTask || m_pCheckNewArrivalDataThread || m_pImmediateTaskThread || m_pTaskRegisterThread )
  {
    return true;
  }

  return false;
}

//-------------------------------------------------------------------------------
/**
* @brief 実行中の非同期通信をキャンセルする
*/
//-------------------------------------------------------------------------------
void BossManager::CancelAsyncInternal( void )
{
  if( m_pListDLThread )
  {
    m_pListDLThread->Kill();
  }
  if( m_pFileReadThread )
  {
    m_pFileReadThread->Kill();
  }
  if( m_pCheckNewArrivalDataThread )
  {
    m_pCheckNewArrivalDataThread->Kill();
  }
  if( m_pImmediateTaskThread )
  {
    m_pImmediateTaskThread->Kill();
  }
  if( m_pTaskRegisterThread )
  {
    m_pTaskRegisterThread->Kill();
  }
}

//-------------------------------------------------------------------------------
/**
* @brief 非同期処理の終了待ちを行います。
*/
//-------------------------------------------------------------------------------
void BossManager::WaitAsyncFinishedInternal( void )
{
  while( CheckAsyncInternal() )
  {
    this->Update();//アップデートしないと終わらない
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
}

//-------------------------------------------------------------------------------
/**
* @brief 非同期処理システムを破棄する
*/
//-------------------------------------------------------------------------------
void BossManager::DeleteAsyncInternal( void )
{
  CancelAsyncInternal();
  WaitAsyncFinishedInternal();

  GFL_SAFE_DELETE( m_pListDLThread );
  GFL_SAFE_DELETE( m_pFileReadThread );
  GFL_SAFE_DELETE( m_pCheckNewArrivalDataThread );
  GFL_SAFE_DELETE( m_pImmediateTaskThread );
  GFL_SAFE_DELETE( m_pTaskRegisterThread );
}



//-------------------------------------------------------------------------------
/**
* @brief デバッグ用DNSチェック
*        開発中のBOSSサーバのテスト配信を受け取る為にはDNS[54.248.80.129]の設定が必要です。
*        この関数では本体にテスト用DNSが設定されているかどうかチェックします。
*        設定されていない場合エラー表示も行います。
*		すでにSocketSystem初期化済みの場合はisInitializeSocketSystemにfalseを設定してください。
*
* @param[in] pHeap SocketSystem用ヒープ
* @param[in] isInitializeSocketSystem SocketSystemの初期化終了を行うかどうか

* @retval true   DNSに[54.248.80.129]が設定されている
* @retval false  設定されていない
*/
//-------------------------------------------------------------------------------
/*static*/bool BossManager::DebugCheckDNS( gfl2::heap::HeapBase* pHeap , bool isInitializeSocketSystem )
{
  bool ret = true;
#if GFL_DEBUG
  const char* TEST_BOSS_DNS = "54.248.80.129";
  //gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  if( isInitializeSocketSystem )
  {
    nn::Result result = gflnet2::socket::SocketSystem::Initialize( pHeap );
    if( result.IsFailure() )
    {
      GFL_PRINT("BossManager::DebugCheckDNS : error gflnet2::socket::SocketSystem::Initialize failed\n");

      return false;
    }
  }



  nn::socket::InAddr dns1, dns2;
  gflnet2::socket::SocketSystem::GetDNS( &dns1 , &dns2 );
  if( strcmp( TEST_BOSS_DNS , gflnet2::socket::SocketSystem::GetAddressString( dns1 ) ) != 0 )
  {
    if( strcmp( TEST_BOSS_DNS , gflnet2::socket::SocketSystem::GetAddressString( dns2 ) ) != 0 )
    {
      GFL_PRINT("BossManager::DebugCheckDNS : warning TEST_BOSS_DNS != dns1,dns2");
      GFL_PRINT( ":TEST_BOSS_DNS[%s]" , TEST_BOSS_DNS );
      GFL_PRINT( ":dns1[%s]",gflnet2::socket::SocketSystem::GetAddressString( dns1 ) );
      GFL_PRINT( ":dns2[%s]\n",gflnet2::socket::SocketSystem::GetAddressString( dns2 ) );

      ret = false;
    }
  }

  if( isInitializeSocketSystem )
  {
    nn::Result result = gflnet2::socket::SocketSystem::Finalize();
    if( result.IsFailure() )
    {
      GFL_PRINT("BossManager::DebugCheckDNS : error gflnet2::socket::SocketSystem::Finalize failed\n");
    }
  }
#endif //GFL_DEBUG
  return ret;
}

GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)
#endif
