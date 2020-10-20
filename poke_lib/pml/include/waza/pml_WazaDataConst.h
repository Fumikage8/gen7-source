//=============================================================================
/**
 * @brief 技データ定数
 * @file pml_WazaDataConst.h
 * @author obata_toshihiro
 * @date 2011.05.06
 */
//=============================================================================
#ifndef __PML_WAZADATACONST_H__
#define __PML_WAZADATACONST_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include "./pml_WazaTarget.h"

namespace pml {
  namespace wazadata {


    enum {
      RANK_STORE_MAX = 3,
      HITRATIO_MUST = 101,  ///< hitPer（命中率）がこの値なら、必中ワザ
      CRITICAL_MUST = 6,    ///< criticalRank がこの値なら、必ずクリティカル
    };

    //-------------------------------------------------------------------------
    /**
     * @brief ワザのカテゴリ最上位分類
     */
    //-------------------------------------------------------------------------
    enum WazaCategory {
      CATEGORY_SIMPLE_DAMAGE,
      CATEGORY_SIMPLE_SICK,
      CATEGORY_SIMPLE_EFFECT,
      CATEGORY_SIMPLE_RECOVER,
      CATEGORY_DAMAGE_SICK,
      CATEGORY_EFFECT_SICK,
      CATEGORY_DAMAGE_EFFECT,
      CATEGORY_DAMAGE_EFFECT_USER,
      CATEGORY_DRAIN,
      CATEGORY_ICHIGEKI,
      CATEGORY_FIELD_EFFECT,
      CATEGORY_SIDE_EFFECT,
      CATEGORY_PUSHOUT,
      CATEGORY_OTHERS,
      CATEGORY_MAX,
    };

    //-------------------------------------------------------------------------
    /**
     * @brief ワザのダメージ種類
     */
    //-------------------------------------------------------------------------
    enum WazaDamageType {
      DMG_NONE = 0,
      DMG_PHYSIC,  ///< 物理ダメージ
      DMG_SPECIAL, ///< 特殊ダメージ
      DMG_MAX,
    };

    //-------------------------------------------------------------------------
    /**
     * @brief 状態異常の継続タイプ分類
     */
    //-------------------------------------------------------------------------
    enum WazaSickCont {
      SICKCONT_NONE = 0,
      SICKCONT_PERMANENT,  ///< ずっと継続
      SICKCONT_TURN,       ///< ターン数継続
      SICKCONT_POKE,       ///< ワザをかけたポケモンが居る間、継続
      SICKCONT_POKETURN,   ///< ターン数経過か＆ポケモンが場から消えるまでを両方チェック
    };

    //-------------------------------------------------------------------------
    /**
     * @brief 状態異常の継続パラメータ
     */
    //-------------------------------------------------------------------------
    struct SICKCONT_PARAM
    {
      union
      {
        u16 raw;

        struct {
          u16  type    : 4;   ///< 継続タイプ（ WazaSickCont ）
          u16  turnMin : 6;   ///< 継続ターン最小
          u16  turnMax : 6;   ///< 継続ターン最大
        };
      };
    };

    //-------------------------------------------------------------------------
    /**
     * @brief ワザによって発生する追加効果（攻撃等のランクアップ・ダウン効果）
     */
    //-------------------------------------------------------------------------
    enum WazaRankEffect {

      RANKEFF_NULL = 0,
      RANKEFF_ORIGIN = 1,

      RANKEFF_ATTACK = RANKEFF_ORIGIN,
      RANKEFF_DEFENCE,
      RANKEFF_SP_ATTACK,
      RANKEFF_SP_DEFENCE,
      RANKEFF_AGILITY,
      RANKEFF_HIT,
      RANKEFF_AVOID,

      RANKEFF_MAX,
      RANKEFF_BASE_MAX = RANKEFF_HIT,
      RANKEFF_NUMS = RANKEFF_MAX - RANKEFF_ORIGIN,


      RANKEFF_SP = RANKEFF_MAX,
      RANKEFF_CRITICAL_RATIO,
      RANKEFF_MULTI5,  ///< こうげき、とくこう、防御、特防、素早さの５つ同時指定

    };

    //-------------------------------------------------------------------------
    /**
     * @brief ワザ優先度
     */
    //-------------------------------------------------------------------------
    enum WazaPriority {
      WAZAPRI_MAX = 5,
      WAZAPRI_MIN = -7,
      WAZAPRI_RANGE = (WAZAPRI_MAX - WAZAPRI_MIN + 1),
    };

