#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexDataStoreClientListener.h
 * @brief  DataStoreClient関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_DATA_STORE_CLIENT_LISTENER_H__
#define __GFLNET_NEX_DATA_STORE_CLIENT_LISTENER_H__

#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief データストア通知イベントハンドラ
 *
 * @note データストア関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexDataStoreClientListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexDataStoreClientListener); //コピーコンストラクタ＋代入禁止
public:
  //------------------------------------------------------------------
  /**
  * @brief アップロードの成功通知
  *
  * @param[in] dataId アップロードデータに割り当てられたユニークID。
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreUploadSuccess( const nn::nex::qUnsignedInt64 dataId ){};

  //------------------------------------------------------------------
  /**
  * @brief 更新の成功通知
  *
  * @param[in] dataId アップロードデータに割り当てられたユニークID。
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreUpdateSuccess( const nn::nex::qUnsignedInt64 dataId ){};

  //------------------------------------------------------------------
  /**
  * @brief データ検索の成功通知
  *
  * @param[in] searchInfo 検索結果情報
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreSearchSuccess( const nn::nex::DataStoreSearchResult& searchInfo ){};

  //------------------------------------------------------------------
  /**
  * @brief ストレージサーバからのデータダウンロード成功通知
  *
  * @param[in] downloadFileSize  取得したファイルサイズ
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreDownloadDataSuccess( u32 downloadFileSize ){};

  //------------------------------------------------------------------
  /**
  * @brief ストレージサーバのデータ参照カウンタ値更新完了通知
  *
  * @param[in] results 実行結果
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreTouchSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief メタデータダウンロードの完了通知
  *
  * @param[in] results 実行結果リスト
  * @param[in] metas   取得したメタデータリスト
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreDownloadMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results,
                                                     const nn::nex::qVector<nn::nex::DataStoreMetaInfo>& metas ){};

  //------------------------------------------------------------------
  /**
  * @brief メタデータ更新の完了通知
  *
  * @param[in] results 実行結果リスト
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreChangeMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results ){};

  //------------------------------------------------------------------
  /**
  * @brief データ削除の完了通知
  *
  * @param[in] results 実行結果リスト
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreDeleteDataFinish( const nn::nex::qVector<nn::nex::qResult>& results ){};

  //------------------------------------------------------------------
  /**
  * @brief データ評価リセットの完了通知
  *
  * @param[in] results 実行結果リスト
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreResetRatingFinish( const nn::nex::qVector<nn::nex::qResult>& results ){};

  //------------------------------------------------------------------
  /**
  * @brief データ評価設定の成功通知
  *
  * @param[in] slotId   対象スロット番号
  * @param[in] rateInfo 更新された最新の評価情報
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreSetRatingSuccess( const nn::nex::qInt8 slotId, nn::nex::DataStoreRatingInfo& rateInfo ){};

  //------------------------------------------------------------------
  /**
  * @brief データ評価取得の成功通知
  *
  * @param[in] rateInfos データ評価情報リスト
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreGetRatingSuccess( const nn::nex::qVector< nn::nex::qMap<nn::nex::qInt8,nn::nex::DataStoreRatingInfo> >& rateInfos ){};

  //------------------------------------------------------------------
  /**
  * @brief 永続化情報取得の成功通知
  *
  * @param[in] persistenceInfoList 永続化情報リスト
  * @param[in] results 実行結果リスト
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreGetPersistenceInfo( const nn::nex::qVector< nn::nex::DataStorePersistenceInfo >& persistenceInfoList, const nn::nex::qVector<nn::nex::qResult>& results ){};

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
  virtual void OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief NexDataStoreClientのキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnNexDataStoreClientCanceled( void ){};
};


} // nex
} // gflnet



#endif // __GFLNET_NEX_DATA_STORE_CLIENT_LISTENER_H__
#endif
