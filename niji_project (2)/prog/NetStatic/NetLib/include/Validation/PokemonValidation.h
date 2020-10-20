//======================================================================
/**
 * @file  PokemonValidation.h
 *
 * @author  suzuki_hisanao
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef _NETAPP_POKEMON_VALIDATION_H_
#define _NETAPP_POKEMON_VALIDATION_H_
#pragma once

#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include "NetStatic/NetLib/include/Validation/PokemonValidationDefine.h"
#include "NetStatic/NetLib/include/Validation/PokemonValidationListener.h"
#include <NetStatic/NetLib/include/HTTP/NexHttpClientListener.h>
#include <NetStatic/NetLib/include/HTTP/NexHttpClient.h>
#include <NetApp/GTS/source/GTSDefine.h>
#include <NetApp/GTS/source/GTSTypes.h>
#include <System/include/SystemEventListener.h>
#include <pml/include/pmlib.h>

namespace NetLib{

/**
 * @namespace NetLib::validation
 * @brief     ポケモン不正チェックサーバアクセス関連
 */
namespace Validation{


//------------------------------------
// デバッグ証明書の利用
//
// 本定義をコメントアウトすることでCTR本体内蔵の証明書を利用します。
// 製品版では必ずコメントアウトしてください。
//------------------------------------
//  #define VALIDATION_USE_DEBUG_CER 1


//---------------------------------------------------------------------------
/**
 * @brief ポケモン不正チェックサーバアクセスクラス。<p>
 *        ポケモンが改竄されているか否かを不正チェックサーバを利用し確認することができます。
 *
 * @note  インターネット接続時のみ利用可能です。<p>
 *        事前にＮＥＸサーバへのログインを行ってください。
 */
//---------------------------------------------------------------------------
class PokemonValidation : public NetLib::HTTP::NexHttpClientListener , public System::SystemEventListener
{
public:
  //文字列チェック
  enum NAME_CHECK_RESULT
  {
    NAME_CHECK_OK = 0,
    NAME_CHECK_NG_BLANK,
    NAME_CHECK_NG_INVALID,
    NAME_CHECK_NG_NO_END,
  };

  /**
   * @brief 独自のHTTPステータスコード
   */
  typedef enum
  {
    HTTP_STATUS_SERVER_MAINTENANCE = 503,  //!< サーバメンテナンス中を表すエラーコード
  }HttpStatusCode;

