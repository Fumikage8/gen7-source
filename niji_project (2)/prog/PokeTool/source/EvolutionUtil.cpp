//======================================================================
/**
 * @file EvolutionUtil.cpp
 * @date 2015/09/10 17:30:31
 * @author saita_kazuki
 * @brief ポケモン進化用補助ツール
 * @note sangoから移殖
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __POKE_TOOL_EVOLUTION_UTIL_H_INCLUDED__
#define __POKE_TOOL_EVOLUTION_UTIL_H_INCLUDED__
#pragma once

#include <PokeTool/include/EvolutionUtil.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <SaveData/include/MyStatus.h>

#include "Field/FieldStatic/include/Time/EvTime.h"

#include "niji_conv_header/field/WeatherKindDef.h"
#include "niji_conv_header/field/zone/zone_id.h"

GFL_NAMESPACE_BEGIN( PokeTool )

//======================================================================
//======================================================================
//------------------------------------------------------------------------
/**
 * @brief 進化条件生成ツール
 */
//------------------------------------------------------------------------
void SetupEvolveSituation(
  pml::pokepara::EvolveSituation * situation,
  GameSys::GameManager * p_gman,
  bool is_device_turned_over,
  Field::weather::WeatherKind weather
  )
{
  GameSys::GameData * gamedata = p_gman->GetGameData();

  situation->is_magnetic_field = false;
  situation->is_ice_field = false;
  situation->is_moss_field = false;
  situation->is_snow_mountain = false;
  situation->is_ultra_space = false;
  situation->is_morning_or_noon = false;
  situation->is_night = false;
  situation->is_evening = false;
  situation->is_device_turned_over = is_device_turned_over;
  situation->is_rain = false;
  
  gamedata->GetPlayerStatus()->CreateOwnerInfo( situation->owner_info );

  //時間帯の判定
  GameSys::TimeZone::Zone timezone = Field::EvTime::GetAlolaTimeZone( gamedata);
  switch ( timezone )
  {
  case GameSys::TimeZone::MORNING:
  case GameSys::TimeZone::NOON:
    situation->is_morning_or_noon = true;
    break;
  case GameSys::TimeZone::EVENING:  //夕方は午後にも含まれる
    situation->is_morning_or_noon = true;
    situation->is_evening = true;
    break;


  case GameSys::TimeZone::NIGHT:
  case GameSys::TimeZone::MIDNIGHT:
    situation->is_night = true;
    break;
  }

  //特定場所かどうかの判定
  switch ( gamedata->GetNowZoneID() )
  {
  // 磁場のある場所
  // 対象ポケモン : ジバコイル、ダイノーズ、クワガタ3
  // niji -> ポニの大峡谷 momiji->ホテリ山を追加
  case Z_F0401D0201:
  case Z_F0401D0202:
  case Z_F0401D0203:
  case Z_F0401D0204:
  case Z_F0401D0205:
  case Z_F0401H0701:
  case Z_F0301D0601://ホテリ山
  // @fix GFMMCat[497] フィールド進化条件に抜けているゾーンが存在する >> 対象ゾーンを追加
  //↓ここから
  case Z_F0301I1401://地熱発電所
  //↑ここまで
    situation->is_magnetic_field = true;
    break;

  // 氷の岩のある場所
  // 対象ポケモン : グレイシア
  // niji -> こおりの洞窟
  case Z_F0301D1201:
    situation->is_ice_field = true;
    situation->is_snow_mountain = true; // @fix NMCat[3287] こおりの洞窟はラナキラマウンテンの中にある
    break;

  // 苔むした岩のある場所
  // 対象ポケモン : リーフィア
  // niji -> シェードジャングル
  case Z_F0201H0403:
    situation->is_moss_field = true;
    break;

  // 雪山
  // 対象ポケモン : ヤシガニ2
  // niji -> ラナキラマウンテン
  case Z_F0301D1001:
  case Z_F0301D1002:
  case Z_F0301D1003:
  case Z_F0301D1004:
  // @fix GFMMCat[497] フィールド進化条件に抜けているゾーンが存在する >> 対象ゾーンを追加
  //↓ここから
  case Z_F0301D1005: //チャンピオンロード外マップ
  case Z_F0301D1203: //こおりの洞窟：チャンピオンロード洞窟マップ２
  //↑ここまで
    situation->is_snow_mountain = true;
    break;

  // ウルトラスペース
  // 対象ポケモン : ピカチュウ、タマタマ、カラカラ
  // momiji -> ＵＨ世界
  case Z_F0701PH0101:
  case Z_F0701PH0201:
  case Z_F0701PH0202:
  case Z_F0701PH0203:
  case Z_F0701PH0204:
  case Z_F0701PH0301:
  case Z_F0701PH0401:
  case Z_F0701PH0501:
  case Z_F0701PH0601:
  case Z_F0701PH0701:
  case Z_F0701PH0801:
  case Z_F0701PH0802:
  case Z_F0701PH0901:
  case Z_F0701PH1001:
  case Z_F0701PH1101:
  case Z_F0701PH1201:
    situation->is_ultra_space = true;
    break;
  }

  //天候の判定
  switch( weather )
  {
  case Field::weather::RAIN:
  case Field::weather::THUNDERSTORM:
  case Field::weather::MIST:
  case Field::weather::SUNSHOWER:
    situation->is_rain = true;
    break;
  }

}

GFL_NAMESPACE_END( PokeTool )

#endif // __POKE_TOOL_EVOLUTION_UTIL_H_INCLUDED__
