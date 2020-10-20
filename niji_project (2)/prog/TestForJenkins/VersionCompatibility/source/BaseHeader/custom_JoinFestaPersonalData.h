#if !defined(CUSTOM_JOINFESTAPERSONALDATA_H_INCLUDED)
#define CUSTOM_JOINFESTAPERSONALDATA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   custom_JoinFestaPersonalData.h
 * @date   2016/11/02 Wed. 19:31:07
 * @author muto_yu
 * @brief  JoinFestaPersonalData.hからチェック用に定数定義のみを抽出
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================


// =============================================================================
/**/
//==============================================================================

namespace NetAppLib {
namespace JoinFesta {

/* ----------------------------------------------------------------------------------------- */
/**
* @brief ヘッダー(8byte)
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketHeader
{
  u8                    version;                    //!< ビーコンバージョン          1byte 
  u8                    requestCommand;             //!< リクエストコマンドタイプ    1byte
  u8                    unionPacketType;            //!< 共有情報タイプ              1byte
  u8                    padding;
  u32                   timestamp;                  //!< タイムスタンプ              4byte
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 個人情報(104byte)
* @attention 8byteアライメント!!!
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketPersonalInfo
{
  // パーソナル情報
  gfl2::str::STRCODE     name[ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ];    //!< 名前         26byte(2x(12+1))
  //----------------------------------------------------------------------------------
  u16                    sex                  : 3;                  //!< 性別                           0byte
  u16                    eula                 : 1;                  //!< EULA承認済みフラグ             0byte
  u16                    isGameClear          : 1;                  //!< ゲームクリアしているか？       0byte
  u16                    isRejectFriend       : 1;                  //!< 友達との通信拒否フラグ         0byte
  u16                    isRejectVip          : 1;                  //!< VIPとの通信拒否フラグ          0byte
  u16                    isRejectGuest        : 1;                  //!< ゲストとの通信拒否フラグ       0byte
  u16                    isAlwaysBattleOk     : 1;                  //!< 通信対戦いつでもOkフラグ
  u16                    isAlwaysTradeOk      : 1;                  //!< 通信交換いつでもOkフラグ
  u16                    isAlwaysAttractionOk : 1;                  //!< アトラクションいつでもOkフラグ
  u16                    isLowerBattleReject  : 1;                  //!< 下位バージョン対戦禁止フラグ   0byte
  u16                    padding              : 4;
  //----------------------------------------------------------------------------------
  u32                    myPrincipalId;                             //!< 自身のプリンシバルID           4Byte
  u64                    transferdId;                               //!< トランスファードID             8byte
  u64                    localFriendCode;                           //!< ローカルフレンドコード         8byte
  //----------------------------------------------------------------------------------
  u32                    addressId;                                 //!< 簡易アドレス情報(国など）      4Byte
  //----------------------------------------------------------------------------------
  u16                    patternHello;                              //!< 定型：あいさつ
  u16                    patternParting;                            //!< 定型：別れ
  u16                    patternEmotion;                            //!< 定型：感動
  u16                    patternRegret;                             //!< 定型：残念
  //----------------------------------------------------------------------------------
  Savedata::MyStatus::ICON_DATA         icon;                       //!< 着せ替え                      36byte
  u8                     regionCode;                                //!< リージョン                     1byte
  u8                     romCode;                                   //!< ROMコード
  u8                     languageId;                                //!< 言語コード
  u8                     padding3;
  //----------------------------------------------------------------------------------
  u16                    patternSecret;                             //!< 定型：秘密
  //----------------------------------------------------------------------------------
  u16                    recommendFacilitiesId          : 8;        //!< 一押し施設ID 全施設の連番の値 
  u16                    recommendFacilitiesColor       : 3;        //!< 一押し施設カラー
  u16                    recommendFacilitiesCharacterId : 4;        //!< 一押し施設店員のキャラクターID
  u16                    padding2                       : 1;
  //----------------------------------------------------------------------------------
  //
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 通信対戦の招待用（親）48Byte
*
* @note guestFriendIdはインターネット時はプリンシバルID、WIRELESS時はtrandfredIdとして扱う
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketBattleInvite
{
  u64                   guestFriendId[JoinFestaDefine::JOIN_FESTA_BEACON_GUEST_USER_MAX];  //!< 招待対象ユーザーID        24byte(3*8)
  u32                   ticketID;                                             //!< チケットID                4byte
  //----------------------------------------------------------------------------------
  u32                   guestUserNum         : 2;                             //!< 招待対象ユーザー登録数    4byte
  u32                   isWIFI               : 1;                             //!< 通信タイプ(Wifi/Wireless) 0byte
  u32                   commitUserFlag       : 3;                             //!< 確定ユーザーBitフラグ。対戦参加者として確定しているユーザーをビットフラグで管理。 0byte
  u32                   battle_rule          : 8;                             //!< 対戦ルール                0byte
  u32                   padding              : 18;                            //!< 予約                      0byte
  //----------------------------------------------------------------------------------
  //
  u8                    relation[JoinFestaDefine::JOIN_FESTA_BEACON_GUEST_USER_MAX];   //!< ユーザー関係              3byte
  //
  u8                    reserved[13];                                          //!< 予約                      3byte
};


/* ----------------------------------------------------------------------------------------- */
/**
* @brief 通信対戦の参加意思返信用（子）48Byte
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketBattleRecieve
{
  u32                   ticketID;                                         //!< 主催者のチケットID        4byte
  u32                   hostPrincipalId;                                  //!< 主催者のプリンシバルID    4byte
  u64                   hostLocalFriendCode;                              //!< 主催者のローカルID        8byte
  u8                    isRegulation;                                     //!< レギュレーション条件可否  1byte
  u8                    battle_rule;                                      //!< 対戦ルール
  u8                    reserved[30];                                     //!< 予約 23BYTE
};


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 通信交換の招待用（親）48Byte
 */
/* ----------------------------------------------------------------------------------------- */
struct PacketTradeInvite
{
  u32                       ticketID;                                     //!< チケットID                4byte
  u32                       guestPrincipalId;                             //!< 招待対象プリンシバルID    4byte
  u64                       guestLocalFriendCode;                         //!< 招待対象ユーザーID        8byte
  u8                        guestUserNum;                                 //!< 招待対象ユーザー登録数    1byte
  u8                        relation;                        //!< ユーザー関係              1byte
  u8                        reserved[30];                                 //!< 予約 21Byte
};


/* ----------------------------------------------------------------------------------------- */
/**
* @brief 通信交換の参加意思返信用（子）48Byte
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketTradeRecieve
{
  u32                   ticketID;                                         //!< 主催者のチケットID        4byte
  u32                   hostPrincipalId;                                  //!< 主催者のプリンシバルID    4byte
  u64                   hostLocalFriendCode;                              //!< 主催者のローカルID        8byte
  u8                    reserved[32];                                     //!< 予約 24BYTE
};


/* ----------------------------------------------------------------------------------------- */
/**
* @brief メッセージ送信用 48Byte
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketMessage
{
  u32                       hostPrincipalId;                              //!< 発言者のプリンシバルID    4byte
  u32                       targetPrincipalId;                            //!< 対象者のプリンシバルID    4byte
  u64                       hostLocalFriendCode;                          //!< 発言者のローカルID        8byte
  u64                       targetLocalFriendCode;                        //!< 対象者のローカルID        8byte
  u32                       ticketID;                                     //!< チケットID                4byte
  u8                        relation;                        //!< ユーザー関係              1byte
  u8                        reserved[19];                                 //!< 予約                      11byte
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief アトラクション用 48Byte
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketAttraction
{
  u64                       ownerTransferdID;                                            //!< 主催者のTID                                                                    8byte
  u32                       time;                                                        //!< ローカル：アトラクションの残り時間　or Wifi：アトラクション開始時のサーバ時間　4byte
  u32                       totalScore;                                                  //!< アトラクションのトータルスコア                                                 4byte
  u16                       myScore;                                                     //!< 自分のスコア                                                                   2byte
  gfl2::str::STRCODE        name[ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ];    //!< 主催者の名前                                                                  26byte
  u16                       ticketID;                                                    //!< アトラクションチケットID　　　　　　　　　　　　　　　　　　　　　　　　　　　2byte
  u8                        attractionID;                                                //!< アトラクションの種別                                                           1byte
  u8                        reserved[1];                                                    //!< 予約                                                                           1byte
};


/* ----------------------------------------------------------------------------------------- */
/**
* @brief 拡張情報1  48byte
*        個人情報枠
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketGameStatus1
{
  u32                   jfCoinCount;                                      //!< コインの累計               
  u32                   haveDressUpItemNum;                               //!< 持っている着せ替えアイテムの総数
  u16                   jfRank;                                           //!< フェスタのランク           
  u8                    padding[3];
  u8                    questionnaireAnswer[16];                          //!< アンケート回答
  u8                    facilitiesId[7];                                  //!< 自分のフェスサークルの施設ID

  u8                    reserved[11];                                      //!< 予約

  u8                    headerSymbol;                                     //!< 初期化済み判定フラグ
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 拡張情報2  48byte
*        レコード枠
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketGameStatus2
{
  // 4バイトレコード
  u32                   record1;                                          //!< RECID_COMM_BATTLE
  u32                   record2;                                          //!< RECID_WIFI_BATTLE
  u32                   record3;                                          //!< RECID_BTREE_CHALLENGE
  u32                   record4;                                          //!< RECID_BTL_ENCOUNT
  u32                   record5;                                          //!< RECID_WEAK_ATTACK
  u32                   record6;                                          //!< RECID_COMM_TRADE
  u32                   record7;                                          //!< RECID_MIRACLE_TRADE_COUNT
  u32                   record8;                                          //!< RECID_GTS_TRADE_COUNT
  u32                   record9;                                          //!< RECID_GTS_PUT
  u32                   record10;                                         //!< RECID_DAYMAX_EXP
  u32                   record11;                                         //!< RECID_MAME_GET_CNT

  u8                    headerSymbol;                                     //!< 初期化済み判定フラグ
  u8                    reserved[3];                                      //!< 予約
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 拡張情報3  48byte
*        レコード枠
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketGameStatus3
{
  // 4バイトレコード
  u32                   record1;                                          //!< RECID_BERRY_HARVEST
  u32                   record2;                                          //!< RECID_SHOPPING_MONEY
  u32                   record3;                                          //!< RECID_USE_BP
  u32                   record4;                                          //!< RECID_CAPTURE_POKE
  u32                   record5;                                          //!< RECID_TAMAGO_HATCHING
  u32                   record6;                                          //!< RECID_MAME_PRESENT_CNT
  u32                   record7;                                          //!< RECID_NICKNAME
  u32                   record8;                                          //!< RECID_RIDE_POKE
  u32                   record9;                                          //!< RECID_JF_USE_JCOIN
  u32                   record10;                                         //!< RECID_ZWAZA_CNT
  u32                   record11;                                         //!< RECID_JF_GEST_TALK_CNT

  u8                    headerSymbol;                                     //!< 初期化済み判定フラグ
  u8                    reserved[3];                                      //!< 予約
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 拡張情報4  48byte
*        レコード枠
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketGameStatus4
{
  // 2バイトレコード
  u16                   record1;                                          //!< RECID_DENDOU_CNT
  u16                   record2;                                          //!< RECID_KAWAIGARI_CNT
  u16                   record3;                                          //!< RECID_DAYMAX_CAPTURE
  u16                   record4;                                          //!< RECID_DAYMAX_EVOLUTION
  u16                   record5;                                          //!< RECID_DAYMAX_KILL
  u16                   record6;                                          //!< RECID_DAYMAX_TRAINER_BATTLE
  u16                   record7;                                          //!< RECID_JF_ATT_CNT
  u16                   record8;                                          //!< RECID_JF_SHOP_GET_CNT
  u16                   record9;                                          //!< RECID_RARE_COLOUR_ENC_COUNT
  u16                   record10;                                         //!< RECID_IDKUJI_ATARI_CNT
  u16                   record11;                                         //!< RECID_POINTUP_USE
  u16                   record12;                                         //!< RECID_KISEKAE_CNT
  u16                   record13;                                         //!< RECID_QR_SCAN
  u16                   record14;                                         //!< RECID_CAMERA_CNT
  u16                   dummy2;                                           //!< 未使用
  u16                   dummy3;                                           //!< 未使用
  u16                   dummy4;                                           //!< 未使用
  u16                   dummy5;                                           //!< 未使用
  u16                   dummy6;                                           //!< 未使用
  u16                   dummy7;                                           //!< 未使用
  u16                   dummy8;                                           //!< 未使用
  u16                   dummy9;                                           //!< 未使用

  u8                    headerSymbol;                                     //!< 初期化済み判定フラグ       1byte
  u8                    reserved[1];                                         //!< 予約                       1byte
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 拡張情報5  48byte
*        PokeDiary枠
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketGameStatus5
{
  // 最新のダイアリーデータが３件入る
  Savedata::PokeDiarySave::DIARY_DATA pokeDiary[3];
  u8                    headerSymbol;                                     //!< 初期化済み判定フラグ       1byte
  u8                    reserved[11];                                     //!< 予約                       1byte
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 拡張情報6  48byte
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketGameStatus6
{
  u8                    headerSymbol;                                     //!< 初期化済み判定フラグ       1byte
  u8                    dummy[47];
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 拡張情報7  48byte
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketGameStatus7
{
  u8                    headerSymbol;                                     //!< 初期化済み判定フラグ       1byte
  u8                    dummy[47];
};

/* ----------------------------------------------------------------------------------------- */
/**
* @brief 拡張情報8  48byte
*/
/* ----------------------------------------------------------------------------------------- */
struct PacketGameStatus8
{
  u8                    headerSymbol;                                     //!< 初期化済み判定フラグ       1byte
  u8                    dummy[47];
};

/* ----------------------------------------------------------------------------------------- */
/**
*
*  @brief 通信対戦パケット共用体(48Byte)
*
*  @note  以降の情報は48Byteを共通領域で使用するもの。
*         いずれか１つの情報しか保持できない
*/
/* ----------------------------------------------------------------------------------------- */
typedef union
{
  PacketBattleInvite     battleInvite;                                    //!< バトル招待側情報
  PacketBattleRecieve    battleReceive;                                   //!< バトル参加側情報
  PacketTradeInvite      tradeInvite;                                     //!< トレード招待側情報
  PacketTradeRecieve     tradeReceive;                                    //!< トレード参加側情報
  PacketMessage          message;                                         //!< メッセージ情報
  PacketAttraction       attraction;                                      //!< アトラクション情報
  PacketGameStatus1      status1;                                         //!< 拡張情報1
  PacketGameStatus2      status2;                                         //!< 拡張情報2
  PacketGameStatus3      status3;                                         //!< 拡張情報3
  PacketGameStatus4      status4;                                         //!< 拡張情報4
  PacketGameStatus5      status5;                                         //!< 拡張情報5
  PacketGameStatus6      status6;                                         //!< 拡張情報6
  PacketGameStatus7      status7;                                         //!< 拡張情報7  // nijiでは未使用
  PacketGameStatus8      status8;                                         //!< 拡張情報8  // nijiでは未使用
}UnionPacket;


/* ----------------------------------------------------------------------------------------- */
/**
* @brief JoinFesta用送受信データ ( MAX: PACKET_MAX_SIZE )
*
* @note 8Byteアライメント！ 最大160byte
*/
/* ----------------------------------------------------------------------------------------- */
struct JoinFestaPacketData
{
  //----------------------------------------------------------------------------------
  PacketHeader           header;                 //!< ヘッダー                     8byte
  //----------------------------------------------------------------------------------
  PacketPersonalInfo     personalInfo;           //!< 個人情報                     104byte
  //----------------------------------------------------------------------------------
  // ここはみんなで使う！
  UnionPacket            unionInfo;              //!< 共有情報                     48byte
  //----------------------------------------------------------------------------------
};



class JoinFestaPersonalData
{
public:
  /*
   * @brief 時間情報
   */
  struct Date
  {
    u32 year:7;     //!< 年
    u32 month:4;    //!< 月
    u32 day:5;      //!< 日
    u32 hour:5;     //!< 時
    u32 minute:6;   //!< 分
    u32 half_sec:5; //!< 秒
  };

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief パーソナルコアデータ（ジョインフェスタの1人辺りのセーブデータ）
   *
   * @attention 他人の「JoinFestaPacketData」「PacketGameStatus」は変更してはならない。<p>
   *            ビーコン受信時に差異があれば新しいパケットが届いたと判断される。
   */
  /* ----------------------------------------------------------------------------------------- */
  typedef struct
  {
    // パケットのヘッダー 8byte
    PacketHeader                           m_header;               //!< ヘッダー
    // パケットの個人情報 104byte
    PacketPersonalInfo                     m_personalInfo;         //!< 個人情報
    //========================================================================================
    // 拡張情報 48x8 384byte
    PacketGameStatus1                      m_gameStatus1;          //!< 拡張情報1                     40byte
    PacketGameStatus2                      m_gameStatus2;          //!< 拡張情報2
    PacketGameStatus3                      m_gameStatus3;          //!< 拡張情報3
    PacketGameStatus4                      m_gameStatus4;          //!< 拡張情報4
    PacketGameStatus5                      m_gameStatus5;          //!< 拡張情報5
    PacketGameStatus6                      m_gameStatus6;          //!< 拡張情報6 // nijiでは未使用
    PacketGameStatus7                      m_gameStatus7;          //!< 拡張情報7 // nijiでは未使用
    PacketGameStatus8                      m_gameStatus8;          //!< 拡張情報8 // nijiでは未使用
    //========================================================================================
    // クライアントで必要な情報 8byte
    Date                                   m_date;                 //!< 更新日時
    // レコードイベント系で必要な情報
    u8                                     m_eventRecordCategory[3];
    /// PERSONAL_EVENT_QUESTIONNAIRE_GUESS ズバリ当てろアンケートのID
    u8                                     m_eventQuestionId;
    u8                                     m_eventId;              //!< JoinFestaイベントID
    u8                                     m_eventSaveIndex;       //!< JoinFestaパーソナルイベントのどのインデックス使っているか
    u8                                     m_commRoute;            //!< 通信経路（CommRouteType定義）
    b8                                     m_isScriptListSeleted;  //!< スクリプトからのプレイヤーリストで選択されたかどうか 
    // レコードイベント系で必要な情報
    u8                                     m_eventRecordIndex[3];
    /// PERSONAL_EVENT_RECORD_INTRODUCTION レコード紹介で必要な情報
    u8                                     m_penaltyTime:1;        //!< GFNMCat[2524]対処。本体時間改ざん時にON。パケット受信時にOFFになる
    u8                                     padding:7;
    //========================================================================================
    // 計 512byte
  }CoreData;

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief パーソナルサブデータ（ジョインフェスタの1人辺りのメモリのみのデータ）
   */
  /* ----------------------------------------------------------------------------------------- */
  typedef struct
  {
    // memo:自分のm_unionPacketは送信時に使われる。受信相手のm_unionPacketはm_unionPakcetTypeの情報が入っている。
    UnionPacket                                       m_unionPacket;        //!< 共有情報の保存用。
    u8/*E_JOIN_FESTA_UNION_PACKET_TYPE*/              m_unionPakcetType;    //!< 共有情報タイプ
    //
    u8                                                m_relation;               //!< 自身との関係
    //
    b8                                                m_isLock;                 //!< 削除対象から除外するようにロック。リークの危険性があるので、管理は慎重に。
    b8                                                m_isOnline;               //!< オンラインフラグ
    //
    u8                                                m_isAttractionAddPoint;   //!< アトラクション中専用：加点対象かどうか
    u8/*JoinFestaAttractionDefine::AppearanceState*/  m_attractionState;        //!< アトラクション中専用：出現状態
    b8                                                m_isDummyData;            //!< アトラクション中専用：ダミーデータかどうか
    u8                                                m_attractionEventId;      //!< アトラクション中専用：JoinFestaイベントID
    u8                                                m_attractionPokeType1;    //!< アトラクション中専用：ポケモンタイプ1（相性アトラクション）
    u8                                                m_attractionPokeType2;    //!< アトラクション中専用：ポケモンタイプ2（相性アトラクション）
    u16                                               m_attractionMonsNo;       //!< アトラクション中専用：ポケモン番号（タイプ探すアトラクション)
    //
    b8                                                m_isPresetData;           //!< プリセットNPCフラグ
    u8                                                m_presetDataId;           //!< プリセットNPCID
    u8                                                padding[2];
    //========================================================================================
    // 計 64byte
  }SubData;
};

} /*  namespace JoinFesta */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(CUSTOM_JOINFESTAPERSONALDATA_H_INCLUDED)  */
