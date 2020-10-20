//=============================================================================
/**
 *
 *  @file EncountData.h
 *  @brief エンカウントデータ
 *
 *  @niji nijiのエンカウントデータにあわせて全体的にデータ型を修正
 *
 *  @author saita_kazuki,hosaka genya,takahasi tomoya
 *  @date 2015.05.25
 */
//=============================================================================

#if !defined( FIELD_ENCOUNT_DATA_H_INCLUDED )
#define FIELD_ENCOUNT_DATA_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "niji_conv_header/field/encount/FieldEncountDataTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Encount );

///@niji フォーマットにあわせて修正
enum
{ 
  DATA_INVALID_ID           = 0xffff, ////< 無効エンカウントデータID
  
  MONS_NUM_NORMAL           = 10,     ///< 通常エンカウントの登録数
  MONS_NUM_NORMAL_INTRUDER  = 10,     ///< 通常乱入エンカウントの登録数
  MONS_NUM_RAIN_INTRUDER    = 1,      ///< 雨乱入エンカウントの登録数
  MONS_NUM_SNOW_INTRUDER    = 1,      ///< 雪乱入エンカウントの登録数
  MONS_NUM_SAND_INTRUDER    = 1,      ///< 砂嵐乱入エンカウントの登録数

  MONS_NUM_MAX              = 10,     ///< 各テーブルを含めたポケモン数の最大数
  
  MONS_FORM_REGION_CODE     = 0x1E,   ///< フォルム特殊コード（リアル地域ポケモン用）
  MONS_FORM_RND_CODE        = 0x1F,   ///< フォルムをランダムで決める特殊コード


  BLOCK_NUM_NORMAL          = 1,      ///< 通常エンカウントのブロック数。ブロック数 = MonsNo、FormNo、DecoNoが設定される各ジャンルごとの塊 
  BLOCK_NUM_NORMAL_INTRUDER = 7,      ///< 通常乱入エンカウントのブロック数
  BLOCK_NUM_RAIN_INTRUDER   = 2,      ///< 雨乱入エンカウントのブロック数
  BLOCK_NUM_SNOW_INTRUDER   = 2,      ///< 雪乱入エンカウントのブロック数
  BLOCK_NUM_SAND_INTRUDER   = 2,      ///< 砂嵐乱入エンカウントのブロック数
};

/// 乱入種類
enum IntruderType
{
  INTRUDER_TYPE_NORMAL,
  INTRUDER_TYPE_RAIN,
  INTRUDER_TYPE_SNOW,
  INTRUDER_TYPE_SAND,

  INTRUDER_TYPE_MAX
};

//--------------------------------------------------------------
///  エンカウントロケーションID
//
// @niji エンカウントフォーマットが変わったのでsangoまでと同じ使い方はしない
// ただ、例の水際などで領域が被った際に、どのデータを採用すべきか？という観点から
// ロケーションの概念を扱うかも。6月は要らない
//==============================================================
typedef enum
{ 
  LOCATION_GROUND_1,
  LOCATION_GROUND_SP,
  LOCATION_GROUND_EX,
  LOCATION_WATER,
  LOCATION_STONE,
  LOCATION_FISHING_1,
  LOCATION_FISHING_2,
  LOCATION_FISHING_3,
  LOCATION_GROUP,
  LOCATION_MAX,
  LOCATION_NONE = 0xFF, ///< 無効ID
}LOCATION;

//--------------------------------------------------------------
///  拡張の確率テーブル
//==============================================================
enum EX_TABLE
{ 
  EX_TABLE_NONE = 0,
  EX_TABLE_GROUND,    ///< 地上
  EX_TABLE_WATER,     ///< 水上
};

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

//--------------------------------------------------------------
///エンカウント率補整パターンデータ
//==============================================================
struct ENCPROB_PATTERN{
  u8 up;
  u8 interval;
  u8 max;
  u8 safety;
};

//--------------------------------------------------------------
///  エンカウントモンスターテーブルデータ @niji ポケデコが後から追加されると思うけど、デコは一旦スルー
//==============================================================
struct COMMON_POKE_DATA
{ 
  u16 monsNo : 11;
  u16 form : 5;
  u16 _unuse; // niji未使用 旧ポケデコ用領域
};
// 4byte

struct COMMON_TIME_ZONE_DATA
{
  u8 minLevel;
  u8 maxLevel;
  u8 prob[MONS_NUM_NORMAL];

