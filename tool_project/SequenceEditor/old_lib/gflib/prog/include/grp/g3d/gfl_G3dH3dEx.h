#ifndef __GFL_G3DH3DEX_H__
#define __GFL_G3DH3DEX_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dEx.h
 *	@brief  H3D�ǉ�
 *	@author	Koji Kawada
 *	@date		2012.08.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
//����
//������H3D�ɒǉ����ė~�����悤�ȃN���X�������܂��B
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>

#include <grp/util/gfl_UtilDrawUtil.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D�ǉ��萔�N���X
//=====================================

// TextureSampler
struct H3dTextureSamplerExConst
{
  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  // �e�N�X�`���}�b�p�[  �T���v���[�^�C�v
  enum SamplerType
  {
    SAMPLER_TYPE_TEXTURE_2D  = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D             ,  // �e�N�X�`�����W�ł��B 
    SAMPLER_TYPE_CUBE_MAP    = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_CUBE_MAP       ,  // �J�����L���[�u���W�ł��B 
    SAMPLER_TYPE_SHADOW      = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_SHADOW_2D_DMP  ,  // �V���h�E�p�̓��e�ł��B 
    SAMPLER_TYPE_PROJECTION  = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_PROJECTION_DMP ,  // ���e���܂��B   
    SAMPLER_TYPE_SHADOW_CUBE = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_SHADOW_CUBE_DMP,  // �V���h�E�L���[�u�p�̓��e�ł��B
    SAMPLER_TYPE_FALSE       = PICA_DATA_TEXTURE0_SAMPLER_TYPE_FALSE                  ,  // FALSE
  };
  enum SamplerTypeEnable
  {
    SAMPLER_TYPE_ENABLE_FALSE      = PICA_DATA_TEXTURE1_SAMPLER_TYPE_FALSE,      // TEXTURE1��TEXTURE2�������l�Ȃ̂ŁATEXTURE1�̂ق��������Ă����B
    SAMPLER_TYPE_ENABLE_TEXTURE_2D = PICA_DATA_TEXTURE1_SAMPLER_TYPE_TEXTURE_2D,
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  // �e�N�X�`���}�b�p�[  ���b�v�ݒ�(���s�[�g�ݒ�)
  enum Wrap
  {
    WRAP_CLAMP_TO_EDGE   = PICA_DATA_TEXTURE_WRAP_CLAMP_TO_EDGE  ,  // �e�N�X�`���̒[�̃s�N�Z���������L�΂��܂��B 
    WRAP_CLAMP_TO_BORDER = PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,  // �e�N�X�`���̒[�Ń{�[�_�[�J���[�������L�΂��܂��B
    WRAP_REPEAT          = PICA_DATA_TEXTURE_WRAP_REPEAT         ,  // �e�N�X�`���̒[�ŌJ��Ԃ��܂��B 
    WRAP_MIRRORED_REPEAT = PICA_DATA_TEXTURE_WRAP_MIRRORED_REPEAT,  // �e�N�X�`���̒[�Ŕ��]���Ȃ���J��Ԃ��܂��B 
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  // �e�N�X�`���}�b�p�[  �g�厞�t�B���^
  enum MagFilter
  {
    MAG_FILTER_NEAREST = PICA_DATA_TEXTURE_MAG_FILTER_NEAREST,  // �j�A���X�g(��ԂȂ�)�ł��B
    MAG_FILTER_LINEAR  = PICA_DATA_TEXTURE_MAG_FILTER_LINEAR ,  // ���j�A(��Ԃ���)�ł��B
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  // �e�N�X�`���}�b�p�[  �k�����t�B���^
  enum MinFilter
  {
    MIN_FILTER_NEAREST                = PICA_DATA_TEXTURE_MIN_FILTER_NEAREST                ,  // �j�A���X�g(��ԂȂ�)�ŁA�~�b�v�}�b�v�͎g�p���܂���B 
    MIN_FILTER_NEAREST_MIPMAP_NEAREST = PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_NEAREST ,  // �j�A���X�g(��ԂȂ�)�ŁA�~�b�v�}�b�v���j�A���X�g(��ԂȂ�)�ł��B
    MIN_FILTER_NEAREST_MIPMAP_LINEAR  = PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_LINEAR  ,  // �j�A���X�g(��ԂȂ�)�ŁA�~�b�v�}�b�v�̓��j�A(��Ԃ���)�ł��B 
    MIN_FILTER_LINEAR                 = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR                 ,  // ���j�A(��Ԃ���)�ŁA�~�b�v�}�b�v�͎g�p���܂���B 
    MIN_FILTER_LINEAR_MIPMAP_NEAREST  = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_NEAREST  ,  // ���j�A(��Ԃ���)�ŁA�~�b�v�}�b�v�̓j�A���X�g(��ԂȂ�)�ł��B 
    MIN_FILTER_LINEAR_MIPMAP_LINEAR   = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_LINEAR   ,  // ���j�A(��Ԃ���)�ŁA�~�b�v�}�b�v�����j�A(��Ԃ���)�ł��B      
  };
  enum MinFilterDown  // 2:2
  {
    MIN_FILTER_DOWN_0 = 0,
    MIN_FILTER_DOWN_1 = 1,
  };
  enum MinFilterUp  // 24:24
  {
    MIN_FILTER_UP_0 = 0,
    MIN_FILTER_UP_1 = 1,
  };
  // Up Down Mipmap �g��������
  // 0  0    0      MIN_FILTER_NEAREST
  // 0  1    0      MIN_FILTER_LINEAR
  // 0  0    1      MIN_FILTER_NEAREST_MIPMAP_NEAREST
  // 0  1    1      MIN_FILTER_LINEAR_MIPMAP_NEAREST
  // 1  0    *      MIN_FILTER_NEAREST_MIPMAP_LINEAR
  // 1  1    *      MIN_FILTER_LINEAR_MIPMAP_LINEAR

  //MinLodLevel�������BMaxLodLevel���ǉ����Ă����������Benum�Ȃ��Ő����ł������ȁB
  //LodBias�������Benum�Ȃ��Ő����ł������ȁB
  
