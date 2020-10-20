//=======================================================================================
/**
 * @brief  ポケモンパラメータで使用する型定義
 * @file   pml_PokemonParamType.h
 * @author tamada
 * @date   2012.06.10
 */
//=======================================================================================
#ifndef __PML_POKEMONPARAMTYPE_H__
#define __PML_POKEMONPARAMTYPE_H__
#pragma once
#include <types/include/gfl2_Typedef.h>

#include <pml/include/pml_Const.h>
#include <pml/include/pml_Waza.h>

///
namespace pml {

  ///@namespace pml::pokepara
  namespace pokepara {


    //------------------------------------------------------------------------
    /**
     * @brief 能力値
     */
    //------------------------------------------------------------------------
    enum PowerID {
      POWER_HP,     // 最大HP
      POWER_ATK,    // こうげき
      POWER_DEF,    // ぼうぎょ
      POWER_SPATK,  // とくこう
      POWER_SPDEF,  // とくぼう
      POWER_AGI,    // すばやさ
      POWER_NUM,
    };

    //------------------------------------------------------------------------
    /**
     * @brief コンディション
     * @note  現在使用予定はない
     */
    //------------------------------------------------------------------------
    enum Condition {
      CONDITION_STYLE,      // かっこよさ
      CONDITION_BEAUTIFUL,  // うつくしさ
      CONDITION_CUTE,       // かわいさ
      CONDITION_CLEVER,     // かしこさ
      CONDITION_STRONG,     // たくましさ
      CONDITION_FUR,        // 毛艶
    };

    //------------------------------------------------------------------------
    /**
     * @brief タマゴのチェック
     */
    //------------------------------------------------------------------------
    enum EggCheckType {
      CHECK_ONLY_LEGAL_EGG,    // ( 正規タマゴ && !不正タマゴ ) かどうか
      CHECK_ONLY_ILLEGAL_EGG,  // ( 不正タマゴ ) かどうか
      CHECK_BOTH_EGG,          // ( 正規タマゴ || 不正タマゴ ) かどうか
    };

    //------------------------------------------------------------------------
    /**
     * @brief 技覚えの結果
     */
    //------------------------------------------------------------------------
    enum WazaLearningResult {
      WAZA_LEARNING_SUCCEEDED,         // 成功：新しい技を覚えた
      WAZA_LEARNING_FAILED_FULL,       // 失敗：手持ち技がいっぱいで覚えられなかった
      WAZA_LEARNING_FAILED_SAME,       // 失敗：すでに覚えている
      WAZA_LEARNING_FAILED_NOT_EXIST,  // 失敗：覚える技が無かった
    };

    //------------------------------------------------------------------------
    /**
     * @brief 思い出データ
     */
    //------------------------------------------------------------------------
    enum Memories {
      MEMORIES_EGG_TAKEN_YEAR,       // タマゴをもらった年
      MEMORIES_EGG_TAKEN_MONTH,      // タマゴをもらった月
      MEMORIES_EGG_TAKEN_DAY,        // タマゴをもらった日
      MEMORIES_EGG_TAKEN_PLACE,      // タマゴをもらった場所
      MEMORIES_FIRST_CONTACT_YEAR,   // タマゴが孵化した or 捕獲した年
      MEMORIES_FIRST_CONTACT_MONTH,  // タマゴが孵化した or 捕獲した月
      MEMORIES_FIRST_CONTACT_DAY,    // タマゴが孵化した or 捕獲した日
      MEMORIES_FIRST_CONTACT_PLACE,  // タマゴが孵化した or 捕獲した場所
      MEMORIES_CAPTURED_BALL,        // 捕まえたボール
      MEMORIES_CAPTURED_LEVEL,       // 捕まえたレベル
      MEMORIES_LEVEL_WITH_PARENT,    // 親との思い出レベル
      MEMORIES_CODE_WITH_PARENT,     // 親との思い出事象コード
      MEMORIES_DATA_WITH_PARENT,     // 親との思い出内容データ
      MEMORIES_FEEL_WITH_PARENT,     // 親との思い出気持ちデータ
      MEMORIES_LEVEL_WITH_OTHERS,    // 親以外のトレーナーとの思い出レベル
      MEMORIES_CODE_WITH_OTHERS,     // 親以外のトレーナーとの思い出事象コード
      MEMORIES_DATA_WITH_OTHERS,     // 親以外のトレーナーとの思い出内容データ
      MEMORIES_FEEL_WITH_OTHERS,     // 親以外のトレーナーとの思い出気持ちデータ
    };

