/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_Date.cpp
 *  @brief  時間管理クラス
 *  @author miyachi_soichi
 *  @date   2015.50.21
 */

// gfl2
#include <debug/include/gfl2_Assert.h>
#include <system/include/Date/gfl2_Date.h>

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( system )

#if defined( GF_PLATFORM_WIN32 )
static const s32 WIN_BASE_YEAR = 1900;  //!< Win版は基準位置が1900年
static const s32 WIN_2000_YEAR = 100;   //!< CTR合わせでの2000年用差分値
static const s32 WIN_MAXI_YEAR = 289;   //!< 最大年数用差分値
static const double WIN_SEC_TO_DAY = 60.0 * 60.0 * 24.0;  //!< 秒から日への変換用
static const double WIN_SEC_TO_MSEC = 1000.0; // 秒からミリ秒への変換用
#endif

// 日時コア情報
#if defined( GF_PLATFORM_WIN32 )
time_t            Date::m_CoreDate;
#elif defined( GF_PLATFORM_CTR )
nn::fnd::DateTime Date::m_CoreDate;
#endif
bool              Date::m_Initialize = false;
s32               Date::m_Wait = 0;


#if GFL_DEBUG
// フェイクモード用情報
Date::FakeMode    Date::m_FakeMode  = Date::FAKE_NONE;
u32               Date::m_Rate      = 1;
s64               Date::m_BeginSec  = 0;
s64               Date::m_CoreSec   = 0;
s64               Date::m_RecSec    = 0;
#if defined( GF_PLATFORM_WIN32 )
time_t            Date::m_FakeDate;
#elif defined( GF_PLATFORM_CTR )
nn::fnd::DateTime Date::m_FakeDate;
#endif // GF_PLATFORM_WIN32

#endif // GFL_DEBUG

/**
 *  @brief  コンストラクタ
 */
Date::Date( void )
  : m_Date()
{
}

/**
 *  @brief  コンストラクタ
 */
Date::Date( s32 year, s32 month, s32 day, s32 hour, s32 minute, s32 second, s32 millisecond )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  date.tm_sec   = second;
  date.tm_min   = minute;
  date.tm_hour  = hour;
  date.tm_mday  = day;
  date.tm_mon   = month - 1;  // struct tmでは月の扱いが0-11なので[-1]してずらす
  date.tm_year  = year - WIN_BASE_YEAR;
  date.tm_isdst = -1;

  m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_Date = nn::fnd::DateTime::FromParameters( year, month, day, hour, minute, second, millisecond );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  コンストラクタ
 *  @param  date_parameters  日時用構造体
 */
Date::Date( DateParameters date_parameters )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  date.tm_sec   = date_parameters.second;
  date.tm_min   = date_parameters.minute;
  date.tm_hour  = date_parameters.hour;
  date.tm_mday  = date_parameters.day;
  date.tm_mon   = date_parameters.month - 1;  // struct tmでは月の扱いが0-11なので[-1]してずらす
  date.tm_year  = date_parameters.year - WIN_BASE_YEAR;
  date.tm_isdst = -1;

  m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_Date = nn::fnd::DateTime::FromParameters( date_parameters.year, date_parameters.month, date_parameters.day,
                                              date_parameters.hour, date_parameters.minute, date_parameters.second, date_parameters.millisecond );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  デストラクタ
 */
Date::~Date( void )
{
}

/**
 *  @brief  初期化
 */
void Date::Initialize( void )
{
  UpdateCoreDate();
  // 初期化完了
  m_Initialize = true;
}

/**
 *  @brief  更新処理
 */
void Date::Update( bool in_bForceUpdate )
{
  // 初期化チェック
  if( !m_Initialize )
  {
    GFL_ASSERT_MSG( m_Initialize, "DateTime is not Initialized !!\n" );
    return;
  }

  // 通常更新
  {
    ++ m_Wait;
    // 更新タイミングが来た or 強制更新を行う場合
    if( m_Wait >= DATE_UPDATE_WAIT || in_bForceUpdate )
    {
      // コアタイム更新
      UpdateCoreDate();
      m_Wait = 0;
    }
  }

#if GFL_DEBUG
  // フェイクモードの更新
  if( m_FakeMode == FAKE_FAST )
  {
    UpdateFake();
  }
#endif // GFL_DEBUG
}

