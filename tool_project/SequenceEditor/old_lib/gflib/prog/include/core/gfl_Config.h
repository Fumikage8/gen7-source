/* 
 * File:   gfl_Config.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 11:06
 */

#ifndef GFL_CONFIG_H
#define	GFL_CONFIG_H
#pragma once

// =============================================================================
// プラットフォーム設定
// =============================================================================

#if defined(_XBOX)

// Xenon =======================================================================

#define GFL_PLATFORM_NAME "Xenon"
#define GFL_PLATFORM_XENON
#define GFL_PLATFORM_GAME_CONSOLE
#define GFL_PLATFORM_MICROSOFT
#define GFL_BIG_ENDIAN
#define GFL_REGISTER_64BIT
#define GFL_QUAT_NEEDS_16BYTE_ALIGN
#define GFL_VECTOR_NEEDS_16BYTE_ALIGN
#define GFL_MATRIX_NEEDS_16BYTE_ALIGN

#elif defined(__PPU__) || defined(_PS3)

// Play Station 3 ==============================================================

#define GFL_PLATFORM_NAME "PS3"
#define GFL_PLATFORM_PS3
#define GFL_PLATFORM_GAME_CONSOLE
#define GFL_PLATFORM_UNIX
#define GFL_PLATFORM_SONY
#define GFL_BIG_ENDIAN
//#define GFL_REGISTER_64BIT
#define GFL_QUAT_NEEDS_16BYTE_ALIGN
#define GFL_VECTOR_NEEDS_16BYTE_ALIGN
#define GFL_MATRIX_NEEDS_16BYTE_ALIGN

#elif defined(__PPCGEKKO__)

// Nintendo Wii ================================================================

#define GFL_PLATFORM_NAME "Wii"
#define GFL_PLATFORM_WII
#define GFL_PLATFORM_GAME_CONSOLE
#define GFL_PLATFORM_NINTENDO
#define GFL_BIG_ENDIAN

#elif defined(__PSP__)

// Play Station Portable =======================================================

#define GFL_PLATFORM_NAME "PSP"
#define GFL_PLATFORM_PSP
#define GFL_PLATFORM_GAME_CONSOLE
#define GFL_PLATFORM_UNIX
#define GFL_PLATFORM_SONY
#define GFL_LITTLE_ENDIAN

#define _POSIX_THREADS

#elif defined(__NDS__)

// Nintendo Dual Screen ========================================================

#define GFL_PLATFORM_NAME "NDS"
#define GFL_PLATFORM_NDS
#define GFL_PLATFORM_GAME_CONSOLE
#define GFL_PLATFORM_NINTENDO
#define GFL_LITTLE_ENDIAN

#elif defined(NN_COMPILER_RVCT)

// Nintendo 3DS ========================================================

#define GFL_PLATFORM_NAME "3DS"
#define GFL_PLATFORM_3DS
#define GFL_PLATFORM_GAME_CONSOLE
#define GFL_PLATFORM_NINTENDO
#define GFL_LITTLE_ENDIAN

//#ifdef NN_BUILD_DEVELOPMENT
//#define GFL_DEBUG 1
//#else
//#define GFL_DEBUG 0
//#define GFL_RELEASE
//#endif

#elif defined(__MINGW32__)

// Mingw =======================================================================

#define GFL_PLATFORM_NAME "Mingw"
#define GFL_PLATFORM_MINGW
#define GFL_PLATFORM_PC
#define GFL_PLATFORM_UNIX
#define GFL_LITTLE_ENDIAN

#elif defined(__CYGWIN__)

// Cygwin ======================================================================

#define GFL_PLATFORM_NAME "Cygwin"
#define GFL_PLATFORM_CYGWIN
#define GFL_PLATFORM_PC
#define GFL_LITTLE_ENDIAN
#define GFL_PLATFORM_UNIX
#define GFL_NO_QT