  /**
   * @brief ゲームモード
   */
  typedef enum
  {
    E_GAME_MODE_GTS_UPLOAD   = 0x0100,   //!< GTS交換に出すポケモンの検査
    E_GAME_MODE_GTS_CHECK    = 0x0200,   //!< GTSに欲しいポケモンをリクエストする際にあり得るポケモンか検査
    E_GAME_MODE_TRADE        = 0x0300,   //!< 交換に出すポケモンの検査
    E_GAME_MODE_BATTLE       = 0x0400,   //!< 対戦で使うポケモンパーティの検査
    E_GAME_MODE_BATTLE_VIDEO = 0x0500,   //!< バトルビデオに保存されているポケモンパーティの検査
    E_GAME_MODE_QR_TEAM      = 0x0600,    //!< QRポケモンパーティの検査
    E_GAME_MODE_GTS_UPLOAD_NOT_INDEX_CHECK    = 0x0700,   //!< GTS交換に出すポケモンの検査(インデックスデータの検査なし)
    E_GAME_MODE_SALMON_VC    = 0x0800,    //!< salmonのVCポケモン用検査
    // ↓↓↓ momiji追加
    E_GAME_MODE_GTS_UPLOAD_M   = 0x0900,   //!< momiji:GTS交換に出すポケモンの検査
    E_GAME_MODE_GTS_CHECK_M    = 0x0A00,   //!< momiji:GTSに欲しいポケモンをリクエストする際にあり得るポケモンか検査
    E_GAME_MODE_TRADE_M        = 0x0B00,   //!< momiji:交換に出すポケモンの検査
    E_GAME_MODE_BATTLE_M       = 0x0C00,   //!< momiji:対戦で使うポケモンパーティの検査
    E_GAME_MODE_BATTLE_VIDEO_M = 0x0D00,   //!< momiji:バトルビデオに保存されているポケモンパーティの検査
    E_GAME_MODE_QR_TEAM_M      = 0x0E00,    //!< momiji:QRポケモンパーティの検査
    E_GAME_MODE_GTS_UPLOAD_NOT_INDEX_CHECK_M    = 0x0F00    //!< momiji:GTS交換に出すポケモンの検査(インデックスデータの検査なし)
    // ↑↑↑ momiji追加
  }GameMode;

public:
  static const nn::nex::qInt32 POST_BUFF_SIZE      = 8192;  //!< POSTバッファサイズ
  static const nn::nex::qInt32 SIGNATURE_SIZE      = 256;   //!< 署名のサイズ
  static const nn::nex::qInt32 RESULT_STATUS_SIZE  = 1;     //!< 返却ステータスサイズ
  static const nn::nex::qInt32 RESULT_POKENUM_SIZE = 2;     //!< 返却ポケモン数サイズ
  static const nn::nex::qInt32 POKEMON_NUM         = 12;    //!< チェックするポケモンの数
  static const nn::nex::qInt32 CA_BUFFER_SIZE      = 1024;  //!< ルート証明書の格納に使用するバッファサイズ
  static const nn::nex::qInt32 ROM_BUFFER_SIZE     = 128;   //!< ROMアーカイブのマウントに使用するバッファサイズ
  //
  static const nn::nex::qInt32 PUBLIC_KEY_SIZE     = 392;   //!< サーバから取得できる公開鍵のサイズ

private:
  //---------------------------------------------------------------------------
  /**
   * @brief ポケモン１体分のチェック用データ
   */
  //---------------------------------------------------------------------------
  struct GTS_UPLOAD_PACKAGE
  {
    u8 m_index[168];      //!< インデックス情報
    NetApp::GTS::GtsSerializePokemonParam m_pokemon;    //!< ポケモン情報
  };

  union UNION_PACKAGE
  {
    struct
    {
      GTS_UPLOAD_PACKAGE        m_gtsUpload[POKEMON_NUM];  //!< GTSアップロード情報
    };
    NetApp::GTS::GtsSerializePokemonParam m_pokemon[POKEMON_NUM];    //!< ポケモン情報
  };

  //---------------------------------------------------------------------------
  /**
  * @brief 不正チェックサーバに渡すデータ構造
   */
  //---------------------------------------------------------------------------
  struct POKMEON_CHECKDATA
  {
    char           authToken[nn::nex::IndependentServer::AUTH_TOKEN_SIZE+1]; //!< 認証トークン
    u16            gameMode;                                                 //!< ゲームモード
    UNION_PACKAGE  data;                                        //!< チェック対象のデータ

    /**
     * @brief 初期化処理
     */
    void Initialize( void )
    {
      std::memset( authToken, 0, sizeof(authToken) );
      gameMode = 0;
      std::memset( &data, 0, sizeof(data) );
    }

    /**
     * @brief コンストラクタ
     */
    POKMEON_CHECKDATA()
    {
      Initialize();
    }
  };

public:
  static const nn::nex::qInt32 INDEX_SIZE      = sizeof(NetApp::GTS::GtsIndexData);  //!< インデクスデータのサイズ
  static const nn::nex::qInt32 POKEMON_SIZE    = sizeof(NetApp::GTS::GtsSerializePokemonParam);  //!< ポケモンデータのサイズ

public:
  //---------------------------------------------------------------------------
  /**
  * @brief コンストラクタ
  */
  //---------------------------------------------------------------------------
  PokemonValidation( void );

