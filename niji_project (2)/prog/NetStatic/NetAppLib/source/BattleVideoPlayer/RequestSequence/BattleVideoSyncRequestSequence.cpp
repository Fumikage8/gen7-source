// ============================================================================
/*
 * @file BattleVideoSyncRequestSequence.cpp
 * @brief 保存しているバトルビデオとサーバの同期を行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoSyncRequestSequence.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideo)

//------------------------------------------------------------------
/**
* @brief  コンストラクタ/デストラクタ
*/
//------------------------------------------------------------------
BattleVideoSyncRequestSequence::BattleVideoSyncRequestSequence()
: m_pListener( NULL )
, m_dummyWaitCount( 0 )
, m_endSeq( 0 )
, m_nowSaveDataNum( 0 )
, m_updateCount( 0 )
{
#ifdef GF_PLATFORM_CTR
  m_enableDataIds.clear();
  m_deleteDataIds.clear();
#endif
  memset( &m_requestParam , 0 , sizeof(m_requestParam) );
  memset( m_updateDataArray , 0 , sizeof(m_updateDataArray) );
  BattleVideoRequestClient::BindDataStoreClient( this );
}
BattleVideoSyncRequestSequence::~BattleVideoSyncRequestSequence()
{
  BattleVideoRequestClient::UnbindDataStoreClient( this );
}


//------------------------------------------------------------------
/**
* @brief  更新処理。
*
* @note   	メインループ毎にポーリングされる
*           キャンセル操作が行われた後はこのアップデートは呼ばれなくなり、
*           UpdateCancelが呼ばれるようになります。
*/
//------------------------------------------------------------------
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE BattleVideoSyncRequestSequence::Update( void )
{
  if( m_dummyWaitCount++ < DUMMY_WAIT )
  {//ダミーウェイト
    return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
  }

#ifdef GF_PLATFORM_CTR

  switch( GetSequenceNo() )
  {
    case SEQ_INIT:
      {
        m_endSeq  = 0;
        m_nowSaveDataNum  = 0;

        bool result = PersistenceStoreData();
        if( result == true )
        {
          SetSequenceNo( SEQ_PARSISTENCE_WAIT );
        }
        else
        {
          SetSequenceNo( SEQ_ERROR );
        }
      }
    break;
    case SEQ_PARSISTENCE_WAIT:
    break;
    case SEQ_END:
      // @fix NMCat[410] ネットから取得した情報をＳＤカードに保存する。こうしないとローカルビデオモード側で正しい情報が表示できない
      switch( m_endSeq )
      {
        case 0:
        {
          if( m_updateCount == 0 )
          {//更新が0件だったらセーブしない
            m_endSeq = 2;
            break;
          }

          NetAppLib::BattleVideoPlayer::VIDEO_DATA* pSaveVideoData  = m_updateDataArray[ m_nowSaveDataNum ];

          std::memcpy( m_requestParam.pBattleRecorderSaveData->GetBattleRecDataDirect() , &pSaveVideoData->sdCardData, sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA) );
          m_requestParam.pBattleRecorderSaveData->SetBattleRecUploadData( &pSaveVideoData->sdCardData.upload );
          m_requestParam.pBattleRecorderSaveData->SaveData( pSaveVideoData->saveDataListIdx, true );
          
          ++m_endSeq;

          break;
        }

        case 1:
        {
          if( m_requestParam.pBattleRecorderSaveData->WaitAsync( true ) != ExtSavedata::BattleRecorderSaveData::RESULT_WAIT_ASYNC )
          {
            //  処理終了
            ++m_nowSaveDataNum;
            if( m_nowSaveDataNum < m_updateCount )
            {
              //  まだセーブデータがあるのでシーケンスを元に戻して次のデータをセーブする
              m_endSeq  = 0;
            }
            else
            {
              //  全リストのセーブ完了
              ++m_endSeq;
            }
          }
          else
          {
            //処理中  
          }

          break;
        }

        case 2:
        {
          if( m_pListener )
          {
            m_pListener->OnSyncSuccess();
          }

          return gflnet2::ServerClient::REQUEST_STATE_FINISH;
        }
      }

      return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

    case SEQ_ERROR:
      if( m_pListener )
      {
        m_pListener->OnError( BattleVideoResponseListener::ERROR_TYPE_default );
      }
    return gflnet2::ServerClient::REQUEST_STATE_FINISH;
  }



#endif

  return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
}