#elif defined(_WIN32) || defined(_WIN64)

// Microsoft Windows ===========================================================

#define GFL_PLATFORM_NAME "Windows"
#define GFL_PLATFORM_WINDOWS
#define GFL_PLATFORM_PC
#define GFL_PLATFORM_MICROSOFT
//#define GFL_GRAPHIC_DIRECTX			// 新規追加DirectXフラグ
#define GFL_GRAPHIC_OPENGL			// 新規追加OpenGLフラグ
#define GFL_INPUT_DIRECTX            // パッドのみ DirectX を使用
#define GFL_LITTLE_ENDIAN

// コンソールモードかウィンドーモードか
#ifdef  _CONSOLE
#define GFL_PLATFORM_WINDOWS_CONSOLE
#elif   defined(_WINDOWS)
#define GFL_PLATFORM_WINDOWS_WINDOWS
#else
#error  "This windows application is not defined _CONSOLE nor _WINDOWS"
#endif

// Pragma
#pragma warning(disable:4996)	// strcat→strcat_s MS の馬鹿！
#pragma warning(disable:4201)	// 無名の構造体

#elif defined(__linux)

// Linux =======================================================================

#define GFL_PLATFORM_NAME "Linux"
#define GFL_PLATFORM_LINUX
#define GFL_PLATFORM_PC
#define GFL_PLATFORM_UNIX
#define GFL_LITTLE_ENDIAN

#else // サポートされていないターゲットなので Doxygen 用のコメントにする

// Unsupported Platform ========================================================

//! @brief プラットフォームの名前の文字列
#define GFL_PLATFORM_NAME "PlatformName"

//! @brief プラットフォームが XBOX360 の場合に定義される
#define GFL_PLATFORM_XENON
//! @brief プラットフォームが PlayStation3 の場合に定義される
#define GFL_PLATFORM_PS3
//! @brief プラットフォームが Wii の場合に定義される
#define GFL_PLATFORM_WII
//! @brief プラットフォームが Windows の場合に定義される
#define GFL_PLATFORM_WINDOWS
//! @brief プラットフォームが Cygwin の場合に定義される
#define GFL_PLATFORM_CYGWIN
//! @brief プラットフォームが MinGW の場合に定義される
#define GFL_PLATFORM_MINGW
//! @brief プラットフォームが Linux の場合に定義される
#define GFL_PLATFORM_LINUX

//! @brief プラットフォームがゲーム実機の場合に定義される xenon ps3 wii
#define GFL_PLATFORM_GAME_CONSOLE
//! @brief プラットフォームが PC の場合に定義される windows cygwin mingw linux
#define GFL_PLATFORM_PC
//! @brief プラットフォームが MicroSoft の場合に定義される windows xenon
#define GFL_PLATFORM_MICROSOFT
//! @brief ユニックスタイプの場合に定義される ps3 cygwin linux mingw
#define GFL_PLATFORM_UNIX

//! @brief リトルエンディアンの場合に定義される
#define GFL_LITTLE_ENDIAN
//! @brief ビッグエンディアンの場合に定義される
#define GFL_BIG_ENDIAN

//! @brief ６４ビットレジスターが存在する
#define GFL_REGISTER_64BIT

#error Please set GFL_PLATFORM_*
#endif

// =============================================================================
// コンパイラ設定
// =============================================================================

#if defined(GFL_PLATFORM_3DS) // arm cc も　__GNUC__ が定義されてるみたいなので一番まえへ

// ArmCompiler =================================================================

#define GFL_COMPILER_NAME "ARMCC"
#define GFL_COMPILER_ARMCC
#define GFL_COMPILER_BASE_C99

#elif defined(__GNUC__)         // GCC

// GCC =========================================================================

