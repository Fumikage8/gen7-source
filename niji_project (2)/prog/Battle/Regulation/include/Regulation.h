//======================================================================
/**
 * @file Regulation.h
 * @brief レギュレーションデータ構造
 * @author ichiraku_katsuhiko
 * @date 2015/07/15 19:55:58
 */
//======================================================================

#if !defined __REGULATION_H_INCLUDED__
#define __REGULATION_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_StrBuf.h>
#include <heap/include/gfl2_Heap.h>

#include "System/include/GameStrLen.h"

//GFL_NAMESPACE_BEGIN(Battle)

// 前方宣言
#if PM_DEBUG
class RegulationDebug;
#endif

/**
 * @class Regulation
 * @brief レギュレーション
 */
class Regulation
{
  GFL_FORBID_COPY_AND_ASSIGN(Regulation);
public:

#if PM_DEBUG
  // レギュレーションデバッグクラス
  friend class RegulationDebug;
#endif

  /**
  * @enum BATTLE_CATEGORY
  * バトル場所
  */
  enum BATTLE_CATEGORY{
    BATTLE_CATEGORY_NET_BTL,              //! 通信対戦(P2P)
    BATTLE_CATEGORY_SPOT_FREE_LEGEND,     //! ランダムマッチ[フリー]伝説あり
    BATTLE_CATEGORY_SPOT_FREE,            //! ランダムマッチ[フリー]伝説無し
    BATTLE_CATEGORY_SPOT_RATE,            //! ランダムマッチ[レーティング]
    BATTLE_CATEGORY_SPOT_SP,              //! ランダムマッチ[スペシャル]
    BATTLE_CATEGORY_TREE_NORMAL,          //! バトルツリーノーマル
    BATTLE_CATEGORY_TREE_SUPER,           //! バトルツリースーパー
    BATTLE_CATEGORY_ROYAL_FACILITY_NORMAL,//! バトルロイヤル施設ノーマル
    BATTLE_CATEGORY_ROYAL_FACILITY_SUPER, //! バトルロイヤル施設スーパー
    BATTLE_CATEGORY_ROYAL_FACILITY_HYPER, //! バトルロイヤル施設ハイパー
    BATTLE_CATEGORY_ROYAL_FACILITY_MASTER,//! バトルロイヤル施設マスター
    BATTLE_CATEGORY_LIVE_CUP,             //! ライブ大会
    BATTLE_CATEGORY_NET_CUP,              //! インターネット大会
    BATTLE_CATEGORY_GROUP_CUP,            //! 仲間大会
    BATTLE_CATEGORY_DELIVERY,             //! 配信レギュレーション
    BATTLE_CATEGORY_WCS,                  //! ランダムマッチ[レーティング]WCSルール
    //
    // ↓↓momiji追加↓↓
    //
    BATTLE_CATEGORY_BATTLE_FES,           //! バトルフェス

    BATTLE_CATEGORY_MAX,
  };

  /**
  * @enum BATTLE_RULE
  * レギュレーションデータ用バトルルール
  *
  * バトルのルールだとマルチが定義に含まれていないが、
  * (マルチ=ダブルな為）
  * バトル以外のアプリでは、下記４つを選択するUIなので、マルチを含んでいた方が都合が良い。
  */
  enum BATTLE_RULE{
    BATTLE_RULE_SINGLE,       //! シングル
    BATTLE_RULE_DOUBLE,       //! ダブル
    BATTLE_RULE_ROYAL,        //! ロイヤル
    BATTLE_RULE_MULTI,        //! マルチ
  };

