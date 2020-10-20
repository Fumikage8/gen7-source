//==============================================================================
/**
 * @file	NijiNetworkError.cpp
 * @brief	Niji通信エラー情報クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 16:39:24
 */
// =============================================================================
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
// message
#include <niji_conv_header/message/msg_message_error.h>
// mine
#include "NetStatic/NetLib/include/Error/NijiNetworkError.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
NijiNetworkError::NijiNetworkError()
{
  // 初期化
  m_errorInfoData.error_level  = ERROR_LEVEL_1; // 0でなくて1が初期値
  m_errorInfoData.error_type   = ERROR_TYPE_NONE;
  m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;
  m_errorInfoData.applet_arg   = 0;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
NijiNetworkError::~NijiNetworkError()
{
}

//=============================================================================
/**
 *  publicメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   エラー扱いではない設定にする
 * 
 * @note    エラーアプレットは表示するが、通信エラー状態にしたくない場合にコールする 
 */
//------------------------------------------------------------------
void NijiNetworkError::SetNotError()
{
  if( m_errorInfoData.error_level == ERROR_LEVEL_3 )
  {
    GFL_ASSERT_MSG(0, ">NetError エラー扱いが必須\n");
    return;
  }
  m_errorInfoData.error_level = ERROR_LEVEL_0;
  GFL_PRINT(">NetError ERROR_LEVEL_0に設定\n");
}

//------------------------------------------------------------------
/**
 * @brief   FatalError扱いにする
 * 
 * @note    エラーアプレットは表示しません。
 *          これ以降、通信エラーは無視します。
 */
//------------------------------------------------------------------
void NijiNetworkError::SetFatalError()
{
  // 通信エラーの後に、FatalErrorになるパターンはある。
  // 例）WiFi切断→Finalizeで拡張セーブ終了待ち→そこでFatalError
  m_errorInfoData.error_type   = ERROR_TYPE_FATAL;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );

  // FatalErrorはアプレットは表示しないので使いません
  m_errorInfoData.applet_type  = APPLET_TYPE_CODE;
  m_errorInfoData.applet_arg   = 0;
}

//------------------------------------------------------------------
/**
* @brief   エラー理由格納（直接エラー種類指定）
* @param   ENUM_ERROR_TYPE
*/
//------------------------------------------------------------------
void NijiNetworkError::SetResult( const ENUM_ERROR_TYPE result )
{
  // パラメータが必須なものは除外
  switch( result ){
  case ERROR_TYPE_WIFI_DISCONNECT:   ///< WiFi切断
  case ERROR_TYPE_NEX:               ///< Nexのエラー
  case ERROR_TYPE_ERROR_CODE:        ///< エラーコード直接代入
  case ERROR_TYPE_NETZ_FATAL:        ///< NetZのFatal
  case ERROR_TYPE_PGL:               ///< PGLサーバエラー
  case ERROR_TYPE_SERIAL:            ///< 不思議な贈り物シリアルサーバエラー
  case ERROR_TYPE_VALIDATION:        ///< 不正チェックサーバエラー
    // 開発中のみ発生するアサート
    GFL_ASSERT_MSG(0, "エラー格納関数が違います\n");
    return;
  }

  // 直接指定は、エラーメッセージ表示
  m_errorInfoData.error_type   = result;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );
  m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;  // メッセージ表示
  m_errorInfoData.applet_arg   = GetErrorMessageFromType( m_errorInfoData.error_type );
  GFL_PRINT(">NetError SetResult %d\n", result );
}


//------------------------------------------------------------------
/**
 * @brief   エラー理由格納(WiFi切断)  ERROR_LEVEL_3
 * @param   u32     U32のエラーコードが変換済みのもののみ代入
 *
 * @caution むやみに数値を入れる事が出来ますが、
 *            きちんと把握していないと不具合に繋がります。
 *            使用前にライブラリをよく見ること
 */
//------------------------------------------------------------------
void NijiNetworkError::SetResultWifiDisconnect( const u32 errorCode )
{
  m_errorInfoData.error_type   = ERROR_TYPE_WIFI_DISCONNECT;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );
  m_errorInfoData.applet_type  = APPLET_TYPE_CODE;  // エラーコード表示
  m_errorInfoData.applet_arg   = errorCode;
  GFL_PRINT(">NetError SetResultWifiDisconnect %d\n", m_errorInfoData.applet_arg );
}

