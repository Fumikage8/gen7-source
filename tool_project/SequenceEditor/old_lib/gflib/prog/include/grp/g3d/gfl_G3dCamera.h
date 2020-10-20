#ifndef __GFL_G3DCAMERA_H__
#define __GFL_G3DCAMERA_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dCamera.h
 *	@brief  3D�J����
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>
#include <grp/gfl_Camera.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dAnimationManager.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dCameraViewClip.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�J�����N���X
//=====================================
class Camera : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Camera);


  friend class AnimationManager;
  friend class StereoCamera;


public:

  // �r���[���[�h
  enum ViewMode
  {
    VIEW_MODE_AIM,      // Aim�J����
    VIEW_MODE_LOOK_AT,  // LookAt�J����
    VIEW_MODE_ROTATE    // Rotate�J����
  };

  // �ˉe���[�h
  enum ProjectionMode
  {
    PROJECTION_MODE_PERSPECTIVE,
    PROJECTION_MODE_FRUSTUM,
    PROJECTION_MODE_ORTHO,
  };

  // ���\�[�X����J�����𐶐�����Ƃ��̐ݒ���e
  struct ResourceDescription
  {
    b32  animation_enable;          // �A�j���[�V�������s���ꍇ��true(�A�j���[�V���������蓖�Ă�X���b�g��1����܂�)
    u32  animation_blend_unit_num;  // �X���b�g���ŃA�j���[�V�����̃u�����h���s���ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł��X���b�g����1���j�b�g������܂�)
    u32  animation_num_in_unit;     // ���j�b�g���ŕ�ԃu�����h���Ȃ���A�j���[�V������ύX����ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł����j�b�g����1����܂�)
    ResourceDescription(void)
      : animation_enable(false),
        animation_blend_unit_num(0),
        animation_num_in_unit(0)
    {}
  };
  // ResourceDescription�̏������֐�
  // �f�t�H���g�l��ݒ肷��
  static void InitializeResourceDescriptionForDefault(
      ResourceDescription*  description
  );

  // �ݒ���e
  struct Description
  {
    ViewMode                   view_mode;
    gfl::math::VEC3            camera_position;          //  AIM  LOOK_AT  ROTATE  �̂Ƃ��g�p
    gfl::math::VEC3            target_position;          //  AIM  LOOK_AT          �̂Ƃ��g�p
    gfl::math::VEC3            up_vector;                //       LOOK_AT          �̂Ƃ��g�p
    f32                        twist;  // [radian]       //  AIM                   �̂Ƃ��g�p
    gfl::math::VEC3            rotate_xyz;  // [radian]  //                ROTATE  �̂Ƃ��g�p
    f32                        w_scale;                  // W�X�P�[���̒l��0.0f�ȊO�̂Ƃ�W�o�b�t�@(�N���b�v���W�n�ŗp������W�̒l���i�[����ׂ̋L���̈�)���g�p
    ProjectionMode             projection_mode;
    f32                        fovy;  // [radian]        //  PROJECTION_MODE_PERSPECTIVE                                                  �̂Ƃ��g�p
    f32                        aspect;                   //  PROJECTION_MODE_PERSPECTIVE  PROJECTION_MODE_FRUSTUM  PROJECTION_MODE_ORTHO  �̂Ƃ��g�p
    f32                        height;                   //                               PROJECTION_MODE_FRUSTUM  PROJECTION_MODE_ORTHO  �̂Ƃ��g�p
    gfl::math::VEC2            center;                   //                               PROJECTION_MODE_FRUSTUM  PROJECTION_MODE_ORTHO  �̂Ƃ��g�p
    f32                        near;
    f32                        far;
    gfl::math::PivotDirection  pivot;
    s32                        max_callbacks;            // �Ǘ��ł���R�[���o�b�N�̍ő吔
    s32                        max_children;             // �q�̍ő吔
    b32                        is_fixed_size_memory;     // �������ȊO�ɂ����������m�ۂ��邩�ǂ����̃t���O
    b32                        animation_enable;          // �A�j���[�V�������s���ꍇ��true(�A�j���[�V���������蓖�Ă�X���b�g��1����܂�)
    u32                        animation_blend_unit_num;  // �X���b�g���ŃA�j���[�V�����̃u�����h���s���ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł��X���b�g����1���j�b�g������܂�)
    u32                        animation_num_in_unit;     // ���j�b�g���ŕ�ԃu�����h���Ȃ���A�j���[�V������ύX����ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł����j�b�g����1����܂�)
    Description(void)
      : view_mode(VIEW_MODE_LOOK_AT),
        camera_position(0.0f,0.0f,100.0f),
        target_position(0.0f,0.0f,0.0f),
        up_vector(0.0f,1.0f,0.0f),
        twist(GFL_MATH_DEG_TO_RAD(0.0f)),
        rotate_xyz(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f)),
        w_scale(0.0f),
        projection_mode(PROJECTION_MODE_PERSPECTIVE),
        fovy(GFL_MATH_DEG_TO_RAD(37.8f)),
        aspect(400.0f/240.0f),
        height(240.0f),
        center(0.0f,0.0f),
        near(1.0f),
        far(10000.0f),
        pivot(gfl::math::PIVOT_NONE),
        max_callbacks(0),
        max_children(0),
        is_fixed_size_memory(false),
        animation_enable(false),
        animation_blend_unit_num(0),
        animation_num_in_unit(0)
    {}
  };

  // Description�̏������֐�
  // �f�t�H���g�l��ݒ肷��
  static void InitializeDescriptionForDefault(
      Description*  description
  );

  // Description�̏������֐�
  // AIM     PERSPECTIVE
  static void InitializeDescriptionForAimPerspective(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const f32                         twist,  // [radian]
    const f32                         w_scale,
    const f32                         fovy,  // [radian]
    const f32                         aspect,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // AIM     FRUSTUM
  static void InitializeDescriptionForAimFrustum(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const f32                         twist,  // [radian]
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // AIM     ORTHO
  static void InitializeDescriptionForAimOrtho(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const f32                         twist,  // [radian]
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // LOOK_AT PERSPECTIVE
  static void InitializeDescriptionForLookAtPerspective(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const gfl::math::VEC3&            up_vector,
    const f32                         w_scale,
    const f32                         fovy,  // [radian]
    const f32                         aspect,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // LOOK_AT FRUSTUM
  static void InitializeDescriptionForLookAtFrustum(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const gfl::math::VEC3&            up_vector,
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // LOOK_AT ORTHO
  static void InitializeDescriptionForLookAtOrtho(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const gfl::math::VEC3&            up_vector,
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // ROTATE  PERSPECTIVE
  static void InitializeDescriptionForRotatePerspective(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            rotate_xyz,  // [radian]
    const f32                         w_scale,
    const f32                         fovy,  // [radian]
    const f32                         aspect,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // ROTATE  FRUSTUM
  static void InitializeDescriptionForRotateFrustum(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            rotate_xyz,  // [radian]
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // ROTATE  ORTHO
  static void InitializeDescriptionForRotateOrtho(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            rotate_xyz,  // [radian]
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );


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
  Camera(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~Camera();




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
   *  @param[in]     description    
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
  // �g�����X�t�H�[��
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̈ʒu�𓾂�
   *
   *  @param[out]   camera_position    �J�����̈ʒu��Ԃ�
   */
  //-----------------------------------------------------------------------------
  void GetCameraPosition( gfl::math::VEC3& camera_position ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̒����_�̈ʒu�𓾂�
   *
   *  @param[out]   target_position    �J�����̒����_�̈ʒu��Ԃ�
   */
  //-----------------------------------------------------------------------------
  void GetTargetPosition( gfl::math::VEC3& target_position ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu�𓾂�
   *
   *  @param[out]   camera_position    �J�����̈ʒu��Ԃ�
   *  @param[out]   target_position    �J�����̒����_�̈ʒu��Ԃ�
   */
  //-----------------------------------------------------------------------------
  void GetCameraAndTargetPosition( gfl::math::VEC3& camera_position, gfl::math::VEC3& target_position );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̈ʒu��ݒ肷��
   *
   *  @param[in]    camera_position    �J�����̈ʒu
   */
  //-----------------------------------------------------------------------------
  virtual void SetCameraPosition( const gfl::math::VEC3& camera_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̒����_�̈ʒu��ݒ肷��
   *
   *  @param[in]    target_position    �J�����̒����_�̈ʒu
   */
  //-----------------------------------------------------------------------------
  virtual void SetTargetPosition( const gfl::math::VEC3& target_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu��ݒ肷��
   *
   *  @param[in]    camera_position    �J�����̈ʒu
   *  @param[in]    target_position    �J�����̒����_�̈ʒu
   */
  //-----------------------------------------------------------------------------
  virtual void SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ��]�J�����̊p�x
   *                �P�ʂ̓��W�A���B
   */
  //-----------------------------------------------------------------------------
  void GetRotateXYZ(gfl::math::VEC3* r) const;
  virtual void SetRotateXYZ(const gfl::math::VEC3& r);
  virtual void SetRotateXYZ(f32 x, f32 y, f32 z);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X���擾����
   *
   *  @param[out]    matrix          �擾�����}�g���N�X���i�[���ĕԂ�
   *
   *  @retval        bool            ����Ɏ擾�ł�����true
   */
  //-----------------------------------------------------------------------------
  bool GetWorldMatrix(gfl::math::MTX34* matrix) const;
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
  bool SetWorldMatrix(const gfl::math::MTX34& matrix);
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �}�g���b�N�X�̃j�A�A�t�@�[�l��ύX
   *	@note   �g�p��A SetMatrixNearFarDefault �ŕK�����ɖ߂����ƁB
   *
   *	@param	f32 near �j�A�l
   *	@param	f32 far  �t�@�[�l
   */
  //-----------------------------------------------------------------------------
  void SetMatrixNearFar( const f32 near, const f32 far );
    
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �}�g���b�N�X�̃j�A�A�t�@�[������ŕێ����Ă���l�ɖ߂�
   */
  //-----------------------------------------------------------------------------
  void SetMatrixNearFarDefault( void );




  // 
  // �ϊ�
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         grp�J�����̓��e��g3d�J�����ɔ��f����
   *
   *  @param[in]     grp_camera    grp�J����
   */
  //-----------------------------------------------------------------------------
  void ConvertGrpCameraToG3dCamera( const gfl::grp::Camera& grp_camera );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         g3d�J�����̓��e��grp�J�����ɔ��f����
   *
   *  @param[out]    grp_camera    grp�J����
   */
  //-----------------------------------------------------------------------------
  void ConvertG3dCameraToGrpCamera( gfl::grp::Camera& grp_camera );




  //
  // �J�����}�g���N�X
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����
   */
  //-----------------------------------------------------------------------------
  virtual void UpdateCameraMatrix(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �r���[�}�g���N�X�𓾂�
   *
   *  @param[out]    view_matrix    �r���[�}�g���N�X
   */
  //-----------------------------------------------------------------------------
  void GetViewMatrix( gfl::math::MTX34* view_matrix ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �r���[�}�g���N�X��ݒ肷��
   *                �y���Ӂz���̌��UpdateCameraMatrix���s���ƁAUpdateCameraMatrix�Ōv�Z�����l�ŏ㏑������Ă��܂��܂��B
   *
   *  @param[in]    view_matrix     �r���[�}�g���N�X
   *  @param[in]    update_matrix   �r���[�}�g���N�X�̋t�s����v�Z�������Ƃ�true
   */
  //-----------------------------------------------------------------------------
  void SetViewMatrix( const gfl::math::MTX34& view_matrix, const b32 update_matrix = true );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v���W�F�N�����}�g���N�X�𓾂�
   *
   *  @param[out]    projection_matrix    �v���W�F�N�V�����}�g���N�X
   */
  //-----------------------------------------------------------------------------
  void GetProjectionMatrix( gfl::math::MTX44* projection_matrix ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v���W�F�N�����}�g���N�X��ݒ肷��
   *                �y���Ӂz���̌��UpdateCameraMatrix���s���ƁAUpdateCameraMatrix�Ōv�Z�����l�ŏ㏑������Ă��܂��܂��B
   *
   *  @param[in]    projection_matrix   �v���W�F�N�V�����}�g���N�X
   *  @param[in]    update_matrix       �v���W�F�N�V�����}�g���N�X�̋t�s����v�Z�������Ƃ�true
   */
  //-----------------------------------------------------------------------------
  void SetProjectionMatrix( const gfl::math::MTX44& projection_matrix, const b32 update_matrix = true );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �r���[�}�g���N�X�ƃv���W�F�N�����}�g���N�X�𓾂�
   *
   *  @param[out]    view_matrix          �r���[�}�g���N�X
   *  @param[out]    projection_matrix    �v���W�F�N�V�����}�g���N�X
   */
  //-----------------------------------------------------------------------------
  void GetViewAndProjectionMatrix( gfl::math::MTX34* view_matrix, gfl::math::MTX44* projection_matrix ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �r���[�}�g���N�X�ƃv���W�F�N�����}�g���N�X��ݒ肷��
   *                �y���Ӂz���̌��UpdateCameraMatrix���s���ƁAUpdateCameraMatrix�Ōv�Z�����l�ŏ㏑������Ă��܂��܂��B
   *
   *  @param[in]    view_matrix         �r���[�}�g���N�X
   *  @param[in]    projection_matrix   �v���W�F�N�V�����}�g���N�X
   *  @param[in]    update_matrix       �r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X�̋t�s����v�Z�������Ƃ�true
   */
  //-----------------------------------------------------------------------------
  void SetViewAndProjectionMatrix( const gfl::math::MTX34& view_matrix, const gfl::math::MTX44& projection_matrix, const b32 update_matrix = true );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �X�N���[�����W�n�̓_�����[���h���W�n�̔�����(�J�����̈ʒu����X�N���[���̓_�ɔ�΂������C)�ɕϊ�����
   *
   *  @param[out]   world_ray_direction   ���[���h���W�n�̔������̕����x�N�g��(�傫��1)
   *  @param[out]   world_ray_origin      ���[���h���W�n�̔������̎n�_���W(�J�����̈ʒuGetCameraPosition�Ɠ����l�������܂�)
   *  @param[in]    screen_point_x        �X�N���[�����W�n�̓_�̈ʒu���W(���オ(0.0f,0.0f)�A�E����(1.0f,1.0f))
   *  @param[in]    screen_point_y
   *  @param[in]    render_target         �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void ConvertScreenPointToWorldRay(
      gfl::math::VEC3*              world_ray_direction,
      gfl::math::VEC3*              world_ray_origin,
      const f32                     screen_point_x,
      const f32                     screen_point_y,
      const gfl::grp::RenderTarget& render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���ʃX�N���[�����W�n�̓_�����[���h���W�n�̔�����(�J�����̈ʒu����X�N���[���̓_�ɔ�΂������C)�ɕϊ�����
   *
   *  @param[out]   world_ray_direction   ���[���h���W�n�̔������̕����x�N�g��(�傫��1)
   *  @param[out]   world_ray_origin      ���[���h���W�n�̔������̎n�_���W(�J�����̈ʒuGetCameraPosition�Ɠ����l�������܂�)
   *  @param[in]    upper_screen_point_x  ���ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(400-1,240-1))
   *  @param[in]    upper_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B 
   *  @param[in]    upper_render_target   �����_�[�^�[�Q�b�g(���ꂪ�x�[�X�J�����̂Ƃ��͍��ł��E�ł��ǂ���ł�����)
   */
  //-----------------------------------------------------------------------------
  void ConvertUpperScreenPointToWorldRay(
      gfl::math::VEC3*              world_ray_direction,
      gfl::math::VEC3*              world_ray_origin,
      const s32                     upper_screen_point_x,
      const s32                     upper_screen_point_y,
      const gfl::grp::RenderTarget& upper_render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ����ʃX�N���[�����W�n�̓_�����[���h���W�n�̔�����(�J�����̈ʒu����X�N���[���̓_�ɔ�΂������C)�ɕϊ�����
   *
   *  @param[out]   world_ray_direction   ���[���h���W�n�̔������̕����x�N�g��(�傫��1)
   *  @param[out]   world_ray_origin      ���[���h���W�n�̔������̎n�_���W(�J�����̈ʒuGetCameraPosition�Ɠ����l�������܂�)
   *  @param[in]    lower_screen_point_x  ����ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(320-1,240-1))
   *  @param[in]    lower_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B 
   *  @param[in]    lower_render_target   �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void ConvertLowerScreenPointToWorldRay(
      gfl::math::VEC3*              world_ray_direction,
      gfl::math::VEC3*              world_ray_origin,
      const s32                     lower_screen_point_x,
      const s32                     lower_screen_point_y,
      const gfl::grp::RenderTarget& lower_render_target
  ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �X�N���[�����W�n�̓_�����[���h���W�n�̓_(�j�A�ʏ�or�t�@�[�ʏ�)�ɕϊ�����
   *
   *  @param[out]   world_point           ���[���h���W�n�̓_�̈ʒu���W
   *  @param[in]    is_near               true�̂Ƃ��j�A�ʏ�Afalse�̂Ƃ��t�@�[�ʏ�
   *  @param[in]    screen_point_x        �X�N���[�����W�n�̓_�̈ʒu���W(���オ(0.0f,0.0f)�A�E����(1.0f,1.0f))
   *  @param[in]    screen_point_y
   *  @param[in]    render_target         �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void ConvertScreenPointToWorldPoint(
      gfl::math::VEC3*              world_point,
      const b32                     is_near,
      const f32                     screen_point_x,
      const f32                     screen_point_y,
      const gfl::grp::RenderTarget& render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���ʃX�N���[�����W�n�̓_�����[���h���W�n�̓_(�j�A�ʏ�or�t�@�[�ʏ�)�ɕϊ�����
   *
   *  @param[out]   world_point           ���[���h���W�n�̓_�̈ʒu���W
   *  @param[in]    is_near               true�̂Ƃ��j�A�ʏ�Afalse�̂Ƃ��t�@�[�ʏ�
   *  @param[in]    upper_screen_point_x  ���ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(400-1,240-1))
   *  @param[in]    upper_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B 
   *  @param[in]    render_target         �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void ConvertUpperScreenPointToWorldPoint(
      gfl::math::VEC3*              world_ray_direction,
      const b32                     is_near,
      const s32                     upper_screen_point_x,
      const s32                     upper_screen_point_y,
      const gfl::grp::RenderTarget& upper_render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ����ʃX�N���[�����W�n�̓_�����[���h���W�n�̓_(�j�A�ʏ�or�t�@�[�ʏ�)�ɕϊ�����
   *
   *  @param[out]   world_point           ���[���h���W�n�̓_�̈ʒu���W
   *  @param[in]    is_near               true�̂Ƃ��j�A�ʏ�Afalse�̂Ƃ��t�@�[�ʏ�
   *  @param[in]    lower_screen_point_x  ����ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(320-1,240-1))
   *  @param[in]    lower_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B 
   *  @param[in]    render_target         �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void ConvertLowerScreenPointToWorldPoint(
      gfl::math::VEC3*              world_point,
      const b32                     is_near,
      const s32                     lower_screen_point_x,
      const s32                     lower_screen_point_y,
      const gfl::grp::RenderTarget& lower_render_target
  ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���[���h���W�n�̓_���X�N���[�����W�n�̓_�ɕϊ�����
   *
   *  @param[out]    screen_point_x        �X�N���[�����W�n�̓_�̈ʒu���W(���オ(0.0f,0.0f)�A�E����(1.0f,1.0f))
   *  @param[out]    screen_point_y        
   *  @param[in]     world_point           ���[���h���W�n�̓_�̈ʒu���W
   *  @param[in]     render_target         �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void ConvertWorldPointToScreenPoint(
      f32*                          screen_point_x,
      f32*                          screen_point_y,
      const gfl::math::VEC3&        world_point,
      const gfl::grp::RenderTarget& render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���[���h���W�n�̓_�����ʃX�N���[�����W�n�̓_�ɕϊ�����
   *
   *  @param[out]    upper_screen_point_x  ���ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(400-1,240-1))
   *  @param[out]    upper_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B     
   *  @param[in]     world_point           ���[���h���W�n�̓_�̈ʒu���W
   *  @param[in]     upper_render_target   �����_�[�^�[�Q�b�g(���ꂪ�x�[�X�J�����̂Ƃ��͍��ł��E�ł��ǂ���ł�����)
   */
  //-----------------------------------------------------------------------------
  void ConvertWorldPointToUpperScreenPoint(
      s32*                          upper_screen_point_x,
      s32*                          upper_screen_point_y,
      const gfl::math::VEC3&        world_point,
      const gfl::grp::RenderTarget& upper_render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���[���h���W�n�̓_������ʃX�N���[�����W�n�̓_�ɕϊ�����
   *
   *  @param[out]    lower_screen_point_x  ����ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(320-1,240-1))
   *  @param[out]    lower_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B
   *  @param[in]     world_point           ���[���h���W�n�̓_�̈ʒu���W
   *  @param[in]     lower_render_target   �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void ConvertWorldPointToLowerScreenPoint(
      s32*                          lower_screen_point_x,
      s32*                          lower_screen_point_y,
      const gfl::math::VEC3&        world_point,
      const gfl::grp::RenderTarget& lower_render_target
  ) const;




  //
  // �A�j���[�V����
  //
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V������ύX����
   *                 ���߂Ă��̊֐����Ă񂾂Ƃ��́AAnimation�𐶐�����B
   *
   *  @param[in]     allocator               �A���P�[�^
   *  @param[in]     res_anim                �A�j���[�V�������\�[�X
   *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B
   *                                         ���߂Ă��̊֐����Ă񂾂Ƃ��́A�g�p�����B
   *                                         Resource::GetCameraMemberAnimSetCount�œ����l��n���Ƃ悢�B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
   *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
   */
  //-----------------------------------------------------------------------------
  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V������ύX����
   *                 ���߂Ă��̊֐����Ă񂾂Ƃ��́AAnimation�𐶐�����B
   *
   *  @param[in]     allocator               �A���P�[�^
   *  @param[in]     resource                ���\�[�X
   *  @param[in]     index_in_resource       ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B
   *                                         ���߂Ă��̊֐����Ă񂾂Ƃ��́A�g�p�����B
   *                                         Resource::GetCameraMemberAnimSetCount�œ����l��n���Ƃ悢�B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
   *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
   */
  //-----------------------------------------------------------------------------
  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    Resource*               resource,
    s32                     index_in_resource,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );


  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    Resource*               resource,
    s32                     index_in_resource,
    u32                     change_frame,
    s32                     max_anim_members
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index��unit_index�Ɋ��蓖�Ă�A�j���[�V������ύX����
   *
   *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
   *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
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
   */
  //-----------------------------------------------------------------------------
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    u32                     unit_index,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator*          heap_allocator,
    gfl::heap::NwAllocator*          device_allocator,
    u32                              unit_index,
    Resource*                        resource,
    s32                              index_in_resource,
    u32                              change_frame,
    s32                              max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  void SetAnimationFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimationFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
   *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
   *
   *  @param[in]     step_frame      �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  void SetAnimationStepFrame(f32 step_frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
   *
   *  @retval        �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimationStepFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  void SetAnimationStartFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimationStartFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  void SetAnimationEndFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimationEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
   *
   *  @param[in]     start_frame     �J�n�t���[��
   *  @param[in]     end_frame       �I���t���[��
   */
  //-----------------------------------------------------------------------------
  void SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
   *
   *  @retval        bool    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  bool IsAnimationFrameEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
   *
   *  @retval        bool            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
   */
  //-----------------------------------------------------------------------------
  bool IsAnimationLoop(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
   *
   *  @param[in]     loop            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
   */
  //-----------------------------------------------------------------------------
  void SetAnimationLoop(bool loop);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t���[������
   */
  //-----------------------------------------------------------------------------
  void SetUnitFrame(u32 unit_index, f32 frame);
  f32  GetUnitFrame(u32 unit_index) const;
  void SetUnitStepFrame(u32 unit_index, f32 step_frame);
  f32  GetUnitStepFrame(u32 unit_index) const;
  void SetUnitStartFrame(u32 unit_index, f32 frame);
  f32  GetUnitStartFrame(u32 unit_index) const;
  void SetUnitEndFrame(u32 unit_index, f32 frame);
  f32  GetUnitEndFrame(u32 unit_index) const;
  void SetUnitStartAndEndFrame(u32 unit_index, f32 start_frame, f32 end_frame);
  b32  IsUnitFrameEndFrame(u32 unit_index) const;
  b32  IsUnitLoop(u32 unit_index) const;
  void SetUnitLoop(u32 unit_index, b32 loop);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index���ɂ��������j�b�g�̃A�j���[�V�������u�����h����
   *
   *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
   *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
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
      AnimationSlot::Mode        mode,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );
  void BlendUnit2Animation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
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
  void SetUnitBlendWeight(u32 unit_index, f32 weight);
  f32  GetUnitBlendWeight(u32 unit_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnim::Type�̃A�j���[�V�����ŕύX�����|�[�Y�����Z�b�g����
   *
   *  �A�j���[�V���������蓖�Ă��Ă���Ƃ��́A���̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ��B
   *  �A�j���[�V���������蓖�Ă��Ă��Ȃ��Ƃ��́A�S�Ẵ����o�����Z�b�g�����B
   */
  //-----------------------------------------------------------------------------
  void ResetAnimationPose(void);




  //
  // ���[�e�B���e�B�֐�
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �r���[�A�b�v�f�[�^ ���[�e�B���e�B�֐�
   */
  //-----------------------------------------------------------------------------
  void GetUpVector(gfl::math::VEC3* up_vector) const;
  void SetUpVector(const gfl::math::VEC3& up_vector);
  f32  GetTwist(void) const;  // [radian]
  void SetTwist(f32 twist);   // [radian]

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �v���W�F�N�V�����A�b�v�f�[�^ ���[�e�B���e�B�֐�
   */
  //-----------------------------------------------------------------------------
  f32  GetFovy(void) const;  // [radian]
  void SetFovy(f32 fovy);    // [radian]
  f32  GetAspect(void) const;
  void SetAspect(f32 aspect);
  f32  GetHeight(void) const;
  void SetHeight(f32 height);  // aspect��ێ����邽�߁Aheight��ύX����Ɖ������ύX����܂��B
  void GetCenter(gfl::math::VEC2* center) const;
  void SetCenter(const gfl::math::VEC2& center);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ��ʂ̏������ݒ肷��
   *
   *  @param[in]    pivot_direction    ��ʂ̏����
   */
  //-----------------------------------------------------------------------------
  void SetPivotDirection( gfl::math::PivotDirection pivot_direction );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ��ʂ̏�������擾����
   *
   *  @retval    gfl::math::PivotDirection    ��ʂ̏����
   */
  //-----------------------------------------------------------------------------
  gfl::math::PivotDirection  GetPivotDirection(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �j�A�N���b�v / �t�@�[�N���b�v
   */
  //-----------------------------------------------------------------------------
  void SetNearClip( f32 near_clip );
  f32  GetNearClip( void ) const;
  void SetFarClip( f32 far_clip );
  f32  GetFarClip( void ) const;




  //
  // WScale
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        WScale
   */
  //-----------------------------------------------------------------------------
  f32  GetWScale(void) const;
  void SetWScale(f32 w_scale);




  //
  // �擾 / �ݒ�
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �r���[���[�h���擾����
   */
  //-----------------------------------------------------------------------------
  ViewMode GetViewMode(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �ˉe���[�h���擾����
   */
  //-----------------------------------------------------------------------------
  ProjectionMode GetProjectionMode(void) const;

  ///< �r���[�N���b�v����N���X���擾����(���͔̂h���悪����)
  virtual const CameraViewClip* GetViewClip() const { return NULL; }

public:
  //---------------------------------------------------------------------------
  /**
   *           �萔�錾
   */
  //---------------------------------------------------------------------------
  static const gfl::math::VEC3  CAMERA_POSITION_DEFAULT;    // �J�����ʒu�̃f�t�H���g�l
  static const gfl::math::VEC3  TARGET_POSITION_DEFAULT;    // �J�����^�[�Q�b�g�̃f�t�H���g�l
  static const f32              NEAR_CLIP_DEFAULT;          // �j�A�N���b�v�̃f�t�H���g�l
  static const f32              FAR_CLIP_DEFAULT;           // �t�@�[�N���b�v�̃f�t�H���g�l
  static const f32              FOVY_RADIAN_DEFAULT;        // FOV��Y�����̃f�t�H���g�l
  static const gfl::math::VEC2  PROJECTION_CENTER_DEFAULT;  // �J�����̓��e�ʂ̒��S�ʒu�̃f�t�H���g�l
  static const f32              PROJECTION_HEIGHT_DEFAULT;  // �J�����̓��e�ʂ̍����̃f�t�H���g�l
    
public:
  //
  // NW4C�֘A static ���� / �j��
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����𐶐�����
   * 
   *  @param[in]    allocator          �����Ɏg�p����A���P�[�^�[
   *  @param[in]    camera_position    �J�����̈ʒu���W
   *  @param[in]    target_position    �J�����^�[�Q�b�g�̍��W
   *  @param[in]    near_clip          �j�A�N���b�v�̒l
   *  @param[in]    far_clip           �t�@�[�N���b�v�̒l
   *  @param[in]    fovy_radian        FOV��Y�����̒l
   *  @param[in]    pivot_direction    ��ʂ̏����
   *  @param[in]    w_scale            WScale�̒l
   *
   *  @retval       nw::gfx::Camera*  ���������J������Ԃ�
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  static nw::gfx::Camera* CreateCamera(
      gfl::heap::NwAllocator*          allocator,
      const gfl::math::VEC3&           camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&           target_position   = TARGET_POSITION_DEFAULT,
      f32                              near_clip         = NEAR_CLIP_DEFAULT,
      f32                              far_clip          = FAR_CLIP_DEFAULT,
      f32                              fovy_radian       = FOVY_RADIAN_DEFAULT,
      gfl::math::PivotDirection        pivot_direction   = gfl::math::PIVOT_NONE,//gfl::math::PIVOT_UPSIDE_TO_TOP,
      f32                              w_scale           = 0.0f
  );

  static nw::gfx::Camera*
  CreateFrustumCamera(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3& cameraPosition,
    const gfl::math::VEC3& targetPosition,
    f32 nearClip,
    f32 farClip,
    const gfl::math::VEC2& projectionCenter,
    f32 projectionHeight,
    gfl::math::PivotDirection pivotDirection,
    f32 wScale
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ���̎��Ɏg�p����x�[�X�J�����ƍ��E�J�����𐶐�����
   *
   *                ���ۂ͉E�ڗp�J�����ƍ��ڗp�J�����̋�ʂ͂���܂���B
   *
   *  @param[out]   pp_base_camera     ���������x�[�X�J������Ԃ����߂̃|�C���^
   *  @param[out]   pp_left_camera     ���������E�ڗp�J������Ԃ����߂̃|�C���^
   *  @param[out]   pp_right_camera    �����������ڗp�J������Ԃ����߂̃|�C���^
   *  @param[in]    allocator          �����Ɏg�p����A���P�[�^�[
   *  @param[in]    camera_position    �J�����̈ʒu���W
   *  @param[in]    target_position    �J�����^�[�Q�b�g�̍��W
   *  @param[in]    near_clip          �j�A�N���b�v�̒l
   *  @param[in]    far_clip           �t�@�[�N���b�v�̒l
   *  @param[in]    fovy_radian        FOV��Y�����̒l
   *  @param[in]    w_scale            WScale�̒l
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  static void CreateStereoCameras(
      nw::gfx::Camera**           pp_base_camera,
      nw::gfx::Camera**           pp_left_camera,
      nw::gfx::Camera**           pp_right_camera,
      gfl::heap::NwAllocator*     allocator,
      const gfl::math::VEC3&      camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&      target_position   = TARGET_POSITION_DEFAULT,
      f32                         near_clip         = NEAR_CLIP_DEFAULT,
      f32                         far_clip          = FAR_CLIP_DEFAULT,
      f32                         fovy_radian       = FOVY_RADIAN_DEFAULT,
      f32                         w_scale           = 0.0f
  );

  static void
  CreateFrustumStereoCameras(
    nw::gfx::Camera**           pp_base_camera,
    nw::gfx::Camera**           pp_left_camera,
    nw::gfx::Camera**           pp_right_camera,
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3& cameraPosition,
    const gfl::math::VEC3& targetPosition,
    f32 nearClip,
    f32 farClip,
    const gfl::math::VEC2& projectionCenter,
    f32 projectionHeight,
    gfl::math::PivotDirection pivotDirection,
    f32 w_scale
  );

  //
  // NW4C�֘A static �ݒ� / �擾
  //
  static void SetFrustum(
     nw::gfx::Camera* camera,
     f32 left,
     f32 right,
     f32 bottom,
     f32 top,
     f32 nearClip,
     f32 farClip
  );
  static void SetFrustum(
     nw::gfx::Camera* camera,
     const nw::gfx::Viewport & viewport
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̃A�X�y�N�g���ݒ肷��
   * 
   *                �w�肵�������_�[�^�[�Q�b�g�ɍ����悤�ɐݒ肳��܂��B
   *                �J�������A�^�b�`����Ă���V�[�����X�V���Ă�����s����K�v������܂��B
   * 
   *  @param[in]    camera          �ݒ肷��J����
   *  @param[in]    render_target   �A�X�y�N�g������킹�郌���_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  static void SetCameraAspectRatio(
      nw::gfx::Camera*                camera,
      const nw::gfx::IRenderTarget*   render_target
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̃A�X�y�N�g���ݒ肷��
   * 
   *                �J�������A�^�b�`����Ă���V�[�����X�V���Ă�����s����K�v������܂��B
   * 
   *  @param[in]    camera        �ݒ肷��J����
   *  @param[in]    aspect_ratio   �A�X�y�N�g��
   */
  //-----------------------------------------------------------------------------
  static void SetCameraAspectRatio(
      nw::gfx::Camera*  camera,
      f32               aspect_ratio
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̈ʒu�𓾂�
   *
   *  @param[in]    nw_camera          �J����
   *  @param[out]   camera_position    �J�����̈ʒu��Ԃ�
   */
  //-----------------------------------------------------------------------------
  static void GetCameraPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& camera_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̒����_�̈ʒu�𓾂�
   *
   *  @param[in]    nw_camera          �J����
   *  @param[out]   target_position    �J�����̒����_�̈ʒu��Ԃ�
   */
  //-----------------------------------------------------------------------------
  static void GetTargetPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& target_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu�𓾂�
   *
   *  @param[in]    nw_camera          �J����
   *  @param[out]   camera_position    �J�����̈ʒu��Ԃ�
   *  @param[out]   target_position    �J�����̒����_�̈ʒu��Ԃ�
   */
  //-----------------------------------------------------------------------------
  static void GetCameraAndTargetPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& camera_position, gfl::math::VEC3& target_position );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̈ʒu��ݒ肷��
   *
   *  @param[in]    nw_camera          �J����
   *  @param[in]    camera_position    �J�����̈ʒu
   */
  //-----------------------------------------------------------------------------
  static void SetCameraPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& camera_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̒����_�̈ʒu��ݒ肷��
   *
   *  @param[in]    nw_camera          �J����
   *  @param[in]    target_position    �J�����̒����_�̈ʒu
   */
  //-----------------------------------------------------------------------------
  static void SetTargetPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& target_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu��ݒ肷��
   *
   *  @param[in]    nw_camera          �J����
   *  @param[in]    camera_position    �J�����̈ʒu
   *  @param[in]    target_position    �J�����̒����_�̈ʒu
   */
  //-----------------------------------------------------------------------------
  static void SetCameraAndTargetPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position );




public:
  //
  // static ���� / �j��
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
   *
   *  @retval       Camera*  ���������J�����N���X
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  static Camera* Create(
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
  /**
   *  @brief        ���̎��Ɏg�p����x�[�X�J�����ƍ��E�J�����𐶐�����
   *
   *                ���ۂ͉E�ڗp�J�����ƍ��ڗp�J�����̋�ʂ͂���܂���B
   *
   *  @param[out]   pp_base_camera     ���������x�[�X�J������Ԃ����߂̃|�C���^
   *  @param[out]   pp_left_camera     ���������E�ڗp�J������Ԃ����߂̃|�C���^
   *  @param[out]   pp_right_camera    �����������ڗp�J������Ԃ����߂̃|�C���^
   *  @param[in]    allocator          �����Ɏg�p����A���P�[�^�[
   *  @param[in]    camera_position    �J�����̈ʒu���W
   *  @param[in]    target_position    �J�����^�[�Q�b�g�̍��W
   *  @param[in]    near_clip          �j�A�N���b�v�̒l
   *  @param[in]    far_clip           �t�@�[�N���b�v�̒l
   *  @param[in]    fovy_radian        FOV��Y�����̒l
   *  @param[in]    w_scale            WScale�̒l
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  static void CreateStereoCameras(
      Camera**                    pp_base_camera,
      Camera**                    pp_left_camera,
      Camera**                    pp_right_camera,
      gfl::heap::NwAllocator*     allocator,
      const gfl::math::VEC3&      camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&      target_position   = TARGET_POSITION_DEFAULT,
      f32                         near_clip         = NEAR_CLIP_DEFAULT,
      f32                         far_clip          = FAR_CLIP_DEFAULT,
      f32                         fovy_radian       = FOVY_RADIAN_DEFAULT,
      f32                         w_scale           = 0.0f
  );




public:
  //
  // NW4C�֘A ���� / �j��
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     nw_camera      �J����nw::gfx::Camera
   *                                �����œn��������nw_camera�́A���ケ�̃N���X�ŊǗ������̂ŉ�����Ȃ��悤�ɁB
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      nw::gfx::Camera*      nw_camera
  )
  {
    SetNwSceneNode(nw_camera);
  }


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
  virtual nw::gfx::Camera*  GetNwCamera(void) const  {  return  nw::ut::DynamicCast<nw::gfx::Camera*>( GetNwSceneNode() );  } 


protected:
  virtual nw::gfx::SceneNode* GetNwSceneNode(void) const
  {
    return SceneNode::GetNwSceneNode();
  }


private:
  //
  // private�֐�
  //
  
  nw::gfx::CameraViewUpdater* CreateAimViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     target_position,
    f32                        twist  // [radian]
  );
  nw::gfx::CameraViewUpdater* CreateLookAtViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     target_position,
    const gfl::math::VEC3&     up_vector
  );
  nw::gfx::CameraViewUpdater* CreateRotateViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     rotate_xyz  // [radian]
  );
  nw::gfx::CameraProjectionUpdater* CreatePerspectiveProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        fovy,  // [radian]
    f32                        aspect
  );
  nw::gfx::CameraProjectionUpdater* CreateFrustumProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        aspect,
    f32                        height,
    const gfl::math::VEC2&     center
  );
  nw::gfx::CameraProjectionUpdater* CreateOrthoProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        aspect,
    f32                        height,
    const gfl::math::VEC2&     center
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     nw_scene_node      �V�[���m�[�h
   *
   *  @retval        Camera*  ���������J�����N���X
   *
   * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  static Camera* Create(
      gfl::heap::NwAllocator*           allocator,
      nw::gfx::SceneNode*               nw_scene_node
  );


  
  //----------------------------------------------------------------------------
  /**
   *	@brief  LookAt��Aim���[�h���̃A���O���擾�֐�
   *
   *	@param	r   �A���O���i�[��
   *	@param  view_mode �r���[���[�h
   */
  //-----------------------------------------------------------------------------
  void GetAimAndTwistRotateXYZ( gfl::math::VEC3* r, ViewMode view_mode ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  LookAt��Aim���[�h���̃A���O���ݒ�֐�
   *
   *	@param	x   X����]�p�x
   *	@param	y   Y����]�p�x
   *	@param	z   Z����]�p�x
   *	@param  view_mode �r���[���[�h
   */
  //-----------------------------------------------------------------------------
  void SetAimAndTwistRotateXYZ( f32 x, f32 y, f32 z, ViewMode view_mode );
  


protected:
  //Animation*  m_animation;
  u32                m_animation_slot_num;  // 0��1  // 0�̂Ƃ��A�j���[�V������t�����Ȃ��A1�̂Ƃ��A�j���[�V������t������(���ۂɕt���Ă��邩�ǂ�����m_animation_manager�Ŋm�F���ĉ�����)�B
  u32                m_animation_blend_unit_num;
  u32                m_animation_num_in_unit;
  AnimationManager*  m_animation_manager;

};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DCAMERA_H__
