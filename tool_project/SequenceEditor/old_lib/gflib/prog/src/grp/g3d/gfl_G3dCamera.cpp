//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dCamera.cpp
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
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_Math.h>
#include <gfl_debug.h>
#include <grp/gfl_Camera.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>

#include <grp/g3d/gfl_G3dCamera.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�J�����N���X
//=====================================

//
// ResourceDescription�̏������֐�
//

// �f�t�H���g�l��ݒ肷��
void Camera::InitializeResourceDescriptionForDefault(
    ResourceDescription*  description
)
{
  description->animation_enable = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;
}




//
// Description�̏������֐�
//

// Description�̏������֐�
// �f�t�H���g�l��ݒ肷��
void Camera::InitializeDescriptionForDefault(
    Description*  description
)
{
  description->view_mode                = VIEW_MODE_LOOK_AT;
  description->camera_position          = gfl::math::VEC3(0.0f,0.0f,100.0f);
  description->target_position          = gfl::math::VEC3(0.0f,0.0f,0.0f);
  description->up_vector                = gfl::math::VEC3(0.0f,1.0f,0.0f);
  description->twist                    = GFL_MATH_DEG_TO_RAD(0.0f);
  description->rotate_xyz               = gfl::math::VEC3(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f));
  description->w_scale                  = 0.0f;
  description->projection_mode          = PROJECTION_MODE_PERSPECTIVE;
  description->fovy                     = GFL_MATH_DEG_TO_RAD(37.8f);
  description->aspect                   = 400.0f/240.0f;
  description->height                   = 240.0f;
  description->center                   = gfl::math::VEC2(0.0f,0.0f);
  description->near                     = 1.0f;
  description->far                      = 10000.0f;
  description-> pivot                   = gfl::math::PIVOT_NONE;
  description->max_callbacks            = 0;
  description->max_children             = 0;
  description->is_fixed_size_memory     = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}

// Description�̏������֐�
// AIM     PERSPECTIVE
void Camera::InitializeDescriptionForAimPerspective(
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
)
{
  description->view_mode            = VIEW_MODE_AIM;
  description->camera_position      = camera_position;
  description->target_position      = target_position;
  description->up_vector            = gfl::math::VEC3(0.0f,1.0f,0.0f);
  description->twist                = twist;
  description->rotate_xyz           = gfl::math::VEC3(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f));
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_PERSPECTIVE;
  description->fovy                 = fovy;
  description->aspect               = aspect;
  description->height               = 240.0f;
  description->center               = gfl::math::VEC2(0.0f,0.0f);
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}
// AIM     FRUSTUM
void Camera::InitializeDescriptionForAimFrustum(
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
)
{
  description->view_mode            = VIEW_MODE_AIM;
  description->camera_position      = camera_position;
  description->target_position      = target_position;
  description->up_vector            = gfl::math::VEC3(0.0f,1.0f,0.0f);
  description->twist                = twist;
  description->rotate_xyz           = gfl::math::VEC3(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f));
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_FRUSTUM;
  description->fovy                 = GFL_MATH_DEG_TO_RAD(37.8f);
  description->aspect               = aspect;
  description->height               = height;
  description->center               = center;
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}
// AIM     ORTHO
void Camera::InitializeDescriptionForAimOrtho(
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
)
{
  description->view_mode            = VIEW_MODE_AIM;
  description->camera_position      = camera_position;
  description->target_position      = target_position;
  description->up_vector            = gfl::math::VEC3(0.0f,1.0f,0.0f);
  description->twist                = twist;
  description->rotate_xyz           = gfl::math::VEC3(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f));
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_ORTHO;
  description->fovy                 = GFL_MATH_DEG_TO_RAD(37.8f);
  description->aspect               = aspect;
  description->height               = height;
  description->center               = center;
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}
// LOOK_AT PERSPECTIVE
void Camera::InitializeDescriptionForLookAtPerspective(
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
)
{
  description->view_mode            = VIEW_MODE_LOOK_AT;
  description->camera_position      = camera_position;
  description->target_position      = target_position;
  description->up_vector            = up_vector;
  description->twist                = GFL_MATH_DEG_TO_RAD(0.0f);
  description->rotate_xyz           = gfl::math::VEC3(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f));
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_PERSPECTIVE;
  description->fovy                 = fovy;
  description->aspect               = aspect;
  description->height               = 240.0f;
  description->center               = gfl::math::VEC2(0.0f,0.0f);
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}
// LOOK_AT FRUSTUM
void Camera::InitializeDescriptionForLookAtFrustum(
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
)
{
  description->view_mode            = VIEW_MODE_LOOK_AT;
  description->camera_position      = camera_position;
  description->target_position      = target_position;
  description->up_vector            = up_vector;
  description->twist                = GFL_MATH_DEG_TO_RAD(0.0f);
  description->rotate_xyz           = gfl::math::VEC3(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f));
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_FRUSTUM;
  description->fovy                 = GFL_MATH_DEG_TO_RAD(37.8f);
  description->aspect               = aspect;
  description->height               = height;
  description->center               = center;
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}
// LOOK_AT ORTHO
void Camera::InitializeDescriptionForLookAtOrtho(
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
)
{
  description->view_mode            = VIEW_MODE_LOOK_AT;
  description->camera_position      = camera_position;
  description->target_position      = target_position;
  description->up_vector            = up_vector;
  description->twist                = GFL_MATH_DEG_TO_RAD(0.0f);
  description->rotate_xyz           = gfl::math::VEC3(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f));
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_ORTHO;
  description->fovy                 = GFL_MATH_DEG_TO_RAD(37.8f);
  description->aspect               = aspect; 
  description->height               = height;
  description->center               = center;
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}
// ROTATE  PERSPECTIVE
void Camera::InitializeDescriptionForRotatePerspective(
  Description*                      description,
  const gfl::math::VEC3&            camera_position,
  const gfl::math::VEC3&            rotate_xyz,  // [radian]
  const f32                         w_scale,
  const f32                         fovy,  // [radian]
  const f32                         aspect,
  const f32                         near,
  const f32                         far,
  const gfl::math::PivotDirection   pivot
)
{
  description->view_mode            = VIEW_MODE_ROTATE;
  description->camera_position      = camera_position;
  description->target_position      = gfl::math::VEC3(0.0f,0.0f,0.0f);
  description->up_vector            = gfl::math::VEC3(0.0f,1.0f,0.0f);
  description->twist                = GFL_MATH_DEG_TO_RAD(0.0f);
  description->rotate_xyz           = rotate_xyz;
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_PERSPECTIVE;
  description->fovy                 = fovy;
  description->aspect               = aspect;
  description->height               = 240.0f;
  description->center               = gfl::math::VEC2(0.0f,0.0f);
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}
// ROTATE  FRUSTUM
void Camera::InitializeDescriptionForRotateFrustum(
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
)
{
  description->view_mode            = VIEW_MODE_ROTATE;
  description->camera_position      = camera_position;
  description->target_position      = gfl::math::VEC3(0.0f,0.0f,0.0f);
  description->up_vector            = gfl::math::VEC3(0.0f,1.0f,0.0f);
  description->twist                = GFL_MATH_DEG_TO_RAD(0.0f);
  description->rotate_xyz           = rotate_xyz;
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_FRUSTUM;
  description->fovy                 = GFL_MATH_DEG_TO_RAD(37.8f);
  description->aspect               = aspect;
  description->height               = height;
  description->center               = center;
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}
// ROTATE  ORTHO
void Camera::InitializeDescriptionForRotateOrtho(
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
)
{
  description->view_mode            = VIEW_MODE_ROTATE;
  description->camera_position      = camera_position;
  description->target_position      = gfl::math::VEC3(0.0f,0.0f,0.0f);
  description->up_vector            = gfl::math::VEC3(0.0f,1.0f,0.0f);
  description->twist                = GFL_MATH_DEG_TO_RAD(0.0f);
  description->rotate_xyz           = rotate_xyz;
  description->w_scale              = w_scale;
  description->projection_mode      = PROJECTION_MODE_ORTHO;
  description->fovy                 = GFL_MATH_DEG_TO_RAD(37.8f);
  description->aspect               = aspect;
  description->height               = height;
  description->center               = center;
  description->near                 = near;
  description->far                  = far;
  description-> pivot               = pivot;
  description->max_callbacks        = 0;
  description->max_children         = 0;
  description->is_fixed_size_memory = false;
  description->animation_enable         = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit    = 0;
}