#if defined(GF_PLATFORM_CTR)
//------------------------------------------------------------------
/**
* @brief   エラー理由格納(Nex)
* @param   nn::nex::qResult
*/
//------------------------------------------------------------------
void NijiNetworkError::SetResultNex( const nn::nex::qResult & result )
{
  m_errorInfoData.error_type   = ERROR_TYPE_NEX;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );
  m_errorInfoData.applet_type  = APPLET_TYPE_CODE;  // エラーコード表示
  m_errorInfoData.applet_arg   = nn::nex::ErrorCodeConverter::ConvertToNetworkErrorCode( result );
  GFL_PRINT(">NetError SetResultNex %d\n", m_errorInfoData.applet_arg );
}
#endif // GF_PLATFORM_CTR

//------------------------------------------------------------------
/**
* @brief   エラー理由格納
* @param   u32     ライブラリにはint32型でエラーを返す関数があるのでそれを代入
*
* @caution むやみに数値を入れる事が出来ますが、
*            きちんと把握していないと不具合に繋がります。
*            使用前にライブラリをよく見ること
*/
//------------------------------------------------------------------
void NijiNetworkError::SetResultErrorCode( const u32 errorCode )
{
  m_errorInfoData.error_type   = ERROR_TYPE_ERROR_CODE;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );
  m_errorInfoData.applet_type  = APPLET_TYPE_CODE;  // エラーコード表示
  m_errorInfoData.applet_arg   = errorCode;
  GFL_PRINT(">NetError SetResultErrorCode %d\n", m_errorInfoData.applet_arg );
}

#if defined(GF_PLATFORM_CTR)
//------------------------------------------------------------------
/**
 * @brief   エラー理由格納(NetZFatal)  ERROR_LEVEL_2
 * @param   nn::nex::qResult
 * @note    NexErrorListenerのOnNexErrorHandling専用
 */
//------------------------------------------------------------------
void NijiNetworkError::SetResultNetZFatal( const nn::nex::qResult & result )
{
  m_errorInfoData.error_type   = ERROR_TYPE_NETZ_FATAL;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );
  m_errorInfoData.applet_type  = APPLET_TYPE_CODE;  // エラーコード表示
  m_errorInfoData.applet_arg   = nn::nex::ErrorCodeConverter::ConvertToNetworkErrorCode( result );
  GFL_PRINT(">NetError SetResultNetZFatal %d\n", m_errorInfoData.applet_arg );
}
#endif // GF_PLATFORM_CTR

#if defined(GF_PLATFORM_CTR)
//------------------------------------------------------------------
/**
* @brief   エラー理由格納(PGL)
* @param   NetLib::Pgl::PGL_COMMON_RESULT
*/
//------------------------------------------------------------------
void NijiNetworkError::SetResultPGL( const NetLib::Pgl::PGL_COMMON_RESULT result )
{
  m_errorInfoData.error_type   = ERROR_TYPE_PGL;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );
  m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;  // メッセージ表示

  // PGL専用のエラーメッセージ
  u32 message = error_pgl_1000;
  switch( result ){
  case NetLib::Pgl::E_RET_ERR_SYSTEM:          //!< システム側で何らかのエラーが発生
    message = error_pgl_1000;
    break;
  case NetLib::Pgl::E_RET_ERR_PARAMETER:       //!< パラメータが不正
    message = error_pgl_1001;
    break;
  case NetLib::Pgl::E_RET_ERR_HTTPMETHOD:      //!< HTTPメソッド(POST/GET)が不正
    message = error_pgl_1002;
    break;
  case NetLib::Pgl::E_RET_ERR_SERVER_FULL:     //!< サーバーの容量オーバー
    message = error_pgl_1003;
    break;
  case NetLib::Pgl::E_RET_ERR_ILLEGAL_TOKEN:   //!< 認証トークンに問題がある
    message = error_pgl_1004;
    break;
  case NetLib::Pgl::E_RET_SAVEDATA_EXISTS:     //!< サーバーに既にセーブデータがある
    message = error_pgl_2000;
    break;
  case NetLib::Pgl::E_RET_SAVEDATA_NO:         //!< サーバにセーブデータがない
    message = error_pgl_2001;
    break;
  case NetLib::Pgl::E_RET_SAVEDATA_RECIVE_LIMIT:///!ゲームシンク受け取り制限中のため、受信不可
    message = error_pgl_2999;
    break;
  case NetLib::Pgl::E_RET_ERR_DATA_ILLEGAL:    //!< アップロードされたセーブデータが破損
    message = error_pgl_3000;
    break;
  case NetLib::Pgl::E_RET_ERR_DATA_CHEAT:      //!< アップロードされたセーブデータが不正
    message = error_pgl_3001;
    break;
  case NetLib::Pgl::E_RET_ERR_BAN:             //!< アカウントBAN
    message = error_ban_01;
    break;
  case NetLib::Pgl::E_RET_ERR_TRAINER_NAME:    //!< トレーナー名不一致
    message = error_pgl_4001;
    break;
  case NetLib::Pgl::E_RET_SERVICE_MAINTENANCE: //!< サービスがメンテナンス中
    message = error_pgl_9000;
    break;
  case NetLib::Pgl::E_RET_SERVICE_END:         //!< サービスが耀了している
    message = error_pgl_9999;
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
  m_errorInfoData.applet_arg   = message;
  GFL_PRINT(">NetError SetResultPGL %d\n", m_errorInfoData.applet_arg );
}
#endif // GF_PLATFORM_CTR

