#if !defined __BTL_SERVER_CONST_H__
#define      __BTL_SERVER_CONST_H__

#pragma once
#include <pml/include/pml_Waza.h>
#include "./btl_common.h"

GFL_NAMESPACE_BEGIN(btl)


//--------------------------------------------------------------
/**
 *  アダプターコマンド
 */
//--------------------------------------------------------------
enum AdapterCommand
{
  ACMD_NONE = 0,
  ACMD_WAIT_SETUP,
  ACMD_SELECT_ACTION,
  ACMD_CONFIRM_IREKAE,
  ACMD_SELECT_POKEMON_FOR_COVER,
  ACMD_SELECT_POKEMON_FOR_CHANGE,
  ACMD_SELECT_CHANGE_OR_ESCAPE,
  ACMD_SERVER_CMD,
  ACMD_QUIT_BTL,
  ACMD_RECORD_DATA,
  ACMD_EXIT_NPC,
  ACMD_EXIT_SUBWAY_TRAINER,
  ACMD_EXIT_COMM,
  ACMD_EXIT_WIN_WILD,
  ACMD_EXIT_LOSE_WILD,
  ACMD_EXIT_CAPTURE,
  ACMD_NOTIFY_TIMEUP,
  ACMD_FADEOUT,
  ACMD_SEND_LATEST_REPLY,
  ACMD_CLIENT_LIMIT_TIME_SEND,
  ACMD_CLIENT_LIMIT_TIME_SYNC,

  BTL_ACMD_MAX,
};



enum {
  BTL_SERVERCMD_QUE_SIZE = 6000,
  BTL_SERVERCMD_ARG_MAX = 16,
};

//--------------------------------------------------------------
/**
 *    サーバコマンド
 */
