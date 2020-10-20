// ============================================================================
/*
 * @file BattleVideoUploadRequestSequence.cpp
 * @brief バトルビデオの不正チェックとアップロードをするリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoUploadRequestSequence.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>

#include "NetStatic/NetAppLib/include/Util/NetAppConvertUtility.h"
#include <heap/include/gfl2_heap_manager.h>
#include "system/include/HeapDefine.h"
#include "system/include/HeapSize.h"

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include <NetStatic/NetLib/include/P2P/P2pConnectionManager.h>
#include <NetApp/BattleSpot/source/System/BattleSpotWork.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideo)

//------------------------------------------------------------------
/**
* @brief  コンストラクタ/デストラクタ
*/
//------------------------------------------------------------------
BattleVideoUploadRequestSequence::BattleVideoUploadRequestSequence()
: m_pListener( NULL )
, m_dummyWaitCount( 0 )
, m_isRequestEnd( false )
{
  memset( &m_requestParam , 0 , sizeof(m_requestParam) );
#ifdef GF_PLATFORM_CTR
  BattleVideoRequestClient::BindDataStoreClient( this );
  m_pValidator = NULL;
#endif
}
BattleVideoUploadRequestSequence::~BattleVideoUploadRequestSequence()
{
#ifdef GF_PLATFORM_CTR
  BattleVideoRequestClient::UnbindDataStoreClient( this );
  if( m_pValidator )
  {
    m_pValidator->Finalize();
  }
  GFL_SAFE_DELETE( m_pValidator );
#endif
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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE BattleVideoUploadRequestSequence::Update( void )
{
#ifdef GF_PLATFORM_CTR
  switch( GetSequenceNo() )
  {
  case SEQ_INIT:
    {
      //事前準備
      std::memcpy( m_requestParam.pBattleRecorderSaveData->GetBattleRecDataDirect() , m_requestParam.pUploadData , sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA) );
      m_requestParam.pBattleRecorderSaveData->SetBattleRecUploadData( &m_requestParam.pUploadData->upload );

      //不正チェック
      NetLib::Validation::PokemonValidation::GameMode mode = NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE_VIDEO_M;
      GFL_PRINT("ValidationMode[%d]\n",mode);

      m_pValidator = GFL_NEW( m_requestParam.pClient->GetHeap()->GetLowerHandle() ) NetLib::Validation::PokemonValidation();
      m_pValidator->Initialize( m_requestParam.pClient->GetHeap()->GetLowerHandle() , m_requestParam.pClient->GetHeap()->GetLowerHandle() , 
        GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetAuthenticationToken() ,
        mode
        );
      m_pValidator->SetPokemonValidationListener( this );
      u32 memberCount = 0;
      for( u32 clientIndex = 0 ; clientIndex < BTL_CLIENT_NUM ; clientIndex++ )
      {
        pml::PokeParty* tmpParty = m_requestParam.pBattleRecorderSaveData->GetPokeParty( static_cast<BTL_CLIENT_ID>(clientIndex) , m_requestParam.pClient->GetHeap()->GetLowerHandle() );
        if( tmpParty )
        {
          NetApp::GTS::GtsSerializePokemonParam tmpBuffer;

          for( u32 index = 0 ; index < tmpParty->GetMemberCount() ; ++index )
          {
            tmpParty->GetMemberPointer( index )->Serialize_Core( tmpBuffer.data );
            m_pValidator->SetPokemon( memberCount , NULL , &tmpBuffer );
            memberCount++;
          }

          GFL_SAFE_DELETE( tmpParty );
        }
      }

      m_pValidator->CheckPokemonASync( m_requestParam.pClient->GetHeap()->GetLowerHandle() , memberCount );

      SetSequenceNo( SEQ_VALIDATION_WAIT );
    }
  break;
  case SEQ_VALIDATION_WAIT:
    {
    }
  break;
  case SEQ_DATASTORE:
    {
      if( m_requestParam.pClient->GetBattleSpotWork() == NULL )
      {
        bool result = PersistenceStoreData();
        if( result == true )
        {
          SetSequenceNo( SEQ_WAIT );
        }
        else
        {
          SetSequenceNo( SEQ_ERROR );
        }
      }
      else
      {
        bool result = UploadDataStoreDataForBattleSpot();
        if( result == true )
        {
          SetSequenceNo( SEQ_WAIT );
        }
        else
        {
          SetSequenceNo( SEQ_ERROR );
        }
      }
    }
  break;
  case SEQ_WAIT:
    {
      if( m_isRequestEnd == false )
      {
        return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
      }

      if( m_requestParam.pClient->GetBattleSpotWork() )
      {//バトルスポット
        if( m_pListener )
        {
          m_pListener->OnUploadSuccess();
        }

        return gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }

      //アップロードフラグ更新されているので再度上書き
      std::memcpy( m_requestParam.pBattleRecorderSaveData->GetBattleRecDataDirect() , m_requestParam.pUploadData , sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA) );
      m_requestParam.pBattleRecorderSaveData->SetBattleRecUploadData( &m_requestParam.pUploadData->upload );
      m_requestParam.pBattleRecorderSaveData->SaveData( m_requestParam.index , true );

      SetSequenceNo( SEQ_SAVE );
    }
  break;
  case SEQ_SAVE:
    {
      if( m_requestParam.pBattleRecorderSaveData->WaitAsync() == ExtSavedata::BattleRecorderSaveData::RESULT_WAIT_ASYNC )
      {//処理中
        return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
      }

      if( m_pListener )
      {
        m_pListener->OnUploadSuccess();
      }

      return gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
  case SEQ_ERROR:
    if( m_pListener )
    {
      m_pListener->OnError( BattleVideoResponseListener::ERROR_TYPE_default );
    }
    return gflnet2::ServerClient::REQUEST_STATE_FINISH;
  case SEQ_NOT_EMPTY:
    if( m_pListener )
    {
      m_pListener->OnError( BattleVideoResponseListener::ERROR_TYPE_uploadFailed );
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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE BattleVideoUploadRequestSequence::UpdateCancel( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pValidator )
  {
    m_pValidator->CancelConnecting();
    if( m_pValidator->IsConnecting() )
    {
      return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
    }

    m_pValidator->Finalize();
    GFL_SAFE_DELETE( m_pValidator );
  }

#endif

  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//------------------------------------------------------------------
/**
* @brief  リスナー登録解除
*/
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  リスナー設定
*/
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::SetListener( BattleVideoResponseListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief  リクエストパラメータ設定
*/
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::SetRequestParam( BattleVideoRequestClient::UPLOAD_REQUEST_PARAM& pParam )
{
  m_requestParam = pParam;
}


//--------------------------------------------------------------------------
/**
 * @brief     ストレージデータのアップロード
 *
 * @return    アップロードに成功したらtrue
 */
//--------------------------------------------------------------------------
bool BattleVideoUploadRequestSequence::UploadDataStoreData( void )
{
#ifdef GF_PLATFORM_CTR
  // NexManager取得
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  // DataStoreClient取得
  gflnet2::nex::NexDataStoreClient* dataStoreClient = nexMgr->GetDataStoreClient();

  if( dataStoreClient == NULL )
  {
    return false;
  }

  // 送信パラメータ
  nn::nex::DataStorePostParam param;

  // データタイプ
  param.SetDataType( DATASTORE_DATATYPE_BATTLE_VIDEO_MANUAL );

  // データサイズ
  param.SetSize( m_requestParam.pBattleRecorderSaveData->GetBattleRecUploadDataSize() );
  // タイトル
  param.SetName("battle video");
  // 参照権限
  param.SetAccessPermission( nn::nex::DataStorePermission(nn::nex::DataStoreConstants::PERMISSION_PUBLIC) );

  // 評価スロット 0と1のスロットを使用する
  // 完走チェックスロット
  nn::nex::DataStoreRatingInitParam ratingInitParam0;
  ratingInitParam0.SetInitialValue(0); // 初期値
  ratingInitParam0.SetFlag( nn::nex::DataStoreConstants::RATING_FLAG_DISABLE_SELF_RATING ); // 自己評価禁止
  // 再生数スロット
  nn::nex::DataStoreRatingInitParam ratingInitParam1;
  ratingInitParam1.SetInitialValue(0); // 初期値
  // 日数指定ロック。1日1回評価できる。JST 0 時 (UTC 15 時) にロックをリセット
  nn::nex::DataStoreRatingLockInitParam ratingLockInitParam;
  ratingLockInitParam.SetDaysAfterLock(0, 15);
  ratingInitParam1.SetLock( ratingLockInitParam ); // 重複ロック：1日1回
  ratingInitParam1.SetFlag( nn::nex::DataStoreConstants::RATING_FLAG_DISABLE_SELF_RATING ); // 自己評価禁止
  // 初期値をセット
  nn::nex::qMap<nn::nex::qInt8, nn::nex::DataStoreRatingInitParam> ratings;
  ratings.insert( std::make_pair( 0, ratingInitParam0) );
  ratings.insert( std::make_pair( 1, ratingInitParam1) );
  param.SetRatingSetting( ratings );

  // タグ ※同じ値は設定できない
  nn::nex::qVector<nn::nex::String> tags;
  m_requestParam.pClient->SetTagValue( &tags, 1, "T" );   // 送信タイプ
  m_requestParam.pClient->SetTagValue( &tags, 0 );        // 自分のNexUniqueID 設定なし
  m_requestParam.pClient->SetTagValue( &tags, 1 );        // 相手のNexUniqueID 設定なし
  m_requestParam.pClient->SetTagValue( &tags, 0, "R" );   // 両者のレート合計 　設定なし
  m_requestParam.pClient->SetTagValue( &tags, 0, "C" );   // 検索属性 　       設定なし
  m_requestParam.pClient->SetTagValue( &tags, 0, "S1" );  // セッショントークン 設定なし
  m_requestParam.pClient->SetTagValue( &tags, 0, "S2" );  // セッショントークン 設定なし
  m_requestParam.pClient->SetTagValue( &tags, 0, "S3" );  // セッショントークン 設定なし
  m_requestParam.pClient->SetTagValue( &tags, 0, "S4" );  // セッショントークン 設定なし
#if PM_DEBUG
  m_requestParam.pClient->SetTagValue( &tags, m_requestParam.pClient->GetPersistenceSlotId(), "D" );
#endif
  param.SetTags(tags); // タグをセット

  // 永続化設定
  nn::nex::DataStorePersistenceInitParam persistenceInitParam; // 永続化情報
  persistenceInitParam.SetPersistenceSlotId( m_requestParam.pClient->GetPersistenceSlotId() + DATASTORE_PERSISTENCE_SLOT_10 ); // 永続化スロットID
  persistenceInitParam.SetDeleteLastObject( true ); // 以前のデータを削除する
  param.SetPersistenceInitParam( persistenceInitParam );

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"DataStoreClient::PostObject() \n" );
  #endif

  // データアップロード
  if( dataStoreClient->UploadData( param, m_requestParam.pUploadData ) )
  {
    GFL_PRINT("UploadDataStoreData success\n");
    return true;
  }

  GFL_PRINT("UploadDataStoreData failed\n");
  
#endif
  return false;
}


//--------------------------------------------------------------------------
/**
 * @brief     ストレージデータのアップロード
 *
 * @return    アップロードに成功したらtrue
 */
//--------------------------------------------------------------------------
bool BattleVideoUploadRequestSequence::UploadDataStoreDataForBattleSpot( void )
{
#ifdef GF_PLATFORM_CTR
  // NexManager取得
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  // DataStoreClient取得
  gflnet2::nex::NexDataStoreClient* dataStoreClient = nexMgr->GetDataStoreClient();

  if( dataStoreClient == NULL )
  {
    return false;
  }

  NetApp::BattleSpot::BattleSpotWork* pBattleSpotWork = m_requestParam.pClient->GetBattleSpotWork();

  // 送信パラメータ
  nn::nex::DataStorePostParam param;

  // データタイプ
  param.SetDataType( DATASTORE_DATATYPE_BATTLE_VIDEO_AUTO );

  // データサイズ
  param.SetSize( m_requestParam.pBattleRecorderSaveData->GetBattleRecUploadDataSize() );
  // タイトル
  param.SetName("battle video");
  // 参照権限
  param.SetAccessPermission( nn::nex::DataStorePermission(nn::nex::DataStoreConstants::PERMISSION_PUBLIC) );

  // 評価スロット 0と1のスロットを使用する
  // 完走チェックスロット
  nn::nex::DataStoreRatingInitParam ratingInitParam0;
  ratingInitParam0.SetInitialValue(0); // 初期値
  ratingInitParam0.SetRangeMax(1);     // 評価最大値は1
  ratingInitParam0.SetFlag( nn::nex::DataStoreConstants::RATING_FLAG_DISABLE_SELF_RATING ); // 自己評価禁止
  // 再生数スロット
  nn::nex::DataStoreRatingInitParam ratingInitParam1;
  ratingInitParam1.SetInitialValue(0); // 初期値
  // 日数指定ロック。1日1回評価できる。JST 0 時 (UTC 15 時) にロックをリセット
  nn::nex::DataStoreRatingLockInitParam ratingLockInitParam;
  ratingLockInitParam.SetDaysAfterLock(0, 15);
  ratingInitParam1.SetLock( ratingLockInitParam ); // 重複ロック：1日1回
  ratingInitParam1.SetFlag( nn::nex::DataStoreConstants::RATING_FLAG_DISABLE_SELF_RATING ); // 自己評価禁止
  // 初期値をセット
  nn::nex::qMap<nn::nex::qInt8, nn::nex::DataStoreRatingInitParam> ratings;
  ratings.insert( std::make_pair( 0, ratingInitParam0) );
  ratings.insert( std::make_pair( 1, ratingInitParam1) );
  param.SetRatingSetting( ratings );

  // タグ ※同じ値は設定できない
  nn::nex::qVector<nn::nex::String> tags;

  if( pBattleSpotWork->GetRule() == NetApp::BattleSpot::BattleSpotWork::RULE_NET_CUP )
  {//大会
    m_requestParam.pClient->SetTagValue( &tags, 3, "T" );   // 送信タイプ
  }
  else
  {//レート
    m_requestParam.pClient->SetTagValue( &tags, 2, "T" );   // 送信タイプ
  }

  u64 myNexID = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->GetMyStatus( 1 - GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->GetNetID() )->GetNexUniqueID();
  u64 otherNexID = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus()->GetNexUniqueID();

  m_requestParam.pClient->SetTagValueHex( &tags, myNexID );          // 自分のNexUniqueID
  m_requestParam.pClient->SetTagValueHex( &tags, otherNexID );   // 相手のNexUniqueID
  {
    // 両者のレート合計
    int   rate_sum = pBattleSpotWork->GetOpponentRate() + pBattleSpotWork->GetRankData().rate;
    m_requestParam.pClient->SetTagValue( &tags, rate_sum, "R" );
  }

  u32 community_id = pBattleSpotWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
  m_requestParam.pClient->SetTagValue( &tags, community_id, "C" );

  {
    // セッショントークン
    // トークンの最大サイズは26byte
    // 1byteの数字を16進数文字で格納する
    // 
    nn::nex::qVector<qByte> token;
    NetLib::P2P::NexRatingClient* pRatingClient = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->GetRatingClient();
    if( pRatingClient )
    {
      pRatingClient->GetRatingSessionToken( token );
    }

    if( !token.empty() )
    {
      m_requestParam.pClient->SetTagToken( &tags, token, L"S1", 0, 6);
      m_requestParam.pClient->SetTagToken( &tags, token, L"S2", 7, 13);
      m_requestParam.pClient->SetTagToken( &tags, token, L"S3", 14,20);
      m_requestParam.pClient->SetTagToken( &tags, token, L"S4", 21,25);
    }else
    {
      GFL_PRINT("セッショントークンのタグはセットしていない\n");
    }
  }

  param.SetTags(tags); // タグをセット

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"DataStoreClient::PostObject() \n" );
  #endif

  // データアップロード
  if( dataStoreClient->UploadData( param, m_requestParam.pUploadData ) )
  {
    GFL_PRINT("UploadDataStoreData success\n");
    return true;
  }

  GFL_PRINT("UploadDataStoreData failed\n");
  
#endif
  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief     公開中の永続化情報検索
 * @return    関数に成功するとtrue
 */
//--------------------------------------------------------------------------
bool BattleVideoUploadRequestSequence::PersistenceStoreData( void )
{
#ifdef GF_PLATFORM_CTR
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
  
#endif
  return false;
}


#ifdef GF_PLATFORM_CTR

//------------------------------------------------------------------
/**
 * @brief アップロードの成功通知
 *
 * @param[in] dataId アップロードデータに割り当てられたユニークID。
 */
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::OnNexDataStoreUploadSuccess( const nn::nex::qUnsignedInt64 dataId )
{

  GFL_PRINT("OnNexDataStoreUploadSuccess %llu \n", dataId );

  m_requestParam.pUploadData->dataID = dataId;

#if PM_DEBUG
  {
    gfl2::str::StrBuf outStr( 64 , gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
    NetApp::Util::ConvertUtility::U64ToStr( dataId, &outStr , false );
  }
#endif

  if( m_requestParam.pClient->GetBattleSpotWork() )
  {//バトルスポット
    m_isRequestEnd = true;
    return;
  }

  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  
  m_requestParam.pUploadData->upload_flag = true;


  nn::nex::DateTime time;
  if( nexMgr->GetNgsFacade()->GetGameServerTime(&time) )
  {
    m_requestParam.pUploadData->registDate.year = time.GetYear();
    m_requestParam.pUploadData->registDate.month = time.GetMonth();
    m_requestParam.pUploadData->registDate.day = time.GetDay();
    m_requestParam.pUploadData->registDate.hour = time.GetHour();
    m_requestParam.pUploadData->registDate.minute = time.GetMinute();
  }

#if PM_DEBUG
  {
    gfl2::str::StrBuf outStr( 64 , gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
    NetApp::Util::ConvertUtility::U64ToStr( dataId, &outStr , false );
  }
#endif

  m_requestParam.pClient->SetPersistenceInfo( m_requestParam.pClient->GetPersistenceSlotId() , true , dataId );

  m_isRequestEnd = true;
}


//------------------------------------------------------------------
/**
 * @brief 永続化情報取得の成功通知
 *
 * @param[in] persistenceInfoList 永続化情報リスト
 * @param[in] results 実行結果リスト
 */
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::OnNexDataStoreGetPersistenceInfo( const nn::nex::qVector< nn::nex::DataStorePersistenceInfo >& persistenceInfoList, const nn::nex::qVector<nn::nex::qResult>& results )
{
  GFL_PRINT("OnNexDataStoreGetPersistenceInfo\n");

  int slotId = 0;
  nn::nex::qVector<nn::nex::DataStorePersistenceInfo>::const_iterator it;
  for( it = persistenceInfoList.begin(); it != persistenceInfoList.end(); ++it, ++slotId )
  {
    if( !it->IsValid() )
    {
      GFL_PRINT("スロットID:%d  無効\n", slotId);
      m_requestParam.pClient->SetPersistenceInfo( slotId , false , 0 );
      continue;
    }
    GFL_PRINT("スロットID:%d %d  有効\t", slotId, it->GetPersistenceSlotId());
    GFL_PRINT("データID:%llu  \n", it->GetDataId());
    // 永続化スロット使用している

    m_requestParam.pClient->SetPersistenceInfo( slotId , true , it->GetDataId() );
  }

  if( m_requestParam.pClient->IsEmptySlotExists() )
  {//空きがある
     bool result = UploadDataStoreData();
     if( result == false )
     {
       SetSequenceNo( SEQ_ERROR );
     }
  }
  else
  {//空きがない
    SetSequenceNo( SEQ_NOT_EMPTY );
  }
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
void BattleVideoUploadRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQ_ERROR );
}

//------------------------------------------------------------------
/**
  * @brief ポケモン不正チェック処理の完了イベントを検知
  *
  * @note  本イベント終了後、[checkResult]配列、および[certData]は解放される
  *
  * @param[in]  checkResult  個々のポケモンの検査結果が配列で格納される。
  * @param[in]  resultNum    検査結果配列の数
  * @param[in]  certData     署名データ
  * @param[in]  certDataSize 署名データサイズ。この値が0の場合は、不正なポケモンデータであることを指す。
  */
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::OnPokemonValidationSuccess( const nn::nex::qInt32* checkResult, const u32 resultNum,
                                        const u8* certData, const u32 certDataSize )
{
  if( certDataSize == 0 )
  {//不正
    if( m_pListener )
    {
      m_pListener->OnUploadInvalid();
    }
  }
  else
  {//正常
    SetSequenceNo( SEQ_DATASTORE );
  }
}

//------------------------------------------------------------------
/**
* @brief ポケモン不正チェック処理のエラーイベントを検知
*
* @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
*
* @param[in] httpStatus サーバから返却されたHTTPステータスコード
*/
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::OnPokemonValidationError( const s32 httpStatus )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultValidation5xx( httpStatus );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQ_ERROR );
}

//------------------------------------------------------------------
/**
* @brief ポケモン不正チェック処理のエラーイベントを検知
*
* @param[in] result    エラー内容
* @param[in] errorCode ネットワークエラーコード
*/
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::OnPokemonValidationError( const nn::nex::qResult &result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQ_ERROR );
}

//------------------------------------------------------------------
/**
* @brief ポケモン不正チェック処理のキャンセルイベントを検知
*/
//------------------------------------------------------------------
void BattleVideoUploadRequestSequence::OnPokemonValidationCanceled( void )
{
  SetSequenceNo( SEQ_ERROR );
}



#endif

GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)
