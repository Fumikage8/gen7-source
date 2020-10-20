#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWifiSystem.cpp
 * @brief  Wifi配信システムクラス
 * @author Shin Kosaka
 * @date   2012.8.07
 */
//=============================================================================

#include <NetStatic/NetLib/source/Delivery/Wifi/DeliveryWifiSystem.h>
#include <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>
#include <nn/ac/CTR/private/ac_GetConnectingAccessPoint.h>
//#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include <gflnet2/include/wifi/gflnet2_WifiSystem.h>

#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)


//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
DeliveryWifiSystem::DeliveryWifiSystem( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam , gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES* pAttributes , DeliveryFileListFilter* pFileListFilter , bool isWifiConnected )
: DeliverySystem( pHeap , pInitParam )
, m_pRecvListener( NULL )
, m_threadPriority( 0 )
, m_serialId( 0 )
, m_isWifiConnected(false)
, m_isWifiFinish(false)
, m_isPrepareFinalize(false)
, m_isFileReading(false)
, m_wifiStartEvent()
, m_wifiEndEvent()
, m_result()
, m_errorCode(0)
, m_pAttributes( pAttributes )
, m_pFileListFilter( pFileListFilter )
, m_isAreadyWifiConnected(isWifiConnected)
{
  ::std::memset( m_fileName , 0 , sizeof(m_fileName) );

  m_wifiStartEvent.Initialize(false);
  m_wifiEndEvent.Initialize(false);
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
DeliveryWifiSystem::~DeliveryWifiSystem()
{
   Finalize();
}

//------------------------------------------------------------------
/**
 * @brief  DeliveryWifiSystemの初期化
 * @result 	実行結果
 */
//------------------------------------------------------------------
bool DeliveryWifiSystem::Initialize( DeliveryRecvListener* pRecvListener , int threadPriority )
{
  GFL_PRINT("DeliveryWifiSystem::Initialize\n");

  if( !gflnet2::boss::BossManager::DebugCheckDNS( m_pHeap , !m_isAreadyWifiConnected ) )
  {
    GFL_PRINT("!!!please setting DNS for boss server!!!\n");
  }

  bool ret = false;
  if( !m_isInit )
  {//初期化
    if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager) && GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->IsInitialized() )
    {//Bossマネージャの準備が完了している

      if( !m_isAreadyWifiConnected )
      {
        //APに接続
        if( GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->StartWifiASync( this , threadPriority ) )
        {
          m_pRecvListener = pRecvListener;
          m_threadPriority = threadPriority;
          ret = true;

          m_isWifiFinish = false;

          //ヒープを設定
          GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->SetAppHeap( m_pHeap );
        }
        else
        {
          GFL_PRINT("DeliveryWifiSystem::Initialize():error StartWifiASync is failed\n");
        }
      }
      else
      {//接続済み
        m_pRecvListener = pRecvListener;
        m_threadPriority = threadPriority;
        ret = true;
        m_isWifiFinish = false;
        //ヒープを設定
        GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->SetAppHeap( m_pHeap );

        m_wifiStartEvent.Signal();
      }
    }
    else
    {
      GFL_PRINT("DeliveryWifiSystem::Initialize():error BossManager is not initialize\n");
    }
  }

  if( !ret )
  {
    Finalize();
    return false;
  }

  DeliverySystem::InitializeBase();

  return true;
}

//------------------------------------------------------------------
/**
 * @brief  DeliveryWifiSystemの終了処理
 * @result 実行結果
 */
//------------------------------------------------------------------
void DeliveryWifiSystem::Finalize( void )
{
  GFL_PRINT("DeliveryWifiSystem::Finalize\n");
  if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager) && GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->IsInitialized() )
  {
    GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->CancelDownloadFileList();
    GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->CancelImmidiateTask();
    GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->RemoveEventListener( this );

    GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->RemoveAppHeap();//ヒープ設定解除
  }
  m_pRecvListener = NULL;

  DeliverySystem::FinalizeBase();
}

