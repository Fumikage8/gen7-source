#ifndef __GFL_MACROS_H__
#define __GFL_MACROS_H__
//===========================================================================
/**
 * @file    gfl_Macros.h
 * @brief   GAMEFREAK Library / Utility Macros
 * @author  GAMEFREAK inc.
 * @date    2010.10.29
 */
//===========================================================================
#pragma once

//-----------------------------------------------------------------------------
/**
 *          �}�N����`
*/
//-----------------------------------------------------------------------------

/// �g�p���Ȃ��������ɑ΂��Ďg���}�N���i#pragma unused�̑���j
#define GFL_UNUSED(a) static_cast<void>(a)

///�R�s�[�R���X�g���N�^�{������Z�q���֎~����
#define GFL_FORBID_COPY_AND_ASSIGN(TypeName)  \
  private: \
    TypeName( const TypeName& ); \
    void operator= (const TypeName&)

/// �z��̗v�f��
#define GFL_NELEMS(ary) (sizeof(ary)/sizeof(ary[0]))
#define elementof(ary) (sizeof(ary)/sizeof(ary[0]))

/// enum�̃T�C�Y�� DWORD �i�S�o�C�g�j�ɓ��ꂷ��
#define  ENUM_FORCE_DWORD(name) name##_FORCE_DWORD = 0x7FFFFFFF

/// bool�^�̓R���p�C���ɂ���đ傫�����ς��� true false�����i�Ɉ������߂ɒ�`
typedef unsigned char b8;    ///< ����������ꍇ�͂�������g�p���Ă�������
typedef signed long b32;     ///< NITROARM��bool�Ɠ����ł��̂� �󂯂�ꍇ�ɂ͂�������g���Ă�������


/// �X�^�e�B�b�N�A�T�[�g
#define STATIC_ASSERT(expr) { extern char dummy[(expr) ? 1 : -1]; }




#endif // __GFL_MACROS_H__