//------------------------------------------------------------------
/**
* @brief   エラー理由格納(PGL用5xxエラー)
* @param   u32   HTTPステータスコード
*/
//------------------------------------------------------------------
void NijiNetworkError::SetResultPGL5xx( const u32 httpStatusCode )
{
  m_errorInfoData.error_type   = ERROR_TYPE_PGL;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );

  // PGL専用のエラーメッセージ
  switch( httpStatusCode ){
  case 500:
    {
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;
      m_errorInfoData.applet_arg   = error_pgl_http500;
    }
    break;
  case 502:
    {
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;
      m_errorInfoData.applet_arg   = error_pgl_http502;
    }
    break;
  case 503:
    {
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;
      m_errorInfoData.applet_arg   = error_pgl_http503;
    }
    break;
  default:
    {
#if defined(GF_PLATFORM_CTR)
      // 共通HTTPステータスコード処理
      SetCommonHttpStatusError( httpStatusCode );
#endif // GF_PLATFORM_CTR
    }
    break;
  }
  GFL_PRINT(">NetError SetResultPGL5xx %d\n", m_errorInfoData.applet_arg );
}

#if defined(GF_PLATFORM_CTR)
//------------------------------------------------------------------
/**
* @brief   エラー理由格納(不思議な贈り物シリアルコード認証エラー)
* @param   NetLib::Pgl::PGL_COMMON_RESULT
*/
//------------------------------------------------------------------
void NijiNetworkError::SetResultSerial( const NetLib::Pgl::PGL_COMMON_RESULT result )
{
  m_errorInfoData.error_type   = ERROR_TYPE_SERIAL;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );
  m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;  // メッセージ表示

  // 不思議専用のエラーメッセージ
  u32 message = error_serial_1000;
  switch( result ){
  case NetLib::Pgl::E_RET_COMMON_OK:                  //!< OK
    return;
  case NetLib::Pgl::E_RET_ERR_SYSTEM:          //!< システム側で何らかのエラーが発生
    message = error_serial_1000;
    break;
  case NetLib::Pgl::E_RET_ERR_PARAMETER:              //!< パラメータが不正
    message = error_serial_1001;
    break;
  case NetLib::Pgl::E_RET_ERR_HTTPMETHOD:             //!< HTTPメソッド(POST/GET)が不正
    message = error_serial_1002;
    break;
  case NetLib::Pgl::E_RET_ERR_SERVER_FULL:            //!< サーバーの容量オーバー
    message = error_serial_1003;
    break;
  case NetLib::Pgl::E_RET_ERR_ILLEGAL_TOKEN:          //!< 認証トークンに問題がある
    message = error_serial_1004;
    break;
  case NetLib::Pgl::E_RET_SAVEDATA_EXISTS:      //!< サーバーに既にセーブデータがある
    message = error_serial_2000;
    break;
  case NetLib::Pgl::E_RET_SAVEDATA_NO:                //!< サーバにセーブデータがない
    message = error_serial_2001;
    break;
  case NetLib::Pgl::E_RET_SAVEDATA_RECIVE_LIMIT:  ///!ゲームシンク受け取り制限中のため、受信不可
    message = error_pgl_2999;
    break;
  case NetLib::Pgl::E_RET_ERR_DATA_ILLEGAL:      //!< アップロードされたセーブデータが破損
    message = error_pgl_3000;
    break;
  case NetLib::Pgl::E_RET_ERR_DATA_CHEAT:             //!< アップロードされたセーブデータが不正
    message = error_pgl_3001;
    break;
  case NetLib::Pgl::E_RET_ERR_BAN:             //!< アカウントBAN
    message = error_ban_01;
    break;
  case NetLib::Pgl::E_RET_ERR_TRAINER_NAME:    //!< トレーナー名不一致
    message = error_pgl_4001;
    break;
  case NetLib::Pgl::E_RET_SERVICE_MAINTENANCE:   //!< サービスがメンテナンス中
    message = error_serial_9000;
    break;
  case NetLib::Pgl::E_RET_SERVICE_END:           //!< サービスが耀了している
    message = error_serial_9999;
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
  m_errorInfoData.applet_arg   = message;
  GFL_PRINT(">NetError SetResultSerial %d\n", m_errorInfoData.applet_arg );
}
#endif // GF_PLATFORM_CTR