//--------------------------------------------------------------
typedef enum {
  SC_NULL = 0,

  SC_OP_HP_MINUS,                              ///< HPマイナス  [ClientID, マイナス量]
  SC_OP_HP_MINUS_SYNC_WAZAEFF,                 ///< HPマイナス （ワザエフェクト処理に同期する） [ClientID, マイナス量]
  SC_OP_HP_PLUS,                               ///< HPプラス    [ClientID, プラス量]
  SC_OP_HP_ZERO,                               ///< HP0 にする [ pokeID ]
  SC_OP_PP_MINUS,                              ///< PPマイナス （仮ワザワーク用）
  SC_OP_PP_MINUS_ORG,                          ///< PPマイナス （真ワザワーク用）
  SC_OP_WAZA_USED,                             ///< ワザ使用記録
  SC_OP_PP_PLUS,                               ///< PPプラス   （仮ワザワーク用）
  SC_OP_PP_PLUS_ORG,                           ///< PPプラス   （真ワザワーク用）
  SC_OP_RANK_UP,                               ///< ステータスランクアップ  [ClientID, StatusType, プラス量]
  SC_OP_RANK_DOWN,                             ///< ステータスランクダウン  [ClientID, StatusType, マイナス量]
  SC_OP_RANK_SET8,                             ///< ステータスランク８種セット[ pokeID, atk, def, sp_atk, sp_def, agi  ]
  SC_OP_RANK_RECOVER,                          ///< ステータスランク（７種）下がっているもののみフラットに
  SC_OP_RANK_UP_RESET,                         ///< ステータスランク（７種）上がっているもののみフラットに
  SC_OP_RANK_RESET,                            ///< ステータスランク（７種）全てをフラットに
  SC_OP_ADD_CRITICAL,                          ///< クリティカルランク加算[ pokeID, (int)value ]
  SC_OP_SICK_SET,                              ///< 状態異常 [PokeID, Sick, contParam]
  SC_OP_CURE_POKESICK,                         ///< ポケモン系状態異常を回復 [PokeID ]
  SC_OP_CURE_WAZASICK,                         ///< ワザ系状態異常を回復 [PokeID, SickID ]
  SC_OP_MEMBER_IN,                             ///< メンバー入場
  SC_OP_SET_STATUS,                            ///< 能力値（攻撃、防御等）を書き換え
  SC_OP_SET_WEIGHT,                            ///< 体重設定
  SC_OP_WAZASICK_TURNCHECK,                    ///<
  SC_OP_CHANGE_POKETYPE,                       ///< 【計算】ポケモンのタイプ変更（ pokeID, type ）
  SC_OP_EX_POKETYPE,                           ///< 【計算】ポケモンのタイプ拡張（ pokeID, type ）
  SC_OP_CONSUME_ITEM,                          ///< 所有アイテム削除
  SC_OP_UPDATE_USE_WAZA,                       ///< ワザプロセス終了情報
  SC_OP_SET_CONTFLAG,                          ///< 場継続フラグセット
  SC_OP_RESET_CONTFLAG,                        ///< 場継続フラグリセット
  SC_OP_SET_TURNFLAG,                          ///< ターンフラグセット
  SC_OP_RESET_TURNFLAG,                        ///< ターンフラグ強制リセット
  SC_OP_SET_PERMFLAG,                          ///< 永続フラグセット
  SC_OP_INC_BATTLE_TURN_COUNT,                 ///< 経過ターンの総数をインクリメントする
  SC_OP_INC_POKE_TURN_COUNT,                   ///< 場に出たターンの総数をインクリメントする
  SC_OP_CHANGE_TOKUSEI,                        ///< とくせい書き換え
  SC_OP_SET_ITEM,                              ///< アイテム書き換え
  SC_OP_UPDATE_WAZANUMBER,                     ///< ワザ書き換え
  SC_OP_OUTCLEAR,                              ///< 退場時クリア
  SC_OP_ADD_FLDEFF,                            ///< フィールドエフェクト追加
  SC_OP_ADD_FLDEFF_DEPEND,                     ///< フィールドエフェクトの依存ポケを追加
  SC_OP_CHANGE_GROUND,                         ///< フィールドグラウンド効果の変更
  SC_OP_DEL_FLDEFF_DEPEND,                     ///< フィールドエフェクトの依存ポケを削除
  SC_OP_REMOVE_FLDEFF,                         ///< フィールドエフェクト削除
  SC_OP_SET_POKE_COUNTER,                      ///< ポケモンカウンタ値セット
  SC_OP_INC_KILLCOUNT,                         ///< 相手を倒した数をインクリメント
  SC_OP_BATONTOUCH,                            ///< バトンタッチ
  SC_OP_MIGAWARI_CREATE,                       ///< みがわり作成
  SC_OP_MIGAWARI_DELETE,                       ///< みがわり削除
  SC_OP_SET_ILLUSION,                          ///< イリュージョン用参照ポケモン変更
  SC_OP_CLEAR_CONSUMED_ITEM,                   ///< アイテム消費情報のクリア
  SC_OP_CURESICK_DEPEND_POKE,                  ///< ポケモン依存状態異常のクリア
  SC_OP_WAZADMG_REC,                           ///< ワザダメージ記録
  SC_OP_TURN_CHECK,                            ///< ポケモン１体ターンチェック
  SC_OP_TURN_CHECK_FIELD,                      ///< フィールド効果ターンチェック
  SC_OP_SET_DORYOKU,                           ///< 努力値加算[ hp, pow, def, agi, sp_pow, sp_agi ]
  SC_OP_START_POSEFF,                          ///< 位置エフェクト活動開始
  SC_OP_REMOVE_POSEFF,                         ///< 位置エフェクト活動終了
  SC_OP_POSEFFECT_UPDATE_PARAM,                ///< 位置エフェクトのパラメータを更新する
  SC_OP_PGL_RECORD,                            ///< PGL Record ( 防御ポケID, 攻撃ポケID, 攻撃ワザNo )
  SC_OP_SIDEEFFECT_ADD,                        ///< サイドエフェクトを追加する
  SC_OP_SIDEEFFECT_REMOVE,                     ///< サイドエフェクトを解除する
  SC_OP_SIDEEFFECT_INC_TURNCOUNT,              ///< サイドエフェクトの経過ターン数をインクリメントする
  SC_OP_PUBLISH_CLIENT_INFO_APPEARED_POKEMON,  ///< クライアント公開情報を更新する( ポケモンが場に出たことを記録 )
  SC_OP_PUBLISH_CLIENT_INFO_HAVE_POKEMON_ITEM, ///< クライアント公開情報を更新する( ポケモンが道具を持っているかどうか )
  SC_OP_SET_CURRENT_ROYAL_RANKING,             ///< バトルロイヤルの現在順位を更新する
  SC_OP_SET_ZENRYOKU_WAZA_USED,                ///< 全力技を使用したことを記録する
  SC_OP_SET_DIRT,                              ///< ポケモンに汚れをセットする
  SC_OP_INC_WEATHER_PASSED_TURN,               ///< 天候の経過ターン数をインクリメントする
  SC_OP_SET_SPACT_PRIORITY,                    ///< ポケモンの行動の特殊優先度を設定する
  SC_ACT_WAZA_EFFECT,                          ///< 技エフェクトを再生する
  SC_ACT_TAMEWAZA_HIDE,                        ///< 【アクション】そらをとぶなどで画面から消える・現れる設定
  SC_ACT_WAZA_DMG,                             ///< 【アクション】[ AtClient, DefClient, wazaIdx, Affinity ]
  SC_ACT_WAZA_DMG_PLURAL,                      ///< 【アクション】複数体同時ダメージ処理 [ pokeCnt, affAbout, ... ]
  SC_ACT_WAZA_ICHIGEKI,                        ///< 【アクション】一撃ワザ処理
  SC_ACT_SICK_ICON,                            ///< 【アクション】ゲージに状態異常アイコン表示
  SC_ACT_CONF_DMG,                             ///< 【アクション】こんらん自爆ダメージ [ pokeID ]
  SC_ACT_RANKUP,                               ///< 【ランクアップ効果】 ○○の×××があがった！[ ClientID, statusType, volume ]
  SC_ACT_RANKDOWN,                             ///< 【ランクダウン効果】 ○○の×××がさがった！[ ClientID, statusType, volume ]
  SC_ACT_DEAD,                                 ///< 【ポケモンひんし】
  SC_ACT_RELIVE,                               ///< 【ポケモン生き返り】
  SC_ACT_MEMBER_OUT_MSG,                       ///< 【ポケモン退場メッセージ】[ ClientID, pokeID ]
  SC_ACT_MEMBER_OUT,                           ///< 【ポケモン退場】[ ClientID, memberIdx ]
  SC_ACT_MEMBER_IN,                            ///< 【ポケモンイン】[ ClientID, posIdx, memberIdx ]
  SC_ACTOP_WEATHER_START,                      ///< 天候変化
  SC_ACTOP_WEATHER_END,                        ///< ターンチェックで天候終了
  SC_ACTOP_MEGAEVO,                            ///< メガ進化
  SC_ACT_SIMPLE_HP,                            ///< シンプルなHPゲージ増減処理
  SC_ACT_USE_ITEM,                             ///< アイテム使用
  SC_ACT_KINOMI_PRE_WAZA,                      ///< ワザ演出前の木の実演出
  SC_ACT_KILL,                                 ///< 強制瀕死演出（みちづれ、一撃ワザなど）
  SC_ACTOP_MOVE,                               ///< ムーブ
  SC_ACT_EXP,                                  ///< 経験値取得
  SC_ACT_NADERU,                               ///< なでる時間
  SC_ACT_BALL_THROW,                           ///< ボール投げ
  SC_ACT_BALL_THROW_CAPTURED,                  ///< ボール投げで捕獲成功後の追加コマンド
  SC_ACT_BALL_THROW_FORBIDDEN,                 ///< ボール投げ（トレーナー戦）
  SC_ACTOP_CHANGE_TOKUSEI,                     ///< とくせい変更
  SC_ACTOP_SWAP_TOKUSEI,                       ///< とくせいスワップ
  SC_ACT_FAKE_DISABLE,                         ///< イリュージョン解除
  SC_ACT_EFFECT_SIMPLE,                        ///< エフェクト発動（パラメータ指定なし）
  SC_ACT_EFFECT_BYPOS,                         ///< エフェクト発動（位置指定）
  SC_ACT_EFFECT_BYVECTOR,                      ///< エフェクト発動（発動位置＆終点位置を指定）
  SC_ACT_EFFECT_FIELD,                         ///< エフェクト発動（フィールド状態書き換え用）
  SC_ACT_PLURALHIT_EX_2ND,                     ///< 特殊条件により２回攻撃発生した時のヒットエフェクト２回目（おやこあいで使用）
  SC_ACT_CHANGE_FORM,                          ///< フォルムナンバーチェンジ
  SC_ACT_MIGAWARI_CREATE,                      ///< みがわり作成
  SC_ACT_MIGAWARI_DELETE,                      ///< みがわり消去
  SC_ACT_HENSIN,                               ///< へんしん
  SC_ACT_MIGAWARI_DAMAGE,                      ///< みがわりダメージ
  SC_ACT_WIN_BGM,                              ///< 勝利BGM再生
  SC_ACT_MSGWIN_HIDE,                          ///< メッセージウィンドウフェードアウト
  SC_ACT_FRIENDSHIP_EFFECT,                    ///< なかよし度発揮アクション
  SC_ACT_FRIENDSHIP_EFFECT_MSG,                ///< なかよし度発揮アクション＆メッセージ
  SC_ACT_DEAD_PLURAL,                          ///< ポケモンひんし（群れバトル時の複数体同時処理）
  SC_ACT_ZENRYOKUWAZA_BEGIN,                   ///< 全力技開始演出
  SC_ACT_ZENRYOKUWAZA_END,                     ///< 全力技終了演出
  SC_ACT_OTEIRE_BUTTON_DISPLAY,                ///< お手入れボタン表示
  SC_ACT_INTRUDE_CALL,                         ///< 乱入( 助けを呼ぶ演出 )
  SC_ACT_INTRUDE_APPEAR,                       ///< 乱入( 乱入ポケモンの登場演出 )
  SC_ACT_INTRUDE_NOT_APPEARD,                  ///< 乱入( 乱入ポケモンが来なかった場合の演出 )
  SC_ACT_TURN_START,                           ///< ターン開始時の演出
  SC_TOKWIN_IN,                                ///< とくせいウィンドウ表示イン [ClientID]
  SC_TOKWIN_OUT,                               ///< とくせいウィンドウ表示アウト [ClientID]
  SC_MSG_WAZA,                                 ///< ワザメッセージ表示[ ClientID, wazaIdx ]
  SC_MSG_STD,                                  ///< メッセージ表示 [MsgID, numArgs, arg1, arg2, ... ]
  SC_MSG_SET,                                  ///< メッセージ表示 [MsgID, numArgs, arg1, arg2, ... ]
  SC_MSG_STD_SE,                               ///< メッセージ表示＆SE [MsgID, SENo, numArgs, arg1, arg2, ... ]
  SC_MSG_SET_SE,                               ///< メッセージ表示＆SE [MsgID, SENo, numArgs, arg1, arg2, ... ]

  SC_MAX,

  SCEX_RESERVE,
  SCEX_VALIABLE_ARGS,         ///< 可変個引数の追加
  SCEX_WAZAEFF_SYNC_CHAPTER,
  SCEX_SKIP,

}ServerCmd;



