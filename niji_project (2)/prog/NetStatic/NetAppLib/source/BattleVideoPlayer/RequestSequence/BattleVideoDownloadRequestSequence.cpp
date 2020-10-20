// ============================================================================
/*
 * @file BattleVideoDownloadRequestSequence.cpp
 * @brief バトルビデオの検索とダウンロードを行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoDownloadRequestSequence.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>

#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"
#include "Battle/include/battle_SetupParam.h"
#include "Savedata/include/Mystatus.h"

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

    /**
      Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
        デバッグ機能の追加
    */
#if PM_DEBUG
  #include  "../Debug/BattleVideoPlayerDebug.h"
#endif


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideo)

//------------------------------------------------------------------
/**
* @brief  コンストラクタ/デストラクタ
*/
//------------------------------------------------------------------
BattleVideoDownloadRequestSequence::BattleVideoDownloadRequestSequence()
: m_pListener( NULL )
, m_dummyWaitCount( 0 )
, m_isRequestEnd( false )
, m_isDownloadSuccess( false )
, m_playCount( 0 )
{
  std::memset( &m_date , 0 , sizeof(m_date) );

  memset( &m_requestParam , 0 , sizeof(m_requestParam) );
  BattleVideoRequestClient::BindDataStoreClient( this );
}
BattleVideoDownloadRequestSequence::~BattleVideoDownloadRequestSequence()
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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE BattleVideoDownloadRequestSequence::Update( void )
{
  switch( GetSequenceNo() )
  {
  case SEQ_INIT:
    {
      if( m_dummyWaitCount++ < DUMMY_WAIT )
      {//ダミーウェイト
        return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
      }
#ifdef GF_PLATFORM_CTR
      bool result = DownloadDataStoreData( m_requestParam.dataID );
      if( result == true )
      {
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
    {
      if( m_isRequestEnd == false )
      {
        return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
      }

      if( m_pListener )
      {
        if( m_isDownloadSuccess )
        {
          {
            #if defined(GF_PLATFORM_CTR)
            //不正文字置換処理
            ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* pData = m_requestParam.pClient->GetDownloadBufferPtr();
            for( u32 index = 0 ; index < BTL_CLIENT_NUM ; ++index )
            {
              if( pData->body.myStatus[ index ].is_exist == true )
              {
                Savedata::MyStatus tempStatus;
                tempStatus.Deserialize( pData->body.myStatus[ index ].status );
                NetLib::Validation::PokemonValidation::ReplacePlayerName( pData->header.name[index] , tempStatus.GetPokeLanguageId() , tempStatus.GetRomCode() );
              }
            }
            #endif
          }

          m_pListener->OnDownloadSuccess( m_requestParam.pClient->GetDownloadBufferPtr() , m_playCount , &m_date );
        }
        else
        {
          m_pListener->OnDownloadNotFound();
        }
      }

      return gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
  //break;
  case SEQ_ERROR:
    if( m_pListener )
    {
      m_pListener->OnError( BattleVideoResponseListener::ERROR_TYPE_default );
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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE BattleVideoDownloadRequestSequence::UpdateCancel( void )
{
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//------------------------------------------------------------------
/**
* @brief  リスナー登録解除
*/
//------------------------------------------------------------------
void BattleVideoDownloadRequestSequence::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  リスナー設定
*/
//------------------------------------------------------------------
void BattleVideoDownloadRequestSequence::SetListener( BattleVideoResponseListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief  リクエストパラメータ設定
*/
//------------------------------------------------------------------
void BattleVideoDownloadRequestSequence::SetRequestParam( BattleVideoRequestClient::DOWNLOAD_REQUEST_PARAM& pParam )
{
  m_requestParam = pParam;
}

#ifdef GF_PLATFORM_CTR
//--------------------------------------------------------------------------
/**
 * @brief   ストレージデータのダウンロード
 * @param[in]  dataIds  ダウンロードしたいデータID
 * @return  関数の成否 成功でtrue
 */
//--------------------------------------------------------------------------
bool BattleVideoDownloadRequestSequence::DownloadDataStoreData( const nn::nex::qUnsignedInt64 dataId )
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
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"DataStoreClient::GetObject(dataId:%llu) \n", dataId );
  #endif


  ::std::memset( m_requestParam.pClient->GetDownloadBufferPtr() , 0 , sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA) );
  if( dataStoreClient->DownloadData( dataId, m_requestParam.pClient->GetDownloadBufferPtr() , sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA) ) )
  {
    GFL_PRINT("DownloadDataStoreData success %llu \n", dataId );
    return true;
  }

  GFL_PRINT("DownloadDataStoreData failed %llu \n", dataId );
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
bool BattleVideoDownloadRequestSequence::GetMetaStoreData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds )
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
* @brief ストレージサーバからのデータダウンロード成功通知
*
* @param[in] downloadFileSize  取得したファイルサイズ
*/
//------------------------------------------------------------------
void BattleVideoDownloadRequestSequence::OnNexDataStoreDownloadDataSuccess( u32 downloadFileSize )
{
  nn::nex::qVector<nn::nex::qUnsignedInt64> dataIds;
  dataIds.push_back( m_requestParam.dataID );
  
  if( GetMetaStoreData( dataIds ) == false )
  {
    SetSequenceNo( SEQ_ERROR );
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
void BattleVideoDownloadRequestSequence::OnNexDataStoreDownloadMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results,
                                                  const nn::nex::qVector<nn::nex::DataStoreMetaInfo>& metas )
{
  GFL_PRINT("OnNexDataStoreDownloadMetaDataFinish\n");

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

    /**
      Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
        デバッグ機能の追加
    */
    {
#if PM_DEBUG
      m_requestParam.pDebug->OverwriteServerTimeByLocalTimeIfEnable(date);
#endif
    }

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

    m_date.year   = date.GetYear();
    m_date.month  = date.GetMonth();
    m_date.day    = date.GetDay();
    m_date.hour   = date.GetHour();
    m_date.minute = date.GetMinute();
    m_date.second = date.GetSecond();

    m_playCount = rateInfo1.GetTotalValue();

    /*
    // 見つかったデータが存在するか
    if( IsPersistenceSlotId( it->GetDataId() ) )
    {
      // バトルビデオの位置を取得
      u8 dataIndex = GetVideoDataIndexByDataId( it->GetDataId() );
      if( dataIndex == DATA_NOT_FOUND )
      {
        continue;
      }

      VIDEO_DATA* pVideo( GetVideoInfo( dataIndex ) );

      // 登録日付
      pVideo->registDate.SetYear( it->GetCreatedTime().GetYear() );
      pVideo->registDate.SetMonth( it->GetCreatedTime().GetMonth() );
      pVideo->registDate.SetDay( it->GetCreatedTime().GetDay() );
      pVideo->registDate.SetHour( it->GetCreatedTime().GetHour() );
      pVideo->registDate.SetMinute( it->GetCreatedTime().GetMinute() );
      pVideo->registDate.SetSecond( it->GetCreatedTime().GetSecond() );
      // 完走フラグ
      pVideo->isFinishPlay = !( rateInfo0.GetTotalValue() == 0 );

      // 再生回数は違う場合のみ格納
      if( pVideo->playCount != rateInfo1.GetTotalValue() )
      {
        // 再生回数
        pVideo->playCount = rateInfo1.GetTotalValue();
        // 再生回数を一旦保存しておく
        subWork->metaDataPlayCount[ dataIndex ] = pVideo->playCount;
      }
    }
    else if( mwk->downloadDataId == it->GetDataId() )
    {
      // 作成日付
      mwk->downloadCreateDate.SetYear( it->GetCreatedTime().GetYear() );
      mwk->downloadCreateDate.SetMonth( it->GetCreatedTime().GetMonth() );
      mwk->downloadCreateDate.SetDay( it->GetCreatedTime().GetDay() );
      mwk->downloadCreateDate.SetHour( it->GetCreatedTime().GetHour() );
      mwk->downloadCreateDate.SetMinute( it->GetCreatedTime().GetMinute() );
      mwk->downloadCreateDate.SetSecond( it->GetCreatedTime().GetSecond() );
      // 完走フラグ
      mwk->downloadDataIsFinishPlay = !( rateInfo0.GetTotalValue() == 0 );
      // 再生回数
      mwk->downloadDataPlayCount = rateInfo1.GetTotalValue();

      // VIDEO_INFOにも設定
      gfl::std::DateTime::Copy( &mwk->downloadCreateDate, &subWork->downloadVideoInfo.registDate );
      subWork->downloadVideoInfo.isFinishPlay = mwk->downloadDataIsFinishPlay;
      subWork->downloadVideoInfo.playCount  = mwk->downloadDataPlayCount;
    }
    */
  }


  m_isRequestEnd = true;
  m_isDownloadSuccess = true;
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
void BattleVideoDownloadRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  if( result == QERROR(DataStore, NotFound) )
  {//データがない
    GFL_PRINT("OnNexDataStoreClient : not found\n");
    m_isRequestEnd = true;
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
