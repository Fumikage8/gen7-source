//======================================================================
/**
 * @file  PGLConnector.h
 *
 * @author  suzuki_hisanao
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef _PGL_CONNECTOR_H_
#define _PGL_CONNECTOR_H_

#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <system/include/Date/gfl2_Date.h>

#include <NetStatic/NetLib/include/PGL/PGLDefine.h>
#include <NetStatic/NetLib/include/PGL/PGLConnectorListener.h>
#include <NetStatic/NetLib/include/HTTP/NexHttpClientListener.h>
#include <NetStatic/NetLib/include/HTTP/NexHttpClient.h>

#include <System/include/SystemEventListener.h>

//------------------------------------
// デバッグ証明書の利用
// 本定義を無効化した際は、CTR本体内蔵の証明書が使用される。
// 製品版は必ず無効化すること
//------------------------------------
#if PM_DEBUG
//  #define PGL_USE_DEBUG_CER
#endif


namespace NetLib{

/**
 * @namespace NetLib::pgl
 * @brief     PGL通信処理
 */
namespace Pgl{


//--------------------------------------------------------------------------------------------
/**
 * @brief PGLサーバ接続用クラス。<p>
 *        PGLサーバとのHTTP通信を行う際は本クラスを介して行うこと。
 *
 * @par  非同期通信関数の引数に渡すBASE64エンコードデータは以下の対応が必須
 *            - 終端1BYTEに文字列終端コード「\0」が必須。
 *            - 通信完了までインスタンスを保持する必要がある。
 */
//--------------------------------------------------------------------------------------------
class PGLConnector : public NetLib::HTTP::NexHttpClientListener , public System::SystemEventListener
{
public:
  static const nn::nex::qInt32 POST_BUFF_SIZE  = 8192;  //!< POSTバッファサイズ
  static const u32             URL_LENGTH      = 128;   //!< 最大URL文字数
  static const nn::nex::qInt32 CA_BUFFER_SIZE  = 2048;  //!< ルート証明書の格納に使用するバッファサイズ
  static const u32             RESULT_ITEM_SLOT= 20;    //!< 返却されるアイテムスロット数
  static const u32             GA_NUM = 12;
  
#if PM_DEBUG
  static u32 m_dbgBattleType;
  static u32* GetDebugBattleType( void ){ return &m_dbgBattleType; }


#endif

  enum
  {
    BATTLE_TYPE_INTERNET = 0,
    BATTLE_TYPE_FRIEND = 1,
  };

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
   PGLConnector( void );

