#if !defined( __BTLV_EFFECT_DEF_H__ )
#define __BTLV_EFFECT_DEF_H__
//======================================================================
/**
 * @file  btlv_effect_def.h
 * @brief バトル描画 エフェクト定義
 * @author  ariizumi
 * @data  11/01/13
 */
//======================================================================
#pragma once

#include "btlv_types.h"

GFL_NAMESPACE_BEGIN(btl)

//--------------------------------------------
/**
 *  ターン種類指定
 */
//--------------------------------------------
typedef enum {
  BTLV_WAZAEFF_INDEX_DEFAULT = 0,

  // 溜めワザ全般（そらをとぶなど）
  BTLV_WAZAEFF_TAME_START = 0,      ///< 溜め開始
  BTLV_WAZAEFF_TAME_RELEASE,        ///< 溜め解放

  // 時間差ワザ全般（みらいよち、はめつのねがい等）
  BTLV_WAZAEFF_DELAY_START = 0,     
  ///< ワザ撃ち
  BTLV_WAZAEFF_DELAY_ATTACK,        ///< 攻撃実行

  // どろぼう
  BTLV_WAZAEFF_DOROBOU_NORMAL = 0,  ///< ヒットのみ
  BTLV_WAZAEFF_DOROBOU_STEAL,       ///< ヒット＆盗み成功

  // のろい
  BTLV_WAZAEFF_NOROI_NORMAL = 0,    ///< ゴースト以外
  BTLV_WAZAEFF_NOROI_GHOST,         ///< ゴースト

  // プレゼント
  BTLV_WAZAEFF_PRESENT_DAMAGE = 0,  ///< ダメージ
  BTLV_WAZAEFF_PRESENT_RECOVER,     ///< HP回復

  // かわらわり
  BTLV_WAZAEFF_KAWARAWARI_DEFAULT = 0, ///< 通常
  BTLV_WAZAEFF_KAWARAWARI_BREAK,       ///< リフレクター等を破壊

  // ウェザーボール
  BTLV_WAZAEFF_WEATHERBALL_NORMAL = 0,  ///< ノーマル
  BTLV_WAZAEFF_WEATHERBALL_SHINE,       ///< にほんばれ
  BTLV_WAZAEFF_WEATHERBALL_SNOW,        ///< あられ
  BTLV_WAZAEFF_WEATHERBALL_SAND,        ///< すなあらし
  BTLV_WAZAEFF_WEATHERBALL_RAIN,        ///< あまごい

  // テクノバスター
  BTLV_WAZAEFF_TECKNOBASTER_NORMAL = 0, ///< ノーマル
  BTLV_WAZAEFF_TECKNOBASTER_MIZU,       ///< 水
  BTLV_WAZAEFF_TECKNOBASTER_DENKI,      ///< 電気
  BTLV_WAZAEFF_TECKNOBASTER_HONOO,      ///< 炎
  BTLV_WAZAEFF_TECKNOBASTER_KOORI,      ///< こおり

  // フレイムソウル・サンダーソウル
  BTLV_WAZAEFF_LINKWAZA_NORMAL = 0,     ///< 通常
  BTLV_WAZAEFF_LINKWAZA_LINK,           ///< リンクした（続けて出した）

  // がまん
  BTLV_WAZAEFF_GAMAN_TAME = 0,          ///< がまん中
  BTLV_WAZAEFF_GAMAN_ATTACK,            ///< 攻撃ターン

  // じばく・だいばくはつ
  BTLV_WAZAEFF_JIBAKU_SELF = 0,         ///< 自分が爆発
  BTLV_WAZAEFF_JIBAKU_TARGET_DAMAGE,    ///< 相手にダメージ

  // かふんだんご
  BTLV_WAZAEFF_KAHUNDANGO_DAMAGE = 0,  ///< ダメージ
  BTLV_WAZAEFF_KAHUNDANGO_RECOVER,     ///< HP回復

  // みずしゅりけん
  BTLV_WAZAEFF_MIZUSYURIKEN_NORMAL = 0,  ///< 通常
  BTLV_WAZAEFF_MIZUSYURIKEN_SATOSHI,     ///< サトシゲッコウガ用

}BtlvWazaEffect_TurnType;

