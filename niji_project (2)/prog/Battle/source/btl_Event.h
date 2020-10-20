#if !defined( __BTL_EVENT_H__ )
#define __BTL_EVENT_H__
//=============================================================================================
/**
 * @file    btl_Event.h
 * @brief   ポケモンXY バトルシステム イベント＆ハンドラ処理
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================
#pragma once

#include "btl_common.h"

GFL_NAMESPACE_BEGIN(btl)

class ServerFlow;

//--------------------------------------------------------------
/**
* イベントタイプ
*/
//--------------------------------------------------------------
typedef enum 
{
  BTL_EVENT_NULL = 0,
  BTL_EVENT_ACTPROC_START,                      ///< アクション処理１件開始
  BTL_EVENT_ACTPROC_END,                        ///< アクション処理１件終了
  BTL_EVENT_WAZASEQ_START,                      ///< ワザ処理開始
  BTL_EVENT_WAZASEQ_END,                        ///< ワザ処理終了
  BTL_EVENT_MIGAWARI_THREW,                     ///< みがわり貫通チェック
  BTL_EVENT_CHECK_DELAY_WAZA,                   ///< 遅延発動ワザ準備チェック
  BTL_EVENT_DECIDE_DELAY_WAZA,                  ///< 遅延発動ワザ準備完了
  BTL_EVENT_WAZASEQ_ROB,                        ///< ワザ乗っ取り確定
  BTL_EVENT_WAZASEQ_REFRECT,                    ///< ワザ跳ね返し確定
  BTL_EVENT_CHECK_CHANGE,                       ///< いれかえチェック
  BTL_EVENT_SKIP_NIGERU_CALC,                   ///< にげる確率計算スキップチェック
  BTL_EVENT_NIGERU_FORBID,                      ///< にげる封じチェック
  BTL_EVENT_NIGERU_EXMSG,                       ///< にげる特殊メッセージ
  BTL_EVENT_CHECK_INEMURI,                      ///< いねむり成否チェック
  BTL_EVENT_CHECK_SP_PRIORITY,                  ///< 特殊優先度チェック
  BTL_EVENT_WORKED_SP_PRIORITY,                 ///< 特殊優先度効果発生した
  BTL_EVENT_GET_WAZA_PRI,                       ///< ワザプライオリティ取得
  BTL_EVENT_CHECK_FLYING,                       ///< 浮遊チェック
  BTL_EVENT_CALC_AGILITY,                       ///< すばやさ計算
  BTL_EVENT_CHECK_CONF,                         ///< 混乱チェック
  BTL_EVENT_FIX_CONF_DAMAGE,                    ///< 混乱ダメージ補正
  BTL_EVENT_CONF_DAMAGE_REACTION,               ///< 混乱ダメージ後のリアクション
  BTL_EVENT_BEFORE_FIGHT,                       ///< ターン最初のワザシーケンス直前
  BTL_EVENT_GET_ZENRYOKU_WAZA,                  ///< 全力技取得
  BTL_EVENT_GET_ZENRYOKU_EFFECT,                ///< 全力技効果取得
  BTL_EVENT_REQWAZA_FOR_ACT_ORDER,              ///< 優先順計算に使う他ワザ呼び出し
  BTL_EVENT_REQWAZA_CHECKFAIL,                  ///< 他ワザ呼び出し失敗チェック
  BTL_EVENT_REQWAZA_PARAM,                      ///< 他ワザ呼び出し時パラメータ
  BTL_EVENT_REQWAZA_MSG,                        ///< 他ワザ呼び出し時メッセージ
  BTL_EVENT_REQWAZA_ZENRYOKU_MSG,               ///< 派生された技を全力ブースト化するメッセージ
  BTL_EVENT_CHECK_WAZA_ROB,                     ///< ワザ乗っ取りチェック
  BTL_EVENT_WAZA_NOEFF_BY_FLYING,               ///< 浮遊状態による地面ワザ無効化
  BTL_EVENT_SKIP_AVOID_CHECK,                   ///< ワザ命中判定スキップチェック
  BTL_EVENT_WAZA_FAIL_THREW,                    ///< ワザ失敗要因の無視判定
  BTL_EVENT_WAZA_EXECUTE_CHECK_1ST,             ///< ワザ実行チェック（ワザメッセージ前・混乱より優先）
  BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,             ///< ワザ実行チェック（ワザメッセージ後・ふんじん前）
  BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,             ///< ワザ実行チェック（ワザメッセージ後・ふんじん後）
  BTL_EVENT_WAZA_EXECUTE_FAIL,                  ///< ワザ出し失敗
  BTL_EVENT_WAZA_CALL_DECIDE,                   ///< ワザ出し（ワザ名宣言=PP減少）確定
  BTL_EVENT_WAZA_EXE_DECIDE,                    ///< ワザ出し確定（ふういん等に除外されず実行が確定）
  BTL_EVENT_WAZA_EXE_START,                     ///< ワザ出し処理開始
  BTL_EVENT_WAZA_EXECUTE_EFFECTIVE,             ///< ワザ出し成功（効果あり）
  BTL_EVENT_WAZA_EXECUTE_NO_EFFECT,             ///< ワザ出し成功（効果なし）
  BTL_EVENT_WAZA_EXECUTE_DONE,                  ///< ワザ出し終了
  BTL_EVENT_WAZA_PARAM,                         ///< ワザパラメータチェック
  BTL_EVENT_WAZA_PARAM_2ND,                     ///< ワザパラメータチェック（２）
  BTL_EVENT_AFTER_WAZA_PARAM_FIXED,             ///< ワザパラメータ確定後
  BTL_EVENT_DECIDE_TARGET,                      ///< ワザ対象決定
  BTL_EVENT_TEMPT_TARGET,                       ///< ワザ対象を自分に引き寄せ
  BTL_EVENT_TEMPT_TARGET_END,                   ///< ワザ対象を自分に引き寄せ:終了
  BTL_EVENT_NOEFFECT_CHECK_FIELD,               ///< フィールド効果による技無効化チェック
  BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_BEGIN, ///< 防御系サイドエフェクトによるワザ無効化チェック：開始
  BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD,       ///< 防御系サイドエフェクトによるワザ無効化チェック
  BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_END,   ///< 防御系サイドエフェクトによるワザ無効化チェック：終了
  BTL_EVENT_NOEFFECT_CHECK_L1,                  ///< ワザ無効化チェック（lv1 = 必中には負ける）ハンドラ
  BTL_EVENT_NOEFFECT_CHECK_L2,                  ///< ワザ無効化チェック（lv2 = 必中にも勝つ）ハンドラ
  BTL_EVENT_NOEFFECT_CHECK_MAMORU,              ///< ワザ無効化チェック（まもると同様）
  BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,             ///< 特性によるワザ無効化チェック
  BTL_EVENT_CHECK_MAMORU_BREAK,                 ///< まもる無効化チェック
  BTL_EVENT_WAZA_AVOID,                         ///< ワザ外れた
  BTL_EVENT_DMG_TO_RECOVER_CHECK,               ///< ワザダメージ->回復化チェックハンドラ
  BTL_EVENT_DMG_TO_RECOVER_FIX,                 ///< ワザダメージ->回復化決定ハンドラ
  BTL_EVENT_CALC_HIT_CANCEL,                    ///< 命中率計算を飛ばすかチェック
  BTL_EVENT_WAZA_HIT_RANK,                      ///< 命中率・回避率の決定ハンドラ
  BTL_EVENT_WAZA_HIT_RATIO,                     ///< ワザの命中率補正ハンドラ
  BTL_EVENT_WAZA_HIT_RATIO_ICHIGEKI,            ///< ワザの命中率補正ハンドラ( 一撃必殺技用 )
  BTL_EVENT_WAZA_HIT_COUNT,                     ///< ヒット回数決定（複数回ヒットワザのみ）
  BTL_EVENT_CRITICAL_CHECK,                     ///< クリティカルチェック
  BTL_EVENT_AFTER_CRITICAL,                     ///< 急所メッセージ後
  BTL_EVENT_WAZA_POWER_BASE,                    ///< ワザ威力基本値ハンドラ
  BTL_EVENT_WAZA_POWER,                         ///< ワザ威力補正ハンドラ
  BTL_EVENT_ATTACKER_POWER_PREV,                ///< 攻撃側の能力値（こうげきorとくこう）取得前ハンドラ
  BTL_EVENT_DEFENDER_GUARD_PREV,                ///< 防御側の能力値（ぼうぎょorとくぼう）取得前ハンドラ
  BTL_EVENT_ATTACKER_POWER,                     ///< 攻撃側の能力値（こうげきorとくこう）補正ハンドラ
  BTL_EVENT_DEFENDER_GUARD,                     ///< 防御側の能力値（ぼうぎょorとくぼう）補正ハンドラ
  BTL_EVENT_CHECK_REMOVEALL_CANCEL,             ///< ターゲットが残っていなくてもワザ処理続行するか判定
  BTL_EVENT_CHECK_AFFINITY_ENABLE,              ///< 相性による無効チェックを行うか確認
  BTL_EVENT_CHECK_AFFINITY,                     ///< 相性計算
  BTL_EVENT_CHECK_AFFINITY_ONLY_ATTACKER,       ///< 相性計算( 攻撃側のイベントハンドラのみが反応し、防御側の特性等を考慮しない )
  BTL_EVENT_REWRITE_AFFINITY,                   ///< 計算後の相性を強制的に書き換える
  BTL_EVENT_AFFINE_FLOATING_CANCEL,             ///< 相性計算時、ふゆう状態のチェックを行わない
  BTL_EVENT_ATTACKER_TYPE,                      ///< 攻撃側ポケタイプ
  BTL_EVENT_TYPEMATCH_CHECK,                    ///< 攻撃側タイプ一致チェック
  BTL_EVENT_TYPEMATCH_RATIO,                    ///< 攻撃側タイプ一致時倍率
  BTL_EVENT_WAZA_ATK_TYPE,                      ///< 攻撃側ポケタイプチェックハンドラ
  BTL_EVENT_WAZA_DEF_TYPE,                      ///< 防御側ポケタイプチェックハンドラ
  BTL_EVENT_PREV_WAZA_DMG,                      ///< ダメージ演出直後
  BTL_EVENT_WAZA_DMG_DETERMINE,                 ///< ダメージワザが当たること確定した（計算前）
  BTL_EVENT_WAZA_DMG_PROC1,                     ///< タイプ計算前ダメージ補正ハンドラ
  BTL_EVENT_WAZA_DMG_PROC2,                     ///< タイプ計算前ダメージ補正ハンドラ
  BTL_EVENT_WAZA_DMG_PROC3,                     ///< タイプ計算後ダメージ補正ハンドラ
  BTL_EVENT_WAZA_DMG_PROC_Z,                    ///< Z技に対するダメージ補正ハンドラ
  BTL_EVENT_WAZA_DMG_PROC_END,                  ///< ダメージ計算最終
  BTL_EVENT_WAZA_DMG_LAST,                      ///< 最終ダメージ補正ハンドラ
  BTL_EVENT_WAZA_DMG_REACTION_PREV,             ///< ダメージ処理直前チェック
  BTL_EVENT_WAZA_DMG_REACTION,                  ///< ダメージ処理１体ごとの反応
  BTL_EVENT_WAZA_DMG_REACTION_L2,               ///< ダメージ処理１体ごとの反応（２段階目）
  BTL_EVENT_WAZA_DMG_REACTION_L3,               ///< ダメージ処理１体ごとの反応（３段階目）
  BTL_EVENT_WAZADMG_SIDE_AFTER,                 ///< ダメージを対象全部に与えた後の追加処理（１回のみ）
  BTL_EVENT_DECREMENT_PP,                       ///< 使用ワザの減少PP値を取得
  BTL_EVENT_DECREMENT_PP_DONE,                  ///< 使用ワザのPP値を減少後
  BTL_EVENT_CALC_KICKBACK,                      ///< 反動計算ハンドラ
  BTL_EVENT_ADD_RANK_TARGET,                    ///< ワザを受けた側への追加効果
  BTL_EVENT_ADD_RANK_USER,                      ///< ワザを使った側への追加効果
  BTL_EVENT_MENBERCHANGE_INTR,                  ///< メンバー入れ替え割り込み
  BTL_EVENT_MEMBER_OUT_FIXED,                   ///< 個別ポケ退場確定後
  BTL_EVENT_MEMBER_IN_DEFAULT_POWERUP,          ///< 個別ポケ入場直後、デフォルト能力アップ
  BTL_EVENT_MEMBER_IN_BATONTOUCH,               ///< 個別ポケ入場直後、バトンタッチ受け継ぎ
  BTL_EVENT_MEMBER_IN,                          ///< 個別ポケ入場直後
  BTL_EVENT_MEMBER_IN_PREV,                     ///< 全ポケ入場イベント直前
  BTL_EVENT_MEMBER_IN_COMP,                     ///< 全ポケ入場イベント処理後
  BTL_EVENT_GET_RANKEFF_VALUE,                  ///< ワザによる能力ランク増減値チェック
  BTL_EVENT_RANKVALUE_CHANGE,                   ///< ランク増減値の最終チェック
  BTL_EVENT_RANKEFF_LAST_CHECK,                 ///< ランク増減成否チェック
  BTL_EVENT_RANKEFF_FAILED,                     ///< 能力ランク増減失敗確定
  BTL_EVENT_RANKEFF_FIXED,                      ///< 能力ランク増減処理後
  BTL_EVENT_WAZA_RANKEFF_FIXED,                 ///< ワザによるランク増減効果成功
  BTL_EVENT_WAZASICK_TURN_COUNT,                ///< 特殊処理の状態異常ターン数
  BTL_EVENT_WAZASICK_SPECIAL,                   ///< 特殊処理の状態異常ID決定
  BTL_EVENT_WAZASICK_STR,                       ///< ワザによる通常の状態異常決定時テキスト
  BTL_EVENT_WAZASICK_PARAM,                     ///< ワザによる状態異常のパラメータチェック
  BTL_EVENT_ADD_SICK_TYPE,                      ///< ワザの追加効果による状態異常のタイプ調整
  BTL_EVENT_ADD_SICK,                           ///< ワザの追加効果による状態異常の発生チェック
  BTL_EVENT_ADDSICK_CHECKFAIL,                  ///< 状態異常の失敗チェック
  BTL_EVENT_ADDSICK_FIX,                        ///< 状態異常確定
  BTL_EVENT_ADDSICK_FAILED,                     ///< 状態異常失敗
  BTL_EVENT_POKESICK_FIXED,                     ///< 基本状態異常確定
  BTL_EVENT_WAZASICK_FIXED,                     ///< ワザ系状態異常確定
  BTL_EVENT_IEKI_FIXED,                         ///< いえきによる特性無効化の確定
  BTL_EVENT_SICK_DAMAGE,                        ///< 状態異常によるダメージ計算
  BTL_EVENT_WAZA_SHRINK_PER,                    ///< ワザによるひるみ確率計算
  BTL_EVENT_SHRINK_CHECK,                       ///< ひるみ計算
  BTL_EVENT_SHRINK_FAIL,                        ///< ひるみ失敗
  BTL_EVENT_SHRINK_FIX,                         ///< ひるみ確定後
  BTL_EVENT_ICHIGEKI_CHECK,                     ///< 一撃必殺チェック
  BTL_EVENT_ICHIGEKI_GUARD,                     ///< みがわりチェック後の一撃必殺技無効化チェック
  BTL_EVENT_NOT_WAZA_DAMAGE,                    ///< ワザ以外のダメージチェック
  BTL_EVENT_USE_ITEM,                           ///< 装備アイテム使用
  BTL_EVENT_USE_ITEM_TMP,                       ///< 装備アイテム使用（一時処理）
  BTL_EVENT_KORAERU_CHECK,                      ///<「こらえる」チェック
  BTL_EVENT_KORAERU_EXE,                        ///<「こらえる」発動
  BTL_EVENT_TURNCHECK_BEGIN,                    ///< ターンチェック（先頭）
  BTL_EVENT_TURNCHECK_END,                      ///< ターンチェック（終端）
  BTL_EVENT_TURNCHECK_DONE,                     ///< ターンチェック（終了後）
  BTL_EVENT_NOTIFY_AIRLOCK,                     ///< エアロック開始
  BTL_EVENT_WEATHER_CHECK,                      ///< 天候のチェック
  BTL_EVENT_WEIGHT_RATIO,                       ///< 体重変化率チェック
  BTL_EVENT_WAZA_WEATHER_TURNCNT,               ///< ワザによる天候変化時のターン数チェック
  BTL_EVENT_WEATHER_CHANGE,                     ///< 天候の変化（直前）
  BTL_EVENT_WEATHER_CHANGE_AFTER,               ///< 天候の変化（直後）
  BTL_EVENT_WEATHER_REACTION,                   ///< 天候ダメージに対する反応
  BTL_EVENT_SIMPLE_DAMAGE_ENABLE,               ///< ワザ以外ダメージ有効判定
  BTL_EVENT_SIMPLE_DAMAGE_BEFORE,               ///< ワザ以外ダメージを受ける直前
  BTL_EVENT_SIMPLE_DAMAGE_AFTER,                ///< ワザ以外ダメージを受けた後( アイテム反応後 )
  BTL_EVENT_SIMPLE_DAMAGE_FAILED,               ///< ワザ以外ダメージを防いだ
  BTL_EVENT_DAMAGEPROC_START,                   ///< ダメージワザシーケンス開始
  BTL_EVENT_DAMAGEPROC_END_HIT_PREV,            ///< ダメージワザシーケンス終了直前
  BTL_EVENT_DAMAGEPROC_END_HIT_REAL,            ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり除く）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L1,              ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり含む）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L2,              ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり含む）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L3,              ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり含む）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L4,              ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり除く）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L5,              ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり含む）が居る）
  BTL_EVENT_DAMAGEPROC_END,                     ///< ダメージワザシーケンス終了（常に呼び出し）
  BTL_EVENT_UNDAMAGEPROC_END,                   ///< 非ダメージワザシーケンス終了（常に呼び出し）
  BTL_EVENT_CHANGE_TOKUSEI_BEFORE,              ///< とくせい書き換え直前（書き換え確定）
  BTL_EVENT_CHANGE_TOKUSEI_AFTER,               ///< とくせい書き換え後
  BTL_EVENT_CHECK_PUSHOUT,                      ///< ふきとばし系ワザチェック
  BTL_EVENT_CALC_DRAIN,                         ///< ドレイン系ワザ回復量計算
  BTL_EVENT_CALC_DRAIN_END,                     ///< ドレイン系ワザ回復量計算
  BTL_EVENT_CALC_SPECIAL_DRAIN,                 ///< ドレイン系ワザ回復量計算
  BTL_EVENT_RECOVER_HP_RATIO,                   ///< HP回復ワザの回復率計算
  BTL_EVENT_CHECK_ITEMEQUIP_FAIL,               ///< 装備アイテム使用可否チェック
  BTL_EVENT_AFTER_ITEMEQUIP,                    ///< 装備アイテム使用後チェック
  BTL_EVENT_CHECK_ITEM_REACTION,                ///< アイテム反応チェック
  BTL_EVENT_ITEM_CONSUMED,                      ///< 装備アイテム消費後
  BTL_EVENT_CHECK_TAMETURN_FAIL,                ///< 溜めターン失敗判定
  BTL_EVENT_CHECK_TAMETURN_SKIP,                ///< 溜めターンスキップ判定
  BTL_EVENT_TAME_START,                         ///< 溜め開始
  BTL_EVENT_TAME_START_FIXED,                   ///< 溜め開始確定
  BTL_EVENT_TAME_SKIP,                          ///< 溜めスキップ確定
  BTL_EVENT_TAME_RELEASE,                       ///< 溜め解放
  BTL_EVENT_TAME_RELEASE_FIXED,                 ///< 溜め解放確定
  BTL_EVENT_CHECK_POKE_HIDE,                    ///< ポケモンが消えている状態のワザ命中チェック
  BTL_EVENT_ITEMSET_CHECK,                      ///< アイテム書き換え前の成否チェック
  BTL_EVENT_ITEMSET_FAILED,                     ///< アイテム書き換え失敗
  BTL_EVENT_ITEMSET_DECIDE,                     ///< アイテム書き換え確定
  BTL_EVENT_ITEMSET_FIXED,                      ///< アイテム書き換え完了
  BTL_EVENT_FIELD_EFFECT_CALL,                  ///< フィールドエフェクト追加
  BTL_EVENT_CHECK_SIDEEFF_PARAM,                ///< サイドエフェクトパラメータ調整
  BTL_EVENT_UNCATEGORIZE_WAZA,                  ///< 未分類ワザ処理
  BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,        ///< 未分類ワザ処理（ターゲットいないワザ）
  BTL_EVENT_COMBIWAZA_CHECK,                    ///< 合体ワザ成立チェック
  BTL_EVENT_NOTIFY_DEAD,                        ///< 死亡直前
  BTL_EVENT_AFTER_MOVE,                         ///< ムーブ動作直後
  BTL_EVENT_MAMORU_SUCCESS,                     ///< 「まもる」系が成功して相手のワザを防いだ直後
  BTL_EVENT_FREEFALL_START_GUARD,               ///< フリーフォールの溜めターンをガードするか判定
  BTL_EVENT_PUSHOUT_EFFECT_NO,                  ///< おしだしワザのエフェクトナンバー決定
  BTL_EVENT_CHECK_ATTACKER_DEAD,                ///< 攻撃ポケモンのひん死を、ワザ対象より前に行うか判定
  BTL_EVENT_SP_ADDITIONAL_PER,                  ///< 特別なワザの追加効果確率チェック
  BTL_EVENT_TAMERELEASE_FAILED,                 ///< 溜めワザ解放処理の失敗時
  BTL_EVENT_KILL_HANDLER,                       ///< 強制ひんし処理が呼ばれた
  BTL_EVENT_MEMBER_IN_EVO,                      ///< カイグラ進化用(アイテムチェックをスキップする!
  BTL_EVENT_CHANGE_POKE_BEFORE,                 ///< へんしん・メガシンカ・フォルムチェンジ前(新規天候特性キャンセル処理
  BTL_EVENT_CHANGE_GROUND_BEFORE,               ///< 地形効果変更前
  BTL_EVENT_CHANGE_GROUND_AFTER,                ///< 地形効果変更後
  BTL_EVENT_DEAD_ACTION_AFTER,                  ///< ポケモン死亡による退場の直後
  BTL_EVENT_DEAD_AFTER,                         ///< ポケモン死亡による退場処理の後
  BTL_EVENT_TOKUSEI_CHANGE_CHECKFAIL,           ///< 特性の変更が可能かチェック
  BTL_EVENT_TOKUSEI_CHANGE_FAILED,              ///< 特性の変更に失敗
  BTL_EVENT_CHECK_ADDSICK_FAIL_STD_SKIP,        ///< 基本ルールによる状態異常の失敗チェックをスキップするか？
  BTL_EVENT_FAIL_WAZAMELT_CHECK,                ///< ワザ出し時のこおり解除で確実に失敗させるかどうか
  BTL_EVENT_ICHIGEKI_SUCCEED,                   ///< 一撃必殺ワザの成功確定直後
  BTL_EVENT_REPLACE_ACT_WAZA,                   ///< アンコール効果で強制的にワザを差し替えられた
  BTL_EVENT_ACTPROC_CANCELED,                   ///< アクション実行がキャンセルされた（フリーフォールなどによる）
  BTL_EVENT_TURN_END,                           ///< ターン終了

} BtlEventType;