//--------------------------------------------------------------
/**
 * @brief サーバーとクライアント間のやり取り一覧
 */
//--------------------------------------------------------------
enum ServerRequest
{
  SERVER_REQ_START_SETUP,                               // 戦闘開始直後のセットアップを開始
  SERVER_REQ_START_ACTION_SELECT,                       // 行動選択を開始
  SERVER_REQ_START_CHANGE_OR_ESCAPE,                    // 逃げる、次を出す　選択を開始
  SERVER_REQ_START_COVER_POKE_SELECT,                   // 場に投入するカバーポケモンの選択を開始
  SERVER_REQ_START_SELECT_POKE_BY_INTERVAL_POKECHANGE,  // ターン途中での入れ替えポケモン選択を開始
  SERVER_REQ_START_CONFIRM_COUNTER_POKECHANGE,          // 相手のカバーポケモン投入に対して、後だしで入れ替えを行うかどうか確認

  SERVER_REQ_SERVER_COMMAND_FIRST_BATTLE_IN,            // サーバーコマンド再生( 初期ポケモンの投入 )
  SERVER_REQ_SERVER_COMMAND_AFTER_ACTION_SELECT,        // サーバーコマンド再生( 行動選択後のターン進行 )
  SERVER_REQ_SERVER_COMMAND_AFTER_COVER_POKE_SELECT,    // サーバーコマンド再生( カバーポケモンの選択後のターン進行 )
  SERVER_REQ_SERVER_COMMAND_AFTER_INTERVAL_POKECHANGE,  // サーバーコマンド再生( ターン途中での入れ替え後のターン進行 )
  SERVER_REQ_SERVER_COMMAND_ESCAPE_BY_CHANGE_OR_ESCAPE, // サーバーコマンド再生( 逃げる、次を出す　選択にて「逃げる」を選択した後 )
  SERVER_REQ_SERVER_COMMAND_INTRUDE,                    // サーバーコマンド再生( 乱入ポケモン入場 )

