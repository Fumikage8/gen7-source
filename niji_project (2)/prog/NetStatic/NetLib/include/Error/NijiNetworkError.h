//==============================================================================
/**
 * @file	NijiNetworkError.h
 * @brief	Niji通信エラー情報クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 16:35:27
 */
// =============================================================================
#ifndef __NIJI_NETWORK_ERROR_H__
#define __NIJI_NETWORK_ERROR_H__
#pragma once

#if defined(GF_PLATFORM_CTR)
#include <nex.h>
#endif // GF_PLATFORM_CTR

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include "NetStatic/NetLib/include/PGL/PGLDefine.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   Niji通信エラー情報クラス
 */
//------------------------------------------------------------------
class NijiNetworkError
{
  GFL_FORBID_COPY_AND_ASSIGN(NijiNetworkError);

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
public:
  //-------------------------------------
  /// エラーレベル（高いほど危険）
  // 　　エラーレベルが高いものにエラーは上書きされる！
  //=====================================
  typedef enum{
    ERROR_LEVEL_0 = 0,    ///< エラーレベル0（内部的にはエラーにしない。アプレット表示したいだけの時）
    // 
    // 以下は、通信エラーとなる
    //
    ERROR_LEVEL_1,        ///< エラーレベル1（アプリを終了しなければいけないAPIエラー）
    ERROR_LEVEL_2,        ///< エラーレベル2（P2P切断)
    //
    // 以下は、再初期化が必要なエラーとなる
    //
    ERROR_LEVEL_3,        ///< エラーレベル3（WiFi切断)
    //
    // 以下は、処理を進める事が出来ないエラーとなる
    //
    ERROR_LEVEL_4,        ///< エラーレベル4（FatalError)
  } ENUM_ERROR_LEVEL;

  //-------------------------------------
  /// エラー種類
  //=====================================
  typedef enum{
    ERROR_TYPE_NONE = 0,          ///< なし
    //
    ERROR_TYPE_SYSTEM,            ///< システムエラー（ライブラリなどで失敗が返った場合など）
    ERROR_TYPE_MEMORY_FULL,       ///< メモリ不足
    ERROR_TYPE_TIMEOUT,           ///< タイムアウト
    ERROR_TYPE_SESSION_LEFT,      ///< P2P切断
    ERROR_TYPE_LOGINSERVER_DISCONNECT,///< ゲームサーバーとの接続が切れた
    ERROR_TYPE_IR_BROKEN,         ///< 赤外線壊れている
    ERROR_TYPE_IR_MISSMATCH,      ///< 赤外線繋いだタイプが違う
    ERROR_TYPE_FATAL,             ///< FatalError
    //
    // 以下追加でパラメータが必要なもの
    // 
    ERROR_TYPE_WIFI_DISCONNECT,   ///< WiFi切断
    ERROR_TYPE_NEX,               ///< Nexのエラー
    ERROR_TYPE_ERROR_CODE,        ///< エラーコード直接代入
    ERROR_TYPE_NETZ_FATAL,        ///< NetZのFatal
    ERROR_TYPE_PGL,               ///< PGLサーバエラー
    ERROR_TYPE_SERIAL,            ///< 不思議な贈り物シリアルサーバエラー
    ERROR_TYPE_VALIDATION,        ///< 不正チェックサーバエラー
    ERROR_TYPE_BOSS,              ///< BOSS
    //
    ERROR_TYPE_MAX,
  } ENUM_ERROR_TYPE;

  //-------------------------------------
  /// エラーアプレット種類
  //=====================================
  typedef enum{
    APPLET_TYPE_CODE,       ///< エラーコード表示_
    APPLET_TYPE_MESSAGE,    ///< エラーメッセージ表示
  } ENUM_APPLET_TYPE;

  //-------------------------------------
  /// エラー情報データ
  //=====================================
  typedef struct{ 
    ENUM_ERROR_LEVEL  error_level;   ///< エラーレベル
    ENUM_ERROR_TYPE   error_type;    ///< エラー種類
    ENUM_APPLET_TYPE  applet_type;   ///< アプレット種類
    u32               applet_arg;    ///< アプレットに渡す情報
  } ERROR_INFO_DATA;

public:
  // コンストラクタ
  NijiNetworkError();
  // デストラクタ
  virtual ~NijiNetworkError();

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
   *          ※エラー理由を格納した後にコールする事
   */
  //------------------------------------------------------------------
  void SetNotError();

