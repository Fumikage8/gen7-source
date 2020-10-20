#if !defined( __BTL_HEMANAGER_H__ )
#define __BTL_HEMANAGER_H__
//=============================================================================================
/**
 * @file    btl_Hemanager.h
 * @brief   ポケモンXY バトルシステム サーバコマンド生成ハンドラの反応処理プロセスマネージャ
 * @author  taya
 *
 * @date    2011.01.12  作成
 */
//=============================================================================================
#pragma once

#include  "./btl_common.h"
#include  "./btl_pokeparam.h"
#include  "./btl_SickCont.h"
#include  "./btl_Field.h"
#include  "./btl_str.h"
#include  "./btl_SideEff.h"
#include  "./btl_PosEff.h"
#include  "./btl_PosEffect.h"
#include  "./btl_server_const.h"
#include  "./btl_ACTION_ORDER_WORK.h"

GFL_NAMESPACE_BEGIN(btl)

/**
 *  ハンドラ挙動（効果が表出するもの）
 */
typedef enum {

  BTL_HANDEX_USE_ITEM,            ///< アイテム使用
  BTL_HANDEX_TOKWIN_IN,           ///< とくせいウィンドウイン
  BTL_HANDEX_TOKWIN_OUT,          ///< とくせいウィンドウアウト
  BTL_HANDEX_MESSAGE,             ///< メッセージ表示
  BTL_HANDEX_RECOVER_HP,          ///< HP回復
  BTL_HANDEX_DRAIN,               ///< HP回復（ドレイン系）
  BTL_HANDEX_DAMAGE,              ///< ダメージを与える
  BTL_HANDEX_SHIFT_HP,            ///< HPを変更（ダメージ・回復として扱わない）
  BTL_HANDEX_RECOVER_PP,          ///< PP回復
  BTL_HANDEX_DECREMENT_PP,        ///< PP減少
  BTL_HANDEX_CURE_SICK,           ///< 状態異常を回復
  BTL_HANDEX_ADD_SICK,            ///< 状態異常にする
  BTL_HANDEX_SET_SICKCONT,        ///< 状態異常のパラメータ再設定
  BTL_HANDEX_RANK_EFFECT,         ///< ランク増減効果
  BTL_HANDEX_SET_RANK,            ///< ランクを指定地に強制書き換え
  BTL_HANDEX_RESET_RANK,          ///< ランク効果を全てフラットに
  BTL_HANDEX_RESET_RANKUP,        ///< 上昇しているランク効果を全てフラットに
  BTL_HANDEX_SET_STATUS,          ///< 能力値（攻撃、防御等）を強制書き換え
  BTL_HANDEX_RECOVER_RANK,        ///< マイナスランク効果のみをフラットに
  BTL_HANDEX_KILL,                ///< 瀕死にする
  BTL_HANDEX_DEAD_CHECK,          ///< 対象が瀕死なら、退場処理を行う
  BTL_HANDEX_CHANGE_TYPE,         ///< ポケモンのタイプを変える
  BTL_HANDEX_EX_TYPE,             ///< ポケモンのタイプ拡張（３タイプ目を追加）
  BTL_HANDEX_SET_TURNFLAG,        ///< ターンフラグセット
  BTL_HANDEX_RESET_TURNFLAG,      ///< ターンフラグ強制リセット
  BTL_HANDEX_SET_CONTFLAG,        ///< 継続フラグセット
  BTL_HANDEX_RESET_CONTFLAG,      ///< 継続フラグリセット
  BTL_HANDEX_SET_PERMFLAG,        ///< 永続フラグセット
  BTL_HANDEX_SIDEEFF_ADD,         ///< サイドエフェクト追加
  BTL_HANDEX_SIDEEFF_REMOVE,      ///< サイドエフェクト削除
  BTL_HANDEX_SIDEEFF_SLEEP,       ///< サイドエフェクト一時停止・再開
  BTL_HANDEX_ADD_FLDEFF,          ///< フィールドエフェクト追加
  BTL_HANDEX_REMOVE_FLDEFF,       ///< フィールドエフェクト追加
  BTL_HANDEX_CHANGE_WEATHER,      ///< 天候変化
  BTL_HANDEX_POSEFF_ADD,          ///< 位置エフェクト追加
  BTL_HANDEX_CHANGE_TOKUSEI,      ///< とくせい書き換え
  BTL_HANDEX_SET_ITEM,            ///< アイテム書き換え
  BTL_HANDEX_CHECK_ITEM_EQUIP,    ///< アイテム装備効果発動チェック
  BTL_HANDEX_ITEM_SP,             ///< アイテム効果発動
  BTL_HANDEX_CONSUME_ITEM,        ///< 自分でアイテムを消費
  BTL_HANDEX_SWAP_ITEM,           ///< アイテム入れ替え
  BTL_HANDEX_UPDATE_WAZA,         ///< ワザ書き換え
  BTL_HANDEX_COUNTER,             ///< ポケモンカウンタ値書き換え
  BTL_HANDEX_DELAY_WAZADMG,       ///< 時間差ワザダメージ
  BTL_HANDEX_QUIT_BATTLE,         ///< バトル離脱
  BTL_HANDEX_CHANGE_MEMBER,       ///< メンバー交換
  BTL_HANDEX_BATONTOUCH,          ///< バトンタッチ（ランク効果等の引き継ぎ）
  BTL_HANDEX_ADD_SHRINK,          ///< ひるませる
  BTL_HANDEX_RELIVE,              ///< 生き返らせる
  BTL_HANDEX_SET_WEIGHT,          ///< 体重を設定
  BTL_HANDEX_PUSHOUT,             ///< 場から吹き飛ばす
  BTL_HANDEX_INTR_POKE,           ///< 指定ポケモンの割り込み行動を発生させる
  BTL_HANDEX_INTR_WAZA,           ///< 指定ワザを使おうとしているポケモンの割り込み行動を発生させる
  BTL_HANDEX_SEND_LAST,           ///< 指定ポケモンの行動を一番最後に回す
  BTL_HANDEX_SWAP_POKE,           ///< 場に出ているポケモン同士を入れ替える
  BTL_HANDEX_HENSIN,              ///< へんしんする
  BTL_HANDEX_FAKE_BREAK,          ///< イリュージョン解除
  BTL_HANDEX_JURYOKU_CHECK,       ///< じゅうりょく発動時のチェック処理
  BTL_HANDEX_TAMEHIDE_CANCEL,     ///< 特定の消え状態（そらをとぶなど）をキャンセル
  BTL_HANDEX_ADD_EFFECT,          ///< 位置指定してエフェクト発動
  BTL_HANDEX_VANISH_MSGWIN,       ///< メッセージウィンドウフェードアウト
  BTL_HANDEX_CHANGE_FORM,         ///< フォルムナンバーチェンジ
  BTL_HANDEX_SET_EFFECT_IDX,      ///< ワザエフェクトインデックス変更
  BTL_HANDEX_WAZAEFFECT_ENABLE,   ///< 強制ワザエフェクト有効
  BTL_HANDEX_FREEFALL,            ///< フリーフォール
  BTL_HANDEX_FRIENDSHIP_EFFECT,   ///< なかよし度エフェクト
  BTL_HANDEX_INSERT_WAZA_ACTION,  ///< 直後に技実行アクションを追加
  BTL_HANDEX_SET_DIRT,            ///< ポケモンに汚れをセット

  BTL_HANDEX_MAX,

//}BtlEventHandlerExhibition;
}EventHandlerExhibition;


