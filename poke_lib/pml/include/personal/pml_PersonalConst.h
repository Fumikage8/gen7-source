//=============================================================================
/**
 * @file   pml_PersonalConst.h
 * @brief  ポケモンパーソナルデータ
 * @author obata_toshihiro
 * @date   2010.12.20
 */
//=============================================================================
#ifndef __PML_PERSONALCONST_H__
#define __PML_PERSONALCONST_H__
#pragma once

#include <types/include/gfl2_Typedef.h>

namespace pml { 
  namespace personal {


    /**
     * @brief ポケモンパーソナルデータを取得するためのインデックス
     */
    enum ParamID{ 
      PARAM_ID_basic_hp,             ///< 基本ＨＰ
      PARAM_ID_basic_atk,            ///< 基本攻撃力
      PARAM_ID_basic_def,            ///< 基本防御力
      PARAM_ID_basic_agi,            ///< 基本素早さ
      PARAM_ID_basic_spatk,          ///< 基本特殊攻撃力
      PARAM_ID_basic_spdef,          ///< 基本特殊防御力
      PARAM_ID_type1,                ///< 属性1
      PARAM_ID_type2,                ///< 属性2
      PARAM_ID_get_rate,             ///< 捕獲率
      PARAM_ID_give_exp,             ///< 贈与経験値
      PARAM_ID_pains_hp,             ///< 贈与努力値ＨＰ
      PARAM_ID_pains_atk,            ///< 贈与努力値攻撃力
      PARAM_ID_pains_def,            ///< 贈与努力値防御力
      PARAM_ID_pains_agi,            ///< 贈与努力値素早さ
      PARAM_ID_pains_spatk,          ///< 贈与努力値特殊攻撃力
      PARAM_ID_pains_spdef,          ///< 贈与努力値特殊防御力
      PARAM_ID_no_jump,              ///< 浮くの禁止フラグ
      PARAM_ID_item1,                ///< アイテム1
      PARAM_ID_item2,                ///< アイテム2
      PARAM_ID_item3,                ///< アイテム3
      PARAM_ID_sex,                  ///< 性別ベクトル
      PARAM_ID_egg_birth,            ///< タマゴの孵化歩数
      PARAM_ID_initial_familiarity,  ///< なつき度初期値
      PARAM_ID_grow,                 ///< 成長曲線識別
      PARAM_ID_egg_group1,           ///< こづくりグループ1
      PARAM_ID_egg_group2,           ///< こづくりグループ2
      PARAM_ID_tokusei1,             ///< 特殊能力1
      PARAM_ID_tokusei2,             ///< 特殊能力2
      PARAM_ID_tokusei3,             ///< 特殊能力3
      PARAM_ID_form_index,           ///< 別フォルムパーソナルデータ開始位置
      PARAM_ID_form_gra_index,       ///< 別フォルムグラフィックデータ開始位置
      PARAM_ID_form_max,             ///< 別フォルムMAX
      PARAM_ID_color,                ///< 色（図鑑で使用）
      PARAM_ID_rank,                 ///< ポケモンランク
      PARAM_ID_height,               ///< 高さ[cm]
      PARAM_ID_weight,               ///< 重さ[100g単位]
      PARAM_ID_machine1,             ///< 技マシンフラグ1
      PARAM_ID_machine2,             ///< 技マシンフラグ2
      PARAM_ID_machine3,             ///< 技マシンフラグ3
      PARAM_ID_machine4,             ///< 技マシンフラグ4
      PARAM_ID_waza_oshie_kyukyoku,  ///< 技教え究極
      PARAM_ID_waza_oshie_momiji1,   ///< 技教え(momiji追加分1)
      PARAM_ID_waza_oshie_momiji2,   ///< 技教え(momiji追加分2)
      PARAM_ID_waza_oshie_momiji3,   ///< 技教え(momiji追加分3)
      PARAM_ID_sos_rate,             ///< 助けを呼ぶ割合
      PARAM_ID_zenryoku_waza_item,   ///< ポケモン専用全力技を使用するための道具No.
      PARAM_ID_zenryoku_waza_before, ///< ポケモン専用全力技に変化する技の技No.
      PARAM_ID_zenryoku_waza_after,  ///< ポケモン専用全力技の技No.
      PARAM_ID_region,               ///< 地方種ID
    };

    /**
     * @brief 表示上のサイズ
     */
    enum DispSize {
      DISP_SIZE_S = 0,
      DISP_SIZE_M = 1,
      DISP_SIZE_L = 2,
    };

