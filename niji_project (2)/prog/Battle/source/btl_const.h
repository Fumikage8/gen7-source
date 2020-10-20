#if !defined( __BTL_CONST_H__ )
#define __BTL_CONST_H__
//=============================================================================================
/**
 * @file  btl_const.h
 * @brief ポケモンXY バトルシステム 主にサーバ計算用途の定数群
 * @author  taya
 *
 * @date  2010.12.20  作成
 */
//=============================================================================================
#pragma once

#if !defined PAWN_COMPILER
#include "./btl_fx32.h"
#endif


#if !defined PAWN_COMPILER
GFL_NAMESPACE_BEGIN(btl)
#endif

#if !defined PAWN_COMPILER
enum {


  BTL_CALC_BASERANK_DEFAULT = 0,

  BTL_CALC_HITRATIO_MID = 6,
  BTL_CALC_HITRATIO_MIN = 0,
  BTL_CALC_HITRATIO_MAX = 12,

  BTL_CALC_CRITICAL_MIN = 0,
  BTL_CALC_CRITICAL_MAX = 3,

// ダメージ計算関連
  BTL_CALC_DMG_TARGET_RATIO_PLURAL       = FX32_CONST(0.75f), ///< 複数体が対象ワザのダメージ補正率
  BTL_CALC_DMG_TARGET_RATIO_PLURAL_ROYAL = FX32_CONST(0.5f),  ///< 複数体が対象ワザのダメージ補正率( バトルロイヤル )
  BTL_CALC_DMG_TARGET_RATIO_NONE   = FX32_CONST(1),           ///< 対象によるダメージ補正なし
  BTL_CALC_DMG_TARGET_RATIO_QUAT   = FX32_CONST(0.25f),       ///< 強制的に複数回ヒットさせる際の補正率
  BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE = FX32_CONST(1.5f),    ///< ワザタイプ＆天候の組み合わせで有利な場合の補正率
  BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE = FX32_CONST(0.5f), ///< ワザタイプ＆天候の組み合わせで不利な場合の補正率
  BTL_CALC_DMG_WEATHER_RATIO_NONE = FX32_CONST(1),            ///< ワザタイプ＆天候の組み合わせによる補正なし


// 状態異常処理関連
  BTL_NEMURI_TURN_MIN = 2,  ///< 「ねむり」最小ターン数
  BTL_NEMURI_TURN_MAX = 4,  ///< 「ねむり」最大ターン数
  BTL_NEMURI_TURN_RANGE = (BTL_NEMURI_TURN_MAX - BTL_NEMURI_TURN_MIN + 1),

  BTL_MAHI_EXE_PER = 25,        ///< 「まひ」でしびれて動けない確率
  BTL_MAHI_AGILITY_RATIO = 50,  ///< 「まひ」時のすばやさ減衰率
  BTL_KORI_MELT_PER = 20,       ///< 「こおり」が溶ける確率
  BTL_MEROMERO_EXE_PER = 50,    ///< 「メロメロ」で動けない確率

  BTL_DOKU_SPLIT_DENOM = 8,         ///< 「どく」で最大HPの何分の１減るか
  BTL_MOUDOKU_INC_MAX = 15,         ///< 「どくどく」でダメージ量が増加するターン数最大
  BTL_YAKEDO_SPLIT_DENOM = 16,       ///< 「やけど」で最大HPの何分の１減るか
  BTL_YAKEDO_DAMAGE_RATIO = 50,     ///< 「やけど」で物理ダメージを減じる率
  BTL_MOUDOKU_SPLIT_DENOM = 16,     ///< もうどく時、最大HPの何分の１減るか（基本値=最小値）
  BTL_MOUDOKU_COUNT_MAX = BTL_MOUDOKU_SPLIT_DENOM-1,  ///< もうどく時、ダメージ倍率をいくつまでカウントアップするか

  BTL_AKUMU_SPLIT_DENOM = 4, ///< 「あくむ」で最大HPの何分の１減るか
  BTL_NOROI_SPLIT_DENOM = 4, ///< 「のろい」で最大HPの何分の１減るか

  BTL_BIND_TURN_MAX = 6,        ///< まきつき系の継続ターン数最大値


  BTL_CONF_EXE_RATIO = 33,    ///< 混乱時の自爆確率
  BTL_CONF_TURN_MIN = 2,
  BTL_CONF_TURN_MAX = 4,
  BTL_CONF_TURN_RANGE = (BTL_CONF_TURN_MAX - BTL_CONF_TURN_MIN)+1,

  BTL_KANASIBARI_EFFECTIVE_TURN = 4,  ///< 「かなしばり」の実効ターン数
  BTL_TYOUHATSU_EFFECTIVE_TURN = 3,   ///< 「ちょうはつ」の実効ターン数

  BTL_CALC_SICK_TURN_PERMANENT = 0xff,  ///< 継続ターン数を設定しない（永続する）場合の指定値