/**
 *  ハンドラ挙動ワーク共有ヘッダ
 */
typedef struct {
  u32    equip          : 8; ///< ハンドラ挙動（BtlEventHandlerEquip）
  u32    userPokeID     : 5; ///< ハンドラ主体のポケモンID
  u32    size           :10; ///< 構造体サイズ
  u32    tokwin_flag    : 1; ///< 主体ポケモンのとくせい効果であることを表示する
  u32    failSkipFlag   : 1; ///< 直前の命令が失敗したらスキップされる
  u32    autoRemoveFlag : 1; ///< 主体ポケモンが死んでいたら自動削除
  u32    usingFlag      : 1; ///< 使用中フラグ
  u32    _padding       : 5; ///< パディング
}BTL_HANDEX_PARAM_HEADER;

/**
 *  ハンドラ用メッセージ出力パラメータ
 */
typedef struct {

  u16   ID;           ///< 文字列ID
  u16   type    : 7;  ///< 文字列タイプ（ BtlStrType  (btl_str.h) )
  u16   argCnt  : 7;  ///< 引数の数
  u16   fSEAdd  : 1;  ///< 引数終端にSEナンバー格納
  u16   fFailMsg : 1;

  int   args[ BTL_STR_ARG_MAX ];  ///< 引数

}BTL_HANDEX_STR_PARAMS;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u32     fSkipHPFull : 1;   // HP満タンなら実行しない
 u32     fUseDead    : 1;   // 死んでいても実行する
 u32     _pad        : 30;
}BTL_HANDEX_PARAM_USE_ITEM;


