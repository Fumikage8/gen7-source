//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   TimeZone.cpp
 *  @brief  時間帯管理　
 *  @author tomoya takahashi
 *  @date   2011.09.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <system/include/Date/gfl2_Date.h>

#include "GameSys/include/TimeZone.h"
#include "GameSys/include/NijiTime.h"
#include "debug/include/gfl2_Assert.h"

namespace GameSys
{

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

  // @note 後々は、プランナーが操作できる形にしたかった・・・。
  const TimeZone::Zone TimeZone::sc_SEASON_TIME_ZONE[ 24 ] = {
    TimeZone::MIDNIGHT,     // 0
    TimeZone::MIDNIGHT,     // 1
    TimeZone::MIDNIGHT,     // 2
    TimeZone::MIDNIGHT,     // 3
    TimeZone::MIDNIGHT,     // 4
    TimeZone::MIDNIGHT,     // 5
    TimeZone::MORNING,      // 6
    TimeZone::MORNING,      // 7
    TimeZone::MORNING,      // 8
    TimeZone::MORNING,      // 9
    TimeZone::NOON,         // 10
    TimeZone::NOON,         // 11
    TimeZone::NOON,         // 12
    TimeZone::NOON,         // 13
    TimeZone::NOON,         // 14
    TimeZone::NOON,         // 15
    TimeZone::NOON,         // 16
    TimeZone::EVENING,      // 17
    TimeZone::NIGHT,        // 18
    TimeZone::NIGHT,        // 19
    TimeZone::NIGHT,        // 20
    TimeZone::NIGHT,        // 21
    TimeZone::NIGHT,        // 22
    TimeZone::NIGHT,        // 23
  };

  const TimeZone::Zone TimeZone::sc_SKYBOX_TIME_ZONE[ 24 ] = 
  {
    TimeZone::MIDNIGHT,     // 0
    TimeZone::MIDNIGHT,     // 1
    TimeZone::MIDNIGHT,     // 2
    TimeZone::MIDNIGHT,     // 3
    TimeZone::MIDNIGHT,     // 4
    TimeZone::MIDNIGHT,     // 5
    TimeZone::MORNING,      // 6
    TimeZone::MORNING,      // 7
    TimeZone::MORNING,      // 8
    TimeZone::MORNING,      // 9
    TimeZone::NOON,         // 10
    TimeZone::NOON,         // 11
    TimeZone::NOON,         // 12
    TimeZone::NOON,         // 13
    TimeZone::NOON,         // 14
    TimeZone::NOON,         // 15
    TimeZone::NOON,         // 16
    TimeZone::EVENING,      // 17
    TimeZone::NIGHT,        // 18
    TimeZone::NIGHT,        // 19
    TimeZone::NIGHT,        // 20
    TimeZone::NIGHT,        // 21
    TimeZone::NIGHT,        // 22
    TimeZone::NIGHT,        // 23
  };

  // 単位は分
  const u32 TimeZone::sc_SKYBOX_TIME_ZONE_CHANGE[ TimeZone::MAX ] = {
    20, // MIDNIGHT TO MORNING 
    20, // MORNING TO NOON
    20, // NOON TO EVENING
    10, // EVENING TO NIGHT
    20, // NIGHT TO MIDNIGHT
  };


