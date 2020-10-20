// ============================================================================
/*
 * @file SendPlayCompleteRequestSequence.h
 * @brief 再生完走を通知するリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( SENDPLAYCOMPLETEREQUESTSEQUENCE_H_INCLUDE )
#define SENDPLAYCOMPLETEREQUESTSEQUENCE_H_INCLUDE
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
* @brief  再生完走を通知するリクエスト
*/
//------------------------------------------------------------------
class SendPlayCompleteRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase, public gflnet2::nex::NexDataStoreClientListener
{
  GFL_FORBID_COPY_AND_ASSIGN( SendPlayCompleteRequestSequence );
private:

  enum
  {
    DUMMY_WAIT = 30,//ダミーウェイト1秒
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
  SendPlayCompleteRequestSequence();
  virtual~SendPlayCompleteRequestSequence();

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
  void SetRequestParam( BattleVideoRequestClient::COMPLETE_REQUEST_PARAM& pParam );


public:
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
  bool SetRatingStoreData( const nn::nex::DataStoreRatingTarget& target, const nn::nex::DataStoreRateObjectParam& param );

  //------------------------------------------------------------------
  /**
   * @brief データ評価設定の成功通知
   *
   * @param[in] slotId   対象スロット番号
   * @param[in] rateInfo 更新された最新の評価情報
   */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreSetRatingSuccess( const nn::nex::qInt8 slotId, nn::nex::DataStoreRatingInfo& rateInfo );

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
  BattleVideoRequestClient::COMPLETE_REQUEST_PARAM m_requestParam;
  u32 m_dummyWaitCount;

};


GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)

#endif // SENDPLAYCOMPLETEREQUESTSEQUENCE_H_INCLUDE