  // 天候関連
  BTL_CALC_WEATHER_MIST_HITRATIO = FX32_CONST(75),    ///< 「きり」の時の命中率補正率
  BTL_WEATHER_TURN_DEFAULT = 5,         ///< ワザによる天候変化の継続ターン数デフォルト値
  BTL_WEATHER_TURN_PERMANENT = 0xff,    ///< 天候継続ターン無制限

  // とくせい関連
  BTL_CALC_TOK_CHIDORI_HITRATIO = FX32_CONST(0.5f),     /// 「ちどりあし」の減衰命中率
  BTL_CALC_TOK_HARIKIRI_HITRATIO = FX32_CONST(0.8f),    /// 「はりきり」の命中率変化率
  BTL_CALC_TOK_FUKUGAN_HITRATIO = FX32_CONST(1.3f),     /// 「ふくがん」の命中変化率
  BTL_CALC_TOK_SUNAGAKURE_HITRATIO = FX32_CONST(0.8f),  /// 「すながくれ」の命中率変化率
  BTL_CALC_TOK_YUKIGAKURE_HITRATIO = FX32_CONST(0.8f),  /// 「ゆきがくれ」の命中変化率
  BTL_CALC_TOK_HARIKIRI_POWRATIO = FX32_CONST(1.5f),    /// 「はりきり」の攻撃力変化率
  BTL_CALC_TOK_TETUNOKOBUSI_POWRATIO = FX32_CONST(1.2f),/// 「てつのこぶし」の攻撃力変化率
  BTL_CALC_TOK_SUTEMI_POWRATIO = FX32_CONST(1.2f),      /// 「すてみ」の攻撃力変化率
  BTL_CALC_TOK_PLUS_POWRATIO = FX32_CONST(1.5f),        /// 「プラス」の攻撃力変化率
  BTL_CALC_TOK_MINUS_POWRATIO = FX32_CONST(1.5f),       /// 「マイナス」の攻撃力変化率
  BTL_CALC_TOK_FLOWERGIFT_POWRATIO = FX32_CONST(1.5f),  /// 「フラワーギフト」の攻撃力変化率
  BTL_CALC_TOK_FLOWERGIFT_GUARDRATIO = FX32_CONST(1.5f),/// 「フラワーギフト」の特防変化率
  BTL_CALC_TOK_MORAIBI_POWRATIO = FX32_CONST(1.5f),     /// 「もらいび」の攻撃力変化率
  BTL_CALC_TOK_TECKNICIAN_POWRATIO = FX32_CONST(1.5f),  /// 「テクニシャン」の攻撃力変化率
  BTL_CALC_TOK_HAYAASI_AGIRATIO = FX32_CONST(1.5f),     /// 「はやあし」のすばやさ変化率
  BTL_CALC_TOK_SLOWSTART_ENABLE_TURN = 5,               /// 「スロースタート」の適用ターン
  BTL_CALC_TOK_SLOWSTART_AGIRATIO = FX32_CONST(0.5f),   /// 「スロースタート」のすばやさ変化率
  BTL_CALC_TOK_SLOWSTART_ATKRATIO = FX32_CONST(0.5f),   /// 「スロースタート」のこうげき変化率
  BTL_CALC_TOK_FUSIGINAUROKO_GDRATIO = FX32_CONST(1.5f),  /// 「ふしぎなうろこ」のとくぼう変化率

  BTL_CALC_TOK_DOKUNOTOGE_PER   = 30,   ///「どくのトゲ」発生確率
  BTL_CALC_TOK_HONONOKARADA_PER = 30,   ///「ほのおのからだ」発生確率
  BTL_CALC_TOK_SEIDENKI_PER     = 30,   ///「せいでんき」発生確率
  BTL_CALC_TOK_HOUSI_PER        = 30,   ///「ほうし」発生確率
  BTL_CALC_TOK_MEROMEROBODY_PER = 30,   ///「メロメロボディ」発生確率

  // 素早さ補正関連
  BTL_CALC_AGILITY_MAX = 10000,

  // 体重処理関連
  BTL_POKE_WEIGHT_MIN = 1,

  // フィールド・進化画面等への連絡関連
  BTL_STANDALONE_PLAYER_CLIENT_ID = 0,
  BTL_BONUS_MONEY_MAX = 99999,
  BTL_REGULAR_MONEY_MAX = 999999,

  // 仮モンスターナンバー
  MONSNO_CN_MERODHIA = 648,
  MONSNO_CN_INSEKUTA = 649,

  FORMNO_CN_MERODHIA_VOICE = 0,
  FORMNO_CN_MERODHIA_STEP = 1,