    //-------------------------------------------------------------------------
    /**
     * @brief ワザで発生する天候効果
     */
    //-------------------------------------------------------------------------
    enum WazaWeather {
      WAZA_WEATHER_NONE = 0,   ///< 天候なし
      WAZA_WEATHER_SHINE,    ///< はれ
      WAZA_WEATHER_RAIN,     ///< あめ
      WAZA_WEATHER_SNOW,     ///< あられ
      WAZA_WEATHER_SAND,     ///< すなあらし
      WAZA_WEATHER_MIST,     ///< きり
      WAZA_WEATHER_MAX,
    };

    //-------------------------------------------------------------------------
    /**
     * @brief 全力技の追加効果
     */
    //-------------------------------------------------------------------------
    enum ZenryokuWazaEffect {
      ZENRYOKUWAZA_EFFECT_NONE,               // なし
      ZENRYOKUWAZA_EFFECT_ATTACK_UP1,         // 攻撃↑
      ZENRYOKUWAZA_EFFECT_ATTACK_UP2,         // 攻撃↑↑
      ZENRYOKUWAZA_EFFECT_ATTACK_UP3,         // 攻撃↑↑↑
      ZENRYOKUWAZA_EFFECT_DEFENSE_UP1,        // 防御↑
      ZENRYOKUWAZA_EFFECT_DEFENSE_UP2,        // 防御↑↑
      ZENRYOKUWAZA_EFFECT_DEFENSE_UP3,        // 防御↑↑↑
      ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP1,      // 特攻↑
      ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP2,      // 特攻↑↑
      ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP3,      // 特攻↑↑↑
      ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP1,     // 特防↑
      ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP2,     // 特防↑↑
      ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP3,     // 特防↑↑↑
      ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP1,     // 素早↑
      ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP2,     // 素早↑↑
      ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP3,     // 素早↑↑↑
      ZENRYOKUWAZA_EFFECT_SP_HIT_UP1,         // 命中↑
      ZENRYOKUWAZA_EFFECT_SP_HIT_UP2,         // 命中↑↑
      ZENRYOKUWAZA_EFFECT_SP_HIT_UP3,         // 命中↑↑↑
      ZENRYOKUWAZA_EFFECT_SP_AVOID_UP1,       // 回避↑
      ZENRYOKUWAZA_EFFECT_SP_AVOID_UP2,       // 回避↑↑
      ZENRYOKUWAZA_EFFECT_SP_AVOID_UP3,       // 回避↑↑↑
      ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP1,    // 攻撃防御特攻特防素早さ↑
      ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP2,    // 攻撃防御特攻特防素早さ↑↑
      ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP3,    // 攻撃防御特攻特防素早さ↑↑↑
      ZENRYOKUWAZA_EFFECT_CRITICAL_RATIO_UP1, // 急所↑
      ZENRYOKUWAZA_EFFECT_RESET_RANK_DOWN,    // 能力↓リセット
      ZENRYOKUWAZA_EFFECT_RECOVER_HP,         // 自分のHP回復
      ZENRYOKUWAZA_EFFECT_RECOVER_HP_POS,     // 次に場に出たポケモンのHPを回復
      ZENRYOKUWAZA_EFFECT_KONOYUBITOMARE,     // このゆびとまれ
      ZENRYOKUWAZA_EFFECT_BY_POKETYPE,        // 使用するポケモンのタイプで変化する( のろい )
      ZENRYOKUWAZA_EFFECT_NUM,
    };

    //-------------------------------------------------------------------------
    /**
     * @brief 各種フラグ
     */
    //-------------------------------------------------------------------------
    enum WazaFlag {
      WAZAFLAG_Touch = 0,     ///< 接触ワザ
      WAZAFLAG_Tame,          ///< １ターン溜めワザ
      WAZAFLAG_Tire,          ///< １ターン反動ワザ
      WAZAFLAG_Mamoru,        ///< 「まもる」でガード対象となるワザ
      WAZAFLAG_MagicCoat,     ///< 「マジックコート」対象
      WAZAFLAG_Yokodori,      ///< 「よこどり」対象
      WAZAFLAG_Oumu,          ///< 「おうむがえし」対象
      WAZAFLAG_Punch,         ///< 「てつのこぶし」対象（パンチのイメージ）
      WAZAFLAG_Sound,         ///< 「ぼうおん」対象（音による攻撃・効果のイメージ）
      WAZAFLAG_Flying,        ///< 重力時に失敗する（飛んだり浮いたりイメージ）
      WAZAFLAG_KooriMelt,     ///< 自分が凍りでも解除してワザを出す（高温イメージ）
      WAZAFLAG_TripleFar,     ///< トリプルバトルで、離れた相手も選択出来る
      WAZAFLAG_KaifukuHuuji,  ///< かいふく封じで封じられる
      WAZAFLAG_MigawariThru,  ///< 相手がみがわりでも当たる（ダメージワザ以外の判定に）
      WAZAFLAG_SkyBattleFail, ///< スカイバトルで失敗する
      WAZAFLAG_EffectToFriend,///< 味方対象のワザエフェクトを出す（効果範囲=通常ポケの場合に参照する）
      WAZAFLAG_Dance,         ///< 踊りワザ
      WAZAFLAG_MAX,
    };

