#ifndef __GFL_G3DMODEL_H__
#define __GFL_G3DMODEL_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dModel.h
 *	@brief  3D���f��
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


/*
[�R���X�g���C���̗p��]
�o�g������Ɏ������l�Ԃ��v�������ׂ�B
�����Ă���l�Ԃ�human�Ahuman�̎��hand�Ahand�Ɏ������o�g����baton�ƌĂԂ��Ƃɂ���B
���f����human�A��̃{�[����hand�A���f�����̃{�[���ɂ��������郂�f����baton�ƂȂ�B
*/


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dAnimationSlot.h>
#include <grp/g3d/gfl_G3dAnimationManager.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dUserData.h>

#include <math/gfl_MathAABB.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�^�錾
 */
//-----------------------------------------------------------------------------
typedef void (*ModelCalculateCallbackFunction)(Model* model, void* work);




//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------


namespace internal {

//-------------------------------------
///	�R�[���o�b�N�N���X�̑O���錾
//=====================================
class CallbackConstraintOfSkeleton;
class CallbackConstraintOfTransformNode;

}  // namespace internal


//-------------------------------------
///	3D���f���N���X
//=====================================
class Model : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Model);


  // ���L�N���X��RenderQueue�֘A��private�����o�֐��ւ̃A�N�Z�X��������
  friend class Scene;
  friend class RenderSystem;
  friend class ShadowSystem;
  friend class SceneNode;


public:
  // �}�e���A���̃o�b�t�@�̐����I�v�V����
  enum MaterialBufferOption
  {
    MATERIAL_BUFFER_OPTION_NOT_USE,                             // �o�b�t�@��Ǝ��Ɏ����Ȃ�
    MATERIAL_BUFFER_OPTION_MATERIAL_COLOR,                      // �}�e���A���J���[�̃o�b�t�@��Ǝ��Ɏ���(�}�e���A���J���[�A�R���X�^���g�J���[�A���̑��J���[�̃o�b�t�@)
    MATERIAL_BUFFER_OPTION_MATERIAL_COLOR_SHADING_PARAMETER,    // �V�F�[�f�B���O�p�����[�^�̃o�b�t�@��Ǝ��Ɏ���(MATERIAL_COLOR | �t�H�O�̃o�b�t�@)
    MATERIAL_BUFFER_OPTION_ANIMATABLE_MATERIAL,                 // �A�j���[�V��������}�e���A���̃o�b�t�@��Ǝ��Ɏ���(�}�e���A���A�j���[�V�����̃o�b�t�@)
    MATERIAL_BUFFER_OPTION_ALL_MATERIAL,                        // �S�Ẵ}�e���A���̃o�b�t�@��Ǝ��Ɏ���
    MATERIAL_BUFFER_OPTION_MAX
  };

  // �ݒ���e
  struct Description
  {
    MaterialBufferOption  material_buffer_option;

    Model* material_share_source_model;              // �}�e���A���̋��L���̃��f��
        // material_share_source_model��NULL�łȂ��Ƃ��Amaterial_buffer_option��MATERIAL_BUFFER_OPTION_NOT_USE�łȂ���΂Ȃ�Ȃ��B
        // gfx_SceneBuilder.h BufferOption, gfx_SkeletalModel.h BufferOption�Q�l�B
    Model* mesh_node_visibility_share_source_model;  // ���b�V���m�[�h�r�W�r���e�B�̋��L���̃��f��
   
    s32  max_callbacks;             // �Ǘ��ł���R�[���o�b�N�̍ő吔

    u32  constraint_human_baton_num_max;  // human�ɍS�������baton�̍ő吔
    u32  constraint_hand_num_max;         // ����������hand�̍ő吔(���̃��f���̃{�[���̌��ȉ�)
    u32  constraint_hand_baton_num_max;   // 1��hand�ɍS�������baton�̍ő吔
                                          // (���vconstraint_hand_num_max*constraint_hand_baton_num_max�̃V�[���m�[�h���{�[���ɂ���������)

    u32  animation_slot_num;        // �A�j���[�V���������蓖�Ă�X���b�g�̌�
    u32  animation_blend_unit_num;  // �X���b�g���ŃA�j���[�V�����̃u�����h���s���ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł��X���b�g����1���j�b�g������܂�)
    u32  animation_num_in_unit;     // ���j�b�g���ŕ�ԃu�����h���Ȃ���A�j���[�V������ύX����ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł����j�b�g����1����܂�)

    u32  calculate_callback_num_max;    // �v�Z�����ɂČĂяo�����R�[���o�b�N�̍ő吔

    Description(void)
      : material_buffer_option(MATERIAL_BUFFER_OPTION_NOT_USE),
        material_share_source_model(NULL),
        mesh_node_visibility_share_source_model(NULL),
        max_callbacks(4),
        constraint_human_baton_num_max(0),
        constraint_hand_num_max(0),
        constraint_hand_baton_num_max(0),
        animation_slot_num(0),
        animation_blend_unit_num(0),
        animation_num_in_unit(0),
        calculate_callback_num_max(1)
    {
      // �������Ȃ�
    }
  };
  static void InitializeDescriptionForDefault(
      Description*  description
  );

#if 0
    // �t���O��|(or)�Ōq���̂ł̓��[�U�ɐe�؂ł͂Ȃ��C�������̂ŁA�r�b�g�t�B�[���h�Őݒ肵�Ă��炤���Ƃɂ����B
    unsigned material_buffer_option_shader_parameter          : 1;  // �V�F�[�_�[�p�����[�^�𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_shading_parameter         : 1;  // �V�F�[�f�B���O�p�����[�^�𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_material_color            : 1;  // �}�e���A���J���[�𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_rasterization             : 1;  // ���X�^���C�[�[�V�����𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_texture_coordinator       : 1;  // �e�N�X�`���R�[�f�B�l�[�^�𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_texture_mapper            : 1;  // �e�N�X�`���}�b�p�[�𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_procedural_texture_mapper : 1;  // �������ł��B
    unsigned material_buffer_option_fragment_lighting         : 1;  // �t���O�����g���C�e�B���O�𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_fragment_lighting_table   : 1;  // �t���O�����g���C�e�B���O�e�[�u���𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_texture_combiner          : 1;  // �e�N�X�`���R���o�C�i�𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_alpha_test                : 1;  // �A���t�@�e�X�g�𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_fragment_operation        : 1;  // �t���O�����g�I�y���[�V�����𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    unsigned material_buffer_option_scene_environment         : 1;  // �V�[�����𗘗p����Ȃ�΁A�P�ɂȂ�܂��B
    
    // �r�b�g�t�B�[���h��1���w�肵�Ă����̂ł̓��[�U�ɐe�؂ł͂Ȃ��C�������̂ŁA�悭�g�����̂�enum�ŗp�ӂ��Ă������Ƃɂ����B
#endif


public:
  // �}�e���A���p�̒萔
  enum
  {
    MATERIAL_INDEX_NOT_FOUND    = -1  // �w�肵�����O�����}�e���A�������݂��Ȃ��ꍇ�̃}�e���A���̃C���f�b�N�X
  };
  // �}�e���A���̃J�����O���[�h
  enum MaterialCullingMode
  {
    MATERIAL_CULLING_MODE_FRONT     = nw::gfx::res::ResRasterization::CULLINGMODE_FRONT,  // �\�ʃJ�����O
    MATERIAL_CULLING_MODE_BACK      = nw::gfx::res::ResRasterization::CULLINGMODE_BACK,   // ���ʃJ�����O
    MATERIAL_CULLING_MODE_NONE      = nw::gfx::res::ResRasterization::CULLINGMODE_NONE    // ���ʕ\��
  };
  // �}�e���A���̃e�X�g�֐�
  enum MaterialTestFunction  // nw::gfx::res::ResStencilOperation::TestFunc��nw::gfx::res::ResDepthOperation::TestFunc, nw::gfx::res::ResAlphaTest::TestFunc�ƒl�����킹�Ă�������
  {                          // �l�Ƃ͎Q�ƒl��f�v�X�o�b�t�@�̒l�̂���
    MATERIAL_TEST_FUNCTION_NEVER,        // ���ׂĕs�ʉ߂ł��B
    MATERIAL_TEST_FUNCTION_ALWAYS,       // ���ׂĒʉ߂ł��B
    MATERIAL_TEST_FUNCTION_EQUAL,        // �l�Ɠ�������Βʉ߂��܂��B
    MATERIAL_TEST_FUNCTION_NOTEQUAL,     // �l�Ɠ������Ȃ���Βʉ߂��܂��B
    MATERIAL_TEST_FUNCTION_LESS,         // �l��菬������Βʉ߂��܂��B
    MATERIAL_TEST_FUNCTION_LEQUAL,       // �l�ȉ��ł���Βʉ߂��܂��B
    MATERIAL_TEST_FUNCTION_GREATER,      // �l���傫����Βʉ߂��܂��B
    MATERIAL_TEST_FUNCTION_GEQUAL        // �l�ȏ�ł���Βʉ߂��܂��B
  };
  // �}�e���A���̃X�e���V���e�X�g��̏���
  enum MaterialStencilOperation
  {
    MATERIAL_STENCIL_OPERATION_KEEP           = nw::gfx::res::ResStencilOperation::STENCILOP_KEEP,           // ���݂̒l��ێ����܂��B
    MATERIAL_STENCIL_OPERATION_ZERO           = nw::gfx::res::ResStencilOperation::STENCILOP_ZERO,           // 0�ɒu�����܂��B 
    MATERIAL_STENCIL_OPERATION_REPLACE        = nw::gfx::res::ResStencilOperation::STENCILOP_REPLACE,        // �Q�ƒl�ɒu�����܂��B 
    MATERIAL_STENCIL_OPERATION_INCREASE       = nw::gfx::res::ResStencilOperation::STENCILOP_INCREASE,       // ���������܂��B�ɒl�����̓N�����v�ł��B
    MATERIAL_STENCIL_OPERATION_DECREASE       = nw::gfx::res::ResStencilOperation::STENCILOP_DECREASE,       // ���������܂��B�ɒl�����̓N�����v�ł��B
    MATERIAL_STENCIL_OPERATION_INVERT         = nw::gfx::res::ResStencilOperation::STENCILOP_INVERT,         // �r�b�g�P�ʂŔ��]�����܂��B 
    MATERIAL_STENCIL_OPERATION_INCREASE_WRAP  = nw::gfx::res::ResStencilOperation::STENCILOP_INCREASE_WRAP,  // ���������܂��B�ɒl�����͉�荞�݂ł��B
    MATERIAL_STENCIL_OPERATION_DECREASE_WRAP  = nw::gfx::res::ResStencilOperation::STENCILOP_DECREASE_WRAP   // ���������܂��B�ɒl�����͉�荞�݂ł��B 
  };
  // �}�e���A���̃u�����_�[�̃u�����h���[�h
