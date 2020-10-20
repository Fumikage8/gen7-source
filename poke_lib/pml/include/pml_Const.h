//========================================================================================
/**
 * @brief ポケモン関連の定数
 * @file pml_Const.h
 * @author obata_toshihiro
 * @date 2011.05.13
 */
//========================================================================================
#if !defined __PML_CONST_H__
#define __PML_CONST_H__
#pragma once

#include <types/include/gfl2_Typedef.h>

namespace pml {

  static const u8 MONS_NAME_LENGTH = 12;         ///< ポケモン名の文字数( 英文字 )
  static const u8 PERSON_NAME_LENGTH = 12;       ///< 人物名の文字数( 英文字 )
  static const u8 MAX_WAZA_NUM = 4;              ///< 所持技の最大数
  static const u8 MIN_POKE_LEVEL = 1;            ///< ポケモンの最小レベル
  static const u8 MAX_POKE_LEVEL = 100;          ///< ポケモンの最大レベル
  static const u32 MAX_POKE_ID = 0xffffffff;     ///< ポケモンのIDNo.の最大値
  static const u8 MAX_EFFORT_POWER = 252;        ///<「努力値」の個別の上限値
  // @note syachiまでは努力値255まで設定可能だったが,
  // 意味のある最大値は252なので, あまりの3は無駄になっていた。
  static const u16 MAX_TOTAL_EFFORT_POWER = 510; ///<「努力値」の合計の上限値
  static const u8 MAX_TALENT_POWER = 31;         ///<「個体値」の上限値
  static const u8 MAX_WAZAPP_UPCOUNT = 3;        ///<「ポイントアップ」の最大使用回数
  static const u8 MAX_FAMILIARITY = 255;         ///<「なつき度」の最大値
  static const u8 MAX_FRIENDSHIP = 255;          ///<「仲良し度」の最大値
  static const u8 MAX_STYLE = 255;               ///<「かっこよさ」の最大値
  static const u8 MAX_BEAUTIFUL = 255;           ///<「うつくしさ」の最大値
  static const u8 MAX_CUTE = 255;                ///<「かわいさ」の最大値
  static const u8 MAX_CLEVER = 255;              ///<「かしこさ」の最大値
  static const u8 MAX_STRONG = 255;              ///<「たくましさ」の最大値
  static const u8 MAX_FUR = 255;                 ///<「毛艶」の最大値
  static const u8 MAX_FEED = 0xff;               ///<「満腹度」の最大値
  static const u8 MAX_NADENADE_VALUE = 0xff;     ///<「なでなで値」の最大値
  static const u8 MAX_PAST_PARENTS_COUNT = 5;    ///< 過去の親の最大数
  static const u8 MAX_TOKUSEI_INDEX = 2;         ///< 特性インデックスの最大値
  static const u8 MAX_TRAINING_GOODS   = 26;     ///< トレーニンググッズの最大値
  static const u8 MAX_TRAINING_BADGE   = 48;     ///< トレーニング勲章の最大値（確保してある最大値）
  static const u8 MAX_GET_TRAINING_BADGE = 38;   ///< トレーニング勲章の最大値（実際に取得できる最大値）


  static const u8 EOM_LENGTH = 1;
  static const u8 MONS_NAME_BUFFER_SIZE = MONS_NAME_LENGTH + EOM_LENGTH;  ///< ポケモン名のバッファサイズ[Byte]
  static const u8 PERSON_NAME_BUFFER_SIZE = PERSON_NAME_LENGTH + EOM_LENGTH;  ///< 人物名のバッファサイズ[Byte]

  /// 性別(xy_projectのscript_define_inner.incと対応しています！！)
  enum Sex {
    SEX_MALE,    // オス
    SEX_FEMALE,  // メス
    SEX_UNKNOWN, // 不明
    SEX_NUM,
  };

}

#endif // __PML_CONST_H__