  /**
  * @enum PRESET
  * プリセットレギュレーション
  * レギュレーションのGARCのインデックス番号と同じ
  */
  enum PRESET{
    PRESET_SINGLE_NORMAL,           //! シングル・ノーマルルール
    PRESET_SINGLE_FLAT,             //! シングル・フラットルール
    PRESET_SINGLE_UNLIMITED,        //! シングル・制限なし
    PRESET_DOUBLE_NORMAL,           //! ダブル・ノーマルルール
    PRESET_DOUBLE_FLAT,             //! ダブル・フラットルール
    PRESET_DOUBLE_UNLIMITED,        //! ダブル・制限なし
    PRESET_MULTI_NORMAL,            //! マルチ・ノーマルルール
    PRESET_MULTI_FLAT,              //! マルチ・フラットルール
    PRESET_MULTI_UNLIMITED,         //! マルチ・制限なし
    PRESET_ROYAL_NORMAL,            //! ロイヤル・ノーマルルール
    PRESET_ROYAL_FLAT,              //! ロイヤル・フラットルール
    PRESET_ROYAL_UNLIMITED,         //! ロイヤル・制限なし
    PRESET_BATTLE_TREE_NORMAL_S,    //! バトルツリーノーマル　シングル
    PRESET_BATTLE_TREE_NORMAL_D,    //! バトルツリーノーマル　ダブル
    PRESET_BATTLE_TREE_NORMAL_M,    //! バトルツリーノーマル　マルチ
    PRESET_BATTLE_TREE_SUPER_S,     //! バトルツリースーパー　シングル
    PRESET_BATTLE_TREE_SUPER_D,     //! バトルツリースーパー　ダブル
    PRESET_BATTLE_TREE_SUPER_M,     //! バトルツリースーパー　マルチ
    PRESET_ROYAL_FACILITY_NORMAL,   //! ロイヤル・バトル施設 ノーマルランク
    PRESET_ROYAL_FACILITY_SUPER,    //! ロイヤル・バトル施設 スーパーランク
    PRESET_ROYAL_FACILITY_HYPER,    //! ロイヤル・バトル施設 ハイパーランク
    PRESET_ROYAL_FACILITY_MASTER,   //! ロイヤル・バトル施設 マスターランク
    PRESET_MOCK_SINGLE,             //! 模擬戦・シングル
    PRESET_MOCK_DOUBLE,             //! 模擬戦・ダブル
    //
    // ↓↓momiji追加↓↓
    //
    PRESET_BATTLE_FES,              //! バトルフェス

    PRESET_MAX,
  };

  /**
  * @enum LEVEL_RANGE_TYPE
  * レベル補正条件
  */
  enum LEVEL_RANGE_TYPE{
    LEVEL_RANGE_NORMAL,       //!なし
    LEVEL_RANGE_OVER,         //!以上
    LEVEL_RANGE_LESS,         //!以下
    LEVEL_RANGE_DRAG_DOWN,    //!以上補正   補正レベル以上ならば補正レベルにする（50補正で51なら50）
    LEVEL_RANGE_SAME,         //!全補正
    LEVEL_RANGE_PULL_UP,      //!以下補正  補正レベル以下ならば補正レベルにする(50補正で49なら50)
    LEVEL_RANGE_MAX   //
  };

  /**
  * @enum TIME_LIMIT_TYPE
  * 制限時間タイプ
  */
  enum TIME_LIMIT_TYPE
  {
    TIME_LIMIT_TOTAL,         //! 総合時間制（今までと同じ）
    TIME_LIMIT_CONTROL,       //! 持ち時間制（将棋、囲碁）
  };


  /**
  * @enum BACKGROUND_TYPE
  * 背景指定種類
  */
  enum BACKGROUND_TYPE
  {
    BACKGROUND_NORMAL1, //! 通常の通信対戦、15秒でカメラ動作開始
    BACKGROUND_NORMAL2, //! 通常の通信対戦、2秒でカメラ動作開始
    BACKGROUND_WCS1,    //! WCS、15秒でカメラ動作開始
    BACKGROUND_WCS2,    //! WCS、2秒でカメラ動作開始

    BACKGROUND_MAX,
  };

