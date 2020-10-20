// ============================================================================
/*
 * @file BattleVideoSyncRequestSequence.h
 * @brief 保存しているバトルビデオとサーバの同期を行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( BATTLEVIDEOSYNCREQUESTSEQUENCE_H_INCLUDE )
#define BATTLEVIDEOSYNCREQUESTSEQUENCE_H_INCLUDE
#pragma once
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include <macro/include/gfl2_macros.h>
#include <NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoResponseListener.h>
#include <NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoRequestClient.h>

#include <gflnet2/include/nex/gflnet2_NexDataStoreClient.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClientListener.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideo)

//------------------------------------------------------------------
/**
* @brief  保存しているバトルビデオとサーバの同期を行うリクエスト
*/
//------------------------------------------------------------------
class BattleVideoSyncRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase, public gflnet2::nex::NexDataStoreClientListener
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleVideoSyncRequestSequence );
private:

  enum
  {
    DUMMY_WAIT = 30,//ダミーウェイト1秒
    MAX_UPDATE_NUM = 10,
  };

  enum
  {
    SEQ_INIT = 0,
    SEQ_PARSISTENCE_WAIT,
    SEQ_END,
    SEQ_ERROR
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief  コンストラクタ/デストラクタ
  */
  //------------------------------------------------------------------
  BattleVideoSyncRequestSequence();
  virtual~BattleVideoSyncRequestSequence();

public://RequestSequenceBaseのオーバーライド

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングされる
  *			      継承して実装する通信シーケンス本体
  *           クラスに用意されているセッター/ゲッターを使用して個別の
  *           通信シーケンスを実装してください
  *           キャンセル操作が行われた後はこのアップデートは呼ばれなくなり、
  *           UpdateCancelが呼ばれるようになります。
  */
  //------------------------------------------------------------------
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update( void );

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
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel( void );


  //------------------------------------------------------------------
  /**
  * @brief  リスナー登録解除
  */
  //------------------------------------------------------------------
  virtual void RemoveListener( void );

  //------------------------------------------------------------------
  /**
  * @brief  リスナー設定
  */
  //------------------------------------------------------------------
  void SetListener( BattleVideoResponseListener* pListener );

  //------------------------------------------------------------------
  /**
  * @brief  リクエストパラメータ設定
  */
  //------------------------------------------------------------------
  void SetRequestParam( BattleVideoRequestClient::SYNC_REQUEST_PARAM& pParam );


public:
#ifdef GF_PLATFORM_CTR

  //--------------------------------------------------------------------------
  /**
   * @brief     公開中の永続化情報検索
   * @return    関数に成功するとtrue
   */
  //--------------------------------------------------------------------------
  bool PersistenceStoreData( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   ストレージデータの削除
   * @param[in]  dataIds  削除対象データIDリスト
   * @return  関数の成否 成功でtrue
   */
  //--------------------------------------------------------------------------
  bool DeleteDataStoreData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds );

  //------------------------------------------------------------------
  /**
   * @brief メタデータをダウンロードする(非同期通信）
   *
   * @param[in]  dataId  取得対象データID
   *
   * @return 実行結果。成功時はtrueが返却される。
   */
  //------------------------------------------------------------------
  bool GetMetaStoreData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds );

  //------------------------------------------------------------------
  /**
   * @brief データ削除の完了通知
   *
   * @param[in] results 実行結果リスト
   */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreDeleteDataFinish( const nn::nex::qVector<nn::nex::qResult>& results );

  //------------------------------------------------------------------
  /**
   * @brief メタデータダウンロードの完了通知
   *
   * @param[in] results 実行結果リスト
   * @param[in] metas   取得したメタデータリスト
   */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreDownloadMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results,
                                                    const nn::nex::qVector<nn::nex::DataStoreMetaInfo>& metas );

  //------------------------------------------------------------------
  /**
  * @brief 永続化情報取得の成功通知
  *
  * @param[in] persistenceInfoList 永続化情報リスト
  * @param[in] results 実行結果リスト
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreGetPersistenceInfo( const nn::nex::qVector< nn::nex::DataStorePersistenceInfo >& persistenceInfoList, const nn::nex::qVector<nn::nex::qResult>& results );


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
  virtual void OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode );
#endif
private:

  BattleVideoResponseListener*           m_pListener;
  BattleVideoRequestClient::SYNC_REQUEST_PARAM m_requestParam;
  u32 m_dummyWaitCount;

#ifdef GF_PLATFORM_CTR
  nn::nex::qVector<nn::nex::qUnsignedInt64> m_enableDataIds;
  nn::nex::qVector<nn::nex::qUnsignedInt64> m_deleteDataIds;
#endif

  u32 m_endSeq;
  u32 m_nowSaveDataNum;

  NetAppLib::BattleVideoPlayer::VIDEO_DATA* m_updateDataArray[MAX_UPDATE_NUM];
  u32 m_updateCount;
};


GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)

#endif // BATTLEVIDEOSYNCREQUESTSEQUENCE_H_INCLUDE