#if 1
  enum MaterialBlendMode
  {
    MATERIAL_BLEND_MODE_NOT_USE       ,  // �g�p���܂���B
    MATERIAL_BLEND_MODE_BLEND         ,  // �u�����h�����ł��B           // MATERIAL_BLEND_MODE_BLEND�ɂ��Ă�MATERIAL_BLEND_MODE_SEPARATE_BLEND���ݒ肳��܂��B
    MATERIAL_BLEND_MODE_SEPARATE_BLEND,  // �Z�p���[�g�u�����h�����ł��B
    MATERIAL_BLEND_MODE_LOGIC            // �_�����Z�ł��B
  };
#else
  /*
  NintendoWare\CTR\include\nw/gfx/res/gfx_ResMaterial.h
  NW_DEPRECATED_FUNCTION(void SetMode(Mode value))
  �֐����p�~�ɂȂ����̂Œl���g��Ȃ����Ƃɂ���
   */
  enum MaterialBlendMode
  {
    MATERIAL_BLEND_MODE_NOT_USE          = nw::gfx::res::ResBlendOperation::MODE_NOT_USE,         // �g�p���܂���B 
    MATERIAL_BLEND_MODE_BLEND            = nw::gfx::res::ResBlendOperation::MODE_BLEND,           // �u�����h�����ł��B
    MATERIAL_BLEND_MODE_SEPARATE_BLEND   = nw::gfx::res::ResBlendOperation::MODE_SEPARATE_BLEND,  // �Z�p���[�g�u�����h�����ł��B
    MATERIAL_BLEND_MODE_LOGIC            = nw::gfx::res::ResBlendOperation::MODE_LOGIC            // �_�����Z�ł��B
  };
#endif
  // �}�e���A���̃u�����_�[�̘_�����Z�̉��Z���@
  enum MaterialBlendLogicOperation
  {
    MATERIAL_BLEND_LOGIC_OPERATION_CLEAR         = nw::gfx::res::ResBlendOperation::LOGICOP_CLEAR,          // ���ׂẴr�b�g�l�� 0 �ɂ��܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_AND           = nw::gfx::res::ResBlendOperation::LOGICOP_AND,            // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���ς��Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_AND_REVERSE   = nw::gfx::res::ResBlendOperation::LOGICOP_AND_REVERSE,    // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l�̘_���ς��Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_COPY          = nw::gfx::res::ResBlendOperation::LOGICOP_COPY,           // �\�[�X�J���[�̃r�b�g�l���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_SET           = nw::gfx::res::ResBlendOperation::LOGICOP_SET,            // ���ׂẴr�b�g�l�� 1 �ɂ��܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_COPY_INVERTED = nw::gfx::res::ResBlendOperation::LOGICOP_COPY_INVERTED,  // �\�[�X�J���[�̔��]�r�b�g�l���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_NOOP          = nw::gfx::res::ResBlendOperation::LOGICOP_NOOP,           // �f�B�X�e�B�l�[�V�����J���[�̃r�b�g�l���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_INVERT        = nw::gfx::res::ResBlendOperation::LOGICOP_INVERT,         // �f�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_NAND          = nw::gfx::res::ResBlendOperation::LOGICOP_NAND,           // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���ς̔��]�r�b�g�l���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_OR            = nw::gfx::res::ResBlendOperation::LOGICOP_OR,             // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���a���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_NOR           = nw::gfx::res::ResBlendOperation::LOGICOP_NOR,            // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���a�̔��]�r�b�g�l���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_XOR           = nw::gfx::res::ResBlendOperation::LOGICOP_XOR,            // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔r���I�_���a���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_EQUIV         = nw::gfx::res::ResBlendOperation::LOGICOP_EQUIV,          // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔r���I�_���a�̔��]�r�b�g�l���Ƃ�܂��B
    MATERIAL_BLEND_LOGIC_OPERATION_AND_INVERTED  = nw::gfx::res::ResBlendOperation::LOGICOP_AND_INVERTED,   // �\�[�X�J���[�̔��]�r�b�g�l�ƁA�f�B�X�e�B�l�[�V�����J���[�̘_���ς��Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_OR_REVERSE    = nw::gfx::res::ResBlendOperation::LOGICOP_OR_REVERSE,     // �\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l�̘_���a���Ƃ�܂��B 
    MATERIAL_BLEND_LOGIC_OPERATION_OR_INVERTED   = nw::gfx::res::ResBlendOperation::LOGICOP_OR_INVERTED     // �\�[�X�J���[�̔��]�r�b�g�l�ƁA�f�B�X�e�B�l�[�V�����J���[�̘_���a���Ƃ�܂��B 
  };
  // �}�e���A���̃u�����_�[�̓��o�̗͂v�f
  enum MaterialBlendFactor
  {
    MATERIAL_BLEND_FACTOR_ZERO                     = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ZERO,                      // �w��̐����Ɂu 0 �v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_ONE                      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE,                       // �w��̐����Ɂu 1 �v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_SRC_COLOR                = nw::gfx::res::ResBlendOperation::BLENDFACTOR_SRC_COLOR,                 // �w��̐����Ɂu�\�[�X�J���[�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_SRC_COLOR,       // �w��̐����Ɂu1 - �\�[�X�J���[�v �𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_DST_COLOR                = nw::gfx::res::ResBlendOperation::BLENDFACTOR_DST_COLOR,                 // �w��̐����Ɂu�f�X�e�B�l�[�V�����J���[�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_DST_COLOR      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_DST_COLOR,       // �w��̐����Ɂu 1 - �f�X�e�B�l�[�V�����J���[�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_SRC_ALPHA                = nw::gfx::res::ResBlendOperation::BLENDFACTOR_SRC_ALPHA,                 // �w��̐����Ɂu�\�[�X�A���t�@�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_SRC_ALPHA,       // �w��̐����Ɂu 1 - �\�[�X�A���t�@�v �𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_DST_ALPHA                = nw::gfx::res::ResBlendOperation::BLENDFACTOR_DST_ALPHA,                 // �w��̐����Ɂu�f�X�e�B�l�[�V�����A���t�@�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_DST_ALPHA,       // �w��̐����Ɂu 1 - �f�X�e�B�l�[�V�����A���t�@�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_CONSTANT_COLOR           = nw::gfx::res::ResBlendOperation::BLENDFACTOR_CONSTANT_COLOR,            // �w��̐����Ɂu�R���X�^���g�J���[(�u�����h�J���[)�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR,  // �w��̐����Ɂu 1 - �R���X�^���g�J���[�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_CONSTANT_ALPHA           = nw::gfx::res::ResBlendOperation::BLENDFACTOR_CONSTANT_ALPHA,            // �w��̐����Ɂu�R���X�^���g�A���t�@(�u�����h�A���t�@)�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_CONSTANT_ALPHA,  // �w��̐����Ɂu 1 - �R���X�^���g�A���t�@�v�𗘗p���܂��B 
    MATERIAL_BLEND_FACTOR_SRC_ALPHA_SATURATE       = nw::gfx::res::ResBlendOperation::BLENDFACTOR_SRC_ALPHA_SATURATE,        // �w��̐����Ɂu MIN( �\�[�X�A���t�@ , 1 - �f�X�e�B�l�[�V�����A���t�@ ) �v�𗘗p���܂��B 
  };
  // �}�e���A���̃u�����_�[�̃u�����h��
  enum MaterialBlendEquation
  {
    MATERIAL_BLEND_EQUATION_FUNC_ADD              = nw::gfx::res::ResBlendOperation::BLENDEQ_FUNC_ADD,               // �\�[�X�ƃf�X�e�B�l�[�V���������Z���܂��B 
    MATERIAL_BLEND_EQUATION_FUNC_SUBTRACT         = nw::gfx::res::ResBlendOperation::BLENDEQ_FUNC_SUBTRACT,          // �\�[�X����f�X�e�B�l�[�V���������Z���܂��B
    MATERIAL_BLEND_EQUATION_FUNC_REVERSE_SUBTRACT = nw::gfx::res::ResBlendOperation::BLENDEQ_FUNC_REVERSE_SUBTRACT,  // �f�X�e�B�l�[�V��������\�[�X�����Z���܂��B
    MATERIAL_BLEND_EQUATION_MIN                   = nw::gfx::res::ResBlendOperation::BLENDEQ_MIN,                    // �\�[�X�ƃf�X�e�B�l�[�V�����̂�菬�������ł��B
    MATERIAL_BLEND_EQUATION_MAX                   = nw::gfx::res::ResBlendOperation::BLENDEQ_MAX                     // �\�[�X�ƃf�X�e�B�l�[�V�����̂��傫�����ł��B
  };
  // �}�e���A���̕`�惌�C���[
  enum MaterialTranslucencyKind
  {
    MATERIAL_TRANSLUCENCY_KIND_LAYER0       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER0,       // ���C���[ 0 ( �s�������b�V���Ƃ��ĕ`�� ) �ł��B 
    MATERIAL_TRANSLUCENCY_KIND_LAYER1       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER1,       // ���C���[ 2 ( ���������b�V���Ƃ��ĕ`�� ) �ł��B 
    MATERIAL_TRANSLUCENCY_KIND_LAYER2       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER2,       // ���C���[ 3 ( ���Z�������b�V���Ƃ��ĕ`�� ) �ł� 
    MATERIAL_TRANSLUCENCY_KIND_LAYER3       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER3,       // ���C���[ 4 ( ���Z�������b�V���Ƃ��ĕ`�� ) �ł� 
    MATERIAL_TRANSLUCENCY_KIND_OPAQUE       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_OPAQUE,       // TRANSLUCENCY_KIND_LAYER0 �Ɠ����ł��B 
    MATERIAL_TRANSLUCENCY_KIND_TRANSLUCENT  = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_TRANSLUCENT,  // TRANSLUCENCY_KIND_LAYER1 �Ɠ����ł��B 
    MATERIAL_TRANSLUCENCY_KIND_SUBTRACTIVE  = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_SUBTRACTIVE,  // TRANSLUCENCY_KIND_LAYER2 �Ɠ����ł��B 
    MATERIAL_TRANSLUCENCY_KIND_ADDITIVE     = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_ADDITIVE,     // TRANSLUCENCY_KIND_LAYER3 �Ɠ����ł��B 
    MATERIAL_TRANSLUCENCY_KIND_END          = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_END           // �Ō�̕`�惌�C���[�ł��B                       
  };
  // �}�e���A���̃e�N�X�`���̌J��Ԃ��ݒ�
  enum MaterialTextureWrap
  {
    MATERIAL_TEXTURE_WRAP_CLAMP_TO_EDGE    = nw::gfx::res::ResStandardTextureSampler::WRAP_CLAMP_TO_EDGE,    // �e�N�X�`���̒[�̃s�N�Z���������L�΂��܂��B 
    MATERIAL_TEXTURE_WRAP_CLAMP_TO_BORDER  = nw::gfx::res::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER,  // �e�N�X�`���̒[�Ń{�[�_�[�J���[�������L�΂��܂��B
    MATERIAL_TEXTURE_WRAP_REPEAT           = nw::gfx::res::ResStandardTextureSampler::WRAP_REPEAT,           // �e�N�X�`���̒[�ŌJ��Ԃ��܂��B 
    MATERIAL_TEXTURE_WRAP_MIRRORED_REPEAT  = nw::gfx::res::ResStandardTextureSampler::WRAP_MIRRORED_REPEAT   // �e�N�X�`���̒[�Ŕ��]���Ȃ���J��Ԃ��܂��B 
  };