    /**
     * @brief 性別ベクトル ( パーソナル上のオスメス比率 ) 
     *
     * 1〜253 でオスメスの比率が変わる
     */
    enum SexVector {
      SEX_VECTOR_ONLY_MALE         = 0,                      ///< オスのみ
      SEX_VECTOR_RANDOM_MIN        = 1,                      ///< ここからランダム
      SEX_VECTOR_RANDOM_MAX        = 253,                    ///< ここまでランダム
      SEX_VECTOR_ONLY_FEMALE       = 254,                    ///< メスのみ
      SEX_VECTOR_UNKNOWN           = 255,                    ///< 性別なし
      SEX_VECTOR_RANDOM_FEMALE_MIN = SEX_VECTOR_RANDOM_MIN,  ///< ここからメス
      SEX_VECTOR_RANDOM_MALE_MAX   = SEX_VECTOR_RANDOM_MAX,  ///< ここまでオス
    };

    /**
     * @brief 性別ベクトルのタイプ ( オスメスランダム or 固定 )
     */
    enum SexType {
      SEX_TYPE_RANDOM, ///< ランダム
      SEX_TYPE_FIX,    ///< 固定
    };

    /**
     * @brief パーソナルデータの色定義
     */
    enum Color {
      COLOR_RED,      ///< 赤
      COLOR_BLUE,     ///< 青
      COLOR_YELLOW,   ///< 黄
      COLOR_GREEN,    ///< 緑
      COLOR_BLACK,    ///< 黒
      COLOR_BROWN,    ///< 茶
      COLOR_PERPLE,   ///< 紫
      COLOR_GRAY,     ///< 灰
      COLOR_WHITE,    ///< 白
      COLOR_PINK,     ///< 桃
      COLOR_MAX,      ///< 色定義の数
    };

    /**
     * @brief こづくりグループ
     */
    enum EggGroup {
      EGG_GROUP_NONE,        ///<  0: なし(この値は不正)
      EGG_GROUP_KAIJUU,      ///<  1: 怪獣
      EGG_GROUP_SUISEI,      ///<  2: 水棲
      EGG_GROUP_MUSI,        ///<  3: 虫
      EGG_GROUP_TORI,        ///<  4: 鳥
      EGG_GROUP_DOUBUTU,     ///<  5: 動物
      EGG_GROUP_YOUSEI,      ///<  6: 妖精
      EGG_GROUP_SYOKUBUTU,   ///<  7: 植物
      EGG_GROUP_HITOGATA,    ///<  8: 人型
      EGG_GROUP_KOUKAKU,     ///<  9: 甲殻
      EGG_GROUP_MUKIBUTU,    ///< 10: 無機物
      EGG_GROUP_FUTEIKEI,    ///< 11: 不定形
      EGG_GROUP_SAKANA,      ///< 12: 魚
      EGG_GROUP_METAMON,     ///< 13: メタモン
      EGG_GROUP_DORAGON,     ///< 14: ドラゴン
      EGG_GROUP_MUSEISYOKU,  ///< 15: 無生殖
    };

