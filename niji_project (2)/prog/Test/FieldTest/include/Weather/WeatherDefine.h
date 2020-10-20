//===================================================================
/**
 * @file    WeatherDefine.h
 * @brief   天候の定義
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#if !defined( WEATHERDEFINE_H_INCLUDED )
#define WEATHERDEFINE_H_INCLUDED
#pragma once

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Weather )


/**
 * @brief 天候の種類
*/
enum Type
{
  TYPE_SHINE,           // 晴れ
  TYPE_RAIN,            // 雨

#if 0
  // 他にもこんなの
  TYPE_CLOUDY,          // 曇り
  TYPE_THUNDERSTORM,    // 雷雨
#endif

  TYPE_MAX,
  TYPE_NONE = TYPE_MAX,
};


GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

#endif // WEATHERDEFINE_H_INCLUDED