  //-----------------------------------------------------------------------------
  static const u32 DELIVERY_NAME_MOJINUM = 36;  //! 配信レギュレーションの名前文字列文字数
  static const u32 DELIVERY_TEXT_MOJINUM = 198; //! 配信レギュレーションの説明文字列文字数
  static const u32 WIFI_PLAYER_TIX_CUPNAME_MOJINUM = 36; //! デジタル選手証大会名文字列文字数
  static const u32 MUST_POKE_NUM = 6;           //! 18byte 6匹まで
  static const u32 ALL_FORM_SELECT = 255;       //! 全て(0～254)のフォルムを対象にする定義

  // レギュレーションデータ最大サイズ（スクリプト含む、証明書含まない）
  static const u32 REGULATION_MAX_DATA_SIZE      = 1024 * 64;   //! レギュレーションデータ最大サイズ                    64kb
  static const u32 REGULATION_MAX_DATA_SIZE_LIVE = 1024 *  8;   //! レギュレーションデータ（ライブ大会専用）最大サイズ   8kb

  /**
  * @enum ValueParamId
  * レギュレーションの数値パラメータID
  */
  typedef enum{
    //==============================
    //基本レギュレーションデータ
    // 未使用 REGULATION_VERSION,                                 //! レギュレーションバージョン情報
    // 未使用 PATCH_VERSION_MAJOR,                                //! パッチバージョン(メジャー)
    // 未使用 PATCH_VERSION_MINOR,                                //! パッチバージョン(マイナー)
    // 未使用 PATCH_VERSION_BUILD,                                //! パッチバージョン(ビルド)
    TEAM_NUM_LO,                                        //! バトルチーム下限
    TEAM_NUM_HI,                                        //! バトルチーム上限
    SELECT_NUM_LO,                                      //! 選出ポケモン下限
    SELECT_NUM_HI,                                      //! 選出ポケモン上限
    LEGEND_NUM_HI,                                      //! 伝説参加上限
    LEVEL,                                              //! 参加レベル指定
    TIME_VS,                                            //! 制限時間 分単位バトルに渡すときには秒に変換してください
    TIME_COMMAND,                                       //! 入力時間 秒単位
    SHOW_POKE_TIME,                                     //! ポケモン見せ合い時間
    DISCRIMINANT_ID,                                    //! 識別ID（カテゴリにより意味が変わる）
    //==============================
    // 特殊データ
    URL_DATA,                                           //! 詳細情報urlデータ
    BTL_VIDEO,                                          //! バトルビデオ送信条件
    //==============================
    // 大会専用のデータ
    BATTLE_NUM,                                         //! 対戦回数
    BATTLE_NUM_LIMIT,                                   //! 1日の対戦回数上限
    START_ENABLE_TIME,                                  //! 対戦可能時刻（開始）
    END_ENABLE_TIME,                                    //! 対戦可能時刻（終了
    START_YEAR,                                         //! 開始年：0000-9999
    START_MONTH,                                        //! 開始月：01-12
    START_DAY,                                          //! 開始日：01-31
    END_YEAR,                                           //! 終了年：0000-9999
    END_MONTH,                                          //! 終了月：01-12
    END_DAY                                             //! 終了日：01-31
  } ValueParamId;

  /**
  * @enum BoolParamId
  * レギュレーションのフラグパラメータID
  */
  typedef enum{
    //==============================
    //基本レギュレーションデータ
    REVERSE,                                            //! 逆さバトルフラグ
    BOTH_POKE,                                          //! 同じポケモン
    BOTH_ITEM,                                          //! 同じどうぐ
    HURAETTE_HAKAI,                                     //! フラエッテ破壊フォルム（幻）の禁止フラグ
    QR_TEAM,                                            //! qrバトルチーム使用可能フラグ 
    NICKNAME,                                           //! ニックネーム表示
    SHOW_POKE,                                          //! ポケモン見せ合いフラグ
    //==============================
    // 特殊データ
    WATCH,                                              //! 観戦機能フラグ
  } BoolParamId;

