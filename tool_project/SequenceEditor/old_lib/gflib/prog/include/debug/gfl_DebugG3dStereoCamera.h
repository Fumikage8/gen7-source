#ifndef __GFL_DEBUGG3DSTEREOCAMERA_H__
#define __GFL_DEBUGG3DSTEREOCAMERA_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugG3dStereoCamera.h
 *	@brief  �f�o�b�O3D�X�e���I�J����
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
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <ui/ctr/gfl_UI_CTR_DeviceManager.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dStereoCamera.h>

namespace gfl {
namespace debug {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�f�o�b�O3D�X�e���I�J�����N���X
//=====================================
class DebugG3dStereoCamera : public gfl::grp::g3d::StereoCamera
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugG3dStereoCamera);

public:
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  DebugG3dStereoCamera(void)
    : gfl::grp::g3d::StereoCamera(),
      m_initial_rotate(0.0f, 0.0f, 0.0f),
      m_initial_target_pos(0.0f, 0.0f, 0.0f),
      m_initial_target_distance(DEFAULT_DISTANCE),
      m_rotate(0.0f, 0.0f, 0.0f),
      m_target_pos(0.0f, 0.0f, 0.0f),
      m_target_distance(DEFAULT_DISTANCE)
  {
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~DebugG3dStereoCamera()
  {
    Destroy();
  }
 
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
   *  @param[in]    depth_level        �x�[�X�J���������ʂ܂ł̋���
   *  @param[in]    depth_range        �����̕␳�l
   *  @param[in]    real_switch        ���������o�����߂Ɏ���p�̕ύX��������(true�̂Ƃ�����)
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*          allocator,
      const gfl::math::VEC3&           camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&           target_position   = TARGET_POSITION_DEFAULT,
      f32                              near_clip         = NEAR_CLIP_DEFAULT,
      f32                              far_clip          = FAR_CLIP_DEFAULT,
      f32                              fovy_radian       = FOVY_RADIAN_DEFAULT,
      f32                              w_scale           = 0.0f,
      f32                              depth_level       = DEPTH_LEVEL_DEFAULT,
      f32                              depth_range       = DEPTH_RANGE_DEFAULT,
      b32                              real_switch       = REAL_SWITCH_DEFAULT
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
  // Control�t���O
  enum
  {
    FLAG_CONT_RESET = 1<<0,
    FLAG_CONT_FORWARD_BACK = 1<<1,
    FLAG_CONT_TRANSLATE = 1<<2,
    FLAG_CONT_TRANSLATE_XZ = 1<<3,
    FLAG_CONT_UP_DOWN = 1<<4,
    FLAG_CONT_ROTATE = 1<<5,

    FLAG_CONT_NORMAL = (FLAG_CONT_RESET | FLAG_CONT_FORWARD_BACK | FLAG_CONT_TRANSLATE | FLAG_CONT_UP_DOWN |FLAG_CONT_ROTATE),
  };
  
  //-----------------------------------------------------------------------------
  /*
   *  @brief         ����
   *                 �Ă񂾃t���[���̑��삪���f�����B���t���[���ĂׂΖ��t���[������\�B
   *
   *  @param[in]     device_manager    �f�o�C�X�}�l�[�W���[
   */
  //-----------------------------------------------------------------------------
  void Control(const gfl::ui::DeviceManager* device_manager, u32 flag = FLAG_CONT_NORMAL);

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

#endif // __GFL_DEBUGG3DSTEREOCAMERA_H__