//------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------
void DeliveryWifiSystem::Update( DeliveryRecvListener* pRecvListener )
{

  if( m_wifiStartEvent.TryWait() )
  {//スタートイベントが到着している
    m_isWifiConnected = true;

    if( m_pRecvListener == NULL )
    {
      GFL_PRINT("DeliveryWifiSystem::Update ConnectSuccess m_pRecvListener is NULL\n");
      return;
    }

    if( m_isPrepareFinalize )
    {//すでに終了要求がきている
      GFL_PRINT("DeliveryWifiSystem::OnWifiConnectSuccess already finalize\n");
    }
    else
    {
      //AP接続完了
      nn::ac::ApType apType = nn::ac::AP_TYPE_NONE;
      nn::ac::GetConnectingApType( &apType );
      if( apType == nn::ac::AP_TYPE_NINTENDO_ZONE )
      {//ニンテンドーゾーンに接続した
        m_pRecvListener->OnNintendoZoneConnected();
      }

      if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->AddEventListener( this ) )
      {//リスナー登録成功

        if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->StartDownloadFileList( m_threadPriority , NULL , m_pAttributes ) )
        {//リストダウンロード開始成功
          //ret = true;
        }
        else
        {
          GFL_PRINT("DeliveryWifiSystem::Initialize():error StartDownloadFileList is failed\n");
          m_pRecvListener->OnNetworkError( nn::Result() , 0 );
        }
      }
      else
      {
        GFL_PRINT("DeliveryWifiSystem::Initialize():error AddEventListener is failed\n");
        m_pRecvListener->OnNetworkError( nn::Result() , 0 );
      }
    }
  }

  if( m_wifiEndEvent.TryWait() )
  {//終了イベントが到着している
    m_isWifiConnected = false;
    m_isWifiFinish = true;

    // WifiSystemスレッドを削除する
    if( !m_isAreadyWifiConnected )
    {//切断する場合
      GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->DeleteAsync();
    }

    if( !m_result.IsSuccess() )
    {//何かエラーが発生している
      if( m_pRecvListener == NULL )
      {
        GFL_PRINT("DeliveryWifiSystem::Update ConnectFailed m_pRecvListener is NULL\n");
      }
      else if( m_errorCode == 0 )
      {
        m_pRecvListener->OnNetworkError( m_result , 0 );
      }
      else
      {
        m_pRecvListener->OnWifiError( m_errorCode );
      }
    }
  }

  if( m_isWifiConnected )
  {//Wifi接続済み
    if( !GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->IsRunningThread() )
    {//フォアグラウンド処理をしていないので終了処理
      if( !m_isAreadyWifiConnected )
      {//切断する場合
        GFL_PRINT("DeliveryWifiSystem::Update EndWifiASync\n");
        GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->EndWifiASync( this, m_threadPriority );//切断済みでもイベントは返ってくる？
      }
      else
      {//接続継続
        m_result = nn::ResultSuccess();
        m_wifiEndEvent.Signal();
      }
      m_isWifiConnected = false;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  終了準備
 */
//------------------------------------------------------------------
void DeliveryWifiSystem::PrepareFinalize( void )
{
  if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager) && GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->IsInitialized() )
  {
    //フォアグラウンド処理キャンセル
    GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->CancelDownloadFileList();
    GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->CancelImmidiateTask();
    //ファイルリードキャンセルつけたい

    m_isPrepareFinalize = true;
  }
}

//------------------------------------------------------------------
/**
 * @brief  停止確認
 */
//------------------------------------------------------------------
bool DeliveryWifiSystem::IsCompretePrepareFinalize( void )
{
  if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager) && GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->IsInitialized() )
  {
    if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->IsRunningThread() )
    {
      GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->Update();//アップデートしないと終わらない
      return false;
    }
  }

  return m_isWifiFinish && !m_isFileReading;
}

//------------------------------------------------------------------
/**
* @brief   WifiConnectListenerのイベント(これらのイベントは別のスレッドからコールされる)
 */