  SERVER_REQ_RECORD_BATTLE_START_TIMING,                // データ記録( バトル開始タイミングコード )
  SERVER_REQ_RECORD_BATTLE_START_CHAPTER,               // データ記録( バトル開始チャプタ )
  SERVER_REQ_RECORD_SELECTED_ACTION,                    // データ記録( 行動選択の結果 )
  SERVER_REQ_RECORD_COVER_POKE_SELECT,                  // データ記録( カバーポケモンの選択結果 )
  SERVER_REQ_RECORD_INTERVAL_POKECHANGE,                // データ記録( ターン途中での入れ替え結果 )
  SERVER_REQ_RECORD_TIMEUP_CHAPTER,                     // データ記録( 制限時間終了チャプタ )

  SERVER_REQ_NOTIFY_TIMEUP,                             // 制限時間終了を通知
  SERVER_REQ_EXIT_WILD_WIN,                             // 野生戦を勝ちで終了
  SERVER_REQ_EXIT_WILD_LOSE,                            // 野生戦を負けで終了
  SERVER_REQ_EXIT_WILD_CAPTURE,                         // 野生戦を捕獲で終了
  SERVER_REQ_EXIT_COMM,                                 // 通信対戦を終了
  SERVER_REQ_EXIT_NPC,                                  // NPC戦を終了
  SERVER_REQ_EXIT_BATTLE_SPOT_TRAINER,                  // バトル施設トレーナー戦を終了
  SERVER_REQ_EXIT_FADEOUT,                              // 戦闘終了時のフェードアウト
  SERVER_REQ_QUIT_BATTLE,                               // 戦闘終了