    //-------------------------------------------------------------------------
    /**
     * @brief ワザ関連定数
     */
    //-------------------------------------------------------------------------
    enum {
      RANKEFF_NUM_MAX = 3,   ///< １つのワザに割り当てられるランク効果の種類数最大値
    };

    //-------------------------------------------------------------------------
    /**
     * @brief ワザパラメータ種類
     */
    //-------------------------------------------------------------------------
    enum ParamID {
      PARAM_ID_TYPE,                   ///< タイプ（みず、くさ、ほのお…）
      PARAM_ID_CATEGORY,               ///< カテゴリ（ enum WazaCategory )
      PARAM_ID_DAMAGE_TYPE,            ///< ダメージタイプ（ enum WazaDamageType )
      PARAM_ID_POWER,                  ///< 威力
      PARAM_ID_HITPER,                 ///< 命中率
      PARAM_ID_BASE_PP,                ///< 基本PPMax
      PARAM_ID_PRIORITY,               ///< ワザ優先度
      PARAM_ID_CRITICAL_RANK,          ///< クリティカルランク
      PARAM_ID_HITCOUNT_MAX,           ///< 最大ヒット回数
      PARAM_ID_HITCOUNT_MIN,           ///< 最小ヒット回数
      PARAM_ID_SHRINK_PER,             ///< ひるみ確率
      PARAM_ID_SICK,                   ///< 状態異常コード（enum WazaSick）
      PARAM_ID_SICK_PER,               ///< 状態異常の発生率
      PARAM_ID_SICK_CONT,              ///< 状態異常継続パターン（enum WazaSickCont）
      PARAM_ID_SICK_TURN_MIN,          ///< 状態異常の継続ターン最小
      PARAM_ID_SICK_TURN_MAX,          ///< 状態異常の継続ターン最大
      PARAM_ID_RANKTYPE_1,             ///< ステータスランク効果１
      PARAM_ID_RANKTYPE_2,             ///< ステータスランク効果２
      PARAM_ID_RANKTYPE_3,             ///< ステータスランク効果３
      PARAM_ID_RANKVALUE_1,            ///< ステータスランク効果１増減値
      PARAM_ID_RANKVALUE_2,            ///< ステータスランク効果２増減値
      PARAM_ID_RANKVALUE_3,            ///< ステータスランク効果３増減値
      PARAM_ID_RANKPER_1,              ///< ステータスランク効果１の発生率
      PARAM_ID_RANKPER_2,              ///< ステータスランク効果２の発生率
      PARAM_ID_RANKPER_3,              ///< ステータスランク効果３の発生率
      PARAM_ID_DAMAGE_RECOVER_RATIO,   ///< ダメージ回復率
      PARAM_ID_HP_RECOVER_RATIO,       ///< HP回復率
      PARAM_ID_TARGET,                 ///< ワザ効果範囲( enum WazaTarget )
      PARAM_ID_AI_SEQNO,               ///< AI用シーケンスナンバー
      PARAM_ID_DAMAGE_REACTION_RATIO,  ///< ダメージ反動率
      PARAM_ID_HP_REACTION_RATIO,      ///< HP反動率
      PARAM_ID_MAST_HIT_FLAG,          ///< 必中ワザ（回避率によるハズれ計算をしない）フラグ
      PARAM_ID_ZENRYOKU_WAZANO,        ///< 全力技として使用した場合の、変化後の技No.
      PARAM_ID_ZENRYOKU_POWER,         ///< 全力技として使用した場合の、変化後の威力
      PARAM_ID_ZENRYOKU_EFFECT,        ///< 全力技として使用した場合の、追加効果
      PARAM_ID_DIRT_TYPE,              ///< 0x22 汚れタイプ( pml::pokepara::DirtType )
      PARAM_ID_DIRT_RATE,              ///< 0x23 汚れる確率[%]
      PARAM_ID_MAX,
    };


  } //namespace wazadata
} //namespace pml

#endif //__PML_WAZADATACONST_H__
