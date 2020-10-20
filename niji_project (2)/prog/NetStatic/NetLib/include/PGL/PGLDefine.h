//=============================================================================
/**
 * @file   PGLDefine.h
 *
 * @brief  PGL通信定義
 *
 * @author suzuki_hisanao
 */
//=============================================================================

#if defined(GF_PLATFORM_CTR)
#ifndef __PG_DEFINE_H__
#define __PG_DEFINE_H__

#include <gfx/include/gfl2_types.h>
#include <system/include/Date/gfl2_Date.h>
namespace NetLib {
namespace Pgl {

  /**
   * @brief PGL通信コマンドタイプ
   */
  typedef enum
  {
    E_CMD_NONE,                     //!< コマンド未定義
    //
    E_CMD_SAVEDATA_CREATE,          //!< セーブデータ生成
    E_CMD_SAVEDATA_SYNC,            //!< セーブデータ同期（BOXなし）
    E_CMD_SAVEDATA_FULL_SYNC,       //!< セーブデータ同期（すべて）
    E_CMD_SAVEDATA_FINISH,          //!< セーブデータ同期完了
    //
    E_CMD_SAVEDATA_DOWNLOAD,        //!< セーブデータダウンロード
    //
    E_CMD_ACCOUNT_GETPLAYSTATUS,    //!< プレイステータス確認
    //
    E_CMD_WORLDBATTLE_GETSTATUS,    //!< バトルエントリー確認
    E_CMD_WORLDBATTLE_UPDATESTATUS, //!< バトル参加状態更新
    //
    E_CMD_BATTLEBOX_UPLOAD,         //!< バトルボックス登録
    E_CMD_BATTLEBOX_DOWNLOAD,       //!< バトルボックス取得
    //
    E_CMD_SERIAL_AUTH,              //!< ふしぎなおくりものシリアル認証
    E_CMD_SERIAL_UPDATE_STATUS,     //!< ふしぎなおくりもの状態更新
  }PGL_COMMAND_TYPE;

  /**
   * @brief プレイステータス確認の目的
   */
  typedef enum
  {
    E_PURPOSE_GAMESYNC,            //!< ゲームシンク
    E_PURPOSE_RATING_BATTLE,       //!< レーティングバトル
    E_PURPOSE_INTERNET_CUP,        //!< インターネット大会 or なかま大会
  }PGL_PURPOSE_TYPE;


  /**
   * @brief ダウンロードセーブデータのタイプ
   */
  typedef enum
  {
    E_DL_TYPE_NO_BOX_SAVEDATA,      //!< ボックスなし
    E_DL_TYPE_FULL_SAVEDATA,        //!< ボックスあり
    E_DL_TYPE_LATEST_SAVEDATA,      //!< 最新版
  }PGL_SAVEDATA_DOWNLOAD_TYPE;


  /**
   * @brief PGL不思議な贈り物シリアルコード認証レスポンスステータス
   */
  typedef enum
  {
    E_RET_SERIAL_OK = 0,              //!< シリアルコードの認証成功
    E_RET_SERIAL_NG = 10,             //!< シリアルコードが無効
    E_RET_SERIAL_MISMATCH_ROM,        //!< 該当のキャンペーンで受け取れるROMと不一致
    E_RET_SERIAL_MISMATCH_LANG,       //!< 該当のキャンペーンで受け取れる言語コードと不一致
    E_RET_SERIAL_MISMATCH_AREA,       //!< 該当のキャンペーンで受け取れる国地域と不一致
    E_RET_SERIAL_MISMATCH_REGION,     //!< 該当のキャンペーンで受け取れるリージョンと不一致
    E_RET_SERIAL_ALREADY_DOWNLOADED,  //!< 既にキャンペーンアイテム取得済み
    E_RET_SERIAL_CAMPAIGN_DOWNLOADED, //!< 同一キャンペーンで既にアイテム取得済み
    E_RET_SERIAL_LOCKED_OTHER,        //!< 他ユーザーがロック中（該当のゲームシンク以外のユーザーがロックした場合）
    E_RET_SERIAL_CAMPAIGN_NOT_STARTED,//!< キャンペーン開始前
    E_RET_SERIAL_CAMPAIGN_END,        //!< キャンペーン終了
  }PGL_SERIAL_AUTH_RESULT;


