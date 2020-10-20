#ifndef __NET_DEFINE_H__
#define __NET_DEFINE_H__
//=============================================================================
/**
 * @file   net_define.h
 * @brief   通信関係のバージョン等の定義を書くためのヘッダー
 * @date   
 */
//=============================================================================

#include <System/include/PokemonVersion.h>


/*
SUN
製品コード          ：CTR-P-BNDA(---)
追加コンテンツコード：---
OLVアクセスキー     ：1923409225
UniqueID            ：0x01648
*/
/*
----------
仮称      ：momiji - s
製品コード：CTR - P - A2AA(-- - )
UniqueID  ：0x01B50
----------
仮称      ：momiji - m
製品コード：CTR - P - A2BA(-- - )
UniqueID  ：0x01B51
----------
*/
//-----------------------------------------------------------------------------
//任天堂から割り当てられる製品毎の通信番号
//-----------------------------------------------------------------------------
/*
  SUN/MOON共通であるべき部分で使用する製品固有ID
  代表値としてSUNのIDを使用する
  ※各種通信など
*/
#define NIJI_PRODUCT_CODE  (0x01648)


/*
  SUN/MOONで別々の値となるべき部分で使用するための製品固有ID
  ※eshopパッチページへのジャンプなど
*/
#if PM_VERSION==VERSION_SUN2
#define NIJI_PRODUCT_CODE_INTRINSIC   (0x01B50)
#else
#define NIJI_PRODUCT_CODE_INTRINSIC   (0x01B51)
#endif

//-----------------------------------------------------------------------------
// 独自サーバ通信用アクセストークン取得情報
//-----------------------------------------------------------------------------
// アクセスキー
#define INDEPENDENT_HASH      ("c3f5b22e")

//-----------------------------------------------------------------------------
// NEXサーバ用ＩＤ(for Nex3.0)
//-----------------------------------------------------------------------------
// ゲームＩＤ
#define NEX_GAME_ID      0x00164800
// アクセスキー
#define NEX_ACCESS_KEY _T("086f9d28")



//-----------------------------------------------------------------------------
// NEXに割り当てるヒープサイズ
//-----------------------------------------------------------------------------
#define  FOR_NEX_LIB_HEAP_SIZE       (1024*900)   //!< NEXに割り当てるヒープサイズ
#define  FOR_NEX_LIB_SPARE_HEAP_SIZE (1024*64)   //!< 上記サイズのうち、緊急用に割り当てるヒープサイズ(128KB)


//-----------------------------------------------------------------------------
// PGLに割り当てるヒープサイズ
//-----------------------------------------------------------------------------
#define GAMESYNC_BUFFSIZE  (4096*160)

//-----------------------------------------------------------------------------
// すれ違い通信ID生成用ユニークID（BSF の CecUniqueIDと一致させる）
//-----------------------------------------------------------------------------
#define NIJI_CEC_UNIQUE_ID 0x00164800

//-----------------------------------------------------------------------------
// BOSSサーバ関連の定義
//-----------------------------------------------------------------------------
#define NIJI_BOSS_STORAGE_SIZE 1024*1024 //拡張セーブデータ領域内でBOSSストレージとして利用するサイズ。
#define NIJI_BOSS_ENTRY_COUNT 1 //BOSS ストレージ内に格納できる最大の NS データ数

#if PM_VERSION==VERSION_SUN2
#define NIJI_BOSS_CODE "fnCAH3KrGIl9dgSd"//BOSSコードSUN2用  [A2AA] momiji-s
#else//#elif (PM_VERSION==VERSION_MOON)
#define NIJI_BOSS_CODE "b3Gq6LF6EqE1bvKy"//BOSSコードMOON2用 [A2BA] momiji-m 
#endif




//-----------------------------------------------------------------------------
/// 通信バージョン番号  ROMパッチを当てた時に通信が変更になる可能性があるため
/// v1.0 =  0x10  v1.1 = 0x11
/// 上位バージョンが下位バージョンの対応をする時に使う事
//-----------------------------------------------------------------------------
#define NIJI_NET_VERSION (0x01)

//-----------------------------------------------------------------------------
//デバッグ用 通信混線防止処理
//-----------------------------------------------------------------------------
#if defined(DEBUG_ONLY_FOR_kousaka_shin)
#define DEBUG_GAMENO_MATCH (0)
#elif defined(DEBUG_ONLY_FOR_shimoyamada)
#define DEBUG_GAMENO_MATCH (0)
#elif defined(DEBUG_ONLY_FOR_ohta_tetsuji)
#define DEBUG_GAMENO_MATCH (0)
#elif defined(DEBUG_ONLY_FOR_ichiraku_katsuhiko)
#define DEBUG_GAMENO_MATCH (0)
#else
#define DEBUG_GAMENO_MATCH (0)
#endif