/**
 * メッセージ表示
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 BTL_HANDEX_STR_PARAMS     str;
}BTL_HANDEX_PARAM_MESSAGE;

/**
 * HP回復処理
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u16  recoverHP;                     ///< 回復HP量
 u8   pokeID;                        ///< 対象ポケモンID
 u8   fFailCheckThru;                ///< かいふくふうじ等のチェックを無視
 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_RECOVER_HP;

/**
 * HP吸収
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u16  recoverHP;                     ///< 回復HP量
 u8   recoverPokeID;                 ///< 回復対象ポケモンID
 u8   damagedPokeID;                 ///< ダメージを受けた側のポケモンID
 u8   fFailCheckThru;                ///< かいふくふうじ等のチェックを無視
 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_DRAIN;

/**
 * ダメージを与える
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 DamageCause damageCause;            ///< ダメージの原因
 u16  damage;                        ///< ダメージ量
 u8   pokeID;                        ///< 対象ポケモンID
 u8   damageCausePokeID;             ///< ダメージの原因となったポケモンのID( 存在しないなら BTL_POKEID_NULL )
 u8   fAvoidHidePoke      : 1;       ///< そらをとぶ状態など画面から消えているポケに効果なし
 u8   fExEffect           : 1;       ///< 成功時呼び出しエフェクト有効
 u8   fDisableDeadProcess : 1;       ///< ダメージによって死亡した場合でも、退場処理を行わない
 u8   _padd               : 5;
 u16  effectNo;                ///< 成功時呼び出しエフェクトナンバー
 u8   pos_from;                ///< 成功時呼び出しエフェクト開始位置（不要ならBTL_POS_NULL：初期値(0)はBTL_POS_NULLではないので注意）
 u8   pos_to;                  ///< 成功時呼び出しエフェクト終了位置（不要ならBTL_POS_NULL：初期値(0)はBTL_POS_NULLではないので注意）

 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}
BTL_HANDEX_PARAM_DAMAGE;

/**
 * HPを増減（ダメージ・回復とは見なされない）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
  u8     poke_cnt;
  u8     fEffectDisable;              ///< ゲージエフェクト等無効
  u8     fItemReactionDisable;        ///< アイテム反応チェックを行わない
  u8     pokeID[ BTL_POS_NUM ];
  int    volume[ BTL_POS_NUM ];
  DamageCause damageCause;            ///< HP変動の原因
}BTL_HANDEX_PARAM_SHIFT_HP;

/**
 * PPを回復
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8   volume;                        ///< PP量
 u8   pokeID;                        ///< 対象ポケモンID
 u8   wazaIdx;                       ///< 対象ワザインデックス
 u8   fSurfacePP       : 1;          ///< 仮ワザワークのPPを対象にする
 u8   fDeadPokeEnable  : 1;          ///< ひん死のポケモンにも有効（トレーナー道具使用時）
 u8   _padd            : 6;
 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_PP;

/**
 * 対象の状態異常を回復
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;      ///< 共有ヘッダ
 BtlWazaSickEx             sickCode;    ///< 対応する状態異常コード（拡張可）
 u8   pokeID[ BTL_PARTY_MEMBER_MAX*2 ]; ///< 対象ポケモンID
 u8   poke_cnt;                         ///< 対象ポケモン数
 u8   fStdMsgDisable;                   ///< 成功時、標準メッセージを出力しない
 BTL_HANDEX_STR_PARAMS     exStr;       ///< 成功時特殊メッセージ
}BTL_HANDEX_PARAM_CURE_SICK;

/**
 * 対象を状態異常にする
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 WazaSick       sickID;              ///< 状態異常ID
 BTL_SICKCONT   sickCont;            ///< 状態異常継続パラメータ
 SickCause      sickCause;           ///< 状態異常の原因
 u8   fAlmost              : 1;      ///< ほぼ確定フラグ（特殊要因で失敗した場合に原因メッセージを表示する）
 u8   fStdMsgDisable       : 1;      ///< 標準メッセージオフ
 u8   fItemReactionDisable : 1;      ///< アイテム反応オフ
 u8   fEffectiveToDeadPoke : 1;      ///< HP0になっているポケモンに対しても強制的に効果を発動する
 u8   _padd :                4;      ///< ビットフィールド余り
 u8   _reserved1;                    ///< 各種フラグのビットフィールド化でサイズ等が変更されるのを防ぐため追加
 u8   _reserved2;                    ///< 各種フラグのビットフィールド化でサイズ等が変更されるのを防ぐため追加
 u8   pokeID;                        ///< 対象ポケモンID
 BtlSickOverWriteMode  overWriteMode;///< 上書きモード
 BTL_HANDEX_STR_PARAMS  exStr;       ///< 特殊メッセージ表示（標準メッセージはオフ）
}BTL_HANDEX_PARAM_ADD_SICK;

/**
 * ランク効果（上昇、下降）
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 WazaRankEffect rankType;            ///< ランク増減種類
 RankEffectCause cause;              ///< ランク効果の原因
 u32  effSerial;                     ///< ランク効果シリアル（不要なら0）
 u8   pokeID[ BTL_POS_NUM ];         ///< 対象ポケモンID
 u8   poke_cnt;                      ///< 対象ポケモン数
 s8   rankVolume;                    ///< ランク増減値(=0だと強制リセット）
 u8   fStdMsgDisable : 1;            ///< 標準メッセージを表示しないフラグ
 u8   fAlmost : 1;                   ///< ほぼ確定フラグ（特殊要因で失敗した場合に原因メッセージを表示する）
 u8   fWazaEffect : 1;               ///< ワザによる効果であるフラグ
 u8   fMsgPreEffect : 1;             ///< 演出前に特殊メッセージを出す
 u8   fViewEffectDisable : 1;        ///< ランク増減演出を再生しないフラグ
 u8   bMigawariThrew : 1;            ///< みがわりを必ず貫通する
 u8   bMigawariCheck : 1;            ///< みがわり貫通するかどうかチェックルーチンを通す

 BTL_HANDEX_STR_PARAMS  exStr;       ///< 特殊メッセージ詳細
}BTL_HANDEX_PARAM_RANK_EFFECT;

/**
 * ランク効果を特定値にセット
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8 pokeID;                          ///< 対象ポケモンID
 s8 attack;
 s8 defence;
 s8 sp_attack;
 s8 sp_defence;
 s8 agility;
 s8 hit_ratio;
 s8 avoid_ratio;
 u8 critical_rank;
}BTL_HANDEX_PARAM_SET_RANK;

/**
 * 対象の下がっているランク効果をフラットに戻す
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8 pokeID;                          ///< 対象ポケモンID
}BTL_HANDEX_PARAM_RECOVER_RANK;

/**
 * 対象の上がっているランク効果をフラットに戻す
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8 pokeID;                          ///< 対象ポケモンID
}BTL_HANDEX_PARAM_RESET_RANKUP;

/**
 * 対象の全てのランク効果をフラットに戻す
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8   poke_cnt;                      ///< 対象ポケモン数
 u8   pokeID[ BTL_POS_NUM ];         ///< 対象ポケモンID
}BTL_HANDEX_PARAM_RESET_RANK;

/**
 * 対象の能力値を直接書き換え
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u16  attack;
 u16  defence;
 u16  sp_attack;
 u16  sp_defence;
 u16  agility;
 u8  pokeID;        ///< 対象ポケモンID
 u8  fAttackEnable    : 1;
 u8  fDefenceEnable   : 1;
 u8  fSpAttackEnable  : 1;
 u8  fSpDefenceEnable : 1;
 u8  fAgilityEnable   : 1;
 u8  dmy : 3;
 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SET_STATUS;

/**
 * 対象をひん死にする
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;  ///< 共有ヘッダ
 u8      pokeID;                    ///< 対象ポケID
 u8      fDeadPokeEnable     : 1;   ///< すでにひん死のポケモンも対象にし、ひん死演出などを強制的に行う
 u8      fDisableDeadProcess : 1;   ///< ひん死にしたポケモンの退場処理を行わない
 u8      padding             : 6;
 WazaID  recordWazaID;              ///< ワザによる効果であることを記録する場合、有効なワザナンバーをセットする
 DamageCause deadCause;             ///< ひん死になった原因
 BTL_HANDEX_STR_PARAMS      exStr;
}BTL_HANDEX_PARAM_KILL;

/**
 * 対象がひん死なら、退場処理を行う
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;  ///< 共有ヘッダ
 u8      pokeID;                    ///< 対象ポケID
}BTL_HANDEX_PARAM_DEAD_CHECK;

/**
 * タイプ変更処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  PokeTypePair               next_type;
  BTL_POKEPARAM::ExTypeCause exTypeCause;  ///< 新しいタイプに拡張タイプが含まれるなら、その原因
  u8                         pokeID;
  u8                         fStdMsgDisable;
  u8                         fPutFailMsg;  ///< まったく同一タイプに変更しようとした時に失敗メッセージ表示（XYみずびたし対応)
}BTL_HANDEX_PARAM_CHANGE_TYPE;

/**
 * 拡張タイプ追加処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER    header;
  pml::PokeType              ex_type;
  BTL_POKEPARAM::ExTypeCause exTypeCause;  ///< タイプ拡張の原因
  u8                         pokeID;
}BTL_HANDEX_PARAM_EX_TYPE;

/**
 * ターンフラグセット処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER   header;
  BTL_POKEPARAM::TurnFlag   flag;
  u8                        pokeID;
}BTL_HANDEX_PARAM_TURNFLAG;

/**
 * 継続フラグセット処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER    header;
  BTL_POKEPARAM::ContFlag    flag;
  u8                         pokeID;
}BTL_HANDEX_PARAM_SET_CONTFLAG;

typedef BTL_HANDEX_PARAM_SET_CONTFLAG  BTL_HANDEX_PARAM_RESET_CONTFLAG;

/**
 * 永続フラグセット処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER    header;
  BTL_POKEPARAM::PermFlag    flag;
  u8                         pokeID;
}BTL_HANDEX_PARAM_SET_PERMFLAG;

/**
 * サイドエフェクト追加処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlSideEffect            effect;
  BTL_SICKCONT             cont;
  BtlSide                  side;
  BTL_HANDEX_STR_PARAMS    exStr;
  bool                     fReplaceExStrArgs0ByExpandSide;  // true なら、exStr.args[0] が複数陣営を含む陣営IDである場合、展開後の陣営IDに置き換える
}BTL_HANDEX_PARAM_SIDEEFF_ADD;

/**
 * サイドエフェクト除去処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              flags[ BTL_SIDEEFF_BITFLG_BYTES ];
  u8              side;
}BTL_HANDEX_PARAM_SIDEEFF_REMOVE;

/**
 * サイドエフェクト一時停止・再開処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              flags[ BTL_SIDEEFF_BITFLG_BYTES ];
  u8              side;
  u8              bWeak;  // true なら再開処理
}BTL_HANDEX_PARAM_SIDEEFF_SLEEP;

/**
 * フィールドエフェクト追加処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  FieldStatus::EffectType  effect;
  BtlGround                ground;    ///< effect == EFF_GROUND の時に参照
  BTL_SICKCONT             cont;
  BTL_HANDEX_STR_PARAMS    exStr;

  u8                       fAddDependPoke;      ///< 追加に失敗した時に依存対象ポケモンとして登録する
  
}BTL_HANDEX_PARAM_ADD_FLDEFF;

/**
 * フィールドエフェクト除去処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER   header;
  FieldStatus::EffectType   effect;
}BTL_HANDEX_PARAM_REMOVE_FLDEFF;



/**
 * 天候変化処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlWeather               weather;
  u8                       turn;         // 天候の継続ターン数
  u8                       turnUpCount;  // 天候の継続ターン数のうち、道具や特性による増加ターン数
  u8                       fAirLock;
  BTL_HANDEX_STR_PARAMS    exStr;

}BTL_HANDEX_PARAM_CHANGE_WEATHER;

/**
 * 位置エフェクト追加
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlPosEffect             effect;
  BtlPokePos               pos;
  PosEffect::EffectParam   effectParam;
  bool                     fSkipHpRecoverSpFailCheck; ///< HP回復効果について、「かいふくふうじ」等のチェックを無視
}BTL_HANDEX_PARAM_POSEFF_ADD;

/**
 *  とくせい書き換え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  TokuseiChangeCause       cause;               ///< 書き換えの原因
  u16                      tokuseiID;           ///< 書き換え後のとくせい（POKETOKUSEI_NULLならとくせいを消す）
  u8                       pokeID;              ///< 対象ポケモンID
  u8                       fSameTokEffective;   /// < おなじ「とくせい」を持つ対象にも効く
  u8                       fSkipMemberInEvent;  ///< メンバー入場イベント１回スキップ（トレース対策）
  BTL_HANDEX_STR_PARAMS    exStr;               ///< 成功時メッセージ
}BTL_HANDEX_PARAM_CHANGE_TOKUSEI;

/**
 *  アイテム書き換え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16             itemID;           ///< 書き換え後のアイテム（ITEM_DUMMY_DATA ならアイテムを消す）
  u8              pokeID;           ///< 対象ポケモンID
  u8              fClearConsume;    ///< 対象ポケモンの消費情報をクリア
  u8              fClearConsumeOtherPoke; ///< 対象ポケモン以外の消費情報クリア
  u8              clearConsumePokeID;     ///< 消費情報クリアするポケモンID
  u8              fCallConsumedEvent;     ///< itemID=ITEM_DUMMY_DATA の際、消費イベントをコールする
  BTL_HANDEX_STR_PARAMS  exStr;     ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SET_ITEM;

/**
 *  アイテム入れ替え（ヘッダの使用ポケモンと）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;             ///< 対象ポケモンID
  bool            fIncRecordCount_StealItemFromWildPoke;  ///< 成功時に、レコードカウント「野生ポケモンから道具を奪った回数」をインクリメントする
  BTL_HANDEX_STR_PARAMS   exStr;      ///< 成功時メッセージ
  BTL_HANDEX_STR_PARAMS   exSubStr1;  ///< 成功時メッセージ
  BTL_HANDEX_STR_PARAMS   exSubStr2;  ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SWAP_ITEM;

/**
 *  アイテム効果の発動チェック
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8                pokeID;            ///< 対象ポケモンID
  BtlItemReaction   reactionType;
}BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP;

/**
 *  アイテム効果の特殊使用（ついばむ、なげつけるなど対応）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< 対象ポケモンID
  u8              bAteKinomi;        ///< 使ったアイテムがきのみなら、それを食べたことにするか？
  u16             itemID;
}BTL_HANDEX_PARAM_ITEM_SP;

/**
 *  アイテム消費処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8                       fNoAction;
  u8                       fNoAteKinomi;
  BTL_HANDEX_STR_PARAMS    exStr;   ///< 成功時メッセージ
}BTL_HANDEX_PARAM_CONSUME_ITEM;


/**
 * ワザ書き換え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< 対象ポケモンID
  u8              wazaIdx;
  u8              ppMax;             ///< PPMax値:0ならデフォルト
  u8              fPermanent;        ///< バトル後まで書き換えが残るフラグ
  WazaID          waza;
}BTL_HANDEX_PARAM_UPDATE_WAZA;

/**
 * カウンタ書き換え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< 対象ポケモンID
  u8              counterID;         ///< カウンタID（BppCounter)
  u8              value;             ///< カウンタに設定する値
}BTL_HANDEX_PARAM_COUNTER;

/**
 * 時間差ワザダメージ処理用
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8                       attackerPokeID;
  u8                       targetPokeID;
  BtlPokePos               attackerPos;
  WazaID                   wazaID;
}BTL_HANDEX_PARAM_DELAY_WAZADMG;

/**
 * バトル離脱
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  bool                     fForceNigeru; ///< true なら、必ず逃げられる
}BTL_HANDEX_PARAM_QUIT_BATTLE;

/**
 * メンバー入れ替え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BTL_HANDEX_STR_PARAMS    preStr;  ///< 実行開始メッセージ
  BTL_HANDEX_STR_PARAMS    exStr;   ///< 成功時メッセージ
  u8                       pokeID;
  u8                       fIntrDisable;  ///< 割り込み（おいうちなど）禁止
}BTL_HANDEX_PARAM_CHANGE_MEMBER;

/**
 * バトンタッチ処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8  userPokeID;     ///< 状態を引き継がせるポケID
  u8  targetPokeID;   ///< 状態を引き継ぐポケID
}BTL_HANDEX_PARAM_BATONTOUCH;

/**
 * ひるませる処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8  pokeID;     ///< 対象ポケID
  u8  per;        ///< 確率
}BTL_HANDEX_PARAM_ADD_SHRINK;

/**
 * 生き返らせる処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;        ///< 対象ポケID
  u16  recoverHP;     ///< 回復HP量
  BTL_HANDEX_STR_PARAMS  exStr; ///< 成功時メッセージ
}BTL_HANDEX_PARAM_RELIVE;

/**
 * 体重を設定
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;        ///< 対象ポケID
  u16  weight;        ///< 体重
  BTL_HANDEX_STR_PARAMS  exStr; ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SET_WEIGHT;

/**
 * ポケモンを場から吹き飛ばし
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16  effectNo;                ///< 成功時エフェクトナンバー
  u8   pokeID;                  ///< 対象ポケID
  u8   fForceChange : 4;        ///< 強制入れ替えモードフラグ
  u8   fIgnoreLevel : 4;        ///< レベル差判定を無視するフラグ
  BTL_HANDEX_STR_PARAMS  exStr; ///< 成功時メッセージ
}BTL_HANDEX_PARAM_PUSHOUT;

/**
 * 指定ポケモン割り込み行動
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                  ///< 対象ポケID
  BTL_HANDEX_STR_PARAMS  exStr; ///< 成功時メッセージ
}BTL_HANDEX_PARAM_INTR_POKE;

/**
 * 指定ワザ使用ポケ割り込み行動
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  WazaID   waza;                ///< 対象ワザID
}BTL_HANDEX_PARAM_INTR_WAZA;

/**
 * 指定ポケモンの行動順を最後に回す
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< 対象ポケID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SEND_LAST;

/**
 * 場に出ているポケモン同士を入れ替える（味方のみ）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID1;                      ///< 対象ポケID 1
  u8   pokeID2;                      ///< 対象ポケID 2
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SWAP_POKE;

/**
 * へんしん（ヘッダ指定ポケモン）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< 対象ポケID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_HENSIN;

/**
 * イリュージョン解除
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< 対象ポケID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_FAKE_BREAK;

/**
 * 溜めワザによる消え状態（そらをとぶ等）をキャンセルする
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER   header;
  u8                        pokeID;    ///< 対象ポケID
  BTL_POKEPARAM::ContFlag   flag;      ///< キャンセルする状態フラグ
  BTL_HANDEX_STR_PARAMS     exStr;     ///< 成功時メッセージ

}BTL_HANDEX_PARAM_TAMEHIDE_CANCEL;

/**
 * エフェクト呼び出し
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16  effectNo;                      ///< エフェクトナンバー
  u8   pos_from;                      ///< エフェクト発動位置（不要ならBTL_POS_NULL：初期値(0)はBTL_POS_NULLではないので注意）
  u8   pos_to;                        ///< エフェクト終点位置（不要ならBTL_POS_NULL：初期値(0)はBTL_POS_NULLではないので注意）
  u16  reservedQuePos;                ///< Que予約領域
  u8   fQueReserve;                   ///< Que予約フラグ
  u8   fMsgWinVanish;                 ///< メッセージウィンドウ消去フラグ
  BTL_HANDEX_STR_PARAMS    exStr;    ///< エフェクト後メッセージ
}BTL_HANDEX_PARAM_ADD_EFFECT;



/**
 * なかよし度エフェクト呼び出し
 */
