// 時間取得
// 関数が増えればプラットフォームごとにファイルを分割します

#if defined(GFL_PLATFORM_MICROSOFT) | defined(GFL_PLATFORM_MINGW)

#include <windows.h>
// -----------------------------------------------------------------------------
// マイクロ秒取得
// -----------------------------------------------------------------------------
gfl::core::Time::Type gfl::core::Time::GetCurrentMicroSecond( void )
{
	LARGE_INTEGER tv;
	LARGE_INTEGER freq;

	QueryPerformanceCounter( &tv );
	QueryPerformanceFrequency( &freq );

	return static_cast < Type >( ( tv.QuadPart * GFL_MICRO_SECOND ) / freq.QuadPart );
}

// -----------------------------------------------------------------------------
// ミリ秒スリープ
// -----------------------------------------------------------------------------
void gfl::core::Time::SleepMilliSecond( const gfl::core::Time::Type val )
{
	Sleep( ( DWORD )val );
}

#elif defined(GFL_PLATFORM_UNIX)

#include <sys/time.h>

// -----------------------------------------------------------------------------
// マイクロ秒取得
// -----------------------------------------------------------------------------
gfl::core::Time::Type gfl::core::Time::GetCurrentMicroSecond( void )
{
	struct timeval tv;
	u64 ret_time;
	gettimeofday( &tv, 0 );
	ret_time = ( ( u64 )( tv.tv_sec * GFL_MICRO_SECOND )+( u64 )tv.tv_usec );
	return static_cast < Type >( ret_time );
}

// -----------------------------------------------------------------------------
// ミリ秒スリープ
// -----------------------------------------------------------------------------
void gfl::core::Time::SleepMilliSecond( const gfl::core::Time::Type val )
{
	usleep( val );
}

#elif defined(GFL_PLATFORM_3DS)

// -----------------------------------------------------------------------------
// マイクロ秒取得
// -----------------------------------------------------------------------------
gfl::core::Time::Type gfl::core::Time::GetCurrentMicroSecond( void )
{
	return nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();
}

// -----------------------------------------------------------------------------
// ミリ秒スリープ
// -----------------------------------------------------------------------------
void gfl::core::Time::SleepMilliSecond( const gfl::core::Time::Type t )
{
	nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(t) );
}

#else // その他

// -----------------------------------------------------------------------------
// マイクロ秒取得
// -----------------------------------------------------------------------------
gfl::core::Time::Type gfl::core::Time::GetCurrentMicroSecond( void )
{
	return 0;
}

// -----------------------------------------------------------------------------
// ミリ秒スリープ
// -----------------------------------------------------------------------------
void gfl::core::Time::SleepMilliSecond( const gfl::core::Time::Type )
{
}

#endif
