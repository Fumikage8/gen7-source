//=============================================================================
/**
 * @file    TownMapDef.h
 * @brief   タウンマップ画面汎用定義
 * @author  Hiroyuki Nakamura
 * @date    2015.10.22
 */
//=============================================================================
#if !defined( TOWNMAP_DEF_H_INCLUDED )
#define TOWNMAP_DEF_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)

enum
{
  LAND_DATA_MAX = 5,        //!< 島データ数
  LAND_ZONE_DATA_MAX = 16,  //!< 島ごとのゾーンデータ数

  PUT_WARPMSG_NONE = -1,    //!< 飛び先不明のワープメッセージ

  TM_DUMMY_DATA = -1,       //!< 汎用エラーコード
};

//! 場所アイコンタイプ
enum PlaceType
{
  PLACE_TYPE_PC = 0,        //!< ポケモンセンター
  PLACE_TYPE_DUNGEON,       //!< ダンジョン
  PLACE_TYPE_SHOP,          //!< 施設（汎用）
  PLACE_TYPE_WARP,          //!< ワープポイント
  PLACE_TYPE_EVENT,         //!< イベント
  PLACE_TYPE_MONS,          //!< 分布用
  PLACE_TYPE_MONS_A,        //!< 分布用（昼）
  PLACE_TYPE_MONS_E,        //!< 分布用（夜）
  PLACE_TYPE_FISH,          //!< 分布・釣り用
  PLACE_TYPE_FISH_A,        //!< 分布・釣り用（昼）
  PLACE_TYPE_FISH_E,        //!< 分布・釣り用（夜）
  PLACE_TYPE_MONS_ENTER,    //!< 分布・乱入
  PLACE_TYPE_MONS_ENTER_A,  //!< 分布・乱入（昼）
  PLACE_TYPE_MONS_ENTER_E,  //!< 分布・乱入（夜）
  PLACE_TYPE_FISH_ENTER,    //!< 分布・釣り乱入
  PLACE_TYPE_FISH_ENTER_A,  //!< 分布・釣り乱入（昼）
  PLACE_TYPE_FISH_ENTER_E,  //!< 分布・釣り乱入（夜）
  PLACE_TYPE_NONE,          //!< 表示なし
  PLACE_TYPE_MAX,           //!< 最大値
};

//! リソースデータ：ゾーン
struct ZONE_RESOURCE
{
  s32 zone_id;        //!< ゾーンID
  s32 arrival_flag;   //!< 到着フラグ
  s32 info_txt;       //!< 説明テキスト
};

//!< リソースデータ：アイコン
struct ICON_RESOURCE
{
  s32 land_no;        //!< 島番号
  s32 zone_id;        //!< ゾーンID
  s32 icon_type;      //!< アイコン種類
  u32 x;              //!< X座標
  u32 y;              //!< Y座標
  s32 arrival_flag;   //!< 到着フラグ
  s32 warp_id;        //!< ワープID
  s32 warp_use_flag;  //!< ワープ使用フラグ
  s32 icon_flag;      //!< アイコン表示フラグ
  s32 facility_name;  //!< 施設名
  s32 is_place_name;  //!< 施設名が場所名か
};


GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)

#endif  // TOWNMAP_DEF_H_INCLUDED