typedef struct {

  BTL_HANDEX_PARAM_HEADER   header;
  u8                        pokeID;    ///< 対象ポケID
  FriendshipEffect          effType;   ///< エフェクトタイプ
  BTL_HANDEX_STR_PARAMS     exStr;     ///< 同時に流すメッセージ

}BTL_HANDEX_PARAM_FRIENDSHIP_EFFECT;

/**
 * フォルムナンバーチェンジ
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                   ///< ポケモンID
  u8   formNo;                   ///< フォルムナンバー
  bool fDontResetFormByOut;      ///< true なら、場から退場してもデフォルトフォルムに戻さない
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_CHANGE_FORM;

/**
 * ワザエフェクトIndexセット
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8     effIndex;
}BTL_HANDEX_PARAM_SET_EFFECT_IDX;

/**
 *  ワザエフェクトを強制的に有効にする
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
}BTL_HANDEX_PARAM_WAZAEFFECT_ENABLE;

/**
 *  ワザエフェクトを強制的に有効にする
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8                       targetPokeID;
}BTL_HANDEX_PARAM_FREEFALL;

/**
 *  直後に技実行アクションを追加する
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  ActionDesc               actionDesc;        ///< アクションの詳細情報
  u8                       actPokeID;         ///< 技を撃つポケモンのID
  WazaNo                   actWazaNo;         ///< 使用する技
  BtlPokePos               targetPos;         ///< 技ターゲットの位置
  BTL_HANDEX_STR_PARAMS    successMessage;    ///< 成功時に表示するメッセージ
}BTL_HANDEX_PARAM_INSERT_WAZA_ACTION;

/**
 *  ポケモンに汚れをセットする
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8                       pokeID;    ///< 汚れをセットするポケモンのID
  DirtType                 dirtType;  ///< 汚れの種類
}BTL_HANDEX_PARAM_SET_DIRT;


/**
 *  文字列パラメータ構造体操作
 */