//戦闘エフェクト定義
enum{
  BTLEFF_ETC_MEGA_EVOLUTION,
  BTLEFF_LVUP,                                //レベルアップ
  BTLEFF_RARE,                                //レアエフェクト
  BTLEFF_USE_ITEM,                            //ポケモンにアイテムを使用
  BTLEFF_SOUBI_ITEM,                          //装備道具発動 10
  BTLEFF_EAT_NUTS_POKE,                       //ポケモンが木の実を使用
  BTLEFF_LAND_SMOKE,                          //着地の砂煙
  BTLEFF_FORMCHANGE,  //フォルムチェンジ
  BTLEFF_HENSHIN,     //変身
  BTLEFF_ILLUSION,    //イリュージョン
  BTLEFF_KAWAIGARI_EFFECT,                    //かわいがり効果発動(エフェクト)
  BTLEFF_KAWAIGARI_SHAKE,                     //かわいがり効果発動(揺らし)
  BTLEFF_KAWAIGARI_JUMP,                      //かわいがり効果発動(ジャンプ)
  BTLEFF_KAWAIGARI_TURN,                      //かわいがり効果発動(振り向き)
  BTLEFF_WEATHER_DAMAGE,                      //天候ダメージエフェクト
  BTLEFF_BENCH_RECOVERY,                      //控えの回復エフェクト
  BTLEFF_RAINBOW,                             //虹エフェクト
  BTLEFF_BURNING,                             //火の海エフェクト
  BTLEFF_MOOR,                                //湿地エフェクト
  BTLEFF_RESET_FIELD_EFFECT,                  //フィールド技リセット
  BTLEFF_ASSIST_POWER,                        //アシストパワー発動
  BTLEFF_KAWAIGARI_STROKE,                    //かわいがり効果発動(なでる)
  BTLEFF_LAND_SMOKE_S,                          //着地の砂煙
  BTLEFF_SIMPLE_DAMAGE,                       //演出省略時のダメージ
  BTLEFF_SIMPLE_DAMAGE_NO_SOUND,              //演出省略時のダメージ(サウンド無)
  BTLEFF_FORMCHANGE_GIRUGARUDO_TATE_TO_HOKO,  //フォルムチェンジ(ギルガルド盾→剣)
  BTLEFF_FORMCHANGE_GIRUGARUDO_HOKO_TO_TATE,  //フォルムチェンジ(ギルガルド剣→盾)
  BTLEFF_ETC_CAPTURE_START,
  BTLEFF_ETC_CAPTURE_SEQ1,
  BTLEFF_ETC_CAPTURE_SEQ2,
  BTLEFF_ETC_CAPTURE_SEQ3,
  BTLEFF_ETC_CAPTURE_SUCCESS,
  BTLEFF_ETC_CAPTURE_FAIL_1,
  BTLEFF_ETC_CAPTURE_FAIL_2,
  BTLEFF_ETC_CAPTURE_FAIL_3,
  BTLEFF_ETC_CAPTURE_FAIL_4,
  BTLEFF_ETC_CAPTURE_START_CRI,
  BTLEFF_ETC_CAPTURE_SUCCESS_CRI,
  BTLEFF_ETC_CAPTURE_FAIL_CRI,
  BTLEFF_ETC_CAPTURE_FAIL_TRAINER,
  BTLEFF_ETC_TRAINER_NORMAL_LOSE_E,  //ダミー
  BTLEFF_ETC_TRAINER_MODEL_LOSE_E,

