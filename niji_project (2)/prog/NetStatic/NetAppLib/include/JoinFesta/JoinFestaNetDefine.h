//=============================================================================
/**
 * @file   JoinFestaDefine.h
 * @brief  JoinFesta通信定義用クラス
 * @author ichiraku_katsuhiko
 *
 * @note   PSSから移植
 */
//=============================================================================

#ifndef __JOIN_FESTA_NET_DEFINE_H__
#define __JOIN_FESTA_NET_DEFINE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

#ifdef  GF_PLATFORM_CTR
#include <gflnet2/include/beacon/gflnet2_Beacon.h> // for BEACON_CONNECT_MAXBYTE
#include <nn/friends.h>
#endif // GF_PLATFORM_CTR 

/* ----------------------------------------------------------------------------------------- */
/**
* @brief JoinFesta用フレンドキー定義。
* @note メンバフィールド[localFriendCode]にはローカルフレンドコードではなく「transferedId」が格納される
*/
/* ----------------------------------------------------------------------------------------- */
#ifdef  GF_PLATFORM_CTR
  typedef nn::friends::FriendKey JoinFestaFriendKey;
#else
  // Win版では動かないが、CTRと同じのがないと
  // ビルド通らないのでfriensから取ってきた
  #define NN_FRIENDS_INVALID_PRINCIPAL_ID             (0)     //!< 無効な PrincipalId です。
  #define NN_FRIENDS_INVALID_LOCAL_FRIEND_CODE        (0ULL)  //!< 無効な LocalFriendCode です。
  struct JoinFestaFriendKey{
    u32 principalId;           //!< プリンシパル ID が入っています。
    u32 padding;                                    //!< 
    u64 localFriendCode;       //!< ローカルフレンドコードが入っています。

#ifdef __cplusplus
    inline void initialize()
    {
        this->principalId     = NN_FRIENDS_INVALID_PRINCIPAL_ID;
        this->localFriendCode = NN_FRIENDS_INVALID_LOCAL_FRIEND_CODE;
    }

    inline void initialize( u32 principalId )
    {
        this->principalId     = principalId;
        this->localFriendCode = NN_FRIENDS_INVALID_LOCAL_FRIEND_CODE;
    }

    inline void initialize( u64 localFriendCode )
    {
        this->principalId     = NN_FRIENDS_INVALID_PRINCIPAL_ID;
        this->localFriendCode = localFriendCode;
    }

    inline void initialize( u32 principalId, u64 localFriendCode )
    {
        this->principalId     = principalId;
        this->localFriendCode = localFriendCode;
    }

    inline JoinFestaFriendKey() {}
    inline JoinFestaFriendKey( u32     principalId )     { initialize(principalId);     }
    inline JoinFestaFriendKey( u64 localFriendCode ) { initialize(localFriendCode); }
    inline JoinFestaFriendKey( u32 principalId, u64 localFriendCode ) { initialize(principalId, localFriendCode); }
#endif
  };
#endif // GF_PLATFORM_CTR 


//=========================================================================================
//=========================================================================================
//=========================================================================================

namespace NetAppLib {

namespace JoinFesta {

/**
 * @brief JoinFesta汎用定義用クラス
 */
class JoinFestaDefine
{
public:
  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief ビーコンバージョン(4bit)
  * @brief 値が一致しないビーコンは受信時に排除される。
  */
  /* ----------------------------------------------------------------------------------------- */
  static const u8 JOIN_FESTA_BEACON_VERSION_SYMBOL = 7;

#if PM_DEBUG
  // デバッグでバージョンを変えることがあるので変数化
  static u8 JOIN_FESTA_BEACON_VERSION;
#else
  static const u8 JOIN_FESTA_BEACON_VERSION = JOIN_FESTA_BEACON_VERSION_SYMBOL;
#endif