  enum
  {
    TEXTURE_NUM = nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY,  // = 3  // �e�N�X�`����(�e�N�X�`���}�b�p�[���ł���A�e�N�X�`���T���v���[���ł���)(�e�N�X�`���R�[�f�B�l�[�^���ł͂Ȃ�)
                                                                                 // 0,1,2,Proc����Proc�͏���
  };
};



// TextureCombiner
struct H3dTextureCombinerExConst
{
  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum Combine
  {
    COMBINE_REPLACE      = PICA_DATA_TEX_ENV_COMBINE_REPLACE     ,    //!< A �ł�
    COMBINE_MODULATE     = PICA_DATA_TEX_ENV_COMBINE_MODULATE    ,    //!< A * B �ł��B
    COMBINE_ADD          = PICA_DATA_TEX_ENV_COMBINE_ADD         ,    //!< A + B �ł��B
    COMBINE_ADDSIGNED    = PICA_DATA_TEX_ENV_COMBINE_ADD_SIGNED  ,    //!< A + B - 0.5 �ł��B
    COMBINE_INTERPOLATE  = PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE ,    //!< A * C + B * ( 1 - C ) �ł��B
    COMBINE_SUBTRACT     = PICA_DATA_TEX_ENV_COMBINE_SUBTRACT    ,    //!< A - B �ł��B
    COMBINE_DOT3_RGB     = PICA_DATA_TEX_ENV_COMBINE_DOT3_RGB    ,    //!< RGB �� Dot ( A , B ) �ł��B
    COMBINE_DOT3_RGBA    = PICA_DATA_TEX_ENV_COMBINE_DOT3_RGBA   ,    //!< RGBA �� Dot ( A , B ) �ł��B
    COMBINE_MULT_ADD     = PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP,    //!< ( A * B ) + C �ł��B
    COMBINE_ADD_MULT     = PICA_DATA_TEX_ENV_COMBINE_ADD_MULT_DMP     //!< ( A + B ) * C �ł��B
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum Scale
  {
    SCALE_ONE   = PICA_DATA_TEX_ENV_SCALE_1,    //!< 1.0 �{�B
    SCALE_TWO   = PICA_DATA_TEX_ENV_SCALE_2,    //!< 2.0 �{�B
    SCALE_FOUR  = PICA_DATA_TEX_ENV_SCALE_4     //!< 4.0 �{�B
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum OperandRgb
  {
    OPERANDRGB_SRC_COLOR            = PICA_DATA_OPE_RGB_SRC_COLOR          ,     //!< RGB �ł��B
    OPERANDRGB_ONE_MINUS_SRC_COLOR  = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_COLOR,     //!< 1 - RGB �ł��B
    OPERANDRGB_SRC_ALPHA            = PICA_DATA_OPE_RGB_SRC_ALPHA          ,     //!< �A���t�@�ł��B
    OPERANDRGB_ONE_MINUS_SRC_ALPHA  = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_ALPHA,     //!< 1 - �A���t�@�ł��B
    OPERANDRGB_RED                  = PICA_DATA_OPE_RGB_SRC_R_DMP          ,     //!< R �����ł��B
    OPERANDRGB_ONE_MINUS_RED        = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_R_DMP,     //!< 1 - R �����ł��B
    OPERANDRGB_GREEN                = PICA_DATA_OPE_RGB_SRC_G_DMP          ,     //!< G �����ł��B
    OPERANDRGB_ONE_MINUS_GREEN      = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_G_DMP,     //!< 1 - G �����ł��B
    OPERANDRGB_BLUE                 = PICA_DATA_OPE_RGB_SRC_B_DMP          ,     //!< B �����ł��B
    OPERANDRGB_ONE_MINUS_BLUE       = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_B_DMP      //!< 1 - B �����ł��B
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum OperandAlpha
  {
    OPERANDALPHA_SRC_ALPHA            = PICA_DATA_OPE_ALPHA_SRC_ALPHA          ,   //!< �A���t�@�ł��B
    OPERANDALPHA_SRC_ONE_MINUS_ALPHA  = PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_ALPHA,   //!< 1 - �A���t�@�ł� 
    OPERANDALPHA_RED                  = PICA_DATA_OPE_ALPHA_SRC_R_DMP          ,   //!< R �����ł��B
    OPERANDALPHA_ONE_MINUS_RED        = PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_R_DMP,   //!< 1 - R �����ł��B
    OPERANDALPHA_GREEN                = PICA_DATA_OPE_ALPHA_SRC_G_DMP          ,   //!< G �����ł��B
    OPERANDALPHA_ONE_MINUS_GREEN      = PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_G_DMP,   //!< 1 - G �����ł��B
    OPERANDALPHA_BLUE                 = PICA_DATA_OPE_ALPHA_SRC_B_DMP          ,   //!< B �����ł��B
    OPERANDALPHA_ONE_MINUS_BLUE       = PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_B_DMP    //!< 1 - B �����ł��B
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum Source
  {
    SOURCE_TEXTURE0                  = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0                    ,    //!< �e�N�X�`�� 0 �ł��B
    SOURCE_TEXTURE1                  = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1                    ,    //!< �e�N�X�`�� 1 �ł��B
    SOURCE_TEXTURE2                  = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2                    ,    //!< �e�N�X�`�� 2 �ł��B
    SOURCE_TEXTURE3                  = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE3                    ,    //!< �e�N�X�`�� 3 �ł��B
    SOURCE_CONSTANT                  = PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT                    ,    //!< �R���X�^���g�J���[�ł��B
    SOURCE_PRIMARY_COLOR             = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR               ,    //!< ���_�V�F�[�_�[�̏o�͌��ʂł��B
    SOURCE_FRAGMENT_PRIMARY_COLOR    = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP  ,    //!< �v���C�}���J���[�ł��B
    SOURCE_FRAGMENT_SECONDARY_COLOR  = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_SECONDARY_COLOR_DMP,    //!< �Z�J���_���J���[�ł��B
    SOURCE_PREVIOUS                  = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS                    ,    //!< �O�i�̏o�͌��ʂł��B
    SOURCE_PREVIOUS_BUFFER           = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP              //!< �O�i�̃o�b�t�@�ł��B
  };

  enum
  {
    STEP_NUM    = 6,  // �e�N�X�`���R���o�C�i�̒i��
    SOURCE_NUM  = 3   // �e�N�X�`���R���o�C�i1�i�ɂ�����\�[�X��
  };
};

// FragmentShader
struct H3dFragmentShaderExConst
{
  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum BufferInput
  {
    BUFFERINPUT_PREVIOUS_BUFFER  = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP,   //!< �O�i�̃o�b�t�@�ł��B
    BUFFERINPUT_PREVIOUS         = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS               //!< �O�i�̏o�͌��ʂł��B
  };
};

// StencilOperation
struct H3dStencilOperationExConst
{
  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum TestFunc
  {
    TESTFUNC_NEVER     = PICA_DATA_STENCIL_TEST_NEVER   ,  //!< ���ׂĕs�ʉ߂ł��B 
    TESTFUNC_ALWAYS    = PICA_DATA_STENCIL_TEST_ALWAYS  ,  //!< ���ׂĒʉ߂ł��B 
    TESTFUNC_EQUAL     = PICA_DATA_STENCIL_TEST_EQUAL   ,  //!< �Q�ƒl�Ɠ�������Βʉ߂��܂��B 
    TESTFUNC_NOTEQUAL  = PICA_DATA_STENCIL_TEST_NOTEQUAL,  //!< �Q�ƒl�Ɠ������Ȃ���Βʉ߂��܂��B 
    TESTFUNC_LESS      = PICA_DATA_STENCIL_TEST_LESS    ,  //!< �Q�ƒl��菬������Βʉ߂��܂��B 
    TESTFUNC_LEQUAL    = PICA_DATA_STENCIL_TEST_LEQUAL  ,  //!< �Q�ƒl�ȉ��ł���Βʉ߂��܂��B 
    TESTFUNC_GREATER   = PICA_DATA_STENCIL_TEST_GREATER ,  //!< �Q�ƒl���傫����Βʉ߂��܂��B 
    TESTFUNC_GEQUAL    = PICA_DATA_STENCIL_TEST_GEQUAL  ,  //!< �Q�ƒl�ȏ�ł���Βʉ߂��܂��B 
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum StencilOp
  {
    STENCILOP_KEEP           = PICA_DATA_STENCIL_OP_KEEP     ,   //!< ���݂̒l��ێ����܂��B 
    STENCILOP_ZERO           = PICA_DATA_STENCIL_OP_ZERO     ,   //!< 0 �ɒu�����܂��B 
    STENCILOP_REPLACE        = PICA_DATA_STENCIL_OP_REPLACE  ,   //!< �Q�ƒl�ɒu�����܂��B 
    STENCILOP_INCREASE       = PICA_DATA_STENCIL_OP_INCR     ,   //!< ���������܂��B�ɒl�����̓N�����v�ł��B 
    STENCILOP_DECREASE       = PICA_DATA_STENCIL_OP_DECR     ,   //!< ���������܂��B�ɒl�����̓N�����v�ł��B 
    STENCILOP_INVERT         = PICA_DATA_STENCIL_OP_INVERT   ,   //!< �r�b�g�P�ʂŔ��]�����܂��B 
    STENCILOP_INCREASE_WRAP  = PICA_DATA_STENCIL_OP_INCR_WRAP,   //!< ���������܂��B�ɒl�����͉�荞�݂ł��B 
    STENCILOP_DECREASE_WRAP  = PICA_DATA_STENCIL_OP_DECR_WRAP,   //!< ���������܂��B�ɒl�����͉�荞�݂ł��B 
  };
};

// AlphaTest
struct H3dAlphaTestExConst
{
  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum TestFunc
  {
    TESTFUNC_NEVER     = PICA_DATA_ALPHA_TEST_NEVER   ,  //!< ���ׂĕs�ʉ߂ł��B 
    TESTFUNC_ALWAYS    = PICA_DATA_ALPHA_TEST_ALWAYS  ,  //!< ���ׂĒʉ߂ł��B 
    TESTFUNC_EQUAL     = PICA_DATA_ALPHA_TEST_EQUAL   ,  //!< �Q�ƒl�Ɠ�������Βʉ߂��܂��B 
    TESTFUNC_NOTEQUAL  = PICA_DATA_ALPHA_TEST_NOTEQUAL,  //!< �Q�ƒl�Ɠ������Ȃ���Βʉ߂��܂��B 
    TESTFUNC_LESS      = PICA_DATA_ALPHA_TEST_LESS    ,  //!< �Q�ƒl��菬������Βʉ߂��܂��B 
    TESTFUNC_LEQUAL    = PICA_DATA_ALPHA_TEST_LEQUAL  ,  //!< �Q�ƒl�ȉ��ł���Βʉ߂��܂��B 
    TESTFUNC_GREATER   = PICA_DATA_ALPHA_TEST_GREATER ,  //!< �Q�ƒl���傫����Βʉ߂��܂��B 
    TESTFUNC_GEQUAL    = PICA_DATA_ALPHA_TEST_GEQUAL  ,  //!< �Q�ƒl�ȏ�ł���Βʉ߂��܂��B 
  };
};

// DepthTest
struct H3dDepthTestExConst
{
  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum TestFunc
  {
    TESTFUNC_NEVER     = PICA_DATA_DEPTH_TEST_NEVER   ,  //!< ���ׂĕs�ʉ߂ł��B 
    TESTFUNC_ALWAYS    = PICA_DATA_DEPTH_TEST_ALWAYS  ,  //!< ���ׂĒʉ߂ł��B 
    TESTFUNC_EQUAL     = PICA_DATA_DEPTH_TEST_EQUAL   ,  //!< �Q�ƒl�Ɠ�������Βʉ߂��܂��B 
    TESTFUNC_NOTEQUAL  = PICA_DATA_DEPTH_TEST_NOTEQUAL,  //!< �Q�ƒl�Ɠ������Ȃ���Βʉ߂��܂��B 
    TESTFUNC_LESS      = PICA_DATA_DEPTH_TEST_LESS    ,  //!< �Q�ƒl��菬������Βʉ߂��܂��B 
    TESTFUNC_LEQUAL    = PICA_DATA_DEPTH_TEST_LEQUAL  ,  //!< �Q�ƒl�ȉ��ł���Βʉ߂��܂��B 
    TESTFUNC_GREATER   = PICA_DATA_DEPTH_TEST_GREATER ,  //!< �Q�ƒl���傫����Βʉ߂��܂��B 
    TESTFUNC_GEQUAL    = PICA_DATA_DEPTH_TEST_GEQUAL  ,  //!< �Q�ƒl�ȏ�ł���Βʉ߂��܂��B 
  };
};

// Rasterization
struct H3dRasterizationExConst
{
  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum CullingMode  // PICA_REG_CULL_FACE, PICA_CMD_DATA_CULL_FACE�����Ȃ��A�J�����O���[�h�̒�`���Ȃ��̂Œ萔���w��B
  {
    CULLINGMODE_NONE  = 0,  //!< ���ʕ\���ł��B
    CULLINGMODE_FRONT = 1,  //!< �\�ʃJ�����O�ł��B
    CULLINGMODE_BACK  = 2,  //!< ���ʃJ�����O�ł��B
  };
};


// Blend
struct H3dBlendExConst
{
  // PICA_DATA_ENABLE_COLOR_LOGIC_OP��PICA_DATA_ENABLE_BLEND��PICA_REG_COLOR_OPERATION�ɐݒ肷��
  enum Mode
  {
    MODE_NOT_USE       ,  // �g�p���܂���B
    MODE_BLEND         ,  // �u�����h�����ł��B           // MODE_BLEND�ɂ��Ă�MODE_SEPARATE_BLEND���ݒ肳��܂��B  // @note h3d�ł��L���ȋL�q���H��h3d�̏ꍇ�͎�����gfx�̂��́uMODE_BLEND�ɂ��Ă�MODE_SEPARATE_BLEND���ݒ肳��܂��B�v�ɍ����悤�ɐݒ肵�Ă���B
    MODE_SEPARATE_BLEND,  // �Z�p���[�g�u�����h�����ł��B
    MODE_LOGIC            // �_�����Z�ł��B
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum LogicOperation
  {
    LOGIC_OPERATION_CLEAR         = PICA_DATA_LOGIC_CLEAR         ,  // ���ׂẴr�b�g�l�� 0 �ɂ��܂��B 
    LOGIC_OPERATION_AND           = PICA_DATA_LOGIC_AND           ,  // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���ς��Ƃ�܂��B 
    LOGIC_OPERATION_AND_REVERSE   = PICA_DATA_LOGIC_AND_REVERSE   ,  // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l�̘_���ς��Ƃ�܂��B 
    LOGIC_OPERATION_COPY          = PICA_DATA_LOGIC_COPY          ,  // �\�[�X�J���[�̃r�b�g�l���Ƃ�܂��B 
    LOGIC_OPERATION_SET           = PICA_DATA_LOGIC_SET           ,  // ���ׂẴr�b�g�l�� 1 �ɂ��܂��B 
    LOGIC_OPERATION_COPY_INVERTED = PICA_DATA_LOGIC_COPY_INVERTED ,  // �\�[�X�J���[�̔��]�r�b�g�l���Ƃ�܂��B 
    LOGIC_OPERATION_NOOP          = PICA_DATA_LOGIC_NOOP          ,  // �f�B�X�e�B�l�[�V�����J���[�̃r�b�g�l���Ƃ�܂��B 
    LOGIC_OPERATION_INVERT        = PICA_DATA_LOGIC_INVERT        ,  // �f�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l���Ƃ�܂��B 
    LOGIC_OPERATION_NAND          = PICA_DATA_LOGIC_NAND          ,  // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���ς̔��]�r�b�g�l���Ƃ�܂��B 
    LOGIC_OPERATION_OR            = PICA_DATA_LOGIC_OR            ,  // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���a���Ƃ�܂��B 
    LOGIC_OPERATION_NOR           = PICA_DATA_LOGIC_NOR           ,  // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���a�̔��]�r�b�g�l���Ƃ�܂��B 
    LOGIC_OPERATION_XOR           = PICA_DATA_LOGIC_XOR           ,  // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔r���I�_���a���Ƃ�܂��B 
    LOGIC_OPERATION_EQUIV         = PICA_DATA_LOGIC_EQUIV         ,  // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔r���I�_���a�̔��]�r�b�g�l���Ƃ�܂��B
    LOGIC_OPERATION_AND_INVERTED  = PICA_DATA_LOGIC_AND_INVERTED  ,  // �\�[�X�J���[�̔��]�r�b�g�l�ƁA�f�B�X�e�B�l�[�V�����J���[�̘_���ς��Ƃ�܂��B 
    LOGIC_OPERATION_OR_REVERSE    = PICA_DATA_LOGIC_OR_REVERSE    ,  // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l�̘_���a���Ƃ�܂��B 
    LOGIC_OPERATION_OR_INVERTED   = PICA_DATA_LOGIC_OR_INVERTED   ,  // �\�[�X�J���[�̔��]�r�b�g�l�ƁA�f�B�X�e�B�l�[�V�����J���[�̘_���a���Ƃ�܂��B 
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum Factor
  {
    FACTOR_ZERO                     = PICA_DATA_BLEND_FUNC_ZERO                     ,  // �w��̐����Ɂu 0 �v�𗘗p���܂��B 
    FACTOR_ONE                      = PICA_DATA_BLEND_FUNC_ONE                      ,  // �w��̐����Ɂu 1 �v�𗘗p���܂��B 
    FACTOR_SRC_COLOR                = PICA_DATA_BLEND_FUNC_SRC_COLOR                ,  // �w��̐����Ɂu�\�[�X�J���[�v�𗘗p���܂��B 
    FACTOR_ONE_MINUS_SRC_COLOR      = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR      ,  // �w��̐����Ɂu1 - �\�[�X�J���[�v �𗘗p���܂��B 
    FACTOR_DST_COLOR                = PICA_DATA_BLEND_FUNC_DST_COLOR                ,  // �w��̐����Ɂu�f�X�e�B�l�[�V�����J���[�v�𗘗p���܂��B 
    FACTOR_ONE_MINUS_DST_COLOR      = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR      ,  // �w��̐����Ɂu 1 - �f�X�e�B�l�[�V�����J���[�v�𗘗p���܂��B 
    FACTOR_SRC_ALPHA                = PICA_DATA_BLEND_FUNC_SRC_ALPHA                ,  // �w��̐����Ɂu�\�[�X�A���t�@�v�𗘗p���܂��B 
    FACTOR_ONE_MINUS_SRC_ALPHA      = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA      ,  // �w��̐����Ɂu 1 - �\�[�X�A���t�@�v �𗘗p���܂��B 
    FACTOR_DST_ALPHA                = PICA_DATA_BLEND_FUNC_DST_ALPHA                ,  // �w��̐����Ɂu�f�X�e�B�l�[�V�����A���t�@�v�𗘗p���܂��B 
    FACTOR_ONE_MINUS_DST_ALPHA      = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA      ,  // �w��̐����Ɂu 1 - �f�X�e�B�l�[�V�����A���t�@�v�𗘗p���܂��B 
    FACTOR_CONSTANT_COLOR           = PICA_DATA_BLEND_FUNC_CONSTANT_COLOR           ,  // �w��̐����Ɂu�R���X�^���g�J���[(�u�����h�J���[)�v�𗘗p���܂��B 
    FACTOR_ONE_MINUS_CONSTANT_COLOR = PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_COLOR ,  // �w��̐����Ɂu 1 - �R���X�^���g�J���[�v�𗘗p���܂��B 
    FACTOR_CONSTANT_ALPHA           = PICA_DATA_BLEND_FUNC_CONSTANT_ALPHA           ,  // �w��̐����Ɂu�R���X�^���g�A���t�@(�u�����h�A���t�@)�v�𗘗p���܂��B 
    FACTOR_ONE_MINUS_CONSTANT_ALPHA = PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_ALPHA ,  // �w��̐����Ɂu 1 - �R���X�^���g�A���t�@�v�𗘗p���܂��B 
    FACTOR_SRC_ALPHA_SATURATE       = PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE       ,  // �w��̐����Ɂu MIN( �\�[�X�A���t�@ , 1 - �f�X�e�B�l�[�V�����A���t�@ ) �v�𗘗p���܂��B 
  };

  // PICA�R�}���h�����ɂ��̂܂܎g�p���Ă���̂Œl��ς��Ȃ��悤�ɁB
  enum Equation
  {
    EQUATION_FUNC_ADD              = PICA_DATA_BLEND_EQUATION_ADD              ,  // �\�[�X�ƃf�X�e�B�l�[�V���������Z���܂��B 
    EQUATION_FUNC_SUBTRACT         = PICA_DATA_BLEND_EQUATION_SUBTRACT         ,  // �\�[�X����f�X�e�B�l�[�V���������Z���܂��B
    EQUATION_FUNC_REVERSE_SUBTRACT = PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT ,  // �f�X�e�B�l�[�V��������\�[�X�����Z���܂��B
    EQUATION_MIN                   = PICA_DATA_BLEND_EQUATION_MIN              ,  // �\�[�X�ƃf�X�e�B�l�[�V�����̂�菬�������ł��B
    EQUATION_MAX                   = PICA_DATA_BLEND_EQUATION_MAX              ,  // �\�[�X�ƃf�X�e�B�l�[�V�����̂��傫�����ł��B
  };
};



//-------------------------------------
///	H3D�V�F�[�_�ǉ��X�e�[�g�N���X
//=====================================
class H3dShaderExState
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dShaderExState);

  friend class H3dModel;
  friend class H3dMaterialExState;


public:  // ���J���Ă��郁���o

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �\����
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // @brief  �V�F�[�_�V���{�����
  struct ShaderSymbolVSInputParameter
  {
    bit32                          bitflag;      // 1�̂Ƃ��ύX�����B0�̂Ƃ��ύX���Ă��Ȃ��B
    nn::gr::BindSymbolVSInput      bind_symbol;
    gfl::math::VEC4                value;
  };

  struct ShaderSymbolVSFloatParameter
  {
    bit32                          bitflag;      // 1�̂Ƃ��ύX�����B0�̂Ƃ��ύX���Ă��Ȃ��B
    nn::gr::BindSymbolVSFloat      bind_symbol;
    gfl::math::VEC4                value;
  };

  struct ShaderSymbolVSIntegerParameter
  {
    bit32                            bitflag;      // 1�̂Ƃ��ύX�����B0�̂Ƃ��ύX���Ă��Ȃ��B
    nn::gr::BindSymbolVSInteger      bind_symbol;
    gfl::math::VEC4                  value;
  };
  // @note ���ɂ����邯�ǖ��Ή��B�����Ƃ��܂��܂Ƃ߂����B
  
  // @brief  �ݒ���e
  struct Description
  {
    struct SyambolData{
      b32               fTopNode;
      b32               hasDefaultValue;
      gfl::math::VEC4   defaultValue;
      s32               fixIndex;
    };
    
    //SyambolData�B
    const char** shader_symbol_vs_float_name_list;  // ����͌Ăяo�����ł��̂܂ܕێ����Ă����ĉ������B
                                                    // ���̃N���X���Ń|�C���^���o���Ă����ăA�N�Z�X���܂��B
    const SyambolData* shader_symbol_vs_float_param_list;  // ����͌Ăяo�����ł��̂܂ܕێ����Ă����ĉ������B
                                                             // ���̃N���X���Ń|�C���^���o���Ă����ăA�N�Z�X���܂��B
    
    ShaderSymbolVSFloatParameter        *p_shader_symbol_vs_float_param_static_buffer;
    
    s32          shader_symbol_vs_float_list_num;
    // @note �V�F�[�_�V���{���͑��ɂ����邯�ǖ��Ή��B
    
    Description(void)
      : shader_symbol_vs_float_name_list(NULL),
        shader_symbol_vs_float_param_list(NULL),
        p_shader_symbol_vs_float_param_static_buffer(NULL),
        shader_symbol_vs_float_list_num(0)
    {}
  };
  /*
    �����S���K�v���H
      const char** shader_symbol_vs_input_name_list,
      const char** shader_symbol_vs_float_name_list,
      const char** shader_symbol_vs_integer_name_list,
      const char** shader_symbol_vs_bool_name_list,
      const char** shader_symbol_gs_float_name_list,
      const char** shader_symbol_gs_bool_name_list,
      s32 shader_symbol_vs_input_list_num,
      s32 shader_symbol_vs_float_list_num,
      s32 shader_symbol_vs_integer_list_num,
      s32 shader_symbol_vs_bool_list_num,
      s32 shader_symbol_gs_float_list_num,
      s32 shader_symbol_gs_bool_list_num
  */ 


private:  // friend�ɂ͌��J���Ă������J�����o

  // �R���X�g���N�^ / �f�X�g���N�^
  H3dShaderExState(void);
  virtual ~H3dShaderExState();

  // ���� / �j��
  void Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const nw::h3d::res::MaterialContent* cont,
      const Description* desc
  ); 
  void Destroy(void);

  // �m�F�p
  void DumpShaderSymbolName(
      const nw::h3d::res::MaterialContent* cont
  ) const;

  // �V�F�[�_�V���{��
  void SetShaderSymbolVSFloat(const char* name, const gfl::math::VEC4& value);  // index�ł��g���ĉ������B
  void SetShaderSymbolVSFloat(s32 index, const gfl::math::VEC4& value);  // Create�œn����shader_symbol_vs_float_name_list�̃C���f�b�N�X

  void GetShaderSymbolVSFloat(const char* name, gfl::math::VEC4* value) const;  // index�ł��g���ĉ������B
  void GetShaderSymbolVSFloat(s32 index, gfl::math::VEC4* value) const;  // Create�œn����shader_symbol_vs_float_name_list�̃C���f�b�N�X
  
  s32 GetShaderSymbolIndex(const char* name) const;

  // �R�}���h
  void MakeShaderCommand(H3dCommand* command);

private:  // friend�ɂ����J���Ă��Ȃ�����J�����o
  void constructorThis(void);

private:  // friend�ɂ����J���Ă��Ȃ�����J�����o
  
  struct ShaderSymbolInfo
  {
    ShaderSymbolInfo() :
      vs_input_name_list(NULL),
      vs_float_name_list(NULL),
      vs_integer_name_list(NULL),
      vs_input_list_num(0),
      vs_float_list_num(0),
      vs_integer_list_num(0),
      f_static_vs_float_list(false),
      vs_input_list(NULL),
      vs_float_list(NULL),
      vs_integer_list(NULL)
    {
      
    }
    
    const char** vs_input_name_list;  // const char* vs_input_name_list[list_num];  // ���̂Ƃ���ł��������X�g�̃|�C���^���o���Ă�������
    const char** vs_float_name_list;
    const char** vs_integer_name_list;
    // @note ���ɂ����邯�ǖ��Ή��B�����Ƃ��܂��܂Ƃ߂����B
    
    s32          vs_input_list_num;
    s32          vs_float_list_num;
    s32          vs_integer_list_num;
    b32          f_static_vs_float_list;
    // @note ���ɂ����邯�ǖ��Ή��B�����Ƃ��܂��܂Ƃ߂����B

    ShaderSymbolVSInputParameter**    vs_input_list;     // vs_input_list = GFL_NEW() ShaderSymbolVSInputParameter*[list_num];
    ShaderSymbolVSFloatParameter**    vs_float_list;
    ShaderSymbolVSIntegerParameter**  vs_integer_list;
    // @note ���ɂ����邯�ǖ��Ή��B�����Ƃ��܂��܂Ƃ߂����B
  };

private:  // friend�ɂ����J���Ȃ�����J�����o
  ShaderSymbolInfo                          m_shader_symbol_info;

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
public:
  // �R�}���h���J�E���g�����Z�b�g����
  void ResetCommandCount(void);
  // 1�t���[���̃R�}���h���J�E���g�J�n
  void StartOneFrameCommandCount(void);
  // 1�t���[���̃R�}���h���J�E���g�I��
  void EndOneFrameCommandCount(void);
  // �R�}���h�����o�͂���
  void PrintCommandCount(void);
private:
  // �R�}���h���J�E���g
  struct CommandCount  // bit32�Ő������Ƃ��̃R�}���h��
  {
    s16 shader_command;
  };
  static CommandCount m_shader_command_max_set;   // shader_command���ő吔�ɂȂ����Ƃ���CommandCount
  static CommandCount m_command_curr_set;   // ����CommandCount
  
  void printCommandCount(const CommandCount& command_count);
#endif  // #if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT

};



//-------------------------------------
///	H3D�}�e���A���ǉ��X�e�[�g�N���X
//=====================================
class H3dMaterialExState
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dMaterialExState);


  friend class H3dModel;
  friend class H3dCommandListenerReadMaterial;


private:  // friend�ɂ͌��J���Ă������J�����o

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �R���X�g���N�^ / �f�X�g���N�^
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  H3dMaterialExState(void);
  ~H3dMaterialExState();


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���� / �j��
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void Initialize(gfl::heap::NwAllocator* device_allocator, const nw::h3d::res::ModelContent* model, s32 materialIndex );
 
  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // textureSampler��no�̃e�N�X�`���T���v���̃f�[�^
  void InitializeTextureSampler(s32 no, const nw::h3d::res::TextureSamplerData* textureSampler, s32 mipmapSize = 1 );



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // TextureCombiner
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
 
  // step�i��
  void InitializeTextureCombinerCombineRgb(s32 step, H3dTextureCombinerExConst::Combine combine);
  void SetTextureCombinerCombineRgb(s32 step, H3dTextureCombinerExConst::Combine combine);
  H3dTextureCombinerExConst::Combine  GetTextureCombinerCombineRgb(s32 step);

  // step�i��
  void InitializeTextureCombinerCombineAlpha(s32 step, H3dTextureCombinerExConst::Combine combine);
  void SetTextureCombinerCombineAlpha(s32 step, H3dTextureCombinerExConst::Combine combine);
  H3dTextureCombinerExConst::Combine  GetTextureCombinerCombineAlpha(s32 step);



  // step�i��
  void InitializeTextureCombinerScaleRgb(s32 step, H3dTextureCombinerExConst::Scale scale);
  void SetTextureCombinerScaleRgb(s32 step, H3dTextureCombinerExConst::Scale scale);
  H3dTextureCombinerExConst::Scale  GetTextureCombinerScaleRgb(s32 step);
  
  // step�i��
  void InitializeTextureCombinerScaleAlpha(s32 step, H3dTextureCombinerExConst::Scale scale);
  void SetTextureCombinerScaleAlpha(s32 step, H3dTextureCombinerExConst::Scale scale);
  H3dTextureCombinerExConst::Scale  GetTextureCombinerScaleAlpha(s32 step);



  // step�i��
  // no �\�[�Xno  A:�\�[�X0  B:�\�[�X1  C:�\�[�X2
  void InitializeTextureCombinerOperandRgb(s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope);
  void SetTextureCombinerOperandRgb(s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope);
  H3dTextureCombinerExConst::OperandRgb  GetTextureCombinerOperandRgb(s32 step, s32 no);
  
  // step�i��
  // no �\�[�Xno  A:�\�[�X0  B:�\�[�X1  C:�\�[�X2
  void InitializeTextureCombinerOperandAlpha(s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope);
  void SetTextureCombinerOperandAlpha(s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope);
  H3dTextureCombinerExConst::OperandAlpha  GetTextureCombinerOperandAlpha(s32 step, s32 no);



  // step�i��
  // no �\�[�Xno  A:�\�[�X0  B:�\�[�X1  C:�\�[�X2
  void InitializeTextureCombinerSourceRgb(s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  void SetTextureCombinerSourceRgb(s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  H3dTextureCombinerExConst::Source  GetTextureCombinerSourceRgb(s32 step, s32 no);
  
  // step�i��
  // no �\�[�Xno  A:�\�[�X0  B:�\�[�X1  C:�\�[�X2
  void InitializeTextureCombinerSourceAlpha(s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  void SetTextureCombinerSourceAlpha(s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  H3dTextureCombinerExConst::Source  GetTextureCombinerSourceAlpha(s32 step, s32 no);
  
  //SDK 5.0 �Ή�
  
  void SetMaterialTextureCombinerConstant(s32 step, s32 no, const gfl::grp::ColorU8& rColor );
  s32 GetMaterialTextureCombinerConstant(s32 step) const;
  gfl::grp::ColorU8 GetMaterialTextureCombinerConstantColor(s32 step) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // FragmentShader
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // step�i�ڂ͑O�i�̉����g����(step�i�ڂ͌�i�ɉ���n�����ł͂Ȃ��̂Œ��ӂ��邱��)
  void InitializeFragmentShaderBufferInputRgb(s32 step, H3dFragmentShaderExConst::BufferInput input);
  void SetFragmentShaderBufferInputRgb(s32 step, H3dFragmentShaderExConst::BufferInput input);
  H3dFragmentShaderExConst::BufferInput  GetFragmentShaderBufferInputRgb(s32 step);

  // step�i�ڂ͑O�i�̉����g����(step�i�ڂ͌�i�ɉ���n�����ł͂Ȃ��̂Œ��ӂ��邱��)
  void InitializeFragmentShaderBufferInputAlpha(s32 step, H3dFragmentShaderExConst::BufferInput input);
  void SetFragmentShaderBufferInputAlpha(s32 step, H3dFragmentShaderExConst::BufferInput input);
  H3dFragmentShaderExConst::BufferInput  GetFragmentShaderBufferInputAlpha(s32 step);




  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // StencilOperation
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeStencilOperationTest(u8 enable, H3dStencilOperationExConst::TestFunc func_func, u8 mask, u8 func_ref, u8 func_mask);
  void InitializeStencilOperationOp(
      H3dStencilOperationExConst::StencilOp op_fail, H3dStencilOperationExConst::StencilOp op_zfail, H3dStencilOperationExConst::StencilOp op_zpass);

  b32 IsStencilOperationTestEnable(void) const;
  void SetStencilOperationTestEnable(b32 enable);
  H3dStencilOperationExConst::TestFunc GetStencilOperationTestFunc(void) const;
  void SetStencilOperationTestFunc(H3dStencilOperationExConst::TestFunc func_func);
  
  void SetStencilOperationTestFuncRef(u8 func_ref);
  u8 GetStencilOperationTestFuncRef(void) const;

  u8 GetStencilOperationTestMask(void) const;  // @note Mask��FuncMask�ǂ��炩���g�p�֎~�ɂ������B�ǂ������I�[�T�����O�c�[���Őݒ�ł���ق����H
  void SetStencilOperationTestMask(u8 mask);  // @note Mask��FuncMask�ǂ��炩���g�p�֎~�ɂ������B�ǂ������I�[�T�����O�c�[���Őݒ�ł���ق����H
  u8 GetStencilOperationTestFuncMask(void) const;  // @note Mask��FuncMask�ǂ��炩���g�p�֎~�ɂ������B�ǂ������I�[�T�����O�c�[���Őݒ�ł���ق����H
  void SetStencilOperationTestFuncMask(u8 func_mask);  // @note Mask��FuncMask�ǂ��炩���g�p�֎~�ɂ������B�ǂ������I�[�T�����O�c�[���Őݒ�ł���ق����H

  H3dStencilOperationExConst::StencilOp GetStencilOperationOpFail(void) const;
  void SetStencilOperationOpFail(H3dStencilOperationExConst::StencilOp op_fail);
  H3dStencilOperationExConst::StencilOp GetStencilOperationOpZFail(void) const;
  void SetStencilOperationOpZFail(H3dStencilOperationExConst::StencilOp op_zfail);
  H3dStencilOperationExConst::StencilOp GetStencilOperationOpZPass(void) const;
  void SetStencilOperationOpZPass(H3dStencilOperationExConst::StencilOp op_zpass);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // AlphaTest
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeAlphaTest(u8 enable, H3dAlphaTestExConst::TestFunc func, u8 ref);

  b32 IsAlphaTestEnable(void) const;
  void SetAlphaTestEnable(b32 enable);  // true�̂Ƃ��A���t�@�e�X�g�L��
  H3dAlphaTestExConst::TestFunc GetAlphaTestFunc(void) const;
  void SetAlphaTestFunc(H3dAlphaTestExConst::TestFunc func);

  void SetAlphaTestRef(u8 ref);
  u8 GetAlphaTestRef(void) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // DepthTest
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeDepthTest(u8 enable, u8 mask_enable, H3dDepthTestExConst::TestFunc func);
  b32 IsDepthTestEnable(void) const;
  void SetDepthTestEnable(b32 enable);  // true�̂Ƃ��f�v�X�e�X�g�L��
  b32 IsDepthTestMaskEnable(void) const;
  void SetDepthTestMaskEnable(b32 enable);  // true�̂Ƃ��f�v�X�}�X�N�L��(true�̂Ƃ��f�v�X�o�b�t�@���X�V����)
  H3dDepthTestExConst::TestFunc GetDepthTestFunc(void) const;
  void SetDepthTestFunc(H3dDepthTestExConst::TestFunc func);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �u�����_�[
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeBlendMode(H3dBlendExConst::Mode mode);
  void InitializeBlendFunction(
      H3dBlendExConst::Equation rgb_equation,
      H3dBlendExConst::Factor   rgb_src_function,
      H3dBlendExConst::Factor   rgb_dst_function,
      H3dBlendExConst::Equation alpha_equation,
      H3dBlendExConst::Factor   alpha_src_function,
      H3dBlendExConst::Factor   alpha_dst_function
  );
  void InitializeBlendLogicOperation(H3dBlendExConst::LogicOperation operation);
  // �S�Ă̐ݒ肪�I�������ɌĂԏ�����
  void InitializeBlendAfterAllSet(void);

  // @attention �y���Ӂz�u�����h���[�h�ɂ���Đݒ�ł����肵�Ȃ������肷��悤�ɐ��䂵�Ă���̂ŁA�܂��ŏ��Ƀu�����h���[�h��ύX���ĉ������B
  void SetBlendMode(H3dBlendExConst::Mode mode);
  H3dBlendExConst::Mode GetBlendMode(void) const;

  //void SetBlendColor(const gfl::grp::ColorF32& c);  // nw::h3d::MaterialState�ɂ���
  //void GetBlendColor(gfl::grp::ColorF32* c) const;  // nw::h3d::MaterialState�ɂ���
  
  H3dBlendExConst::LogicOperation GetBlendLogicOperation(void) const;
  void SetBlendLogicOperation(H3dBlendExConst::LogicOperation operation);

  H3dBlendExConst::Factor GetBlendFunctionSourceRgb(void) const;
  void SetBlendFunctionSourceRgb(H3dBlendExConst::Factor factor);
  H3dBlendExConst::Factor GetBlendFunctionSourceAlpha(void) const;
  void SetBlendFunctionSourceAlpha(H3dBlendExConst::Factor factor);

  H3dBlendExConst::Factor GetBlendFunctionDestinationRgb(void) const;
  void SetBlendFunctionDestinationRgb(H3dBlendExConst::Factor factor);
  H3dBlendExConst::Factor GetBlendFunctionDestinationAlpha(void) const;
  void SetBlendFunctionDestinationAlpha(H3dBlendExConst::Factor factor);

  H3dBlendExConst::Equation GetBlendEquationRgb(void) const;
  void SetBlendEquationRgb(H3dBlendExConst::Equation equation);
  H3dBlendExConst::Equation GetBlendEquationAlpha(void) const;
  void SetBlendEquationAlpha(H3dBlendExConst::Equation equation);


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �o�b�t�@����
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeColorBufferRead( b8 fColorRead );  
  void InitializeColorBufferWrite( b8 fColorWrite );  
  void InitializeDepthBufferRead( b8 fDepthRead, b8 fStencilRead );  
  void InitializeDepthBufferWrite( b8 fDepthWrite, b8 fStencilWrite );  

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �e�N�X�`���}�b�p�[(�e�N�X�`���T���v���[)
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // U�����J��Ԃ��ݒ���s���B
  void InitializeTextureSamplerWrapU(s32 no, H3dTextureSamplerExConst::Wrap wrap);
  void SetTextureSamplerWrapU(s32 no, H3dTextureSamplerExConst::Wrap wrap);
  // U�����J��Ԃ��ݒ�𓾂�B
  H3dTextureSamplerExConst::Wrap  GetTextureSamplerWrapU(s32 no) const;

  // V�����J��Ԃ��ݒ���s���B
  void InitializeTextureSamplerWrapV(s32 no, H3dTextureSamplerExConst::Wrap wrap);
  void SetTextureSamplerWrapV(s32 no, H3dTextureSamplerExConst::Wrap wrap);
  // V�����J��Ԃ��ݒ�𓾂�B
  H3dTextureSamplerExConst::Wrap  GetTextureSamplerWrapV(s32 no) const;
  // �e�N�X�`���̍��W�́AST�͎g�킸�AUV�œ��ꂵ�Ă����B

  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // �e�N�X�`���T���v���[�^�C�v�L������
  void InitializeTextureSamplerTypeEnable(s32 no, H3dTextureSamplerExConst::SamplerTypeEnable enable);
  void SetTextureSamplerTypeEnable(s32 no, H3dTextureSamplerExConst::SamplerTypeEnable enable);
  H3dTextureSamplerExConst::SamplerTypeEnable  GetTextureSamplerTypeEnable(s32 no) const;

  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // �e�N�X�`���T���v���[�^�C�v
  void InitializeTextureSamplerType(s32 no, H3dTextureSamplerExConst::SamplerType sampler_type);
  void SetTextureSamplerType(s32 no, H3dTextureSamplerExConst::SamplerType sampler_type);
  H3dTextureSamplerExConst::SamplerType  GetTextureSamplerType(s32 no) const;
  //res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B

  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // �g�厞�̕�ԏ���
  void InitializeTextureSamplerMagFilter(s32 no, H3dTextureSamplerExConst::MagFilter mag_filter);
  void SetTextureSamplerMagFilter(s32 no, H3dTextureSamplerExConst::MagFilter mag_filter);
  H3dTextureSamplerExConst::MagFilter GetTextureSamplerMagFilter(s32 no) const;

  // �k�����̕�ԏ���
  void InitializeTextureSamplerMinFilter(s32 no, H3dTextureSamplerExConst::MinFilter min_filter);
  void InitializeTextureSamplerMinFilterDown(s32 no, H3dTextureSamplerExConst::MinFilterDown min_filter_down);
  void InitializeTextureSamplerMinFilterUp(s32 no, H3dTextureSamplerExConst::MinFilterUp min_filter_up);
  void SetTextureSamplerMinFilter(s32 no, H3dTextureSamplerExConst::MinFilter min_filter);
  H3dTextureSamplerExConst::MinFilter GetTextureSamplerMinFilter(s32 no) const;
  
  // �~�b�v�}�b�v�ݒ�
  void SetTextureMipMapLodMinLevel( s32 no, s32 level );
  void SetTextureMipMapLodBias( s32 no, s32 bias );

private:
  // ���ݐݒ肳��Ă���down�Aup���g���āAmin_filter���X�V���Ă����B
  void setTextureSamplerMinFilterByUpDown(s32 no);


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // Rasterization
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeRasterizationCullingMode(H3dRasterizationExConst::CullingMode cull);
  void SetRasterizationCullingMode(H3dRasterizationExConst::CullingMode cull);
  H3dRasterizationExConst::CullingMode GetRasterizationCullingMode(void);





  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �R�}���h
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  void MakeMutableCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs);
  void ResetDirtyBits();

public:  
  
  // SDK 5.0�Ή�
  void MakeCombinerColorCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs, const nw::h3d::MaterialState* materialState) const;
  // textureSamplers��nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY�̃e�N�X�`���T���v���̃f�[�^�̔z��
  void MakeTextureSamplerCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs, const nw::h3d::MaterialState* materialState/*, const nw::h3d::res::TextureSamplerData* textureSamplers*/) const;
  
private:  
  void makeMutableBufferBlensCommand( nn::gr::CTR::CommandBufferJumpHelper& dcs ) const;

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friend���擾���邽�߂̃����o�֐�
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:  
  H3dShaderExState* GetShaderExState(void) { return &m_shader_ex_state; }




private:  // friend�ɂ����J���Ȃ�����J�����o

  // �R���X�g���N�^�ł���Ă���̂Ɠ���������
  void constructorThis(void);
  void debugPrint(void);


private:  // friend�ɂ����J���Ȃ�����J�����o

  // @brief  �e�N�X�`���R���o�C�i1�\�[�X���̏��
  struct TextureCombinerSource
  {
    enum
    {
      TEXTURE_COMBINER_OPERAND_RGB_MUTABLE    = 1 <<  0,
      TEXTURE_COMBINER_OPERAND_ALPHA_MUTABLE  = 1 <<  1,
      TEXTURE_COMBINER_SOURCE_RGB_MUTABLE     = 1 <<  2,
      TEXTURE_COMBINER_SOURCE_ALPHA_MUTABLE   = 1 <<  3
    };
    bit32 bitflag;  // �ύX�������̂̃r�b�g�𗧂Ă�

    //H3dTextureCombinerExConst::OperandRgb    operand_rgb;
    //H3dTextureCombinerExConst::OperandAlpha  operand_alpha;
    //H3dTextureCombinerExConst::Source        source_rgb;
    //H3dTextureCombinerExConst::Source        source_alpha;

    u8  operand_rgb;
    u8  operand_alpha;
    u8  source_rgb;
    u8  source_alpha;
  };
 
  // @brief  �e�N�X�`���R���o�C�i1�i���̏��
  struct TextureCombinerStep
  {
    enum
    {
      TEXTURE_COMBINER_COMBINE_RGB_MUTABLE        = 1 <<  0,
      TEXTURE_COMBINER_COMBINE_ALPHA_MUTABLE      = 1 <<  1,
      TEXTURE_COMBINER_SCALE_RGB_MUTABLE          = 1 <<  2,
      TEXTURE_COMBINER_SCALE_ALPHA_MUTABLE        = 1 <<  3,
      FRAGMENT_SHADER_BUFFER_INPUT_RGB_MUTABLE    = 1 <<  4,
      FRAGMENT_SHADER_BUFFER_INPUT_ALPHA_MUTABLE  = 1 <<  5
    };
    bit32 bitflag;  // �ύX�������̂̃r�b�g�𗧂Ă�

    //H3dTextureCombinerExConst::Combine  combine_rgb;
    //H3dTextureCombinerExConst::Combine  combine_alpha;
    //H3dTextureCombinerExConst::Scale    scale_rgb;
    //H3dTextureCombinerExConst::Scale    scale_alpha;

    u8  combine_rgb;
    u8  combine_alpha;
    u8  scale_rgb;
    u8  scale_alpha;
    
    TextureCombinerSource  source[H3dTextureCombinerExConst::SOURCE_NUM];
    
    //H3dFragmentShaderExConst::BufferInput  buffer_input_rgb;
    //H3dFragmentShaderExConst::BufferInput  buffer_input_alpha;

    u8  buffer_input_rgb;
    u8  buffer_input_alpha;
  };
  
  // @brief  �X�e���V���I�y���[�V�����̏��
  struct StencilOperationInfo
  {
    enum
    {
      STENCIL_TEST_MUTABLE    = 1 <<  0,
      STENCIL_OP_MUTABLE      = 1 <<  1,
    };
    bit32 bitflag;  // �ύX�������̂̃r�b�g�𗧂Ă�

    u8 enable;
    u8 func_func;
    u8 mask;
    u8 func_ref;
    u8 func_mask;
    u8 op_fail;
    u8 op_zfail;
    u8 op_zpass;

#if 0
/*
CTR
DMPGL2.0�V�X�e��API�d�l��

glDisable(GL_STENCIL_TEST)�A
glEnable(GL_STENCIL_TEST)
glStencilFunc��func
glStencilMask
glStencilFunc��ref
glStencilFunc��mask
glStencilOp��fail
glStencilOp��zfail
glStencilOp��zpass
*/
#endif

  };

  // @brief  �A���t�@�e�X�g�̏��
  struct AlphaTestInfo
  {
    enum
    {
      ALPHA_TEST_MUTABLE     = 1 <<  0,
    };
    bit32 bitflag;  // �ύX�������̂̃r�b�g�𗧂Ă�

    u8 enable;
    u8 func;
    u8 ref;
  };

  // @brief  �f�v�X�e�X�g�̏��
  struct DepthColorMaskInfo
  {
    enum
    {
      DEPTH_TEST_MUTABLE        = 1 <<  0,
      DEPTH_COLOR_MASK_MUTABLE  = 1 <<  1,
    };
    u32 bitflag            :8;  // �ύX�������̂̃r�b�g�𗧂Ă�

    u32 depth_test_enable  :1;  // 1�̂Ƃ��f�v�X�e�X�g�L��
    u32 depth_test_func    :7;
    u32 red_mask           :1;  // ���̂Ƃ���ύX����\��͂Ȃ��̂ŏ��1�ɂ��Ă���
    u32 green_mask         :1;  // ���̂Ƃ���ύX����\��͂Ȃ��̂ŏ��1�ɂ��Ă���
    u32 blue_mask          :1;  // ���̂Ƃ���ύX����\��͂Ȃ��̂ŏ��1�ɂ��Ă���
    u32 alpha_mask         :1;  // ���̂Ƃ���ύX����\��͂Ȃ��̂ŏ��1�ɂ��Ă���
    u32 depth_mask         :1;  // 1�̂Ƃ��f�v�X�}�X�N�L��(1�̂Ƃ��f�v�X�o�b�t�@���X�V����)
  };

  // @brief  �u�����_�[�̏��
  struct BlendInfo
  {
    enum
    {
      BLEND_MODE_MUTABLE             = 1 <<  0,
      BLEND_FUNCTION_MUTABLE         = 1 <<  1,
      BLEND_LOGIC_OPERATION_MUTABLE  = 1 <<  2,
    };
    u32 bitflag                    :3;

    u32 blend_mode                 :3;
    
    u32 blend_equation_rgb         :3;
    u32 blend_equation_alpha       :3;

    u32 blend_function_src_rgb     :4;
    u32 blend_function_src_alpha   :4;
    u32 blend_function_dst_rgb     :4;
    u32 blend_function_dst_alpha   :4;
    
    u32 logic_operation            :4;
  };
  
  // @brief �o�b�t�@���[�h�̏��
  struct BufferReadInfo{
    u32 fColorRead      : 1;
    u32 fColorWrite     : 1;
    u32 fDepthRead      : 1;
    u32 fDepthWrite     : 1;
    u32 fStencilRead    : 1;
    u32 fStencilWrite   : 1;
    u32 padding         : 26;
  };

  // @brief  �e�N�X�`���T���v���[���
  struct TextureSamplerInfo
  {
    enum
    {
      SAMPLER_TYPE_MUTABLE   = 1 <<  0,
      WRAP_MUTABLE           = 1 <<  1,
      FILTER_MUTABLE         = 1 <<  2,
      MAG_LOD_LEVEL_MUTABLE  = 1 <<  3,
      MIN_LOD_LEVEL_MUTABLE  = 1 <<  4,
      LOD_BIAS_MUTABLE       = 1 <<  5,
    };
    u32 bitflag             :3;  // �ύX�������̂̃r�b�g�𗧂Ă�
    u32 sampler_type_enable :1;  // sampler_type_enable��sampler_type�̗�����
    u32 sampler_type        :3;  // ������ƍX�V���邱�ƁB
    u32 wrap_u              :2;
    u32 wrap_v              :2;
    u32 mag_filter          :1;  // down�Aup���{���̃f�[�^�Ȃ̂ŁA����̈����ɂ͒��ӂ��邱�ƁB
    u32 min_filter_down     :1;  // �{���̃f�[�^
    u32 min_filter_up       :1;  // �{���̃f�[�^
    u32 min_filter          :3;
    u32 mag_lod_level       :4;
    u32 min_lod_level       :4;
    u32 mipmapSize          :4;
    f32 lod_bias;              
  };

  // @brief  ���X�^���C�[�[�V�������
  struct RasterizationInfo
  {
    enum
    {
      CULLING_MODE_MUTABLE  = 1 << 0,
    };
    bit32 bitflag;  // �ύX�������̂̃r�b�g�𗧂Ă�

    u8 culling_mode;
  };

private:  // friend�ɂ����J���Ȃ�����J�����o
  TextureCombinerStep  m_tex_comb_step[H3dTextureCombinerExConst::STEP_NUM];
  StencilOperationInfo m_stencil_op_info;
  AlphaTestInfo        m_alpha_test_info;
  DepthColorMaskInfo   m_depth_color_mask_info;
  BlendInfo            m_blend_info;
  BufferReadInfo       m_buffer_read_info;
  TextureSamplerInfo   m_tex_sampler_info[H3dTextureSamplerExConst::TEXTURE_NUM];
  RasterizationInfo    m_raster_info;
  
  s32                  m_MaterialTextureCombinerConstantNo[H3dTextureCombinerExConst::STEP_NUM];
  gfl::grp::ColorU8    m_ConbinerColor[H3dTextureCombinerExConst::STEP_NUM];
  
  H3dShaderExState     m_shader_ex_state;


#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
public:
  // �R�}���h���J�E���g�����Z�b�g����
  void ResetCommandCount(void);
  // 1�t���[���̃R�}���h���J�E���g�J�n
  void StartOneFrameCommandCount(void);
  // 1�t���[���̃R�}���h���J�E���g�I��
  void EndOneFrameCommandCount(void);
  // �R�}���h�����o�͂���
  void PrintCommandCount(void);
private:
  // �R�}���h���J�E���g
  struct CommandCount  // bit32�Ő������Ƃ��̃R�}���h��
  {
    s16 mutable_command;
    s16 tex_sampler_command;
  };
  static CommandCount m_mutable_command_max_set;  // mutable_command���ő吔�ɂȂ����Ƃ���CommandCount
  static CommandCount m_tex_sampler_command_max_set;  // tex_sampler_command���ő吔�ɂȂ����Ƃ���CommandCount
  static CommandCount m_command_curr_set;   // ����CommandCount
  
  void printCommandCount(const CommandCount& command_count);
#endif  // #if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT

};

//--------------------------------------------------------------------------
///	�C�V���l���璸�����A�g�����[�U�[�f�[�^�A�N�Z�b�T
//==========================================================================
enum CullFaceFlag
{
    CULL_FACE_FRONT,
    CULL_FACE_BACK,
    CULL_FACE_DISABLE
};

//! @name Material Optional Data �̃w�b�_�̃r�b�g�z�u�ł��B
//@{

static const u32 FRAGMENT_OP_BIT = (1 << 0);
static const u32 CULL_FACE_FLAG_SHIFT = 6;
static const u32 CULL_FACE_FLAG_MASK = (0x3 << CULL_FACE_FLAG_SHIFT);
static const u32 COMBINER_CMD_OFFSET_SHIFT = 8;
static const u32 COMBINER_CMD_OFFSET_MASK = (0xff << COMBINER_CMD_OFFSET_SHIFT);

//@}
//! @name Material Optional Data �̃t���O�����g�I�y���[�V�������̃r�b�g�z�u�ł��B
//@{

static const u32 STENCIL_CMD_OFFSET_SHIFT = 0;
static const u32 STENCIL_CMD_OFFSET_MASK = (0xffu << STENCIL_CMD_OFFSET_SHIFT);
static const u32 ALPHA_TEST_CMD_OFFSET_SHIFT = 8;
static const u32 ALPHA_TEST_CMD_OFFSET_MASK = (0xffu << ALPHA_TEST_CMD_OFFSET_SHIFT);
static const u32 DEPTH_COLOR_MASK_CMD_OFFSET_SHIFT = 16;
static const u32 DEPTH_COLOR_MASK_CMD_OFFSET_MASK = (0xffu << DEPTH_COLOR_MASK_CMD_OFFSET_SHIFT);
static const u32 BLEND_LOGIC_OP_CMD_OFFSET_SHIFT = 24;
static const u32 BLEND_LOGIC_OP_CMD_OFFSET_MASK = (0xffu << BLEND_LOGIC_OP_CMD_OFFSET_SHIFT);

//@}

//---------------------------------------------------------------------------
//! @brief Material Optional Data �̏����擾���邽�߂̃w���p�[�ł��B
//---------------------------------------------------------------------------
class ModHelper
{
    ModHelper();

public:

    //! MetaData ���t���O�����g�I�y���[�V�����f�[�^�������ďꍇ�� true ��Ԃ��܂��B
    static bool HasFragmentOpData(const nw::h3d::res::MetaData& metaData)
    {
        NW_ASSERT(metaData.dataCount > 0);
        return (static_cast<int*>(metaData.data)[0] & FRAGMENT_OP_BIT) && (metaData.dataCount > 1);
    }

    //! MetaData ���� CullFace �̐ݒ���擾���܂��B
    static CullFaceFlag GetCullFace(const nw::h3d::res::MetaData& metaData)
    {
        NW_ASSERT(metaData.dataCount > 0);
        return static_cast<CullFaceFlag>(
            (static_cast<int*>(metaData.data)[0] & CULL_FACE_FLAG_MASK) >> CULL_FACE_FLAG_SHIFT);
    }

    //! @name �R���o�C�i
    //@{

    //! �C�ӂ̒i�̃R���o�C�i�̃R�}���h���擾���܂��B
    static bit32* GetCombinerCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content, int combinerId)
    {
        NW_MINMAXLT_ASSERT(combinerId, 0, nw::h3d::Constants::COMBINER_QUANTITY);

        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 0, COMBINER_CMD_OFFSET_MASK, COMBINER_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        cmd += combinerId * MOD_COMBINER_STAGE_CMD_SIZE;
        NW_ASSERT((*(cmd + 1) & 0xffff) == GetCombinerTopAddress(combinerId));
        return cmd;
    }
    //! �C�ӂ̒i�̃R���o�C�i�̃R�}���h���� Source �ݒ���擾���܂��B
    static bit32 GetCombinerSource(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_SOURCE_OFFSET); }
    //! �C�ӂ̒i�̃R���o�C�i�̃R�}���h���� Operand �ݒ���擾���܂��B
    static bit32 GetCombinerOperand(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_OPERAND_OFFSET); }
    //! �C�ӂ̒i�̃R���o�C�i�̃R�}���h���� Combine �ݒ���擾���܂��B
    static bit32 GetCombinerCombine(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_COMBINE_OFFSET); }
    //! �C�ӂ̒i�̃R���o�C�i�̃R�}���h���� Const �ݒ���擾���܂��B
    static bit32 GetCombinerConst(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_CONST_OFFSET); }
    //! �C�ӂ̒i�̃R���o�C�i�̃R�}���h���� Scale �ݒ���擾���܂��B
    static bit32 GetCombinerScale(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_SCALE_OFFSET); }

