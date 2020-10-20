//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   TimeZone.h
 *  @brief  時間帯定義
 *  @author tamada GAME FREAK inc.
 *  @author takahashi tomoya 
 *  @date   2011.09.10
 *  
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __TIMEZONE_H__
#define __TIMEZONE_H__

#include <heap/include/gfl2_Heap.h>

namespace GameSys
{ 


//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------
class TimeZone
{
public:

  // タイムゾーンの取得タイプ
  enum TimeType
  {
    TYPE_DEVICE, ///< カントー時間
    TYPE_ALOLA, ///< アローラ時間

    TYPE_MAX,   ///< 最大値
  };

  //! weather/WeatherDefine.h にエイリアスを作成
  enum Zone {
    MORNING = 0,      ///< 朝
    NOON    = 1,      ///< 昼
    EVENING = 2,      ///< 夕
    NIGHT   = 3,      ///< 夜
    MIDNIGHT= 4,      ///< 深夜

    MAX,

    // 移行時間帯
    MIDNIGHT_TO_MORNING = MAX,    ///< 深夜から朝
    MORNING_TO_NOON,              ///< 朝から昼
    NOON_TO_EVENING,              ///< 昼から夕
    EVENING_TO_NIGHT,             ///< 夕から夜
    NIGHT_TO_MIDNIGHT,            ///< 夜から深夜

    ALL_MAX,
  };

  //----------------------------------------------------------------------------
  /**
   *  @brief  今の時間からタイムゾーンを取得
   *
   *  @return タイムゾーン
   */
  //-----------------------------------------------------------------------------
  static TimeZone::Zone GetNowTimeZone(TimeType type = TYPE_ALOLA);


  //----------------------------------------------------------------------------
  /**
   *  @brief  時間からタイムゾーンを取得
   *
   *  @param  hour    時間
   *
   *  @return タイムゾーン
   */
  //-----------------------------------------------------------------------------
  static TimeZone::Zone ConvertHourToTimeZone( s32 hour );

  //----------------------------------------------------------------------------
  /**
   *  @brief  タイムゾーンからタイムゾーン開始時間を取得
   *
   *  @param  timezone    
   *
   *  @return タイムゾーン開始時間
   */
  //-----------------------------------------------------------------------------
  static s32 ConvertTimeZoneToStartHour( TimeZone::Zone timezone );



  /**
   *  @note nijiではSkyboxTimeZoneは使わないことになった為、非公開
   */
private:
  //----------------------------------------------------------------------------
  /**
   *  @brief  今の時間から天球タイムゾーンを取得
   *
   *  @return タイムゾーン
   */
  //-----------------------------------------------------------------------------
  static TimeZone::Zone GetNowSkyboxTimeZone(TimeType type = TYPE_ALOLA);



  //----------------------------------------------------------------------------
  /**
   *  @brief  時間から天球タイムゾーンを取得
   *
   *  @param  hour    時間
   *
   *  @return タイムゾーン
   */
  //-----------------------------------------------------------------------------
  static TimeZone::Zone ConvertHourToSkyboxTimeZone( s32 hour );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  天球タイムゾーンからタイムゾーン開始時間を取得
   *
   *  @param  timezone    天球のタイムゾーン
   *
   *  @return タイムゾーン開始時間
   */
  //-----------------------------------------------------------------------------
  static s32 ConvertSkyboxTimeZoneToStartHour( TimeZone::Zone timezone );


  //----------------------------------------------------------------------------
  /**
   *  @brief  演出用タイムゾーン　取得
   *
   *  @param  p_frame     現在フレーム格納先  (0.0f <= frame < frame_max）
   *  @param  p_frame_max 最大フレーム格納先
   *
   *  @retval 時間帯  （移行時間帯込）
   */
  //-----------------------------------------------------------------------------
  static TimeZone::Zone GetSkyboxProductionTimeZone( f32 * p_frame = NULL, const f32 frame_max = 100.0f, TimeType type = TYPE_ALOLA );
  static TimeZone::Zone GetSkyboxProductionTimeZone( s32 hour, s32 minute, s32 second, s32 milli, f32 * p_frame = NULL, const f32 frame_max = 100.0f );

  //----------------------------------------------------------------------------
  /**
   *  @brief  太陽移動フレーム計算  朝⇒夜　で 0〜frame_max段階の値を返す。　ちゃんと12:00はframe_max / 2.0の値になる。
   *
   *  @param  frame_max   フレーム最大値
   *
   *  @retval 太陽移動フレーム　frame
   *  @retval -1.0f       太陽が昇る前
   *  @retval frame_max   太陽が降りた後
   */
  //-----------------------------------------------------------------------------
  static f32 GetSkyboxSunFrame( const f32 frame_max = 100.0f, TimeType type = TYPE_ALOLA );
  static f32 GetSkyboxSunFrame( s32 hour, s32 minute, s32 second, s32 milli, const f32 frame_max = 100.0f );  ///< 太陽移動フレーム計算  朝⇒夜　で 0〜frame_max段階の値を返す。　ちゃんと12:00はframe_max / 2.0の値になる。


private:

  static void GetTime( TimeType type, s32* hour, s32* minute, s32* second, s32* milli );


  static const Zone sc_SEASON_TIME_ZONE[ 24 ];   ///< 季節ごとの時間帯

  static const Zone sc_SKYBOX_TIME_ZONE[ 24 ];    ///< 天球の時間帯
  static const u32 sc_SKYBOX_TIME_ZONE_CHANGE[ TimeZone::MAX ]; ///< 天球時間帯の遷移にかかる分
  
};

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------



} // namespace GameSys

#endif  // __TIMEZONE_H__

