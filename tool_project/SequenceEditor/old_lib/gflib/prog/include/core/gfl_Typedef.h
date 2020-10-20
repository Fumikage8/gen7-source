/* 
 * File:   gfl_Typedef.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 15:35
 */

#ifndef GFL_TYPEDEF_H
#define	GFL_TYPEDEF_H
#pragma once

// =============================================================================
// �����I�Ƀr�b�g����\�������^�錾�B
// =============================================================================

// �C�V���̏ꍇ�͊��ɒ�`����Ă���̂ŁA���̃^�C�v�f�t���`���Ȃ�
#ifndef GFL_PLATFORM_NINTENDO
// unix �̏ꍇ�͎����悤�Ȓ�`������̂ł���𗘗p����
#ifdef GFL_PLATFORM_UNIX
#include <stdint.h>
typedef uint8_t u8; /**< �������W�r�b�g�ϐ� */
typedef uint16_t u16; /**< ������16�r�b�g�ϐ� */
typedef uint32_t u32; /**< ������32�r�b�g�ϐ� */
typedef uint64_t u64; /**< ������64�r�b�g�ϐ� */

typedef int8_t s8; /**< �����t�W�r�b�g�ϐ� */
typedef int16_t s16; /**< �����t16�r�b�g�ϐ� */
typedef int32_t s32; /**< �����t32�r�b�g�ϐ� */
typedef int64_t s64; /**< �����t64�r�b�g�ϐ� */
#else // !GFL_PLATFORM_UNIX
typedef unsigned char u8; /**< �������W�r�b�g�ϐ� */
typedef unsigned short u16; /**< ������16�r�b�g�ϐ� */
typedef unsigned int u32; /**< ������32�r�b�g�ϐ� */
typedef unsigned long long u64; /**< ������64�r�b�g�ϐ� */

typedef signed char s8; /**< �����t�W�r�b�g�ϐ� */
typedef signed short s16; /**< �����t16�r�b�g�ϐ� */
typedef signed int s32; /**< �����t32�r�b�g�ϐ� */
typedef signed long long s64; /**< �����t64�r�b�g�ϐ� */
#endif // GFL_PLATFORM_UNIX

typedef float f32; /**< �Z���x���������_�ϐ� */
typedef double f64; /**< �{���x���������_�ϐ� */
typedef signed int b32; /**<	32�r�b�g�̃u�[�� */
#endif // !GFL_PLATFORM_NINTENDO

// �I���W�i���A�T�C��
typedef unsigned short f16; /**< 16�r�b�g���������_�ϐ� */
typedef char c8; /**<	8�r�b�g�̕����ϐ� */

#endif	/* GFL_TYPEDEF_H */
