//=============================================================================
/**
 * @file   PGLConnectorListener.h
 *
 * @brief  ポケモン不正チェック用イベントリスナ
 *
 * @author suzuki_hisanao
 */
//=============================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef __PG_CONNECTOR_LISTENER_H__
#define __PG_CONNECTOR_LISTENER_H__

#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <NetStatic/NetLib/include/PGL/PGLDefine.h>


namespace NetLib {
namespace Pgl {


/**
 * @brief PGL通信の終了通知用イベントリスナクラス
 */
class PGLConnectorListener
{
public:
  //------------------------------------------------------------------
  /**
  * @brief セーブデータ登録通信の完了イベントを検知
  */
  //------------------------------------------------------------------
  virtual void OnPGLCreateSavedataSuccess( void ){}

  //------------------------------------------------------------------
  /**
  * @brief セーブデータ同期通信の完了イベントを検知
  *
  * @param[in]  isSignIn  連携サイトへの登録有無（true:登録済み）
  * @param[in]  itemCount 持ち帰ったアイテム数
  * @param[in]  itemInf   持ち帰ったアイテム情報配列(itemCountが0の時はNULL)
  */
  //------------------------------------------------------------------
  virtual void OnPGLSyncSaveDataSuccess( bool isSignIn, u8 itemCount, NetLib::Pgl::ResultItemInfo* itemInf ){}

  //------------------------------------------------------------------
  /**
  * @brief セーブデータ同期確定通信の完了イベントを検知
  */
  //------------------------------------------------------------------
  virtual void OnPGLCommitGameSyncSuccess( void ){}

  //------------------------------------------------------------------
  /**
  * @brief プレイステータス確認通信の完了イベントを検知
  *
  * @param[in]  isSignIn  連携サイトへの登録有無（true:登録済み）
  */
  //------------------------------------------------------------------
  virtual void OnPGLGetPlayStatusSuccess( bool isSignIn ){}

  //------------------------------------------------------------------
  /**
  * @brief PGLバトルエントリー確認通信の完了イベントを検知
  *
  * @param[in]  wifiMatchUpID     大会ID
  * @param[in]  gpfEntryFlg       PGLエントリーワーク（更新通信に必要）
  * @param[in]  wifiMatchupState  大会の進行情報（更新通信に必要）
  * @param[in]  isSignIn          連携サイトへの登録有無（true:登録済み）
  * @param[in]  timeZone          タイムゾーンオフセット
  */
  //------------------------------------------------------------------
  virtual void OnPGLGetWorldBattleStatusSuccess( u32 wifiMatchUpID, u16 gpfEntryFlg, u16 wifiMatchupState, bool isSignIn, s8 timezone ){}

  //------------------------------------------------------------------
  /**
  * @brief PGLバトルエントリー状態更新通信の完了イベントを検知
  */
  //------------------------------------------------------------------
  virtual void OnPGLUpdateWorldBattleStatusSuccess( void ){}

  //------------------------------------------------------------------
  /**
  * @brief PGLバトルボックス登録通信の完了イベントを検知
  */
  //------------------------------------------------------------------
  virtual void OnPGLUploadBattleBoxSuccess( void ){}

  //------------------------------------------------------------------
  /**
  * @brief PGLバトルボックス登録通信の完了イベントを検知
  *
  * @param[in]  boxData   ボックスデータ
  * @param[in]  dataSize  ボックスデータサイズ
  */
  //------------------------------------------------------------------
  virtual void OnPGLDownloadBattleBoxSuccess( const u8* boxData, u32 dataSize ){}

  //------------------------------------------------------------------
  /**
  * @brief PGL不思議な贈り物シリアルコード認証通信の完了イベントを検知
  *
  * @param[in]  serialstatus  シリアルコード利用状況ステータス
  * @param[in]  datano        BOSSサーバーに登録されているデータNO
  */
  //------------------------------------------------------------------
  virtual void OnPGLAuthSerialSuccess( NetLib::Pgl::PGL_SERIAL_AUTH_RESULT serialstatus, u32 datano ){}

  //------------------------------------------------------------------
  /**
  * @brief PGL不思議な贈り物シリアルコード状態更新通信の完了イベントを検知
  *
  * @param[in]  serialstatus  シリアルコード利用状況ステータス
  */
  //------------------------------------------------------------------
  virtual void OnPGLUpdateSerialStatusSuccess( NetLib::Pgl::PGL_SERIAL_AUTH_RESULT serialstatus ){}

  //------------------------------------------------------------------
  /**
  * @brief PGLセーブデータダウンロードの完了イベントを検知
  *
  * @param[in]  saveData  セーブデータ
  * @param[in]  saveSize  セーブデータサイズ
  * @param[in]  type      セーブデータタイプ
  */
  //------------------------------------------------------------------
  virtual void OnPGLDownloadSavedataSuccess( const u8* saveData, u32 saveSize, NetLib::Pgl::PGL_SAVEDATA_DOWNLOAD_TYPE type ){}

public:
  //------------------------------------------------------------------
  /**
  * @brief PGL通信のエラーイベントを検知
  *
  * @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
  *
  * @param[in] commandType 完了したPGL通信コマンド
  * @param[in] httpStatus サーバから返却されたHTTPステータスコード
  */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief PGL通信のレスポンスデータのエラーコードを検知
  *
  * @param[in]  commandType  完了したPGL通信コマンド
  * @param[in]  resultCode   PGLサーバから受信したレスポンスエラーコード
  */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief PGL通信のエラーイベントを検知
  *
  * @param[in]  commandType  完了したPGL通信コマンド
  * @param[in]  result       エラー内容
  * @param[in]  errorCode    ネットワークエラーコード
  */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief PGL通信のキャンセルイベントを検知
  *
  * @param[in] commandType キャンセルされたPGL通信コマンド
  */
  //------------------------------------------------------------------
  virtual void OnPGLConnectCanceled( const NetLib::Pgl::PGL_COMMAND_TYPE commandType ){};

};


}	// pgl
}	// netapp


#endif // __PG_CONNECTOR_LISTENER_H__
#endif