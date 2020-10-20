//======================================================================
/**
 * @file  PGLConnector.cpp
 *
 * @author suzuki_hisanao
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#include <pml/include/pmlib.h>
#include <system/include/PokemonVersion.h>
#include "system/include/PokemonLanguage.h"
#include <NetStatic/NetLib/include/PGL/PGLConnector.h>
//#include <arc/arc_def.h>
//#include <net/net_define.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/JoinFestaDataSave.h>
// グローバルアトラクション定義
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaGlobalAttractionDefine.h"
//#include <arc/cert.gaix>
#include  <debug/include/gfl2_DebugLogWindow.h>

#include <System/include/SystemEventManager.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "NetStatic/NetAppLib/include/Util/NetAppDateTime.h"

namespace NetLib{
namespace Pgl{


//------------------------------------
// フロントエンドURL
//------------------------------------

// momiji用
char*  PGLConnector::FRONTEND_BASE_URL   = "https://3ds4-**.pokemon-gl.com/api/";
char*  PGLConnector::FRONTEND_SERIAL_URL = "https://3ds4-fushigi.pokemon-gl.com/api/";

char  PGLConnector::m_baseURL[PGLConnector::URL_LENGTH];
char  PGLConnector::m_serialURL[PGLConnector::URL_LENGTH];
char  PGLConnector::m_jsonURL[PGLConnector::URL_LENGTH];

u16   PGLConnector::m_ganeSyncTimeout = TIMEOUT_GAMESYSNC;

#if PM_DEBUG
u32 PGLConnector::m_dbgBattleType = 0;
#endif

/**
 * @brief リージョンドメイン
 */
const char PGLConnector::REGION_DOMAIN[nn::cfg::CTR::CFG_REGION_MAX][3] =
{
  "jp", //!< 日本リージョン
  "us", //!< 北米リージョン
  "eu", //!< 欧州および豪州リージョン
  "eu", //!< 豪州リージョンは欧州リージョンと統合されました。この値は使用されません。
  "eu", //!< 中国リージョン
  "kr", //!< 韓国リージョン
  "eu", //!< 台湾リージョン
};



//--------------------------------------------------------------------------------------------
/**
* @brief コンストラクタ
*/
//--------------------------------------------------------------------------------------------
PGLConnector::PGLConnector( void ) : m_isInit(false), m_listener(NULL), m_httpCon(NULL), m_currentCommand(E_CMD_NONE)
{
  m_setting.Initialize();

  m_postWorkUseNum = 0;
  m_crtSize        = 0;
  m_region         = nn::cfg::CTR::GetRegion();

  if( ::std::strlen(m_baseURL) == 0 ){
    ::std::strncpy( m_baseURL, FRONTEND_BASE_URL, sizeof(m_baseURL)-1 );
  }
  if( ::std::strlen(m_serialURL) == 0 ){
    ::std::strncpy( m_serialURL, FRONTEND_SERIAL_URL, sizeof(m_serialURL)-1 );
  }
  if( ::std::strlen(m_jsonURL) == 0 ){
    ::std::strncpy( m_jsonURL, "http://t1045.3hc.jp/frontend/", sizeof(m_jsonURL)-1 );
  }

  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);
}


//--------------------------------------------------------------------------------------------
/**
* @brief デストラクタ
*/
//--------------------------------------------------------------------------------------------
PGLConnector::~PGLConnector()
{
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RemoveSystemEventListener(this);
  Finalize();
}


//--------------------------------------------------------------------------------------------
/**
* @brief 初期化処理(ブロック関数)
*
* @param heap        内部バッファ確保用に使用されるヒープ
* @param nexUniqueId 使用されるＮＥＸユニークＩＤ
* @param authToken   認証トークン
*
* @return 成功ならtrue、失敗したならfalseを返却
*/
//--------------------------------------------------------------------------------------------
bool PGLConnector::Initialize( gfl2::heap::HeapBase *heap, u64 nexUniqueId, const char* authToken, const nn::cfg::CTR::CfgRegionCode region )
{
  if( !m_isInit )
  {
    m_httpCon = GFL_NEW(heap) NetLib::HTTP::NexHttpClient();
    if( m_httpCon == NULL )
    {
      Finalize();
      return false;
    }
    m_httpCon->Initialize();
    m_httpCon->RegistNexHttpClientListener(this);

    const u8 langCode = System::GetLang();

    ::std::snprintf( m_nexUniqueId, sizeof(m_nexUniqueId), "%llu", nexUniqueId );
    ::std::snprintf( m_romCode,     sizeof(m_romCode),     "%u",   static_cast<u32>(GET_VERSION()) );
    ::std::snprintf( m_langCode,    sizeof(m_langCode),    "%u",   static_cast<u32>(langCode) );

    ::std::memcpy( m_authToken, authToken, nn::nex::IndependentServer::AUTH_TOKEN_SIZE);

#ifdef PGL_USE_DEBUG_CER
    LoadCertifyFile( heap, GARC_cert_PGL_K_CA_DER, m_crtBuffer, &m_crtSize );
#endif

    m_isInit       = true;
    m_region       = region;
  }

  return true;
}


//--------------------------------------------------------------------------------------------
/**
* @brief 終了処理
 *
 * @caution 必ず事前にIsConnecting()がfalseであることを確認してください
*/
//--------------------------------------------------------------------------------------------
void PGLConnector::Finalize()
{
  if( m_isInit )
  {
    if( m_httpCon != NULL )
    {
      m_httpCon->UnregistNexHttpClientListener(this);
      m_httpCon->Finalize();
    }

    GflHeapSafeFreeMemory( m_httpCon  );
    m_isInit = false;
  }
}