//------------------------------------------------------------------
/**
* @brief   エラー理由格納(不思議な贈り物用5xxエラー)
* @param   u32   HTTPステータスコード
*/
//------------------------------------------------------------------
void NijiNetworkError::SetResultSerial5xx( const u32 httpStatusCode )
{
  m_errorInfoData.error_type   = ERROR_TYPE_SERIAL;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );

  // 不思議専用のエラーメッセージ
  switch( httpStatusCode ){
  case 500:
    {
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;
      m_errorInfoData.applet_arg   = error_serial_http500;
    }
    break;
  case 502:
    {
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;
      m_errorInfoData.applet_arg   = error_serial_http502;
    }
    break;
  case 503:
    {
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;
      m_errorInfoData.applet_arg   = error_serial_http503;
    }
    break;
  default:
    {
#if defined(GF_PLATFORM_CTR)
      // 共通HTTPステータスコード処理
      SetCommonHttpStatusError( httpStatusCode );
#endif // GF_PLATFORM_CTR
    }
    break;
  }
  GFL_PRINT(">NetError SetResultSerial5xx %d\n", m_errorInfoData.applet_arg );
}

//------------------------------------------------------------------
/**
* @brief   エラー理由格納(不正チェックサーバ用5xxエラー)
* @param   u32   HTTPステータスコード
*/
//------------------------------------------------------------------
void NijiNetworkError::SetResultValidation5xx( const u32 httpStatusCode )
{
  m_errorInfoData.error_type   = ERROR_TYPE_VALIDATION;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );

  switch( httpStatusCode ){
  case 500: // サーバーの内部エラー
    {
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;  // メッセージ表示「メンテナンス」
      m_errorInfoData.applet_arg = error_validation_http500;
    }
    break;
  case 502:
    {
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;  // メッセージ表示「メンテナンス」
      m_errorInfoData.applet_arg = error_validation_http502;
    }
    break;
  case 503:
    {
      // メンテナンスのメッセージ表示
      m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;  // メッセージ表示「メンテナンス」
      m_errorInfoData.applet_arg = error_validation_http503;
    }
    break;
  default:
    {
#if defined(GF_PLATFORM_CTR)
      // 共通HTTPステータスコード処理
      SetCommonHttpStatusError( httpStatusCode );
#endif // GF_PLATFORM_CTR
    }
    break;
  }
  GFL_PRINT(">NetError SetResultValidation5xx %d\n", httpStatusCode );
}



#if defined(GF_PLATFORM_CTR)
//------------------------------------------------------------------
/**
  * @brief   Bossエラー
  * @param   result
  */