  //ステータスエフェクト
  BTLEFF_NEMURI,                              //ねむり
  BTLEFF_DOKU,                                //どく
  BTLEFF_YAKEDO,                              //やけど
  BTLEFF_KOORI,                               //こおり
  BTLEFF_MAHI,                                //まひ
  BTLEFF_KONRAN,                              //こんらん
  BTLEFF_MEROMERO,                            //メロメロ
  BTLEFF_STATUS_UP,                           //ステータス上昇
  BTLEFF_STATUS_DOWN,                         //ステータス下降
  BTLEFF_HP_RECOVER,                          //HP回復
  BTLEFF_MIGAWARI_WAZA_BEFORE,                //みがわり時に技を繰り出す前エフェクト
  BTLEFF_MIGAWARI_WAZA_AFTER,                 //みがわり時に技を繰り出した後エフェクト
  BTLEFF_ITEM_ESCAPE,                         //エネコのしっぽorピッピ人形
  BTLEFF_WEATHER_KIRI,                        //きり（シャチではないかも？）
  BTLEFF_WEATHER_AME,                         //あめ
  BTLEFF_WEATHER_ARARE,                       //あられ  20
  BTLEFF_WEATHER_SUNAARASHI,                  //すなあらし
  BTLEFF_WEATHER_HARE,                        //はれ
  BTLEFF_POKEMON_MODOSU,                      //ポケモン引っ込めるエフェクト
  BTLEFF_POKEMON_KURIDASU,                    //ポケモン繰り出しエフェクト
  BTLEFF_MIGAWARI_FALL,                       //みがわり落ちてくる
  BTLEFF_MIGAWARI_DEAD,                       //みがわり消滅
  BTLEFF_TRAINER_IN,                          //トレーナーフレームイン
  BTLEFF_TRAINER_OUT,                         //トレーナーフレームアウト
  BTLEFF_KIAIPUNCH_TAME,                      //きあいパンチのタメエフェクト
  BTLEFF_NOROI,                               //のろい  30
  BTLEFF_AKUMU,                               //あくむ
  BTLEFF_YADORIGI,                            //やどりぎ
  BTLEFF_SIMETUKERU,                          //しめつける
  BTLEFF_MAKITUKU,                            //まきつく
  BTLEFF_HONOONOUZU,                          //ほのおのうず
  BTLEFF_MAGUMASUTOOMU,                       //マグマストーム
  BTLEFF_KARADEHASAMU,                        //からではさむ
  BTLEFF_UZUSIO,                              //うずしお
  BTLEFF_SUNAZIGOKU,                          //すなじごく
  BTLEFF_NEWOHARU,                            //ねをはる  40
  BTLEFF_MATOWARITUKU,                        //まとわりつく(kujira追加
  BTLEFF_ZOOM_IN,                             //カメラズームイン
  BTLEFF_IYASINONEGAI_KAIHUKU,                //いやしのねがい回復エフェクト
  BTLEFF_MIKADUKINOMAI_KAIHUKU,               //みかづきのまい回復エフェクト
  BTLEFF_RESET_MOVE,                          //リセットムーブエフェクト
  BTLEFF_SHOOTER_EFFECT,                      //シューターエフェクト
  BTLEFF_TONBOGAERI_RETURN,                   //とんぼがえり戻りエフェクト
  BTLEFF_VOLTCHANGE_RETURN,                   //ボルトチェンジ戻りエフェクト
  BTLEFF_POKEMON_VANISH_ON,                   //ポケモンバニッシュON
  BTLEFF_POKEMON_VANISH_OFF,                  //ポケモンバニッシュOFF
  BTLEFF_RESHUFFLE_FOCUS_DEFAULT,             //入れ替えカメラフォーカスデフォルト 50
  BTLEFF_RESHUFFLE_FOCUS_2vs2_POS_A,          //入れ替えカメラフォーカス2vs2POS_A
  BTLEFF_RESHUFFLE_FOCUS_3vs3_POS_A,          //入れ替えカメラフォーカス3vs3POS_A
  BTLEFF_RESHUFFLE_FOCUS_3vs3_POS_E,          //入れ替えカメラフォーカス3vs3POS_E
  BTLEFF_TOMOENAGE_RETURN,                    //ともえなげ戻りエフェクト
  BTLEFF_DRAGONTAIL_RETURN,                   //ドラゴンテール戻りエフェクト
  BTLEFF_HOERU_RETURN,                        //ほえる戻りエフェクト
  BTLEFF_HUKITOBASI_RETURN,                   //ふきとばし戻りエフェクト
  BTLEFF_ZOOM_IN_RESET,                       //ズームインをリセット（初期位置に戻す）
                                              //カメラの処理はないが、連続技終了時の身代わり戻しに使っている
  BTLEFF_RESET_WEATHER,                       //天候リセット
  BTLEFF_SIMPLE_HIT,                          //ヒットエフェクト(おやこあい・混乱ダメージ)
  BTLEFF_JIBAKU_DAMAGE,                       //自爆ダメージ部
  BTLEFF_FUNJIN,                              //ふんじん
  BTLEFF_GRASS_FIELD,                         //グラスフィールド
  BTLEFF_MIST_FIELD,                          //ミストフィールド
  BTLEFF_EREKI_FIELD,                         //エレキフィールド

