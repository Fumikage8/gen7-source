/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_Date.h
 *  @brief  時間管理クラス
 *  @author miyachi_soichi
 *  @date   2015.05.21
 */

#if !defined( __GFL2_DATE_H__ )
#define __GFL2_DATE_H__
#pragma once

#if defined( GF_PLATFORM_WIN32 )
#include <time.h>
#elif defined( GF_PLATFORM_CTR )
#include <nn.h>
#endif // GF_PLATFORM_WIN32

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( system )

/**
 *  日時保持用構造体
 */
struct DateParameters
{
  s32 year;
  s32 month;
  s32 day;
  s32 week;
  s32 hour;
  s32 minute;
  s32 second;
  s32 millisecond;
}; // struct DateParameters

/**
 *  クラス宣言
 *
 */
class Date
{
  // コピーコンストラクタ、代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN( Date );

public:

#if GFL_DEBUG
  /**
   *  フェイクモード定義
   */
  enum FakeMode
  {
    FAKE_NONE,
    FAKE_FIX,
    FAKE_FAST,
  }; // enum FakeMode
#endif // GFL_DEBUG

  /**
   *  @brief  コンストラクタ
   */
  Date( void );

  /**
   *  @brief  コンストラクタ
   *  @param  year        年
   *  @param  month       月
   *  @param  day         日
   *  @param  hour        時
   *  @param  minute      分
   *  @param  second      秒
   *  @param  millisecond ミリ秒
   */
  Date( s32 year, s32 month, s32 day, s32 hour = 0, s32 minute = 0, s32 second = 0, s32 millisecond = 0 );

  /**
   *  @brief  コンストラクタ
   *  @param  date_parameters  日時用構造体
   */
  Date( DateParameters date_parameters );

  /**
   *  @brief  デストラクタ
   */
  ~Date( void );

public:
  /**
   *  @brief  初期化
   *
   *  ゲーム開始時に一度だけ呼び出しを行ってください
   */
  static void Initialize( void );

  /**
   *  @brief  更新処理
   *  @param  in_bForceUpdate   強制更新フラグ
   */
  static void Update( bool in_bForceUpdate = false );

  /**
   *  @brief  現在日時の取得
   *  @param  out_pDate    格納先
   */
  static void GetNow( Date *out_pDate );

  /**
   *  @brief  情報のコピー
   *  @param  in_pSource    情報元
   *  @param  out_pDate     格納先
   */
  static void Copy( const Date *in_pSource, Date *out_pDate );

  /**
   *  @brief  最少日時の取得
   *  @param  out_pDate     格納先
   *  nn::fnd::DateTimeの最少日時( 1900-01-01 00:00:00.000 )を取得します
   */
  static void GetMin( Date *out_pDate );

  /**
   *  @brief  最大日時の取得
   *  @param  out_pDate     格納先
   *  nn::fnd::DateTimeの最大日時( 2189-12-31 23:59:59.999 )を取得します
   */
  static void GetMax( Date *out_pDate );

public:
  //! 年の設定
  void SetYear( s32 year );
  //! 月の設定
  void SetMonth( s32 month );
  //! 日の設定
  void SetDay( s32 day );
  //! 時の設定
  void SetHour( s32 hour );
  //! 分の設定
  void SetMinute( s32 minute );
  //! 秒の設定
  void SetSecond( s32 second );
  //! ミリ秒の設定　Win版非対応
  void SetMilliSecond( s32 millisecond );

  //! 年の取得
  s32 GetYear( void ) const;
  //! 月の取得
  s32 GetMonth( void ) const;
  //! 曜日の取得
  s32 GetWeek( void ) const;
  //! 日の取得
  s32 GetDay( void ) const;
  //! 時の取得
  s32 GetHour( void ) const;
  //! 分の取得
  s32 GetMinute( void ) const;
  //! 秒の取得
  s32 GetSecond( void ) const;
  //! ミリ秒の取得　Win版非対応
  s32 GetMilliSecond( void ) const;

  //! パラメータをまとめて取得
  DateParameters GetParameters( void ) const;

  /**
   *  @brief  経過日数を取得する
   *
   *  2000/01/01 00:00:00.000 からの経過時間が返却されます
   */
  s64 GetDateTimeToDay( void ) const;

  /**
   *  @brief  経過秒数を取得する
   *
   *  2000/01/01 00:00:00.000 からの経過時間が返却されます
   */
  s64 GetDateTimeToSecond( void ) const;

  /**
   *  @brief  経過ミリ秒を取得する
   *
   *  2000/01/01 00:00:00.000 からの経過時間が返却されます
   */
  s64 GetDateTimeToMilliSecond( void ) const;

  /**
   *  @brief  2000/01/01/00:00:00.00からの経過秒数を設定する
   */
  void SetDateTimeToSecond( s64 second );

  /**
   *  @brief  正常な日時か判断する
   *  Win版非対応
   */
  bool IsValidDate( void ) const;

  /**
   *  @brief  うるう年か判断する
   */
   bool IsLeapYear( void ) const;

#if GFL_DEBUG
   /**
    *  @brief  フェイクモード開始
    */
   static void StartFakeTime( u32 rate );

   /**
    * @brief  フェイクモード終了
    */
   static void StopFakeTime( void );

   /**
    * @brief  時間と分をFixする
    * @param  hour    時
    * @param  minute  分
    */
   static void FixFakeTime( s32 hour, s32 minute );

   /**
    * @brief  日時をすべてFixする
    * @param  in_pDate  指定時間
    */
   static void FixFakeDate( const Date *in_pDate );

   /**
    * @brief  FakeModeの取得
    */
   static FakeMode GetFakeMode( void );

   /**
    * @brief  Fakeレートの取得
    */
   static u32 GetFakeRate( void );

#endif // GFL_DEBUG

private:
  //! コアタイム更新
  static void UpdateCoreDate( void );

#if GFL_DEBUG
  /**
   *  @brief  フェイクデータ更新
   */
  static void UpdateFake( void );
#endif // GFL_DEBUG


private:
  // 日時情報
#if defined( GF_PLATFORM_WIN32 )
  time_t                        m_Date;
  static time_t                 m_CoreDate;
#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTime             m_Date;
  static nn::fnd::DateTime      m_CoreDate;
#endif // GF_PLATFORM_WIN32

  static bool                   m_Initialize;
  static s32                    m_Wait;

  // 更新頻度
  static const s32              DATE_UPDATE_WAIT = 10;

#if GFL_DEBUG

  static FakeMode               m_FakeMode;
  static u32                    m_Rate;
  static s64                    m_BeginSec;
  static s64                    m_RecSec;
  static s64                    m_CoreSec;
#if defined( GF_PLATFORM_WIN32 )
  static time_t                 m_FakeDate;
#elif defined( GF_PLATFORM_CTR )
  static nn::fnd::DateTime      m_FakeDate;
#endif // GF_PLATFORM_WIN32

#endif // GFL_DEBUG

}; // class Date

GFL_NAMESPACE_END( system )
GFL_NAMESPACE_END( gfl2 )

#endif // !defined( __GFL2_DATE_H__ )