  BTL_CLIENT_MAX = 4,       ///< 稼働するクライアント数
  BTL_POSIDX_MAX = 5,       ///< クライアントが管理する最大の戦闘位置数
  BTL_PARTY_MEMBER_MAX = 6, ///< パーティのメンバー最大数
  BTL_FRONT_POKE_MAX   = 6, ///< バトルに出ているポケモンの最大数（3vs3, 1vs5 ... いずれも6）
  BTL_POKEID_MAX = (BTL_CLIENT_MAX*BTL_PARTY_MEMBER_MAX), ///< ポケモンIDの最大値
  BTL_POKEID_NULL = 31,     ///< 無効が保証されるポケモンID（サーバコマンド生成の都合上、5bitに収まるように…）
  BTL_TURNCOUNT_MAX = 9999, ///< カウントするターン数の最大
  BTL_SHOOTER_ENERGY_MAX = 14,  ///< シューターの蓄積エネルギー最大値
  BTL_ROTATE_NUM = 3,       ///< ローテーション対象ポケ数
  BTL_CAPTURE_BALL_YURE_CNT_MAX = 3,  ///< 捕獲時にボールが揺れる最大回数
  BTL_CLIENT_ACTION_PER_TURN_MAX  = BTL_POSIDX_MAX + 1, ///< クライアントが１ターンに行うアクション最大（戦闘位置数の最大＋メガシンカなど）


  BTL_CLIENTID_NULL = BTL_CLIENT_MAX,

  BTL_CLIENTID_SA_PLAYER = 0,
  BTL_CLIENTID_SA_ENEMY1 = 1,
  BTL_CLIENTID_SA_FRIEND = 2,
  BTL_CLIENTID_SA_ENEMY2 = 3,

  BTL_BGM_FADEOUT_FRAMES = 30,
  BTL_STR_ARG_MAX        = 8,


  BTL_STRBUF_STD_LENGTH = 1600, ///< 各所で用意する文字列バッファの長さ

};
#endif

//--------------------------------------------------------------
/**
 * @brief 勝敗判定の判定理由
 */
//--------------------------------------------------------------
enum ResultCause
{
  RESULT_CAUSE_OTHER,    // その他
  RESULT_CAUSE_ALL_DEAD, // 全滅した
  RESULT_CAUSE_TIMEOVER, // 試合時間 or 持ち時間 がゼロになった
};

//--------------------------------------------------------------
/**
 *  逃げ・交換選択コード
 */
//--------------------------------------------------------------
enum BtlClientChangeEscSelect {
  BTL_CLIENTASK_CHANGE = 0,
  BTL_CLIENTASK_ESCAPE,
};

//--------------------------------------------------------------
/**
 * 特殊優先順コード（素早さより上位、ワザ優先順より下位：0～7に収まるように！）
 */
//--------------------------------------------------------------
enum BtlSpecialPri {
  BTL_SPPRI_LOW     = 0,
  BTL_SPPRI_DEFAULT = 1,
  BTL_SPPRI_HIGH    = 2,
};

//--------------------------------------------------------------
/**
 *  お小遣いダブルアップ要因（要因が重なれば２倍、４倍、、、と増える）
 */
//--------------------------------------------------------------
enum  MoneyDblUpCause {
  MONEY_DBLUP_STD = 0,    ///< おまもりこばん・こううんのおこう 等
  MONEY_DBLUP_EXTRA,      ///< ハッピータイム

  MONEY_DBLUP_CAUSE_MAX,
};

//--------------------------------------------------------------
/**
 *  地形効果
 */
//--------------------------------------------------------------
enum BtlGround_tag{

  BTL_GROUND_NONE = 0,   ///< なし

  BTL_GROUND_GRASS,    ///< グラスフィールド
  BTL_GROUND_MIST,     ///< ミストフィールド
  BTL_GROUND_ELEKI,    ///< エレキフィールド
  BTL_GROUND_PHYCHO,   ///< サイコフィールド
  BTL_GROUND_AQUA,
  BTL_GROUND_ICE,


  BTL_GROUND_MAX,

};
#if !defined PAWN_COMPILER
typedef u8 BtlGround;
#endif


//--------------------------------------------------------------
/**
 * @brief 乱入の種類
 */
//--------------------------------------------------------------
enum BtlIntrudeType
{
  BTL_INTRUDE_TYPE_INTRUDE,   // 乱入
  BTL_INTRUDE_TYPE_REINFORCE, // 援軍
};


//--------------------------------------------------------------
/**
 * @brief ダメージ計算時のクリティカル判定タイプ
 */
//--------------------------------------------------------------
enum CriticalType {
  CRITICAL_NONE = 0,    ///< クリティカルではない
  CRITICAL_NORMAL,      ///< クリティカル（通常）
  CRITICAL_FRIENDSHIP,  ///< クリティカル（なかよし度の影響あり）
};

#if !defined PAWN_COMPILER
GFL_NAMESPACE_END(btl)
#endif


#endif // __BTL_CONST_H__
