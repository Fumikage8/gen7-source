//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dCamera.cpp
 *	@brief  3Dカメラ
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
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dカメラクラス
//=====================================

//
// ResourceDescriptionの初期化関数
//

// デフォルト値を設定する
void Camera::InitializeResourceDescriptionForDefault(
    ResourceDescription*  description
)
{
  description->animation_enable = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;
}




//
// Descriptionの初期化関数
//

// Descriptionの初期化関数
// デフォルト値を設定する
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

// Descriptionの初期化関数
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
// コンストラクタ / デストラクタ
//

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
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
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
Camera::~Camera()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetCameraManager()->RemoveObject(this);
}




//
// 生成 / 破棄
//

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 *
 *  @param[in]     allocator          アロケータ
 *  @param[in]     device_allocator   デバイスメモリのアロケータ
 *  @param[in]     resource           リソース
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @param[in]     description        リソースからカメラを生成するときの設定内容
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
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
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
      (m_animation_slot_num>0)?(true):(false),  // ここがfalseだとnw::gfx::AnimGroupが取得できない  // これを外部から設定できるようにするか、アニメーションなしカメラクラスをつくるか
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      m_animation_slot_num
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  //m_animation_manager = GFL_NEW(allocator->GetHeapBase()) AnimationManager;
  //m_animation_manager->NoSlot_Create(
  //    allocator,
  //    allocator,
  //    this
  //);
  //↑ここで生成すると、後にフレーム制御関数を呼び出したときにm_animation_managerがないときがあった。
  //ここを通っておらず生成されていないのかも。ここを通らずにアニメーションを付けられるとは思えないのだが。
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 *
 *  @param[in]     description    
 *
 * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
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

  // ViewMode生成
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
      GFL_ASSERT_STOP_MSG(0, "projection_mode=%dが不正のため、カメラを生成できません。\n", l_description.projection_mode);
      break;
  }
  
  GFL_ASSERT_STOP_MSG(projectionUpdater, "projectionUpdaterがNULLのため、カメラを生成できません。\n");

  projectionUpdater->SetPivotDirection( static_cast<nw::math::PivotDirection>(l_description.pivot) );
  projectionUpdater->GetResource().SetNear( l_description.near );
  projectionUpdater->GetResource().SetFar( l_description.far );

  // カメラ
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

  m_animation_slot_num = (l_description.animation_enable)?(1):(0);  // 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
                                                                    // だから1にしても無駄です。
                                                                    // nw::gfx::Camera::GetAnimGroup  カメラを DynamicBuilder で生成した場合は NULL を返します。 その場合、アニメーションは設定できません。
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;

  SetNwSceneNode(camera);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void Camera::Destroy(void)
{
  // 独自処理 ここから
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
  // 独自処理 ここまで

  SceneNode::Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         計算処理
 */
//-----------------------------------------------------------------------------
void Camera::Calculate(void)
{
  // アニメーションマネージャ
  if(m_animation_manager)
  {
    m_animation_manager->Calculate();
  }
}




//
// トランスフォーム
//

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの位置を得る
 *
 *  @param[in]    nw_camera          カメラ
 *  @param[out]   camera_position    カメラの位置を返す
 */
//-----------------------------------------------------------------------------
void Camera::GetCameraPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& camera_position )
{
  nw_camera.Transform().GetTranslate(&camera_position);
}
//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの注視点の位置を得る
 *
 *  @param[in]    nw_camera          カメラ
 *  @param[out]   target_position    カメラの注視点の位置を返す
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
 *  @brief        カメラの位置と注視点の位置を得る
 *
 *  @param[in]    nw_camera          カメラ
 *  @param[out]   camera_position    カメラの位置を返す
 *  @param[out]   target_position    カメラの注視点の位置を返す
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
 *  @brief        カメラの位置を設定する
 *
 *  @param[in]    nw_camera          カメラ
 *  @param[out]   camera_position    カメラの位置
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& camera_position )
{
  nw_camera.Transform().SetTranslate(camera_position);
}
//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの注視点の位置を設定する
 *
 *  @param[in]    nw_camera          カメラ
 *  @param[out]   target_position    カメラの注視点の位置
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
 *  @brief        カメラの位置と注視点の位置を設定する
 *
 *  @param[in]    nw_camera          カメラ
 *  @param[in]    camera_position    カメラの位置
 *  @param[in]    target_position    カメラの注視点の位置
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
 *  @brief        回転カメラの角度
 *                単位はラジアン。
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
 *  @brief         ワールドマトリクスを取得する
 *
 *  @param[out]    matrix          取得したマトリクスを格納して返す
 *
 *  @retval        bool            正常に取得できたらtrue
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
 *  @brief         ワールドマトリクスを設定する
 *                 SceneのCalculateの後でないと反映されません。
 *
 *  @param[in]     matrix          設定するマトリクス
 *
 *  @retval        bool            正常に設定できたらtrue
 */
