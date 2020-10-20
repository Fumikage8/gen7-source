#ifndef __GFL_FRAGMENTOPERATION_H__
#define __GFL_FRAGMENTOPERATION_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FragmentOperation.h
 *	@brief  �t���O�����g�I�y���[�V�����@�Ǘ�
 *	@author	tomoya takahashi
 *	@date		2010.10.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <GLES2/gl2.h>

#include <gfl_Math.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>

#include <grp/gfl_RenderTarget.h>

namespace gfl {
namespace grp {


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
enum FragmentOperationEnum{
  // �J�����O���[�h
  CULL_NONE = 0,
  CULL_FRONT,
  CULL_BACK,
  CULL_MAX,  // �V�X�e�����g�p

  // �X�e���V���e�X�g�E�f�v�X�e�X�g::��r�֐�
  TEST_FUNC_NEVER    = PICA_DATA_DEPTH_TEST_NEVER,   // �˂Ƀt�F�C��
  TEST_FUNC_ALWAYS   = PICA_DATA_DEPTH_TEST_ALWAYS,  // �˂Ƀp�X
  TEST_FUNC_LESS     = PICA_DATA_DEPTH_TEST_LESS,    // (ret & mask) < (stencil & mask)�̂Ƃ��p�X
  TEST_FUNC_LEQUAL   = PICA_DATA_DEPTH_TEST_LEQUAL,  // (ret & mask) <= (stencil & mask)�̂Ƃ��p�X
  TEST_FUNC_EQUAL    = PICA_DATA_DEPTH_TEST_EQUAL,   // (ret & mask) == (stencil & mask)�̂Ƃ��p�X
  TEST_FUNC_GREATER  = PICA_DATA_DEPTH_TEST_GREATER, // (ret & mask) > (stencil & mask)�̂Ƃ��p�X
  TEST_FUNC_GEQUAL   = PICA_DATA_DEPTH_TEST_GEQUAL,  // (ret & mask) >= (stencil & mask)�̂Ƃ��p�X
  TEST_FUNC_NOTEQUAL = PICA_DATA_DEPTH_TEST_NOTEQUAL,// (ret & mask) != (stencil & mask)�̂Ƃ��p�X

  // �X�e���V���e�X�g::����ݒ�
  STNCL_TEST_OP_KEEP       = PICA_DATA_STENCIL_OP_KEEP,    // �X�e���V���l�͌��݂̒l��ێ�
  STNCL_TEST_OP_ZERO       = PICA_DATA_STENCIL_OP_ZERO,    // �X�e���V���l�͂O�ɒu�������
  STNCL_TEST_OP_REPLACE    = PICA_DATA_STENCIL_OP_REPLACE, // �X�e���V���l�͎Q�ƒl�ɒu�������
  STNCL_TEST_OP_INCR       = PICA_DATA_STENCIL_OP_INCR,    // �X�e���V���l��1���Z�����@255�𒴂��鎖�͂Ȃ�
  STNCL_TEST_OP_DECR       = PICA_DATA_STENCIL_OP_DECR,    // �X�e���V���l��1���Z�����  0�����ɂȂ邱�Ƃ͂Ȃ�
  STNCL_TEST_OP_INVERT     = PICA_DATA_STENCIL_OP_INVERT,  // �X�e���V���l�̓r�b�g���]����
  STNCL_TEST_OP_INCR_WRAP  = PICA_DATA_STENCIL_OP_INCR_WRAP,// �X�e���V���l��1���Z����� 255�̎���0�ɂȂ�
  STNCL_TEST_OP_DECR_WRAP  = PICA_DATA_STENCIL_OP_DECR_WRAP,// �X�e���V���l��1���Z����� 0�̎���255�ɂȂ�


  ENUM_FORCE_DWORD(FragmentOperationEnum)
};

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�U�[�e�X�g�@��`���
//=====================================
typedef struct {
  u16 x;
  u16 y;
  u16 width;
  u16 height;
} ScissorRect;

//-------------------------------------
///	�X�e���V���e�X�g�@�p�����[�^
//=====================================
typedef struct {
  u32 func_type;        // ��r�^�C�v (TEST_FUNC_�`)
  s32 func_ref;         // ��r�Q�ƒl
  u32 func_msk;         // ��r�}�X�N�l
  u32 write_msk;        // �X�e���V���o�b�t�@�ւ̏������ݗL���E�����}�X�N
  
  u32 op_fail;          // �X�e���V���e�X�g�̌��ʃt�F�C�������ꍇ�̑���(STNCL_TEST_OP_�`)
  u32 op_zfail;         // �X�e���V���e�X�g���p�X�A�f�v�X�e�X�g���t�F�C�������ꍇ�̑���(STNCL_TEST_OP_�`)
  u32 op_zpass;         // �X�e���V���e�X�g����уf�v�X�e�X�g���p�X�����ꍇ�̑���(STNCL_TEST_OP_�`)
} StnclTestParam;


//-------------------------------------
///	�t���O�����g�I�y���[�V����
//=====================================
typedef struct {
  u16  cullmode;                        // �J�����O���[�h�iCULL�`�j
  u16  scissor;                         // �V�U�[�e�X�g�itrue/false�j
  ScissorRect scissor_rect;
  u16  arly_depth_test;                 // �A�[���[�f�v�X�e�X�g(true/false)
  u16  stencil_test;                    // �X�e���V���e�X�g(true/false)
  StnclTestParam stencil_para;  // 
  f32  wscale;                          // W�o�b�t�@�ݒ�i0.0f�Ŗ����j
  f32  depth_range_near;                // �f�v�X�����W Near
  f32  depth_range_far;                 // �f�v�X�����W Far
  f32  polygon_offset;                  // �f�v�X�l �����ŏ��l�ɑ΂���X�P�[���l
  u16  depth_test;                      // �f�v�X�e�X�g(true/false)
  u16  depth_func;                      // �f�v�X�e�X�g��r�֐�(TEST_FUNC_�`)
  u8  depth_write;                      // �f�v�X�o�b�t�@�ւ̏������݂��\��(true/false)
  u8  colormsk_red;                     // �J���[�o�b�t�@�֐Ԃ��������ނ��itrue/false�j
  u8  colormsk_green;                   // �J���[�o�b�t�@�֐Ԃ��������ނ��itrue/false�j
  u8  colormsk_blue;                    // �J���[�o�b�t�@�֐Ԃ��������ނ��itrue/false�j
  u8  colormsk_alpha;                   // �J���[�o�b�t�@�֐Ԃ��������ނ��itrue/false�j
  u8  frame_color_read;                 // �J���[�o�b�t�@�̓ǂݍ��݂��\���itrue/false�j
  u8  frame_color_write;                // �J���[�o�b�t�@�̏������݂��\���itrue/false�j
  u8  frame_depth_read;                 // �f�v�X�o�b�t�@�̓ǂݍ��݂��\���itrue/false�j
  u8  frame_depth_write;                // �f�v�X�o�b�t�@�̏������݂��\���itrue/false�j
  u8  frame_stencil_read;               // �X�e���V���o�b�t�@�̓ǂݍ��݂��\���itrue/false�j
  u8  frame_stencil_write;              // �X�e���V���o�b�t�@�̏������݂��\���itrue/false�j

  
  u8  pad;
} FragmentOperationParam;

  
//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
class FragmentOperation
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(FragmentOperation);

public:
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���f�t�H���g�ݒ�
   */
  //-----------------------------------------------------------------------------
  static const FragmentOperationParam DefaultParam;
  
public:
  //-----------------------------------------------------------------------------
  /**
   *      �R���X�g���N�^�E�f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  FragmentOperation(void);
  ~FragmentOperation();

  //----------------------------------------------------------------------------
  /**
   *	@brief  �p�����[�^�Z�b�g�A�b�v
   *
   *	@param	param
   */
  //-----------------------------------------------------------------------------
  void SetParam( const FragmentOperationParam& param );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���݂̃p�����[�^���擾
   *
   *	@return ���݂̐ݒ�
   */
  //-----------------------------------------------------------------------------
  const FragmentOperationParam& GetParam(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  3D�R�}���h���J�����g�̃R�}���h���X�g�ɐݒ�
   *
   *	@param	�t���[���o�b�t�@�N���X
   */
  //-----------------------------------------------------------------------------
  void Add3DCommand( const RenderTarget& renderTarget ) const;
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  3D�R�}���h���J�����g�̃R�}���h���X�g�ɐݒ�
   *
   *  @param  vp_x            �r���[�|�[�gX
   *  @param  vp_y            �r���[�|�[�gY
   *	@param	vp_width        �J���[�o�b�t�@�@����
   *	@param	vp_height       �J���[�o�b�t�@�@�c��
   *	@param  depth_format    �f�v�X�o�b�t�@�@�t�H�[�}�b�g
   */
  //-----------------------------------------------------------------------------
  void Add3DCommand( u32 vp_x, u32 vp_y, u32 vp_width, u32 vp_height, RenderDepthFormat depth_format ) const;

private:

  FragmentOperationParam m_setup;
};




} /* namespace end grp */
} /* namespace end gfl */
#endif // __GFL_FRAGMENTOPERATION_H__
