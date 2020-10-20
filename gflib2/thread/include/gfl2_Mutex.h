#ifndef __GFL2_MUTEX_H__
#define __GFL2_MUTEX_H__
//=============================================================================
/**
 * @file	 gfl2_mutex.h
 * @brief	 ミューテックス
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


/*
    セマフォ 	ある処理があって、同時にその処理ができるスレッド数が限られている場合に使用。プロセスをまたいで排他制御可能
★    ミューテックス 	ある処理があって、同時にその処理ができるスレッドが一つだけに限られている場合に使用。プロセスをまたいで排他制御可能
    クリティカルセクション 	ある処理があって、同時にその処理ができるスレッドが一つだけに限られている場合に使用。プロセスをまたいで排他制御不可能
*/


#if defined(GF_PLATFORM_WIN32)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mutex>

#endif

#if defined(GF_PLATFORM_CTR)
#include <nn/os.h>
#endif


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)

class Mutex
#if defined(GF_PLATFORM_WIN32)
: public ::std::mutex
#elif defined(GF_PLATFORM_CTR)
: public nn::os::CriticalSection
#endif
{
public:
#if defined(GF_PLATFORM_CTR)

	Mutex( ) {
		Initialize( );
	}

	inline void lock( ) {
		Enter( );
	}

	inline void tryLock( ) {
		TryEnter( );
	}

	inline void unlock( ) {
		Leave( );
	}
#elif defined(GF_PLATFORM_WIN32)

	inline void lock( ) {
    ::std::mutex::lock( );
	}

	inline void tryLock( ) {
		::std::mutex::try_lock( );
	}

	inline void unlock( ) {
		::std::mutex::unlock( );
	}
#endif
};

///  以下はロックガードテンプレートです
///  このように書いて使用してください -->  gfl2::thread::lock_guard<gfl2::thread::Mutex> lock(*mpMtx);
#if defined(GF_PLATFORM_WIN32)

// LOCK PROPERTIES
struct adopt_lock_t
	{	// indicates adopt lock
	};

struct defer_lock_t
	{	// indicates defer lock
	};

struct try_to_lock_t
	{	// indicates try to lock
	};

extern _CRTIMP2_PURE const adopt_lock_t adopt_lock;
extern _CRTIMP2_PURE const defer_lock_t defer_lock;
extern _CRTIMP2_PURE const try_to_lock_t try_to_lock;

	// LOCKS
template<class _Mutex>
	class lock_guard
	{	// class with destructor that unlocks mutex
public:
	typedef _Mutex mutex_type;

	explicit lock_guard(_Mutex& _Mtx)
		: _MyMutex(_Mtx)
		{	// construct and lock
		_MyMutex.lock();
		}

	lock_guard(_Mutex& _Mtx, adopt_lock_t)
		: _MyMutex(_Mtx)
		{	// construct but don't lock
		}

	~lock_guard() _NOEXCEPT
		{	// unlock
		_MyMutex.unlock();
		}

private:
	lock_guard(const lock_guard&);  // not defined
	lock_guard& operator=(const lock_guard&);	// not defined

private:
	_Mutex& _MyMutex;
	};


#endif





GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif// __GFL2_MUTEX_H__
