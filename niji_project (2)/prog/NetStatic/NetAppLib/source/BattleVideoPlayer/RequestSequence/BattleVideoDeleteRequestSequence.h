// ============================================================================
/*
 * @file BattleVideoDeleteRequestSequence.h
 * @brief バトルビデオの削除を行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( BATTLEVIDEODELETEREQUESTSEQUENCE_H_INCLUDE )
#define BATTLEVIDEODELETEREQUESTSEQUENCE_H_INCLUDE
#pragma once
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include <macro/include/gfl2_macros.h>
#include <NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoResponseListener.h>
#include <NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoRequestClient.h>

#include <gflnet2/include/nex/gflnet2_NexDataStoreClient.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClientListener.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideo)

//削除用パラメータ（削除と公開取り消しを共通で行う）
struct DELETE_REQUEST_PARAM_INTERNAL
{
  ExtSavedata::BattleRecorderSaveData* pBattleRecorderSaveData;//バトルビデオデータ全体（このクラスは大きいので、通信モジュール側でコピーしません）
  u32 deleteNum;//削除する数
  u32* indexArray;//インデックスの配列
  ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA** dataArray;//削除するデータのID配列（dataIDArray[deleteNum - 1]までアクセス可能にしてください）
  ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pCancelData;
  u32 cancelDataIndex;
  bool isOnline;//オンラインかどうか（オンラインだったら、サーバからも削除する）
  bool isLocalDelete;//SDカードのデータも削除するかどうか
  BattleVideoRequestClient* pClient;
};

//------------------------------------------------------------------
/**
* @brief  バトルビデオの削除を行うリクエスト
*/
//------------------------------------------------------------------
class BattleVideoDeleteRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase , public gflnet2::nex::NexDataStoreClientListener
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleVideoDeleteRequestSequence );
private:

  enum
  {
    DUMMY_WAIT = 30,//ダミーウェイト1秒
    MAX_DATA_NUM = 100,
  };

  enum
  {
    SEQ_INIT = 0,
    SEQ_WAIT,
    SEQ_END,
    SEQ_ERROR
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief  コンストラクタ/デストラクタ
  */
  //------------------------------------------------------------------
  BattleVideoDeleteRequestSequence();
  virtual~BattleVideoDeleteRequestSequence();

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
  void SetRequestParam( DELETE_REQUEST_PARAM_INTERNAL& pParam );

public:
#ifdef GF_PLATFORM_CTR


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
   * @brief データ削除の完了通知
   *
   * @param[in] results 実行結果リスト
   */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreDeleteDataFinish( const nn::nex::qVector<nn::nex::qResult>& results );

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
  u32 m_deleteCount;
  DELETE_REQUEST_PARAM_INTERNAL m_requestParam;
  u32 m_indexArray[MAX_DATA_NUM];

  u32 m_dummyWaitCount;
  u32 m_endSeq;

};


GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)

#endif // BATTLEVIDEODELETEREQUESTSEQUENCE_H_INCLUDE