//--------------------------------------------------------------
/**
* イベント変数ラベル
*/
//--------------------------------------------------------------
typedef enum {
  BTL_EVAR_NULL = 0,
  BTL_EVAR_SYS_SEPARATE,
  //---ここから上はシステム予約領域-------------------------
  BTL_EVAR_POKEID,
  BTL_EVAR_POKEID_ATK,
  BTL_EVAR_POKEID_DEF,
  BTL_EVAR_TARGET_POKECNT,
  BTL_EVAR_POKEID_TARGET1,
  BTL_EVAR_POKEID_TARGET2,
  BTL_EVAR_POKEID_TARGET3,
  BTL_EVAR_POKEID_TARGET4,
  BTL_EVAR_POKEID_TARGET5,
  BTL_EVAR_POKEID_TARGET6,
  BTL_EVAR_ACTION,
  BTL_EVAR_POKEPOS,
  BTL_EVAR_POKEPOS_ORG,
  BTL_EVAR_TOKUSEI_PREV,
  BTL_EVAR_TOKUSEI_NEXT,
  BTL_EVAR_SP_PRIORITY,
  BTL_EVAR_WAZAID,
  BTL_EVAR_EFFECT_WAZAID,
  BTL_EVAR_ORG_WAZAID,
  BTL_EVAR_ZENRYOKU_WAZAID,
  BTL_EVAR_ZENRYOKU_WAZA_FLAG,
  BTL_EVAR_ZENRYOKU_WAZA_KIND,
  BTL_EVAR_POKE_TYPE,
  BTL_EVAR_WAZA_TYPE,
  BTL_EVAR_WAZA_TYPE_EX,
  BTL_EVAR_WAZA_IDX,
  BTL_EVAR_WAZA_PRI,
  BTL_EVAR_WAZA_SERIAL,
  BTL_EVAR_DAMAGE_TYPE,
  BTL_EVAR_TARGET_TYPE,
  BTL_EVAR_USER_TYPE,
  BTL_EVAR_SICKID,
  BTL_EVAR_SICK_CONT_ADRS,
  BTL_EVAR_STATUS_TYPE,
  BTL_EVAR_VOLUME,
  BTL_EVAR_POKE_HIDE,
  BTL_EVAR_FAIL_CAUSE,
  BTL_EVAR_CONF_DMG,
  BTL_EVAR_TURN_COUNT,
  BTL_EVAR_ADD_DEFAULT_PER,
  BTL_EVAR_ADD_PER,
  BTL_EVAR_HIT_RANK,
  BTL_EVAR_AVOID_RANK,
  BTL_EVAR_HITCOUNT_MAX,
  BTL_EVAR_HITCOUNT,
  BTL_EVAR_HIT_PER,
  BTL_EVAR_CRITICAL_RANK,
  BTL_EVAR_ITEM,
  BTL_EVAR_ITEM_REACTION,
  BTL_EVAR_AGILITY,
  BTL_EVAR_WAZA_POWER,
  BTL_EVAR_WAZA_POWER_RATIO,
  BTL_EVAR_DAMAGE,
  BTL_EVAR_POWER,
  BTL_EVAR_GUARD,
  BTL_EVAR_RATIO,
  BTL_EVAR_RATIO_EX,
  BTL_EVAR_FIX_DAMAGE,
  BTL_EVAR_FIX_DAMAGE_FLAG,
  BTL_EVAR_TYPEAFF,
  BTL_EVAR_WEATHER,
  BTL_EVAR_KORAERU_CAUSE,
  BTL_EVAR_SWAP_POKEID,
  BTL_EVAR_VID,
  BTL_EVAR_VID_SWAP_CNT,
  BTL_EVAR_ENABLE_MODE,
  BTL_EVAR_WORK_ADRS,
  BTL_EVAR_NOEFFECT_FLAG,
  BTL_EVAR_FAIL_FLAG,
  BTL_EVAR_AVOID_FLAG,
  BTL_EVAR_ALMOST_FLAG,
  BTL_EVAR_TYPEMATCH_FLAG,
  BTL_EVAR_CRITICAL_FLAG,
  BTL_EVAR_MIGAWARI_FLAG,
  BTL_EVAR_RINPUNGUARD_FLG,
  BTL_EVAR_TIKARAZUKU_FLG,
  BTL_EVAR_ITEMUSE_FLAG,
  BTL_EVAR_TRICK_FLAG,
  BTL_EVAR_FLAT_FLAG,
  BTL_EVAR_FLATMASTER_FLAG,
  BTL_EVAR_DELAY_ATTACK_FLAG,
  BTL_EVAR_MAGICCOAT_FLAG,
  BTL_EVAR_MSG_FLAG,
  BTL_EVAR_FIX_FLAG,
  BTL_EVAR_KINOMI_FLAG,
  BTL_EVAR_GEN_FLAG,
  BTL_EVAR_DISABLE_YAKEDO_FLAG,
  BTL_EVAR_SIDE,
  BTL_EVAR_SIDE_EFFECT,
  BTL_EVAR_AURA_BREAK_FLAG,
  BTL_EVAR_AURA_RESERVE_FLAG,
  BTL_EVAR_ZENRYOKU_EFFECT,
  BTL_EVAR_EFFECT_NO,
  BTL_EVAR_TEMPT_TARGET_PRIORITY,
  BTL_EVAR_TEMPT_TARGET_CAUSE,
  BTL_EVAR_WAZA_TOUCH_FLAG,
  BTL_EVAR_WALL_EFFECT_FLAG_PHYSIC,
  BTL_EVAR_WALL_EFFECT_FLAG_SPECIAL,
  BTL_EVAR_GROUND,
  BTL_EVAR_RANKEFFECT_CAUSE,
  BTL_EVAR_DAMAGE_CAUSE,
  BTL_EVAR_DAMAGE_CAUSE_POKEID,
  BTL_EVAR_SICK_CAUSE,
  BTL_EVAR_TOKUSEI_CHANGE_CAUSE,
  BTL_EVAR_ITAZURAGOKORO_EFFECT,
  BTL_EVAR_OIUTI_INTERRUPT_FLAG,
  BTL_EVAR_ACTION_DESC_IS_YOKODORI_ROB_ACTION,
  BTL_EVAR_ACTION_DESC_IS_MAGICCOAT_REACTION,
  BTL_EVAR_ACTION_DESC_IS_ODORIKO_REACTION,
  BTL_EVAR_ACTION_DESC_IS_OIUTI_INTERRUPT_ACTION,

  BTL_EVAR_MAX,

}BtlEvVarLabel;


