// ============================================================================
/*
 * @file GTSTypes.h
 * @brief GTSアプリで使用する型の定義ファイルです。
 * @date 2015.04.07
 */
// ============================================================================
#if !defined( GTSTYPES_H_INCLUDE )
#define GTSTYPES_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/GTS/source/GTSDefine.h"
#include "pml/include/pokepara/pml_PokemonCoreParam.h"
#include "Savedata/include/MyStatus.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


//------------------------------------------------------------------
/**
 * @brief 検索用簡易ポケモンデータ(48byte)
 */
//------------------------------------------------------------------
typedef struct
{
  u16         kind;                        ///< ポケモンの種族NO         +2  = 2
  u8          gender;                      ///< 性別(※男→1、女→2です) +1  = 3
  u8          levelBand;                   ///< レベル幅( 0 - 10)        +1  = 4

  u16         requestKind;                 ///< 欲しいポケモン種族NO     +2  = 6
  u8          requestGender;               ///< 欲しいポケモンの性別     +1  = 7
  u8          requestLevelBand;            ///< 欲しいポケモンのレベル幅 +1  = 8
  u8          requestNoLegend;             ///< 欲しいのは伝説・幻以外   +1  = 9
  u8          searchDisable;               ///< 検索不可フラグ(1で不可） +1  = 12
  u8          form;                        ///< フォルム( 0 - 28 )       +1  = 10
  u8          level;                       ///< レベル( 1 - 100 )        +1  = 11
  u16         nickname[POKEMON_NAME_SIZE]; ///< ポケモンのニックネーム   +26 = 38
  u16         item;                        ///< 持っている道具           +2  = 40
  u8          isRare;                      ///< レアかどうかフラグ       +1  = 41
  u8          haveNickName;                ///< ニックネームをつけている +1  = 42
  u8          ballID;                      ///< 預けたポケモンのボールID +1  = 43
  u8          dummy;                       ///< ダミー（未使用）         +1  = 44
  u32         personalRandom;              ///< レア乱数(レア計算用      +4  = 48 byte
}SimplePokemonInfo;


//------------------------------------------------------------------
/**
 * @brief GTSインデクスデータ(168byte)
 */
//------------------------------------------------------------------
typedef struct
{
  SimplePokemonInfo                 PokeInfo;                        ///< 検索用ポケモン簡易データ       +48 = 48
  u64                               uniqId;                          ///< ユニークID                     +8  = 56
  u64                               dataStoreId;                     ///< データストアID                 +8  = 64
  u32                               principalId;                     ///< プリンシパルID                 +4  = 68
  u16                               TrainerName[TRAINER_NAME_SIZE];  ///< トレーナー名                   +26 = 94
  u16                               messageID;                       ///< 一言メッセージのID             +2  = 96
  u16                               Country;                         ///< 国コード                       +2  = 98
  u16                               Region;                          ///< 地域コード                     +2  = 100
  Savedata::MyStatus::ICON_DATA     iconData;                        ///< 自分の容姿                     +36 = 136
  u8                                romCode;                         ///< 預けた人のROMバージョン        +1  = 137
  u8                                getVersion;                      ///< 捕獲バージョン                 +1  = 138
  u8                                trainerGender;                   ///< 預けたトレーナーの性別         +1  = 139  (※男→1、女→2です)
  u8                                pokemonLanguageCode;             ///< ポケモンパラメータの言語コード +1  = 140
  u8                                languageCode;                    ///< ゲームの言語コード             +1  = 141
  u8                                isNijiNg;                        ///< momiji追加：niji表示禁止フラグ +1  = 142
  u8                                reserve[26];                     ///< 予備領域                       +26 = 168 byte
}GtsIndexData;


//------------------------------------------------------------------
/**
 * @brief GTSポケモンデータ(232byte)
 */
//------------------------------------------------------------------
typedef struct
{
  u8  data[ pml::pokepara::CoreParam::DATASIZE ];    ///< pml::pokepara::CoreParam::Serialize_Core()でデータ取得すること
} GtsSerializePokemonParam;


typedef struct
{
  u8 data[64];    ///< nn::nex::GlobalTradeStationRecordKey の static const qUnsignedInt32 SERIALIZED_SIZE_MAX = 64; と同じ値にすること
  u32 size;
} GtsSerializeRecordKey;


typedef struct
{
  u8 data[64];    ///< nn::nex::GlobalTradeStationTradeKey の static const qUnsignedInt32 SERIALIZED_SIZE_MAX = 64; と同じ値にすること
  u32 size;
} GtsSerializeTradeKey;


typedef struct
{
  GtsIndexData indexData;
  GtsSerializeTradeKey serializeTradeKey;
} GtsSearchResultInfo;


typedef struct
{
  u8 data[256];
  u32 size;
} GtsSignature;


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTYPES_H_INCLUDE