    //------------------------------------------------------------------------
    /**
     * @brief ポケモンにつけるボックスマークの種類
     */
    //------------------------------------------------------------------------
    enum BoxMark {
      BOX_MARK_1,
      BOX_MARK_2,
      BOX_MARK_3,
      BOX_MARK_4,
      BOX_MARK_5,
      BOX_MARK_6,
      BOX_MARK_NUM,  // 総数
    };

    //------------------------------------------------------------------------
    /**
     * @brief ポケモンにつけるボックスマークの色
     */
    //------------------------------------------------------------------------
    enum BoxMarkColor
    {
      BOX_MARK_COLOR_NONE,  // 色なし( デフォルト )
      BOX_MARK_COLOR_BLUE,  // 青
      BOX_MARK_COLOR_RED,   // 赤
      BOX_MARK_COLOR_NUM,   // 総数
    };

    //------------------------------------------------------------------------
    /**
     * @brief ポケモン１体分のボックスマークを扱う構造体
     */
    //------------------------------------------------------------------------
    struct BoxMarkContainer
    {
      BoxMarkColor markColor[ BOX_MARK_NUM ];  // 各マークの色
    };

    //------------------------------------------------------------------------
    /**
     * @brief 状態異常
     *
     * @note ここに列挙されている状態異常は, 以下の条件を満たす
     * (1) フィールドでもバトルでもその状態に成りうる
     * (2) 相互の重複は起こらない
     */
    //------------------------------------------------------------------------
    enum Sick {
      SICK_NULL   = wazadata::WAZASICK_NULL,    ///< 異常なし
      SICK_MAHI   = wazadata::WAZASICK_MAHI,    ///< まひ
      SICK_NEMURI = wazadata::WAZASICK_NEMURI,  ///< ねむり
      SICK_KOORI  = wazadata::WAZASICK_KOORI,   ///< こおり
      SICK_YAKEDO = wazadata::WAZASICK_YAKEDO,  ///< やけど
      SICK_DOKU   = wazadata::WAZASICK_DOKU,    ///< どく
      SICK_MAX,
    };

    //------------------------------------------------------------------------
    /**
     * @brief ポケモンが好む(好まない)味
     * @note  現在使用予定はない
     */
    //------------------------------------------------------------------------
    enum Taste {
      TASTE_KARAI,  // 辛い
      TASTE_SIBUI,  // 渋い
      TASTE_AMAI,   // 甘い
      TASTE_NIGAI,  // 苦い
      TASTE_SUPPAI, // すっぱい
      TASTE_MAX,
    };

    
    //------------------------------------------------------------------------
    /**
     * @brief ポケモンリゾートの各ポケモンのイベント状態
     * @note  niji
     */
    //------------------------------------------------------------------------
    enum ResortEventState {
 /*     RESORT_EVENT_NONE,
      RESORT_EVENT_SEIKAKU,  //せいかく
      RESORT_EVENT_RESOLUTION,      //解決
      RESORT_EVENT_QUARREL,         //けんか
      RESORT_EVENT_CARE,            //おせわ
      RESORT_EVENT_PALRE,           //パルレ
      RESORT_EVENT_GOOD_FRIEND,     //グッドフレンド
      RESORT_EVENT_POKEMAME_GET,    //ポケマメGET
      RESORT_EVENT_GIM,             //運動希望
      RESORT_EVENT_HOTSPA,          //おんせん
      RESORT_EVENT_WILD,          //やせい
			RESORT_EVENT_FRIEND,     //仲間になる*/
  
//ここから新定義
      RESORTEVENT_NONE =0,
      RESORTEVENT_SEIKAKU,  //性格表示     
      RESORTEVENT_CARE,      //かまってほしい
      RESORTEVENT_LIKE_RESORT,         //リゾート好き
      RESORTEVENT_LIKE_BATTLE,            //バトル好き
      RESORTEVENT_LIKE_ADV,           //冒険好き
      RESORTEVENT_GOOD_FRIEND,     //いちゃいちゃ状態
      RESORTEVENT_GIM,             //運動希望
      RESORTEVENT_HOTSPA,          //温泉希望
      RESORTEVENT_WILD,          //野生出現
      RESORTEVENT_WILD_LOVE,       //野生問題解決状態
      RESORTEVENT_WILD_LIVE,     // 野生が住んでいる  ==仲間になる
      RESORTEVENT_POKEMAME_GET1,    //ポケマメGET1
      RESORTEVENT_POKEMAME_GET2,    //ポケマメGET2
      RESORTEVENT_POKEMAME_GET3,    //ポケマメGET3
      RESORTEVENT_KINOMI_HELP,      // 15: きのみ栽培お手伝い
      RESORTEVENT_PLAY_STATE,       // 16: 遊びの様子
      RESORTEVENT_HOTSPA_STATE,     // 17: おんせんの様子
      RESORTEVENT_HOTSPA_DIZZY,     // 18: のぼせ
			RESORTEVENT_HOTSPA_EGG_HATCHING, //19:タマゴが孵化直前

