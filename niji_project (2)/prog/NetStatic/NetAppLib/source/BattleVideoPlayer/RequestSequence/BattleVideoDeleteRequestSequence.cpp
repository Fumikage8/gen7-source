// ============================================================================
/*
 * @file BattleVideoDeleteRequestSequence.cpp
 * @brief バトルビデオの削除を行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoDeleteRequestSequence.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideo)

//------------------------------------------------------------------
/**
* @brief  コンストラクタ/デストラクタ
*/
//------------------------------------------------------------------
BattleVideoDeleteRequestSequence::BattleVideoDeleteRequestSequence()
: m_pListener( NULL )
, m_deleteCount( 0 )
, m_dummyWaitCount( 0 )
, m_endSeq( 0 )
{
  ::std::memset( &m_requestParam , 0 , sizeof(m_requestParam) );
  ::std::memset( m_indexArray , 0 , sizeof(m_indexArray) );
  BattleVideoRequestClient::BindDataStoreClient( this );
}
BattleVideoDeleteRequestSequence::~BattleVideoDeleteRequestSequence()
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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE BattleVideoDeleteRequestSequence::Update( void )
{
  if( m_dummyWaitCount++ < DUMMY_WAIT )
  {//ダミーウェイト
    return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
  }

  switch( GetSequenceNo() )
  {
  case SEQ_INIT:
    {
#ifdef GF_PLATFORM_CTR
      if( m_requestParam.isOnline == false )
      {//サーバからの削除はしない
        SetSequenceNo( SEQ_END );
        break;
      }

      nn::nex::qVector<nn::nex::qUnsignedInt64> dataIDs;

      if( m_requestParam.isLocalDelete == false )
      {//公開とりけしのみ
        GFL_ASSERT( m_requestParam.deleteNum == 1 );
        ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pData = m_requestParam.pCancelData;

        nn::nex::DateTime dateTime;
        if( GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetNgsFacade()->GetGameServerTime( &dateTime ) )
        {
          u64 now = dateTime.GetDay() + dateTime.GetMonth() * 100 + dateTime.GetYear() * 10000;
          u64 getTime = pData->registDate.day + pData->registDate.month * 100 + pData->registDate.year * 10000;
          GFL_PRINT("now[%llu]getTime[%llu]\n",now,getTime);
          if( now == getTime )
          {
            if( m_pListener )
            {
              m_pListener->OnCancelUploadFailed();
            }
            return gflnet2::ServerClient::REQUEST_STATE_FINISH;
          }

          dataIDs.push_back( pData->dataID );
        }
      }

      if( m_requestParam.dataArray )
      {
        ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA** pDataPoint = m_requestParam.dataArray;
        for( u32 index = 0 ; index < m_requestParam.deleteNum ; ++index )
        {
          ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pDeleteData = *pDataPoint;
          GFL_PRINT("delete data ID[%llu]\n",pDeleteData->dataID);
          dataIDs.push_back( pDeleteData->dataID );
          pDataPoint++;
        }
      }

      bool result = DeleteDataStoreData( dataIDs );
      if( result == true )
      {
        m_endSeq  = 0;
        SetSequenceNo( SEQ_WAIT );
      }
      else
      {
        SetSequenceNo( SEQ_ERROR );
      }
#endif

    }
    break;
  case SEQ_WAIT:
    break;
  case SEQ_END:
    {
      //SDからデリート
      if( m_requestParam.isLocalDelete )
      {
        if( m_deleteCount < m_requestParam.deleteNum )
        {
          if( ExtSavedata::BattleRecorderSaveData::RESULT_FAILED_FATAL_ERROR == m_requestParam.pBattleRecorderSaveData->DeleteData( m_indexArray[m_deleteCount] ) )
          {
            if( m_pListener )
            {
              m_pListener->OnError( BattleVideoResponseListener::ERROR_TYPE_fatal );
            }

            return gflnet2::ServerClient::REQUEST_STATE_FINISH;
          }

          m_deleteCount++;
          return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
        }
      }
      else
      {
        // @fix GFNMCat[1901] アップロードキャンセルをSDに書き込む
        GFL_ASSERT( m_requestParam.deleteNum == 1 );
        if( m_deleteCount < m_requestParam.deleteNum )
        {
          switch( m_endSeq )
          {
            case 0:
            {
              std::memcpy( m_requestParam.pBattleRecorderSaveData->GetBattleRecDataDirect() , m_requestParam.pCancelData , sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA) );
              m_requestParam.pBattleRecorderSaveData->SetBattleRecUploadData( &m_requestParam.pCancelData->upload );
              m_requestParam.pBattleRecorderSaveData->SaveData( m_requestParam.cancelDataIndex, true );

              ++m_endSeq;
              break;
            }

            case 1:
            {
              if( m_requestParam.pBattleRecorderSaveData->WaitAsync() != ExtSavedata::BattleRecorderSaveData::RESULT_WAIT_ASYNC )
              {//処理中
                m_endSeq  = 0;
                m_deleteCount++;
              }

              break;
            }
          }

          return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
        }        
      }

      if( m_pListener )
      {
        m_pListener->OnDeleteSuccess();
      }
    }
    return gflnet2::ServerClient::REQUEST_STATE_FINISH;
  case SEQ_ERROR:
    {
      if( m_pListener )
      {
        m_pListener->OnError( BattleVideoResponseListener::ERROR_TYPE_default );
      }
    }
    return gflnet2::ServerClient::REQUEST_STATE_FINISH;
  }

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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE BattleVideoDeleteRequestSequence::UpdateCancel( void )
{
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//------------------------------------------------------------------
/**
* @brief  リスナー登録解除
*/
//------------------------------------------------------------------
void BattleVideoDeleteRequestSequence::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  リスナー設定
*/
//------------------------------------------------------------------
void BattleVideoDeleteRequestSequence::SetListener( BattleVideoResponseListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief  リクエストパラメータ設定
*/
//------------------------------------------------------------------
void BattleVideoDeleteRequestSequence::SetRequestParam( DELETE_REQUEST_PARAM_INTERNAL& pParam )
{
  m_requestParam = pParam;
  for( u32 index = 0 ; index < m_requestParam.deleteNum ; ++index )
  {
    m_indexArray[index] = m_requestParam.indexArray[index];
  }
}

#ifdef GF_PLATFORM_CTR

//--------------------------------------------------------------------------
/**
 * @brief   ストレージデータの削除
 * @param[in]  dataIds  削除対象データIDリスト
 * @return  関数の成否 成功でtrue
 */
//--------------------------------------------------------------------------
bool BattleVideoDeleteRequestSequence::DeleteDataStoreData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds )
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
 * @brief データ削除の完了通知
 *
 * @param[in] results 実行結果リスト
 */
//------------------------------------------------------------------
void BattleVideoDeleteRequestSequence::OnNexDataStoreDeleteDataFinish( const nn::nex::qVector<nn::nex::qResult>& results )
{
  GFL_PRINT("OnNexDataStoreDeleteDataFinish\n");

  if( m_requestParam.dataArray )
  {
    ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA** pDataPoint = m_requestParam.dataArray;
    for( u32 index = 0 ; index < results.size() ; index++ )
    {
      ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pDeleteData = *pDataPoint;

      if( results[index] )
      {
        GFL_PRINT("success [%d]\n",index);
        pDeleteData->upload_flag = false;

        // @fix NMCat[750] 公開取り消しをしたら再生回数を０にする
        pDeleteData->playCount  = 0;

      }
      else
      {
        GFL_PRINT("failed [%d]\n",index);
      }

      pDataPoint++;
    }
  }

  if( m_requestParam.pCancelData )
  {
    if( results[0] )
    {
      GFL_PRINT("success\n");
      m_requestParam.pCancelData->upload_flag = false;

      // @fix NMCat[750] 公開取り消しをしたら再生回数を０にする
      m_requestParam.pCancelData->playCount  = 0;

    }
    else
    {
      GFL_PRINT("failed\n");
    }
  }

  // 削除したデータの永続スロットを無効に戻す
  for( int index = 0 ; index < m_requestParam.deleteNum ; index++ )
  {
    u8 slotID = m_requestParam.pClient->GetPersistenceSlotId( m_indexArray[index] );
    if( slotID < BattleVideoRequestClient::ONLINE_DATA_MAX )
    {
      // 永続化スロットを無効にする
      m_requestParam.pClient->SetPersistenceInfo( slotID , false , 0 );
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
void BattleVideoDeleteRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  if( result == QERROR(DataStore, NotFound) )
  {//データがない
    GFL_PRINT("OnNexDataStoreClient : not found\n");
    SetSequenceNo( SEQ_END );
  }
  else
  {//他のエラー
    GFL_PRINT("OnNexDataStoreClientError\n");

    NetLib::Error::NijiNetworkError error;
    error.SetResultErrorCode( errorCode );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    SetSequenceNo( SEQ_ERROR );
  }
}

#endif

GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)