//------------------------------------------------------------------
void DeliveryWifiSystem::OnWifiConnectSuccess( void )
{
  GFL_PRINT("DeliveryWifiSystem::OnWifiConnectSuccess()\n");

  m_wifiStartEvent.Signal();

}
void DeliveryWifiSystem::OnWifiConnectFailed( const nn::Result& result, u32 errorCode )
{
  GFL_PRINT("DeliveryWifiSystem::OnWifiConnectFailed():error WifiConnect is failed[%d]\n",errorCode);

  m_result = result;
  m_errorCode = errorCode;

  m_wifiEndEvent.Signal();
}

void DeliveryWifiSystem::OnWifiDisconnectSuccess( void )
{
  GFL_PRINT("DeliveryWifiSystem::OnWifiDisconnectSuccess()\n");

  m_result = nn::ResultSuccess();

  m_wifiEndEvent.Signal();
}
void DeliveryWifiSystem::OnWifiDisconnecFailed( const nn::Result& result, u32 errorCode )
{
  GFL_PRINT("DeliveryWifiSystem::OnWifiDisconnecFailed():error[%d]\n",errorCode);

  m_result = result;
  m_errorCode = errorCode;

  m_wifiEndEvent.Signal();
}



//------------------------------------------------------------------
/**
* @brief   BossEventListenerのイベント(これらのイベントは同じスレッドからコールされる)
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
* @brief 新着データ通知イベント
* @param[in] serialId			データのシリアルID
*
*		新着データがBOSSストレージに到着したことを通知するイベントです。
*/
//------------------------------------------------------------------
void DeliveryWifiSystem::OnBossNewArrivalData( const u32 serialId , const bool isFinalData )
{
  GFL_PRINT("DeliveryWifiSystem::OnBossNewArrivalData[%u]\n",serialId);
  if( m_pRecvListener == NULL )
  {
    GFL_PRINT("DeliveryWifiSystem::OnBossNewArrivalData:error m_pRecvListener is NULL\n");
    return;
  }

  if( m_isPrepareFinalize )
  {//すでに終了要求がきている
    GFL_PRINT("DeliveryWifiSystem::OnBossNewArrivalData already finalize\n");
    return;//ここでリターンすればアップデートで終了処理が行われる
  }

  if( m_isFileReading )
  {//すでに読み込み中
    GFL_PRINT("DeliveryWifiSystem::OnBossNewArrivalData already reading\n");
    return;
  }

  if( m_pFileListFilter )
  {//フィルターあり
    //シリアルIDでチェックを行う
    u32 serialIdMax = m_pFileListFilter->GetSerialIdMax();
    u32 serialIdMin = m_pFileListFilter->GetSerialIdMin();
    if( serialId < serialIdMin || serialId > serialIdMax )
    {
      GFL_PRINT("DeliveryWifiSystem::OnBossNewArrivalData : pass data[%u][%u][%u]\n" , serialId , serialIdMax , serialIdMin );

      if( isFinalData )
      {//最後まで条件に合うデータが無い
        m_pRecvListener->OnNetworkError( nn::Result() , 0 );
      }

      return;
    }
  }

  m_pRecvListener->OnUpdateProgress( PROGRESS_NEW_ARRIVAL_DATA );

  //読み込み開始
  m_serialId = serialId;
  bool result = GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->RequestDataBody( serialId , m_threadPriority );
  if( !result )
  {//失敗
    GFL_PRINT("DeliveryWifiSystem::OnBossNewArrivalData:error RequestDataBody is failed\n");
    m_pRecvListener->OnNetworkError( nn::Result() , 0 );
  }

  m_isFileReading = true;
}

//------------------------------------------------------------------
/**
* @brief  新着確認をしたがデータが空の時に発行されるイベント
*/
//------------------------------------------------------------------
void DeliveryWifiSystem::OnBossNewArrivalDataEmpty( void )
{
  GFL_PRINT("DeliveryWifiSystem::OnBossNewArrivalDataEmpty:error data is empty\n");
  if( m_pRecvListener == NULL )
  {
    GFL_PRINT("DeliveryWifiSystem::OnBossNewArrivalData:error m_pRecvListener is NULL\n");
    return;
  }
  m_pRecvListener->OnNetworkError( nn::Result() , 0 );
}