  /**
  * @enum TextParamId
  * レギュレーションの文字パラメータID
  */
  typedef enum{
    //==============================
    // 特殊データ(480byte)
    DELIVERY_NAME, //! 配信レギュレーションの名称(36文字+EOM)
    DELIVERY_TEXT, //! 配信レギュレーションの説明(198文字+EOM)
    //==============================
    // 大会専用のデータ(164byte)
    CUPNAME1,  //大会名（36文字＋EOM）
    CUPNAME2,  //大会名（36文字＋EOM）
  } TextParamId;

  /**
  * @enum BitParamId
  * レギュレーションのビットパラメータID
  */
  typedef enum{
    //==============================
    //基本レギュレーションデータ
    VETO_POKE_BIT,              //! 禁止ポケモン 128byte
    VETO_WAZA_BIT,              //! 禁止わざ 128byte
    VETO_ITEM_BIT,              //! 禁止道具 128byte
  } BitParamId;

  
private:
  //----------------------------------------------------------
  /**
  * @brief  バトルレギュレーションデータ型定義  fushigi_data.h参照の為外部公開に
  */
  //----------------------------------------------------------
  static const u32 SPECIFIC_VETO_POKE_NUM = 24; //! 72byte 24匹まで
  static const u32 REG_POKENUM_MAX_BYTE = 128;  //! 128byte 1024bit
  static const u32 REG_WAZANUM_MAX_BYTE = 128;  //! 128byte 1024bit
  static const u32 REG_ITEMNUM_MAX_BYTE = 128;  //! 128byte 1024bit
  static const u32 PRODUCING_AREA_MAX_BYTE = 8; //! 8byte 8個まで指定
  static const u32 ROM_MAX_BYTE = 4;            //! 4byte 32bit
  static const u32 LANGUAGE_MAX_BYTE = 4;       //! 4byte 32bit


  //-------------------------------------
  /// レギュレーションデータ
  //=====================================
  /**
   * @struct CoreData
   * @brief  レギュレーションデータの構造体型(POD)
   *         計1190byte
   */
  struct CoreData{
    //==============================
    //基本レギュレーションデータ(540byte)
    u8  regulation_version;                                 //! レギュレーションバージョン情報
    u8  patch_version_major;                                //! パッチバージョン(メジャー)
    u8  patch_version_minor;                                //! パッチバージョン(マイナー)
    u8  patch_version_build;                                //! パッチバージョン(ビルド)
    u8  category;                                           //! カテゴリ種別
    u8  btl_rule;                                           //! バトルルール
    u8  team_num_lo;                                        //! バトルチーム下限
    u8  team_num_hi;                                        //! バトルチーム上限
    u8  select_num_lo;                                      //! 選出ポケモン下限
    u8  select_num_hi;                                      //! 選出ポケモン上限
    u8  legend_num_hi;                                      //! 伝説参加上限
    u8  level;                                              //! 参加レベル指定
    u8  level_range;                                        //! レベル補正条件
    u8  reverse;                                            //! 逆さバトルフラグ
    u8  both_poke;                                          //! 同じポケモン
    u8  both_item;                                          //! 同じどうぐ
    u16 must_poke_and[ MUST_POKE_NUM ];                     //! 必須ポケモンAND 12byte
    u8  must_poke_form_and[ MUST_POKE_NUM ];                //! 必須ポケモンANDフォルム 6byte
    u16 must_poke_or[ MUST_POKE_NUM ];                      //! 必須ポケモンOR 12byte
    u8  must_poke_form_or[ MUST_POKE_NUM ];                 //! 必須ポケモンORフォルム 6byte
    u16 specific_veto_poke[ SPECIFIC_VETO_POKE_NUM ];       //! 禁止特定フォルムポケモン 種族番号 48byte
    u8  specific_veto_poke_form[ SPECIFIC_VETO_POKE_NUM ];  //! 禁止特定フォルムポケモン フォルム番号 24byte
    u8  veto_poke_bit[ REG_POKENUM_MAX_BYTE ];              //! 禁止ポケモン 128byte
    u8  veto_waza_bit[ REG_WAZANUM_MAX_BYTE ];              //! 禁止わざ 128byte
    u8  veto_item_bit[ REG_ITEMNUM_MAX_BYTE ];              //! 禁止道具 128byte
    u8  huraette_hakai;                                     //! フラエッテ破壊フォルム（幻）の禁止フラグ
    u8  producing_area[ PRODUCING_AREA_MAX_BYTE ];          //! 産地限定 8byte
    u8  qr_team;                                            //! QRバトルチーム使用可能フラグ 
    u8  time_limit_type;                                    //! 制限時間タイプ TIME_LIMIT_TYPE
    u8  time_vs;                                            //! 制限時間 分単位バトルに渡すときには秒に変換してください
    u8  time_command;                                       //! 入力時間 秒単位
    u8  nickname;                                           //! ニックネーム表示
    u8  background;                                         //! 背景指定 BACKGROUND_TYPE
    u8  show_poke;                                          //! ポケモン見せ合いフラグ
    u8  show_poke_time;                                     //! ポケモン見せ合い時間
    u8  _padding1[3];
    u8  rom_bit[ ROM_MAX_BYTE ];                            //! 参加ロムバージョン 4byte
    u8  language_bit[ LANGUAGE_MAX_BYTE ];                  //! 参加言語バージョン 4byte
    u32 discriminant_id;                                    //! 識別ID（カテゴリにより意味が変わる）