  //---------------------------------------------------------------------------
  /**
  * @brief デストラクタ
  */
  //---------------------------------------------------------------------------
  virtual ~PokemonValidation();

public:
  virtual void OnNexHttpConnectionSuccess( const NetLib::HTTP::NexHttpResponse& response );
  virtual void OnNexHttpConnectionError( const nn::nex::qResult &result, u32 errorCode );
  virtual void OnNexHttpConnectionCanceled( void );

public:
  //---------------------------------------------------------------------------
  /**
   * @brief 初期化処理（ブロック関数）
   *
   * @attention 事前にhttp, fs, friend, nexを初期化すること
   * @attention またNEXへの接続を事前に確立させること
   *
   * @param[in] heap      内部バッファ確保用に使用されるヒープ
   * @param[in] devHeap   証明書読み込みに使用されるデバイスヒープ
   * @param[in] authToken 認証トークン
   * @param[in] gameMode  ゲームモード
   *
   * @note ゲームモード別に設定可能なポケモン数については<a href="../suzuki_hisanao/pdf/validation_gamemode.pdf">不正チェックゲームモードについて</a>を参照のこと。
   *
   * @retval true  初期化成功
   * @retval false 初期化失敗
   */
  //---------------------------------------------------------------------------
  bool Initialize( gfl2::heap::HeapBase *heap, gfl2::heap::HeapBase* devHeap, const char* authToken, GameMode gameMode=E_GAME_MODE_GTS_UPLOAD );

  //---------------------------------------------------------------------------
  /**
   * @brief 終了処理
   *
   * @attention 必ず事前にIsConnecting()がfalseであることを確認してください
   */
  //---------------------------------------------------------------------------
  void Finalize( void );

public:
  //---------------------------------------------------------------------------
  /**
  * @brief 不正チェック処理で判定するデータをセットする
  *
  * @param[in] slotNo      格納用スロット番号(0～9)
  * @param[in] indexInfo   インデックス情報へのポインタ(サイズは160byte固定)
  * @param[in] pokemonBody ポケモン情報へのポインタ(サイズは232byte固定)
  *
  * @return データセットが正常に行えたならtrueを返却
  */
  //---------------------------------------------------------------------------
  bool SetPokemon( u32 slotNo, NetApp::GTS::GtsIndexData* indexInfo, NetApp::GTS::GtsSerializePokemonParam* pokemonBody );

  //---------------------------------------------------------------------------
  /**
  * @brief ポケモン不正チェックの開始
  *
  * @note 不正チェックサーバでの検査終了のタイミングでPokemonValidationListenerを経由して結果を取得することができる。
  * @note なお本関数がfalseを返却した場合は、PokemonValidationListenerイベントは発行されない。
  *
  * @attention 本関数実行前にSetPokemon()にて検査対象となるポケモンデータをセットしておくこと
  *
  * @param[in] heap       通信スレッドの生成に使用されるヒープメモリ
  * @param[in] pokemonNum チェックするポケモン数
  *
  * @return 実行開始が正常に行えたならtrueを返却
  */
  //---------------------------------------------------------------------------
  bool CheckPokemonASync( gfl2::heap::HeapBase* heap, u32 pokemonNum );

  //---------------------------------------------------------------------------
  /**
   * @brief 不正チェックサーバの証明書を使用して、ポケモンデータが改竄されていないかチェック
   *
   * @attention 本関数実行前にSetPokemon()にて検査対象となるポケモンデータをセットしておくこと
   *
   * @param[in] pokemonNum   登録されているポケモン数
   * @param[in] certData     証明書データ
   * @param[in] certDataSize 証明書データサイズ
   *
   * @retval true  改竄が行われていない
   * @retval false 改竄が行われている
   */
  //---------------------------------------------------------------------------
  bool CheckRsaValidation( u32 pokemonNum, const u8* certData, const u32 certDataSize );

