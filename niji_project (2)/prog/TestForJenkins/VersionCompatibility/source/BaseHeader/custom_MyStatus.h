#if !defined(CUSTOM_MYSTATUS_H_INCLUDED)
#define CUSTOM_MYSTATUS_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   custom_MyStatus.h
 * @date   2016/11/02 Wed. 19:57:24
 * @author muto_yu
 * @brief  MyStatus.hからチェック用に定数定義のみを抽出
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








/*
  Savedata::MyStatusで参照しているSystem::DressUpParamSaveDataの定義
*/
namespace System  {
  // セーブデータのビット
  enum
  {
    DRESSUP_PARAM_SAVE_DATA_BIT_VERSION               = 2,  // バージョン情報

    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_SEX        = 1,  //BODY_CATEGORY_SEX,              // 性別
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_BODY_COLOR = 5,  //BODY_CATEGORY_BODY_COLOR,       // 体の色
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_EYE_COLOR  = 5,  //BODY_CATEGORY_EYE_COLOR,        // 目の色
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_HAIR_COLOR = 5,  //BODY_CATEGORY_HAIR_COLOR,       // 髪の色
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_LIP_COLOR  = 5,  //BODY_CATEGORY_FEMALE_LIP_COLOR, // リップの色

    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAIR       = 6,  //ITEM_CATEGORY_HAIR_STYLE,       // 髪型
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_00            = 3,  // パディング00
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCEYE     = 9,  //ITEM_CATEGORY_ACCEYE,           // アイアクセサリ
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCHAIR    = 9,  //ITEM_CATEGORY_ACCHAIR,          // ヘアアクセサリ
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BAG        = 10, //ITEM_CATEGORY_BAG,              // バッグ
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_01            = 4,  // パディング01
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BOTTOMS    = 10, //ITEM_CATEGORY_BOTTOMS,          // ボトムス
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAT        = 10, //ITEM_CATEGORY_HAT,              // 帽子
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_LEGS       = 10, //ITEM_CATEGORY_LEGS,             // レッグ
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_02            = 2,  // パディング02
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_SHOES      = 10, //ITEM_CATEGORY_SHOES,            // シューズ
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_TOPS       = 10, //ITEM_CATEGORY_TOPS,             // トップス
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_03            = 8,  // パディング03

    DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_ZRING      = 1,  // hasZRing,                      // Zリングを所持している
    DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_BAGCHARM   = 1,  // hasBagCharm,                   // 島巡りの証を所持している
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_04            = 2,  // パディング04
  };

  /**
   * @brief 着せ替え用パラメータ：セーブ用
   */
  struct DressUpParamSaveData
  {
/*
    enum DressUpVersion
    {
      DRESSUP_VERSION_NIJI,                                                                    // 着せ替えバージョン：niji
      DRESSUP_VERSION_RESERVED_0,                                                              // 着せ替えバージョン：予約０
      DRESSUP_VERSION_RESERVED_1,                                                              // 着せ替えバージョン：予約１
      DRESSUP_VERSION_RESERVED_2,                                                              // 着せ替えバージョン：予約２

      DRESSUP_VERSION_COUNT                                                                    // 着せ替えバージョン：総数
    };
*/
  #if PM_DEBUG
    static u32 s_debugVersion;                                                                 // デバッグ用のセーブデータバージョン
  #endif

    u32 version : DRESSUP_PARAM_SAVE_DATA_BIT_VERSION;                                         // バージョン情報

    u32 bodyParamSex : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_SEX;                             // 性別
    u32 bodyParamBodyColor : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_BODY_COLOR;                // 体の色
    u32 bodyParamEyeColor : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_EYE_COLOR;                  // 目の色
    u32 bodyParamHairColor : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_HAIR_COLOR;                // 髪の色
    u32 bodyParamLipColor : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_LIP_COLOR;                  // リップの色

    u32 itemParamHair : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAIR;                           // 髪型
    u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_00; // パディング00
    u32 itemParamAccEye : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCEYE;                       // アイアクセサリ
    u32 itemParamAccHair : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCHAIR;                     // ヘアアクセサリ
    u32 itemParamBag : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BAG;                             // バッグ
    u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_01; // パディング01
    u32 itemParamBottoms : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BOTTOMS;                     // ボトムス
    u32 itemParamHat : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAT;                             // 帽子
    u32 itemParamLegs : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_LEGS;                           // レッグ
    u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_02; // パディング02
    u32 itemParamShoes : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_SHOES;                         // シューズ
    u32 itemParamTops : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_TOPS;                           // トップス
    u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_03; // パディング03