//------------------------------------------------------------------
/**
* @brief  キャンセル処理の更新
*
* @note   	
*			      リクエストをキャンセルするとUpdateではなくこちらが呼ばれるようになります。
*           キャンセル処理が完了したらREQUEST_STATE_FINISHを返すとリクエストは終了します。
*           キャンセルが行われなかった場合、この関数は呼ばれません。
*/
//------------------------------------------------------------------
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE BattleVideoSyncRequestSequence::UpdateCancel( void )
{
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//------------------------------------------------------------------
/**
* @brief  リスナー登録解除
*/
//------------------------------------------------------------------
void BattleVideoSyncRequestSequence::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  リスナー設定
*/
//------------------------------------------------------------------
void BattleVideoSyncRequestSequence::SetListener( BattleVideoResponseListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief  リクエストパラメータ設定
*/
//------------------------------------------------------------------
void BattleVideoSyncRequestSequence::SetRequestParam( BattleVideoRequestClient::SYNC_REQUEST_PARAM& pParam )
{
  m_requestParam = pParam;
}

#ifdef GF_PLATFORM_CTR

//--------------------------------------------------------------------------
/**
 * @brief     公開中の永続化情報検索
 * @return    関数に成功するとtrue
 */
//--------------------------------------------------------------------------
bool BattleVideoSyncRequestSequence::PersistenceStoreData( void )
{

  // NexManager取得
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  // DataStoreClient取得
  gflnet2::nex::NexDataStoreClient* dataStoreClient = nexMgr->GetDataStoreClient();

  if( dataStoreClient == NULL ){
    return false;
  }

  // 永続化スロットIDのリスト
  nn::nex::qVector<nn::nex::qUnsignedInt16> slotIds;
  for( int i=DATASTORE_PERSISTENCE_SLOT_10; i <= DATASTORE_PERSISTENCE_SLOT_19; i++ ){
    slotIds.push_back(i);
  }

  GFL_PRINT("PID:%d\n", nn::friends::GetMyPrincipalId());

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"DataStoreClient::GetPersistenceInfo() \n" );
  #endif

  if( dataStoreClient->GetPersistenceInfo( nn::friends::GetMyPrincipalId(), slotIds ) )
  {
    GFL_PRINT("PersistenceStoreData success\n");
    return true;
  }
  GFL_PRINT("PersistenceStoreData failed\n");
  

  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief   ストレージデータの削除
 * @param[in]  dataIds  削除対象データIDリスト
 * @return  関数の成否 成功でtrue
 */
//--------------------------------------------------------------------------
bool BattleVideoSyncRequestSequence::DeleteDataStoreData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds )
{
  // NexManager取得
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  // DataStoreClient取得
  gflnet2::nex::NexDataStoreClient* dataStoreClient = nexMgr->GetDataStoreClient();

  if( dataStoreClient == NULL )
  {
    return false;
  }

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"DataStoreClient::DeleteObject() \n" );
  #endif

  if( dataStoreClient->DeleteData(dataIds) )
  {
    GFL_PRINT("DeleteDataStoreData success\n");
    return true;
  }

  GFL_PRINT("DeleteDataStoreData failed\n");
  return false;
}

//------------------------------------------------------------------
/**
 * @brief メタデータをダウンロードする(非同期通信）
 *
 * @param[in]  dataId  取得対象データID
 *
 * @return 実行結果。成功時はtrueが返却される。
 */
//------------------------------------------------------------------
bool BattleVideoSyncRequestSequence::GetMetaStoreData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds )
{
  // NexManager取得
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  // DataStoreClient取得
  gflnet2::nex::NexDataStoreClient* dataStoreClient = nexMgr->GetDataStoreClient();

  if( dataStoreClient == NULL )
  {
    return false;
  }

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"DataStoreClient::GetMeta() \n" );
  #endif

  if( dataStoreClient->DownloadMetaData( dataIds, gflnet2::nex::NexDataStoreClient::DL_TARGET_RATINGS ) )
  {
    //subWork->isDataStoreWait = true;
    GFL_PRINT("GetMetaStoreData success\n");
    return true;
  }

  GFL_PRINT("GetMetaStoreData failed\n");
  return false;
}

//------------------------------------------------------------------
/**
 * @brief 永続化情報取得の成功通知
 *
 * @param[in] persistenceInfoList 永続化情報リスト
 * @param[in] results 実行結果リスト
 */