  /**
   * @brief PGL共通レスポンスコード
   */
  typedef enum
  {
    E_RET_COMMON_OK,                  //!< OK
    E_RET_ERR_SYSTEM = 1000,          //!< システム側で何らかのエラーが発生
    E_RET_ERR_PARAMETER,              //!< パラメータが不正
    E_RET_ERR_HTTPMETHOD,             //!< HTTPメソッド(POST/GET)が不正
    E_RET_ERR_SERVER_FULL,            //!< サーバーの容量オーバー
    E_RET_ERR_ILLEGAL_TOKEN,          //!< 認証トークンに問題がある
    E_RET_SAVEDATA_EXISTS=2000,       //!< サーバーに既にセーブデータがある
    E_RET_SAVEDATA_NO,                //!< サーバにセーブデータがない
    E_RET_SAVEDATA_RECIVE_LIMIT=2999, //!< ゲームシンク受け取り制限中のため、受信不可
    E_RET_ERR_DATA_ILLEGAL=3000,      //!< アップロードされたセーブデータが破損
    E_RET_ERR_DATA_CHEAT,             //!< アップロードされたセーブデータが不正
    E_RET_ERR_BAN=4000,               //!< アカウントBAN
    E_RET_ERR_TRAINER_NAME,           //!< トレーナー名不一致
    E_RET_SERVICE_MAINTENANCE=9000,   //!< サービスがメンテナンス中
    E_RET_SERVICE_END =9999           //!< サービスが終了している
  }PGL_COMMON_RESULT;


  /**
   * @brief PGLから持ち帰ったアイテム情報
   */
  typedef struct
  {
    u16 itemId;                       //!< アイテムID
    u16 itemNum;                      //!< アイテム数
    u16 crc;                          //!< CRC値
    u16 padding;                      //!< padding
  }ResultItemInfo;

  /**
   * @brief グローバルアトラクション結果
   */
  typedef struct
  {
    u32 gacount;                       //!< トータルスコア
    u8 gastate;                      //!< 0：未開催,1：途中経過,2：最終結果
    u8 padding[3];                      //!< padding
  }GlobalAttractionInfo;

  /**
   * @brief PGL共通レスポンスデータ
   */
  typedef struct
  {
    u32  responseCode;                //!<レスポンスコード
    u32  bodySize;                    //!<レスポンスデータサイズ
    u32  startSec;                    //!<サーバー処理時間
    u16  descCode;                    //!<詳細エラーコード
    char descMessage[80];             //!<詳細エラーメッセージ
    char reserved1[8];                //!<予備
    char reserved2[25];               //!<予備
    u8   ext_info;                    //!<位置確認用
  }ResultCommonInfo;

  /**
   * @brief 3Dカメラ写真データ送信用情報
   */
  struct GameSyncCameraInfo
  {
    const char*               m_base64Picture;      //!< BASE64エンコードされたカメラ写真データ
    u32                       m_pictureBinarySize;  //!< カメラ写真データのバイナリ時のサイズ
    const gfl2::system::Date* m_pictureDate;        //!< カメラ写真の撮影日

    /**
     * @brief 初期化関数
     *
     * @param[in] base64Picture      BASE64エンコードされた写真データへのポインタ
     * @param[in] pictureBinarySize  BASE64エンコード前の写真データサイズ
     * @param[in] pictureDate        写真撮影時間
     */
    void Initialize( const void* base64Picture, u32 pictureBinarySize )//, const gfl2::system::Date* pictureDate )
    {
      m_base64Picture     = reinterpret_cast<const char*>(base64Picture);
      m_pictureBinarySize = pictureBinarySize;
      //m_pictureDate       = pictureDate;
    }

    /**
     * @brief コンストラクタ
     */
    GameSyncCameraInfo( void )
    {
      Initialize( NULL, 0 );
    }

    /**
     * @brief コンストラクタ
     *
     * @param[in] base64Picture      BASE64エンコードされた写真データへのポインタ
     * @param[in] pictureBinarySize  BASE64エンコード前の写真データサイズ
     * @param[in] pictureDate        写真撮影時間
     */
    GameSyncCameraInfo( const void* base64Picture, u32 pictureBinarySize )//, const gfl2::system::Date* pictureDate )
    {
      Initialize( base64Picture, pictureBinarySize );
    }
  };


}	// Pgl
}	// NetLib


#endif // __PG_DEFINE_H__
#endif // GF_PLATFORM_CTR