  //----------------------------------------------------------------------------
  /**
   *  @brief  今の時間からタイムゾーンを取得
   *
   *  @return タイムゾーン
   */
  //-----------------------------------------------------------------------------
  TimeZone::Zone TimeZone::GetNowTimeZone(TimeType type)
  {
    s32 hour, minute, second, milli;
    GetTime(type, &hour, &minute, &second, &milli);
    return ConvertHourToTimeZone( hour );
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  今の時間からタイムゾーンを取得
   *
   *  @return タイムゾーン
   */
  //-----------------------------------------------------------------------------
  TimeZone::Zone TimeZone::GetNowSkyboxTimeZone(TimeType type)
  {
    s32 hour, minute, second, milli;
    GetTime(type, &hour, &minute, &second, &milli);
    return ConvertHourToSkyboxTimeZone( hour );
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  時間からタイムゾーンを取得
   *
   *  @param  hour    時間
   *
   *  @return タイムゾーン
   */
  //-----------------------------------------------------------------------------
  TimeZone::Zone TimeZone::ConvertHourToTimeZone( s32 hour )
  {
    if(0 <= hour && hour < 24)
    {
      return sc_SEASON_TIME_ZONE[hour];
    }
    GFL_ASSERT(0);
    return MORNING;
  }


  //----------------------------------------------------------------------------
  /**
   *  @brief  タイムゾーンからタイムゾーン開始時間を取得
   *
   *  @param  timezone    
   *
   *  @return タイムゾーン開始時間
   */
  //-----------------------------------------------------------------------------
  s32 TimeZone::ConvertTimeZoneToStartHour( TimeZone::Zone timezone )
  {
    if( timezone < TimeZone::MAX )
    {
      s32 hour;
      for( hour=0; hour<24; ++hour ){
        if( sc_SEASON_TIME_ZONE[hour] == timezone ){
          return hour;
        }
      }
    }
    return 0;
  }


  //----------------------------------------------------------------------------
  /**
   *  @brief  時間から天球タイムゾーンを取得
   *
   *  @param  hour    時間
   *
   *  @return タイムゾーン
   */
  //-----------------------------------------------------------------------------
  TimeZone::Zone TimeZone::ConvertHourToSkyboxTimeZone( s32 hour )
  {
    // @fix KW[1652]
    if( hour >= 24 ){
      GFL_ASSERT(0);
      hour = 0;
    }
    return sc_SKYBOX_TIME_ZONE[hour];
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  タイムゾーンからタイムゾーン開始時間を取得
   *
   *  @param  timezone    天球のタイムゾーン
   *
   *  @return タイムゾーン開始時間
   */
  //-----------------------------------------------------------------------------
  s32 TimeZone::ConvertSkyboxTimeZoneToStartHour( TimeZone::Zone timezone )
  {
    if( timezone < TimeZone::MAX )
    {
      s32 hour;
      for( hour=0; hour<24; ++hour ){
        if( sc_SKYBOX_TIME_ZONE[hour] == timezone ){
          return hour;
        }
      }
    }
    
    return 0;
  }



  //----------------------------------------------------------------------------
  /**
   *  @brief  演出用タイムゾーン　取得
   *
   *  @param  p_frame     現在フレーム格納先
   *  @param  frame_max   最大フレーム
   *
   *  @retval 時間帯  （移行時間帯込）
   */
  //-----------------------------------------------------------------------------
  TimeZone::Zone TimeZone::GetSkyboxProductionTimeZone( s32 hour, s32 minute, s32 second, s32 milli, f32 * p_frame, const f32 frame_max )
  {
    // 分単位の今の時間にする。
    s32 now_minutes = (hour * 60) + minute;
    s32 now_minutes_ms = (now_minutes * 60 * 1024) + (second * 1024) + milli;

    // 純粋時間帯取得
    Zone zone = ConvertHourToSkyboxTimeZone( hour );

    // 純粋時間帯開始　時間を求める。
    s32 timezone_start_minutes = ConvertSkyboxTimeZoneToStartHour( zone );
    timezone_start_minutes *= 60;

    // 移行完了時間を求める。
    s32 timezone_change_end_minutes = timezone_start_minutes + sc_SKYBOX_TIME_ZONE_CHANGE[ zone ];

    // 次のタイムゾーンの開始時間
    s32 nexttimezone_start_minutes;
    if( zone + 1 < TimeZone::MAX ){
      nexttimezone_start_minutes = ConvertSkyboxTimeZoneToStartHour( static_cast<Zone>(zone + 1) ) * 60;
    }else{
      nexttimezone_start_minutes = ConvertSkyboxTimeZoneToStartHour( MORNING ) * 60;
    }
    nexttimezone_start_minutes = (nexttimezone_start_minutes < timezone_start_minutes) ? (nexttimezone_start_minutes + 24*60) : nexttimezone_start_minutes;
    

    //　タイムゾーン　タイムゾーンローカル　完了時間（ミリ秒）　今時間（ミリ秒）
    TimeZone::Zone ret_timezone;

    // 移行期間
    if( timezone_change_end_minutes > now_minutes )
    {
      s32 local_ms = now_minutes_ms - (timezone_start_minutes * 60 * 1024);
      s32 local_max_ms = (timezone_change_end_minutes - timezone_start_minutes) * 60 * 1024;

      ret_timezone = static_cast<Zone>(zone + TimeZone::MAX);  

      if( p_frame != NULL ){
        *p_frame = static_cast<f32>((static_cast<f64>(frame_max) * static_cast<f64>(local_ms)) / static_cast<f64>(local_max_ms));
      }
    }
    // 移行後期間
    else
    {
      s32 local_ms = now_minutes_ms - (timezone_change_end_minutes * 60 * 1024);
      s32 local_max_ms = (nexttimezone_start_minutes - timezone_change_end_minutes) * 60 * 1024;

      ret_timezone = zone;

      if( p_frame != NULL ){
        *p_frame = static_cast<f32>((static_cast<f64>(frame_max) * static_cast<f64>(local_ms)) / static_cast<f64>(local_max_ms));
      }
    }

    return ret_timezone;
  }

  TimeZone::Zone TimeZone::GetSkyboxProductionTimeZone( f32 * p_frame, const f32 frame_max, TimeType type )
  {
    s32 hour, minute, second, milli;
    GetTime(type, &hour, &minute, &second, &milli);
    return GetSkyboxProductionTimeZone( hour, minute, second, milli, p_frame, frame_max );
  }


  //----------------------------------------------------------------------------
  /**
   *  @brief  太陽移動フレーム計算  朝⇒夜　で 0〜frame_max段階の値を返す。　ちゃんと12:00はframe_max / 2.0の値になる。
   *
   *  @param  frame_max   フレーム最大値
   *
   *  @return frame 
   */
  //-----------------------------------------------------------------------------
  f32 TimeZone::GetSkyboxSunFrame( const f32 frame_max, TimeType type )
  {
    s32 hour, minute, second, milli;
    GetTime(type, &hour, &minute, &second, &milli);
    return GetSkyboxSunFrame( hour, minute, second, milli, frame_max );
  }

  f32 TimeZone::GetSkyboxSunFrame( s32 hour, s32 minute, s32 second, s32 milli, const f32 frame_max )
  {
    // 分単位の今の時間にする。
    s32 now_minutes = (hour * 60) + minute;
    s32 now_minutes_ms = (now_minutes * 60 * 1024) + (second * 1024) + milli;

    // 純粋時間帯取得
    s32 morning_start_minutes = (ConvertSkyboxTimeZoneToStartHour( MORNING ) * 60) + sc_SKYBOX_TIME_ZONE_CHANGE[ MORNING ];
    s32 night_start_minutes = (ConvertSkyboxTimeZoneToStartHour( NIGHT ) * 60) + sc_SKYBOX_TIME_ZONE_CHANGE[ NIGHT ];

    s32 morning_start_ms = morning_start_minutes * 60 * 1024;
    s32 night_start_ms = night_start_minutes * 60 * 1024;
    s32 local_ms = 0;
    s32 max_ms = 0;
    f32 offset_rate = 0.0f;
    const f32 half_frame = frame_max * 0.25f;
    const s32 noon_ms = 12 * 60 * 60 * 1024;
    const s32 midnight_ms = 24 * 60 * 60 * 1024;

    // 正午 未満 以後 
    // 0:00　未満　以後で、時間のscaleが変わる。
    // 朝以降　正午　未満
    if( (noon_ms > now_minutes_ms) && (morning_start_ms <= now_minutes_ms) ){
      local_ms = now_minutes_ms - morning_start_ms;
      max_ms = noon_ms - morning_start_ms;
      offset_rate = 0.0f;
    }
    // 正午以後　夜未満
    else if( (night_start_ms > now_minutes_ms) && (noon_ms <= now_minutes_ms) ){
      local_ms = now_minutes_ms - noon_ms;
      max_ms = night_start_ms - noon_ms;
      offset_rate = half_frame;
    }
    // 夜以後　24:00未満
    else if( (midnight_ms > now_minutes_ms) && (night_start_ms <= now_minutes_ms) ){
      local_ms = now_minutes_ms - night_start_ms;
      max_ms = midnight_ms - night_start_ms;
      offset_rate = half_frame * 2.0f;
    }
    // 0:00以降　朝未満
    else {
      local_ms = now_minutes_ms;
      max_ms = morning_start_ms;
      offset_rate = half_frame * 3.0f;
    }

    /*
    TOMOYA_PRINT( "noon_ms %d now_minutes_ms %d night_start_ms %d morning_start_ms %d\n", static_cast<s32>(noon_ms), static_cast<s32>(now_minutes_ms), static_cast<s32>(night_start_ms), static_cast<s32>(morning_start_ms) );
    TOMOYA_PRINT( "frame %f hour %d minute %d second %d\n", offset_rate + static_cast<f32>(static_cast<f64>(local_ms) * static_cast<f64>(half_frame) / static_cast<f64>(max_ms)),
        hour, minute, second );
        */
    
    return offset_rate + static_cast<f32>(static_cast<f64>(local_ms) * static_cast<f64>(half_frame) / static_cast<f64>(max_ms));
  }


  /**
   *  @brief 時間の取：得
   */
  void TimeZone::GetTime( TimeType type, s32* hour, s32* minute, s32* second, s32* milli )
  {
    if( type == TYPE_ALOLA )
    {
      AlolaTime date;
      date.Initialize();
      *hour     = date.GetHour();
      *minute   = date.GetMinute();
      *second   = date.GetSecond();
      *milli    = date.GetMilliSecond();
    }
    else if( type == TYPE_DEVICE )
    {
      DeviceDate date;
      *hour     = date.GetHour();
      *minute   = date.GetMinute();
      *second   = date.GetSecond();
      *milli    = date.GetMilliSecond();
    }
    else
    {
      GFL_ASSERT(0);
      *hour     = 0;
      *minute   = 0;
      *second   = 0;
      *milli    = 0;
    }
  }

}  // namespace GameSys

