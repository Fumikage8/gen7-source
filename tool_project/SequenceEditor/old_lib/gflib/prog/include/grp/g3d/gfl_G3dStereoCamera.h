#ifndef __GFL_G3DSTEREOCAMERA_H__
#define __GFL_G3DSTEREOCAMERA_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dStereoCamera.h
 *	@brief  3D�X�e���I�J����
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK
#include <nn/ulcd.h>

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dCamera.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�X�e���I�J�����N���X
//=====================================
class StereoCamera : public Camera
{
  GFL_FORBID_COPY_AND_ASSIGN(StereoCamera);


  friend class AnimationManager;


public:
  //
  // �萔
  //

  //-----------------------------------------------------------------------------
  /*
   *    �f�t�H���g�l
   */
  //-----------------------------------------------------------------------------
  static const f32 DEPTH_LEVEL_DEFAULT;
  static const f32 DEPTH_RANGE_DEFAULT;
  static const b32 REAL_SWITCH_DEFAULT;




public:
  //
  // �R���X�g���N�^ / �f�X�g���N�^
  //
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  StereoCamera(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~StereoCamera();


#if 0
  /*C++ Tips!
  �p���A�f�t�H���g�����Œ��ӂ��邱�ƁI
  class A
  {
  virtual void Create(s32 a);
  };
  class B : public A
  {
  virtual void Create(s32 a, s32 b=1);
  };
  A* p = new B;
  p->Create(0);
  �Ƃ����A::Create���Ă΂��̂ŁA���ӁI
  ��
  ���N���X��Camera��Create�Ɩ{�N���XStereoCamera��Create�𒍈ӂ��Ă������B*/
#endif

  //
  // ���� / �j��
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
   *  @param[in]     resource           ���\�[�X
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     description        ���\�[�X����J�����𐶐�����Ƃ��̐ݒ���e
   *
   *  depth_level �� DEPTH_LEVEL_DEFAULT
   *  depth_range �� DEPTH_RANGE_DEFAULT 
   *  real_switch �� REAL_SWITCH_DEFAULT
   *  ���ݒ肳���B
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource,
      ResourceDescription*              description = NULL
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]    allocator          �A���P�[�^
   *  @param[in]    device_allocator   �f�o�C�X�������̃A���P�[�^
   *  @param[in]    resource           ���\�[�X
   *  @param[in]    index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]    depth_level        �x�[�X�J���������ʂ܂ł̋���
   *  @param[in]    depth_range        �����̕␳�l
   *  @param[in]    real_switch        ���������o�����߂Ɏ���p�̕ύX��������(true�̂Ƃ�����)
   *  @param[in]    description        ���\�[�X����J�����𐶐�����Ƃ��̐ݒ���e
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*          allocator,
      gfl::heap::NwAllocator*          device_allocator,
      Resource*                        resource,
      s32                              index_in_resource,
      f32                              depth_level,
      f32                              depth_range       = DEPTH_RANGE_DEFAULT,
      b32                              real_switch       = REAL_SWITCH_DEFAULT,
      ResourceDescription*             description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     description    
   *
   *  depth_level �� DEPTH_LEVEL_DEFAULT
   *  depth_range �� DEPTH_RANGE_DEFAULT 
   *  real_switch �� REAL_SWITCH_DEFAULT
   *  ���ݒ肳���B
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*    allocator,
      Description*               description = NULL
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]    description    
   *  @param[in]    depth_level        �x�[�X�J���������ʂ܂ł̋���
   *  @param[in]    depth_range        �����̕␳�l
   *  @param[in]    real_switch        ���������o�����߂Ɏ���p�̕ύX��������(true�̂Ƃ�����)
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*    allocator,
      Description*               description,
      f32                        depth_level,
      f32                        depth_range       = DEPTH_RANGE_DEFAULT,
      b32                        real_switch       = REAL_SWITCH_DEFAULT
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]    allocator          �A���P�[�^
   *  @param[in]    base_camera        �x�[�X�J����
   *  @param[in]    base_camera_owner_this  �����base_camera���Ǘ����Ă��炤�ꍇ��true(true�ɂ����Ƃ��͂��ꂪbase_camera��������܂�)
   *  @param[in]    depth_level        �x�[�X�J���������ʂ܂ł̋���
   *  @param[in]    depth_range        �����̕␳�l
   *  @param[in]    real_switch        ���������o�����߂Ɏ���p�̕ύX��������(true�̂Ƃ�����)
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*          allocator,
      Camera*                          base_camera,
      b32                              base_camera_owner_this = false,
      f32                              depth_level       = DEPTH_LEVEL_DEFAULT,
      f32                              depth_range       = DEPTH_RANGE_DEFAULT,
      b32                              real_switch       = REAL_SWITCH_DEFAULT
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




  //
  // �J�����}�g���N�X
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����
   *                �x�[�X�J�����A���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
   */
  //-----------------------------------------------------------------------------
  virtual void UpdateCameraMatrix(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����
   *                (�x�[�X�J�����A)���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
   *
   *  @param[in]    update_base_camera  true�̂Ƃ��A�x�[�X�J�����A���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
   *                                    false�̂Ƃ��A�x�[�X�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X�͍X�V�����A
   *                                                 ���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
   *                                                 ���̍ہA�x�[�X�J�����Ɍ��ݐݒ肵�Ă���r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���g�p���܂��B
   */
  //-----------------------------------------------------------------------------
  void UpdateCameraMatrices(b32 update_base_camera = true);

  // �ȉ��͑S�ăx�[�X�J�����ɑ΂��Ă̊֐��ƂȂ�܂��B
#if 0
  void GetViewMatrix( gfl::math::MTX34* view_matrix ) const;
  void SetViewMatrix( const gfl::math::MTX34& view_matrix );
  void GetProjectionMatrix( gfl::math::MTX44* projection_matrix ) const;
  void SetProjectionMatrix( const gfl::math::MTX44& projection_matrix );
  void GetViewAndProjectionMatrix( gfl::math::MTX34* view_matrix, gfl::math::MTX44* projection_matrix ) const;
  void SetViewAndProjectionMatrix( const gfl::math::MTX34& view_matrix, const gfl::math::MTX44& projection_matrix );
#endif

  // �ȉ��͐̂̓x�[�X�J�����ɑ΂��Ă̊֐��������̂ł����A���̓x�[�X�A���ڗp�A�E�ڗp�ɑ΂��Ă̊֐��ƂȂ�܂��B
#if 0
  void UpdateCameraMatrix(void);
#endif




  //
  // �擾 / �ݒ�
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �x�[�X�J���������ʂ܂ł̋�����ݒ肷��
   *
   *  @param[in]    depth_level     �x�[�X�J���������ʂ܂ł̋���
   */
  //-----------------------------------------------------------------------------
  void SetDepthLevel(f32 depth_level) {  m_depth_level = depth_level;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �����̕␳�l��ݒ肷��
   *
   *  @param[in]    depth_range     �����̕␳�l
   */
  //-----------------------------------------------------------------------------
  void SetDepthRange(f32 depth_range)
  {
    // Panic: ulcd_StereoCamera.cpp:685
    // factor must be [0,1].

    if(depth_range<0.0f)      m_depth_range = 0.0f;
    else if(depth_range>1.0f) m_depth_range = 1.0f;
    else                      m_depth_range = depth_range;
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���������o�����߂Ɏ���p�̕ύX���������̃t���O��ݒ肷��
   *
   *  @param[in]    real_switch     ���������o�����߂Ɏ���p�̕ύX��������(true�̂Ƃ�����)
   */
  //-----------------------------------------------------------------------------
  void SetRealSwitch(b32 real_switch) {  m_real_switch = real_switch;  }

  //-----------------------------------------------------------------------------
  /**
   * @brief ���s�������̌��E������ݒ肵�܂��B
   *        limit�ɂ́A0�ȏォ�K�C�h���C���Œ�߂��Ă��鉜�s�������̎����̏���l�ȉ��̒l��ݒ�ł��܂��B�������A���݂͏���l�ȏ�̒l���w�肷��ƌx�����o�͂���݂̂ŁA�l�͔��f����܂��B
   *        limit��CalculateMatrices�֐��̌��ʂɔ��f����܂��B��̓I�ɂ́A�t�@�[�N���b�v�ʂ̈ʒu�̃I�u�W�F�N�g���ALCD�\�ʏ�ɂ����Ė{�֐��Ŏw�肵��������������悤�Ɍv�Z���s���܂��B
   *        �{�֐��Őݒ肷����E�����́A�u�A�v���P�[�V�����D��̕��@�v���u���s�������v�ɑ΂��Ă̂ݗL���ł��B
   *        �{�N���X�ł����΁ASetRealSwitch(false)�̎��̂ݗL���ł��B
   *
   * @param fLimit �ݒ肷����E����(mm)
   */
  //-----------------------------------------------------------------------------
  void SetLimitParallax(const float fLimit) { m_nn_stereo_camera->SetLimitParallax(fLimit); }

  //-----------------------------------------------------------------------------
  /**
   * @brief �ݒ肳��Ă��鉜�s�������̌��E�������擾���܂��B
   *        ��x�� SetLimitParallax �֐����g�p���Ă��Ȃ��ꍇ�́A�f�t�H���g�Ƃ��� �K�C�h���C���Œ�߂��Ă���ő�̌��E�����i���s�������j���Ԃ���܂��B 
   *
   *        �K�C�h���C���ɕύX���Ȃ����10.0f���ő�̌��E�����B
   *
   * @retval �ݒ蒆�̌��E����(mm)
   */
  //-----------------------------------------------------------------------------
  f32 GetLimitParallax(void) const { return m_nn_stereo_camera->GetLimitParallax(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �x�[�X�J���������ʂ܂ł̋������擾����
   *
   *  @retval       f32    �x�[�X�J���������ʂ܂ł̋���
   */
  //-----------------------------------------------------------------------------
  f32 GetDepthLevel(void) const {  return m_depth_level;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �����̕␳�l���擾����
   *
   *  @retval       f32    �����̕␳�l
   */
  //-----------------------------------------------------------------------------
  f32 GetDepthRange(void) const {  return m_depth_range;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���������o�����߂Ɏ���p�̕ύX���������̃t���O���擾����
   *
   *  @retval       b32    ���������o�����߂Ɏ���p�̕ύX��������(true�̂Ƃ�����)
   */
  //-----------------------------------------------------------------------------
  b32 GetRealSwitch(void) const {  return m_real_switch;  }
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �x�[�X�J�������擾����
   *
   *  @retval       Camera*    �x�[�X�J����
   */
  //-----------------------------------------------------------------------------
  Camera* GetBaseCamera(void) const
  {
    if( m_base_camera )
    {
      return m_base_camera;
    }
    else
    {
      const Camera* base_camera = this;         // this���g���Ă��邹����const�֐��ɂł��Ȃ������̂ŁA
      return const_cast<Camera*>(base_camera);  // ����ȕ��ɂ��Ă��܂��B
    }
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���ڗp�J�������擾����
   *
   *  @retval       Camera*    ���ڗp�J����
   */
  //-----------------------------------------------------------------------------
  Camera* GetLeftCamera(void) const {  return m_left_camera;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �E�ڗp�J�������擾����
   *
   *  @retval       Camera*    �E�ڗp�J����
   */
  //-----------------------------------------------------------------------------
  Camera* GetRightCamera(void) const {  return m_right_camera;  }




  //
  // ���� / �j��
  //

  //-----------------------------------------------------------------------------
  /**
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
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
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

  virtual void CreateFrustum(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3& cameraPosition,
    const gfl::math::VEC3& targetPosition,
    f32 nearClip,
    f32 farClip,
    const gfl::math::VEC2& projectionCenter,
    f32 projectionHeight,
    gfl::math::PivotDirection pivotDirection,
    f32 w_scale,
    f32                              depth_level       = DEPTH_LEVEL_DEFAULT,
    f32                              depth_range       = DEPTH_RANGE_DEFAULT,
    b32                              real_switch       = REAL_SWITCH_DEFAULT
  );



  //
  // NW4C�֘A �擾 / �ݒ�
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X���ێ����Ă���nw::gfx::Camera*���擾
   *
   *  @retval        nw::gfx::Camera*    ���̃N���X���ێ����Ă���nw::gfx::Camera
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::Camera* GetNwCamera(void) const
  {
  #if 1
    if( m_base_camera )
    {
      return m_base_camera->GetNwCamera();
    }
    else
    {
      return Camera::GetNwCamera();
    }
  #else
    return GetBaseCamera()->Camera::GetNwCamera();  // virtual�ɂ��Ă���̂ŁACamera::��t���Ă����Ȃ��ƁAStereoCamera��GetNwCamera�֐������x���Ăяo����Ă��܂��B
  #endif
  } 
  // ����GetNwCamera��GetNwSceneNode���ĂׂȂ��悤�ɂ��A�����GetNwCameraOfBase�Ƃ���p�ӂ��ׂ��B
  // ���������ꂪ�g���Ȃ��Ɗ��N���XCamera�̃A�j���[�V�����ō��������ƂɂȂ�B


protected:
  virtual nw::gfx::SceneNode* GetNwSceneNode(void) const
  {
    if( m_base_camera )
    {
      return m_base_camera->GetNwSceneNode();  // �e�N���X��protected�֐����C���X�^���X����ĂԂ��Ƃ͂ł��Ȃ��B���q�N���X��e�N���X��friend�ɂ����B
    }
    else
    {
      return Camera::GetNwSceneNode();  // �e�N���X��protected�֐��𒼐ڌĂԂ��Ƃ͂ł���B
    }
  }



private:
  b32                           m_base_camera_owner_this;  // m_base_camera!=NULL�̂Ƃ��̂ݗL��
                                                           // ���ꂪm_base_camera���Ǘ�����ꍇ��true(true�̂Ƃ��͂��ꂪm_base_camera��������܂�)
  Camera*                       m_base_camera;             // �x�[�X�J����  // ���N���X��m_nw_scene_node���g���ꍇ��NULL
  
  Camera*                       m_left_camera;    // ���ڗp�J����
  Camera*                       m_right_camera;   // �E�ڗp�J����

  nn::ulcd::CTR::StereoCamera*  m_nn_stereo_camera;
  
  f32                           m_depth_level;
  f32                           m_depth_range;
  b32                           m_real_switch;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSTEREOCAMERA_H__
