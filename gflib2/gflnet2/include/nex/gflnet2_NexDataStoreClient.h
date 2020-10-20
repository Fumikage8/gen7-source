#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexDataStoreClient.h
 *
 * @brief  nn::nex::DataStoreClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_DATASTORE_CLIENT_H__
#define __GFLNET_NEX_DATASTORE_CLIENT_H__

#pragma once

#include <nex.h>
#include <nex_P2p.h>

#include <nn/friends.h>
#include <RendezVous/Services/Messaging.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClientListener.h>

namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/**
*  @brief データストア（DataStoreClientアクセス）用クラス
*
*  @attention 事前にゲームサーバへログインしておくこと
*  @attention 各非同期通信は排他制御となる為、同時実行は不可。
*
**/
//------------------------------------------------------------------
class NexDataStoreClient : public nn::nex::RootObject
{
  GFL_FORBID_COPY_AND_ASSIGN(NexDataStoreClient); //コピーコンストラクタ＋代入禁止

public:
  /**
   * @brief 拡張情報の取得指定用フラグ
   */
  typedef enum
  {
    DL_TARGET_BASIC      = 0,                                                    //!< 基本情報のみ
    DL_TARGET_TAGS       = nn::nex::DataStoreConstants::RESULT_FLAG_TAGS,        //!< タグ
    DL_TARGET_RATINGS    = nn::nex::DataStoreConstants::RESULT_FLAG_RATINGS,     //!< 評価
    DL_TARGET_METABINARY = nn::nex::DataStoreConstants::RESULT_FLAG_METABINARY,  //!< バイナリデータ
    DL_TRGET_ALL         = (DL_TARGET_METABINARY|DL_TARGET_TAGS|DL_TARGET_METABINARY)
  }DOWNLOAD_TARGET;

public:
  NexDataStoreClient( void );
  virtual ~NexDataStoreClient();

public:
  //------------------------------------------------------------------
  /**
  * @brief  DataStoreClientの初期化
  *
  * @param[in]  credentials     ゲームサーバーログイン情報
  *
  * @return 初期化に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool Initialize( nn::nex::Credentials* credentials );

  //------------------------------------------------------------------
  /**
  * @brief  DataStoreClientの終了
  *
  * @attention 必ず事前にIsConnecting()がfalseであることを確認してください
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
  * @brief  DataStoreClientインスタンスの取得
  */
  //------------------------------------------------------------------
  nn::nex::DataStoreClient* GetDataStoreClient( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief データストア用リスナの登録
  *
  * @param[in] listener 登録対象のデータストア用リスナ。
  *
  * @return 登録に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool RegistDataStoreClientListener( NexDataStoreClientListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief データストア用リスナの解除
  *
  * @param[in] listener 削除対象のデータストア用リスナ。
  */
  //------------------------------------------------------------------
  void UnregistDataStoreClientListener( NexDataStoreClientListener* listener );

public:
  //------------------------------------------------------------------
  /**
  * @brief データをストレージサーバへアップロードする(非同期通信）
  *
  * @attention アップロードできるサイズは10MBバイトまで。
  * @attention 永続化情報を設定しない場合、データは有効期限経過後にサーバから削除される。
  *
  * @param[in]  param  アップロード詳細情報
  * @param[in]  buffer アップロードするデータ。DataStorePreparePostParam::SetSize()で指定したサイズがアップロードされる。
  * @param[in]  useInternalBuffer trueを指定すると、bufferの内容がNEX用メモリにコピーされる。false指定時はbufferポインタを参照し続ける。
  *
  * @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreUploadSuccess()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用するため低速。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool UploadData( const nn::nex::DataStorePostParam& param, const void* buffer, bool useInternalBuffer=false );

  //------------------------------------------------------------------
  /**
  * @brief 非永続データをストレージサーバからダウンロードする(非同期通信）
  *
  * @attention ダウンロードできるサイズは10MBバイトまで。
  * @attention 参照権限を持っている人のみダウンロードが可能。
  *
  * @param[in]  dataId     ダウンロードしたいデータID
  * @param[in]  buffer     ダウンロードしたデータ格納先バッファへ
  * @param[in]  bufferSize ダウンロードしたいデータサイズ。
  *
  * @note  終了時に「NexDataStoreClientListener::OnNexDataStoreDownloadDataFinish()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用するため低速。
  * @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool DownloadData( const nn::nex::qUnsignedInt64 dataId, void* buffer, nn::nex::qUnsignedInt32 bufferSize );

  //------------------------------------------------------------------
  /**
  * @brief 永続データをストレージサーバからダウンロードする(非同期通信）
  *
  * @attention ダウンロードできるサイズは10MBバイトまで。
  * @attention 参照権限を持っている人のみダウンロードが可能。
  *
  * @param[in]  pid        ダウンロード対象データ所有者のプリンシバルID
  * @param[in]  slotId     ダウンロード対象データのスロットID
  * @param[in]  bufferSize ダウンロードしたいデータサイズ。
  *
  * @note  終了時に「NexDataStoreClientListener::OnNexDataStoreDownloadDataFinish()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用するため低速。
  * @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool DownloadPersistenceData( const nn::friends::PrincipalId& pid, const nn::nex::qInt8 slotId,
                                void* buffer, nn::nex::qUnsignedInt32 bufferSize );

  //------------------------------------------------------------------
  /**
  * @brief ストレージサーバのデータを更新する(非同期通信）
  *
  * @attention 更新できるサイズは10MBバイトまで。
  *
  * @param[in]  param  更新詳細情報
  * @param[in]  buffer 更新に使用するデータ。DataStoreUpdateParam::SetSize()で指定したサイズがアップロードされる。
  * @param[in]  useInternalBuffer trueを指定すると、bufferの内容がNEX用メモリにコピーされる。false指定時はbufferポインタを参照し続ける。
  *
  * @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreUpdateSuccess()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用するため低速。
  * @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool UpdateData( const nn::nex::DataStoreUpdateParam& param, const void* buffer, bool useInternalBuffer );

  //------------------------------------------------------------------
  /**
  * @brief データをストレージサーバから削除する(非同期通信）
  *
  * @attention １度に削除できるデータ数は100件まで。
  *
  * @param[in]  dataIds  削除対象データIDリスト
  *
  * @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreDeleteDataFinish()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool DeleteData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds );

  //------------------------------------------------------------------
  /**
  * @brief ストレージサーバからデータを検索する(非同期通信）
  *
  * @attention 参照権限を持っているデータのみを検索する。
  *
  * @param[in]  param  検索詳細情報
  *
  * @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreSearchSuccess()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用するため低速。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool SearchData( const nn::nex::DataStoreSearchParam& param );

  //------------------------------------------------------------------
  /**
  * @brief ストレージサーバのデータ参照カウンタ値を更新する(非同期通信）
  *
  * @attention 参照権限を持っているデータのみ更新可能。
  *
  * @param[in]  param  更新詳細情報
  *
  * @note  終了時は「NexDataStoreClientListener::OnNexDataStoreTouchFinish()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用するため低速。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool TouchData( const nn::nex::DataStoreTouchObjectParam& param );

public:
  //------------------------------------------------------------------
  /**
  * @brief メタデータをアップロードする(非同期通信）
  *
  * @attention アップロードできるサイズは1024バイトまで。
  * @attention 永続化情報を設定しない場合、データは有効期限経過後にサーバから削除される。
  *
  * @param[in]  param  アップロード詳細情報
  * @param[in]  buffer アップロードするデータ。DataStorePreparePostParam::SetSize()で指定したサイズがアップロードされる。
  * @param[in]  useInternalBuffer trueを指定すると、bufferの内容がNEX用メモリにコピーされる。false指定時はbufferポインタを参照し続ける。
  *
  * @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreUploadSuccess()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用しないため高速動作が可能。
  * @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool UploadMetaData( const nn::nex::DataStorePostParam& param );

  //------------------------------------------------------------------
  /**
  * @brief 非永続メタデータを一括ダウンロードする(非同期通信）
  *
  * @attention １度にダウンロードできるメタデータ数は100件まで。
  *
  * @param[in]  dataIds   ダウンロード対象データIDリスト
  * @paran[in]  targetBit 拡張情報の取得指定用フラグ。「DOWNLOAD_TARGET」の論理和で指定する。
  *
  * @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreDownloadMetaDataFinish()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用しないため高速動作が可能。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool DownloadMetaData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds,
                         const nn::nex::qUnsignedInt32 targetBit=DL_TARGET_BASIC );

  //------------------------------------------------------------------
  /**
  * @brief 永続メタデータをダウンロードする(非同期通信）
  *
  * @param[in]  pid       ダウンロード対象データ所有者のプリンシバルID
  * @param[in]  slotId    ダウンロード対象データのスロットID
  * @paran[in]  targetBit 拡張情報の取得指定用フラグ。「DOWNLOAD_TARGET」の論理和で指定する。
  *
  * @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreDownloadMetaDataFinish()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用しないため高速動作が可能。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool DownloadPersistenceMetaData( const nn::friends::PrincipalId& pid,
                                    const nn::nex::qInt8 slotId,
                                    const nn::nex::qUnsignedInt32 targetBit=DL_TARGET_BASIC );

  //------------------------------------------------------------------
  /**
  * @brief メタデータを一括更新する(非同期通信）
  *
  * @attention １度に更新できるメタデータ数は100件まで。
  *
  * @param[in]  param    更新詳細情報リスト
  *
  * @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreChangeMetaDataFinish()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  * @note  ストレージサーバを使用しないため高速動作が可能。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool CheageMetaData( const nn::nex::qVector<nn::nex::DataStoreChangeMetaParam>& param );

public:
  //------------------------------------------------------------------
  /**
  * @brief データへの評価をリセットする(非同期通信）
  *
  * @attention １度にリセットできるデータ数は100件まで。
  *
  * @param[in]  dataIds  評価リセット対象データIDリスト
  *
  * @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreResetRatingFinish()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool ResetRating( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds );

  //------------------------------------------------------------------
  /**
  * @brief データへの評価を取得する(非同期通信）
  *
  * @attention １度に取得できるデータ数は100件まで。
  *
  * @param[in]  dataIds  評価取得対象データIDリスト
  *
  * @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreGetRatingSuccess()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool GetRating( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds );

  //------------------------------------------------------------------
  /**
  * @brief データへ評価をセットする(非同期通信）
  *
  * @attention 評価を行うには、予め該当のデータスロットが初期化されている必要がある。
  * @attention 初期化は DataStoreClient::UploadData()する際に、DataStorePreparePostParam::SetRatingSetting()で行う。
  *
  * @param[in]  target  評価設定対象データ
  * @param[in]  param   評価設定パラメータ
  *
  * @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreSetRatingSuccess()」がコールされる。
  * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
  *
  * @return 実行結果。成功時はtrueが返却される。
  */
  //------------------------------------------------------------------
  bool SetRating( const nn::nex::DataStoreRatingTarget& target, const nn::nex::DataStoreRateObjectParam& param );

  //------------------------------------------------------------------
  /**
   * @brief 永続化情報を一括取得する（非同期通信）
   *
   * @param[in] pid     永続化情報を取得したい人のプリンシパルID
   * @param[in] slotIds 永続化スロットIDのリスト
   *
   * @return 実行結果。成功時はtrueが返却される。
   */
  //------------------------------------------------------------------
  bool GetPersistenceInfo( const nn::friends::PrincipalId& pid, const nn::nex::qVector<nn::nex::qUnsignedInt16>& slotIds );

public:
  //------------------------------------------------------------------
  /**
   * @brief 通信中か判定
   *
   * @return 通信中ならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsConnecting( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する。
   * @brief キャンセルが実施された場合、完了時にOnNexRatingClientCanceled()がコールされる。
   *
   * @note  通信状態にかかわらず、コールしても良い。
   *
   * @return キャンセル処理が実施されたならtrueを返却する
   */
  //------------------------------------------------------------------
  bool CancelConnecting( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief  メタデータのログ出力（デバッグ用）
   *
   * @param[in]  metaInfo ログ出力対象となるメタデータ
   */
  //------------------------------------------------------------------
  static void PrintGetMetaResult( const nn::nex::DataStoreMetaInfo& metaInfo );

public:
  //------------------------------------------------------------------
  /**
   * @brief  非同期処理の完了通知検知用関数
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   */
  //------------------------------------------------------------------
  void OnUploadCompletion(nn::nex::CallContext *pContext);
  void OnUpdateCompletion(nn::nex::CallContext *pContext);
  void OnDownloadDataCompletion(nn::nex::CallContext *pContext);
  void OnDownloadMetaCompletion(nn::nex::CallContext *pContext);
  void OnChangeMetaCompletion(nn::nex::CallContext *pContext);
  void OnDeleteCompletion(nn::nex::CallContext *pContext);
  void OnSearchCompletion(nn::nex::CallContext *pContext);
  void OnTouchCompletion(nn::nex::CallContext *pContext);
  void OnResetRatingCompletion(nn::nex::CallContext *pContext);
  void OnGetRatingCompletion(nn::nex::CallContext *pContext);
  void OnSetRatingCompletion(nn::nex::CallContext *pContext);
  void OnGetPersistenceInfo(nn::nex::CallContext *pContext);

private:
  bool GetMetaData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds, const nn::nex::DataStoreGetMetaParam& param );
  bool GetData( const nn::nex::DataStoreGetParam& param, void* buffer, nn::nex::qUnsignedInt32 bufferSize );
  bool LockConnection( void );
  void UnlockConnection( void );
  void ReportError( const nn::nex::qResult& result );
  void ReportFunctionError( void );
  void ReportCanceled( void );

private:
  static const u32                              LISTENER_NUM = 4;         //!< リスナ最大登録可能数

private:
  bool                                          m_isInit;                 //!< 初期化フラグ
  bool                                          m_isAccess;               //!< 通信中フラグ
  u32                                           m_writeSize;              //!< 書き込みサイズ
  //
  nn::nex::DataStoreClient*                     m_dataStoreClient;        //!< DataStoreクライアント
  nn::nex::ProtocolCallContext*                 m_context;                //!< カレントコールコンテキスト
  //
  NexDataStoreClientListener*                   m_listener[LISTENER_NUM]; //!< データストア用リスナ
  //
  nn::nex::DataStoreSearchResult*               m_searchInf;              //!< データ検索結果
  nn::nex::qUnsignedInt64                       m_dataId;                 //!< 発行されたデータID
  nn::nex::qVector<nn::nex::qResult>*           m_results;                //!< qResult用リスト
  nn::nex::qVector<nn::nex::DataStoreMetaInfo>* m_metas;                  //!< メタデータ用リスト
  //
  nn::nex::qVector< nn::nex::qMap<nn::nex::qInt8,nn::nex::DataStoreRatingInfo> >* m_ratingList;  //!< 評価情報リスト
  nn::nex::qInt8                                                                  m_rateSlotId;  //!< 評価スロットID
  nn::nex::DataStoreRatingInfo*                                                   m_ratingInf;   //!< 評価情報
  //
  nn::nex::qVector< nn::nex::DataStorePersistenceInfo > *m_persistenceInfoList; //!< 永続化情報のリスト
};



} // nex
} // gflnet

#endif  // __GFLNET_NEX_DATASTORE_CLIENT_H__
#endif
