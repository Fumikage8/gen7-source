// ���Ԏ擾
// �֐���������΃v���b�g�t�H�[�����ƂɃt�@�C���𕪊����܂�

#if defined(GFL_PLATFORM_MICROSOFT) | defined(GFL_PLATFORM_MINGW)

#include <windows.h>
// -----------------------------------------------------------------------------
// �}�C�N���b�擾
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
// �~���b�X���[�v
// -----------------------------------------------------------------------------
void gfl::core::Time::SleepMilliSecond( const gfl::core::Time::Type val )
{
	Sleep( ( DWORD )val );
}

#elif defined(GFL_PLATFORM_UNIX)

#include <sys/time.h>

// -----------------------------------------------------------------------------
// �}�C�N���b�擾
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
// �~���b�X���[�v
// -----------------------------------------------------------------------------
void gfl::core::Time::SleepMilliSecond( const gfl::core::Time::Type val )
{
	usleep( val );
}

#elif defined(GFL_PLATFORM_3DS)

// -----------------------------------------------------------------------------
// �}�C�N���b�擾
// -----------------------------------------------------------------------------
gfl::core::Time::Type gfl::core::Time::GetCurrentMicroSecond( void )
{
	return nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();
}

// -----------------------------------------------------------------------------
// �~���b�X���[�v
// -----------------------------------------------------------------------------
void gfl::core::Time::SleepMilliSecond( const gfl::core::Time::Type t )
{
	nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(t) );
}

#else // ���̑�

// -----------------------------------------------------------------------------
// �}�C�N���b�擾
// -----------------------------------------------------------------------------
gfl::core::Time::Type gfl::core::Time::GetCurrentMicroSecond( void )
{
	return 0;
}

// -----------------------------------------------------------------------------
// �~���b�X���[�v
// -----------------------------------------------------------------------------
void gfl::core::Time::SleepMilliSecond( const gfl::core::Time::Type )
{
}

#endif