  //sango追加分
  BTLEFF_WEATHER_STORM,                       //sango追加 おおあめ
  BTLEFF_WEATHER_DAY,                         //sango追加 おおひでり
  BTLEFF_WEATHER_TURBULENCE,                  //sango追加 乱気流
  BTLEFF_FORMCHANGE_KAIOOGA,                //フォルムチェンジ(カイオーガ)
  BTLEFF_FORMCHANGE_GURAADON,               //フォルムチェンジ(グラードン)

  //niji追加分
  BTLEFF_KUTIBASICANON_TAME ,  //くちばしキャノンタメエフェクト
  BTLEFF_TRAPSHELL_TAME ,      //トラップシェルタメエフェクト
  BTLEFF_SAIKO_FIELD,                         //サイコフィールド
  BTLEFF_FORMCHANGE_GEKKOUGA_SATOSHI,       //フォルムチェンジ(ゲッコウガサトシ)
  BTLEFF_FORMCHANGE_JIGARUDE,               //フォルムチェンジ(ジガルデ)
  BTLEFF_FORMCHANGE_YOWASHI_SMALL_TO_BIG,   //フォルムチェンジ(ヨワシ)
  BTLEFF_FORMCHANGE_YOWASHI_BIG_TO_SMALL,   //フォルムチェンジ(ヨワシ)
  BTLEFF_FORMCHANGE_METENO,                 //フォルムチェンジ(メテノ)
  BTLEFF_FORMCHANGE_MIMIKKYU,               //フォルムチェンジ(ミミッキュ)
  BTLEFF_GUARD,                             //相手の技を無効化したエフェクト
  BTLEFF_SHADOWSTEAL,                       //シャドースチール
  BTLEFF_FORMCHANGE_GIRUGARUDO_TATE_TO_HOKO_Z,  //フォルムチェンジ(ギルガルド盾→剣)Z技中用(カメラ変化が無い
  BTLEFF_FORMCHANGE_GIRUGARUDO_HOKO_TO_TATE_Z,  //フォルムチェンジ(ギルガルド剣→盾)Z技中用(カメラ変化が無い

  //momiji追加
  BTLEFF_FORMCHANGE_HIKARIDORAGON,          //ヒカリドラゴンのフォルムチェンジ
  BTLEFF_ROTOM_POWER,                       //ロトムパワー
  BTLEFF_Z_ROTOM_POWER,                     //Zロトムパワー
  BTLEFF_PP_RECOVER,                        //PP回復

  BTLEFF_MAX,

  BTLEFF_CAMERA_INIT = 2000,
  BTLEFF_CAMERA_INIT_ORTHO = 2001,
  BTLEFF_CAMERA_INIT_ORTHO_NO_WAIT = 2002,
  //上記のコマンドを使っているところのためにダミーを用意

};

enum
{
  BTL_ENCOUNT_WILD = 0,
  BTL_ENCOUNT_TRAINER_2D = 1,
  BTL_ENCOUNT_TRAINER_3D = 2,
  BTL_ENCOUNT_TRAINER_GYM = 3,
};

GFL_NAMESPACE_END(btl)

#endif // __BTLV_EFFECT_DEF_H__
