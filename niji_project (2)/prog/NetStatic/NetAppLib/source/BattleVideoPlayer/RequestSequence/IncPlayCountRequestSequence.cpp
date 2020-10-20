// ============================================================================
/*
 * @file IncPlayCountRequestSequence.cpp
 * @brief 再生回数をインクリメントするリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/IncPlayCountRequestSequence.h"

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
IncPlayCountRequestSequence::IncPlayCountRequestSequence()
: m_pListener( NULL )
, m_dummyWaitCount(0)
{
  memset( &m_requestParam , 0 , sizeof(m_requestParam) );
  BattleVideoRequestClient::BindDataStoreClient( this );
}
IncPlayCountRequestSequence::~IncPlayCountRequestSequence()
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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE IncPlayCountRequestSequence::Update( void )
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
      nn::nex::DataStoreRatingTarget target;
      target.SetDataId( m_requestParam.dataID );
      target.SetSlot(1);  //
      nn::nex::DataStoreRateObjectParam param;// 評価パラメータ デフォルト1
      bool result = SetRatingStoreData( target, param );
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
    break;
  case SEQ_END:
    {
      if( m_pListener )
      {
        m_pListener->OnIncPlayCountSuccess();
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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE IncPlayCountRequestSequence::UpdateCancel( void )
{
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//------------------------------------------------------------------
/**
* @brief  リスナー登録解除
*/
//------------------------------------------------------------------
void IncPlayCountRequestSequence::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  リスナー設定
*/
//------------------------------------------------------------------
void IncPlayCountRequestSequence::SetListener( BattleVideoResponseListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief  リクエストパラメータ設定
*/
//------------------------------------------------------------------
void IncPlayCountRequestSequence::SetRequestParam( BattleVideoRequestClient::INC_REQUEST_PARAM& pParam )
{
  m_requestParam = pParam;
}

#ifdef GF_PLATFORM_CTR

//------------------------------------------------------------------
/**
 * @brief データへ評価をセットする(非同期通信）
 *
 * @param[in]  target  評価設定対象データ
 * @param[in]  param   評価設定パラメータ
 *
 * @return 実行結果。成功時はtrueが返却される。
 */
//------------------------------------------------------------------
bool IncPlayCountRequestSequence::SetRatingStoreData( const nn::nex::DataStoreRatingTarget& target, const nn::nex::DataStoreRateObjectParam& param )
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
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"DataStoreClient::RateObject() \n" );
  #endif

  if( dataStoreClient->SetRating( target, param ) )
  {
    GFL_PRINT("SetRatingStoreData success\n");
    return true;
  }

  GFL_PRINT("SetRatingStoreData failed\n");
  return false;
}

//------------------------------------------------------------------
/**
 * @brief データ評価設定の成功通知
 *
 * @param[in] slotId   対象スロット番号
 * @param[in] rateInfo 更新された最新の評価情報
 */
//------------------------------------------------------------------
void IncPlayCountRequestSequence::OnNexDataStoreSetRatingSuccess( const nn::nex::qInt8 slotId, nn::nex::DataStoreRatingInfo& rateInfo )
{
  GFL_PRINT("OnNexDataStoreSetRatingSuccess\n");

  GFL_PRINT("評価スロット %d\n", slotId );
  GFL_PRINT("評価合計値:%d\n", rateInfo.GetTotalValue());
  GFL_PRINT("評価された回数:%d\n", rateInfo.GetCount());
  GFL_PRINT("評価合計値の初期値:%d\n", rateInfo.GetInitialValue());

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
void IncPlayCountRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  if( result == QERROR(DataStore, NotFound) )
  {//データがない
    GFL_PRINT("OnNexDataStoreClient : not found\n");
    SetSequenceNo( SEQ_END );
  }
  else if( result == QERROR(DataStore, OperationNotAllowed) )
  {//評価失敗
    GFL_PRINT("OnNexDataStoreClient : rating failed\n");
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