//--------------------------------------------------------------
/**
* イベント変数スタック
*/
//--------------------------------------------------------------
extern void EVENTVAR_Pop( void );
extern void EVENTVAR_Push( void );

extern void EVENTVAR_CheckStackCleared( void );
extern void EVENTVAR_SetValue( BtlEvVarLabel label, int value );
extern void EVENTVAR_SetConstValue( BtlEvVarLabel label, int value );
extern void EVENTVAR_SetRewriteOnceValue( BtlEvVarLabel label, int value );
extern void EVENTVAR_RecoveryRewriteOncePermission( BtlEvVarLabel label );
extern void EVENTVAR_SetMulValue( BtlEvVarLabel label, int value, fx32 mulMin, fx32 mulMax );
extern bool EVENTVAR_RewriteValue( BtlEvVarLabel label, int value );
extern void EVENTVAR_MulValue( BtlEvVarLabel label, fx32 value );
extern int  EVENTVAR_GetValue( BtlEvVarLabel label );
extern bool EVENTVAR_GetValueIfExist( BtlEvVarLabel label, int* value );




//------------------------------------------------------------------------------------------
/**
* イベントハンドラ用ワーク型＆定数
*/
//------------------------------------------------------------------------------------------

/**
 *  ワザ出し失敗ハンドラ用ワーク
 */