//------------------------------------------------------------------
void NijiNetworkError::SetResultBossError( const nn::Result& result )
{
  m_errorInfoData.error_type   = ERROR_TYPE_BOSS;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );
  m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;  // メッセージ表示

  u32 message = error_boss_03;

  if( result == nn::Result() || result.IsSuccess() )
  {//ファイルが無い
    message = error_boss_02;
  }
  else if( result <= nn::fs::ResultAlreadyExists() )
  {//その他のエラー
    message = error_boss_02;
  }
  else if( result <= nn::fs::ResultNotEnoughSpace() )
  {//容量不足
    message = error_boss_01;
  }
  else
  {//その他のエラー
    message = error_boss_03;
  }

  m_errorInfoData.applet_arg   = message;

}

//------------------------------------------------------------------
/**
  * @brief   BossTaskエラー
  * @param   errorCode
  */
//------------------------------------------------------------------
void NijiNetworkError::SetResultBossTaskError( u32 errorCode )
{
  m_errorInfoData.error_type   = ERROR_TYPE_BOSS;
  m_errorInfoData.error_level  = GetErrorLevelFromType( m_errorInfoData.error_type );

  if( errorCode != 0 )
  {
    nn::boss::TaskResultCode targetResult = (nn::boss::TaskResultCode)errorCode;

    { 
      nn::nex::qUnsignedInt err = 0; 

      nn::Result ret = nn::boss::GetErrorCode( &err, targetResult); 
      
      if(!ret.IsSuccess())
      { 
        GFL_ASSERT_STOP(0);   //ここは開発段階でつぶされるはず ここに来た原因があるはず  fix 
      } 

      m_errorInfoData.applet_type  = APPLET_TYPE_CODE;  // エラーコード表示_
      m_errorInfoData.applet_arg   = err;
    } 
  }
  else
  {
    m_errorInfoData.applet_type  = APPLET_TYPE_MESSAGE;  // メッセージ表示
    m_errorInfoData.applet_arg   = error_boss_03;
  }


}

#endif // GF_PLATFORM_CTR


//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   エラー種類からエラーレベルを取得
 * @param   ENUM_ERROR_TYPE
 * @return  ENUM_ERROR_LEVEL エラーレベル
 */
//------------------------------------------------------------------
NijiNetworkError::ENUM_ERROR_LEVEL NijiNetworkError::GetErrorLevelFromType( const ENUM_ERROR_TYPE errorType )
{
  if( m_errorInfoData.error_level == ERROR_LEVEL_0 ) { GFL_ASSERT_MSG(0, ">NetError SetNotError呼び出し順番違う\n"); }

  switch( errorType ){
  case ERROR_TYPE_TIMEOUT:           ///< タイムアウト
  case ERROR_TYPE_IR_BROKEN:         ///< 赤外線壊れている
  case ERROR_TYPE_IR_MISSMATCH:      ///< 赤外線繋いだタイプが違う
  case ERROR_TYPE_NEX:               ///< Nexのエラー
  case ERROR_TYPE_ERROR_CODE:        ///< エラーコード直接代入
  case ERROR_TYPE_PGL:               ///< PGLサーバエラー
  case ERROR_TYPE_SERIAL:            ///< 不思議な贈り物シリアルサーバエラー
  case ERROR_TYPE_VALIDATION:        ///< 不正チェックサーバエラー
  case ERROR_TYPE_BOSS:              ///< BOSS
    GFL_PRINT(">NetError ERROR_LEVEL_1に設定\n");
    return ERROR_LEVEL_1;

  case ERROR_TYPE_SYSTEM:            ///< システムエラー（ライブラリなどで失敗が返った場合など）
  case ERROR_TYPE_SESSION_LEFT:      ///< P2P切断
  case ERROR_TYPE_NETZ_FATAL:        ///< NetZのFatal
    GFL_PRINT(">NetError ERROR_LEVEL_2に設定\n");
    return ERROR_LEVEL_2;

  case ERROR_TYPE_MEMORY_FULL:       ///< メモリ不足
  case ERROR_TYPE_LOGINSERVER_DISCONNECT:///< ゲームサーバーとの接続が切れた
  case ERROR_TYPE_WIFI_DISCONNECT:   ///< WiFi切断
    GFL_PRINT(">NetError ERROR_LEVEL_3に設定\n");
    return ERROR_LEVEL_3;

  case ERROR_TYPE_FATAL:             ///< FatalError
    GFL_PRINT(">NetError ERROR_LEVEL_4に設定\n");
    return ERROR_LEVEL_4;

  default:
    GFL_ASSERT(0);
    return ERROR_LEVEL_1;
  }
}

