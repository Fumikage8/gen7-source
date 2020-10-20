//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dStereoCamera.cpp
 *	@brief  3D�X�e���I�J����
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK
#include <nn/ulcd.h>
#include <nn/math.h>

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Math.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dCamera.h>

#include <grp/g3d/gfl_G3dStereoCamera.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�X�e���I�J�����N���X
//=====================================

//
// �萔
//

//-----------------------------------------------------------------------------
/*
 *    �f�t�H���g�l
 */
//-----------------------------------------------------------------------------
const f32 StereoCamera::DEPTH_LEVEL_DEFAULT = Camera::NEAR_CLIP_DEFAULT + 5.0f;
const f32 StereoCamera::DEPTH_RANGE_DEFAULT = 0.0f;
const b32 StereoCamera::REAL_SWITCH_DEFAULT = false;




//
// �R���X�g���N�^ / �f�X�g���N�^
//

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
StereoCamera::StereoCamera(void)
  : Camera(),
    m_base_camera_owner_this(false),
    m_base_camera(NULL),
    m_left_camera(NULL),
    m_right_camera(NULL),
    m_nn_stereo_camera(NULL),
    m_depth_level(DEPTH_LEVEL_DEFAULT),
    m_depth_range(DEPTH_RANGE_DEFAULT),
    m_real_switch(REAL_SWITCH_DEFAULT)
{
  // ���N���X�ł���Camera�̃R���X�g���N�^�œo�^�������̂��폜����
  GFL_SINGLETON_INSTANCE(System)->GetCameraManager()->RemoveObject(this);
  // ���̃R���X�g���N�^�œo�^������
  GFL_SINGLETON_INSTANCE(System)->GetStereoCameraManager()->AddObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
StereoCamera::~StereoCamera()
{
  Destroy();

  // ���̃R���X�g���N�^�œo�^�������̂��폜����
  GFL_SINGLETON_INSTANCE(System)->GetStereoCameraManager()->RemoveObject(this);
  // ���N���X�ł���Camera�̃}�l�[�W���ɓo�^������
  GFL_SINGLETON_INSTANCE(System)->GetCameraManager()->AddObject(this);
}




//
// ���� / �j��
//

//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource,
    ResourceDescription*              description
)
{
  this->Create(
      allocator,
      device_allocator,
      resource,
      index_in_resource,
      DEPTH_LEVEL_DEFAULT,
      DEPTH_RANGE_DEFAULT,
      REAL_SWITCH_DEFAULT,
      description
  );
}

//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*          allocator,
    gfl::heap::NwAllocator*          device_allocator,
    Resource*                        resource,
    s32                              index_in_resource,
    f32                              depth_level,
    f32                              depth_range,
    b32                              real_switch,
    ResourceDescription*             description
)
{
  ResourceDescription l_description;
  if( description )
  {
    l_description = *description;
  }

  // �x�[�X�J����
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }

  m_animation_slot_num = (l_description.animation_enable)?(1):(0);
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResCamera res = nw_res_graphics_file->GetCameras(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      (m_animation_slot_num>0)?(true):(false),  // ������false����nw::gfx::AnimGroup���擾�ł��Ȃ�  // ������O������ݒ�ł���悤�ɂ��邩�A�A�j���[�V�����Ȃ��J�����N���X�����邩
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      m_animation_slot_num
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  //m_animation_manager = GFL_NEW(allocator->GetHeapBase()) AnimationManager;
  //m_animation_manager->NoSlot_Create(
  //    allocator,
  //    allocator,
  //    this
  //);

  // ���ڗp�J����
  nw::gfx::Camera* nw_left_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
  GFL_ASSERT_STOP(nw_left_camera);
  // SetWScale����Ȃ炱��
  m_left_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_left_camera->Create(nw_left_camera);
    
  // �E�ڗp�J����
  nw::gfx::Camera* nw_right_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
  GFL_ASSERT_STOP(nw_right_camera);
  // SetWScale����Ȃ炱��
  m_right_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_right_camera->Create(nw_right_camera);

  m_depth_level = depth_level;
  m_depth_range = depth_range;
  m_real_switch = real_switch;

  // �X�e���I�J�����𐶐�����
  m_nn_stereo_camera = GFL_NEW(allocator->GetHeapBase()) nn::ulcd::CTR::StereoCamera;
      // ���������s��Ȃ����߁A�ʓr�AInitialize ���ĂԕK�v������܂��B
  m_nn_stereo_camera->Initialize();
}

//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*    allocator,
    Description*               description
)
{
  this->Create(
      allocator,
      description,
      DEPTH_LEVEL_DEFAULT,
      DEPTH_RANGE_DEFAULT,
      REAL_SWITCH_DEFAULT
  );
}
//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*    allocator,
    Description*               description,
    f32                        depth_level,
    f32                        depth_range,
    b32                        real_switch
)
{
  Camera* base_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  base_camera->Create(
      allocator,
      description
  );

  this->Create(
    allocator,
    base_camera,
    true,
    depth_level,
    depth_range,
    real_switch
  );
}