//-----------------------------------------------------------------------------
bool Camera::SetWorldMatrix(const gfl::math::MTX34& matrix)
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  if( nw_camera )
  {
    gfl::math::MTX34& mtx = nw_camera->WorldMatrix();
    mtx = matrix;
    
    // ワールド行列を更新したので、逆行列のキャッシュを無効化します。
    nw_camera->InvalidateInverseWorldMatrix();

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  マトリックスのニア、ファー値を変更
 *	@note   使用後、 SetMatrixNearFarDefault で必ず元に戻すこと。
 *
 *	@param	f32 near ニア値
 *	@param	f32 far  ファー値
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
  // プロジェクションマトリクスを直接変更し、UpdateCameraMatrixは呼ばないようにしている。

  // 参考
  // https://ntsc.nintendo.co.jp/twl/twl_mancollection-DL/Packages/TWL_SDK/TwlSDK/man/ja_JP/fx/mtx/MTX_Perspective.html
  // TWLマニュアル大全集
  // TWL-SDK
  // 関数リファレンス
  // MTX
  // 4x4行列計算
  // 固定小数関連 API 機能一覧
  // 4x4行列計算
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
のとき

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
 *	@brief  マトリックスのニア、ファーを内部で保持している値に戻す
 */
//-----------------------------------------------------------------------------
void Camera::SetMatrixNearFarDefault( void )
{
  f32 near = GetNearClip();
  f32 far = GetFarClip();
  
  SetMatrixNearFar( near, far );
}




// 
// 変換
//

//-----------------------------------------------------------------------------
/*
 *  @brief         grpカメラの内容をg3dカメラに反映する
 *
 *  @param[in]     grp_camera    grpカメラ
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

        // 気を付けて使えば
        // nw_camera->SetPerspective(projection_param.perspective.fovy, projection_param.perspective.aspect, projection_param.near, projection_param.far);
        // でもよさそう。
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
 *  @brief         g3dカメラの内容をgrpカメラに反映する
 *
 *  @param[out]    grp_camera    grpカメラ
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

        // 気を付けて使えば
        // nw_camera->SetPerspective(projection_param.perspective.fovy, projection_param.perspective.aspect, projection_param.near, projection_param.far);
        // でもよさそう。
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
// カメラマトリクス
//

//-----------------------------------------------------------------------------
/*
 *  @brief        ビューマトリクスとプロジェクションマトリクスを更新する
 */
//-----------------------------------------------------------------------------
void Camera::UpdateCameraMatrix(void)
{
  GetNwCamera()->UpdateCameraMatrix();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ビューマトリクスを得る
 *
 *  @param[out]    view_matrix    ビューマトリクス
 */
//-----------------------------------------------------------------------------
void Camera::GetViewMatrix( gfl::math::MTX34* view_matrix ) const
{
  *view_matrix = GetNwCamera()->ViewMatrix();
}
//-----------------------------------------------------------------------------
/*
 *  @brief        ビューマトリクスを設定する
 *                【注意】この後にUpdateCameraMatrixを行うと、UpdateCameraMatrixで計算した値で上書きされてしまいます。
 *
 *  @param[in]    view_matrix     ビューマトリクス
 *  @param[in]    update_matrix   ビューマトリクスの逆行列を計算し直すときtrue
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
    GFL_ASSERT_MSG(exist, "ビューマトリクスの逆行列が存在しません。\n");  //@check
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         プロジェクョンマトリクスを得る
 *
 *  @param[out]    projection_matrix    プロジェクションマトリクス
 */
//-----------------------------------------------------------------------------
void Camera::GetProjectionMatrix( gfl::math::MTX44* projection_matrix ) const
{
  *projection_matrix = GetNwCamera()->ProjectionMatrix();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         プロジェクョンマトリクスを設定する
 *                【注意】この後にUpdateCameraMatrixを行うと、UpdateCameraMatrixで計算した値で上書きされてしまいます。
 *
 *  @param[in]    projection_matrix   プロジェクションマトリクス
 *  @param[in]    update_matrix       プロジェクションマトリクスの逆行列を計算し直すときtrue
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
    GFL_ASSERT_MSG(exist, "プロジェクションマトリクスの逆行列が存在しません。\n");  //@check
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ビューマトリクスとプロジェクョンマトリクスを得る
 *
 *  @param[out]    view_matrix          ビューマトリクス
 *  @param[out]    projection_matrix    プロジェクションマトリクス
 */
//-----------------------------------------------------------------------------
void Camera::GetViewAndProjectionMatrix( gfl::math::MTX34* view_matrix, gfl::math::MTX44* projection_matrix ) const
{
  GetViewMatrix( view_matrix );
  GetProjectionMatrix( projection_matrix );
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ビューマトリクスとプロジェクョンマトリクスを設定する
 *                【注意】この後にUpdateCameraMatrixを行うと、UpdateCameraMatrixで計算した値で上書きされてしまいます。
 *
 *  @param[in]    view_matrix         ビューマトリクス
 *  @param[in]    projection_matrix   プロジェクションマトリクス
 *  @param[in]    update_matrix       ビューマトリクスとプロジェクションマトリクスの逆行列を計算し直すときtrue
 */
//-----------------------------------------------------------------------------
void Camera::SetViewAndProjectionMatrix( const gfl::math::MTX34& view_matrix, const gfl::math::MTX44& projection_matrix, const b32 update_matrix )
{
  SetViewMatrix( view_matrix, update_matrix );
  SetProjectionMatrix( projection_matrix, update_matrix );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        スクリーン座標系の点をワールド座標系の半直線(カメラの位置からスクリーンの点に飛ばしたレイ)に変換する
 *
 *  @param[out]   world_ray_direction   ワールド座標系の半直線の方向ベクトル(大きさ1)
 *  @param[out]   world_ray_origin      ワールド座標系の半直線の始点座標(カメラの位置GetCameraPositionと同じ値が得られます)
 *  @param[in]    screen_point_x        スクリーン座標系の点の位置座標(左上が(0.0f,0.0f)、右下が(1.0f,1.0f))
 *  @param[in]    screen_point_y        
 *  @param[in]    render_target         レンダーターゲット
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
  // (screen_point_x, screen_point_y)は、ビューポートで制限された範囲を0.0f<= <=1.0fとしたスクリーン座標
  f32 left, top, right, bottom;
  render_target.GetViewport(&left, &top, &right, &bottom);
  GFL_ASSERT_MSG( left==0.0f&&top==0.0f, "left=%f, top=%f\n", left, top );  // これ以外は未対応 //@check
  s32 width  = render_target.GetWidth();
  s32 height = render_target.GetHeight();

#if 0
  //迷走中
  f32 l_screen_point_x = screen_point_x * (right-left)/width;  // レンダーターゲットの中での比率にする
  f32 l_screen_point_y = screen_point_y * (bottom-top)/height;  // レンダーターゲットの中での比率にしてから下詰めにする
  f32 l_screen_point_y = screen_point_y * (bottom-top)/height + (1.0f-(bottom-top)/height);  // レンダーターゲットの中での比率にしてから下詰めにする
  // ↑はやはり要る処理だった。よって↓は要らない処理になった。
  //f32 l_screen_point_x = screen_point_x;
  //f32 l_screen_point_y = screen_point_y;
#endif
  // 最終結果↓これが正しい。
  f32 l_screen_point_x = screen_point_x;  // レンダーターゲットの中での比率にする
  f32 l_screen_point_y = screen_point_y;  // レンダーターゲットの中での比率にしてから下詰めにする

  // -1.0f<= <=1.0fに変更する
  gfl::math::PivotDirection pivot = this->GetPivotDirection();
  gfl::math::VEC4 screen_point;
  if( pivot == gfl::math::PIVOT_UPSIDE_TO_TOP )  // 縦横を入れ替える
  {
    screen_point = gfl::math::VEC4(2.0f*l_screen_point_y-1.0f, -2.0f*l_screen_point_x+1.0f, 0.0f, 1.0f);  // 縦横を入れ替える  // zは0.0fでも-1.0fでも1.0fでもいい
  }
  else  // if( pivot == gfl::math::PIVOT_NONE )  // 縦横を入れ替えない
  {
    GFL_ASSERT_MSG(pivot == gfl::math::PIVOT_NONE, "未対応のPivotDirection %dです。\n", pivot); //@check
    screen_point = gfl::math::VEC4(2.0f*l_screen_point_x-1.0f, -2.0f*l_screen_point_y+1.0f, 0.0f, 1.0f);  // 縦横を入れ替えない  // zは0.0fでも-1.0fでも1.0fでもいい
  }

  gfl::math::MTX34 view_matrix_34;
  gfl::math::MTX44 projection_matrix;
  GetViewAndProjectionMatrix(&view_matrix_34, &projection_matrix);
  
  gfl::math::MTX44 view_matrix;
  gfl::math::MTX44Identity(&view_matrix);
  // 方向が欲しいので3x3要素のみコピー
  for(u32 i=0; i<3; ++i)
  {
    for(u32 j=0; j<3; ++j)
    //for(u32 j=0; j<4; ++j)こちらだと画面に映らない
    {
      view_matrix.m[i][j] = view_matrix_34.m[i][j];
    }
  }

  gfl::math::MTX44 view_projection_matrix;
  gfl::math::MTX44 view_projection_matrix_inverse;
  gfl::math::MTX44Mult(&view_projection_matrix, &projection_matrix, &view_matrix);
  gfl::math::MTX44Inverse(&view_projection_matrix_inverse, &view_projection_matrix);

  // 半直線の方向ベクトル
  gfl::math::VEC4 world_ray_direction_4;
  nn::math::VEC4Transform(&world_ray_direction_4, &view_projection_matrix_inverse, &screen_point);
  *world_ray_direction = gfl::math::VEC3(world_ray_direction_4.x, world_ray_direction_4.y, world_ray_direction_4.z);
  gfl::math::VEC3Normalize(world_ray_direction, world_ray_direction);

  // 半直線の始点ベクトル
  GetNwCamera()->Transform().GetTranslate(world_ray_origin);
}

//-----------------------------------------------------------------------------
/*
 *  @brief        上画面スクリーン座標系の点をワールド座標系の半直線(カメラの位置からスクリーンの点に飛ばしたレイ)に変換する
 *
 *  @param[out]   world_ray_direction   ワールド座標系の半直線の方向ベクトル(大きさ1)
 *  @param[out]   world_ray_origin      ワールド座標系の半直線の始点座標(カメラの位置GetCameraPositionと同じ値が得られます)
 *  @param[in]    upper_screen_point_x  上画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(400-1,240-1))
 *  @param[in]    upper_screen_point_y  画面範囲外も表せるようにsignedにしました。
 *  @param[in]    upper_render_target   レンダーターゲット(これがベースカメラのときは左でも右でもどちらでもいい)
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
 *  @brief        下画面スクリーン座標系の点をワールド座標系の半直線(カメラの位置からスクリーンの点に飛ばしたレイ)に変換する
 *
 *  @param[out]   world_ray_direction   ワールド座標系の半直線の方向ベクトル(大きさ1)
 *  @param[out]   world_ray_origin      ワールド座標系の半直線の始点座標(カメラの位置GetCameraPositionと同じ値が得られます)
 *  @param[in]    lower_screen_point_x  下画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(320-1,240-1))
 *  @param[in]    lower_screen_point_y  画面範囲外も表せるようにsignedにしました。
 *  @param[in]    lower_render_target   レンダーターゲット
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
 *  @brief        スクリーン座標系の点をワールド座標系の点(ニア面上orファー面上)に変換する
 *
 *  @param[out]   world_point           ワールド座標系の点の位置座標
 *  @param[in]    is_near               trueのときニア面上、falseのときファー面上
 *  @param[in]    screen_point_x        スクリーン座標系の点の位置座標(左上が(0.0f,0.0f)、右下が(1.0f,1.0f))
 *  @param[in]    screen_point_y
 *  @param[in]    render_target         レンダーターゲット
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
  // ど真ん中を通る半直線
  gfl::math::VEC3 center_world_ray_direction;
  gfl::math::VEC3 center_world_ray_origin;
  this->ConvertScreenPointToWorldRay(
    &center_world_ray_direction,
    &center_world_ray_origin,
    0.5f,
    0.5f,
    render_target
  );

  // (screen_point_x, screen_point_y)を通る半直線
  gfl::math::VEC3 current_world_ray_direction;
  gfl::math::VEC3 current_world_ray_origin;
  this->ConvertScreenPointToWorldRay(
    &current_world_ray_direction,
    &current_world_ray_origin,
    screen_point_x,
    screen_point_y,
    render_target
  );

  // center_world_ray_originとcurrent_world_ray_originは同じになっています。

  // center_world_ray_directionとcurrent_world_ray_directionの間の角度のcosをcos(theta)とすると、方向ベクトルの大きさは1にしてあるので、
  f32 cos_theta = gfl::math::VEC3Dot(&center_world_ray_direction, &current_world_ray_direction);
 
  // ニア面上orファー面上までの距離
  if( cos_theta > 0.0f )
  {
    f32 center_distance = (is_near)?(this->GetNearClip()):(this->GetFarClip());
    f32 current_distance = center_distance / cos_theta;
    gfl::math::VEC3 temp;
    gfl::math::VEC3Add( world_point, gfl::math::VEC3Scale(&temp, &current_world_ray_direction, current_distance), &current_world_ray_origin );
  }
  else
  {
    GFL_ASSERT_MSG(0, "角度が異常です。\n");
    // ワールド座標系の半直線の始点座標(カメラの位置GetCameraPositionと同じ値が得られます)にでもしておく
    *world_point = current_world_ray_origin;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief        上画面スクリーン座標系の点をワールド座標系の点(ニア面上orファー面上)に変換する
 *
 *  @param[out]   world_point           ワールド座標系の点の位置座標
 *  @param[in]    is_near               trueのときニア面上、falseのときファー面上
 *  @param[in]    upper_screen_point_x  上画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(400-1,240-1))
 *  @param[in]    upper_screen_point_y  画面範囲外も表せるようにsignedにしました。 
 *  @param[in]    render_target         レンダーターゲット
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
 *  @brief        下画面スクリーン座標系の点をワールド座標系の点(ニア面上orファー面上)に変換する
 *
 *  @param[out]   world_point           ワールド座標系の点の位置座標
 *  @param[in]    is_near               trueのときニア面上、falseのときファー面上
 *  @param[in]    lower_screen_point_x  下画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(320-1,240-1))
 *  @param[in]    lower_screen_point_y  画面範囲外も表せるようにsignedにしました。 
 *  @param[in]    render_target         レンダーターゲット
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
 *  @brief        ワールド座標系の点をスクリーン座標系の点に変換する
 *
 *  @param[out]    screen_point_x        スクリーン座標系の点の位置座標(左上が(0.0f,0.0f)、右下が(1.0f,1.0f))
 *  @param[out]    screen_point_y        
 *  @param[in]     world_point           ワールド座標系の点の位置座標
 *  @param[in]     render_target         レンダーターゲット
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
      // wが0.0fだと「-1.0f<= <=1.0fのスクリーン座標」のところの計算結果が、
      // 「-1.3fくらい<= <=1.3fくらいのスクリーン座標」でかつ、スクリーン座標の符号も逆転し、カメラの距離によって「-0.011fくらい<= <=0.011fくらいのスクリーン座標」という計算結果になったりもする。
      //

  gfl::math::MTX34 view_matrix_34;
  gfl::math::MTX44 projection_matrix;
  GetViewAndProjectionMatrix(&view_matrix_34, &projection_matrix);

#if 1
  // ここの#ifはどっちでも同じ結果だった
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
  // 方向が欲しいので3x3要素のみコピー
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

  // -1.0f<= <=1.0fのスクリーン座標(ビューポートで範囲を制限していないレンダーターゲット全体のスクリーン座標)
  gfl::math::VEC2 screen_point_2;
  switch( this->GetPivotDirection() )
  {
  case gfl::math::PIVOT_NONE:
    {
      screen_point_2.x =   screen_point_4.x / screen_point_4.w;  // 縦横を入れ替えない
      screen_point_2.y = - screen_point_4.y / screen_point_4.w;
    } 
    break; 
  case gfl::math::PIVOT_UPSIDE_TO_TOP:
    {
      screen_point_2.x = - screen_point_4.y / screen_point_4.w;  // 縦横を入れ替える
      screen_point_2.y = - screen_point_4.x / screen_point_4.w;
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "PivotDirection=%dは未対応です。\n", this->GetPivotDirection() );
      screen_point_2.x = - screen_point_4.y / screen_point_4.w;  // 縦横を入れ替える
      screen_point_2.y = - screen_point_4.x / screen_point_4.w;
    } 
    break;
  }

  // 0.0f<= <=1.0fに変更する(ビューポートで範囲を制限していないレンダーターゲット全体のスクリーン座標)
  *screen_point_x = screen_point_2.x / 2.0f + 0.5f;
  *screen_point_y = screen_point_2.y / 2.0f + 0.5f;

  // ビューポートで制限された範囲を0.0f<= <=1.0fとしたスクリーン座標
  f32 left, top, right, bottom;
  render_target.GetViewport(&left, &top, &right, &bottom);
  GFL_ASSERT_MSG( left==0.0f&&top==0.0f, "left=%f, top=%f\n", left, top );  // これ以外は未対応
  s32 width  = render_target.GetWidth();
  s32 height = render_target.GetHeight();
  //*screen_point_x = (*screen_point_x) * width /(right-left);// + (width -(right-left))/(right-left);
  //*screen_point_y = (*screen_point_y) * height/(bottom-top);// + (height-(bottom-top))/(bottom-top);
  //↑要らない処理だった
}
//-----------------------------------------------------------------------------
/*
 *  @brief        ワールド座標系の点を上画面スクリーン座標系の点に変換する
 *
 *  @param[out]    upper_screen_point_x  上画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(400-1,240-1))
 *  @param[out]    upper_screen_point_y  画面範囲外も表せるようにsignedにしました。     
 *  @param[in]     world_point           ワールド座標系の点の位置座標
 *  @param[in]     upper_render_target   レンダーターゲット(これがベースカメラのときは左でも右でもどちらでもいい)
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
 *  @brief        ワールド座標系の点を下画面スクリーン座標系の点に変換する
 *
 *  @param[out]    lower_screen_point_x  下画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(320-1,240-1))
 *  @param[out]    lower_screen_point_y  画面範囲外も表せるようにsignedにしました。
 *  @param[in]     world_point           ワールド座標系の点の位置座標
 *  @param[in]     lower_render_target   レンダーターゲット
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
// アニメーション
//

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションを変更する
 *                 初めてこの関数を呼んだときは、Animationを生成する。
 *
 *  @param[in]     allocator               アロケータ
 *  @param[in]     res_anim                アニメーションリソース
 *  @param[in]     max_anim_members        アニメーションするメンバの最大数。
 *                                         初めてこの関数を呼んだときは、使用される。
 *                                         Resource::GetCameraMemberAnimSetCountで得た値を渡すとよい。
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
 *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
 *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
 *                                         即ち、そのアニメーションタイプにおける最大数にします。
 */
//-----------------------------------------------------------------------------
void Camera::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  ResAnim*                res_anim,
  s32                     max_anim_members
)
{
#if 0
  // 初めてこの関数を呼んだときは、Animationを生成する。
  if( m_animation == NULL )
  {
    nw::gfx::Camera* nw_camera = GetNwCamera();
    nw::gfx::AnimGroup* anim_group = nw_camera->GetAnimGroup();
    GFL_ASSERT_STOP(anim_group);  // アニメーショングループがありません。

    m_animation = GFL_NEW(allocator->GetHeapBase()) Animation(
      anim_group->GetMemberCount(),
      max_anim_members,
      false
    );

    // アニメーションを初めて適用する
    m_animation->Change(
      allocator,
      res_anim
    );

    nw::gfx::AnimObject* nw_anim_object = m_animation->GetNwAnimObject();
    GFL_ASSERT_STOP(nw_anim_object);

    // アニメーショングループを AnimEvaluator にバインドします。
    // これにより、アニメーション対象メンバにアニメーションデータが関連付けられます。
    bool bind_result = nw_anim_object->Bind(anim_group);
    GFL_ASSERT_STOP(bind_result);
    
    // AnimEvaluator をモデルに設定します。
    // AnimEvaluator は一つのモデルに対して複数設定することができ、
    // その際には、AnimEvaluator 毎に objectIndex を指定します。
    // 詳しくは、 PartialAnimationDemo を参照してください。
    //
    // アニメーションの対象や種類によって用いる関数が異なります。
    // SkeletalModel::SetSkeletalAnimObject
    // Model::SetVisibilityAnimObject
    // Model::SetMaterialAnimObject
    // Light::SetAnimObject
    nw_camera->SetAnimObject(nw_anim_object);
  }
  else
  {
    // アニメーションを直ちに変更する
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
 *  @brief         アニメーションを変更する
 *                 初めてこの関数を呼んだときは、Animationを生成する。
 *
 *  @param[in]     allocator               アロケータ
 *  @param[in]     resource                リソース
 *  @param[in]     index_in_resource       リソース内におけるインデックス
 *  @param[in]     max_anim_members        アニメーションするメンバの最大数。
 *                                         初めてこの関数を呼んだときは、使用される。
 *                                         Resource::GetCameraMemberAnimSetCountで得た値を渡すとよい。
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
 *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
 *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
 *                                         即ち、そのアニメーションタイプにおける最大数にします。
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
  // 初めてこの関数を呼んだときは、Animationを生成する。
  if( m_animation == NULL )
  {
    nw::gfx::Camera* nw_camera = GetNwCamera();
    nw::gfx::AnimGroup* anim_group = nw_camera->GetAnimGroup();
    GFL_ASSERT_STOP(anim_group);  // アニメーショングループがありません。

    m_animation = GFL_NEW(allocator->GetHeapBase()) Animation(
      anim_group->GetMemberCount(),
      max_anim_members,
      false
    );

    // アニメーションを初めて適用する
    m_animation->Change(
      allocator,
      resource,
      Resource::ANIMATION_RESOURCE_TYPE_CAMERA,
      index_in_resource
    );

    nw::gfx::AnimObject* nw_anim_object = m_animation->GetNwAnimObject();
    GFL_ASSERT_STOP(nw_anim_object);

    // アニメーショングループを AnimEvaluator にバインドします。
    // これにより、アニメーション対象メンバにアニメーションデータが関連付けられます。
    bool bind_result = nw_anim_object->Bind(anim_group);
    GFL_ASSERT_STOP(bind_result);
    
    // AnimEvaluator をモデルに設定します。
    // AnimEvaluator は一つのモデルに対して複数設定することができ、
    // その際には、AnimEvaluator 毎に objectIndex を指定します。
    // 詳しくは、 PartialAnimationDemo を参照してください。
    //
    // アニメーションの対象や種類によって用いる関数が異なります。
    // SkeletalModel::SetSkeletalAnimObject
    // Model::SetVisibilityAnimObject
    // Model::SetMaterialAnimObject
    // Light::SetAnimObject
    nw_camera->SetAnimObject(nw_anim_object);
  }
  else
  {
    // アニメーションを直ちに変更する
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
 *  @brief         slot_indexのunit_indexに割り当てるアニメーションを変更する
 *
 *  @param[in]     heap_allocator          ヒープアロケータ
 *  @param[in]     device_allocator        デバイスアロケータ
 *  @param[in]     unit_index              スロット内につくったユニットのインデックス
 *  @param[in]     res_anim                アニメーションリソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
 *  @param[in]     resource                リソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
 *  @param[in]     animation_resource_type アニメーションリソースのタイプ(resourceがNULLのときは使用されないのでてきとーな値でOK)
 *  @param[in]     index_in_resource       リソース内におけるインデックス(resourceがNULLのときは使用されないのでてきとーな値でOK)
 *  @param[in]     change_frame            何フレームで変更するか(この間ブレンドされる)
 *  @param[in]     max_anim_members        アニメーションするメンバの最大数。(res_animがNULLのときは使用されないのでてきとーな値でOK)
 *                                         Resource::Get???MemberAnimSetCountで得た値を渡すとよい。
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
 *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
 *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
 *                                         即ち、そのアニメーションタイプにおける最大数にします。
 *  @param[in]     force                   強制的にアニメーションを関連付けます。(res_animがNULLのときは使用されないのでてきとーな値でOK)
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
        m_animation_slot_num,  // 1個しかない
        m_animation_blend_unit_num,
        m_animation_num_in_unit
    );
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_ChangeAnimation(
      heap_allocator,
      device_allocator,
      0,  // 1個しかないのでインデックスは0
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
 *  @brief         アニメーションのフレームを設定する
 *
 *  @param[in]     frame           フレーム
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションのフレームを取得する
 *
 *  @retval        フレーム
 */
//-----------------------------------------------------------------------------
f32 Camera::GetAnimationFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
    return 0.0f; 
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの更新フレームを設定する
 *                 0.0fで停止、1.0fで等倍、2.0fで2倍速、-1.0fで逆等倍再生
 *
 *  @param[in]     step_frame      更新フレーム
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationStepFrame(f32 step_frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの更新フレームを取得する
 *
 *  @retval        更新フレーム
 */
//-----------------------------------------------------------------------------
f32 Camera::GetAnimationStepFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
    return 0.0f; 
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームを設定する
 *
 *  @param[in]     frame           フレーム
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationStartFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームを取得する
 *
 *  @retval        フレーム
 */
//-----------------------------------------------------------------------------
f32 Camera::GetAnimationStartFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
    return 0.0f; 
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの終了フレームを設定する
 *
 *  @param[in]     frame           フレーム
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationEndFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの終了フレームを取得する
 *
 *  @retval        フレーム
 */
//-----------------------------------------------------------------------------
f32 Camera::GetAnimationEndFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームと終了フレームを設定する
 *
 *  @param[in]     start_frame     開始フレーム
 *  @param[in]     end_frame       終了フレーム
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの現在のフレームが終了フレームか否か判定する
 *
 *  @retval        bool    現在のフレームが終了フレームの場合tureを返す
 */
//-----------------------------------------------------------------------------
bool Camera::IsAnimationFrameEndFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションがループ再生か否か取得する
 *
 *  @retval        bool            ループのときtrue、ループでないときfalseを返す
 */
//-----------------------------------------------------------------------------
bool Camera::IsAnimationLoop(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
    return false; 
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションにループ再生を設定する
 *
 *  @param[in]     loop            ループのときtrue、ループでないときfalseを設定する
 */
//-----------------------------------------------------------------------------
void Camera::SetAnimationLoop(bool loop)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
    GFL_ASSERT_MSG( m_animation_manager, "アニメーションがありません。\n" );  //@check
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         フレーム操作
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
 *  @brief         slot_index内につくったユニットのアニメーションをブレンドする
 *
 *  @param[in]     heap_allocator          ヒープアロケータ
 *  @param[in]     device_allocator        デバイスアロケータ
 *  @param[in]     mode                    AnimationSlot::MODE_NONE                slot_indexのアニメーションをなしにする
 *                                         AnimationSlot::MODE_ANIMATION           ブレンドをやめてanimation_unit_indexで指定したアニメーションを割り当てる
 *                                         AnimationSlot::MODE_BLEND_INTERPOLATOR  補間ブレンド
 *                                         AnimationSlot::MODE_BLEND_ADDER         加算ブレンド
 *  @param[in]     weight0                 AnimationSlot::MODE_BLEND_INTERPOLATORかAnimationSlot::MODE_BLEND_ADDERのときのみ使用。
 *                                         ユニット0のブレンド重み
 *  @param[in]     weight1                 AnimationSlot::MODE_BLEND_INTERPOLATORかAnimationSlot::MODE_BLEND_ADDERのときのみ使用。
 *                                         ユニット1のブレンド重み
 *  @param[in]     interpolator_ignore_no_anim_member   AnimationSlot::MODE_BLEND_INTERPOLATORのときのみ使用。
 *                                                      falseにすると、アニメーションが存在しないメンバはバインド時の値がブレンドされます。
 *                                                      trueにすると、アニメーションが存在しないメンバは重み 0 としてブレンドされます。
 *  @param[in]     interpolator_normalization_enable    AnimationSlot::MODE_BLEND_INTERPOLATORのときのみ使用。
 *                                                      trueにすると、ブレンド重みが合計で 1 になるよう正規化してからブレンドを行います。
 *                                                      falseにすると、設定した重みがそのままブレンドに使用される。
 *  @param[in]     animation_unit_index                 AnimationSlot::MODE_ANIMATIONのときのみ使用。
 *                                                      スロット内のどのユニットにあるアニメーションを割り当てるかを指定する。
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
        m_animation_slot_num,  // 1個しかない
        m_animation_blend_unit_num,
        m_animation_num_in_unit
    );
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_BlendAnimation(
      heap_allocator,
      device_allocator,
      0,  // 1個しかないのでインデックスは0
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
 *  @brief         ブレンド重み
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
 *  @brief         ResAnim::Typeのアニメーションで変更したポーズをリセットする
 *
 *  アニメーションが割り当てられているときは、そのアニメーションが動かすメンバはリセットされない。
 *  アニメーションが割り当てられていないときは、全てのメンバがリセットされる。
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
// ユーティリティ関数
//

//-----------------------------------------------------------------------------
/*
 *  @brief        ビューアップデータ ユーティリティ関数
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
 *  @brief        プロジェクションアップデータ ユーティリティ関数
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
/*NintendoWare for CTR 2.2.2 リリースノート
2011年12月26日
ノート 52-G11: (ライブラリ) Camera::GetFrustum と GetOrtho の API を変更
Camera::GetFrustum と GetOrtho で得られた値をそのまま SetFrustum および SetOrtho で設定すると top と bottom が反転していました。 GetFrustum と GetOrtho を廃止予定とし、top と bottom を反転せずに取得する GetFrustumWithoutFlip と GetOrthoWithoutFlip、および従来通り top と bottom を反転して取得する GetFrustumWithYFlip と GetOrthoWithYFlip を追加しました。 */
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
/*NintendoWare for CTR 2.2.2 リリースノート
2011年12月26日
ノート 52-G11: (ライブラリ) Camera::GetFrustum と GetOrtho の API を変更
Camera::GetFrustum と GetOrtho で得られた値をそのまま SetFrustum および SetOrtho で設定すると top と bottom が反転していました。 GetFrustum と GetOrtho を廃止予定とし、top と bottom を反転せずに取得する GetFrustumWithoutFlip と GetOrthoWithoutFlip、および従来通り top と bottom を反転して取得する GetFrustumWithYFlip と GetOrthoWithYFlip を追加しました。 */
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
      GetNwCamera()->SetFrustum(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip問題に対応するため上下を逆にしておく。
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
      GetNwCamera()->SetOrtho(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip問題に対応するため上下を逆にしておく。
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
void Camera::SetHeight(f32 height)  // aspectを保持するため、heightを変更すると横幅も変更されます。
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
      GetNwCamera()->SetFrustum(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip問題に対応するため上下を逆にしておく。
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
      GetNwCamera()->SetOrtho(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip問題に対応するため上下を逆にしておく。
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
      GetNwCamera()->SetFrustum(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip問題に対応するため上下を逆にしておく。
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
      GetNwCamera()->SetOrtho(left,right,top,bottom,near,far);  // SetRect, SetRectWithoutFlip, SetRectWithYFlip問題に対応するため上下を逆にしておく。

    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief        画面の上方向を設定する
 *
 *  @param[in]    pivot_direction    画面の上方向
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
 *  @brief        画面の上方向を取得する
 *
 *  @retval    gfl::math::PivotDirection    画面の上方向
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
 *  @brief        ニアクリップ / ファークリップ
 */
//-----------------------------------------------------------------------------
void Camera::SetNearClip( f32 near_clip )
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  nw_camera->SetNear(near_clip);  // この関数はパラメータを簡単に取得するためのユーティリティ関数です。
                                  // とのことなので、使う前にカメラのタイプをチェックしたほうがいいかも。
}
f32  Camera::GetNearClip( void ) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  return nw_camera->GetNear();  // この関数はパラメータを簡単に取得するためのユーティリティ関数です。
                                // とのことなので、使う前にカメラのタイプをチェックしたほうがいいかも。
}
void Camera::SetFarClip( f32 far_clip )
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  nw_camera->SetFar(far_clip);  // この関数はパラメータを簡単に取得するためのユーティリティ関数です。
                                // とのことなので、使う前にカメラのタイプをチェックしたほうがいいかも。
}
f32  Camera::GetFarClip( void ) const
{
  nw::gfx::Camera* nw_camera = GetNwCamera();
  return nw_camera->GetFar();  // この関数はパラメータを簡単に取得するためのユーティリティ関数です。
                               // とのことなので、使う前にカメラのタイプをチェックしたほうがいいかも。
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
// 取得 / 設定
//

//-----------------------------------------------------------------------------
/*
 *  @brief         ビューモードを取得する
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
 *  @brief         射影モードを取得する
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
 *           定数定義
 */
//---------------------------------------------------------------------------
const gfl::math::VEC3  Camera::CAMERA_POSITION_DEFAULT    = gfl::math::VEC3(7.0f, 3.5f, -5.0f);   // カメラ位置のデフォルト値
const gfl::math::VEC3  Camera::TARGET_POSITION_DEFAULT    = gfl::math::VEC3(0.0f, 0.0f, 0.0f);    // カメラターゲットのデフォルト値
const f32              Camera::NEAR_CLIP_DEFAULT          =    0.1f;                              // ニアクリップのデフォルト値
const f32              Camera::FAR_CLIP_DEFAULT           = 1000.0f;                              // ファークリップのデフォルト値
const f32              Camera::FOVY_RADIAN_DEFAULT        = GFL_MATH_DEG_TO_RAD(37.8f);           // FOVのY方向のデフォルト値
const gfl::math::VEC2  Camera::PROJECTION_CENTER_DEFAULT  = gfl::math::VEC2(0.0f, 0.0f);          // カメラの投影面の中心位置のデフォルト値
const f32              Camera::PROJECTION_HEIGHT_DEFAULT  =    1.0f;                              // カメラの投影面の高さのデフォルト値



//
// NW4C関連 static 生成 / 破棄
//

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラを生成する
 * 
 *  @param[in]    allocator          生成に使用するアロケーター
 *  @param[in]    camera_position    カメラの位置座標
 *  @param[in]    target_position    カメラターゲットの座標
 *  @param[in]    near_clip          ニアクリップの値
 *  @param[in]    far_clip           ファークリップの値
 *  @param[in]    fovy_radian        FOVのY方向の値
 *  @param[in]    pivot_direction    画面の上方向
 *  @param[in]    w_scale            WScaleの値
 *
 *  @retval       nw::gfx::Camera*  生成したカメラを返す
 *
 * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
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
 *  @brief        立体視に使用するベースカメラと左右カメラを生成する
 *
 *                実際は右目用カメラと左目用カメラの区別はありません。
 *
 *  @param[out]   pp_base_camera     生成したベースカメラを返すためのポインタ
 *  @param[out]   pp_left_camera     生成した右目用カメラを返すためのポインタ
 *  @param[out]   pp_right_camera    生成した左目用カメラを返すためのポインタ
 *  @param[in]    allocator          生成に使用するアロケーター
 *  @param[in]    camera_position    カメラの位置座標
 *  @param[in]    target_position    カメラターゲットの座標
 *  @param[in]    near_clip          ニアクリップの値
 *  @param[in]    far_clip           ファークリップの値
 *  @param[in]    fovy_radian        FOVのY方向の値
 *  @param[in]    w_scale            WScaleの値
 *
 * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
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
// NW4C関連 static 設定 / 取得
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
 *  @brief        カメラのアスペクト比を設定する
 * 
 *                指定したレンダーターゲットに合うように設定されます。
 *                カメラがアタッチされているシーンを更新してから実行する必要があります。
 * 
 *  @param[in]    camera          設定するカメラ
 *  @param[in]    render_target   アスペクト比を合わせるレンダーターゲット
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraAspectRatio(
    nw::gfx::Camera*                camera,
    const nw::gfx::IRenderTarget*   render_target
)
{
  GFL_ASSERT_STOP(render_target);

  // オンスクリーンバッファは縦と横が逆になっているため、
  // 幅と高さを逆にしてアスペクト比を求めています。
  SetCameraAspectRatio(
      camera, 
      static_cast<f32>(render_target->GetDescription().height) / static_cast<f32>(render_target->GetDescription().width)
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラのアスペクト比を設定する
 * 
 *                カメラがアタッチされているシーンを更新してから実行する必要があります。
 * 
 *  @param[in]    camera        設定するカメラ
 *  @param[in]    aspect_ratio   アスペクト比
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
 *  @brief        カメラの位置を得る
 *
 *  @param[out]   camera_position    カメラの位置を返す
 */
//-----------------------------------------------------------------------------
void Camera::GetCameraPosition( gfl::math::VEC3& camera_position ) const
{
  GetCameraPosition( *GetNwCamera(), camera_position );
}
//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの注視点の位置を得る
 *
 *  @param[out]   target_position    カメラの注視点の位置を返す
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
 *  @brief        カメラの位置と注視点の位置を得る
 *
 *  @param[out]   camera_position    カメラの位置を返す
 *  @param[out]   target_position    カメラの注視点の位置を返す
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
 *  @brief        カメラの位置を設定する
 *
 *  @param[in]    camera_position    カメラの位置
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraPosition( const gfl::math::VEC3& camera_position )
{
  SetCameraPosition( *GetNwCamera(), camera_position );
}
//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの注視点の位置を設定する
 *
 *  @param[in]    target_position    カメラの注視点の位置
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
 *  @brief        カメラの位置と注視点の位置を設定する
 *
 *  @param[in]    camera_position    カメラの位置
 *  @param[in]    target_position    カメラの注視点の位置
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
// static 生成 / 破棄
//

//-----------------------------------------------------------------------------
/*
 *  @brief        生成
 * 
 *  @param[in]    allocator          生成に使用するアロケーター
 *  @param[in]    camera_position    カメラの位置座標
 *  @param[in]    target_position    カメラターゲットの座標
 *  @param[in]    near_clip          ニアクリップの値
 *  @param[in]    far_clip           ファークリップの値
 *  @param[in]    fovy_radian        FOVのY方向の値
 *  @param[in]    pivot_direction    画面の上方向
 *  @param[in]    w_scale            WScaleの値
 *
 *  @retval       Camera*  生成したカメラクラス
 *
 * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
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
 *  @brief        立体視に使用するベースカメラと左右カメラを生成する
 *
 *                実際は右目用カメラと左目用カメラの区別はありません。
 *
 *  @param[out]   pp_base_camera     生成したベースカメラを返すためのポインタ
 *  @param[out]   pp_left_camera     生成した右目用カメラを返すためのポインタ
 *  @param[out]   pp_right_camera    生成した左目用カメラを返すためのポインタ
 *  @param[in]    allocator          生成に使用するアロケーター
 *  @param[in]    camera_position    カメラの位置座標
 *  @param[in]    target_position    カメラターゲットの座標
 *  @param[in]    near_clip          ニアクリップの値
 *  @param[in]    far_clip           ファークリップの値
 *  @param[in]    fovy_radian        FOVのY方向の値
 *  @param[in]    w_scale            WScaleの値
 *
 * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
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
// private関数
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
 *  @brief         生成
 *
 *  @param[in]     allocator          アロケータ
 *  @param[in]     nw_scene_node      シーンノード
 *
 *  @retval        Camera*  生成したカメラクラス
 *
 * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
 */
//-----------------------------------------------------------------------------
Camera* Camera::Create(
    gfl::heap::NwAllocator*           allocator,
    nw::gfx::SceneNode*               nw_scene_node
)
{
  Camera* camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  camera->SetNwSceneNode(nw_scene_node);

  // 独自処理
  //
  //

  return camera;
}



//----------------------------------------------------------------------------
/**
 *	@brief  LookAtとAimモード時のアングル取得関数
 *
 *	@param	r   アングル格納先
 *	@param  view_mode ビューモード
 */
//-----------------------------------------------------------------------------
void Camera::GetAimAndTwistRotateXYZ( gfl::math::VEC3* r, ViewMode view_mode ) const
{
  // AIMorLOOK_ATは、自身で計算
  // Camera → Targetを見たときのアングルを返す。
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
 *	@brief  LookAtとAimモード時のアングル設定関数
 *
 *	@param	x   X軸回転角度
 *	@param	y   Y軸回転角度
 *	@param	z   Z軸回転角度
 *	@param  view_mode ビューモード
 */
//-----------------------------------------------------------------------------
void Camera::SetAimAndTwistRotateXYZ( f32 x, f32 y, f32 z, ViewMode view_mode )
{

  // カメラ座標はそのままで、
  // ターゲット座標をRotateにあった位置に動かす。
  // RotateZは上方向に反映する。
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

  // ターゲットPositionを更新
  this->SetTargetPosition(targetpos);

  // カメラ上方向を更新
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