//------------------------------------------------------------------
void BattleVideoSyncRequestSequence::OnNexDataStoreGetPersistenceInfo( const nn::nex::qVector< nn::nex::DataStorePersistenceInfo >& persistenceInfoList, const nn::nex::qVector<nn::nex::qResult>& results )
{
  GFL_PRINT("OnNexDataStoreGetPersistenceInfo\n");


  m_deleteDataIds.clear();
  m_enableDataIds.clear();

  int slotId = 0;
  nn::nex::qVector<nn::nex::DataStorePersistenceInfo>::const_iterator it;
  for( it = persistenceInfoList.begin(); it != persistenceInfoList.end(); ++it, ++slotId )
  {
    if( !it->IsValid() )
    {
      GFL_PRINT("スロットID:%d  無効\n", slotId);
      continue;
    }
    GFL_PRINT("スロットID:%d %d  有効\t", slotId, it->GetPersistenceSlotId());
    GFL_PRINT("データID:%llu  \n", it->GetDataId());
    // 永続化スロット使用している

    bool isExists = false;
    NetAppLib::BattleVideoPlayer::VIDEO_DATA** pDataPoint = m_requestParam.pDataList;
    for( u32 index = 0 ; index < m_requestParam.dataListNum ; ++index )
    {
      NetAppLib::BattleVideoPlayer::VIDEO_DATA* pData = *pDataPoint;

      u64 dataID = pData->sdCardData.dataID;
      if( dataID == it->GetDataId() )
      {
        GFL_PRINT("data find SD card[%llu]\n",it->GetDataId());
        isExists = true;
        break;
      }
      pDataPoint++;
    }

    if( isExists == false )
    {//SDカードに存在しない
      GFL_PRINT("delete target[%llu]\n",it->GetDataId());
      m_deleteDataIds.push_back( it->GetDataId() );
    }
    else
    {
      GFL_PRINT("enable target[%llu]\n",it->GetDataId());
      m_requestParam.pClient->SetPersistenceInfo( slotId , true , it->GetDataId() );
      m_enableDataIds.push_back( it->GetDataId() );
    }
  }

  NetAppLib::BattleVideoPlayer::VIDEO_DATA** pDataPoint = m_requestParam.pDataList;
  for( u32 index = 0 ; index < m_requestParam.dataListNum ; ++index )
  {
    NetAppLib::BattleVideoPlayer::VIDEO_DATA* pData = *pDataPoint;
    
    u64 dataID = pData->sdCardData.dataID;
    if( m_requestParam.pClient->GetPersistenceSlotId( dataID ) == BattleVideoRequestClient::ONLINE_DATA_MAX )
    {//サーバ上に存在しない
      pData->sdCardData.upload_flag = false;//アップロードフラグ落とす
    }
    pDataPoint++;
  }

  if( m_deleteDataIds.size() > 0 )
  {//サーバに削除対象がある
    if( DeleteDataStoreData( m_deleteDataIds ) == false )
    {
      SetSequenceNo( SEQ_ERROR );
    }
  }
  else if( m_enableDataIds.size() > 0 )
  {//アップロード済みがある
    if( GetMetaStoreData( m_enableDataIds ) == false )
    {
      SetSequenceNo( SEQ_ERROR );
    }
  }
  else
  {//なにもなし
    SetSequenceNo( SEQ_END );
  }
}

//------------------------------------------------------------------
/**
 * @brief データ削除の完了通知
 *
 * @param[in] results 実行結果リスト
 */
//------------------------------------------------------------------
void BattleVideoSyncRequestSequence::OnNexDataStoreDeleteDataFinish( const nn::nex::qVector<nn::nex::qResult>& results )
{
  GFL_PRINT("OnNexDataStoreDeleteDataFinish\n");


  for( u32 index = 0 ; index < results.size() ; index++ )
  {
    if( results[index] )
    {
      GFL_PRINT("success [%d]\n",index);
    }
    else
    {
      GFL_PRINT("failed [%d]\n",index);
    }
  }

  // 削除したデータの永続スロットを無効に戻す
  /*
  for( int index = 0 ; index < 0 ; index++ )
  {
    u8 slotID = m_requestParam.pClient->GetPersistenceSlotId( 0 );
    if( slotID < BattleVideoRequestClient::ONLINE_DATA_MAX )
    {
      // 永続化スロットを無効にする
      m_requestParam.pClient->SetPersistenceInfo( slotID , false , 0 );
    }
  }
  */

  if( m_enableDataIds.size() > 0 )
  {//アップロード済みがある
    if( GetMetaStoreData( m_enableDataIds ) == false )
    {
      SetSequenceNo( SEQ_ERROR );
    }
  }
  else
  {
    SetSequenceNo( SEQ_END );
  }
}