//------------------------------------------------------------------
/**
* @brief データ読み込み完了イベント
*
* @param[in] pHeader		データヘッダ(NsDataHeaderInfo)
* @param[in] bodySize    	ボディサイズ
* @param[in] pBody			データ本体
* @return 	true : 獲得済みフラグをONにする（以降のリスナーにも通知されません）
* 			false: 獲得済みフラグをONにしない（以降のリスナーにも通知されます）
*
* 			struct NsDataHeaderInfo
* 			{
* 			   s64 titleId;					//64ビットのタイトルID
* 			   bit32 flags;					//NSDのフラグ
* 	    	   bit32 dataType;				//NSDのデータタイプ
* 	    	   s32 length;					//NSDの長さ
* 	    	   u32 serialId;				//NSDのシリアルID
* 	    	   u32 version;					//NSDのバージョン番号
* 	    	};
*
*/
//------------------------------------------------------------------
bool DeliveryWifiSystem::OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody )
{
  GFL_PRINT("DeliveryWifiSystem::OnBossReadData\n");

  m_isFileReading = false;

  if( m_pRecvListener == NULL )
  {
    GFL_PRINT("DeliveryWifiSystem::OnBossReadData:error m_pRecvListener is NULL\n");
    return true;
  }

  if( m_isPrepareFinalize )
  {//すでに終了要求がきている
    GFL_PRINT("DeliveryWifiSystem::OnBossReadData already finalize\n");
    return true;//ここでリターンすればアップデートで終了処理が行われる
  }

  if( pHeader && pHeader->serialId == m_serialId )
  {
    m_pRecvListener->OnUpdateProgress( PROGRESS_COMPLETE );
    bool isContinue = m_pRecvListener->OnRecvComplete( pBody , bodySize );
    if( isContinue )
    {
      GFL_PRINT("DeliveryWifiSystem::OnBossReadData:error Wifiでの受信継続は未対応\n");
    }

    return true;
  }
  else
  {
    GFL_PRINT("DeliveryWifiSystem::OnBossReadData:error size[%d]\n",bodySize);
    m_pRecvListener->OnNetworkError( nn::Result() , 0 );
  }

  return true;
}

//------------------------------------------------------------------
/**
* @brief ファイルリストダウンロード完了イベント
*
* @param[in] pTaskName		タスク名（ユーザがタスクダウンロード開始時に設定したもの）
* @param[in] pFileList    	ファイルリスト(nn::boss::NsaList::NsaInformation)
* @param[in] fileNum    		ファイル数
*
* @note pFileListは配列になっています。
*       pFileList[fileNum-1]までアクセスできます。
*
* 		struct NsaInformation
* 		{
* 		   char fileName[32];      	//ファイル名(最大31文字)
* 		   u32 fileSize;			//ファイルサイズ(単位はバイト)
* 	       u32 updateEpochTime;		//ファイルの更新時間(unix epochであるため、1970-01-01 00:00:00からの経過秒数)。NSAの公開開始日を値を用いるため、ダウンロード時のHTTPレスポンスのLast-Modifiedと一致する値となる。
* 	       char attribute1[10];		//属性1(最大9文字の文字列)。使用可能な文字は、数値(0-9)、アルファベット(a-z,A-Z)、アンダーバー(_)のみである。サーバ側UIからNSAごとに任意の値を設定できる。NSAリスト取得時にそれに含ませるNSAを指定するための検索要素となる。
* 	       char attribute2[10];		//属性2(最大9文字の文字列)。使用可能な文字や位置づけは、attribute1 と同様
* 	       char attribute3[10];		//属性3(最大9文字の文字列)。使用可能な文字や位置づけは、attribute1 と同様
* 	       u8 caption[150];			//説明文。UTF-8で最長50文字の文字列。サーバ側UIからNSAごとに任意の値を設定できる。
*		};
*/
//------------------------------------------------------------------
void DeliveryWifiSystem::OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{
  GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList\n");
  if( m_pRecvListener == NULL )
  {
    GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList:error m_pRecvListener is NULL\n");
    return;
  }

  if( m_isPrepareFinalize )
  {//すでに終了要求がきている
    GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList already finalize\n");
    return;//ここでリターンすればアップデートで終了処理が行われる
  }

  GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList : file exists [%d]\n",fileNum);

  //ファイルリストをフィルタリングする
  if( m_pFileListFilter )
  {//フィルターあり
    s32 downloadFileIndex = m_pFileListFilter->CheckFileList( pFileList , fileNum );
    if( downloadFileIndex < 0 || downloadFileIndex >= fileNum )
    {//該当ファイルなし
      GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList : Filelist not found\n");
//      m_pRecvListener->OnNetworkError( nn::Result() , 0 );
      m_pRecvListener->OnRecvComplete(NULL, 0);
    }
    else
    {
      GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList : file index [%d]\n",downloadFileIndex);

      m_pRecvListener->OnUpdateProgress( PROGRESS_FILELIST_DOWNLOADED );

      //ダウンロード
      {
        ::std::memcpy( m_fileName , pFileList[downloadFileIndex].fileName , sizeof( m_fileName ) );
        nn::Result result = GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->StartImmidiateTask( m_threadPriority , m_fileName , NULL , NetAppLib::Util::BossUtility::GetRegionURL() );
        if( !result.IsSuccess() )
        {//失敗
          GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList:error StartImmidiateTask is failed\n");
          m_pRecvListener->OnNetworkError( result , 0 );
        }
      }
    }
  }
  else
  {//フィルターなし
    if( fileNum == 0 )
    {//該当ファイル無し
      GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList : Filelist not found\n");
//      m_pRecvListener->OnNetworkError( nn::Result() , 0 );
      m_pRecvListener->OnRecvComplete(NULL, 0);
    }
    else
    {
      //先頭ダウンロード
      {
        ::std::memcpy( m_fileName , pFileList[0].fileName , sizeof( m_fileName ) );
        nn::Result result = GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->StartImmidiateTask( m_threadPriority , m_fileName  , NULL , NetAppLib::Util::BossUtility::GetRegionURL() );
        if( !result.IsSuccess() )
        {//失敗
          GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileList:error StartImmidiateTask is failed\n");
          m_pRecvListener->OnNetworkError( result , 0 );
        }
      }
    }
  }
}