//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*          allocator,
    Camera*                          base_camera,
    b32                              base_camera_owner_this,
    f32                              depth_level,
    f32                              depth_range,
    b32                              real_switch
)
{
  // �x�[�X�J����
  m_base_camera_owner_this = base_camera_owner_this;
  m_base_camera = base_camera; 

  // ���ڗp�J����
  nw::gfx::Camera* nw_left_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
  GFL_ASSERT_STOP(nw_left_camera);
  // SetWScale����Ȃ炱���B
  m_left_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_left_camera->Create(nw_left_camera);
    
  // �E�ڗp�J����
  nw::gfx::Camera* nw_right_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
  GFL_ASSERT_STOP(nw_right_camera);
  // SetWScale����Ȃ炱���B
  m_right_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_right_camera->Create(nw_right_camera);

  m_depth_level = depth_level;
  m_depth_range = depth_range;
  m_real_switch = real_switch;

  // �X�e���I�J�����𐶐�����
  m_nn_stereo_camera = GFL_NEW(allocator->GetHeapBase()) nn::ulcd::CTR::StereoCamera;
      // ���������s��Ȃ����߁A�ʓr�AInitialize ���ĂԕK�v������܂��B
  m_nn_stereo_camera->Initialize();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void StereoCamera::Destroy(void)
{
  if( m_nn_stereo_camera )
  {
    GFL_DELETE m_nn_stereo_camera;
        // ������ Finalize() ���Ăяo���܂��B
  }

  if( m_right_camera )
  {
    GFL_DELETE m_right_camera;
  }
  if( m_left_camera )
  {
    GFL_DELETE m_left_camera;
  }

  if( m_base_camera )
  {
    if( m_base_camera_owner_this )
    {
      GFL_DELETE m_base_camera;
    }
  }

  Camera::Destroy();

  m_base_camera_owner_this = false;
  m_base_camera            = NULL;

  m_left_camera         = NULL;
  m_right_camera        = NULL;
 
  m_nn_stereo_camera    = NULL;

  m_depth_level         = DEPTH_LEVEL_DEFAULT;
  m_depth_range         = DEPTH_RANGE_DEFAULT;
  m_real_switch         = REAL_SWITCH_DEFAULT;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z����
 */
//-----------------------------------------------------------------------------
void StereoCamera::Calculate(void)
{
  Camera::Calculate();
}




//
// �J�����}�g���N�X
//

//-----------------------------------------------------------------------------
/*
 *  @brief        �r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����
 *                �x�[�X�J�����A���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
 *
 *  @param[in]    update_base_camera  true�̂Ƃ��A�x�[�X�J�����A���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
 *                                    false�̂Ƃ��A�x�[�X�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X�͍X�V�����A
 *                                                 ���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
 *                                                 ���̍ہA�x�[�X�J�����Ɍ��ݐݒ肵�Ă���r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���g�p���܂��B
 */
//-----------------------------------------------------------------------------
void StereoCamera::UpdateCameraMatrix(void)
{
  UpdateCameraMatrices(true);
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����
 *                (�x�[�X�J�����A)���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
 *
 *  @param[in]    update_base_camera  true�̂Ƃ��A�x�[�X�J�����A���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
 *                                    false�̂Ƃ��A�x�[�X�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X�͍X�V�����A
 *                                                 ���ڗp�J�����A�E�ڗp�J�����̃r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����B
 *                                                 ���̍ہA�x�[�X�J�����Ɍ��ݐݒ肵�Ă���r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���g�p���܂��B
 */
//-----------------------------------------------------------------------------
void StereoCamera::UpdateCameraMatrices(b32 update_base_camera)
{
  // �x�[�X�J����
  if( update_base_camera )
  {
    GetBaseCamera()->Camera::UpdateCameraMatrix();
  }

  // ���ڗp�J�����A�E�ڗp�J����
  gfl::math::MTX44& base_proj  = GetBaseCamera()->GetNwCamera()->ProjectionMatrix();
  gfl::math::MTX34& base_view  = GetBaseCamera()->GetNwCamera()->ViewMatrix();
  gfl::math::MTX44& left_proj  = m_left_camera->GetNwCamera()->ProjectionMatrix();
  gfl::math::MTX34& left_view  = m_left_camera->GetNwCamera()->ViewMatrix();
  gfl::math::MTX44& right_proj = m_right_camera->GetNwCamera()->ProjectionMatrix();
  gfl::math::MTX34& right_view = m_right_camera->GetNwCamera()->ViewMatrix();

  // Ortho�J�������X�e���I�\������ꍇ�͎����ŊG��p�ӂ��Ȃ���΂����܂���B
  if( GetBaseCamera()->GetNwCamera()->GetResCamera().GetProjectionType() == nw::gfx::ResCamera::PROJTYPE_ORTHO )
  {
    // @note ���݂�Ortho�J�����̃X�e���I�\���ɂ͖��Ή��ł��B
    gfl::math::MTX44Copy(&left_proj, &base_proj);
    gfl::math::MTX34Copy(&left_view, &base_view);
    gfl::math::MTX44Copy(&right_proj, &base_proj);
    gfl::math::MTX34Copy(&right_view, &base_view);

    // nn::ulcd::CTR::StereoCamera::CalculateMatrices(....)�ŉ�ʂɂ��킹�Ẳ�]���s���Ă���̂ŁA
    // Ortho�̏ꍇ��pivot��ݒ肵�܂��B
    GetBaseCamera()->GetNwCamera()->GetProjectionUpdater()->SetPivotDirection(static_cast<nn::math::PivotDirection>(gfl::math::PIVOT_UPSIDE_TO_TOP));
    m_left_camera->GetNwCamera()->GetProjectionUpdater()->SetPivotDirection(static_cast<nn::math::PivotDirection>(gfl::math::PIVOT_UPSIDE_TO_TOP));
    m_right_camera->GetNwCamera()->GetProjectionUpdater()->SetPivotDirection(static_cast<nn::math::PivotDirection>(gfl::math::PIVOT_UPSIDE_TO_TOP));
  }
  else
  {
    // �Q�[���Ƃ��ăX�e���I�`����֎~���Ă���ۂ̑Ώ�
    f32 depth_range = 0.0f;
    if( GraphicsSystem::IsGameStereoFlag() ){
      if( nn::gx::IsStereoVisionAllowed()  ){
        depth_range = m_depth_range;
      }
    }

    m_nn_stereo_camera->CalculateMatrices(
        &left_proj,
        &left_view,
        &right_proj,
        &right_view,
        &base_proj,
        &base_view,
        m_depth_level,
        depth_range,
        m_real_switch
    );
  }

  // �t�s����X�V����
  GetLeftCamera()->SetProjectionMatrix(left_proj);
  GetLeftCamera()->SetViewMatrix(left_view);
  GetRightCamera()->SetProjectionMatrix(right_proj);
  GetRightCamera()->SetViewMatrix(right_view);
}




//
// ���� / �j��
//

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
 *
 * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
 */
//-----------------------------------------------------------------------------
void StereoCamera::Create(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3&           camera_position,
    const gfl::math::VEC3&           target_position,
    f32                              near_clip,
    f32                              far_clip,
    f32                              fovy_radian,
    f32                              w_scale,
    f32                              depth_level,
    f32                              depth_range,
    b32                              real_switch
)
{
  nw::gfx::Camera* nw_base_camera;
  nw::gfx::Camera* nw_left_camera;
  nw::gfx::Camera* nw_right_camera;

  CreateStereoCameras(
    &nw_base_camera,
    &nw_left_camera,
    &nw_right_camera,
    allocator,
    camera_position,
    target_position,
    near_clip,
    far_clip,
    fovy_radian,
    w_scale
  );

  SetNwSceneNode(nw_base_camera);
 
  m_left_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_left_camera->Create(nw_left_camera);
  
  m_right_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_right_camera->Create(nw_right_camera);

  m_depth_level = depth_level;
  m_depth_range = depth_range;
  m_real_switch = real_switch;

  // �X�e���I�J�����𐶐�����
  m_nn_stereo_camera = GFL_NEW(allocator->GetHeapBase()) nn::ulcd::CTR::StereoCamera;
      // ���������s��Ȃ����߁A�ʓr�AInitialize ���ĂԕK�v������܂��B
  m_nn_stereo_camera->Initialize();
}

void StereoCamera::CreateFrustum(
  gfl::heap::NwAllocator*          allocator,
  const gfl::math::VEC3& cameraPosition,
  const gfl::math::VEC3& targetPosition,
  f32 nearClip,
  f32 farClip,
  const gfl::math::VEC2& projectionCenter,
  f32 projectionHeight,
  gfl::math::PivotDirection pivotDirection,
  f32 w_scale,

    f32                              depth_level,
    f32                              depth_range,
    b32                              real_switch
    )
{
  nw::gfx::Camera* nw_base_camera;
  nw::gfx::Camera* nw_left_camera;
  nw::gfx::Camera* nw_right_camera;

  CreateFrustumStereoCameras(
    &nw_base_camera,
    &nw_left_camera,
    &nw_right_camera,
    allocator,
    cameraPosition,
    targetPosition,
    nearClip,
    farClip,
    projectionCenter,
    projectionHeight,
    pivotDirection,
    w_scale
  );

  SetNwSceneNode(nw_base_camera);
 
  m_left_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_left_camera->Create(nw_left_camera);
  
  m_right_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_right_camera->Create(nw_right_camera);

  m_depth_level = depth_level;
  m_depth_range = depth_range;
  m_real_switch = real_switch;

  // �X�e���I�J�����𐶐�����
  m_nn_stereo_camera = GFL_NEW(allocator->GetHeapBase()) nn::ulcd::CTR::StereoCamera;
      // ���������s��Ȃ����߁A�ʓr�AInitialize ���ĂԕK�v������܂��B
  m_nn_stereo_camera->Initialize();
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

