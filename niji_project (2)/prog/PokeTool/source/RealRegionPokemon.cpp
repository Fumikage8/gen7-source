//======================================================================
/**
 * @file RealRegionPokemon.cpp
 * @date 2015/12/22 19:30:36
 * @author onoue_masayuki
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "PokeTool/include/RealRegionPokemon.h"

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN(PokeTool)
GFL_NAMESPACE_BEGIN(RealRegion)

enum {
  LATITUDE_INDEX_MAX = 16,
  LONGITUDE_INDEX_MAX = 36,
};

//-----------------------------------------------------------------------------
//xy_doc/ポケモン関連/リアル地域ポケモン仕様.xlsをcsvで出力、手作業で成形したもの
//-----------------------------------------------------------------------------
static const u8 RegionTable[LATITUDE_INDEX_MAX][LONGITUDE_INDEX_MAX] = {
  {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  {  3, 3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  {  5, 5, 5, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
  {  9, 9, 9, 7, 9, 9, 9,11,11,11,11,11,11,11,11, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3,11,11, 8, 8, 2, 2, 2, 2, 2, 2 },
  {  9, 9, 9, 9, 9, 9,12,12,12,12, 4, 4, 4, 4, 4, 4, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9,11,11,11, 8, 8, 8, 8, 8, 8, 8 },
  { 17,13,13,13,13,13,12,12,12,12,14,14,14,14, 4,14,17,17,17,17,17,17,17,17,17,17,17,11,11,11,10,10,10,10,10,10 },
  { 17,13,13,13,13,13,12,12,12,12,14,14,14,14,14,14,17,17,17,17,17,17,17,17,17,17,17,17,16,16,16,10,10,10,10,10 },
  { 17,18,18,18,18,18,16,12,17,17,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17,17,17,18,18,18,18,17,17 },
  { 17,17,17,18,18,18,16,16,17,17,17,17,17,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17,18,18,18,18,15,15 },
  { 17,17,17,17,16,16,16,16,17,17,17,17,17,17,16,16,16,16,17,17,17,17,17,17,17,17,17,17,17,17,17,15,15,15,15,15 },
  { 17,17,17,17,13,13,16,16,17,17,17,17,17,17,13,13,13,13,13,17,17,17,17,17,17,17,17,17,17,17,17, 9,15,15,15,15 },
  { 10,10,10,10,10,10,10,10,10,10,10,13,13,13,13,13,13,13,13, 5, 5, 5, 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 4,15, 4, 4 },
  { 10,10,10,10,10,10,10,10,10,10,10,13,13,13,13,13,13, 5, 5, 5, 5, 5, 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 4, 4, 4, 4 },
  {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1 },
  {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
};

//-----------------------------------------------------------------------------
/**
 * @brief フォルムの決定
 *
 * latitude（緯度）
 *  0x0000～0x4000  : 北緯0度～90度
 *  0xffff～0xc000  : 南緯0.005度～90度
 *
 * longitude(経度）
 *  0x0000～0x7fff  ：東経0度～179.995度
 *  0xffff～0x8000  ：西経0.005度～180度
 */
//-----------------------------------------------------------------------------
u8 DecideRegion( u16 latitude, u16 longitude )
{
  // 緯度80度以上はすべて80度として扱う
  static  const float LATITUDE_LIMIT = 80.0f;

  // 経度0度とテーブルの開始位置とのずれ
  static const float LONGITUDE_GAP = 30.0f;

  float deg_latitude;
  if ( 0x0000 <= latitude && latitude <= 0x4000 )
  {
    //RawValue →度に変換
    deg_latitude = latitude * 90.0f / 0x4000;
    if ( deg_latitude > LATITUDE_LIMIT ) deg_latitude = LATITUDE_LIMIT;
  }
  else if ( 0xc000 <= latitude && latitude <=0xffff )
  {
    deg_latitude = static_cast<s16>(latitude) * 90.0f / 0x4000;
    if ( deg_latitude < -LATITUDE_LIMIT ) deg_latitude = -LATITUDE_LIMIT;
  }
  else
  {
    GFL_ASSERT(0);
    return 0;
  }
  int index_latitude = (LATITUDE_LIMIT - deg_latitude) / 10;

  float deg_longitude = (longitude * 360.0f ) / 0x10000 + LONGITUDE_GAP;
  if ( deg_longitude > 360.0f ) deg_longitude -= 360.0f;
  int index_longitude = deg_longitude / 10;

  ONOUE_PRINT("latitude=%04X(%f, %d)\n", latitude, deg_latitude, index_latitude );
  ONOUE_PRINT("longitude=%04X(%f, %d)\n", longitude, deg_longitude, index_longitude );

  int form_index = RegionTable[index_latitude][index_longitude] - 1; // 1 org --> 0 org
  
  //20種類いるが2種類は配布用
  if( 0 <= form_index && form_index < 18 )
  {
    return form_index;
  }
  GFL_ASSERT(0);  //範囲外のフォルム
  return 0;
}

GFL_NAMESPACE_END(RealRegion)
GFL_NAMESPACE_END(PokeTool)