typedef struct {

  /** 各種効果の有効bitフラグ */
  union {
    u8 bitFlags;
    struct {
      u8    flag_rankEffect : 1;  ///< ランク増減効果
      u8    flag_padding    : 7;
    };
  };

  WazaRankEffect  rankType;   ///< ランク効果種類
  s16             rankVolume; ///< ランク効果増減値

  u8              tokWinFlag; ///< とくせいウィンドウ出しフラグ

}BTL_EVWK_WAZAEXE_FAIL;

/**
 *  ワザダメージ->回復化チェックハンドラ用ワーク
 */
typedef struct {
  u8  pokeID;                 ///< 対象ポケモンID
  u8  tokFlag;                ///< とくせいウインドウ表示
  u16 recoverHP;              ///< 回復量

  WazaRankEffect  rankEffect; ///< 回復に加えランク効果が発生する場合に設定
  s16             rankVolume; ///< ランク効果増減値

}BTL_EVWK_DMG_TO_RECOVER;


/**
 *  ワザダメージ受けた後のハンドラ反応タイプ
 */
typedef enum {
  BTL_EV_AFTER_DAMAGED_REACTION_NONE,

  BTL_EV_AFTER_DAMAGED_REACTION_SICK,       ///< 対象ポケモンを状態異常にする
  BTL_EV_AFTER_DAMAGED_REACTION_RANK,       ///< 対象ポケモンのランク増減効果
  BTL_EV_AFTER_DAMAGED_REACTION_DAMAGE,     ///< 対象ポケモンにダメージを与える
  BTL_EV_AFTER_DAMAGED_REACTION_POKETYPE,   ///< 対象ポケモンのタイプ変更
  BTL_EV_AFTER_DAMAGED_REACTION_MAX,

}BtlEvAfterDamagedReactionType;