    //! �R���o�C�i�o�b�t�@�̃R�}���h���擾���܂��B
    static bit32* GetCombinerBufferCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 0, COMBINER_CMD_OFFSET_MASK, COMBINER_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        cmd += MOD_COMBINER_BUFFER_OFFSET;
        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_TEX_ENV_BUFFER_INPUT);
        return cmd;
    }
    //! �R���o�C�i�o�b�t�@�̃R�}���h���� Input �ݒ���擾���܂��B
    static bit32 GetCombinerBufferInput(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_BUFFER_INPUT_OFFSET); }
    //! �R���o�C�i�o�b�t�@�̃R�}���h���� Color �ݒ���擾���܂��B
    static bit32 GetCombinerBufferColor(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_BUFFER_COLOR_OFFSET); }

    //@}
    //! @name �X�e���V���e�X�g
    //@{

    //! �X�e���V���e�X�g�̃R�}���h���擾���܂��B
    static bit32* GetStencilCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 1, STENCIL_CMD_OFFSET_MASK, STENCIL_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_STENCIL_TEST);
        return cmd;
    }

    //! �X�e���V���e�X�g�̃R�}���h���� Test �ݒ���擾���܂��B
    static bit32 GetStencilTest(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_STENCIL_TEST_OFFSET); }
    //! �X�e���V���e�X�g�̃R�}���h���� Operation �ݒ���擾���܂��B
    //!
    //! �X�e���V���e�X�g�������̏ꍇ�Ɏ擾�ł��܂���Bfnd::Optional �ŕԂ�l�̗L�����m�F���Ă��������B
    static nw::h3d::fnd::Optional<bit32> GetStencilOp(bit32* cmd)
    {
        if (cmd && ((*(cmd + MOD_STENCIL_OP_OFFSET + 1) & 0xffff) == PICA_REG_STENCIL_OP))
        {
            return *(cmd + MOD_STENCIL_OP_OFFSET);
        }
        return nw::h3d::fnd::Optional<bit32>();
    }

    //@}
    //! @name �A���t�@�e�X�g
    //@{

    //! �A���t�@�e�X�g�̃R�}���h���擾���܂��B
    static bit32* GetAlphaTestCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 1, ALPHA_TEST_CMD_OFFSET_MASK, ALPHA_TEST_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_FRAGOP_ALPHA_TEST);
        return cmd;
    }
    //! �A���t�@�e�X�g�̃R�}���h���� Test �ݒ���擾���܂��B
    static bit32 GetAlphaTest(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_ALPHA_TEST_OFFSET); }

    //@}
    //! @name �f�v�X�E�J���[�}�X�N
    //@{

    //! �f�v�X�E�J���[�}�X�N�̃R�}���h���擾���܂��B
    static bit32* GetDepthColorMaskCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 1, DEPTH_COLOR_MASK_CMD_OFFSET_MASK, DEPTH_COLOR_MASK_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_DEPTH_COLOR_MASK);
        return cmd;
    }
    //! �f�v�X�E�J���[�}�X�N�̃R�}���h���� Mask �ݒ���擾���܂��B
    static bit32 GetDepthColorMask(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_DEPTH_COLOR_MASK_TEST_OFFSET); }

    //@}
    //! @name �u�����h�E�_�����Z
    //@{

    //! �u�����h�E�_�����Z�̃R�}���h���擾���܂��B
    static bit32* GetBlendLogicOpCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 1, BLEND_LOGIC_OP_CMD_OFFSET_MASK, BLEND_LOGIC_OP_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_COLOR_OPERATION);
        return cmd;
    }
    //! �u�����h�E�_�����Z�̃R�}���h���� ColorOp �ݒ���擾���܂��B
    static bit32 GetColorOp(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COLOR_OP_OFFSET); }
    //! �u�����h�E�_�����Z�̃R�}���h���� BlendFunc �ݒ���擾���܂��B
    static bit32 GetBlendFunc(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_BLEND_FUNC_OFFSET); }
    //! �u�����h�E�_�����Z�̃R�}���h���� LogicOp �ݒ���擾���܂��B
    //!
    //! �u�����h�Ƙ_���`�R�������̏ꍇ�Ɏ擾�ł��܂���Bfnd::Optional �ŕԂ�l�̗L�����m�F���Ă��������B
    static nw::h3d::fnd::Optional<bit32> GetLogicOp(bit32* cmd)
    {
        if (cmd && ((*(cmd + MOD_LOGIC_OP_OFFSET + 1) & 0xffff) == PICA_REG_LOGIC_OP))
        {
            return *(cmd + MOD_LOGIC_OP_OFFSET);
        }
        return nw::h3d::fnd::Optional<bit32>();
    }
    //! �u�����h�E�_�����Z�̃R�}���h���� BlendColor �ݒ���擾���܂��B
    //!
    //! �u�����h�������̏ꍇ�Ɏ擾�ł��܂���Bfnd::Optional �ŕԂ�l�̗L�����m�F���Ă��������B
    static nw::h3d::fnd::Optional<bit32> GetBlendColor(bit32* cmd)
    {
        if (cmd && ((*(cmd + MOD_BLEND_COLOR_OFFSET + 1) & 0xffff) == PICA_REG_BLEND_COLOR))
        {
            return *(cmd + MOD_BLEND_COLOR_OFFSET);
        }
        return nw::h3d::fnd::Optional<bit32>();
    }

    //@}

