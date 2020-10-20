/* 
 * File:   gfl_Debug.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 16:13
 */

#ifndef GFL_DEBUG_H
#define	GFL_DEBUG_H
#pragma once

namespace gfl {
namespace core {

/**
 *  �f�o�b�O�N���X
 */

class Debug {
public:
#ifdef GFL_PLATFORM_WINDOWS
#ifdef GFL_UNICODE

	static inline void PrintConsoleDirect( const c8 * const str ) {
		OutputDebugString( (LPCWSTR) str );
		printf( str );
	}
#else // !GFL_UNICODE

	static inline void PrintConsoleDirect( const c8 * const str ) {
		OutputDebugString( str );
		printf( str );
	}
#endif // end of GFL_UNICODE
	//#endif
#elif defined(GFL_PLATFORM_WII)

	static inline void PrintConsoleDirect( const c8 * const str ) {
		OSReport( str );
	}
#elif defined(GFL_PLATFORM_3DS)

	static inline void PrintConsoleDirect( const c8 * const str ) {
		NN_LOG( str );
	}
#else // !GFL_PLATFORM_WINDOWS && !GFL_PLATFORM_WII == GFL_PLATFORM_UNIX

	/**
	 *   ���ڕ�����o��
	 * @param  str ������
	 * @return ����
	 * @remark �ψ����͎��Ȃ�
	 */

	static inline void PrintConsoleDirect( const c8 * const str ) {
		printf( str );
	}
#endif
	/**
	 *   ������o��
	 * @param  str ������
	 * @return ����
	 * @remark �ψ����Ή�
	 */
	static void PrintConsole( const c8 * const str, ... );
};

}
}

// =============================================================================
// �f�o�b�O�}�N��
// =============================================================================

// �W���w�b�_�Q
#include <ctype.h>
#include <stdio.h>

#ifdef GFL_COMPILER_BASE_C99              //====================================
#define GFL_FUNC_NAME __func__
#elif defined(GFL_COMPILER_VC)            //====================================
#define GFL_FUNC_NAME __FUNCTION__
#else                                    //=====================================
/**
 *   �֐������擾
 * @remark �R���p�C���Ԃ̍��ق��z�����Ċ֐������擾�ł���B
 */
#define GFL_FUNC_NAME ""
#endif                                   //=====================================

#ifdef GFL_COMPILER_GCC
#define GFL_FULL_FUNC_NAME __PRETTY_FUNCTION__
#else
/**
 *   �֐������擾�i�N���X�����܂ށj
 * @remark GCC �ȊO�ɂ͖����̂� GFL_FUNC_NAME �Ɠ���
 */
#define GFL_FULL_FUNC_NAME GFL_FUNC_NAME
#endif

#ifdef GFL_DEBUG //============== GFL_DEBUG
#include <cassert>

// �}�N���Ȃ̂Ŏg��Ȃ���� C ����̃\�[�X�ŃC���N���[�h����Ă����v

#if defined(GFL_COMPILER_VC)
#ifndef GFL_PLATFORM_3DS
#define GFL_ASSERT_MSG(cond, ...)																\
	if(!(cond)){																			\
		gfl::core::Debug::PrintConsole("%s(%d): error: %s(): ",__FILE__,__LINE__,GFL_FUNC_NAME);		\
		gfl::core::Debug::PrintConsole(__VA_ARGS__);												\
		DebugBreak(); \
	}
#endif
#define GFL_ASSERT_FILE_LINE(cond, file, line, ...)											\
	if(!(cond)){																			\
		gfl::core::Debug::PrintConsole("%s(%d): error: Called [%s()] in \n%s:%d: ",file,line,GFL_FUNC_NAME,__FILE__,__LINE__);	\
		gfl::core::Debug::PrintConsole(__VA_ARGS__);												\
		DebugBreak();																		\
	}
#define GFL_WARNING(...)																		\
	gfl::core::Debug::PrintConsole("%s(%d) : warning: %s(): ",__FILE__,__LINE__,GFL_FUNC_NAME);		\
	gfl::core::Debug::PrintConsole(__VA_ARGS__);

#define GFL_ASSERT(cond) GFL_ASSERT_MSG(cond,"")

