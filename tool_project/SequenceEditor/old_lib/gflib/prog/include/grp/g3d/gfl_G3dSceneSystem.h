#ifndef __GFL_G3DSCENESYSTEM_H__
#define __GFL_G3DSCENESYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneSystem.h
 *	@brief  3D�V�[���V�X�e��
 *	@author	Koji Kawada
 *	@date		2010.10.12
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
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�V�[���V�X�e���N���X
//=====================================
class SceneSystem 
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneSystem);


  // ���L�N���X���炵���A�N�Z�X���Ȃ�
  friend class RenderSystem;
  friend class Scene;


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
    // m_scene_initializer��IMaterialIdGenerator�p�̐ݒ�
    s32  max_materials;           // �}�e���A���̍ő吔
    bool is_fixed_size_memory;    // �������ȊO�ɂ����������m�ۂ��邩�ǂ����̃t���O

    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : max_materials         (128),
        is_fixed_size_memory  (true)
    {}
  };

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     description  �V�[���V�X�e���̐ݒ���e
   *                              (NULL�̂Ƃ��̓f�t�H���g�l�B
   *                              �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   */
  //-----------------------------------------------------------------------------
  SceneSystem(gfl::heap::NwAllocator* allocator, const Description* description = NULL);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~SceneSystem();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���̏�����
   *
   *  @param[in]     scene_root                ����������V�[���̃��[�g�m�[�h
   *  @param[in]     scene_initialize_process  �����������̓��e
   */
  //-----------------------------------------------------------------------------
  void InitializeScene(SceneCalculateGroup* scene_calculate_group, const SceneInitializeProcess scene_itialize_process);

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[�g�m�[�h�̃g���o�[�X
   *
   *  @param[in]     scene_root   �Ώۂ̃��[�g�m�[�h
   */
  //-----------------------------------------------------------------------------
  void TraverseScene(SceneCalculateGroup* scene_calculate_group);

public:

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // �V�[���̍X�V��
  // UpdateScene���ĂԂ��A
  // UpdateSceneBeforeCulling��UpdateSceneAfterCulling�����̏��ԂŌĂԂ�
  // �̂ǂ��炩�ɂ��ĉ������B
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���̍X�V
   */
  //-----------------------------------------------------------------------------
  void UpdateScene(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���̍X�V�i�J�����O�O�j
   */
  //-----------------------------------------------------------------------------
  void UpdateSceneBeforeCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���̍X�V�i�J�����O��j
   */
  //-----------------------------------------------------------------------------
  void UpdateSceneAfterCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �p�[�e�B�N���̃m�[�h�Ɋւ���Calculate�����s��(SceneUpdater���������s���āAParticleSceneUpdater�����͍s��Ȃ�)
   */
  //-----------------------------------------------------------------------------
  void UpdateParticleNode(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle);
  void UpdateParticleNodeBeforeCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle);
  void UpdateParticleNodeAfterCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �p�[�e�B�N���̍X�V(ParticleSceneUpdater����)
   */
  //-----------------------------------------------------------------------------
  void UpdateParticle(SceneCalculateGroup* scene_calculate_group);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���A�b�v�f�[�^�̎擾
   *
   *  @retval        nw::gfx::ISceneUpdater*    �V�[���A�b�v�f�[�^
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ISceneUpdater*  GetSceneUpdater(void) const  {  return m_scene_updater;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �p�[�e�B�N���V�[���A�b�v�f�[�^�̎擾
   *
   *  @retval        nw::gfx::ParticleSceneUpdater*    �V�[���A�b�v�f�[�^
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ParticleSceneUpdater*  GetParticleSceneUpdater(void) const  {  return m_particle_scene_updater;  }

  //----------------------------------------------------------------------------
  /**
   *	@brief    UpdateScene�ɂ����������Ԃ����O�o��
   */
  //-----------------------------------------------------------------------------
  void DebugUpdateSceneTickOutPut(void);

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         SkeletonUpdater�𓾂�
   *
   *  @retval        SkeletonUpdater
   */
  //-----------------------------------------------------------------------------
  nw::gfx::SkeletonUpdater*   GetSkeletonUpdater(void)  const { return m_skeleton_updater; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         BillboardUpdater�𓾂�
   *
   *  @retval        BillboardUpdater
   */
  //-----------------------------------------------------------------------------
  nw::gfx::BillboardUpdater*  GetBillboardUpdater(void) const { return m_billboard_updater; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         DepthSortMode�𓾂�
   *
   *  @retval        DepthSortMode
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ISceneUpdater::DepthSortMode  GetDepthSortMode(void) const
  {
    if(m_scene_updater){ return m_scene_updater->GetDepthSortMode(); }
    else{ GFL_ASSERT_MSG(0, "m_scene_updater=NULL\n"); return nw::gfx::ISceneUpdater::SORT_DEPTH_OF_ALL_MESH; }
  }

private:
  nw::gfx::SceneTraverser*     m_scene_traverser;
  nw::gfx::WorldMatrixUpdater* m_world_matrix_updater;  // �o���Ă��������A�j���͑��ɔC����
  nw::gfx::SkeletonUpdater*    m_skeleton_updater;      // �o���Ă��������A�j���͑��ɔC����
  nw::gfx::BillboardUpdater*   m_billboard_updater;     // �o���Ă��������A�j���͑��ɔC����
  nw::gfx::SceneUpdater*       m_scene_updater;
  nw::gfx::SceneInitializer*   m_scene_initializer;

  nw::gfx::ParticleSceneUpdater*  m_particle_scene_updater;

private:
  void SetParticleEnableSwapBuffer(SceneCalculateGroup* scene_calculate_group, b32 particle_enable_swap_buffer)
  {
    scene_calculate_group->SetParticleEnableSwapBuffer(particle_enable_swap_buffer);
  }
};
 
}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENESYSTEM_H__
