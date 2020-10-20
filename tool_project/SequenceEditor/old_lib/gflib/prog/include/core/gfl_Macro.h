/* 
 * File:   gfl_Macro.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 15:05
 */

#ifndef GFL_MACRO_H
#define	GFL_MACRO_H
#pragma once

// =============================================================================
// �}�N��
// =============================================================================
/**
 *   main �֐����`
 * @param  ����
 * @remark windows �̏ꍇ�� main �֐����ςȂ̂Œ�`�����܂����ƂŐ؂�ւ�����悤�ɂ����B
 * �Q�[���v���b�g�t�H�[���̏ꍇ�� int main() �ɁA
 * ���̑��̏ꍇ�� int main(int argc, char *argv[]) �ɓW�J�����
 */
#ifdef GFL_PLATFORM_WINDOWS // === windows
#include <tchar.h>
#ifdef GFL_PLATFORM_WINDOWS_CONSOLE
#define GFL_MAIN() int _tmain(int argc, _TCHAR* argv[])
#else
#define GFL_MAIN() int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#endif // GFL_PLATFORM_WINDOWS_CONSOLE
#elif defined(GFL_PLATFORM_XENON) // === xenon
#include <tchar.h>
#define GFL_MAIN() void __cdecl main()
#elif defined(GFL_PLATFORM_3DS) // == 3ds
#define GFL_MAIN() void nnMain(void)
#elif defined(GFL_PLATFORM_GAME_CONSOLE) // == game console
#define GFL_MAIN() int main() // avoid warning
#else // === other
#define GFL_MAIN() int main(int argc, char *argv[])
#endif

/**
 *   main �֐��̕Ԃ�l
 * @param  val  �Ԃ�l
 * @return �Ԃ�l
 * @remark �w���������� main �֐��͕Ԃ�l�������Ȃ��̂ł���ȂɂȂ�܂����B
 */
#if defined(GFL_PLATFORM_XENON) || defined(GFL_PLATFORM_3DS)
#define GFL_MAIN_RETURN(val)
#elif defined(_CRTDBG_MAP_ALLOC)
#define GFL_MAIN_RETURN(val) _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF); return (val)
#else
#define GFL_MAIN_RETURN(val) return (val)
#endif

/**
 *   ���[�j���O�����
 * @param  val  �g�p���Ȃ��ϐ�
 * @remark �g�p���Ȃ��ϐ�������Ƃ������[�j���O����������邽��
 */
#define GFL_AVOID_WARNING(val) static_cast<void>(val)


/**
 *   �����[�X���̃��[�j���O�����
 * @param  val  �g�p���Ȃ��ϐ�
 * @remark �f�o�b�O���ɂ͎g�p���Ă��邪�A�����[�X���ɂ͎g�p���Ȃ��ϐ�������Ƃ������[�j���O����������邽��
 */
#ifdef GFL_DEBUG
#define GFL_AVOID_RELEASE_WARNING(val)
#else
#define GFL_AVOID_RELEASE_WARNING GFL_AVOID_WARNING
#endif

/**
 *   ���C���֐��̈����̃��[�j���O�����
 * @param  ����
 */
#ifdef GFL_PLATFORM_GAME_CONSOLE
#define GFL_NO_QT
#define GFL_AVOID_ARG_WARNING()
#elif defined(GFL_PLATFORM_WINDOWS_WINDOWS)
#define GFL_AVOID_ARG_WARNING() \
UNREFERENCED_PARAMETER( hPrevInstance );\
UNREFERENCED_PARAMETER( lpCmdLine )
#else
#define GFL_AVOID_ARG_WARNING() \
	static_cast<void>(argc);	\
	static_cast<void>(argv)
#endif

/**
 *   �R���X�g�A�g���r���[�g�ݒ�
 * @param  ����
 */
#if defined(GFL_COMPILER_GCC) || defined(GFL_COMPILER_CW)
#define GFL_ATTR_CONST	__attribute__((const))
#elif defined(GFL_COMPILER_VC)
#define GFL_ATTR_CONST
#else
#define GFL_ATTR_CONST
#endif

/**
 *   �A���C�������g���w�肵���ϐ��̐錾
 * @param  val �錾�������ϐ�
 * @param  aln �A���C�������g�o�C�g��
 * @remark �g�p��@��u16 GFL_ATTR_ALIGN(u16_val,8);
 * ���̗�ł� u16_val �Ƃ����ϐ����W�o�C�g�A���C�������g�Ő錾
 * �I�[�g�ϐ��ɑ΂��Ă͖���
 */
#if defined(GFL_COMPILER_GCC) || defined(GFL_COMPILER_CW) || defined(GFL_COMPILER_ARMCC)
#define GFL_ALIGN(aln)	__attribute__((aligned(aln)))
#define GFL_ATTR_ALIGN(val,aln)	val __attribute__((aligned(aln)))
#elif defined(GFL_COMPILER_VC)
#define GFL_ALIGN(aln)	__declspec(align(aln))
#define GFL_ATTR_ALIGN(val,aln)	__declspec(align(aln)) val
#else
#define GFL_ALIGN(aln) val
#define GFL_ATTR_ALIGN(val,aln) val
#endif

/**
 *   val �� type �ŃA�N�Z�X����
 * @param  type �^
 * @param  val �ϐ��̃|�C���^�[
 * @remark �Ⴆ�΂P�o�C�g�̃f�[�^�S��S32�ŃA�N�Z�X�������ꍇ�Ɏg���B
 * �G���f�B�A�����Ⴄ�Ƌ������ς��̂Œ��ӂ��K�v�B
 */
#define GFL_ACCESS_TYPE(type,val) (*(reinterpret_cast<type*>(val)))

/**
 *   �r�b�g����
 * @param  val ���Ă����r�b�g
 */
#define GFL_BIT(val) (1<<(val))

/**
 *  �N���X�̃����o�[�ϐ��̃I�t�Z�b�g���v�Z
 * @param  name �N���X��
 * @param  para �����o�[�ϐ���
 * @remark �{���͂O�ł��������ǃ��[�j���O���o��̂łO���P�O�O
 */
#define GFL_OFFSET_OF(name,para)  (reinterpret_cast<size_t>(&((name*)0x100)->para) - 0x100)

/**
 *  �N���X�̃����o�[�ϐ��̃I�t�Z�b�g�̃|�C���^���擾
 * @param  ptr �N���X�|�C���^
 * @param  type �擾�������^
 * @param  ofs �I�t���b�g�l�@���O�� GFL_OFFSET_OF �Ŏ擾���Ă���
 */
#define GFL_GET_POINTER_BY_OFFSET(ptr,type,ofs) ((type*)((size_t)ptr + ofs))

/**
 *  ���[�g����P�ʂƂ��Đݒ� Meter
 * @param x ���[�g��
 * @remark �Q�[�����̒P�ʌn�����[�g���Ȃ̂��Z���`�Ȃ̂����ӎ������Ȃ��悤�Ƀ}�N�������܂���
 */
#if UNIT_METER
#define METER(x) (x)
#else
#define METER(x) ((x)*100.0f)
#endif

/**
 *  �Z���`���[�g����P�ʂƂ��Đݒ� UnitMeter
 * @param x �Z���`���[�g��
 */
#if UNIT_METER
#define CENTI(x) (x*0.01f)
#else
#define CENTI(x) (x)
#endif

#define GFL_SECOND(x) (GFL_FPS*x)

/** �[���C���f���g��������邽�� */
#define GFL_NAMESPACE_BEGIN(ns) namespace ns {
#define GFL_NAMESPACE_END(ns) }

#endif	/* GFL_MACRO_H */