//------------------------------------------------------------------
/**
 * @brief   エラー種類のエラーメッセージを取得
 * @param   ENUM_ERROR_TYPE
 * @return  u32 エラーメッセージ番号
 */
//------------------------------------------------------------------
u32 NijiNetworkError::GetErrorMessageFromType( const ENUM_ERROR_TYPE errorType )
{
  u32 message = error_comm_02;

  switch( errorType ){
  case ERROR_TYPE_NONE:          ///< なし
    GFL_ASSERT(0);
    break;
  case ERROR_TYPE_SYSTEM:            ///< システムエラー（ライブラリなどで失敗が返った場合など）
    message = error_comm_05;
    break;
  case ERROR_TYPE_MEMORY_FULL:       ///< メモリ不足
    message = error_comm_02;
    break;
  case ERROR_TYPE_TIMEOUT:           ///< タイムアウト
    message = error_comm_06;
    break;
  case ERROR_TYPE_SESSION_LEFT:      ///< P2P切断
    message = error_comm_03;
    break;
  case ERROR_TYPE_LOGINSERVER_DISCONNECT:///< ゲームサーバーとの接続が切れた
    message = error_comm_04;
    break;
  case ERROR_TYPE_IR_BROKEN:         ///< 赤外線壊れている
    message = error_comm_08;
    break;
  case ERROR_TYPE_IR_MISSMATCH:      ///< 赤外線繋いだタイプが違う
    message = error_comm_10;
    break;

  // 下記は専用メッセージがある
  //case ERROR_TYPE_PGL:               ///< PGLサーバエラー
  //case ERROR_TYPE_SERIAL:            ///< 不思議な贈り物シリアルサーバエラー
  //case ERROR_TYPE_VALIDATION:        ///< 不正チェックサーバエラー

  // 下記はエラーコード表示なのでメッセージ必要ない
  //case ERROR_TYPE_WIFI_DISCONNECT:   ///< WiFi切断
  //case ERROR_TYPE_NEX:               ///< Nexのエラー
  //case ERROR_TYPE_ERROR_CODE:        ///< エラーコード直接代入
  //case ERROR_TYPE_NETZ_FATAL:        ///< NetZのFatal
  }
  GFL_PRINT(">NetError Message=%dに設定\n", message );
  return message;
}

#if defined(GF_PLATFORM_CTR)
//------------------------------------------------------------------
/**
 * @brief   エラー理由格納(汎用HTTPステータスコード)  ERROR_LEVEL_1
 * @param   u32   HTTPステータスコード
 */
//------------------------------------------------------------------
void NijiNetworkError::SetCommonHttpStatusError( const u32 httpStatusCode )
{
  switch( httpStatusCode ){
  case 401: // IncorrectRemoteAuthentication	接続先のステーションが正しく認証しませんでした。
    {
      nn::nex::qResult result = QRESULT_ERROR(Transport, IncorrectRemoteAuthentication);
      m_errorInfoData.applet_type  = APPLET_TYPE_CODE;  // エラーコード表示
      m_errorInfoData.applet_arg   = nn::nex::ErrorCodeConverter::ConvertToNetworkErrorCode( result );
    }
    break;
  case 404: // NotFound	要求されたリソースが見つかりません。
    {
      nn::nex::qResult result = QRESULT_ERROR(Transport, NotFound);
      m_errorInfoData.applet_type  = APPLET_TYPE_CODE;  // エラーコード表示
      m_errorInfoData.applet_arg   = nn::nex::ErrorCodeConverter::ConvertToNetworkErrorCode( result );
    }
    break;
  default:
    {
      nn::nex::qResult result = QRESULT_ERROR(Transport, Unknown);
      m_errorInfoData.applet_type  = APPLET_TYPE_CODE;  // エラーコード表示
      m_errorInfoData.applet_arg   = nn::nex::ErrorCodeConverter::ConvertToNetworkErrorCode( result );
    }
    break;
  }
}
#endif // GF_PLATFORM_CTR


GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