  COMMON_POKE_DATA normalMons[ BLOCK_NUM_NORMAL ][ MONS_NUM_NORMAL ];
  COMMON_POKE_DATA normalIntruderMons[ BLOCK_NUM_NORMAL_INTRUDER ][ MONS_NUM_NORMAL_INTRUDER ];
  COMMON_POKE_DATA rainIntruderMons[ BLOCK_NUM_RAIN_INTRUDER ][ MONS_NUM_RAIN_INTRUDER ];
  COMMON_POKE_DATA snowIntruderMons[ BLOCK_NUM_SNOW_INTRUDER ][ MONS_NUM_SNOW_INTRUDER ];
  COMMON_POKE_DATA sandIntruderMons[ BLOCK_NUM_SAND_INTRUDER ][ MONS_NUM_SAND_INTRUDER ];
};

struct ENCOUNT_DATA{ //@niji エンカウントデータのバイナリフォーマットとあわせる。unionは使わなくてもいい。乱入はα後に考える

  u8 probPattern;
  u8 padding1[3];

  COMMON_TIME_ZONE_DATA day;
  COMMON_TIME_ZONE_DATA night;
};


//==============================================================
/// シンボル援軍パラメータ
//==============================================================
struct SymbolReinforeParam
{
  u8  index;
  s8  count;
  u8  type;
  u8  pad;
};

//==============================================================
/// シンボルエンカウトパラメータ
//==============================================================
struct SymbolEncountParam
{
  u16 monsNo;   //モンスターナンバー
  u8  form;     //ポケモンのフォルム(0-31)
  u8  level;    //ポケモンのレベル（1～100）
  u16 item;     //アイテムナンバー
  u16 rare:2;   //レア制御    0：制御しない 1：レア抽選　2：レア禁止　WildEncRarePattern
  u16 sex:2;    //性別制御    0：制御しない 1：オス固定　2：メス固定　WildEncSexPattern
  u16 spabi:3;  //特性制御    0：制御しない 1：特性１ 2：特性２ 3：特性３　WildEncSpeabiPattern
  u16 mode:8;   //エンカウントモード制御 　WildEncMode　←オダマキ削除　ぬし追加
  u16 :1;       //パディング

  u16  far_bg;   //FAR_BG(戦闘背景　遠景)　bgsys_far_type_id_def.rb
  u16  near_bg;  //NEAR_BG(戦闘背景　近景)　bgsys_near_type_id_def.rb

  //以下追加
  u16 waza[4];       ///< ポケモンの技4つ分　wazano_hash.rb　

  u8 seikaku;         ///< 性格指定
  s8 talent_hp;       ///< 個体値：HP(-1～31)
  s8 talent_atk;      ///< 個体値：ATK(-1～31)
  s8 talent_def;      ///< 個体値：DEF(-1～31)

  s8 talent_spatk;    ///< 個体値：SPATK(-1～31)
  s8 talent_spdef;    ///< 個体値：SPDEF(-1～31)
  s8 talent_agi;      ///< 個体値：AGI (-1～31)
  u8 effort_hp;       ///< 努力値：HP(0-252 かつ 全ての努力値の合計が510以下)

  u8 effort_atk;      ///< 努力値：ATK(0-252 かつ 全ての努力値の合計が510以下)
  u8 effort_def;      ///< 努力値：DEF(0-252 かつ 全ての努力値の合計が510以下)
  u8 effort_spatk;    ///< 努力値：SPATK(0-252 かつ 全ての努力値の合計が510以下)
  u8 effort_spdef;    ///< 努力値：SPDEF(0-252 かつ 全ての努力値の合計が510以下)

  u8 effort_agi;      ///< 努力値：AGI (0-252 かつ 全ての努力値の合計が510以下)
  u8 aura_color_r;  //オーラの色 R(0-255)
  u8 aura_color_g;  //オーラの色 G(0-255)
  u8 aura_color_b;  //オーラの色 B(0-255)

  u8 aura_color_a;  //オーラの色 A(0-255)
  u8 aura_mode; //オーラ効果 WildEncAulaMode　日本語定義はエクセルの値リスト、オーラ効果アップを見てください
  u8  btl_eff;    //バトルエフェクト　battle_effect_hash.rb
  u8 reinforce_num;  //援軍パラメータ
  SymbolReinforeParam reinforce[REINFORCE_POKE_NUM];
};



GFL_NAMESPACE_END( Encount );
GFL_NAMESPACE_END( Field );

#endif // FIELD_ENCOUNT_DATA_H_INCLUDED 