    //==============================
    // 特殊データ(480byte)
    gfl2::str::STRCODE delivery_name[ DELIVERY_NAME_MOJINUM + System::EOM_LEN ]; //! 配信レギュレーションの名称(36文字+EOM)
    gfl2::str::STRCODE delivery_text[ DELIVERY_TEXT_MOJINUM + System::EOM_LEN ]; //! 配信レギュレーションの説明(198文字+EOM)
    u32 url_data;                                           //! 詳細情報URLデータ
    u16 btl_video;                                          //! バトルビデオ送信条件
    u8  watch;                                              //! 観戦機能フラグ
    u8  _padding2;

    //==============================
    // 大会専用のデータ(164byte)
    gfl2::str::STRCODE cupname1[WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN];  //大会名（36文字＋EOM）
    gfl2::str::STRCODE cupname2[WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN];  //大会名（36文字＋EOM）
    u8  battle_num;                                         //! 対戦回数
    u8  battle_num_limit;                                   //! 1日の対戦回数上限
    u8  start_enable_time;                                  //! 対戦可能時刻（開始）
    u8  end_enable_time;                                    //! 対戦可能時刻（終了
    u16 start_year;                                         //! 開始年：0000-9999
    u8  start_month;                                        //! 開始月：01-12
    u8  start_day;                                          //! 開始日：01-31
    u16 end_year;                                           //! 終了年：0000-9999
    u8  end_month;                                          //! 終了月：01-12
    u8  end_day;                                            //! 終了日：01-31

    //==============================
    // スクリプト用データ(8byte)
    u32 amx_size;                                           //! amxのサイズ
    u32 amx_offset;                                         //! amxへのオフセット

    //==============================
    // CRCデータ(4byte)
    u16 crc;                                                //! 整合性検査
    u8  _padding3[2];
  };