    u32 hasZRing : DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_ZRING;                               // Zリングを所持している
    u32 hasBagCharm : DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_BAGCHARM;                         // 島巡りの証を所持している
    u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_04; // パディング04
  };


  /*
    GameStrLen.hより
  */
  enum
  {
    STRLEN_PLAYER_NAME  = 12,
    EOM_LEN = 1,
  };

}






namespace Savedata  {

  class PokeDiarySave
  {
  public:
    /// 時間情報
    struct DIARY_DATE
    {
      u32 year:7;     ///< 年
      u32 month:4;    ///< 月
      u32 day:5;      ///< 日
      u32 hour:5;     ///< 時
      u32 minute:6;   ///< 分
      u32 half_sec:5; ///< 秒
    };
    /// アップロードするデータ構造
    struct DIARY_DATA
    {
      u16 rom_code:6;     /**< 生成データのロムバージョン*/
      u16 diary_type:2;   /**< ダイアリー種別*/
      u16 diary_id:8;     /**< ダイアリーID（種別内分類）*/
      u16 data[3];        /**< 固有データ */
      DIARY_DATE date;    /**< 日付 */
    };
  };



  class MyStatus
  {
  public:
      //  アイコンデータ
      struct ICON_DATA
      {
        //  着せ替え
        //  16 byte
        System::DressUpParamSaveData dressUpParam;

        //  モーションid
        //  1 byte
        s8 animeId;

        //  モーションフレーム値
        //  1 byte
        u8 animeFrameNum;

        //  キャラの顔情報
        //  1 byte
        u8  eyeIndex;

        //  着せ替えをデフォルトにするか
        //  1byte
        u8  bDressDefault;

        //  カメラ情報
        //  カメラ座標
        //  8 byte
        f32 camPosX;
        f32 camPosY;

        //  カメラの画角
        //  4 byte
        f32 camFovy;

        //  キャラRot情報
        //  4 byte
        f32 modelYRot;

      };  // 36 byte




      struct CORE_DATA
      {
        // 基本情報
        u32                  ID;                          ///< プレーヤーID                       // 4
        u8                   rom_code;                    ///< PM_VERSION                         // 5
        u8                   sex;                         ///< 性別                               // 6
        u8                   padding1[2];                 ///< パディング領域

        // 本体情報
        u64                  pssDataStoreId;              ///< PssPersonalData格納先データストアId 368
        u64                  nexUniqueId;                 ///< NEXユニークID                      // 16
        u64                  transferdId;                 ///< 使用中CTRのTransferdID             // 24
        u64                  firstTransferdId;            ///< セーブデータ生成時のTransferdID    // 32
        u32                  principalId;                 ///< プリンシパルID                     // 36
        u32                  addressId;                   ///< 簡易アドレス情報(国など）          // 40
        u16                  latitude;                    ///< 緯度(nn::cfg::SimpleAddresssより)  // 42
        u16                  longitude;                   ///< 経度(nn::cfg::SimpleAddresssより)  // 44
        u8                   regionCode;                  ///< 3DS本体リージョンコード            // 45
        u8                   pokeLanguageId;              ///< ポケモン言語コード(LANG_～)        // 46
        u8                   coppacsRestriction;          ///< COPPA制限フラグ                    // 47
        u8                   coppacsSetting;              ///< 前回のCOPPA制限フラグ              // 48

        // 文字情報
        gfl2::str::STRCODE    name[ System::STRLEN_PLAYER_NAME+System::EOM_LEN ];                        ///< 自身の名前   2Byte*STRLEN_PLAYER_NAME(12+1)文字*1 // 98

        //  アイコン情報
        ICON_DATA                            icon;