/**
 *  ワザダメージ受けた後のハンドラワーク
 */
typedef struct {

  BtlEvAfterDamagedReactionType   reaction; ///< 反応タイプ

  u8  damagedPokeID;    ///< ダメージを受けたポケモンID
  u8  targetPokeID;     ///< 効果対象ポケモンID
  u8  tokFlag;          ///< とくせいウインドウ表示
  u8  itemUseFlag;      ///< アイテム使用フラグ

  WazaSick        sick;       ///< 対象ポケモンに与える状態異常効果
  BTL_SICKCONT    sickCont;   ///< 対象ポケモンに与える状態異常の継続パターン
  pml::PokeType        pokeType;   ///< 対象ポケモンを変化させるタイプ
  WazaRankEffect  rankType;   ///< 対象ポケモンに与えるランク効果タイプ
  int             rankVolume; ///< 対象ポケモンに与えるランク効果値
  u16             damage;     ///< 対象ポケモンに与えるダメージ量

}BTL_EVWK_DAMAGE_REACTION;


/**
 *  状態異常チェックハンドラの反応タイプ
 */
typedef enum {

  BTL_EV_SICK_REACTION_NONE = 0,  ///< 反応なし
  BTL_EV_SICK_REACTION_DISCARD,   ///< 効果を打ち消す
  BTL_EV_SICK_REACTION_REFRECT,   ///< 相手にもうつす

}BtlEvSickReaction;