/**
 *  @brief  現在日時の取得
 */
void Date::GetNow( Date *out_pDate )
{
  if( out_pDate )
  {
#if GFL_DEBUG
    if( m_FakeMode != Date::FAKE_NONE )
    {
      out_pDate->m_Date = m_FakeDate;
    }
    else
    {
      out_pDate->m_Date = m_CoreDate;
    }
#else
    out_pDate->m_Date = m_CoreDate;
#endif // GFL_DEBUG
  }
}

/**
 *  @brief  コピー
 */
void Date::Copy( const Date *in_pSource, Date *out_pDate )
{
  if( in_pSource != NULL && out_pDate != NULL )
  {
    out_pDate->m_Date = in_pSource->m_Date;
  }
}

/**
 *  @brief  最少日時の取得
 */
void Date::GetMin( Date *out_pDate )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  date.tm_year = 0;
  date.tm_mon = 0;
  date.tm_mday = 1;
  date.tm_hour = 0;
  date.tm_min = 0;
  date.tm_sec = 0;
  date.tm_isdst = -1;
  out_pDate->m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  out_pDate->m_Date = nn::fnd::DateTime::MIN_DATETIME;
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  最大日時の取得
 */
void Date::GetMax( Date *out_pDate )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  date.tm_year = WIN_MAXI_YEAR; // 2189(最大年数) - 1900(基準年数)
  date.tm_mon = 11;
  date.tm_mday = 31;
  date.tm_hour = 23;
  date.tm_min = 59;
  date.tm_sec = 59;
  date.tm_isdst = -1;
  out_pDate->m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  out_pDate->m_Date = nn::fnd::DateTime::MAX_DATETIME;
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  年の設定
 */
void Date::SetYear( s32 year )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  date.tm_year = year - WIN_BASE_YEAR;
  m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_Date = m_Date.ReplaceYear( year );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  月の設定
 */
void Date::SetMonth( s32 month )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  date.tm_mon = month - 1;
  m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_Date = m_Date.ReplaceMonth( month );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  日の設定
 */
void Date::SetDay( s32 day )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  date.tm_mday = day;
  m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_Date = m_Date.ReplaceDay( day );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  時の設定
 */
void Date::SetHour( s32 hour )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  date.tm_hour = hour;
  m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_Date = m_Date.ReplaceHour( hour );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  分の設定
 */
void Date::SetMinute( s32 minute )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  date.tm_min = minute;
  m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_Date = m_Date.ReplaceMinute( minute );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  秒の設定
 */