  //---------------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する。
   * @brief キャンセルが実施された場合、完了時にOnPokemonValidationCanceled()がコールされる。
   *
   * @note  通信状態にかかわらず、コールしても良い
   *
   * @return キャンセル処理が実施されたならtrueを返却する
   */
  //---------------------------------------------------------------------------
  bool CancelConnecting( void );

public:
  //---------------------------------------------------------------------------
  /**
   * @brief 稼働状態(Initialize済み)判定
   *
   * @retval true  稼働状態にある
   * @retval false 稼働状態にない
   */
  //---------------------------------------------------------------------------
  bool IsEnable( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 通信接続中か判定
   *
   * @return ポケモン不正チェックサーバと通信状態にあるならtrueを返却
   */
  //---------------------------------------------------------------------------
  bool IsConnecting( void ) const;

public:
  //---------------------------------------------------------------------------
  /**
  * @brief 検査結果通知用イベントリスナを登録
  *
  * @param[in] listener 検査結果通知先イベント
  */
  //---------------------------------------------------------------------------
  void SetPokemonValidationListener( PokemonValidationListener* listener );

  //---------------------------------------------------------------------------
  /**
  * @brief 検査結果通知用イベントリスナを解除
  */
  //---------------------------------------------------------------------------
  void RemovePokemonValidationListener( void );

private:
#ifdef VALIDATION_USE_DEBUG_CER
  //---------------------------------------------------------------------------
  /**
   * @brief 証明書ファイルの読み込み(デバッグ用。そのうち消えます)
   *
   * @param[in] devHeap ロードに使用するデバイスメモリ
   *
   * @return ロードに成功したならtrueを返却
   */
  //---------------------------------------------------------------------------
  bool LoadCertifyFile( gfl2::heap::HeapBase * devHeap );
#endif

private:
  //---------------------------------------------------------------------------
  /**
   * @brief ポケモン検査用POST通信バッファをセット
   *
   * @param[in] heap       通信スレッドの生成に使用されるヒープメモリ
   * @param[in] pokemonNum チェックするポケモン数
   *
   * @return 生成されたPOSTデータサイズ
   */
  //---------------------------------------------------------------------------
  u32 SetupValidationPostBuffer( gfl2::heap::HeapBase* heap, u32 pokemonNum );

  //---------------------------------------------------------------------------
  /**
   * @brief 公開鍵取得用POST通信バッファをセット
   *
   * @param[in] heap 使用されるヒープエリア
   *
   * @return 生成されたPOSTデータサイズ
   */
  //---------------------------------------------------------------------------
  u32 SetupUpdatingPublicKeyPostBuffer( gfl2::heap::HeapBase* heap );

  //---------------------------------------------------------------------------
  /**
   * @brief HTTPステータスエラーのハンドリング
   *
   * @param[in] statusCode HTTPステータスコード
   */
  //---------------------------------------------------------------------------
  void DoHttpStatusError( s32 statusCode );

  //---------------------------------------------------------------------------
  /**
   * @brief  ポケモン検査時のレスポンスデータのパース処理
   *
   * @param[in]  response  サーバから返却されたレスポンス情報
   */
  //---------------------------------------------------------------------------
  void ParsePokemonValidationResponse( const NetLib::HTTP::NexHttpResponse& response );

  //---------------------------------------------------------------------------
  /**
   * @brief  公開鍵更新時のレスポンスデータのパース処理
   *
   * @param[in]  response  サーバから返却されたレスポンス情報
   */
  //---------------------------------------------------------------------------
  void ParseUpdationgPublicKeyResponse( const NetLib::HTTP::NexHttpResponse& response );

  //---------------------------------------------------------------------------
  /**
   * @brief RsaContextをSavedata::ValidationSaveを使用してセットアップする
   */
  //---------------------------------------------------------------------------
  void SetupRsaContext( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 指定数値をネットワークバイトオーダーに変換
   *
   * @param[in] val 変換前数値
   *
   * @return 変換されたデータ
   */
  //---------------------------------------------------------------------------
  static u16 SwapU16(u16 val);

  //---------------------------------------------------------------------------
  /**
   * @brief 指定数値をネットワークバイトオーダーに変換
   *
   * @param[in] val 変換前数値
   *
   * @return 変換されたデータ
   */
  //---------------------------------------------------------------------------
  static s32 SwapS32(s32 val);

  //---------------------------------------------------------------------------
  /**
  * @brief 公開鍵更新通信の開始（非同期）
  *
  * @return 実行開始が正常に行えたならtrueを返却
  */
  //---------------------------------------------------------------------------
  bool GetPublicKeyASync( void );

  //---------------------------------------------------------------------------
  /**
   * @brief ゲームモード別に送信する1体のポケモン情報のサイズを取得する
   *
   * @return 1体分のポケモン情報のサイズ
   */
  //---------------------------------------------------------------------------
  u32 GetPokemonDataSize( void ) const;


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
  //------------------------------------------------------------------
  /**
   * @brief   ローカル不正チェック関連
   */
  //------------------------------------------------------------------
  //---------------------------------------------------------------------------
  /**
   * @brief   モンスター名チェック＆置換
   */
  //---------------------------------------------------------------------------
  static void ReplaceMonsName( pml::pokepara::CoreParam* pCoreParam );

  //---------------------------------------------------------------------------
  /**
   * @brief   モンスター名チェック＆置換
   */
  //---------------------------------------------------------------------------
  static void ReplaceMonsName( gfl2::str::STRCODE* pNickName , u8 pokeLang , MonsNo monsNo );

  //---------------------------------------------------------------------------
  /**
   * @brief   プレイヤー名チェック＆置換
   */
  //---------------------------------------------------------------------------
  static void ReplacePlayerName( gfl2::str::STRCODE* pName , u8 pokeLang , u8 romVersion );

private:

  //---------------------------------------------------------------------------
  /**
   * @brief   文字列不正チェック
   */
  //---------------------------------------------------------------------------
  static bool IsValidName( const gfl2::str::STRCODE* pName , u8 langCode );

  //---------------------------------------------------------------------------
  /**
   * @brief  文字コードチェック
   */
  //---------------------------------------------------------------------------
  static bool check_unicodemap( gfl2::str::STRCODE code , int index , bool* pZenkaku );

  //---------------------------------------------------------------------------
  /**
   * @brief  文字列チェック
   */
  //---------------------------------------------------------------------------
  static PokemonValidation::NAME_CHECK_RESULT checkName( const gfl2::str::STRCODE * name, int length );

public:
#if PM_DEBUG
  // for Debug
  static bool m_debugMaintenance;     //!< メンテナンスデバッグ
  static bool m_debugFaileFlag;       //!< 通信失敗デバッグ
  static bool m_debugInvalidPokemon;  //!< 不正ポケモンデバッグ
  static bool m_debugInvalidCert;     //!< 不正署名デバッグ
#endif

private:
  /**
   * @brief 動作モード
   */
  enum EXEC_MODE
  {
    E_EXEC_NONE,                //!< 未設定
    E_EXEC_POKEMON_VALIDATION,  //!< ポケモンの検査
    E_EXEC_UPDATE_PUBLIC_KEY,   //!< 公開鍵の更新
  };
  //
  static const nn::nex::qChar8* VALIDATOR_URL;
  static const nn::nex::qChar8* PUBLIC_KEY_URL;
  static const nn::nex::qChar8* DEBUG_VALIDATOR_URL;
  static const nn::nex::qChar8* DEBUG_PUBLIC_KEY_URL;
  //
  bool                                m_isInit;         //!< 初期化フラグ
  EXEC_MODE                           m_execMode;       //!< 動作モード
  POKMEON_CHECKDATA                   m_pokeData;       //!< 作業用バッファ
  u8*                                 m_caBuffer;       //!< 証明書用バッファ
  u8*                                 m_postBuffer;     //!< POST用バッファ
  size_t                              m_caSize;         //!< 証明書のサイズ
  u32                                 m_sendPokemonNum; //!< 送付ポケモン数
  PokemonValidationListener*          m_listener;       //!< イベントリスナ
  //
  u8                                  m_shaWork[512];   //!< Work
  nn::crypto::RsaContext              m_rsaContext;     //!< RSA-Context
  //
  gfl2::heap::HeapBase*                m_heap;           //!< ワークメモリ
  //
  NetLib::HTTP::NexHttpClient*        m_httpCon;        //!< HTTPクライアント
  NetLib::HTTP::HttpConnectSetting    m_setting;        //!< HTTP設定
};


} // validation
} // netapp

#endif  // _POKEMON_VALIDATION_H_
#endif