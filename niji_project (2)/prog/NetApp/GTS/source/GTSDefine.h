// ============================================================================
/*
 * @file GTSDefine.h
 * @brief GTSアプリで使用する定数等の定義ファイルです。
 * @date 2015.06.10
 */
// ============================================================================
#if !defined( GTSDEFINE_H_INCLUDE )
#define GTSDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "System/include/GameStrLen.h"
#include "Savedata/include/MyStatus.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

static const u32  GTS_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //   1 MB
static const u32  GTS_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 18;   //  18 MB
static const u32  GTS_APP_NAME_HEAP_SIZE                = 1024 * 1024 *  2;   //   2 MB

static const u32  GTS_APP_WORK_HEAP_SIZE                = 1024 * 256;         // 256 KB

static const u32  GTS_TRAINER_ICON_NUM                  = 5;

static const u32  GTS_SEARCH_POKEMON_RESULT_MAX         = 100;

static const int  GTS_NAME_INPUT_LENGTH                 = 128;   // 名前入力の文字列の長さ

static const u32  GTS_DEFAULT_NOTIFICATION_WAIT_FRAME   = 60;

static const u32  GTS_RANDOM_WAIT_FRAME                 = 10;    // セーブする時のランダムウェイト値

static const u32  GTS_WARNING_MESSAGE_WAIT_TIME_MILLI_SECOND = 3000; // 3.0秒(3000ミリ秒)


static const int EOM_SIZE            = System::EOM_LEN;                     ///< EndOfMessage サイズ
static const int TRAINER_NAME_LENGTH = System::STRLEN_PLAYER_NAME;          ///< トレーナー名の長さ
static const int TRAINER_NAME_SIZE   = ( TRAINER_NAME_LENGTH + EOM_SIZE );  ///< トレーナー名のデータサイズ 26byte
static const int POKEMON_NAME_LENGTH = System::STRLEN_POKEMON_NAME;         ///< ポケモン名の長さ
static const int POKEMON_NAME_SIZE   = ( POKEMON_NAME_LENGTH + EOM_SIZE );  /// ポケモン名のデータサイズ 26byte

static const int POKE_IN_FRAME_TOP      =  40;
static const int POKE_IN_FRAME_BOTTOM   = 160;
static const int POKE_IN_FRAME_LEFT     = 265;
static const int POKE_IN_FRAME_RIGHT    = 385;

static const int POKE_OUT_FRAME_TOP     =  85;
static const int POKE_OUT_FRAME_BOTTOM  = 115;
static const int POKE_OUT_FRAME_LEFT    = 460;
static const int POKE_OUT_FRAME_RIGHT   = 490;

enum Gender
{
  GENDER_UNKNOWN,         // きにしない
  GENDER_MALE,            // 男
  GENDER_FMALE,           // 女
  GENDER_MAX,
};

enum LevelBand
{
  LEVEL_BAND_NONE,        // きにしない
  LEVEL_BAND_1_10,        // 1～10
  LEVEL_BAND_11_20,       // 11～20
  LEVEL_BAND_21_30,       // 21～30
  LEVEL_BAND_31_40,       // 31～40
  LEVEL_BAND_41_50,       // 41～50
  LEVEL_BAND_51_60,       // 51～60
  LEVEL_BAND_61_70,       // 61～70
  LEVEL_BAND_71_80,       // 71～80
  LEVEL_BAND_81_90,       // 81～90
  LEVEL_BAND_91_100,      // 91～100
  LEVEL_BAND_MAX,
};


//-----------------------------------
// 検索無視データにする要因
//-----------------------------------
enum IGNORE_REASON
{
  IGNORE_MYPOKE_VALIDATION_FAIL      = 1,      ///< 預けるポケモンが不正チェックで蹴られた
  IGNORE_REQUESTPOKE_VALIDATION_FAIL,          ///< 欲しいポケモンの条件が不正だった
  IGNORE_REQUESTPOKE_NOT_EXIST,                ///< 存在しないポケモン（直接入力）だった
};

//-----------------------------------
// 伝説検索オプション
//-----------------------------------
enum SEARCH_LEGEND_OPTION
{
  SEARCH_LEGEND_OPTION_ON = 0,
  SEARCH_LEGEND_OPTION_OFF,
};

GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSDEFINE_H_INCLUDE
