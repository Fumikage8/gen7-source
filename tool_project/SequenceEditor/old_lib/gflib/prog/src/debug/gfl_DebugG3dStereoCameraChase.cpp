//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugG3dStereoCameraChase.cpp
 *	@brief  �f�o�b�O3D�X�e���I�J�����ǐՌ^
 *	@author	Koji Kawada
 *	@date		2011.02.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Math.h>

#include <ui/gfl_UI_DeviceManager.h>
#include <ui/gfl_UI_VectorDevice.h>
#include <ui/gfl_UI_Button.h>
#include <ui/ctr/gfl_UI_CTR_DeviceManager.h>

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <debug/gfl_DebugG3dStereoCameraChase.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dStereoCamera.h>

namespace gfl {
namespace debug {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�f�o�b�O3D�X�e���I�J�����ǐՌ^�N���X
//=====================================

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
void DebugG3dStereoCameraChase::Create(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3&           camera_position,
    const gfl::math::VEC3&           target_position,
    f32                              near_clip,
    f32                              far_clip,
    f32                              fovy_radian,
    f32                              w_scale,
    f32                              depth_level,
    f32                              depth_range,
    bool                             real_switch
)
{
  gfl::grp::g3d::StereoCamera::Create(
    allocator,
    camera_position,
    target_position,
    near_clip,
    far_clip,
    fovy_radian,
    w_scale,
    depth_level,
    depth_range,
    real_switch
  );

  // �Ǝ����� ��������
  UpdateDebugCamera();
  {
    GFL_PRINT("[camera controller usage]\n");
    GFL_PRINT("Cross Key : rotate\n");
    GFL_PRINT("        L : near\n");
    GFL_PRINT("        R : far\n");
    GFL_PRINT("        X : up\n");
    GFL_PRINT("        Y : down\n");
  }
  // �Ǝ����� �����܂�
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::Destroy(void)
{
  gfl::grp::g3d::StereoCamera::Destroy();

  m_initial_rotate           = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_initial_target_pos       = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_initial_target_distance  = DEFAULT_DISTANCE;
  m_initial_target_offset_y  = 0.0f;
  m_rotate                   = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_target_pos               = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_target_distance          = DEFAULT_DISTANCE;
  m_target_offset_y          = 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *                 �Ă񂾃t���[���̑��삪���f�����B���t���[���ĂׂΖ��t���[������\�B
 *
 *  @param[in]     device_manager    �f�o�C�X�}�l�[�W���[
 *  @param[in]     target_position   �J�����̒����_�̈ʒu
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::Control(const gfl::ui::DeviceManager* device_manager, const gfl::math::VEC3& target_position )
{
  const gfl::ui::VectorDevice* stick     = device_manager->GetStick(0);     // �A�i���O�X�e�B�b�N
  const gfl::ui::VectorDevice* cross_key = device_manager->GetCrossKey(0);  // �\���L�[
  const gfl::ui::Button*       button    = device_manager->GetButton(0);    // �{�^��

  if( button->IsHold( gfl::ui::BUTTON_X ) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_offset_y += move_speed;
  }
  else if( button->IsHold( gfl::ui::BUTTON_Y ) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_offset_y -= move_speed;
  }

  else if( button->IsHold( gfl::ui::BUTTON_L ) )
  {
    f32 dolly_speed = DOLLY_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_distance -= dolly_speed;
    if( m_target_distance < MIN_DISTANCE ) m_target_distance = MIN_DISTANCE; 
  }
  else if( button->IsHold( gfl::ui::BUTTON_R ) )
  {
    f32 dolly_speed = DOLLY_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_distance += dolly_speed;
  }

  else if( cross_key->IsHold( gfl::ui::DIR_UP ) )
  {
    m_rotate.x -= 1.0f * TUMBLE_SPEED;
    m_rotate.x = gfl::math::Clamp(m_rotate.x, -ROTATE_X_LIMIT, ROTATE_X_LIMIT);
  }
  else if( cross_key->IsHold( gfl::ui::DIR_DOWN ) )
  {
    m_rotate.x += 1.0f * TUMBLE_SPEED;
    m_rotate.x = gfl::math::Clamp(m_rotate.x, -ROTATE_X_LIMIT, ROTATE_X_LIMIT);
  }
  else if( cross_key->IsHold( gfl::ui::DIR_LEFT ) )
  {
    m_rotate.y += 1.0f * TUMBLE_SPEED;
  }
  else if( cross_key->IsHold( gfl::ui::DIR_RIGHT ) )
  {
    m_rotate.y -= 1.0f * TUMBLE_SPEED;
  }
  
  Control(target_position);
}

void DebugG3dStereoCameraChase::Control( const gfl::math::VEC3& target_position )
{
  m_initial_target_pos = m_target_pos = target_position;
  
  UpdateDebugCameraFromParameter(); 
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu��ݒ肷��
 *
 *  @param[in]    camera_position    �J�����̈ʒu
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::SetCameraPosition( const gfl::math::VEC3& camera_position )
{
  gfl::grp::g3d::Camera::SetCameraPosition( camera_position );
  
  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̒����_�̈ʒu��ݒ肷��
 *
 *  @param[in]    target_position    �J�����̒����_�̈ʒu
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::SetTargetPosition( const gfl::math::VEC3& target_position )
{
  gfl::grp::g3d::Camera::SetTargetPosition( target_position );
  
  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu��ݒ肷��
 *
 *  @param[in]    camera_position    �J�����̈ʒu
 *  @param[in]    target_position    �J�����̒����_�̈ʒu
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position )
{
  gfl::grp::g3d::Camera::SetCameraAndTargetPosition( camera_position, target_position );

  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �p�����[�^�o��
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::PrintParameter(void) const
{
//  GFL_PRINT("m_initial_rotate=(%f, %f, %f)\n", m_initial_rotate.x, m_initial_rotate.y, m_initial_rotate.z);
//  GFL_PRINT("m_initial_target_pos=(%f, %f, %f)\n", m_initial_target_pos.x, m_initial_target_pos.y, m_initial_target_pos.z);
//  GFL_PRINT("m_initial_target_distance=%f\n", m_initial_target_distance);
//  GFL_PRINT("m_initial_target_offset_y=%f\n", m_initial_target_offset_y);
  GFL_PRINT("m_rotate=(%f, %f, %f)\n", m_rotate.x, m_rotate.y, m_rotate.z);
//  GFL_PRINT("m_target_pos=(%f, %f, %f)\n", m_target_pos.x, m_target_pos.y, m_target_pos.z);
  GFL_PRINT("m_target_distance=%f\n", m_target_distance);
  GFL_PRINT("m_target_offset_y=%f\n", m_target_offset_y);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �p�����[�^�̎擾/�ݒ�
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::SetParameterRotate(const gfl::math::VEC3& rotate)
{
  m_initial_rotate = m_rotate = rotate;
  UpdateDebugCameraFromParameter();
}
void DebugG3dStereoCameraChase::GetParameterRotate(gfl::math::VEC3* rotate) const
{
  *rotate = m_rotate;
}

void DebugG3dStereoCameraChase::SetParameterTargetPos(const gfl::math::VEC3& pos)
{
  m_initial_target_pos = m_target_pos = pos;
  UpdateDebugCameraFromParameter();
}
void DebugG3dStereoCameraChase::GetParameterTargetPos(gfl::math::VEC3* pos) const
{
  *pos = m_target_pos;
}

void DebugG3dStereoCameraChase::SetParameterTargetDistance(const f32 distance)
{
  m_initial_target_distance = m_target_distance = distance;
  UpdateDebugCameraFromParameter();
}
f32  DebugG3dStereoCameraChase::GetParameterTargetDistance(void) const
{
  return m_target_distance;
}

void DebugG3dStereoCameraChase::SetParameterTargetOffsetY(const f32 y)
{
  m_initial_target_offset_y = m_target_offset_y = y;
  UpdateDebugCameraFromParameter();
}
f32  DebugG3dStereoCameraChase::GetParameterTargetOffsetY(void) const
{
  return m_target_offset_y;
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ���ݐݒ肳��Ă���J�����̈ʒu�ƒ����_�̈ʒu���f�o�b�O�J�����ɔ��f����
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::UpdateDebugCamera(void)
{
  gfl::math::VEC3 camera_pos;
  gfl::math::VEC3 rotate;
  gfl::math::VEC3 target_pos;
  f32             target_distance;
  
  GetCameraPosition(camera_pos);
  GetTargetPosition(target_pos);

  target_distance = gfl::math::VEC3Dist(&target_pos, &camera_pos);
  GFL_ASSERT( target_distance >= MIN_DISTANCE );  // @note ������0�ɋ߂��Ƃ����l��
  if( target_distance < MIN_DISTANCE ) target_distance = MIN_DISTANCE; 

  gfl::math::VEC3 look_reverse = (camera_pos - target_pos) / target_distance;
  rotate.x = gfl::math::AsinFIdx(-look_reverse.y);
  rotate.y = gfl::math::Atan2FIdx(-look_reverse.x, look_reverse.z);
  rotate.z = 0.0f;

  m_initial_rotate          = m_rotate           = rotate;
  m_initial_target_pos      = m_target_pos       = target_pos;
  m_initial_target_distance = m_target_distance  = target_distance;
  m_initial_target_offset_y = m_target_offset_y  = 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ���ݐݒ肳��Ă���p�����[�^�Ńf�o�b�O�J�������X�V����
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCameraChase::UpdateDebugCameraFromParameter(void)
{
  {
    gfl::math::VEC3 target_pos( m_target_pos.x, m_target_pos.y + m_target_offset_y, m_target_pos.z );
    gfl::grp::g3d::Camera::SetTargetPosition(target_pos);

    f32 xs = gfl::math::SinFIdx(m_rotate.x);
    f32 xc = gfl::math::CosFIdx(m_rotate.x);
    f32 ys = gfl::math::SinFIdx(m_rotate.y);
    f32 yc = gfl::math::CosFIdx(m_rotate.y);
    gfl::math::VEC3 camera_pos(
      target_pos.x - m_target_distance * xc * ys,
      target_pos.y - m_target_distance * xs,
      target_pos.z + m_target_distance * xc * yc
    );
    gfl::grp::g3d::Camera::SetCameraPosition(camera_pos);
  }
}

}  // namespace debug
}  // namespace gfl