  SERVER_REQ_SEND_LATEST_REPLY,                         // 最後にサーバーに返信したデータを再送信
  SERVER_REQ_SEND_CLIENT_LIMIT_TIME,                    // 残り手持ち時間を送信してもらう
  SERVER_REQ_SYNC_CLIENT_LIMIT_TIME,                    // 残り手持ち時間を送信する

  SERVER_REQ_NUM,
};


//--------------------------------------------------------------
/**
 * @brief サーバーのシーケンス
 */
//--------------------------------------------------------------
enum ServerSequence
{
  // 戦闘開始
  SERVER_SEQ_BATTLE_START_SETUP,                    // 戦闘開始　セットアップ                    ( ACMD_WAIT_SETUP  送信　→　クライアントからの返信待ち )
  SERVER_SEQ_BATTLE_START_SWITCH,                   // 戦闘開始　セットアップ後の分岐
  SERVER_SEQ_BATTLE_START_TIMING,                   // 戦闘開始　戦闘開始時のタイミングコード送信( ACMD_RECORD_DATA 送信　→　クライアントからの返信待ち )
  SERVER_SEQ_BATTLE_START_COMMAND,                  // 戦闘開始　初期ポケモン投入コマンド        ( ACMD_SERVER_CMD  送信　→　クライアントからの返信待ち )
  SERVER_SEQ_BATTLE_START_RECORD,                   // 戦闘開始　録画チャプター記録              ( ACMD_RECORD_DATA 送信　→　クライアントからの返信待ち )

  // 行動選択
  SERVER_SEQ_ACTION_SELECT_START,                   // 行動選択　行動選択開始                  ( ACMD_SELECT_ACTION 送信　→　クライアントからの返信待ち )
  SERVER_SEQ_ACTION_SELECT_SWITCH,                  // 行動選択　行動選択開始後の分岐
  SERVER_SEQ_ACTION_SELECT_RECORD,                  // 行動選択　行動選択の結果を記録          ( ACMD_RECORD_DATA   送信　→　クライアントからの返信待ち )
  SERVER_SEQ_ACTION_SELECT_COMMAND,                 // 行動選択　行動選択後のターン再生コマンド( ACMD_SERVER_CMD    送信　→　クライアントからの返信待ち )
  SERVER_SEQ_ACTION_SELECT_COMMAND_SWITCH,          // 行動選択　ターン再生後の分岐

  // 逃げる、次を出す　選択
  SERVER_SEQ_CHANGE_OR_ESCAPE_START,                // 逃げる、次を出す　選択開始                       ( ACMD_SELECT_CHANGE_OR_ESCAPE 送信　→　クライアントからの返信待ち )
  SERVER_SEQ_CHANGE_OR_ESCAPE_SWITCH,               // 逃げる、次を出す　選択後の分岐
  SERVER_SEQ_CHANGE_OR_ESCAPE_ESCAPE_SELECTED,      // 逃げる、次を出す　「逃げる」を選択した場合の分岐( ACMD_SERVER_CMD              送信　→　クライアントからの返信待ち )

  // カバーポケモン投入
  SERVER_SEQ_COVER_SELECT_START,                    // カバーポケモン投入　投入ポケモン選択開始                ( ACMD_SELECT_POKEMON_FOR_COVER 送信　→　クライアントからの返信待ち )
  SERVER_SEQ_COVER_COMFIRM_PLAYER_POKECHANGE,       // カバーポケモン投入　プレイヤへのポケモン入れ替え確認    ( ACMD_CONFIRM_IREKAE           送信　→　クライアントからの返信待ち )
  SERVER_SEQ_COVER_SELECT_SWITCH,                   // カバーポケモン投入　投入ポケモン選択後の分岐            
  SERVER_SEQ_COVER_SELECT_RECORD,                   // カバーポケモン投入　投入ポケモン選択結果を記録          ( ACMD_RECORD_DATA              送信　→　クライアントからの返信待ち )
  SERVER_SEQ_COVER_COMMAND,                         // カバーポケモン投入　ポケモン投入後のコマンド            ( ACMD_SERVER_CMD               送信　→　クライアントからの返信待ち )
  SERVER_SEQ_COVER_COMMAND_SWITCH,                  // カバーポケモン投入　ポケモン投入後のコマンド再生後の分岐

