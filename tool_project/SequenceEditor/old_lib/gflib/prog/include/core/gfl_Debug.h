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
 *  デバッグクラス
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
	 *   直接文字列出力
	 * @param  str 文字列
	 * @return 無し
	 * @remark 可変引数は取れない
	 */

	static inline void PrintConsoleDirect( const c8 * const str ) {
		printf( str );
	}
#endif
	/**
	 *   文字列出力
	 * @param  str 文字列
	 * @return 無し
	 * @remark 可変引数対応
	 */
	static void PrintConsole( const c8 * const str, ... );
};

}
}

// =============================================================================
// デバッグマクロ
// =============================================================================

// 標準ヘッダ群
#include <ctype.h>
#include <stdio.h>

#ifdef GFL_COMPILER_BASE_C99              //====================================
#define GFL_FUNC_NAME __func__
#elif defined(GFL_COMPILER_VC)            //====================================
#define GFL_FUNC_NAME __FUNCTION__
#else                                    //=====================================
/**
 *   関数名を取得
 * @remark コンパイラ間の差異を吸収して関数名を取得できる。
 */
#define GFL_FUNC_NAME ""
#endif                                   //=====================================

#ifdef GFL_COMPILER_GCC
#define GFL_FULL_FUNC_NAME __PRETTY_FUNCTION__
#else
/**
 *   関数名を取得（クラス名も含む）
 * @remark GCC 以外には無いので GFL_FUNC_NAME と同じ
 */
#define GFL_FULL_FUNC_NAME GFL_FUNC_NAME
#endif

#ifdef GFL_DEBUG //============== GFL_DEBUG
#include <cassert>

// マクロなので使わなければ C 言語のソースでインクルードされても大丈夫

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

#else // VC2005以前は駄目っぽい
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
#elif defined(GFL_COMPILER_VC) // __noop で引数を廃棄
#ifndef GFL_PLATFORM_3DS
#define GFL_ASSERT_MSG() __noop
#endif
#define GFL_ASSERT_FILE_LINE() __noop
#define GFL_WARNING() __noop
#else
/**
 *   可変引数を持つアサート
 * @remark GFL_ASSERT(i,"No such data %d %d\n",j,k); のような使い方が可能。
 * C99準拠でないコンパイラでは可変引数は正常に動作しません。
 */
#define GFL_ASSERT

/**
 *   ファイル名、行数を指定する可変引数を持つアサート
 * @remark アサートする時点ではなく引数で受け取ってアサートするファイルと行数を指定することが出来る。
 * 例 func(__FILE__,__LINE__);
 *    void func(c8* name, u32 line){
 *         GFL_ASSERT_FILE_LINE(0,name,line,"Error");
 *    }
 */
#define GFL_ASSERT_FILE_LINE

/**
 *   可変引数を持つワーニングメッセージ表示
 * @remark GFL_WARNING("No such data %d %d\n",j,k); のような使い方が可能。
 */
#define GFL_WARNING

#endif

/**
 *   実行時ではなくコンパイル時点でエラーにしたいとき。
 * @remark VC の場合”添え字が負の数です”と言われます。
 */
#define GFL_STATIC_ASSERT(cond)

#endif //======================= !GFL_RELEASE

// Skip Assert
#if defined(GFL_COMPILER_BASE_C99) || defined(GFL_COMPILER_VC_2005)
#define GFL_SKIP_ASSERT(cond, ...)
#elif defined(GFL_COMPILER_VC) // __noop で引数を廃棄
#define GFL_SKIP_ASSERT() __noop
#else
/**
 *   アサートを廃棄する
 */
#define GFL_SKIP_ASSERT()
#endif

#endif	/* GFL_DEBUG_H */

