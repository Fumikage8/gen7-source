//=========================================================
/**
 * @file  poketype_def.h
 * @brief ポケモンWB  タイプ指定子等の定義
 * @author  taya
 *
 * @date  2009.10.22  作成
 */
//=========================================================
#if !defined __POKETYPEDEF_H__
#define __POKETYPEDEF_H__
#pragma once

//---------------------------------------------------------
/**
 *  ポケモン・わざのタイプ
 */
//---------------------------------------------------------
enum PokeType_tag{

  POKETYPE_ORIGIN = 0,

  POKETYPE_NORMAL = POKETYPE_ORIGIN,
  POKETYPE_KAKUTOU,
  POKETYPE_HIKOU,
  POKETYPE_DOKU,
  POKETYPE_JIMEN,
  POKETYPE_IWA,
  POKETYPE_MUSHI,
  POKETYPE_GHOST,
  POKETYPE_HAGANE,
  POKETYPE_HONOO,
  POKETYPE_MIZU,
  POKETYPE_KUSA,
  POKETYPE_DENKI,
  POKETYPE_ESPER,
  POKETYPE_KOORI,
  POKETYPE_DRAGON,
  POKETYPE_AKU,
  POKETYPE_FAIRY,

  POKETYPE_MAX,
  POKETYPE_NUMS = POKETYPE_MAX,
  POKETYPE_NULL = POKETYPE_MAX,

  // 以下は古い表記の仕方です。新しく書き起こすソースでは使わないようにお願いします。
  POKETYPE_BATTLE = POKETYPE_KAKUTOU,
  POKETYPE_POISON = POKETYPE_DOKU,
  POKETYPE_METAL = POKETYPE_HAGANE,
  POKETYPE_FIRE = POKETYPE_HONOO,
  POKETYPE_WATER = POKETYPE_MIZU,
  POKETYPE_ELECTRIC = POKETYPE_DENKI,
  POKETYPE_SP = POKETYPE_ESPER,

};


#endif  // __POKETYPEDEF_H__