  // ターン途中のポケモン入れ替え
  SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT,        // ターン途中のポケモン入れ替え　投入ポケモンの選択                                             ( ACMD_SELECT_POKEMON_FOR_CHANGE 送信　→　クライアントからの返信待ち )
  SERVER_SEQ_INTERVAL_POKECHANGE_POKESELECT_SWITCH, // ターン途中のポケモン入れ替え　投入ポケモン選択後の分岐                                       
  SERVER_SEQ_INTERVAL_POKECHANGE_RECORD,            // ターン途中のポケモン入れ替え　投入ポケモンの選択結果を記録                                   ( ACMD_RECORD_DATA               送信　→　クライアントからの返信待ち )
  SERVER_SEQ_INTERVAL_POKECHANGE_COMMAND,           // ターン途中のポケモン入れ替え　ターン途中のポケモン入れ替え後のターン再生コマンド             ( ACMD_SERVER_CMD                送信　→　クライアントからの返信待ち )
  SERVER_SEQ_INTERVAL_POKECHANGE_SWITCH,            // ターン途中のポケモン入れ替え　ターン途中のポケモン入れ替え後のターン再生コマンド再生後の分岐

  // 乱入
  SERVER_SEQ_INTRUDE_PREPARE,                       // 乱入　準備
  SERVER_SEQ_INTRUDE_COMMAND,                       // 乱入　入場コマンド

  // 制限時間終了
  SERVER_SEQ_TIMEUP_RECORD,                         // 制限時間終了　録画チャプター記録( ACMD_RECORD_DATA   送信　→　クライアントからの返信待ち )
  SERVER_SEQ_TIMEUP_COMMAND,                        // 制限時間終了　終了コマンド      ( ACMD_NOTIFY_TIMEUP 送信　→　クライアントからの返信待ち )

  // 戦闘終了
  SERVER_SEQ_EXIT_COMMON,                           // 戦闘終了　共通処理
  SERVER_SEQ_EXIT_SWITCH,                           // 戦闘終了　対戦相手ごとの分岐
  SERVER_SEQ_EXIT_NPC,                              // 戦闘終了　NPC                 ( ACMD_EXIT_NPC            送信　→　クライアントからの返信待ち )
  SERVER_SEQ_EXIT_BATTLE_SPOT_TRAINER,              // 戦闘終了　バトル施設トレーナー( ACMD_EXIT_SUBWAY_TRAINER 送信　→　クライアントからの返信待ち )
  SERVER_SEQ_EXIT_COMM,                             // 戦闘終了　通信対戦            ( ACMD_EXIT_COMM           送信　→　クライアントからの返信待ち )
  SERVER_SEQ_EXIT_WILD_WIN,                         // 戦闘終了　野生戦勝ち          ( ACMD_EXIT_WIN_WILD       送信　→　クライアントからの返信待ち )
  SERVER_SEQ_EXIT_WILD_LOSE,                        // 戦闘終了　野生戦負け          ( ACMD_EXIT_LOSE_WILD      送信　→　クライアントからの返信待ち )
  SERVER_SEQ_EXIT_WILD_CAPTURE,                     // 戦闘終了　野生戦捕獲          ( ACMD_EXIT_CAPTURE_WILD   送信　→　クライアントからの返信待ち )
  SERVER_SEQ_EXIT_KEY_WAIT,                         // 戦闘終了　キー入力待ち
  SERVER_SEQ_EXIT_FADEOUT,                          // 戦闘終了　フェードアウト      ( ACMD_FADEOUT             送信　→　クライアントからの返信待ち )
  SERVER_SEQ_EXIT_QUIT,                             // 戦闘終了　終了コマンド発行    ( ACMD_QUIT_BTL            送信　→　クライアントからの返信待ち )

  // 新サーバーとしてのスタートアップ
  SERVER_SEQ_NEW_STARTUP_SEND_LATEST_REPLY,         // 新サーバーとしてのスタートアップ　最後に返信したデータを再送信してもらう
  SERVER_SEQ_NEW_STARTUP_SWITCH,                    // 新サーバーとしてのスタートアップ　再送信してもらったデータを元にシーケンス遷移する

  // おわり
  SERVER_SEQ_END,

  // 総数
  SERVER_SEQ_NUM,

  // 無効値
  SERVER_SEQ_NULL = SERVER_SEQ_NUM,
};




//--------------------------------------------------------------
/**
 *    サーバキュー構造
 */
//--------------------------------------------------------------
typedef struct {
  enum {
    INVALID_PTR_VALUE = 0xffff,
  };
  u32   writePtr;
  u32   readPtr;
  u8    buffer[ BTL_SERVERCMD_QUE_SIZE ];

  u16   CalcCRC( void ) const
  {
    u16  crc = 0xffff;
    for(u32 i=0; i<writePtr; ++i)
    {
      crc ^= ((u16)(buffer[i]) << 8);
      for(u32 j=0; j<8; ++j)
      {
        if( crc & 0x8000 ){
          crc <<= 1;
          crc ^= 0x1021;
        }else{
          crc <<= 1;
        }
      }
    }
    return crc;
  }
}BTL_SERVER_CMD_QUE;

