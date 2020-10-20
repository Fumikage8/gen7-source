//=============================================================================
/**
 * @file   NexGlobalTradeStationClient.h
 *
 * @brief  DataStoreClinentのGlobalTradeStation機能用ラッパークラス
 *
 * @author h.suzuki
 */
//=============================================================================
#ifdef GF_PLATFORM_CTR

#if !defined( NEXGLOBALTRADESTATIONCLIENT_H_INCLUDE )
#define NEXGLOBALTRADESTATIONCLIENT_H_INCLUDE
#pragma once

#include <nex.h>
#include "NetApp/GTS/source/NexGlobalTradeStationClientListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


//------------------------------------------------------------------
/**
*  @brief ＧＴＳ通信用クラス（拡張DataStoreClientアクセス）用クラス。
*
*  @caution 事前にgflnet::nex::NexDataStoreClientのインスタンスを生成すること
*  @attention 各非同期通信は排他制御される。
*
**/
//------------------------------------------------------------------
class NexGlobalTradeStationClient
{
public:
    /**
    * @brief GTSサーバ固有のエラーコード
    */
    typedef enum
    {
      E_ERR_NOT_FOUND          = 61204,  //!< データの有効期限切れによる消失
      E_ERR_POKEMON_NOT_EXIST  = 61270,  //!< 該当するポケモンが存在しない
      E_ERR_INVALID_SIGNATURE  = 61271,  //!< 署名が不正
    }GTS_ERROR_CODE;

public:
    NexGlobalTradeStationClient( void );
    virtual ~NexGlobalTradeStationClient();

public:
  //------------------------------------------------------------------
  /**
  * @brief  初期化処理
  *
  * @param[in]  client DataStoreClientのインスタンス
  *
  * @return 初期化に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool Initialize( nn::nex::DataStoreClient* client );

  //------------------------------------------------------------------
  /**
  * @brief  終了処理
  *
  * @caution 必ず事前にIsConnecting()がfalseであることを確認してください
  */
  //------------------------------------------------------------------
  void Finalize( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief リスナの登録
  *
  * @param[in] listener 登録対象のNexGlobalTradeStationClient用リスナ。
  *
  * @return 登録に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool RegistGlobalTradeStationClientListener( NetApp::GTS::NexGlobalTradeStationClientListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief リスナの解除
  *
  * @param[in] listener 削除対象のNexGlobalTradeStationClient用リスナ。
  */
  //------------------------------------------------------------------
  void UnregistGlobalTradeStationClientListener( NetApp::GTS::NexGlobalTradeStationClientListener* listener );

public:
  //------------------------------------------------------------------
  /**
  * @brief  通信中か判定
  *
  * @return 通信中ならtrueを返却
  */
  //------------------------------------------------------------------
  bool IsConnecting( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する。
   * @brief キャンセルが実施された場合、完了時にOnGlobalTradeStationClientCanceled()がコールされる。
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
  * @brief  自分のポケモンを預ける準備をする（非同期通信）
  *
  * @note   通信終了時は「NexGlobalTradeStationClientListener::OnPrepareUploadPokemonSuccess()」がコールされる。
  * @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool PrepareUploadPokemon( void );

  //------------------------------------------------------------------
  /**
  * @brief  自分のポケモンを預ける（非同期通信）
  *
  * @caution 事前にPrepareUploadPokemon()をコールすること。
  *
  * @param  prepareUploadKey 自分が預けたポケモンを特定するキー(PrepareUploadPokemon()で取得した値)
  * @param  indexData        インデックスデータ
  * @param  pokemonData      ポケモンデータ
  * @param  signature        署名データ
  * @param  signatureSize    署名データサイズ
  * @param  period           預けるポケモンを参照できる期限(日数)
  *
  * @note   通信終了時は「NexGlobalTradeStationClientListener::OnUploadPokemonSuccess()」がコールされる。
  * @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool UploadPokemon( const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey,
                      const NetApp::GTS::GtsIndexData* indexData,
                      const NetApp::GTS::GtsSerializePokemonParam* pokemonData,
                      const void* signature,   u32 signatureSize,
                      u16 period=nn::nex::DataStoreConstants::DEFAULT_PERIOD );

  //------------------------------------------------------------------
  /**
  * @brief  交換可能なポケモンを検索する（非同期通信）
  *
  * @param  searchParam 検索設定情報。
  * @param              設定内容については、NexSDKのAPIリファレンス[nn::nex::DataStoreClient::SearchPokemon]を参照。
  *
  * @note   通信終了時は「NexGlobalTradeStationClientListener::OnSearchPokemonSuccess()」がコールされる。
  * @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool SearchPokemon( const nn::nex::GlobalTradeStationSearchPokemonParam& searchParam );

  //------------------------------------------------------------------
  /**
  * @brief  ポケモン交換の準備をする（非同期通信）
  *
  * @param  prepareUploadKey 自分が預けたポケモンを特定するキー(PrepareUploadPokemon()で取得した値)
  * @param  tradeKey         交換相手のポケモンを特定するキー(SearchPokemon()で取得した値)
  *
  * @note   通信終了時は「NexGlobalTradeStationClientListener::OnPrepareTradePokemonSuccess()」がコールされる。
  * @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool PrepareTradePokemon( const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey,
                            const NetApp::GTS::GtsSerializeTradeKey&  tradeKey );

  //------------------------------------------------------------------
  /**
  * @brief  自分のポケモンを交換する（非同期通信）
  *
  * @caution 事前にPrepareTradePokemon()をコールすること。
  *
  * @param  tradeKey         交換相手のポケモンを特定するキー(SearchPokemon()で取得した値)
  * @param  prepareTradeKey  自分が交換に出すポケモンを特定するキー(PrepareTradePokemon()で取得した値)
  * @param  prepareUploadKey 自分が預けたポケモンを特定するキー(PrepareUploadPokemon()で取得した値)
  * @param  indexData        インデックスデータ
  * @param  pokemonData      ポケモンデータ
  * @param  signature        署名データ
  * @param  signatureSize    署名データサイズ
  * @param  period           預けるポケモンを参照できる期限(日数)
  * @param  isNeedData       インデクスデータ、ポケモンデータを取得するならtrueを指定
  *
  * @note   通信終了時は「NexGlobalTradeStationClientListener::OnTradePokemonSuccess()」がコールされる。
  * @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool TradePokemon( const NetApp::GTS::GtsSerializeTradeKey& tradeKey,
                     const NetApp::GTS::GtsSerializeRecordKey& prepareTradeKey,
                     const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey,
                     const NetApp::GTS::GtsIndexData* indexData,
                     const NetApp::GTS::GtsSerializePokemonParam*  pokemonData,
                     const void* signature,   u32 signatureSize,
                     u16 period=nn::nex::DataStoreConstants::DEFAULT_PERIOD,
                     bool isNeedData=false );

  //------------------------------------------------------------------
  /**
  * @brief  自分のポケモンを削除する（非同期通信）
  *
  * @param  prepareUploadKey 自分が預けたポケモンを特定するキー(PrepareUploadPokemon()で取得した値)
  * @param  deleteFlag       削除挙動設定フラグ
  *
  * @note   通信終了時は「NexGlobalTradeStationClientListener::OnDeletePokemonSuccess()」がコールされる。
  * @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool DeletePokemon( const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey,
                      const nn::nex::GlobalTradeStationConstants::DeleteFlag deleteFlag );

  //------------------------------------------------------------------
  /**
  * @brief  自分のポケモンを取得する（非同期通信）
  *
  * @param  prepareUploadKey 自分が預けたポケモンを特定するキー(PrepareUploadPokemon()で取得した値)
  *
  * @note   通信終了時は「NexGlobalTradeStationClientListener::OnDownloadMyPokemonSuccess()」がコールされる。
  * @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool DownloadMyPokemon( const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey );

  //------------------------------------------------------------------
  /**
  * @brief  交換相手のポケモンを取得する（非同期通信）
  *
  * @param  prepareUploadKey 自分が預けたポケモンを特定するキー(PrepareUploadPokemon()で取得した値)
  *
  * @note   通信終了時は「NexGlobalTradeStationClientListener::OnDownloadOtherPokemonSuccess()」がコールされる。
  * @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool DownloadOtherPokemon( const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey );

public:
  //------------------------------------------------------------------
  /*
   * @brief  完了通知検知用関数
   *
   * @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void OnPrepareUploadCompletion(nn::nex::CallContext *pContext);
  void OnUploadPokemonCompletion(nn::nex::CallContext *pContext);
  void OnSearchPokemonCompletion(nn::nex::CallContext *pContext);
  void OnPrepareTradePokemonCompletion(nn::nex::CallContext *pContext);
  void OnTradePokemonCompletion(nn::nex::CallContext *pContext);
  void OnDeletePokemonCompletion(nn::nex::CallContext *pContext);
  void OnDownloadMyPokemonCompletion(nn::nex::CallContext *pContext);
  void OnDownloadOtherPokemonCompletion(nn::nex::CallContext *pContext);

private:
  //------------------------------------------------------------------
  /**
   * @brief 通信ロック設定
   *
   * @return すでに通信ロックされているならfalseを返却
   */
  //------------------------------------------------------------------
  bool SetAccessLock( void );

  //------------------------------------------------------------------
  /**
   * @brief エラー通知処理
   *
   * @param result 実行結果
   */
  //------------------------------------------------------------------
  void ReportErrorCodeToListener( const nn::nex::qResult& result );

  //------------------------------------------------------------------
  /**
   * @brief キャンセル通知処理
   */
  //------------------------------------------------------------------
  void ReportCanceledListener( void );

  //------------------------------------------------------------------
  /**
   * @brief 関数呼び出しエラー通知処理
   */
  //------------------------------------------------------------------
  void ReportFunctionErrorCodeToListener( void );

  //------------------------------------------------------------------
  /**
   * @brief NexのキーをシリアライズしてserializeRecodeKeyに出力します
   *
   * @param nexKey 入力パラメータのNexのキー
   * @param serializeRecordKey 出力パラメータのserializeRecodeKey
   */
  //------------------------------------------------------------------
  void ConvertNexRecordKeyToSerializeRecordKey( const nn::nex::GlobalTradeStationRecordKey& nexKey, GtsSerializeRecordKey& serializeRecordKey );

private:
    static const u32                         LISTENER_NUM = 4;             //!< リスナ最大登録可能数
    //
    NexGlobalTradeStationClientListener*     m_listener[LISTENER_NUM];     //!< レーティング用リスナ
    //
    bool                                     m_isInit;                     //!< 初期化フラグ
    bool                                     m_isAccess;                   //!< 通信フラグ
    //
    nn::nex::DataStoreClient*                m_dataStoreClient;            //!< データストアクライアント
    nn::nex::ProtocolCallContext*            m_context;                    //!< コンテキスト
    //
    nn::nex::GlobalTradeStationDownloadMyPokemonResult*   m_myDownloadInf; //!< ダウンロード情報
    nn::nex::GlobalTradeStationPrepareTradePokemonResult* m_preTradeInf;   //!< トレード準備情報
    nn::nex::GlobalTradeStationTradePokemonResult*        m_tradeInf;      //!< 交換結果情報
    nn::nex::GlobalTradeStationSearchPokemonResult*       m_searchInf;     //!< 検索結果情報
    nn::nex::GlobalTradeStationRecordKey*                 m_recordKey;     //!< レコードキー情報
};





GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif      // NEXGLOBALTRADESTATIONCLIENT_H_INCLUDE
#endif // GF_PLATFORM_CTR