  //-------------------------------------
  /// 圧縮レギュレーションデータ
  //=====================================
  /**
   * @struct SimpleData
   */
  struct SimpleData
  {
    u8  regulation_version;                                 //! レギュレーションバージョン情報
    u8  patch_version_major;                                //! パッチバージョン(メジャー)
    u8  patch_version_minor;                                //! パッチバージョン(マイナー)
    u8  patch_version_build;                                //! パッチバージョン(ビルド)
    u8  category;                                           //! カテゴリ種別
    u8  btl_rule;                                           //! バトルルール
    u8  team_num_lo;                                        //! バトルチーム下限
    u8  team_num_hi;                                        //! バトルチーム上限
    u8  select_num_lo;                                      //! 選出ポケモン下限
    u8  select_num_hi;                                      //! 選出ポケモン上限
    u8  legend_num_hi;                                      //! 伝説参加上限
    u8  level;                                              //! 参加レベル指定
    u8  level_range;                                        //! レベル補正条件
    u8  reverse;                                            //! 逆さバトルフラグ
    u8  both_poke;                                          //! 同じポケモン
    u8  both_item;                                          //! 同じどうぐ
    u8  veto_poke_bit[ REG_POKENUM_MAX_BYTE ];              //! 禁止ポケモン 128byte
    u8  huraette_hakai;                                     //! フラエッテ破壊フォルム（幻）の禁止フラグ
    u8  _padding0;
    u16 specific_veto_poke[ SPECIFIC_VETO_POKE_NUM ];       //! 禁止特定フォルムポケモン 種族番号 20byte
    u8  specific_veto_poke_form[ SPECIFIC_VETO_POKE_NUM ];  //! 禁止特定フォルムポケモン フォルム番号 10byte
    u16 must_poke_and[ MUST_POKE_NUM ];                     //! 必須ポケモンAND 12byte
    u8  must_poke_form_and[ MUST_POKE_NUM ];                //! 必須ポケモンANDフォルム 6byte
    u16 must_poke_or[ MUST_POKE_NUM ];                      //! 必須ポケモンOR 12byte
    u8  must_poke_form_or[ MUST_POKE_NUM ];                 //! 必須ポケモンORフォルム 6byte


    u8  producing_area[ PRODUCING_AREA_MAX_BYTE ];          //! 産地限定 8byte
    u8  qr_team;                                            //! QRバトルチーム使用可能フラグ 
    u8  time_limit_type;                                    //! 制限時間タイプ TIME_LIMIT_TYPE
    u8  time_vs;                                            //! 制限時間 分単位バトルに渡すときには秒に変換してください
    u8  time_command;                                       //! 入力時間 秒単位
    u8  nickname;                                           //! ニックネーム表示
    u8  background;                                         //! 背景指定 BACKGROUND_TYPE
    u8  show_poke;                                          //! ポケモン見せ合いフラグ
    u8  show_poke_time;                                     //! ポケモン見せ合い時間
    u8  rom_bit[ ROM_MAX_BYTE ];                            //! 参加ロムバージョン 4byte
    u8  language_bit[ LANGUAGE_MAX_BYTE ];                  //! 参加言語バージョン 4byte
    u8  _padding1[2];
    u32 discriminant_id;                                    //! 識別ID（カテゴリにより意味が変わる）


    gfl2::str::STRCODE cupname1[WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN];  //大会名（36文字＋EOM）
    gfl2::str::STRCODE cupname2[WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN];  //大会名（36文字＋EOM）
    u8  battle_num;                                         //! 対戦回数
    u8  battle_num_limit;                                   //! 1日の対戦回数上限
    u8  start_enable_time;                                  //! 対戦可能時刻（開始）
    u8  end_enable_time;                                    //! 対戦可能時刻（終了
    u16 start_year;                                         //! 開始年：0000-9999
    u8  start_month;                                        //! 開始月：01-12
    u8  start_day;                                          //! 開始日：01-31
    u16 end_year;                                           //! 終了年：0000-9999
    u8  end_month;                                          //! 終了月：01-12
    u8  end_day;                                            //! 終了日：01-31
    u8  _padding2[2];

