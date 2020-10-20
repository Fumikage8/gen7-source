#ifndef __GFL_G3DSHADOWGROUP_H__
#define __GFL_G3DSHADOWGROUP_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dShadowGroup.h
 *	@brief  3D�e�O���[�v
 *	@author	Koji Kawada
 *	@date		2011.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp
#include <grp/gfl_RenderTarget.h>

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
///	3D�e�O���[�v�N���X
//=====================================
class ShadowGroup
{
  GFL_FORBID_COPY_AND_ASSIGN(ShadowGroup);

  
  // ���L��friend�����private�����o�֐��ɃA�N�Z�X���Ă悢
  friend class ShadowSystem;




public:
  //---------------------------------------------------------------------------
  /**
   *           �萔�錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	�e�̃^�C�v
  //=====================================
  enum Type
  {
    TYPE_NONE,                 // �Ȃ�
    TYPE_PROJECTION_SHADOW,    // �v���W�F�N�V�����V���h�E
    TYPE_SHADOW_MAP            // �V���h�E�}�b�v
  };




  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	�ݒ���e
  //=====================================
  struct Description
  {
    Type                                   type;                       // �e�̃^�C�v
    s32                                    scene_draw_group_index;     // �V�[���`��O���[�v
    s32                                    camera_index;               // �J�����Ɋ���U��C���f�b�N�X
    gfl::grp::g3d::Camera::Description*    camera_description;         // �J�����̐ݒ���e
    gfl::grp::RenderTarget::Description*   render_target_description;  // �����_�[�^�[�Q�b�g�̐ݒ���e

    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : type                          (TYPE_NONE),
        scene_draw_group_index        (SCENE_DRAW_GROUP_INDEX_NONE),
        camera_index                  (CAMERA_INDEX_NONE),
        camera_description            (NULL), 
        render_target_description     (NULL)
    {}
  };




  // �ȉ��̃����o�֐���this�y��friend���炵���A�N�Z�X���܂���
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  ShadowGroup(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  ShadowGroup(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const Description*      description = NULL);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~ShadowGroup();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  void Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const Description*      description = NULL);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �擾
   */
  //-----------------------------------------------------------------------------
  Type                          GetType(void) const                { return m_type; }
  s32                           GetSceneDrawGroupIndex(void) const { return m_scene_draw_group_index; }
  s32                           GetCameraIndex(void) const         { return m_camera_index; }
  gfl::grp::g3d::Camera*        GetCamera(void) const              { return m_camera; }
  gfl::grp::RenderTarget*       GetRenderTarget(void) const        { return m_render_target; }



  
  // �ȉ��̃����o�ϐ���this���炵���A�N�Z�X���܂���
private:
  Type                          m_type;
  s32                           m_scene_draw_group_index;
  s32                           m_camera_index;
  gfl::grp::g3d::Camera*        m_camera;
  gfl::grp::RenderTarget*       m_render_target;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSHADOWGROUP_H__
