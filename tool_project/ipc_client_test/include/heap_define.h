//����̓}�N���W�J�p�Ȃ̂ŃC���N���[�h�K�[�h�s�v�ł��B
//#if !defined( __HEAP_DEFINE_H__ )
//#define __HEAP_DEFINE_H__
//======================================================================
/**
 * @file  heap_define.h
 * @brief HEAPID��`�̌��ƂȂ��`
 * @author  ariizumi
 * @data  10/10/20
 */
//======================================================================

//HEAP_DEFINE�ƌ����}�N����ǂݍ��ݐ�ŗp�ӂ�enum�W�J���܂��B

//�풓�q�[�v
HEAP_DEFINE( HEAPID_DEVICE )
HEAP_DEFINE( HEAPID_DEBUG )   //�f�o�b�O�N�����̂݊m��
HEAP_DEFINE( HEAPID_DEBUG_DEVICE )   //�f�o�b�O�N�����̂݊m��

//�}�N���𖳌�������
#undef  HEAP_DEFINE

//����̓}�N���W�J�p�Ȃ̂ŃC���N���[�h�K�[�h�s�v�ł��B
//#endif // __HEAP_DEFINE_H__