    /**
     * @brief 進化するための条件
     * @caution コンバータ側の定義と一致させる必要がある
     */
    enum EvolveCond { 
      EVOLVE_COND_NONE,                     ///< 進化しない
      EVOLVE_COND_FAMILIARITY_HIGH,         ///< なつき度が高い状態でレベルアップ
      EVOLVE_COND_FAMILIARITY_HIGH_NOON,    ///< なつき度が高い状態で朝昼にレベルアップ
      EVOLVE_COND_FAMILIARITY_HIGH_NIGHT,   ///< なつき度が高い状態で夜にレベルアップ
      EVOLVE_COND_LEVELUP,                  ///< レベルアップ
      EVOLVE_COND_TUUSHIN,                  ///< 通信交換
      EVOLVE_COND_TUUSHIN_ITEM,             ///< 特定のアイテムを持った状態で通信交換
      EVOLVE_COND_TUUSHIN_YUUGOU,           ///< 特定の組み合わせで通信交換
      EVOLVE_COND_ITEM,                     ///< 特定のアイテムを使用する
      EVOLVE_COND_SPECIAL_POW,              ///< 「こうげき」>「ぼうぎょ」の状態でレベルアップ
      EVOLVE_COND_SPECIAL_EVEN,             ///< 「こうげき」=「ぼうぎょ」の状態でレベルアップ
      EVOLVE_COND_SPECIAL_DEF,              ///< 「こうげき」<「ぼうぎょ」の状態でレベルアップ
      EVOLVE_COND_SPECIAL_RND_EVEN,         ///< 個性乱数が偶数の状態でレベルアップ
      EVOLVE_COND_SPECIAL_RND_ODD,          ///< 個性乱数が奇数の状態でレベルアップ
      EVOLVE_COND_SPECIAL_LEVELUP,          ///< 特殊　レベルアップ (ツチニン→テッカニン専用)
      EVOLVE_COND_SPECIAL_NUKENIN,          ///< 特殊　手持ちに空きがある (ツチニン→ヌケニン専用)
      EVOLVE_COND_SPECIAL_BEAUTIFUL,        ///< 「うつくしさ」が一定値以上の状態でレベルアップ
      EVOLVE_COND_ITEM_MALE,                ///< ♂に特定のアイテムを使用する
      EVOLVE_COND_ITEM_FEMALE,              ///< ♀に特定のアイテムを使用する
      EVOLVE_COND_SOUBI_NOON,               ///< 特定のアイテムを持たせた状態で朝昼にレベルアップ
      EVOLVE_COND_SOUBI_NIGHT,              ///< 特定のアイテムを持たせた状態で夜にレベルアップ
      EVOLVE_COND_WAZA,                     ///< 特定の技を習得した状態でレベルアップ
      EVOLVE_COND_POKEMON,                  ///< 手持ちに特定のポケモンがいる状態でレベルアップ
      EVOLVE_COND_MALE,                     ///< ♂がレベルアップ
      EVOLVE_COND_FEMALE,                   ///< ♀がレベルアップ
      EVOLVE_COND_PLACE_MAGNETIC,           ///< 「特殊磁場」でレベルアップ
      EVOLVE_COND_PLACE_KOKE,               ///< 「苔生した岩」付近でレベルアップ
      EVOLVE_COND_PLACE_ICE,                ///< 「凍結した岩」付近でレベルアップ
      EVOLVE_COND_DEVICE_TURNOVER,          ///< 3DSをひっくり返している状態でレベルアップ
      EVOLVE_COND_FRIENDLY_WAZATYPE,        ///< なかよし度が高い状態で特定タイプの技を持ってレベルアップ
      EVOLVE_COND_WITH_POKETYPE_AKU,        ///< 悪タイプのポケモンと一緒にいるときにレベルアップ
      EVOLVE_COND_WEATHER_RAIN,             ///< 天候が雨のときにレベルアップ
      EVOLVE_COND_NOON,                     ///< 朝昼にレベルアップ
      EVOLVE_COND_NIGHT,                    ///< 夜にレベルアップ
      EVOLVE_COND_FEMALE_FORMCHANGE,        ///< ♀がレベルアップでフォルムも変更
      EVOLVE_COND_FRIENDLY,                 ///< なかよし度が高い状態でレベルアップ
      EVOLVE_COND_CASSETTE_VERSION,         ///< 特定のカセットバージョンでレベルアップ
      EVOLVE_COND_CASSETTE_VERSION_NOON,    ///< 特定のカセットバージョンで、朝昼にレベルアップ
      EVOLVE_COND_CASSETTE_VERSION_NIGHT,   ///< 特定のカセットバージョンで、夜にレベルアップ
      EVOLVE_COND_PLACE_SNOW_MOUNTAIN,      ///< 「雪山」でレベルアップ
      EVOLVE_COND_EVENING,                  ///< 夕方にレベルアップ
      EVOLVE_COND_PLACE_ULTRA_SPACE,        ///< 「ＵＳ」でレベルアップ
      EVOLVE_COND_PLACE_ULTRA_SPACE_ITEM,   ///< 「ＵＳ」で特定のアイテムを使用する
    };

    /**
     * @brief メガ進化するための条件
     */
    enum MegaEvolveCond {
      MEGA_EVOLVE_COND_NONE,     ///< メガ進化しない
      MEGA_EVOLVE_COND_ITEM,     ///< 道具を所持
      MEGA_EVOLVE_COND_REKKUUZA, ///< レックウザ特殊
    };

    /**
     * @brief  保持特性のパターン
     * @note  仕様としてこの3パターン以外存在しない
     */
    enum TokuseiPattern {
      TOKUSEI_PATTERN_AAA = 0,  ///<１，２，３すべて同じ
      TOKUSEI_PATTERN_AAB,      ///<１、２同じ、第3特性は別
      TOKUSEI_PATTERN_ABC,      ///<１、２、３すべて別
    };
    
    /**
     * @brief アイテムスロット数
     */
    static const u8 ITEM_SLOT_NUM = 3;


    /**
     * @brief パーソナルデータ「助けを呼ぶ割合」の値域
     */
    static const u8 SOS_RATE_MIN   = 0;                               // 最小値
    static const u8 SOS_RATE_MAX   = 9;                               // 最大値
    static const u8 SOS_RATE_RANGE = SOS_RATE_MAX - SOS_RATE_MIN + 1; // 幅


    /**
     * @brief 地方種ID
     */
    enum RegionID
    {
      REGION_NORMAL,  // 既存種
      REGION_ALOLA,   // アローラ種
      REGION_NUM,
    };


  } // namespace personal
} // namespace pml

#endif //__PML_PERSONALCONST_H__