        u16                  has_mega_ring     :1;             ///< メガリング入手フラグ       　332
        u16                  has_zenryoku_ring :1;             ///< ぜんりょくリング入手フラグ
        u16                  quick_is_penalty  :1;             ///< クイックマッチ用：交換ペナルティフラグ
        u16                  quick_trade_ok    :1;             ///< クイックマッチ用：交換可能フラグ
        u16                  quick_doubule_ok  :1;             ///< クイックマッチ用：ダブルバトル可能フラグ
        u16                  has_shimameguri_no_akashi : 1;    ///< しまめぐりのあかし入手フラグ
        u16                  quick_type        :3;             ///< クイックマッチタイプ
        u16                  padding2          :7;            ///< パディング領域

        u8                   ballThrowType;                    ////<バトルのボール投げの種類(0～7)
        u8                   padding3[64];
      };


      static const u32 SERIALIZE_DATA_SIZE      = sizeof(CORE_DATA);   ///< シリアライズバッファのサイズ

    /*
      CORE_DATAのメンバ位置
      通信互換性チェック用
      CORE_DATAがfinalでprivateになるため、必要な情報のみpublicに出すこととした
    */
    static const u32 CORE_DATA_OffsetOf_ID                  = offsetof(CORE_DATA, ID);
    static const u32 CORE_DATA_OffsetOf_rom_code            = offsetof(CORE_DATA, rom_code);
    static const u32 CORE_DATA_OffsetOf_sex                 = offsetof(CORE_DATA, sex);
    static const u32 CORE_DATA_OffsetOf_padding1            = offsetof(CORE_DATA, padding1);
    static const u32 CORE_DATA_OffsetOf_pssDataStoreId      = offsetof(CORE_DATA, pssDataStoreId);              ///< PssPersonalData格納先データストアId 368
    static const u32 CORE_DATA_OffsetOf_nexUniqueId         = offsetof(CORE_DATA, nexUniqueId);                 ///< NEXユニークID                      // 16
    static const u32 CORE_DATA_OffsetOf_transferdId         = offsetof(CORE_DATA, transferdId);                 ///< 使用中CTRのTransferdID             // 24
    static const u32 CORE_DATA_OffsetOf_firstTransferdId    = offsetof(CORE_DATA, firstTransferdId);            ///< セーブデータ生成時のTransferdID    // 32
    static const u32 CORE_DATA_OffsetOf_principalId         = offsetof(CORE_DATA, principalId);                 ///< プリンシパルID                     // 36
    static const u32 CORE_DATA_OffsetOf_addressId           = offsetof(CORE_DATA, addressId);                   ///< 簡易アドレス情報(国など）          // 40
    static const u32 CORE_DATA_OffsetOf_latitude            = offsetof(CORE_DATA, latitude);                    ///< 緯度(nn::cfg::SimpleAddresssより)  // 42
    static const u32 CORE_DATA_OffsetOf_longitude           = offsetof(CORE_DATA, longitude);                   ///< 経度(nn::cfg::SimpleAddresssより)  // 44
    static const u32 CORE_DATA_OffsetOf_regionCode          = offsetof(CORE_DATA, regionCode);                  ///< 3DS本体リージョンコード            // 45
    static const u32 CORE_DATA_OffsetOf_pokeLanguageId      = offsetof(CORE_DATA, pokeLanguageId);              ///< ポケモン言語コード(LANG_～)        // 46
    static const u32 CORE_DATA_OffsetOf_coppacsRestriction  = offsetof(CORE_DATA, coppacsRestriction);          ///< COPPA制限フラグ                    // 47
    static const u32 CORE_DATA_OffsetOf_coppacsSetting      = offsetof(CORE_DATA, coppacsSetting);              ///< 前回のCOPPA制限フラグ              // 48
    static const u32 CORE_DATA_OffsetOf_name                = offsetof(CORE_DATA, name);                        ///< 自身の名前   2Byte*STRLEN_PLAYER_NAME(12+1)文字*1 // 98
    static const u32 CORE_DATA_OffsetOf_icon                = offsetof(CORE_DATA, icon);
    static const u32 CORE_DATA_OffsetOf_ballThrowType       = offsetof(CORE_DATA, ballThrowType);                    ////<バトルのボール投げの種類(0～7)
    static const u32 CORE_DATA_OffsetOf_padding3            = offsetof(CORE_DATA, padding3);

  };



} /*  namespace Savedata  */
#endif  /*  #if !defined(CUSTOM_MYSTATUS_H_INCLUDED)  */