public:
  // ���b�V���p�̒萔
  enum
  {
    MESH_INDEX_NOT_FOUND    = -1  // �w�肵�����O�������b�V�������݂��Ȃ��ꍇ�̃��b�V���̃C���f�b�N�X
  };

public:
  // �{�[���p�̒萔
  enum
  {
        // BONE_INDEX_NOT_FOUND��BONE_INDEX_NONE���C���f�b�N�X���Ȃ����Ƃ������Ă��邾���Ȃ̂�-1�ł����B
    BONE_INDEX_NOT_FOUND  = -1,  // �w�肵�����O�����{�[�������݂��Ȃ��ꍇ�̃{�[���̃C���f�b�N�X
    BONE_INDEX_NONE       = -1,  // �C���f�b�N�X�Ȃ�
    BONE_INDEX_IGNORE     = -2   // �������Ă����{�[��
  };

public:
  // ���[�U�f�[�^�p�̒萔
  enum
  {
    USER_DATA_INDEX_NOT_FOUND    = -1  // �w�肵�����O�������[�U�f�[�^�����݂��Ȃ��ꍇ�̃��[�U�f�[�^�̃C���f�b�N�X
  };

public:
#if 0
  //������
  // �v�Z����邩�ǂ���
  enum CalculateState
  {
    CALCULATE_STATE_NO,  // �v�Z����Ȃ�
    CALCULATE_STATE_OK   // �v�Z�����
      // �ꕔ�v�Z������ԂȂǂ�����ǉ������\��
  };
#endif
  // �`�悳��邩�ǂ���
  enum DrawState
  {
    DRAW_STATE_NO,  // �`�悳��Ȃ�
    DRAW_STATE_OK   // �`�悳���
  };

private:
  // ���L�^�C�v
  enum ShareType
  {
    SHARE_TYPE_NONE,
    SHARE_TYPE_RESOURCE,           // ���\�[�X���琶�����A���L���Ă��Ȃ�(���L���ꋤ�L���ɂȂ����狤�L�^�C�v���ς��܂�)
    SHARE_TYPE_SHARE_SOURCE,       // ���L���Ă���A���L���ł��錠���������Ă���  // @note NW4C�̃f���Ƀ}�e���A�����L�̃T���v�����邩�H
    SHARE_TYPE_SHARE_DESTINATION   // ���L���Ă���A���L�����Q�Ƃ��Ă���
  };

private:
/*
�����ւ�
�����ւ���replace�ƌĂԂ��Ƃɂ���B
���郂�f���̈ꕔ���Ƃ��ĕʂ̃��f����\������悤�ȏꍇ�Ɏg�p����@�\�B�g�p��Ƃ��Ē����ւ�����������B

[�p��]
��{���f��  �S���J�X�^�}�C�Y�����Ă��Ȃ����f���Bbase�ƌĂԂ��Ƃɂ���B��F�S�g�B
�������f��  ��{���f���̈ꕔ���������ւ��邽�߂̃��f���Bpart�ƌĂԂ��Ƃɂ���B��F���A�Y�{���B
�ǉ����f��  ���̃��f���ɂ������郂�f���Badd�ƌĂԂ��Ƃɂ���BSceneNode::SetupConstraint�ł������郂�f���B��F�C�������O�A�r���v�B

[�v�Z�̏���]
(1) part���f����p�̃A�j��(�X�P���^���A�j���ł��}�e���A���A�j���ł����ł�����)�̌v�Z������B
(2) base���f���̃A�j��(�X�P���^���A�j���̂�)�̌v�Z���ʂ��Apart���f���ɏ㏑������B
(3) part���f���ɂ͂��邪base���f���ɂ͂Ȃ��{�[���̈ʒu�����̂悤�ɏC������(�X�P���^���A�j���̂�)�B
    base���f���Ő�c�ɂ�����{�[���̃g�����X�t�H�[���ŁApart���f���ɂ�������{�[���̈ʒu��e���珇�ԂɏC������B
*/
  // �����ւ��p�̃{�[���̃f�[�^
  struct ReplaceBoneData
  {
    s32 base_index;           // ����Ɠ����{�[����base�ł̃C���f�b�N�X
    s32 base_ancestor_index;  // base_index��gfl::grp::g3d::Model::BONE_INDEX_NONE�̂Ƃ��̂ݗL���Ȓl�������Ă���A�����base�̃{�[���̒��Ő�c�ɓ�����{�[���̃C���f�b�N�X
                              // base_index��0�ȏ�̒l��������gfl::grp::g3d::Model::BONE_INDEX_IGNORE�̂Ƃ��Abase_ancestor_index�ɂ͉��������Ă��邩�s��
        // (base_index==gfl::grp::g3d::Model::BONE_INDEX_NONE&&base_ancestor_index==gfl::grp::g3d::Model::BONE_INDEX_NONE)�̂Ƃ��Abase��human�g�b�v�ɒǏ]����
        
    ReplaceBoneData(void)
      : base_index(gfl::grp::g3d::Model::BONE_INDEX_NONE),
        base_ancestor_index(gfl::grp::g3d::Model::BONE_INDEX_NONE)
    {}
  };

