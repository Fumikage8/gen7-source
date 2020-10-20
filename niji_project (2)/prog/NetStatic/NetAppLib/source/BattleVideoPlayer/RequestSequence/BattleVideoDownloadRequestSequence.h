// ============================================================================
/*
 * @file BattleVideoDownloadRequestSequence.h
 * @brief バトルビデオの検索とダウンロードを行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( BATTLEVIDEODOWNLOADREQUESTSEQUENCE_H_INCLUDE )
#define BATTLEVIDEODOWNLOADREQUESTSEQUENCE_H_INCLUDE
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
* @brief  バトルビデオの検索とダウンロードを行うリクエスト
*/
//------------------------------------------------------------------
class BattleVideoDownloadRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase , public gflnet2::nex::NexDataStoreClientListener
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleVideoDownloadRequestSequence );
private:

  enum
  {
    DUMMY_WAIT = 30 * 6,//ダミーウェイト6秒
  };

  enum
  {
    SEQ_INIT = 0,
    SEQ_WAIT,
    SEQ_ERROR
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief  コンストラクタ/デストラクタ
  */
  //------------------------------------------------------------------
  BattleVideoDownloadRequestSequence();
  virtual~BattleVideoDownloadRequestSequence();

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
  void SetRequestParam( BattleVideoRequestClient::DOWNLOAD_REQUEST_PARAM& pParam );

#ifdef GF_PLATFORM_CTR
  //--------------------------------------------------------------------------
  /**
   * @brief   ストレージデータのダウンロード
   * @param[in]  dataIds  ダウンロードしたいデータID
   * @return  関数の成否 成功でtrue
   */
  //--------------------------------------------------------------------------
  bool DownloadDataStoreData( const nn::nex::qUnsignedInt64 dataId );

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

#endif

public:

#ifdef GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
  * @brief ストレージサーバからのデータダウンロード成功通知
  *
  * @param[in] downloadFileSize  取得したファイルサイズ
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreDownloadDataSuccess( u32 downloadFileSize );

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
  BattleVideoRequestClient::DOWNLOAD_REQUEST_PARAM m_requestParam;
  u32 m_dummyWaitCount;
  bool m_isRequestEnd;
  bool m_isDownloadSuccess;

  u32 m_playCount;
  ExtSavedata::BattleRecorderSaveData::REC_DATE m_date;

};


GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)

#endif // BATTLEVIDEODOWNLOADREQUESTSEQUENCE_H_INCLUDE