private:
    enum
    {
        MOD_INVALID_OFFSET = 255,
        MOD_COMBINER_STAGE_CMD_SIZE = 6,
        MOD_COMBINER_BUFFER_OFFSET = MOD_COMBINER_STAGE_CMD_SIZE * nw::h3d::Constants::COMBINER_QUANTITY,

        MOD_COMBINER_SOURCE_OFFSET = 0,
        MOD_COMBINER_OPERAND_OFFSET = 2,
        MOD_COMBINER_COMBINE_OFFSET = 3,
        MOD_COMBINER_CONST_OFFSET = 4,
        MOD_COMBINER_SCALE_OFFSET = 5,

        MOD_COMBINER_BUFFER_INPUT_OFFSET = 0,
        MOD_COMBINER_BUFFER_COLOR_OFFSET = 2,

        MOD_STENCIL_TEST_OFFSET = 0,
        MOD_STENCIL_OP_OFFSET = 2,
        MOD_ALPHA_TEST_OFFSET = 0,
        MOD_DEPTH_COLOR_MASK_TEST_OFFSET = 0,
        MOD_COLOR_OP_OFFSET = 0,
        MOD_BLEND_FUNC_OFFSET = 2,
        MOD_LOGIC_OP_OFFSET = 4,
        MOD_BLEND_COLOR_OFFSET = 6
    };

    static bit32* GetCmd(
        const nw::h3d::res::MetaData& metaData,
        const nw::h3d::res::MaterialContent* content,
        int dataIdx,
        u32 mask,
        u32 shift)
    {
        NW_NULL_ASSERT(content);
        int idx = (static_cast<int*>(metaData.data)[dataIdx] & mask) >> shift;
        if (idx < 0 || MOD_INVALID_OFFSET <= idx) { return NULL; }

        return content->paramCmdSrc + idx;
    }

    static u16 GetCombinerTopAddress(int combinerId) { return PICA_REG_TEX_ENV0 + combinerId * 8 + ((combinerId > 3) ? 0x10 : 0); }

};



}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DEX_H__

