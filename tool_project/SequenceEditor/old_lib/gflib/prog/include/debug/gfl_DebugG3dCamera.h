#ifndef __GFL_DEBUGG3DCAMERA_H__
#define __GFL_DEBUGG3DCAMERA_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugG3dCamera.h
 *	@brief  3D�J����
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <ui/ctr/gfl_UI_CTR_DeviceManager.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dCamera.h>

namespace gfl {
namespace debug {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�J�����N���X
//=====================================
class DebugG3dCamera : public gfl::grp::g3d::Camera
{
public:
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  DebugG3dCamera(void)
    : gfl::grp::g3d::Camera(),
      m_initial_rotate(0.0f, 0.0f, 0.0f),
      m_initial_target_pos(0.0f, 0.0f, 0.0f),
      m_initial_target_distance(DEFAULT_DISTANCE)
  {
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~DebugG3dCamera()
  {
    //gfl::grp::g3d::Camera::~Camera();  // @note �e�̃f�X�g���N�^�͏����Ȃ��Ă��Ă΂��̂ŁA��������2��Ăяo���ɂȂ��Ă��܂��̂ŁA�_���B
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         ����
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
   *  @param[in]     resource           ���\�[�X
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      gfl::grp::g3d::Resource*          resource,
      s32                               index_in_resource
  );
  //-----------------------------------------------------------------------------
  /*
   *  @brief        ����
   * 
   *  @param[in]    allocator          �����Ɏg�p����A���P�[�^�[
   *  @param[in]    camera_position    �J�����̈ʒu���W
   *  @param[in]    target_position    �J�����^�[�Q�b�g�̍��W
   *  @param[in]    near_clip          �j�A�N���b�v�̒l
   *  @param[in]    far_clip           �t�@�[�N���b�v�̒l
   *  @param[in]    fovy_radian        FOV��Y�����̒l
   *  @param[in]    pivot_direction    ��ʂ̏����
   *  @param[in]    w_scale            WScale�̒l
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*          allocator,
      const gfl::math::VEC3&           camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&           target_position   = TARGET_POSITION_DEFAULT,
      f32                              near_clip         = NEAR_CLIP_DEFAULT,
      f32                              far_clip          = FAR_CLIP_DEFAULT,
      f32                              fovy_radian       = FOVY_RADIAN_DEFAULT,
      gfl::math::PivotDirection        pivot_direction   = gfl::math::PIVOT_NONE,//gfl::math::PIVOT_UPSIDE_TO_TOP,
      f32                              w_scale           = 0.0f
  );
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /*
   *  @brief        �J�����̈ʒu��ݒ肷��
   *
   *  @param[in]    camera_position    �J�����̈ʒu
   */
  //-----------------------------------------------------------------------------
  virtual void SetCameraPosition( const gfl::math::VEC3& camera_position );
  //-----------------------------------------------------------------------------
  /*
   *  @brief        �J�����̒����_�̈ʒu��ݒ肷��
   *
   *  @param[in]    target_position    �J�����̒����_�̈ʒu
   */
  //-----------------------------------------------------------------------------
  virtual void SetTargetPosition( const gfl::math::VEC3& target_position );
  //-----------------------------------------------------------------------------
  /*
   *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu��ݒ肷��
   *
   *  @param[in]    camera_position    �J�����̈ʒu
   *  @param[in]    target_position    �J�����̒����_�̈ʒu
   */
  //-----------------------------------------------------------------------------
  virtual void SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position );

public:
  //-----------------------------------------------------------------------------
  /*
   *  @brief         ����
   *                 �Ă񂾃t���[���̑��삪���f�����B���t���[���ĂׂΖ��t���[������\�B
   *
   *  @param[in]     device_manager    �f�o�C�X�}�l�[�W���[
   */
  //-----------------------------------------------------------------------------
  void Control(const gfl::ui::DeviceManager* device_manager);

private:
  //-----------------------------------------------------------------------------
  /*
   *  @brief        ���ݐݒ肳��Ă���J�����̈ʒu�ƒ����_�̈ʒu���f�o�b�O�J�����ɔ��f����
   */
  //-----------------------------------------------------------------------------
  void UpdateDebugCamera(void);

private:
  static const f32 DOLLY_SPEED_BASE = 30.0f / 60.0f;
  static const f32 TUMBLE_SPEED     = 128.0f / 60.0f;
  static const f32 MOVE_SPEED_BASE  = 30.0f / 60.0f;
  static const f32 ROTATE_X_LIMIT   = 64.f - TUMBLE_SPEED;
  static const f32 DEFAULT_DISTANCE = 10.0f;
  static const f32 MIN_DISTANCE     = 0.01f;

private:
  gfl::math::VEC3    m_initial_rotate;
  gfl::math::VEC3    m_initial_target_pos;
  f32                m_initial_target_distance;

  gfl::math::VEC3    m_rotate;
  gfl::math::VEC3    m_target_pos;
  f32                m_target_distance;
};

}  // namespace debug
}  // namespace gfl

#endif // __GFL_DEBUGG3DCAMERA_H__