/**
 *  状態異常チェックハンドラ用ワーク
 */
typedef struct {

  BtlEvSickReaction   reaction;           ///< 反応タイプ
  WazaSick            discardSickType;    ///< 特定の状態異常のみ打ち消す場合に指定
  u8                  fDiscardByTokusei;  ///< とくせいによる打ち消しならTRUEにする
  u8                  discardPokeID;      ///< 打ち消したポケモンID
  u8                  fItemResponce;      ///< アイテム使用する場合TRUEにする

}BTL_EVWK_ADDSICK;

/**
 *  ランク効果チェックハンドラ用ワーク
 */
typedef struct {

  u8              failFlag;           ///< とにかく失敗
  u8              failTokuseiFlag;    ///< 対象ポケモンのとくせいによる失敗
  u8              useItemFlag;        ///< アイテム発動フラグ
  WazaRankEffect  failSpecificType;   ///< 特定の効果のみ失敗させるなら指定

}BTL_EVWK_CHECK_RANKEFF;

/**
 *  相性チェックハンドラ用ワーク
 */
typedef struct {

  BtlTypeAff      aff;              ///< 相性ID
  u8              weakedByItem;     ///< アイテムによって相性が弱められている
  u8              weakReserveByItem;///< アイテムによってダメージ値を半減させる
  u16             weakedItemID;     ///< 弱めるために使われたアイテム

}BTL_EVWK_CHECK_AFFINITY;