//------------------------------------------------------------------
/**
 * @brief メタデータダウンロードの完了通知
 *
 * @param[in] results 実行結果リスト
 * @param[in] metas   取得したメタデータリスト
 */
//------------------------------------------------------------------
void BattleVideoSyncRequestSequence::OnNexDataStoreDownloadMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results,
                                                  const nn::nex::qVector<nn::nex::DataStoreMetaInfo>& metas )
{
  GFL_PRINT("OnNexDataStoreDownloadMetaDataFinish\n");

  //更新用データ初期化
  m_updateCount = 0;
  ::std::memset( m_updateDataArray , 0 , sizeof(m_updateDataArray) );

  // 全部のデータを参照して、存在するdataIDのものがあれば
  // VIDEO_INFOに上書きする
  nn::nex::qVector<nn::nex::DataStoreMetaInfo>::const_iterator it;
  for ( it = metas.begin(); it != metas.end(); ++it )
  {
    // データが有効か
    if( !it->IsValid() )
    {
      continue;
    }

    GFL_PRINT("--------------------------------\n");
    GFL_PRINT("DataId:    %llu\n", it->GetDataId());
    GFL_PRINT("OwnerId:   %u\n", it->GetOwnerId());
    nn::nex::DateTime date = it->GetCreatedTime();
    GFL_PRINT("CreateData:%d/%d/%d %d:%d:%d\n",
              date.GetYear(), date.GetMonth(), date.GetDay(),
              date.GetHour(), date.GetMinute(), date.GetSecond() );

    nn::nex::qMap<nn::nex::qInt8, nn::nex::DataStoreRatingInfo> ratings;
    it->GetRating( ratings );
    // 評価スロット0 完走チェックスロット
    nn::nex::DataStoreRatingInfo rateInfo0 = ratings[0];
    GFL_PRINT("評価スロット 0\n");
    GFL_PRINT("評価合計値:%d\n", rateInfo0.GetTotalValue());
    GFL_PRINT("評価された回数:%d\n", rateInfo0.GetCount());
    GFL_PRINT("評価合計値の初期値:%d\n", rateInfo0.GetInitialValue());

    // 評価スロット1 再生数スロット
    nn::nex::DataStoreRatingInfo rateInfo1 = ratings[1];
    GFL_PRINT("評価スロット 1\n");
    GFL_PRINT("評価合計値:%d\n", rateInfo1.GetTotalValue());
    GFL_PRINT("評価された回数:%d\n", rateInfo1.GetCount());
    GFL_PRINT("評価合計値の初期値:%d\n", rateInfo1.GetInitialValue());


    NetAppLib::BattleVideoPlayer::VIDEO_DATA** pDataPoint = m_requestParam.pDataList;


    for( u32 index = 0 ; index < m_requestParam.dataListNum ; ++index )
    {
      NetAppLib::BattleVideoPlayer::VIDEO_DATA* pData = *pDataPoint;

      u64 dataID = pData->sdCardData.dataID;
      if( dataID == it->GetDataId() )
      {
        if( pData->sdCardData.playCount != rateInfo1.GetTotalValue() )
        {//値が更新されている
          if( m_updateCount == MAX_UPDATE_NUM )
          {
            GFL_ASSERT(0);//@fix
          }
          else
          {
            m_updateDataArray[m_updateCount] = pData;
            m_updateCount++;
          }
        }
        pData->sdCardData.playCount = rateInfo1.GetTotalValue();
      }

      pDataPoint++;
    }
  }

  SetSequenceNo( SEQ_END );

}


//------------------------------------------------------------------
/**
* @brief NexDataStoreClientのエラー通知
*
* @param[in] result    実行結果
* @param[in] errorCode エラーアプレット用エラーコード
*
* @note 以下のエラーをハンドリングする必要がある
*        QERROR(DataStore, NotFound) 指定したデータ ID や 永続化スロット にデータが見つからない。
*        QERROR(DataStore, PermissionDenied) 権限を持っていないデータに対してパスワードを指定せずに操作しようとした。
*        QERROR(DataStore, UnderReviewing) 審査中のため参照不可です。
*        QERROR(DataStore, InvalidPassword) 誤ったパスワードを指定して操作しようとした場合に発生します。
*        QERROR(DataStore, ValueNotEqual) 値が一致しないため、更新できなかった。
*/
//------------------------------------------------------------------
void BattleVideoSyncRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQ_ERROR );
}

#endif

GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)