// クリア
extern void HANDEX_STR_Clear( BTL_HANDEX_STR_PARAMS* param );
// 内容があるか（クリア状態でないか）チェック
extern bool HANDEX_STR_IsEnable( const BTL_HANDEX_STR_PARAMS* param );
// 文字列ID設定
extern void HANDEX_STR_Setup( BTL_HANDEX_STR_PARAMS* param, BtlStrType type, u16 strID );
// 引数１件追加
extern void HANDEX_STR_AddArg( BTL_HANDEX_STR_PARAMS* param, int arg );
// 終端SE設定
extern void HANDEX_STR_AddSE( BTL_HANDEX_STR_PARAMS* param, u32 SENo );
// 失敗メッセージフラグを設定
extern void HANDEX_STR_SetFailMsgFlag( BTL_HANDEX_STR_PARAMS* param );

// 失敗メッセージフラグが立っているかチェック
extern bool HANDEX_STR_IsFailMsg( const BTL_HANDEX_STR_PARAMS* param );

//---------------------------------------------------------------------------------------
/**
 *
 */
//---------------------------------------------------------------------------------------
class HEManager {
  GFL_FORBID_COPY_AND_ASSIGN( HEManager );

public:

  HEManager( void );
  ~HEManager(){ };

  void                  Init( void );
  u32                   PushState( void );
  u32                   PushStateUseItem( u16 itemNo );
  void                  PopState( u32 state );
  BTL_HANDEX_PARAM_HEADER*  ReadWork( void );