//! @brief  コンパイラーの名前を定義
#define GFL_COMPILER_NAME "GCC"
//! @brief  gcc,g++ の場合に定義される
#define GFL_COMPILER_GCC
//! @brief  C99 準拠の場合に定義される（VCでは定義されない）
#define GFL_COMPILER_BASE_C99
//! @brief  GCC のバージョンチェック
#define GFL_COMPILER_GCC_VERSION_GREATER_THAN(major,minor) (( major == __GNUC__ && minor < __GNUC_MINOR__ ) || major < __GNUC__)

#elif defined(_MSC_VER) // MSVC

// MicroSoftVisualC ============================================================

#define GFL_COMPILER_NAME "VC"
#define GFL_COMPILER_VC

#if (1400 <= _MSC_VER)
#define GFL_COMPILER_VC_2005
#endif
#elif defined(__MWERKS__)

// CodeWarrior =================================================================

#define GFL_COMPILER_NAME "CW"
#define GFL_COMPILER_CW
#define GFL_COMPILER_BASE_C99

#else // サポートされていないコンパイラなので Doxygen 用のコメントにする

// Unsupported compiler ========================================================

#define GFL_COMPILER_NAME "GCC or VC or CW"
#define GFL_COMPILER_GCC
//! @brief  VisualC++ の場合に定義される
#define GFL_COMPILER_VC
//! @brief  VisualC++2005 の場合に定義される（この場合 GFL_COMPILER_VC も定義される）
#define GFL_COMPILER_VER_VC_2005
//! @brief  CodeWarrior の場合に定義される
#define GFL_COMPILER_CW

#define GFL_COMPILER_BASE_C99

//! @brief  デバッグの場合に定義される（_DEBUGを見て自動的に定義する）
#define GFL_DEBUG
//! @brief  リリースの場合に定義される（NDEBUGを見て自動的に定義する）
#define GFL_RELEASE

#error Unsupported compiler ...

#endif

// -----------------------------------------------------------------------------
// コンパイラオプション設定
// -----------------------------------------------------------------------------

#ifdef GFL_COMPILER_GCC
//! @brief BSS 領域に確保
#define ATTR_BSS __attribute__ ((section ("bss")))
#else // !GFL_COMPILER_GCC
#define ATTR_BSS
#endif // end of GFL_COMPILER_GCC

#ifdef UNICODE
#define GFL_UNICODE
#endif

#if defined(GFL_COMPILER_ARMCC)

#define GFL_TEMPLATE_PARAMETER this->

#elif defined(GFL_COMPILER_GCC)

#if GFL_COMPILER_GCC_VERSION_GREATER_THAN(3,3)
#define GFL_TEMPLATE_PARAMETER this->template
#else // !GFL_COMPILER_GCC_VERSION_GREATER_THAN(3,3)
#define GFL_TEMPLATE_PARAMETER
#endif // end of GFL_COMPILER_GCC_VERSION_GREATER_THAN(3,3)

#else // !GFL_COMPLIER_GCC

#define GFL_TEMPLATE_PARAMETER

#endif // end of GFL_COMPLIER_GCC

// =============================================================================
// デバッグリリース設定
// =============================================================================

#ifdef _DEBUG
#undef  GFL_DEBUG
#define GFL_DEBUG 1
#undef  GFL_RELEASE
#elif defined(NDEBUG)
#define  GFL_DEBUG 0
#undef  GFL_RELEASE
#define GFL_RELEASE
#elif !(defined(GFL_DEBUG) || defined(GFL_RELEASE)) // 定義されていなかったらデバッグに変更
#undef  GFL_DEBUG
#define GFL_DEBUG 1
#undef  GFL_RELEASE
#endif

#if GFL_DEBUG
#undef  _DEBUG
#define _DEBUG
#else
#undef  NDEBUG
#define NDEBUG
#endif

// メートルなら１　センチなら０
#define UNIT_METER 0

#define GFL_FPS 30 // デフォルトのFPS

#define GFL_DEFAULT_THREAD_STACK_SIZE 0x1000

#endif	/* GFL_CONFIG_H */

