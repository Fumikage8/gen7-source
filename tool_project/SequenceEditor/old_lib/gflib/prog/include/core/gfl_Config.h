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
// �v���b�g�t�H�[���ݒ�
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
//#define GFL_GRAPHIC_DIRECTX			// �V�K�ǉ�DirectX�t���O
#define GFL_GRAPHIC_OPENGL			// �V�K�ǉ�OpenGL�t���O
#define GFL_INPUT_DIRECTX            // �p�b�h�̂� DirectX ���g�p
#define GFL_LITTLE_ENDIAN

// �R���\�[�����[�h���E�B���h�[���[�h��
#ifdef  _CONSOLE
#define GFL_PLATFORM_WINDOWS_CONSOLE
#elif   defined(_WINDOWS)
#define GFL_PLATFORM_WINDOWS_WINDOWS
#else
#error  "This windows application is not defined _CONSOLE nor _WINDOWS"
#endif

// Pragma
#pragma warning(disable:4996)	// strcat��strcat_s MS �̔n���I
#pragma warning(disable:4201)	// �����̍\����

#elif defined(__linux)

// Linux =======================================================================

#define GFL_PLATFORM_NAME "Linux"
#define GFL_PLATFORM_LINUX
#define GFL_PLATFORM_PC
#define GFL_PLATFORM_UNIX
#define GFL_LITTLE_ENDIAN

#else // �T�|�[�g����Ă��Ȃ��^�[�Q�b�g�Ȃ̂� Doxygen �p�̃R�����g�ɂ���

// Unsupported Platform ========================================================

//! @brief �v���b�g�t�H�[���̖��O�̕�����
#define GFL_PLATFORM_NAME "PlatformName"

//! @brief �v���b�g�t�H�[���� XBOX360 �̏ꍇ�ɒ�`�����
#define GFL_PLATFORM_XENON
//! @brief �v���b�g�t�H�[���� PlayStation3 �̏ꍇ�ɒ�`�����
#define GFL_PLATFORM_PS3
//! @brief �v���b�g�t�H�[���� Wii �̏ꍇ�ɒ�`�����
#define GFL_PLATFORM_WII
//! @brief �v���b�g�t�H�[���� Windows �̏ꍇ�ɒ�`�����
#define GFL_PLATFORM_WINDOWS
//! @brief �v���b�g�t�H�[���� Cygwin �̏ꍇ�ɒ�`�����
#define GFL_PLATFORM_CYGWIN
//! @brief �v���b�g�t�H�[���� MinGW �̏ꍇ�ɒ�`�����
#define GFL_PLATFORM_MINGW
//! @brief �v���b�g�t�H�[���� Linux �̏ꍇ�ɒ�`�����
#define GFL_PLATFORM_LINUX

//! @brief �v���b�g�t�H�[�����Q�[�����@�̏ꍇ�ɒ�`����� xenon ps3 wii
#define GFL_PLATFORM_GAME_CONSOLE
//! @brief �v���b�g�t�H�[���� PC �̏ꍇ�ɒ�`����� windows cygwin mingw linux
#define GFL_PLATFORM_PC
//! @brief �v���b�g�t�H�[���� MicroSoft �̏ꍇ�ɒ�`����� windows xenon
#define GFL_PLATFORM_MICROSOFT
//! @brief ���j�b�N�X�^�C�v�̏ꍇ�ɒ�`����� ps3 cygwin linux mingw
#define GFL_PLATFORM_UNIX

//! @brief ���g���G���f�B�A���̏ꍇ�ɒ�`�����
#define GFL_LITTLE_ENDIAN
//! @brief �r�b�O�G���f�B�A���̏ꍇ�ɒ�`�����
#define GFL_BIG_ENDIAN

//! @brief �U�S�r�b�g���W�X�^�[�����݂���
#define GFL_REGISTER_64BIT

#error Please set GFL_PLATFORM_*
#endif

// =============================================================================
// �R���p�C���ݒ�
// =============================================================================

#if defined(GFL_PLATFORM_3DS) // arm cc ���@__GNUC__ ����`����Ă�݂����Ȃ̂ň�Ԃ܂���

// ArmCompiler =================================================================

#define GFL_COMPILER_NAME "ARMCC"
#define GFL_COMPILER_ARMCC
#define GFL_COMPILER_BASE_C99

#elif defined(__GNUC__)         // GCC

// GCC =========================================================================

//! @brief  �R���p�C���[�̖��O���`
#define GFL_COMPILER_NAME "GCC"
//! @brief  gcc,g++ �̏ꍇ�ɒ�`�����
#define GFL_COMPILER_GCC
//! @brief  C99 �����̏ꍇ�ɒ�`�����iVC�ł͒�`����Ȃ��j
#define GFL_COMPILER_BASE_C99
//! @brief  GCC �̃o�[�W�����`�F�b�N
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

#else // �T�|�[�g����Ă��Ȃ��R���p�C���Ȃ̂� Doxygen �p�̃R�����g�ɂ���

// Unsupported compiler ========================================================

#define GFL_COMPILER_NAME "GCC or VC or CW"
#define GFL_COMPILER_GCC
//! @brief  VisualC++ �̏ꍇ�ɒ�`�����
#define GFL_COMPILER_VC
//! @brief  VisualC++2005 �̏ꍇ�ɒ�`�����i���̏ꍇ GFL_COMPILER_VC ����`�����j
#define GFL_COMPILER_VER_VC_2005
//! @brief  CodeWarrior �̏ꍇ�ɒ�`�����
#define GFL_COMPILER_CW

#define GFL_COMPILER_BASE_C99

//! @brief  �f�o�b�O�̏ꍇ�ɒ�`�����i_DEBUG�����Ď����I�ɒ�`����j
#define GFL_DEBUG
//! @brief  �����[�X�̏ꍇ�ɒ�`�����iNDEBUG�����Ď����I�ɒ�`����j
#define GFL_RELEASE

#error Unsupported compiler ...

#endif

// -----------------------------------------------------------------------------
// �R���p�C���I�v�V�����ݒ�
// -----------------------------------------------------------------------------

#ifdef GFL_COMPILER_GCC
//! @brief BSS �̈�Ɋm��
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
// �f�o�b�O�����[�X�ݒ�
// =============================================================================

#ifdef _DEBUG
#undef  GFL_DEBUG
#define GFL_DEBUG 1
#undef  GFL_RELEASE
#elif defined(NDEBUG)
#define  GFL_DEBUG 0
#undef  GFL_RELEASE
#define GFL_RELEASE
#elif !(defined(GFL_DEBUG) || defined(GFL_RELEASE)) // ��`����Ă��Ȃ�������f�o�b�O�ɕύX
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

// ���[�g���Ȃ�P�@�Z���`�Ȃ�O
#define UNIT_METER 0

#define GFL_FPS 30 // �f�t�H���g��FPS

#define GFL_DEFAULT_THREAD_STACK_SIZE 0x1000

#endif	/* GFL_CONFIG_H */