typedef BTL_SERVER_CMD_QUE  SCQUE;


/**
 *  状態異常指定の拡張コード
 */
typedef enum {

  // 全ての WazaSick コードも対象となります

  WAZASICK_EX_POKEFULL = pml::wazadata::WAZASICK_MAX,  ///< 全ポケモン系異常が対象
  WAZASICK_EX_POKEFULL_PLUS,                           ///< 全ポケモン系異常＋こんらんも対象
  WAZASICK_EX_MENTAL,

}BtlWazaSickEx;

/**
 *  状態異常上書きモード
 */
typedef enum {
  BTL_SICK_OW_NONE = 0,   ///< 上書きできない
  BTL_SICK_OW_POKESICK,   ///< 他の基本状態異常にかかっている時、無視して書き換え
  BTL_SICK_OW_STRONG,     ///< なんでも上書き
}BtlSickOverWriteMode_tag;

typedef u8 BtlSickOverWriteMode;

/**
 *  アイテム反応リクエストタイプ
 */
typedef enum {
  BTL_ITEMREACTION_HP     = (1<<0),      ///< HP減少による
  BTL_ITEMREACTION_PP     = (1<<1),      ///< PP減少による
  BTL_ITEMREACTION_SICK   = (1<<2),      ///< 状態異常による
  BTL_ITEMREACTION_RENEW  = (1<<3),      ///< 所持アイテム更新による

  BTL_ITEMREACTION_GEN  = (BTL_ITEMREACTION_HP|BTL_ITEMREACTION_PP|BTL_ITEMREACTION_SICK), ///< 汎用
  BTL_ITEMREACTION_FULL = (BTL_ITEMREACTION_GEN|BTL_ITEMREACTION_RENEW),                   ///< 全部入り
}BtlItemReactionBit;

typedef u8 BtlItemReaction;


/**
 *  HandEx 戻り値
 */
typedef enum {
  HandExResult_NULL,    ///< 何も反応なし
  HandExResult_Fail,    ///< 反応あったが、効果は無かった
  HandExResult_Enable,  ///< 反応もあり、効果もあった
}HandExResult;



/**
 *  サーバフロー＆ハンドラから参照するワザパラメータ
 */

typedef struct {

  WazaID           wazaID;
  WazaID           orgWazaID;    ///< 派生元のワザID
  PokeTypePair     userType;
  pml::PokeType    wazaType;
  WazaDamageType   damageType;
  bool             touchFlag;    ///< 接触技か？
  WazaTarget       targetType;
  int              wazaPri;
  u16              wazaPower;        ///< 実行する技の威力
  ZenryokuWazaKind zenryokuWazaKind; ///< 実行する技が該当する全力技の種類

  union {
    u32 flags;

    struct {
      u32             fMagicCoat     : 1; ///< マジックコート処理中
      u32             fYokodori      : 1; ///< よこどり処理中
      u32             fReqWaza       : 1; ///< 派生ワザ処理中
      u32             fItazuraGokoro : 1; ///< いたずらごころ　により優先度が高まった
      u32             _padding       : 28;
    };
  };

}SVFL_WAZAPARAM;

typedef enum {

  TRITEM_RESULT_NORMAL = 0, ///< 通常道具効果発生
  TRITEM_RESULT_ESCAPE,     ///< 強制的に逃げる
  TIITEM_RESULT_CAPTURED,   ///< 野生ポケモン捕獲成功

}TrItemResult;




/**
 * @brief ランク効果の原因コード
 */
enum RankEffectCause
{
  RANKEFFECT_CAUSE_OTHER,      // その他
  RANKEFFECT_CAUSE_IKAKU,      // いかく
  RANKEFFECT_CAUSE_NUMENUME,   // ぬめぬめ・カーリーヘアー
  RANKEFFECT_CAUSE_KINGSHIELD, // キングシールド
};


/**
 * @brief ダメージの原因コード
 */