//-----------------------------------------------------------------------------
//ビーコンマネージャやビーコンクラスがビーコンを識別する為のID
//-----------------------------------------------------------------------------
enum NIJI_BEACON_TYPE_ID
{
  NIJI_BEACON_TYPE_ID_JF = 10,//システムが若い番号を使用しているので10から
  NIJI_BEACON_TYPE_ID_DELIVERY,//配信用
  NIJI_BEACON_TYPE_ID_LIVEREC,//ライブ大会観戦
};

/*


/// 赤外線通信用同期コード(0〜65535)
typedef enum
{
  E_IR_SYNC_PSS_BATTLE_START = 47361,        //!< PSS赤外線マッチング(開始）
  E_IR_SYNC_PSS_BATTLE_END   = 47363,        //!< PSS赤外線マッチング(終了）
  E_IR_SYNC_PSS_TRADE_START  = 26361,        //!< PSS赤外線マッチング(開始）
  E_IR_SYNC_PSS_TRADE_END    = 26363,        //!< PSS赤外線マッチング(終了）
}E_IR_SYNC_CODE;

//-------------------------------------
///  WiFiマッチング用の最大接続人数定義
//=====================================
typedef enum{
  MATCHMAKE_JOIN_DEFAULT  = 2,      ///< 最大2人接続
  MATCHMAKE_JOIN_MULTI    = 4,      ///< 最大4人接続
}ENUM_MATCHMAKE_JOIN_NUM;

//-------------------------------------
///  アクセスポイントのスキャン間隔(ms)
//=====================================
#define XY_NET_AP_SCAN_INTERVAL 1000 * 60
//#define XY_NET_AP_SCAN_INTERVAL 1000 * 60 * 3

*/

//-------------------------------------
///  DataStore データタイプ
//=====================================
typedef enum
{
  DATASTORE_DATATYPE_DEFAULT   = 0,      ///< デフォルト
  //
  DATASTORE_DATATYPE_BATTLE_VIDEO_MANUAL = 200, ///< バトルビデオ(手動：BTLレコーダー)
  DATASTORE_DATATYPE_BATTLE_VIDEO_AUTO = 201,   ///< バトルビデオ(自動:ランダムマッチ、大会)
  //
  DATASTORE_DATATYPE_JF_PERSONAL = 300, ///< JFパーソナル

}ENUM_DATASTORE_DATA_TYPE;

//-------------------------------------
///  アトラクションGameMode
//=====================================
enum ATTRACTION_GAMEMODE
{

  ATTRACTION_GAMEMODE_ID = 300, ///< NexのMatchMakeClientに指定するゲームモード

};



//-------------------------------------
/// DataStore 永続化スロット
//=====================================
enum {
  DATASTORE_PERSISTENCE_SLOT_0,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_1,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_2,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_3,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_4,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_5,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_6,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_7,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_8,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_9,       ///< バトルビデオアップロード
  DATASTORE_PERSISTENCE_SLOT_10,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_11,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_12,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_13,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_14,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_15,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_16,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_17,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_18,      ///< momijiで使用
  DATASTORE_PERSISTENCE_SLOT_19,      ///< momijiで使用
};

/*

//赤外線SUBID
typedef enum{
  IR_SUBID_PSS = 0,  //PSS
  IR_SUBID_REGULATION,  //大会選手証
  IR_SUBID_LIVECUP,        // 赤外線大会本番
  IR_SUBID_BATTLETOWER,        //バトルタワーマルチ
  IR_SUBID_MYSTERY,        //不思議な贈り物
  IR_SUBID_PSS_TRADE,  //PSS交換
  IR_SUBID_CONTEST,//コンテスト
} ENUM_IR_SUBID;


//GFEachOtherの起動方法
enum EACHOTHER_INITREQ_E{
  EOIR_IR_BATTLE_AND_TRADE,//   ・赤外線対戦＆交換
  EOIR_WIRELESS_VCT,  //・ボイスチャット（ローカル通信）
  EOIR_WIRELESS_TRADE,  //・プロモ交換（ローカル通信）
  EOIR_BATTLE_HOUSE,  //・バトルハウス
  EOIR_RESTART,  // ・PSS停止状態からの再開
  EOIR_ERROR_RECOVER,  //     ・インターネット接続時の通信エラー
  EOIR_HOLO_MAIL,    //ホログラムメール
  EOIR_WIRLESS_BATTLE_RECRUIT,  //ワイヤレスのバトルの募集
  EOIR_OTHER,
};



//-----------------------------------------------------------------------------
//  交換に絡む通信仕様において、セーブ処理を必ず実施するための定義
//-----------------------------------------------------------------------------
#define NET_TRADE_SAVE_FORCED (true)

*/

#endif// __NET_DEFINE_H__