#ifdef PGL_USE_DEBUG_CER
//--------------------------------------------------------------------------------------------
/**
 * @brief 証明書ファイルの読み込み
 *
 * @param heap     ロードに使用するメモリ
 * @param fileId   ロードするファイルID
 * @param caBuffer 証明書の格納先
 * @param caSize   証明書のサイズ
 *
 * @return ロードに成功したならtrueを返却
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::LoadCertifyFile( gfl2::heap::HeapBase * heap, u32 fileId, u8* caBuffer, size_t* caSize )
{
  if( (caBuffer == NULL) || (caSize==NULL) ){
    return false;
  }
  gfl2::fs::ArcFile * af = GFL_NEW( heap ) gfl2::fs::ArcFile( heap, ARCID_CERT, gfl2::fs::ArcFile::OPEN );
  size_t buffSize = af->GetDataSize( fileId );

  if( buffSize == 0 )
  {
    GFL_SAFE_DELETE( af );
    return false;
  }
  *caSize = af->GetRealDataSize( fileId );

  size_t readSize = af->LoadData( fileId, caBuffer );
  GFL_SAFE_DELETE( af );

  return readSize == buffSize;
}
#endif


//--------------------------------------------------------------------------------------------
/**
* @brief HTTP接続の開始
*
* @note 本関数コール前に[NetLib::HTTP::HttpConnectSetting]を正しく設定すること
* @note なお関数がfalseを返却した場合は、PGLConnectorListenerイベントは発行されない。
*
* @param setting ＨＴＴＰ接続用設定
*
* @return 実行開始が正常に行えたならtrueを返却
*/
//--------------------------------------------------------------------------------------------
bool PGLConnector::Connect( const NetLib::HTTP::HttpConnectSetting &setting )
{
  m_setting.url = m_url;

  SetupCert( &m_setting );

  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"-------------", m_url );
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"POST[%s]", m_url );

  for( int i = 0; i < m_setting.addPostParamNum; i++ )
  {
    gflnet2::http::PostParam *param = &m_setting.addPostParam[i];

    if( std::strlen( param->queryValue ) >= 60 ){
      gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"[%s=**********]", param->queryName );
    }
    else{
      gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"[%s=%s]", param->queryName, param->queryValue );
    }
  }
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"" );
  #endif


  return m_httpCon->StartPost( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
* @brief イベントリスナを登録
*
* @param listener 検査結果通知先イベント
*/
//--------------------------------------------------------------------------------------------
void PGLConnector::SetPGLConnectorListener( PGLConnectorListener* listener )
{
  m_listener = listener;
}


//--------------------------------------------------------------------------------------------
/**
* @brief イベントリスナを解除
*/
//--------------------------------------------------------------------------------------------
void PGLConnector::RemovePGLConnectorListener( void )
{
  m_listener = NULL;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 通信キャンセルリクエストを発行する
 * @brief キャンセルが実施された場合、完了時にOnPGLConnectCanceled()がコールされる。
 *
 * @note  通信状態にかかわらず、コールしても良い
 *
 * @return キャンセル処理が実施されたならtrueを返却する
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::CancelConnecting( void )
{
  if( m_httpCon == NULL ){
    return false;
  }
  return m_httpCon->CancelConnecting();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief セーブデータ登録通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLCreateSavedataSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param base64Savedata      BASE64エンコードされたセーブデータ
 * @param savedataBinarySize  セーブデータのバイナリ時のサイズ
 * @param base64PlayerIcon    BASE64エンコードされたプレイヤーアイコン画像データ
 * @param payerIconBinarySize アイコン画像のバイナリ時のサイズ
 * @param isParentalControl   ペアレンタルコントロールの有無(ペアレンタル制限有効ならtrue)
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::CreateSaveData( const char* base64Savedata, u32 savedataBinarySize,
                                   const char* base64PlayerIcon, u32 payerIconBinarySize,
                                   bool  isParentalControl )
{
  return ConnectGameSyncInternal( E_CMD_SAVEDATA_CREATE,
                                  m_region,
                                  base64Savedata, savedataBinarySize,
                                  base64PlayerIcon, payerIconBinarySize,
                                  NOT_SEND_ITEMSPACE, isParentalControl, NULL );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief ポケモンBOX情報なしセーブデータ同期用通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLSyncSaveDataSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param base64Savedata      BASE64エンコードされたセーブデータ
 * @param savedataBinarySize  セーブデータのバイナリ時のサイズ
 * @param base64PlayerIcon    BASE64エンコードされたプレイヤーアイコン画像データ
 * @param payerIconBinarySize アイコン画像のバイナリ時のサイズ
 * @param itemFreeSpace       ゲーム側で取得可能なアイテム数
 * @param isParentalControl   ペアレンタルコントロールの有無(ペアレンタル制限有効ならtrue)
 * @param photoInfo           3Dカメラ写真情報（省略可能）
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::SyncNonboxSaveData( const char* base64Savedata, u32 savedataBinarySize,
                                       const char* base64PlayerIcon, u32 payerIconBinarySize,
                                       u8    itemFreeSpace,
                                       bool  isParentalControl,
                                       const GameSyncCameraInfo* photoInfo )
{
  return ConnectGameSyncInternal( E_CMD_SAVEDATA_SYNC,
                                  m_region,
                                  base64Savedata, savedataBinarySize,
                                  base64PlayerIcon, payerIconBinarySize,
                                  itemFreeSpace, isParentalControl, photoInfo );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief FULL情報セーブデータ同期用通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLSyncSaveDataSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param base64Savedata      BASE64エンコードされたセーブデータ
 * @param savedataBinarySize  セーブデータのバイナリ時のサイズ
 * @param base64PlayerIcon    BASE64エンコードされたプレイヤーアイコン画像データ
 * @param payerIconBinarySize アイコン画像のバイナリ時のサイズ
 * @param itemFreeSpace       ゲーム側で取得可能なアイテム数
 * @param isParentalControl   ペアレンタルコントロールの有無(ペアレンタル制限有効ならtrue)
 * @param photoInfo           3Dカメラ写真情報（省略可能）
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::SyncFullSaveData( const char* base64Savedata, u32 savedataBinarySize,
                                     const char* base64PlayerIcon, u32 payerIconBinarySize,
                                     u8    itemFreeSpace,
                                     bool  isParentalControl,
                                     const GameSyncCameraInfo* photoInfo )
{
  return ConnectGameSyncInternal( E_CMD_SAVEDATA_FULL_SYNC,
                                  m_region,
                                  base64Savedata, savedataBinarySize,
                                  base64PlayerIcon, payerIconBinarySize,
                                  itemFreeSpace, isParentalControl, photoInfo );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief ゲームシンク送信用内部関数（非同期通信）
 *
 * @param command             実行するゲームシンクコマンド
 * @param region              本体リージョン
 * @param base64Savedata      BASE64エンコードされたセーブデータ
 * @param savedataBinarySize  セーブデータのバイナリ時のサイズ
 * @param base64PlayerIcon    BASE64エンコードされたプレイヤーアイコン画像データ
 * @param payerIconBinarySize アイコン画像のバイナリ時のサイズ
 * @param itemSpace           ゲーム側で取得可能なアイテム数。負の値をセットするとサーバに送信しない。
 * @param isParentalControl   ペアレンタルコントロールの有無(ペアレンタル制限有効ならtrue)
 * @param photoInfo           3Dカメラ写真情報（省略可能）
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::ConnectGameSyncInternal( PGL_COMMAND_TYPE command,
                                   const nn::cfg::CTR::CfgRegionCode region,
                                   const char* base64Savedata, u32 savedataBinarySize,
                                   const char* base64PlayerIcon, u32 payerIconBinarySize,
                                   s32   itemSpace,
                                   bool  isParentalControl,
                                   const GameSyncCameraInfo* photoInfo )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }

  GFL_PRINT( "player icon[%s]\n" , base64PlayerIcon );

  u32 addParamNum = 11;
  SetupUrl( command );

  // BASE64エンコードされたセーブデータ
  AddPostParam( "savedata", base64Savedata );
  // セーブデータのバイナリ時のサイズ
  AddPostParamWithTmp( "savedatasize", static_cast<u32>(savedataBinarySize) );
  //  BASE64エンコードされたプレイヤーアイコン画像データ
  AddPostParam( "playericon", base64PlayerIcon );
  // アイコン画像のバイナリ時のサイズ
  AddPostParamWithTmp( "playericonsize", static_cast<u32>(payerIconBinarySize) );
  // ペアレンタルコントロール制限の有無
  AddPostParamWithTmp( "parental", static_cast<u32>(isParentalControl) );
  // 本体リージョン
  AddPostParamWithTmp( "region", static_cast<u32>(region) );

  if( itemSpace >= 0 )
  {
    // ゲーム側で取得可能なアイテム数
    AddPostParamWithTmp( "itemspace", static_cast<u32>(itemSpace) );
    addParamNum++;
  }

  if( photoInfo != NULL )
  {
    // BASE64エンコードされた3Dカメラデータ
    AddPostParam( "picdata", photoInfo->m_base64Picture );
    // 3Dカメラデータのバイナリ時のサイズ
    AddPostParamWithTmp( "picdatasize", static_cast<u32>(photoInfo->m_pictureBinarySize) );
    // 日付
    AddPostParamWithTmp( "picdate", photoInfo->m_pictureDate );

    addParamNum += 3;
  }

  //グローバルアトラクション
  for( u32 index = 0 ; index < NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX ; ++index )
  {
    if( NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK == GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->participateState &&
        NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_NOW == GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->openState
      )
    {
      // @fix GFNMCat[5599] カンスト処理
      u16 localRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->localRecord;
      if( localRecord >= GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->localRecordMax )
      {
        localRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->localRecordMax;
      }
      // momiji変更：nijiのgaidは1～12,momijiのgaidは13～24になる為、12プラス。
      AddPostParamWithTmp( "gaid", static_cast<u32>( index + 1 + 12 ) );
      AddPostParamWithTmp( "gacount", static_cast<u32>( localRecord ) );
      GFL_PRINT("UPLOAD_GA : [%d][%d]\n" , index+1+12 , localRecord );
      addParamNum += 2;

      break;
    }
  }

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==addParamNum ); // @check

  return Connect( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief ゲームシンク完了報告通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLCommitGameSyncSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::CommitGameSync( void )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_SAVEDATA_FINISH );

  // 本体リージョン
  AddPostParamWithTmp( "region", static_cast<u32>(m_region) );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==6 ); // @check

  return Connect( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief セーブデータダウンロード通信（非同期通信）
 *
 * @attention Initialize()で指定したnexUniqueIdのセーブデータをダウンロードする
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLDownloadSavedataSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param savedataType ダウンロードするセーブデータタイプ
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::DownloadSaveData( PGL_SAVEDATA_DOWNLOAD_TYPE savedataType )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_SAVEDATA_DOWNLOAD );

  // 本体リージョン
  AddPostParamWithTmp( "region", static_cast<u32>(m_region) );
  // セーブデータタイプ
  AddPostParamWithTmp( "savedatatype", static_cast<u32>(savedataType) );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==7 ); // @check

  return Connect( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief プレイステータス確認通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLGetPlayStatusSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param  purpose プレイステータス確認の目的
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::GetPlayStatus( PGL_PURPOSE_TYPE purpose )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_ACCOUNT_GETPLAYSTATUS );

  // プレイステータス確認の目的
  AddPostParamWithTmp( "purpose", static_cast<u32>(purpose) );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==6 ); // @check

  return Connect( m_setting );
}


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
bool PGLConnector::GetWorldBattleStatus( u32 battleType )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_WORLDBATTLE_GETSTATUS );