  // 送信パケット最大サイズ
#ifdef  GF_PLATFORM_CTR
  static const u8 PACKET_MAX_SIZE =  gflnet2::base::BEACON_CONNECT_MAXBYTE;
#else
  static const u8 PACKET_MAX_SIZE =  164;
#endif // GF_PLATFORM_CTR

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief PacketGameStatus初期化判定用シンボル
  */
  /* ----------------------------------------------------------------------------------------- */
  static const u8 PACKET_GAME_STATUS_INITIALIZED_SYMBOL = 0x4D;

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief 対戦時に接続できる最大ゲストユーザー数
  */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 JOIN_FESTA_BEACON_GUEST_USER_MAX = 3;

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief ブラックリストのMAX人数
  */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 BLACK_LIST_MAX = 100;

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief パーソナル種別
   */
  /* ----------------------------------------------------------------------------------------- */
  typedef enum
  {
    E_JOIN_FESTA_RELATION_VIP,             //!< VIP
    E_JOIN_FESTA_RELATION_GUEST,           //!< ゲスト（すれ違いのこと）
    //
    E_JOIN_FESTA_RELATION_ANOTHER_MAX,     //!< 他人までの種別数
    //
    E_JOIN_FESTA_RELATION_MY  = E_JOIN_FESTA_RELATION_ANOTHER_MAX, //!< 自分自身(ライブラリ用。ユーザー使用禁止）
    E_JOIN_FESTA_RELATION_MAX = E_JOIN_FESTA_RELATION_MY,          //!< 自分も含めた最大種別数
  }E_JOIN_FESTA_RELATION;

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief プレイ状況
   */
  /* ----------------------------------------------------------------------------------------- */
  typedef enum
  {
    E_PLAY_SITUATION_JOIN_FESTA_NORAML,     //!< ジョインフェスタ：フィールド遊び中
    E_PLAY_SITUATION_JOIN_FESTA_SHOP,       //!< ジョインフェスタ：お店で買い物中
    E_PLAY_SITUATION_JOIN_FESTA_PERSONAL,   //!< ジョインフェスタ：人に話しかけ中
    //
    E_PLAY_SITUATION_FIELD,                 //!< フィールドで遊び中（ジョインフェスタ外）
    //
    E_PLAY_SITUATION_BATTLE_SINGLE,         //!< シングルバトル中
    E_PLAY_SITUATION_BATTLE_DOUBLE,         //!< ダブルバトル中
    E_PLAY_SITUATION_BATTLE_MULTI,          //!< マルチバトル中
    E_PLAY_SITUATION_BATTLE_ROYAL,          //!< ロイヤルバトル中
    E_PLAY_SITUATION_TRADE,                 //!< 通信交換中
    E_PLAY_SITUATION_GTS,                   //!< GTS中
    E_PLAY_SITUATION_MIRACLE_TRADE,         //!< ミラクル交換中
    E_PLAY_SITUATION_BATTLE_SPOT,           //!< バトルスポット中
    E_PLAY_SITUATION_ATTRACTION,            //!< アトラクション中 
  }E_PLAY_SITUATION;

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief リクエストコマンドタイプ(最大127まで指定可能)
   */
  /* ----------------------------------------------------------------------------------------- */
  typedef enum
  {
    //==========================================
    E_JOIN_FESTA_PACKET_REQUEST_NONE,                     //!<  0:設定コマンドなし
    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_BASIC_TOP,               //!<  1:基本情報コマンド用セパレータ(使用禁止）
    //==========================================
    E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS,         //!<  2:ゲーム情報更新（ジョインフェスタ内）
    E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS_FIELD,   //!<  3:ゲーム情報更新（フィールド版）
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_SINGLE,            //!<  4:シングルバトル中
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DOUBLE,            //!<  5:ダブルバトル中
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_MULTI,             //!<  6:マルチバトル中
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_ROYAL,             //!<  7:ロイヤルバトル中
    E_JOIN_FESTA_PACKET_REQUEST_TRADE,                    //!<  8:通信交換中
    E_JOIN_FESTA_PACKET_REQUEST_GTS,                      //!<  9:GTSプレイ中
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_SPOT,              //!<  10:バトルスポットプレイ中
    E_JOIN_FESTA_PACKET_REQUEST_MIRACLE_TRADE,            //!<  11:ミラクルトレードプレイ中
    E_JOIN_FESTA_PACKET_REQUEST_JF_SHOP,                  //!<  12:ジョインフェスタのお店で買い物中
    E_JOIN_FESTA_PACKET_REQUEST_JF_PERSONAL,              //!<  13:ジョインフェスタの人に話しかけ中
    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_BASIC_BOTTOM,            //!<  14:基本情報コマンド用セパレータ(使用禁止）
    //==========================================

    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_BATTLE_TOP,              //!< 15:対戦コマンド用セパレータ(使用禁止）
    //==========================================
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE,            //!< 16:対戦申し込み(親：招待)
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CANCEL,            //!< 17:対戦申し込み(親：中止)
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CONNECT,           //!< 18:対戦申し込み(親：接続まち)
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_JOIN,              //!< 19:対戦申し込み(子：参加OK)
    E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DISJOIN,           //!< 20:対戦申し込み(子：参加NG)
    //
    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_BATTLE_BOTTOM,           //!< 21:対戦コマンド用セパレータ(使用禁止）
    //==========================================

    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_TRADE_TOP,               //!< 22:交換コマンド用セパレータ(使用禁止）
    //==========================================
    E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE,             //!< 23:交換申し込み(親：招待)
    E_JOIN_FESTA_PACKET_REQUEST_TRADE_CANCEL,             //!< 24:交換申し込み(親：中止)
    E_JOIN_FESTA_PACKET_REQUEST_TRADE_CONNECT,            //!< 25:交換申し込み(親：接続まち)
    E_JOIN_FESTA_PACKET_REQUEST_TRADE_JOIN,               //!< 26:交換申し込み(子：参加OK)
    E_JOIN_FESTA_PACKET_REQUEST_TRADE_DISJOIN,            //!< 27:交換申し込み(子：参加NG)
    //
    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_TRADE_BOTTOM,            //!< 28:交換コマンド用セパレータ(使用禁止）
    //==========================================

    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_MESSAGE_TOP,             //!< 29:メッセージコマンド用セパレータ(使用禁止）
    //==========================================
    //E_JOIN_FESTA_PACKET_REQUEST_MESSAGE_NICE,             //!< :「ナイス！」メッセージコマンド
    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_MESSAGE_BOTTOM,          //!< 30:メッセージコマンド用セパレータ(使用禁止）
    //==========================================


    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_ATTRACTION_TOP,             //!< 31:アトラクションコマンド用セパレータ(使用禁止）
    //==========================================
    E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION,                  //!< 32:アトラクション情報
    //==========================================
    E_JOIN_FESTA_PACKET_SEPARTOR_ATTRACTION_BOTTOM,          //!< 33:アトラクションコマンド用セパレータ(使用禁止）
    //==========================================

    E_JOIN_FESTA_PACKET_REQUESR_TERMINATE_FOR_NIJI,       //!< nijiでは以降は非対応のコマンドとして扱う
    //==========================================

    // アッパーで新パケット増えたらここに定義追加する
#if PM_DEBUG
    E_JOIN_FESTA_PACKET_UPPER_DUMMY,                      //!< デバッグでアッパー確認用
#endif

    //==========================================
  }E_JOIN_FESTA_PACKET_REQUEST_KIND;

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief パケット共有情報タイプ
   */
  /* ----------------------------------------------------------------------------------------- */
  typedef enum
  {
    E_JOIN_FESTA_UNION_PACKET_NONE,               //!<   0:なし
    E_JOIN_FESTA_UNION_PACKET_BATTLE_INVITE,      //!<   1:バトル招待側情報
    E_JOIN_FESTA_UNION_PACKET_BATTLE_RECIEVE,     //!<   2:バトル参加側情報
    E_JOIN_FESTA_UNION_PACKET_TRADE_INVITE,       //!<   3:交換親機変更情報
    E_JOIN_FESTA_UNION_PACKET_TRADE_RECIEVE,      //!<   4:交換参加側情報
    E_JOIN_FESTA_UNION_PACKET_MESSAGE,            //!<   5:メッセージ情報
    E_JOIN_FESTA_UNION_PACKET_ATTRACTION,         //!<   6:アトラクション情報
    E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1,      //!<   7:拡張情報1
    E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_2,      //!<   8:拡張情報2
    E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_3,      //!<   9:拡張情報3
    E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_4,      //!<  10:拡張情報4
    E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_5,      //!<  11:拡張情報5
    E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_6,      //!<  12:拡張情報6
    E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_7,      //!<  13:拡張情報7 // nijiでは未使用
    E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_8,      //!<  14:拡張情報8 // nijiでは未使用

    E_JOIN_FESTA_UNION_PACKET_TERMINATE_FOR_NIJI, //!<  15:nijiでは以降は非対応として扱う

    //==========================================

    // アッパーで新パケット増えたらここに定義追加する
#if PM_DEBUG
    E_JOIN_FESTA_UNION_PACKET_UPPER_DUMMY,        //!< デバッグでアッパー確認用
#endif

    //==========================================
  }E_JOIN_FESTA_UNION_PACKET_TYPE;

};



}	// JoinFesta
}	// NetAppLib


#endif // __JOIN_FESTA_NET_DEFINE_H__
