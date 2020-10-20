// ============================================================================
/*
 * @file BattleVideoRequestClient.cpp
 * @brief BattleVideoアプリで必要な通信リクエストを投げる為のクラスです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( BATTLEVIDEOREQUESTCLIENT_H_INCLUDE )
#define BATTLEVIDEOREQUESTCLIENT_H_INCLUDE
#pragma once

#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoResponseListener.h"

#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClientListener.h>


#include <NetStatic/NetLib/include/NetDefine.h>

#ifdef GF_PLATFORM_CTR
#include <nex.h>
#endif



//--------------------------------------------------------------------------
/**
  */
//--------------------------------------------------------------------------

#ifdef GF_PLATFORM_CTR
#else
namespace gflnet2
{
  namespace nex
  {
    class NexDataStoreClientListener
    {
    public:
      NexDataStoreClientListener(){};
      ~NexDataStoreClientListener(){};
    };
  }
}
#endif


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)
struct VIDEO_DATA;
/**
  Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
    デバッグ機能の追加
*/
#if PM_DEBUG
  GFL_NAMESPACE_BEGIN(Debug)
    class VideoPlayerDataDebug;
  GFL_NAMESPACE_END(Debug)
#endif
GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)

GFL_NAMESPACE_BEGIN(BattleSpot)
class BattleSpotWork;
GFL_NAMESPACE_END(BattleSpot)

GFL_NAMESPACE_BEGIN(BattleVideo)





//--------------------------------------------------------------------------------------------
/**
  * @brief   チーム選択アプリ用のリクエストクライアント
  */
//--------------------------------------------------------------------------------------------
class BattleVideoRequestClient
{
public:

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   リクエストパラメータ群
   */
  //--------------------------------------------------------------------------------------------

  //同期用リクエストパラメータ
  struct SYNC_REQUEST_PARAM
  {
    ExtSavedata::BattleRecorderSaveData* pBattleRecorderSaveData;//バトルビデオデータ全体（このクラスは大きいので、通信モジュール側でコピーしません）
    NetAppLib::BattleVideoPlayer::VIDEO_DATA** pDataList;//アプリ側でロードしたビデオデータリスト
    u32 dataListNum;//ビデオデータの数
    BattleVideoRequestClient* pClient;
  };

  //削除用リクエストパラメータ
  struct DELETE_REQUEST_PARAM
  {
    ExtSavedata::BattleRecorderSaveData* pBattleRecorderSaveData;//バトルビデオデータ全体（このクラスは大きいので、通信モジュール側でコピーしません）
    u32 deleteNum;//削除する数
    u32* indexArray;//インデックスの配列
    ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA** dataArray;//削除するデータのID配列（dataIDArray[deleteNum - 1]までアクセス可能にしてください）
    bool isOnline;//オンラインかどうか（オンラインだったら、サーバからも削除する）
    BattleVideoRequestClient* pClient;
  };

  //公開取り消し用リクエストパラメータ
  struct CANCEL_UPLOAD_REQUEST_PARAM
  {
    ExtSavedata::BattleRecorderSaveData* pBattleRecorderSaveData;//バトルビデオデータ全体（このクラスは大きいので、通信モジュール側でコピーしません）
    u32 index;//セーブデータのインデックス
    ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pCancelUploadData;//公開取り消しするデータ
    BattleVideoRequestClient* pClient;
  };

  //アップロード用リクエストパラメータ
  struct UPLOAD_REQUEST_PARAM
  {
    ExtSavedata::BattleRecorderSaveData* pBattleRecorderSaveData;//バトルビデオデータ全体（このクラスは大きいので、通信モジュール側でコピーしません）
    u32 index;//セーブデータのインデックス
    ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pUploadData;//アップロードするデータ
    BattleVideoRequestClient* pClient;
  };

  //ダウンロード用リクエストパラメータ
  struct DOWNLOAD_REQUEST_PARAM
  {
    u64 dataID;//ダウンロードするデータID
    BattleVideoRequestClient* pClient;
    /**
      Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
        デバッグ機能の追加
    */
#if PM_DEBUG
    NetAppLib::BattleVideoPlayer::Debug::VideoPlayerDataDebug*  pDebug;
#endif

  };

  //再生回数インクリメント用リクエストパラメータ
  struct INC_REQUEST_PARAM
  {
    u64 dataID;//対象のデータID
    BattleVideoRequestClient* pClient;
  };

  // 再生完走通知用リクエストパラメータ
  struct COMPLETE_REQUEST_PARAM
  {
    u64 dataID;//対象のデータID
    BattleVideoRequestClient* pClient;
  };


  struct UPLOAD_REQUEST_PARAM_FOR_BS
  {
    ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pUploadData;//アップロードするデータ
    BattleVideoRequestClient* pClient;
    NetApp::BattleSpot::BattleSpotWork* pBSWork;
    ExtSavedata::BattleRecorderSaveData* pBattleRecorderSaveData;
  };

public:

  enum
  {
    ONLINE_DATA_MAX = DATASTORE_PERSISTENCE_SLOT_10,
  };

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ/デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  BattleVideoRequestClient( BattleVideoResponseListener* pListener, gfl2::heap::HeapBase* pHeap );
  virtual ~BattleVideoRequestClient();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   保存しているバトルビデオとサーバの同期をとる為のリクエスト
   * @param   pListener 結果を受け取るリスナー
   *                    OnSyncSuccess　か　OnError　が返ります
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void SyncBattleVideoRequest( SYNC_REQUEST_PARAM& requestParam );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルビデオを削除する為のリクエスト（パラメータの設定によって、サーバ上のデータ削除も行います）
   * @param   pListener 結果を受け取るリスナー
   *                    OnDeleteSuccess　か　OnError　が返ります
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void DeleteBattleVideoRequest( DELETE_REQUEST_PARAM& requestParam );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルビデオの公開取り消しをする為のリクエスト
   * @param   pListener 結果を受け取るリスナー
   *                    OnDeleteSuccess　か　OnDeleteFailed か　OnError　が返ります
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void CancelUploadBattleVideoRequest( CANCEL_UPLOAD_REQUEST_PARAM& requestParam );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルビデオの不正チェックとアップロードを行う為のリクエスト
   * @param   pListener 結果を受け取るリスナー
   *                    OnUploadSuccess　か　OnUploadInvalid か　OnError　が返ります
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void UploadBattleVideoRequest( UPLOAD_REQUEST_PARAM& requestParam );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルビデオの検索＆ダウンロードを行う為のリクエスト
   * @param   pListener 結果を受け取るリスナー
   *                    OnDownloadSuccess　か　OnDownloadNotFound か　OnError　が返ります
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void DownloadBattleVideoRequest( DOWNLOAD_REQUEST_PARAM& requestParam );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief   再生回数をインクリメントする為のリクエスト
   * @param   pListener 結果を受け取るリスナー
   *                    OnIncPlayCountSuccess　か OnError　が返ります
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void IncPlayCountRequest( INC_REQUEST_PARAM& requestParam );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief   再生完走を通知する為のリクエスト
   * @param   pListener 結果を受け取るリスナー
   *                    OnPlayCompleteSuccess　か　OnError　が返ります
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void SendPlayCompleteRequest( COMPLETE_REQUEST_PARAM& requestParam );


  void UploadBattleVideoRequestForBattleSpot( UPLOAD_REQUEST_PARAM_FOR_BS& requestParam );


//↓内部用
public:


  //--------------------------------------------------------------------------------------------
  /**
    * @brief   永続化スロットID取得
    */
  //--------------------------------------------------------------------------------------------
  u8 GetPersistenceSlotId( void );

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   永続化スロットID取得
    */
  //--------------------------------------------------------------------------------------------
  u8 GetPersistenceSlotId( u64 dataID );

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   永続化情報設定
    */
  //--------------------------------------------------------------------------------------------
  void SetPersistenceInfo( int slotID , bool isEnable , u64 dataID );

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   永続化情報取得
    */
  //--------------------------------------------------------------------------------------------
  bool IsPersistenceEnable( int slotID );

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   永続化情報取得
    */
  //--------------------------------------------------------------------------------------------
  u64 GetPersistenceDataID( int slotID );

  bool IsEmptySlotExists( void );


  //ヒープ再設定
  void ResetHeap( void );
  gfl2::heap::HeapBase* GetHeap( void ){ return m_pHeap; }

  NetApp::BattleSpot::BattleSpotWork* GetBattleSpotWork(void){ return m_pBSWork; }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   ダウンロード用バッファ取得
    */
  //--------------------------------------------------------------------------------------------
  ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* GetDownloadBufferPtr( void ){ return m_pDownloadBuffer; };



#ifdef GF_PLATFORM_CTR

  //--------------------------------------------------------------------------
  /**
   * @brief アップロードのパラメータのタグに値をセットする
   * @param [in/out]tag     タグ
   * @param [in]value       値
   * @param [in]header      ヘッダ文字
   */
  //--------------------------------------------------------------------------
  void SetTagValue( nn::nex::qVector<nn::nex::String>* tags, const int value, const char* header = NULL );
  void SetTagValueHex( nn::nex::qVector<nn::nex::String>* tags, const u64 value );
  void SetTagToken( nn::nex::qVector<nn::nex::String>* tags, nn::nex::qVector<qByte>& token, const wchar_t* header, const u8 loop_init, const u8 loop_end );

#endif

public:

  //内部用
  //--------------------------------------------------------------------------
  /**
   * @brief     DataStoreClientのBind
   */
  //--------------------------------------------------------------------------
  static void BindDataStoreClient( gflnet2::nex::NexDataStoreClientListener* pListener );

  //--------------------------------------------------------------------------
  /**
   * @brief   DataStoreClientのUnbind
   *
   * @retval  true:破棄終了
   * @retval  false:破棄中
   */
  //--------------------------------------------------------------------------
  static bool UnbindDataStoreClient( gflnet2::nex::NexDataStoreClientListener* pListener );

private:

  gflnet2::ServerClient::RequestHandle m_handle;//リクエストハンドル
  BattleVideoResponseListener* m_pListener;
  ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* m_pDownloadBuffer;
  gfl2::heap::HeapBase* m_pHeap;
  u64 m_persistenceDataIDArray[ONLINE_DATA_MAX];
  bool m_parsistanceSlotEnable[ONLINE_DATA_MAX];

  NetApp::BattleSpot::BattleSpotWork* m_pBSWork;
};



GFL_NAMESPACE_END(BattleVideo)
GFL_NAMESPACE_END(NetApp)





#endif // BATTLEVIDEOREQUESTCLIENT_H_INCLUDE