#if PM_DEBUG
  if( m_dbgBattleType != 0 )
  {//強制的に0にする
    battleType = 0;
  }
#endif

  // 大会タイプ
  AddPostParamWithTmp( "battletype", battleType );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==6 ); // @check

  return Connect( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief バトルエントリー状態更新通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLUpdateWorldBattleStatusSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param  gpfEntryFlg       PGLエントリーワーク
 * @param  wifiMatchupState  大会の進行情報
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::UpdateWorldBattleStatus( u8 gpfEntryFlg, u8 wifiMatchupState , u32 battleType )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_WORLDBATTLE_UPDATESTATUS );

  // PGLエントリーワーク
  AddPostParamWithTmp( "gpfentryflg", static_cast<u32>(gpfEntryFlg) );

  // 大会の進行情報
  AddPostParamWithTmp( "wifimatchupstate", static_cast<u32>(wifiMatchupState) );

#if PM_DEBUG
  if( m_dbgBattleType != 0 )
  {//強制的に0にする
    battleType = 0;
  }
#endif

  // 大会タイプ
  AddPostParamWithTmp( "battletype", battleType );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==8 ); // @check

  return Connect( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief バトルボックス登録通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLUploadBattleBoxSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param base64BattleBoxData  BASE64エンコードを行ったバトルボックスデータ。終端に'\0'必須！
 * @param battleBoxBinarySize  バトルボックスデータのバイナリ時のサイズ
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::UploadBattleBox( const char* base64BattleBoxData, u32 battleBoxBinarySize , u32 battleType )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_BATTLEBOX_UPLOAD );

  // BASE64エンコードを行ったバトルボックスデータ
  AddPostParam( "battlebox", base64BattleBoxData );

  // バトルボックスデータのバイナリ時のサイズ
  AddPostParamWithTmp( "battleboxsize", battleBoxBinarySize );

  // 大会タイプ
  AddPostParamWithTmp( "battletype", static_cast<u32>(battleType) );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==8 ); // @check

  return Connect( m_setting );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief バトルボックス取得通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLDownloadBattleBoxSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param nexUniqueIdOfTargertUser  取得対象者のＮＥＸユニークＩＤ
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::DownloadBattleBox( u64 nexUniqueIdOfTargertUser , u32 battleType )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_BATTLEBOX_DOWNLOAD );

  // 取得するバトルボックスの持ち主のGSID
  AddPostParamWithTmp( "opponent_gsid", nexUniqueIdOfTargertUser );

  // 大会タイプ
  AddPostParamWithTmp( "battletype", static_cast<u32>(battleType) );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==7 ); // @check

  return Connect( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 不思議な贈り物シリアル認証通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLAuthSerialSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param address    アドレス情報
 * @param serialCode シリアルコード文字列(通信完了までインスタンス保持必須）
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::AuthSerialCode( const nn::cfg::CTR::SimpleAddressId& address,
                                   const char* serialCode )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_SERIAL_AUTH );

  // シリアルコード
  AddPostParamWithTmp( "serialcode", serialCode );
  // 国コード
  AddPostParamWithTmp( "countrycode", static_cast<u32>(address.GetCountryCode()) );
  // 地域コード
  AddPostParamWithTmp( "areacode", static_cast<u32>(address.GetRegionCode()) );
  // 本体リージョン
  AddPostParamWithTmp( "region", static_cast<u32>(m_region) );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==9 ); // @check

  return Connect( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 不思議な贈り物シリアル受け取り完了報告通信（非同期通信）
 *
 * @note  通信成功時に「PGLConnectorListener::OnPGLUpdateSerialStatusSuccess()」がコールされる。
 * @note  本関数失敗時はPGLConnectorListenerイベントはコールされない。
 *
 * @param address    アドレス情報
 * @param serialCode シリアルコード文字列(通信完了までインスタンス保持必須）
 *
 * @return 通信開始結果。正常に通信を開始できたならtrueを返却する。
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::UpdateSerialCode( const nn::cfg::CTR::SimpleAddressId& address,
                                     const char* serialCode )
{
  if( !m_isInit || m_httpCon->IsConnecting() ){
    return false; // 未初期化, 通信中
  }
  SetupUrl( E_CMD_SERIAL_UPDATE_STATUS );

  // 国コード
  AddPostParamWithTmp( "countrycode", static_cast<u32>(address.GetCountryCode()) );
  // 地域コード
  AddPostParamWithTmp( "areacode", static_cast<u32>(address.GetRegionCode()) );
  // 本体リージョン
  AddPostParamWithTmp( "region", static_cast<u32>(m_region) );
  // シリアルコード
  AddPostParamWithTmp( "serialcode", serialCode );

  // パラメータ数をチェック
  GFL_ASSERT( m_setting.addPostParamNum==9 ); // @check

  return Connect( m_setting );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief ＵＲＬの設定
 *
 * @param command 実行するコマンド
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::SetupUrl( PGL_COMMAND_TYPE command )
{
  bool isSerialURL = false;
  switch( command )
  {
    case  E_CMD_SAVEDATA_CREATE:          // セーブデータ生成
      std::strncpy( m_command, "savedata.create", sizeof(m_command) );
      break;
    case  E_CMD_SAVEDATA_SYNC:            // セーブデータ同期（BOXなし）
      std::strncpy( m_command, "savedata.sync", sizeof(m_command) );
      break;
    case  E_CMD_SAVEDATA_FULL_SYNC:       // セーブデータ同期（すべて）
      std::strncpy( m_command, "savedata.fullsync", sizeof(m_command) );
      break;
    case  E_CMD_SAVEDATA_FINISH:          // セーブデータ同期完了
      std::strncpy( m_command, "savedata.finish", sizeof(m_command) );
      break;
    case  E_CMD_SAVEDATA_DOWNLOAD:        // セーブデータダウンロード
      std::strncpy( m_command, "savedata.download", sizeof(m_command) );
      break;
    case  E_CMD_ACCOUNT_GETPLAYSTATUS:    // プレイステータス確認
      std::strncpy( m_command, "account.getplaystatus", sizeof(m_command) );
      break;
    case  E_CMD_WORLDBATTLE_GETSTATUS:    // バトルエントリー確認
      std::strncpy( m_command, "worldbattle.getstatus", sizeof(m_command) );
      break;
    case  E_CMD_WORLDBATTLE_UPDATESTATUS: // バトル参加状態更新
      std::strncpy( m_command, "worldbattle.updatestatus", sizeof(m_command) );
      break;
    case  E_CMD_BATTLEBOX_UPLOAD:         // バトルボックス登録
      std::strncpy( m_command, "battlebox.upload", sizeof(m_command) );
      break;
    case   E_CMD_BATTLEBOX_DOWNLOAD:      // バトルボックス取得
      std::strncpy( m_command, "battlebox.download", sizeof(m_command) );
      break;
    case  E_CMD_SERIAL_AUTH:              // ふしぎなおくりものシリアル認証
      std::strncpy( m_command, "serial.auth", sizeof(m_command) );
      isSerialURL = true;
      break;
    case  E_CMD_SERIAL_UPDATE_STATUS:     // ふしぎなおくりもの状態更新
      std::strncpy( m_command, "serial.updatestatus", sizeof(m_command) );
      isSerialURL = true;
      break;
    default:
      GFL_ASSERT(false); // @check
      break;
  }
  m_currentCommand = command;

  // URLを設定
  if( isSerialURL ){
    ::std::snprintf( m_url, sizeof(m_url), "%s%s", m_serialURL, m_command );
  }
  else{
    ::std::snprintf( m_url, sizeof(m_url), "%s%s", m_baseURL, m_command );
  }
  SetUrlWithRegion( m_url );

  SetupBasicPostBody();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief リージョン別URLの設定
 *
 * @param url リージョンコードを埋め込む対象となるURL
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::SetUrlWithRegion( char* url )
{
  char* hitStr = ::std::strstr( url, "**" );

  if( hitStr != NULL )
  {
    ::std::memcpy( hitStr, REGION_DOMAIN[m_region], 2 );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief デバッグ用証明書の割り当て
 *
 * @param setting ＨＴＴＰ接続用設定
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::SetupCert( NetLib::HTTP::HttpConnectSetting* setting )
{
#ifdef PGL_USE_DEBUG_CER
  {
    setting->certData     = m_crtBuffer;
    setting->certDataSize = m_crtSize;
  }
#else
  {
    setting->certData     = NULL;
    setting->certDataSize = 0;
  }
#endif
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 共通ポストパラメータを設定
 *
 * @attention 事前にSetupUrl()をコールすること
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::SetupBasicPostBody( void )
{
  m_setting.Initialize();
  m_postWorkUseNum = 0;

  // タイムアウト時間
  switch( m_currentCommand )
  {
    case E_CMD_SAVEDATA_CREATE:
    case E_CMD_SAVEDATA_SYNC:
    case E_CMD_SAVEDATA_FULL_SYNC:
      m_setting.timeoutMillSeconds = m_ganeSyncTimeout;
      break;

    default:
      m_setting.timeoutMillSeconds = TIMEOUT_MIN;
      break;
  }
  m_setting.timeoutMillSeconds *= 1000; // ミリ秒に変換

  // パラメータ名(APIコマンド名)
  AddPostParam( "p", m_command );

  // ゲームシンクID
  AddPostParam( "gsid", m_nexUniqueId );

  // ロムＩＤ
  AddPostParam( "rom", m_romCode );

  // 言語コード
  AddPostParam( "langcode", m_langCode );

  // 認証トークン
  AddPostParam( "tok", m_authToken );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief クエリパラメータを登録する(内部テンポラリバッファを利用）
 *
 * @param queryName クエリ名
 * @param value     パラメータへのポインタ
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::AddPostParamWithTmp( const char* queryName, const gfl2::system::Date* value )
{
  GFL_ASSERT(m_postWorkUseNum<POST_BUFFER_SIZE); // @fix

  if( m_postWorkUseNum < POST_BUFFER_SIZE )
  {
    char *tmp = m_postWork[m_postWorkUseNum];

    s32 year  = value->GetYear();
    s32 month = value->GetMonth();
    s32 day   = value->GetDay();
    s32 hour  = value->GetHour();
    s32 min   = value->GetMinute();
    s32 sec   = value->GetSecond();

    //スペースはURLエンコード(%20)で記載
    ::std::snprintf( tmp, sizeof(m_postWork[m_postWorkUseNum]), "%04d/%02d/%02d %02d:%02d:%02d", year, month, day, hour, min, sec );
    m_postWorkUseNum++;

    AddPostParam( queryName, tmp );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief クエリパラメータを登録する(内部テンポラリバッファを利用）
 *
 * @param queryName クエリ名
 * @param value     パラメータへのポインタ
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::AddPostParamWithTmp( const char* queryName, u64 value )
{
  GFL_ASSERT(m_postWorkUseNum<POST_BUFFER_SIZE); // @fix

  if( m_postWorkUseNum<POST_BUFFER_SIZE )
  {
    char *tmp = m_postWork[m_postWorkUseNum];

    std::snprintf( tmp, sizeof(m_postWork[m_postWorkUseNum]), "%llu", value );
    m_postWorkUseNum++;

    AddPostParam( queryName, tmp );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief クエリパラメータを登録する(内部テンポラリバッファを利用）
 *
 * @param queryName クエリ名
 * @param value     パラメータへのポインタ
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::AddPostParamWithTmp( const char* queryName, const char* value )
{
  GFL_ASSERT(m_postWorkUseNum<POST_BUFFER_SIZE); // @check
  char *tmp = m_postWork[m_postWorkUseNum];

  std::strncpy( tmp, value, sizeof(m_postWork[m_postWorkUseNum])-1 );
  m_postWorkUseNum++;

  AddPostParam( queryName, tmp );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief クエリパラメータを登録する(内部テンポラリバッファを利用）
 *
 * @param queryName クエリ名
 * @param value     パラメータへのポインタ
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::AddPostParamWithTmp( const char* queryName, u32 value )
{
  GFL_ASSERT(m_postWorkUseNum<POST_BUFFER_SIZE); // @check
  char *tmp = m_postWork[m_postWorkUseNum];

  std::snprintf( tmp, sizeof(m_postWork[m_postWorkUseNum]), "%u", value );
  m_postWorkUseNum++;

  AddPostParam( queryName, tmp );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief クエリパラメータを登録する
 *
 * @param queryName クエリ名
 * @param value     パラメータへのポインタ
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::AddPostParam( const char* queryName, const char* value )
{
  if( m_setting.addPostParamNum >= HTTP_SETTING_PARAM_SIZE ){
    GFL_ASSERT( false ); // @check
    return;
  }

  gflnet2::http::PostParam* postParam = &m_setting.addPostParam[m_setting.addPostParamNum];

  postParam->type = gflnet2::http::E_POST_TYPE_ASCII;
  std::strncpy( postParam->queryName, queryName, sizeof(postParam->queryName)-1 );
  postParam->queryValue = const_cast<char*>(value);

  m_setting.addPostParamNum++;
}


//--------------------------------------------------------------------------------------------
/**
* @brief 稼働状態判定
*
* @return 稼働状態にあるならtrue、非稼働状態ならfalseを返却
*/
//--------------------------------------------------------------------------------------------
bool PGLConnector::IsEnable( void ) const
{
  return m_isInit;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 通信接続中か判定
 *
 * @return ポケモン不正チェックサーバと通信状態にあるならtrueを返却
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::IsConnecting( void ) const
{
  if( m_httpCon == NULL ){
    return false;
  }
  return m_httpCon->IsConnecting();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief レスポンスデータの解析する
 *
 * @param bodyPtr  レスポンスデータ
 * @param bodySize レスポンスデータサイズ
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::ParseResponse( const u8* bodyPtr, u32 bodySize )
{
  if( bodySize < sizeof(ResultCommonInfo) )
  {
    m_listener->OnPGLConnectError( m_currentCommand, nn::nex::qResult(false) );
    return; // 返却サイズが不正
  }
  ResultCommonInfo retCommonInf;
  u32              index = ParseResponseCommonHeader(&retCommonInf, bodyPtr);

  if( m_listener != NULL )
  {
    // 継続不可能なステータスコード
    if( retCommonInf.responseCode != E_RET_COMMON_OK )
    {
      m_listener->OnPGLConnectError( m_currentCommand, static_cast<PGL_COMMON_RESULT>(retCommonInf.responseCode) );
      return;
    }

    if( !ParseResponseBody( retCommonInf, &bodyPtr[index], bodySize-index ) )
    {
      // 不正な返却値
      m_listener->OnPGLConnectError( m_currentCommand, E_RET_ERR_SYSTEM );
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief レスポンス共通情報を解析する
 *
 * @param retCommonInf レスポンス共通情報格納先
 * @param bodyPtr      レスポンスデータ
 *
 * @return 取得したデータサイズ
 */
//--------------------------------------------------------------------------------------------
u32 PGLConnector::ParseResponseCommonHeader( ResultCommonInfo* retCommonInf, const u8* bodyPtr )
{
  // 基本情報をコピー
  std::memcpy( retCommonInf, bodyPtr, sizeof(ResultCommonInfo) );

  GFL_PRINT( "=================================\n" );
  GFL_PRINT( "retCommonInf.ret_cd[%u]\n",    retCommonInf->responseCode );
  GFL_PRINT( "retCommonInf.bodySize[%u]\n",  retCommonInf->bodySize );
  GFL_PRINT( "retCommonInf.start_sec[%u]\n", retCommonInf->startSec );
  GFL_PRINT( "retCommonInf.desc_cd[%u]\n",   retCommonInf->descCode );
  GFL_PRINT( "retCommonInf.message[%s]\n",   retCommonInf->descMessage );
  GFL_PRINT( "retCommonInf.yobi1[%s]\n",     retCommonInf->reserved1 );
  GFL_PRINT( "retCommonInf.yobi2[%s]\n",     retCommonInf->reserved2 );
  GFL_PRINT( "retCommonInf.ext_info[%u]\n",  retCommonInf->ext_info );
  GFL_PRINT( "=================================\n" );

  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"ret_cd[%u]",    retCommonInf->responseCode );
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"bodySize[%u]",  retCommonInf->bodySize );
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"start_sec[%u]", retCommonInf->startSec );
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"desc_cd[%u]",   retCommonInf->descCode );
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"message[%s]",   retCommonInf->descMessage );
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"yobi1[%s]",     retCommonInf->reserved1 );
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"yobi2[%s]",     retCommonInf->reserved2 );
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"ext_info[%u]",  retCommonInf->ext_info );
  #endif

  return sizeof(ResultCommonInfo);
}


//--------------------------------------------------------------------------------------------
/**
 * @brief ASCII数値を静数値に変換
 *
 * @param value     ASCII文字列
 * @param byteSize  文字データ格納サイズ
 *
 * @return 変換された整数値
 */
//--------------------------------------------------------------------------------------------
u32 PGLConnector::ConvertAsciiToInt( void* value, u32 byteSize )
{
  char tmp[16];
  std::memset( tmp, 0, sizeof(tmp) );

  std::memcpy( tmp, value, byteSize );

  return std::atol( tmp );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief アイテム情報のパース
 *
 * @param dstItemInf パースしたアイテム情報を格納する先
 * @param bodyPtr    レスポンスボディ
 * @param bodySize   レスポンスボディサイズ
 *
 * @return 格納されたアイテム数
 */
//--------------------------------------------------------------------------------------------
u32 PGLConnector::ParseItemInfo( ResultItemInfo* dstItemInf, const u8* bodyPtr, u32 bodySize , bool isSignIn )
{
  u16            resultItemNum = 0;  // サーバから返却されたアイテム数
  u8             validItemNum  = 0;  // 有効なアイテム数
  u32            address       = 0;  // 参照アドレス（

  ::std::memcpy( &resultItemNum, &bodyPtr[address], 2 );
  address += 3; // (itemCount[2byte] + dummyPadding[1byte])

  if( resultItemNum > RESULT_ITEM_SLOT )
  {
    return 0;   // 不正な値
  }

  u32 itemDataSize = (RESULT_ITEM_SLOT*sizeof(ResultItemInfo));

  if( bodySize >= (itemDataSize+address) )
  {
    for( u32 i = 0; i < RESULT_ITEM_SLOT; i++ )//アイテムが無くても回す
    {
      ::std::memcpy( &dstItemInf[validItemNum], &bodyPtr[address], sizeof(ResultItemInfo) );
      address += sizeof(ResultItemInfo);

      GFL_PRINT("PGLConnector::ParseItemInfo : itemID[%d]itemNum[%d]\n",dstItemInf[validItemNum].itemId,dstItemInf[validItemNum].itemNum);

      if( dstItemInf[validItemNum].itemId == 0 || dstItemInf[validItemNum].itemNum == 0 )
        continue;

      // CRCチェック
      if( dstItemInf[validItemNum].crc == nn::util::Crc16Ccitt::Calculate(&dstItemInf[validItemNum], 4) ){
        validItemNum++; // 有効データとしてカウント
      }
    }
  }

  u64 currentTime = 0;

  //グローバルアトラクション情報もパース
  if( bodySize > address )
  {
    // @fix GFNMCat[5587] 集計時間取得
    u16 year    = 0;
    u8  month   = 0;
    u8  day     = 0;
    u8  hour    = 0;
    u8  minute  = 0;

    gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager );
    
    nn::nex::DateTime nexDateTime;
    nn::nex::qBool bGetGameServerTimeResult = pNexManager->GetNgsFacade()->GetGameServerTime( &nexDateTime );
    
    if( bGetGameServerTimeResult )
    {
      year      = nexDateTime.GetYear();
      month     = nexDateTime.GetMonth();
      day       = nexDateTime.GetDay();
      hour      = nexDateTime.GetHour();
      minute    = nexDateTime.GetMinute();

      NetAppLib::Util::NetAppDateTime currentDateTime( year, month, day, hour, minute );
      currentTime = currentDateTime.GetTime();

      if( year >= 2000 )
      {
        year -= 2000; // @note : セーブデータが u8 なのと 仕様上 2100 年以降は未対応との事
      }

      if( year >= 99 )
      {
        year = 99; // @note : 表示仕様上 2 桁表示しか対応しない為 99 でカンストになる
      }
    }

    for( u32 i = 0 ; i < GA_NUM ; ++i )
    {
      GlobalAttractionInfo gaInfo;
      ::std::memcpy( &gaInfo , &bodyPtr[address] , sizeof(GlobalAttractionInfo) );
      address += sizeof(GlobalAttractionInfo);

      //セーブに反映
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->totalRecord = gaInfo.gacount;
      if( gaInfo.gastate == 0 )
      {
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->openState = gaInfo.gastate;
      }
      else
      if( gaInfo.gastate == 1 )
      {
        if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->openState != NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_TOTAL_NOW )
        {
          GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->openState = gaInfo.gastate;
        }
      }
      else
      if( gaInfo.gastate == 2 ) // @note : サーバー側の 2 は クライアント側の 3( OPEN_STATE_TOTAL_FINISH,//!< 開催終了で集計完了 )に相当する
      {
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->openState = NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_TOTAL_FINISH;
      }

      // @fix NMCat[4026] 開催中の時のみPGL会員登録フラグを更新する
      if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->openState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_NOW )
      {
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->isPglMember = isSignIn;
      }


      GFL_PRINT( "PGLConnector::ParseItemInfo : gacount[%d]gstate[%d]\n" , gaInfo.gacount , gaInfo.gastate );

      // @fix GFNMCat[5587] セーブデータに集計時間を設定
      if( bGetGameServerTimeResult )
      {
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->totalDlYear    = static_cast<u8>( year );
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->totalDlMonth   = month;
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->totalDlDay     = day;
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->totalDlHour    = hour;
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( i )->totalDlMinitue = minute;
      }
    }
  }

  {
    //グローバルアトラクションカウント更新
    for( u32 index = 0 ; index < NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX ; ++index )
    {
      u16 year    = static_cast<u16>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->endYear );
      u8  month   = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->endMonth;
      u8  day     = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->endDay;
      u8  hour    = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->endHour;
      u8  minute  = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->endMinitue;

      year += 2000; // @note セーブデータの方は2桁までなので2000足す必要がある

      NetAppLib::Util::NetAppDateTime endDateTime( year, month, day, hour, minute );
      u64 endTime = endDateTime.GetTime();

      bool bTimeCheckOK = false;

      // @fix GFNMCat[5606] 集計期間中はカウントアップさせないようにする
      if( currentTime > 0 && endTime > 0 )
      {
        if( currentTime <= endTime )
        {
          bTimeCheckOK = true;
        }
      }

      if( NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK == GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->participateState
       && NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_NOW == GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->openState
       && bTimeCheckOK )
      {
        // @fix GFNMCat[5599] カンスト処理
        u16 localRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->localRecord;
        if( localRecord >= GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->localRecordMax )
        {
          localRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->localRecordMax;
        }
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->uploadRecord = localRecord;
        GFL_PRINT( "PGLConnector::ParseItemInfo : update score[%d][%d]\n" , GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->GetGlobalAttractionData( index )->localRecord , index );
        break;
      }
    }
  }

  return validItemNum;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief レスポンスボディを解析する
 *
 * @param retCommonInf レスポンス共通情報
 * @param bodyPtr      レスポンスボディ
 * @param bodySize     レスポンスボディサイズ
 *
 * @return パース処理が成功したならtrueを返却する
 */
//--------------------------------------------------------------------------------------------
bool PGLConnector::ParseResponseBody( const ResultCommonInfo& retCommonInf, const u8* bodyPtr, u32 bodySize )
{
  GFL_PRINT( "ParseResponseBody(%u), size[%u]\n", m_currentCommand, bodySize );

  if( bodySize != retCommonInf.bodySize ){
    return false;   // ボディサイズがヘッダ情報記載のサイズと異なる
  }

  switch( m_currentCommand )
  {
    case  E_CMD_SAVEDATA_CREATE:          // セーブデータ生成
      m_listener->OnPGLCreateSavedataSuccess();
      return true;

    case  E_CMD_SAVEDATA_SYNC:            // セーブデータ同期（BOXなし）
    case  E_CMD_SAVEDATA_FULL_SYNC:       // セーブデータ同期（すべて）
      if( bodySize > 0 )
      {
        bool isSignIn = !(bodyPtr[0] == '0');

        if( bodySize >= 4 )
        {
          ResultItemInfo itemInf[20];
          u32 validItemNum = ParseItemInfo( itemInf, &bodyPtr[1], bodySize-1 , isSignIn );
          m_listener->OnPGLSyncSaveDataSuccess( isSignIn, validItemNum, itemInf );
        }
        else{
          m_listener->OnPGLSyncSaveDataSuccess( isSignIn, 0, NULL );
        }
        return true;
      }
      break;

    case  E_CMD_SAVEDATA_FINISH:          // セーブデータ同期完了

      m_listener->OnPGLCommitGameSyncSuccess();
      return true;

    case  E_CMD_SAVEDATA_DOWNLOAD:        // セーブデータダウンロード
      if( bodySize >= 5 )
      {
        u32 saveSize;
        u8  savedataType;
        u32 headerSize = sizeof(saveSize);
        ::std::memcpy( &savedataType, &bodyPtr[0], 1 );
        ::std::memcpy( &saveSize,     &bodyPtr[1], headerSize );
        // どうやらタイプはASCIIで返却されているのでここで整理する
        savedataType -= '0';

        m_listener->OnPGLDownloadSavedataSuccess( &bodyPtr[1+headerSize], saveSize, static_cast<PGL_SAVEDATA_DOWNLOAD_TYPE>(savedataType) );
        return true;
      }
      break;

    case  E_CMD_ACCOUNT_GETPLAYSTATUS:    // プレイステータス確認
      if( bodySize == 4)
      {
        u16 isSignIn;
        u16 timeout;
        ::std::memcpy( &isSignIn, &bodyPtr[0], 2 );
        ::std::memcpy( &timeout,  &bodyPtr[2], 2 );
        m_ganeSyncTimeout = gfl2::math::Clamp( timeout, TIMEOUT_MIN, TIMEOUT_MAX );

        m_listener->OnPGLGetPlayStatusSuccess( static_cast<bool>(isSignIn) );
        return true;
      }
      break;

    case  E_CMD_WORLDBATTLE_GETSTATUS:    // バトルエントリー確認
      if( bodySize == 11 )
      {
        u32  wifiMatchUpID;
        u16  gpfEntryFlg;
        u16  wifiMatchupState;
        u16  isSignIn;
        s8   timezone;

        ::std::memcpy( &wifiMatchUpID,    &bodyPtr[0],  4 );
        ::std::memcpy( &gpfEntryFlg,      &bodyPtr[4],  2 );
        ::std::memcpy( &wifiMatchupState, &bodyPtr[6],  2 );
        ::std::memcpy( &isSignIn,         &bodyPtr[8],  2 );
        ::std::memcpy( &timezone,         &bodyPtr[10], 1 );
        m_listener->OnPGLGetWorldBattleStatusSuccess( wifiMatchUpID, gpfEntryFlg, wifiMatchupState, static_cast<bool>(isSignIn), timezone );
        return true;
      }
      break;

    case  E_CMD_WORLDBATTLE_UPDATESTATUS: // バトル参加状態更新
      m_listener->OnPGLUpdateWorldBattleStatusSuccess();
      return true;

    case  E_CMD_BATTLEBOX_UPLOAD:         // バトルボックス登録
      m_listener->OnPGLUploadBattleBoxSuccess();
      return true;

    case   E_CMD_BATTLEBOX_DOWNLOAD:      // バトルボックス取得
      if( bodySize >= 4 )
      {
        u32 boxSize;
        ::std::memcpy( &boxSize, &bodyPtr[0], 4 );
        m_listener->OnPGLDownloadBattleBoxSuccess( &bodyPtr[4], boxSize );
        return true;
      }
      break;

    case  E_CMD_SERIAL_AUTH:              // ふしぎなおくりものシリアル認証
      if( bodySize == 6 )
      {
        u16 resultCode;
        u32 bossCode;
        ::std::memcpy( &resultCode, &bodyPtr[0], 2 );
        ::std::memcpy( &bossCode,   &bodyPtr[2], 4 );
        m_listener->OnPGLAuthSerialSuccess( static_cast<PGL_SERIAL_AUTH_RESULT>(resultCode), bossCode );
        return true;
      }
      break;

    case  E_CMD_SERIAL_UPDATE_STATUS:     // ふしぎなおくりもの状態更新
      if( bodySize == 2 )
      {
        u16 resultCode;
        ::std::memcpy( &resultCode, &bodyPtr[0], 2 );
        m_listener->OnPGLUpdateSerialStatusSuccess( static_cast<PGL_SERIAL_AUTH_RESULT>(resultCode) );
        return true;
      }
      break;
  }

  return false;
}



//--------------------------------------------------------------------------------------------
/**
 * @brief   HTTP通信の成功イベント<BR>
 *          ただし、サーバからのステータスコードについては必ず確認すること。
 *
 * @caution   イベントリスナ公開用の為、ユーザーコール禁止
 *
 * @param  response  サーバから返却されたレスポンス情報
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::OnNexHttpConnectionSuccess( const NetLib::HTTP::NexHttpResponse& response )
{
  s32 retCode = response.GetStatusCode();

  if( m_listener != NULL )
  {
    // レスポンスコード200台なら正常
    if( (retCode/100) == 2 )
    {
      const nn::nex::qVector<nn::nex::qByte>& bodyBuf = response.GetReceiveBuffer();

      #if PM_DEBUG
      gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"response[statusCode=%d][%d byte]", retCode, bodyBuf.size() );
      #endif

      ParseResponse( &(bodyBuf[0]), bodyBuf.size() );
    }
    else
    {
      #if PM_DEBUG
      gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"response[statusCode=%d] error!", retCode );
      #endif

      if( (retCode / 100) == 5 )
      {//5xxエラーだったら1時間アクセスロック
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetSaveData()->GetGameSync()->LockGameSyncSaveFor5xxError();
      }

      m_listener->OnPGLConnectError( m_currentCommand, retCode );
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   HTTP通信のエラーイベント
 *
 * @caution   イベントリスナ公開用の為、ユーザーコール禁止
 *
 * @param   result     エラーコード
 * @param   errorCode  ネットワークエラーコード
 */
//--------------------------------------------------------------------------------------------
void PGLConnector::OnNexHttpConnectionError( const nn::nex::qResult &result, u32 errorCode )
{
  GFL_PRINT( "PGLConnector::OnHttpConnectionError! code[%u] \n", errorCode );

  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"OnPGLConnectError[%u]", errorCode );
  #endif

  if( m_listener != NULL ){
    m_listener->OnPGLConnectError( m_currentCommand, result, errorCode );
  }
}


//---------------------------------------------------------------------------
/**
 * @brief   HTTP通信のキャンセルイベント
 */
//---------------------------------------------------------------------------
void PGLConnector::OnNexHttpConnectionCanceled( void )
{
  GFL_PRINT( "PGLConnector::OnNexHttpConnectionCanceled\n" );

  if( m_listener != NULL ){
    m_listener->OnPGLConnectCanceled( m_currentCommand );
  }
}



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
bool PGLConnector::OnSuspendWithHomeSleepAndPower( void )
{
  while( IsConnecting() )
  {
    CancelConnecting();
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
  
  return true;
}


} // pgl
} // netapp
#endif