//------------------------------------------------------------------
/**
* @brief ファイルリストダウンロード失敗通知イベント
*
* @param[in] pTaskName		タスク名（ユーザがタスク登録時に設定したもの）
* @param[in] result			実行結果
*/
//------------------------------------------------------------------
void DeliveryWifiSystem::OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result )
{
  GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileListFailed\n");
  if( m_pRecvListener == NULL )
  {
    GFL_PRINT("DeliveryWifiSystem::OnBossRecvFileListFailed:error m_pRecvListener is NULL\n");
    return;
  }

  m_pRecvListener->OnNetworkError( result , 0 );
}

//------------------------------------------------------------------
/**
* @brief エラー通知イベント
*
* @param[in] result		実行結果
*/
//------------------------------------------------------------------
void DeliveryWifiSystem::OnBossError( const nn::Result& result )
{
  GFL_PRINT("DeliveryWifiSystem::OnBossError:error\n");

  m_isFileReading = false;

  if( m_pRecvListener == NULL )
  {
    GFL_PRINT("DeliveryWifiSystem::OnBossError:error m_pRecvListener is NULL\n");
    return;
  }

  m_pRecvListener->OnNetworkError( result , 0 );
}

//------------------------------------------------------------------
/**
* @brief タスクエラー通知イベント
*
* @param[in] pTaskName		タスク名（ユーザがタスク登録時に設定したもの）
* @param[in] errorCode		エラーコード(nn::boss::TaskResultCode)
*/
//------------------------------------------------------------------
void DeliveryWifiSystem::OnBossTaskError( const char* pTaskName , const u32 errorCode )
{
  GFL_PRINT("DeliveryWifiSystem::OnBossTaskError:error[%u]\n",errorCode);

  m_isFileReading = false;

  if( m_pRecvListener == NULL )
  {
    GFL_PRINT("DeliveryWifiSystem::OnBossTaskError:error m_pRecvListener is NULL\n");
    return;
  }

  m_pRecvListener->OnNetworkError( nn::Result() , errorCode );
}


GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)
#endif
