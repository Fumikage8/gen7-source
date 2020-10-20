// ============================================================================
/*
 * @file BattleVideoRequestClient.cpp
 * @brief BattleVideoアプリで必要な通信リクエストを投げる為のクラスです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoRequestClient.h"
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoSyncRequestSequence.h"
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoDeleteRequestSequence.h"
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoUploadRequestSequence.h"
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoDownloadRequestSequence.h"
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/IncPlayCountRequestSequence.h"
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/SendPlayCompleteRequestSequence.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

#include <NetApp/BattleSpot/source/System/BattleSpotWork.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideo)


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ/デストラクタ
 */
//--------------------------------------------------------------------------------------------
BattleVideoRequestClient::BattleVideoRequestClient( BattleVideoResponseListener* pListener, gfl2::heap::HeapBase* pHeap )
: m_handle( 0 )
, m_pListener( pListener )
, m_pDownloadBuffer( NULL )
, m_pHeap( pHeap )
, m_pBSWork(NULL)
{
  GFL_ASSERT( pListener );//@fix

  m_pDownloadBuffer = GFL_NEW( pHeap ) ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA;
  std::memset( m_pDownloadBuffer , 0 , sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA) );
  std::memset( m_persistenceDataIDArray , 0 , sizeof(m_persistenceDataIDArray) );
  std::memset( m_parsistanceSlotEnable , 0 , sizeof(m_parsistanceSlotEnable) );

  ResetHeap();
}
BattleVideoRequestClient::~BattleVideoRequestClient()
{
  gflnet2::ServerClient::ServerClientRequestManager::RemoveListener( m_handle );
  if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) ) 
  {
    GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Finalize();
  }

  GFL_SAFE_DELETE( m_pDownloadBuffer );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   保存しているバトルビデオとサーバの同期をとる為のリクエスト
 * @param   pListener 結果を受け取るリスナー
 *                    OnSyncSuccess　か　OnError　が返ります
 * @param   requestParam リクエストパラメータ
 */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::SyncBattleVideoRequest( SYNC_REQUEST_PARAM& requestParam )
{
  ResetHeap();
  requestParam.pClient = this;
  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< BattleVideoSyncRequestSequence, BattleVideoResponseListener , SYNC_REQUEST_PARAM >( m_pListener, requestParam );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   バトルビデオを削除する為のリクエスト（パラメータの設定によって、サーバ上のデータ削除も行います）
  * @param   pListener 結果を受け取るリスナー
  *                    OnDeleteSuccess　か　OnError　が返ります
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::DeleteBattleVideoRequest( DELETE_REQUEST_PARAM& requestParam )
{
  ResetHeap();

  DELETE_REQUEST_PARAM_INTERNAL param;
  ::std::memset( &param , 0 , sizeof(param) );
  param.dataArray = requestParam.dataArray;
  param.deleteNum = requestParam.deleteNum;
  param.indexArray = requestParam.indexArray;
  param.isOnline = requestParam.isOnline;
  param.pBattleRecorderSaveData = requestParam.pBattleRecorderSaveData;
  param.isLocalDelete = true;
  param.pClient = this;

  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< BattleVideoDeleteRequestSequence, BattleVideoResponseListener , DELETE_REQUEST_PARAM_INTERNAL >( m_pListener, param );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   バトルビデオの公開取り消しをする為のリクエスト
  * @param   pListener 結果を受け取るリスナー
  *                    OnDeleteSuccess　か　OnDeleteFailed か　OnError　が返ります
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::CancelUploadBattleVideoRequest( CANCEL_UPLOAD_REQUEST_PARAM& requestParam )
{
  ResetHeap();

  DELETE_REQUEST_PARAM_INTERNAL param;
  ::std::memset( &param , 0 , sizeof(param) );
  param.pCancelData = requestParam.pCancelUploadData;
  param.cancelDataIndex = requestParam.index;
  param.deleteNum = 1;
  param.indexArray = &requestParam.index;
  param.isOnline = true;
  param.pBattleRecorderSaveData = requestParam.pBattleRecorderSaveData;
  param.isLocalDelete = false;
  param.pClient = this;

  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< BattleVideoDeleteRequestSequence, BattleVideoResponseListener , DELETE_REQUEST_PARAM_INTERNAL >( m_pListener, param );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   バトルビデオの不正チェックとアップロードを行う為のリクエスト
  * @param   pListener 結果を受け取るリスナー
  *                    OnUploadSuccess　か　OnUploadInvalid か　OnError　が返ります
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::UploadBattleVideoRequest( UPLOAD_REQUEST_PARAM& requestParam )
{
  ResetHeap();

  requestParam.pClient = this;
  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< BattleVideoUploadRequestSequence, BattleVideoResponseListener , UPLOAD_REQUEST_PARAM >( m_pListener, requestParam );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   バトルビデオの検索＆ダウンロードを行う為のリクエスト
  * @param   pListener 結果を受け取るリスナー
  *                    OnDownloadSuccess　か　OnDownloadNotFound か　OnError　が返ります
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::DownloadBattleVideoRequest( DOWNLOAD_REQUEST_PARAM& requestParam )
{
  ResetHeap();

  requestParam.pClient = this;
  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< BattleVideoDownloadRequestSequence, BattleVideoResponseListener , DOWNLOAD_REQUEST_PARAM >( m_pListener, requestParam );
}


//--------------------------------------------------------------------------------------------
/**
  * @brief   再生回数をインクリメントする為のリクエスト
  * @param   pListener 結果を受け取るリスナー
  *                    OnIncPlayCountSuccess　か OnError　が返ります
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::IncPlayCountRequest( INC_REQUEST_PARAM& requestParam )
{
  ResetHeap();

  requestParam.pClient = this;
  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< IncPlayCountRequestSequence, BattleVideoResponseListener , INC_REQUEST_PARAM >( m_pListener, requestParam );
}


//--------------------------------------------------------------------------------------------
/**
  * @brief   再生完走を通知する為のリクエスト
  * @param   pListener 結果を受け取るリスナー
  *                    OnPlayCompleteSuccess　か　OnError　が返ります
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::SendPlayCompleteRequest( COMPLETE_REQUEST_PARAM& requestParam )
{
  ResetHeap();

  requestParam.pClient = this;
  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< SendPlayCompleteRequestSequence, BattleVideoResponseListener , COMPLETE_REQUEST_PARAM >( m_pListener, requestParam );
}


//--------------------------------------------------------------------------------------------
/**
  * @brief   バトルビデオの不正チェックとアップロードを行う為のリクエスト
  * @param   pListener 結果を受け取るリスナー
  *                    OnUploadSuccess　か　OnUploadInvalid か　OnError　が返ります
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::UploadBattleVideoRequestForBattleSpot( UPLOAD_REQUEST_PARAM_FOR_BS& requestParam )
{
  ResetHeap();

  UPLOAD_REQUEST_PARAM uploadParam;
  std::memset( &uploadParam , 0 , sizeof(uploadParam) );

  uploadParam.index = 0;
  uploadParam.pClient = this;
  uploadParam.pUploadData = requestParam.pUploadData;
  uploadParam.pBattleRecorderSaveData = requestParam.pBattleRecorderSaveData;

  m_pBSWork = requestParam.pBSWork;

  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< BattleVideoUploadRequestSequence, BattleVideoResponseListener , UPLOAD_REQUEST_PARAM >( m_pListener, uploadParam );
}



///↓内部用


//--------------------------------------------------------------------------------------------
/**
  * @brief   永続化スロットID取得
  */
//--------------------------------------------------------------------------------------------
u8 BattleVideoRequestClient::GetPersistenceSlotId( void )
{
  for( int i=0; i<ONLINE_DATA_MAX; i++ )
  {
    if( m_parsistanceSlotEnable[i] == false )
    {
      return i;
    }
  }

  return ONLINE_DATA_MAX;
}


//--------------------------------------------------------------------------------------------
/**
  * @brief   永続化スロットID取得
  */
//--------------------------------------------------------------------------------------------
u8 BattleVideoRequestClient::GetPersistenceSlotId( u64 dataID )
{
  for( int i=0; i<ONLINE_DATA_MAX; i++ )
  {
    if( m_parsistanceSlotEnable[i] == true )
    {
      if( m_persistenceDataIDArray[i] == dataID )
      {
        return i;
      }
    }
  }

  return ONLINE_DATA_MAX;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   永続化情報設定
  */
//--------------------------------------------------------------------------------------------
void BattleVideoRequestClient::SetPersistenceInfo( int slotID , bool isEnable , u64 dataID )
{
  if( slotID >= ONLINE_DATA_MAX )
  {
    GFL_ASSERT(0);//@fix
    return;
  }

  m_parsistanceSlotEnable[slotID] = isEnable;
  m_persistenceDataIDArray[slotID] = dataID;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   永続化情報取得
  */
//--------------------------------------------------------------------------------------------
bool BattleVideoRequestClient::IsPersistenceEnable( int slotID )
{
  if( slotID >= ONLINE_DATA_MAX )
  {
    GFL_ASSERT(0);//@fix
    return false;
  }

  return m_parsistanceSlotEnable[slotID];
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   永続化情報取得
  */
//--------------------------------------------------------------------------------------------
u64 BattleVideoRequestClient::GetPersistenceDataID( int slotID )
{
  if( slotID >= ONLINE_DATA_MAX )
  {
    GFL_ASSERT(0);//@fix
    return 0;
  }

  return m_persistenceDataIDArray[slotID];
}


bool BattleVideoRequestClient::IsEmptySlotExists( void )
{
  return GetPersistenceSlotId() != ONLINE_DATA_MAX;
}



//ヒープ再設定
void BattleVideoRequestClient::ResetHeap( void )
{
  if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) ) 
  {
    GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Initialize( m_pHeap );
  }
}



#ifdef GF_PLATFORM_CTR
//--------------------------------------------------------------------------
/**
 * @brief アップロードのパラメータのタグに値をセットする
 * @param [in/out]tag     タグ
 * @param [in]value       値
 * @param [in]header      ヘッダ文字
 */
//--------------------------------------------------------------------------
void BattleVideoRequestClient::SetTagValue( nn::nex::qVector<nn::nex::String>* tags, const int value, const char* header )
{
  char tmp[ nn::nex::DataStoreConstants::MAX_TAG_LENGTH + 1 ];
  if( header )
  {
    ::std::sprintf( tmp, "%s%d", header, value );
    GFL_PRINT("DataStoreUpload:タグ[ %s ] %s %d\n", tmp, header, value);
  }else
  {
    ::std::sprintf( tmp, "%d", value );
    GFL_PRINT("DataStoreUpload:タグ[ %s ] %d\n", tmp, value);
  }

  tags->push_back(tmp);
}


//--------------------------------------------------------------------------
/**
 * @brief アップロードのパラメータのタグに値をセットする
 * @param [in/out]tag     タグ
 * @param [in]value       値
 * 
 * @note  16進数表記でセットする
 */
//--------------------------------------------------------------------------
void BattleVideoRequestClient::SetTagValueHex( nn::nex::qVector<nn::nex::String>* tags, const u64 value )
{
  char tmp[ nn::nex::DataStoreConstants::MAX_TAG_LENGTH + 1 ];
  // 16文字固定です
  std::sprintf( tmp, "%016llx", value );

  // 終端文字は内部でとってくれる
  tags->push_back( tmp );

  GFL_PRINT("DataStoreUpload:タグ[ %s ] %llx %llu\n", tmp, value, value);
}


//--------------------------------------------------------------------------
/**
 * @brief アップロードのパラメータのタグにセッショントークンを設定する
 * @param [in/out]tag     タグ
 * @param [in]token       セッショントークン
 * @param [in]header      ヘッダ文字
 * @param [in]loop_init   ループはじめの数値
 * @param [in]loop_end    ループおわりの数値
 */
//--------------------------------------------------------------------------
void BattleVideoRequestClient::SetTagToken( nn::nex::qVector<nn::nex::String>* tags, nn::nex::qVector<qByte>& token, const wchar_t* header, const u8 loop_init, const u8 loop_end )
{
  nn::nex::StringStream stream;
  // ヘッダ
  stream << header;
  
  for( u8 i=loop_init; i<=loop_end; i++ ){

    // サイズ超えていたら 0で。
    if( i >= token.size() ){
      stream << L"00";
    }else{
      wchar_t tmp[ 2 + 1 ];
      std::swprintf( tmp, 3, L"%02x", token.at(i) );
      stream << tmp;
    }
  }

  // Stringクラスに移す
  nn::nex::String str( stream.CStr() );
  tags->push_back( str );
  GFL_PRINT("DataStoreUpload:タグ[ %ls ] \n", str.CStr());
}


#endif

//--------------------------------------------------------------------------
/**
 * @brief     DataStoreClientのBind
 * @retval    false:成功
 * @retval    true:失敗
 */
//--------------------------------------------------------------------------
void BattleVideoRequestClient::BindDataStoreClient( gflnet2::nex::NexDataStoreClientListener* pListener )
{
#ifdef GF_PLATFORM_CTR
  if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {
    return;
  }

  // NexManager取得
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);

  // Bind
  if( !nexMgr->BindDataStoreClient() )
  {
    GFL_ASSERT( 0 );//@fix
    GFL_PRINT("BindDataStoreClient failed\n");
    return;
  }

  // リスナー登録
  nexMgr->GetDataStoreClient()->RegistDataStoreClientListener( pListener );


  GFL_PRINT("BindDataStoreClient success\n");
#endif
}

//--------------------------------------------------------------------------
/**
 * @brief   DataStoreClientのUnbind
 *
 * @retval  true:破棄終了
 * @retval  false:破棄中
 */
//--------------------------------------------------------------------------
bool BattleVideoRequestClient::UnbindDataStoreClient( gflnet2::nex::NexDataStoreClientListener* pListener )
{
#ifdef GF_PLATFORM_CTR
  if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {
    return true;
  }

  // NexManager取得
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  // DataStoreClient取得
  gflnet2::nex::NexDataStoreClient* dataStoreClient = nexMgr->GetDataStoreClient();

  if( dataStoreClient != NULL )
  {
    // 通信中か
    if( dataStoreClient->IsConnecting() != false )
    {
      GFL_PRINT("データストアサーバ通信中\n");
      dataStoreClient->CancelConnecting();
      return false;
    }

    // リスナー解除
    dataStoreClient->UnregistDataStoreClientListener( pListener );
  }

  // Unbind
  nexMgr->UnbindDataStoreClient();
  GFL_PRINT("UnbindDataStoreClient success\n");
#endif
  return true;
}



GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)
