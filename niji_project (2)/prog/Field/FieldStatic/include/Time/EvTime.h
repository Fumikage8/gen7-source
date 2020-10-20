//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EvTime.h
 *  @brief  ゲーム内時間制御関連
 *  @author tamada
 *  @date 2006.02.03  
 *  @date 2011.09.11    takahashi tomoya が移植
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __EVTIME_H__
#define __EVTIME_H__


#include "GameSys/include/GameData.h"
#include "GameSys/include/NijiTime.h"
#include "GameSys/include/TimeZone.h"

#include "Savedata/include/GameTime.h"


namespace Field{

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * @class EvTime
 * @brief   イベント時間更新管理クラス
*/
//-----------------------------------------------------------------------------
class EvTime
{
public:

  //------------------------------------------------------------------
  /// イベント時間更新処理
  //------------------------------------------------------------------
  static void Update(GameSys::GameData * gdata, b32 is_field = false );

  static void PenaltyTimeSet( GameSys::GameData * gdata );

  //------------------------------------------------------------------
  /// イベント時間更新処理（ゲーム開始時の初回のみ）
  //------------------------------------------------------------------
  static void DeviceUpdateForFirstDay( GameSys::GameData * gdata );

  //------------------------------------------------------------------
  /// イベント時間の取得
  //------------------------------------------------------------------
  static s32 GetAlolaHour( const GameSys::GameData * gdata );   ///< アローラ時間　時間の取得
  static s32 GetAlolaMinute( const GameSys::GameData * gdata ); ///< アローラ時間　分の取得
  static s32 GetAlolaSecond( const GameSys::GameData * gdata ); ///< アローラ時間　秒の取得
  static GameSys::TimeZone::Zone GetAlolaTimeZone(const GameSys::GameData * gdata);  ///< アローラ時間　タイムゾーンの取得
  static bool IsAlolaDayTime(const GameSys::GameData * gdata);  ///< アローラ時間　タイムゾーンが昼間か夜間か？

  static s32 GetDeviceWeek( const GameSys::GameData * gdata );  ///< 本体時間　曜日取得

  static void GetGameStartDeviceDateTime(const GameSys::GameData * gdata, gfl2::system::Date * date);  ///< 本体時間　ゲーム開始時間の取得
  static void GetGameClearDeviceDateTime(const GameSys::GameData * gdata, gfl2::system::Date * date);  ///< 本体時間　ゲームクリア時間の取得
  static void SetGameClearDeviceDateTime(GameSys::GameData * gdata); ///<本体時間　 ゲームクリア時間の設定

  //----------------------------------------------------------------------------
  /**
   *  @brief  現在時間の取得  (本体時間：EvTime)
   */
  //-----------------------------------------------------------------------------
  static void GetDeviceDateTime(GameSys::GameData * gdata, gfl2::system::Date * date);

  //----------------------------------------------------------------------------
  /**
   *  @brief  現在時間の取得  (本体時間：現在進行形)
   */
  //-----------------------------------------------------------------------------
  static void GetDeviceRealDateTime(gfl2::system::Date * date);

  //------------------------------------------------------------------
  ///   誕生日判定結果を取得する (本体時間)
  //------------------------------------------------------------------
  static bool IsBirthday( const GameSys::GameData * gdata );

  //------------------------------------------------------------------
  ///   誕生日かどうかの判定をする (本体時間)
  //------------------------------------------------------------------
  static bool IsBirthday( const GameSys::GameData * gdata, int month, int day );

  //------------------------------------------------------------------
  ///   誕生日設定されている日付を取得する (本体設定)
  //------------------------------------------------------------------
  static void GetBirthday( const GameSys::GameData * gdata, s32* birth_month, s32* birth_day );

  // @fix MMCat[231][226] マップジャンプ時にEvTimeの更新が行われてしまっているために仕方なく実装
  //------------------------------------------------------------------
  ///   分更新イベント処理が発動したか否か
  //------------------------------------------------------------------
  static bool IsUpdateMinuteEvent();

  //------------------------------------------------------------------
  ///   日更新イベント処理が発動したか否か
  //------------------------------------------------------------------
  static bool IsUpdateDayEvent();

private:
  //------------------------------------------------------------------
  ///   誕生日判定を更新する
  //------------------------------------------------------------------
  static bool updateBirthdayFlag( GameSys::GameData * gdata );

  static void UpdateDayEvent(GameSys::GameData * gdata, s32 diff_days, b32 is_first, b32 is_field ); ///< 日付更新関数
  static void UpdateMinuteEvent(GameSys::GameData * gdata, s32 diff_minute, const GameSys::INijiDate* pDate, b32 is_first, b32 is_field);  ///< 分更新関数

  static bool ChangeTimeCalc( Savedata::GameTime* tm,
      const GameSys::DeviceDate* pDate, s32 * diff_day, s32 * diff_minute ); ///< 経過時間の算出

  // 差分の取得
  static void CalcDateDiff( const gfl2::system::Date& sv_date, const gfl2::system::Date& now_date, b32 * is_reverse, s64* diff_minute, s32* diff_day );

  // 本体時間の取得
  static void SetupDeviceDate( const GameSys::GameData * gdata, GameSys::DeviceDate * pDeviceDate );
  static void SetupAlolaTime( const GameSys::GameData * gdata, GameSys::AlolaTime * pAlola );

  //------------------------------------------------------------------
  ///   m_isUpdateMinuteEvent＆m_isUpdateDayEventのクリア処理
  //------------------------------------------------------------------
  static void ClearUpdateEventFlag();

private:

  //! 最初の更新チェック
  static b32 m_FirstUpdate;
  
  //! 一度に更新する分の最大値
  static const u32 MINUTE_UPDATE_MAX;

  // @fix MMCat[231][226] マップジャンプ時にEvTimeの更新が行われてしまっているために仕方なく実装
  static b8 m_isUpdateMinuteEvent;
  static b8 m_isUpdateDayEvent;
};


} // namespace Field

#endif  // __PROG\SRC\FIELD\EVTIME_H__