   //--------------------------------------------------------------------------------------------
   /**
    * @brief デストラクタ
    */
   //--------------------------------------------------------------------------------------------
   virtual ~PGLConnector();

public:
   virtual void OnNexHttpConnectionSuccess( const NetLib::HTTP::NexHttpResponse& response );
   virtual void OnNexHttpConnectionError( const nn::nex::qResult &result, u32 errorCode );
   virtual void OnNexHttpConnectionCanceled( void );

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 初期化処理（ブロック関数）
   *
   * @attention 事前にhttp, fs, friend, nexを初期化すること。<p>
   *            またNEXへの接続を事前に確立させること
   *
   * @param[in] heap        内部バッファ確保用に使用されるヒープ
   * @param[in] nexUniqueId 使用されるＮＥＸユニークＩＤ
   * @param[in] authToken   認証トークン
   * @param[in] region      本体リージョン
   *
   * @retval true  初期化成功
   * @retval false 初期化失敗
   */
  //--------------------------------------------------------------------------------------------
  bool Initialize( gfl2::heap::HeapBase *heap, u64 nexUniqueId, const char* authToken,
                   const nn::cfg::CTR::CfgRegionCode region );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 終了処理
   *
   * @attention 必ず事前にIsConnecting()がfalseであることを確認してください
   */
  //--------------------------------------------------------------------------------------------
  void Finalize();

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief セーブデータ登録通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLCreateSavedataSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @attention  HTTP通信用POSTバッファを大きく使用する。
   * @attention  gflnet2::http::HttpSystem::Initialize()で適切にメモリを割り当てること。
   *
   * @param[in] base64Savedata      BASE64エンコードされたセーブデータ
   * @param[in] savedataBinarySize  セーブデータのバイナリ時のサイズ
   * @param[in] base64PlayerIcon    BASE64エンコードされたプレイヤーアイコン画像データ
   * @param[in] payerIconBinarySize アイコン画像のバイナリ時のサイズ
   * @param[in] isParentalControl   ペアレンタルコントロールの有無(ペアレンタル制限有効ならtrue)
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool CreateSaveData( const char* base64Savedata, u32 savedataBinarySize,
                       const char* base64PlayerIcon, u32 payerIconBinarySize,
                       bool  isParentalControl );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ポケモンBOX情報なしセーブデータ同期用通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLSyncSaveDataSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @attention  HTTP通信用POSTバッファを大きく使用する。
   * @attention  gflnet2::http::HttpSystem::Initialize()で適切にメモリを割り当てること。
   *
   * @param[in] base64Savedata      BASE64エンコードされたセーブデータ
   * @param[in] savedataBinarySize  セーブデータのバイナリ時のサイズ
   * @param[in] base64PlayerIcon    BASE64エンコードされたプレイヤーアイコン画像データ
   * @param[in] payerIconBinarySize アイコン画像のバイナリ時のサイズ
   * @param[in] itemFreeSpace       ゲーム側で取得可能なアイテム数
   * @param[in] isParentalControl   ペアレンタルコントロールの有無(ペアレンタル制限有効ならtrue)
   * @param[in] photoInfo           3Dカメラ写真情報（省略可能）
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool SyncNonboxSaveData( const char* base64Savedata, u32 savedataBinarySize,
                           const char* base64PlayerIcon, u32 payerIconBinarySize,
                           u8    itemFreeSpace,
                           bool  isParentalControl,
                           const GameSyncCameraInfo* photoInfo=NULL );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief FULL情報セーブデータ同期用通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLSyncSaveDataSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @attention  HTTP通信用POSTバッファを大きく使用する。
   * @attention  gflnet2::http::HttpSystem::Initialize()で適切にメモリを割り当てること。
   *
   * @param[in] base64Savedata      BASE64エンコードされたセーブデータ
   * @param[in] savedataBinarySize  セーブデータのバイナリ時のサイズ
   * @param[in] base64PlayerIcon    BASE64エンコードされたプレイヤーアイコン画像データ
   * @param[in] payerIconBinarySize アイコン画像のバイナリ時のサイズ
   * @param[in] itemFreeSpace       ゲーム側で取得可能なアイテム数
   * @param[in] isParentalControl   ペアレンタルコントロールの有無(ペアレンタル制限有効ならtrue)
   * @param[in] photoInfo           3Dカメラ写真情報（省略可能）
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool SyncFullSaveData( const char* base64Savedata, u32 savedataBinarySize,
                         const char* base64PlayerIcon, u32 payerIconBinarySize,
                         u8    itemFreeSpace,
                         bool  isParentalControl,
                         const GameSyncCameraInfo* photoInfo=NULL );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief セーブデータ同期確定通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLCommitGameSyncSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool CommitGameSync( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief セーブデータダウンロード通信（非同期通信）
   *
   * @attention Initialize()で指定したnexUniqueIdのセーブデータをダウンロードする。
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLDownloadSavedataSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @param[in] savedataType ダウンロードするセーブデータタイプ
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool DownloadSaveData( PGL_SAVEDATA_DOWNLOAD_TYPE savedataType );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief プレイステータス確認通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLGetPlayStatusSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @param[in]  purpose プレイステータス確認の目的
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool GetPlayStatus( PGL_PURPOSE_TYPE purpose );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief バトルエントリー確認通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLGetWorldBattleStatusSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool GetWorldBattleStatus( u32 battleType );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief バトルエントリー状態更新通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLUpdateWorldBattleStatusSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @param[in]  gpfEntryFlg       PGLエントリーワーク
   * @param[in]  wifiMatchupState  大会の進行情報
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool UpdateWorldBattleStatus( u8 gpfEntryFlg, u8 wifiMatchupState , u32 battleType );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief バトルボックス登録通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLUploadBattleBoxSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @param[in] base64BattleBoxData  BASE64エンコードを行ったバトルボックスデータ。
   * @param[in] battleBoxBinarySize  バトルボックスデータのバイナリ時のサイズ
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool UploadBattleBox( const char* base64BattleBoxData, u32 battleBoxBinarySize , u32 battleType );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief バトルボックス取得通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLDownloadBattleBoxSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @param[in] nexUniqueIdOfTargertUser  取得対象者のＮＥＸユニークＩＤ
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool DownloadBattleBox( u64 nexUniqueIdOfTargertUser , u32 battleType );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 不思議な贈り物シリアル認証通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLAuthSerialSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @param[in] address    アドレス情報
   * @param[in] serialCode シリアルコード文字列(通信完了までインスタンス保持必須）
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool AuthSerialCode( const nn::cfg::CTR::SimpleAddressId& address,
                       const char* serialCode );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 不思議な贈り物シリアル受け取り完了報告通信（非同期通信）
   *
   * @note  通信成功時に「PGLConnectorListener::OnPGLUpdateSerialStatusSuccess()」がコールされる。
   * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
   *
   * @param[in] address    アドレス情報
   * @param[in] serialCode シリアルコード文字列(通信完了までインスタンス保持必須）
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool UpdateSerialCode( const nn::cfg::CTR::SimpleAddressId& address,
                         const char* serialCode );

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する。
   * @brief キャンセルが実施された場合、完了時にOnPGLConnectCanceled()がコールされる。
   *
   * @note  通信状態にかかわらず、何度でもコールしても良い
   *
   * @return キャンセル処理が実施されたならtrueを返却する
   */
  //--------------------------------------------------------------------------------------------
  bool CancelConnecting( void );

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 稼働状態(Initialize済み)判定
   *
   * @return 稼働状態にあるならtrue、非稼働状態ならfalseを返却
   */
  //--------------------------------------------------------------------------------------------
  bool IsEnable( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 通信接続中か判定
   *
   * @return ポケモン不正チェックサーバと通信状態にあるならtrueを返却
   */
  //--------------------------------------------------------------------------------------------
  bool IsConnecting( void ) const;

public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief イベントリスナを登録
  *
  * @param[in] listener 検査結果通知先イベント
  */
  //--------------------------------------------------------------------------------------------
  void SetPGLConnectorListener( PGLConnectorListener* listener );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief イベントリスナを解除
  */
  //--------------------------------------------------------------------------------------------
  void RemovePGLConnectorListener( void );

private:
#ifdef PGL_USE_DEBUG_CER
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 証明書ファイルの読み込み
   *
   * @param[in] heap     ロードに使用するメモリ
   * @param[in] fileId   ロードするファイルID
   * @param[in] caBuffer 証明書の格納先
   * @param[in] caSize   証明書のサイズ
   *
   * @return ロードに成功したならtrueを返却
   */
  //--------------------------------------------------------------------------------------------
  bool LoadCertifyFile( gfl2::heap::HeapBase * heap, u32 fileId, u8* caBuffer, size_t* caSize );
#endif

private:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief デバッグ用証明書の割り当て
   *
   * @param[in] setting ＨＴＴＰ接続用設定
   */
  //--------------------------------------------------------------------------------------------
  void SetupCert( NetLib::HTTP::HttpConnectSetting* setting );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ＵＲＬの設定
   *
   * @param[in] command 実行するコマンド
   */
  //--------------------------------------------------------------------------------------------
  void SetupUrl( PGL_COMMAND_TYPE command );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief リージョン別URLの設定
   *
   * @param[in] url リージョンコードを埋め込む対象となるURL
   */
  //--------------------------------------------------------------------------------------------
  void SetUrlWithRegion( char* url );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 共通ポストパラメータを設定
   *
   * @attention 事前にSetupUrl()をコールすること
   */
  //--------------------------------------------------------------------------------------------
  void SetupBasicPostBody( void );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief HTTP接続の開始
  *
  * @note 本関数コール前に[NetLib::HTTP::HttpConnectSetting]を正しく設定すること
  * @note なお関数がfalseを返却した場合は、PGLConnectorListenerイベントは発行されない。
  *
  * @param[in] setting ＨＴＴＰ接続用設定
  *
  * @return 実行開始が正常に行えたならtrueを返却
  */
  //--------------------------------------------------------------------------------------------
  bool Connect( const NetLib::HTTP::HttpConnectSetting &setting );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief クエリパラメータを登録する
   *
   * @param[in] queryName クエリ名
   * @param[in] value     パラメータへのポインタ
   */
  //--------------------------------------------------------------------------------------------
  void AddPostParam( const char* queryName, const char* value );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ASCII数値を整数値に変換
   *
   * @param[in] value     ASCII文字列
   * @param[in] byteSize  文字データ格納サイズ
   *
   * @return 変換された整数値
   */
  //--------------------------------------------------------------------------------------------
  u32 ConvertAsciiToInt( void* value, u32 byteSize );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief クエリパラメータを登録する(内部テンポラリバッファを利用）
   *
   * @param[in] queryName クエリ名
   * @param[in] value     パラメータへのポインタ
   */
  //--------------------------------------------------------------------------------------------
  void AddPostParamWithTmp( const char* queryName, u32 value );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief クエリパラメータを登録する(内部テンポラリバッファを利用）
   *
   * @param[in] queryName クエリ名
   * @param[in] value     パラメータへのポインタ
   */
  //--------------------------------------------------------------------------------------------
  void AddPostParamWithTmp( const char* queryName, u64 value );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief クエリパラメータを登録する(内部テンポラリバッファを利用）
   *
   * @param[in] queryName クエリ名
   * @param[in] value     パラメータへのポインタ
   */
  //--------------------------------------------------------------------------------------------
  void AddPostParamWithTmp( const char* queryName, const char* value );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief クエリパラメータを登録する(内部テンポラリバッファを利用）
   *
   * @param[in] queryName クエリ名
   * @param[in] value     パラメータへのポインタ
   */
  //--------------------------------------------------------------------------------------------
  void AddPostParamWithTmp( const char* queryName, const gfl2::system::Date* value );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief レスポンスデータの解析する
   *
   * @param[in] bodyPtr  レスポンスデータ
   * @param[in] bodySize レスポンスデータサイズ
   */
  //--------------------------------------------------------------------------------------------
  void ParseResponse( const u8* bodyPtr, u32 bodySize );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief レスポンス共通情報を解析する
   *
   * @param[out] retCommonInf レスポンス共通情報格納先
   * @param[in]  bodyPtr      レスポンスデータ
   *
   * @return 取得したデータサイズ
   */
  //--------------------------------------------------------------------------------------------
  u32 ParseResponseCommonHeader( ResultCommonInfo* retCommonInf, const u8* bodyPtr );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief レスポンスボディを解析する
   *
   * @param[in] retCommonInf レスポンス共通情報
   * @param[in] bodyPtr      レスポンスボディ
   * @param[in] bodySize     レスポンスボディサイズ
   *
   * @return パース処理が成功したならtrueを返却する
   */
  //--------------------------------------------------------------------------------------------
  bool ParseResponseBody( const ResultCommonInfo& retCommonInf, const u8* bodyPtr, u32 bodySize );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief アイテム情報のパース
   *
   * @param[in] dstItemInf パースしたアイテム情報を格納する先
   * @param[in] bodyPtr    レスポンスボディ
   * @param[in] bodySize   レスポンスボディサイズ
   *
   * @return 格納されたアイテム数
   */
  //--------------------------------------------------------------------------------------------
  u32 ParseItemInfo( ResultItemInfo* dstItemInf, const u8* bodyPtr, u32 bodySize , bool isSignIn );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ゲームシンク送信用内部関数（非同期通信）
   *
   * @param[in] command             実行するゲームシンクコマンド
   * @param[in] region              本体リージョン
   * @param[in] base64Savedata      BASE64エンコードされたセーブデータ
   * @param[in] savedataBinarySize  セーブデータのバイナリ時のサイズ
   * @param[in] base64PlayerIcon    BASE64エンコードされたプレイヤーアイコン画像データ
   * @param[in] payerIconBinarySize アイコン画像のバイナリ時のサイズ
   * @param[in] itemSpace           ゲーム側で取得可能なアイテム数。負の値をセットするとサーバに送信しない。
   * @param[in] isParentalControl   ペアレンタルコントロールの有無(ペアレンタル制限有効ならtrue)
   * @param[in] photoInfo           3Dカメラ写真情報（省略可能）
   *
   * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
   */
  //--------------------------------------------------------------------------------------------
  bool ConnectGameSyncInternal( PGL_COMMAND_TYPE command,
                                const nn::cfg::CTR::CfgRegionCode region,
                                const char* base64Savedata, u32 savedataBinarySize,
                                const char* base64PlayerIcon, u32 payerIconBinarySize,
                                s32   itemSpace,
                                bool  isParentalControl,
                                const GameSyncCameraInfo* photoInfo );

public:

  //------------------------------------------------------------------
  /**
   * @brief   ホーム、スリープ、ゲーム終了処理発生時イベント
   *
   * @retval  true    終了処理完了
   * @retval  false   終了処理中
   *
   * @note    適切な終了処理を記述してください。
   * @attention  On～という名前なのに他のメンバとふるまいが違うので注意
   * @attention  Observerパターンとして名前が適切なのかは疑問の余地あり
   */
  //------------------------------------------------------------------
  virtual bool OnSuspendWithHomeSleepAndPower( void );

public:
  static char                         m_baseURL[URL_LENGTH];   //!< ベース用URL[デバッグ用に公開はしているが、ユーザー操作禁止]
  static char                         m_serialURL[URL_LENGTH]; //!< シリアルコード用URL[デバッグ用に公開はしているが、ユーザー操作禁止]
  static char                         m_jsonURL[URL_LENGTH];   //!< JSONテスト用URL[デバッグ用に公開はしているが、ユーザー操作禁止]

private:
  static const s32                    NOT_SEND_ITEMSPACE = -1; //!< アイテム未送信定義定数
  static const u32                    POST_BUFFER_SIZE   = 10; //!< POSTバッファサイズ
  //
  static char*                        FRONTEND_BASE_URL;       //!< フロントエンドURL
  static char*                        FRONTEND_SERIAL_URL;     //!< フロントエンドURL
  //
  //
  static const u16                    TIMEOUT_GAMESYSNC = 120; //!< ゲームシンク時のタイムアウト時間（デフォルト値）
  static const u16                    TIMEOUT_MIN = 30;        //!< タイムアウト時間（最小値）
  static const u16                    TIMEOUT_MAX = 300;       //!< タイムアウト時間（最大値）
  //
  static const char                   REGION_DOMAIN[nn::cfg::CTR::CFG_REGION_MAX][3]; //!< リージョンドメイン用文字列
  //
  static u16                          m_ganeSyncTimeout;      //!< ゲームシンク用タイムアウト時間
  //
  char                                m_langCode[32];         //!< 言語コード
  char                                m_romCode[32];          //!< ROMコード
  char                                m_nexUniqueId[32];      //!< NEXユニークID
  char                                m_command[32];          //!< 実行コマンド名
  char                                m_url[URL_LENGTH];      //!< URL
  //
  char                                m_postWork[POST_BUFFER_SIZE][32];   //!< POSTワークエリア
  u32                                 m_postWorkUseNum;                   //!< POSTワーク使用数
  //
  char                                m_authToken[nn::nex::IndependentServer::AUTH_TOKEN_SIZE]; //!< 認証トークン
  //
  u8                                  m_crtBuffer[CA_BUFFER_SIZE];
  size_t                              m_crtSize;              //!< 証明書のサイズ
  //
  bool                                m_isInit;               //!< 初期化フラグ
  PGLConnectorListener*               m_listener;             //!< イベントリスナ
  //
  nn::cfg::CTR::CfgRegionCode         m_region;               //!< リージョンコード
  //
  NetLib::HTTP::NexHttpClient*        m_httpCon;              //!< HTTPクライアント
  NetLib::HTTP::HttpConnectSetting    m_setting;              //!< HTTP設定
  //
  PGL_COMMAND_TYPE                    m_currentCommand;       //!< カレント通信コマンド
};


} // pgl
} // netapp

#endif  // _POKEMON_VALIDATION_H_
#endif