/**
 * @brief 技ターゲット誘導の要因
 */
enum TemptTargetCause
{
  TEMPT_TARGET_CAUSE_NONE,      // 誘導されていない
  TEMPT_TARGET_CAUSE_YOBIMIZU,  // 特性「よびみず」
  TEMPT_TARGET_CAUSE_HIRAISIN,  // 特性「ひらいしん」
  TEMPT_TARGET_CAUSE_OTHER,     // そのた
};

/**
 * @brief 技ターゲット誘導の優先度
 *
 * 同時に複数の要因によるターゲット誘導が発生した場合、より優先度が高い方が優先されます。
 * 優先度が同じ場合、先勝ちになります( イベントファクターの優先度が高い方が優先されます )。
 */
enum TemptTargetPriority
{
  TEMPT_TARGET_PRIORITY_NULL,      // 誘導されていない状態
  TEMPT_TARGET_PRIORITY_TOKUSEI,   // 特性( よびみず・ひらいしん )
  TEMPT_TARGET_PRIORITY_WAZA_1,    // 技( このゆびとまれ・いかりのこな・フリーフォール )
  TEMPT_TARGET_PRIORITY_WAZA_2,    // 技( スポットライト )
  TEMPT_TARGET_PRIORITY_AURA,      // オーラ効果
};