void Date::SetSecond( s32 second )
{
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  date.tm_sec = second;
  m_Date = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_Date = m_Date.ReplaceSecond( second );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  ミリ秒の設定
 */
void Date::SetMilliSecond( s32 millisecond )
{
#if defined( GF_PLATFORM_WIN32 )
  // struct tmが非対応のため処理せず
#elif defined( GF_PLATFORM_CTR )
  m_Date = m_Date.ReplaceMilliSecond( millisecond );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  年の取得
 */
s32 Date::GetYear( void ) const
{
  s32 result = 2000;
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  result = date.tm_year + WIN_BASE_YEAR;
#elif defined( GF_PLATFORM_CTR )
  result = m_Date.GetYear();
#endif // GF_PLATFORM_WIN32
  return result;
}

/**
 *  @brief  月の取得
 */
s32 Date::GetMonth( void ) const
{
  s32 result = 1;
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  result = date.tm_mon + 1;  // struct tmでは月の扱いが0-11なので[+1]してずらす
#elif defined( GF_PLATFORM_CTR )
  result = m_Date.GetMonth();
#endif // GF_PLATFORM_WIN32
  return result;
}

/**
 *  @brief  曜日の取得
 */
s32 Date::GetWeek( void ) const
{
  s32 result = 0;
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  result = date.tm_wday;
#elif defined( GF_PLATFORM_CTR )
  result = m_Date.GetWeek();
#endif // GF_PLATFORM_WIN32
  return result;
}

/**
 *  @brief  日の取得
 */
s32 Date::GetDay( void ) const
{
  s32 result = 1;
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  result = date.tm_mday;
#elif defined( GF_PLATFORM_CTR )
  result = m_Date.GetDay();
#endif // GF_PLATFORM_WIN32
  return result;
}

/**
 *  @brief  時の取得
 */
s32 Date::GetHour( void ) const
{
  s32 result = 0;
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  result = date.tm_hour;
#elif defined( GF_PLATFORM_CTR )
  result = m_Date.GetHour();
#endif // GF_PLATFORM_WIN32
  return result;
}

/**
 *  @brief  分の取得
 */
s32 Date::GetMinute( void ) const
{
  s32 result = 0;
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  result = date.tm_min;
#elif defined( GF_PLATFORM_CTR )
  result = m_Date.GetMinute();
#endif
  return result;
}

/**
 *  @brief  秒の取得
 */
s32 Date::GetSecond( void ) const
{
  s32 result = 0;
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  result = date.tm_sec;
#elif defined( GF_PLATFORM_CTR )
  result = m_Date.GetSecond();
#endif // GF_PLATFORM_WIN32
  return result;
}

/**
 *  @brief  ミリ秒の取得
 */
s32 Date::GetMilliSecond( void ) const
{
#if defined( GF_PLATFORM_WIN32 )
  // Win32版は非サポート
  return 0;
#elif defined( GF_PLATFORM_CTR )
  return m_Date.GetMilliSecond();
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  パラメータをまとめて取得する
 */
DateParameters Date::GetParameters( void ) const
{
  // 返却用
  DateParameters result;

#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  result.year         = date.tm_year + WIN_BASE_YEAR;// struct tmでは年の扱いが1900からなので[+1900]してずらす
  result.month        = date.tm_mon + 1;  // struct tmでは月の扱いが0-11なので[+1]してずらす
  result.day          = date.tm_mday;
  result.week         = date.tm_wday;
  result.hour         = date.tm_hour;
  result.minute       = date.tm_min;
  result.second       = date.tm_sec;
  result.millisecond  = 0;
#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTimeParameters dst = m_Date.GetParameters();
  result.year         = dst.year;
  result.month        = dst.month;
  result.day          = dst.day;
  result.week         = dst.week;
  result.hour         = dst.hour;
  result.minute       = dst.minute;
  result.second       = dst.second;
  result.millisecond  = dst.milliSecond;
#endif // GF_PLATFORM_WIN32

  return result;
}

/**
 *  @brief  経過日数を取得する
 */
s64 Date::GetDateTimeToDay( void ) const
{
  s64 result = 0;
#if defined( GF_PLATFORM_WIN32 )
  time_t date2000;
  tm dstTM;
  dstTM.tm_year = WIN_2000_YEAR;
  dstTM.tm_mon = 0;
  dstTM.tm_mday = 1;
  dstTM.tm_hour = 0;
  dstTM.tm_min = 0;
  dstTM.tm_sec = 0;
  dstTM.tm_isdst = -1;

  date2000 = mktime( &dstTM );

  double diff = difftime( m_Date, date2000 ); // 秒で返却されているので変換が必要
  diff = diff / WIN_SEC_TO_DAY;
  result = static_cast<s64>( diff );
#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTime date2000;
  nn::fnd::TimeSpan time = m_Date - date2000;

  result = time.GetDays();
#endif //GF_PLATFORM_WIN32

  return result;
}

/**
 *  @brief  経過秒数を取得する
 */
s64 Date::GetDateTimeToSecond( void ) const
{
  s64 result = 0;
#if defined( GF_PLATFORM_WIN32 )
  time_t date2000;
  tm dstTM;
  dstTM.tm_year = WIN_2000_YEAR;
  dstTM.tm_mon = 0;
  dstTM.tm_mday = 1;
  dstTM.tm_hour = 0;
  dstTM.tm_min = 0;
  dstTM.tm_sec = 0;
  dstTM.tm_isdst = -1;

  date2000 = mktime( &dstTM );

  double diff = difftime( m_Date, date2000 ); // 秒で返却されている
  result = static_cast<s64>( diff );
#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTime date2000;
  nn::fnd::TimeSpan time = m_Date - date2000;

  result = time.GetSeconds();
#endif // GF_PLATFORM_WIN32

  return result;
}

/**
 *  @brief  経過ミリ秒を取得する
 */
s64 Date::GetDateTimeToMilliSecond( void ) const
{
  s64 result = 0;
#if defined( GF_PLATFORM_WIN32 )
  time_t date2000;
  tm dstTM;
  dstTM.tm_year = WIN_2000_YEAR;
  dstTM.tm_mon = 0;
  dstTM.tm_mday = 1;
  dstTM.tm_hour = 0;
  dstTM.tm_min = 0;
  dstTM.tm_sec = 0;
  dstTM.tm_isdst = -1;

  date2000 = mktime( &dstTM );

  double diff = difftime( m_Date, date2000 ) * WIN_SEC_TO_MSEC; // 秒で返却されている
  result = static_cast<s64>( diff );
#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTime date2000;
  nn::fnd::TimeSpan time = m_Date - date2000;

  result = time.GetMilliSeconds();
#endif // GF_PLATFORM_WIN32

  return result;
}

/**
 *  @brief  経過秒数を設定する
 */
void Date::SetDateTimeToSecond( s64 second )
{
#if defined( GF_PLATFORM_WIN32 )
  time_t date2000;
  tm dstTM;
  dstTM.tm_year = WIN_2000_YEAR;
  dstTM.tm_mon = 0;
  dstTM.tm_mday = 1;
  dstTM.tm_hour = 0;
  dstTM.tm_min = 0;
  dstTM.tm_sec = 0;
  dstTM.tm_isdst = -1;

  date2000 = mktime( &dstTM );
  m_Date = date2000 + second; 
#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTime date2000;
  nn::fnd::TimeSpan time = nn::fnd::TimeSpan::FromSeconds( second );

  m_Date = date2000 + time;
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  正常な日時か判断する
 *  Win版非対応
 */
bool Date::IsValidDate( void ) const
{
  bool result = true;

#if defined( GF_PLATFORM_WIN32 )
  // Win版非対応
#elif defined( GF_PLATFORM_CTR )
  result = nn::fnd::DateTime::IsValidParameters(
    m_Date.GetYear(),
    m_Date.GetMonth(),
    m_Date.GetDay(),
    m_Date.GetHour(),
    m_Date.GetMinute(),
    m_Date.GetSecond(),
    m_Date.GetMilliSecond()
    );
#endif // GF_PLATFORM_WIN32

  return result;
}

/**
 *  @brief  うるう年か判断する
 */
bool Date::IsLeapYear( void ) const
{
  bool result = false;

#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_Date );
  s32 year = date.tm_year;
  // 400区切りはうるう年
  if( year % 400 == 0 )
  {
    result = true;
  }
  // 100区切りはちがう
  else if( year % 100 == 0 )
  {
    result = false;
  }
  // 4区切りはうるう年
  else if( year % 4 == 0 )
  {
    result = true;
  }
#elif defined( GF_PLATFORM_CTR )
  result = nn::fnd::DateTime::IsLeapYear( m_Date.GetYear() );
#endif

  return result;
}

#if GFL_DEBUG

/**
 *  @brief  フェイクモード開始
 */
void Date::StartFakeTime( u32 rate )
{
  if( m_FakeMode == FAKE_NONE )
  {
    m_FakeDate = m_CoreDate;
  }

  m_FakeMode  = FAKE_FAST;
  m_Rate      = rate;
#if defined( GF_PLATFORM_WIN32 )
  time_t date2000;
  tm dstTM;
  dstTM.tm_year = WIN_2000_YEAR;
  dstTM.tm_mon = 0;
  dstTM.tm_mday = 1;
  dstTM.tm_hour = 0;
  dstTM.tm_min = 0;
  dstTM.tm_sec = 0;
  dstTM.tm_isdst = -1;

  date2000 = mktime( &dstTM );
  double diff = difftime( m_FakeDate, date2000 );
  m_BeginSec = static_cast<s64>( diff );
  diff = difftime( m_CoreDate, date2000 );
  m_CoreSec = static_cast<s64>( diff );
  m_RecSec = m_CoreSec;
#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTime date2000;
  nn::fnd::TimeSpan time = m_FakeDate - date2000;
  m_BeginSec = time.GetSeconds();
  time = m_CoreDate - date2000;
  m_CoreSec = time.GetSeconds();
  m_RecSec = m_CoreSec;
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  フェイクモード終了
 */
void Date::StopFakeTime( void )
{
  m_FakeMode = FAKE_NONE;
}

/**
 *  @brief  時間と分をFixする
 */
void Date::FixFakeTime( s32 hour, s32 minute )
{
  m_FakeMode = FAKE_FIX;
  m_FakeDate = m_CoreDate;
#if defined( GF_PLATFORM_WIN32 )
  tm date;
  localtime_s( &date, &m_FakeDate );
  date.tm_hour = hour;
  m_FakeDate = mktime( &date );
#elif defined( GF_PLATFORM_CTR )
  m_FakeDate = m_FakeDate.ReplaceHour( hour );
  m_FakeDate = m_FakeDate.ReplaceMinute( minute );
#endif // GF_PLATFORM_WIN32
}

/**
 *  @brief  日時をすべてFixする
 */
void Date::FixFakeDate( const Date *in_pDate )
{
  m_FakeMode = FAKE_FIX;
  m_FakeDate = in_pDate->m_Date;
}

/**
 *  @brief  FakeModeの取得
 */
Date::FakeMode Date::GetFakeMode( void )
{
  return m_FakeMode;
}

/**
 *  @brief  Fakeレートの取得
 */
u32 Date::GetFakeRate( void )
{
  return m_Rate;
}

#endif // GFL_DEBUG


/**
 *  @brief  コアタイム更新
 */
void Date::UpdateCoreDate( void )
{
#if defined( GF_PLATFORM_WIN32 )
  time_t timer;
  timer = time( NULL );
  m_CoreDate = timer;
#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTime date = nn::fnd::DateTime::GetNow();
  m_CoreDate = date;
#endif
}

#if GFL_DEBUG
/**
 *  @brief  フェイクデータ更新
 */
void Date::UpdateFake( void )
{
  s64 now = 0;
  s64 fake = 0;
#if defined( GF_PLATFORM_WIN32 )
  time_t date2000;

  {
    tm dstTM;
    dstTM.tm_year = WIN_2000_YEAR;
    dstTM.tm_mon = 0;
    dstTM.tm_mday = 1;
    dstTM.tm_hour = 0;
    dstTM.tm_min = 0;
    dstTM.tm_sec = 0;
    dstTM.tm_isdst = -1;

    date2000 = mktime( &dstTM );
    double diff = difftime( m_CoreDate, date2000 );
    now = static_cast<s64>( diff );
    if( now == m_RecSec )
    {
      return;
    }
  }

  // 偽時間の算出
  fake = m_BeginSec + ( ( now - m_CoreSec ) * m_Rate );
  m_RecSec = now;
  m_FakeDate = date2000 + fake;

#elif defined( GF_PLATFORM_CTR )
  nn::fnd::DateTime date2000;
  nn::fnd::TimeSpan time = m_CoreDate - date2000;
  now = time.GetSeconds();

  if( now == m_RecSec )
  {
    return;
  }

  // 偽時間の算出
  fake = m_BeginSec + ( ( now - m_CoreSec ) * m_Rate );
  m_RecSec = now;
  m_FakeDate = date2000 + nn::fnd::TimeSpan::FromSeconds( fake );
#endif // GF_PLATFORM_WIN32
}
#endif // GFL_DEBUG

GFL_NAMESPACE_END( system )
GFL_NAMESPACE_END( gfl2 )