#elif defined(GFL_COMPILER_BASE_C99)
#ifndef GFL_PLATFORM_3DS
#define GFL_ASSERT_MSG(cond, ...)																\
	if(!(cond)){																			\
		gfl::core::Debug::PrintConsole("%s:%d: error: %s(): ",__FILE__,__LINE__,GFL_FUNC_NAME);		\
		gfl::core::Debug::PrintConsole(__VA_ARGS__);												\
		assert(cond); \
	}
#endif
#define GFL_ASSERT_FILE_LINE(cond, file, line, ...)											\
	if(!(cond)){																			\
		gfl::core::Debug::PrintConsole("%s:%d: error: Called [%s()] in \n%s:%d: ",file,line,GFL_FUNC_NAME,__FILE__,__LINE__);	\
		gfl::core::Debug::PrintConsole(__VA_ARGS__);												\
		assert(cond);																		\
 	}

#define GFL_WARNING(...)																		\
	gfl::core::Debug::PrintConsole("%s:%d: warning: %s(): ",__FILE__,__LINE__,GFL_FUNC_NAME);		\
	gfl::core::Debug::PrintConsole(__VA_ARGS__);

#else // VC2005�ȑO�͑ʖڂ��ۂ�
#define GFL_ASSERT_FILE_LINE _GFL_assert
#ifndef GFL_PLATFORM_3DS
#define GFL_ASSERT_MSG _GFL_assert
#endif

inline void _GFL_assert( const bool cond, ... ) {
	assert( cond );
}
#define GFL_WARNING																			\
	gfl::core::Debug::PrintConsole("%s:%d: warning: ",__FILE__,__LINE__);							\
	gfl::core::Debug::PrintConsole
#endif

#ifndef GFL_PLATFORM_3DS
#define GFL_STATIC_ASSERT(expr) extern u8 StaticAssertError[(expr)?1:-1]; static_cast<void>(StaticAssertError)
#endif

#else  //======================= GFL_RELEASE
#if defined(GFL_COMPILER_BASE_C99) || defined(GFL_COMPILER_VC_2005)
#ifndef GFL_PLATFORM_3DS
#define GFL_ASSERT_MSG(...)
#endif
#define GFL_ASSERT_FILE_LINE(...)
#define GFL_WARNING(...)
#elif defined(GFL_COMPILER_VC) // __noop �ň�����p��
#ifndef GFL_PLATFORM_3DS
#define GFL_ASSERT_MSG() __noop
#endif
#define GFL_ASSERT_FILE_LINE() __noop
#define GFL_WARNING() __noop
#else
/**
 *   �ψ��������A�T�[�g
 * @remark GFL_ASSERT(i,"No such data %d %d\n",j,k); �̂悤�Ȏg�������\�B
 * C99�����łȂ��R���p�C���ł͉ψ����͐���ɓ��삵�܂���B
 */
#define GFL_ASSERT

/**
 *   �t�@�C�����A�s�����w�肷��ψ��������A�T�[�g
 * @remark �A�T�[�g���鎞�_�ł͂Ȃ������Ŏ󂯎���ăA�T�[�g����t�@�C���ƍs�����w�肷�邱�Ƃ��o����B
 * �� func(__FILE__,__LINE__);
 *    void func(c8* name, u32 line){
 *         GFL_ASSERT_FILE_LINE(0,name,line,"Error");
 *    }
 */
#define GFL_ASSERT_FILE_LINE

/**
 *   �ψ����������[�j���O���b�Z�[�W�\��
 * @remark GFL_WARNING("No such data %d %d\n",j,k); �̂悤�Ȏg�������\�B
 */
#define GFL_WARNING

#endif

/**
 *   ���s���ł͂Ȃ��R���p�C�����_�ŃG���[�ɂ������Ƃ��B
 * @remark VC �̏ꍇ�h�Y���������̐��ł��h�ƌ����܂��B
 */
#define GFL_STATIC_ASSERT(cond)

#endif //======================= !GFL_RELEASE

// Skip Assert
#if defined(GFL_COMPILER_BASE_C99) || defined(GFL_COMPILER_VC_2005)
#define GFL_SKIP_ASSERT(cond, ...)
#elif defined(GFL_COMPILER_VC) // __noop �ň�����p��
#define GFL_SKIP_ASSERT() __noop
#else
/**
 *   �A�T�[�g��p������
 */
#define GFL_SKIP_ASSERT()
#endif

#endif	/* GFL_DEBUG_H */