/*
�̂̓N���X���N���X�Ƃ���AnimationSlot��p�ӂ��Ă����B
private:
  //-------------------------------------
  ///	�A�j���[�V���������蓖�Ă�X���b�g
  //=====================================
  class AnimationSlot
  {
  };
*/

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  Model(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~Model();
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
   *  @param[in]     resource           ���\�[�X
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     description        �ݒ���e
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource,
      Description*                      description  = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �X�P���g�������L�������f���̐���
   *
   *  @param[in]     allocator                   �A���P�[�^
   *  @param[in]     device_allocator            �f�o�C�X�������̃A���P�[�^
   *  @param[in]     resource                    ���\�[�X
   *  @param[in]     index_in_resource           ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     skeleton_share_source_model �X�P���g���̋��L���̃��f��
   *  @param[in]     description                 �ݒ���e
   */
  //-----------------------------------------------------------------------------
  virtual void CreateSkeletalModelSharingSkeleton(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource,
      Model*                            skeleton_share_source_model,
      Description*                      description  = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z����
   */
  //-----------------------------------------------------------------------------
  virtual void Calculate(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X���ێ����Ă���nw::gfx::Model*���擾
   *
   *  @retval        nw::gfx::Model*    ���̃N���X���ێ����Ă���nw::gfx::Model
   */
  //-----------------------------------------------------------------------------
  nw::gfx::Model*  GetNwModel(void) const  {  return nw::ut::DynamicCast<nw::gfx::Model*>( GetNwSceneNode() );  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���W�ϊ�
   */
  //-----------------------------------------------------------------------------
  void SetTransformMatrix(const gfl::math::MTX34& m);
  void SetTransformMatrix(
      f32 m00,
      f32 m01,
      f32 m02,
      f32 m03,
      f32 m10,
      f32 m11,
      f32 m12,
      f32 m13,
      f32 m20,
      f32 m21,
      f32 m22,
      f32 m23
  );
  void GetTransformMatrix(gfl::math::MTX34* m) const;  // �����̃X�P�[���͊|�����Ă��炸�A�e�̃X�P�[���͊|�����Ă��܂��B
  gfl::math::MTX34 GetTransformMatrix(void) const;  // �����̃X�P�[���͊|�����Ă��炸�A�e�̃X�P�[���͊|�����Ă��܂��B
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���s�ړ�
   */
  //-----------------------------------------------------------------------------
  void SetTranslate(f32 x, f32 y, f32 z);
  void SetTranslate(const gfl::math::VEC3& t);
  void GetTranslate(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetTranslate(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��]
   *                 �P�ʂ̓��W�A���B
   */
  //-----------------------------------------------------------------------------
  void SetRotateXYZ(f32 x, f32 y, f32 z);
  void SetRotateXYZ(const gfl::math::VEC3& t);
  void GetRotateXYZ(gfl::math::VEC3* t) const;  // �y���ӁzSetRotateXYZ�œn���ꂽ�l��Ԃ������ł��BSetRotateXYZ�ȊO�ŉ�]���w�肵�Ă����ꍇ��
  gfl::math::VEC3 GetRotateXYZ(void) const;     //         �����ڂƍ���Ȃ��l���Ԃ��Ă��܂��B
  template <typename TMatrix> void SetRotateMatrix(const TMatrix& m)
  {
    nw::gfx::TransformNode* transform_node = GetNwTransformNode();
    transform_node->Transform().SetRotateMatrix(m);
  }
  void SetRotateMatrix(
      f32 m00,
      f32 m01,
      f32 m02,
      f32 m10,
      f32 m11,
      f32 m12,
      f32 m20,
      f32 m21,
      f32 m22
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �X�P�[��
   */
  //-----------------------------------------------------------------------------
  void SetScale(f32 x, f32 y, f32 z);
  void SetScale(const gfl::math::VEC3& t);
  void GetScale(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetScale(void) const;

#if 0
//  ���̃��f���Ɉڂ�����̓��삪�悭�Ȃ��̂ŁA���̊֐��͂����ƃe�X�g���Ă���łȂ��Ǝg���Ȃ��B
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �X�P���g���̋��L���ł��錠���𑼂̃��f���Ɉڂ�
   *                 this���X�P���g���̋��L���ł���A
   *                 skeleton_share_new_source_model�����̃X�P���g�������L���Ă��郂�f���ł���K�v������܂��B
   *
   *  @param[in]     skeleton_share_new_source_model  �V�����X�P���g���̋��L���̌�����^�����郂�f��
   */
  //-----------------------------------------------------------------------------
  void ChangeSkeletonShareSource(Model* skeleton_share_new_source_model);
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�揇
   *                 �v���O���}�������ݒ�ł��郂�f���̃��C���[ID
   *                 �ŏ��ɕ`�� 0 <= u8 <= 127 �Ō�ɕ`��
   *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
   *
   *                 **** Model��Particle�ɂ���S�������ɕ��ׂ�����ԍ��ł��B�f�t�H���g0000000�ł��B ****
   */
  //-----------------------------------------------------------------------------
  void SetModelLayerId(u8 model_layer_id);
  u8 GetModelLayerId(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A��
   */
  //-----------------------------------------------------------------------------
  s32 GetMaterialCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���̃C���f�b�N�X�ԍ����擾����
   *
   *  @param[in]     key  �}�e���A���̖��O
   * 
   *  @retval        s32    �}�e���A���̃C���f�b�N�X�ԍ�
   *                        �w�肵�����O�����}�e���A�������݂��Ȃ��ꍇ�AMATERIAL_INDEX_NOT_FOUND��Ԃ�
   */
  //-----------------------------------------------------------------------------
  s32 GetMaterialIndex(const char* key) const;
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���J���[
   *                 u8�̂Ƃ���0����&�����A255����&�s����
   *                 f32�̂Ƃ���0.0f����&�����A1.0f����&�s����
   */
  //-----------------------------------------------------------------------------
  // �f�B�t���[�Y�J���[
  void SetMaterialColorDiffuse(s32 material_index, f32 r, f32 g, f32 b);
  void SetMaterialColorDiffuse(s32 material_index, f32 r, f32 g, f32 b, f32 a);
  void SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorF32& c);
  void SetMaterialColorDiffuse(s32 material_index, u8 r, u8 g, u8 b);
  void SetMaterialColorDiffuse(s32 material_index, u8 r, u8 g, u8 b, u8 a);
  void SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetMaterialColorDiffuse(s32 material_index) const;
  void GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorF32* c) const;
  void GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorU8*  c) const;

  // �R���X�^���g�J���[
  void SetMaterialColorConstant(s32 material_index, s32 no, f32 r, f32 g, f32 b);
  void SetMaterialColorConstant(s32 material_index, s32 no, f32 r, f32 g, f32 b, f32 a);
  void SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorF32& c);
  void SetMaterialColorConstant(s32 material_index, s32 no, u8 r, u8 g, u8 b);
  void SetMaterialColorConstant(s32 material_index, s32 no, u8 r, u8 g, u8 b, u8 a);
  void SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetMaterialColorConstant(s32 material_index, s32 no) const;
  void GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorF32* c) const;
  void GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorU8*  c) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A�����e�����󂯂�t�H�O
   */
  //-----------------------------------------------------------------------------
  void SetMaterialFogEnable(s32 material_index, bool enable);
  bool IsMaterialFogEnable(s32 material_index) const;
  void SetMaterialFogIndex(s32 material_index, s32 index);
  s32  GetMaterialFogIndex(s32 material_index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A�����e�����󂯂郉�C�g
   */
  //-----------------------------------------------------------------------------
  //void SetMaterialFragmentLightEnable(s32 material_index, bool enable);
  //bool IsMaterialFragmentLightEnable(s32 material_index) const;
  //void SetMaterialVertexLightEnable(s32 material_index, bool enable);
  //bool IsMaterialVertexLightEnable(s32 material_index) const;
  //void SetMaterialHemiSphereLightEnable(s32 material_index, bool enable, bool oclusion_enable);
  //bool IsMaterialHemiSphereLightEnable(s32 material_index, bool* oclusion_enable) const;
  void SetMaterialLightSetIndex(s32 material_index, s32 index);
  s32  GetMaterialLightSetIndex(s32 material_index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���̃J�����O���[�h
   */
  //-----------------------------------------------------------------------------
  void SetMaterialCullingMode(s32 material_index, MaterialCullingMode mode);
  MaterialCullingMode GetMaterialCullingMode(s32 material_index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���̃A���t�@�e�X�g
   */
  //-----------------------------------------------------------------------------
  bool IsMaterialAlphaTestEnable(s32 material_index) const; 
  void SetMaterialAlphaTestEnable(s32 material_index, bool enable);
  MaterialTestFunction GetMaterialAlphaTestFunction(s32 material_index) const;
  void SetMaterialAlphaTestFunction(s32 material_index, MaterialTestFunction function);
  f32 GetMaterialAlphaTestReference(s32 material_index) const;
  void SetMaterialAlphaTestReference(s32 material_index, f32 reference);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���̃f�v�X�e�X�g
   */
  //-----------------------------------------------------------------------------
  bool IsMaterialDepthTestEnable(s32 material_index) const;          // DepthOperation IsTestEnabled
  void SetMaterialDepthTestEnable(s32 material_index, bool enable);  // true�̂Ƃ��f�v�X�e�X�g�L��
  bool IsMaterialDepthTestMaskEnable(s32 material_index) const;          // DepthOperation IsMaskEnabled
  void SetMaterialDepthTestMaskEnable(s32 material_index, bool enable);  // true�̂Ƃ��f�v�X�}�X�N�L��(true�̂Ƃ��f�v�X�o�b�t�@���X�V����)
  MaterialTestFunction GetMaterialDepthTestFunction(s32 material_index) const;  // DepthOperation TestFunction
  void SetMaterialDepthTestFunction(s32 material_index, MaterialTestFunction function);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���̃X�e���V���e�X�g
   */
  //-----------------------------------------------------------------------------
  bool IsMaterialStencilTestEnable(s32 material_index) const;
  void SetMaterialStencilTestEnable(s32 material_index, bool enable);
  MaterialTestFunction GetMaterialStencilTestFunction(s32 material_index) const;
  void SetMaterialStencilTestFunction(s32 material_index, MaterialTestFunction function);
  s32 GetMaterialStencilTestReference(s32 material_index) const;
  void SetMaterialStencilTestReference(s32 material_index, s32 value);
  u32 GetMaterialStencilTestMask(s32 material_index) const;
  void SetMaterialStencilTestMask(s32 material_index, u32 value);
  MaterialStencilOperation GetMaterialStencilTestFailOperation(s32 material_index) const;
  void SetMaterialStencilTestFailOperation(s32 material_index, MaterialStencilOperation operation);
  MaterialStencilOperation GetMaterialStencilTestZFailOperation(s32 material_index) const;
  void SetMaterialStencilTestZFailOperation(s32 material_index, MaterialStencilOperation operation);
  MaterialStencilOperation GetMaterialStencilTestPassOperation(s32 material_index) const;
  void SetMaterialStencilTestPassOperation(s32 material_index, MaterialStencilOperation operation);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���̃u�����_�[
   */
  //-----------------------------------------------------------------------------
  MaterialBlendMode GetMaterialBlendMode(s32 material_index) const;
  void SetMaterialBlendMode(s32 material_index, MaterialBlendMode mode);

  void SetMaterialBlendColor(s32 material_index, f32 r, f32 g, f32 b);
  void SetMaterialBlendColor(s32 material_index, f32 r, f32 g, f32 b, f32 a);
  void SetMaterialBlendColor(s32 material_index, const gfl::grp::ColorF32& c);
  void SetMaterialBlendColor(s32 material_index, u8 r, u8 g, u8 b);
  void SetMaterialBlendColor(s32 material_index, u8 r, u8 g, u8 b, u8 a);
  void SetMaterialBlendColor(s32 material_index, const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetMaterialBlendColor(s32 material_index) const;
  void GetMaterialBlendColor(s32 material_index, gfl::grp::ColorF32* c) const;
  void GetMaterialBlendColor(s32 material_index, gfl::grp::ColorU8*  c) const;
  
  MaterialBlendLogicOperation GetMaterialBlendLogicOperation(s32 material_index) const;
  void SetMaterialBlendLogicOperation(s32 material_index, MaterialBlendLogicOperation operation);

  MaterialBlendFactor GetMaterialBlendFunctionSourceRgb(s32 material_index) const;
  void SetMaterialBlendFunctionSourceRgb(s32 material_index, MaterialBlendFactor factor);
  MaterialBlendFactor GetMaterialBlendFunctionSourceAlpha(s32 material_index) const;
  void SetMaterialBlendFunctionSourceAlpha(s32 material_index, MaterialBlendFactor factor);

  MaterialBlendFactor GetMaterialBlendFunctionDestinationRgb(s32 material_index) const;
  void SetMaterialBlendFunctionDestinationRgb(s32 material_index, MaterialBlendFactor factor);
  MaterialBlendFactor GetMaterialBlendFunctionDestinationAlpha(s32 material_index) const;
  void SetMaterialBlendFunctionDestinationAlpha(s32 material_index, MaterialBlendFactor factor);

  MaterialBlendEquation GetMaterialBlendEquationRgb(s32 material_index) const;
  void SetMaterialBlendEquationRgb(s32 material_index, MaterialBlendEquation equation);
  MaterialBlendEquation GetMaterialBlendEquationAlpha(s32 material_index) const;
  void SetMaterialBlendEquationAlpha(s32 material_index, MaterialBlendEquation equation);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�揇
   *                 �������̎��(�`�惌�C���[)
   *                 �f�U�C�i���G�f�B�^��Őݒ�ł���}�e���A���̕`�惌�C���[
   *                 �ŏ��ɕ`�� MATERIAL_TRANSLUCENCY_KIND_LAYER0 <= MaterialTranslucencyKind <= MATERIAL_TRANSLUCENCY_KIND_LAYER3 �Ō�ɕ`��
   *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
   */
  //-----------------------------------------------------------------------------
  MaterialTranslucencyKind GetMaterialTranslucencyKind(s32 material_index) const;
  void SetMaterialTranslucencyKind(s32 material_index, MaterialTranslucencyKind kind);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���̃e�N�X�`��
   */
  //-----------------------------------------------------------------------------
public:
  // material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��texture_resource_pack�Ŏw���e�N�X�`����ݒ肷��B
  // ���X�\���Ă������e�N�X�`���͂��̃��\�[�X����������Ƃ��ɉ�������̂ŁA�����ւ����Ă�Setup���ꂽ�܂܂̏�ԂŎc���Ă��܂��B
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, Resource::TextureResourcePack* texture_resource_pack);
  // material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��texture_resource�Ŏw���e�N�X�`����ݒ肷��B
  // ���X�\���Ă������e�N�X�`���͂��̃��\�[�X����������Ƃ��ɉ�������̂ŁA�����ւ����Ă�Setup���ꂽ�܂܂̏�ԂŎc���Ă��܂��B
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, Resource* texture_resource, s32 index_in_resource);
  // material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��texture�Ŏw���e�N�X�`����ݒ肷��B
  // ���X�\���Ă������e�N�X�`���͂��̃��\�[�X����������Ƃ��ɉ�������̂ŁA�����ւ����Ă�Setup���ꂽ�܂܂̏�ԂŎc���Ă��܂��B
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, Texture* texture);
private:
  void setMaterialTextureMapperTexture(s32 material_index, s32 no, nw::gfx::ResTexture res_texture);

public:
  // material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��U�����J��Ԃ��ݒ���s���B
  void SetMaterialTextureMapperWrapU(s32 material_index, s32 no, MaterialTextureWrap wrap);
  // material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��U�����J��Ԃ��ݒ�𓾂�B
  MaterialTextureWrap  GetMaterialTextureMapperWrapU(s32 material_index, s32 no) const;
  // material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��V�����J��Ԃ��ݒ���s���B
  void SetMaterialTextureMapperWrapV(s32 material_index, s32 no, MaterialTextureWrap wrap);
  // material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��V�����J��Ԃ��ݒ�𓾂�B
  MaterialTextureWrap  GetMaterialTextureMapperWrapV(s32 material_index, s32 no) const;
  // �e�N�X�`���̍��W�́AST�͎g�킸�AUV�œ��ꂵ�Ă����B

  // material_index�ԃ}�e���A����no�ԃe�N�X�`���R�[�f�B�l�[�^(0<=no<=2)�̐ݒ�
  // texture_mapper_no�ԃe�N�X�`���}�b�p�[��texture_coordinator_no�ԃe�N�X�`���R�[�f�B�l�[�^���������ꍇ�A
  // �u2�ԃe�N�X�`���}�b�p�[�Ȃ�2�ԃe�N�X�`���R�[�f�B�l�[�^���g���v�Ƃ����ӂ��ɕK�������Ȃ��Ă���킯�ł͂Ȃ��B
  // �u1�ԃe�N�X�`���}�b�p�[��2�ԃe�N�X�`���}�b�p�[��1�ԃe�N�X�`���R�[�f�B�l�[�^�����L���Ďg���v�Ƃ������Ƃ����蓾��B
  // �܂�texture_mapper_no=texture_coordinator_no�ƕK���Ȃ�킯�ł͂Ȃ��A�Ƃ������Ƃɒ��ӂ��ĉ������B
  void SetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, const gfl::math::VEC2& t);
  void GetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, gfl::math::VEC2* t) const;
  
  void SetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no, const f32 t);  // [radian]
  f32 GetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no) const;  // [radian]
  
  void SetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, const gfl::math::VEC2& t);
  void GetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, gfl::math::VEC2* t) const;

  void SetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no, s32 scene_camera_index);
  s32  GetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���b�V��
   */
  //-----------------------------------------------------------------------------
  s32 GetMeshCount() const;
  s32 GetMeshIndex(const char* key) const;
  b32 IsMeshVisible(const s32 mesh_index) const;
  void SetMeshVisible(const s32 mesh_index, const b32 visible);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[�U�f�[�^
   */
  //-----------------------------------------------------------------------------
  // ���f��
  s32 GetModelUserDataCount() const;
  s32 GetModelUserDataIndex(const char* key) const;
  void GetModelUserData(const s32 user_data_index, UserData* user_data) const;
  // �}�e���A��
  s32 GetMaterialUserDataCount(const s32 material_index) const;
  s32 GetMaterialUserDataIndex(const s32 material_index, const char* key) const;
  void GetMaterialUserData(const s32 material_index, const s32 user_data_index, UserData* user_data) const;
  // ���b�V��
  s32 GetMeshUserDataCount(const s32 mesh_index) const;
  s32 GetMeshUserDataIndex(const s32 mesh_index, const char* key) const;
  void GetMeshUserData(const s32 mesh_index, const s32 user_data_index, UserData* user_data) const;
  
  bool GetModelAABB( gfl::math::AABB *p_dest_aabb ) const ;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z�����ɂČĂяo�����R�[���o�b�N
   */
  //-----------------------------------------------------------------------------
  enum
  {
    CALLBACK_INDEX_NOT_ADD   = -1    // �R�[���o�b�N��ǉ����Ă��Ȃ��Ƃ�
  };
  // �r���ŃR�[���o�b�N���Ȃ��ɂ���ꍇ�́A�߂�l���o���Ă����ĉ������B
  // �R�[���o�b�N��ǉ��ł��Ȃ������Ƃ�CALLBACK_INDEX_NOT_ADD��Ԃ��B
  s32 AddCalculateCallback(ModelCalculateCallbackFunction function, void* work=NULL);
  // ����calculate_callback_index�ɂ�AddCalculateCallback�̖߂�l��n���ĉ������B
  void RemoveCalculateCallback(s32 calculate_callback_index);


#if 0
  //������
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z�t���O
   *                 true�̂Ƃ��v�Z�����(@note �S���H�ꕔ�H)
   *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͌v�Z����Ȃ�
   */
  //-----------------------------------------------------------------------------
  void SetCalculateFlag(b32 flag);
  b32  GetCalculateFlag(void) const;
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`��t���O
   *                 true�̂Ƃ��`�悳���
   *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͕`�悳��Ȃ�
   */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(b32 visible);
  virtual b32  IsVisible(void) const;

#if 0
  //SceneNode�ֈڍs����
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���c���[�t���O
   *                 true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������Afalse�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����
   *                 ���ꂪtrue�ł��邱�ƂƁA���[�U��Scene��Add�������Ƃ́A�����ł͂���܂���B
   */
  //-----------------------------------------------------------------------------
  void SetTreeFlag(b32 flag);
  b32  GetTreeFlag(void) const;
#endif

#if 0
  //������
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z����邩�ǂ���
   */
  //-----------------------------------------------------------------------------
  CalculateState GetCalculateState(void) const;
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�悳��邩�ǂ���
   */
  //-----------------------------------------------------------------------------
  DrawState GetDrawState(void) const;


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V���������蓖�Ă�X���b�g�����p�ӂ���Ă��邩
   *
   *  @retval        u32    �p�ӂ���Ă���X���b�g�̌�
   */
  //-----------------------------------------------------------------------------
  u32 GetAnimationSlotNum(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V���������蓖�Ă�X���b�g�ɃA�j���[�V���������蓖�Ă��Ă��邩
   *
   *  @param[in]     slot_index    �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
   *
   *  @retval        b32    �A�j���[�V���������蓖�Ă��Ă�����true
   */
  //-----------------------------------------------------------------------------
  b32 IsAnimationSlotHavingAnimation(u32 slot_index) const;  // having�͉p��Ƃ��ċC�ɂȂ�


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����X���b�g�̗L��/�������擾����
   *
   *  @param[in]     slot_index    �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
   *
   *  @retval        b32    �A�j���[�V�����X���b�g���L���Ȃ�true
   */
  //-----------------------------------------------------------------------------
  b32  IsAnimationSlotEnable(u32 slot_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����X���b�g�̗L��/������ݒ肷��
   *
   *  @param[in]     slot_index    �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
   *  @param[in]     enable        �A�j���[�V�����X���b�g��L���ɂ���Ȃ�true
   */
  //-----------------------------------------------------------------------------
  void SetAnimationSlotEnable(u32 slot_index, b32 enable);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V������ύX����
   *                 slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂Ă��̊֐����Ă񂾂Ƃ��́A�ΏۃX���b�g��Animation�𐶐�����B
   *                 slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂�change_frame>0�ł��̊֐����Ă񂾂Ƃ����A�ΏۃX���b�g��Animation�𐶐�����B
   *
   *  @param[in]     allocator               �A���P�[�^
   *  @param[in]     res_anim                �A�j���[�V�������\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
   *  @param[in]     change_frame            ���t���[���ŕύX���邩(���̊ԃu�����h�����)
   *  @param[in]     slot_index              �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
   *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *                                         slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂Ă��̊֐����Ă񂾂Ƃ��́A�g�p�����B
   *                                         slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂�change_frame>0�ł��̊֐����Ă񂾂Ƃ����A�g�p�����B
   *                                         Resource::Get???MemberAnimSetCount�œ����l��n���Ƃ悢�B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
   *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
   *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   */
  //-----------------------------------------------------------------------------
  virtual void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    u32                     change_frame = 0,
    u32                     slot_index = 0,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST,
    bool                    force = false
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V������ύX����
   *                 slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂Ă��̊֐����Ă񂾂Ƃ��́A�ΏۃX���b�g��Animation�𐶐�����B
   *                 slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂�change_frame>0�ł��̊֐����Ă񂾂Ƃ����A�ΏۃX���b�g��Animation�𐶐�����B
   *
   *  @param[in]     allocator               �A���P�[�^
   *  @param[in]     resource                ���\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
   *  @param[in]     animation_resource_type �A�j���[�V�������\�[�X�̃^�C�v(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *  @param[in]     index_in_resource       ���\�[�X���ɂ�����C���f�b�N�X(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *  @param[in]     change_frame            ���t���[���ŕύX���邩(���̊ԃu�����h�����)
   *  @param[in]     slot_index              �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
   *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *                                         slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂Ă��̊֐����Ă񂾂Ƃ��́A�g�p�����B
   *                                         slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂�change_frame>0�ł��̊֐����Ă񂾂Ƃ����A�g�p�����B
   *                                         Resource::Get???MemberAnimSetCount�œ����l��n���Ƃ悢�B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
   *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
   *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   */
  //-----------------------------------------------------------------------------
/**
  ���̂����������邩���B
   *  @param[in]     resource                ���\�[�X(NULL��n���ƃA�j���[�V�������������܂�)
   *  @param[in]     animation_resource_type �A�j���[�V�������\�[�X�̃^�C�v(resource��NULL�̂Ƃ��̂�ANIMATION_RESOURCE_TYPE_ALL��n���Ă悢�B
   *                                         ANIMATION_RESOURCE_TYPE_ALL�̂Ƃ��S��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
*/
  void ChangeAnimation(
    gfl::heap::NwAllocator*          allocator,
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource,
    u32                              change_frame = 0,
    u32                              slot_index = 0,
    s32                              max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST,
    bool                             force = false
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index��unit_index�Ɋ��蓖�Ă�A�j���[�V������ύX����
   *
   *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
   *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
   *  @param[in]     slot_index              �X���b�g�̃C���f�b�N�X
   *  @param[in]     unit_index              �X���b�g���ɂ��������j�b�g�̃C���f�b�N�X
   *  @param[in]     res_anim                �A�j���[�V�������\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
   *  @param[in]     resource                ���\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
   *  @param[in]     animation_resource_type �A�j���[�V�������\�[�X�̃^�C�v(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *  @param[in]     index_in_resource       ���\�[�X���ɂ�����C���f�b�N�X(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *  @param[in]     change_frame            ���t���[���ŕύX���邩(���̊ԃu�����h�����)
   *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *                                         Resource::Get???MemberAnimSetCount�œ����l��n���Ƃ悢�B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
   *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
   *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   */
  //-----------------------------------------------------------------------------
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    u32                     slot_index,
    u32                     unit_index,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST,
    b32                     force = false
  );
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator*          heap_allocator,
    gfl::heap::NwAllocator*          device_allocator,
    u32                              slot_index,
    u32                              unit_index,
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource,
    u32                              change_frame,
    s32                              max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST,
    b32                              force = false
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationFrame(f32 frame, u32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[�����擾����
   *
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   * 
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
   *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
   *
   *  @param[in]     step_frame      �X�V�t���[��
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStepFrame(f32 step_frame, u32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
   *
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   * 
   *  @retval        �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStepFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartFrame(f32 frame, u32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
   *
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStartFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationEndFrame(f32 frame, u32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[�����擾����
   *
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationEndFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
   *
   *  @param[in]     start_frame     �J�n�t���[��
   *  @param[in]     end_frame       �I���t���[��
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame, u32 slot_index = 0);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[�����J�n�t���[�����ۂ����肷��
   *
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   *
   *  @retval        bool    ���݂̃t���[�����J�n�t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationFrameStartFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
   *
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   *
   *  @retval        bool    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationFrameEndFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[����
   *                     step_frame>0�̂Ƃ��͏I���t���[���̂Ƃ�true��Ԃ�
   *                     step_frame<0�̂Ƃ��͊J�n�t���[���̂Ƃ�true��Ԃ�
   *                     step_frame==0�̂Ƃ��͊J�n�t���[�����I���t���[���̂Ƃ�true��Ԃ��A����ȊO�̂Ƃ�false��Ԃ�
   *
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   *
   *  @retval        bool    ���݂̃t���[����step_frame���猩�Ē[�t���[���ƂȂ��Ă���ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationFrameStepTerminalFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
   *
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   *
   *  @retval        bool            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationLoop(u32 slot_index = 0) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
   *
   *  @param[in]     loop            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
   *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationLoop(bool loop, u32 slot_index = 0);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t���[������
   */
  //-----------------------------------------------------------------------------
  void SetUnitFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  GetUnitFrame(u32 slot_index, u32 unit_index) const;
  void SetUnitStepFrame(u32 slot_index, u32 unit_index, f32 step_frame);
  f32  GetUnitStepFrame(u32 slot_index, u32 unit_index) const;
  void SetUnitStartFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  GetUnitStartFrame(u32 slot_index, u32 unit_index) const;
  void SetUnitEndFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  GetUnitEndFrame(u32 slot_index, u32 unit_index) const;
  void SetUnitStartAndEndFrame(u32 slot_index, u32 unit_index, f32 start_frame, f32 end_frame);
  b32  IsUnitFrameEndFrame(u32 slot_index, u32 unit_index) const;
  b32  IsUnitLoop(u32 slot_index, u32 unit_index) const;
  void SetUnitLoop(u32 slot_index, u32 unit_index, b32 loop);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index���ɂ��������j�b�g�̃A�j���[�V�������u�����h����
   *
   *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
   *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
   *  @param[in]     slot_index              �X���b�g�̃C���f�b�N�X
   *  @param[in]     mode                    AnimationSlot::MODE_NONE                slot_index�̃A�j���[�V�������Ȃ��ɂ���
   *                                         AnimationSlot::MODE_ANIMATION           �u�����h����߂�animation_unit_index�Ŏw�肵���A�j���[�V���������蓖�Ă�
   *                                         AnimationSlot::MODE_BLEND_INTERPOLATOR  ��ԃu�����h
   *                                         AnimationSlot::MODE_BLEND_ADDER         ���Z�u�����h
   *  @param[in]     weight0                 AnimationSlot::MODE_BLEND_INTERPOLATOR��AnimationSlot::MODE_BLEND_ADDER�̂Ƃ��̂ݎg�p�B
   *                                         ���j�b�g0�̃u�����h�d��
   *  @param[in]     weight1                 AnimationSlot::MODE_BLEND_INTERPOLATOR��AnimationSlot::MODE_BLEND_ADDER�̂Ƃ��̂ݎg�p�B
   *                                         ���j�b�g1�̃u�����h�d��
   *  @param[in]     interpolator_ignore_no_anim_member   AnimationSlot::MODE_BLEND_INTERPOLATOR�̂Ƃ��̂ݎg�p�B
   *                                                      false�ɂ���ƁA�A�j���[�V���������݂��Ȃ������o�̓o�C���h���̒l���u�����h����܂��B
   *                                                      true�ɂ���ƁA�A�j���[�V���������݂��Ȃ������o�͏d�� 0 �Ƃ��ău�����h����܂��B
   *  @param[in]     interpolator_normalization_enable    AnimationSlot::MODE_BLEND_INTERPOLATOR�̂Ƃ��̂ݎg�p�B
   *                                                      true�ɂ���ƁA�u�����h�d�݂����v�� 1 �ɂȂ�悤���K�����Ă���u�����h���s���܂��B
   *                                                      false�ɂ���ƁA�ݒ肵���d�݂����̂܂܃u�����h�Ɏg�p�����B
   *  @param[in]     animation_unit_index                 AnimationSlot::MODE_ANIMATION�̂Ƃ��̂ݎg�p�B
   *                                                      �X���b�g���̂ǂ̃��j�b�g�ɂ���A�j���[�V���������蓖�Ă邩���w�肷��B
   */
  //-----------------------------------------------------------------------------
  void BlendUnitAnimation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      u32                        slot_index,
      AnimationSlot::Mode        mode,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );
  void BlendUnit2Animation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      u32                        slot_index,
      AnimationSlot::Mode        mode,
      f32                        weight0,
      f32                        weight1,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �u�����h�d��
   */
  //-----------------------------------------------------------------------------
  void SetUnitBlendWeight(u32 slot_index, u32 unit_index, f32 weight);
  f32  GetUnitBlendWeight(u32 slot_index, u32 unit_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnim::Type�̃A�j���[�V�����ŕύX�����|�[�Y�����Z�b�g����
   *
   *                 slot_index�X���b�g�Ɋ��蓖�ĂĂ���A�j���[�V������ResAnim::Type�̃|�[�Y�����Z�b�g����
   *                 �������A���蓖�Ē��̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ�
   *
   *  @param[in]     type          �A�j���[�V�����̃^�C�v
   *  @param[in]     slot_index    �X���b�g�̃C���f�b�N�X
   *
   *  �A�j���[�V���������蓖�Ă��Ă���Ƃ��́A���̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ��B
   *  �A�j���[�V���������蓖�Ă��Ă��Ȃ��Ƃ��́A�S�Ẵ����o�����Z�b�g�����B
   *
   *  slot_index�X���b�g�ɃA�j���[�V���������蓖�Ă��Ă��Ȃ��Ƃ��́A���̃X���b�g�̃A�j���[�V������ResAnim::Type���Ȃ����߁A
   *  �������Z�b�g����Ȃ��̂Œ��ӂ��邱�ƁB
   *
   *  type��ResAnim::TYPE_MATERIAL�̏ꍇ�́A���̃��f���𐶐������Ƃ���
   *  Description�Ŏw�肵�Ă���MaterialBufferOption��
   *  MATERIAL_BUFFER_OPTION_ANIMATABLE_MATERIAL��
   *  MATERIAL_BUFFER_OPTION_ALL_MATERIAL�łȂ���΂Ȃ�Ȃ��B
   */
  //-----------------------------------------------------------------------------
  void ResetAnimationPose(ResAnim::Type type);
  void ResetAnimationSlotPose(u32 slot_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X���擾����
   *
   *  @param[out]    matrix          �擾�����}�g���N�X���i�[���ĕԂ�
   *
   *  @retval        bool            ����Ɏ擾�ł�����true
   */
  //-----------------------------------------------------------------------------
  virtual bool GetWorldMatrix(gfl::math::MTX34* matrix) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X��ݒ肷��
   *                 Scene��Calculate�̌�łȂ��Ɣ��f����܂���B
   *
   *  @param[in]     matrix          �ݒ肷��}�g���N�X
   *
   *  @retval        bool            ����ɐݒ�ł�����true
   */
  //-----------------------------------------------------------------------------
  virtual bool SetWorldMatrix(const gfl::math::MTX34& matrix);  // ���C�g��J�����A�{�[���̂Ȃ����f���Ȃ�A�����ݒ肷��Ό����ڂɒl�����f�����B
                                                                // �{�[���̂��郂�f�����Ƃ����ݒ肵�Ă������ڂɒl�����f����Ă���悤�ɂ͌����Ȃ��B

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X�̌v�Z�̗L���t���O
   *
   *  @param[in]     enable         �L���ɂ���Ƃ���true�A�����ɂ���Ƃ���false
   */
  //-----------------------------------------------------------------------------
  void SetWorldMatrixCalculationEnable(bool enable);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X�̌v�Z�̗L���t���O
   *
   *  @retval        bool            �L���̂Ƃ���true�A�����̂Ƃ���false
   */
  //-----------------------------------------------------------------------------
  bool IsWorldMatrixCalculationEnable(void) const;


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �{�[���̐����擾����
   *
   *  @retval        s32             �{�[���̐�
   */
  //-----------------------------------------------------------------------------
  virtual s32  GetBonesCount(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �{�[���̃C���f�b�N�X�ԍ����擾����
   *
   *  @param[in]     key             �{�[���̖��O
   *
   *  @retval        s32             �{�[���̃C���f�b�N�X�ԍ�        
   */
  //-----------------------------------------------------------------------------
  virtual s32  GetBonesIndex(const char* key) const;
   //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X�|�[�Y�̃{�[���̐����擾����
   *
   *  @retval        s32             �{�[���̐�       
   */
  //-----------------------------------------------------------------------------
  virtual s32  GetBonesCountOfWorldMatrixPose(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X�|�[�Y�̃}�g���N�X���擾����
   *
   *  @param[in]     bone_index      �{�[���̃C���f�b�N�X
   *  @param[out]    matrix          �擾�����}�g���N�X���i�[���ĕԂ�
   *
   *  @retval        bool            ����Ɏ擾�ł�����true
   */
  //-----------------------------------------------------------------------------
  virtual bool GetBoneMatrixOfWorldMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X�|�[�Y�̃}�g���N�X��ݒ肷��
   *
   *  @param[in]     bone_index      �{�[���̃C���f�b�N�X
   *  @param[in]     matrix          �ݒ肷��}�g���N�X
   *
   *  @retval        bool            ����ɐݒ�ł�����false
   */
  //-----------------------------------------------------------------------------
  virtual bool SetBoneMatrixOfWorldMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X�|�[�Y�̃}�g���N�X�̃|�C���^���擾����
   *
   *  @param[in]     bone_index         �{�[���̃C���f�b�N�X
   *
   *  @retval        gfl::math::MTX34*  �}�g���N�X�̃|�C���^��Ԃ��B
   *                                    NULL�̂Ƃ��Ȃ��B
   *                                    ����ɒl��������΃��[���h�}�g���N�X�|�[�Y�̃}�g���N�X�ɔ��f����܂��B
   */
  //-----------------------------------------------------------------------------
  virtual gfl::math::MTX34* GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index) const;

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����ւ��p�̃{�[���f�[�^���쐬����
   *
   *  @param[in]     heap_allocator  �q�[�v�������̃A���P�[�^
   *  @param[in]     base_model      ��{���f��
   */
  //-----------------------------------------------------------------------------
  void CreateReplaceBoneData(gfl::heap::NwAllocator* heap_allocator, Model* base_model);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����ւ��p�̃{�[���f�[�^��j������
   */
  //-----------------------------------------------------------------------------
  void DestroyReplaceBoneData(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����ւ��p�̃{�[���̖����ݒ���s��(�e����)
   *
   *  @param[in]     name   name�̃{�[�����e�̃{�[���͑S�Ė����ݒ�ɂ���(name�̃{�[���͗L���ݒ�̂܂�)
   */
  //-----------------------------------------------------------------------------
  void IgnoreParentReplaceBone(const char* name); 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����ւ��p�̃{�[���̖����ݒ���s��(�q����)
   *
   *  @param[in]     name   name�̃{�[�����q�̃{�[���͑S�Ė����ݒ�ɂ���(name�̃{�[���͗L���ݒ�̂܂�)
   */
  //-----------------------------------------------------------------------------
  void IgnoreChildReplaceBone(const char* name); 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����ւ��p�̃{�[���̖����ݒ���s��(�q����)
   *
   *  @param[in]     name   name�̃{�[���Ƃ�����q�̃{�[���͑S�Ė����ݒ�ɂ���(name�̃{�[���������ݒ�ɂȂ�)
   */
  //-----------------------------------------------------------------------------
  void IgnoreSelfChildReplaceBone(const char* name); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �ubase�̃{�[���v�̃��[���h�}�g���N�X���upart�̃{�[��(���̃��f���̃{�[��)�v�̃��[���h�}�g���N�X�ɐݒ肷��
   *                 �����Őݒ肵���l��Scene��Calculate���ĂԂƏ㏑������Ă��܂��̂ŁAScene��Calculate�̌�ŌĂԂ��ƁB
   *                 
   *                 // @note ���[���h�}�g���N�X�ɐݒ肷��֐��Ȃ̂ŁAhuman�g�b�v�ɂ��ݒ肵���ق���������������Ȃ����A
   *                          ���͂܂��ݒ肵�Ă��Ȃ��B
   *                 // @note �����������[���h�A���[�J���ƕ������ɁA�S���ɐݒ肷��̂������̂�������Ȃ��B
   *                 // @note ���̊֐��ɂāA�R�[���o�b�N�̂���human�g�b�v��hand�{�[���ɒl��ݒ肵����A
   *                          �R�[���o�b�N���ĂԂ悤�ɂ�����肾���A���͂܂��ĂׂĂ��Ȃ��B
   *
   *  @param[in]     base_model      ��{���f��
   */
  //-----------------------------------------------------------------------------
  void SetWorldReplaceBone(Model* base_model);
  //void SetLocalReplaceBone(Model* base_model);  // @note ���[�J���}�g���N�X�ł����A�܂��ł��Ă��Ȃ�
                                                  //       NW4C�̃X�P���g�����L�Ɠ������ʂ𓾂����̊֐��B�X�P���g���A�j���͂��Ă��邪�ꏊ���Ⴄ�Ƃ������́B
  //void SetWorldLocalReplaceBone(Model* base_model);  // @note ���[���h�}�g���N�X�ɂ����[�J���}�g���N�X�ɒl��ݒ肷��ł����A�܂��ł��Ă��Ȃ�
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         res_bone�̃��[���h�}�g���N�X��parent_world_matrix�𔽉f�������̂�ݒ肷��B
   *                 ���̌�A�ċA�I�Ɏq�{�[���ɂ��̏������s���Ă����B
   *                 
   *  @param[in]     skeleton             part�̃X�P���g��
   *  @param[in]     res_bone             part�̃{�[��
   *  @param[in]     parent_world_matrix  res_bone�̐e�̃��[���h�}�g���N�X
   */
  //-----------------------------------------------------------------------------
  void SetWorldReplaceBoneRecursive(
      nw::gfx::Skeleton*     skeleton,
      nw::gfx::res::ResBone  res_bone,
      const nw::math::MTX34* parent_world_matrix
#if GFL_DEBUG
      , const s32            recursive_count 
#endif
  ); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����ւ��p�̃{�[���̖����ݒ���ċA�I�ɍs��(�q����)
   *
   *  @param[in]     res_bone   res_bone�̃{�[���𖳎��ݒ�ɂ��Ares_bone���q�̃{�[�����S�Ė����ݒ�ɂ���(res_bone�̃{�[���������ݒ�ɂȂ�)
   */
  //-----------------------------------------------------------------------------
  void ignoreSelfChildReplaceBoneRecursive(
      nw::gfx::res::ResBone  res_bone
#if GFL_DEBUG
      , const s32            recursive_count 
#endif
  ); 




#if 0
  /*
  baton�͂ǂ���1�ɂ����S�������邱�Ƃ��ł��Ȃ��B
  hand�͕�����baton�����Ă�B
  human��������baton�����Ă�B
  ����āA�R���X�g���C���̏���baton�Ɏ������Ă����ق��������̂ŁA
  Model����SceneNode�Ɉړ�����B
  �����̓R�����g�A�E�g�B
   */
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���C���̃Z�b�g�A�b�v������
   *
   *                 ���ɃZ�b�g�A�b�v����Ă���baton_scene_node���Z�b�g�A�b�v����ƁA���̃Z�b�g�A�b�v�ŏ㏑�������B
   *                 ������Ainherit��ύX�������Ƃ���hand�{�[����ύX�������Ƃ��ɂ����̊֐����Ăׂ΂悢�B
   *
   *  @param[in]     hand_bone_index     hand�{�[���̃C���f�b�N�X
   *                                     BONE_INDEX_NONE�̂Ƃ��Ahand�{�[���ł͂Ȃ�human�g�b�v�ɂ���
   *  @param[in]     hand_bone_key       hand�{�[���̖��O
   *                                     NULL�̂Ƃ��Ahand�{�[���ł͂Ȃ�human�g�b�v�ɂ���
   *  @param[in]     baton_scene_node    �S�������baton�V�[���m�[�h
   *  @param[in]     inherit_scale       true�̂Ƃ��X�P�[���̒Ǐ]����
   *  @param[in]     inherit_rotate      true�̂Ƃ���]�̒Ǐ]����
   *  @param[in]     inherit_translate   true�̕��s�ړ��̒Ǐ]����
   *
   *  @retval        b32                 ����ɃZ�b�g�A�b�v�ł�����true
   */
  //-----------------------------------------------------------------------------
  b32 SetupConstraint(
      const s32 hand_bone_index,
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const b32 inherit_scale     = true,
      const b32 inherit_rotate    = true,
      const b32 inherit_translate = true
  );
  b32 SetupConstraint(
      const char* hand_bone_key,
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const b32 inherit_scale     = true,
      const b32 inherit_rotate    = true,
      const b32 inherit_translate = true
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���C���̃N���[���A�b�v������
   *
   *                 baton_scene_node���S������Ȃ��Ȃ�B
   *
   *  @param[in]     hand_bone_index     hand�{�[���̃C���f�b�N�X
   *                                     BONE_INDEX_NONE�̂Ƃ��Ahand�{�[���ł͂Ȃ�human�g�b�v�ɂ����Ă���
   *  @param[in]     hand_bone_key       hand�{�[���̖��O
   *                                     NULL�̂Ƃ��Ahand�{�[���ł͂Ȃ�human�g�b�v�ɂ����Ă���
   *  @param[in]     baton_scene_node    �S������Ă���baton�V�[���m�[�h
   *
   *  @retval        b32                 ����ɃN���[���A�b�v�ł�����true
   */
  //-----------------------------------------------------------------------------
  b32 CleanupConstraint(
      const s32 hand_bone_index,
      gfl::grp::g3d::SceneNode* baton_scene_node
  );
  b32 CleanupConstraint(
      const char* hand_bone_key,
      gfl::grp::g3d::SceneNode* baton_scene_node
  );
#endif
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���m�[�h������ɍS������
   *                 SceneNode::SetupConstraint����Ă΂��
   *
   *  @param[in]     baton_scene_node    �S�������baton�V�[���m�[�h
   *  @param[in]     hand_scene_index    hand�{�[���̃C���f�b�N�X
   *  @param[in]     inherit_scale       true�̂Ƃ��X�P�[���̒Ǐ]����
   *  @param[in]     inherit_rotate      true�̂Ƃ���]�̒Ǐ]����
   *  @param[in]     inherit_translate   true�̕��s�ړ��̒Ǐ]����
   *
   *  @retval        �ǉ��ł�����true��Ԃ�
   *                 ���X�ǉ�����Ă���V���ɒǉ��ł��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈������
   */
  //-----------------------------------------------------------------------------
  b32 AddConstraintBatonSceneNode(
    SceneNode* baton_scene_node,
    const s32  hand_bone_index,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���m�[�h�̍S���̒Ǐ]��ύX����
   *                 baton_scene_node��hand_bone_index�͕ύX���Ȃ��ŁAinherit�����ύX����
   *                 SceneNode::SetupConstraint����Ă΂��
   *
   *  @param[in]     baton_scene_node    �S�����Ă���baton�V�[���m�[�h
   *  @param[in]     hand_scene_index    baton���������Ă���hand�{�[���̃C���f�b�N�X
   *  @param[in]     inherit_scale       true�̂Ƃ��X�P�[���̒Ǐ]����
   *  @param[in]     inherit_rotate      true�̂Ƃ���]�̒Ǐ]����
   *  @param[in]     inherit_translate   true�̕��s�ړ��̒Ǐ]����
   *
   *  @retval        �ύX�ł�����true��Ԃ�
   *                 ���X�ǉ�����Ă��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈������
   */
  //-----------------------------------------------------------------------------
  b32 ChangeConstraintBatonSceneNodeInherit(
    SceneNode* baton_scene_node,
    const s32  hand_bone_index,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���m�[�h�̍S������������
   *                 SceneNode::SetupConstraint����Ă΂��
   *                 SceneNode::CleanupConstraint����Ă΂��
   *
   *  @param[in]     baton_scene_node    �S������Ă���baton�V�[���m�[�h
   *  @param[in]     hand_scene_index    baton���������Ă���hand�{�[���̃C���f�b�N�X
   *
   *  @retval        �폜�ł��Ȃ��Ƃ���false��Ԃ�
   *                 ���X�ǉ�����Ă��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈������
   */
  //-----------------------------------------------------------------------------
  b32 RemoveConstraintBatonSceneNode(
    SceneNode* baton_scene_node,
    const s32  hand_bone_index
  );


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����_�[�L���[�֒ǉ��A�����_�[�L���[����폜
   */
  //-----------------------------------------------------------------------------
  // UserParameter��RenderQueue�ɂ͎g���Ă��Ȃ����Ƃ����������̂ŁA
  // SafeDeleteCheck�p�̎g�킹�Ă��炤�B
  virtual void AddAllRenderQueue(void);  // SafeDeleteCheck�p�Ɏg���Ă��܂��B
  virtual void RemoveAllRenderQueue(void);  // SafeDeleteCheck�p�Ɏg���Ă��܂��B
  virtual void AddRenderQueue(s32 render_queue_index);  // SafeDeleteCheck�p�Ɏg���Ă��܂��B
  virtual void RemoveRenderQueue(s32 render_queue_index);  // SafeDeleteCheck�p�Ɏg���Ă��܂��B

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[�U�[�p�����[�^�̃`�F�b�N
   */
  //-----------------------------------------------------------------------------
  // ���̃��f�������̃����_�[�L���[�ŕ`�悷�邩
  // UserParameter��RenderQueue�ɂ͎g���Ă��Ȃ����Ƃ����������̂ŁA
  // SafeDeleteCheck�p�̎g�킹�Ă��炤�B
  static bool IsUserParameterRenderQueue( const nw::gfx::Model* nw_model, s32 render_queue_index );  // SafeDeleteCheck�p�Ɏg���Ă��܂��B


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �{�N���X�̃}�e���A���̃o�b�t�@�̐����I�v�V������NintendoWare�̃o�b�t�@�̐����I�v�V�����ɕύX����
   */
  //-----------------------------------------------------------------------------
  static bit32 ConvertMaterialBufferOptionToNwBufferOption(
      const MaterialBufferOption  material_buffer_option
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �������ɃX�P���g�����L���`�F�b�N���A�K�؂ɏ�������
   */
  //-----------------------------------------------------------------------------
  static ShareType CheckSkeletonShareSourceModel(
      Model*               skeleton_share_source_model,
      nw::gfx::Skeleton**  nw_skeleton 
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �������Ƀ}�e���A�����L���`�F�b�N���A�K�؂ɏ�������
   */
  //-----------------------------------------------------------------------------
  static ShareType CheckMaterialShareSourceModel(
      Model*                material_share_source_model,
      MaterialBufferOption  material_buffer_option,
      nw::gfx::Model**      nw_model
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �������Ƀ��b�V���m�[�h�r�W�r���e�B���L���`�F�b�N���A�K�؂ɏ�������
   */
  //-----------------------------------------------------------------------------
  static ShareType CheckMeshNodeVisibilityShareSourceModel(
      Model*                mesh_node_visibility_share_source_model,
      nw::gfx::Model**      nw_model
  );


private:
  // ���[�U�[�p�����[�^(32�r�b�g)
  enum
  {
    // ���[�U�[�p�����[�^��31�`24�r�b�g���A���̃��f�����ǂ̃����_�[�L���[�ŕ`�悷�邩�̃t���O�Ɏg�p����
    USER_PARAMETER_RENDER_QUEUE_BIT_NUM    = 8,            // �g�p����r�b�g��
    USER_PARAMETER_RENDER_QUEUE_BIT_SHIFT  = 24,           // �r�b�g�V�t�g
    USER_PARAMETER_RENDER_QUEUE_BIT_MASK   = 0xFF000000    // �r�b�g�}�X�N

    // ���[�U�[�p�����[�^�́H�`�H�r�b�g���A�H�H�H�H
    //USER_PARAMeTER_�H�H�H�H_BIT_NUM    = ,
    //USER_PARAMeTER_�H�H�H�H_BIT_SHIFT  = ,
    //USER_PARAMeTER_�H�H�H�H_BIT_MASK   =
  };


private:
  b32             m_tree_flag;  // true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������Afalse�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����

  u32             m_animation_slot_num;
  //AnimationSlot*  m_animation_slot_array;
  u32             m_animation_blend_unit_num;
  u32             m_animation_num_in_unit;
  AnimationManager*  m_animation_manager;

  ShareType    m_skeleton_share_type;
  ShareType    m_material_share_type;
  ShareType    m_mesh_node_visibility_share_type;

  MaterialBufferOption   m_material_buffer_option;

  // �v�Z�����ɂČĂяo�����R�[���o�b�N
  u32                              m_calculate_callback_num_max;
  ModelCalculateCallbackFunction*  m_calculate_callback_function_array;
  void**                           m_calculate_callback_work_array;

  // �R���X�g���C��
  internal::CallbackConstraintOfSkeleton*       m_callback_constraint_of_skeleton;
  internal::CallbackConstraintOfTransformNode*  m_callback_constraint_of_transform_node;

  // �����ւ�
  ReplaceBoneData*  m_replace_bone_data_array;  // m_replace_bone_data_array[�{�[����]


#if GFL_DEBUG
  //----------------------------------------------------------------
  //
  // ���S�Ƀ��f��������ł��������m�F����@�\�̗L�������ݒ�
  //
  //----------------------------------------------------------------
private:
  static b32 s_SafeDeleteCheckEnable;  // true�̂Ƃ����S����`�F�b�N�L���B
  static b32 s_SafeDeleteCheckAssert;  // true�̂Ƃ�ASSERT�Ŏ~�߂�Bfalse�̂Ƃ�PRINT�ōς܂���B
public: 
  static void SafeDeleteCheckEnable(b32 fEnable){ s_SafeDeleteCheckEnable = fEnable; }
  static b32 IsSafeDeleteCheckEnable(void){ return s_SafeDeleteCheckEnable; }
  static void SafeDeleteCheckAssert(b32 fAssert){ s_SafeDeleteCheckAssert = fAssert; }
  static b32 IsSafeDeleteCheckAssert(void){ return s_SafeDeleteCheckAssert; }

private:
  u32   m_DrawFrameCount;
  char  m_ModelName[32];
private:
  void SetSafeDeleteCheckInfo(const char* pName);
  void SetModelName(const char* pName);
public:
  void SetDrawFrameCount(u32 cnt){ m_DrawFrameCount = cnt; }
  u32 GetDrawFrameCount(void){ return m_DrawFrameCount; }
#endif    


};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DMODEL_H__
