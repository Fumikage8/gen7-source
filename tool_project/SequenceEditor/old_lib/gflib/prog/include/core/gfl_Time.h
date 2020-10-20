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
 *  ���ԃN���X
 */

class Time {
public:
	typedef u64 Type;
	static const Type GFL_MILLI_SECOND = 1000; /**< 1�b */
	static const Type GFL_MICRO_SECOND = 1000000; /**< 1�b */

private:
	Type _StartTime; /**< �o�ߎ��Ԃ��v�邽�߂ɕK�v */

public:

	/**
	 *   �R���X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	Time( void ) : _StartTime( 0 ) {
	}

	/**
	 *   �f�X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	~Time( void ) {
	}

	/**
	 *   ���Ԍv���J�n���Ԃ��擾
	 * @param  ����
	 * @return gfl::core::Time::Type �^�̎��Ԍv���J�n����
	 */

	inline Type GetStartTime( void ) const {
		return _StartTime;
	}

	/**
	 *   ���݂̎��Ԃ��擾
	 * @param  ����
	 * @return gfl::core::Time::Type �^�̌��ݎ���
	 */
	static Type GetCurrentMicroSecond( void );

	/**
	 *   ���ԑ�����J�n
	 * @param  ����
	 * @return ����
	 */

	inline void Start( void ) {
		_StartTime = GetCurrentMicroSecond( );
	}

	/**
	 *   ���ԑ�����I��
	 * @param  ����
	 * @return �o�ߎ���
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
	 *   �~���b�̂������X���[�v����
	 * @param  val �~���b�l
	 * @return ����
	 */
	static void SleepMilliSecond( const Type val );

	/**
	 *   �}�C�N���b���~���b�ɕϊ�
	 * @param  val �}�C�N���b
	 * @return �~���b
	 */

	static inline Type MicroSecondToMilliSecond( Type val ) {
		return val / 1000;
	}

	/**
	 *   �~���b���}�C�N���b�ɕϊ�
	 * @param  val �~���b
	 * @return �}�C�N���b
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