    //圧縮レギュレーション固有
    u8  vetoBeads;                                          //! ビーズ禁止かどうか
    u8  vetoMegaRing;                                       //! メガストーン禁止かどうか
    u64 nexUniqueID;                                        //! 読み取れるユーザのNexuniqueID
  };

public:
  //! 固定レギュレーションのサイズ
  static const u32 FIX_CORE_DATA_SIZE = sizeof(CoreData);

  // シリアライズデータサイズ
  u32 GetSerializeDataSize() const;
  // シリアライズ
  void Serialize( void* dst ) const;
  // デシリアライズ
  void Deserialize( const void* data, const u32 size );

public:
  /// コンストラクタ
  Regulation( gfl2::heap::HeapBase* pHeap );
  /// デストラクタ
  virtual ~Regulation( void );

public:

  /**
   * @breif       ＲＯＭからレギュレーションデータを得る
   * @param[in]   preset プリセットレギュレーション
   */
  void LoadData( const PRESET preset );

  /**
   * @breif       レギュレーションデータの削除
   */
  void DeleteData( void );


  /**
   * @breif       DLしたレギュレーションのバッファをセット
   * @param[in]   pBuffer   レギュレーションデータ
   * @param[in]   size      レギュレーションデータのサイズ
   */
  void SetData( const void* pBuffer, const u32 size );

  /**
   * @breif       圧縮レギュレーションのバッファをセット
   * @param[in]   pBuffer   レギュレーションデータ
   */
  void SetSimpleData( const void* pBuffer );

  /**
   * @breif       圧縮レギュレーションのバッファをセット
   * @param[in]   pBuffer   レギュレーションデータ
   */
  bool SetSimpleData( const void* pBuffer, u64 nexUniqueID );

  /**
   * @breif       amxデータを取得
   * @return      amxデータ
   */
  void* GetAmxData() const;

  /**
   * @breif       amxデータのサイズを取得
   * @return      amxデータのサイズ
   */
  u32 GetAmxDataSize() const;

  /**
   * @breif       データが読み込まれているか
   * @return      読み込まれているなら true
   */
  bool IsExistsData() const;

  //----------------------------------------------------------
  // 各パラメータ
  //----------------------------------------------------------

  /**
   * @brief 数値のパラメーターを取得する
   * @param id    ValueParamId enum のどれか
   * @return パラメータ値
   */
  u32 GetValueParam( ValueParamId id ) const;

  /**
   * @brief フラグのパラメーターを取得する
   * @param id    BoolParamId enum のどれか
   * @return パラメータ値
   */
  bool GetBoolParam( BoolParamId id ) const;

  /**
   * @brief テキストのパラメータを取得する
   * @param id  TextParamId enum のどれか
   * @param pReturnCupName  テキストを入れるSTRBUFポインタ
   * @return  none
   */
  void GetTextParam( TextParamId id, gfl2::str::StrBuf* pReturnText ) const;

  /**
   * @brief テキストのパラメータを取得する（STRBUFを生成）
   * @param id  TextParamId enum のどれか
   * @return  gfl::str::StrBuf  テキストを格納したSTRBUFへのポインタ
   */
  gfl2::str::StrBuf* GetTextParam( TextParamId id ) const;

  //! バトル場所取得
  BATTLE_CATEGORY GetBattleCategory() const;
  //! バトルルール取得
  BATTLE_RULE GetBattleRule() const;
  //! レベル補正条件取得
  LEVEL_RANGE_TYPE GetLevelRangeType() const;
  //! 制限時間タイプ取得
  TIME_LIMIT_TYPE GetTimeLimitType() const;
  //! バトル背景取得
  BACKGROUND_TYPE GetBackGroundType() const;
  //! バージョン取得
  u8 GetVersion() const;

  /**
   * @brief パラメータのBitが建っているかどうかを検査
   * @param id    BitParamId enum のどれか
   * @param no    ポケモン番号かアイテム番号か技番号
   * @return BitがOnでtrue
   */
  bool CheckParamBit( BitParamId id, u32 no ) const;

