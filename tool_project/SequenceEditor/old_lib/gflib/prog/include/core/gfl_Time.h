/* 
 * File:   gfl_Time.h
 * Author: s-kimura
 *
 * Created on 2010/07/13, 14:37
 */

#ifndef GFL_TIME_H
#define	GFL_TIME_H
#pragma once

namespace gfl {
namespace core {

/**
 *  時間クラス
 */

class Time {
public:
	typedef u64 Type;
	static const Type GFL_MILLI_SECOND = 1000; /**< 1秒 */
	static const Type GFL_MICRO_SECOND = 1000000; /**< 1秒 */

private:
	Type _StartTime; /**< 経過時間を計るために必要 */

public:

	/**
	 *   コンストラクタ
	 * @param  無し
	 * @return 無し
	 */

	Time( void ) : _StartTime( 0 ) {
	}

	/**
	 *   デストラクタ
	 * @param  無し
	 * @return 無し
	 */

	~Time( void ) {
	}

	/**
	 *   時間計測開始時間を取得
	 * @param  無し
	 * @return gfl::core::Time::Type 型の時間計測開始時間
	 */

	inline Type GetStartTime( void ) const {
		return _StartTime;
	}

	/**
	 *   現在の時間を取得
	 * @param  無し
	 * @return gfl::core::Time::Type 型の現在時間
	 */
	static Type GetCurrentMicroSecond( void );

	/**
	 *   時間測定を開始
	 * @param  無し
	 * @return 無し
	 */

	inline void Start( void ) {
		_StartTime = GetCurrentMicroSecond( );
	}

	/**
	 *   時間測定を終了
	 * @param  無し
	 * @return 経過時間
	 */

	inline Type Elapsed( void ) {
		Type end_time = GetCurrentMicroSecond( );
		Type begin_time = GetStartTime( );
#ifdef GFL_PLATFORM_PC
		if( end_time < begin_time ){
			end_time += GFL_MICRO_SECOND;
		}
#endif
		return end_time - begin_time;
	}

	/**
	 *   ミリ秒のあいだスリープする
	 * @param  val ミリ秒値
	 * @return 無し
	 */
	static void SleepMilliSecond( const Type val );

	/**
	 *   マイクロ秒をミリ秒に変換
	 * @param  val マイクロ秒
	 * @return ミリ秒
	 */

	static inline Type MicroSecondToMilliSecond( Type val ) {
		return val / 1000;
	}

	/**
	 *   ミリ秒をマイクロ秒に変換
	 * @param  val ミリ秒
	 * @return マイクロ秒
	 */

	static inline Type MilliSecondToMicroSecond( Type val ) {
		return val * 1000;
	}
};

class Mutex
#ifdef GFL_PLATFORM_3DS
//: public nn::os::Mutex
: public nn::os::CriticalSection
#else
: public QMutex
#endif
{
public:
#ifdef GFL_PLATFORM_3DS
#if 0 // 

	inline void UnLock( ) {
		Unlock( );
	}
#else

	Mutex( ) {
		Initialize( );
	}

	inline void Lock( ) {
		Enter( );
	}

	inline void TryLock( ) {
		TryEnter( );
	}

	inline void UnLock( ) {
		Leave( );
	}
#endif
#else

	inline void Lock( ) {
		lock( );
	}

	inline void TryLock( ) {
		tryLock( );
	}

	inline void UnLock( ) {
		unlock( );
	}
#endif
};

class CpuCost : public MinMaxAverage<Time::Type > {
public:

	CpuCost( const u32 reset_frame = GFL_SECOND( 5 ) ) : m_ResetFrame( reset_frame ) {
	}

	bool Update( Time::Type val ) {
		MinMaxAverage< Time::Type >::Update( val );
		if( m_ResetFrame < GetWorkCount( ) ){
			Reset( );
			return true;
		}
		return false;
	}

private:
	u32 m_ResetFrame;
};


}
}

#endif	/* GFL_TIME_H */
