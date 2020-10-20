#ifndef __GFL_G3DSCENE_H__
#define __GFL_G3DSCENE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dScene.h
 *	@brief  3D�V�[��
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// ����
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

�^�ꗗ
                                             [�O���猩���^]   [�����ŕێ����Ă���^]
scene_calculate_group_index                  s32              s16
scene_calculate_group_num                    s32              s16
scene_draw_group_index                       s32              s16
scene_draw_group_num                         s32              s16

scene_calculate_group_model_index            s32              u16
scene_calculate_group_model_num              s32              u16
scene_calculate_group_light_index            s32              u16
scene_calculate_group_light_num              s32              u16
scene_calculate_group_camera_index           s32              u8
scene_calculate_group_camera_num             s32              u8
scene_calculate_group_fog_index              s32              u8
scene_calculate_group_fog_num                s32              u8
scene_calculate_group_particle_index         s32              u16
scene_calculate_group_particle_num           s32              u16

scene_own_model_index                        s32              u16
scene_own_model_num                          s32              u16
scene_own_light_index                        s32              u16
scene_own_light_num                          s32              u16
scene_own_camera_index                       s32              u8
scene_own_camera_num                         s32              u8
scene_own_fog_index                          s32              u8
scene_own_fog_num                            s32              u8
scene_own_scene_environment_setting_index    s32              u8
scene_own_scene_environment_setting_num      s32              u8

�������m�ۂ�[�����ŕێ����Ă���^]�ōs�����A�֐��̈�����֐����̃��[�J���ϐ���[�O���猩���^]�ōs���B
�܂�A�ŏI�I�ɕێ�����Ƃ���[�����ŕێ����Ă���^]�ɂȂ邪�A����ȊO��[�O���猩���^]�ōs���A�Ƃ������ƁB

*/



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// include
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Base.h>

// gflib math
#include <math/gfl_MathAABB.h>

// gflib grp
#include <grp/gfl_GraphicsSystem.h>
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dSceneSystem.h>
#include <grp/g3d/gfl_G3dRenderSystem.h>
#include <grp/g3d/gfl_G3dShadowSystem.h>
#include <grp/g3d/gfl_G3dParticle.h>
#include <grp/g3d/gfl_G3dEdgeMapSystem.h>
#include <grp/g3d/gfl_G3dDepthOfFieldSystem.h>
#include <grp/g3d/gfl_G3dSceneEnvironmentSetting.h>
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dStereoCamera.h>
#include <grp/g3d/gfl_G3dLight.h>
#include <grp/g3d/gfl_G3dLightSet.h>
#include <grp/g3d/gfl_G3dFog.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dPostEffectSystem.h>
#include <grp/g3d/gfl_G3dBloomFilter.h>

#if GFL_GRP_G3D_H3D_USE
#include <grp/g3d/gfl_G3dH3dModel.h>
#endif  // GFL_GRP_G3D_H3D_USE



namespace gfl {
namespace grp {
namespace g3d {



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// extern�֐��錾
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/**
 *  @brief         ���L�҂�ݒ肷��
 *                 ���L�҂�ID��ݒ肷��
 *                 class Object�̃t�����h�֐�
 *
 *  @param[in]     object       �I�u�W�F�N�g 
 *  @param[in]     a_owner      �I�u�W�F�N�g�̏��L��
 *  @param[in]     a_owner_id   ���L�҂�ID
 */
//-----------------------------------------------------------------------------
extern void Object_SetOwnerAndOwnerId(Object* object, ObjectOwner a_owner, s32 a_owner_id);

//-----------------------------------------------------------------------------
/**
 *  @brief         ���[�U���C�ӂŕt������ID��ݒ肷��(�����I)
 *                 ���[�U���Ǘ����Ă��Ȃ��I�u�W�F�N�g�ł��A���[�U���C�ӂŕt������ID��ݒ�ł���B
 *
 *  @param[in]     object     �I�u�W�F�N�g 
 *  @param[in]     a_user_id  ���[�U���C�ӂŕt������ID
 */
//-----------------------------------------------------------------------------
extern void Object_SetUserIdForcedly(Object* object, s32 a_user_id);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// �N���X�錾
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------
///	3D�V�[���N���X
//=====================================
class Scene : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Scene);


  friend class SceneCalculateGroup;

#if GFL_GRP_G3D_H3D_USE
  friend class H3dModel;
  friend class H3dEnvironment;
#endif  // GFL_GRP_G3D_H3D_USE


  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // �萔�錾
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
public:
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �V�[���v�Z�O���[�v
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //-------------------------------------
  //
  /// �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O
  //
  //=====================================
  enum
  {
    //////// �Ȃ� ////////
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_NONE    = 0,  // �������Ȃ��B


    //////// �P�� ////////
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE                 = 0x1 << 0,  // Calculate�ɂ�����Scale,Rotate,Translate�����s��
                                                                                             // ���[���h�}�g���N�X���X�V����B
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL                 = 0x1 << 1,  // Calculate�ɂ����ăX�P���^�����f���A�r���{�[�h���f���̍���
                                                                                             // ���[���h�}�g���N�X���X�V����B
                                                                                             // ���݂̎p���̒l�����̃��[���h�}�g���N�X�ɔ��f����̂�
                                                                                             // �����ڂ��ς��܂��B
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           = 0x1 << 2,  // Calculate�ɂ����ăA�j���[�V�����t���[����i�߂�B
                                                                                             // �A�j���[�V�����̃t���[���̐��l��i�߂邾���ł��B
                                                                                             // �t���[���̐��l��n���������̂�n+m�ɂȂ邾���ł��B
                                                                                             // ���̎p������������F���ς������͂��܂���B
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     = 0x1 << 3,  // Calculate�ɂ����ăr�W�r���e�B�A�j���[�V�������X�V����B
                                                                                             // �`��t���O���X�V�����̂ŁA�\����ON/OFF���ς��܂��B
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     = 0x1 << 4,  // Calculate�ɂ�����
                                                                                             // �X�P���^���A�j���[�V����(���[���h�}�g���N�X�͕ς��Ȃ�)��
                                                                                             // �}�e���A���A�j���[�V�������X�V����B
                                                                                             // �����A�j���[�V�������ʂ̎p���ɂȂ�܂����A
                                                                                             // ���̃��[���h�}�g���N�X�͍X�V����Ȃ��̂ŁA
                                                                                             // �����ډ����ς��܂���B
                                                                                             // �}�e���A�����A�j���[�V�������ʂ̐F�ɕς��܂��B
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                       = 0x1 << 5,  // Calculate�ɂ����ăp�[�e�B�N�����X�V����B

    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE = 0x1 << 6,  // Calculate�ɂ�����g3d�I�u�W�F�N�g�ŗL�̍X�V�������s���B
                                                                                             // �J�����O�̗L���Ɋւ�炸�K���K�v�ȍX�V���s���܂��B
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING = 0x1 << 7,  // Calculate�ɂ�����g3d�I�u�W�F�N�g�ŗL�̍X�V�������s���B
                                                                                             // �J�����O����Č����Ȃ��Ƃ��́A
                                                                                             // ���Ȃ��Ă���肪�N���Ȃ��Ȃ��X�V���s���܂��B


    //////// ���� ////////

    // �f�t�H���g�p�̒ʏ�ݒ�BCalculate�ɂ����đS�Ă̍X�V���s���B
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_DEFAULT                        =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE                 \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL                 \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                       \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING ,

    // Calculate�ɂ����ăA�j���[�V������]������B
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM                  =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // Calculate�ɂ�����g3d�I�u�W�F�N�g�ŗL�̍X�V�������s���B
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT              =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING ,

    // Calculate�ɂ����ĉ�ʓ��ɕ\������Ă��邩�ۂ��̔�����s���̂ɕK�v�ƂȂ���̂��X�V����B
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_BEFORE_CULLING          =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE                 ,

    // Calculate�ɂ�����BEFORE_CULLING�ōs���Ă��Ȃ����̂��X�V����B
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_AFTER_CULLING           =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL                 \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                       ,

    // Scale,Rotate,Translate�͊����ɍX�V����
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SRT_PERFECTLY           =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE                 ,

    // �X�P���^���A�j���[�V�������܂ރ��f����SRT�����Ő���ɍX�V����
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_EXACTLY        =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL                 \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // �}�e���A���A�j���[�V�������܂ރ��f����SRT�����Ő���ɍX�V����
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_MATERIAL_EXACTLY        =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // �r�W�r���e�B�A�j���[�V�������܂ރ��f����SRT�����Ő���ɍX�V����
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_VISIBILITY_EXACTLY      =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     ,

    // �J�����A�j���[�V�������܂ރJ������SRT�����Ő���ɍX�V����
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_CAMERA_EXACTLY          =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // ���C�g�A�j���[�V�������܂ރ��C�g��SRT�����Ő���ɍX�V����
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_LIGHT_EXACTLY           =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // �p�[�e�B�N����SRT�����Ő���ɍX�V����
        // @note �p�[�e�B�N���Ń}�e���A���A�j���[�V�������s���Ȃ�A���̃t���O�𗧂Ă�K�v������B
        // ����g3d�̃p�[�e�B�N��������ɑΉ����Ă��Ȃ��̂ŗ��ĂĂ��Ȃ��B
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE_EXACTLY        =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                       ,
  };
/*
  //////// Calculate�̎��s����鏇�� ////////

  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE 
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE 
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE             
  
      **** �J�����O�͂����ɓ��� ****

  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING 
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING 
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL             
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                       
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                   
*/



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �V�[���`��O���[�v
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //-------------------------------------
  //
  /// �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O
  //
  //=====================================
  enum
  {
    // �Ȃ�
    SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_NONE      = 0,           // �������Ȃ��B

    // �P��
    SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE   = 0x1  <<  0,  // SubmitView�ɂ����ă����_�[�L���[�ɐςށB
    SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT      = 0x1  <<  1,  // SubmitView�ɂ����ă����_�[�L���[���\�[�g����B
    
    // ����
    SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_DEFAULT   =   SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE \
                                                 | SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT
                                                     // �f�t�H���g�p�̒ʏ�ݒ�BSubmitView�ɂ����ă����_�[�L���[�ɐς݃\�[�g����B
  };

  //-------------------------------------
  //
  /// �V�[���`��O���[�v�̃����_�[�L���[���}�[�W������@
  //
  //=====================================
  // (A, B, method)  �u�����_�[�L���[B�v���u�����_�[�L���[A�v�Ɂu���@method�v�Ń}�[�W����B
  // �}�[�W��A�y�u�VA�v�z�́y�u��A�v�ƁuB�v�����킹�����́z�ɂȂ�A�uB�v�́uB�v�̂܂ܕύX����Ȃ��B
  enum SceneDrawGroupMergeMethod
  {
    SCENE_DRAW_GROUP_MERGE_METHOD_HEAD,           // B��A�̑O�ɒu���B(�\�[�g�͂��Ȃ��̂ŁAA,B�̓\�[�g���Ă�������)
    SCENE_DRAW_GROUP_MERGE_METHOD_TAIL,           // B��A�̌�ɒu���B(�\�[�g�͂��Ȃ��̂ŁAA,B�̓\�[�g���Ă�������)
    SCENE_DRAW_GROUP_MERGE_METHOD_SORTED_MIX,     // �\�[�g�ς݂�B���\�[�g�ς�A�ɍ�����B(�VA�̓\�[�g���ꂽ���̂ɂȂ�)
                                                  // (A,B���\�[�g����Ă��Ȃ��ꍇ�A����͕ۏႳ��Ȃ�)
    SCENE_DRAW_GROUP_MERGE_METHOD_MIX_NEED_SORT   // B��A�ɍ����A�\�[�g����B(�\�[�g�������̂�A,B�̓\�[�g����Ă��Ȃ��Ă��悢)
  };



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // �\���̐錾
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
public:
  //-------------------------------------
  //
  /// �V�[���̐ݒ���e
  //
  //=====================================
  struct Description  // max�ƕt���Ă���̂͊Ǘ�����ő吔�A�t���Ă��Ȃ��̂͂��̌�����
  {
    u32                                  own_model_num_max;
    u32                                  own_light_num_max;
    u32                                  own_camera_num_max;
    u32                                  own_fog_num_max;
    u32                                  own_scene_environment_setting_num_max;
    u32                                  user_model_num_max;
    u32                                  user_light_num_max;
    u32                                  user_camera_num_max;
    u32                                  user_fog_num_max;
    u32                                  user_particle_num_max;
    SceneSystem::Description*            scene_system_description;
    RenderSystem::Description*           render_system_description;
    u32                                  scene_calculate_group_num;
    SceneCalculateGroup::Description*    scene_calculate_group_description_array;  // scene_calculate_group_num���̔z���������NULL��n���ĉ������B
        // ��
        // gfl::grp::g3d::Scene::Description                scene_description;
        // gfl::grp::g3d::SceneCalculateGroup::Description  scene_calculate_group_description_array[3];
        // scene_description.scene_calculate_group_num               = 3;
        // scene_description.scene_calculate_group_description_array = scene_calculate_group_description_array;
    u32                                  scene_draw_group_num;
    SceneDrawGroup::Description*         scene_draw_group_description_array;  // scene_draw_group_num���̔z���������NULL��n���ĉ������B

    // �R���X�g���N�^
    Description(void)
      : own_model_num_max                          (256),
        own_light_num_max                          (8),
        own_camera_num_max                         (8),
        own_fog_num_max                            (8),
        own_scene_environment_setting_num_max      (4),
        user_model_num_max                         (256),
        user_light_num_max                         (8),
        user_camera_num_max                        (8),
        user_fog_num_max                           (8),
        user_particle_num_max                      (8),
        scene_system_description                   (NULL),
        render_system_description                  (NULL),
        scene_calculate_group_num                  (1),
        scene_calculate_group_description_array    (NULL),
        scene_draw_group_num                       (2),
        scene_draw_group_description_array         (NULL)
    {}
  };



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // ���Jstatic�֐�
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
public:
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �J�����O����X�P�[��
   * @param scale     �X�P�[��
   */
  //-----------------------------------------------------------------------------
  static void SetViewCullBoxScale( f32 scale );
  //-----------------------------------------------------------------------------
  /**
   * @brief ���f���̎���J�����O
   * @param r_model     �Ώۃ��f��
   * @param r_camera    �ΏۃJ����
   * @return            �������
   */
  //-----------------------------------------------------------------------------
  static bool ViewCullingModel( const Model &r_model, const Camera &r_camera );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief AABB�̎���J�����O
   * @param aabb        �Ώۂ`�`�a�a
   * @param r_camera    �ΏۃJ����
   * @return            �������
   */
  //-----------------------------------------------------------------------------
  static bool ViewCullingAABB( const gfl::math::AABB &aabb, const Camera &r_camera, const nn::math::MTX34 &worldMat );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �X�N���[�����W�n�ł̂`�`�a�a�擾
   * @param p_dest_aabb     �o�͐�
   * @param r_src_aabb      �Ώۂ`�`�a�a
   * @param r_camera        �ΏۃJ����
   */
  //-----------------------------------------------------------------------------
  static void GetScreanCoordinatesAABB( gfl::math::AABB *p_dest_aabb, const gfl::math::AABB &r_src_aabb, const Camera &r_camera, const nn::math::MTX34 &worldMat );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �X�N���[�����W�n�ł̃��f���`�`�a�a�擾
   * @param p_dest_aabb       �o�͐�
   * @param r_model           �Ώۃ��f��
   * @param r_camera          �ΏۃJ����
   *
   * @retval  true  �����AABB
   * @retval  false �s����AABB
   */
  //-----------------------------------------------------------------------------
  static bool GetScreanCoordinatesModelAABB( gfl::math::AABB *p_dest_aabb, const Model &r_model, const Camera &r_camera );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �X�N���[�����W�n�ł̃��f���`�`�a�a�擾
   * @param p_dest_aabb       �o�͐�
   * @param r_h3d_model           �Ώۃ��f��
   * @param r_camera          �ΏۃJ����
   *
   * @retval  true  �����AABB
   * @retval  false �s����AABB
   */
  //-----------------------------------------------------------------------------
  static bool GetScreanCoordinatesModelAABB( gfl::math::AABB *p_dest_aabb, const H3dModel &r_h3d_model, const Camera &r_camera );


#if GFL_GRP_G3D_H3D_USE
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief ���f���̎���J�����O
   * @param r_model     �Ώۃ��f��
   * @param r_camera    �ΏۃJ����
   * @return            �������
   */
  //-----------------------------------------------------------------------------
  static bool ViewCullingH3dModel( const H3dModel &r_model, const Camera &r_camera );
#endif  // GFL_GRP_G3D_H3D_USE


  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // ���J�֐�
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �R���X�g���N�^ / �f�X�g���N�^
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  Scene(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create���Ăт܂��B
   *
   *  @param[in]     device_allocator    �f�o�C�X�A���P�[�^
   *  @param[in]     description  �V�[���̐ݒ���e
   *                              (NULL�̂Ƃ��̓f�t�H���g�l�B
   *                              �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     heap_allocator      �q�[�v�A���P�[�^(NULL�̂Ƃ���device_allocator���g�p���܂�)
   */
  //-----------------------------------------------------------------------------
  Scene( gfl::heap::NwAllocator* device_allocator, const Description* description = NULL, gfl::heap::NwAllocator* heap_allocator = NULL );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~Scene(); 



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �f�o�b�O�@�\
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  void DebugTickSetEnableFlag(b32 flag);        // true�̂Ƃ��v������Afalse�̂Ƃ��v���Ȃ�
  void DebugTickSetDumpAndResetFlag(b32 flag);  // true�̂Ƃ��_���v�����Z�b�g����Afalse�̂Ƃ��Ȃ�
  b32  DebugTickGetDumpAndResetFlag(void);
  
  //----------------------------------------------------------------------------
  /**
   *	@brief    UpdateScene�ɂ����������Ԃ����O�o��
   */
  //-----------------------------------------------------------------------------
  void DebugUpdateSceneTickOutPut(void);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // ���� / �j��
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     device_allocator    �f�o�C�X�A���P�[�^
   *  @param[in]     description  �V�[���̐ݒ���e
   *                              (NULL�̂Ƃ��̓f�t�H���g�l�B
   *                              �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     heap_allocator      �q�[�v�A���P�[�^(NULL�̂Ƃ���device_allocator���g�p���܂�)
   */
  //-----------------------------------------------------------------------------
  void Create(gfl::heap::NwAllocator* device_allocator, const Description* description = NULL, gfl::heap::NwAllocator* heap_allocator = NULL );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);



 
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // Initialize
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����������
   *                 (�I�������͂Ȃ�)
   */
  //-----------------------------------------------------------------------------
  // �O������Ă΂�鏉��������
  void Initialize(const SceneInitializeProcess scene_initialize_process = SCENE_INITIALIZE_PROCESS_ALL); 
  void Initialize(const s32 scene_calculate_group_index, const SceneInitializeProcess scene_initialize_process = SCENE_INITIALIZE_PROCESS_ALL);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �v�Z����
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z����
   */
  //-----------------------------------------------------------------------------
  // �v�Z���� 
  virtual void Calculate(void);  // �S��(�p�[�e�B�N�����܂�)�̌v�Z�������s��
  virtual void Calculate(s32 scene_calculate_group_index);  // scene_calculate_group_index�ɂ���S��(�p�[�e�B�N�����܂�)�̌v�Z�������s��

  // �p�[�e�B�N���̃m�[�h�Ɋւ���Calculate�����s��(SceneUpdater���������s���āAParticleSceneUpdater�����͍s��Ȃ�)
  void CalculateParticleNode(Particle* particle = NULL);  // particle��NULL�̂Ƃ�Scene�ɓo�^����Ă���S�Ẵp�[�e�B�N���ɑ΂��čs��  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
  void CalculateParticleNode(s32 scene_calculate_group_index, Particle* particle = NULL);  // particle��NULL�̂Ƃ�scene_calculate_group_index�ɓo�^����Ă���S�Ẵp�[�e�B�N���ɑ΂��čs��  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B

  // �p�[�e�B�N�������t���[����i�߂�
  void RepeatCalculateParticle(s32 repeat_count, Particle* particle = NULL);  // �p�[�e�B�N�������v�Z�������s��  // particle��NULL�̂Ƃ�Scene�ɓo�^����Ă���S�Ẵp�[�e�B�N���ɑ΂��čs��  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
  void RepeatCalculateParticle(s32 repeat_count, s32 scene_calculate_group_index, Particle* particle = NULL);  // �p�[�e�B�N�������v�Z�������s��  // particle��NULL�̂Ƃ�scene_calculate_group_index�ɓo�^����Ă���S�Ẵp�[�e�B�N���ɑ΂��čs��  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B

  // �J�����O
  void CalculateCulling(Camera* camera, bool update_camera_matrix = true);
  void CalculateCulling(s32 scene_calculate_group_index, Camera* camera, bool update_camera_matrix = true);
  void ResetCalculateCullingFlag(void);
  void ResetCalculateCullingFlag(s32 scene_calculate_group_index);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O��ݒ肷��
   *
   *  @param[in]  scene_calculate_group_enable_bit_flag_array  �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O�̔z��(�V�[���v�Z�O���[�v�S�Ă̕�)
   *
   *      bit32 bit_flag_array[calc_group_num] =
   *      {
   *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_NONE,                       // calc_group_0
   *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE,      // calc_group_1
   *           gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE \
   *         | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM           \
   *         | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM,            // calc_group_2
   *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_DEFAULT                     // calc_group_3
   *      };
   */
  //-----------------------------------------------------------------------------
  void SetSceneCalculateGroupEnableBitFlagArray(bit32* scene_calculate_group_enable_bit_flag_array);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O��ݒ肷��
   *
   *  @param[in]  scene_calculate_group_index            �V�[���v�Z�O���[�v
   *  @param[in]  scene_calculate_group_enable_bit_flag  �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O
   *                  ��
   *                  scene_calculate_group_enable_bit_flag = \
   *                        gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE \
   *                      | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM           \
   *                      | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM
   */
  //-----------------------------------------------------------------------------
  void SetSceneCalculateGroupEnableBitFlag(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O���擾����
   *
   *  @param[in]  scene_calculate_group_index            �V�[���v�Z�O���[�v
   *
   *  @retval     �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O
   */
  //-----------------------------------------------------------------------------
  bit32 GetSceneCalculateGroupEnableBitFlag(s32 scene_calculate_group_index) const;



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // SubmitView
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////    ��SubmitView����x�ɍs�����߂̊֐���    //////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t�H�O�̍X�V���s��
   *
   *  @param[in]    camera    �����œn�����J��������̋����Ńt�H�O�̔Z���𒲐����܂�
   *
   *  �y���ӎ����z
   *      (1) �t�H�O�N���X�̊��N���X�ł���V�[���m�[�h�̍X�V�Ɋւ���
   *          �t�H�O�N���X�̊��N���X�ł���V�[���m�[�h�̍X�V��Calculate�ōs����̂ŁA���̊֐����Ăяo���O��Calculate�͍ς܂��Ă����ĉ������B
   *      (2) �J�����Ɋւ���
   *          �t�H�O�̍X�V�Ɏg���J�����̃J�����}�g���N�X�̍X�V�́A���̊֐����Ăяo���O�ɍς܂��Ă����ĉ������B(�J�����̃}�g���N�X�̍X�V�̑O�ɁACalculate���ς܂��Ă����K�v������܂��B)
   */
  //-----------------------------------------------------------------------------
  void UpdateFog(Camera* camera);  // �S�ẴV�[���v�Z�O���[�v�̃t�H�O�̍X�V���s��
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�Ŏg�p����J������S�Đݒ肷��
   *
   *  @param[in]    scene_draw_group_camera_array    �V�[���`��O���[�v�̔z��ʂ�ɃJ������ݒ肵���z��(�V�[���`��O���[�v�S�Ă̕�)
   *                                                 
   *                                                 ��
   *                                                 Camera* camera_a;  // for draw_group_0, draw_group_1
   *                                                 Camera* camera_b;  // for draw_group_2
   *
   *                                                 Camera* camera_array[draw_group_num] =
   *                                                 {
   *                                                   camera_a,  // draw_group_0
   *                                                   camera_a,  // draw_group_1
   *                                                   camera_b   // draw_group_2
   *                                                 };
   *
   *  �y���ӎ����z
   *      (1) �����Őݒ肵���J�������g�p�����̂́E�E�E
   *          �����Őݒ肵���J�������g�p�����̂́ASubmitViewAll�ł݂̂ł��B
   *          �ݒ肵�Ă������J�������j������Ă�SubmitViewAll���Ă΂Ȃ���Εs��������邱�Ƃ͂���܂��񂪁A
   *          �ݒ肵�Ă������J������j������Ƃ��͈��S�̂���NULL��ݒ肷��悤�ɂ��ĉ������B
   */
  //-----------------------------------------------------------------------------
  void SetSubmitViewCameraArray(const Camera** scene_draw_group_camera_array);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�Ŏg�p����J������ݒ肷��
   *
   *  @param[in]    scene_draw_group_index    scene_draw_group_camera��ݒ肷��V�[���`��O���[�v
   *  @param[in]    scene_draw_group_camera   scene_draw_group_index�ɐݒ肷��J����
   *
   *  �y���ӎ����z
   *      (1) �����Őݒ肵���J�������g�p�����̂́E�E�E
   *          �����Őݒ肵���J�������g�p�����̂́ASubmitViewAll�ł݂̂ł��B
   *          �ݒ肵�Ă������J�������j������Ă�SubmitViewAll���Ă΂Ȃ���Εs��������邱�Ƃ͂���܂��񂪁A
   *          �ݒ肵�Ă������J������j������Ƃ��͈��S�̂���NULL��ݒ肷��悤�ɂ��ĉ������B
   */
  //-----------------------------------------------------------------------------
  void SetSubmitViewCamera(s32 scene_draw_group_index, const Camera* scene_draw_group_camera);
 
  // �V�[���`��O���[�v�Ŏg�p����J�������擾����
  const Camera* GetSubmitViewCamera(s32 scene_draw_group_index) const;
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O��ݒ肷��
   *
   *  @param[in]  scene_draw_group_enable_bit_flag_array  �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O�̔z��(�V�[���`��O���[�v�S�Ă̕�)
   *
   *      ��
   *      bit32 bit_flag_array[draw_group_num] =
   *      {
   *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_NONE,                                                                   // draw_group_0
   *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE,                                                                // draw_group_1
   *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE | gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT,  // draw_group_2
   *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_DEFAULT                                                                 // draw_group_3
   *      };
   *
   *  �y���ӎ����z
   *      (1) �����Őݒ肵���t���O���g�p�����̂́E�E�E
   *          �����Őݒ肵���t���O���g�p�����̂́ASubmitViewAll�ł݂̂ł��B
   *              **** �ʏ��SubmitView�̂ɂ��K�p���Ă��悢���ǂ����悤�E�E�E ****
   */
  //-----------------------------------------------------------------------------
  void SetSceneDrawGroupEnableBitFlagArray(bit32* scene_draw_group_enable_bit_flag_array);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O��ݒ肷��
   *
   *  @param[in]  scene_draw_group_index            �V�[���`��O���[�v
   *  @param[in]  scene_draw_group_enable_bit_flag  �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O
   *                  ��
   *                  scene_draw_group_enable_bit_flag = \
   *                      gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE | gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT;
   *
   *  �y���ӎ����z
   *      (1) �����Őݒ肵���t���O���g�p�����̂́E�E�E
   *          �����Őݒ肵���t���O���g�p�����̂́ASubmitViewAll�ł݂̂ł��B
   *              **** �ʏ��SubmitView�̂ɂ��K�p���Ă��悢���ǂ����悤�E�E�E ****
   */
  //-----------------------------------------------------------------------------
  void SetSceneDrawGroupEnableBitFlag(s32 scene_draw_group_index, bit32 scene_draw_group_enable_bit_flag);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O���擾����
   *
   *  @param[in]  scene_draw_group_index            �V�[���`��O���[�v
   *
   *  @retval     �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O
   *
   *  �y���ӎ����z
   *      (1) �����Ŏ擾�����t���O���g�p����Ă���̂́E�E�E
   *          �����Ŏ擾�����t���O���g�p����Ă���̂́ASubmitViewAll�ł݂̂ł��B
   *              **** �ʏ��SubmitView�̂ɂ��K�p���Ă��悢���ǂ����悤�E�E�E ****
   */
  //-----------------------------------------------------------------------------
  bit32 GetSceneDrawGroupEnableBitFlag(s32 scene_draw_group_index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�S�Ă�SubmitView����x�ɍs��
   *
   *  �y���ӎ����z
   *      (1) �J�����Ɋւ���
   *          ���̊֐����Ăяo���O�ɁASetSubmitViewCameraArray��SetSubmitViewCamera�ŁA�V�[���`��O���[�v�S�Ăɑ΂��Ďg�p����J������ݒ肵�Ă����ĉ������B
   *          ��L�ɂĐݒ肵�Ă������J�����̃J�����}�g���N�X�̍X�V�́A���̊֐����Ăяo���O�ɍς܂��Ă����ĉ������B(�J�����̃}�g���N�X�̍X�V�̑O�ɁACalculate���ς܂��Ă����K�v������܂��B)
   *      (2) �t�H�O�Ɋւ���
   *          ���̊֐����Ăяo���O�ɁAUpdateFog�Ńt�H�O�̍X�V���ς܂��Ă����ĉ������B
   */
  //-----------------------------------------------------------------------------
  void SubmitViewAll(void);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�̃����_�[�L���[���}�[�W����
   * 
   *  @param[in]  scene_draw_group_index_A  �V�[���`��O���[�v�BB���ǉ�������̂ŕύX����܂��B
   *  @param[in]  scene_draw_group_index_B  �V�[���`��O���[�v�B�ύX����܂���B
   *  @param[in]  method                    �}�[�W���@
   *
   *  �y���ӎ����z
   *      (1) A��B�𓯂��ɂ��Ȃ��ŉ������B
   *      (2)�u�����_�[�L���[B�v���u�����_�[�L���[A�v�Ɂu���@method�v�Ń}�[�W����B
   *          �}�[�W��A�y�u�VA�v�z�́y�u��A�v�ƁuB�v�����킹�����́z�ɂȂ�A�uB�v�́uB�v�̂܂ܕύX����Ȃ��B
   */
  //-----------------------------------------------------------------------------
  void MergeRenderQueue(s32 scene_draw_group_index_A, const s32 scene_draw_group_index_B, const SceneDrawGroupMergeMethod method);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////    ��SubmitView����x�ɍs�����߂̊֐���    //////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         SubmitView
   *
   *  @param[in]     camera                  �J����
   *  @param[in]     update_camera_matrix    camera�̃}�g���N�X���X�V����ꍇtrue
   *  @param[in]     updata_for              �t�H�O���X�V����ꍇtrue
   *  @param[in]     scene_draw_group_index  �V�[���`��O���[�v�̃C���f�b�N�X
   * 
   *  camera�� gfl::grp::g3d::Camera* camera = GFL_NEW() StereoCamera; �Ƃ����ӂ��ɐ��������J������n�����ꍇ�A
   *  Camera* camera �������Ɏ��������SubmitView���Ă΂�邽�߁A
   *  update_camera_matrix��true�̂Ƃ��x�[�X�J���������X�V����āA���ڗp�J�����A�E�ڗp�J�������X�V����Ȃ����ۂ��N���܂��B
   *  ���Ώ��ς݁Aupdate_camera_matrix��true�̂Ƃ��x�[�X�J���������ڗp�J�������E�ڗp�J�������X�V���Afalse�̂Ƃ�3�Ƃ��X�V���Ȃ��B
   */
  //-----------------------------------------------------------------------------
  void SubmitView(
      Camera* camera,
      bool update_camera_matrix = true,
      bool update_fog = true,
      s32 scene_draw_group_index = 0);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // Draw
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     display_type           �\����
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void Draw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera,
    bool                          command_cache_dump = false
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �X�e���I�`�揈��
   *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void StereoDraw(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera,
    bool                         command_cache_dump = false
  );

  // Draw 
  void Draw(
      gfl::grp::GraphicsSystem* graphics_system,
      Camera* camera,
      gfl::grp::RenderTarget* render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );

/*
  h3d�ŃT�|�[�g������Ȃ��̂ŃR�����g�A�E�g�B
  // �R�}���h�Z�[�u��������Draw���Ȃ�
  void CommandSaveNotDraw(
      gfl::grp::GraphicsSystem* graphics_system,
      gfl::grp::RenderTarget* render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );

  // �R�}���h�Z�[�u���Ă�����̂��g����Draw
  void ReuseCommandSaveDraw(
      gfl::grp::GraphicsSystem* graphics_system,
      Camera* camera,
      gfl::grp::RenderTarget* render_target);
*/

  // �R�}���h�Z�[�u���Ȃ���StereoDraw
  void StereoDraw(
      GraphicsSystem* graphics_system,
      Camera* left_camera, 
      gfl::grp::RenderTarget* left_render_target,
      Camera* right_camera,
      gfl::grp::RenderTarget* right_render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );
  void StereoDraw(
      GraphicsSystem* graphics_system,
      StereoCamera* camera,
      gfl::grp::RenderTarget* left_render_target,
      gfl::grp::RenderTarget* right_render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );

  // �R�}���h�Z�[�u����StereoDraw
private:
  void CommandSaveStereoDraw(
      gfl::grp::GraphicsSystem* graphics_system, 
      Camera* left_camera, 
      gfl::grp::RenderTarget* left_render_target,
      Camera* right_camera,
      gfl::grp::RenderTarget* right_render_target,
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      s32 scene_draw_group_index,
      Camera* base_camera,
      bool command_cache_dump = false
#else
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false
#endif
      );
public:
  void CommandSaveStereoDraw(
      gfl::grp::GraphicsSystem* graphics_system, 
      StereoCamera* camera,
      gfl::grp::RenderTarget* left_render_target,
      gfl::grp::RenderTarget* right_render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );

/*
  h3d�ŃT�|�[�g������Ȃ��̂ŃR�����g�A�E�g�B
  // �R�}���h�Z�[�u���Ă�����̂��g����StereoDraw
  void ReuseCommandSaveStereoDraw(
      gfl::grp::GraphicsSystem* graphics_system, 
      Camera* left_camera, 
      gfl::grp::RenderTarget* left_render_target,
      Camera* right_camera,
      gfl::grp::RenderTarget* right_render_target );
  void ReuseCommandSaveStereoDraw(
      gfl::grp::GraphicsSystem* graphics_system, 
      StereoCamera* camera,
      gfl::grp::RenderTarget* left_render_target,
      gfl::grp::RenderTarget* right_render_target );
*/

#if GFL_GRP_G3D_H3D_USE
private:
  void drawH3dModel(
      gfl::grp::GraphicsSystem* graphics_system, 
      Camera* camera,
      gfl::grp::RenderTarget* render_target,
      s32 scene_draw_group_index
  );
  void commandSaveStereoDrawH3dModel(
      gfl::grp::GraphicsSystem* graphics_system, 
      Camera* left_camera,
      gfl::grp::RenderTarget* left_render_target,
      Camera* right_camera,
      gfl::grp::RenderTarget* right_render_target,
      s32 scene_draw_group_index
  );
#endif



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �V�[���m�[�h�� �ǉ� / �폜
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // 
  // �����ߊ֐�
  // 
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // AddNodeToCalc���Ă񂾌�AAddNodeToDraw���ĂԊ֐� 
  b32 AddNodeToCalcAndDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index);
  // AddNodeToCalc���Ă񂾌�AAddNodeToExDraw���ĂԊ֐� 
  b32 AddNodeToCalcAndExDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index);
  // AddNodeToCalc���Ă񂾌�AAddNodeToAllDraw���ĂԊ֐� 
  b32 AddNodeToCalcAndAllDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index);

  // scene_node��scene_calculate_group_index�ɓ����
  // ���ɑ��̃V�[���v�Z�O���[�v�ɓ����Ă���ꍇ�͈ړ�����B
  // �V�[���`��O���[�v�ɑ΂��Ă͉������Ȃ��B
  // �V�K�ł��ړ��ł�scene_node��scene_calculate_group_index�ɓ�����Ȃ�������false��Ԃ��B
  // �ړ��̍ۂ́A���X�����Ă����V�[���v�Z�O���[�v�����菜���Ă���scene_calculate_group_index�ɓ����̂ŁA
  // ��菜���̂͂��܂�����scene_calculate_group_index�ɓ����̂������s�����ꍇ�́A
  // �ǂ̃V�[���v�Z�O���[�v�ɂ������Ă��Ȃ���ԂɂȂ���false��Ԃ��B���̂悤�Ȃǂ̃V�[���v�Z�O���[�v�ɂ������Ă��Ȃ���ԂɂȂ����Ƃ��́A
  // �S�ẴV�[���`��O���[�v����������(�V�[���v�Z�O���[�v�ɓ����Ă��Ȃ��Ƃ��̓V�[���`��O���[�v�ɂ͓o�^����Ȃ��A�Ƃ������[������邽��)�B
  b32 AddNodeToCalc(SceneNode* scene_node, s32 scene_calculate_group_index);

  // scene_node��scene_draw_index�ɓ����
  // ���ɑ��̃V�[���`��O���[�v�ɓ����Ă���ꍇ�́Ascene_draw_group_index�ɂ������̂ŕ����ɓo�^����邱�ƂɂȂ�B
  // �V�[���v�Z�O���[�v�ɑ΂��Ă͉������Ȃ��B
  // scene_node��scene_draw_index�ɓ�����Ȃ�������false��Ԃ��B
  // ���X�����Ă����V�[���`��O���[�v�ɑ΂��Ă͉������Ȃ��B
  // �V�[���v�Z�O���[�v�ɓ����Ă��Ȃ��Ƃ��́A�V�[���`��O���[�v�ɂ͓o�^����Ȃ��̂�false��Ԃ��B
  b32 AddNodeToDraw(SceneNode* scene_node, s32 scene_draw_group_index);
  // scene_node��r���I��scene_draw_index�ɓ����
  // ���ɑ��̃V�[���`��O���[�v�ɓ����Ă���ꍇ�́A���̓o�^��S�ĉ�����scene_draw_group_index�ɂ��������B
  // �V�[���v�Z�O���[�v�ɑ΂��Ă͉������Ȃ��B
  // scene_node��scene_draw_index�ɓ�����Ȃ�������false��Ԃ��B
  // ���X�����Ă����V�[���`��O���[�v�����菜���Ă���scene_draw_group_index�ɓ����̂ŁA
  // ��菜���̂͂��܂�����scene_draw_group_index�ɓ����̂������s�����ꍇ�́A
  // �ǂ̃V�[���`��O���[�v�ɂ������Ă��Ȃ���ԂɂȂ���false��Ԃ��B
  // �V�[���v�Z�O���[�v�ɓ����Ă��Ȃ��Ƃ��́A�V�[���`��O���[�v�ɂ͓o�^����Ȃ��̂�false��Ԃ��B
  b32 AddNodeToExDraw(SceneNode* scene_node, s32 scene_draw_group_index);
  // scene_node��S�ẴV�[���`��O���[�v�ɓ����
  // �V�[���v�Z�O���[�v�ɑ΂��Ă͉������Ȃ��B
  // 1�ł�������Ȃ��V�[���`��O���[�v���������Ƃ���false��Ԃ����A�������Ƃ���ɂ͓��ꂽ��ԂŖ߂�B
  // �V�[���v�Z�O���[�v�ɓ����Ă��Ȃ��Ƃ��́A�V�[���`��O���[�v�ɂ͓o�^����Ȃ��̂�false��Ԃ��B
  b32 AddNodeToAllDraw(SceneNode* scene_node);
  
  // scene_node���V�[���v�Z�O���[�v�ƑS�ẴV�[���`��O���[�v�����������
  void RemoveNodeFromCalcAndAllDraw(SceneNode* scene_node);
  // scene_node���V�[���v�Z�O���[�v�ƑS�ẴV�[���`��O���[�v�����������  // RemoveNodeFromCalcAndAllDraw�ƒ��g�͓����B
  void RemoveNodeFromCalc(SceneNode* scene_node);                          // ModelNodeProperty�łȂ���΃V�[���`��O���[�v�ɂ͓����Ă��Ȃ��̂�
                                                                           // ���̖��O�̊֐��������Ă������Ƃɂ����B
  // scene_node��scene_draw_group_index�����������
  // scene_draw_group_index�ȊO�̃V�[���`��O���[�v�ɂ������Ă���ꍇ�́A���̃V�[���`��O���[�v�ɂ͎c��B
  // �V�[���v�Z�O���[�v�ɑ΂��Ă͉������Ȃ��B
  void RemoveNodeFromDraw(SceneNode* scene_node, s32 scene_draw_group_index);
  // scene_node��S�ẴV�[���`��O���[�v�����������
  // �V�[���v�Z�O���[�v�ɑ΂��Ă͉������Ȃ��B
  void RemoveNodeFromAllDraw(SceneNode* scene_node);

  // scene_calculate_group_index�ɓo�^����Ă���S�ẴV�[���m�[�h���Ascene_calculate_group_index�ƑS�ẴV�[���`��O���[�v�����������
  void RemoveAllNodeFromCalcAndAllDraw(s32 scene_calculate_group_index);
  // scene_draw_group_index�ɓo�^����Ă���S�ẴV�[���m�[�h���Ascene_draw_group_index�����������
  // scene_draw_group_index�ɓo�^����Ă���e�V�[���m�[�h��scene_draw_group_index�ȊO�̃V�[���`��O���[�v�ɂ������Ă���ꍇ�́A
  // ���̃V�[���`��O���[�v�ɂ͎c��B
  // �V�[���v�Z�O���[�v�ɑ΂��Ă͉������Ȃ��B
  void RemoveAllNodeFromDraw(s32 scene_draw_group_index);
  // �S�ẴV�[���v�Z�O���[�v�ƑS�ẴV�[���`��O���[�v����S�ẴV�[���m�[�h����������B
  // �����A�V�[���ɓo�^����Ă���S�ẴV�[���m�[�h���V�[�������������
  void RemoveAllNodeFromAllCalcAndAllDraw(void);

#if GFL_GRP_G3D_H3D_USE
  // scene_node���V�[���v�Z�O���[�vscene_calculate_group_index�ɒǉ�����
  b32  AddH3dModelToCalc(H3dModel* scene_node, s32 scene_calculate_group_index);
  
  // scene_node���V�[���`��O���[�vscene_draw_group_index�ɒǉ�����
  b32  AddH3dModelToDraw(H3dModel* scene_node, s32 scene_draw_group_index);

  //   scene_node��o�^���Ă���V�[���v�Z�O���[�v�����菜��
  // & scene_node��o�^���Ă���S�ẴV�[���`��O���[�v�����菜��
  void RemoveH3dModelFromCalcAndAllDraw(H3dModel* scene_node);

  // scene_node���V�[���`��O���[�vscene_draw_group_index�����菜��
  void RemoveH3dModelFromDraw(H3dModel* scene_node, s32 scene_draw_group_index);
  
  //   �V�[���v�Z�O���[�vscene_calculate_group_index�ɓo�^����Ă���S�Ă�H3dModel����菜��
  // & ��菜����H3dModel�͓o�^���Ă������S�ẴV�[���`��O���[�v�������菜�����
  void RemoveAllH3dModelFromCalcAndAllDraw(s32 scene_calculate_group_index);

  //   �S�Ă�H3dModel��S�ẴV�[���v�Z�O���[�v�����菜��
  // & �S�Ă�H3dModel��S�ẴV�[���`��O���[�v�����菜��
  void RemoveAllH3dModelFromAllCalcAndAllDraw(void);
#endif  // GFL_GRP_G3D_H3D_USE


  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // 
  // �]���ʂ�̊֐�
  // 
  /////////////////////////////////////////////
  /////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v�ɃV�[���m�[�h��o�^����B
   *
   *                 [�V�[���v�Z�O���[�v]
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A�V�[���v�Z�O���[�vscene_calculate_group_index�Ԃɓo�^�����B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A�V�[���v�Z�O���[�vscene_calculate_group_index�ԂɕύX�����B
   *                 �V�[���v�Z�O���[�v��1�ɂ����o�^�ł��Ȃ��B
   *                 
   *                 [�V�[���`��O���[�v]
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A�V�[���`��O���[�v0�Ԃɂ��������I�ɓo�^�����B
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A���̃V�[���`��O���[�v�ɓo�^�ς݂̏ꍇ�ł��A�V�[���`��O���[�v0�Ԃ����ɂȂ�B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A�V�[���`��O���[�v�͕ύX����Ȃ��B
   *  
   *  @param[in]     scene_node                     �o�^����V�[���m�[�h
   *  @param[in]     scene_calculate_group_index    scene_node������V�[���v�Z�O���[�v
   *
   *  @retval        b32   �o�^�ł�����true
   */
  //-----------------------------------------------------------------------------
  b32  AddSceneNode( SceneNode* scene_node, s32 scene_calculate_group_index = 0 );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v����V�[���m�[�h�̓o�^����������B
   *                 ���̃V�[���m�[�h���o�^����Ă���S�ẴV�[���`��O���[�v��������������B
   *  
   *  @param[in]     scene_node    �o�^����������V�[���m�[�h
   *  
   *  @retval        b32   �����ł�����true
   */
  //-----------------------------------------------------------------------------
  b32  RemoveSceneNode( SceneNode* scene_node );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v����S�ẴV�[���m�[�h�̓o�^����������B
   *                 �V�[���`��O���[�v������S�ĉ��������B
   *  
   *  @retval        b32   true�����Ԃ��Ȃ��B(�S�ĉ����ł�����true�A�����ł��Ȃ����̂�1�ł���������false�A�Ƃ����ӂ��ɂ͂ł��Ȃ�)
   */
  //-----------------------------------------------------------------------------
  b32  RemoveAllSceneNodes(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v�ɃV�[���m�[�h��o�^����B
   *
   *                 [�V�[���v�Z�O���[�v]
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A�V�[���v�Z�O���[�vscene_calculate_group_index�Ԃɓo�^�����B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A�V�[���v�Z�O���[�vscene_calculate_group_index�ԂɕύX�����B
   *                 �V�[���v�Z�O���[�v��1�ɂ����o�^�ł��Ȃ��B
   *                 
   *                 [�V�[���`��O���[�v]
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A�V�[���`��O���[�v0�Ԃɂ��������I�ɓo�^�����B
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A���̃V�[���`��O���[�v�ɓo�^�ς݂̏ꍇ�ł��A�V�[���`��O���[�v0�Ԃ����ɂȂ�B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A�V�[���`��O���[�v�͕ύX����Ȃ��B
   *  
   *  @param[in]     model, light, camera, fog, particle    �o�^����V�[���m�[�h
   *  @param[in]     scene_calculate_group_index            scene_node������V�[���v�Z�O���[�v
   *
   *  @retval        b32   �o�^�ł�����true
   */
  //-----------------------------------------------------------------------------
  b32 AddModelToSceneCalculateGroup( Model*    model,    s32 scene_calculate_group_index = 0 );
  b32 AddLightToSceneCalculateGroup( Light*    light,    s32 scene_calculate_group_index = 0 );
  b32 AddCameraToSceneCalculateGroup( Camera*   camera,   s32 scene_calculate_group_index = 0 );
  b32 AddFogToSceneCalculateGroup( Fog*      fog,      s32 scene_calculate_group_index = 0 );
  b32 AddParticleToSceneCalculateGroup( Particle* particle, s32 scene_calculate_group_index = 0 );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v����V�[���m�[�h�̓o�^����������B
   *                 ���̃V�[���m�[�h���o�^����Ă���S�ẴV�[���`��O���[�v��������������B
   *  
   *  @param[in]     model, light, camera, fog, particle   �o�^����������V�[���m�[�h
   *  
   *  @retval        b32   �����ł�����true
   */
  //-----------------------------------------------------------------------------
  b32 RemoveModelFromSceneCalculateGroup( Model*    model    );
  b32 RemoveLightFromSceneCalculateGroup( Light*    light    );
  b32 RemoveCameraFromSceneCalculateGroup( Camera*   camera   );
  b32 RemoveFogFromSceneCalculateGroup( Fog*      fog      );
  b32 RemoveParticleFromSceneCalculateGroup( Particle* particle );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v�A�V�[���`��O���[�v�ɃV�[���m�[�h��o�^����B
   *
   *                 [�V�[���v�Z�O���[�v]
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A�V�[���v�Z�O���[�vscene_calculate_group_index�Ԃɓo�^�����B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A�V�[���v�Z�O���[�vscene_calculate_group_index�ԂɕύX�����B
   *                 �V�[���v�Z�O���[�v��1�ɂ����o�^�ł��Ȃ��B
   *                 
   *                 [�V�[���`��O���[�v]
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A�V�[���`��O���[�vscene_draw_group_index�Ԃɂ����o�^�����B
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A���̃V�[���`��O���[�v�ɓo�^�ς݂̏ꍇ�ł��A�V�[���`��O���[�vscene_draw_group_index�Ԃ����ɂȂ�B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A�V�[���`��O���[�vscene_draw_group_index�Ԃɓo�^�����B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A���̃V�[���`��O���[�v�ɓo�^�ς݂̂Ƃ��͕����̃V�[���`��O���[�v�ɓo�^����邱�ƂɂȂ�B
   *                 �V�[���`��O���[�v�͕����ɓo�^�ł���B
   *  
   *  @param[in]     model, particle                �o�^����V�[���m�[�h
   *  @param[in]     scene_calculate_group_index    scene_node������V�[���v�Z�O���[�v
   *  @param[in]     scene_draw_group_index         scene_node������V�[���`��O���[�v
   *
   *  @retval        b32   �o�^�ł�����true
   */
  //-----------------------------------------------------------------------------
  b32 AddModelToSceneCalculateGroupAndSceneDrawGroup( Model*  model, s32 scene_calculate_group_index = 0, s32 scene_draw_group_index = 0 );
  b32 AddParticleToSceneCalculateGroupAndSceneDrawGroup( Particle* particle, s32 scene_calculate_group_index = 0, s32 scene_draw_group_index = 0 );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v����S�ẴV�[���m�[�h�̓o�^����������B
   *                 �V�[���`��O���[�v������S�ĉ��������B
   *  
   *  @retval        b32   true�����Ԃ��Ȃ��B(�S�ĉ����ł�����true�A�����ł��Ȃ����̂�1�ł���������false�A�Ƃ����ӂ��ɂ͂ł��Ȃ�)
   */
  //-----------------------------------------------------------------------------
  b32  RemoveAllSceneNodesFromSceneCalculateGroup(void);
  //b32  RemoveAllSceneNodesFromSceneCalculateGroup(s32 scene_calculate_group_index);  // �V�[���v�Z�O���[�vscene_calculate_group�ɓo�^����Ă�����̂�����������A�Ƃ����֐����K�v���H

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�ɃV�[���m�[�h��o�^����B
   *
   *                 [�V�[���v�Z�O���[�v]
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A�V�[���v�Z�O���[�v0�ԂɎ����I�ɓo�^�����B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A�V�[���v�Z�O���[�v�͕ύX����Ȃ��B
   *                 
   *                 [�V�[���`��O���[�v]
   *                 �V�[���`��O���[�vscene_draw_group_index�Ԃɓo�^�����B
   *                 ���̃V�[���`��O���[�v�ɓo�^�ς݂̂Ƃ��͕����̃V�[���`��O���[�v�ɓo�^����邱�ƂɂȂ�B
   *                 �V�[���`��O���[�v�͕����ɓo�^�ł���B
   *  
   *  @param[in]     model, particle           �o�^����V�[���m�[�h
   *  @param[in]     scene_draw_group_index    scene_node������V�[���`��O���[�v
   *
   *  @retval        b32   �o�^�ł�����true
   */
  //-----------------------------------------------------------------------------
  b32 AddModelToSceneDrawGroup(Model* model, s32 scene_draw_group_index);
  b32 AddParticleToSceneDrawGroup(Particle* particle, s32 scene_draw_group_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �S�ẴV�[���`��O���[�v�ɃV�[���m�[�h��o�^����B
   *
   *                 [�V�[���v�Z�O���[�v]
   *                 �V�[���v�Z�O���[�v�ɖ��o�^�̏ꍇ�́A�V�[���v�Z�O���[�v0�ԂɎ����I�ɓo�^�����B
   *                 �V�[���v�Z�O���[�v�ɓo�^�ς݂̏ꍇ�́A�V�[���v�Z�O���[�v�͕ύX����Ȃ��B
   *                 
   *                 [�V�[���`��O���[�v]
   *                 �S�ẴV�[���`��O���[�v�ɓo�^�����B
   *                 �V�[���`��O���[�v�͕����ɓo�^�ł���B
   *  
   *  @param[in]     model, particle           �o�^����V�[���m�[�h
   *  @param[in]     scene_draw_group_index    scene_node������V�[���`��O���[�v
   *
   *  @retval        b32   �o�^�ł�����true
   */
  //-----------------------------------------------------------------------------
  b32 AddModelToAllSceneDrawGroup(Model* model);
  b32 AddParticleToAllSceneDrawGroup(Particle* particle);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v����V�[���m�[�h�̓o�^����������B
   *
   *                 [�V�[���v�Z�O���[�v]
   *                 �V�[���v�Z�O���[�v�͕ύX���Ȃ��B
   *                 
   *                 [�V�[���`��O���[�v]
   *                 �V�[���`��O���[�vscene_draw_group_index�Ԃ̓o�^����������B
   *                 �V�[���`��O���[�vscene_draw_group_index�ԈȊO�ɂ������o�^����Ă���Ƃ��́Ascene_draw_group_index�ԈȊO�̓o�^�͎c��B
   *                 �V�[���`��O���[�v�͕����ɓo�^�ł���B
   *  
   *  @param[in]     model, particle           �o�^����������V�[���m�[�h
   *  @param[in]     scene_draw_group_index    scene_node����菜���V�[���`��O���[�v
   *
   *  @retval        b32   �o�^�������ł�����true
   *                       �o�^�������ł��Ȃ�������false
   *                       ���X�o�^����Ă��Ȃ��V�[���`��O���[�v�����菜�����Ƃ�����false
   */
  //-----------------------------------------------------------------------------
  b32 RemoveModelFromSceneDrawGroup(Model* model, s32 scene_draw_group_index);
  b32 RemoveParticleFromSceneDrawGroup(Particle* particle, s32 scene_draw_group_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �S�ẴV�[���`��O���[�v����V�[���m�[�h�̓o�^����������B
   *
   *                 [�V�[���v�Z�O���[�v]
   *                 �V�[���v�Z�O���[�v�͕ύX���Ȃ��B
   *                 
   *                 [�V�[���`��O���[�v]
   *                 �S�ẴV�[���`��O���[�v����o�^����������B
   *                 �V�[���`��O���[�v�͕����ɓo�^�ł���B
   *  
   *  @param[in]     model, particle           �o�^����������V�[���m�[�h
   *
   *  @retval        b32   �o�^�������ł�����true
   *                       �o�^�������ł��Ȃ�������false
   *                       ���X�o�^����Ă��Ȃ�������false
   */
  //-----------------------------------------------------------------------------
  b32 RemoveModelFromAllSceneDrawGroup(Model* model);
  b32 RemoveParticleFromAllSceneDrawGroup(Particle* particle);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �V�[����
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[������ݒ肷��
   *
   *  @param[in]     scene_environment_setting  �ݒ肷��V�[����
   */
  //-----------------------------------------------------------------------------
  void SetSceneEnvironmentSetting( SceneEnvironmentSetting* scene_environment_setting );

/*

[����][NW4C]SceneEnvironment�̃��C�g���f���@�ɂ���

�J���� : RVCT 4.0 for Nintendo (20100728 build 839) + NintendoWare for CTR 1.0.0 �O���t�B�b�N�X�p�b�`(20100803 ��)


�݂� 2010-09-15 18:50:59 

�����b�ɂȂ��Ă��܂��B
�����I�Ȏ���ŋ��k�Ȃ̂ł����A���f���ɏ�肭���C�g�����f����Ă��Ȃ��󋵂ł��B

SceneEnvironmentSettings�����t�@�C�����\�[�X(.cenv)����쐬���A
���ɍ쐬����Ă���RenderContext��SceneEnvironment�ɔ��f���Ă���̂ł����A
���f���̕��ɏ�肭���C�g�����f����Ȃ��󋵂ł��i�^�����ɂȂ�܂��j

SceneEnvironmentSetting::ResolveReference �� SceneEnvironment::ApplyFrom

���s������A���\�[�X����Light���C���X�^���X�𐶐����āA�c���[�ɓo�^���Ă��܂��B
���̑��ɂȂɂ��葱���͕K�v�ł��傤���H
�T���v�������Ă��銴���ł͑��ɂ���Ă��邱�Ƃ͂Ȃ������悤�Ȃ̂ł����c�B

��낵�����肢���܂��B


maru 2010-09-15 18:59:32 

�����玸�炵�܂��B

������������Ƃ͂܂����̂ł����A�ǂ�����ɃV�[�����[�g��
���C�g���ǉ�����Ă��Ȃ��Ƃ��߂��ۂ��ł���B
�܂������Ă����炲�߂�Ȃ����B�B�B

����Ȋ�����

// ���C�g�̃C���X�^���X�𐶐����܂��B
��
// �t�H�O�̃C���X�^���X�𐶐����܂��B
��
// �V�[���ɒǉ����܂��B
��
// �V�[���c���[���g���o�[�X���ď��������s���܂��B
��
�Ō�ɂ���>SceneEnvironmentSetting::ResolveReference �� SceneEnvironment::ApplyFrom



�c����NTSC 2010-09-15 19:11:03 

�͂��B�����Ȃ�܂��B
ApplyFrom����œo�^����̂ł���Ύ����ŁA
SceneEnvironment����LightSet���擾���āA
�����������C�g��ǉ�����Ȃǂ��Ă��������B


�݂� 2010-09-16 15:24:20 

�����b�ɂȂ��Ă��܂��B

�w���v�̊�{�I�ȋ@�\������ResolveReferrence��ApplyFrom���悾�Ɛ���ς��������悤�ł��B
���C�g�Ȃǂ��쐬������Ɋ���ݒ肵����A�Ӑ}�����ʂ胉�C�g�����f�����悤�ɂȂ�܂����B

���肪�Ƃ��������܂��B

*/


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �J�����ݒ�
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����ɃJ������ݒ肷��
   *
   *  @param[in]     camera         �J����(camera_index�ɐݒ肷��)
   *                                StereoCamera�^����
   *  @param[in]     camera_index   �J�����̃C���f�b�N�X
   *
   *  �ucamera=NULL, camera_index=�C���f�b�N�X�v��n����camera_index�Ɋ��蓖�ĂĂ����J�������Ȃ��ɂ���
   */
  //-----------------------------------------------------------------------------
  void AssignCameraIndex( Camera* camera, s32 camera_index );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �J������ݒ肷��
   *
   *  @param[in]     index      �ݒ肷��C���f�b�N�X
   *  @param[in]     camera     �ݒ肷��J����
   * 
   *  �ucamera=NULL, camera_index=�C���f�b�N�X�v��n����index�Ɋ��蓖�ĂĂ����J�������Ȃ��ɂ���
   */
  //-----------------------------------------------------------------------------
  void SetCamera( s32 index, Camera* camera );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����ɐݒ肵���J�����̃C���f�b�N�X���擾����
   *
   *  @param[in]     camera         �J����
   *                                StereoCamera�^����
   *
   *  @retval        s32    �J�����̃C���f�b�N�X
   *                        �J�����ɃC���f�b�N�X������U���Ă��Ȃ�������CAMERA_INDEX_NONE��Ԃ�
   */
  //-----------------------------------------------------------------------------
  s32  GetAssignedCameraIndex( const Camera* camera ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����ɐݒ肵���J�������擾����
   *
   *  @param[in]     camera_index         �J�����̃C���f�b�N�X
   *
   *  @retval        Camera*    �J����
   *                            StereoCamera�^�ł����Ă�Camera�^�ŕԂ��̂ŁA���̊֐��Ŏ擾��ɔ��ʂ���K�v������
   *                            camera_index�ɃJ���������蓖�ĂĂ��Ȃ�������NULL��Ԃ�
   */
  //-----------------------------------------------------------------------------
  Camera* GetAssignedCamera( const s32 camera_index ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�̃A�N�e�B�u�J�����̃J�����C���f�b�N�X��ݒ肷��
   *
   *  @param[in]     camera_index             �J�����̃C���f�b�N�X
   *  @param[in]     scene_draw_group_index   �V�[���`��O���[�v�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  void SetActiveCameraIndex( s32 camera_index, s32 scene_draw_group_index = 0 );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�̃A�N�e�B�u�J�����̃J�����C���f�b�N�X���擾����
   *
   *  @param[in]     scene_draw_group_index   �V�[���`��O���[�v�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  s32  GetActiveCameraIndex( const s32 scene_draw_group_index = 0 ) const;


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // ���C�g�Z�b�g�ݒ�
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����Ƀ��C�g�Z�b�g��ݒ肷��
   *
   *  @param[in]     light_set_index   �ݒ��ƂȂ郉�C�g�Z�b�g�̃C���f�b�N�X 
   *  @param[in]     light_set         ���C�g�Z�b�g�B
   *                                   �Ăяo�����͂��̃��C�g�Z�b�g��ێ��������ĉ������B
   *                                   light_set_index�ɐݒ肵�Ă��郉�C�g�Z�b�g���O���ꍇ��NULL��n���ĉ������B
   *                                   ���̃��C�g�Z�b�g�ɒǉ����Ă��郉�C�g�́A���̃V�[���ɒǉ�����Ă��Ȃ���΂Ȃ�܂���B
   */
  //-----------------------------------------------------------------------------
  void SetLightSet(s32 light_set_index, LightSet* light_set);


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �`���
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief       �`���������������
   */
  //-----------------------------------------------------------------------------
  void ResetRenderState(void)
  {
    m_render_system->ResetState();
  }



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // ��
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����o�͂���(Release�ł͉������Ȃ�)
   *                 InNwSceneContext�֐����g�p���Ă���̂ŁAInitialize�����Ă���łȂ��Ɛ��m�Ȓl���擾�ł��܂���B
   */
  //-----------------------------------------------------------------------------
  void PrintNum(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v�̌��𓾂�
   */
  //-----------------------------------------------------------------------------
  u32 GetSceneCalculateGroupNum(void) const { return m_scene_calculate_group_num; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�̌��𓾂�
   */
  //-----------------------------------------------------------------------------
  u32 GetSceneDrawGroupNum(void) const { return m_scene_draw_group_num; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���v�Z�O���[�v�ɒǉ�����Ă���V�[���m�[�h�̌����擾����
   */
  //-----------------------------------------------------------------------------
  u32 GetModelNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
  u32 GetLightNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
  u32 GetCameraNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
  u32 GetFogNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
  u32 GetParticleNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
#if GFL_GRP_G3D_H3D_USE
  u32 GetH3dModelNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
#endif 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �S�V�[���v�Z�O���[�v�ɒǉ�����Ă���V�[���m�[�h�̌������v���Ď擾����
   */
  //-----------------------------------------------------------------------------
  u32 GetModelNum(void) const;
  u32 GetLightNum(void) const;
  u32 GetCameraNum(void) const;
  u32 GetFogNum(void) const;
  u32 GetParticleNum(void) const;
#if GFL_GRP_G3D_H3D_USE
  u32 GetH3dModelNum(void) const;
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �S�V�[���v�Z�O���[�v�ɒǉ�����Ă��郂�f���̃}�e���A���̌������v���Ď擾����
   */
  //-----------------------------------------------------------------------------
  u32 GetModelMaterialCount(void) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  �����_�����O�������b�V���̐����J�E���g
   */
  //-----------------------------------------------------------------------------
  inline s32 GetRenderMeshCount( void ) const { return m_render_system->GetRenderMeshCount(); }
  

  //-----------------------------------------------------------------------------
  /**
   *  @brief         Nw�V�[���R���e�L�X�g�ɒǉ�����Ă���Nw�V�[���m�[�h�̌����擾����
   *                 Initialize�����Ă���łȂ��Ɛ��m�Ȓl���擾�ł��܂���B
   */
  //-----------------------------------------------------------------------------
  // �S�V�[���v�Z�O���[�v�𑫂����킹������
  u32 GetNwSceneNodeNumInAllNwSceneContext(void) const
  {
    u32 num = 0;
    for(u32 i=0; i<m_scene_calculate_group_num; ++i)
    {
      num += GetNwSceneNodeNumInNwSceneContext(i);
    }
    return num;
  }
  
  // �e�V�[���v�Z�O���[�v�ɂ��鑍��
  u32 GetNwSceneNodeNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwSceneNodeNumInNwSceneContext();
  }
  
  // ��
  // UserRenderNode
  u32 GetNwUserRenderNodeNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwUserRenderNodeNumInNwSceneContext();
  }
  // ���f��(SkeletalModel, ParticleModel�̌����܂�)
  u32 GetNwModelNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwModelNumInNwSceneContext();
  }
  // �X�P���^�����f��
  u32 GetNwSkeletalModelNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwSkeletalModelNumInNwSceneContext();
  }
  // ���C�g(FragmentLight, VertexLight, HemiSphereLight, AmbientLight�̌����܂�)
  u32 GetNwLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwLightNumInNwSceneContext();
  }
  // �t���O�����g���C�g
  u32 GetNwFragmentLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwFragmentLightNumInNwSceneContext();
  }
  // ���_���C�g
  u32 GetNwVertexLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwVertexLightNumInNwSceneContext();
  }
  // �������C�g
  u32 GetNwHemiSphereLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwHemiSphereLightNumInNwSceneContext();
  }
  // �A���r�G���g���C�g
  u32 GetNwAmbientLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwAmbientLightNumInNwSceneContext();
  }
  // �J����
  u32 GetNwCameraNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwCameraNumInNwSceneContext();
  }
  // �t�H�O
  u32 GetNwFogNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwFogNumInNwSceneContext();
  }
  // �p�[�e�B�N���Z�b�g
  u32 GetNwParticleSetNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwParticleSetNumInNwSceneContext();
  }
  // �p�[�e�B�N���G�~�b�^
  u32 GetNwParticleEmitterNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwParticleEmitterNumInNwSceneContext();
  }
  // �p�[�e�B�N�����f��
  u32 GetNwParticleModelNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwParticleModelNumInNwSceneContext();
  }
  // �A�j���[�V����
  u32 GetNwAnimatableNodeNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwAnimatableNodeNumInNwSceneContext();
  }

  // ���f���̃}�e���A��(�e���f���̃}�e���A���̌������v��������)(SkeletalModel, ParticleModel�̃}�e���A���̌����܂�)
  u32 GetNwModelMaterialCountInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwModelMaterialCountInNwSceneContext();
  }
  // �X�P���^�����f���̃}�e���A��(�e�X�P���^�����f���̃}�e���A���̌������v��������)
  u32 GetNwSkeletalModelMaterialCountInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwSkeletalModelMaterialCountInNwSceneContext();
  }
  // �p�[�e�B�N�����f���̃}�e���A��(�e�p�[�e�B�N�����f���̃}�e���A���̌������v��������)
  u32 GetNwParticleModelMaterialCountInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwParticleModelMaterialCountInNwSceneContext();
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         Scene�����L���Ǘ�������̂̌����擾����(�V�[���v�Z�O���[�v�ɓo�^����Ă��邩�ǂ����͌��Ă��Ȃ�)
   */
  //-----------------------------------------------------------------------------
  u32 GetOwnModelNum(void) const;
  u32 GetOwnLightNum(void) const;
  u32 GetOwnCameraNum(void) const;
  u32 GetOwnFogNum(void) const;
  u32 GetOwnSceneEnvironmentSettingNum(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v�ɓo�^����Ă���V�[���m�[�h�̌����擾����
   *                 �S�V�[���v�Z�O���[�v�𑖍�����̂Ŏ��Ԃ��|����܂��B
   */
  //-----------------------------------------------------------------------------
  u32 GetModelNumInSceneDrawGroup(s32 scene_draw_group_index) const;
  u32 GetParticleNumInSceneDrawGroup(s32 scene_draw_group_index) const;


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �e
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ShadowSystem
   */
  //-----------------------------------------------------------------------------
  // �e�̐���
  // CreateShadowSystem���Ă񂾌�͕K��Scene::Initialize���Ă�ŉ������B
  void CreateShadowSystem(
      gfl::heap::NwAllocator*                               heap_allocator,
      gfl::heap::NwAllocator*                               device_allocator,
      const gfl::grp::g3d::ShadowSystem::Description* description = NULL );
  // �e�̔j��
  void DestroyShadowSystem(void);
  // �e�̃��V�[�o�[�̃Z�b�g�A�b�v
  void SetupShadowReceiverModel(
      Model*                                 model,
      const s32                              material_index,
      const f32                              shadow_intensity,
      const ShadowSystem::ReceiverType receiver_type,
      const s32                              shadow_group_index = 0);
  // �e�̃L���X�^�[��On/Off
  void SwitchShadowCasterModel(Model* model, const b32 cast_on, const s32 shadow_group_index = 0);
/*
  // �e�̃��V�[�o�[��On/Off
  void SwitchShadowReceiverModel(
      Model*                                 model,
      const s32                              material_index,
      const ShadowSystem::ReceiverType receiver_type,
      const b32                              receive_on,
      const s32                              shadow_group_index = 0);
  );
*/
  // �e�̃��V�[�o�[�̉e�̋��x��ݒ肷��
  void SetShadowReceiverModelShadowIntensity(
      Model*                                 model,
      const s32                              material_index,
      const f32                              shadow_intensity,
      const ShadowSystem::ReceiverType receiver_type,
      const s32                              shadow_group_index = 0);  // @note �g���Ă��Ȃ�����������̂ŁA�����𐸍������ق��������B
  // �e��SubmitView
  void SubmitViewOfShadow(const b32 update_camera_matrix = true); 
  // �e��Draw
  void DrawShadow(const b32 command_cache_dump = false);
  // �e�̏����擾
  gfl::grp::g3d::Camera* GetShadowGroupCamera(const s32 shadow_group_index = 0) const;



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �G�b�W
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�V�X�e�������֐�
   *
   * @param heap_allocator          ���������Ɋm�ۂ��郁����������������܂�
   * @param device_allocator        ���������Ɋm�ۂ��郁����������������܂�
   * @param scene_draw_group_index  �\���O���[�v
   * @param p_desc                  �����_�[�^�[�Q�b�g�ݒ���e
   * @param f_delete_memory_control �P�t���x��j����C���邩�ǂ���
   */
  //-----------------------------------------------------------------------------
  void CreateEdgeMapSystem( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, s32 scene_draw_group_index, gfl::grp::RenderTarget::Description* p_desc = NULL, gfl::grp::RenderTarget::Description* p_low_desc = NULL, b32 f_delete_memory_control = true );

  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�V�X�e���j��
   */
  //-----------------------------------------------------------------------------
  void DestroyEdgeMapSystem(void);
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�V�X�e���擾
   * @return 
   */
  //-----------------------------------------------------------------------------
  G3DEdgeMapSystem* GetG3DEdgeMapSystem(){ return m_g3d_edge_map_system; }

  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�`��ݒ�
   *
   * @param model     ���f���I�u�W�F�N�g
   * @param cast_on   �L������
   * @param group_index �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   */
  //-----------------------------------------------------------------------------
  void SwitchEdgeMapModel( Model* model, const b32 cast_on , const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�`��ݒ�
   *
   * @param model     H3d���f���I�u�W�F�N�g
   * @param cast_on   �L������
   * @param group_index �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   */
  //-----------------------------------------------------------------------------
  void SwitchEdgeMapModel( H3dModel* model, const b32 cast_on , const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �G�b�W�}�b�v�`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     display_type           �\����
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   *  @param[in]     group_index            �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void EdgeMapDraw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera,
    const s32                     group_index = SCENE_DRAW_GROUP_INDEX_NONE ,
    bool                          command_cache_dump = false
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �G�b�W�}�b�v�`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     command_target         �R�}���h��
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   *  @param[in]     group_index            �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void EdgeMapDraw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::RenderTarget*       command_target,
    Camera*                       camera,
    const s32                     group_index = SCENE_DRAW_GROUP_INDEX_NONE ,
    bool                          command_cache_dump = false
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �G�b�W�}�b�v�X�e���I�`�揈��
   *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
   *  @param[in]     group_index            �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   *  @param[in]     clear_buffer           �o�b�t�@���N���A���邩
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void StereoEdgeMapDraw(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera,
    const s32                    group_index = SCENE_DRAW_GROUP_INDEX_NONE,
    bool                         clear_buffer = true,
    bool                         command_cache_dump = false
  );
  
  //! �G�b�W�}�b�v�`����
  struct EdgeMapDrawInfo{
    gfl::grp::GraphicsSystem*    graphics_system;       //! �O���t�B�b�N�X�V�X�e��
    StereoCamera*                camera;                //! ���̃V�[����`�悷��X�e���I�J����
    gfl::grp::RenderTarget*      left_render_target;    //! ���ڗp�����_�[�^�[�Q�b�g
    gfl::grp::RenderTarget*      right_render_target;   //! �E�ڗp�����_�[�^�[�Q�b�g
    s32                          group_index;           //! �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
    bool                         clear_color;           //! �J���[�o�b�t�@���N���A���邩
    bool                         clear_zbuffer;         //! �[�x�o�b�t�@���N���A���邩
    bool                         command_cache_dump;    //! �R�}���h�L���b�V�������O�o�͂���
  };
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�X�e���I�`�揈��
   * @param rInfo       �G�b�W�}�b�v�`����
   */
  //-----------------------------------------------------------------------------
  void StereoEdgeMapDraw( const EdgeMapDrawInfo &rInfo );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �G�b�W�}�b�v�X�e���I�`�揈��
   *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     left_camera            ���ڗp�J����
   *  @param[in]     left_render_target     ���ڗp�����_�[�^�[�Q�b�g
   *  @param[in]     right_camera           �E�ڗp�J����
   *  @param[in]     right_render_target    �E�ڗp�����_�[�^�[�Q�b�g
   *  @param[in]     group_index            �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   *  @param[in]     clear_buffer           �o�b�t�@���N���A���邩
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void StereoEdgeMapDraw(
      GraphicsSystem*              graphics_system,
      Camera*                      left_camera, 
      gfl::grp::RenderTarget*      left_render_target,
      Camera*                      right_camera,
      gfl::grp::RenderTarget*      right_render_target,
      const s32                    group_index = SCENE_DRAW_GROUP_INDEX_NONE,
      bool                         clear_buffer = true,
      bool                         command_cache_dump = false )
  {
  }
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�t�B���^�̃Z�b�g�A�b�v
   */
  //-----------------------------------------------------------------------------
  void SetUpEdgeFilter( gfl::heap::NwAllocator* heap_allocator, const gfl::grp::GraphicsSystem* graphics_system, gfl::grp::g3d::G3DPostEffectSystem::Description* p_desc );
 

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �|�X�g����
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief �|�X�g�����V�X�e������
   * @param heap_allocator      �����ɗp����A���P�[�^�[
   * @param device_allocator    �����ɗp����A���P�[�^�[
   * @param p_desc              �������ݒ�
   */
  //-----------------------------------------------------------------------------
  void CreatePostEffectSystem(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      gfl::grp::g3d::G3DPostEffectSystem::Description* p_desc,
      const gfl::grp::GraphicsSystem* graphics_system          = NULL,
      const gfl::grp::MemoryArea      frame_buffer_memory_area = gfl::grp::MEMORY_AREA_NONE
  );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �|�X�g�����V�X�e���j��
   */
  //-----------------------------------------------------------------------------
  // �����1�t���[���x�������ł��Ă��Ȃ��̂ŁA���ӂ��Ďg�p���ĉ������B
  void DestroyPostEffectSystem();
  
  //�錾����
  void SetPostEffectOrder();
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�L�������ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetEdgeEnable(b32 f);
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W���L�����H
   * @return �L�������t���O
   */
  //-----------------------------------------------------------------------------
  b32 IsEdgeEnable();
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�^�C�v�w��
   * @param type     �^�C�v
   */
  //-----------------------------------------------------------------------------
  void SetEdgeType( G3DEdgeFilter::EdgeType type );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�̑�������
   * @param scale     �X�P�[���l
   */
  //-----------------------------------------------------------------------------
  void SetEdgeWidthScale( f32 scale = 1.0f );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�̐F�ݒ�
   * @param scale     �X�P�[���l
   */
  //-----------------------------------------------------------------------------
  void SetEdgeColor( f32 r, f32 g, f32 b, f32 a = 0.0f );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�`��O���[�v�pSubmitView�֐�
   *
   * @param camera �J����
   * @param update_camera_matrix
   * @param group_index �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   */
  //-----------------------------------------------------------------------------
  void SubmitViewOfEdge( StereoCamera* camera, bool update_camera_matrix = true , const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE );

  //-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�`��O���[�v�pSubmitView�֐�
   *
   * @param camera �J����
   * @param update_camera_matrix
   * @param group_index �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   */
  //-----------------------------------------------------------------------------
  void SubmitViewOfEdge( Camera* camera, bool update_camera_matrix = true , const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �|�X�g�G�t�F�N�g�`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     display_type           �\����
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void PostEffectDraw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera,
    bool                          command_cache_dump = false
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �|�X�g�G�t�F�N�g�`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     render_target          �\���^�[�Q�b�g
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void DrawPostEdge(
    gfl::grp::GraphicsSystem* graphics_system,
    gfl::grp::RenderTarget* render_target,
    Camera* camera,
    bool command_cache_dump = false
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �|�X�g�G�t�F�N�g�`�揈��
   *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void StereoPostEffectDraw(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera,
    bool                         command_cache_dump = false
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �G�b�W�`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     display_type           �\����
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void DrawPostEdge(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera,
    bool                          command_cache_dump = false
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �G�b�W�X�e���I�`�揈��
   *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void StereoDrawPostEdge(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera,
    bool                         command_cache_dump = false
  );


 
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // ��ʊE�[�x
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief ��ʊE�[�x�V�X�e�������֐�
   *
   * @param heap_allocator          ���������Ɋm�ۂ��郁����������������܂�
   * @param device_allocator        ���������Ɋm�ۂ��郁����������������܂�
   * @param graphics_system         ���������Ɋm�ۂ���r�f�I������������������܂�
   */
  //-----------------------------------------------------------------------------
  void CreateDepthOfFieldSystem( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, gfl::grp::GraphicsSystem* graphics_system );

  //-----------------------------------------------------------------------------
  /**
   * @brief ��ʊE�[�x�V�X�e���j��
   */
  //-----------------------------------------------------------------------------
  void DestroyDepthOfFieldSystem(void);
  
  //-----------------------------------------------------------------------------
  /**
   * @brief ��ʊE�[�x�^�C�v�w��
   */
  //-----------------------------------------------------------------------------
  void SetDofType( DofType::Enum type );

  //-----------------------------------------------------------------------------
  /**
   * @brief ��ʊE�[�x�t�H�[�J�X�����w��
   */
  //-----------------------------------------------------------------------------
  void SetFocusLength( f32 focus );
  
  //-----------------------------------------------------------------------------
  /**
   * @brielf    �ڂ₯���C���[�ݒ�i�ő�4�i�K�j
   * @param layer             �ڂ₯���C���[�ԍ�
   * @param rangeDistance     �e���͈�
   * @param blurred           �ڂ₯�( �ŏ�0.0f ? 1.0f�ő� )
   */
  //-----------------------------------------------------------------------------
  void SetFocusRange( u32 layer, f32 rangeDistance, f32 blurred );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief   �ڂ₯�ݒ胊�Z�b�g
   */
  //-----------------------------------------------------------------------------
  void ReSetFocusRange();
  
  //-----------------------------------------------------------------------------
  /**
   * @brief   �ڂ����X�P�[���ݒ�(���̂Ƃ���O���ڂ����݂̂ɑΉ�)
   */
  //-----------------------------------------------------------------------------
  void SetBlurScale( f32 blurScale );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��ʊE�[�x�`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     display_type           �\����
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void DrawPostDepthOfField(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��ʊE�[�x�`�揈��
   *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
   */
  //-----------------------------------------------------------------------------
  void StereoDrawPostDepthOfField(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera
  );
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��ʊE�[�x�p�`��o�b�t�@�擾
   *
   *  @param[in]     DisplayType        DisplayType
   */
  //-----------------------------------------------------------------------------
  gfl::grp::RenderTarget* GetDepthOfFieldRenderTarget( DisplayType type = DISPLAY_UPPER );

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // BloomFilter
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         BloomFilter
   */
  //-----------------------------------------------------------------------------
public:
  // createBloomFilter���Ă񂾌�͕K��Scene::Initialize���Ă�ŉ������B
  void CreateBloomFilter(void); 
  // �����1�t���[���x�������ł��Ă��Ȃ��̂ŁA���ӂ��Ďg�p���ĉ������B
  void DestroyBloomFilter(void);
  void SubmitViewOfBloom(void); 
  void DrawBloomPlainImageFrameBuffer(
      gfl::grp::GraphicsSystem*   graphics_system,
      const gfl::grp::DisplayType plain_image_display_type,  // plain_image_display_type��render_target�̃R�}���h���X�g���قȂ�ꍇ�A���ʂ͕ۏ؂���Ȃ��B
      gfl::grp::RenderTarget*     render_target,
      const b32 command_cache_dump = false
  );
  void DrawBloomPlainImageOffScreenBuffer(
      gfl::grp::GraphicsSystem*     graphics_system,
      const gfl::grp::RenderTarget* plain_image_off_screen_buffer,  // plain_image_off_screen_buffer��render_target�̃R�}���h���X�g���قȂ�ꍇ�A���ʂ͕ۏ؂���Ȃ��B
      gfl::grp::RenderTarget*       render_target,
      const b32 command_cache_dump = false
  );
  void StereoDrawBloomPlainImageFrameBuffer(
      gfl::grp::GraphicsSystem* graphics_system,
      // plain_image��gfl::grp::DISPLAY_UPPER
      gfl::grp::RenderTarget* left_render_target,   // gfl::grp::DISPLAY_UPPER��left_render_target�̃R�}���h���X�g���قȂ�ꍇ�A���ʂ͕ۏ؂���Ȃ��B
      // plain_image��gfl::grp::DISPLAY_UPPER_RIGHT
      gfl::grp::RenderTarget* right_render_target,  // gfl::grp::DISPLAY_UPPER_RIGHT��right_render_target�̃R�}���h���X�g���قȂ�ꍇ�A���ʂ͕ۏ؂���Ȃ��B
      const b32 command_cache_dump = false
  );
 
  u32  GetBloomSmallImageNum(void) const;
  u32  GetBloomBloomBoardNum(void) const;
  void SetBloomHighLuminanceColorWeight(const gfl::grp::ColorF32& c);
  void GetBloomHighLuminanceColorWeight(gfl::grp::ColorF32* c) const;
  void SetBloomHighLuminanceMinValue(const f32 v);
  f32  GetBloomHighLuminanceMinValue(void) const;
  void SetBloomBloomBoardExpanse(const f32 bloom_board_expanse);
  f32  GetBloomBloomBoardExpanse(void) const;
  void SetBloomBloomBoardIntensity(const f32 bloom_board_intensity);
  f32  GetBloomBloomBoardIntensity(void) const;
  void SetBloomBloomBoardSmallImageCount(const u32 bloom_board_index, const u32 small_image_count);
  u32  GetBloomBloomBoardSmallImageCount(const u32 bloom_board_index) const;


#if GFL_DEBUG
  Model* GetBloomTextureCheckBoardModel(void) const;
/*
���܂��\������Ȃ��̂ŁA�g�p�֎~�B
  // �R�}���h�~��(���s���֐����Ă�)
  void   SetImageFrameBufferToBloomTextureCheckBoard(const gfl::grp::DisplayType image_display_type);
*/
  // �R�}���h�~��(���s���֐����Ă�)
  void   SetImageOffScreenBufferToBloomTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer);
  // �R�}���h�~��(���s���֐����Ă�)
  void   SetBloomHighLuminanceImageToBloomTextureCheckBoard(void);
  // �R�}���h�~��(���s���֐����Ă�)
  void   SetBloomHighLuminanceImageToBloomTextureCheckBoard(const gfl::math::VEC2& uv_scale, const gfl::math::VEC2& uv_translate);
#endif  // #if GFL_DEBUG

  //-----------------------------------------------------------------------------
  /**
   * @brief �u���[���L�������ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetBloomEnable(b32 f);
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �u���[�����L�����H
   * @return �L�������t���O
   */
  //-----------------------------------------------------------------------------
  b32 IsBloomEnable();



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �ȈՏ���
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X����V�[�����\�z����
   *                 ���������I�u�W�F�N�g��Scene���Ǘ�����B
   *  
   *  @param[in]     heap_allocator         �q�[�v�A���P�[�^
   *  @param[in]     device_allocator  �f�o�C�X�������̃A���P�[�^
   *  @param[in]     Resource*         �V�[���\�z�Ɏg�p���郊�\�[�X
   *                                   nw::gfx::res::ResSceneNode�̐e�q�K�w�𔽉f����B
   *                                   ����Ɋ܂܂��ȉ��̃��\�[�X���琶�������I�u�W�F�N�g��
   *                                   �V�[���ɊǗ�����A�V�[���ɓo�^�����B
   *                                   nw::gfx::res::ResSceneNode(nw::gfx::res::ResModel, nw::gfx::res::Light, �Ȃ�)
   *                                   ����Ɋ܂܂��ȉ��̃��\�[�X���琶�������I�u�W�F�N�g��
   *                                   �V�[���ɊǗ������B
   *                                   nw::gfx::res::ResSceneEnvironmentSetting
   *  @param[in]     user_id           ���������I�u�W�F�N�g�ɑ΂��āA���[�U���C�ӂŕt������ID
   */
  //-----------------------------------------------------------------------------
  void  BuildSceneUsingResource(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      Resource*               resource,
      s32                     user_id         = OBJECT_USER_ID_NOT_USE
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����Ǘ�����I�u�W�F�N�g���g���āA�V�[������ݒ肷��
   */
  //-----------------------------------------------------------------------------
  void  SetSceneEnvironmentUsingOwnObject(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����Ǘ�����I�u�W�F�N�g��j������
   */
  //-----------------------------------------------------------------------------
  void  DestroyOwnObject(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����Ǘ�����I�u�W�F�N�g���擾����
   *                 �I�u�W�F�N�g���擾�͂��邪�A�V�[�����Ǘ���������̂ŁA������Ă͂Ȃ�Ȃ��B
   *  
   *  @param[in]     a_resource_id        ���\�[�X��ID
   *  @param[in]     a_index_in_resource  ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X
   *  @param[in]     a_user_id            ���[�U���C�ӂŕt����ID
   *
   *  @retval        NULL�̂Ƃ��Ȃ�
   */
  //-----------------------------------------------------------------------------
  Model*                    GetOwnModel( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );
  Camera*                   GetOwnCamera( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );
  Light*                    GetOwnLight( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );
  Fog*                      GetOwnFog( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );
  SceneEnvironmentSetting*  GetOwnSceneEnvironmentSetting( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );




  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �R�[���o�b�N
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  // �V�[���`��O���[�v�ɁADraw�̑O�ɌĂяo���R�[���o�b�N�֐���ݒ肷��
  void SetDrawBeginCommandInvokeCallback(SceneDrawGroupCommandInvokeCallback callback = NULL, void* work = NULL, s32 scene_draw_group_index = 0 )
  {
    m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group->SetBeginCommandInvokeCallback(callback, work);
  }
  // �V�[���`��O���[�v�ɁADraw�̌�ɌĂяo���R�[���o�b�N�֐���ݒ肷��
  void SetDrawEndCommandInvokeCallback(SceneDrawGroupCommandInvokeCallback callback = NULL, void* work = NULL, s32 scene_draw_group_index = 0 )
  {
    m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group->SetEndCommandInvokeCallback(callback, work);
  }




  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  //
  // ����J�萔
  //
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
private:
  //-------------------------------------
  //
  /// �V�[���m�[�h�𐧌䂷��r�b�g�t���OSCENE_NODE_PROPERTY_BIT_FLAG
  //
  //=====================================
  enum
  {
    // �P��
    SCENE_NODE_PROPERTY_BIT_FLAG_NONE             = 0x0,        // �Ȃ�
    SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLE     = 0x1  << 0,  // ModelNodeProperty�ł̂ݎg�p�B���f���̕`��t���O��SubmitView�O�ɋL�����A
                                                                // SubmitView��ɖ߂��B
    SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG = 0x1  << 1,  // ModelNodeProperty�ł̂ݎg�p�BCalculateCulling�����o�֐��ŕύX�����V�[���c���[�t���O�B
                                                                // 1�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������A
                                                                // 0�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����B
    // ����
    SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT          = SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG  // �f�t�H���g
  };
 
  //-------------------------------------
  // 
  /// �z���Ă͂Ȃ�Ȃ������ 
  //
  //=====================================
  enum
  {
    // �V�[�������L���Ǘ�������̂̌�
    SCENE_OWN_MODEL_NUM_MAX                      = 32000,  // u16�̍ő�l
    SCENE_OWN_LIGHT_NUM_MAX                      = 32000,  // u16�̍ő�l
    SCENE_OWN_CAMERA_NUM_MAX                     = 250,    // u8�̍ő�l
    SCENE_OWN_FOG_NUM_MAX                        = 250,    // u8�̍ő�l
    SCENE_OWN_SCENE_ENVIRONMENT_SETTING_NUM_MAX  = 250,    // u8�̍ő�l

    // �V�[���v�Z�O���[�v1�ɂ����̌����ő�l�ƂȂ�
    SCENE_CALCULATE_GROUP_MODEL_NUM_MAX          = 32000,  // u16�̍ő�l
    SCENE_CALCULATE_GROUP_LIGHT_NUM_MAX          = 32000,  // u16�̍ő�l
    SCENE_CALCULATE_GROUP_CAMERA_NUM_MAX         = 250,    // u8�̍ő�l
    SCENE_CALCULATE_GROUP_FOG_NUM_MAX            = 250,    // u8�̍ő�l
    SCENE_CALCULATE_GROUP_PARTICLE_NUM_MAX       = 32000,  // u16�̍ő�l
                                                 
    // �V�[���v�Z�O���[�v�A�V�[���`��O���[�v�̌�
    SCENE_CALCULATE_GROUP_NUM_MAX                = 16000,  // s16�̐��̍ő�l
    SCENE_DRAW_GROUP_NUM_MAX                     = 16000   // s16�̐��̍ő�l
  };



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  //
  // ����J�\����
  //
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
private:
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �V�[���m�[�h�̃v���p�e�B
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  
  //-------------------------------------
  // 
  /// �V�[���m�[�h�ɃV�[�����L�̏���t�������\����
  // 
  //=====================================
  struct SceneNodeProperty
  {
    SceneNode* scene_node;
    bit32      bit_flag;     // SCENE_NODE_PROPERTY_BIT_FLAG
    SceneNodeProperty(void)
        : scene_node(NULL),
          bit_flag(SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT)
    {}
  };

  //-------------------------------------
  // 
  /// �V�[���m�[�h�̃��f���A�p�[�e�B�N���ɃV�[�����L�̏���t�������\����
  // 
  //=====================================
  struct ModelNodeProperty : public SceneNodeProperty
  {
    s16             scene_draw_group_index_one;  // scene_draw_group_index_one>=0�̂Ƃ�1�̃V�[���`��O���[�v�ɂ��������Ă��Ȃ��B
                                                 // SCENE_CALCULATE_GROUP_INDEX_NONE�̂Ƃ��ǂ̃V�[���`��O���[�v�ɂ������Ă��Ȃ��B
                                                 // SCENE_DRAW_GROUP_INDEX_MULTI�̂Ƃ�2�ȏ�̃V�[���`��O���[�v�ɓ����Ă���B
    s8              padding[2];                  // �e�ɂ�padding���ĕϐ������������獬���̂��Ƃ�������Ȃ����A
                                                 // padding�ϐ��Ȃ�ăA�N�Z�X���Ȃ�����C�ɂ��Ȃ��Ă������B
    gfl::base::Bit* scene_draw_group_bit;        // m_scene_draw_group_num�����r�b�g��p�ӂ���B
                                                 // scene_draw_group_index_one��SCENE_DRAW_GROUP_INDEX_MULTI�̂Ƃ��A
                                                 // ���̃r�b�g�̃V�[���`��O���[�v�ɓ����Ă���B
    ModelNodeProperty(void)
        : SceneNodeProperty(),
          scene_draw_group_index_one(SCENE_DRAW_GROUP_INDEX_NONE),
          scene_draw_group_bit(NULL)
    {}
  };

#if GFL_GRP_G3D_H3D_USE
  //-------------------------------------
  // 
  /// h3d���f���ɃV�[�����L�̏���t�������\����
  // 
  //=====================================
  struct H3dModelProperty
  {
    H3dModel* scene_node;
    bit32      bit_flag;     // SCENE_NODE_PROPERTY_BIT_FLAG

    s16             scene_draw_group_index_one;
    s8              padding[2];          
    gfl::base::Bit* scene_draw_group_bit;
    
    H3dModelProperty(void)
        : scene_node(NULL),
          bit_flag(SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT),

          scene_draw_group_index_one(SCENE_DRAW_GROUP_INDEX_NONE),
          scene_draw_group_bit(NULL)
    {}
  };
#endif  // GFL_GRP_G3D_H3D_USE

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // ���܂Ƃߍ\����
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

  //-------------------------------------
  // 
  // SceneNodeProperty���܂Ƃ߂��\����
  // 
  //=====================================

  // �^��` 
  typedef SceneNode* SceneNodePointer;
  typedef gfl::base::FixedListElement<SceneNodeProperty> SceneNodePropertyElement;
  typedef gfl::base::FixedListElement<ModelNodeProperty> ModelNodePropertyElement;
  typedef gfl::base::FixedListArraySentinel<SceneNodeProperty, SceneNodePointer> SceneNodePropertyArray;
  typedef gfl::base::FixedListArraySentinel<ModelNodeProperty, SceneNodePointer> ModelNodePropertyArray;
  struct ModelNodePropertyFunctionWork
  {
    gfl::heap::HeapBase* heap_memory;
    s16                  scene_draw_group_num;
    s8                   padding[2];
  };

  // �֐���`
  // SceneNodeProperty
  // CreateDataFunction
  static void CreateSceneNodePropertyFunction(SceneNodeProperty* a, void* work)
  {
    // �������Ȃ�
  }
  // DestroyDataFunction
  static void DestroySceneNodePropertyFunction(SceneNodeProperty* a, void* work)
  {
    // �������Ȃ�
  }
  // InitializeDefaultDataFunction
  static void InitializeDefaultSceneNodePropertyFunction(SceneNodeProperty* a, void* work) 
  {
    a->scene_node = NULL;
    a->bit_flag   = SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT;
  }
  // CompareDataFunction
  static s32  CompareSceneNodePropertyFunction(const SceneNodeProperty* a, const SceneNodePointer b, void* work)
  {
    if( a->scene_node == b )          return  0;
    else if( a->scene_node >  b )     return  1;
    else /*if( a->scene_node <  b )*/ return -1;
  }
  // CopyDataFunction
  static void CopySceneNodePropertyFunction(SceneNodeProperty* dst, const SceneNodeProperty* src, void* work)
  {
    dst->scene_node = src->scene_node;
    dst->bit_flag   = src->bit_flag;
  }

  // ModelNodeProperty
  // CreateDataFunction
  static void CreateModelNodePropertyFunction(ModelNodeProperty* a, void* work)
  {
    // work��ModelNodePropertyFunctionWork�ɂ��Ă����B
    ModelNodePropertyFunctionWork* l_work = reinterpret_cast<ModelNodePropertyFunctionWork*>(work);
    a->scene_draw_group_bit = GFL_NEW(l_work->heap_memory) gfl::base::Bit(l_work->heap_memory, l_work->scene_draw_group_num);
  }
  // DestroyDataFunction
  static void DestroyModelNodePropertyFunction(ModelNodeProperty* a, void* work)
  {
    GFL_DELETE a->scene_draw_group_bit;
  }
  // InitializeDefaultDataFunction
  static void InitializeDefaultModelNodePropertyFunction(ModelNodeProperty* a, void* work) 
  {
    InitializeDefaultSceneNodePropertyFunction(a, work); 
    a->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    a->scene_draw_group_bit->OffAll();  // ����������͂��Ȃ�
  }
  // CompareDataFunction
  static s32  CompareModelNodePropertyFunction(const ModelNodeProperty* a, const SceneNodePointer b, void* work)
  {
    return CompareSceneNodePropertyFunction(a, b, work);
  }
  // CopyDataFunction
  static void CopyModelNodePropertyFunction(ModelNodeProperty* dst, const ModelNodeProperty* src, void* work)
  {
    CopySceneNodePropertyFunction(dst, src, work);
    dst->scene_draw_group_index_one = src->scene_draw_group_index_one;
    dst->scene_draw_group_bit->Copy(src->scene_draw_group_bit);
  }

#if GFL_GRP_G3D_H3D_USE
  // �^��` 
  typedef H3dModel* H3dModelPointer;
  typedef gfl::base::FixedListElement<H3dModelProperty> H3dModelPropertyElement;
  typedef gfl::base::FixedListArraySentinel<H3dModelProperty, H3dModelPointer> H3dModelPropertyArray;
  struct H3dModelPropertyFunctionWork
  {
    gfl::heap::HeapBase* heap_memory;
    s16                  scene_draw_group_num;
    s8                   padding[2];
  };

  // �֐���`
  // H3dModelProperty
  // CreateDataFunction
  static void CreateH3dModelPropertyFunction(H3dModelProperty* a, void* work)
  {
    // work��H3dModelPropertyFunctionWork�ɂ��Ă����B
    H3dModelPropertyFunctionWork* l_work = reinterpret_cast<H3dModelPropertyFunctionWork*>(work);
    a->scene_draw_group_bit = GFL_NEW(l_work->heap_memory) gfl::base::Bit(l_work->heap_memory, l_work->scene_draw_group_num);
  }
  // DestroyDataFunction
  static void DestroyH3dModelPropertyFunction(H3dModelProperty* a, void* work)
  {
    GFL_DELETE a->scene_draw_group_bit;
  }
  // InitializeDefaultDataFunction
  static void InitializeDefaultH3dModelPropertyFunction(H3dModelProperty* a, void* work) 
  {
    a->scene_node = NULL;
    a->bit_flag   = SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT;
 
    a->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    a->scene_draw_group_bit->OffAll();  // ����������͂��Ȃ�
  }
  // CompareDataFunction
  static s32  CompareH3dModelPropertyFunction(const H3dModelProperty* a, const H3dModelPointer b, void* work)
  {
    if( a->scene_node == b )          return  0;
    else if( a->scene_node >  b )     return  1;
    else /*if( a->scene_node <  b )*/ return -1;
  }
  // CopyDataFunction
  static void CopyH3dModelPropertyFunction(H3dModelProperty* dst, const H3dModelProperty* src, void* work)
  {
    dst->scene_node = src->scene_node;
    dst->bit_flag   = src->bit_flag;
    
    dst->scene_draw_group_index_one = src->scene_draw_group_index_one;
    dst->scene_draw_group_bit->Copy(src->scene_draw_group_bit);
  }
#endif  // GFL_GRP_G3D_H3D_USE

  // �V�[���v�Z�O���[�v���ƂɊǗ�����
  struct SceneNodePropertyPack
  {
    ModelNodePropertyArray* model_array;
    SceneNodePropertyArray* light_array;
    SceneNodePropertyArray* camera_array;
    SceneNodePropertyArray* fog_array;
    ModelNodePropertyArray* particle_array;
#if GFL_GRP_G3D_H3D_USE
    H3dModelPropertyArray*  h3d_model_array;
#endif  // GFL_GRP_G3D_H3D_USE

    SceneNodePropertyPack(void)
        : model_array(NULL),
          light_array(NULL),
          camera_array(NULL),
          fog_array(NULL),
          particle_array(NULL)
#if GFL_GRP_G3D_H3D_USE
          , h3d_model_array(NULL)
#endif  // GFL_GRP_G3D_H3D_USE
    {}
  };
 
  //-------------------------------------
  // 
  // �I�u�W�F�N�g���܂Ƃ߂��\����
  // 
  //=====================================
  // Scene�����L���Ǘ�����
  struct ObjectPack
  {
    u16                       model_num_max;
    u16                       model_num;
    u16                       light_num_max;
    u16                       light_num;
    u8                        camera_num_max;
    u8                        camera_num;
    u8                        fog_num_max;
    u8                        fog_num;
    u8                        scene_environment_setting_num_max;
    u8                        scene_environment_setting_num;
    u8                        padding[2];
    Model**                   model_array;   // ���[0]���猄�ԂȂ��l�߂Ă����l�������Ă���Ō����[model_num-1]�ƂȂ�悤�ɂ��Ă����B
    Light**                   light_array;   // ���[0]���猄�ԂȂ��l�߂Ă����l�������Ă���Ō����[light_num-1]�ƂȂ�悤�ɂ��Ă����B
    Camera**                  camera_array;  // ���[0]���猄�ԂȂ��l�߂Ă����l�������Ă���Ō����[camera_num-1]�ƂȂ�悤�ɂ��Ă����B
    Fog**                     fog_array;     // ���[0]���猄�ԂȂ��l�߂Ă����l�������Ă���Ō����[fog_num-1]�ƂȂ�悤�ɂ��Ă����B
    SceneEnvironmentSetting** scene_environment_setting_array;   // ���[0]���猄�ԂȂ��l�߂Ă����l�������Ă���
                                                                 // �Ō����[scene_environment_setting_num-1]�ƂȂ�悤�ɂ��Ă����B

    ObjectPack(void)
        : model_num_max(0),
          model_num(0),
          light_num_max(0),
          light_num(0),
          camera_num_max(0),
          camera_num(0),
          fog_num_max(0),
          fog_num(0),
          scene_environment_setting_num_max(0),
          scene_environment_setting_num(0),
          model_array(NULL),
          light_array(NULL),
          camera_array(NULL),
          fog_array(NULL)
    {}
  };

  //-------------------------------------
  // 
  /// �V�[���v�Z�O���[�v�Ƃ��̕t�����
  // 
  //=====================================
  struct SceneCalculateGroupPack
  {
    SceneCalculateGroup*     scene_calculate_group;
    SceneNodePropertyPack    scene_node_property_pack;
    bit32                    scene_calculate_group_enable_bit_flag;  // �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O

    SceneCalculateGroupPack(void)
      : scene_calculate_group(NULL),
        scene_node_property_pack(),
        scene_calculate_group_enable_bit_flag(SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_DEFAULT)
    {}
  };
 
  //-------------------------------------
  // 
  /// �V�[���`��O���[�v�Ƃ��̕t�����
  // 
  //=====================================
  struct SceneDrawGroupPack
  {
    SceneDrawGroup*          scene_draw_group;
    const Camera*            scene_draw_group_camera;  // ���̂Ƃ���̃J�����̃|�C���^���o���Ă��邾���ŁA�J�����̎��̂͊Ǘ����Ȃ�
    bit32                    scene_draw_group_enable_bit_flag;  // �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O

    SceneDrawGroupPack(void)
      : scene_draw_group(NULL),
        scene_draw_group_camera(NULL),
        scene_draw_group_enable_bit_flag(SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_DEFAULT)
    {}
  };



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  //
  // �����o�ϐ�
  //
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
private:
  // ��
  s16                       m_scene_calculate_group_num;
  s16                       m_scene_draw_group_num;
 
  // �V�[���v�Z�O���[�v
  SceneCalculateGroupPack*       m_scene_calculate_group_pack_array;
  ModelNodePropertyFunctionWork* m_model_node_property_function_work;
#if GFL_GRP_G3D_H3D_USE
  H3dModelPropertyFunctionWork*  m_h3d_model_property_function_work;
#endif  // GFL_GRP_G3D_H3D_USE

  // �V�[���`��O���[�v
  SceneDrawGroupPack*       m_scene_draw_group_pack_array;
  nw::gfx::RenderQueue*     m_render_queue_TEMP;

  // Scene�����L���Ǘ��������
  ObjectPack*               m_scene_own_object_pack;

  // �K�{�V�X�e��
  SceneSystem*              m_scene_system;
  RenderSystem*             m_render_system;

  // �ǉ��V�X�e��
  ShadowSystem*             m_shadow_system;
  G3DEdgeMapSystem*         m_g3d_edge_map_system;
  G3DPostEffectSystem*      m_g3d_post_effect_system;
  G3DDepthOfFieldSystem*    m_G3DDepthOfFieldSystem;
  
  static f32                s_CullBoxScale;

#if GFL_DEBUG
private:
  // �f�o�b�O
  b32  m_debug_tick_enable_flag;
  b32  m_debug_tick_dump_and_reset_flag;
#endif



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // ����Jstatic�֐�
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // SceneNodeProperty(ModelNodeProperty)�ɑ΂��鑀��
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // �y�߂�l���ӁzSCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG��1��������y0�ȊO(1�Ƃ͌���Ȃ��������Ⴄ�l�ɂȂ�)�z��Ԃ��A0��������y0�z��Ԃ��B
  static inline u32 isSceneNodePropertyNotCullingFlagOn(const SceneNodeProperty& scene_node_property)
  { return IsBitFlagOnOne(scene_node_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG); }
  // SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG�̃r�b�g�t���O��ݒ肷��
  static inline void setSceneNodePropertyNotCullingFlag(SceneNodeProperty* scene_node_property, b32 is_on)
  { SetBitFlag(&(scene_node_property->bit_flag), SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG, is_on); }
  
  // �y�߂�l���ӁzSCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLE��1��������y0�ȊO(1�Ƃ͌���Ȃ��������Ⴄ�l�ɂȂ�)�z��Ԃ��A0��������y0�z��Ԃ��B
  static inline u32 isSceneNodePropertyTempVisibleOn(const SceneNodeProperty& scene_node_property)
  { return IsBitFlagOnOne(scene_node_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLE); }
  // SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLE�̃r�b�g�t���O��ݒ肷��
  static inline void setSceneNodePropertyTempVisible(SceneNodeProperty* scene_node_property, b32 visible)
  { SetBitFlag(&(scene_node_property->bit_flag), SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLE, visible); }
  
  // �����݃V�[���m�[�h���V�[���c���[�ɓ����Ă�����true��Ԃ��A�����Ă��Ȃ�������false��Ԃ��B
  // Scene��SceneNode��Add����Ă��邱�Ƃ��m�F����A������[�܂��Ă��Ȃ���
  static inline b32 isSceneNodePropertyInSceneTree(const SceneNodeProperty& scene_node_property)
  {
    return \
      ( \
           IsBitFlagOnOne(scene_node_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG) \
        && scene_node_property.scene_node \
        && scene_node_property.scene_node->IsAddedToScene() \
        && scene_node_property.scene_node->GetTreeFlag() \
      );
      // Scene��SceneNode��Add����Ă��邱�Ƃ��m���ȏꍇ��
      //     && scene_node_property.scene_node \
      //     && scene_node_property.scene_node->IsAddedToScene() \
      // �̊m�F�͕K�v�Ȃ��̂ŁA
      // ������[�܂����ł̃`�F�b�N�֐�isSceneNodePropertyInSceneTreeEasy���g���Ă悢�B
  }

  // �����݃V�[���m�[�h���V�[���c���[�ɓ����Ă�����true��Ԃ��A�����Ă��Ȃ�������false��Ԃ��B
  // Scene��SceneNode��Add����Ă��邱�Ƃ��m���ȏꍇ�ɂ����Ă�ł悢�A������[�܂�����
  static inline b32 isSceneNodePropertyInSceneTreeEasy(const SceneNodeProperty& scene_node_property)
  {
    return \
      ( \
           IsBitFlagOnOne(scene_node_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG) \
        && scene_node_property.scene_node->GetTreeFlag() \
      );
  }
#if GFL_GRP_G3D_H3D_USE
  // �y�߂�l���ӁzSCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG��1��������y0�ȊO(1�Ƃ͌���Ȃ��������Ⴄ�l�ɂȂ�)�z��Ԃ��A0��������y0�z��Ԃ��B
  static inline u32 isH3dModelPropertyNotCullingFlagOn(const H3dModelProperty& h3d_model_property)
  { return IsBitFlagOnOne(h3d_model_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG); }
  // SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG�̃r�b�g�t���O��ݒ肷��
  static inline void setSceneNodePropertyNotCullingFlag(H3dModelProperty* h3d_model_property, b32 is_on)
  { SetBitFlag(&(h3d_model_property->bit_flag), SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG, is_on); }

  static inline b32 isH3dModelPropertyInSceneTreeEasy(const H3dModelProperty& h3d_model_property)
  {
    return \
      ( \
           IsBitFlagOnOne(h3d_model_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG) \
        && h3d_model_property.scene_node->GetTreeFlag() \
      );
  }
#endif  // GFL_GRP_G3D_H3D_USE

  // �V�[���m�[�h�̃v���p�e�B���f�t�H���g��ԂɂȂ�悤�ɃN���A����B�m�ۃ������̉���͂��Ȃ��B
  static inline void clearDefaultSceneNodeProperty(SceneNodeProperty* scene_node_property)
  {
    scene_node_property->scene_node = NULL;
    scene_node_property->bit_flag   = SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT;
  }
  static inline void clearDefaultModelNodeProperty(ModelNodeProperty* scene_node_property)
  {
    clearDefaultSceneNodeProperty(scene_node_property);
    scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    scene_node_property->scene_draw_group_bit->OffAll();
  }

  // �V�[���m�[�h�̃v���p�e�B���R�s�[����B
  static inline void copySceneNodeProperty(SceneNodeProperty* dst, const SceneNodeProperty* src)
  {
    dst->scene_node = src->scene_node;
    dst->bit_flag   = src->bit_flag;
  }
  static inline void copyModelNodeProperty(ModelNodeProperty* dst, const ModelNodeProperty* src)
  {
    copySceneNodeProperty(dst, src);
    dst->scene_draw_group_index_one = src->scene_draw_group_index_one;
    dst->scene_draw_group_bit->Copy(src->scene_draw_group_bit);
  }

  // �V�[���m�[�h�̃v���p�e�B�̃V�[���`��O���[�v�̐ݒ���X�V����(addModelNodePropertyToDraw��removeModelNodePropertyFromDraw���炵���Ă�ł͂Ȃ�Ȃ�)
  static inline void updateModelNodePropertyDraw(ModelNodeProperty* scene_node_property)
  {
    // ���݂�scene_node_property->scene_draw_group_bit�Ɋ�Â��āAscene_node_property->scene_draw_group_index_one���X�V����
    u32 bit_num = scene_node_property->scene_draw_group_bit->GetOnBitNum();
    if( bit_num == 0 )
    {
      scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    }
    else if( bit_num == 1 )
    {
      u32 index;
      scene_node_property->scene_draw_group_bit->GetOnBitMinIndex(&index);
      scene_node_property->scene_draw_group_index_one = index;
    }
    else
    {
      scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_MULTI;
    }
  }
  // �V�[���m�[�h�̃v���p�e�B�ɃV�[���`��O���[�v��ݒ肷��
  static inline b32 addModelNodePropertyToDraw(ModelNodeProperty* scene_node_property, s32 scene_draw_group_index)
  {
    scene_node_property->scene_draw_group_bit->On(scene_draw_group_index);
    updateModelNodePropertyDraw(scene_node_property);
    return true;
  }
  // �V�[���m�[�h�̃v���p�e�B�ɐݒ肳��Ă���V�[���`��O���[�v����������
  static inline void removeModelNodePropertyFromDraw(ModelNodeProperty* scene_node_property, s32 scene_draw_group_index)
  {
    scene_node_property->scene_draw_group_bit->Off(scene_draw_group_index);
    updateModelNodePropertyDraw(scene_node_property);
  }
  // �V�[���m�[�h�̃v���p�e�B�ɃV�[���`��O���[�v��S�Đݒ肷��
  static inline b32 addModelNodePropertyToAllDraw(ModelNodeProperty* scene_node_property)
  {
    scene_node_property->scene_draw_group_bit->OnAll();
    if( scene_node_property->scene_draw_group_bit->GetBitNum() == 1 )
    {
      scene_node_property->scene_draw_group_index_one = 0;
    }
    else
    {
      scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_MULTI;
    }
    return true;
  }
  // �V�[���m�[�h�̃v���p�e�B�ɐݒ肳��Ă���V�[���`��O���[�v��S�ĉ�������
  static inline void removeModelNodePropertyFromAllDraw(ModelNodeProperty* scene_node_property)
  {
    scene_node_property->scene_draw_group_bit->OffAll();
    scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
  }



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // ����J�֐�
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // friend�N���X�ł���SceneCalculateGroup���炾���A�N�Z�X���Ă��悢�֐�
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // ���݂̃V�[���c���[�t���O�ŃV�[���c���[�̏�Ԃ��X�V����
  void RefreshNodeTreeFlag(SceneNode* scene_node);
#if GFL_GRP_G3D_H3D_USE
  //void RefreshNodeTreeFlag(H3dModel* scene_node);
#endif  // GFL_GRP_G3D_H3D_USE


#if GFL_GRP_G3D_H3D_USE
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // friend�N���X�ł���H3dEnvironment���炾���A�N�Z�X���Ă��悢�֐�
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // �V�[�����𓾂� 
  const nw::gfx::SceneEnvironment& GetSceneEnvironment(s32* max_cameras, s32* max_light_sets, s32* max_fogs, s32* max_vertex_lights) const
  {
    return m_render_system->GetSceneEnvironment(max_cameras, max_light_sets, max_fogs, max_vertex_lights);
  }
#endif  // GFL_GRP_G3D_H3D_USE



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // ���� / �j��
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // �V�[���v�Z�O���[�v�̐���
  void createSceneCalculateGroupPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description);
  // SceneNodePropertyArray�̐���
  inline SceneNodePropertyArray* createSceneNodePropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num);
  // ModelNodePropertyArray�̐���
  inline ModelNodePropertyArray* createModelNodePropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num);
#if GFL_GRP_G3D_H3D_USE
  // H3dModelPropertyArray�̐��� 
  inline H3dModelPropertyArray* createH3dModelPropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num);
#endif  // GFL_GRP_G3D_H3D_USE
  // �V�[���`��O���[�v�̐���
  void createSceneDrawGroupPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description);
  // Scene�����L���Ǘ�������̂̐���
  void createSceneOwnObjectPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description);
  // �V�[���v�Z�O���[�v�̔j��
  void destroySceneCalculateGroupPack(void);
  // �V�[���`��O���[�v�̔j��
  void destroySceneDrawGroupPack(void);
  // Scene�����L���Ǘ�������̂̔j��
  void destroySceneOwnObjectPack(void);
  // �R���X�g���N�^�Ɠ���������
  // �R���X�g���N�^�̏��������X�g(constructor initialize list)��
  // �R���X�g���N�^�̒��g�ł���Ă��鏉�����Ɠ������e
  void constructorInitialize(void);

  // DetachChild�ŃG���[���N���Ȃ����̃`�F�b�N
  void checkErrorDetach(nw::gfx::SceneNode* parent_node, nw::gfx::SceneNode* child_node, const char* msg);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // Initialize
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // �����Ŏg�p���鏉��������
  void initializeInternal(const SceneInitializeProcess scene_initialize_process = SCENE_INITIALIZE_PROCESS_ALL);
  void initializeInternal(SceneCalculateGroup* calc_group, const SceneInitializeProcess scene_initialize_process = SCENE_INITIALIZE_PROCESS_ALL);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  // �v�Z����
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // g3d�I�u�W�F�N�g�ŗL�̍X�V�������s���B
  // �J�����O�̗L���Ɋւ�炸�K���K�v�ȍX�V���s���܂��B
  void updateObjectBeforeCulling(SceneCalculateGroupPack* scene_calculate_group_pack);
  // g3d�I�u�W�F�N�g�ŗL�̍X�V�������s���B
  // �J�����O����Č����Ȃ��Ƃ��́A
  // ���Ȃ��Ă���肪�N���Ȃ��Ȃ��X�V���s���܂��B
  void updateObjectAfterCulling(SceneCalculateGroupPack* scene_calculate_group_pack);

  // �J�����̃J�����}�g���b�N�X���X�V����
  void updateCameraMatrix(const s32 scene_calculate_group_index);

  // �J�����O
  void resetCalculateCullingFlag(s32 scene_calculate_group_index, ModelNodePropertyArray* property_array);
  void setCalculateCullingFlag(s32 scene_calculate_group_index, SceneNodeProperty* scene_node_property, b32 not_culling_flag);  // false�̂Ƃ��J�����O����ăV�[���c���[����O���
  void calculateBeforeCulling(s32 scene_calculate_group_index);
  void calculateAfterCulling(s32 scene_calculate_group_index);
  void culling(Camera* camera);
  void culling(s32 scene_calculate_group_index, Camera* camera);

#if GFL_GRP_G3D_H3D_USE
  ///////////////////////////
  // �S������
  ///////////////////////////
  void calculateH3dModel(s32 scene_calculate_group_index);
  
  ////////////////////////////////////////////////////////////////////////////////
  // �y���Ӂz�S������ƌʂ�1�t���[���ɂǂ��炩�����g��Ȃ��悤�ɂ��ĉ������B
  ////////////////////////////////////////////////////////////////////////////////
 
  ///////////////////////////
  // ��
  ///////////////////////////
  void calculateH3dModelBeforeCulling(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag);
  void calculateH3dModelAfterCulling(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag);

  ///////////////////////////
  // �J�����O
  ///////////////////////////
  void resetH3dCalculateCullingFlag(s32 scene_calculate_group_index, H3dModelPropertyArray* property_array);
  void setH3dCalculateCullingFlag(s32 scene_calculate_group_index, H3dModelProperty* h3d_model_property, b32 not_culling_flag);  // false�̂Ƃ��J�����O����ăV�[���c���[����O���
#endif  // GFL_GRP_G3D_H3D_USE



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  // SubmitView
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  void submitViewAllModel(SceneNodePropertyPack* property_pack);
  void submitViewAllParticle(SceneNodePropertyPack* property_pack);
#if GFL_GRP_G3D_H3D_USE
  void submitViewAllH3dModel(SceneNodePropertyPack* property_pack);
#endif  // GFL_GRP_G3D_H3D_USE

  void mergeRenderQueueMethodHead(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B);
  void mergeRenderQueueMethodTail(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B);
  void mergeRenderQueueMethodSortedMix(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B);
  void mergeRenderQueueMethodMixNeedSort(SceneDrawGroup* draw_group_A,
      nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B);

  void storeSceneNodeVisible(ModelNodePropertyArray* property_array, s32 scene_draw_group_index);
  void restoreSceneNodeVisible(ModelNodePropertyArray* property_array);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �V�[���m�[�h�� �ǉ� / �폜
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // 
  // �����ߊ֐�
  // 
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // old_scene_calculate_group_index�ɓo�^����Ă���object_type��old_element���Ascene_calculate_group_index�Ɉړ�����
  // �ړ��ł��Ȃ�������old_element��scene_node���o�^����Ă���V�[���`��O���[�v��S�ĉ�������
  b32 moveNodeFromCalcToCalc(void* old_element, ObjectType object_type, s32 old_scene_calculate_group_index, s32 scene_calculate_group_index);
  // scene_calculate_group_index��scene_node��o�^����
  // �o�^����SceneNodeProperty�̃|�C���^��scene_node_property�ɓ���ĕԂ��܂��B
  // scene_node���܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ��ɌĂԂ���(����ȊO�ł͂��܂����삵�܂���)
  b32 addNodeToCalc(SceneNode* scene_node, s32 scene_calculate_group_index, void** element);
  // old_scene_calculate_group_index��old_index�Ԗڂɓo�^����Ă���scene_node���Aold_scene_calculate_group_index�ƑS�ẴV�[���`��O���[�v�����������
  void removeNodeFromCalcAndAllDraw(void* old_element, ObjectType object_type, s32 old_scene_calculate_group_index);
  // �S�V�[���m�[�h����菜��
  void removeAllSceneNodeFromCalcAndAllDraw(SceneNodePropertyArray* property_array, ObjectType object_type, s32 scene_calculate_group_index)
  {
    const SceneNodePropertyElement* sentinel = property_array->GetSentinelElement();
    SceneNodePropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      SceneNodePropertyElement* temp = element->next;
      removeNodeFromCalcAndAllDraw(element, object_type, scene_calculate_group_index);
      element = temp;
    }
  }
  // �S���f���m�[�h����菜��
  void removeAllModelNodeFromCalcAndAllDraw(ModelNodePropertyArray* property_array, ObjectType object_type, s32 scene_calculate_group_index)
  {
    const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
    ModelNodePropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      ModelNodePropertyElement* temp = element->next;
      removeNodeFromCalcAndAllDraw(element, object_type, scene_calculate_group_index);
      element = temp;
    }
  }
  // �V�[���`��O���[�v�ɓo�^����Ă���S���f���m�[�h����菜��
  void removeAllModelNodeFromDraw(ModelNodePropertyArray* property_array, s32 scene_draw_group_index)
  {
    const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
    ModelNodePropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      removeModelNodePropertyFromDraw(&(element->data), scene_draw_group_index);
      element = element->next;
    }
  }
 

  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // 
  // �]���ʂ�̊֐�
  // 
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // scene_node��scene_calculate_group_index�ɓo�^����B
  // scene_node��ModelNodeProperty�̏ꍇ�A�܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ��̓V�[���`��O���[�v0�Ԃɂ��o�^����B
  // scene_node�����ɂǂꂩ�̑��̃V�[���v�Z�O���[�v�ɓo�^����Ă�����A���̃V�[���v�Z�O���[�v�����菜���āAscene_calculate_group_index�ɓo�^����B
  // scene_node��ModelNodeProperty�̏ꍇ�A���ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă�����A�V�[���`��O���[�v�ɑ΂��Ă͉����ύX�����Ȃ�(���̏ꍇ�́A
  // �V�[���`��O���[�v�ɓo�^����Ă��Ȃ��ꍇ�͂��̂܂ܓo�^�Ȃ��ɂ��Ă���)�B
  // scene_node��ModelNodeProperty�̏ꍇ�A���ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă��肻�̃V�[���v�Z�O���[�v�����菜������A
  // scene_calculate_group_index�ɓo�^����ۂɃG���[�ƂȂ�����Ascene_node���o�^����Ă���V�[���`��O���[�v��S�ĉ�������B
  // scen_node��scene_calculate_group_index�ɓo�^�ł��Ȃ������Ƃ��Afalse��Ԃ��B
  b32 addSceneNodeToSceneCalculateGroup(SceneNode* scene_node, s32 scene_calculate_group_index);
  // scene_node���V�[���v�Z�O���[�v�����菜���B
  // scene_node��ModelNodeProperty�̏ꍇ�Ascene_node���o�^����Ă���S�ẴV�[���`��O���[�v�����������B
  // scene_node�����ɃV�[���v�Z�O���[�v�ɓo�^����Ă��Ȃ��Ƃ�false��Ԃ��B
  b32 removeSceneNodeFromSceneCalculateGroup(SceneNode* scene_node);
  // �S�ẴV�[���v�Z�O���[�v�ɓo�^����Ă���S�ẴV�[���m�[�h�ɑ΂��āAremoveSceneNodeFromSceneCalculateGroup���Ă񂾂̂Ɠ������ʂ̂���֐��B
  // true�����Ԃ��Ȃ��B
  // (������removeSceneNodeFromSceneCalculateGroup���g���Ă��Ȃ��̂ŁA�S��true�̂Ƃ�true��Ԃ��A1�ł�false�̂Ƃ�false��Ԃ��A�Ƃ����ӂ��ɂ͂ł��Ȃ�)
  b32 removeAllSceneNodeFromSceneCalculateGroup(void);

  // scene_node��scene_calculate_group_index�Ascene_draw_group_index�ɓo�^����B
  // scene_node��ModelNodeProperty�łȂ���΂Ȃ�Ȃ��B
  // scene_node���܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ��̓V�[���`��O���[�v��scene_draw_group_index�ɂ����o�^����邱�ƂɂȂ�B
  // scene_node�����ɂǂꂩ�̑��̃V�[���v�Z�O���[�v�ɓo�^����Ă�����A���̃V�[���v�Z�O���[�v�����菜���āAscene_calculate_group_index�ɓo�^����B
  // scene_node�����ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă�����A�V�[���`��O���[�v�͂���܂ł̃V�[���`��O���[�v��scene_draw_group_index��
  // �����邱�ƂɂȂ�(�ǂ̃V�[���`��O���[�v�ɂ��o�^����Ă��Ȃ��ꍇ�͖ܘ_scene_draw_group_index�ɂ����o�^����邱�ƂɂȂ�)�B
  // scene_node�����ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă��肻�̃V�[���v�Z�O���[�v�����菜������A
  // scene_calculate_group_index�ɓo�^����ۂɃG���[�ƂȂ�����Ascene_node���o�^����Ă���V�[���`��O���[�v��S�ĉ�������B
  // scen_node��scene_calculate_group_index�ɓo�^�ł��Ȃ������Ƃ��Afalse��Ԃ��B
  b32 addModelNodeToSceneCalculateGroupAndSceneDrawGroup(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index);

  // scene_node��S�ẴV�[���`��O���[�v�ɉ�����B
  // scene_node��ModelNodeProperty�łȂ���΂Ȃ�Ȃ��B
  // scene_node���܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ��̓V�[���v�Z�O���[�v0�Ԃɓo�^�����B
  // scene_node���܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ��ɁA�V�[���v�Z�O���[�v0�Ԃɓo�^�ł��Ȃ������Ƃ�false��Ԃ��B
  b32 addModelNodeToAllSceneDrawGroup(SceneNode* scene_node);
  // scene_node������܂ŉ������Ă����V�[���`��O���[�v�͂��̂܂܂�scene_draw_group_index�ɉ�����B
  // scene_node��ModelNodeProperty�łȂ���΂Ȃ�Ȃ��B
  // scene_node���܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ��̓V�[���v�Z�O���[�v0�Ԃɓo�^�����B
  // scene_node���܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ��ɁA�V�[���v�Z�O���[�v0�Ԃɓo�^�ł��Ȃ������Ƃ�false��Ԃ��B
  b32 addModelNodeToSceneDrawGroup(SceneNode* scene_node, s32 scene_draw_group_index);
  // scene_node��S�ẴV�[���`��O���[�v�����菜���B
  // scene_node��ModelNodeProperty�łȂ���΂Ȃ�Ȃ��B
  // �V�[���v�Z�O���[�v�ɑ΂��Ă͉������Ȃ��B
  // scene_node���ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ���false��Ԃ��B
  // scene_node���V�[���`��O���[�v�ɓo�^����Ă��Ȃ������̂Ƃ���false�ɂȂ�Ȃ��B
  b32 removeModelNodeFromAllSceneDrawGroup(SceneNode* scene_node);
  // scene_node��scene_draw_group_index�����菜���B
  // ���̃V�[���`��O���[�v�ɂ��o�^����Ă���ꍇ�A���̂��̂ւ̓o�^�ɑ΂��Ă͉������Ȃ��B
  // scene_node��ModelNodeProperty�łȂ���΂Ȃ�Ȃ��B
  // scene_node���ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��Ƃ���false��Ԃ��B
  // scene_node��scene_draw_group_index�ɓo�^����Ă��Ȃ������̂Ƃ���false�ɂȂ�Ȃ��B
  b32 removeModelNodeFromSceneDrawGroup(SceneNode* scene_node, s32 scene_draw_group_index);


 

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  // �J�����ݒ�
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  void assignNotSetCameraIndex( Camera* camera );
  s32  getNotSetCameraIndex(void);
  s32  getFirstAssignedCameraIndex(void);
  // setupNotSetCamera��cleanupNotSetCamera�͕K���΂Ŏg�p������������n���ĉ������B
  void setupNotSetCamera(Camera* camera, s32 scene_draw_group_index, b32* is_assign_camera_index, b32* is_set_active_camera_index);
  void cleanupNotSetCamera(Camera* camera, s32 scene_draw_group_index, const b32 is_assign_camera_index, const b32 is_set_active_camera_index);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  // �擾
  //
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // �w�肵���V�[���m�[�hmodel, light, camera, fog, particle���ǂ��ɓ����Ă��邩�擾����B
  // ���t����ꂽ��true��Ԃ��B
  // ���t�����Ȃ�������false��Ԃ��A�����߂�l�ɓ���l�͕s��ƂȂ�B 

  // @param[out] scene_calculate_group_index �S�V�[���v�Z�O���[�v����T���A���t�����V�[���v�Z�O���[�v�̃C���f�b�N�X�����ĕԂ��B
  // @param[out] object_type                 ObjectType�����ĕԂ��B
  // @retval                                 ���t�����Ȃ�������NULL��Ԃ��A
  //                                         ���t����ꂽ��model, light, camera_fog, particle�������Ă��郊�X�g�̗v�f�̃|�C���^��Ԃ��B 
  //                                         light, camera, fog�̂Ƃ��͖߂�l��(SceneNodePropertyElement*)�ɃL���X�g���Ďg�p���A
  //                                         model, particle�̂Ƃ��͖߂�l��(ModelNodePropertyElement*)�ɃL���X�g���Ďg�p����B
  void* getNodeElement(const SceneNode* scene_node, s32* scene_calculate_group_index, ObjectType* object_type); 
  ModelNodePropertyElement* getModelElement(const Model* scene_node, s32* scene_calculate_group_index); 
  SceneNodePropertyElement* getLightElement(const Light* scene_node, s32* scene_calculate_group_index); 
  SceneNodePropertyElement* getCameraElement(const Camera* scene_node, s32* scene_calculate_group_index); 
  SceneNodePropertyElement* getFogElement(const Fog* scene_node, s32* scene_calculate_group_index); 
  ModelNodePropertyElement* getParticleElement(const Particle* scene_node, s32* scene_calculate_group_index); 
#if GFL_GRP_G3D_H3D_USE
  H3dModelPropertyElement* getH3dModelElement(const H3dModel* scene_node, s32* scene_calculate_group_index);
#endif  // GFL_GRP_G3D_H3D_USE


  // @param[in]  scene_node_property_pack    ���̃p�b�N���炾���T���B
  // @param[out] scene_node_property         model, light, camera_fog, particle�������Ă���V�[���m�[�h�̃v���p�e�B�̃|�C���^�����ĕԂ��B
  // @param[out] object_type                 ObjectType�����ĕԂ��B
  // @retval                                 ���t�����Ȃ�������NULL��Ԃ��A
  //                                         ���t����ꂽ��model, light, camera_fog, particle�������Ă��郊�X�g�̗v�f�̃|�C���^��Ԃ��B 
  //                                         light, camera, fog�̂Ƃ��͖߂�l��(SceneNodePropertyElement*)�ɃL���X�g���Ďg�p���A
  //                                         model, particle�̂Ƃ��͖߂�l��(ModelNodePropertyElement*)�ɃL���X�g���Ďg�p����B
  void* getNodeElement(const SceneNode* scene_node, const SceneNodePropertyPack* scene_node_property_pack, ObjectType* object_type); 
  ModelNodePropertyElement* getModelElement(const Model* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
  SceneNodePropertyElement* getLightElement(const Light* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
  SceneNodePropertyElement* getCameraElement(const Camera* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
  SceneNodePropertyElement* getFogElement(const Fog* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
  ModelNodePropertyElement* getParticleElement(const Particle* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
#if GFL_GRP_G3D_H3D_USE
  H3dModelPropertyElement* getH3dModelElement(const H3dModel* scene_node, const SceneNodePropertyPack* scene_node_property_pack);
#endif  // GFL_GRP_G3D_H3D_USE


  // @param[in]  scene_calculate_group_index ���̃V�[���v�Z�O���[�v���炾���T���B
  // @param[out] object_type                 ObjectType�����ĕԂ��B
  // @retval                                 ���t�����Ȃ�������NULL��Ԃ��A
  //                                         ���t����ꂽ��model, light, camera_fog, particle�������Ă��郊�X�g�̗v�f�̃|�C���^��Ԃ��B 
  //                                         light, camera, fog�̂Ƃ��͖߂�l��(SceneNodePropertyElement*)�ɃL���X�g���Ďg�p���A
  //                                         model, particle�̂Ƃ��͖߂�l��(ModelNodePropertyElement*)�ɃL���X�g���Ďg�p����B
  void* getNodeElement(const SceneNode* scene_node, const s32 scene_calculate_group_index, ObjectType* object_type); 
  ModelNodePropertyElement* getModelElement(const Model* scene_node, const s32 scene_calculate_group_index); 
  SceneNodePropertyElement* getLightElement(const Light* scene_node, const s32 scene_calculate_group_index); 
  SceneNodePropertyElement* getCameraElement(const Camera* scene_node, const s32 scene_calculate_group_index); 
  SceneNodePropertyElement* getFogElement(const Fog* scene_node, const s32 scene_calculate_group_index); 
  ModelNodePropertyElement* getParticleElement(const Particle* scene_node, const s32 scene_calculate_group_index); 
#if GFL_GRP_G3D_H3D_USE
  H3dModelPropertyElement* getH3dModelElement(const H3dModel* scene_node, const s32 scene_calculate_group_index); 
#endif  // GFL_GRP_G3D_H3D_USE

  
  // �擪�v�f�𓾂�
  inline ModelNodePropertyElement* getModelElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline SceneNodePropertyElement* getLightElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline SceneNodePropertyElement* getCameraElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline SceneNodePropertyElement* getFogElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline ModelNodePropertyElement* getParticleElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
#if GFL_GRP_G3D_H3D_USE
  inline H3dModelPropertyElement* getH3dModelElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
#endif  // GFL_GRP_G3D_H3D_USE

  inline ModelNodePropertyElement* getModelElementHead(const s32 scene_calculate_group_index) const;
  inline SceneNodePropertyElement* getLightElementHead(const s32 scene_calculate_group_index) const;
  inline SceneNodePropertyElement* getCameraElementHead(const s32 scene_calculate_group_index) const;
  inline SceneNodePropertyElement* getFogElementHead(const s32 scene_calculate_group_index) const;
  inline ModelNodePropertyElement* getParticleElementHead(const s32 scene_calculate_group_index) const;
#if GFL_GRP_G3D_H3D_USE
  inline H3dModelPropertyElement* getH3dModelElementHead(const s32 scene_calculate_group_index) const;
#endif  // GFL_GRP_G3D_H3D_USE
  

  // �ԕ��v�f�𓾂�
  inline const ModelNodePropertyElement* getModelSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline const SceneNodePropertyElement* getLightSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline const SceneNodePropertyElement* getCameraSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline const SceneNodePropertyElement* getFogSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline const ModelNodePropertyElement* getParticleSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
#if GFL_GRP_G3D_H3D_USE
  inline const H3dModelPropertyElement* getH3dModelSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
#endif  // GFL_GRP_G3D_H3D_USE

  inline const ModelNodePropertyElement* getModelSentinelElement(const s32 scene_calculate_group_index) const;
  inline const SceneNodePropertyElement* getLightSentinelElement(const s32 scene_calculate_group_index) const;
  inline const SceneNodePropertyElement* getCameraSentinelElement(const s32 scene_calculate_group_index) const;
  inline const SceneNodePropertyElement* getFogSentinelElement(const s32 scene_calculate_group_index) const;
  inline const ModelNodePropertyElement* getParticleSentinelElement(const s32 scene_calculate_group_index) const;
#if GFL_GRP_G3D_H3D_USE
  inline const H3dModelPropertyElement* getH3dModelSentinelElement(const s32 scene_calculate_group_index) const;
#endif  // GFL_GRP_G3D_H3D_USE
 


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // �f�o�b�O�@�\
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  void debugTickDumpAndReset(const char* message);  // message�͍Ō�̉��s�Ȃ���OK
};




}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENE_H__