//--------------------------------------------------------------
/**
* イベントファクタータイプ
*/
//--------------------------------------------------------------
typedef enum {

  BTL_EVENT_FACTOR_WAZA,
  BTL_EVENT_FACTOR_POS,
  BTL_EVENT_FACTOR_SIDE,
  BTL_EVENT_FACTOR_FIELD,
  BTL_EVENT_FACTOR_TOKUSEI,
  BTL_EVENT_FACTOR_ITEM,
  BTL_EVENT_FACTOR_ZENRYOKUWAZA,
  BTL_EVENT_FACTOR_DEFAULT_POWERUP,

  BTL_EVENT_FACTOR_ISOLATE, // あらゆるタイプのハンドラからコンバートされる特殊タイプで、
                            // ポケモン・アイテムに依存しないので、死んだりアイテムを消費しても実行される。
                            // 毎ターン終了時にまとめて消去される。

  BTL_EVENT_FACTOR_MAX,

}BtlEventFactorType;

/**
 *  イベント処理順序
 */
typedef enum {

  BTL_EVPRI_DEFAULT_POWERUP,

  BTL_EVPRI_ZENRYOKUWAZA_DEFAULT,

  BTL_EVPRI_WAZA_DEFAULT,

  BTL_EVPRI_POS_DEFAULT,

  BTL_EVPRI_SIDE_DEFAULT,

  BTL_EVPRI_FIELD_DEFAULT,

    BTL_EVPRI_TOKUSEI_dokusyu,
  BTL_EVPRI_TOKUSEI_DEFAULT,

  BTL_EVPRI_ITEM_DEFAULT,
    BTL_EVPRI_TOKUSEI_atodasi,

  BTL_EVPRI_MAX,

}BtlEventPriority;


extern void EVENT_InitSystem( void );
extern void EVENT_StartTurn( void );
extern void EVENT_CallHandlers( ServerFlow* flowWork, BtlEventType type );
extern void EVENT_ForceCallHandlers( ServerFlow* flowWork, BtlEventType eventID );
extern void EVENT_CallHandlersTargetType( ServerFlow* flowWork, BtlEventType eventID, BtlEventFactorType type );
extern void EVENT_RemoveIsolateFactors( void );
extern void EVENT_ReOrderFactorsByPokeAgility( ServerFlow* flowWork );
extern void EVENT_SleepFactorMagicMirrorUser( u16 pokeID );
extern void EVENT_WakeFactorMagicMirrorUser( u16 pokeID );


extern void EVENT_SleepFactorMagicMirrorUser( u16 pokeID );
extern void EVENT_WakeFactorMagicMirrorUser( u16 pokeID );

extern void EVENT_SleepFactorRotation( u16 pokeID, BtlEventFactorType factorType );
extern bool EVENT_WakeFactorRotation( u16 pokeID, BtlEventFactorType factorType );




GFL_NAMESPACE_END(btl)

#endif // __BTL_EVENT_H__