//
// �R���X�g���N�^ / �f�X�g���N�^
//

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
Camera::Camera(void)
  : SceneNode(),
    //m_animation(NULL)
    m_animation_slot_num(0),
    m_animation_blend_unit_num(0),
    m_animation_num_in_unit(0),
    m_animation_manager(NULL)
{
  GFL_SINGLETON_INSTANCE(System)->GetCameraManager()->AddObject(this);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
Camera::~Camera()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetCameraManager()->RemoveObject(this);
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
 */
//-----------------------------------------------------------------------------
void Camera::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource,
    ResourceDescription*              description
)
{
  ResourceDescription l_description;
  if( description )
  {
    l_description = *description;
  }

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
  //�������Ő�������ƁA��Ƀt���[������֐����Ăяo�����Ƃ���m_animation_manager���Ȃ��Ƃ����������B
  //������ʂ��Ă��炸��������Ă��Ȃ��̂����B������ʂ炸�ɃA�j���[�V������t������Ƃ͎v���Ȃ��̂����B
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     description    
 *
 * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
 */
//-----------------------------------------------------------------------------
void Camera::Create(
    gfl::heap::NwAllocator*    allocator,
    Description*               description
)
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  nw::gfx::CameraViewUpdater*        viewUpdater       = NULL;
  nw::gfx::CameraProjectionUpdater*  projectionUpdater = NULL;

  // ViewMode����
  switch( l_description.view_mode )
  {
  case VIEW_MODE_AIM:     viewUpdater = CreateAimViewUpdater(
                              allocator,
                              l_description.target_position,
                              l_description.twist);
                          break;
  case VIEW_MODE_LOOK_AT: viewUpdater = CreateLookAtViewUpdater(
                              allocator,
                              l_description.target_position,
                              l_description.up_vector);
                          break;
  case VIEW_MODE_ROTATE:  viewUpdater = CreateRotateViewUpdater(allocator,
                              l_description.rotate_xyz);
                          break;
  }

  // ProjectionMode
  switch( l_description.projection_mode )
  {
  case PROJECTION_MODE_PERSPECTIVE: projectionUpdater = CreatePerspectiveProjectionUpdater(
                                        allocator,
                                        l_description.fovy,
                                        l_description.aspect);
                                    break;
  case PROJECTION_MODE_FRUSTUM:     projectionUpdater = CreateFrustumProjectionUpdater(
                                        allocator,
                                        l_description.aspect,
                                        l_description.height,
                                        l_description.center);
                                    break;
  case PROJECTION_MODE_ORTHO:       projectionUpdater = CreateOrthoProjectionUpdater(
                                        allocator,
                                        l_description.aspect,
                                        l_description.height,
                                        l_description.center);
                                    break;
  default:
      GFL_ASSERT_STOP_MSG(0, "projection_mode=%d���s���̂��߁A�J�����𐶐��ł��܂���B\n", l_description.projection_mode);
      break;
  }
  
  GFL_ASSERT_STOP_MSG(projectionUpdater, "projectionUpdater��NULL�̂��߁A�J�����𐶐��ł��܂���B\n");

  projectionUpdater->SetPivotDirection( static_cast<nw::math::PivotDirection>(l_description.pivot) );
  projectionUpdater->GetResource().SetNear( l_description.near );
  projectionUpdater->GetResource().SetFar( l_description.far );

  // �J����
  nw::gfx::Camera* camera =
    nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(l_description.is_fixed_size_memory)
      .MaxChildren(l_description.max_children)
      .MaxCallbacks(l_description.max_callbacks)
      .ViewUpdater(viewUpdater)
      .ProjectionUpdater(projectionUpdater)
      .Create(allocator);
    
  camera->SetWScale(l_description.w_scale);
  camera->Transform().SetTranslate(l_description.camera_position);

  m_animation_slot_num = (l_description.animation_enable)?(1):(0);  // �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
                                                                    // ������1�ɂ��Ă����ʂł��B
                                                                    // nw::gfx::Camera::GetAnimGroup  �J������ DynamicBuilder �Ő��������ꍇ�� NULL ��Ԃ��܂��B ���̏ꍇ�A�A�j���[�V�����͐ݒ�ł��܂���B
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;

  SetNwSceneNode(camera);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void Camera::Destroy(void)
{
  // �Ǝ����� ��������
#if 0
  if( m_animation )
  {
    GFL_DELETE m_animation;
    m_animation = NULL;
  }
#endif
  if( m_animation_manager )
  {
    GFL_DELETE m_animation_manager;
    m_animation_manager = NULL;
  }
  m_animation_slot_num = 0;
  m_animation_blend_unit_num = 0;
  m_animation_num_in_unit = 0;
  // �Ǝ����� �����܂�

  SceneNode::Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z����
 */
//-----------------------------------------------------------------------------
void Camera::Calculate(void)
{
  // �A�j���[�V�����}�l�[�W��
  if(m_animation_manager)
  {
    m_animation_manager->Calculate();
  }
}




//
// �g�����X�t�H�[��
//

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu�𓾂�
 *
 *  @param[in]    nw_camera          �J����
 *  @param[out]   camera_position    �J�����̈ʒu��Ԃ�
 */
//-----------------------------------------------------------------------------
void Camera::GetCameraPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& camera_position )
{
  nw_camera.Transform().GetTranslate(&camera_position);
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̒����_�̈ʒu�𓾂�
 *
 *  @param[in]    nw_camera          �J����
 *  @param[out]   target_position    �J�����̒����_�̈ʒu��Ԃ�
 */
//-----------------------------------------------------------------------------
void Camera::GetTargetPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& target_position )
{
  nw::gfx::ResCameraViewUpdater res_view_updater = nw_camera.GetViewUpdater()->GetResource();
    
  nw::gfx::ResLookAtTargetViewUpdater res_look_at_target_view_updater =
    nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(res_view_updater);

  if(res_look_at_target_view_updater.IsValid())
  {
    target_position = res_look_at_target_view_updater.GetTargetPosition();
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu�𓾂�
 *
 *  @param[in]    nw_camera          �J����
 *  @param[out]   camera_position    �J�����̈ʒu��Ԃ�
 *  @param[out]   target_position    �J�����̒����_�̈ʒu��Ԃ�
 */
//-----------------------------------------------------------------------------
void Camera::GetCameraAndTargetPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& camera_position, gfl::math::VEC3& target_position )
{
  nw::gfx::ResCameraViewUpdater res_view_updater = nw_camera.GetViewUpdater()->GetResource();
    
  nw::gfx::ResLookAtTargetViewUpdater res_look_at_target_view_updater =
    nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(res_view_updater);

  nw_camera.Transform().GetTranslate(&camera_position);

  if(res_look_at_target_view_updater.IsValid())
  {
    target_position = res_look_at_target_view_updater.GetTargetPosition();
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu��ݒ肷��
 *
 *  @param[in]    nw_camera          �J����
 *  @param[out]   camera_position    �J�����̈ʒu
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& camera_position )
{
  nw_camera.Transform().SetTranslate(camera_position);
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̒����_�̈ʒu��ݒ肷��
 *
 *  @param[in]    nw_camera          �J����
 *  @param[out]   target_position    �J�����̒����_�̈ʒu
 */
//-----------------------------------------------------------------------------
void Camera::SetTargetPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& target_position )
{
  nw::gfx::ResCameraViewUpdater res_view_updater = nw_camera.GetViewUpdater()->GetResource();
    
  nw::gfx::ResLookAtTargetViewUpdater res_look_at_target_view_updater =
    nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(res_view_updater);

  if(res_look_at_target_view_updater.IsValid())
  {
    res_look_at_target_view_updater.SetTargetPosition(target_position);
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu��ݒ肷��
 *
 *  @param[in]    nw_camera          �J����
 *  @param[in]    camera_position    �J�����̈ʒu
 *  @param[in]    target_position    �J�����̒����_�̈ʒu
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraAndTargetPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position )
{
  nw::gfx::ResCameraViewUpdater res_view_updater = nw_camera.GetViewUpdater()->GetResource();
    
  nw::gfx::ResLookAtTargetViewUpdater res_look_at_target_view_updater =
    nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(res_view_updater);

  if(res_look_at_target_view_updater.IsValid())
  {
    res_look_at_target_view_updater.SetTargetPosition(target_position);
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }

  nw_camera.Transform().SetTranslate(camera_position);
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ��]�J�����̊p�x
 *                �P�ʂ̓��W�A���B
 */
//-----------------------------------------------------------------------------
void Camera::GetRotateXYZ(gfl::math::VEC3* r) const
{
  ViewMode view_mode = GetViewMode();
  if( view_mode == VIEW_MODE_ROTATE )
  {
    *r = GetNwCamera()->GetViewRotate();
  }
  else
  {
    this->GetAimAndTwistRotateXYZ( r, view_mode );
  }
}
void Camera::SetRotateXYZ(const gfl::math::VEC3& r)
{
  ViewMode view_mode = GetViewMode();
  if( view_mode == VIEW_MODE_ROTATE )
  {
    GetNwCamera()->SetViewRotate(r);
  }
  else
  {
    this->SetAimAndTwistRotateXYZ( r.x, r.y, r.z, view_mode );
  }
}
void Camera::SetRotateXYZ(f32 x, f32 y, f32 z)
{
  ViewMode view_mode = GetViewMode();
  if( view_mode == VIEW_MODE_ROTATE )
  {
    GetNwCamera()->SetViewRotate(x,y,z);
  }
  else
  {
    this->SetAimAndTwistRotateXYZ( x, y, z, view_mode );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X���擾����
 *
 *  @param[out]    matrix          �擾�����}�g���N�X���i�[���ĕԂ�
 *
 *  @retval        bool            ����Ɏ擾�ł�����true
 */
//-----------------------------------------------------------------------------
bool Camera::GetWorldMatrix(gfl::math::MTX34* matrix) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  if( nw_camera )
  {
    const gfl::math::MTX34& mtx = nw_camera->WorldMatrix();
    *matrix = mtx;
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X��ݒ肷��
 *                 Scene��Calculate�̌�łȂ��Ɣ��f����܂���B
 *
 *  @param[in]     matrix          �ݒ肷��}�g���N�X
 *
 *  @retval        bool            ����ɐݒ�ł�����true
 */
//-----------------------------------------------------------------------------
bool Camera::SetWorldMatrix(const gfl::math::MTX34& matrix)
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  if( nw_camera )
  {
    gfl::math::MTX34& mtx = nw_camera->WorldMatrix();
    mtx = matrix;
    
    // ���[���h�s����X�V�����̂ŁA�t�s��̃L���b�V���𖳌������܂��B
    nw_camera->InvalidateInverseWorldMatrix();

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �}�g���b�N�X�̃j�A�A�t�@�[�l��ύX
 *	@note   �g�p��A SetMatrixNearFarDefault �ŕK�����ɖ߂����ƁB
 *
 *	@param	f32 near �j�A�l
 *	@param	f32 far  �t�@�[�l
 */
//-----------------------------------------------------------------------------
void Camera::SetMatrixNearFar( const f32 near, const f32 far )
{
  gfl::grp::g3d::ObjectType type = this->GetObjectType();

#if 0
  internal::PrintCameraMatrix(this, "before");
  if(type == gfl::grp::g3d::OBJECT_TYPE_STEREO_CAMERA)
  {
    internal::PrintCameraMatrix((reinterpret_cast<gfl::grp::g3d::StereoCamera*>(this))->GetLeftCamera(), "before left");
    internal::PrintCameraMatrix((reinterpret_cast<gfl::grp::g3d::StereoCamera*>(this))->GetRightCamera(), "before right");
  }
#endif

#if 0
  this->SetFarClip( far );
  this->UpdateCameraMatrix();
#else  
  // �v���W�F�N�V�����}�g���N�X�𒼐ڕύX���AUpdateCameraMatrix�͌Ă΂Ȃ��悤�ɂ��Ă���B

  // �Q�l
  // https://ntsc.nintendo.co.jp/twl/twl_mancollection-DL/Packages/TWL_SDK/TwlSDK/man/ja_JP/fx/mtx/MTX_Perspective.html
  // TWL�}�j���A����S�W
  // TWL-SDK
  // �֐����t�@�����X
  // MTX
  // 4x4�s��v�Z
  // �Œ菬���֘A API �@�\�ꗗ
  // 4x4�s��v�Z
  // MTX_Perspective

  gfl::math::MTX44 projection_matrix;
  this->GetProjectionMatrix( &projection_matrix );
 
  //GFL_PRINT("ProjectionMatrix\n");
  //GFL_PRINT("%f  %f  %f  %f\n", projection_matrix.f._00, projection_matrix.f._01, projection_matrix.f._02, projection_matrix.f._03);
  //GFL_PRINT("%f  %f  %f  %f\n", projection_matrix.f._10, projection_matrix.f._11, projection_matrix.f._12, projection_matrix.f._13);
  //GFL_PRINT("%f  %f  %f  %f\n", projection_matrix.f._20, projection_matrix.f._21, projection_matrix.f._22, projection_matrix.f._23);
  //GFL_PRINT("%f  %f  %f  %f\n", projection_matrix.f._30, projection_matrix.f._31, projection_matrix.f._32, projection_matrix.f._33);

  projection_matrix.f._22 = far/(far-near);
  projection_matrix.f._23 = far*near/(far-near);
  this->SetProjectionMatrix( projection_matrix );
  if(type == gfl::grp::g3d::OBJECT_TYPE_STEREO_CAMERA)
  {
    (reinterpret_cast<gfl::grp::g3d::StereoCamera*>(this))->UpdateCameraMatrices(false);
   
    //this->SetCameraMatrixNearFar(
    //  (reinterpret_cast<gfl::grp::g3d::StereoCamera*>(this))->GetLeftCamera(),
    //  near,
    //  far
    //);
    //this->SetCameraMatrixNearFar(
    //  (reinterpret_cast<gfl::grp::g3d::StereoCamera*>(this))->GetRightCamera(),
    //  near,
    //  far
    //);
  }
#endif

#if 0
  internal::PrintCameraMatrix(this, "after");
  if(type == gfl::grp::g3d::OBJECT_TYPE_STEREO_CAMERA)
  {
    internal::PrintCameraMatrix((reinterpret_cast<gfl::grp::g3d::StereoCamera*>(this))->GetLeftCamera(), "after left");
    internal::PrintCameraMatrix((reinterpret_cast<gfl::grp::g3d::StereoCamera*>(this))->GetRightCamera(), "after right");
  }
#endif

#if 0
/*near =   32.0f;
far  = 1800.0f;
�̂Ƃ�

// Base
ProjectionMatrix
2.239229  0.000000  0.000000  0.000000
0.000000  3.732048  0.000000  0.000000
0.000000  0.000000  1.018100  32.579185
0.000000  0.000000  -1.000000  0.000000

// Left
ProjectionMatrix
0.000000  3.732048  0.000000  0.000000
-2.239229  -0.000000  -0.000000  -0.000000
0.000000  0.000000  1.018100  32.579185
0.000000  0.000000  -1.000000  0.000000

// Right
ProjectionMatrix
0.000000  3.732048  0.000000  0.000000
-2.239229  -0.000000  -0.000000  -0.000000
0.000000  0.000000  1.018100  32.579185
0.000000  0.000000  -1.000000  0.000000*/
#endif

}

//-----------------------------------------------------------------------------
/**
 *	@brief  �}�g���b�N�X�̃j�A�A�t�@�[������ŕێ����Ă���l�ɖ߂�
 */
//-----------------------------------------------------------------------------
void Camera::SetMatrixNearFarDefault( void )
{
  f32 near = GetNearClip();
  f32 far = GetFarClip();
  
  SetMatrixNearFar( near, far );
}




// 
// �ϊ�
//

//-----------------------------------------------------------------------------
/*
 *  @brief         grp�J�����̓��e��g3d�J�����ɔ��f����
 *
 *  @param[in]     grp_camera    grp�J����
 */
//-----------------------------------------------------------------------------
void Camera::ConvertGrpCameraToG3dCamera( const gfl::grp::Camera& grp_camera )
{
  const gfl::grp::ViewParam&       view_param       = grp_camera.GetViewParam();
  const gfl::grp::ProjectionParam& projection_param = grp_camera.GetProjectionParam();

  nw::gfx::Camera* nw_camera = GetNwCamera();
 
  // view_param
  nw::gfx::ResCameraViewUpdater res_view_updater = nw_camera->GetViewUpdater()->GetResource();

  nw::gfx::ResLookAtTargetViewUpdater res_look_at_target_view_updater =
    nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(res_view_updater);

  if(res_look_at_target_view_updater.IsValid())
  {
    SetCameraAndTargetPosition( view_param.pos, view_param.target );
    res_look_at_target_view_updater.SetUpwardVector( view_param.up );
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }

  // projection_param
  nw::gfx::CameraProjectionUpdater*        projection_updater                 = nw_camera->GetProjectionUpdater();
  nw::gfx::ResCameraProjectionUpdater      res_projection_updater             = projection_updater->GetResource();
  
  projection_updater->SetPivotDirection( static_cast<nw::math::PivotDirection>(projection_param.pivot) );

  res_projection_updater.SetNear(projection_param.near);
  res_projection_updater.SetFar(projection_param.far);
  
  switch(projection_param.proj_type)
  {
  case PROJECTION_FRUSTUM:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  case PROJECTION_PERSPECTIVE:
    {
      nw::gfx::ResPerspectiveProjectionUpdater res_perspective_projection_updater =
        nw::gfx::ResStaticCast<nw::gfx::ResPerspectiveProjectionUpdater>(res_projection_updater);

      if(res_perspective_projection_updater.IsValid())
      {
        res_perspective_projection_updater.SetFovy(projection_param.perspective.fovy);
        res_perspective_projection_updater.SetAspectRatio(projection_param.perspective.aspect);

        // �C��t���Ďg����
        // nw_camera->SetPerspective(projection_param.perspective.fovy, projection_param.perspective.aspect, projection_param.near, projection_param.far);
        // �ł��悳�����B
      }
      else
      {
        GFL_ASSERT_STOP(0);
      }
    }
    break;
  case PROJECTION_ORTHO:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  }

//  nw_camera->UpdateCameraMatrix();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         g3d�J�����̓��e��grp�J�����ɔ��f����
 *
 *  @param[out]    grp_camera    grp�J����
 */
//-----------------------------------------------------------------------------
void Camera::ConvertG3dCameraToGrpCamera( gfl::grp::Camera& grp_camera )
{
  gfl::grp::ViewParam       view_param;
  gfl::grp::ProjectionParam projection_param;

  nw::gfx::Camera* nw_camera = GetNwCamera();
 
  // view_param
  nw::gfx::ResCameraViewUpdater res_view_updater = nw_camera->GetViewUpdater()->GetResource();

  nw::gfx::ResLookAtTargetViewUpdater res_look_at_target_view_updater =
    nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(res_view_updater);

  if(res_look_at_target_view_updater.IsValid())
  {
    GetCameraAndTargetPosition( view_param.pos, view_param.target );
    view_param.up = res_look_at_target_view_updater.GetUpwardVector();
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }

  // projection_param
  nw::gfx::CameraProjectionUpdater*        projection_updater                 = nw_camera->GetProjectionUpdater();
  nw::gfx::ResCameraProjectionUpdater      res_projection_updater             = projection_updater->GetResource();
  
  projection_param.pivot = static_cast<gfl::math::PivotDirection>(projection_updater->GetPivotDirection());

  projection_param.near = res_projection_updater.GetNear();
  projection_param.far  = res_projection_updater.GetFar();

  projection_param.proj_type = gfl::grp::PROJECTION_PERSPECTIVE;
  switch(projection_param.proj_type)
  {
  case PROJECTION_FRUSTUM:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  case PROJECTION_PERSPECTIVE:
    {
      nw::gfx::ResPerspectiveProjectionUpdater res_perspective_projection_updater =
        nw::gfx::ResStaticCast<nw::gfx::ResPerspectiveProjectionUpdater>(res_projection_updater);

      if(res_perspective_projection_updater.IsValid())
      {
        projection_param.perspective.fovy   = res_perspective_projection_updater.GetFovy();
        projection_param.perspective.aspect = res_perspective_projection_updater.GetAspectRatio();

        // �C��t���Ďg����
        // nw_camera->SetPerspective(projection_param.perspective.fovy, projection_param.perspective.aspect, projection_param.near, projection_param.far);
        // �ł��悳�����B
      }
      else
      {
        GFL_ASSERT_STOP(0);
      }
    }
    break;
  case PROJECTION_ORTHO:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  }

  grp_camera.SetUpViewParam(view_param);
  grp_camera.SetUpProjectionParam(projection_param);
}




//
// �J�����}�g���N�X
//

//-----------------------------------------------------------------------------
/*
 *  @brief        �r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X���X�V����
 */
//-----------------------------------------------------------------------------
void Camera::UpdateCameraMatrix(void)
{
  GetNwCamera()->UpdateCameraMatrix();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �r���[�}�g���N�X�𓾂�
 *
 *  @param[out]    view_matrix    �r���[�}�g���N�X
 */
//-----------------------------------------------------------------------------
void Camera::GetViewMatrix( gfl::math::MTX34* view_matrix ) const
{
  *view_matrix = GetNwCamera()->ViewMatrix();
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �r���[�}�g���N�X��ݒ肷��
 *                �y���Ӂz���̌��UpdateCameraMatrix���s���ƁAUpdateCameraMatrix�Ōv�Z�����l�ŏ㏑������Ă��܂��܂��B
 *
 *  @param[in]    view_matrix     �r���[�}�g���N�X
 *  @param[in]    update_matrix   �r���[�}�g���N�X�̋t�s����v�Z�������Ƃ�true
 */
//-----------------------------------------------------------------------------
void Camera::SetViewMatrix( const gfl::math::MTX34& view_matrix, const b32 update_matrix )
{
  gfl::math::MTX34& mtx = GetNwCamera()->ViewMatrix();

  mtx.f._00 = view_matrix.f._00;    mtx.f._01 = view_matrix.f._01;    mtx.f._02 = view_matrix.f._02;    mtx.f._03 = view_matrix.f._03;
  mtx.f._10 = view_matrix.f._10;    mtx.f._11 = view_matrix.f._11;    mtx.f._12 = view_matrix.f._12;    mtx.f._13 = view_matrix.f._13;
  mtx.f._20 = view_matrix.f._20;    mtx.f._21 = view_matrix.f._21;    mtx.f._22 = view_matrix.f._22;    mtx.f._23 = view_matrix.f._23;

  if(update_matrix)
  {
    gfl::math::MTX34& m = GetNwCamera()->InverseViewMatrix();
    u32 exist = gfl::math::MTX34Inverse(&m, &mtx);
    GFL_ASSERT_MSG(exist, "�r���[�}�g���N�X�̋t�s�񂪑��݂��܂���B\n");  //@check
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �v���W�F�N�����}�g���N�X�𓾂�
 *
 *  @param[out]    projection_matrix    �v���W�F�N�V�����}�g���N�X
 */
//-----------------------------------------------------------------------------
void Camera::GetProjectionMatrix( gfl::math::MTX44* projection_matrix ) const
{
  *projection_matrix = GetNwCamera()->ProjectionMatrix();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �v���W�F�N�����}�g���N�X��ݒ肷��
 *                �y���Ӂz���̌��UpdateCameraMatrix���s���ƁAUpdateCameraMatrix�Ōv�Z�����l�ŏ㏑������Ă��܂��܂��B
 *
 *  @param[in]    projection_matrix   �v���W�F�N�V�����}�g���N�X
 *  @param[in]    update_matrix       �v���W�F�N�V�����}�g���N�X�̋t�s����v�Z�������Ƃ�true
 */
//-----------------------------------------------------------------------------
void Camera::SetProjectionMatrix( const gfl::math::MTX44& projection_matrix, const b32 update_matrix )
{
  gfl::math::MTX44& mtx = GetNwCamera()->ProjectionMatrix();

  mtx.f._00 = projection_matrix.f._00;    mtx.f._01 = projection_matrix.f._01;    mtx.f._02 = projection_matrix.f._02;    mtx.f._03 = projection_matrix.f._03;
  mtx.f._10 = projection_matrix.f._10;    mtx.f._11 = projection_matrix.f._11;    mtx.f._12 = projection_matrix.f._12;    mtx.f._13 = projection_matrix.f._13;
  mtx.f._20 = projection_matrix.f._20;    mtx.f._21 = projection_matrix.f._21;    mtx.f._22 = projection_matrix.f._22;    mtx.f._23 = projection_matrix.f._23;
  mtx.f._30 = projection_matrix.f._30;    mtx.f._31 = projection_matrix.f._31;    mtx.f._32 = projection_matrix.f._32;    mtx.f._33 = projection_matrix.f._33;
  
  if(update_matrix)
  {
    gfl::math::MTX44& m = GetNwCamera()->InverseProjectionMatrix();
    u32 exist = gfl::math::MTX44Inverse(&m, &mtx);
    GFL_ASSERT_MSG(exist, "�v���W�F�N�V�����}�g���N�X�̋t�s�񂪑��݂��܂���B\n");  //@check
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �r���[�}�g���N�X�ƃv���W�F�N�����}�g���N�X�𓾂�
 *
 *  @param[out]    view_matrix          �r���[�}�g���N�X
 *  @param[out]    projection_matrix    �v���W�F�N�V�����}�g���N�X
 */
//-----------------------------------------------------------------------------
void Camera::GetViewAndProjectionMatrix( gfl::math::MTX34* view_matrix, gfl::math::MTX44* projection_matrix ) const
{
  GetViewMatrix( view_matrix );
  GetProjectionMatrix( projection_matrix );
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �r���[�}�g���N�X�ƃv���W�F�N�����}�g���N�X��ݒ肷��
 *                �y���Ӂz���̌��UpdateCameraMatrix���s���ƁAUpdateCameraMatrix�Ōv�Z�����l�ŏ㏑������Ă��܂��܂��B
 *
 *  @param[in]    view_matrix         �r���[�}�g���N�X
 *  @param[in]    projection_matrix   �v���W�F�N�V�����}�g���N�X
 *  @param[in]    update_matrix       �r���[�}�g���N�X�ƃv���W�F�N�V�����}�g���N�X�̋t�s����v�Z�������Ƃ�true
 */
//-----------------------------------------------------------------------------
void Camera::SetViewAndProjectionMatrix( const gfl::math::MTX34& view_matrix, const gfl::math::MTX44& projection_matrix, const b32 update_matrix )
{
  SetViewMatrix( view_matrix, update_matrix );
  SetProjectionMatrix( projection_matrix, update_matrix );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �X�N���[�����W�n�̓_�����[���h���W�n�̔�����(�J�����̈ʒu����X�N���[���̓_�ɔ�΂������C)�ɕϊ�����
 *
 *  @param[out]   world_ray_direction   ���[���h���W�n�̔������̕����x�N�g��(�傫��1)
 *  @param[out]   world_ray_origin      ���[���h���W�n�̔������̎n�_���W(�J�����̈ʒuGetCameraPosition�Ɠ����l�������܂�)
 *  @param[in]    screen_point_x        �X�N���[�����W�n�̓_�̈ʒu���W(���オ(0.0f,0.0f)�A�E����(1.0f,1.0f))
 *  @param[in]    screen_point_y        
 *  @param[in]    render_target         �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::ConvertScreenPointToWorldRay(
    gfl::math::VEC3*              world_ray_direction,
    gfl::math::VEC3*              world_ray_origin,
    const f32                     screen_point_x,
    const f32                     screen_point_y,
    const gfl::grp::RenderTarget& render_target
) const
{
  // (screen_point_x, screen_point_y)�́A�r���[�|�[�g�Ő������ꂽ�͈͂�0.0f<= <=1.0f�Ƃ����X�N���[�����W
  f32 left, top, right, bottom;
  render_target.GetViewport(&left, &top, &right, &bottom);
  GFL_ASSERT_MSG( left==0.0f&&top==0.0f, "left=%f, top=%f\n", left, top );  // ����ȊO�͖��Ή� //@check
  s32 width  = render_target.GetWidth();
  s32 height = render_target.GetHeight();

#if 0
  //������
  f32 l_screen_point_x = screen_point_x * (right-left)/width;  // �����_�[�^�[�Q�b�g�̒��ł̔䗦�ɂ���
  f32 l_screen_point_y = screen_point_y * (bottom-top)/height;  // �����_�[�^�[�Q�b�g�̒��ł̔䗦�ɂ��Ă��牺�l�߂ɂ���
  f32 l_screen_point_y = screen_point_y * (bottom-top)/height + (1.0f-(bottom-top)/height);  // �����_�[�^�[�Q�b�g�̒��ł̔䗦�ɂ��Ă��牺�l�߂ɂ���
  // ���͂�͂�v�鏈���������B����ā��͗v��Ȃ������ɂȂ����B
  //f32 l_screen_point_x = screen_point_x;
  //f32 l_screen_point_y = screen_point_y;
#endif
  // �ŏI���ʁ����ꂪ�������B
  f32 l_screen_point_x = screen_point_x;  // �����_�[�^�[�Q�b�g�̒��ł̔䗦�ɂ���
  f32 l_screen_point_y = screen_point_y;  // �����_�[�^�[�Q�b�g�̒��ł̔䗦�ɂ��Ă��牺�l�߂ɂ���

  // -1.0f<= <=1.0f�ɕύX����
  gfl::math::PivotDirection pivot = this->GetPivotDirection();
  gfl::math::VEC4 screen_point;
  if( pivot == gfl::math::PIVOT_UPSIDE_TO_TOP )  // �c�������ւ���
  {
    screen_point = gfl::math::VEC4(2.0f*l_screen_point_y-1.0f, -2.0f*l_screen_point_x+1.0f, 0.0f, 1.0f);  // �c�������ւ���  // z��0.0f�ł�-1.0f�ł�1.0f�ł�����
  }
  else  // if( pivot == gfl::math::PIVOT_NONE )  // �c�������ւ��Ȃ�
  {
    GFL_ASSERT_MSG(pivot == gfl::math::PIVOT_NONE, "���Ή���PivotDirection %d�ł��B\n", pivot); //@check
    screen_point = gfl::math::VEC4(2.0f*l_screen_point_x-1.0f, -2.0f*l_screen_point_y+1.0f, 0.0f, 1.0f);  // �c�������ւ��Ȃ�  // z��0.0f�ł�-1.0f�ł�1.0f�ł�����
  }

  gfl::math::MTX34 view_matrix_34;
  gfl::math::MTX44 projection_matrix;
  GetViewAndProjectionMatrix(&view_matrix_34, &projection_matrix);
  
  gfl::math::MTX44 view_matrix;
  gfl::math::MTX44Identity(&view_matrix);
  // �������~�����̂�3x3�v�f�̂݃R�s�[
  for(u32 i=0; i<3; ++i)
  {
    for(u32 j=0; j<3; ++j)
    //for(u32 j=0; j<4; ++j)�����炾�Ɖ�ʂɉf��Ȃ�
    {
      view_matrix.m[i][j] = view_matrix_34.m[i][j];
    }
  }

  gfl::math::MTX44 view_projection_matrix;
  gfl::math::MTX44 view_projection_matrix_inverse;
  gfl::math::MTX44Mult(&view_projection_matrix, &projection_matrix, &view_matrix);
  gfl::math::MTX44Inverse(&view_projection_matrix_inverse, &view_projection_matrix);

  // �������̕����x�N�g��
  gfl::math::VEC4 world_ray_direction_4;
  nn::math::VEC4Transform(&world_ray_direction_4, &view_projection_matrix_inverse, &screen_point);
  *world_ray_direction = gfl::math::VEC3(world_ray_direction_4.x, world_ray_direction_4.y, world_ray_direction_4.z);
  gfl::math::VEC3Normalize(world_ray_direction, world_ray_direction);

  // �������̎n�_�x�N�g��
  GetNwCamera()->Transform().GetTranslate(world_ray_origin);
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ���ʃX�N���[�����W�n�̓_�����[���h���W�n�̔�����(�J�����̈ʒu����X�N���[���̓_�ɔ�΂������C)�ɕϊ�����
 *
 *  @param[out]   world_ray_direction   ���[���h���W�n�̔������̕����x�N�g��(�傫��1)
 *  @param[out]   world_ray_origin      ���[���h���W�n�̔������̎n�_���W(�J�����̈ʒuGetCameraPosition�Ɠ����l�������܂�)
 *  @param[in]    upper_screen_point_x  ���ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(400-1,240-1))
 *  @param[in]    upper_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B
 *  @param[in]    upper_render_target   �����_�[�^�[�Q�b�g(���ꂪ�x�[�X�J�����̂Ƃ��͍��ł��E�ł��ǂ���ł�����)
 */
//-----------------------------------------------------------------------------
void Camera::ConvertUpperScreenPointToWorldRay(
    gfl::math::VEC3*              world_ray_direction,
    gfl::math::VEC3*              world_ray_origin,
    const s32                     upper_screen_point_x,
    const s32                     upper_screen_point_y,
    const gfl::grp::RenderTarget& upper_render_target
) const
{
  ConvertScreenPointToWorldRay(
      world_ray_direction,
      world_ray_origin,
      upper_screen_point_x/(static_cast<f32>(400-1)),
      upper_screen_point_y/(static_cast<f32>(240-1)),
      upper_render_target
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ����ʃX�N���[�����W�n�̓_�����[���h���W�n�̔�����(�J�����̈ʒu����X�N���[���̓_�ɔ�΂������C)�ɕϊ�����
 *
 *  @param[out]   world_ray_direction   ���[���h���W�n�̔������̕����x�N�g��(�傫��1)
 *  @param[out]   world_ray_origin      ���[���h���W�n�̔������̎n�_���W(�J�����̈ʒuGetCameraPosition�Ɠ����l�������܂�)
 *  @param[in]    lower_screen_point_x  ����ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(320-1,240-1))
 *  @param[in]    lower_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B
 *  @param[in]    lower_render_target   �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::ConvertLowerScreenPointToWorldRay(
    gfl::math::VEC3*              world_ray_direction,
    gfl::math::VEC3*              world_ray_origin,
    const s32                     lower_screen_point_x,
    const s32                     lower_screen_point_y,
    const gfl::grp::RenderTarget& lower_render_target
) const
{
  ConvertScreenPointToWorldRay(
      world_ray_direction,
      world_ray_origin,
      lower_screen_point_x/(static_cast<f32>(320-1)),
      lower_screen_point_y/(static_cast<f32>(240-1)),
      lower_render_target
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �X�N���[�����W�n�̓_�����[���h���W�n�̓_(�j�A�ʏ�or�t�@�[�ʏ�)�ɕϊ�����
 *
 *  @param[out]   world_point           ���[���h���W�n�̓_�̈ʒu���W
 *  @param[in]    is_near               true�̂Ƃ��j�A�ʏ�Afalse�̂Ƃ��t�@�[�ʏ�
 *  @param[in]    screen_point_x        �X�N���[�����W�n�̓_�̈ʒu���W(���オ(0.0f,0.0f)�A�E����(1.0f,1.0f))
 *  @param[in]    screen_point_y
 *  @param[in]    render_target         �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::ConvertScreenPointToWorldPoint(
    gfl::math::VEC3*              world_point,
    const b32                     is_near,
    const f32                     screen_point_x,
    const f32                     screen_point_y,
    const gfl::grp::RenderTarget& render_target
) const
{
  // �ǐ^�񒆂�ʂ锼����
  gfl::math::VEC3 center_world_ray_direction;
  gfl::math::VEC3 center_world_ray_origin;
  this->ConvertScreenPointToWorldRay(
    &center_world_ray_direction,
    &center_world_ray_origin,
    0.5f,
    0.5f,
    render_target
  );

  // (screen_point_x, screen_point_y)��ʂ锼����
  gfl::math::VEC3 current_world_ray_direction;
  gfl::math::VEC3 current_world_ray_origin;
  this->ConvertScreenPointToWorldRay(
    &current_world_ray_direction,
    &current_world_ray_origin,
    screen_point_x,
    screen_point_y,
    render_target
  );

  // center_world_ray_origin��current_world_ray_origin�͓����ɂȂ��Ă��܂��B

  // center_world_ray_direction��current_world_ray_direction�̊Ԃ̊p�x��cos��cos(theta)�Ƃ���ƁA�����x�N�g���̑傫����1�ɂ��Ă���̂ŁA
  f32 cos_theta = gfl::math::VEC3Dot(&center_world_ray_direction, &current_world_ray_direction);
 
  // �j�A�ʏ�or�t�@�[�ʏ�܂ł̋���
  if( cos_theta > 0.0f )
  {
    f32 center_distance = (is_near)?(this->GetNearClip()):(this->GetFarClip());
    f32 current_distance = center_distance / cos_theta;
    gfl::math::VEC3 temp;
    gfl::math::VEC3Add( world_point, gfl::math::VEC3Scale(&temp, &current_world_ray_direction, current_distance), &current_world_ray_origin );
  }
  else
  {
    GFL_ASSERT_MSG(0, "�p�x���ُ�ł��B\n");
    // ���[���h���W�n�̔������̎n�_���W(�J�����̈ʒuGetCameraPosition�Ɠ����l�������܂�)�ɂł����Ă���
    *world_point = current_world_ray_origin;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ���ʃX�N���[�����W�n�̓_�����[���h���W�n�̓_(�j�A�ʏ�or�t�@�[�ʏ�)�ɕϊ�����
 *
 *  @param[out]   world_point           ���[���h���W�n�̓_�̈ʒu���W
 *  @param[in]    is_near               true�̂Ƃ��j�A�ʏ�Afalse�̂Ƃ��t�@�[�ʏ�
 *  @param[in]    upper_screen_point_x  ���ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(400-1,240-1))
 *  @param[in]    upper_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B 
 *  @param[in]    render_target         �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::ConvertUpperScreenPointToWorldPoint(
    gfl::math::VEC3*              world_point,
    const b32                     is_near,
    const s32                     upper_screen_point_x,
    const s32                     upper_screen_point_y,
    const gfl::grp::RenderTarget& upper_render_target
) const
{
  ConvertScreenPointToWorldPoint(
      world_point,
      is_near,
      upper_screen_point_x/(static_cast<f32>(400-1)),
      upper_screen_point_y/(static_cast<f32>(240-1)),
      upper_render_target
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ����ʃX�N���[�����W�n�̓_�����[���h���W�n�̓_(�j�A�ʏ�or�t�@�[�ʏ�)�ɕϊ�����
 *
 *  @param[out]   world_point           ���[���h���W�n�̓_�̈ʒu���W
 *  @param[in]    is_near               true�̂Ƃ��j�A�ʏ�Afalse�̂Ƃ��t�@�[�ʏ�
 *  @param[in]    lower_screen_point_x  ����ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(320-1,240-1))
 *  @param[in]    lower_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B 
 *  @param[in]    render_target         �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::ConvertLowerScreenPointToWorldPoint(
    gfl::math::VEC3*              world_point,
    const b32                     is_near,
    const s32                     lower_screen_point_x,
    const s32                     lower_screen_point_y,
    const gfl::grp::RenderTarget& lower_render_target
) const
{
  ConvertScreenPointToWorldPoint(
      world_point,
      is_near,
      lower_screen_point_x/(static_cast<f32>(320-1)),
      lower_screen_point_y/(static_cast<f32>(240-1)),
      lower_render_target
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ���[���h���W�n�̓_���X�N���[�����W�n�̓_�ɕϊ�����
 *
 *  @param[out]    screen_point_x        �X�N���[�����W�n�̓_�̈ʒu���W(���オ(0.0f,0.0f)�A�E����(1.0f,1.0f))
 *  @param[out]    screen_point_y        
 *  @param[in]     world_point           ���[���h���W�n�̓_�̈ʒu���W
 *  @param[in]     render_target         �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::ConvertWorldPointToScreenPoint(
    f32*                          screen_point_x,
    f32*                          screen_point_y,
    const gfl::math::VEC3&        world_point,
    const gfl::grp::RenderTarget& render_target
) const
{
  gfl::math::VEC4 world_point_4(world_point.x, world_point.y, world_point.z, 1.0f);
  //gfl::math::VEC4 world_point_4(world_point.x, world_point.y, world_point.z, 0.0f);
      // w��0.0f���Ɓu-1.0f<= <=1.0f�̃X�N���[�����W�v�̂Ƃ���̌v�Z���ʂ��A
      // �u-1.3f���炢<= <=1.3f���炢�̃X�N���[�����W�v�ł��A�X�N���[�����W�̕������t�]���A�J�����̋����ɂ���āu-0.011f���炢<= <=0.011f���炢�̃X�N���[�����W�v�Ƃ����v�Z���ʂɂȂ����������B
      //

  gfl::math::MTX34 view_matrix_34;
  gfl::math::MTX44 projection_matrix;
  GetViewAndProjectionMatrix(&view_matrix_34, &projection_matrix);

#if 1
  // ������#if�͂ǂ����ł��������ʂ�����
  gfl::math::MTX44 view_matrix;
  gfl::math::MTX44Identity(&view_matrix);
  for(u32 i=0; i<3; ++i)
  {
    for(u32 j=0; j<4; ++j)
    {
      view_matrix.m[i][j] = view_matrix_34.m[i][j];
    }
  }
#else
  gfl::math::MTX44 view_matrix;
  gfl::math::MTX44Identity(&view_matrix);
  // �������~�����̂�3x3�v�f�̂݃R�s�[
  for(u32 i=0; i<3; ++i)
  {
    for(u32 j=0; j<3; ++j)
    {
      view_matrix.m[i][j] = view_matrix_34.m[i][j];
    }
  }
#endif

  gfl::math::MTX44 view_projection_matrix;
  gfl::math::MTX44Mult(&view_projection_matrix, &projection_matrix, &view_matrix);

  gfl::math::VEC4 screen_point_4;
  nn::math::VEC4Transform(&screen_point_4, &view_projection_matrix, &world_point_4);

  // -1.0f<= <=1.0f�̃X�N���[�����W(�r���[�|�[�g�Ŕ͈͂𐧌����Ă��Ȃ������_�[�^�[�Q�b�g�S�̂̃X�N���[�����W)
  gfl::math::VEC2 screen_point_2;
  switch( this->GetPivotDirection() )
  {
  case gfl::math::PIVOT_NONE:
    {
      screen_point_2.x =   screen_point_4.x / screen_point_4.w;  // �c�������ւ��Ȃ�
      screen_point_2.y = - screen_point_4.y / screen_point_4.w;
    } 
    break; 
  case gfl::math::PIVOT_UPSIDE_TO_TOP:
    {
      screen_point_2.x = - screen_point_4.y / screen_point_4.w;  // �c�������ւ���
      screen_point_2.y = - screen_point_4.x / screen_point_4.w;
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "PivotDirection=%d�͖��Ή��ł��B\n", this->GetPivotDirection() );
      screen_point_2.x = - screen_point_4.y / screen_point_4.w;  // �c�������ւ���
      screen_point_2.y = - screen_point_4.x / screen_point_4.w;
    } 
    break;
  }

  // 0.0f<= <=1.0f�ɕύX����(�r���[�|�[�g�Ŕ͈͂𐧌����Ă��Ȃ������_�[�^�[�Q�b�g�S�̂̃X�N���[�����W)
  *screen_point_x = screen_point_2.x / 2.0f + 0.5f;
  *screen_point_y = screen_point_2.y / 2.0f + 0.5f;

  // �r���[�|�[�g�Ő������ꂽ�͈͂�0.0f<= <=1.0f�Ƃ����X�N���[�����W
  f32 left, top, right, bottom;
  render_target.GetViewport(&left, &top, &right, &bottom);
  GFL_ASSERT_MSG( left==0.0f&&top==0.0f, "left=%f, top=%f\n", left, top );  // ����ȊO�͖��Ή�
  s32 width  = render_target.GetWidth();
  s32 height = render_target.GetHeight();
  //*screen_point_x = (*screen_point_x) * width /(right-left);// + (width -(right-left))/(right-left);
  //*screen_point_y = (*screen_point_y) * height/(bottom-top);// + (height-(bottom-top))/(bottom-top);
  //���v��Ȃ�����������
}
//-----------------------------------------------------------------------------
/*
 *  @brief        ���[���h���W�n�̓_�����ʃX�N���[�����W�n�̓_�ɕϊ�����
 *
 *  @param[out]    upper_screen_point_x  ���ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(400-1,240-1))
 *  @param[out]    upper_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B     
 *  @param[in]     world_point           ���[���h���W�n�̓_�̈ʒu���W
 *  @param[in]     upper_render_target   �����_�[�^�[�Q�b�g(���ꂪ�x�[�X�J�����̂Ƃ��͍��ł��E�ł��ǂ���ł�����)
 */
//-----------------------------------------------------------------------------
void Camera::ConvertWorldPointToUpperScreenPoint(
    s32*                          upper_screen_point_x,
    s32*                          upper_screen_point_y,
    const gfl::math::VEC3&        world_point,
    const gfl::grp::RenderTarget& upper_render_target
) const
{
  f32 screen_point_x;
  f32 screen_point_y;

  ConvertWorldPointToScreenPoint(
      &screen_point_x,
      &screen_point_y,
      world_point,
      upper_render_target
  );

  *upper_screen_point_x = static_cast<s32>( screen_point_x * (static_cast<f32>(400-1)) );
  *upper_screen_point_y = static_cast<s32>( screen_point_y * (static_cast<f32>(240-1)) );
}
//-----------------------------------------------------------------------------
/*
 *  @brief        ���[���h���W�n�̓_������ʃX�N���[�����W�n�̓_�ɕϊ�����
 *
 *  @param[out]    lower_screen_point_x  ����ʃX�N���[�����W�n�̓_�̈ʒu���W(���オ(0,0)�A�E����(320-1,240-1))
 *  @param[out]    lower_screen_point_y  ��ʔ͈͊O���\����悤��signed�ɂ��܂����B
 *  @param[in]     world_point           ���[���h���W�n�̓_�̈ʒu���W
 *  @param[in]     lower_render_target   �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::ConvertWorldPointToLowerScreenPoint(
    s32*                          lower_screen_point_x,
    s32*                          lower_screen_point_y,
    const gfl::math::VEC3&        world_point,
    const gfl::grp::RenderTarget& lower_render_target
) const
{
  f32 screen_point_x;
  f32 screen_point_y;

  ConvertWorldPointToScreenPoint(
      &screen_point_x,
      &screen_point_y,
      world_point,
      lower_render_target
  );

  *lower_screen_point_x = static_cast<s32>( screen_point_x * (static_cast<f32>(320-1)) );
  *lower_screen_point_y = static_cast<s32>( screen_point_y * (static_cast<f32>(240-1)) );
}




//
// �A�j���[�V����
//

//-----------------------------------------------------------------------------
/*
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
void Camera::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  ResAnim*                res_anim,
  s32                     max_anim_members
)
{
#if 0
  // ���߂Ă��̊֐����Ă񂾂Ƃ��́AAnimation�𐶐�����B
  if( m_animation == NULL )
  {
    nw::gfx::Camera* nw_camera = GetNwCamera();
    nw::gfx::AnimGroup* anim_group = nw_camera->GetAnimGroup();
    GFL_ASSERT_STOP(anim_group);  // �A�j���[�V�����O���[�v������܂���B

    m_animation = GFL_NEW(allocator->GetHeapBase()) Animation(
      anim_group->GetMemberCount(),
      max_anim_members,
      false
    );

    // �A�j���[�V���������߂ēK�p����
    m_animation->Change(
      allocator,
      res_anim
    );

    nw::gfx::AnimObject* nw_anim_object = m_animation->GetNwAnimObject();
    GFL_ASSERT_STOP(nw_anim_object);

    // �A�j���[�V�����O���[�v�� AnimEvaluator �Ƀo�C���h���܂��B
    // ����ɂ��A�A�j���[�V�����Ώۃ����o�ɃA�j���[�V�����f�[�^���֘A�t�����܂��B
    bool bind_result = nw_anim_object->Bind(anim_group);
    GFL_ASSERT_STOP(bind_result);
    
    // AnimEvaluator �����f���ɐݒ肵�܂��B
    // AnimEvaluator �͈�̃��f���ɑ΂��ĕ����ݒ肷�邱�Ƃ��ł��A
    // ���̍ۂɂ́AAnimEvaluator ���� objectIndex ���w�肵�܂��B
    // �ڂ����́A PartialAnimationDemo ���Q�Ƃ��Ă��������B
    //
    // �A�j���[�V�����̑Ώۂ��ނɂ���ėp����֐����قȂ�܂��B
    // SkeletalModel::SetSkeletalAnimObject
    // Model::SetVisibilityAnimObject
    // Model::SetMaterialAnimObject
    // Light::SetAnimObject
    nw_camera->SetAnimObject(nw_anim_object);
  }
  else
  {
    // �A�j���[�V�����𒼂��ɕύX����
    m_animation->Change(
      allocator,
      res_anim
    );
  }
#endif
  
  ChangeAnimation(
      allocator,
      res_anim,
      0,
      max_anim_members
  );
}

//-----------------------------------------------------------------------------
/*
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
void Camera::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  Resource*               resource,
  s32                     index_in_resource,
  s32                     max_anim_members
)
{
#if 0
  // ���߂Ă��̊֐����Ă񂾂Ƃ��́AAnimation�𐶐�����B
  if( m_animation == NULL )
  {
    nw::gfx::Camera* nw_camera = GetNwCamera();
    nw::gfx::AnimGroup* anim_group = nw_camera->GetAnimGroup();
    GFL_ASSERT_STOP(anim_group);  // �A�j���[�V�����O���[�v������܂���B

    m_animation = GFL_NEW(allocator->GetHeapBase()) Animation(
      anim_group->GetMemberCount(),
      max_anim_members,
      false
    );

    // �A�j���[�V���������߂ēK�p����
    m_animation->Change(
      allocator,
      resource,
      Resource::ANIMATION_RESOURCE_TYPE_CAMERA,
      index_in_resource
    );

    nw::gfx::AnimObject* nw_anim_object = m_animation->GetNwAnimObject();
    GFL_ASSERT_STOP(nw_anim_object);

    // �A�j���[�V�����O���[�v�� AnimEvaluator �Ƀo�C���h���܂��B
    // ����ɂ��A�A�j���[�V�����Ώۃ����o�ɃA�j���[�V�����f�[�^���֘A�t�����܂��B
    bool bind_result = nw_anim_object->Bind(anim_group);
    GFL_ASSERT_STOP(bind_result);
    
    // AnimEvaluator �����f���ɐݒ肵�܂��B
    // AnimEvaluator �͈�̃��f���ɑ΂��ĕ����ݒ肷�邱�Ƃ��ł��A
    // ���̍ۂɂ́AAnimEvaluator ���� objectIndex ���w�肵�܂��B
    // �ڂ����́A PartialAnimationDemo ���Q�Ƃ��Ă��������B
    //
    // �A�j���[�V�����̑Ώۂ��ނɂ���ėp����֐����قȂ�܂��B
    // SkeletalModel::SetSkeletalAnimObject
    // Model::SetVisibilityAnimObject
    // Model::SetMaterialAnimObject
    // Light::SetAnimObject
    nw_camera->SetAnimObject(nw_anim_object);
  }
  else
  {
    // �A�j���[�V�����𒼂��ɕύX����
    m_animation->Change(
      allocator,
      resource,
      Resource::ANIMATION_RESOURCE_TYPE_CAMERA,
      index_in_resource
    );
  }
#endif

  ChangeAnimation(
    allocator,
    resource,
    index_in_resource,
    0,
    max_anim_members
  );
}

void Camera::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  ResAnim*                res_anim,
  u32                     change_frame,
  s32                     max_anim_members
)
{
  ChangeUnitAnimation(
    allocator,
    allocator,
    0,
    res_anim,
    change_frame,
    max_anim_members
  );
}

void Camera::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  Resource*               resource,
  s32                     index_in_resource,
  u32                     change_frame,
  s32                     max_anim_members
)
{
  ResAnim* res_anim_ptr = NULL;
  ResAnim  res_anim;
  if( resource )
  {
    resource->GetResAnim(Resource::ANIMATION_RESOURCE_TYPE_CAMERA, index_in_resource, &res_anim);
    res_anim_ptr = &res_anim;
  }
  ChangeAnimation(
      allocator,
      res_anim_ptr,
      change_frame,
      max_anim_members
  );
}


//-----------------------------------------------------------------------------
/*
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
 *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 */
//-----------------------------------------------------------------------------
void Camera::ChangeUnitAnimation(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  u32                     unit_index,
  ResAnim*                res_anim,
  u32                     change_frame,
  s32                     max_anim_members
)
{
  if(m_animation_slot_num == 0 )
  {
    GFL_ASSERT_MSG(0, "NOT Animatable Camera\n");
    return;
  }

  if( !m_animation_manager )
  {
    m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) AnimationManager;
    m_animation_manager->UseOwn_Create(
        heap_allocator,
        device_allocator,
        this,
        m_animation_slot_num,  // 1�����Ȃ�
        m_animation_blend_unit_num,
        m_animation_num_in_unit
    );
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_ChangeAnimation(
      heap_allocator,
      device_allocator,
      0,  // 1�����Ȃ��̂ŃC���f�b�N�X��0
      unit_index,
      res_anim,
      change_frame,
      max_anim_members,
      false
    );
  }
}

void Camera::ChangeUnitAnimation(
  gfl::heap::NwAllocator*          heap_allocator,
  gfl::heap::NwAllocator*          device_allocator,
  u32                              unit_index,
  Resource*                        resource,
  s32                              index_in_resource,
  u32                              change_frame,
  s32                              max_anim_members
)
{
  ResAnim* res_anim_ptr = NULL;
  ResAnim  res_anim;
  if( resource )
  {
    resource->GetResAnim(Resource::ANIMATION_RESOURCE_TYPE_CAMERA, index_in_resource, &res_anim);
    res_anim_ptr = &res_anim;
  }
  ChangeUnitAnimation(
      heap_allocator,
      device_allocator,
      unit_index,
      res_anim_ptr,
      change_frame,
      max_anim_members
  );
}




//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetFrame(0, frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Camera::GetAnimationFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return 0.0f;
  }
  return m_animation->GetFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return 0.0f; 
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
 *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
 *
 *  @param[in]     step_frame      �X�V�t���[��
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationStepFrame(f32 step_frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetStepFrame(step_frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStepFrame(0, step_frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
 *
 *  @retval        �X�V�t���[��
 */
//-----------------------------------------------------------------------------
f32 Camera::GetAnimationStepFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return 0.0f;
  }
  return m_animation->GetStepFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetStepFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return 0.0f; 
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationStartFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetStepFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStartFrame(0, frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Camera::GetAnimationStartFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return 0.0f;
  }
  return m_animation->GetStartFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetStartFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return 0.0f; 
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationEndFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetEndFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetEndFrame(0, frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̏I���t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Camera::GetAnimationEndFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return 0.0f;
  }
  return m_animation->GetEndFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetEndFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
 *
 *  @param[in]     start_frame     �J�n�t���[��
 *  @param[in]     end_frame       �I���t���[��
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetStartAndEndFrame(start_frame, end_frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStartAndEndFrame(0, start_frame, end_frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
 *
 *  @retval        bool    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
 */
//-----------------------------------------------------------------------------
bool Camera::IsAnimationFrameEndFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return false;
  }
  return m_animation->IsFrameEndFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->IsFrameEndFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
 *
 *  @retval        bool            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
 */
//-----------------------------------------------------------------------------
bool Camera::IsAnimationLoop(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return false;
  }
  return m_animation->IsLoop();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->IsLoop(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return false; 
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
 *
 *  @param[in]     loop            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationLoop(bool loop)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetLoop(loop);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetLoop(0, loop);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �t���[������
 */
//-----------------------------------------------------------------------------
void Camera::SetUnitFrame(u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitFrame(0, unit_index, frame);
  }
}
f32  Camera::GetUnitFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitFrame(0, unit_index);
  }
  return 0.0f;
}
void Camera::SetUnitStepFrame(u32 unit_index, f32 step_frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStepFrame(0, unit_index, step_frame);
  }
}
f32  Camera::GetUnitStepFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitStepFrame(0, unit_index);
  }
  return 0.0f;
}
void Camera::SetUnitStartFrame(u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStartFrame(0, unit_index, frame);
  }
}
f32  Camera::GetUnitStartFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitStartFrame(0, unit_index);
  }
  return 0.0f;
}
void Camera::SetUnitEndFrame(u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitEndFrame(0, unit_index, frame);
  }
}
f32  Camera::GetUnitEndFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitEndFrame(0, unit_index);
  }
  return 0.0f;
}
void Camera::SetUnitStartAndEndFrame(u32 unit_index, f32 start_frame, f32 end_frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStartAndEndFrame(0, unit_index, start_frame, end_frame);
  }
}
b32  Camera::IsUnitFrameEndFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_IsUnitFrameEndFrame(0, unit_index);
  }
  return false;
}
b32  Camera::IsUnitLoop(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_IsUnitLoop(0, unit_index);
  }
  return false;
}
void Camera::SetUnitLoop(u32 unit_index, b32 loop)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitLoop(0, unit_index, loop);
  }
}


//-----------------------------------------------------------------------------
/*
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
void Camera::BlendUnitAnimation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    AnimationSlot::Mode        mode,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  if(m_animation_slot_num == 0 )
  {
    GFL_ASSERT_MSG(0, "NOT Animatable Camera\n");
    return;
  }

  if( !m_animation_manager )
  {
    m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) AnimationManager;
    m_animation_manager->UseOwn_Create(
        heap_allocator,
        device_allocator,
        this,
        m_animation_slot_num,  // 1�����Ȃ�
        m_animation_blend_unit_num,
        m_animation_num_in_unit
    );
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_BlendAnimation(
      heap_allocator,
      device_allocator,
      0,  // 1�����Ȃ��̂ŃC���f�b�N�X��0
      mode,
      interpolator_ignore_no_anim_member,
      interpolator_normalization_enable,
      animation_unit_index
    );
  }
}
void Camera::BlendUnit2Animation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    AnimationSlot::Mode        mode,
    f32                        weight0,
    f32                        weight1,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  BlendUnitAnimation(
    heap_allocator,
    device_allocator,
    mode,
    interpolator_ignore_no_anim_member,
    interpolator_normalization_enable,
    animation_unit_index
  );
  { 
    if(    mode == AnimationSlot::MODE_BLEND_INTERPOLATOR
        || mode == AnimationSlot::MODE_BLEND_ADDER )
    {
      SetUnitBlendWeight(0, weight0);
      SetUnitBlendWeight(1, weight1);
    }
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �u�����h�d��
 */
//-----------------------------------------------------------------------------
void Camera::SetUnitBlendWeight(u32 unit_index, f32 weight)
{
  if( m_animation_manager )
  {
    m_animation_manager->SetBlendWeight(0, unit_index, weight);
  }
}
f32  Camera::GetUnitBlendWeight(u32 unit_index)
{
  if( m_animation_manager )
  {
    return m_animation_manager->GetBlendWeight(0, unit_index);
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ResAnim::Type�̃A�j���[�V�����ŕύX�����|�[�Y�����Z�b�g����
 *
 *  �A�j���[�V���������蓖�Ă��Ă���Ƃ��́A���̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ��B
 *  �A�j���[�V���������蓖�Ă��Ă��Ȃ��Ƃ��́A�S�Ẵ����o�����Z�b�g�����B
 */
//-----------------------------------------------------------------------------
void Camera::ResetAnimationPose(void)
{
  if( m_animation_manager )
  {
    m_animation_manager->Reset(ResAnim::TYPE_CAMERA);
  }
}




//
// ���[�e�B���e�B�֐�
//

//-----------------------------------------------------------------------------
/*
 *  @brief        �r���[�A�b�v�f�[�^ ���[�e�B���e�B�֐�
 */
//-----------------------------------------------------------------------------
void Camera::GetUpVector(gfl::math::VEC3* up_vector) const
{
  ViewMode view_mode = GetViewMode();
  if( view_mode == VIEW_MODE_LOOK_AT )
  {
    *up_vector = GetNwCamera()->GetUpwardVector();
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
}
void Camera::SetUpVector(const gfl::math::VEC3& up_vector)
{
  ViewMode view_mode = GetViewMode();
  if( view_mode == VIEW_MODE_LOOK_AT )
  {
    GetNwCamera()->SetUpwardVector(up_vector);
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
}
f32  Camera::GetTwist(void) const  // [radian]
{
  f32 twist;
  ViewMode view_mode = GetViewMode();
  if( view_mode == VIEW_MODE_AIM )
  {
    twist = GetNwCamera()->GetTwist();
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
  return twist;
}
void Camera::SetTwist(f32 twist)   // [radian]
{
  ViewMode view_mode = GetViewMode();
  if( view_mode == VIEW_MODE_AIM )
  {
    GetNwCamera()->SetTwist(twist);
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief        �v���W�F�N�V�����A�b�v�f�[�^ ���[�e�B���e�B�֐�
 */
//-----------------------------------------------------------------------------
f32  Camera::GetFovy(void) const  // [radian]
{
  f32 fovy;
  ProjectionMode projection_mode = GetProjectionMode();
  if( projection_mode == PROJECTION_MODE_PERSPECTIVE )
  {
    f32 aspect;
    f32 near;
    f32 far;
    GetNwCamera()->GetPerspective(&fovy,&aspect,&near,&far);
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
  return fovy;
}
void Camera::SetFovy(f32 fovy)    // [radian]
{
  ProjectionMode projection_mode = GetProjectionMode();
  if( projection_mode == PROJECTION_MODE_PERSPECTIVE )
  {
    f32 curr_fovy;
    f32 aspect;
    f32 near;
    f32 far;
    GetNwCamera()->GetPerspective(&curr_fovy,&aspect,&near,&far);
    GetNwCamera()->SetPerspective(fovy,aspect,near,far);
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
}

f32  Camera::GetAspect(void) const
{
  f32 aspect = 0.0f;
  ProjectionMode projection_mode = GetProjectionMode();
  switch( projection_mode )
  {
  case PROJECTION_MODE_PERSPECTIVE:
    {
      f32 fovy;
      f32 near;
      f32 far;
      GetNwCamera()->GetPerspective(&fovy,&aspect,&near,&far);
    }
    break;
  case PROJECTION_MODE_FRUSTUM:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetFrustumWithYFlip(&left,&right,&bottom,&top,&near,&far);
#if 0
/*NintendoWare for CTR 2.2.2 �����[�X�m�[�g
2011�N12��26��
�m�[�g 52-G11: (���C�u����) Camera::GetFrustum �� GetOrtho �� API ��ύX
Camera::GetFrustum �� GetOrtho �œ���ꂽ�l�����̂܂� SetFrustum ����� SetOrtho �Őݒ肷��� top �� bottom �����]���Ă��܂����B GetFrustum �� GetOrtho ��p�~�\��Ƃ��Atop �� bottom �𔽓]�����Ɏ擾���� GetFrustumWithoutFlip �� GetOrthoWithoutFlip�A����я]���ʂ� top �� bottom �𔽓]���Ď擾���� GetFrustumWithYFlip �� GetOrthoWithYFlip ��ǉ����܂����B */
#endif
      aspect = (right - left) / (bottom - top);
    }
    break;
  case PROJECTION_MODE_ORTHO:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetOrthoWithYFlip(&left,&right,&bottom,&top,&near,&far);
#if 0
/*NintendoWare for CTR 2.2.2 �����[�X�m�[�g
2011�N12��26��
�m�[�g 52-G11: (���C�u����) Camera::GetFrustum �� GetOrtho �� API ��ύX
Camera::GetFrustum �� GetOrtho �œ���ꂽ�l�����̂܂� SetFrustum ����� SetOrtho �Őݒ肷��� top �� bottom �����]���Ă��܂����B GetFrustum �� GetOrtho ��p�~�\��Ƃ��Atop �� bottom �𔽓]�����Ɏ擾���� GetFrustumWithoutFlip �� GetOrthoWithoutFlip�A����я]���ʂ� top �� bottom �𔽓]���Ď擾���� GetFrustumWithYFlip �� GetOrthoWithYFlip ��ǉ����܂����B */
#endif
      aspect = (right - left) / (bottom - top);
    }
    break;
  }
  return aspect;
}
void Camera::SetAspect(f32 aspect)
{
  ProjectionMode projection_mode = GetProjectionMode();
  switch( projection_mode )
  {
  case PROJECTION_MODE_PERSPECTIVE:
    {
      f32 fovy;
      f32 curr_aspect;
      f32 near;
      f32 far;
      GetNwCamera()->GetPerspective(&fovy,&curr_aspect,&near,&far);
      GetNwCamera()->SetPerspective(fovy,aspect,near,far);
    }
    break;
  case PROJECTION_MODE_FRUSTUM:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetFrustumWithYFlip(&left,&right,&bottom,&top,&near,&far);
      f32 height = bottom - top;
      f32 center = left + (right - left) / 2.0f;
      f32 width = height * aspect;
      left  = center - width/2.0f;
      right = center + width/2.0f;
      GetNwCamera()->SetFrustum(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip���ɑΉ����邽�ߏ㉺���t�ɂ��Ă����B
    }
    break;
  case PROJECTION_MODE_ORTHO:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetOrthoWithYFlip(&left,&right,&bottom,&top,&near,&far);
      f32 height = bottom - top;           
      f32 center = left + (right - left) / 2.0f; 
      f32 width = height * aspect;
      left  = center - width/2.0f;
      right = center + width/2.0f;
      GetNwCamera()->SetOrtho(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip���ɑΉ����邽�ߏ㉺���t�ɂ��Ă����B
    }
    break;
  }
}
f32  Camera::GetHeight(void) const
{
  f32 height = 0.0f;
  ProjectionMode projection_mode = GetProjectionMode();
  switch( projection_mode )
  {
  case PROJECTION_MODE_PERSPECTIVE:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  case PROJECTION_MODE_FRUSTUM:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetFrustumWithYFlip(&left,&right,&bottom,&top,&near,&far);
      height = bottom - top;
    }
    break;
  case PROJECTION_MODE_ORTHO:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetOrthoWithYFlip(&left,&right,&bottom,&top,&near,&far);
      height = bottom - top;
    }
    break;
  }
  return height;
}
void Camera::SetHeight(f32 height)  // aspect��ێ����邽�߁Aheight��ύX����Ɖ������ύX����܂��B
{
  ProjectionMode projection_mode = GetProjectionMode();
  switch( projection_mode )
  {
  case PROJECTION_MODE_PERSPECTIVE:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  case PROJECTION_MODE_FRUSTUM:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetFrustumWithYFlip(&left,&right,&bottom,&top,&near,&far);
      f32 curr_height = bottom - top;
      f32 width  = right - left;    
      f32 aspect = width / curr_height;
      f32 center = top + (bottom - top) / 2.0f;
      top = center - height/2.0f;
      bottom = center + height/2.0f;
      GetNwCamera()->SetFrustum(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip���ɑΉ����邽�ߏ㉺���t�ɂ��Ă����B
      SetAspect(aspect);
    }
    break;
  case PROJECTION_MODE_ORTHO:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetOrthoWithYFlip(&left,&right,&bottom,&top,&near,&far);
      f32 curr_height = bottom - top;
      f32 width  = right - left;    
      f32 aspect = width / curr_height;
      f32 center = top + (bottom - top) / 2.0f;
      top = center - height/2.0f;
      bottom = center + height/2.0f;
      GetNwCamera()->SetOrtho(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip���ɑΉ����邽�ߏ㉺���t�ɂ��Ă����B
      SetAspect(aspect);
    }
    break;
  }
}
void Camera::GetCenter(gfl::math::VEC2* center) const
{
  ProjectionMode projection_mode = GetProjectionMode();
  switch( projection_mode )
  {
  case PROJECTION_MODE_PERSPECTIVE:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  case PROJECTION_MODE_FRUSTUM:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetFrustumWithYFlip(&left,&right,&bottom,&top,&near,&far);
      center->x = left + (right - left) / 2.0f;
      center->y = top + (bottom - top) / 2.0f;
    }
    break;
  case PROJECTION_MODE_ORTHO:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetOrthoWithYFlip(&left,&right,&bottom,&top,&near,&far);
      center->x = left + (right - left) / 2.0f;
      center->y = top + (bottom - top) / 2.0f;
    }
    break;
  }
}
void Camera::SetCenter(const gfl::math::VEC2& center)
{
  ProjectionMode projection_mode = GetProjectionMode();
  switch( projection_mode )
  {
  case PROJECTION_MODE_PERSPECTIVE:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  case PROJECTION_MODE_FRUSTUM:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetFrustumWithYFlip(&left,&right,&bottom,&top,&near,&far);
      f32 width  = right - left;
      f32 height = bottom - top;
      left  = center.x - width/2.0f;
      right = center.x + width/2.0f;
      top    = center.y - height/2.0f;
      bottom = center.y + height/2.0f;
      GetNwCamera()->SetFrustum(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip���ɑΉ����邽�ߏ㉺���t�ɂ��Ă����B
    }
    break;
  case PROJECTION_MODE_ORTHO:
    {
      f32 left;
      f32 right;
      f32 bottom;
      f32 top;
      f32 near;
      f32 far;
      GetNwCamera()->GetOrthoWithYFlip(&left,&right,&bottom,&top,&near,&far);
      f32 width  = right - left;  
      f32 height = bottom - top;  
      left  = center.x - width/2.0f;
      right = center.x + width/2.0f;
      top    = center.y - height/2.0f;
      bottom = center.y + height/2.0f;
      GetNwCamera()->SetOrtho(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip���ɑΉ����邽�ߏ㉺���t�ɂ��Ă����B

    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ��ʂ̏������ݒ肷��
 *
 *  @param[in]    pivot_direction    ��ʂ̏����
 */
//-----------------------------------------------------------------------------
void Camera::SetPivotDirection( gfl::math::PivotDirection pivot_direction )
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  nw::gfx::CameraProjectionUpdater* projection_updater = nw_camera->GetProjectionUpdater();
  projection_updater->SetPivotDirection( static_cast<nw::math::PivotDirection>(pivot_direction) );
}
//-----------------------------------------------------------------------------
/*
 *  @brief        ��ʂ̏�������擾����
 *
 *  @retval    gfl::math::PivotDirection    ��ʂ̏����
 */
//-----------------------------------------------------------------------------
gfl::math::PivotDirection  Camera::GetPivotDirection(void) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  nw::gfx::CameraProjectionUpdater* projection_updater = nw_camera->GetProjectionUpdater();
  return  static_cast<gfl::math::PivotDirection>( projection_updater->GetPivotDirection() );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �j�A�N���b�v / �t�@�[�N���b�v
 */
//-----------------------------------------------------------------------------
void Camera::SetNearClip( f32 near_clip )
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  nw_camera->SetNear(near_clip);  // ���̊֐��̓p�����[�^���ȒP�Ɏ擾���邽�߂̃��[�e�B���e�B�֐��ł��B
                                  // �Ƃ̂��ƂȂ̂ŁA�g���O�ɃJ�����̃^�C�v���`�F�b�N�����ق������������B
}
f32  Camera::GetNearClip( void ) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  return nw_camera->GetNear();  // ���̊֐��̓p�����[�^���ȒP�Ɏ擾���邽�߂̃��[�e�B���e�B�֐��ł��B
                                // �Ƃ̂��ƂȂ̂ŁA�g���O�ɃJ�����̃^�C�v���`�F�b�N�����ق������������B
}
void Camera::SetFarClip( f32 far_clip )
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  nw_camera->SetFar(far_clip);  // ���̊֐��̓p�����[�^���ȒP�Ɏ擾���邽�߂̃��[�e�B���e�B�֐��ł��B
                                // �Ƃ̂��ƂȂ̂ŁA�g���O�ɃJ�����̃^�C�v���`�F�b�N�����ق������������B
}
f32  Camera::GetFarClip( void ) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  return nw_camera->GetFar();  // ���̊֐��̓p�����[�^���ȒP�Ɏ擾���邽�߂̃��[�e�B���e�B�֐��ł��B
                               // �Ƃ̂��ƂȂ̂ŁA�g���O�ɃJ�����̃^�C�v���`�F�b�N�����ق������������B
}




//
// WScale
//

//-----------------------------------------------------------------------------
/*
 *  @brief        WScale
 */
//-----------------------------------------------------------------------------
f32  Camera::GetWScale(void) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  return nw_camera->GetWScale();
}
void Camera::SetWScale(f32 w_scale)
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  nw_camera->SetWScale(w_scale);
}




//
// �擾 / �ݒ�
//

//-----------------------------------------------------------------------------
/*
 *  @brief         �r���[���[�h���擾����
 */
//-----------------------------------------------------------------------------
Camera::ViewMode Camera::GetViewMode(void) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();

  nw::gfx::ResCameraViewUpdater resViewUpdater = nw_camera->GetViewUpdater()->GetResource();

  nw::gfx::ResAimTargetViewUpdater resAimTargetViewUpdater =
    nw::gfx::ResDynamicCast<nw::gfx::ResAimTargetViewUpdater>(resViewUpdater);
  if(resAimTargetViewUpdater.IsValid())
  {
    return VIEW_MODE_AIM;
  }

  nw::gfx::ResLookAtTargetViewUpdater resLookAtTargetViewUpdater =
    nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(resViewUpdater);
  if(resLookAtTargetViewUpdater.IsValid())
  {
    return VIEW_MODE_LOOK_AT;
  }

  nw::gfx::ResRotateViewUpdater resRotateViewUpdater =
    nw::gfx::ResDynamicCast<nw::gfx::ResRotateViewUpdater>(resViewUpdater);
  if(resRotateViewUpdater.IsValid())
  {
    return VIEW_MODE_ROTATE;
  }

  GFL_ASSERT_STOP(0);  
  return VIEW_MODE_LOOK_AT;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �ˉe���[�h���擾����
 */
//-----------------------------------------------------------------------------
Camera::ProjectionMode Camera::GetProjectionMode(void) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();

  nw::gfx::ResCameraProjectionUpdater resProjectionUpdater = nw_camera->GetProjectionUpdater()->GetResource();

  nw::gfx::ResPerspectiveProjectionUpdater resPerspectiveProjectionUpdater =
    nw::gfx::ResDynamicCast<nw::gfx::ResPerspectiveProjectionUpdater>(resProjectionUpdater);
  if(resPerspectiveProjectionUpdater.IsValid())
  {
    return PROJECTION_MODE_PERSPECTIVE;
  }

  nw::gfx::ResFrustumProjectionUpdater resFrustumProjectionUpdater =
    nw::gfx::ResDynamicCast<nw::gfx::ResFrustumProjectionUpdater>(resProjectionUpdater);
  if(resFrustumProjectionUpdater.IsValid())
  {
    return PROJECTION_MODE_FRUSTUM;
  }

  nw::gfx::ResOrthoProjectionUpdater resOrthoProjectionUpdater =
    nw::gfx::ResDynamicCast<nw::gfx::ResOrthoProjectionUpdater>(resProjectionUpdater);
  if(resOrthoProjectionUpdater.IsValid())
  {
    return PROJECTION_MODE_ORTHO;
  }

  GFL_ASSERT_STOP(0); 
  return PROJECTION_MODE_PERSPECTIVE;
}




//---------------------------------------------------------------------------
/**
 *           �萔��`
 */
//---------------------------------------------------------------------------
const gfl::math::VEC3  Camera::CAMERA_POSITION_DEFAULT    = gfl::math::VEC3(7.0f, 3.5f, -5.0f);   // �J�����ʒu�̃f�t�H���g�l
const gfl::math::VEC3  Camera::TARGET_POSITION_DEFAULT    = gfl::math::VEC3(0.0f, 0.0f, 0.0f);    // �J�����^�[�Q�b�g�̃f�t�H���g�l
const f32              Camera::NEAR_CLIP_DEFAULT          =    0.1f;                              // �j�A�N���b�v�̃f�t�H���g�l
const f32              Camera::FAR_CLIP_DEFAULT           = 1000.0f;                              // �t�@�[�N���b�v�̃f�t�H���g�l
const f32              Camera::FOVY_RADIAN_DEFAULT        = GFL_MATH_DEG_TO_RAD(37.8f);           // FOV��Y�����̃f�t�H���g�l
const gfl::math::VEC2  Camera::PROJECTION_CENTER_DEFAULT  = gfl::math::VEC2(0.0f, 0.0f);          // �J�����̓��e�ʂ̒��S�ʒu�̃f�t�H���g�l
const f32              Camera::PROJECTION_HEIGHT_DEFAULT  =    1.0f;                              // �J�����̓��e�ʂ̍����̃f�t�H���g�l



//
// NW4C�֘A static ���� / �j��
//

//-----------------------------------------------------------------------------
/*
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
nw::gfx::Camera* Camera::CreateCamera(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3&           camera_position,
    const gfl::math::VEC3&           target_position,
    f32                              near_clip,
    f32                              far_clip,
    f32                              fovy_radian,
    gfl::math::PivotDirection        pivot_direction,
    f32                              w_scale
)
{
  GFL_ASSERT_STOP(allocator);
    
  nw::gfx::LookAtTargetViewUpdater* view_updater =
      nw::gfx::LookAtTargetViewUpdater::Create(allocator);
  GFL_ASSERT_STOP(view_updater);
    
  nw::gfx::ResLookAtTargetViewUpdater res_view_updater =
      nw::gfx::ResStaticCast<nw::gfx::ResLookAtTargetViewUpdater>(view_updater->GetResource());
    
  res_view_updater.SetTargetPosition(target_position);
  res_view_updater.SetUpwardVector(0.0f,1.0f,0.0f);

  nw::gfx::PerspectiveProjectionUpdater* projection_updater =
      nw::gfx::PerspectiveProjectionUpdater::Create(allocator);
  GFL_ASSERT_STOP(projection_updater);

  projection_updater->SetPivotDirection( static_cast<nw::math::PivotDirection>(pivot_direction) );

  nw::gfx::ResPerspectiveProjectionUpdater res_projection_updater =
      nw::gfx::ResStaticCast<nw::gfx::ResPerspectiveProjectionUpdater>(
      projection_updater->GetResource());
  res_projection_updater.SetNear(near_clip);
  res_projection_updater.SetFar(far_clip);
  res_projection_updater.SetFovy(fovy_radian);

  nw::gfx::Camera* camera =
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .ViewUpdater(view_updater)
      .ProjectionUpdater(projection_updater)
      .Create(allocator);
    
  GFL_ASSERT_STOP(camera);
    
  camera->Transform().SetTranslate(camera_position);
  camera->SetWScale(w_scale);

  return camera;
}

nw::gfx::Camera*
Camera::CreateFrustumCamera(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3& cameraPosition,
    const gfl::math::VEC3& targetPosition,
    f32 nearClip,
    f32 farClip,
    const gfl::math::VEC2& projectionCenter,
    f32 projectionHeight,
    gfl::math::PivotDirection pivotDirection,
    f32 wScale
)
{
    NW_POINTER_ASSERT(allocator);
    
    nw::gfx::LookAtTargetViewUpdater* viewUpdater =
        nw::gfx::LookAtTargetViewUpdater::Create(allocator);
    NW_POINTER_ASSERT(viewUpdater);
    
    nw::gfx::ResLookAtTargetViewUpdater resViewUpdater =
        nw::gfx::ResStaticCast<nw::gfx::ResLookAtTargetViewUpdater>(viewUpdater->GetResource());
    
    resViewUpdater.SetTargetPosition(targetPosition);
    resViewUpdater.SetUpwardVector(0.0f,1.0f,0.0f);

    nw::gfx::FrustumProjectionUpdater* projectionUpdater =
        nw::gfx::FrustumProjectionUpdater::Create(allocator);
    NW_POINTER_ASSERT(projectionUpdater);

    projectionUpdater->SetPivotDirection(static_cast<nw::math::PivotDirection>(pivotDirection));

    nw::gfx::ResFrustumProjectionUpdater resProjectionUpdater =
        nw::gfx::ResStaticCast<nw::gfx::ResFrustumProjectionUpdater>(
        projectionUpdater->GetResource());
    resProjectionUpdater.SetNear(nearClip);
    resProjectionUpdater.SetFar(farClip);
    nw::gfx::ResProjectionRect rect = resProjectionUpdater.GetRect();
    rect.m_Center = projectionCenter;
    rect.m_Height = projectionHeight;
    resProjectionUpdater.SetRect(rect);

    nw::gfx::Camera* camera =
        nw::gfx::Camera::DynamicBuilder()
        .IsFixedSizeMemory(true)
        .MaxChildren(0)
        .MaxCallbacks(0)
        .ViewUpdater(viewUpdater)
        .ProjectionUpdater(projectionUpdater)
        .Create(allocator);
    
    NW_POINTER_ASSERT(camera);
    
    camera->Transform().SetTranslate(cameraPosition);
    camera->SetWScale(wScale);
    
    return camera;
}

//-----------------------------------------------------------------------------
/*
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
void Camera::CreateStereoCameras(
    nw::gfx::Camera**           pp_base_camera,
    nw::gfx::Camera**           pp_left_camera,
    nw::gfx::Camera**           pp_right_camera,
    gfl::heap::NwAllocator*     allocator,
    const gfl::math::VEC3&      camera_position,
    const gfl::math::VEC3&      target_position,
    f32                         near_clip,
    f32                         far_clip,
    f32                         fovy_radian,
    f32                         w_scale
)
{
  GFL_ASSERT_STOP(pp_base_camera);
  GFL_ASSERT_STOP(pp_left_camera);
  GFL_ASSERT_STOP(pp_right_camera);
  GFL_ASSERT_STOP(allocator);

  *pp_base_camera = CreateCamera(
      allocator,
      camera_position,
      target_position,
      near_clip,
      far_clip,
      fovy_radian,
      gfl::math::PIVOT_NONE
  );
  (*pp_base_camera)->SetWScale(w_scale);
    
  *pp_left_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
    
  GFL_ASSERT_STOP(*pp_left_camera);
    
  (*pp_left_camera)->SetWScale(w_scale);
    
  *pp_right_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
    
  GFL_ASSERT_STOP(*pp_right_camera);

  (*pp_right_camera)->SetWScale(w_scale);
}

void
Camera::CreateFrustumStereoCameras(
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
)
{
  GFL_ASSERT_STOP(pp_base_camera);
  GFL_ASSERT_STOP(pp_left_camera);
  GFL_ASSERT_STOP(pp_right_camera);
  GFL_ASSERT_STOP(allocator);

  *pp_base_camera = CreateFrustumCamera(
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
  (*pp_base_camera)->SetWScale(w_scale);
    
  *pp_left_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
    
  GFL_ASSERT_STOP(*pp_left_camera);
    
  (*pp_left_camera)->SetWScale(w_scale);
    
  *pp_right_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
    
  GFL_ASSERT_STOP(*pp_right_camera);

  (*pp_right_camera)->SetWScale(w_scale);
}




//
// NW4C�֘A static �ݒ� / �擾
//
void Camera::SetFrustum(
     nw::gfx::Camera* camera,
     f32 left,
     f32 right,
     f32 bottom,
     f32 top,
     f32 nearClip,
     f32 farClip
)
{
  camera->SetFrustum(left, right, bottom, top, nearClip, farClip);
}

void Camera::SetFrustum(
     nw::gfx::Camera* camera,
     const nw::gfx::Viewport & viewport
)
{
  camera->SetFrustum(viewport);
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̃A�X�y�N�g���ݒ肷��
 * 
 *                �w�肵�������_�[�^�[�Q�b�g�ɍ����悤�ɐݒ肳��܂��B
 *                �J�������A�^�b�`����Ă���V�[�����X�V���Ă�����s����K�v������܂��B
 * 
 *  @param[in]    camera          �ݒ肷��J����
 *  @param[in]    render_target   �A�X�y�N�g������킹�郌���_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraAspectRatio(
    nw::gfx::Camera*                camera,
    const nw::gfx::IRenderTarget*   render_target
)
{
  GFL_ASSERT_STOP(render_target);

  // �I���X�N���[���o�b�t�@�͏c�Ɖ����t�ɂȂ��Ă��邽�߁A
  // ���ƍ������t�ɂ��ăA�X�y�N�g������߂Ă��܂��B
  SetCameraAspectRatio(
      camera, 
      static_cast<f32>(render_target->GetDescription().height) / static_cast<f32>(render_target->GetDescription().width)
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̃A�X�y�N�g���ݒ肷��
 * 
 *                �J�������A�^�b�`����Ă���V�[�����X�V���Ă�����s����K�v������܂��B
 * 
 *  @param[in]    camera        �ݒ肷��J����
 *  @param[in]    aspect_ratio   �A�X�y�N�g��
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraAspectRatio(
    nw::gfx::Camera*  camera,
    f32               aspect_ratio
)
{
  GFL_ASSERT_STOP(camera);

  f32 fovy;
  f32 near;
  f32 far;
  camera->GetPerspective(&fovy, NULL, &near, &far);

  camera->SetPerspective(fovy, aspect_ratio, near, far);
    
//  camera->UpdateCameraMatrix();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu�𓾂�
 *
 *  @param[out]   camera_position    �J�����̈ʒu��Ԃ�
 */
//-----------------------------------------------------------------------------
void Camera::GetCameraPosition( gfl::math::VEC3& camera_position ) const
{
  GetCameraPosition( *GetNwCamera(), camera_position );
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̒����_�̈ʒu�𓾂�
 *
 *  @param[out]   target_position    �J�����̒����_�̈ʒu��Ԃ�
 */
//-----------------------------------------------------------------------------
void Camera::GetTargetPosition( gfl::math::VEC3& target_position ) const
{
#if 0
  GetTargetPosition( *GetNwCamera(), target_position );
#else
  ViewMode view_mode = GetViewMode();
  if(    view_mode == VIEW_MODE_AIM
      || view_mode == VIEW_MODE_LOOK_AT )
  {
    target_position = GetNwCamera()->GetTargetPosition();
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
#endif
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu�𓾂�
 *
 *  @param[out]   camera_position    �J�����̈ʒu��Ԃ�
 *  @param[out]   target_position    �J�����̒����_�̈ʒu��Ԃ�
 */
//-----------------------------------------------------------------------------
void Camera::GetCameraAndTargetPosition( gfl::math::VEC3& camera_position, gfl::math::VEC3& target_position )
{
#if 0
  GetCameraAndTargetPosition( *GetNwCamera(), camera_position, target_position );
#else
  GetCameraPosition( camera_position );
  GetTargetPosition( target_position );
#endif
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu��ݒ肷��
 *
 *  @param[in]    camera_position    �J�����̈ʒu
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraPosition( const gfl::math::VEC3& camera_position )
{
  SetCameraPosition( *GetNwCamera(), camera_position );
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̒����_�̈ʒu��ݒ肷��
 *
 *  @param[in]    target_position    �J�����̒����_�̈ʒu
 */
//-----------------------------------------------------------------------------
void Camera::SetTargetPosition( const gfl::math::VEC3& target_position )
{
#if 0
  SetTargetPosition( *GetNwCamera(), target_position );
#else
  ViewMode view_mode = GetViewMode();
  if(    view_mode == VIEW_MODE_AIM
      || view_mode == VIEW_MODE_LOOK_AT )
  {
    GetNwCamera()->SetTargetPosition(target_position);
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
#endif
}
//-----------------------------------------------------------------------------
/*
 *  @brief        �J�����̈ʒu�ƒ����_�̈ʒu��ݒ肷��
 *
 *  @param[in]    camera_position    �J�����̈ʒu
 *  @param[in]    target_position    �J�����̒����_�̈ʒu
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position )
{
#if 0
  SetCameraAndTargetPosition( *GetNwCamera(), camera_position, target_position );
#else
  SetCameraPosition(camera_position);
  SetTargetPosition(target_position);
#endif
}




//
// static ���� / �j��
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
 *
 *  @retval       Camera*  ���������J�����N���X
 *
 * �y���Ӂz�v���O�����ŃJ�������������Ƃ��̓A�j���[�V������ݒ�ł��Ȃ��B
 */
//-----------------------------------------------------------------------------
Camera* Camera::Create(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3&           camera_position,
    const gfl::math::VEC3&           target_position,
    f32                              near_clip,
    f32                              far_clip,
    f32                              fovy_radian,
    gfl::math::PivotDirection        pivot_direction,
    f32                              w_scale
)
{
  nw::gfx::Camera* nw_camera = CreateCamera(
    allocator,
    camera_position,
    target_position,
    near_clip,
    far_clip,
    fovy_radian,
    pivot_direction,
    w_scale
  );

  return  Camera::Create(
    allocator,
    nw_camera
  );
}

//-----------------------------------------------------------------------------
/*
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
void Camera::CreateStereoCameras(
    Camera**                    pp_base_camera,
    Camera**                    pp_left_camera,
    Camera**                    pp_right_camera,
    gfl::heap::NwAllocator*     allocator,
    const gfl::math::VEC3&      camera_position,
    const gfl::math::VEC3&      target_position,
    f32                         near_clip,
    f32                         far_clip,
    f32                         fovy_radian,
    f32                         w_scale
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

  *pp_base_camera = Camera::Create(
    allocator,
    nw_base_camera
  );
  *pp_left_camera = Camera::Create(
    allocator,
    nw_left_camera
  );
  *pp_right_camera = Camera::Create(
    allocator,
    nw_right_camera
  );
}




//
// private�֐�
//
nw::gfx::CameraViewUpdater* Camera::CreateAimViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     target_position,
    f32                        twist  // [radian]
)
{
    nw::gfx::AimTargetViewUpdater* viewUpdater =
        nw::gfx::AimTargetViewUpdater::Create(allocator);
    
    nw::gfx::ResAimTargetViewUpdater resViewUpdater =
        nw::gfx::ResStaticCast<nw::gfx::ResAimTargetViewUpdater>(viewUpdater->GetResource());
    
    resViewUpdater.SetTargetPosition(target_position);
    resViewUpdater.SetTwist(twist);

    return viewUpdater;
}
nw::gfx::CameraViewUpdater* Camera::CreateLookAtViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     target_position,
    const gfl::math::VEC3&     up_vector
)
{
    nw::gfx::LookAtTargetViewUpdater* viewUpdater =
        nw::gfx::LookAtTargetViewUpdater::Create(allocator);
    
    nw::gfx::ResLookAtTargetViewUpdater resViewUpdater =
        nw::gfx::ResStaticCast<nw::gfx::ResLookAtTargetViewUpdater>(viewUpdater->GetResource());
    
    resViewUpdater.SetTargetPosition(target_position);
    resViewUpdater.SetUpwardVector(up_vector);
    
    return viewUpdater;
}
nw::gfx::CameraViewUpdater* Camera::CreateRotateViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     rotate_xyz  // [radian]
)
{
    nw::gfx::RotateViewUpdater* viewUpdater =
        nw::gfx::RotateViewUpdater::Create(allocator);
    
    nw::gfx::ResRotateViewUpdater resViewUpdater =
        nw::gfx::ResStaticCast<nw::gfx::ResRotateViewUpdater>(viewUpdater->GetResource());
    
    resViewUpdater.SetViewRotate(rotate_xyz);
    
    return viewUpdater;
}
nw::gfx::CameraProjectionUpdater* Camera::CreatePerspectiveProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        fovy,  // [radian]
    f32                        aspect
)
{
    nw::gfx::PerspectiveProjectionUpdater* projectionUpdater =
        nw::gfx::PerspectiveProjectionUpdater::Create(allocator);

    nw::gfx::ResPerspectiveProjectionUpdater resProjectionUpdater =
        nw::gfx::ResStaticCast<nw::gfx::ResPerspectiveProjectionUpdater>(
        projectionUpdater->GetResource());

    resProjectionUpdater.SetFovy(fovy);
    resProjectionUpdater.SetAspectRatio(aspect);
    
    return projectionUpdater;
}
nw::gfx::CameraProjectionUpdater* Camera::CreateFrustumProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        aspect,
    f32                        height,
    const gfl::math::VEC2&     center
)
{
    nw::gfx::FrustumProjectionUpdater* projectionUpdater =
        nw::gfx::FrustumProjectionUpdater::Create(allocator);

    nw::gfx::ResFrustumProjectionUpdater resProjectionUpdater =
        nw::gfx::ResStaticCast<nw::gfx::ResFrustumProjectionUpdater>(
        projectionUpdater->GetResource());

    nw::gfx::res::ResProjectionRect rect;
    rect.Set(
        aspect,
        height,
        center );
    resProjectionUpdater.SetRect(rect);
    
    return projectionUpdater;
}
nw::gfx::CameraProjectionUpdater* Camera::CreateOrthoProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        aspect,
    f32                        height,
    const gfl::math::VEC2&     center
)
{
    nw::gfx::OrthoProjectionUpdater* projectionUpdater =
        nw::gfx::OrthoProjectionUpdater::Create(allocator);

    nw::gfx::ResOrthoProjectionUpdater resProjectionUpdater =
        nw::gfx::ResStaticCast<nw::gfx::ResOrthoProjectionUpdater>(
        projectionUpdater->GetResource());

    nw::gfx::res::ResProjectionRect rect;
    rect.Set(
        aspect,
        height,
        center );
    resProjectionUpdater.SetRect(rect);

    return projectionUpdater;
}

//-----------------------------------------------------------------------------
/*
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
Camera* Camera::Create(
    gfl::heap::NwAllocator*           allocator,
    nw::gfx::SceneNode*               nw_scene_node
)
{
  Camera* camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  camera->SetNwSceneNode(nw_scene_node);

  // �Ǝ�����
  //
  //

  return camera;
}



//----------------------------------------------------------------------------
/**
 *	@brief  LookAt��Aim���[�h���̃A���O���擾�֐�
 *
 *	@param	r   �A���O���i�[��
 *	@param  view_mode �r���[���[�h
 */
//-----------------------------------------------------------------------------
void Camera::GetAimAndTwistRotateXYZ( gfl::math::VEC3* r, ViewMode view_mode ) const
{
  // AIMorLOOK_AT�́A���g�Ōv�Z
  // Camera �� Target�������Ƃ��̃A���O����Ԃ��B
  gfl::math::VEC3 sub;
  gfl::math::VEC3 camerapos, targetpos;
  f32 xy_length;
  f32 sub_y;
  
  this->GetTargetPosition( targetpos );
  this->GetCameraPosition( camerapos );
  
  sub = targetpos - camerapos;
  sub_y = sub.y;
  sub.y = 0.0f;
  xy_length = sub.Length();
  
  
  r->x = gfl::math::Atan2Rad( sub_y, xy_length );
  r->y = gfl::math::Atan2Rad( sub.x, -sub.z );

  if( view_mode == VIEW_MODE_LOOK_AT )
  {
    gfl::math::VEC3 upward;
    this->GetUpVector( &upward );
    r->z = gfl::math::Atan2Rad(upward.x, upward.y);
  }
  else
  {
    r->z = this->GetTwist();
  }
}

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
void Camera::SetAimAndTwistRotateXYZ( f32 x, f32 y, f32 z, ViewMode view_mode )
{

  // �J�������W�͂��̂܂܂ŁA
  // �^�[�Q�b�g���W��Rotate�ɂ������ʒu�ɓ������B
  // RotateZ�͏�����ɔ��f����B
  gfl::math::VEC3 sub;
  f32 dist;
  gfl::math::VEC3 camerapos, targetpos;
  
  this->GetTargetPosition( targetpos );
  this->GetCameraPosition( camerapos );

  sub = targetpos - camerapos;
  dist = sub.Length();

  targetpos = camerapos;
  targetpos.x += gfl::math::CosRad(x) * gfl::math::SinRad(y) * dist;
  targetpos.z -= gfl::math::CosRad(x) * gfl::math::CosRad(y) * dist;
  targetpos.y += gfl::math::SinRad(x) * dist;

  // �^�[�Q�b�gPosition���X�V
  this->SetTargetPosition(targetpos);

  // �J������������X�V
  if( view_mode == VIEW_MODE_LOOK_AT ){
    gfl::math::VEC3 upward;

    upward.z = 0;
    upward.x = gfl::math::SinRad(z);
    upward.y = gfl::math::CosRad(z);

    this->SetUpVector( upward );
    
  }else{

    this->SetTwist( z );
  }
}



}  // namespace g3d
}  // namespace grp
}  // namespace gfl

