#ifndef __GFL_G3DSCENECALCULATEGROUP_H__
#define __GFL_G3DSCENECALCULATEGROUP_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneCalculateGroup.h
 *	@brief  3D�V�[���v�Z�O���[�v
 *	@author	Koji Kawada
 *	@date		2011.02.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
 */
//-----------------------------------------------------------------------------


//-------------------------------------
///	�N���X�̑O���錾
//=====================================
class Scene;
class SceneNode;


//-------------------------------------
///	3D�V�[���v�Z�O���[�v�N���X
//=====================================
class SceneCalculateGroup
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneCalculateGroup);

  
  // ���L�N���X���炵���A�N�Z�X���Ȃ�
  friend class Scene;
  friend class RenderSystem;
  friend class SceneSystem;
  friend class SceneNode;


public:
  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	�V�[���V�X�e���̐ݒ���e
  //=====================================
  struct Description
  {
    // m_scene_context�p�̐ݒ�
    s32 max_scene_nodes;                        // �V�[���m�[�h�̏����         //@���g�p�폜�\��
    s32 max_models;                             // ���f���̏����               //@���g�p�폜�\��
    s32 max_not_skeletal_models;                // �X�P���^�����f���ȊO�̃��f���̏����
    s32 max_skeletal_models;                    // �X�P���^�����f���̏����
    s32 max_cameras;                            // �J�����̏����
    s32 max_lights;                             // ���C�g�̏����               //@���g�p�폜�\��
    s32 max_fragment_lights;                    // �t���O�����g���C�g�̏����
    s32 max_vertex_lights;                      // ���_���C�g�̏����
    s32 max_hemisphere_lights;                  // �������C�g�̏����
    s32 max_ambient_lights;                     // �A���r�G���g���C�g�̏����
    s32 max_fogs;                               // �t�H�O�̏����
    s32 max_particles;                          // �p�[�e�B�N���̏����
    s32 max_particlesets;                       // �p�[�e�B�N���Z�b�g�̏����   //@���g�p�폜�\��
    s32 max_particleemitters;                   // �p�[�e�B�N���G�~�b�^�̏���� //@���g�p�폜�\��
    s32 max_particlemodels;                     // �p�[�e�B�N�����f���̏����   //@���g�p�폜�\��
    s32 max_animatable_nodes;                   // �A�j���[�V�������s���m�[�h�̏����  //@���g�p�폜�\��
    bool scene_context_is_fixed_size_memory;    // �������ȊO�ɂ����������m�ۂ��邩�ǂ����̃t���O  //@���g�p�폜�\��

    // m_scene_root�p�̐ݒ�
    s32  max_callbacks;         // �Ǘ��ł���R�[���o�b�N�̍ő吔  //@���g�p�폜�\��
    s32  max_children;          // �q�̍ő吔  //@���g�p�폜�\��
    bool scene_root_is_fixed_size_memory;  // �������ȊO�ɂ����������m�ۂ��邩�ǂ����̃t���O  //@���g�p�폜�\��

    // m_particle_context�p�̐ݒ�
    s32  particle_context_max_emission;       // ���o�ʂ̍ő吔  //@���g�p�폜�\��
    s32  particle_context_max_stream_length;  // �X�g���[�����̍ő吔  //@���g�p�폜�\��
    b32  particle_context_use_double_buffer;  // �_�u���o�b�t�@�Btrue�̂Ƃ��_�u���o�b�t�@�L���B  //@���g�p�폜�\��
    b32  particle_context_is_fixed_size_memory;  // �������ȊO�ɂ����������m�ۂ��邩�ǂ����̃t���O��ݒ肵�܂��B  //@���g�p�폜�\��
                                                 // true ���w�肷��ƁA�������̂݌Œ�T�C�Y�̃������m�ۂ��s���܂��B
                                                 // false ���w�肷��ƁA�������ȊO�ɂ��K�v�ɉ����ē��I�Ƀ������m�ۂ��s���܂��B

#if GFL_GRP_G3D_H3D_USE
    // h3d�ݒ�
    s32 max_h3d_models;    // H3dModel�̏����
#endif  // GFL_GRP_G3D_H3D_USE

    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : max_scene_nodes                    (nw::gfx::SceneContext::DEFAULT_MAX_SCENE_NODES + 128),
        max_models                         (nw::gfx::SceneContext::DEFAULT_MAX_MODELS + 128),
        max_not_skeletal_models            (nw::gfx::SceneContext::DEFAULT_MAX_MODELS + 128),
        max_skeletal_models                (nw::gfx::SceneContext::DEFAULT_MAX_SKELETAL_MODELS + 128),
        max_cameras                        (8),//(nw::gfx::SceneContext::DEFAULT_MAX_CAMERAS),
        max_lights                         (nw::gfx::SceneContext::DEFAULT_MAX_LIGHTS),
        max_fragment_lights                (nw::gfx::SceneContext::DEFAULT_MAX_FRAGMENT_LIGHTS),
        max_vertex_lights                  (nw::gfx::SceneContext::DEFAULT_MAX_VERTEX_LIGHTS),
        max_hemisphere_lights              (nw::gfx::SceneContext::DEFAULT_MAX_HEMISPHERE_LIGHTS),
        max_ambient_lights                 (nw::gfx::SceneContext::DEFAULT_MAX_AMBIENT_LIGHTS),
        max_fogs                           (nw::gfx::SceneContext::DEFAULT_MAX_FOGS),
        max_particles                      (nw::gfx::SceneContext::DEFAULT_MAX_PARTICLESETS),
        max_particlesets                   (nw::gfx::SceneContext::DEFAULT_MAX_PARTICLESETS),
        max_particleemitters               (nw::gfx::SceneContext::DEFAULT_MAX_PARTICLEEMITTERS),
        max_particlemodels                 (nw::gfx::SceneContext::DEFAULT_MAX_PARTICLEMODELS),
        max_animatable_nodes               (nw::gfx::SceneContext::DEFAULT_MAX_ANIMATABLE_NODES),
        scene_context_is_fixed_size_memory (true),
        max_callbacks                      (nw::gfx::SceneObject::DEFAULT_MAX_CALLBACKS),
        max_children                       (512),//(nw::gfx::SceneObject::DEFAULT_MAX_CHILDREN),
        scene_root_is_fixed_size_memory    (true),
        particle_context_max_emission        (1000),
        particle_context_max_stream_length   (1000),
        particle_context_use_double_buffer   (false),
        particle_context_is_fixed_size_memory(true)
#if GFL_GRP_G3D_H3D_USE
        , max_h3d_models(8)  // @note �f�t�H���g��0�ɂ��邩
#endif  // GFL_GRP_G3D_H3D_USE
    {}
  };


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  SceneCalculateGroup(gfl::heap::NwAllocator* allocator, const Description* description = NULL);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~SceneCalculateGroup();


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���R���e�L�X�g�ɒǉ�����Ă���V�[���m�[�h�̌����擾����
   */
  //-----------------------------------------------------------------------------

#define GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Name,NwArrayName,NwFunctionName) \
  u32 GetNw##Name##NumInNwSceneContext(void) const \
  { \
    u32 num = 0; \
    nw::gfx::NwArrayName##Array::const_iterator end = m_scene_context->Get##NwFunctionName##End(); \
    for(nw::gfx::NwArrayName##Array::const_iterator i = m_scene_context->Get##NwFunctionName##Begin(); i != end; ++i) ++num; \
    return num; \
  }

  // ����
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(SceneNode,SceneNode,SceneNodes)
  
  // ��
  // UserRenderNode
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(UserRenderNode,UserRenderNode,UserRenderNodes)
  // ���f��(SkeletalModel, ParticleModel�̌����܂�)
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Model,Model,Models)
  // �X�P���^�����f��
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(SkeletalModel,SkeletalModel,SkeletalModels)
  // ���C�g(FragmentLight, VertexLight, HemiSphereLight, AmbientLight�̌����܂�)
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Light,Light,Lights)
  // �t���O�����g���C�g
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(FragmentLight,FragmentLight,FragmentLights)
  // ���_���C�g
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(VertexLight,VertexLight,VertexLights)
  // �������C�g
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(HemiSphereLight,HemiSphereLight,HemiSphereLights)
  // �A���r�G���g���C�g
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(AmbientLight,AmbientLight,AmbientLights)
  // �J����
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Camera,Camera,Camera)
  // �t�H�O
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Fog,Fog,Fog)
  // �p�[�e�B�N���Z�b�g
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(ParticleSet,ParticleSet,ParticleSet)
  // �p�[�e�B�N���G�~�b�^
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(ParticleEmitter,ParticleEmitter,ParticleEmitter)
  // �p�[�e�B�N�����f��
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(ParticleModel,ParticleModel,ParticleModel)
  // �A�j���[�V����
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(AnimatableNode,AnimatableNode,AnimatableNodes)
  
  // ���f���̃}�e���A��(�e���f���̃}�e���A���̌������v��������)(SkeletalModel, ParticleModel�̃}�e���A���̌����܂�)
  u32 GetNwModelMaterialCountInNwSceneContext(void) const
  {
    u32 num = 0;
    nw::gfx::ModelArray::const_iterator end = m_scene_context->GetModelsEnd();
    for(nw::gfx::ModelArray::const_iterator i = m_scene_context->GetModelsBegin(); i != end; ++i)
    {
      const nw::gfx::Model* model = static_cast<const nw::gfx::Model*>(*i);
      num += model->GetMaterialCount();
    }
    return num;
  }
  // �X�P���^�����f���̃}�e���A��(�e�X�P���^�����f���̃}�e���A���̌������v��������)
  u32 GetNwSkeletalModelMaterialCountInNwSceneContext(void) const
  {
    u32 num = 0;
    nw::gfx::SkeletalModelArray::const_iterator end = m_scene_context->GetSkeletalModelsEnd();
    for(nw::gfx::SkeletalModelArray::const_iterator i = m_scene_context->GetSkeletalModelsBegin(); i != end; ++i)
    {
      const nw::gfx::SkeletalModel* model = static_cast<const nw::gfx::SkeletalModel*>(*i);
      num += model->GetMaterialCount();
    }
    return num;
  }
  // �p�[�e�B�N�����f���̃}�e���A��(�e�p�[�e�B�N�����f���̃}�e���A���̌������v��������)
  u32 GetNwParticleModelMaterialCountInNwSceneContext(void) const
  {
    u32 num = 0;
    nw::gfx::ParticleModelArray::const_iterator end = m_scene_context->GetParticleModelEnd();
    for(nw::gfx::ParticleModelArray::const_iterator i = m_scene_context->GetParticleModelBegin(); i != end; ++i)
    {
      const nw::gfx::ParticleModel* model = static_cast<const nw::gfx::ParticleModel*>(*i);
      num += model->GetMaterialCount();
      ++num;
    }
    return num;
  }

#undef GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT
  
private:
  enum State
  {
    STATE_NONE,                 // �O���[�v��������
    STATE_AFTER_ADD_OR_REMOVE,  // �V�[���m�[�h��ǉ��������͍폜��������
    STATE_AFTER_INITIALIZE,     // Initialize����
    STATE_AFTER_CALCULATE,      // Calculate����
    
    STATE_ENUMS32MAX = ENUMS32MAX
  };

private:
  void SetState(const State state)
  {
    m_state = state;
    switch(m_state)
    {
    case STATE_AFTER_ADD_OR_REMOVE: m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;  break;
    case STATE_AFTER_INITIALIZE:    m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_NONE; break;
    }
  }
  State GetState(void) { return m_state; }
  void SetSceneInitializeProcessNeedOnByTreeFlag(b32 is_add)  // true�̂Ƃ�Add�Afalse�̂Ƃ�Remove
  {
    if( is_add )
    {
      m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;
    }
    else
    {
      m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;
      /* 
      if(m_scene_initialize_process_need == SCENE_INITIALIZE_PROCESS_NONE)
      {
        m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_TRAVERSE;
      }
      */
    }
  }
  void SetSceneInitializeProcessNeedOnByNotCullingFlag(b32 is_add)  // true�̂Ƃ�Add�Afalse�̂Ƃ�Remove
  {
    if( is_add )
    {
      m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;
      /*
      if(m_scene_initialize_process_need == SCENE_INITIALIZE_PROCESS_NONE)
      {
        m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_TRAVERSE;  // @note ���̗�2�̏ꍇ��ALL�ɂ��ׂ��Ȃ̂����A���̔��f�������ł͂ł��Ȃ��B
                                                                              //       ������A�œK�ȃ}�e���A��ID�ɂȂ��Ă��Ȃ��\��������B
                                                                              //       �œK�ȃ}�e���A��ID�ɂȂ��Ă��Ȃ������ŉ�������
                                                                              //       �}�e���A��ID�͓����Ă���̂ŁATRAVERSE�����ł�����ɖ��͂Ȃ��B
      }
      */
    }
    else
    {
      m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;
      /*
      if(m_scene_initialize_process_need == SCENE_INITIALIZE_PROCESS_NONE)
      {
        m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_TRAVERSE;
      }
      */
    }

    // ��1
    // (1) ���f������1��NotCullingFlag�ɂ���ăV�[���c���[����O�����V�[���c���[����O���������Ȃ̂�TRAVERSE�����ł悢
    // (2) TRAVERSE
    // (3) ���f������1��NotCullingFlag�ɂ���ăV�[���c���[�ɖ߂����V�[���c���[�͂��̃��f������1�ȊO�͕ύX���Ă��Ȃ��̂�TRAVERSE�����ł悢
    // (4) TRAVERSE
   
    // ��2
    // (1) ���f������1��NotCullingFlag�ɂ���ăV�[���c���[����O�����V�[���c���[����O���������Ȃ̂�TRAVERSE�����ł悢
    // (2) TRAVERSE 
    // (3) ���f������2��NotCullingFlag�ɂ���ăV�[���c���[����O�����V�[���c���[����O���������Ȃ̂�TRAVERSE�����ł悢
    // (4) TRAVERSE 
    // (5) ���̊Ԃɑ��̃��f������3���V�[���c���[�ɓ��遨�V�[���c���[���̏��Ԃ��ς�邩������Ȃ��̂�TRAVERSE�����ł͑���Ȃ�
    // (6) INITIALIZE TRAVERSE 
    // (7) ���f������1��NotCullingFlag�ɂ���ăV�[���c���[�ɖ߂������f������1���O��Ă���Ԃɑ��̃��f������3�������Ă����̂ŁA���f������1�����̂܂ܖ߂������ł͏��Ԃ��悭�Ȃ���������Ȃ��̂ŁATRAVERSE�����ł͑���Ȃ�
    // (8) INITIALIZE TRAVERSE  // @note ���̔��f�����̃N���X�ł͂ł��Ȃ��̂ŁAm_state, m_scene_initialize_process_need��Scene�Ɏ��ׂ�������
    // (9) ���t���[�����o��
    // (10) ���f������2��NotCullingFlag�ɂ���ăV�[���c���[�ɖ߂������f������2���O��Ă���Ԃɑ��̃��f������3�������Ă����̂ŁA���f������1�����̂܂ܖ߂������ł͏��Ԃ��悭�Ȃ���������Ȃ��̂ŁATRAVERSE�����ł͑���Ȃ�
    // (11) INITIALIZE TRAVERSE  // @note ���̔��f�����̃N���X�ł͂ł��Ȃ��̂ŁAm_state, m_scene_initialize_process_need��Scene�Ɏ��ׂ�������
  }
  SceneInitializeProcess GetSceneInitializeProcessNeed(void) { return m_scene_initialize_process_need; }

  nw::gfx::SceneContext*  GetSceneContext(void) const  {  return m_scene_context;  }
  nw::gfx::TransformNode* GetSceneRoot(void)    const  {  return m_scene_root;  }

  nw::gfx::ParticleContext* GetParticleContext(void) const  {  return m_particle_context;  }
  b32                       GetParticleEnableSwapBuffer(void) const  {  return m_particle_enable_swap_buffer;  }
  void                      SetParticleEnableSwapBuffer(b32 particle_enable_swap_buffer)  {  m_particle_enable_swap_buffer = particle_enable_swap_buffer;  }


private:
  // Scene��ݒ肷��
  void SetScene(Scene* scene, s32 scene_calculate_group_index );
  
  // �V�[���m�[�h��ǉ�����
  void AddSceneNode(SceneNode* scene_node);     // �V�[���֓o�^����B
                                                // scene_node�̃V�[���c���[�t���O��0�̂Ƃ��̓V�[���ւ̓o�^�͍s���邪�A�V�[���c���[�ւ̓o�^�͍s���Ȃ��B
  // �V�[���m�[�h���폜����
  void RemoveSceneNode(SceneNode* scene_node);  // �V�[���ւ̓o�^����������B

  // ���݂̃V�[���c���[�t���O�ŃV�[���c���[�̏�Ԃ��X�V����
  void RefreshSceneNodeTreeFlag(SceneNode* scene_node);  // �V�[���ւ̓o�^�͕ێ������܂܂ŁA�V�[���c���[�ւ̓o�^�����ύX�������Ƃ��Ɏg���B
                                                         // �V�[���֓o�^�������A�V�[���ւ̓o�^�������������Ƃ���AddSceneNode, RemoveSceneNode���g���B


private:
  State                       m_state;
  SceneInitializeProcess      m_scene_initialize_process_need;

  nw::gfx::SceneContext*      m_scene_context;
  nw::gfx::TransformNode*     m_scene_root;

  nw::gfx::ParticleContext*   m_particle_context;
  b8                          m_particle_enable_swap_buffer;
  s8                          m_padding[1];

  s16                         m_scene_calculate_group_index_borrow;  // ���̂Ƃ���Ő����������̂��؂�Ċo���Ă���
  Scene*                      m_scene_borrow;  // ���̂Ƃ���Ő����������̂��؂�Ċo���Ă���
};
 
}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENECALCULATEGROUP_H__
