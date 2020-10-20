// ============================================================================
/*
 * @file BattleVideoUploadRequestSequence.h
 * @brief バトルビデオの不正チェックとアップロードをするリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( BATTLEVIDEOUPLOADREQUESTSEQUENCE_H_INCLUDE )
#define BATTLEVIDEOUPLOADREQUESTSEQUENCE_H_INCLUDE
#pragma once
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include <macro/include/gfl2_macros.h>
#include <NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoResponseListener.h>
#include <NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoRequestClient.h>

#include <gflnet2/include/nex/gflnet2_NexDataStoreClient.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClientListener.h>

#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideo)

//------------------------------------------------------------------
/**
* @brief  バトルビデオの不正チェックとアップロードをするリクエスト
*/
//------------------------------------------------------------------
class BattleVideoUploadRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase , public gflnet2::nex::NexDataStoreClientListener
#if defined(GF_PLATFORM_CTR)
  ,public NetLib::Validation::PokemonValidationListener
#endif
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleVideoUploadRequestSequence );
private:

  enum
  {
    DUMMY_WAIT = 30,//ダミーウェイト1秒
  };

  enum
  {
    SEQ_INIT = 0,
    SEQ_VALIDATION_WAIT,
    SEQ_DATASTORE,
    SEQ_WAIT,
    SEQ_SAVE,
    SEQ_ERROR,
    SEQ_NOT_EMPTY
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief  コンストラクタ/デストラクタ
  */
  //------------------------------------------------------------------
  BattleVideoUploadRequestSequence();
  virtual~BattleVideoUploadRequestSequence();

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
  void SetRequestParam( BattleVideoRequestClient::UPLOAD_REQUEST_PARAM& pParam );

  //--------------------------------------------------------------------------
  /**
   * @brief     ストレージデータのアップロード
   *
   * @return    アップロードに成功したらtrue
   */
  //--------------------------------------------------------------------------
  bool UploadDataStoreData( void );
  bool UploadDataStoreDataForBattleSpot( void );

  //--------------------------------------------------------------------------
  /**
   * @brief     公開中の永続化情報検索
   * @return    関数に成功するとtrue
   */
  //--------------------------------------------------------------------------
  bool PersistenceStoreData( void );

public:

#ifdef GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
   * @brief アップロードの成功通知
   *
   * @param[in] dataId アップロードデータに割り当てられたユニークID。
   */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreUploadSuccess( const nn::nex::qUnsignedInt64 dataId );

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
   virtual void OnPokemonValidationSuccess( const nn::nex::qInt32* checkResult, const u32 resultNum,
                                            const u8* certData, const u32 certDataSize );

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のエラーイベントを検知
   *
   * @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
   *
   * @param[in] httpStatus サーバから返却されたHTTPステータスコード
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationError( const s32 httpStatus );

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のエラーイベントを検知
   *
   * @param[in] result    エラー内容
   * @param[in] errorCode ネットワークエラーコード
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationError( const nn::nex::qResult &result, u32 errorCode );

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のキャンセルイベントを検知
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationCanceled( void );


#endif

private:

  BattleVideoResponseListener*           m_pListener;
  BattleVideoRequestClient::UPLOAD_REQUEST_PARAM m_requestParam;
  u32 m_dummyWaitCount;
  bool m_isRequestEnd;

#if defined(GF_PLATFORM_CTR)
  NetLib::Validation::PokemonValidation* m_pValidator;
#endif

};


GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)

#endif // BATTLEVIDEOUPLOADREQUESTSEQUENCE_H_INCLUDE