      RESORT_EVENT_MAX,        //
    };

    //------------------------------------------------------------------------
    /**
     * @brief 味に対する好き嫌い
     * @note  現在使用予定はない
     */
    //------------------------------------------------------------------------
    enum TasteJudge {
      TASTE_JUDGE_NORMAL,   // 普通
      TASTE_JUDGE_LIKE,     // 好き
      TASTE_JUDGE_DISLIKE,  // 嫌い
    };

    //------------------------------------------------------------------------
    /**
     * @brief ポケモン同士の相性
     */
    //------------------------------------------------------------------------
    enum LoveLevel {
      LOVE_LV_GOOD,    ///< 育て屋「とっても　よい」
      LOVE_LV_NORMAL,  ///< 育て屋「まずまず」
      LOVE_LV_BAD,     ///< 育て屋「それほど　よくない」
      LOVE_LV_WORST,   ///< 育て屋「ちがう　ポケモンたちと　あそんでるよ」
    };

    //------------------------------------------------------------------------
    /**
     * @brief まとめリボンの種類指定
     */
    //------------------------------------------------------------------------
    enum LumpingRibbon {
      LUMPING_RIBBON_A,     ///<まとめリボンA
      LUMPING_RIBBON_B,     ///<まとめリボンB
      LUMPING_RIBBON_NUM,   ///<まとめリボン種類数
    };

    //------------------------------------------------------------------------
    /**
     * @brief 親かどうかを識別するための構造体
     */
    //------------------------------------------------------------------------
    struct OwnerInfo {
      u32 trainer_id;
      u8 sex;
      STRCODE name[ PERSON_NAME_BUFFER_SIZE ];
    };

    //------------------------------------------------------------------------
    /**
     * @brief 進化条件を渡すための構造体
     */
    //------------------------------------------------------------------------
    struct EvolveSituation {
      bool is_magnetic_field;         ///<進化条件：場所：磁場(ジバコイル、ダイノーズ）
      bool is_ice_field;              ///<進化条件：場所：氷(グレイシア）
      bool is_moss_field;             ///<進化条件：場所：植物(リーフィア）
      bool is_snow_mountain;          ///<進化条件：場所：雪山( ヤシガニ )
      bool is_ultra_space;            ///<進化条件：場所：ウルトラスペース( ピカチュウ、タマタマ、カラカラ )
      bool is_morning_or_noon;        ///<進化条件：時間帯：朝昼
      bool is_night;                  ///<進化条件：時間帯：夜
      bool is_evening;                ///<進化条件：時間帯：夕方
      bool is_device_turned_over;     ///<進化条件：3DSをひっくり返している
      bool is_rain;                   ///<進化条件：天候：雨
      OwnerInfo owner_info;           ///<なつき度進化判定用、現在の親の情報
    };

    //------------------------------------------------------------------------
    /**
     * @brief 汚れの種類
     */
    //------------------------------------------------------------------------
    enum DirtType
    {
      DIRT_TYPE_NONE,    // 汚れなし
      DIRT_TYPE_HAIR,    // みだれげ
      DIRT_TYPE_STAIN,   // よごれ
      DIRT_TYPE_SAND,    // すなぼこり
      DIRT_TYPE_WATER,   // みずぬれ
      DIRT_TYPE_NUM,     // 種類数
    };



  } //namespace pokepara
} //namespace pml {

#endif  // __PML_POKEMONPARAMTYPE_H__