  bool                  IsExistWork( void ) const;
  u16                   GetUseItemNo( void ) const;
  bool                  IsUsed( void ) const;
  void                  SetResult( bool fSucceed );
  bool                  GetPrevResult( void ) const;
  bool                  GetTotalResult( void ) const;

  BTL_HANDEX_PARAM_HEADER*  PushWork( EventHandlerExhibition eq_type, u8 userPokeID );
  void                      PopWork( void* exWork );


private:

  /**
   *  定数
   */
  enum {
    WORK_TOTALSIZE = 500,
  };

  /**
   *  ハンドラ反応保存用ワークマネージャ
   */
  union {

    u32    m_state;
    struct {
      u32  m_useItem      : 10;
      u32  m_stack_ptr    :  9;
      u32  m_read_ptr     :  9;
      u32  m_fPrevSucceed :  1;  ///< 前回の使用が成功した
      u32  m_fSucceed     :  1;  ///< １度でも成功した
      u32  m_fUsed        :  1;  ///< １度でも実行された
      u32  _padd          :  1;
    };

  };

  u8     m_workBuffer[ WORK_TOTALSIZE ];

  //---- private methods ------------------------------------------------
  void      clearWork( void );

};  // class HEManager


GFL_NAMESPACE_END(btl)

#endif // __BTL_HEMANAGER_H__