enum DamageCause
{
  DAMAGE_CAUSE_OTHER,                  // その他
  DAMAGE_CAUSE_WEATHER,                // 天候ダメージ
  DAMAGE_CAUSE_KONRAN,                 // 混乱による自爆ダメージ
  DAMAGE_CAUSE_WAZA,                   // 技によるダメージ
  DAMAGE_CAUSE_WAZA_KICKBACK,          // 技の反動ダメージ
  DAMAGE_CAUSE_ICHIGEKI,               // 一撃必殺技
  DAMAGE_CAUSE_NEEDLEGUARD,            // 技「ニードルガード」
  DAMAGE_CAUSE_HAJIKERUHONOO_SPARKLE,  // 技「はじけるほのお」の火の粉
  DAMAGE_CAUSE_JIBAKU,                 // 技「じばく」「だいばくはつ」による自滅
  DAMAGE_CAUSE_OKIMIYAGE,              // 技「おきみやげ」による自滅
  DAMAGE_CAUSE_MIKAZUKINOMAI,          // 技「みかづきのまい」による自滅
  DAMAGE_CAUSE_IYASINONEGAI,           // 技「いやしのねがい」による自滅
  DAMAGE_CAUSE_MITIDURE,               // 技「みちづれ」
  DAMAGE_CAUSE_INOTIGAKE,              // 技「いのちがけ」による自滅
  DAMAGE_CAUSE_MIGAWARI,               // 技「みがわり」作成時のHP消費
  DAMAGE_CAUSE_NOROI,                  // ゴーストタイプが技「のろい」を使用した際のHP消費
  DAMAGE_CAUSE_ITAMIWAKE,              // 技「いたみわけ」によるHP増減
  DAMAGE_CAUSE_HARADAIKO,              // 技「はらだいこ」使用時のHP消費
  DAMAGE_CAUSE_WARUAGAKI,              // 技「わるあがき」の反動ダメージ
  DAMAGE_CAUSE_TOBIGERI_FAILED,        // 技「とびげり」「とびひざげり」が外れた時の自爆ダメージ
  DAMAGE_CAUSE_SAMEHADA,               // 特性「さめはだ」
  DAMAGE_CAUSE_TETUNOTOGE,             // 特性「てつのとげ」
  DAMAGE_CAUSE_YUUBAKU,                // 特性「ゆうばく」
  DAMAGE_CAUSE_SUNPOWER,               // 特性「サンパワー」天候「はれ」のときに受けるダメージ
  DAMAGE_CAUSE_KANSOUHADA,             // 特性「かんそうはだ」天候「はれ」のときに受けるダメージ
  DAMAGE_CAUSE_NIGHTMARE,              // 特性「ナイトメア」
  DAMAGE_CAUSE_HEDOROEKI,              // 特性「ヘドロえき」
  DAMAGE_CAUSE_TOBIDASUNAKAMI,         // 特性「とびだすなかみ」
  DAMAGE_CAUSE_JAPO_RENBU,             // 道具「ジャポのみ」「レンブのみ」
  DAMAGE_CAUSE_INOTINOTAMA,            // 道具「いのちのたま」
  DAMAGE_CAUSE_KUROIHEDORO,            // 道具「くろいヘドロ」
  DAMAGE_CAUSE_KUTTUKIBARI,            // 道具「くっつきばり」
  DAMAGE_CAUSE_GOTUGOTUMETTO,          // 道具「ゴツゴツメット」
  DAMAGE_CAUSE_STEALTHROCK,            // サイドエフェクト「ステルスロック」
  DAMAGE_CAUSE_MAKIBISI,               // サイドエフェクト「まきびし」
  DAMAGE_CAUSE_BURNING,                // サイドエフェクト 合体技の効果「火の海」
  DAMAGE_CAUSE_SICK_YADORIGINOTANE,    // 状態異常「やどりぎのたね」
  DAMAGE_CAUSE_SICK_BIND,              // 状態異常「バインド」(「まきつく」等のターンチェック時のダメージ )
  DAMAGE_CAUSE_SICK_DOKU,              // 状態異常( どく )
  DAMAGE_CAUSE_SICK_YAKEDO,            // 状態異常( やけど )
  DAMAGE_CAUSE_SICK_AKUMU,             // 状態異常( あくむ )
  DAMAGE_CAUSE_SICK_NOROI,             // 状態異常( のろい )
  DAMAGE_CAUSE_SICK_HOROBINOUTA,       // 状態異常( ほろびのうた )
  DAMAGE_CAUSE_SICK_FUNJIN,            // 状態異常(「ふんじん」発動時のダメージ )
};


/**
 * @brief 状態異常の原因コード
 */
enum SickCause
{
  SICK_CAUSE_OTHER,                  // その他
  SICK_CAUSE_TOKUSEI_TOUCH_COUNTER,  // 接触技に対するカウンター特性( せいでんき、ほのおのからだ等 )
  SICK_CAUSE_TOOTIKA,                // トーチカ
  SICK_CAUSE_WAZA_EFFECT_SICK,       // カテゴリ「エフェクトシック」の技による直接効果
};


/**
 * @brief 特性変更の原因コード
 */
enum TokuseiChangeCause
{
  TOKUSEI_CHANGE_CAUSE_OTHER, // その他
  TOKUSEI_CHANGE_CAUSE_MIIRA, // ミイラ
};



GFL_NAMESPACE_END(btl)
#endif // __BTL_SERVER_CONST_H__
