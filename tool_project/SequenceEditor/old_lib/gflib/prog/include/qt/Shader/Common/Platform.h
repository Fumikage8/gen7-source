#pragma once
#ifndef GF_PLATFORM_H_INCLUDED 
#define GF_PLATFORM_H_INCLUDED

// デバッグメモリ情報設定
#include <stdio.h>
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define New  ::new(_NORMAL_BLOCK, __FILE__, __LINE__) // rapidxml でエラーが出るので使えない…

#else
	#define New  new
#endif


#if defined(NN_COMPILER_RVCT)

#define			PLATFORM_3DS

#include <assert.h>
#include <stdio.h>
#include <nn/os.h>
#include <nn/dbg.h>
#include <nn/gx.h>
#include <nn/ulcd.h>
#include <nn/math.h>
#include <nn/fnd.h>
#include <nn/fs.h>
#include <nn/os/os_Memory.h>
#include <string.h>

#elif defined(_WIN32) || defined(_WIN64)

#define			PLATFORM_WIN

//------------------------------------------------------------------------------
//  Windows
//------------------------------------------------------------------------------

#include <assert.h>
#include <stdio.h>

#include <WinSock2.h>
#include <windows.h>

#define GLEW_STATIC

#include <GL/glew.h> // glew

#pragma comment(lib, "glew32s.lib")
#if defined( Bits64_ )
	//#pragma comment(lib, "user64.lib")
	//#pragma comment(lib, "gdi64.lib")
#else
	#pragma comment(lib, "user32.lib")
	#pragma comment(lib, "gdi32.lib")
#endif
// GL utility
#include <GL/glut.h>


#endif

// aligned 修飾子
#if defined( _MSC_VER )
	#define GF_ALIGNED( SIZE ) __declspec(align(SIZE))
#else
	#define GF_ALIGNED( SIZE ) __attribute__((aligned(SIZE)))
#endif

#define DefaultAseertPrintFunction //

#define GF_ASSERT_PRINT(CONDITION,MESSAGE)												\
{																						\
	const bool condition = !(CONDITION);												\
	switch((int)(condition)){															\
	case 0: break;																		\
	default: DefaultAseertPrintFunction(MESSAGE, __FILE__,__LINE__,__FUNCTION__); break;\
	}																					\
}

#define GL_ERR()															\
{																			\
	switch( glGetError() )													\
	{																		\
	case GL_NO_ERROR:														\
		break;																\
	case GL_INVALID_ENUM:													\
		GF_ASSERT_PRINT( FALSE, "GL_INVALID_ENUM" );						\
		break;																\
	case GL_INVALID_VALUE:													\
		GF_ASSERT_PRINT( FALSE, "GL_INVALID_VALUE" );						\
		break;																\
	case GL_INVALID_OPERATION:												\
		GF_ASSERT_PRINT( FALSE, "GL_INVALID_OPERATION" );					\
		break;																\
	case GL_STACK_OVERFLOW:													\
		GF_ASSERT_PRINT( FALSE, "GL_STACK_OVERFLOW" );						\
		break;																\
	case GL_STACK_UNDERFLOW:												\
		GF_ASSERT_PRINT( FALSE, "GL_STACK_UNDERFLOW" );						\
		break;																\
	case GL_OUT_OF_MEMORY:													\
		GF_ASSERT_PRINT( FALSE, "GL_OUT_OF_MEMORY" );						\
		break;																\
	case GL_TABLE_TOO_LARGE:												\
		GF_ASSERT_PRINT( FALSE, "GL_TABLE_TOO_LARGE" );						\
		break;																\
	default:																\
		GF_ASSERT_PRINT( FALSE, (char*)::gluErrorString( glGetError() ) );	\
		break;																\
	}																		\
}																			\

#endif