  /**
   * @brief 参加ロムバージョンチェック
   * @param version   チェックするバージョン
   * @return true:参加OK  false:参加NG
   */
  bool CheckParamRom( int version ) const;

  /**
   * @brief 参加言語バージョンチェック
   * @param lang      言語
   * @return true:参加OK  false:参加NG
   */
  bool CheckParamLanguage( int lang ) const;

  /**
   * @brief 必至ポケモンが指定されているか
   * @return trueで指定されている
   */
  bool IsMustPoke() const;

  /**
   * @brief AND条件の必至ポケモン番号取得
   * @param idx   添字
   * @return 必至ポケモン番号
   */
  u16 GetParamMustPokeAND( u8 idx ) const;
  u8  GetParamMustPokeFormAND( u8 idx ) const;

  /**
   * @brief OR条件の必至ポケモン番号取得
   * @param idx   添字
   * @return 必至ポケモン番号
   */
  u16 GetParamMustPokeOR( u8 idx ) const;
  u8  GetParamMustPokeFormOR( u8 idx ) const;

  /**
   * @brief 禁止特定フォルムポケモンが指定されているか
   * @return trueで指定されている
   */
  bool IsSpecificVetoPoke() const;

  /**
   * @brief 禁止特定フォルムポケモンチェック
   * @param monsno   チェックするポケモンのモンスター番号
   * @param formno   チェックするポケモンのフォルム番号
   * @return true:含まれている  false:含まれていない
   */
  bool CheckParamSpecifcVetoPoke( u16 monsno, u8 formno ) const;

  /**
   * @brief 産地限定チェック
   * @param version   チェックするバージョン
   * @return true:参加OK  false:参加NG
   */
  bool CheckParamProducingArea( int version ) const;

  //----------------------------------------------------------
  //----------------------------------------------------------
  // CRCチェック
  bool CheckCrc() const;
  //! CRCを取得
  u32 GetCrc() const;

  //! レーティングバトルかどうか
  bool IsRateBattle() const;

  //! WCSかどうか
  bool IsWcs() const;


  // @fix NMCat[3141]：とくべつなさんかポケモンの表示用関数
  // 特別なポケモンが参加できるか
  bool IsLegendEntryView();

  //----------------------------------------------------------
  //  デバッグ用
  //----------------------------------------------------------
  #if PM_DEBUG
  void DebugPrint();
  void DebugSetValueParam( ValueParamId id, u32 value );
  void DebugSetBoolParam( BoolParamId id, bool flag );
  void DebugSetParamMustPokeAND( u8 idx, u16 pokeNo, u8 pokeForm );
  void DebugSetParamMustPokeOR( u8 idx, u16 pokeNo, u8 pokeForm );
  #else
  void DebugPrint(){}
  void DebugSetValueParam( ValueParamId id, u32 value ){}
  void DebugSetBoolParam( BoolParamId id, bool flag ){}
  void DebugSetParamMustPokeAND( u8 idx, u16 pokeNo, u8 pokeForm ){}
  void DebugSetParamMustPokeOR( u8 idx, u16 pokeNo, u8 pokeForm ){}
  #endif //PM_DEBUG

#if PM_DEBUG
public:
  // for Debug
  static bool m_debugCrcBroken;   //!< CRC不一致デバッグ
#endif


private:

  // メモリ確保
  void AllocWork( const u32 size );

private:
  gfl2::heap::HeapBase*   m_pHeap;              //! レギュレーションバッファ確保用ヒープ
  void*                   m_pRegulationData;    //! レギュレーションデータ
  u32                     m_regulationDataSize; //! レギュレーションデータサイズ
  CoreData*               m_pCoreData;          //! 構造体データ取得用
};

//GFL_NAMESPACE_END(Battle)

#endif // __REGULATION_H_INCLUDED__
