//======================================================================
/**
 * @file WeatherTypeDef.h
 * @date 2015/11/06 22:10:55
 * @author saito_nozomu
 * @brief 天候の定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __WEATHER_TYPE_DEF_H_INCLUDED__
#define __WEATHER_TYPE_DEF_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

  /**
   * @enum WEATHER_TYPE
   * 天候の種類
   */
  enum WEATHER_TYPE
  {
    WEATHER_NONE,         //! 天候無し
    WEATHER_RAIN,         //! 雨
    WEATHER_MIST,         //! 霧
    WEATHER_THUNDER,      //! 雷雨
    WEATHER_SNOW,         //! 雪
    WEATHER_SNOWSTORM,    //! 吹雪
    WEATHER_DIAMONDDUST,  //! ダイヤモンドダスト
    WEATHER_SANDSTORM,    //! 砂嵐
    WEATHER_MAX,      //! 番兵
  };

  /**
   * @enum WEATHER_CATEGORY
   * 天候のカテゴリ
   */
  enum WEATHER_CATEGORY
  {
    CATEGORY_NONE,      //! 無し
    CATEGORY_RAIN,      //! 雨系
    CATEGORY_SNOW,      //! 雪系
    CATEGORY_SANDSTORM, //! 砂嵐系
  };

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)

#endif // __WEATHER_TYPE_DEF_H_INCLUDED__