  //------------------------------------------------------------------
  /**
   * @brief   FatalError扱いにする
   * 
   * @note    エラーアプレットは表示しません。
   *          これ以降、通信エラーは無視します。
   */
  //------------------------------------------------------------------
  void SetFatalError();

  // 
  // 以下、エラー理由格納メソッド
  //
  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納（直接エラー種類指定）
   * @param   ENUM_ERROR_TYPE
   */
  //------------------------------------------------------------------
  void SetResult( const ENUM_ERROR_TYPE result );

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
  void SetResultWifiDisconnect( const u32 errorCode );

#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納(Nex)  ERROR_LEVEL_1
   * @param   nn::nex::qResult
   */
  //------------------------------------------------------------------
  void SetResultNex( const nn::nex::qResult & result );
#endif // GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納  ERROR_LEVEL_1
   * @param   u32     U32のエラーコードが変換済みのもののみ代入
   *
   * @caution むやみに数値を入れる事が出来ますが、
   *            きちんと把握していないと不具合に繋がります。
   *            使用前にライブラリをよく見ること
   */
  //------------------------------------------------------------------
  void SetResultErrorCode( const u32 errorCode );

#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納(NetZFatal)  ERROR_LEVEL_2
   * @param   nn::nex::qResult
   * @note    NexErrorListenerのOnNexErrorHandling専用
   */
  //------------------------------------------------------------------
  void SetResultNetZFatal( const nn::nex::qResult & result );
#endif // GF_PLATFORM_CTR

#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納(PGL)  ERROR_LEVEL_1
   * @param   NetLib::Pgl::PGL_COMMON_RESULT
   */
  //------------------------------------------------------------------
  void SetResultPGL( const NetLib::Pgl::PGL_COMMON_RESULT result );
#endif // GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納(PGL用5xxエラー)  ERROR_LEVEL_1
   * @param   u32   HTTPステータスコード
   */
  //------------------------------------------------------------------
  void SetResultPGL5xx( const u32 httpStatusCode );
  
#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納(不思議な贈り物シリアルコード認証エラー)  ERROR_LEVEL_1
   * @param   NetLib::Pgl::PGL_COMMON_RESULT
   */
  //------------------------------------------------------------------
  void SetResultSerial( const NetLib::Pgl::PGL_COMMON_RESULT result );
#endif // GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納(不思議な贈り物用5xxエラー)  ERROR_LEVEL_1
   * @param   u32   HTTPステータスコード
   */
  //------------------------------------------------------------------
  void SetResultSerial5xx( const u32 httpStatusCode );
  
  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納(不正チェックサーバ用5xxエラー)  ERROR_LEVEL_1
   * @param   u32   HTTPステータスコード
   */
  //------------------------------------------------------------------
  void SetResultValidation5xx( const u32 httpStatusCode );
  
#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
   * @brief   Bossエラー
   * @param   result
   */
  //------------------------------------------------------------------
  void SetResultBossError( const nn::Result& result );

  //------------------------------------------------------------------
  /**
   * @brief   BossTaskエラー
   * @param   errorCode
   */
  //------------------------------------------------------------------
  void SetResultBossTaskError( u32 errorCode );

#endif // GF_PLATFORM_CTR


  // ゲッター
  inline ERROR_INFO_DATA  GetErrorInfoData() const { return m_errorInfoData; };
  
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:
  //------------------------------------------------------------------
  /**
   * @brief   エラー種類からエラーレベルを取得
   * @param   ENUM_ERROR_TYPE
   * @return  ENUM_ERROR_LEVEL エラーレベル
   */
  //------------------------------------------------------------------
  ENUM_ERROR_LEVEL GetErrorLevelFromType( const ENUM_ERROR_TYPE errorType );
  //------------------------------------------------------------------
  /**
   * @brief   エラー種類からエラーメッセージを取得
   * @param   ENUM_ERROR_TYPE
   * @return  u32 エラーメッセージ番号
   */
  //------------------------------------------------------------------
  u32 GetErrorMessageFromType( const ENUM_ERROR_TYPE errorType );

#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
   * @brief   エラー理由格納(汎用HTTPステータスコード)  ERROR_LEVEL_1
   * @param   u32   HTTPステータスコード
   */
  //------------------------------------------------------------------
  void SetCommonHttpStatusError( const u32 httpStatusCode );
#endif // GF_PLATFORM_CTR

private:
  ERROR_INFO_DATA     m_errorInfoData;    ///< エラー情報データ

};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_ERROR_H__
